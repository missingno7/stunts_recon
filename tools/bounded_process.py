"""Bound proof subprocess trees, including packaged Python on Windows."""
import os
from pathlib import Path
import signal
import subprocess
import sys


def _windows_job():
    import ctypes as C
    from ctypes import wintypes as W
    class Limits(C.Structure):
        _fields_ = [('process_time', C.c_int64), ('job_time', C.c_int64), ('flags', W.DWORD),
                    ('min_ws', C.c_size_t), ('max_ws', C.c_size_t), ('processes', W.DWORD),
                    ('affinity', C.c_size_t), ('priority', W.DWORD), ('scheduling', W.DWORD)]
    class Counters(C.Structure):
        _fields_ = [(name, C.c_uint64) for name in ('reads','writes','other','read_bytes','write_bytes','other_bytes')]
    class Extended(C.Structure):
        _fields_ = [('limits', Limits), ('io', Counters), ('process_memory', C.c_size_t),
                    ('job_memory', C.c_size_t), ('peak_process', C.c_size_t), ('peak_job', C.c_size_t)]
    kernel = C.WinDLL('kernel32', use_last_error=True)
    kernel.CreateJobObjectW.argtypes = [C.c_void_p, W.LPCWSTR]
    kernel.CreateJobObjectW.restype = W.HANDLE
    kernel.SetInformationJobObject.argtypes = [W.HANDLE, C.c_int, C.c_void_p, W.DWORD]
    kernel.SetInformationJobObject.restype = W.BOOL
    kernel.AssignProcessToJobObject.argtypes = [W.HANDLE, W.HANDLE]
    kernel.AssignProcessToJobObject.restype = W.BOOL
    kernel.CloseHandle.argtypes = [W.HANDLE]
    kernel.CloseHandle.restype = W.BOOL
    handle = kernel.CreateJobObjectW(None, None)
    if not handle: raise C.WinError(C.get_last_error())
    info = Extended(); info.limits.flags = 0x2000  # JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE
    if not kernel.SetInformationJobObject(handle, 9, C.byref(info), C.sizeof(info)):
        error = C.get_last_error(); kernel.CloseHandle(handle); raise C.WinError(error)
    return kernel, handle


def _resume_windows_child(process):
    """Resume the sole initial thread only after assigning its suspended process."""
    import ctypes as C
    from ctypes import wintypes as W
    class ThreadEntry(C.Structure):
        _fields_ = [('size', W.DWORD), ('usage', W.DWORD), ('thread', W.DWORD),
                    ('process', W.DWORD), ('base_priority', W.LONG),
                    ('delta_priority', W.LONG), ('flags', W.DWORD)]
    kernel = C.WinDLL('kernel32', use_last_error=True)
    kernel.CreateToolhelp32Snapshot.argtypes = [W.DWORD, W.DWORD]
    kernel.CreateToolhelp32Snapshot.restype = W.HANDLE
    kernel.Thread32First.argtypes = [W.HANDLE, C.POINTER(ThreadEntry)]
    kernel.Thread32Next.argtypes = [W.HANDLE, C.POINTER(ThreadEntry)]
    kernel.OpenThread.argtypes = [W.DWORD, W.BOOL, W.DWORD]
    kernel.OpenThread.restype = W.HANDLE
    kernel.ResumeThread.argtypes = [W.HANDLE]
    kernel.ResumeThread.restype = W.DWORD
    kernel.CloseHandle.argtypes = [W.HANDLE]
    snapshot = kernel.CreateToolhelp32Snapshot(4, 0)  # TH32CS_SNAPTHREAD
    if snapshot == C.c_void_p(-1).value:
        raise C.WinError(C.get_last_error())
    try:
        entry = ThreadEntry(); entry.size = C.sizeof(entry)
        found = kernel.Thread32First(snapshot, C.byref(entry))
        while found:
            if entry.process == process.pid:
                thread = kernel.OpenThread(2, False, entry.thread)  # THREAD_SUSPEND_RESUME
                if not thread:
                    raise C.WinError(C.get_last_error())
                try:
                    if kernel.ResumeThread(thread) == 0xFFFFFFFF:
                        raise C.WinError(C.get_last_error())
                    return
                finally:
                    kernel.CloseHandle(thread)
            found = kernel.Thread32Next(snapshot, C.byref(entry))
        raise RuntimeError('Suspended child has no initial thread')
    finally:
        kernel.CloseHandle(snapshot)


def bounded_run(argv, *, cwd, env, log, timeout):
    """Close the Windows job (or POSIX process group) on any interruption.

    Use the actual interpreter image instead of the Windows Store activation
    alias: killing an activation stub need not kill the interpreter it starts.
    """
    job = None
    if os.name == 'nt':
        import ctypes as C
        if os.path.normcase(str(argv[0])) == os.path.normcase(sys.executable):
            kernel = C.WinDLL('kernel32', use_last_error=True)
            image = C.create_unicode_buffer(32768)
            if not kernel.GetModuleFileNameW(None, image, len(image)):
                raise C.WinError(C.get_last_error())
            argv = [image.value, *argv[1:]]
        job = _windows_job()
    process = None
    try:
        with Path(log).open('wb') as stream:
            process = subprocess.Popen(argv, cwd=cwd, env=env, stdin=subprocess.DEVNULL,
                                       stdout=stream, stderr=subprocess.STDOUT,
                                       creationflags=(getattr(subprocess, 'CREATE_NO_WINDOW', 0) |
                                                      (4 if job else 0)),  # CREATE_SUSPENDED
                                       start_new_session=os.name != 'nt')
            if job and not job[0].AssignProcessToJobObject(job[1], int(process._handle)):
                import ctypes as C
                raise C.WinError(C.get_last_error())
            if job:
                _resume_windows_child(process)
            return process.wait(timeout=timeout)
    except BaseException:
        if job:
            job[0].CloseHandle(job[1]); job = None
        elif process is not None:
            try: os.killpg(process.pid, signal.SIGKILL)
            except ProcessLookupError: pass
        if process is not None:
            try:
                process.wait(timeout=5)
            except subprocess.TimeoutExpired:
                process.kill()
                process.wait(timeout=10)
        raise
    finally:
        if job: job[0].CloseHandle(job[1])

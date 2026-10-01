#include "port_runtime.h"

#include <SDL3/SDL.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#ifndef STUNTS_BUILD_ID
#define STUNTS_BUILD_ID "standalone-diagnostics-probe"
#endif

const char *port_diagnostics_build_id(void) { return STUNTS_BUILD_ID; }

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dbghelp.h>

typedef BOOL (WINAPI *DumpWriter)(HANDLE, DWORD, HANDLE, MINIDUMP_TYPE,
                                PMINIDUMP_EXCEPTION_INFORMATION,
                                PMINIDUMP_USER_STREAM_INFORMATION,
                                PMINIDUMP_CALLBACK_INFORMATION);

static char s_directory[1024];
static char s_trace_path[1100];
static char s_executable[1024];
static HANDLE s_log = INVALID_HANDLE_VALUE;
static HANDLE s_request, s_done, s_worker;
static HMODULE s_dbghelp;
static DumpWriter s_write_dump;
static volatile LONG s_crashing, s_shutdown;
static DWORD s_fault_thread;
static EXCEPTION_RECORD s_exception;
static CONTEXT s_context;
static EXCEPTION_POINTERS s_pointers = { &s_exception, &s_context };
static LPTOP_LEVEL_EXCEPTION_FILTER s_previous_filter;
static void (*s_previous_abort)(int);
static SDL_LogOutputFunction s_previous_log;
static void *s_previous_log_data;

static void write_handle(HANDLE file, const char *text)
{
    DWORD written;
    if (file != INVALID_HANDLE_VALUE)
        WriteFile(file, text, (DWORD)strlen(text), &written, NULL);
}

static void SDLCALL log_output(void *unused, int category,
                               SDL_LogPriority priority, const char *message)
{
    char line[2048];
    (void)unused;
    snprintf(line, sizeof(line), "SDL [%d/%d]: %s\r\n", category,
             (int)priority, message);
    write_handle(s_log, line);
    if (s_previous_log != NULL)
        s_previous_log(s_previous_log_data, category, priority, message);
}

static DWORD WINAPI crash_writer(void *unused)
{
    char path[1100], report[4096], status[160], module_path[1024] = "";
    HANDLE file;
    HMODULE module = NULL;
    uintptr_t address, base;
    MINIDUMP_EXCEPTION_INFORMATION exception_info;
    BOOL dumped = FALSE;
    DWORD dump_error = ERROR_PROC_NOT_FOUND;
    (void)unused;
    WaitForSingleObject(s_request, INFINITE);
    if (InterlockedCompareExchange(&s_shutdown, 0, 0))
        return 0;
    address = (uintptr_t)s_exception.ExceptionAddress;
    GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                      GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCSTR)address, &module);
    base = (uintptr_t)module;
    if (module != NULL)
        GetModuleFileNameA(module, module_path, sizeof(module_path) - 1u);
    snprintf(report, sizeof(report),
             "Stunts SDL3 crash\r\nbuild_id=%s\r\nexecutable=%s\r\n"
             "exception_code=0x%08lx\r\nthread_id=%lu\r\n"
             "exception_address=0x%llx\r\nmodule_base=0x%llx\r\n"
             "module_offset=0x%llx\r\nmodule_path=%s\r\n",
             STUNTS_BUILD_ID, s_executable,
             (unsigned long)s_exception.ExceptionCode,
             (unsigned long)s_fault_thread,
             (unsigned long long)address, (unsigned long long)base,
             (unsigned long long)(base ? address - base : 0), module_path);
    snprintf(path, sizeof(path), "%s/crash.txt", s_directory);
    file = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_READ, NULL,
                       CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    write_handle(file, report);
    write_handle(s_log, report);
    snprintf(path, sizeof(path), "%s/stunts-crashed.exe", s_directory);
    if (!CopyFileA(s_executable, path, TRUE))
        write_handle(file, "Could not preserve crashed executable.\r\n");
    if (s_exception.ExceptionCode == EXCEPTION_ACCESS_VIOLATION &&
        s_exception.NumberParameters >= 2) {
        snprintf(status, sizeof(status), "access_operation=%llu\r\naccess_address=0x%llx\r\n",
                 (unsigned long long)s_exception.ExceptionInformation[0],
                 (unsigned long long)s_exception.ExceptionInformation[1]);
        write_handle(file, status);
    }
#if defined(__i386__)
    snprintf(status, sizeof(status), "eip=%08lx esp=%08lx ebp=%08lx\r\n",
             (unsigned long)s_context.Eip, (unsigned long)s_context.Esp,
             (unsigned long)s_context.Ebp);
    write_handle(file, status);
#endif
    /* The worker never enters SDL, trace locks, or guest cleanup after a fault.
       DbgHelp is loaded and this thread is created before gameplay starts. */
    snprintf(path, sizeof(path), "%s/crash.dmp", s_directory);
    if (s_write_dump != NULL) {
        HANDLE dump = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_READ, NULL,
                                  CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (dump != INVALID_HANDLE_VALUE) {
            exception_info.ThreadId = s_fault_thread;
            exception_info.ExceptionPointers = &s_pointers;
            exception_info.ClientPointers = FALSE;
            dumped = s_write_dump(GetCurrentProcess(), GetCurrentProcessId(), dump,
                (MINIDUMP_TYPE)(MiniDumpWithDataSegs |
                               MiniDumpWithIndirectlyReferencedMemory |
                               MiniDumpWithThreadInfo), &exception_info, NULL, NULL);
            dump_error = dumped ? 0 : GetLastError();
            CloseHandle(dump);
        } else {
            dump_error = GetLastError();
        }
    }
    snprintf(status, sizeof(status), "minidump_written=%s\r\nminidump_error=0x%08lx\r\n",
             dumped ? "yes" : "no", (unsigned long)dump_error);
    write_handle(file, status);
    write_handle(s_log, status);
    if (file != INVALID_HANDLE_VALUE) {
        FlushFileBuffers(file);
        CloseHandle(file);
    }
    if (s_log != INVALID_HANDLE_VALUE)
        FlushFileBuffers(s_log);
    SetEvent(s_done);
    return 0;
}

static LONG WINAPI crash_filter(EXCEPTION_POINTERS *fault)
{
    if (InterlockedCompareExchange(&s_crashing, 1, 0) == 0) {
        s_fault_thread = GetCurrentThreadId();
        s_exception = *fault->ExceptionRecord;
        s_exception.ExceptionRecord = NULL;
        s_context = *fault->ContextRecord;
        SetEvent(s_request);
    }
    WaitForSingleObject(s_done, 15000);
    return EXCEPTION_EXECUTE_HANDLER;
}

static void abort_signal(int signal_number)
{
    (void)signal_number;
    RaiseException(0xE0000001u, EXCEPTION_NONCONTINUABLE, 0, NULL);
}

static int make_session(const char *root)
{
    SYSTEMTIME time;
    unsigned attempt;
    if (!SDL_CreateDirectory(root))
        return 0;
    GetLocalTime(&time);
    for (attempt = 0; attempt < 100; ++attempt) {
        int length = snprintf(s_directory, sizeof(s_directory),
            "%s/stunts-%04u%02u%02u-%02u%02u%02u-%lu-%u", root,
            time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute,
            time.wSecond, (unsigned long)GetCurrentProcessId(), attempt);
        if (length < 0 || (size_t)length >= sizeof(s_directory))
            return 0;
        if (CreateDirectoryA(s_directory, NULL))
            return 1;
        if (GetLastError() != ERROR_ALREADY_EXISTS)
            return 0;
    }
    return 0;
}

void port_diagnostics_init(int debug, const char *root_override)
{
    char root[1100], path[1100], metadata[2048], system_dir[MAX_PATH];
    const char *base = SDL_GetBasePath();
    char *preferences = NULL;
    union { FARPROC generic; DumpWriter dump; } procedure;
    s_directory[0] = s_trace_path[0] = '\0';
    if (root_override != NULL) {
        if (!make_session(root_override))
            goto unavailable;
    } else {
        snprintf(root, sizeof(root), "%sdiagnostics", base ? base : "");
        if (base == NULL || !make_session(root)) {
            preferences = SDL_GetPrefPath("Stunts", "SDL3");
            if (preferences == NULL || !make_session(preferences)) {
                SDL_free(preferences);
                goto unavailable;
            }
            SDL_free(preferences);
        }
    }
    GetModuleFileNameA(NULL, s_executable, sizeof(s_executable) - 1u);
    snprintf(path, sizeof(path), "%s/session.log", s_directory);
    s_log = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
                       NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    snprintf(metadata, sizeof(metadata),
             "Stunts SDL3 diagnostics\r\nbuild_id=%s\r\nexecutable=%s\r\n"
             "debug=%d\r\nSDL_version=%d\r\n",
             STUNTS_BUILD_ID, s_executable, debug, SDL_GetVersion());
    write_handle(s_log, metadata);
    fprintf(stderr, "PORT diagnostics: %s\n", s_directory);
    if (debug) {
        /* Leave ordinary launches' console output intact. The verbose mode also
           preserves existing fprintf diagnostics when launched by double-click. */
        if (freopen(path, "ab", stderr) != NULL)
            setvbuf(stderr, NULL, _IONBF, 0);
        if (freopen(path, "ab", stdout) != NULL)
            setvbuf(stdout, NULL, _IONBF, 0);
        snprintf(s_trace_path, sizeof(s_trace_path), "%s/trace.jsonl", s_directory);
    }
    SDL_GetLogOutputFunction(&s_previous_log, &s_previous_log_data);
    SDL_SetLogOutputFunction(log_output, NULL);
    if (GetSystemDirectoryA(system_dir, sizeof(system_dir))) {
        snprintf(path, sizeof(path), "%s/dbghelp.dll", system_dir);
        s_dbghelp = LoadLibraryA(path);
    }
    procedure.generic = s_dbghelp ? GetProcAddress(s_dbghelp, "MiniDumpWriteDump") : NULL;
    s_write_dump = procedure.dump;
    s_request = CreateEventA(NULL, FALSE, FALSE, NULL);
    s_done = CreateEventA(NULL, TRUE, FALSE, NULL);
    if (s_request != NULL && s_done != NULL)
        s_worker = CreateThread(NULL, 0, crash_writer, NULL, 0, NULL);
    if (s_worker != NULL) {
        s_previous_filter = SetUnhandledExceptionFilter(crash_filter);
        s_previous_abort = signal(SIGABRT, abort_signal);
    } else {
        write_handle(s_log, "Crash worker unavailable; Windows default handling remains active.\r\n");
    }
    return;
unavailable:
    s_directory[0] = '\0';
    fprintf(stderr, "PORT diagnostics directory could not be created: %s\n", SDL_GetError());
}

const char *port_diagnostics_trace_path(void) { return s_trace_path; }

void port_diagnostics_note(const char *label, const char *value)
{
    char line[2048];
    snprintf(line, sizeof(line), "%s=%s\r\n", label, value ? value : "");
    write_handle(s_log, line);
}

void port_diagnostics_close(int status, const char *reason)
{
    char value[32];
    snprintf(value, sizeof(value), "%d", status);
    port_diagnostics_note("exit_status", value);
    port_diagnostics_note("stop_reason", reason);
    SDL_SetLogOutputFunction(s_previous_log, s_previous_log_data);
    if (s_worker != NULL) {
        SetUnhandledExceptionFilter(s_previous_filter);
        signal(SIGABRT, s_previous_abort);
        InterlockedExchange(&s_shutdown, 1);
        SetEvent(s_request);
        WaitForSingleObject(s_worker, INFINITE);
        CloseHandle(s_worker);
        s_worker = NULL;
    }
    if (s_request != NULL) CloseHandle(s_request);
    if (s_done != NULL) CloseHandle(s_done);
    if (s_dbghelp != NULL) FreeLibrary(s_dbghelp);
    if (s_log != INVALID_HANDLE_VALUE) CloseHandle(s_log);
    s_log = INVALID_HANDLE_VALUE;
}
#else
void port_diagnostics_init(int debug, const char *root) { (void)debug; (void)root; }
const char *port_diagnostics_trace_path(void) { return ""; }
void port_diagnostics_note(const char *label, const char *value) { (void)label; (void)value; }
void port_diagnostics_close(int status, const char *reason) { (void)status; (void)reason; }
#endif

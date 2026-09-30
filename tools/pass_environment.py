"""Pinned DOS pass environment of the MSC compiler profiles (integ27).

MSC 5.10 CL starts each pass (C1, C2, C3) with a fresh environment that it
builds itself: `MSC_CMD_FLAGS=-il <TEMP prefix> -ef <pass directory>\\c23.err
...`, `NO87=`, `;C_FILE_INFO` and the pass path.  The pass copies these
strings into its near heap, so the DOS-visible pass directory and TEMP
prefix consume part of the budget that also holds the translation unit's
symbol table (FACT-tu-extern-count-limits-cse).  Near that budget, code
generation changes: whole seg000 keeps end_hiscore's dead `pop si` only
with a pass directory string of at least six characters.  Free conventional
memory above the passes' needs had no observable effect (see the integ27
report), but it is pinned as well so that both hosts provide the same pass
memory.

The profile therefore pins:
  * `pass_environment.dos_directory`: the DOS-visible pass directory, which
    MS-DOS Player derives from the host path (upper-cased) and DOSBox-X
    reproduces with a hash-verified read-only mirror mounted at the same
    drive and path;
  * `pass_environment.temp`: TEMP (`.`, giving `-il .\\NNNNNN`);
  * `runner.pass_memory_paragraphs` / `independent_runner.pass_memory_paragraphs`:
    paragraphs from each pass's PSP to the end of the free arena, measured by
    `tools/probes/memprobe.c` substituted for one pass with CL's `-Bn` option.
"""
import json
import os
import re
import shutil
import subprocess
import tempfile
from pathlib import Path

from common import ROOT, identity, read_json, require

PASSES = ('C1', 'C2', 'C3')
_DOS_NAME = re.compile(r"^[A-Z0-9_$~!#%&'(){}^@`-]{1,8}(?:\.[A-Z0-9_$~!#%&'(){}^@`-]{1,3})?$")


def lock():
    return read_json(ROOT/'layout/toolchain.json')


def dos_visible(path):
    """The directory string MS-DOS Player hands to DOS programs for a host path."""
    return str(Path(path)).replace('/', '\\').rstrip('\\').upper()


def pass_environment(config):
    """The pinned pass environment of a C profile, or None for non-CL profiles."""
    if config.get('executable') != 'CL.EXE':
        return None
    env = config.get('pass_environment')
    require(isinstance(env, dict) and env.get('schema') == 'dos-pass-environment-v1',
            'C compiler profile lacks its pinned DOS pass environment')
    require(re.fullmatch(r'[A-Z]:(\\[^\\]+)+', env.get('dos_directory', '')) is not None and
            env.get('temp') == '.', 'Invalid pinned DOS pass environment')
    return env


def check_player_directory(config, tc):
    """The pinned runner must expose exactly the pinned pass directory string."""
    env = pass_environment(config)
    if env is None:
        return
    require(dos_visible(tc) == env['dos_directory'],
            f'MS-DOS Player pass directory {dos_visible(tc)} differs from the pinned '
            f'{env["dos_directory"]}: pass strings are part of the compiler profile')


def independent_runner():
    runner = lock()['independent_runner']
    path = Path(runner['path'])
    require(path.is_file() and identity(path.read_bytes()) ==
            {'size': runner['size'], 'sha256': runner['sha256']},
            'Independent DOSBox-X runner hash mismatch')
    return runner


def mirror(profile, config):
    """Hash-verified read-only DOS view of a C profile at its pinned DOS path.

    Returns (host root to mount, drive letter)."""
    from compiler import profile_file
    env = pass_environment(config)
    drive, *parts = env['dos_directory'].split('\\')
    require(all(_DOS_NAME.fullmatch(p) for p in parts),
            'Pinned DOS pass directory is not representable by the independent DOS host')
    root = ROOT/'build/crosschecks/passdirs'/profile
    target = root.joinpath(*parts)
    logical = Path(config.get('logical_directory', config['directory'])).as_posix()
    for item in config['files']:
        relative = Path(item['path']).as_posix()
        require(relative.startswith(logical + '/'), 'Pinned file outside its profile directory')
        destination = target/relative[len(logical)+1:]
        expected = {'size': item['size'], 'sha256': item['sha256']}
        if not (destination.is_file() and identity(destination.read_bytes()) == expected):
            destination.parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(profile_file(config, item['path']), destination)
        require(identity(destination.read_bytes()) == expected, 'Pass directory mirror hash mismatch')
    return root, drive[0]


def dosbox_conf(work, mounts, commands, runner):
    lines = ['[sdl]', 'output=surface', '[cpu]', 'cycles=max', '[mixer]', 'nosound=true',
             *runner['conf'], '[autoexec]', *mounts, *commands]
    (work/'dosbox.conf').write_text('\n'.join(lines) + '\n')
    return work/'dosbox.conf'


def run_dosbox(work, conf, timeout=45):
    env = os.environ.copy(); env.update(SDL_VIDEODRIVER='dummy', SDL_AUDIODRIVER='dummy')
    startup = subprocess.STARTUPINFO(); startup.dwFlags |= subprocess.STARTF_USESHOWWINDOW
    startup.wShowWindow = 0
    cmd = [lock()['independent_runner']['path'], '-conf', str(conf), '-fastlaunch', '-exit']
    result = subprocess.run(cmd, cwd=work, env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                            timeout=timeout, creationflags=subprocess.CREATE_NO_WINDOW, startupinfo=startup)
    return cmd, result


def dosbox_compile_commands(profile, config, flags):
    """Mounts and commands for one independent C compile in the current work dir."""
    root, drive = mirror(profile, config)
    env = pass_environment(config)
    require(drive != 'E', 'Pinned pass drive collides with the work drive')
    mounts = [f'mount {drive.lower()} "{root}" -ro']
    commands = ['e:', 'set PATH=' + env['dos_directory'], 'set TEMP=' + env['temp']]
    return mounts, commands, env['dos_directory'] + '\\' + config['executable']


# ------------------------------------------------------------ measurement
def _player_run(runner, tc, argv, work):
    env = {'PATH': str(tc), 'MSDOS_PATH': str(tc), 'TEMP': '.', 'TMP': '.', 'MSDOS_TEMP': '.'}
    return subprocess.run([runner['path'], '-e', '-v5.00', *argv], cwd=work, env=env,
                          stdout=subprocess.PIPE, stderr=subprocess.STDOUT, timeout=60,
                          creationflags=getattr(subprocess, 'CREATE_NO_WINDOW', 0))


def build_probe(profile='msc510-medium'):
    """Compile and link tools/probes/memprobe.c with the pinned profile (small model)."""
    from compiler import verify_toolchain
    from preprocessor import prepare
    config, runner = verify_toolchain(profile)
    tc = (ROOT/config['directory']).resolve()
    out = ROOT/'build/pass-environment'; out.mkdir(parents=True, exist_ok=True)
    work = Path(tempfile.mkdtemp(prefix='probe', dir=out))
    source = (ROOT/'tools/probes/memprobe.c').read_bytes()
    (work/'MEMPROBE.C').write_bytes(source.replace(b'\r\n', b'\n').replace(b'\n', b'\r\n'))
    # Headers come from the hash-verified profile INCLUDE directory.
    result = _player_run(runner, tc, [str(tc/'CL.EXE'), '/AS', '/Gs', '/I', str(tc/'INCLUDE'),
                                      'MEMPROBE.C', '/link', '/NOD', str(tc/'SLIBCR.LIB')], work)
    require((work/'MEMPROBE.EXE').is_file(), 'Pass-environment probe build failed: ' +
            result.stdout.decode('ascii', 'replace')[-300:])
    return work/'MEMPROBE.EXE'


def parse_probe(text):
    rows = {}
    arena_end = 0
    for line in text.splitlines():
        words = line.split()
        if words and words[0] in ('psp', 'env_paragraphs'):
            rows[words[0]] = int(words[1])
        elif words and words[0] == 'mcb':
            segment, owner, size = int(words[1]), int(words[3]), int(words[4])
            if owner == 0:
                arena_end = max(arena_end, segment + 1 + size)
    require('psp' in rows and arena_end > rows['psp'], 'Incomplete pass-environment probe report')
    return {'psp': rows['psp'], 'env_paragraphs': rows['env_paragraphs'], 'arena_end': arena_end,
            'pass_paragraphs': arena_end - rows['psp']}


def measure_player(profile, probe):
    from compiler import verify_toolchain
    config, runner = verify_toolchain(profile)
    tc = (ROOT/config['directory']).resolve()
    check_player_directory(config, tc)
    result = {}
    for index, name in enumerate(PASSES, 1):
        work = Path(tempfile.mkdtemp(prefix='mp', dir=ROOT/'build/pass-environment'))
        (work/'UNIT.C').write_bytes(b'int f(int a) { return a + 1; }\r\n')
        # CL forwards its -Bn argument through a DOS command tail; an absolute
        # host path under a freshly named clone can exceed the DOS runner's
        # path limits. Keep the substituted pass alongside UNIT.C instead.
        shutil.copyfile(probe, work/'MEMPROBE.EXE')
        _player_run(runner, tc, [str(tc/config['executable']), '/c', *config['flags'],
                                 f'-B{index}', 'MEMPROBE.EXE', 'UNIT.C'], work)
        require((work/'MEMPROBE.TXT').is_file(), f'Probe did not run as {name}')
        result[name] = parse_probe((work/'MEMPROBE.TXT').read_text())
    return result


def measure_dosbox(profile, probe):
    """Probe each pass under the independent runner with the crosscheck layout.

    The probe sits in a second mirror at the same DOS path and name as the pass
    it replaces, so the pass path strings keep their exact lengths."""
    from compiler import verify_toolchain
    config, _ = verify_toolchain(profile)
    runner = independent_runner()
    result = {}
    for index, name in enumerate(PASSES, 1):
        work = Path(tempfile.mkdtemp(prefix='md', dir=ROOT/'build/pass-environment'))
        (work/'UNIT.C').write_bytes(b'int f(int a) { return a + 1; }\r\n')
        mounts, commands, exe = dosbox_compile_commands(profile, config, config['flags'])
        env = pass_environment(config)
        _, *parts = env['dos_directory'].split('\\')
        probe_root = work/'PROBE'
        (probe_root.joinpath(*parts)).mkdir(parents=True)
        shutil.copyfile(probe, probe_root.joinpath(*parts, f'{name}.EXE'))
        probe_dos = 'F:\\' + '\\'.join(parts) + f'\\{name}.EXE'
        batch = ['@echo off', f'{exe} /c {" ".join(config["flags"])} -B{index} {probe_dos} UNIT.C > COMP.LOG',
                 'exit']
        (work/'RUN.BAT').write_bytes(('\r\n'.join(batch) + '\r\n').encode('ascii'))
        conf = dosbox_conf(work, mounts + [f'mount f "{probe_root}" -ro', f'mount e "{work}"'],
                           commands + ['RUN.BAT'], runner)
        run_dosbox(work, conf)
        require((work/'MEMPROBE.TXT').is_file(), f'Probe did not run as {name} under DOSBox-X')
        result[name] = parse_probe((work/'MEMPROBE.TXT').read_text())
    return result


def main():
    probe = build_probe()
    config = lock()
    report = {}
    for profile, row in config['profiles'].items():
        if pass_environment(row) is None or profile == 'msc600a-medium-zi':
            continue
        report[profile] = {'player': measure_player(profile, probe),
                           'dosbox': measure_dosbox(profile, probe)}
    out = ROOT/'build/pass-environment/report.json'
    out.write_text(json.dumps(report, indent=1))
    for profile, hosts in report.items():
        for host, passes in hosts.items():
            print(profile, host, {k: v['pass_paragraphs'] for k, v in passes.items()})
    return report


if __name__ == '__main__':
    main()

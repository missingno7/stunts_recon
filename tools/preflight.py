"""Report every missing local prerequisite at once, before any gate work starts."""
import shutil
import sys
from pathlib import Path

from common import ROOT, read_json

# Pinned Python packages, installed into ignored build/python (see requirements-build.txt).
PACKAGES = {'capstone': '5.0.3', 'unicorn': '2.1.4'}
INSTALL = 'python -m pip install --no-user --target build/python -r requirements-build.txt'
ASSETS = ('MCGA.HDR', 'EGA.CMN', 'MCGA.DIF', 'MCGA.COD')
# Host tools used only by the port tests; the same defaults as port/build.py and host_probe.py.
PORT_GCC = Path(r'C:\msys64\mingw32\bin\gcc.exe')
PORT_SDL = Path(r'C:\tools\sdl3-3.4.16-i686')
HOST_GCC = Path(r'C:\msys64\mingw64\bin\gcc.exe')


def _directories(node):
    """Every `directory` value recorded anywhere inside one toolchain profile."""
    if isinstance(node, dict):
        for key, value in node.items():
            if key == 'directory' and isinstance(value, str):
                yield value
            else:
                yield from _directories(value)
    elif isinstance(node, list):
        for value in node:
            yield from _directories(value)


def problems(port=True):
    found = []
    site = ROOT/'build/python'
    for name, version in PACKAGES.items():
        if not (site/name/'__init__.py').is_file() or not (site/f'{name}-{version}.dist-info').is_dir():
            found.append(f'Python package {name}=={version} is not installed in build/python. Run: {INSTALL}')
    for name in ASSETS:
        if not (ROOT/'assets'/name).is_file():
            found.append(f'Original asset assets/{name} is missing (copy it from a Stunts 1.1 MCGA installation).')
    toolchain = read_json(ROOT/'layout/toolchain.json')
    for key in ('runner', 'independent_runner'):
        path = Path(toolchain[key]['path'])
        if not path.is_file():
            found.append(f'Pinned {key.replace("_", " ")} not found at {path} (layout/toolchain.json).')
    for name, profile in sorted(toolchain['profiles'].items()):
        for directory in sorted(set(_directories(profile))):
            if not Path(directory).is_dir():
                found.append(f'Pinned toolchain for profile {name} not found at {directory} (layout/toolchain.json).')
    if port:
        if not shutil.which('gcc') and not HOST_GCC.is_file():
            found.append(f'Host GCC not found on PATH or at {HOST_GCC} (needed by the port compile checks).')
        if not PORT_GCC.is_file():
            found.append(f'i686 MinGW GCC not found at {PORT_GCC} (needed to build the SDL3 port).')
        if not PORT_SDL.is_dir():
            found.append(f'SDL3 i686 development package not found at {PORT_SDL} (needed to build the SDL3 port).')
    return found


def check(port=True):
    found = problems(port)
    if found:
        raise SystemExit('Environment is not ready:\n' + '\n'.join('  - ' + line for line in found))


if __name__ == '__main__':
    check(port='--no-port' not in sys.argv)
    print('Environment ready.')

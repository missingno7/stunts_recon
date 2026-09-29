[CmdletBinding()]
param([switch]$SetupOnly)
$ErrorActionPreference = 'Stop'
$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$workspace = Join-Path $repoRoot 'build\porting\runtime-capture'
New-Item -ItemType Directory -Force -Path $workspace | Out-Null
$assetsSource = Join-Path $repoRoot 'assets'
$exe = 'C:\tools\dosbox-x-2026.08.31-vsbuild-win64\bin\x64\Release\dosbox-x.exe'
if (-not (Test-Path -LiteralPath $assetsSource -PathType Container)) { throw "Missing immutable assets directory: $assetsSource" }
if (-not (Test-Path -LiteralPath (Join-Path $assetsSource 'LOAD.EXE') -PathType Leaf)) { throw 'assets/LOAD.EXE is missing' }
if (-not (Test-Path -LiteralPath $exe -PathType Leaf)) { throw "DOSBox-X debugger release not found: $exe" }
$tag = Get-Date -Format 'yyyyMMddTHHmmssfff'
$runRoot = Join-Path $workspace "run-$tag"
$game = Join-Path $runRoot 'game'
$logs = Join-Path $runRoot 'logs'
New-Item -ItemType Directory -Path $game,$logs | Out-Null
Get-ChildItem -LiteralPath $assetsSource -Force | Copy-Item -Destination $game -Recurse -Force
if (-not (Test-Path -LiteralPath (Join-Path $game 'LOAD.EXE') -PathType Leaf)) { throw 'Scratch copy does not contain LOAD.EXE' }
$logfile = Join-Path $logs 'dosbox.log'
$config = Join-Path $runRoot 'capture.conf'
$confText = @"
[sdl]
fullscreen=false
[dosbox]
fastbioslogo=true
startbanner=false
[cpu]
cycles=fixed 30000
core=normal
[log]
logfile=$logfile
vga=debug
int10=debug
pit=debug
pic=debug
keyboard=debug
io=debug
files=normal
int21=true
[autoexec]
mount c `"$game`"
c:
load.exe /u MCGA /ssb
"@
Set-Content -LiteralPath $config -Value $confText -Encoding ascii
Write-Host "Scratch game: $game"
Write-Host "Capture config: $config"
Write-Host "Debugger CPU log will be written as LOGCPU.TXT in the session directory."
Write-Host 'Follow docs/porting/CAPTURE.md for the debugger commands.'
if (-not $SetupOnly) {
    Push-Location $runRoot
    try {
        & $exe -conf $config -break-start -console
        if ($LASTEXITCODE -and $LASTEXITCODE -ne 0) { throw "DOSBox-X exited with status $LASTEXITCODE" }
    }
    finally { Pop-Location }
    Write-Host "DOSBox-X closed. Scratch capture retained at: $runRoot"
}

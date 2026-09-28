[CmdletBinding()]
param(
    [string]$RepoRoot = (Join-Path $PSScriptRoot '..\..'),
    [string]$DosboxX = 'C:\tools\dosbox-x\dosbox-x.exe',
    [ValidateRange(1, 300)]
    [int]$Seconds = 110
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepoRoot).Path
$runner = (Resolve-Path -LiteralPath $DosboxX).Path
$assets = Join-Path $root 'assets'
if (-not (Test-Path -LiteralPath (Join-Path $assets 'LOAD.EXE'))) {
    throw "Original assets/LOAD.EXE is missing under $assets."
}

$runtimeRoot = Join-Path $root 'build\porting\runtime-probe'
New-Item -ItemType Directory -Force -Path $runtimeRoot | Out-Null
$runId = (Get-Date -Format 'yyyyMMdd-HHmmss') + '-' + [Guid]::NewGuid().ToString('N').Substring(0, 8)
$runDir = Join-Path $runtimeRoot $runId
$game = Join-Path $runDir 'game'
$logs = Join-Path $runDir 'logs'
New-Item -ItemType Directory -Force -Path $game,$logs | Out-Null
Copy-Item -Path (Join-Path $assets '*') -Destination $game -Recurse -Force

$log = Join-Path $logs 'runtime-session.log'
$conf = Join-Path $runDir 'runtime-session.conf'
$configText = @"
[sdl]
fullscreen=false
[dosbox]
fastbioslogo=true
startbanner=false
[cpu]
cycles=fixed 30000
core=normal
[log]
logfile=$log
vga=debug
int10=debug
pit=debug
keyboard=debug
pic=debug
io=debug
files=normal
int21=true
[dos]
log console=true
[autoexec]
mount c "$game"
c:
load.exe /u MCGA /ssb
"@
[IO.File]::WriteAllText($conf, $configText, [Text.Encoding]::ASCII)

$arguments = '-conf "' + $conf + '" -noconsole'
$process = Start-Process -FilePath $runner -ArgumentList $arguments -PassThru -WindowStyle Hidden
try {
    if (-not $process.WaitForExit($Seconds * 1000)) {
        Stop-Process -Id $process.Id -Force
        $process.WaitForExit()
    }
} finally {
    if (-not $process.HasExited) {
        Stop-Process -Id $process.Id -Force
        $process.WaitForExit()
    }
}
Write-Output "DOSBox-X process $($process.Id) completed; run: $runDir; log: $log"

param(
    [string]$Python = "python",
    [int]$Port = 27184,
    [string]$Adb = "adb",
    [string]$Serial = "",
    [switch]$NoAdbForward
)

$ErrorActionPreference = "Stop"
$server = Join-Path $PSScriptRoot "mcp_server.py"
$serverArgs = @($server, "--port", [string]$Port, "--adb", $Adb)

if ($Serial) {
    $serverArgs += @("--serial", $Serial)
}
if ($NoAdbForward) {
    $serverArgs += "--no-adb-forward"
}

& $Python @serverArgs
exit $LASTEXITCODE

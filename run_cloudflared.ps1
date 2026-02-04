$ErrorActionPreference = 'Stop'
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
Set-Location $scriptDir
$cloud = Join-Path $scriptDir 'cloudflared.exe'
if (-not (Test-Path $cloud)) {
    Write-Host "Downloading cloudflared..."
    $url = 'https://github.com/cloudflare/cloudflared/releases/latest/download/cloudflared-windows-amd64.exe'
    Invoke-WebRequest -Uri $url -OutFile $cloud
    Write-Host "Downloaded to $cloud"
}
# Kill existing cloudflared
Get-Process -Name cloudflared -ErrorAction SilentlyContinue | ForEach-Object { try { $_.Kill() } catch {} }
Start-Sleep -Milliseconds 200
$log = Join-Path $scriptDir 'cloudflared.log'
if (Test-Path $log) { Remove-Item $log -Force }
Write-Host "Starting cloudflared tunnel to http://127.0.0.1:8080 (logs -> $log)"
# Use cmd start to run in background and redirect output
cmd /c start "" /B "$cloud" tunnel --url http://127.0.0.1:8080 > "$log" 2>&1
# Wait for the tunnel URL to appear in logs
$tries = 0
$url = $null
while (-not $url -and $tries -lt 30) {
    Start-Sleep -Seconds 1
    if (Test-Path $log) {
        $content = Get-Content -LiteralPath $log -ErrorAction SilentlyContinue -Raw
        if ($content -match '(https?://[\w\-\.]+\.trycloudflare\.com)') {
            $url = $Matches[1]
            break
        }
        if ($content -match '(https?://[\w\-\.]+)') {
            # fallback: first https URL
            $url = $Matches[1]
            break
        }
    }
    $tries++
}
if ($url) {
    Write-Host "[OK] cloudflared tunnel established: $url" -ForegroundColor Green
    Start-Process $url
} else {
    Write-Host '[ERROR] cloudflared did not publish a tunnel within timeout; see cloudflared.log' -ForegroundColor Red
    if (Test-Path $log) { Get-Content -LiteralPath $log -Tail 200 }
    exit 1
}
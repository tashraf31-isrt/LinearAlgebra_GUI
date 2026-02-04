Param(
    [Parameter(Mandatory=$true)]
    [string]$Token
)
$ErrorActionPreference = 'Stop'
$ngrok = 'd:\Programming\LinearAlgebra_GUI\ngrok.exe'
if (-not (Test-Path $ngrok)) { Write-Error "ngrok.exe not found at $ngrok"; exit 1 }
Write-Host "Applying authtoken..."
& $ngrok authtoken $Token
Start-Sleep -Milliseconds 200
# Kill any existing ngrok processes to avoid duplicates
Get-Process -Name ngrok -ErrorAction SilentlyContinue | ForEach-Object { try { $_.Kill() } catch {} }
Start-Sleep -Milliseconds 200
Write-Host "Starting ngrok http tunnel on port 8080..."
Start-Process -FilePath $ngrok -ArgumentList 'http 8080' -WindowStyle Hidden
$tries = 0
$url = $null
Write-Host "Waiting for ngrok to publish a tunnel (timeout 30s)..."
while (-not $url -and $tries -lt 30) {
    Start-Sleep -Seconds 1
    try {
        $resp = Invoke-RestMethod -Uri 'http://127.0.0.1:4040/api/tunnels' -ErrorAction Stop
        if ($resp.tunnels -and $resp.tunnels.Count -gt 0) {
            $t = $resp.tunnels | Where-Object { $_.public_url -like 'https:*' } | Select-Object -First 1
            if (-not $t) { $t = $resp.tunnels[0] }
            $url = $t.public_url
        }
    } catch {
        # ignore until available
    }
    $tries++
}
if ($url) {
    Write-Host "[OK] ngrok tunnel established: $url" -ForegroundColor Green
    Start-Process $url
} else {
    Write-Host "[ERROR] ngrok did not publish a tunnel within timeout" -ForegroundColor Red
    exit 1
}

# share_with_ngrok.ps1
# Starts server.exe and ngrok, then prints and opens the public ngrok URL
# Usage: Run in project folder with PowerShell (ExecutionPolicy Bypass if needed)

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
Set-Location $scriptDir

function ExitWith($msg) {
    Write-Host "[ERROR] $msg" -ForegroundColor Red
    exit 1
}

# Ensure server.exe exists
if (-not (Test-Path "server.exe")) {
    ExitWith "server.exe not found in $scriptDir. Build the project first (run build.bat)."
}

# Check for ngrok in PATH or local copy
$ngrokCmd = Get-Command ngrok -ErrorAction SilentlyContinue
$localNgrok = Join-Path $scriptDir 'ngrok.exe'
if (-not $ngrokCmd -and -not (Test-Path $localNgrok)) {
    Write-Host "ngrok not found. You can download it for free from https://ngrok.com/download" -ForegroundColor Yellow
    ExitWith "Install ngrok and make sure 'ngrok' is in PATH or copy ngrok.exe into this folder."
}

# Start server if not running
Write-Host "Starting server.exe..." -ForegroundColor Green
Start-Process -FilePath "$scriptDir\server.exe" -WindowStyle Normal

# Wait a few seconds for server to initialize
Start-Sleep -Seconds 1

# Start ngrok
Write-Host "Starting ngrok tunnel on port 8080..." -ForegroundColor Green
if ($ngrokCmd) {
    Start-Process -FilePath "ngrok" -ArgumentList "http 8080" -WindowStyle Normal
} else {
    Start-Process -FilePath "$localNgrok" -ArgumentList "http 8080" -WindowStyle Normal
}

# Poll ngrok API for tunnel info
$tunnelUrl = $null
$tries = 0
Write-Host "Waiting for ngrok to establish tunnel (this may take a few seconds)..." -ForegroundColor Yellow
while (-not $tunnelUrl -and $tries -lt 30) {
    try {
        Start-Sleep -Seconds 1
        $resp = Invoke-RestMethod -Uri http://127.0.0.1:4040/api/tunnels -ErrorAction Stop
        if ($resp.tunnels -and $resp.tunnels.Count -gt 0) {
            # prefer https
            $t = $resp.tunnels | Where-Object { $_.public_url -like 'https:*' } | Select-Object -First 1
            if (-not $t) { $t = $resp.tunnels[0] }
            $tunnelUrl = $t.public_url
        }
    } catch {
        # ignore until available
    }
    $tries++
}

if (-not $tunnelUrl) {
    ExitWith "ngrok did not respond on http://127.0.0.1:4040. Make sure ngrok is running and not blocked by firewall."
}

Write-Host "ngrok tunnel established:" -ForegroundColor Green
Write-Host "  $tunnelUrl" -ForegroundColor Cyan

# Open the public URL in default browser
Start-Process $tunnelUrl

Write-Host "⚠️  Reminder: This exposes your local server to the internet via ngrok. Do not share the URL publicly unless you want other people to access your instance." -ForegroundColor Yellow
Write-Host "If you want, I can add a simple token check to the server to restrict access."

# End of script

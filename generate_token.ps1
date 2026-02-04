# generate_token.ps1
# Generates a random 32-character hex token and saves it to auth.token
$bytes = New-Object byte[] 16
[Security.Cryptography.RandomNumberGenerator]::Create().GetBytes($bytes)
$token = ($bytes | ForEach-Object { $_.ToString('x2') }) -join ''
Set-Content -Path "auth.token" -Value $token
Write-Host "Token generated and saved to auth.token:" -ForegroundColor Green
Write-Host $token -ForegroundColor Cyan
Write-Host "You can provide this token in the request header 'X-API-Key' or in the JSON body as 'token'." -ForegroundColor Yellow

Param()
$ErrorActionPreference = 'Stop'
$dest = "d:\Programming\LinearAlgebra_GUI\ngrok-stable-windows-amd64.zip"
$url = "https://bin.equinox.io/c/4VmDzA7iaHb/ngrok-stable-windows-amd64.zip"
Write-Host "Downloading ngrok from $url to $dest"
Invoke-WebRequest -Uri $url -OutFile $dest
Write-Host "Extracting to d:\Programming\LinearAlgebra_GUI\"
Expand-Archive -LiteralPath $dest -DestinationPath "d:\Programming\LinearAlgebra_GUI\" -Force
Write-Host "Removing zip file..."
Remove-Item $dest -Force
if (Test-Path "d:\Programming\LinearAlgebra_GUI\ngrok.exe") {
    Write-Host "ngrok.exe is present in project folder."
} else {
    Write-Error "ngrok.exe not found after extraction." 
    exit 1
}
Write-Host "Installation complete."

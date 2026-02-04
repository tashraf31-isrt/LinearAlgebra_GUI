$ErrorActionPreference = 'Stop'
$url = 'https://bin.equinox.io/c/4VmDzA7iaHb/ngrok-v3-stable-windows-amd64.zip'
$dest = 'd:\Programming\LinearAlgebra_GUI\ngrok-v3.zip'
Write-Host "Downloading ngrok v3 from $url to $dest"
Invoke-WebRequest -Uri $url -OutFile $dest
Write-Host "Extracting to d:\Programming\LinearAlgebra_GUI\ngrok-v3\"
Expand-Archive -LiteralPath $dest -DestinationPath 'd:\Programming\LinearAlgebra_GUI\ngrok-v3' -Force
$found = Get-ChildItem 'd:\Programming\LinearAlgebra_GUI\ngrok-v3' -Filter 'ngrok.exe' -Recurse | Select-Object -First 1
if ($found) {
    Write-Host "Found ngrok at: $($found.FullName). Replacing existing ngrok.exe"
    Copy-Item $found.FullName -Destination 'd:\Programming\LinearAlgebra_GUI\ngrok.exe' -Force
    Remove-Item $dest -Force
    Remove-Item -Recurse 'd:\Programming\LinearAlgebra_GUI\ngrok-v3' -Force
    Write-Host "ngrok updated to latest v3 binary."
} else {
    Write-Error "ngrok.exe not found inside the archive. Aborting."
    exit 1
}

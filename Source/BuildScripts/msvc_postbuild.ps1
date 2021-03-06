function Get-CurrFolderName
{
    $s = $(Get-Location).Path.Split("\")
    return $s[$s.Length - 1]
}

Push-Location

while ($(Get-CurrFolderName) -ne "cnnr-engine")
{
    cd ..
}

function Copy-Libraries($target, $configuration)
{
    if (Test-Path "bin\windows\$target\$configuration")
    {
        Copy-Item -Path "$env:CONNORLIB_HOME\bin\windows\$target\serialization.dll" -Destination "bin\windows\$target\$configuration\serialization.dll"
        Copy-Item -Path "$env:CONNORLIB_HOME\bin\windows\$target\imageload.dll" -Destination "bin\windows\$target\$configuration\imageload.dll"
        Copy-Item -Path "$env:CONNORLIB_HOME\bin\windows\$target\messageipc.dll" -Destination "bin\windows\$target\$configuration\messageipc.dll"
        Copy-Item -Path "Source\Vendor\luajit\bin\windows_$target\lua51.dll" -Destination "bin\windows\$target\$configuration\lua51.dll"
    }
}

Copy-Libraries "x86" "Debug"
Copy-Libraries "x86" "Release"
Copy-Libraries "x64" "Debug"
Copy-Libraries "x64" "Release"

Pop-Location

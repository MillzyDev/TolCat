Param(
    [Parameter(Mandatory = $true)]
    [String] $modName = "",

    [Parameter(Mandatory = $true)]
    [String] $targetGameDir = "",

    [Parameter(Mandatory = $true)]
    [String] $buildDir = ""
)

$proxySource = Join-Path -Path $buildDir -ChildPath "version.dll"
$proxyDest = $targetGameDir
Copy-Item $proxySource -Destination $proxyDest -Force

$preloaderSource = Join-Path -Path $buildDir -ChildPath "Preloader.dll"
$proxyDest = Join-Path -Path $targetGameDir -ChildPath $modName"_Data"
Copy-Item $preloaderSource -Destination $proxyDest -Force

$tolCatSource = Join-Path -Path $buildDir -ChildPath "TolCat.dll"
$tolCatDest = Join-Path -Path $targetGameDir -ChildPath $modName"_Data"
Copy-Item $tolCatSource -Destination $tolCatDest -Force

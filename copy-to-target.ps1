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

$modDataName = -join($modName, "_Data")
$modDataDir = Join-Path -Path $targetGameDir -ChildPath $modDataName
New-Item -Type Directory -Path $targetGameDir -Name $modName"_Data" -Force

$preloaderSource = Join-Path -Path $buildDir -ChildPath "Preloader.dll"
$proxyDest = Join-Path -Path $modDataDir -ChildPath "Preloader.dll"
Copy-Item $preloaderSource -Destination $proxyDest -Force

$tolCatSource = Join-Path -Path $buildDir -ChildPath "TolCat.dll"
$tolCatDest = Join-Path -Path $modDataDir -ChildPath "TolCat.dll"
Copy-Item $tolCatSource -Destination $tolCatDest -Force

$gluonSource = Join-Path -Path $buildDir -ChildPath "Gluon/Gluon.dll"
$gluonDest = Join-Path -Path $modDataDir -ChildPath "Gluon.dll"
Copy-Item $gluonSource -Destination $gluonDest -Force
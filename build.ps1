# powershell -ExecutionPolicy Bypass -File "build.ps1"
param(
    [string]$ToolchainPath = "C:\Arm\10_2021_10",
    [string]$MakePath = "C:\msys64\usr\bin\make.exe"
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"
$RepoRoot = $PSScriptRoot

function Add-PathEntry {
    param(
        [Parameter(Mandatory=$true)]
        [string]$PathToAdd
    )

    $fullPath = [System.IO.Path]::GetFullPath($PathToAdd)
    $normalizedTarget = $fullPath.TrimEnd('\\').ToLowerInvariant()

    $currentEntries = @()
    if (-not [string]::IsNullOrWhiteSpace($env:PATH)) {
        $currentEntries = $env:PATH -split ';'
    }

    foreach ($entry in $currentEntries) {
        $trimmedEntry = $entry.Trim()
        if ([string]::IsNullOrEmpty($trimmedEntry)) {
            continue
        }

        try {
            $normalizedExisting = ([System.IO.Path]::GetFullPath($trimmedEntry)).TrimEnd('\\').ToLowerInvariant()
        } catch {
            continue
        }

        if ($normalizedExisting -eq $normalizedTarget) {
            return
        }
    }

    if ([string]::IsNullOrEmpty($env:PATH)) {
        $env:PATH = $fullPath
    } else {
        $env:PATH = "$fullPath;" + $env:PATH
    }
}

Write-Host "Using toolchain path: $ToolchainPath"
Write-Host "Using make: $MakePath"

if (-not (Test-Path -Path $ToolchainPath)) {
    throw "Toolchain path '$ToolchainPath' was not found."
}

$toolchainBin = Join-Path $ToolchainPath "bin"
if (-not (Test-Path -Path $toolchainBin)) {
    throw "Toolchain bin directory '$toolchainBin' was not found."
}

if (-not (Test-Path -Path $MakePath)) {
    throw "make executable '$MakePath' was not found."
}

$makeDir = Split-Path -Path $MakePath -Parent
if ([string]::IsNullOrWhiteSpace($makeDir)) {
    throw "Unable to determine the directory for make executable '$MakePath'."
}

$normalizedToolchainPath = $ToolchainPath -replace "\\", "/"
$normalizedToolchainPath = $normalizedToolchainPath.TrimEnd('/')
$toolchainVar = "TOOLCHAIN_PATH=$normalizedToolchainPath"
$env:TOOLCHAIN_PATH = $normalizedToolchainPath

Add-PathEntry -PathToAdd $toolchainBin
Add-PathEntry -PathToAdd $makeDir

function Invoke-Make {
    param(
        [Parameter(Mandatory=$true)]
        [string[]]$Arguments
    )

    Write-Host "Running: $MakePath $($Arguments -join ' ')" -ForegroundColor Cyan
    & $MakePath @Arguments
    if ($LASTEXITCODE -ne 0) {
        throw "make failed with exit code $LASTEXITCODE"
    }
}

Push-Location -Path $RepoRoot
try {
    Write-Host "Building BCM8915X bare-metal driver library..." -ForegroundColor Green
    Invoke-Make -Arguments @($toolchainVar)

    Write-Host "Building BCM8915X reference applications..." -ForegroundColor Green
    Invoke-Make -Arguments @("-C","App",$toolchainVar)
}
finally {
    Pop-Location
}

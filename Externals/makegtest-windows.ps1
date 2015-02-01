$allVersions = @("120")
$allPlatforms = @("Win32", "x64")
$allTargets = @("Debug", "Release")

$versions = @()
$platforms = @()
$targets = @()
$buildAll = $FALSE

if ($args.Count -eq 0) {
    $buildAll = $TRUE
} elseif ($args.Count -eq 1) {
    if ($args[0].ToLower() -eq "all") {
        $buildAll = $TRUE
    } else {
        Write-Host "Error: If you specify configuration you must also supply MSVC version and vice versa. Must be of the form [Platform:Target Version] or [All]"
    }
}

if ($buildAll -eq $TRUE) {
    $versions = $allVersions
    $platforms = $allPlatforms
    $targets = $allTargets
} else {
    $inputConfiguration = $args[0].ToLower()
    $inputVersion = $args[1].ToLower()

    if ($inputConfiguration -ne "all") {
        $configurationSplit = $inputConfiguration.Split(":")

        if ($configurationSplit.Count -ne 2) {
            Write-Host "Error: Configuration is invalid. Must be of the form [Platform:Target] or [All]"

            exit -1
        } else {
            if ($configurationSplit[0] -eq "all") {
                $platforms = $allPlatforms
            } else {
                $validPlatform = $FALSE

                for ($i = 0; $i -lt $allPlatforms.Count; $i++) {
                    if ($configurationSplit[0] -eq $allPlatforms[$i].ToLower()) {
                        $platforms = @($allPlatforms[$i])
                        $validPlatform = $TRUE

                        break
                    }
                }

                if (!$validPlatform) {
                    Write-Host "Error: Configuration is invalid.  Must be of the form Platform:Release or All"

                    exit -1
                }
            }

            if ($configurationSplit[1] -eq "all") {
                $targets = $allTargets
            } else {
                $validTarget = $FALSE

                for ($i = 0; $i -lt $allTargets.Count; $i++) {
                    if ($configurationSplit[1] -eq $allTargets[$i].ToLower()) {
                        $targets = @($allTargets[$i])
                        $validTarget = $TRUE

                        break
                    }
                }

                if (!$validTarget) {
                    Write-Host "Error: Configuration is invalid.  Must be of the form Platform:Release or All"

                    exit -1
                }
            }
        }
    } else {
        $platforms = $allPlatforms
        $targets = $allTargets
    }

    if ($inputVersion -ne "all")  {
        $validVersion = $FALSE

        for ($i = 0; $i -lt $allVersions.Count; $i++) {
            if ($inputVersion -eq $allVersions[$i]) {
                $validVersion = $TRUE

                break
            }
        }

        if (!$validVersion) {
            Write-Host "Error: Version is invalid"

            exit -1
        }

        $versions = @($inputVersion)
    } else {
        $versions = $allVersions
    }
}

$filteredVersions = @()

for ($i = 0; $i -lt $versions.Count; $i++) {
    $currentVersion = $versions[$i]
    $vsToolsEnvVar = "VS" + $currentVersion + "COMNTOOLS"

    if (!(Test-Path env:$vsToolsEnvVar)) {
        if ($buildAll -ne $TRUE) {
            Write-Host "Error: The msvc version you have selected is not currently installed on your system.  We cannot build this version"

            exit -1
        }
    } else {
        $filteredVersions += $currentVersion
    }
}

$versions = $filteredVersions
$currentLocation = Get-Location
$currentLocation = Join-Path $currentLocation "bin"

if (!(Test-Path $currentLocation)) {
    New-Item -ItemType directory -Path $currentLocation | Out-Null
}

Set-Location $currentLocation

for ($i = 0; $i -lt $versions.Count; $i++) {
    for ($j = 0; $j -lt $platforms.Count; $j++) {
        for ($k = 0; $k -lt $targets.Count; $k++) {
            $currentVersion = $versions[$i]
            $currentPlatform = $platforms[$j]
            $currentTarget = $targets[$k]
            $buildMessage = "- Building: ${currentPlatform}:${currentTarget} MSVC:${currentVersion} -"
            $seperatorMessage = ""

            for ($l = 0; $l -lt $buildMessage.Length; $l++) {
                $seperatorMessage += '-'
            }

            Write-Host $seperatorMessage -ForegroundColor "Green"
            Write-Host $buildMessage -ForegroundColor "Green"
            Write-Host $seperatorMessage -ForegroundColor "Green"
            Write-Host ""

            $makeExpression = "cmake -G"

            if ($currentVersion -eq "120") {
                if ($currentPlatform -eq "win32") {
                    $makeExpression += ' "Visual Studio 12"'
                } else {
                    $makeExpression += ' "Visual Studio 12 Win64"'
                }
            }

            $makeExpression += ' -DBUILD_SHARED_LIBS=FALSE ../../../../../gtest/'

            if (!(Test-Path 'msvc')) {
                New-Item -ItemType directory -Path 'msvc' | Out-Null
            }

            Set-Location 'msvc'

            if (!(Test-Path $currentVersion)) {
                New-Item -ItemType directory -Path $currentVersion | Out-Null
            }

            Set-Location $currentVersion

            if (!(Test-Path "gtest")) {
                New-Item -ItemType directory -Path "gtest" | Out-Null
            }

            Set-Location "gtest"

            if (!(Test-Path $currentPlatform)) {
                New-Item -ItemType directory -Path $currentPlatform | Out-Null
            }

            Set-Location $currentPlatform

            try {
                Invoke-Expression -Command $makeExpression -ErrorAction Stop

                if ($lastexitcode -ne 0) {
                    Write-Host "Error: ${lastexitcode}" -ForegroundColor "Red"

                    exit $lastexitcode
                }
            } catch {
                Write-Host "Error: ${error}" -ForegroundColor "Red"

                exit $lastexitcode
            }

            $numProcessors = $env:NUMBER_OF_PROCESSORS -as [int]

            if (!$numProcessors) {
                $numProcessors = 2
            }

            $numProcessors *= 2
            $vsToolsEnvVar = "VS" + $currentVersion + "COMNTOOLS"
            $buildToolsCommand = Resolve-Path (Join-Path ((Get-Item env:$vsToolsEnvVar).value) "..\..\VC\vcvarsall.bat")
            $buildtenmp = '"{0}" x86_amd64 && msbuild gtest.sln /m:{1} /p:Platform={2} /p:Configuration={3}' -f $buildToolsCommand, $numProcessors, $currentPlatform, $currentTarget

            cmd.exe /C $buildtenmp

            if ($lastexitcode -ne 0) {
                Write-Host "Error: ${lastexitcode}" -ForegroundColor "Red"

                exit $lastexitcode
            }

            Set-Location $currentLocation
            Write-Host  ""
        }
    }
}
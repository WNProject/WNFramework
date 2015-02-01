@ECHO off

SetLocal EnableDelayedExpansion

SET originalDirectory=%cd%

CD /D %~dp0

powershell -ExecutionPolicy Unrestricted -File makellvm-windows.ps1 %*

set result=0

IF NOT !ERRORLEVEL! == 0 (
	set result=!ERRORLEVEL!
)

CD /D %originalDirectory%

EXIT /B !result!

EndLocal
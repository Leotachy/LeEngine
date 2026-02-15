
@echo off

setlocal

for /f "delims=" %%a in ('powershell -nologo -command "Get-Date -Format o"') do set "start=%%a"

cmake ^
 -G "Visual Studio 17 2022" ^
 -A Win32 ^
 -B ..\out ^
 -S .. ^
 %*
 

for /f "delims=" %%a in ('powershell -nologo -command "Get-Date -Format o"') do set "end=%%a"

for /f "delims=" %%a in ('powershell -nologo -command "$start=[datetime]::Parse('%start%'); $end=[datetime]::Parse('%end%'); ($end - $start).TotalSeconds"') do set duration=%%a

echo Configure completed in %duration% seconds

:: Open the solution
cmake --open ..\out

PAUSE

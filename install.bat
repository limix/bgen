@echo off
SETLOCAL
pushd .
cd /D %USERPROFILE%

:: Log file
SET ORIGIN=%cd%
call :joinpath "%ORIGIN%" "install.log"
SET LOG_FILE=%Result%

VERIFY OTHER 2>nul
SETLOCAL ENABLEEXTENSIONS
IF ERRORLEVEL 1 ECHO Unable to enable extensions

:: Configuration
set "PREFIX=%programfiles%\bgen"
set BRANCH=master
set VERSION_URL=https://raw.githubusercontent.com/limix/bgen/%BRANCH%/VERSION
call :winget "%VERSION_URL%" >>%LOG_FILE% 2>&1
set /p VERSION=<VERSION && del VERSION
set FILE=bgen-%VERSION%.zip
set DIR=bgen-%VERSION%
set URL=https://github.com/limix/bgen/archive/%VERSION%.zip

if exist "%PREFIX%" (
  type nul > "%PREFIX%\touch" >>%LOG_FILE% 2>&1
  if ERRORLEVEL 1 (echo You don't seem to have write permission for the "%PREFIX%" folder && popd && exit /B 1)
  del /q "%PREFIX%\touch" ! >nul 2>&1
) else (
  mkdir "%PREFIX%" >>%LOG_FILE% 2>&1
  if ERRORLEVEL 1 (echo You don't seem to have permission to create the "%PREFIX%" folder && popd && exit /B 1)
)

if DEFINED ARCH set ARCH=%ARCH:"=%
if NOT DEFINED ARCH (set ARCH=x64) else if [%ARCH%]==[x86] (set ARCH=)
set "CFLAGS=/MD /GL"

echo [0/4] Library(bgen==%VERSION%)

:: Cleaning up previous mess
del /Q %FILE% ! >nul 2>&1
rd /S /Q %DIR% >nul 2>&1
del /Q %LOG_FILE% ! >nul 2>&1
copy /y nul %LOG_FILE% >nul 2>&1

echo|set /p="[1/4] Downloading... "
echo Fetching %URL% >>%LOG_FILE% 2>&1
call :winget "%URL%" >>%LOG_FILE% 2>&1
if %ERRORLEVEL% NEQ 0 (echo FAILED. && type %LOG_FILE% && popd && exit /B 1) else (echo done.)

echo|set /p="[2/4] Extracting... "
powershell.exe -nologo -noprofile -command "& { Add-Type -A 'System.IO.Compression.FileSystem'; [IO.Compression.ZipFile]::ExtractToDirectory('%FILE%', '.'); }"
if %ERRORLEVEL% NEQ 0 (echo FAILED. && type %LOG_FILE% && popd && exit /B 1) else (echo done.)

cd %DIR% && mkdir build && cd build

echo|set /p="[3/4] Configuring... "
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_GENERATOR_PLATFORM=%ARCH% -DCMAKE_INSTALL_PREFIX="%PREFIX%" >>%LOG_FILE% 2>&1
if %ERRORLEVEL% NEQ 0 (echo FAILED. && type %LOG_FILE% && popd && exit /B 1) else (echo done.)

echo|set /p="[4/4] Compiling and installing... "
cmake --build . --config Release --target install >>%LOG_FILE% 2>&1
if %ERRORLEVEL% NEQ 0 (echo FAILED. && type %LOG_FILE% && popd && exit /B 1)
if NOT exist "%programfiles%\bgen\lib\bgen.lib" (echo FAILED. && type %LOG_FILE% && popd && exit /B 1)
if NOT exist "%programfiles%\bgen\bin\bgen.dll" (echo FAILED. && type %LOG_FILE% && popd && exit /B 1)
if NOT exist "%programfiles%\bgen\lib\bgen_static.lib" (echo FAILED. && type %LOG_FILE% && popd && exit /B 1)
if NOT exist "%programfiles%\bgen\include\bgen.h" (echo FAILED. && type %LOG_FILE% && popd && exit /B 1) else (echo done.)

cd %ORIGIN% >nul 2>&1
del /Q %FILE% >nul 2>&1
rd /S /Q %DIR% >nul 2>&1

echo Details can be found at %LOG_FILE%.

@echo on
@goto :eof

:joinpath
set Path1=%~1
set Path2=%~2
if {%Path1:~-1,1%}=={\} (set Result=%Path1%%Path2%) else (set Result=%Path1%\%Path2%)
goto :eof

:winget    - download file given url
set URL=%~1
for %%F in (%URL%) do set FILE=%%~nxF

setlocal EnableDelayedExpansion
set multiLine=$security_protcols = [Net.ServicePointManager]::SecurityProtocol -bor [Net.SecurityProtocolType]::SystemDefault^

if ([Net.SecurityProtocolType].GetMember('Tls11').Count -gt 0) {^

 $security_protcols = $security_protcols -bor [Net.SecurityProtocolType]::Tls11^

}^

if ([Net.SecurityProtocolType].GetMember('Tls12').Count -gt 0) {^

$security_protcols = $security_protcols -bor [Net.SecurityProtocolType]::Tls12^

}^

[Net.ServicePointManager]::SecurityProtocol = $security_protcols^

Write-Host 'Downloading %URL%... ' -NoNewLine^

(New-Object Net.WebClient).DownloadFile('%URL%', '%FILE%')^

Write-Host 'done.'

powershell -Command !multiLine!
goto:eof

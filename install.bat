@echo off

:: Configuration
set VERSION_URL=https://raw.githubusercontent.com/limix/bgen/1.0.x/VERSION
powershell -Command "(New-Object Net.WebClient).DownloadFile('%VERSION_URL%', 'VERSION.tmp')"
set /p VERSION=<VERSION.tmp && del VERSION.tmp
set FILE=bgen-%VERSION%.zip
set DIR=bgen-%VERSION%
set URL=https://github.com/limix/bgen/archive/%VERSION%.zip
IF "%ARCH%"=="" set ARCH=x64

echo [0/4] Library(bgen==%VERSION%)

SET ORIGIN=%cd%
call :joinpath "%ORIGIN%" "install.log"
SET LOG_FILE=%Result%

:: Cleaning up previous mess
del /Q %FILE% ! >nul 2>&1
rd /S /Q %DIR% >nul 2>&1
del /Q %LOG_FILE% ! >nul 2>&1
copy /y nul %LOG_FILE% >nul 2>&1

echo|set /p="[1/4] Downloading... "
echo Fetching %URL% >>%LOG_FILE% 2>&1
powershell -Command "(New-Object Net.WebClient).DownloadFile('%URL%', '%FILE%')" >>%LOG_FILE% 2>&1
if %ERRORLEVEL% NEQ 0 (echo FAILED. && type %LOG_FILE% && exit /B 1) else (echo done.)

echo|set /p="[2/4] Extracting... "
powershell.exe -nologo -noprofile -command "& { Add-Type -A 'System.IO.Compression.FileSystem'; [IO.Compression.ZipFile]::ExtractToDirectory('%FILE%', '.'); }"
if %ERRORLEVEL% NEQ 0 (echo FAILED. && type %LOG_FILE% && exit /B 1) else (echo done.)

cd %DIR% && mkdir build && cd build

echo|set /p="[3/4] Configuring... "
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_GENERATOR_PLATFORM=%ARCH% -DCMAKE_INSTALL_PREFIX="%programfiles%\bgen" >>%LOG_FILE% 2>&1
if %ERRORLEVEL% NEQ 0 (echo FAILED. && type %LOG_FILE% && exit /B 1) else (echo done.)

echo|set /p="[4/4] Compiling and installing... "
cmake --build . --config Release --target install >>%LOG_FILE% 2>&1
if %ERRORLEVEL% NEQ 0 (echo FAILED. && type %LOG_FILE% && exit /B 1) else (echo done.)

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

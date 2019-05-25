set filename="vcvars64path.txt"
IF NOT EXIST %filename% where /r c:\ vcvars64.bat >> %filename%
set /p mypath=<%filename%
call "%mypath%"

set ROOT_DIR=%CD%

set BIN_DIR="%ROOT_DIR%"

set INCLUDE_DIRS=/I"%ROOT_DIR%"

set FLAGS=/DDEVELOPMENT /Zi /DEBUG /MDd

set MAIN_FILE="%ROOT_DIR%\src\main.cpp"

mkdir build
cd build

cl %MAIN_FILE% %INCLUDE_DIRS% %FLAGS% /link /machine:x64

mkdir assets
xcopy "../assets" "assets" /e /s

pause
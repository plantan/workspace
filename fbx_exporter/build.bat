set filename="vcvars64path.txt"
IF NOT EXIST %filename% where /r c:\ vcvars64.bat >> %filename%
set /p mypath=<%filename%
call "%mypath%"

set ROOT_DIR=%CD%

set BIN_DIR="%ROOT_DIR%"

set INCLUDE_DIRS=/I"C:\\Program Files\\Autodesk\\FBX\\FBX SDK\\2019.2\\include"

set LIB_PATH=/LIBPATH:"C:\\Program Files\\Autodesk\\FBX\\FBX SDK\\2019.2\\lib\\vs2017\\x64\\debug"

set FLAGS=/DDEVELOPMENT /Zi /DEBUG /MDd

set MAIN_FILE="%ROOT_DIR%\\src\\main.cpp"

mkdir build
cd build

cl /LD /DEBUG %MAIN_FILE% %INCLUDE_DIRS% %FLAGS% /link %LIB_PATH% /OUT:FBXExporter.dll

C:\Windows\Microsoft.NET\Framework\v3.5\csc.exe /debug /t:exe /out:FBXFileFinder.exe ..\src\FBXFileFinder.cs

copy "C:\Program Files\Autodesk\FBX\FBX SDK\2019.2\lib\vs2017\x64\debug\libfbxsdk.dll" .

mkdir assets
copy "../assets" "assets"

pause
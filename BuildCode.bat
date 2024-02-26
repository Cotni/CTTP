@echo off
SETLOCAL EnableDelayedExpansion

cls
del build\*.o

SET "debug="
SET "cwDWARF="
if "%1" equ "-d" SET "debug=-debug -map=^"C:\Users\admin\Documents\Dolphin Emulator\Maps\RMCP01.map^" -readelf=^"C:\MinGW\bin\readelf.exe^""
if "%1" equ "-d" SET "cwDWARF=-g"

:: Destination (change as necessary)
SET "SOURCE=CTTP"
SET "RIIVO=C:\Users\admin\Documents\Dolphin Emulator\Load\Riivolution\CTTP"
SET "RELEASE=C:\Modding\Coding\Kamek\CTTP\releases"
SET "ENGINE=C:\Modding\Coding\Kamek\Common\KamekInclude"
echo %RIIVO%


:: CPP compilation settings
SET CC="../Common/cw/mwcceppc.exe"
SET CFLAGS=-I- -i "../Common/KamekInclude" -i "../Common/GameSource" -i "../Common/GameSource/MarioKartWii" -i code -i "../Pulsar" -i "../Pulsar/PulsarEngine"^
 -opt all -inline auto -enum int -proc gekko -fp hard -sdata 0 -sdata2 0 -maxerrors 1 -func_align 4
SET DEFINE=

:: CPP Sources
SET CPPFILES=
for /R code %%f in (*.cpp) do SET "CPPFILES=%%f !CPPFILES!"
for /R C:\Modding\Coding\Kamek\Pulsar\PulsarEngine %%f in (*.cpp) do SET "CPPFILES=%%f !CPPFILES!"

:: Compile CPP
%CC% %CFLAGS% -c -o "build/kamek.o" "%ENGINE%\kamek.cpp"

SET OBJECTS=
FOR %%H IN (%CPPFILES%) DO (
    ::echo "Compiling %%H..."
    %CC% %CFLAGS% %DEFINE% -c -o "build/%%~nH.o" "%%H"
    SET "OBJECTS=build/%%~nH.o !OBJECTS!"
)

:: Link
echo Linking... %time%
"../Common/Kamek" "build/kamek.o" %OBJECTS% %debug% -dynamic -externals="../Common/GameSource/symbols.txt" -versions="../Common/GameSource/versions.txt" -output-combined=build\Code.pul

if %ErrorLevel% equ 0 (
    xcopy /Y build\*.pul "%RELEASE%\Binaries\" >nul
    cd "C:\Modding\Coding\Kamek\%SOURCE%\resources\" >nul
    xcopy /Y /S "%RELEASE%\Binaries" "%RIIVO%\Binaries\" >nul
    echo Binaries copied
    echo Change SetUserInfo
)

:end
ENDLOCAL
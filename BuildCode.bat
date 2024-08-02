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
SET "RIIVO=C:\Users\Cotni\AppData\Roaming\Dolphin Emulator\Load\CTTP"
SET "RELEASE=C:\Users\Cotni\Modding Tools\Pulsar\CTTP"
SET "ENGINE=C:\Code\mkw\CTTP\Pulsar\KamekInclude"
echo %RIIVO%


:: CPP compilation settings
SET CC="C:\Code\the good stuff\mwcceppc.exe"
SET CFLAGS=-I- -i "../Pulsar/KamekInclude" -i "../Pulsar/GameSource" -i "../Pulsar/GameSource/MarioKartWii" -i code -i "../Pulsar" -i "../Pulsar/PulsarEngine"^
 -opt all -inline auto -enum int -proc gekko -fp hard -sdata 0 -sdata2 0 -maxerrors 1 -func_align 4
SET DEFINE=

:: CPP Sources
SET CPPFILES=
for /R code %%f in (*.cpp) do SET "CPPFILES=%%f !CPPFILES!"
for /R C:\Code\mkw\CTTP\Pulsar\PulsarEngine %%f in (*.cpp) do SET "CPPFILES=%%f !CPPFILES!"

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
"C:\Code\mkw\CTTP\Pulsar\KamekLinker\bin\Release\net8.0\Kamek.exe" "build/kamek.o" %OBJECTS% %debug% -dynamic -externals="../Pulsar/GameSource/symbols.txt" -versions="../Pulsar/GameSource/versions.txt" -output-combined=build\Code.pul

if %ErrorLevel% equ 0 (
    xcopy /Y build\*.pul "%RELEASE%\Binaries\" >nul
    cd "C:\Code\mkw\CTTP\%SOURCE%\resources\" >nul
    xcopy /Y /S "%RELEASE%\Binaries" "%RIIVO%\Binaries\" >nul
    echo Binaries copied
    echo Change SetUserInfo
)

:end
ENDLOCAL
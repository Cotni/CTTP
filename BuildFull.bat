@echo off
SETLOCAL EnableDelayedExpansion

cls

:: Destination (change as necessary)
SET "SOURCE=CTTP"
SET "RIIVO=D:\Documents\Dolphin Emulator\Load\CTTP"
SET "RELEASE=D:\Modding\Coding\Kamek\CTTP\releases"
echo %RIIVO%

:: CPP compilation settings
SET CC="../engine/cw/mwcceppc.exe"
SET CFLAGS=-I- -i "../engine/engine" -i "../engine/source" -i "../engine/source/game" -i code -gcc_extensions on -Cpp_exceptions off -enum int -O4,s -use_lmw_stmw on -fp hard -rostr -sdata 0 -sdata2 0 -maxerrors 1 -func_align 4 -rtti off
SET DEFINE=

:: CPP Sources
SET CPPFILES=
for /R code %%f in (*.cpp) do SET "CPPFILES=%%f !CPPFILES!"

:: Compile CPP
%CC% %CFLAGS% -c -o "build/kamek.o" "D:\Modding\Coding\Kamek\Engine\engine\kamek.cpp"

SET OBJECTS=
FOR %%H IN (%CPPFILES%) DO (
    ::echo "Compiling %%H..."
    %CC% %CFLAGS% %DEFINE% -c -o "build/%%~nH.o" "%%H"
    SET "OBJECTS=build/%%~nH.o !OBJECTS!"
)

:: Link
echo Linking...
"../engine/KamekSource/bin/Debug/Kamek" "build/kamek.o" %OBJECTS% -dynamic -externals="../engine/source/symbols.txt" -versions="../engine/source/versions.txt" -output-kamek=build\$KV$.bin
echo Done...
if %ErrorLevel% equ 0 (
    xcopy /Y build\*.bin "%RELEASE%\Binaries"
	del build\*.o
    echo Done...
    cd "D:\Modding\Coding\Kamek\CTTP\resources"
    xcopy /Y /S "%RELEASE%\Binaries" "%RIIVO%\Binaries" >NUL
    echo Binaries copied
    
    

    wbmgt encode .\Menu.txt -o
    xcopy /Y menu.bmg "CTTPUI.d\message"
    wbmgt encode .\Race.txt -o
    xcopy /Y race.bmg "CTTPUI.d\message"
    echo BMG created

    wszst create *.d -o
    xcopy /Y *.szs "%RELEASE%\Assets"
    xcopy /Y "%RELEASE%\Assets" "%RIIVO%\Assets"
    echo Misc SZS Copied

    SET /A "j=0"
    FOR %%L in ("SZS/*.szs") DO (
        xcopy /Y /F /Q "SZS\%%L" "!RELEASE!\Tracks\!j!.szs*"
        SET /A j+=1  
    )
    xcopy /Y "%RELEASE%\Tracks" "%RIIVO%\Tracks"
    echo Tracks SZS Copied
)

:end
ENDLOCAL
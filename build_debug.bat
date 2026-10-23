@echo off
setlocal

set QT_DIR=E:\Qt\6.11.2\mingw_64
set MINGW_DIR=E:\Qt\Tools\mingw1310_64

set CONFIG=debug
set ROOT=%~dp0
set OBJ_DIR=%ROOT%build\obj\%CONFIG%
set OUT_DIR=%ROOT%build\%CONFIG%

set PATH=%QT_DIR%\bin;%MINGW_DIR%\bin;%PATH%

if exist "%OUT_DIR%" rmdir /s /q "%OUT_DIR%" || goto :locked
if exist "%OUT_DIR%" goto :locked
mkdir "%OUT_DIR%"
if not exist "%OBJ_DIR%" mkdir "%OBJ_DIR%"
pushd "%OBJ_DIR%"

echo === qmake (%CONFIG%) ===
qmake "%ROOT%MulticlientCreator.pro" -spec win32-g++ "CONFIG-=debug_and_release" "CONFIG+=%CONFIG%" "DESTDIR=%OUT_DIR%" || goto :error

echo === build ===
mingw32-make -j%NUMBER_OF_PROCESSORS% || goto :error

echo === deploy Qt dlls ===
windeployqt --release --no-translations --no-system-d3d-compiler --no-opengl-sw --no-ffmpeg ^
    --skip-plugin-types generic,iconengines,networkinformation,tls,styles ^
    --exclude-plugins qgif,qjpeg,qsvg,qdirect2d,qminimal,qoffscreen ^
    "%OUT_DIR%\NostaleClientCreator.exe" || goto :error

popd
echo.
echo Build OK: %OUT_DIR%\NostaleClientCreator.exe
pause
exit /b 0

:locked
echo.
echo Cannot clean %OUT_DIR% (NostaleClientCreator.exe still running?)
pause
exit /b 1

:error
popd
echo.
echo BUILD FAILED
pause
exit /b 1

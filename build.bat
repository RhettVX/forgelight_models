@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64

setlocal EnableDelayedExpansion
setlocal EnableExtensions

set LIBRARY_DIR=..\lib\
set INCLUDE_FLAGS=/I..\include\ /I..\src\

set OUTPUT_NAME=forgelight_models.exe
set SOURCES=..\src\*.c
set FL_DEFINES=/DFL_WIN32
set COMPILATION_FLAGS=/Od /Zi /W3
set LINK_FLAGS=/link /DEBUG:FULL !LIBRARY_DIR!raylib.lib !LIBRARY_DIR!forgelight_lib.lib kernel32.lib user32.lib shell32.lib winmm.lib gdi32.lib opengl32.lib

mkdir build
pushd build

copy ..\..\raylib-4.0.0\src\raylib.h ..\include\
copy ..\..\raylib-4.0.0\src\raymath.h ..\include\
copy ..\..\raylib-4.0.0\src\rlgl.h ..\include\
copy ..\..\raylib-4.0.0\src\external\glad.h ..\include\
copy ..\..\raylib-4.0.0\examples\shaders\rlights.h ..\include\
copy ..\..\raylib-4.0.0\build\raylib.lib ..\lib\
rem copy ..\..\raylib-3.7.0\build\raylib.pdb ..\lib\

copy ..\..\forgelight_lib\src\forgelight_lib.h ..\include\
copy ..\..\forgelight_lib\build\forgelight_lib.lib ..\lib\
rem copy ..\..\forgelight_lib\build\forgelight_lib.pdb ..\lib\

cl /nologo /c !FL_DEFINES! !INCLUDE_FLAGS! !COMPILATION_FLAGS! !SOURCES!
cl /nologo /Fe:!OUTPUT_NAME! *.obj !LINK_FLAGS!

popd

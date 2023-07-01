@echo off

IF NOT EXIST build mkdir build

set CommonCompilerFlags=-std=c++17 -m64 -g -Werror -fno-exceptions -fwritable-strings -O0 -Wno-unknown-warning-option -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Dcatalyst_internal=1 -Dcatalyst_slow=1 -Dcatalyst_Win32=1
set LinkerFlags=-I../src -I../libs/sdl/include -I../libs/glew/include -L ../libs/sdl/lib -L ../libs/glew/lib -lglew32 -lopengl32.lib -lshell32.lib -lWinmm.lib -lSDL2main -lSDL2
set SUBFILENAME=%date:~-4,4%%date:~-10,2%%date:~-7,2%_%time:~0,2%%time:~3,2%%time:~6,2%
set SUBFILENAME=%SUBFILENAME: =0%

xcopy /E /I /D /Y "./libs/sdl/lib" "./build"
xcopy /E /I /D /Y "./libs/glew/lib" "./build"

pushd build

REM 64-bit build
del *.pdb > NUL 2> NUL
REM Optimization switches /O2
echo WAITING FOR PDB > lock.tmp
clang++ %CommonCompilerFlags% %LinkerFlags% ../src/win32/win32_main.cpp -o ./main.exe -Wl,/PDB:main_%SUBFILENAME%.pdb -Xlinker -subsystem:windows
del lock.tmp
clang++ %CommonCompilerFlags% -shared -o game.dll ../src/game/game.cpp -I../src -Wl,/PDB:game_%SUBFILENAME%.pdb
popd
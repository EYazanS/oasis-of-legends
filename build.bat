@echo off

IF NOT EXIST build mkdir build

set CommonCompilerFlags=/std:c++17 -MTd -nologo -fp:fast -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -DLeena_Internal=1 -DLeena_Slow=1 -DLeena_Win32=1 -FC -Z7


set SUBFILENAME=%date:~-4,4%%date:~-10,2%%date:~-7,2%_%time:~0,2%%time:~3,2%%time:~6,2%

pushd build

REM 64-bit build
del *.pdb > NUL 2> NUL
REM Optimization switches /O2
echo WAITING FOR PDB > lock.tmp
clang -std=c++17 -m64 -g ../src/win32_main.cpp -o ./main.exe -I../libs/sdl/include -L ../libs/sdl/libs -lshell32.lib -lSDL2main -lSDL2 -fwritable-strings -Werror -fno-exceptions -Wl,/PDB:game_%SUBFILENAME%.pdb -Xlinker -subsystem:windows
del lock.tmp
clang -shared -m64 -o game.dll ../src/game/game.cpp -I../src 
popd
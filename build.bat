@echo off

IF NOT EXIST build mkdir build

clang++ -std=c++17 -m64 -g ./src/main.cpp -o ./build/main.exe -I./libs/sdl/include -L ./libs/sdl/libs -lSDL2main -lSDL2 -fwritable-strings -Werror -fno-exceptions
@echo off

IF NOT EXIST build mkdir build

clang++ -std=c++17 -g ./src/main.cpp -o ./build/main.exe
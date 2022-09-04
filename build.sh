#!/bin/bash

echo off
clear

set -xe

INCLUDE_GLFW="C:\Users\bj\Downloads\glfw-3.3.8\include"
INCLUDE_GLFW_DEPS="C:\Users\bj\Downloads\glfw-3.3.8\deps"

LIB_GLFW="C:\Users\bj\Downloads\glfw-3.3.8.bin.WIN64\lib-mingw-w64"

# g++ -Wall -Wextra -o drawing drawing.c
gcc -Wall -Wextra -I$INCLUDE_GLFW -I$INCLUDE_GLFW_DEPS -o opengl_test opengl_test.c -L$LIB_GLFW -lglfw3 -lgdi32

./opengl_test.exe
cmake_minimum_required(VERSION 3.16)
project(c0_sjx)

set(CMAKE_CXX_STANDARD 14)

add_executable(c0_sjx main.cpp headers/compiler.h src/compiler.cpp headers/generator.h headers/global.h headers/lexical.h headers/log.h headers/optimizer.h headers/syntax.h src/generator.cpp src/global.cpp src/lexical.cpp src/log.cpp src/optimizer.cpp src/syntax.cpp)
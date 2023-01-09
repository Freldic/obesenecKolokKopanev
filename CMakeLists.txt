cmake_minimum_required(VERSION 3.18)
project(obesenecKolokKopanev)

set(CMAKE_CXX_STANDARD 14)

add_executable(obesenecKolokKopanev main.cpp server.cpp server.h)

target_link_libraries(obesenecKolokKopanev pthread m)

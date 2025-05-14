cmake_minimum_required(VERSION 3.29)
project(ProjetTypeS2 C)

set(CMAKE_C_STANDARD 11)

add_executable(ProjetTypeS2 main.c
        decor.c)
target_link_libraries(ProjetTypeS2 -lmingw32 -lalleg44)
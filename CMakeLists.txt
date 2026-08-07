cmake_minimum_required(VERSION 3.20)

project(tempo)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS off)

add_executable(tempo
    source/main.cpp
    source/bitboards/attacks.cpp
    source/board/position.cpp
    source/board/zobrist.cpp
    source/move/movegen.cpp
    source/move/movelist.cpp
    source/search/search.cpp
    source/tt/tt.cpp
    source/uci/uci.cpp
)

target_include_directories(tempo PRIVATE source)
target_compile_options(tempo PRIVATE -O3 -march=native -mbmi2 -flto -DNDEBUG)
target_link_options(tempo PRIVATE -flto)

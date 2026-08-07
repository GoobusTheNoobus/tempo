# Tempo 
## Description
Tempo is a UCI-supporting hobby chess engine. It is written in C++ and developed by @GoobusTheNoobus. It uses bitboards and a mailbox for position representation, bitboards for very fast move generation, and an alpha-beta principle variation search negamax search framework. It is pretty bunz at the game though.

Elo Estimate: 2166 (+/- 31)
Version: 2.2.2

## Building
Tempo uses CMake and Ninja to compile. Navigate to root, then run `cmake -B build -G Ninja` to configure. Run `cmake --build build/` to build.
# Tempo 
## Description
Tempo is a UCI-supporting hobby chess engine. It is written in C++ and developed by @GoobusTheNoobus. It uses bitboards and a mailbox for position representation, bitboards for very fast move generation, and an alpha-beta principle variation search negamax search framework. It is pretty bunz at the game though.

Elo Estimate: 2166 (+/- 31)
Version: 2.1.0

## Building
Tempo uses makefiles to compile its code. Navigate to project root, and use `make` to build into an executable `tempo.exe` located in the same directory
#include <iostream>

#include "bitboards/attacks.hpp"
#include "board/zobrist.hpp"
#include "eval/eval.hpp"
#include "search/search.hpp"
#include "uci/uci.hpp"


using namespace Tempo;

void initialize() {
    Attacks::init();
    Evaluation::init();
    Zobrist::init();
    Search::init();
    TranspositionTable::init();
}

int main(void) {
    initialize();

    UCI::loop();

    return 0;
}
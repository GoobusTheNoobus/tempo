#include <iostream>

#include "eval/eval.hpp"
#include "chess/board/zobrist.hpp"
#include "chess/board/attacks.hpp"
#include "uci/uci.hpp"
#include "search/search.hpp"

using namespace Tempo;

void initialize() {
    Attacks::init();
    Evaluation::init();
    Zobrist::init();
    Search::init();
}

int main(void) {
    initialize();

    UCI::loop();

    return 0;
}
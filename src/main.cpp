#include <iostream>

#include "eval.hpp"
#include "zobrist.hpp"
#include "attacks.hpp"
#include "uci.hpp"
#include "search.hpp"

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
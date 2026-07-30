# TODO: snake_case → camelCase Refactor

## Phase 1: Foundation files (types, bitboards, eval, score, move)
- [ ] source/chess/types.hpp
- [ ] source/chess/board/bitboards.hpp
- [ ] source/eval/eval.hpp
- [ ] source/eval/score.hpp
- [ ] source/chess/move/move.hpp (check if any changes needed)

## Phase 2: Position (depends on types, eval)
- [ ] source/chess/board/position.hpp
- [ ] source/chess/board/position.cpp

## Phase 3: Attacks (depends on position)
- [ ] source/chess/board/attacks.hpp
- [ ] source/chess/board/attacks.cpp

## Phase 4: Move generation (depends on position, attacks)
- [ ] source/chess/move/movegen.hpp
- [ ] source/chess/move/movegen.cpp
- [ ] source/chess/move/movelist.hpp
- [ ] source/chess/move/movelist.cpp

## Phase 5: Search (depends on position, move)
- [ ] source/search/search.hpp
- [ ] source/search/search.cpp
- [ ] source/search/timer.hpp
- [ ] source/search/history.hpp
- [ ] source/search/killer.hpp
- [ ] source/search/tt/tt.hpp
- [ ] source/search/tt/tt.cpp

## Phase 6: UCI (depends on search, position)
- [ ] source/uci/uci.hpp
- [ ] source/uci/uci.cpp

## Phase 7: Main
- [ ] source/main.cpp

## Phase 8: Build & Test
- [ ] Run `make` and fix any compilation errors

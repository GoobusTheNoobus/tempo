# Refactoring Plan: snake_case → camelCase

## Scope
Convert all non-const (mutable) variables and functions from snake_case to camelCase throughout the codebase. Constants (const, constexpr, enum values, type aliases) remain unchanged.

## Files to Modify

### 1. source/chess/types.hpp
- Functions: `make_piece` → `makePiece`, `type_of` → `typeOf`, `color_of` → `colorOf`, `file_of` → `fileOf`, `rank_of` → `rankOf`, `square_to_string` → `squareToString`
- Parameters/args only (no changes to enum names or type aliases)

### 2. source/chess/board/bitboards.hpp
- Functions: `poplsb` → `popLsb`
- Variables: No non-const snake_case (all constexpr)

### 3. source/chess/board/attacks.hpp
- Functions: `knight_attacks` → `knightAttacks`, `king_attacks` → `kingAttacks`, `pawn_attacks` → `pawnAttacks`, `bishop_attack` → `bishopAttack`, `rook_attack` → `rookAttack`, `queen_attack` → `queenAttack`, `is_attacked` → `isAttacked`

### 4. source/chess/board/attacks.cpp
- Functions (anon ns): `out_of_bounds` → `outOfBounds`, `generate_blocker_from_index` → `generateBlockerFromIndex`, `raycast_bishop` → `raycastBishop`, `raycast_rook` → `raycastRook`, `hash_bishop` → `hashBishop`, `hash_rook` → `hashRook`
- Locals: `new_r` → `newR`, `new_f` → `newF`, `new_squ` → `newSqu`

### 5. source/chess/board/position.hpp
- Struct GameState: `en_passant_square` → `enPassantSquare`, `castling_rights` → `castlingRights`, `rule50_clock` → `rule50Clock`
- Struct MoveUndoInfo: `castling_rights` → `castlingRights`, `rule50_clock` → `rule50Clock`, `en_passant_square` → `enPassantSquare`, `captured_piece` → `capturedPiece`
- Class Position methods: `set_up_startpos` → `setUpStartpos`, `parse_fen` → `parseFen`, `to_string` → `toString`, `get_piece_on` → `getPieceOn`, `get_bitboard` → `getBitboard`, `get_side_to_move` → `getSideToMove`, `get_en_passant` → `getEnPassant`, `has_castling_right` → `hasCastlingRight`, `is_rule_50` → `isRule50`, `get_key` → `getKey`, `is_in_check` → `isInCheck`, `make_move` → `makeMove`, `attempt_move` → `attemptMove`, `undo_move` → `undoMove`, `has_non_pawn_material` → `hasNonPawnMaterial`, `is_repetition` → `isRepetition`, `clear_square` → `clearSquare`, `place_piece` → `placePiece`, `push_move_stacks` → `pushMoveStacks`, `pop_undo_info` → `popUndoInfo`
- Members: `side_to_move` → `sideToMove`, `piece_bitboards` → `pieceBitboards`, `color_bitboards` → `colorBitboards`, `move_undo_stack` → `moveUndoStack`, `psqt_scores` → `psqtScores`

### 6. source/chess/board/position.cpp
- All access to renamed members/functions updated
- Locals: `fen_board_part` → `fenBoardPart`, `fen_side_part` → `fenSidePart`, `fen_castling_part` → `fenCastlingPart`, `fen_ep_part` → `fenEpPart`, `fen_rule50_part` → `fenRule50Part`, `moving_piece` → `movingPiece`, `moving_pt` → `movingPt`, `captured_piece` → `capturedPiece`, `hash_before` → `hashBefore`, `capture_square` → `captureSquare`, `king_side` → `kingSide`, `rook_from` → `rookFrom`, `rook_dest` → `rookDest`, `move_str` → `moveStr`, `piece_already_there` → `pieceAlreadyThere`

### 7. source/chess/board/zobrist.hpp
- No changes (all inline variables are PascalCase)

### 8. source/chess/move/move.hpp
- No changes (all functions/values already camelCase or PascalCase)

### 9. source/chess/move/movegen.hpp
- Function: `generate_pseudo_legal_moves` → `generatePseudoLegalMoves`

### 10. source/chess/move/movegen.cpp
- Functions: `extract_pawn` → `extractPawn`, `extract_pawn_promo` → `extractPawnPromo`
- Locals: `single_push_bb` → `singlePushBb`, `double_push_bb` → `doublePushBb`, `left_capture_bb` → `leftCaptureBb`, `right_capture_bb` → `rightCaptureBb`, `single_push_promo_bb` → `singlePushPromoBb`, `single_push_normal_bb` → `singlePushNormalBb`, `left_promo_bb` → `leftPromoBb`, `left_normal_bb` → `leftNormalBb`, `right_promo_bb` → `rightPromoBb`, `right_normal_bb` → `rightNormalBb`, `single_push_offset` → `singlePushOffset`, `double_push_offset` → `doublePushOffset`, `left_capture_offset` → `leftCaptureOffset`, `right_capture_offset` → `rightCaptureOffset`, `rank3_from_bottom` → `rank3FromBottom`, `rank8_from_bottom` → `rank8FromBottom`, `ep_pawns` → `epPawns`

### 11. source/chess/move/movelist.hpp
- Method: `calculate_scores` → `calculateScores`
- Locals in inline `next`: `highest_score_index` → `highestScoreIndex`, `highest_score` → `highestScore`

### 12. source/chess/move/movelist.cpp
- Function: `score_move` → `scoreMove` (both overloads)
- Usage: `calculate_scores` → `calculateScores`

### 13. source/eval/eval.hpp
- Struct TaperedScore: `mg_score` → `mgScore`, `eg_score` → `egScore`, `get_score` → `getScore`
- Function: `calculate_phase` → `calculatePhase`

### 14. source/eval/score.hpp
- Function: `score_string` → `scoreString`
- Local: `mate_dist` → `mateDist`

### 15. source/search/search.hpp
- Struct SearchInfo: `nodes_searched` → `nodesSearched`, `pv_table` → `pvTable`, `pv_lengths` → `pvLengths`
- Parameters: `plies_from_root` → `pliesFromRoot`, `allow_nmp` → `allowNmp`
- Function: `qsearch` → `qSearch`

### 16. source/search/search.cpp
- Variable: `reduction_table` → `reductionTable`
- Functions: `update_pv` → `updatePv`, `is_noisy` → `isNoisy`
- Locals: `child_length` → `childLength`, `best_move` → `bestMove`, `previous_score` → `previousScore`, `best_entry` → `bestEntry`, `highest_depth` → `highestDepth`, `tt_move` → `ttMove`, `static_eval` → `staticEval`, `best_score` → `bestScore`, `move_count` → `moveCount`, `original_alpha` → `originalAlpha`, `search_move` → `searchMove`, `is_legal` → `isLegal`, `in_check` → `inCheck`, `store_flag` → `storeFlag`, `null_score` → `nullScore`

### 17. source/search/history.hpp
- Parameters: already camelCase

### 18. source/search/killer.hpp
- Parameter: `plies_from_root` → `pliesFromRoot`

### 19. source/search/timer.hpp
- Members: `start_time` → `startTime`, `max_time_ms` → `maxTimeMs`, `stop_flag` → `stopFlag`
- Methods: `request_stop` → `requestStop`, `should_stop_search` → `shouldStopSearch`

### 20. source/search/tt/tt.hpp
- Struct Entry: `best_move` → `bestMove`

### 21. source/search/tt/tt.cpp
- Function: `get_index` → `getIndex`
- Local: `replace` (already fine)

### 22. source/uci/uci.hpp
- Functions: `info_depth` → `infoDepth`, `info_string` → `infoString`

### 23. source/uci/uci.cpp
- Functions: `handle_uci` → `handleUci`, `handle_go` → `handleGo`, `handle_position` → `handlePosition`, `handle_isready` → `handleIsready`
- Variable: `search_thread` → `searchThread`
- Locals: `time_limit` → `timeLimit`, `our_time` → `ourTime`, `our_inc` → `ourInc`, `log_currmove` → `logCurrMove`, `currmovenumber` → `currMoveNumber`

### 24. source/main.cpp
- Function: `initialize` (already camelCase)

## Dependent Edges (order matters)
- types.hpp changes affect all files that use those functions
- position.hpp changes affect all files that use Position class
- eval.hpp changes affect position.cpp, movelist.cpp, search.cpp
- search.hpp changes affect search.cpp, uci.cpp
- timer.hpp changes affect search.cpp

## Follow-up
- After all edits, run `make` to compile and fix any errors


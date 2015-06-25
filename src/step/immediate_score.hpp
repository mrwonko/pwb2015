#pragma once

#include "../field.hpp"
#include "../result_manager.hpp"

// returns possible moves in the order of the score they net; that's a fairly bad heuristic but super fast
void highestImmediateScore( const Field& field, PossibleMoves& out_result );
void lowestImmediateScore( const Field& field, PossibleMoves& out_result );

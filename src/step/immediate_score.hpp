#pragma once

#include "../field.hpp"
#include "../result_manager.hpp"

void highestImmediateScore( const Field& field, PossibleMoves& out_result );
void lowestImmediateScore( const Field& field, PossibleMoves& out_result );

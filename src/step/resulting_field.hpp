#pragma once

#include "../field.hpp"
#include "../result_manager.hpp"

// this will often quickly run in dead ends, but the hope is to get few, large matches
// although creatingLargestMatch is probably better at that.
void creatingFewestMatches( const Field& field, PossibleMoves& out_result );
// like creatingFewestMatches, but penalizes last moves
void creatingFewestMatchesAvoidingTermination( const Field& field, PossibleMoves& out_result );

void creatingLargestMatch( const Field& field, PossibleMoves& out_result );

#pragma once

#include "../field.hpp"
#include "../result_manager.hpp"

// this will often quickly run in dead ends, but the hope is to get few, large matches, although creatingLargestMatch is probably better at that.
void creatingFewestMatches( const Field& field, PossibleMoves& out_result );
// like creatingFewestMatches, but penalizes last moves
void creatingFewestMatchesAvoidingTermination( const Field& field, PossibleMoves& out_result );

// sorts the possible move by which results in a field with the largest match; that's a decent heuristic, but takes a while to compute, since for each possible move we both need to calculate the resulting field and its matches
void creatingLargestMatch( const Field& field, PossibleMoves& out_result );

// sort based on resulting fields' tiles' distances to the mean location (the centroid) of tiles of their color; somewhat expensive, but useful.
void creatingSmallestMeanDistanceToCentroid( const Field& field, PossibleMoves& out_result );
void creatingSmallestMaximumDistanceToCentroid( const Field& field, PossibleMoves& out_result );

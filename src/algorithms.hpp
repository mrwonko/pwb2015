#pragma once

#include "field.hpp"
#include "result_manager.hpp"

#include <algorithm>
#include <utility>
#include <iostream>

typedef void( *MoveCalculation )( const Field&, PossibleMoves& );

// Of the sorted possible moves, take the best one
void singleBest( const Field& field, ResultManager& resultManager, MoveCalculation calculatePossibleMoves, const char* name );

// of the sorted possible moves, try the n best ones
void nBest( const Field& field, ResultManager& resultManager, MoveCalculation calculatePossibleMoves, const unsigned int n, const char* name );

struct MoveCalculationInfo
{
    MoveCalculation moveCalculation;
    const char* name;
};

void eachSingleBest( const Field& field, ResultManager& resultManager, const std::vector< MoveCalculationInfo >& moveCalculations );

void priorityExpandHeuristically( const Field& field, ResultManager& resultManager, const unsigned int sizeLimit );

// I'm not sure this ever performs better than the heuristical version
void priorityExpandAll( const Field& field, ResultManager& resultManager, const unsigned int sizeLimit );

void randomly( const Field& field, ResultManager& resultManager, unsigned int seed );

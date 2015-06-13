#pragma once

#include "field.hpp"
#include "result_manager.hpp"

template< bool (*step)( const Field&, Coordinate& ) >
void single_best( const Field& field, ResultManager& resultManager, const char* name )
{
  Field currentField( field );
  Score currentScore = 0;
  Moves moves;
  Coordinate move;
  while( step( currentField, move ) )
  {
    currentScore += currentField.remove( move );
    moves.push_back( move );
  }
  currentScore -= currentField.calculatePenalty();
  resultManager.report( { currentScore, moves }, name );
}

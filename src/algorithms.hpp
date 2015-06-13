#pragma once

#include "field.hpp"
#include "result_manager.hpp"

#include <algorithm>

// Of the sorted possible moves, take the best one
template< void ( *calculatePossibleMoves )( const Field&, PossibleMoves& ) >
void single_best( const Field& field, ResultManager& resultManager, const char* name )
{
  thread_local PossibleMoves possibleMoves;
  Field currentField( field );
  Score currentScore = 0;
  Moves moves;

  calculatePossibleMoves( currentField, possibleMoves );
  while( !possibleMoves.empty() )
  {
    const Coordinate& move = possibleMoves[ 0 ].coordinate;
    currentScore += currentField.remove( move );
    moves.push_back( move );
    calculatePossibleMoves( currentField, possibleMoves );
  }

  currentScore -= currentField.calculatePenalty();
  resultManager.report( { currentScore, moves }, name );
}

namespace detail
{
  template< void( *calculatePossibleMoves )( const Field&, PossibleMoves& ), unsigned int n >
  void n_best( const Field& currentField, ResultManager& resultManager, Score currentScore, Moves& moves, const char* name )
  {
    PossibleMoves possibleMoves;
    calculatePossibleMoves( currentField, possibleMoves );
    if( possibleMoves.empty() )
    {
      currentScore -= Field( currentField ).calculatePenalty();
      resultManager.report( { currentScore, moves }, name );
    }
    else
    {
      // try the best n moves
      possibleMoves.resize( std::min( possibleMoves.size(), n ) );
      for( const PossibleMove& move : possibleMoves )
      {
        Field nextField( currentField );
        Score nextScore = currentScore + nextField.remove( move.coordinate );
        moves.push_back( move.coordinate );
        n_best< calculatePossibleMoves, n >( std::move( nextField ), resultManager, nextScore, moves, name );
        moves.pop_back();
      }
    }
  }
}

// of the sorted possible moves, try the n best ones
template< void( *calculatePossibleMoves )( const Field&, PossibleMoves& ), unsigned int n >
void n_best( const Field& field, ResultManager& resultManager, const char* name )
{
  Moves moves;
  detail::n_best< calculatePossibleMoves, n >( field, resultManager, 0, moves, name );
}
// TODO WIP

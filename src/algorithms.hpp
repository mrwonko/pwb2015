#pragma once

#include "field.hpp"
#include "result_manager.hpp"

#include <algorithm>
#include <utility>
#include <iostream>

typedef void( *MoveCalculation )( const Field&, PossibleMoves& );

namespace detail
{
  template< MoveCalculation calculatePossibleMoves >
  inline void singleBest( const Field& field, ResultManager& resultManager, Score currentScore, Moves& moves,  const char* name )
  {
    thread_local PossibleMoves possibleMoves;
    thread_local Field currentField( { { 0 } }, 1 );
    currentField = field;

    calculatePossibleMoves( currentField, possibleMoves );
    while( !possibleMoves.empty() )
    {
      const Coordinate& move = possibleMoves[ 0 ].coordinate;
      currentScore += currentField.remove( move );
      moves.push_back( move );
      calculatePossibleMoves( currentField, possibleMoves );
    }

    currentScore -= currentField.calculatePenalty();
    resultManager.report( currentScore, moves, name );
  }

  template< MoveCalculation calculatePossibleMoves, unsigned int n >
  void nBest( const Field& currentField, ResultManager& resultManager, Score currentScore, Moves& moves, const char* name )
  {
    PossibleMoves possibleMoves;
    calculatePossibleMoves( currentField, possibleMoves );
    if( possibleMoves.empty() )
    {
      currentScore -= Field( currentField ).calculatePenalty();
      resultManager.report( currentScore, moves, name );
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
        nBest< calculatePossibleMoves, n >( std::move( nextField ), resultManager, nextScore, moves, name );
        moves.pop_back();
      }
    }
  }
}

// Of the sorted possible moves, take the best one
template< MoveCalculation calculatePossibleMoves >
void singleBest( const Field& field, ResultManager& resultManager, const char* name )
{
  thread_local Moves moves;
  moves.clear();
  detail::singleBest< calculatePossibleMoves >( field, resultManager, 0, moves, name );
}

// of the sorted possible moves, try the n best ones
template< MoveCalculation calculatePossibleMoves, unsigned int n >
void nBest( const Field& field, ResultManager& resultManager, const char* name )
{
  Moves moves;
  detail::nBest< calculatePossibleMoves, n >( field, resultManager, 0, moves, name );
  std::cerr << name << " finished" << std::endl;
}


template< MoveCalculation... >
struct each;

template<>
struct each<>
{
  static void _singleBest( const Field& field, ResultManager& resultManager, const Score currentScore, const Moves& moves, const char* name )
  {
  }
};

template< MoveCalculation firstMoveCalculation, MoveCalculation... remainingMoveCalculations >
struct each< firstMoveCalculation, remainingMoveCalculations... >
{
  static void _singleBest( const Field& field, ResultManager& resultManager, const Score currentScore, const Moves& moves, const char* name )
  {
    thread_local Moves currentMoves;
    currentMoves = moves;
    detail::singleBest< firstMoveCalculation >( field, resultManager, currentScore, currentMoves, name );
    each< remainingMoveCalculations... >::_singleBest( field, resultManager, currentScore, moves, name );
  }

  static void singleBest( const Field& field, ResultManager& resultManager, const char* name )
  {
    each< firstMoveCalculation, remainingMoveCalculations... >::_singleBest( field, resultManager, 0, Moves{}, name );
    std::cerr << name << " finished" << std::endl;
  }
};

void priorityExpandHeuristically( const Field& field, ResultManager& resultManager, const unsigned int sizeLimit );

// I'm not sure this ever performs better than the heuristical version
void priorityExpandAll( const Field& field, ResultManager& resultManager, const unsigned int sizeLimit );

void randomly( const Field& field, ResultManager& resultManager, unsigned int seed );

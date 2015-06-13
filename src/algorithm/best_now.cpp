#include "best_now.hpp"

#include <algorithm>
#include <iostream>

void best_now( const Field& field, ResultManager& result_manager )
{
  Field currentField( field );
  Score currentScore = 0;
  PossibleMoves possibleMoves;
  Moves moves;
  while( true )
  {
    currentField.calculateMoves( possibleMoves );

    if( possibleMoves.empty() )
    {
      currentScore -= currentField.calculatePenalty();
      result_manager.report( { currentScore, moves } );
      return;
    }
    
    std::sort( possibleMoves.begin(), possibleMoves.end(), sortByScore );

    const PossibleMove& move = possibleMoves[ 0 ];

    currentScore += currentField.remove( move.coordinate );
    moves.push_back( move.coordinate );

    std::cerr << "best move " << move.coordinate << " yields:\n" << currentField << '\n' << std::endl;
  }
}

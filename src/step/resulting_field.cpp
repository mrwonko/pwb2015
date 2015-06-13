#include "resulting_field.hpp"

#include <algorithm>
#include <limits>


template< Score ( *scoreField )( const Field& ), bool sortDescending >
static inline void creatingBestField( const Field& field, PossibleMoves& out_result )
{
  field.calculateMoves( out_result );
  thread_local Field nextField( { { 0 } }, 1 );
  for( PossibleMove& possibleMove : out_result )
  {
    nextField = field;
    nextField.remove( possibleMove.coordinate );
    possibleMove.score = scoreField( nextField );
  }
  std::sort( out_result.begin(), out_result.end(), sortDescending ? sortByScoreDescending : sortByScore );
}

template< bool penalizeZero >
static inline Score scoreByMatchCount( const Field& field )
{
  thread_local PossibleMoves nextPossibleMoves;
  field.calculateMoves( nextPossibleMoves );
  const Score score = nextPossibleMoves.size();
  // penalize last moves (the assumption is that more moves = more points)
  if( penalizeZero && score == 0 )
  {
    return std::numeric_limits< Score >::max();
  }
  else
  {
    return score;
  }
}

static inline Score scoreByLargestMatch( const Field& field )
{
  thread_local PossibleMoves nextPossibleMoves;
  field.calculateMoves( nextPossibleMoves );
  return nextPossibleMoves.empty() ? 0 : std::max_element( nextPossibleMoves.begin(), nextPossibleMoves.end(), sortByScore )->score;
}

void creatingFewestMatches( const Field& field, PossibleMoves& out_result )
{
  creatingBestField< scoreByMatchCount< false >, false >( field, out_result );
}

void creatingFewestMatchesAvoidingTermination( const Field& field, PossibleMoves& out_result )
{
  creatingBestField< scoreByMatchCount< true >, false >( field, out_result );
}

void creatingLargestMatch( const Field& field, PossibleMoves& out_result )
{
  creatingBestField< scoreByLargestMatch, true >( field, out_result );
}


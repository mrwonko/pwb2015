#include "lowest_immediate_score.hpp"

#include <algorithm>

bool lowest_immediate_score( const Field& field, Coordinate& out_result )
{
  PossibleMoves possibleMoves;
  field.calculateMoves( possibleMoves );
  if( possibleMoves.empty() )
  {
    return false;
  }
  std::sort( possibleMoves.begin(), possibleMoves.end(), sortByScore );
  out_result = possibleMoves.rbegin()->coordinate;
  return true;
}

#include "immediate_score.hpp"

#include <algorithm>

void highestImmediateScore( const Field& field, PossibleMoves& out_result )
{
  field.calculateMoves( out_result );
  std::sort( out_result.begin(), out_result.end(), sortByScore );
}

void lowestImmediateScore( const Field& field, PossibleMoves& out_result )
{
  field.calculateMoves( out_result );
  std::sort( out_result.begin(), out_result.end(), sortByScoreDescending );
}

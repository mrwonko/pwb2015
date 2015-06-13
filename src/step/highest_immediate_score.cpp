#include "highest_immediate_score.hpp"

#include <algorithm>

void highest_immediate_score( const Field& field, PossibleMoves& out_result )
{
  field.calculateMoves( out_result );
  std::sort( out_result.begin(), out_result.end(), sortByScore );
}

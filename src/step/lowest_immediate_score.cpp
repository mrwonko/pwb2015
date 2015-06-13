#include "lowest_immediate_score.hpp"

#include <algorithm>

void lowest_immediate_score( const Field& field, PossibleMoves& out_result )
{
  field.calculateMoves( out_result );
  std::sort( out_result.rbegin(), out_result.rend(), sortByScore );
}

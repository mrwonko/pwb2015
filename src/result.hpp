#pragma once

#include <vector>
#include <utility>
#include <cstdint>
#include <ostream>
#include <tuple>

// game field is 2^8 x 2^8 at most
// so the biggest match is 2^16 at once
// which scores (2^16 - 1)^2
// which is less than (2^16)^2 = 2^32
// due to exponential growth, that's the largest possible score
// so 32 bit is sufficient
typedef std::int_fast32_t Score;
typedef std::uint8_t Dimension;
struct Coordinate
{
  Dimension x;
  Dimension y;
};
typedef std::vector< Coordinate > Moves;

inline std::ostream& operator<<( std::ostream& os, const Coordinate& coord )
{
  return os << '(' << static_cast< unsigned int >( coord.x ) << ',' << static_cast< unsigned int >( coord.y ) << ')';
}

inline bool operator==( const Coordinate& lhs, const Coordinate& rhs )
{
  return std::tie( lhs.x, lhs.y ) == std::tie( rhs.x, rhs.y );
}

struct PossibleMove
{
  Coordinate coordinate;
  Score score;
};
typedef std::vector< PossibleMove > PossibleMoves;

inline bool sortByScoreDescending( const PossibleMove& lhs, const PossibleMove& rhs )
{
  return lhs.score > rhs.score;
}

inline bool sortByScore( const PossibleMove& lhs, const PossibleMove& rhs )
{
  return lhs.score < rhs.score;
}

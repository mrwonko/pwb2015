#pragma once

#include <vector>
#include <utility>
#include <cstdint>
#include <ostream>

// game field is 2^8 x 2^8 at most
// so the biggest match is 2^16 at once
// which scores (2^16 - 1)^2
// which is less than (2^16)^2 = 2^32
// due to exponential growth, that's the largest possible score
// so 32 bit is sufficient
typedef std::uint_fast32_t Score;
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

struct Result
{
  Score score;
  Moves moves;
};

struct PossibleMove
{
  Coordinate coordinate;
  Score score;
};
typedef std::vector< PossibleMove > PossibleMoves;

inline bool sortByScore( const PossibleMove& lhs, const PossibleMove& rhs )
{
  return lhs.score > rhs.score;
}

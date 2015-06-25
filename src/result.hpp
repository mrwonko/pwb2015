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
// thus we're using the fastest 32+ bit int
typedef std::int_fast32_t Score;


// there's one problem here: we also use Dimension to store a field's size, which means maximum size is 255*255, not 256*256
// (dimension does not mean size in this context)
// a 255 sized field uses coordinates in [0, 254]
typedef std::uint8_t Dimension;

struct Coordinate
{
  Dimension x;
  Dimension y;
};

typedef std::vector< Coordinate > Moves;

// printing coordinates
inline std::ostream& operator<<( std::ostream& os, const Coordinate& coord )
{
  return os << '(' << static_cast< unsigned int >( coord.x ) << ',' << static_cast< unsigned int >( coord.y ) << ')';
}

// comparing coordinates
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

// compare PossibleMoves
inline bool sortByScoreDescending( const PossibleMove& lhs, const PossibleMove& rhs )
{
  return lhs.score > rhs.score;
}

// compare PossibleMoves (inverted)
inline bool sortByScore( const PossibleMove& lhs, const PossibleMove& rhs )
{
  return lhs.score < rhs.score;
}

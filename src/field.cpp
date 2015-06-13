#include "field.hpp"

#include <algorithm>
#include <utility>
#include <cassert>

Field::Field( Cells&& cells, const Dimension width )
  : _cells( std::move( cells ) )
  , _size( { width, static_cast< Dimension >( _cells.size() / width ) } )
{
  assert( static_cast< unsigned int >( _size.x ) * _size.y == _cells.size() );
}

// FloodFills with empty (0)
// TODO: use scanline fill for better speed
static Score floodFill( Field::Cells& cells, const Coordinate size, const Coordinate coord, const Color color )
{
  const Field::Cells::size_type index = static_cast< Field::Cells::size_type >( coord.y ) * size.x + coord.x;
  if( cells[ index ] != color )
  {
    return 0;
  }
  cells[ index ] = 0;
  Score count = 1;
  if( coord.x > 0 )
  {
    count += floodFill( cells, size, { coord.x - 1u, coord.y }, color );
  }
  if( coord.x + 1 < size.x )
  {
    count += floodFill( cells, size, { coord.x + 1u, coord.y }, color );
  }
  if( coord.y > 0 )
  {
    count += floodFill( cells, size, { coord.x, coord.y - 1u }, color );
  }
  if( coord.y + 1 < size.y )
  {
    count += floodFill( cells, size, { coord.x, coord.y + 1u }, color );
  }
  return count;
}

void Field::calculateMoves( PossibleMoves& out_moves ) const
{
  thread_local std::vector< Color > cells;
  cells = _cells;
  Coordinate coord{ 0, 0 };
  out_moves.clear();
  // stop on first empty column
  for( coord.x = 0; coord.x < _size.x && get( { coord.x, 0 } ) != 0; ++coord.x )
  {
    Color col;
    // stop ascent at first empty cell
    for( coord.y = 0; coord.y < _size.y && (col = get( coord ) ) != 0; ++coord.y )
    {
      assert( col != 0 );
      Score tiles = floodFill( cells, _size, coord, col );
      if( tiles > 1 )
      {
        out_moves.push_back( { coord, ( tiles - 1 ) * ( tiles - 1 ) } );
      }
    }
  }
}

Score Field::remove( Coordinate coord )
{
  Color color = get( coord );
  if( color == 0 )
  {
    return 0;
  }
  Score tiles = floodFill( _cells, _size, coord, color );
  Coordinate pos{ 0, 0 };
  // move tiles down
  for( pos.x = 0; pos.x < _size.x; ++pos.x )
  {
    Dimension yOut = 0;
    for( pos.y = 0; pos.y < _size.y; ++pos.y )
    {
      const Color color = get( pos );
      if( color == 0 )
      {
        continue;
      }
      if( yOut != pos.y )
      {
        set( Coordinate{ pos.x, yOut }, color );
        set( pos, 0 );
      }
      ++yOut;
    }
  }
  // move rows left (empty rows have an empty bottom tile)
  Dimension xOut = 0;
  for( pos.x = 0; pos.x < _size.x; ++pos.x )
  {
    pos.y = 0;
    if( get( pos ) == 0 )
    {
      continue;
    }
    for( pos.y = 0; pos.y < _size.y; ++pos.y )
    {
      const Color color = get( pos );
      if( color == 0 )
      {
        break;
      }
      if( pos.x != xOut )
      {
        set( Coordinate{ xOut, pos.y }, color );
        set( pos, 0 );
      }
    }
    ++xOut;
  }
  return ( tiles - 1 ) * ( tiles - 1 );
}

Score Field::calculatePenalty()
{
  // sorting in-place, i.e. Field is unusable now
  std::sort( _cells.begin(), _cells.end() );
  auto it = _cells.begin();
  // ignore 0s
  while( it != _cells.end() && *it == 0 )
  {
    ++it;
  }
  if( it == _cells.end() )
  {
    return 0;
  }
  Score penalty = 0;
  Score curCountMinusOne = 0;
  Color curColor = *it;
  ++it;
  while( it != _cells.end() )
  {
    if( *it != curColor )
    {
      curColor = *it;
      penalty += curCountMinusOne * curCountMinusOne;
      curCountMinusOne = 0;
    }
    else
    {
      ++curCountMinusOne;
    }
    ++it;
  }
  return penalty + curCountMinusOne * curCountMinusOne;
}

std::ostream& operator<<( std::ostream& os, const Field& field )
{
  const Coordinate size = field.getSize();
  for( Coordinate pos{ 0, size.y - 1u }; ; --pos.y )
  {
    for( pos.x = 0; pos.x < size.x; ++pos.x )
    {
      os << static_cast< unsigned int >( field.get( pos ) );
      if( pos.x + 1 < size.x )
      {
        os << '\t';
      }
    }
    if( pos.y > 0 )
    {
      os << '\n';
    }
    else
    {
      break;
    }
  }
  return os;
}

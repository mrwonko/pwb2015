#pragma once

#include <vector>
#include <cstdint>
#include <ostream>

#include "result.hpp"

typedef std::uint8_t Color;

// TODO: write a faster, caching data structure

class Field
{
public:
  typedef std::vector< Color > Cells;
public:
  Field( Cells&& cells, const Dimension width );
  Field( const Field& ) = default;
  Field( Field&& ) = default;
  Field& operator=( const Field & ) = default;
  Field& operator=( Field && ) = default;

  inline Color get( const Coordinate coord ) const
  {
    return _cells[ static_cast< Cells::size_type >( coord.y ) * _size.x + coord.x ];
  }
  Score remove( Coordinate coord );
  void calculateMoves( PossibleMoves& out_moves ) const;
  // destructive - sorts the cells (faster and acceptable)
  Score calculatePenalty();

  Coordinate getSize() const
  {
    return _size;
  }
private:
  inline bool isColumnEmpty( Dimension x ) const
  {
    return get( { x, 0 } ) == 0;
  }

  inline void set( const Coordinate coord, const Color color )
  {
    _cells[ static_cast< Cells::size_type >( coord.y ) * _size.x + coord.x ] = color;
  }

  Cells _cells;
  Coordinate _size;
};

std::ostream& operator<<( std::ostream& os, const Field& field );

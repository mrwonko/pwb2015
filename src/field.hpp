#pragma once

#include <vector>
#include <cstdint>
#include <ostream>
#include <iterator>
#include <tuple>

#include "result.hpp"

typedef std::uint8_t Color;

// TODO: write a faster, caching data structure (cache possible moves)
// TODO: use a single size across all fields to save space
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

  struct Entry
  {
    Coordinate coordinate;
    Color color;
  };

  // iterate through cells
  // this is probably me refactoring too much; I use this like twice and I'm not entirely convinced the code is nicer for it.
  // after all, this is not a simple iterator through the cells; it also returns the current coordinate (since we need that), making it a tad icky.
  // Would be nicer with coroutines/generators; maybe in C++17? But it'll be a while until that's supported at FH.
  class const_iterator : public std::iterator< std::input_iterator_tag, Entry, int >
  {
    friend class Field;
  public:
    inline bool operator==( const const_iterator& rhs ) const
    {
      return _entry.coordinate == rhs._entry.coordinate;
    }
    inline bool operator!=( const const_iterator& rhs ) const
    {
      return !( ( *this ) == rhs );
    }
    // invalidated on ++
    const Entry& operator*() const
    {
      return _entry;
    }
    const Entry* operator->() const
    {
      return &_entry;
    }
    const_iterator& operator++()
    {
      _entry.coordinate.x += 1;
      if( _entry.coordinate.x >= _field->getSize().x )
      {
        _entry.coordinate.x = 0;
        _entry.coordinate.y += 1;
      }
      return *this;
    }
    const_iterator operator++( int ) const
    {
      return ++const_iterator( *this );
    }

  private:
    const_iterator( const Field& field )
      : _field( &field )
      , _entry{ { 0, 0 }, 0 }
    {
    }

    const_iterator( const Field& field, const Coordinate& coordinate )
      : _field( &field )
      , _entry{ coordinate, 0 }
    {
    }

  private:
    const Field* _field;
    Entry _entry;
  };

  const_iterator begin() const
  {
    return const_iterator( *this );
  }
  const_iterator end() const
  {
    return const_iterator( *this, { 0, getSize().y + 1u } );
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

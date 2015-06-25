#include "parser.hpp"

#include <utility>
#include <stdexcept>
#include <cctype>
#include <sstream>


Field parse( const std::string& input )
{
  std::string::const_iterator it = input.begin();


  // ignores spaces, throws on end of input
  auto read = [ &it, &input ]()
  {
    while( it != input.end() && std::isspace( *it ) )
    {
      ++it;
    }

    if( it == input.end() )
    {
      throw std::runtime_error( "unexpected end of input" );
    }

    return *( it++ );
  };


  auto expect = [ &read ]( const char expected )
  {
    const char actual = read();

    if( actual != expected )
    {
      std::stringstream error;
      error << "expected '" << expected << "', got '" << actual << "'!";
      throw std::runtime_error( error.str() );
    }
  };



  expect( '[' );


  Field::Cells cells;
  Dimension width{ 0 };

  bool first = true;
  bool fieldDone = false;

  while( !fieldDone )
  {
    switch( read() )
    {
      // we assume valid input, i.e. no leading commas; otherwise, this code will not parse correctly. But what can I do? Garbage in, garbage out.
    case ',':
      expect( '[' );
      // fall through
    case '[':
    {
      // read row
      char currentChar = read();
      Color currentColor = 0;

      while( currentChar != ']' )
      {
        // for simplicity, let's just assume valid input.
        if( currentChar == ',' )
        {
          cells.push_back( currentColor );
          currentColor = 0;
        }
        else
        {
          currentColor *= 10;
          currentColor += currentChar - '0';
        }
        currentChar = read();
      }

      cells.push_back( currentColor );

      if( first )
      {
        width = static_cast< Dimension >( cells.size() );
        first = false;
      }
      else
      {
        if( cells.size() % width != 0 ) // does not catch multiple-ofs, but that won't crash.
        {
          throw std::runtime_error( "varying column count in input!" );
        }
      }
      break;
    }

    case ']':
    {
      fieldDone = true;
      break;
    }

    default:
      throw std::runtime_error( "invalid input!" );
    }
  }

  // skip trailing spaces
  while( it != input.end() && std::isspace( *it ) )
  {
    ++it;
  }


  // must be end of input
  if( it != input.end() )
  {
    throw std::runtime_error(
      "invalid trailing input: \"" + std::string( it, input.end() ) + '"'
    );
  }

  return Field( std::move( cells ), width );
}

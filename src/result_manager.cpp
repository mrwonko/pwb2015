#include "result_manager.hpp"
#include <iostream>

void ResultManager::report( const Result& result )
{
  std::lock_guard< std::mutex > lock( _mutex );
  if( _first )
  {
    _first = false;
  }
  else if( result.score < _bestScore )
  {
    return;
  }
  _bestScore = result.score;
  std::cout << '[';
  bool first = true;
  for( const Coordinate& move : result.moves )
  {
    if( !first )
    {
      std::cout << ',';
    }
    else
    {
      first = false;
    }
    std::cout << move;
  }
  std::cout << ']' << std::endl;
}

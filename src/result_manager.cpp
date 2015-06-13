#include "result_manager.hpp"
#include <iostream>

void ResultManager::report( const Result& result, const char* source )
{
  // do thread-local best-check first to avoid mutex locks when possible
  thread_local bool first = true;
  thread_local Score bestScore;
  if( first )
  {
    first = false;
    bestScore = result.score;
  }
  else
  {
    if( result.score <= bestScore )
    {
      return;
    }
    bestScore = result.score;
  }
  // then do synchronized best-check
  {
    std::lock_guard< std::mutex > lock( _mutex );
    if( _first )
    {
      _first = false;
    }
    else if( result.score <= _bestScore )
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
    std::cerr << source << ": " << result.score << std::endl;
  }
}

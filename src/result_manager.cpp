#include "result_manager.hpp"
#include <iostream>

bool ResultManager::report( const Score score, const Moves& moves, const char* source )
{
  // do thread-local best-check first to avoid mutex locks when possible
  thread_local bool first = true;
  thread_local Score bestScore;
  if( first )
  {
    first = false;
    bestScore = score;
  }
  else
  {
    if( score <= bestScore )
    {
      return false;
    }
    bestScore = score;
  }
  // then do synchronized best-check
  {
    std::lock_guard< std::mutex > lock( _mutex );
    if( _first )
    {
      _first = false;
    }
    else if( score <= _bestScore )
    {
      return false;
    }
    _bestScore = score;
    std::cout << '[';
    bool first = true;
    for( const Coordinate& move : moves )
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
    std::cerr << source << ": " << score << std::endl;
    return true;
  }
}

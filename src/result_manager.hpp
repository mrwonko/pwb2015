#pragma once

#include <mutex>

#include "result.hpp"

// In retrospect I should've just used a function
class ResultManager
{
public:
  bool report( const Score score, const Moves& moves, const char* source );
private:
  mutable std::mutex _mutex;
  Score _bestScore;
  bool _first = true;
};

#pragma once

#include <mutex>

#include "result.hpp"

class ResultManager
{
public:
  void report( const Score score, const Moves& moves, const char* source );
private:
  mutable std::mutex _mutex;
  Score _bestScore;
  bool _first = true;
};

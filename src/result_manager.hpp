#pragma once

#include <mutex>

#include "result.hpp"

class ResultManager
{
public:
  void report( const Result& result );
private:
  mutable std::mutex _mutex;
  Score _bestScore;
  bool _first = true;
};

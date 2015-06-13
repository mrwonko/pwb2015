#include <iostream>
#include <condition_variable>
#include <thread>
#include <vector>
#include <string>
#include <atomic>
#include <stdexcept>

#include "result_manager.hpp"
#include "parser.hpp"
#include "step/highest_immediate_score.hpp"
#include "step/lowest_immediate_score.hpp"
#include "algorithms.hpp"

int main( int argc, char** argv )
{
  std::string input;
  if( !std::getline( std::cin, input ) )
  {
    std::cerr << "No input!" << std::endl;
    return 1;
  }

  try
  {
    const Field field = parse( input );

    ResultManager resultManager;
    std::vector< std::thread > threads;

    // start algorithms
    threads.emplace_back( single_best< highest_immediate_score >, std::ref( field ), std::ref( resultManager ), "single best highest immediate score" );
    threads.emplace_back( single_best< lowest_immediate_score >, std::ref( field ), std::ref( resultManager ), "single best lowest immediate score" );
    threads.emplace_back( n_best< highest_immediate_score, 2 >, std::ref( field ), std::ref( resultManager ), "2 best highest immediate score" );
    threads.emplace_back( n_best< lowest_immediate_score, 2 >, std::ref( field ), std::ref( resultManager ), "2 best lowest immediate score" );

    for( std::thread& thread : threads )
    {
      thread.join();
    }
    return 0;
  }
  catch( std::exception& e )
  {
    std::cerr << "Exception!\n" << e.what() << std::endl;
    return 1;
  }
}

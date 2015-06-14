#include <iostream>
#include <condition_variable>
#include <thread>
#include <vector>
#include <string>
#include <atomic>
#include <stdexcept>

#include "result_manager.hpp"
#include "parser.hpp"
#include "step/immediate_score.hpp"
#include "step/resulting_field.hpp"
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
    threads.emplace_back( each< 
      highestImmediateScore,
      lowestImmediateScore,
      creatingLargestMatch,
      creatingFewestMatches,
      creatingFewestMatchesAvoidingTermination
    >::singleBest, std::ref( field ), std::ref( resultManager ), 0, Moves{}, "single best" );
    threads.emplace_back( nBest< highestImmediateScore, 2 >, std::ref( field ), std::ref( resultManager ), "2 best highest immediate score" );
    threads.emplace_back( nBest< lowestImmediateScore, 2 >, std::ref( field ), std::ref( resultManager ), "2 best lowest immediate score" );

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

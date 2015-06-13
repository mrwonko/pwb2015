#include <iostream>
#include <condition_variable>
#include <thread>
#include <vector>
#include <string>
#include <atomic>
#include <stdexcept>

#include "result_manager.hpp"
#include "parser.hpp"
#include "algorithm/best_now.hpp"

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
    threads.emplace_back( best_now, std::ref( field ), std::ref( resultManager ) );

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

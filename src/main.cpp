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
    threads.emplace_back( [ &field, &resultManager ]()
    {
      each<
        highestImmediateScore,
        lowestImmediateScore,
        creatingLargestMatch,
        creatingFewestMatches,
        creatingFewestMatchesAvoidingTermination,
        creatingSmallestMeanDistanceToCentroid,
        creatingSmallestMaximumDistanceToCentroid
      >::singleBest( field, resultManager, "single best" );
      // by not using a (pseudo) random seed we actually stay deterministic!
      // I don't like relying on "luck" so let's use a deterministic approach.
      randomly( field, resultManager, 0xBAADF00D );
    } );
    // these usually take too long and use too much memory
    if( field.getSize().x * field.getSize().y <= 50 )
    {
      threads.emplace_back( nBest< highestImmediateScore, 2 >, std::ref( field ), std::ref( resultManager ), "2 best highest immediate score" );
      threads.emplace_back( nBest< lowestImmediateScore, 2 >, std::ref( field ), std::ref( resultManager ), "2 best lowest immediate score" );
    }
    // worst case: size 256*256 * 1 Byte, 1024 moves (?) * 2 bytes/move -> 2^16+2^11 bytes < 128KB. allowing for 1024MB that's 8*1024 nodes
    // usually fields are much smaller though, on 16x16 I hardly pass 10MB total
    // since this algorithm doesn't yield any results until it's fully done I use varying lengths (longer = better results, potentially)
    threads.emplace_back( [ &field, &resultManager ]()
    {
      priorityExpandHeuristically( field, resultManager, 16 );
      //priorityExpandAll( field, resultManager, 16 );
      priorityExpandHeuristically( field, resultManager, 8096 );
    } );
    threads.emplace_back( [ &field, &resultManager ]()
    {
      priorityExpandHeuristically( field, resultManager, 64 );
      //priorityExpandAll( field, resultManager, 64 );
      //priorityExpandHeuristically( field, resultManager, 2048 );
    } );
    threads.emplace_back( [ &field, &resultManager ]()
    {
      priorityExpandHeuristically( field, resultManager, 256 );
      //priorityExpandAll( field, resultManager, 256 );
      priorityExpandHeuristically( field, resultManager, 1024 );
    } );

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

#include "algorithms.hpp"
#include "step/immediate_score.hpp"
#include "step/resulting_field.hpp"

#include <set>
#include <map>
#include <iterator>
#include <tuple>
#include <string>
#include <random>

struct Node
{
  Moves moves;
  Field field;
};

typedef std::multimap< Score, Node > NodeMap;

class NodeQueue : private NodeMap
{
public:
  void emplace( Score score, Node&& node, const unsigned int sizeLimit )
  {
    NodeMap::emplace( std::make_pair( score, std::move( node ) ) );
    // remove small score Nodes to satisfy size constraint
    if( NodeMap::size() > sizeLimit )
    {
      auto end = NodeMap::begin();
      std::advance( end, NodeMap::size() - sizeLimit );
      NodeMap::erase( NodeMap::begin(), end );
    }
  }

  using NodeMap::begin;
  using NodeMap::end;
  using NodeMap::swap;
  using NodeMap::NodeMap;
  using NodeMap::empty;
  using NodeMap::iterator;
};

struct coordinateComp
{
  bool operator()( const Coordinate& lhs, const Coordinate& rhs )
  {
    return std::tie( lhs.y, lhs.x ) < std::tie( rhs.y, rhs.x );
  }
};

typedef std::set< Coordinate, coordinateComp > CoordinateSet;

template< typename MovesContainer, void( *calculateMoves )( const Field&, MovesContainer& ) >
static void priorityExpand( const Field& field, ResultManager& resultManager, const unsigned int sizeLimit, const char* name )
{
  NodeQueue queue{ { 0u, { Moves(), field } } };
  PossibleMoves possibleMoves;
  while( !queue.empty() )
  {
    NodeQueue lastQueue( std::move( queue ) );
    for( std::pair< const Score, Node >& item : lastQueue )
    {
      Node& node = item.second;
      const Score& score = item.first;
      MovesContainer moves;
      calculateMoves( node.field, moves );
      if( moves.empty() )
      {
        resultManager.report( item.first - node.field.calculatePenalty(), node.moves, name );
      }
      else
      {
        for( const Coordinate& move : moves )
        {
          Node newNode{ node.moves, node.field };
          newNode.moves.push_back( move );
          const Score increase = newNode.field.remove( move );
          queue.emplace( score + increase, std::move( newNode ), sizeLimit ); // drops low-score entries once size limit is reached
        }
      }
    }
  }
  std::cerr << name << " finished" << std::endl;
}

static void calculateHeuristically( const Field& field, CoordinateSet& out_result )
{
  static const MoveCalculation moveCalculations[] =
  {
    highestImmediateScore,
    lowestImmediateScore,
    creatingLargestMatch,
    creatingFewestMatches,
    creatingFewestMatchesAvoidingTermination,
    creatingSmallestMeanDistanceToCentroid,
    creatingSmallestMaximumDistanceToCentroid
  };
  thread_local PossibleMoves possibleMoves;
  for( MoveCalculation moveCalculation : moveCalculations )
  {
    moveCalculation( field, possibleMoves );
    if( !possibleMoves.empty() )
    {
      out_result.insert( possibleMoves.front().coordinate );
    }
  }
}

void priorityExpandHeuristically( const Field& field, ResultManager& resultManager, const unsigned int sizeLimit )
{
  std::string name = "heuristical limited expansion (" + std::to_string( sizeLimit ) + ")";
  priorityExpand< CoordinateSet, calculateHeuristically >( field, resultManager, sizeLimit, name.c_str() );
}

static void calculateAll( const Field& field, std::vector< Coordinate >& out_result )
{
  thread_local PossibleMoves possibleMoves;
  field.calculateMoves( possibleMoves );
  out_result.reserve( possibleMoves.size() );
  for( const PossibleMove& move : possibleMoves )
  {
    out_result.push_back( move.coordinate );
  }
}

void priorityExpandAll( const Field& field, ResultManager& resultManager, const unsigned int sizeLimit )
{
  std::string name = "limited full expansion (" + std::to_string( sizeLimit ) + ")";
  priorityExpand< std::vector< Coordinate >, calculateAll >( field, resultManager, sizeLimit, name.c_str() );
}


void randomly( const Field& initialField, ResultManager& resultManager, unsigned int seed )
{
  std::default_random_engine engine( seed );
  PossibleMoves possibleMoves;

  struct Step
  {
    Field field;
    Score score;
  };

  Moves moves;
  std::vector< Step > fields;
  while( true )
  {
    fields.push_back( { initialField, 0u } );
    fields.back().field.calculateMoves( possibleMoves );
    while( !possibleMoves.empty() )
    {
      std::uniform_int_distribution< unsigned int > uniform_distribution( 0, possibleMoves.size() - 1 );
      Coordinate move = possibleMoves[ uniform_distribution( engine ) ].coordinate;
      moves.push_back( move );
      fields.push_back( fields.back() );
      fields.back().score += fields.back().field.remove( move );
      fields.back().field.calculateMoves( possibleMoves );
    }
    fields.back().score -= fields.back().field.calculatePenalty();
    resultManager.report( fields.back().score, moves, "randomly" );

    fields.pop_back();
    moves.pop_back();
    // maybe use something > 1? as it stands, there will be some redundancy.
    while( fields.size() > 1 )
    {
      each<
        highestImmediateScore,
        lowestImmediateScore,
        creatingLargestMatch,
        creatingFewestMatches,
        creatingFewestMatchesAvoidingTermination,
        creatingSmallestMeanDistanceToCentroid,
        creatingSmallestMaximumDistanceToCentroid
      >::_singleBest( fields.back().field, resultManager, fields.back().score, moves, "randomly single best" );
      fields.pop_back();
      moves.pop_back();
    }

    moves.clear();
    fields.clear();
  }
}

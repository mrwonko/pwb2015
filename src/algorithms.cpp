#include "algorithms.hpp"
#include "step/immediate_score.hpp"
#include "step/resulting_field.hpp"

#include <set>
#include <map>
#include <iterator>
#include <tuple>
#include <string>

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

void priorityExpand( const Field& field, ResultManager& resultManager, const unsigned int sizeLimit )
{
  MoveCalculation moveCalculations[] = 
  {
    highestImmediateScore,
    lowestImmediateScore,
    creatingLargestMatch,
    creatingFewestMatches,
    creatingFewestMatchesAvoidingTermination,
    creatingSmallestMeanDistanceToCentroid,
    creatingSmallestMaximumDistanceToCentroid
  };
  NodeQueue queue{ { 0u, { Moves(), field } } };
  PossibleMoves possibleMoves;
  std::string name = "limited expansion (" + std::to_string( sizeLimit ) + ")";
  while( !queue.empty() )
  {
    NodeQueue lastQueue( std::move( queue ) );
    for( std::pair< const Score, Node >& item : lastQueue )
    {
      Node& node = item.second;
      const Score& score = item.first;
      CoordinateSet moves;
      for( MoveCalculation moveCalculation : moveCalculations )
      {
        moveCalculation( node.field, possibleMoves );
        if( !possibleMoves.empty() )
        {
          moves.insert( possibleMoves.front().coordinate );
        }
      }
      if( moves.empty() )
      {
        resultManager.report( item.first - node.field.calculatePenalty(), node.moves, name.c_str() );
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

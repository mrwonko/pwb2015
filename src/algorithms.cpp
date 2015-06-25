#include "algorithms.hpp"
#include "step/immediate_score.hpp"
#include "step/resulting_field.hpp"

#include <set>
#include <map>
#include <iterator>
#include <tuple>
#include <string>
#include <random>

static inline void singleBestImpl( const Field& field, ResultManager& resultManager, MoveCalculation calculatePossibleMoves, Score currentScore, Moves& moves,  const char* name )
{
  thread_local PossibleMoves possibleMoves;
  thread_local Field currentField( { { 0 } }, 1 );
  currentField = field;

  calculatePossibleMoves( currentField, possibleMoves );
  while( !possibleMoves.empty() )
  {
    const Coordinate& move = possibleMoves[ 0 ].coordinate;
    currentScore += currentField.remove( move );
    moves.push_back( move );
    calculatePossibleMoves( currentField, possibleMoves );
  }

  currentScore -= currentField.calculatePenalty();
  resultManager.report( currentScore, moves, name );
}

void singleBest( const Field& field, ResultManager& resultManager, MoveCalculation calculatePossibleMoves, const char* name )
{
  thread_local Moves moves;
  moves.clear();
  singleBestImpl( field, resultManager, calculatePossibleMoves, 0, moves, name );
}

static void nBestImpl( const Field& currentField, ResultManager& resultManager, MoveCalculation calculatePossibleMoves, const unsigned int n, Score currentScore, Moves& moves, const char* name )
{
  PossibleMoves possibleMoves;
  calculatePossibleMoves( currentField, possibleMoves );
  if( possibleMoves.empty() )
  {
    currentScore -= Field( currentField ).calculatePenalty();
    resultManager.report( currentScore, moves, name );
  }
  else
  {
    // try the best n moves
    possibleMoves.resize( std::min< PossibleMoves::size_type >( possibleMoves.size(), n ) );
    for( const PossibleMove& move : possibleMoves )
    {
      Field nextField( currentField );
      Score nextScore = currentScore + nextField.remove( move.coordinate );
      moves.push_back( move.coordinate );
      nBestImpl( std::move( nextField ), resultManager, calculatePossibleMoves, n, nextScore, moves, name );
      moves.pop_back();
    }
  }
}

void nBest( const Field& field, ResultManager& resultManager, MoveCalculation calculatePossibleMoves, const unsigned int n, const char* name )
{
  Moves moves;
  nBestImpl( field, resultManager, calculatePossibleMoves, n, 0, moves, name );
  std::cerr << name << " finished" << std::endl;
}

static inline void eachSingleBestImpl( const Field& field, ResultManager& resultManager, const std::vector< MoveCalculationInfo >& moveCalculations, Score currentScore, const Moves& moves )
{
  for( MoveCalculationInfo entry : moveCalculations )
  {
    thread_local Moves currentMoves;
    currentMoves = moves;
    singleBestImpl( field, resultManager, entry.moveCalculation, currentScore, currentMoves, entry.name );
  }
}

void eachSingleBest( const Field& field, ResultManager& resultManager, const std::vector< MoveCalculationInfo >& moveCalculations )
{
  eachSingleBestImpl( field, resultManager, moveCalculations, 0, {} );
}

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
      static const std::vector< MoveCalculationInfo > moveCalculations {
        { highestImmediateScore, "randomly single best highest immediate score" },
        { lowestImmediateScore, "randomly single best lowest immediate score" },
        { creatingLargestMatch, "randomly single best creating largest match" },
        { creatingFewestMatches, "randomly single best creating fewest matches" },
        { creatingFewestMatchesAvoidingTermination, "randomly single best creating fewest matches (avoiding termination)" },
        { creatingSmallestMeanDistanceToCentroid, "randomly single best creating smallest mean distance to centroid" },
        { creatingSmallestMaximumDistanceToCentroid, "randomly single best creating smallest maximum distance to centroid" }
      };
      eachSingleBestImpl( fields.back().field, resultManager, moveCalculations, fields.back().score, moves );
      fields.pop_back();
      moves.pop_back();
    }

    moves.clear();
    fields.clear();
  }
}

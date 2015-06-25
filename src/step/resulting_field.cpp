#include "resulting_field.hpp"

#include <algorithm>
#include <limits>
#include <map>
#include <cmath>


// sort based on the score of the resulting fields, scored by a given function
template< Score ( *scoreField )( const Field& ), bool sortDescending >
static inline void creatingBestField( const Field& field, PossibleMoves& out_result )
{
  field.calculateMoves( out_result );
  thread_local Field nextField( { { 0 } }, 1 );
  for( PossibleMove& possibleMove : out_result )
  {
    nextField = field;
    nextField.remove( possibleMove.coordinate );
    possibleMove.score = scoreField( nextField );
  }
  std::sort( out_result.begin(), out_result.end(), sortDescending ? sortByScoreDescending : sortByScore );
}

// score a field by the number of matches in it; since the one with the fewest matches is chosen, an option to avoid 0 matches (= dead end) is available
template< bool penalizeZero >
static inline Score scoreByMatchCount( const Field& field )
{
  thread_local PossibleMoves nextPossibleMoves;
  field.calculateMoves( nextPossibleMoves );
  const Score score = nextPossibleMoves.size();
  // penalize last moves (the assumption is that more moves = more points)
  if( penalizeZero && score == 0 )
  {
    return std::numeric_limits< Score >::max();
  }
  else
  {
    return score;
  }
}

static inline Score scoreByLargestMatch( const Field& field )
{
  thread_local PossibleMoves nextPossibleMoves;
  field.calculateMoves( nextPossibleMoves );
  // max_element returns an end() iterator for empty containers, thus the check
  return nextPossibleMoves.empty() ? 0 : std::max_element( nextPossibleMoves.begin(), nextPossibleMoves.end(), sortByScore )->score;
}

void creatingFewestMatches( const Field& field, PossibleMoves& out_result )
{
  creatingBestField< scoreByMatchCount< false >, false >( field, out_result );
}

void creatingFewestMatchesAvoidingTermination( const Field& field, PossibleMoves& out_result )
{
  creatingBestField< scoreByMatchCount< true >, false >( field, out_result );
}

void creatingLargestMatch( const Field& field, PossibleMoves& out_result )
{
  creatingBestField< scoreByLargestMatch, true >( field, out_result );
}


// basically creatingBestField, but for float scores.
template< float( *scoreField )( const Field& ), bool sortDescending >
static inline void creatingBestField_float( const Field& field, PossibleMoves& out_result )
{
  field.calculateMoves( out_result );
  thread_local std::vector< float > _scores;
  std::vector< float >& scores = _scores;
  scores.resize( 0 );
  scores.reserve( out_result.size() );
  thread_local Field nextField( { { 0 } }, 1 );
  for( PossibleMoves::size_type i = 0; i < out_result.size(); ++i )
  {
    nextField = field;
    PossibleMove& move = out_result[ i ];
    nextField.remove( move.coordinate );
    move.score = i;
    scores.push_back( scoreField( nextField ) );
  }
  if( sortDescending )
  {
    std::sort( out_result.begin(), out_result.end(), [ &scores ]( const PossibleMove& lhs, const PossibleMove& rhs ) { return scores[ lhs.score ] < scores[ rhs.score ]; } );
  }
  else
  {
    std::sort( out_result.begin(), out_result.end(), [ &scores ]( const PossibleMove& lhs, const PossibleMove& rhs ) { return scores[ lhs.score ] > scores[ rhs.score ]; } );
  }
}

typedef std::map< Color, std::pair< float, float > > Centroids;

static Centroids calculateCentroids( const Field& field )
{
  // worst case: 256 * sum( 0 to 255 ) = 256 * 255 * 256 / 2 = 2^8 * 2^8 * 255 = (2^8 - 1) * 2^16 -> fits in 32 bit
  struct CoordinateSum
  {
    std::uint32_t count;
    std::uint32_t sumX, sumY;
  };
  std::map< Color, CoordinateSum > sums;
  for( Field::Entry cell : field )
  {
    if( cell.color != 0 )
    {
      auto posAndInserted = sums.insert( { cell.color,{ 1u, cell.coordinate.x, cell.coordinate.y } } );
      if( !posAndInserted.second )
      {
        CoordinateSum& entry = posAndInserted.first->second;
        entry.count += 1;
        entry.sumX += cell.coordinate.x;
        entry.sumY += cell.coordinate.y;
      }
    }
  }
  Centroids result;
  for( const std::pair< Color, CoordinateSum >& entry : sums )
  {
    result.insert( { entry.first, { static_cast< float >( entry.second.sumX ) / entry.second.count, static_cast< float >( entry.second.sumY ) / entry.second.count } } );
  }
  return std::move( result );
}

static inline float calculateDistance( const std::pair< float, float >& centroid, const Coordinate& coordinate )
{
  float distX = centroid.first - coordinate.x;
  float distY = centroid.second - coordinate.y;
  return std::sqrt( distX * distX + distY * distY );
}

static inline float scoreByMeanDistanceToCentroid( const Field& field )
{
  const Centroids centroids = calculateCentroids( field );
  float totalDistance = 0.f;
  unsigned int count = 0;
  for( Field::Entry cell : field )
  {
    if( cell.color == 0 ) continue;
    totalDistance += calculateDistance( centroids.at( cell.color ), cell.coordinate );
    count += 1;
  }
  return totalDistance / count;
}

void creatingSmallestMeanDistanceToCentroid( const Field& field, PossibleMoves& out_result )
{
  creatingBestField_float< scoreByMeanDistanceToCentroid, false >( field, out_result );
}

static inline float scoreByLargestDistanceToCentroid( const Field& field )
{
  const Centroids centroids = calculateCentroids( field );
  float maxDistance = 0.f;
  for( Field::Entry cell : field )
  {
    if( cell.color == 0 ) continue;
    maxDistance = std::max( maxDistance, calculateDistance( centroids.at( cell.color ), cell.coordinate ) );
  }
  return maxDistance;
}

void creatingSmallestMaximumDistanceToCentroid( const Field& field, PossibleMoves& out_result )
{
  creatingBestField_float< scoreByLargestDistanceToCentroid, false >( field, out_result );
}

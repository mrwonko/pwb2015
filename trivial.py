import sys

field = eval( sys.stdin.readline() )
height = len( field )
width = len( field[ 0 ] )

allCoords = [ "({},{})".format( x - 1, y - 1 ) for x in range( width, 0, -1 ) for y in range( height, 0, -1 ) ]
sys.stdout.write( "[{}]\n".format( ",".join( allCoords ) ) )

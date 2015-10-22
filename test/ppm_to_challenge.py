import sys

with open( sys.argv[ 1 ] ) as f:
    magic = f.readline()
    assert( magic == "P3\n" )
    comment = f.readline()
    assert( comment.startswith( "#" ) )
    dimensions = f.readline()
    w, h = map( int, dimensions.split() )
    _ = f.readline() # max
    colors = {}
    grid = []
    for y in range( h ):
        line = []
        for x in range( w ):
            color = tuple( [ int( f.readline() ) for _ in range( 3 ) ] )
            if color not in colors:
                colors[ color ] = len( colors ) + 1
            line.append( colors[ color ] )
        grid = [line] + grid
    with open( sys.argv[ 1 ] + ".txt", "w" ) as o:
        o.write( str( grid ).replace( " ", "" ) )
        o.write( "\n" )

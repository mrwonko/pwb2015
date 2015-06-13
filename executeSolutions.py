import subprocess
import select
import sys

programs = [
    [ "python", "trivial.py" ]
]

initial_state = []
best_score = 0

# spawn subprocesses
processes = [ subprocess.Popen( arguments, stdin = subprocess.PIPE, stdout = subprocess.PIPE ) for arguments in programs ]

processes_by_fileno = { process.stdout.fileno(): process for process in processes }

# read and forward input to children
data = sys.stdin.readline()
for process in processes_by_fileno.values():
    # don't try sending data to closed processes
    if process.poll() != None:
        continue
    process.stdin.write( data )
    process.stdin.close()

# forward input to postprocessor
sys.stdout.write( data )

# The owl is always correct (0,0)
sys.stdout.write( "[(0,0)]\n" )

# read and forward output
while len( processes_by_fileno ) > 0:
    # 1 second timeout in case we missed dead processes somehow
    filenos = select.select( processes_by_fileno.keys(), [], [], 1 )[ 0 ]
    for fileno in filenos:
        process = processes_by_fileno[ fileno ]
        sys.stdout.write( process.stdout.readline() )
    # drop finished processes
    processes_by_fileno = { fileno: process for fileno, process in processes_by_fileno.items() if process.poll() == None }

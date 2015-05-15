#!/bin/sh

# Create a named pipe so we can copy the input to best.sh
# (there may still be a previous one if we got killed)
rm -f pipe_input
mkfifo pipe_input

# * head ensures input gets closed
# * tee copies the input so we can also forward it to best.sh
# * "-" is stdin, so cat copies the input before the results
# * best.sh takes care of only displaying the best result yet
head -n 1 \
| tee pipe_input \
| ./executeSolutions.sh \
| cat pipe_input - \
| ./best.sh

rm pipe_input

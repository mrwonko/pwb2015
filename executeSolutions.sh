#!/bin/sh

# FIXME: does not start the programs until the input arrives, wasting time.

read input
# * --colsep splits arguments from commands in "solutions" file
# * --linebuffer makes it forward the output ASAP
#   but only one line at a time unlike --ungroup
# * the "solutions" file contains one command to execute on each line
bin/parallel \
 --colsep ' ' \
 --linebuffer \
 'echo $input | {}' \
 :::: solutions

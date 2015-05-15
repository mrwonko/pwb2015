#!/bin/sh

# fail on error
set -e

mkdir -p bin

# I need GNU parallel; use an installed version if possible
if which parallel
then
	echo "installed parallel found"
	ln -s `which parallel` bin/parallel
else
	# or build it if necessary
	echo "building parallel"
	mkdir -p build
	pushd build
	wget http://ftp.gnu.org/gnu/parallel/parallel-latest.tar.bz2
	tar -xvjf parallel-latest.tar.bz2
	cd parallel-*
	./configure --prefix=`pwd`/install
	make
	make install
	mv install/bin/parallel ../../bin/
	popd
	parallel=todo
fi

echo $parallel

echo "compile"


.PHONY: all run clean name

all:
	@mkdir -p build && cd build && cmake ../src && make

run:
	@build/octarine_optimizer

clean:
	rm -rf build

name:
	@echo Octarine Optimizer


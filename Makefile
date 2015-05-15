.PHONY: all run clean name

bin/parallel:
	@./getParallel.sh >&2

all: bin/parallel
	@echo "successfully compiled" >&2

run: all
	@./run.sh

clean:
	rm -rf build

name:
	@echo Octarine Optimizer


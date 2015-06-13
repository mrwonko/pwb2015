.PHONY: all run clean name

bin/:
	@mkdir -p bin >&2

bin/simplyTheBest: simplyTheBest.hs bin/
	@ghc -o bin/simplyTheBest simplyTheBest.hs >&2

all: bin/simplyTheBest
	@echo "successfully compiled" >&2

run: all
	@python executeSolutions.py | tail -n +2

clean:
	rm -rf build

name:
	@echo Octarine Optimizer


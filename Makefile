.PHONY: all run clean name

all:
	@echo "successfully compiled" >&2

run: all
	@python executeSolutions.py | tail -n +2

clean:
	rm -rf build

name:
	@echo Octarine Optimizer


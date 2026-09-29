PYTHON ?= python

.PHONY: sdl3-build sdl3-run sdl3-regression sdl3-trace-regression

sdl3-build:
	$(PYTHON) port/build.py build

sdl3-run:
	$(PYTHON) port/build.py run

sdl3-regression:
	$(PYTHON) port/build.py run --run-ms=4000
	$(PYTHON) tools/porting/regress_sdl3_trace.py

sdl3-trace-regression: sdl3-regression

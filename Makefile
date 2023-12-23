.PHONY: clean default
.DEFAULT_GOAL := default

CXX = g++
CFLAGS = -g -std=c++20 -pthread -ltbb -O2

default:
	@echo "Please specify a day using the command \"make XX\"."

build/%: Day\ %/solution.cpp
	@mkdir -p build
	$(CXX) $(CFLAGS) "$^" -o $@

clean:
	rm -rf build

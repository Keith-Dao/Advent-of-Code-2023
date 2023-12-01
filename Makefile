CXX = g++
CFLAGS = -g

.PHONY = clean dummy

dummy:
	@echo "Please specify a day using the command \"make XX\"."

%: Day\ %/solution.cpp
	@mkdir -p build
	$(CXX) $(CFLAGS) "$^" -o build/$@

clean:
	rm -rf build

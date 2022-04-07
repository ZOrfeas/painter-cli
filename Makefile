CXX=$(CLANG)++

INCLUDE_DIRS=/opt/homebrew/Cellar/llvm/13.0.1_1/include/c++/v1
INCLUDE_DIRS+= include
INCLUDE_DIRS+= src/include

INCLUDE_FLAGS=$(patsubst %, -I%, $(INCLUDE_DIRS))

CXXFLAGS=$(INCLUDE_FLAGS) -std=c++20 -Wall -Werror

TESTS=test-flag test-command
.PHONY: all tests clean $(TESTS)

all: tests
tests: $(TESTS)

test-flag: bin/test-flag
test-command: bin/test-command

bin/test-%: test/test-%.cpp src/include/%.hpp src/include/test.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -rf bin/*
distclean: clean
	rm -rf build/*

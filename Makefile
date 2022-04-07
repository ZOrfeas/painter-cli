CXX=$(CLANG)++

INCLUDE_DIRS=/opt/homebrew/Cellar/llvm/13.0.1_1/include/c++/v1
INCLUDE_DIRS+= include
INCLUDE_DIRS+= src/include

INCLUDE_FLAGS=$(patsubst %, -I%, $(INCLUDE_DIRS))

CXXFLAGS=$(INCLUDE_FLAGS) -std=c++20 -Wall -Werror

TESTS=test-flag test-command
.PHONY: all tests clean $(TESTS)
TEST_DEPS=src/include/test.hpp src/include/log.hpp

all: tests
tests: $(TESTS)

test-flag: bin/test-flag
test-command: bin/test-command

# manually add flag.hpp dependency to command.hpp tests
bin/test-command: src/include/flag.hpp
bin/test-%: test/test-%.cpp src/include/%.hpp $(TEST_DEPS) 
	$(CXX) $(CXXFLAGS) -O0 -g $< -o $@

clean:
	rm -rf bin/*
distclean: clean
	rm -rf build/*

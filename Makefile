CXX=$(CLANG)++

INCLUDE_DIRS=/opt/homebrew/Cellar/llvm/13.0.1_1/include/c++/v1
INCLUDE_DIRS+= include
INCLUDE_DIRS+= src/include

INCLUDE_FLAGS=$(patsubst %, -I%, $(INCLUDE_DIRS))

CXXFLAGS=$(INCLUDE_FLAGS) -std=c++20 -Wall -Werror

TESTS=test-flag test-command
.PHONY: all test-all clean $(TESTS)

all: tests
test-all: bin/test-all
test-flag: bin/test-flag
test-command: bin/test-command


bin/test-all: build/test-command.o build/test-flag.o
	$(CXX) $(CXXFLAGS) $^ -lgtest -lgtest_main -pthread -o $@
bin/test-flag: build/test-flag.o
	$(CXX) $(CXXFLAGS) $^ -lgtest -lgtest_main -pthread -o $@
bin/test-command: build/test-command.o
	$(CXX) $(CXXFLAGS) $^ -lgtest -lgtest_main -pthread -o $@

# manually add flag.hpp dependency to command.hpp tests
build/test-command.o: src/include/flag.hpp
build/test-%.o: test/test-%.cpp src/include/%.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf bin/*
distclean: clean
	rm -rf build/*

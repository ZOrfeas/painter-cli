CXX=$(CLANG)++

INCLUDE_DIRS=/opt/homebrew/Cellar/llvm/13.0.1_1/include/c++/v1
INCLUDE_DIRS+= include
INCLUDE_DIRS+= src/include

INCLUDE_FLAGS=$(patsubst %, -I%, $(INCLUDE_DIRS))

CXXFLAGS=$(INCLUDE_FLAGS) -std=c++20 -Wall -Werror

TESTS=test-stateless_fsm
.PHONY: all clean $(TESTS)

all:
tests: $(patsubst %,bin/%,$(TESTS)) bin/test-all

bin/test-all: $(patsubst %,build/%.o,$(TESTS))
	$(CXX) $(CXXFLAGS) $^ -lgtest -lgtest_main -o $@
bin/test-%: build/test-%.o 
	$(CXX) $(CXXFLAGS) $< -lgtest -lgtest_main -o $@


build/test-%.o: test/test-%.cpp src/include/%.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


clean:
	rm -rf bin/*
distclean: clean
	rm -rf build/*

CXX=$(CLANG)++

INCLUDE_DIRS=/opt/homebrew/Cellar/llvm/13.0.1_1/include/c++/v1
INCLUDE_DIRS+= include
INCLUDE_DIRS+= src/include

INCLUDE_FLAGS=$(patsubst %, -I%, $(INCLUDE_DIRS))

CXXFLAGS=$(INCLUDE_FLAGS) -std=c++20 -Wall -Werror

.PHONY: all clean

all: test/test-drafts.cpp 
	$(CXX) $(CXXFLAGS) -o bin/test-drafts $<

clean:
	rm -rf bin/*
distclean: clean
	rm -rf build/*

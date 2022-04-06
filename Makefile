.PHONY: all test clean

PROJECT_ROOT=.
BUILD_DIR=./build
BIN_DIR=./bin
SRC_DIR=./src
TEST_DIR=./test

CXX=$(CLANG)++

INCLUDE_DIRS=/opt/homebrew/Cellar/llvm/13.0.1_1/include/c++/v1
INCLUDE_DIRS+= $(PROJECT_ROOT)/include
INCLUDE_DIRS+= $(SRC_DIR)/include

INCLUDE_FLAGS=$(patsubst %, -I%, $(INCLUDE_DIRS))

CXXFLAGS=$(INCLUDE_FLAGS) -std=c++20 -Wall -Werror



all:
	echo $(CXXFLAGS)

test: $(patsubst $(TEST_DIR)/%.cpp,$(BIN_DIR)/%,$(wildcard $(TEST_DIR)/*))

$(BIN_DIR)/%: $(TEST_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -rf $(BIN_DIR)/*

distclean: clean
	rm -rf $(BUILD_DIR)/*

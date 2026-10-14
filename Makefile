CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -O2 -Iinclude

SRC_CORE = src/lexer.cpp src/parser.cpp src/interpreter.cpp
OBJ_CORE = $(SRC_CORE:.cpp=.o)

.PHONY: all test clean

all: expr_cli

expr_cli: $(OBJ_CORE) src/main.o
	$(CXX) $(CXXFLAGS) $^ -o $@

expr_tests: $(OBJ_CORE) tests/test_parser.o
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: expr_tests
	./expr_tests

clean:
	rm -f $(OBJ_CORE) src/main.o tests/test_parser.o expr_cli expr_tests

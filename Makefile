CXXFLAGS=-g -std=c++17 -pedantic-errors
CXX=g++

PARS_DIR=src/parser/
LEX_DIR=src/lexer/
SRC_DIR=src/
BIN_DIR=bin/

OBJECTS=main.o parser.o lexer.o driver.o

BISONFLAGS=-t -v
FLEXFLAGS=-d

.PHONY: all clean debug

all: $(LEX_DIR)lexer.cc $(PARS_DIR)parser.cc transformator

debug: CXXFLAGS += -DDEBUG
debug: transformator

clean:
	rm bin/*

transformator: mkbin $(OBJECTS)
	cd bin; $(CXX) $(OBJECTS) -o $@;

mkbin:
	mkdir -p bin

parser.o: $(PARS_DIR)parser.cc
	$(CXX) $(CXXFLAGS) $< -c -o $(BIN_DIR)$@

lexer.o: $(LEX_DIR)lexer.cc
	$(CXX) $(CXXFLAGS) $< -c -o $(BIN_DIR)$@

main.o: $(SRC_DIR)main.cc
	$(CXX) $(CXXFLAGS) $< -c -o $(BIN_DIR)$@

driver.o: $(SRC_DIR)Driver.cc
	$(CXX) $(CXXFLAGS) $< -c -o $(BIN_DIR)$@

$(LEX_DIR)lexer.cc: $(LEX_DIR)lexer.l
	flex $(FLEXFLAGS) -o $@ $(LEX_DIR)lexer.l

$(PARS_DIR)parser.cc: $(PARS_DIR)parser.y
	bison $(BISONFLAGS) $(PARS_DIR)parser.y -o $@
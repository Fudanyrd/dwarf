CXX=g++
PWD=$(shell pwd)
CXXFLAGS = -fsanitize=address -g -std=c++11 -MD -O2 -Wall
LDFLAGS = -fsanitize=address -g
INCLUDES=-I$(PWD)

#include src/Makefile
SRC_OBJS = src/lex.o src/utils.o src/dwarf.o
SRC_HEADERS = $(shell find src/ -name '*.h')

OBJS = $(shell find -name '*.o')
# probably output of tlex
CSV = $(shell find -name '*.csv')
PROGS = tokenize parse tlex dw-demo funccopy fntree

%.o: %.cc $(SRC_HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

tokenize: $(SRC_OBJS) tool/tokenizer.o
	$(CXX) $(LDFLAGS) tool/tokenizer.o $(SRC_OBJS) -o tokenize

parse: $(SRC_OBJS) tool/parse.o
	$(CXX) $(LDFLAGS) tool/parse.o $(SRC_OBJS) -o parse 

tlex: $(SRC_OBJS) tool/tlex.o
	$(CXX) $(LDFLAGS) tool/tlex.o $(SRC_OBJS) -o tlex

funcs: $(SRC_OBJS) tool/funcs.o 
	$(CXX) $(LDFLAGS) tool/funcs.o $(SRC_OBJS) -o funcs

fntree: $(SRC_OBJS) tool/fntree.o 
	$(CXX) $(LDFLAGS) tool/fntree.o $(SRC_OBJS) -o fntree

funccopy: $(SRC_OBJS) tool/funccopy.o 
	$(CXX) $(LDFLAGS) tool/funccopy.o $(SRC_OBJS) -o funccopy

dw-demo: $(SRC_OBJS) tool/dw-example.o 
	$(CXX) $(LDFLAGS) tool/dw-example.o $(SRC_OBJS) -o dw-demo

.PHONY: clean
clean:
	-rm -f $(OBJS) $(CSV) tlex *.tar.gz $(PROGS)

.PHONY: archive
archive: clean
	tar -czvf tlex.tar.gz src/ tests/ tool/ Makefile

# use tlex to compile c source code,
# and then compile the generated assembly code
.PHONY: run
run: tlex syscall.o
	./tlex $(SRC) > /dev/null && as -g test.S -o a.o && ld a.o syscall.o

syscall.o: tests/syscall.S
	as -g tests/syscall.S -o syscall.o

all: $(PROGS)

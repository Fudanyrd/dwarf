CXX=g++
PWD=$(shell pwd)
CXXFLAGS = -fsanitize=address -g -std=c++11 -MD -O2 -Wall -I$(PWD)
LDFLAGS = -fsanitize=address -g

#include src/Makefile
SRC_OBJS = src/lex.o src/utils.o src/dwarf.o
SRC_HEADERS = $(shell find src/ -name '*.h')

OBJS = $(shell find -name '*.o')
# probably output of tlex
CSV = $(shell find -name '*.csv')

%.o: %.cc $(SRC_HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

tlex: $(SRC_OBJS) tool/tlex.o
	$(CXX) $(LDFLAGS) tool/tlex.o $(SRC_OBJS) -o tlex

dw-demo: $(SRC_OBJS) tool/dw-example.o 
	$(CXX) $(LDFLAGS) tool/dw-example.o $(SRC_OBJS) -o dw-demo

.PHONY: clean
clean:
	-rm -f $(OBJS) $(CSV) tlex *.tar.gz

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

all: dw-demo tlex

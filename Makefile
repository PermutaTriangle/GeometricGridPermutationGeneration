PLATFORM = linux64
INC      = $(GUROBI_HOME)/include/
CC       = gcc
CPP      = g++
# CARGS    = -m64 -g
CARGS    = -O2
CPPLIB   = -L $(GUROBI_HOME)/lib/ -lgurobi_c++ -lgurobi60

all: grid

grid: geometric_grid_class.cpp
	$(CPP) $(CARGS) -o grid geometric_grid_class.cpp -I$(INC) $(CPPLIB) -lpthread -lm

clean:
	rm -rf *.out


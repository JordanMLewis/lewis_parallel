CXX=g++ -m64
CXXFLAGS= -Iobjs/ -O3  -Wall -Wextra -pedantic -Wfatal-errors -std=c++11
OMPFLAGS=-fopenmp

PROG=lab3
SRC=main.C
OBJ=$(SRC:.C=.o)

prog: $(OBJ)
	$(CXX) $(SRC) -o $(PROG) $(CXXFLAGS) $(OMPFLAGS)  

.C.o: $<
	$(CXX) $(CXXFLAGS) -g -I. -c $<

clean:
	rm *.o $(PROG)

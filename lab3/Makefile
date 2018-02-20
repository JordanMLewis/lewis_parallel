CXX=g++ -m64
CXXFLAGS= -Iobjs/ -O3 -Wall -Wextra -pedantic -Wfatal-errors -std=c++11
OMPFLAGS= -fopenmp

PROG=lab3
SRC=main.C
OBJ=$(SRC:.C=.o)

prog: $(OBJ)
	$(CXX) -o $(PROG) $(OMPFLAGS) $(SRC) $(CXXFLAGS)  

.C.o: $<
	$(CXX) $(CXXFLAGS) $(OMPFLAGS) -g -I. -c $<

clean:
	rm *.o $(PROG)

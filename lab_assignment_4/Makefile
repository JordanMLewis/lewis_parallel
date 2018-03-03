CXX=mpiCC
CXXFLAGS= -Iobjs/ -Wall -Wextra -pedantic -Wfatal-errors -std=c++11

PROG=lab4
SRC=MPI_main.C
OBJ=$(SRC:.C=.o)

prog: $(OBJ)
	$(CXX) -o $(PROG) $(SRC) $(CXXFLAGS)  

.C.o: $<
	$(CXX) $(CXXFLAGS) -g -I. -c $<

clean:
	rm *.o $(PROG)

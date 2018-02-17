CXX=g++ -m64
CXXFLAGS= -Iobjs/ -O3 -Wall -Wextra -pedantic -Wfatal-errors -std=c++11
ISPC=ispc
ISPCFLAGS= -O2 --arch=x86-64 --target=sse2

SRC=main.C image.C sink.C source.C PNMreader.C PNMwriter.C filters.C logging.C
SRC_ISPC=parallelized.ispc

OBJ=$(SRC:.C=.o)
OBJ_ISPC=$(SRC_ISPC:.ispc=.o)

prog: $(OBJ) $(OBJ_ISPC)
	$(CXX) $(CXXFLAGS) $(OBJ) $(OBJ_ISPC) -o parallelized_test

.C.o: $<
	$(CXX) $(CXXFLAGS) -g -I. -c $<

parallelized.h parallelized.o: parallelized.ispc
	$(ISPC) $(ISPCFLAGS) $< -o $(OBJ_ISPC) -h parallelized.h

clean:
	rm *.o parallelized_test

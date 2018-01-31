SRC=main.C image.C sink.C source.C PNMreader.C PNMwriter.C filters.C logging.C 
OBJ=$(SRC:.C=.o)

prog: $(OBJ)
	g++ $(OBJ) -o serialized_test

.C.o: $<
	g++  -g -I. -c $<

clean:
	rm *.o serialized_test

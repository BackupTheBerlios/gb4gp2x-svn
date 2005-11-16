OBJ=		objects
SRC=		src
BIN=		bin
HEADER=		include
CCFLAGS=	-Wall -O2 -g -I $(HEADER)

default:
	make $(BIN)/gb4gb2x
	
$(BIN)/gb4gb2x: $(OBJ)/proc.o $(SRC)/boot.c
	gcc $(CCFLAGS) -o $@ $^
	
$(OBJ)/proc.o: $(SRC)/proc.c
	gcc $(CCFLAGS) -c -o $@ $^

clean:
	rm -f $(OBJ)/*o
	rm -f $(BIN)/*

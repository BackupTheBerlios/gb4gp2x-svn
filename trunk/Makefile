OBJ=		objects
SRC=		src
BIN=		bin
HEADER=		include
CCFLAGS=	-Wall -O2 -g -I $(HEADER)

default:
	make $(BIN)/gb4gp2x
	
$(BIN)/gb4gp2x: $(OBJ)/proc.o $(SRC)/boot.c
	gcc $(CCFLAGS) -o $@ $^
	
$(OBJ)/proc.o: $(SRC)/proc.c
	gcc $(CCFLAGS) -c -o $@ $^

install: $(BIN)/gb4gp2x
	ln -sf ./$(BIN)/gb4gp2x ./gb

clean:
	rm -f $(OBJ)/*o
	rm -f $(BIN)/*

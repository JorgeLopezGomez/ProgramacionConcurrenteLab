DIROBJ := obj/
DIREXE := exec/
DIRHEA := include/
DIRSRC := src/

CFLAGS := -I$(DIRHEA) -c -Wall -ggdb
LDLIBS := -lpthread -lrt
CC := gcc

all : dirs manager procesador contador test

dirs:
	mkdir -p $(DIROBJ) $(DIREXE)

manager: $(DIROBJ)manager.o $(DIROBJ)lista.o 
	$(CC) -o $(DIREXE)$@ $^ $(LDLIBS)

procesador: $(DIROBJ)procesador.o 
	$(CC) -o $(DIREXE)$@ $^ $(LDLIBS)

contador: $(DIROBJ)contador.o 
	$(CC) -o $(DIREXE)$@ $^ $(LDLIBS)

$(DIROBJ)%.o: $(DIRSRC)%.c
	$(CC) $(CFLAGS) $^ -o $@

test:
	./$(DIREXE)manager data/solution.txt data/patrones.txt

clean : 
	rm -rf *~ core $(DIROBJ) $(DIREXE) $(DIRHEA)*~ $(DIRSRC)*~

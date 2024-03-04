DIROBJ := obj/
DIREXE := exec/
DIRHEA := include/
DIRSRC := src/

CFLAGS := -I$(DIRHEA) -c -Wall -ggdb
LDFLAGS := 
CC := gcc

all : dirs test_lista

dirs:
	mkdir -p $(DIROBJ) $(DIREXE)

test_lista: $(DIROBJ)test_lista.o $(DIROBJ)lista.o 
	$(CC) $(LDFLAGS) -o $(DIREXE)$@ $^

$(DIROBJ)%.o: $(DIRSRC)%.c
	$(CC) $(CFLAGS) $^ -o $@

clean : 
	rm -rf *~ core $(DIROBJ) $(DIREXE) $(DIRHEA)*~ $(DIRSRC)*~
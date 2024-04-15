DIROBJ := obj/
DIREXE := exec/
DIRHEA := include/
DIRSRC := src/

CFLAGS := -I$(DIRHEA) -c -Wall -ggdb
LDFLAGS := -lpthread -lrt
CC := gcc

all : dirs manager pista slot

dirs:
	mkdir -p $(DIROBJ) $(DIREXE)

manager: $(DIROBJ)manager.o 
	$(CC) -o $(DIREXE)$@ $^ $(LDFLAGS)

pista: $(DIROBJ)pista.o 
	$(CC) -o $(DIREXE)$@ $^ $(LDFLAGS)

slot: $(DIROBJ)slot.o 
	$(CC) -o $(DIREXE)$@ $^ $(LDFLAGS)


$(DIROBJ)%.o: $(DIRSRC)%.c
	$(CC) $(CFLAGS) $^ -o $@

clean : 
	rm -rf *~ core $(DIROBJ) $(DIREXE) $(DIRHEA)*~ $(DIRSRC)*~

DIROBJ := obj/
DIREXE := exec/
DIRHEA := include/
DIRSRC := src/

CFLAGS := -I$(DIRHEA) -c -Wall -ggdb
LDFLAGS := -lpthread -lrt
CC := gcc

all : dirs manager pista avion

dirs:
	mkdir -p $(DIROBJ) $(DIREXE)

manager: $(DIROBJ)manager.o $(DIROBJ)semaforoI.o $(DIROBJ)memoriaI.o 
	$(CC) -o $(DIREXE)$@ $^ $(LDFLAGS)

pista: $(DIROBJ)pista.o $(DIROBJ)semaforoI.o $(DIROBJ)memoriaI.o
	$(CC) -o $(DIREXE)$@ $^ $(LDFLAGS)

avion: $(DIROBJ)avion.o $(DIROBJ)semaforoI.o $(DIROBJ)memoriaI.o
	$(CC) -o $(DIREXE)$@ $^ $(LDFLAGS)


$(DIROBJ)%.o: $(DIRSRC)%.c
	$(CC) $(CFLAGS) $^ -o $@

clean : 
	rm -rf *~ core $(DIROBJ) $(DIREXE) $(DIRHEA)*~ $(DIRSRC)*~

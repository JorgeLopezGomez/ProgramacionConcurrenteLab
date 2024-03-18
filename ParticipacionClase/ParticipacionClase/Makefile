DIROBJ := obj/
DIREXE := exec/
DIRHEA := include/
DIRSRC := src/

CFLAGS := -I$(DIRHEA) -c -Wall -ggdb
LDLIBS := -lpthread -lrt
CC := gcc

all: dirs gastones_ahorradores test

dirs:
	mkdir -p $(DIROBJ) $(DIREXE)

gastones_ahorradores: $(DIROBJ)gastones_ahorradores.o 
	$(CC) -o $(DIREXE)$@ $^ $(LDLIBS)

$(DIROBJ)%.o: $(DIRSRC)%.c
	$(CC) $(CFLAGS) $^ -o $@

test:
	./$(DIREXE)gastones_ahorradores 

clean : 
	rm -rf *~ core $(DIROBJ) $(DIREXE) $(DIRHEA)*~ $(DIRSRC)*~
DIROBJ := obj/
DIREXE := exec/
DIRHEA := include/
DIRSRC := src/

CFLAGS := -I$(DIRHEA) -c -Wall -ggdb
LDLIBS := -lpthread -lrt
CC := gcc

all : dirs patronFachada

dirs:
    mkdir -p $(DIROBJ) $(DIREXE)

patronFachada: $(DIROBJ)patronFachada.o
    $(CC) -o $(DIREXE)$@ $^ $(LDLIBS)

$(DIROBJ)%.o: $(DIRSRC)%.c
    $(CC) $(CFLAGS) $^ -o $@

clean : 
    rm -rf *~ core $(DIROBJ) $(DIREXE) $(DIRHEA)*~ $(DIRSRC)*~
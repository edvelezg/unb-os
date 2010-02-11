#Tools - definitions of toolchain setup.
CC=C:/usr/bin/m6811-elf-gcc
CFLAGS=-g -Os -mshort -Wl,-m,m68hc11elfb -I. #old: CFLAGS+=-fomit-frame-pointer
LDFLAGS=-g -Os -mshort -Wl,-m,m68hc11elfb -Wl


#OBJCOPYFLAGS=-S --only-section=.text --only-section=.rodata --only-section=.vectors --only-section=.data
OBJDUMP=hc12-elf-objdump

#Files - what we are trying to build.
LDSCRIPT=memory.x
#Leave the .c off of SRC
SRC=main
OBJ=$(SRC).o os.o
ELF=$(SRC).elf
S19=$(SRC).s19

# Pattern rules
%.o : %.c
	$(OBJ)
#Rules
all :: $(ELF)

#Objcopy - translate ELF into S19 file.
#$(S19) : $(ELF)
#        $(OBJCOPY) $(OBJCOPYFLAGS) $(ELF) -O srec $(S19)

#Link - combine object files into an ELF.
$(ELF) : $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $(ELF)

#Compile - compile C files into object files.
$(OBJ): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

#Delete temporary/generated files
clean ::
	rm -f $(OBJ) $(ELF) $(S19)

#Disassemble compiled/linked source file.
disassemble :: $(ELF)
	$(OBJDUMP) -D $(ELF)

#Dependencies
#firsttest.o: firsttest.c

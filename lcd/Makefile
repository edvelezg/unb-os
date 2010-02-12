# Make command to use for dependencies
MAKE=make
RM=rm
MKDIR=mkdir

# If no configuration is specified, "Debug" will be used
ifndef CFG
CFG=Debug
endif

#
# Configuration: Debug
#
ifeq "$(CFG)" "Debug"
OUTFILE=main.elf

COMPILE=C:/usr/bin/m6811-elf-gcc -g -Os -mshort -Wl,-m,m68hc11elfb -o $(OUTFILE) main.c

# Pattern rules
#$(OUTDIR)/%.o : %.c
#        $(COMPILE)

# Build rules
all: $(OUTFILE)

$(OUTFILE):
	$(COMPILE)

# Rebuild this project
rebuild: cleanall all

# Clean this project
clean:
	$(RM) -f $(OUTFILE)
	$(RM) -f $(OBJ)

# Clean this project and all dependencies
cleanall: clean
endif

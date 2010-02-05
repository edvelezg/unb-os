/usr/bin/m6811-elf-gcc -g -Os -mshort -Wl,-m,m68hc11elfb -o main.elf main.c os.c
/usr/bin/m6811-elf-run --cpu-config=0xe main.elf

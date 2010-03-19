/usr/bin/m6811-elf-gcc -g -Os -mshort -msoft-reg-count=0 -Wl,-m,m68hc11elfb -o main.elf main.c os.c lcd.c
/usr/bin/m6811-elf-objcopy --only-section=.text --only-section=.rodata --only-section=.vectors --only-section=.data --output-target=srec main.elf main.s19

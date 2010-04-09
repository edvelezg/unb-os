Assignment #1: README file

To compile the Operating System:
Run the build.bat file. Make sure you have the m6811 (** m6811-elf-gcc compiler)
tools installed or if installed in path other than /usr/bin, change accordingly.

To Test the Operating Sytem:
Run the test.bat file and edit the main function accordingly to determine
the order and the processes you want to run.

The operating system runs too fast as is, so it might be more helpful to
run it slowly. For that, run the debug.bat file, and type the following 
commands: (** must have m6811-elf-gdb debugger)
gdb)target sim
gdb)load
gdb)b OS_Start
gdb)b 
gdb)r
gdb)b context_switch
gdb)continue
gdb)c

Use the continue command to view the process that runs after a context switch.

Basic Testing is included in the main.c file, where testing should be done.
Basic tests include:
Running with/without Sporadic Processes.
Terminating Processes.
Using Yield command from Processes.
Running with Device Processes.

To compile the Operating system:
Run the build.bat file. Make sure you have the m6811 tools installed or if installed
in path other than /usr/bin, change accordingly.

To Test the Operating Sytem:
Run the test.bat file and edit the main function accordingly to determine
the order and the processes you want to run.
The operating system runs too fast as is, so it is easier to run it slowly.
For that, run the debug.bat file, and type the following commands:
gdb)target sim
gdb)load

Set a breakpoints for the context_switch function in the os.c file, and 
execute:
gdb) run.

and 

gdb) 'continue'
To view the next context switch.

Basic Tests:
Run with no sporadic processes
Run with no periodic processes
Run with device process.

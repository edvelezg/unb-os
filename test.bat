/usr/bin/m6811-elf-run --cpu-config=0xe main.elf

echo off
IF EXIST main.elf (
echo.
rem echo === Creating S19 file...
rem c:\usr\bin\m6811-elf-objcopy --only-section=.text --only-section=.rodata --only-section=.vectors --only-section=.data --output-target=srec os.elf os.s19
echo === Running main.elf...
c:\usr\bin\m6811-elf-run --cpu-config=0xe main.elf
echo.
) ELSE (
echo.
echo === test failed.
)
pause

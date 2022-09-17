# Picture to Braille Character
A simple program that converts bitmap to Braille characters.

## Build with
```
windres -i PTB.rc -o PTB.o
gcc -mwindows -w -o PTB PTB.o Cleanup.c Convert.c Ctrl.c Main.c Winmain.c -lKernel32 -lGdi32 -lComdlg32 -lComCtl32
```

@echo off
gfx2gba -q -fsrc -t8 -c16 window.bmp
ren *.pal.c window.pal.c
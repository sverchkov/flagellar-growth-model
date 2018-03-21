#File to make the C IFT simulation.

run: launcher.c ift.c ydarrays.c
	gcc -O3 -fno-strict-aliasing -ansi -Wall -lm -msse2 -DHAVE_SSE2 -DMEXP=216091 -o run launcher.c

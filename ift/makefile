#File to make the C IFT simulation.

run-threaded: launcher-threaded.c ift-threaded.c ydarrays.c
	gcc -O3 -fno-strict-aliasing -ansi -Wall -lm -msse2 -DHAVE_SSE2 -DMEXP=216091 -pthread -o run-threaded ../SFMT-src-1.3/SFMT.c launcher-threaded.c

run: launcher.c ift.c ydarrays.c
	gcc -O3 -fno-strict-aliasing -ansi -Wall -lm -msse2 -DHAVE_SSE2 -DMEXP=216091 -o run ../SFMT-src-1.3/SFMT.c launcher.c

test1: testrng.c
	gcc -ansi -Wall -lm -msse2 -DHAVE_SSE2 -DMEXP=216091 -o test1 SFMT-src-1.3/SFMT.c testrng.c

test2: testrng2.c
	gcc -ansi -Wall -lm -msse2 -DHAVE_SSE2 -DMEXP=216091 -o test2 testrng2.c

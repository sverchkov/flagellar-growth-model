This folder contains initial conditions files and the tools to generate them.

write_IFT_init.m
 - Write arbitrary initial conditions to a file.
write_uniform_IFT_init.m
 - Write initial conditions with arbitrary length and an arbitrary number of
 IFTs at uniformly distributed positions.
write_uniform_noverlap_IFT_init.m
 - Write initial conditions with arbitrary length and an arbitrary number of
 IFTs at uniformly distributed distinct positions.

Initial condition files:

File Name,   L,    M, IFT Distribution

ic1.dat    1000    10   U
ic2.dat     100    10   Ud
ic3.dat     400    10   U
ic4.dat     700    10   U
ic5.dat    1300    10   U
ic6.dat    1600    10   U
ic7.dat    1900    10   Ud
ic8.dat    1300    10   Ud
ic9.dat    1300    20   U D
ic10.dat   1300   100   U D
ic11.dat   1000    10   U D
ic12.dat    700     1   U
ic13.dat    700    20   U
ic14.dat    700   100   U

IFT distribution key:
U = Uniform
d = Distinct, but not by design.
D = Generated to be distinct


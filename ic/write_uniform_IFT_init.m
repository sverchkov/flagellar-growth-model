function [ ] = write_uniform_IFT_init( filename, M, L )
%write_uniform_IFT_init( filename, M, L )
% Generates initial conditions for the ift simulation with M
% uniformly distributed IFTs on a flagellum of an initial length L
write_IFT_init(filename, L, randint(1,M,[-L, L]) );
function [ ] = write_uniform_noverlap_IFT_init( filename, M, L )
%write_uniform_noverlap_IFT_init( filename, M, L )
% Generates initial conditions for the ift simulation with M
% distinct uniformly distributed IFTs on a flagellum of an initial
% length L
X = randperm(2*L+1) - L - 1;
write_IFT_init(filename, L, X(1:M) );
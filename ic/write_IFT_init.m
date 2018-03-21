function [ ] = write_IFT_init( filename, L, x0 )
%function [ ] = write_IFT_init( filename, L, x0 )
% Writes an Initial Conditions (binary) file for the IFT C
% simulation (L is the 
%

file = fopen( filename, 'w' );

fwrite( file, L, 'int32' );
fwrite( file, length(x0), 'int32' );
fwrite( file, x0, 'int32' );

fclose( file );
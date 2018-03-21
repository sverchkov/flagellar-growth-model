/* Author: Yuriy Sverchkov
   Filename: launcher-threaded.c
   See usage message for description. 
*/

#include "string.h"
#include "ift-threaded.c"

void print_usage( const char * const name ){
	printf(
"Usage: %s -a|b parameters -a|b input time -a|b output [runs backup]\n\n\
Where 'parameters' is the name of the file containing the simulation\n\
parameters, 'input' is the name of the file containing the initial\n\
conditions, 'time' is the simulation time limit (in seconds), 'output' is\n\
the name of the file to output the results to, and 'runs' is the number\n\
of times to run the simulation.\n\n\
The simulation will run in 'ensemble' mode if 'runs' is specified, and in\n\
'trajectory' mode if it is not.\n\
If 'runs' is specified, then so must 'backup' be specified - a file to store\n\
temporary results (those will be stored in binary format).\n\n\
-a \tspecifies that the file that follows is an ascii file.\n\
-b \tspecified that the file that follows is a binary file.\n"
, name );
	return;
}

int main( int argc, char* argv[]){

   /*Stores simulation parameters*/
   Parameters p;

   /*Variables to store simulation output*/
   DoubleArray t_array;
   IntArray l_array;

   /*Variables to represent simulation input*/
   InitialConditions ic;
   unsigned n_runs;

   /*Other Variables*/
   unsigned i;
   short output_ascii;
   float f_lambda_p, f_lambda_m, f_mu;
   FILE * inparameters;
   FILE * infile;
   FILE * outfile;
	
   printf("IFT Simulation 0.1\n");

   /* Error checking for the impossible */
   if( argc <= 0 ) return 1;

   /* Error checking for incorrect call */
   if( argc < 8 ) {
      print_usage( argv[0] );
      return 1;
   }

   /* Opening files */
   if( ( inparameters = fopen( argv[2], "r" ) ) == NULL ){
      printf( "Cannot open %s.\n", argv[2] );
      return 1;
   }

   if( ( infile = fopen( argv[4], "r" ) ) == NULL ){
      printf( "Cannot open %s.\n", argv[4] );
      fclose( inparameters );
      return 1;
   }

   if( ( outfile = fopen( argv[7], "w" ) ) == NULL ){
      printf( "Cannot open %s.\n", argv[7] );
      fclose( inparameters );
      fclose( infile );
      return 1;
   }

   /* Get parameters and initial conditions */

   if( strcmp( argv[1], "-a" ) == 0 ){

      /* fscanf wants floats... */	
      fscanf( inparameters, "%g", &f_lambda_p );
      fscanf( inparameters, "%g", &f_lambda_m );
      fscanf( inparameters, "%g", &f_mu );
      p.lambda_p = f_lambda_p;
      p.lambda_m = f_lambda_m;
      p.mu = f_mu;
	
   }else if( strcmp( argv[1], "-b" ) == 0 ){
		
      fread( &(p.lambda_p), sizeof(double), 1, inparameters );
      fread( &(p.lambda_m), sizeof(double), 1, inparameters );
      fread( &(p.mu), sizeof(double), 1, inparameters );

   }else{
	
      print_usage( argv[0] );
      return 1;
   }

   if( strcmp( argv[3], "-a" ) == 0 ){

      fscanf( infile, "%d", &(ic.length0) );
      fscanf( infile, "%d", &(ic.n_ifts) );

      ic.x0 = (int *) malloc( ic.n_ifts * sizeof(int) );

      for( i = 0; i < ic.n_ifts; i++ )
         fscanf( infile, "%d", &(ic.x0[i]) );

   }else if( strcmp( argv[3], "-b" ) == 0 ){

      fread( &(ic.length0), sizeof(int), 1, infile );
      fread( &(ic.n_ifts), sizeof(int), 1, infile );

      ic.x0 = (int *) malloc( ic.n_ifts * sizeof(int) );

      fread( ic.x0, sizeof(int), ic.n_ifts, infile );
	
   }else{
	
      print_usage( argv[0] );
      return 1;
   }

   ic.time_limit = strtod( argv[5], NULL );

   /* Determine output format */
   if( strcmp( argv[6], "-a" ) == 0 ){
      output_ascii = 1;
   }else if( strcmp( argv[6], "-b" ) == 0 ){
      output_ascii = 0;
   }else{
      print_usage( argv[0] );
      return 1;
   }
	
   /* Close input files */
   fclose( inparameters );
   fclose( infile );

   /* Print the rates and initial conditions */
   printf("\nRates:  lambda+ = %g  lambda- = %g  mu = %g\n",
      p.lambda_p, p.lambda_m, p.mu );
   printf("\nInitial Conditions:\n");
   printf("\n-Initial Length: %d", ic.length0);
   printf("\n-Initial Positions:");
   for( i=0; i < ic.n_ifts; i++ ) printf(" %d",ic.x0[i]);
   printf("\n-Time Limit: %f\n", ic.time_limit);

   /* For running in trajectory mode */
   if( argc == 8 ){

      ift_trajectory( &p, &ic, &t_array, &l_array );

      /* Write to output file */
      if( output_ascii )
		
         for( i = 0; i < t_array.length && i < l_array.length; i++ )
            fprintf( outfile, "%25.15e %10d\n",
               daGet( t_array, i ), iaGet( l_array, i ) );
		
      else{
			
         fwrite( &t_array.length, sizeof(unsigned int), 1, outfile);
         fwrite( t_array.contents, sizeof(double), t_array.length, outfile);
         fwrite( &l_array.length, sizeof(unsigned int), 1, outfile);
         fwrite( l_array.contents, sizeof(int), l_array.length, outfile);
      }

      iaDestroy( l_array );
      daDestroy( t_array );
      return 0;
   }

   /* For running in ensemble mode */
   if( argc == 10 ){

      n_runs = atoi( argv[8] );

      l_array = iaCreate( NULL, n_runs );

      ift_ensemble( &p, &ic, n_runs, l_array.contents, argv[9] );

      /* Write to output file */
      if( output_ascii )
	
         for( i = 0; i < l_array.length; i++ )
            fprintf( outfile, "%10d\n", iaGet( l_array, i ) );
		
      else{

         fwrite( &l_array.length, sizeof(unsigned int), 1, outfile);
         fwrite( l_array.contents, sizeof(int), l_array.length, outfile);
      }
	
      iaDestroy( l_array );
      return 0;
   }

   print_usage(argv[0]);
   return 0;
}

/* Author: Yuriy Sverchkov
   Filename: ift.c
   Purpose: Simulate the growth of a flagellum as a stochastic process.
   Original simulation algorithm (written in MATLAB) by Dr. Muruhan Rathinam

   *** This version has been modified to take crowding into account.
*/

#ifndef IFT_C_INCLUDED
#define IFT_C_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include "SFMT-src-1.3/SFMT.c"
#include "ydarrays.c"


typedef struct{
   double lambda_p;
   double lambda_m;
   double mu;
} Parameters;
/*Parameters: Stores simulation parameters

lambda_p - the rate and IFT of moving anterograde (towards tip).
lambda_m - the rate of an IFT moving retrograde.
mu - the rate of dissassembly.
*/

typedef struct{
   double time_limit;
   int length0;
   unsigned n_ifts;
   int * x0;
} InitialConditions;
/*InitialConditions: Stores the initial conditions for the simulation

n_ifts - Number of IFT's.

x0 - Array of IFT positions, where positions are numbered -length to +length,
   with positive positions representing anterograde (towards tip) transport,
   and negative positions represent retrograde transport with the base of the
   flagellum at 0 and the tip at +/-length.
   *** Note: The array must be sorted in increasing order by position for
      crowding to be properly handled by the program.

length0 - initial length of flagellum.

time_limit - the time limit for the simulation.
*/


void seed()
/* Seeds the random number generator */
{
/*	unsigned int r[2];
	r[0] = time(NULL);
	r[1] = getpid();
	init_by_array( r, 2 );*/
	init_gen_rand( time(NULL) );
}


int ift_step( const Parameters * const p,
   const double time_limit, double * t, int * length,
   int x[], const unsigned n_ifts )
/*int ift_step( const Parameters * const p,
   const double time_limit, double * t, int * length,
   int x[], const unsigned n_ifts )
Represents a single step of the simulation.

Inputs:
p - (see comment on the Parameters struct)
time_limit - the time limit for the simulation.
n_ifts - the number of IFT's.

Changing (input and output) variables:
t - the current time.
length - the current flagellum length.
x - array of IFT (directed) positions (same notation as x0 in the
   InitialConditions struct).

Return value:
The change in length (+1, 0, or -1)
*/
{
   int i, j, k; double temp, temp2; /* generic variables */
   double tau = 0; /* change in time */
   double rate_sum = 0;
   double rates[n_ifts];
   short length_change = 0;


   /* Rate Calculations */

   for( i = 0; i < n_ifts; i++ )
      /* Get x[i]'s rate */
      rate_sum += ( rates[i] =
         p->lambda_p * ( x[i] >= 0 ) + p->lambda_m * ( x[i] < 0 ) );

   /* Add disassembly rate */
   rate_sum += ( rates[n_ifts] = p->mu * ( *length > 0 ) );

   if( rate_sum <= 0 ){ /* Impossible */
      tau = time_limit - *t;
      printf( "The sum of rates was nonpositive (%g).", rate_sum );
   }else{
      /* Get time until next event */
      tau = ( 1 / rate_sum ) * log( 1.0 / genrand_real2() );

      /* Check if next event is within the time limit */
      if( tau + *t > time_limit )
         tau = time_limit - *t;

      else{ /* Pick next event */

         temp = rate_sum * genrand_real2();

         temp2 = 0;
         j = 0;
         while( ( ( temp2 += rates[j] ) <= temp ) && ( j++ < n_ifts ) );
         /* j is now the event number */

         if( j == n_ifts ){ /* Disassembly */

            /* Decrease Length. */
            *length += ( length_change = -1 );

            /* Move IFTs on Disassembled segment down, possibly
               consequentially pushing adjacent IFTs as well. */
            for( i = 0; i < n_ifts; i++ ){

               k = i;
               while( x[k] == (*length)+1+((i-k)%n_ifts) ){
                  --x[k];
                  k = ( k - 1 ) % n_ifts;
               }

               k = i;
               while( x[j] == -(*length)-1-((i+k)%n_ifts) ){
                  ++x[k];
                  k = ( k + 1 ) % n_ifts;
               }
            }

         }else /* Move IFT */

            if( x[j] > *length - 1 ){ /* Then assembly occurs */

               /* Increase length. */
               *length += ( length_change = +1 );
               /* Change direction on IFT */
               x[j] = -(*length);

            }else /* Move IFT only if it isn't blocked */
               if( x[(j+1)%n_ifts] != x[j] + 1 ) ++x[j];
        }
   }

   /* Update time */
   *t += tau;

   return length_change;
}

void ift_trajectory( const Parameters * const p, const InitialConditions * const ic,
	DoubleArray * t_array, IntArray * l_array)
/*void ift_trajectory( const Parameters * const p, const InitialConditions * const ic,
	DoubleArray * t_array, IntArray * l_array)

Runs the IFT simulation once, recording the length and time at every length change.
 
Input:
p - The transport and disassembly rates (see comment on Parameters struct)
ic - Simulation initial conditions (see comment on InitialConditions struct)

Output parameters:
t_array - Array of times at which the length changes.
l_array - Array of lengths corresponding to the times in t_array.

*/
{
	/*Generic counter*/
	unsigned int i;

	unsigned int n_l_changes = 0; /*Length change counter*/
	int length = ic->length0; /*Current flagellum length*/
	int x[ic->n_ifts]; /*IFT positions*/
	double t = 0; /*Current time*/
	DoubleArray times; /*Records length change times*/
	IntArray lengths; /*Records lengths*/

	/*** Initialization ***/

	/* Random number generator seed */
/*	srand( time( NULL ) );
*/	seed();

	/* Sets Initial positions of IFT's. */
	for( i = 0; i < ic->n_ifts ; i++ ) x[i] = ic->x0[i];

	/* Sets "Step 0" times and lengths */
	times = daCreate( NULL, 0 );
	lengths = iaCreate( NULL, 0 );
	daSet( &times, 0, 0 );
	iaSet( &lengths, 0, ic->length0 );

	/*** Main Loop ***/
	while( t < ic->time_limit )
		if(
			ift_step( p, ic->time_limit, &t, &length, x, ic->n_ifts ) != 0
			|| t == ic->time_limit )
		{
			++n_l_changes;
			daSet( &times, n_l_changes, t );
			iaSet( &lengths, n_l_changes, length );
		}

	*t_array = times;
	*l_array = lengths;

	printf("\nFinished.\n");
	return;
}



void ift_ensemble( const Parameters * const p, const InitialConditions * const ic,
	const unsigned int n_runs, int l_array[], char * backup)
/*void ift_ensemble( const Parameters * const p, const InitialConditions * const ic,
	const unsigned int n_runs, int l_array[])

Runs the IFT simulation repeatedly, recording only the lengths at time_limit for each run.

For backup:
backup - filename of backup file.

Input:
p - The transport and disassembly rates (see comment on Parameters struct)
ic - Simulation initial conditions (see comment on InitialConditions struct)
n_runs - the number of times to run the simulation.

Output:
l_array - Array of lengths corresponding to the different runs.

*/
{
        FILE * out;
	unsigned int i,j;

	int length = ic->length0; /*Current flagellum length*/
	int x[ic->n_ifts]; /*IFT positions*/
	double t; /*Current time*/

	/*** Initialization ***/

	/* Random number generator seed */
/*	srand( time( NULL ) );
*/	seed();

	/*** Main Loop ***/
	for( i = 0; i < n_runs; ){
		printf("run %d\n",i);

		/* Sets Initial positions of IFT's. */
		for( j = 0; j < ic->n_ifts; j++ ) x[j] = ic->x0[j];

		t = 0;
		while( t < ic->time_limit )
			ift_step( p, ic->time_limit, &t, &length, x, ic->n_ifts );

		l_array[i] = length;

		++i;

		/* Write everything read so far (fallback) */
                if( i%10 == 0 && (out = fopen( backup, "w" )) != NULL ){
			fwrite( &i, sizeof(unsigned int), 1, out);
			fwrite( l_array, sizeof(int), i, out);
                        fclose( out );
                }
	}

	printf("\nFinished.\n");
	return;
}

#endif

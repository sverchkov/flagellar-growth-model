/* Author: Yuriy Sverchkov
   Filename: ift_threaded.c
   Purpose: Simulate the growth of a flagellum as a stochastic process.
   This version uses pthread to run faster.
   Original simulation algorithm (written in MATLAB) by Dr. Muruhan Rathinam
*/

#ifndef IFT_C_INCLUDED
#define IFT_C_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include "SFMT-src-1.3/SFMT.h"
#include "ydarrays.c"

/* For how many threads? */
#define N_THREADS 8

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
x0 - Array of IFT positions, where positions are numbered -length to +length, with positive positions
	representing anterograde (towards tip) transport, and negative positions represent retrograde
	transport with the base of the flagellum at 0 and the tip at +/-length.
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
x - array of IFT (directed) positions (same notation as x0 in the InitialConditions struct).

Return value:
The change in length (+1, 0, or -1)
*/
{

	int i, j; double temp, temp2;
	double tau = 0;
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
/*		tau = ( 1 / rate_sum ) * log( ((double)RAND_MAX + 1.0) / (double)rand() );
*/		tau = ( 1 / rate_sum ) * log( 1.0 / genrand_real2() );

		/* Check if next event is within the time limit */
		if( tau + *t > time_limit )
			tau = time_limit - *t;
		else{
			/* Pick next event */

			temp = rate_sum * genrand_real2();

			temp2 = 0;
			j = 0;
			while( ( ( temp2 += rates[j] ) <= temp ) && ( j++ < n_ifts ) );
			/* j is now the event number */

			if( j == n_ifts ){ /* Disassembly */

				/* Decrease Length. */
				*length += ( length_change = -1 );

				/* Move IFTs on Disassembled segment down. */
				for( i = 0; i < n_ifts; i++ )
					x[i] += -(x[i] == (*length)+1) + (x[i] == -(*length)-1);

			}else /* Move IFT */

				if( ++x[j] > *length ){ /* Then assembly occurs */

					/* Increase length. */
					*length += ( length_change = +1 );
					/* Change direction on IFT */
					x[j] = -(*length);
				}
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



/* Thread-related:
 * RunArgs - struct that stores arguments passed to and from the thread
 * function.
 * one_run - function that makes one run (for the pthread).
 */
typedef struct{
	Parameters p;
	InitialConditions ic;
	int * result; /* The result goes here */
}RunArgs;

void * one_run( void * argptr ){

	int j;
	RunArgs * args = argptr;
	/* Initial conditions */
	double t = 0; /* time */
	int x[args->ic.n_ifts]; /* IFT positions */

	   /* Sets Initial positions of IFT's and lengths. */
	   for( j = 0; j < args->ic.n_ifts; j++ )
		   x[j] = args->ic.x0[j];
	   *(args->result) = args->ic.length0;

	   while( t < args->ic.time_limit )
	      ift_step( &(args->p), args->ic.time_limit, &t, args->result, x,
		args->ic.n_ifts );
	   free( argptr );
	return NULL;
}


void ift_ensemble(
   const Parameters * const p,
   const InitialConditions * const ic,
   const unsigned int n_runs,
   int l_array[],
   const char * backup)
/*void ift_ensemble( const Parameters * const p, const InitialConditions * const ic,
	const unsigned int n_runs, int l_array[])

Runs the IFT simulation repeatedly, recording only the lengths at time_limit for each run.

backud - filename of backup file.
 
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

	/*int length; **Current flagellum length**
	double t; **Current time*/

	RunArgs thread_args, *argsptr;
	pthread_t threads[N_THREADS];

	/*** Initialization ***/

	/* Random number generator seed */
	seed();

	/* Initial conditions for all threads */
	thread_args.p = *p; thread_args.ic = *ic; thread_args.result=NULL;

	/*** Main Loop ***/
	for( i = 0; i < n_runs; i += N_THREADS ){

	   /* Sets Initial positions of IFT's and lengths. **
	   for( j = 0; j < ic->n_ifts; j++ ) x[j] = ic->x0[j];
	   length = ic->length0;

	   t = 0;
	   while( t < ic->time_limit )
	      ift_step( p, ic->time_limit, &t, &length, x, ic->n_ifts );

	   l_array[i] = length;
	   */

	   for( j=0; j<N_THREADS && i+j < n_runs; j++ ){

		argsptr = malloc( sizeof( RunArgs ) );
		/*Need to remember to deallocate inside thread*/
		*argsptr = thread_args;
		argsptr->result = &( l_array[i+j] );
		pthread_create(
			&( threads[j] ), NULL, one_run, argsptr );

	   }

	   /* Write backup */
	   if( i>0 && i%10 == 0 && (out = fopen( backup, "w" )) != NULL ){

		fwrite( &i, sizeof(unsigned int), 1, out);
		fwrite( l_array, sizeof(int), i, out);
		fclose( out );
	   }

	   /* Wait for threads */
	   for( j=0; j<N_THREADS && i+j < n_runs; j++ ){
	   	pthread_join( threads[j], NULL );
	   }

	}

	printf("\nFinished.\n");
	return;
}

#endif

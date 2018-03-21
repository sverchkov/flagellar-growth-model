#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "ydarrays.c"

void ift_simulation(
	const unsigned int n_ifts, const int const x0[], const int length0, const double time_limit,
	unsigned * output_size, double t_array[], int l_array[] )
/*void ift_simulation(
	const unsigned int n_x0s, const int const x0[], const unsigned int length0, const double time_limit,
	unsigned * output_size, double t_array[], int l_array[] )

Runs the IFT simulation.
 
Input parameters:
n_ifts - Number of IFT's.
x0 - Array of IFT positions, where positions are numbered -length to +length, with positive positions
	representing anterograde (towards tip) transport, and negative positions represent retrograde
	transport with the base of the flagellum at 0 and the tip at +/-length.
length0 - initial length of flagellum.
time_limit - the time limit for the simulation.

Output parameters:
output_size - Length of t_array and l_array.
t_array[] - Array of times at which the length changes.
l_array - Array of lengths corresponding to the times in t_array.

*/
{
	/*Predetermined Constants*/
	const double lambda_p = 200;
	const double lambda_m = 350;
	const double mu = 1;

	/*Generic dummy variables*/
	unsigned int i;
	unsigned int j;
	double temp;
	double temp2;

	unsigned int n_l_changes = 0; /*Length change counter*/
	short length_change; /*Length change*/
	int length = length0; /*Current flagellum length*/
	double rates[n_ifts+1]; /*Vector of rates*/
	double rate_sum; /*Sum of rates*/
	int x[n_ifts]; /*IFT positions*/
	double t = 0; /*Current time*/
	double tau; /*Time change*/
	DoubleArray times; /*Records length change times*/
	IntArray lengths; /*Records lengths*/

	printf("Initial Parameters:\n");
	printf("\n-Initial Length: %d", length0);
	printf("\n-Initial Positions:");
	for( i=0; i < n_ifts; i++ ) printf(" %d",x0[i]);
	printf("\n-Time Limit: %f\n", time_limit);

	/*** Initialization ***/

	/* Random number generator seed */
	srand( time( NULL ) );

	/* Sets Initial positions of IFT's. */
	for( i = 0; i < n_ifts ; i++ ) x[i] = x0[i];

	/* Sets "Step 0" times and lengths */
	times = daCreate( NULL, 0 );
	lengths = iaCreate( NULL, 0 );
	daSet( &times, 0, 0 );
	iaSet( &lengths, 0, length0 );

	/*** Main Loop ***/
	while( t < time_limit ){
		/* Reset length change */
		length_change = 0;

		/* Rate Calculations */
		rate_sum = 0;

		for( i = 0; i < n_ifts; i++ )
			/* Get x[i]'s rate */
			rate_sum += ( rates[i] =
				lambda_p * ( x[i] >= 0 ) + lambda_m * ( x[i] < 0 ) );

		/* Add disassembly rate */
		rate_sum += ( rates[n_ifts] = mu * ( length > 0 ) );

		if( rate_sum == 0 ) /* Flagellum Died? */
			tau = time_limit - t;

		else{
			/* Get time until next event */
			tau = ( 1 / rate_sum ) * log( (double)RAND_MAX / (double)rand() );

			/* Check if next event is within the time limit */
			if( tau + t > time_limit )
				tau = time_limit - t;

			else{ /* Pick next event */
				temp = rate_sum * (double)rand() / (double)RAND_MAX;

				temp2 = 0;
				j = 0;
				while( ( ( temp2 += rates[j] ) <= temp ) && ( j++ < n_ifts ) );
				/* j is now the event number */

				if( j == n_ifts ){ /* Disassembly */

					/* Decrease Length, count length change. */
					length += ( length_change = -1 );
					++n_l_changes;

					/* Move IFTs on Disassembled segment down. */
					for( i = 0; i < n_ifts; i++ )
						x[i] += -(x[i] == length+1) + (x[i] == -length-1);

				}else /* Move IFT */

					if( ++x[j] > length ){ /* Then assembly occurs */

						/* Decrease Length, count length change. */
						length += ( length_change = +1 );
						++n_l_changes;
						/* Change direction on IFT */
						x[j] = -length;
					}
			}
		}

		/* Update time */
		t += tau;

		/* Record State if there was a length change or the time limit was reached */
		if( length_change != 0 || t == time_limit ){
			daSet( &times, n_l_changes, t );
			iaSet( &lengths, n_l_changes, length );
		}
	}
/*
	daDestroy( times );
	iaDestroy( lengths );
*/
	printf("\nFinished.\n");

	return;
}
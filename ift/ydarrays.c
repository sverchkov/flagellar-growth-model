/* Author: Yuriy Sverchkov
   File: ydarrays.c
   Description: Implements dynamic integer and double arrays.
*/

#include <stdlib.h>

/***INTEGER ARRAYS***/

typedef struct {
	unsigned int real_length;/*Real length of the array (ammount allocated)*/
	unsigned int length;/*Length of array being used*/
	int * contents;/*Contains the data*/
} IntArray;

IntArray iaCreate( const int array[], const unsigned int size )
/* Creates an IntArray out of the int[] of the specified size. */
{
	unsigned int i, real_size = size + (size == 0);
	IntArray result;

	result.contents = (int *) malloc( real_size * sizeof(int) );
	result.real_length = real_size;
	result.length = size;

	if( array != NULL )
		for( i = 0; i < size; i++ )
			result.contents[i] = array[i];
	else
		for( i = 0; i < size; i++ )
			result.contents[i] = 0;

	return result;
}

void iaDestroy( IntArray a )
/* Deallocates an IntArray's dynamic contents. */
{
	free(a.contents);
	return;
}

int iaSet( IntArray * ptr, unsigned int index, int value ){

	/* Repeatedly doubles the length of the array (if needed) to accomodate
	   the new value. */
	while( index >= ptr->real_length )
		ptr->contents = (int *) realloc(
			ptr->contents,
			( ptr->real_length *= 2 ) * sizeof(int) );

	/* Increases the effective ("user-visible") size of the array and fills
	   unused cells with zeros (if needed). */
	while( ptr->length <= index ){
		ptr->contents[ptr->length] = 0;
		++ ptr->length;
	}

	return ( ptr->contents[index] = value );
}

int iaGet( IntArray a, unsigned int index ) {
	if( index < a.length ) return a.contents[index];
	return 0;
}


/***DOUBLE ARRAYS***/

typedef struct {
	unsigned int real_length;/*Real length of the array (ammount allocated)*/
	unsigned int length;/*Length of array being used*/
	double * contents;/*Contains the data*/
} DoubleArray;

DoubleArray daCreate( const double array[], const unsigned int size )
/* Creates an DoubleArray out of the double[] of the specified size. */
{
	unsigned int i, real_size = size + (size == 0);
	DoubleArray result;

	result.contents = (double *) malloc( real_size * sizeof(double) );
	result.real_length = real_size;
	result.length = size;

	if( array != NULL )
		for( i = 0; i < size; i++ )
			result.contents[i] = array[i];
	else
		for( i = 0; i < size; i++ )
			result.contents[i] = 0;

	return result;
}

void daDestroy( DoubleArray a )
/* Deallocates an DoubleArray's dynamic contents. */
{
	free(a.contents);
	return;
}

double daSet( DoubleArray * ptr, unsigned int index, double value ){

	/* Repeatedly doubles the length of the array (if needed) to accomodate
	   the new value. */
	while( index >= ptr->real_length )
		ptr->contents = (double *) realloc(
			ptr->contents,
			( ptr->real_length *= 2 ) * sizeof(double) );

	/* Increases the effective ("user-visible") size of the array and fills
	   unused cells with zeros (if needed). */
	while( ptr->length <= index ){
		ptr->contents[ptr->length] = 0;
		++ ptr->length;
	}

	return ( ptr->contents[index] = value );
}

double daGet( DoubleArray a, unsigned int index ) {
	if( index < a.length ) return a.contents[index];
	return 0;
}

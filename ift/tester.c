#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

int main( int argc, char* argv[]){

	unsigned i;
	srand(time(NULL));

	for( i=0; i<1000; i++)
		printf("%e\n", ( 1/(double)35 ) * log( (double)RAND_MAX / (double)rand() ) );

	return 0;
}

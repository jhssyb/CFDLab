#ifndef _MAIN_C_
#define _MAIN_C_

#include "collision.h"
#include "streaming.h"
#include "initLB.h"
#include "visualLB.h"
#include "boundary.h"
int main(int argc, char *argv[]){

	double *collideField = NULL;
	double *streamField = NULL;
	int *flagField = NULL;
	int xlength;
	double tau;
	double velocityWall[3];
	int timesteps;
	int timestepsPerPlotting;

	readParameters( &xlength, &tau, velocityWall, &timesteps, &timestepsPerPlotting, argc, argv ); // reading parameters from the file.

	// Allocating the main three arrays.
	int domain = (xlength + 2) * (xlength + 2) * (xlength + 2);
	collideField = (double *) malloc(Q_NUMBER * domain * sizeof(double));
	streamField = (double *) malloc(Q_NUMBER * domain * sizeof(double));
	flagField = (int *) malloc(domain * sizeof(int));

	// Init the main three arrays.
	initialiseFields( collideField, streamField, flagField, xlength );

	for(int t = 0; t <= timesteps; t++){
		double *swap = NULL;
		doStreaming( collideField, streamField, flagField, xlength );

		swap = collideField;
		collideField = streamField;
		streamField = swap;

		doCollision( collideField, flagField, &tau, xlength );

		treatBoundary( collideField, flagField, velocityWall, xlength );

		if ( t % timestepsPerPlotting == 0 ) {
      printf("Writing the vtk file for timestep # %d \n", t);
      writeVtkOutput( collideField, flagField, "pics/simLB", t, xlength );
    }
    
	}


	free(collideField);
	free(streamField);
	free(flagField);
	return 0;
}

#endif


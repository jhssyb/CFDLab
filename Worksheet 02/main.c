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

  readParameters( &xlength, &tau, velocityWall, &timesteps, &timestepsPerPlotting, argc, argv );

  // DEBUG: Print what we read 
  printf("xlength = %d \n", xlength);
  printf("tau = %f \n", tau);
  printf("timesteps = %d \n", timesteps);
  printf("timestepsPerPlotting = %d \n", timestepsPerPlotting);
  printf("velocityWall[1] = %f \n", velocityWall[1]);
  printf("velocityWall[2] = %f \n", velocityWall[2]);
  printf("velocityWall[3] = %f \n", velocityWall[3]);

  // Three main arrays allocation..
  int domain = (xlength + 2) * (xlength + 2) * (xlength + 2);
  collideField = (double *) malloc(Q_NUMBER * domain * sizeof(double));
  streamField = (double *) malloc(Q_NUMBER * domain * sizeof(double));
  flagField = (int *) malloc(domain * sizeof(int));

  initialiseFields( collideField, streamField, flagField, xlength );

  for(int t = 0; t < timesteps; t++){
    printf("t = %d \n", t);
    double *swap = NULL;
    doStreaming( collideField, streamField, flagField, xlength );

    swap = collideField;
    collideField = streamField;
    streamField = swap;

    doCollision( collideField, flagField, &tau, xlength );

    treatBoundary( collideField, flagField, velocityWall, xlength );

    if ( t % timestepsPerPlotting == 0 ){
      writeVtkOutput( collideField, flagField, "pics/", t, xlength );
    }

  }

  return 0;
}

#endif


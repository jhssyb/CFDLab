#include "initLB.h"

int readParameters(int *xlength, double *tau, double *velocityWall, int *timesteps, int *timestepsPerPlotting, int argc, char *argv[]){
  /* TODO */



  return 0;
}


void initialiseFields(double *collideField, double *streamField, int *flagField, int xlength){
  /* TODO */

	int x, y, z, i;

	/* stream & collide Fields initialization. */
	for (x = 0; x < xlength + 2; x++){
		for (y = 0; y < xlength+2; y++){
			for (z = 0; z < xlength+2; z++){
				for (i = 0; i < Q_NUMBER; i++){
					streamField[Q_NUMBER * (x + y*xlength + z*xlength*xlength) + i] = LATTICEWEIGHTS[i];
					collideField[Q_NUMBER * (x + y*xlength + z*xlength*xlength) + i] = LATTICEWEIGHTS[i];
				}
			}
		}
	}






	/* flagField init: boundary vs. fluid. */
	/* Boundary initialization: (5 walls: no-slip; 1 wall: moving wall). Fluid: inner part. */
	/* Why that many for statements? We used the loop unrolling approach to get rid of the if-statements, which would be present in the third for loop. */


	// Fluid init (inner part of flagField).
	for (x = 1; x < xlength +1; x++) {
		for (y = 1; y < xlength + 1; y++) {
			for (z = 1; z < xlength + 1; z++) {
				flagField[x + y*xlength + z*xlength*xlength] = FLUID;
			}
		}
	}


	// Moving wall. For every x, y we set z = xlength + 1.
	z = xlength + 1;
	for (x=0; x < xlength+2; x++){
		for (y=0; y < xlength+2; y++){
			flagField[x + y*xlength + z*xlength*xlength] = MOVING_WALL;
		}
	}


	// No slip. For every y, z we set x = 0.
	x = 0;
	for (y=0; y<xlength+2; y++){
		for (z=0; z<xlength+2; z++){
			flagField[x + y*xlength + z*xlength*xlength] = NO_SLIP;
		}
	}


	// No slip. For every x, z we set y = 0.
	y = 0;
	for (x = 0; x < xlength+2; x++){
		for (z = 0; z < xlength+2; z++){
			flagField[x + y*xlength + z*xlength*xlength] = NO_SLIP;
		}
	}

	// No slip. For every x, y we set z = 0.
	z = 0;
        for (x = 0; x < xlength+2; x++){
                for (y = 0; y < xlength+2; y++){
                        flagField[x + y*xlength + z*xlength*xlength] = NO_SLIP;
                }
        }

	// No slip. For every y, z we set x = xlength+1.
	x=xlength+1;
        for (y = 0; y < xlength+2; y++){
                for (z = 0; z < xlength+2; z++){
                        flagField[x + y*xlength + z*xlength*xlength] = NO_SLIP;
                }
        }

	// No slip. For every x, z we set y = xlength+1;
        y = xlength+1;
        for (x = 0; x < xlength+2; x++){
                for (z = 0; z < xlength+2; z++){
                        flagField[x + y*xlength + z*xlength*xlength] = NO_SLIP;
                }
        }



}

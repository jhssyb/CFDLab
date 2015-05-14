#include "visualLB.h"
#include "LBDefinitions.h"
#include "computeCellValues.h"
#include "helper.h"
#include <stdio.h>

void write_vtkHeader(FILE *fp, int xlength);
void write_vtkPointCoordinates( FILE *fp, int xlength);

void writeVtkOutput(const double * const collideField, const int * const flagField, const char * filename, unsigned int t, int xlength) {

// Opening the file.

	char szFileName[80];
	FILE *fp = NULL;
	sprintf (szFileName, "%s.%i.vtk", filename, t);

	fp = fopen(szFileName, "w");
	if (fp == NULL) {
		char szBuff[80];
		sprintf(szBuff, "Failed to open %s", szFileName);
		ERROR(szBuff);
		return;
	}



	write_vtkHeader(fp, xlength); // Write the header.
	write_vtkPointCoordinates(fp, xlength);

	// write the densities and velocities.
	int x, y, z;

	double velocity[3];
	double density;

	/* DENSITIES */
	fprintf(fp,"DIMENSIONS  %i %i %i \n", xlength, xlength, xlength);
	fprintf(fp,"POINTS %i float\n", xlength*xlength*xlength );
	fprintf(fp,"DENSITIES. \n\n");

        for (x = 0; x < xlength + 2; x++) {
                for(y = 0; y < xlength + 2; y++) {
                        for (z = 0; z < xlength + 2; z++){

                                computeDensity (collideField + Q_NUMBER * (x + xlength*y + xlength*xlength*z), &density);

                                fprintf(fp, "%f\n", density);
                        }
                }
        }


	/* VELOCITIES */
        fprintf(fp,"DIMENSIONS  %i %i %i \n", xlength, xlength, xlength);
        fprintf(fp,"POINTS %i float\n", xlength*xlength*xlength );
        fprintf(fp,"VELOCITIES. \n\n");

	for (x = 0; x < xlength + 2; x++) {
		for(y = 0; y < xlength + 2; y++) {
			for (z = 0; z < xlength + 2; z++){

                                computeDensity (collideField + Q_NUMBER * (x + xlength*y + xlength*xlength*z), &density);
                                computeVelocity (collideField + Q_NUMBER * (x + xlength*y + xlength*xlength*z), &density, velocity);

				fprintf(fp, "%f %f %f\n", velocity[0], velocity[1], velocity[2]);
			}
		}
	}

}

void write_vtkHeader( FILE *fp, int xlength) {
  if( fp == NULL )
  {
    char szBuff[80];
    sprintf( szBuff, "Null pointer in write_vtkHeader" );
    ERROR( szBuff );
    return;
  }

  fprintf(fp,"# vtk DataFile Version 2.0\n");
  fprintf(fp,"generated by CFD-lab course output (written by Andreev M., Chourdakis G., Tominec I.) \n");
  fprintf(fp,"ASCII\n");
  fprintf(fp,"\n");
  fprintf(fp,"DATASET STRUCTURED_GRID\n");
  fprintf(fp,"DIMENSIONS  %i %i %i \n", xlength, xlength, xlength);
  fprintf(fp,"POINTS %i float\n", xlength*xlength*xlength );
  fprintf(fp,"\n");
}



void write_vtkPointCoordinates( FILE *fp, int xlength) {
  double originX = 0.0;
  double originY = 0.0;
  double originZ = 0.0;

  int x, y, z;


  // We have unity cubes. So dx = dy = dz = 1 / (xlength+2)
  double dx, dy, dz;
  dx = 1.0 / (xlength+2);
  dy = 1.0 / (xlength+2);
  dz = 1.0 / (xlength+2);

  for (x = 0; x < xlength + 2; x++){
        for (y = 0; y < xlength + 2; y++){
                for (z = 0; z < xlength + 2; z++){
                        fprintf(fp, "%f %f %f\n", originX+(x*dx), originY+(y*dy), originZ+(z*dz) );
                }
        }
  }
}

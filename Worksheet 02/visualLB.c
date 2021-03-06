#include "visualLB.h"
#include "LBDefinitions.h"
#include "computeCellValues.h"
#include "helper.h"
#include <stdio.h>


void write_vtkHeader(FILE *fp, int xlength);
void write_vtkPointCoordinates( FILE *fp, int xlength);

void writeVtkOutput(const double * const collideField, const int * const flagField, char *filename, unsigned int t, int xlength) {

	// Opening the file.
	FILE *fp = NULL;
	char sZFilename[80];

	sprintf (sZFilename, "%s.%i.vtk", filename, t);

	fp = fopen(sZFilename, "w");
	if (fp == NULL) {
		char szBuff[80];
		sprintf(szBuff, "Failed to open %s", sZFilename);
		ERROR(szBuff);
		return;
	}


	write_vtkHeader(fp, xlength); // Write the header.
	write_vtkPointCoordinates(fp, xlength);

	// write the densities and velocities.
	int x, y, z;

	double velocity[3];
	double density;
	int xlen2 = xlength + 2;
	int xlen2sq = xlen2 * xlen2;
	fprintf(fp, "\nPOINT_DATA %d \n", xlength * xlength * xlength );

	/* DENSITIES */
	fprintf(fp, "SCALARS density float 1 \n");
	fprintf(fp, "LOOKUP_TABLE default \n");
	for (z = 1; z <= xlength; ++z) {
		for (y = 1; y <= xlength; ++y) {
			for (x = 1; x <= xlength; ++x){
				computeDensity (collideField + Q_NUMBER * (x + xlen2 * y + xlen2sq * z), &density);
				fprintf(fp, "%f\n", density);
			}
		}
	}

	/* VELOCITIES */
	fprintf(fp, "\nVECTORS velocity float \n");
	for (z = 1; z <= xlength; ++z) {
		for(y = 1; y <= xlength; ++y) {
			for (x = 1; x <= xlength; ++x){
				const double * const idx = collideField + Q_NUMBER * (x + xlen2 * y + xlen2sq * z);
				computeDensity (idx, &density);
				computeVelocity (idx, &density, velocity);
				fprintf(fp, "%f %f %f\n", velocity[0], velocity[1], velocity[2]);
			}
		}
	}
	
	fclose(fp);
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
	fprintf(fp,"POINTS %i float\n", xlength * xlength * xlength );
	fprintf(fp,"\n");
}



void write_vtkPointCoordinates( FILE *fp, int xlength) {
	double x, y, z;

	// We have unity cubes. So dx = dy = dz = 1 / (xlength - 1)
	double dx, dy, dz;

	// start at 0 and finish at 1.0 => len-1
	// e.g. for 15 points there are 14 "cells"
	dx = 1.0 / (xlength - 1);
	dy = 1.0 / (xlength - 1);
	dz = 1.0 / (xlength - 1);

	// " smart indexing ... 10% faster for 20 points(3sec), 3% for 100(10sec)
	// discretization error appears if we don't include an additional "epsilon" factor.
	// We implemented eps in a way, that we are always on a safe side.
	for (z = 0; z <= 1 + dx * 0.5; z += dz){
		for (y = 0; y <= 1 + dy * 0.5; y += dy){
			for (x = 0; x <= 1 + dz * 0.5; x += dx){
				fprintf(fp, "%f %f %f\n", x, y, z);
			}
		}
	}
}

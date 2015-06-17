#include "visualLB.h"
#include "LBDefinitions.h"
#include "computeCellValues.h"
#include "helper.h"
#include <stdio.h>

void writeVtkOutput(const double * const collideField, const int * const flagField, char *filename, unsigned int t, int *xlength, int rank, int *xlength_global,
										int iProc, int jProc, int kProc) {

	// Opening the file.
	FILE *fp = NULL;
	char sZFilename[80];

	sprintf (sZFilename, "%s.cpu-%i_time-%i.vtk", filename, rank, t);

	fp = fopen(sZFilename, "w");
	if (fp == NULL) {
		char szBuff[80];
		sprintf(szBuff, "Failed to open %s", sZFilename);
		ERROR(szBuff);
		return;
	}

	write_vtkHeader(fp, xlength); // Write the header.

	write_vtkPointCoordinates(fp, xlength, xlength_global, iProc, jProc, kProc, rank);

	// write the densities and velocities.
	int x, y, z;

	double velocity[3];
	double density;
	int xlen2 = xlength[0] + 2;
	int ylen2 = xlength[1] + 2;
	int xylen2 = xlen2 * ylen2;
	fprintf(fp, "\nPOINT_DATA %d \n", (xlength[0]+2) * (xlength[1] +2)*( xlength[2]+2) );

	/* DENSITIES */
	fprintf(fp, "SCALARS density float 1 \n");
	fprintf(fp, "LOOKUP_TABLE default \n");
	for (z = 0; z <= xlength[2]+1; ++z) {
		for (y = 0; y <= xlength[1]+1; ++y) {
			for (x = 0; x <= xlength[0]+1; ++x){
				computeDensity (collideField + Q_NUMBER * (x + xlen2 * y + xylen2 * z), &density);
				fprintf(fp, "%f\n", density);
			}
		}
	}

	/* VELOCITIES */
	fprintf(fp, "\nVECTORS velocity float \n");
	for (z = 0; z <= xlength[2]+1; ++z) {
		for(y = 0; y <= xlength[1]+1; ++y) {
			for (x = 0; x <= xlength[0]+1; ++x){
				const double * const idx = collideField + Q_NUMBER * (x + xlen2 * y + xylen2 * z);
				computeDensity (idx, &density);
				computeVelocity (idx, &density, velocity);
				fprintf(fp, "%f %f %f\n", velocity[0], velocity[1], velocity[2]);
			}
		}
	}

	fclose(fp);
}

void write_vtkHeader( FILE *fp, int *xlength) {
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
	fprintf(fp,"DIMENSIONS  %i %i %i \n", xlength[0]+2, xlength[1]+2, xlength[2]+2);
	fprintf(fp,"POINTS %i float\n", (xlength[0]+2) * (xlength[1]+2) * (xlength[2]+2) );
	fprintf(fp,"\n");
}



void write_vtkPointCoordinates( FILE *fp, int *xlength, int *xlength_global, int iProc, int jProc, int kProc, int rank) {
	double x, y, z;

	// We have unity cubes. So dx = dy = dz = 1 / (xlength - 1) // -1 because spaces between points are 1 less than the number of points
	double dx, dy, dz;

	// start at 0 and finish at 1.0 => len-1
	// e.g. for 15 points there are 14 "cells"
	// if (xlength == 1), => dx = 3 => only 1 iteration with (coord == 0)
	dx = 1.0 / (xlength_global[0]-1);
	dy = 1.0 / (xlength_global[1]-1);
	dz = 1.0 / (xlength_global[2]-1);

	// A factor of mapping from local CPU cube to the global one.
	int xFactor = rank % iProc;
	int yFactor = rank / (iProc * kProc);
	int zFactor = rank % (iProc * kProc)/iProc;

	// The actual mapping.
	double xStart = xFactor * xlength[0] * dx;
	double yStart = yFactor * xlength[1] * dy;
	double zStart = zFactor * xlength[2] * dz;

	double xEnd = ((xFactor + 1) * xlength[0] - 1) * dx;
	double yEnd = ((yFactor + 1) * xlength[1] - 1) * dy;
	double zEnd = ((zFactor + 1) * xlength[2] - 1) * dz;
	printf("rank %d\n",rank);
	printf("dx %f\n", dx);
	printf("dy %f\n", dy);
	printf("dz %f\n", dz);

	printf("xFactor %d\n", xFactor);
	printf("yFactor %d\n", yFactor);
	printf("zFactor %d\n", zFactor);

	printf("xStart %f\n", xStart);
	printf("yStart %f\n", yStart);
	printf("zStart %f\n", zStart);

	printf("xEnd %f\n", xEnd);
	printf("yEnd %f\n", yEnd);
	printf("zEnd %f\n\n", zEnd);
	// " smart indexing ... 10% faster for 20 points(3sec), 3% for 100(10sec)
	// discretization error appears if we don't include an additional "epsilon" factor.
	// We implemented eps in a way, that we are always on a safe side.
	for (z = zStart; z < zEnd + dz * 0.5; z += dz){
		for (y = yStart; y < yEnd + dy * 0.5; y += dy){
			for (x = xStart; x < xEnd + dx * 0.5; x += dx){
				fprintf(fp, "%f %f %f\n", x, y, z);
			}
		}
	}
}

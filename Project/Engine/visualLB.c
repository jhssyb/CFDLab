#include "visualLB.h"
#include "LBDefinitions.h"
#include "computeCellValues.h"
#include "helper.h"
#include <stdio.h>

void writeVtkOutput(const double * const collideField, const int * const flagField, char *filename, unsigned int t, int *xlength, int part_id) {

	// Opening the file.
	FILE *fp = NULL;
	char sZFilename[80];

	sprintf (sZFilename, "%s.part-%i_time-%i.vtk", filename, part_id, t);

	fp = fopen(sZFilename, "w");
	if (fp == NULL) {
		char szBuff[80];
		sprintf(szBuff, "Failed to open %s", sZFilename);
		ERROR(szBuff);
		return;
	}

	write_vtkHeader(fp, xlength); // Write the header.

	write_vtkPointCoordinates(fp, part_id);

	// write the densities and velocities.
	int x, y, z;

	double velocity[3];
	double density;
	int xlen2 = xlength[0] + 2;
	int ylen2 = xlength[1] + 2;
	int xylen2 = xlen2 * ylen2;
	fprintf(fp, "\nPOINT_DATA %d \n", (xlength[0]) * (xlength[1])*( xlength[2]) );

	/* DENSITIES */
	fprintf(fp, "SCALARS density float 1 \n");
	fprintf(fp, "LOOKUP_TABLE default \n");
	for (z = 1; z < xlength[2]+1; ++z) {
		for (y = 1; y < xlength[1]+1; ++y) {
			for (x = 1; x < xlength[0]+1; ++x){
				computeDensity (collideField + Q_NUMBER * (x + xlen2 * y + xylen2 * z), &density);
				fprintf(fp, "%f\n", density);
			}
		}
	}

	/* VELOCITIES */
	fprintf(fp, "\nVECTORS velocity float \n");
	for (z = 1; z < xlength[2] + 1; ++z) {
		for(y = 1; y < xlength[1] + 1; ++y) {
			for (x = 1; x < xlength[0] + 1; ++x){
				const double * const idx = collideField + Q_NUMBER * (x + xlen2 * y + xylen2 * z);
				computeDensity (idx, &density);
				computeVelocity (idx, &density, velocity);
				fprintf(fp, "%f %f %f\n", velocity[0], velocity[1], velocity[2]);
			}
		}
	}

	fclose(fp);
}

void write_vtkHeader( FILE *fp, const int * const xlength) {
	if( fp == NULL )
	{
		char szBuff[80];
		sprintf( szBuff, "Null pointer in write_vtkHeader" );
		ERROR( szBuff );
		return;
	}

	fprintf(fp,"# vtk DataFile Version 2.0\n");
	fprintf(fp,"generated by CFD-lab project MI building (written by Andreev M., Chourdakis G., Tominec I.) \n");
	fprintf(fp,"ASCII\n");
	fprintf(fp,"\n");
	fprintf(fp,"DATASET STRUCTURED_GRID\n");
	fprintf(fp,"DIMENSIONS  %i %i %i \n", xlength[0], xlength[1], xlength[2]);
	fprintf(fp,"POINTS %i float\n", (xlength[0]) * (xlength[1]) * (xlength[2]) );
	fprintf(fp,"\n");
}

#define XSIZE 8.0

void write_vtkPointCoordinates( FILE *fp, int part_id) {
	double x, y, z;

	double dx, dy, dz;
 	// 8 lattice points per xsize
	double xsize_ratio = 9.5; // 1xsize corresponds to 9.5m
	dx = xsize_ratio / (XSIZE-1); // 9.5m per 8 points (7 intervals)
	dy = dx;
	dz = dx;

	double origin[3];
	double end[3];

	switch (part_id) {
		case 0:
			origin[0] = 1;
			origin[1] = 8*XSIZE;
			origin[2] = 0;
			end[0] = 8*XSIZE;
			end[1] = 12*XSIZE-3;
			end[2] = 0;
			break;
		case 1:
			origin[0] = 8*XSIZE+1;
			origin[1] = 8*XSIZE;
			origin[2] = 0;
			end[0] = 16*XSIZE;
			end[1] = 12*XSIZE-3;
			end[2] = 0;
			break;
		case 2:
			origin[0] = 16*XSIZE+1;
			origin[1] = 8*XSIZE;
			origin[2] = 0;
			end[0] = 24*XSIZE;
			end[1] = 12*XSIZE-3;
			end[2] = 0;
			break;
		case 3:
			origin[0] = 24*XSIZE+1;
			origin[1] = 8*XSIZE;
			origin[2] = 0;
			end[0] = 32*XSIZE-2;
			end[1] = 12*XSIZE-3;
			end[2] = 0;
			break;
		case 4:
			origin[0] = 6*XSIZE+1;
			origin[1] = 1;
			origin[2] = 0;
			end[0] = 8*XSIZE-2;
			end[1] = 8*XSIZE-2;
			end[2] = 0;
			break;
		case 5:
			origin[0] = 12*XSIZE+1;
			origin[1] = 1;
			origin[2] = 0;
			end[0] = 14*XSIZE-2;
			end[1] = 8*XSIZE-2;
			end[2] = 0;
			break;
		case 6:
			origin[0] = 18*XSIZE+1;
			origin[1] = 1;
			origin[2] = 0;
			end[0] = 20*XSIZE-2;
			end[1] = 8*XSIZE-2;
			end[2] = 0;
			break;
		case 7:
			origin[0] = 24*XSIZE+1;
			origin[1] = 1;
			origin[2] = 0;
			end[0] = 26*XSIZE-2;
			end[1] = 8*XSIZE-2;
			end[2] = 0;
			break;
		case 8:
			origin[0] = 30*XSIZE+1;
			origin[1] = 1;
			origin[2] = 0;
			end[0] = 32*XSIZE-2;
			end[1] = 8*XSIZE-2;
			end[2] = 0;
			break;
		case 9:
			origin[0] = 2*XSIZE+1;
			origin[1] = 12*XSIZE-1;
			origin[2] = 0;
			end[0] = 4*XSIZE-2;
			end[1] = 20*XSIZE-4;
			end[2] = 0;
			break;
		case 10:
			origin[0] = 8*XSIZE+1;
			origin[1] = 12*XSIZE-1;
			origin[2] = 0;
			end[0] = 10*XSIZE-2;
			end[1] = 20*XSIZE-4;
			end[2] = 0;
			break;
		case 11:
			origin[0] = 14*XSIZE+1;
			origin[1] = 12*XSIZE-1;
			origin[2] = 0;
			end[0] = 16*XSIZE-2;
			end[1] = 20*XSIZE-4;
			end[2] = 0;
			break;
		case 12:
			origin[0] = 20*XSIZE+1;
			origin[1] = 12*XSIZE-1;
			origin[2] = 0;
			end[0] = 22*XSIZE-2;
			end[1] = 20*XSIZE-4;
			end[2] = 0;
			break;
		case 13:
			origin[0] = 26*XSIZE+1;
			origin[1] = 12*XSIZE-1;
			origin[2] = 0;
			end[0] = 28*XSIZE-2;
			end[1] = 20*XSIZE-4;
			end[2] = 0;
			break;
	}

	// The actual mapping.
	double xStart = origin[0] * dx;
	double yStart = origin[1] * dy;
	double zStart = origin[2] * dz;

	double xEnd = end[0] * dx;
	double yEnd = end[1] * dy;
	double zEnd = end[2] * dz;

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

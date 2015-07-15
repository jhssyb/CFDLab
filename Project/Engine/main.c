#ifndef _MAIN_C_
#define _MAIN_C_

#include "mpi_helper.h"
#include "collision.h"
#include "streaming.h"
#include "initLB.h"
#include "visualLB.h"
#include "boundary.h"
//sleep()
#include <unistd.h>

// free complains without this library ???
#include "stdlib.h"
int main(int argc, char *argv[]){
	printf("hi!\n");
	int rank;
	int np;
	
	// Start MPI
	// slow for more processors ???
	initializeMPI( &rank, &np, argc, argv);
	double tau;
	double velocityWall[3];
	int timesteps;
	int timestepsPerPlotting;
	double_3d * inflow = (double_3d *) malloc(INFLOW_COUNT * sizeof(double_3d));
	double * pressure_in = (double *) malloc(PRESSURE_IN_COUNT * sizeof(double));
	int error_code;
	double ref_density;

	// Read the config file using only one thread
	if(0 == rank){
		error_code = readParameters(&tau, velocityWall, &timesteps, &timestepsPerPlotting,
inflow, pressure_in, &ref_density, argc, argv);
		// Error checking
		if(error_code) return error_code;
	}
	MPI_Bcast( &tau, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD );
	MPI_Bcast( &timesteps, 1, MPI_INT, 0, MPI_COMM_WORLD );
	MPI_Bcast( &timestepsPerPlotting, 1, MPI_INT, 0, MPI_COMM_WORLD );
	MPI_Bcast( velocityWall, 3, MPI_DOUBLE, 0, MPI_COMM_WORLD );
	for(int i = 0; i < INFLOW_COUNT; ++i){
		MPI_Bcast( &inflow[i].x, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD );
		MPI_Bcast( &inflow[i].y, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD );
		MPI_Bcast( &inflow[i].z, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD );
	}
	
	MPI_Bcast( pressure_in, PRESSURE_IN_COUNT, MPI_DOUBLE, 0, MPI_COMM_WORLD );
	MPI_Bcast( &ref_density, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD );

	/*
//// test for correct data reading
//	printf("\
//xlength x y z		= %d %d %d\n \
//tau			= %f\n \
//timesteps		= %d\n \
//timestepsPerPlotting	= %d\n \
//velocityWall x y z	= %f %f %f\n \
//iProc			= %d\n \
//jProc			= %d\n \
//kProc			= %d\n \
//cpuDomain x y z	= %d %d %d\n", xlength[0], xlength[1], xlength[2], tau, timesteps, timestepsPerPlotting, velocityWall[0], velocityWall[1], velocityWall[2], iProc, jProc, kProc, cpuDomain[0], cpuDomain[1], cpuDomain[2]);
//	sleep(1000);
*/

	//printf("values:\n xlength: %d %d %d\n Proc ijk %d %d %d\n cpuDomain: %d %d %d\n ",xlength[0],xlength[1],xlength[2],iProc,jProc,kProc,cpuDomain[0],cpuDomain[1],cpuDomain[2]);
	// Init the main three arrays.
	//calculate domain size

	int domain_size = 0;
	for(int i = 0; i < chunk_count[rank];++i){
		domain_size += chunk_size[rank][i];
	}
	domain_size *= 3; // because we're in 3D
	double *collideField = (double *) malloc(Q_NUMBER * domain_size * sizeof(double));
	double *streamField = (double *) malloc(Q_NUMBER * domain_size * sizeof(double));
	int *flagField = (int *) malloc(domain_size * sizeof(int));
	char pgm_read_file[1024];

	for(int i = 0; i < chunk_count[rank]; ++i){
		initialiseFields( collideField + Q_NUMBER * chunk_begin_offset[rank][i], streamField + Q_NUMBER * chunk_begin_offset[rank][i], cpuDomain[rank][i]); // collide and stream

		sprintf( pgm_read_file, "cpu_%d.pgm",chunk_id[rank][i]);
		read_assign_PGM(flagField + chunk_begin_offset[rank][i],pgm_read_file,cpuDomain[rank][i]);
	}
	
	//printf("neighbors:\n%d %d %d %d %d %d\n",neighbor[0], neighbor[1], neighbor[2], neighbor[3], neighbor[4], neighbor[5]);
	
	
				// probably this is useless - will keep it for now just in case
				// precomputed values for extraction and injection bounds depending on the direction
				//const side Bsides_ext[6] = {{cpuDomain[0], cpuDomain[0], 0, cpuDomain[1] + 1, 0, cpuDomain[2] + 1},
				//														{1, 1,											 0, cpuDomain[1] + 1, 0, cpuDomain[2] + 1},
				//														{0, cpuDomain[0] + 1, 0, cpuDomain[1] + 1, cpuDomain[2], cpuDomain[2]},
				//														{0, cpuDomain[0] + 1, 0, cpuDomain[1] + 1, 1, 1},
				//														{0, cpuDomain[0] + 1, cpuDomain[1], cpuDomain[1], 0, cpuDomain[2] + 1},
				//														{0, cpuDomain[0] + 1, 1, 1,												0, cpuDomain[2] + 1}};
				//const side Bsides_inj[6] = {{0, 0, 0, cpuDomain[1] + 1, 0, cpuDomain[2] + 1},
				//														{cpuDomain[0] + 1, cpuDomain[0] + 1, 0, cpuDomain[1] + 1, 0, cpuDomain[2] + 1},
				//														{0, cpuDomain[0] + 1, 0, cpuDomain[1] + 1, 0, 0},
				//														{0, cpuDomain[0] + 1, 0, cpuDomain[1] + 1, cpuDomain[2] + 1, cpuDomain[2] + 1},
				//														{0, cpuDomain[0] + 1, 0, 0, 0, cpuDomain[2] + 1},
				//														{0, cpuDomain[0] + 1, cpuDomain[1] + 1, cpuDomain[1] + 1, 0, cpuDomain[2] + 1}};

// send and read buffers for all possible directions :
	// [0:left, 1:right, 2:top, 3:bottom, 4:front, 5:back]
	double * *sendBuffer = (double * *) malloc(neighbours_count[rank] * sizeof(double *));
	double * *readBuffer = (double * *) malloc(neighbours_count[rank] * sizeof(double *));
		for(int i = 0; i < neighbours_count[rank]; ++i){
			sendBuffer[i] = (double *) malloc(neighbours_local_buffer_size[rank][i] * sizeof(double));
			readBuffer[i] = (double *) malloc(neighbours_local_buffer_size[rank][i] * sizeof(double));
		}
	// int sizeBuffer[6]; // do we need this and how to initialize it
	// initialiseBuffers(sendBuffer, readBuffer, cpuDomain, sizeBuffer);

	double *tmp = NULL;
	for(int t = 0; t <= timesteps; t++){
		// TODO: maybe move all these to a separate function?
		//if(!rank)
			//printf("t = %d, rank = %d\n",t,rank);
		// Do extraction, swap, injection for

		// swap needs two checks so they're in the function

		for(int i = 0; i < neighbours_count[rank]; ++i){
			extraction( collideField + Q_NUMBER * chunk_begin_offset[rank][i],
									cpuDomain[rank][i],
									sendBuffer[i],
									neighbours_dir[rank][i],
									neighbours_local_start_x[rank][i],
									neighbours_local_start_y[rank][i],
									neighbours_local_start_z[rank][i],
									neighbours_local_end_x[rank][i],
									neighbours_local_end_y[rank][i],
									neighbours_local_end_z[rank][i]); // should we just pass rank and move all these inside the function call ? would be slightly faster + and will have cleaner main
		}
		printf("after extr\n");
		//swap( sendBuffer, readBuffer, sizeBuffer, DIR_R, neighbor);
			swap( sendBuffer, readBuffer, rank);
			printf("after swap\n");
		for(int i = 0; i < neighbours_count[rank]; ++i){
			injection( collideField + Q_NUMBER * chunk_begin_offset[rank][i],
									cpuDomain[rank][i],
									sendBuffer[i],
									neighbours_dir[rank][i],
									neighbours_local_start_x[rank][i],
									neighbours_local_start_y[rank][i],
									neighbours_local_start_z[rank][i],
									neighbours_local_end_x[rank][i],
									neighbours_local_end_y[rank][i],
									neighbours_local_end_z[rank][i]);
		}
		printf("after inj\n");

		/// old extr-swap-inj
		//		// x+ (left to right)
		//if (neighbor[DIR_R] != MPI_PROC_NULL)	extraction( collideField, cpuDomain, sendBuffer, DIR_R, Bsides_ext);
		////printf("after extr\n");
		//swap( sendBuffer, readBuffer, sizeBuffer, DIR_R, neighbor);
		////printf("after swap\n");
		//if (neighbor[DIR_L] != MPI_PROC_NULL)	injection( collideField, cpuDomain, readBuffer, DIR_R, Bsides_inj);
		////printf("after inj\n");
		//// x- (right to left)
		//if (neighbor[DIR_L] != MPI_PROC_NULL)	extraction( collideField, cpuDomain, sendBuffer, DIR_L, Bsides_ext);
		//swap( sendBuffer, readBuffer, sizeBuffer, DIR_L, neighbor);
		//if (neighbor[DIR_R] != MPI_PROC_NULL)	injection( collideField, cpuDomain, readBuffer, DIR_L, Bsides_inj);
		////printf("after 2\n");
		//// z+ (down to up)
		//if (neighbor[DIR_T] != MPI_PROC_NULL)	extraction( collideField, cpuDomain, sendBuffer, DIR_T, Bsides_ext);
		//swap( sendBuffer, readBuffer, sizeBuffer, DIR_T, neighbor);
		//if (neighbor[DIR_D] != MPI_PROC_NULL)	injection( collideField, cpuDomain, readBuffer, DIR_T, Bsides_inj);
		////printf("after 3\n");
		//// z- (up to down)
		//if (neighbor[DIR_D] != MPI_PROC_NULL)	extraction( collideField, cpuDomain, sendBuffer, DIR_D, Bsides_ext);
		//swap( sendBuffer, readBuffer, sizeBuffer, DIR_D, neighbor);
		//if (neighbor[DIR_T] != MPI_PROC_NULL)	injection( collideField, cpuDomain, readBuffer, DIR_D, Bsides_inj);
		////printf("after 4\n");
		//// y- (forth to back)
		//if (neighbor[DIR_B] != MPI_PROC_NULL)	extraction( collideField, cpuDomain, sendBuffer, DIR_B, Bsides_ext);
		//swap( sendBuffer, readBuffer, sizeBuffer, DIR_B, neighbor);
		//if (neighbor[DIR_F] != MPI_PROC_NULL)	injection( collideField, cpuDomain, readBuffer, DIR_B, Bsides_inj);
		////printf("after 5\n");
		//// y+ (back to forth)
		//if (neighbor[DIR_F] != MPI_PROC_NULL)	extraction( collideField, cpuDomain, sendBuffer, DIR_F, Bsides_ext);
		//swap( sendBuffer, readBuffer, sizeBuffer, DIR_F, neighbor);
		//if (neighbor[DIR_B] != MPI_PROC_NULL)	injection( collideField, cpuDomain, readBuffer,DIR_F, Bsides_inj);

		//printf("before boundary\n");
		for(int i = 0; i < chunk_count[rank]; ++i){
			treatBoundary( collideField + Q_NUMBER * chunk_begin_offset[rank][i], flagField + chunk_begin_offset[rank][i], velocityWall, &ref_density, cpuDomain[rank][i], inflow, pressure_in);
		//printf("before streaming\n");
			doStreaming( collideField + Q_NUMBER * chunk_begin_offset[rank][i], streamField + Q_NUMBER * chunk_begin_offset[rank][i], flagField + chunk_begin_offset[rank][i], cpuDomain[rank][i]);
		}
		tmp = collideField;
		collideField = streamField;
		streamField = tmp;
		//printf("before collision\n");
		for(int i = 0; i < chunk_count[rank]; ++i){
			doCollision( collideField + Q_NUMBER * chunk_begin_offset[rank][i], flagField + chunk_begin_offset[rank][i], &tau, cpuDomain[rank][i] );
		}
		//printf("after collision\n");

		//printf("%d time\n",t);
		if ( t % timestepsPerPlotting == 0 ) {
				//printf("%d cpu x\n", cpuDomain[0]);
				//printf("%d cpu y\n", cpuDomain[1]);
				//printf("%d cpu z\n", cpuDomain[2]);
				//printf("%d rank\n", rank);
				//printf("%d xlength\n", xlength[0]);
				//printf("%d ylength\n", xlength[1]);
				//printf("%d zlength\n", xlength[2]);
				//printf("%d iproc\n", iProc);
				//printf("%d jproc\n", jProc);
				//printf("%d kproc\n\n", kProc);
			if(!rank)
				printf("Write vtk for time # %d \n", t);
			for(int i = 0; i < chunk_count[rank]; ++i){
				printf("rank %d i= %d, cpudDomain is %d %d %d \n\n",rank,i,cpuDomain[rank][i][0],cpuDomain[rank][i][1],cpuDomain[rank][i][2]);
				writeVtkOutput( collideField + Q_NUMBER * chunk_begin_offset[rank][i], flagField + chunk_begin_offset[rank][i], "pics/simLB", t, cpuDomain[rank][i], rank, cpuDomain[rank][i] ); // what are we doing with xlength and cpuDomain ???
			}
		}
	}

	free((void *)collideField);
	free((void *)streamField);
	free((void *)flagField);

	free(inflow);
	free(pressure_in);

	for(int i = 0; i < chunk_count[rank]; ++i){
		free(sendBuffer[i]);
		free(readBuffer[i]);
	}
	free(sendBuffer);
	free(readBuffer);

	finalizeMPI();

	return 0;
}
#endif

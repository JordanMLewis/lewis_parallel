#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <omp.h>
#define UNUSED __attribute__((unused)) 

#define DIMENSIONS 100
#define NUM_ITERATIONS 10

#define NUM_PARTITIONS 4
#define NUM_THREADS 4
 
#define HEAT_FACTOR 100.0
#define COLD_FACTOR -100.0

typedef struct partition {
	int size;
	double** data;
	double* ghost_cells_north;
	double* ghost_cells_south;
} Partition;

int main(){

	//Turn of dynamic thread usage for experimentation purposes
	omp_set_dynamic(0);
	omp_set_num_threads(NUM_THREADS);

	int n = DIMENSIONS; //rows, cols of nxn input
	int n2 = n + 2;
	int partition_size = (n*n)/NUM_PARTITIONS;
	int rows_per_partition = (int) round(((double)n*n/(double)NUM_PARTITIONS)); 
	int total_rows = n;

	printf("%d\n", rows_per_partition);
	//exit(0);

	//Create new double** on heap
	double** in = (double**) malloc(sizeof(double*)*n2);
	double** out = (double**) malloc(sizeof(double*)*n2);
	if(in == NULL or out == NULL){ exit(1);}
	
	//Malloc double* for each index
	for(int i = 0; i < n2; i++){ 
		in[i] = (double*) malloc(sizeof(double)*n2); 
		out[i] = (double*) malloc(sizeof(double)*n2);
	}

	//Constants, minimize computation
	double c = (double) 0.1; 	
	double s = (double) (1.0/(n+1.0));
	double t = (double) ((s*s)/(4.0*c));
	double constantFactor = (double) c * (t / s*s);

	//Initialize values in input array
	double heatFactor = HEAT_FACTOR;
	double coldFactor = COLD_FACTOR;

	//Initialize all values of output
	for(int i = 0; i < n2; i++){
		for(int j = 0; j < n2; j++){
			out[i][j] = coldFactor;	
			in[i][j] = coldFactor;	
		}
	}

	int n1 = n+1;
	//Initialize all border values to be "hot", inner values to be cold
	for(int i = 0; i < n+2; i++){
		in[0 ][i ] = out[0][i ] = heatFactor; //top border
		in[i ][0 ] = out[i][0 ] = heatFactor; //left border
		in[i ][n1] = out[i][n1] = heatFactor; //right border
		in[n1][i ] = out[n1][i] = heatFactor; //bottom border
	}
	
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//Create partition arrays
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	/*
 	* Create a 3D array where layer one is an array of partition pointers,
 	* and each partition pointer points to a 2D array of data
 	*/

	//Partition arrayn
	Partition* parts = (Partition*) malloc(sizeof(Partition)*NUM_PARTITIONS);
	
	//set up arrays
	for(int i = 0; i < NUM_PARTITIONS; i++){
		parts[i].data              = (double**) malloc(sizeof(double*)*rows_per_partition);
		for(int j = 0; j < rows_per_partition; j++){
			parts[i].data[j] = (double*) malloc(sizeof(double)*n);
		}

		parts[i].ghost_cells_south = (double*) malloc(sizeof(double)*n);
		parts[i].ghost_cells_north = (double*) malloc(sizeof(double)*n);
	}

	int current_row = 1;
	int copied_rows = 0;

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//Copy row data for each partition
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	for(int i = 0; i < NUM_PARTITIONS; i++){
		copied_rows = 0;

		//Continue to copy rows until we fill our partition
		while(copied_rows < rows_per_partition && current_row+1 < total_rows){
			//Copy data from cells in row above
			memcpy(&(parts[i].ghost_cells_north), &(in[current_row-1][1]), sizeof(double)*n); 
			//Copy data from cells in current row
			memcpy(&(parts[i].data), &(in[current_row][1]), sizeof(double)*n); 
			//Copy data from cells in row below
			memcpy(&(parts[i].ghost_cells_south), &(in[current_row+1][1]), sizeof(double)*n); 
			//Increment copied rows	
			copied_rows++;
		}

		current_row+=copied_rows;
	}
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	/*
	//Partition*** should be num_partition double**'s
	double*** partitions = (double***) malloc(sizeof(double**));
	for(int i = 0; i < NUM_PARTITIONS; i++){

		//partition** should be num_partitions double*'s 
		partitions[i] = (double**) malloc(sizeof(double*)*NUM_PARTITIONS);
		
		//Malloc (n*n)/partitions space for each partition (partition size)
		for(int j = 0; j < NUM_PARTITIONS; j++){
			partitions[i][j] = (double*) malloc(sizeof(double)*partition_size);
		
			//Memcpy the input data to each partition
			memcpy(partitions[i][j], in[i+1], sizeof(double)*partition_size);		
		}

		
 		* [X  ], [+1], [ ], [ ], [ ], [ ],
 		* [0+1], [+1], [ ], [ ], [ ], [ ],
 		* [1+1], [+1], [ ], [ ], [ ], [ ],
 		* [2+1], [+1], [ ], [ ], [ ], [ ],
 		* [3+1], [+1], [ ], [ ], [ ], [ ],
 		* [   ], [+1], [ ], [ ], [ ], [ ],
 		
	}
	*/			
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//create ghost cell barriers
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	


	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	/*
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			std::cout << partitions[i][j] << " ";
		}
		std::cout << std::endl;
	}
	*/

	//Used to update input/output
	double** swap = NULL;

	//Main loop (cannot be parallelized)
	for(int k = 0; k < NUM_ITERATIONS; k++){

		//#pragma omp parallel for schedule(static)
		for(int i = 1; i <= n; i++){
			for(int j = 1; j <= n; j++){
				out[i][j] = in[i][j] + constantFactor *
				// Incorporate adjacent elements 
				(in[i+1][j] + in[i-1][j] - 4.0 * in[i][j] + in[i][j+1] + in[i][j-1]); 	
			}
		}
		swap = out;
		out = in;
		in = swap;
	}

		
	//Clean up memory	
	/*
	for(int i = 0; i < NUM_PARTITIONS; i++){
		for(int j = 0; j < NUM_PARTITIONS; j++){

		}	
		free(partitions[i]);
	}
	free(partitions);
	*/
	for(int i = 0; i < n2; i++){free(in[i]); free(out[i]);}
	free(in); free(out);
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <omp.h>
#define UNUSED __attribute__((unused)) 

#define DIMENSIONS 100 
#define NUM_ITERATIONS 10000
#define NUM_THREADS 4
 
#define HEAT_FACTOR 100.0
#define COLD_FACTOR -100.0

int main(){

	//Turn of dynamic thread usage for experimentation purposes
	omp_set_dynamic(0);
	omp_set_num_threads(NUM_THREADS);

	int n = DIMENSIONS; //rows, cols of nxn input
	int n2 = n + 2;

	//Create new array on heap
	double** in = (double**) malloc(sizeof(double*)*n2);
	double** out = (double**) malloc(sizeof(double*)*n2);
	if(in == NULL or out == NULL){ exit(1);}
	for(int i = 0; i < n2; i++){
		in[i] = (double*) malloc(sizeof(double)*n2);
		out[i] = (double*) malloc(sizeof(double)*n2);
	}
	
	double c = 0.1; 	//constants
	double s = (1/(n+1));     //constants
	double t = ((s*s)/(4*c)); //constants
	double constantFactor = c * (t / s*s); //Minimize computation in the loop

	//Initialize values in input array
	double heatFactor = HEAT_FACTOR;
	double coldFactor = COLD_FACTOR;

	int j = 0;
	int n1 = n+1;
	//Initialize all border values to be "hot", inner values to be cold
	for(int i = 0; i < n+2; i++){
		in[0  ][i] = out[0][i  ] = heatFactor; //top border
		in[i  ][0] = out[i][0  ] = heatFactor; //left border
		in[i][n1] = out[i][n1] = heatFactor; //right border
		in[n1][i] = out[n1][i] = heatFactor; //bottom border
		in[i][j] = coldFactor;
		j++;
	}		

	
	//Used to update input/output
	double** swap = NULL;

	//Main loop (cannot be parallelized)
	for(int k = 0; k < NUM_ITERATIONS; k++){
		#pragma omp parallel for schedule(static)
		for(int i = 1; i < n+1; i++){
			for(int j = 1; j < n+1; j++){
				out[i][j] = in[i][j] + constantFactor *
				// Incorporate adjacent elements 
				(in[i+1][j] + in[i-1][j] - 4 * in[i][j] + in[i][j+1] + in[i][j-1]); 	
			}
		}
		swap = out;
		out = in;
		in = swap;
	}

	//Clean up memory	
	for(int i = 0; i < n2; i++){free(in[i]); free(out[i]);}
	free(in); free(out);
	return 0;
}

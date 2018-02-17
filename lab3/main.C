#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <opm.h>
#define UNUSED __attribute__((unused)) 

#define NUM_ITERATIONS 10000
#define HEAT_FACTOR 100.0
#define COLD_FACTOR -100.0

int main(UNUSED int argc, UNUSED char** argv){

	int n = 1000; //rows, cols of nxn input
	int n2 = n + 2;

	//Create new array on heap
	double** in = new double*[n2];
	double** out = new double*[n2];
	for(int i = 0; i < n2; i++){
		in[i] = new double[n2];
		out[i] = new double[n2];
	}
	double c = 0.1; 	//constants
	double s = (1/n+1);     //constants
	double t = ((s*s)/4*c); //constants

	//Initialize values in input array
	double heatFactor = HEAT_FACTOR;
	double coldFactor = COLD_FACTOR;

	//Initialize all array values to be cold
	for(int i = 0; i < n+2; i++)
		for(int j = 0; j < n+2; j++)
			in[i][j] = coldFactor;

	//Initialize all border values to be "hot"
	for(int i = 0; i < n+2; i++){
		in[0  ][i] = out[0][i] = heatFactor; //top border
		in[i  ][0] = out[i][0] = heatFactor;//left border
		in[i][n+1] = out[i][n+1] = heatFactor;//right border
		in[n+1][i] = out[n+1][i] = heatFactor;//bottom border
	}		

	for(int k = 0; k < NUM_ITERATIONS; k++)
	{
		#pragma omp parallel num_threads(2)
		for(int i = 1; i < n+1; i++)
		{
			for(int j = 1; j < n+1; j++)
			{
				out[i][j] = in[i][j] + c * (t / s*s) *
				// Incorporate adjacent elements 
				(in[i+1][j] + in[i-1][j] - 4 * in[i][j] + in[i][j+1] + in[i][j-1]); 	
			}
		}
		double** swap;
		swap = out;
		out = in;
		in = swap;
	}

	for(int i = 0; i < n2; i++){
		for(int j = 0; j < n2; j++){
		//	std::cout << out[i][j] << " ";
		}	
		//std::cout << std::endl;
	}

	//Clean up memory	
	for(int i = 0; i < n2; i++){
		delete in[i]; delete out[i];
	}
	delete in; delete out;

	return 0;
}

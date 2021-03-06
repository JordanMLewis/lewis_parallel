#include <stdio.h>
#include <cmath>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define NITERS 1000
#define N 5000
#define CONVERGENCE_THRESHOLD 0.001

using namespace std;

int main(int argc, char** argv){

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// Initialize number of processes and process ID
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	int numProc;
	int myID;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numProc);
	MPI_Comm_rank(MPI_COMM_WORLD, &myID);

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// Calculate Partition size and get neighbors
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    	int partition_size = (N)/(numProc);
	int prevProc = myID - 1;
	int nextProc = myID + 1;
	//Make sure we don't go out of bounds
	if(prevProc < 0){
		prevProc = MPI_PROC_NULL;
	}
	if(nextProc >= numProc){
		nextProc = MPI_PROC_NULL;
	}

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//Create new arrays on the heap
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    	double** input_temp = new double*[partition_size+2];
 	double** output_temp = new double*[partition_size+2];
    				
    	for(int i = 0; i < partition_size+2; i++)
    	{
    		input_temp[i] = new double[N+2];
    		output_temp[i] = new double[N+2];
    	}

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//Create new arrays on the heap
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	for(int i = 0; i < partition_size+2; i++)
    	{
    		for(int j = 0; j <N+2; j++)
    		{
			input_temp[i][j] = -100.0;
			output_temp[i][j] = -100.0;
		}
	}
	
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//If we're rank 0, we want our top cells to be 100
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	if(myID == 0){
		for(int i = 0; i < N+2; i++)
		{
			input_temp[0][i] = 100.0;
			output_temp[0][i] = 100.0;
		}

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//If we're rank n, we want our bottom cells to be 100
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	} else if(myID == (numProc - 1)) {
		for(int i = 0; i < N+2; i++)
		{
			input_temp[partition_size+1][i] = 100.0;
			output_temp[partition_size+1][i] = 100.0;	
		}
	}
	
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//All other ranks between 1 and n, only sides are 100
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	for(int i = 0; i < partition_size+2; i++)
	{
		input_temp[i][0] = 100.0;
		input_temp[i][N+1] = 100.0;
		output_temp[i][0] = 100.0;
		output_temp[i][N+1] = 100.0;
	}
    	
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//Initialize constants
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	double c = 0.1;
	double s = (double) 1.0/(N+1.0);
	double t = (double) (s*s)/(4.0*c);
	double constFactor = (double) (c * (t / s*s));
	
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//Main loop for simulations
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	MPI_Request request1, request2;
	MPI_Status stat1, stat2;	
	int converged = 1;
	int global_convergence = 0;
	int done = 0;

	double** temp = NULL;
	for(int iter = 0; iter < NITERS; iter++)
	{
		for(int i = 1; i < partition_size+1; i++)
		{
			for(int j = 1; j < N+1; j++)
			{
				output_temp[i][j] = 
				input_temp[i][j] + constFactor * (input_temp[i+1][j]+input_temp[i-1][j]
				 - 4*input_temp[i][j] + input_temp[i][j+1] +input_temp[i][j-1]);
			}
		}

		//Send top row to previous partition
		MPI_Isend(output_temp[1], N+3, MPI_DOUBLE, prevProc, 1, MPI_COMM_WORLD, &request1);
		//Send bottom row to next partition
		MPI_Isend(output_temp[partition_size], N+3, MPI_DOUBLE, nextProc, 1, MPI_COMM_WORLD, &request2);
		
		//Blocking receieve previous partition for top ghost cells
		MPI_Recv(input_temp[0], N+3, MPI_DOUBLE, prevProc, 1, MPI_COMM_WORLD, &stat1);
		//Blocking recieve next partition for bottom ghost cells
		MPI_Recv(input_temp[partition_size+1], N+3, MPI_DOUBLE, nextProc, 1, MPI_COMM_WORLD, &stat2);
		
		//Test for convergence. If partition has not converged, continue.	
		converged = 1;
		for(int k = 0; k < partition_size+2; k++){
			for(int l = 0; l < N+2; l++){
				//If the difference was greater than the
				if(abs(input_temp[k][l] - output_temp[k][l]) > CONVERGENCE_THRESHOLD){
					converged = 0;	
				}
			}
		}

		MPI_Reduce(&converged, &global_convergence, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
		if(global_convergence != 0){
			done = 1;
		}	

		MPI_Bcast(&done, 1, MPI_INT, 0, MPI_COMM_WORLD);	

		if(done){
			break;
		}

		//swap input and output	
		temp = input_temp;
		input_temp = output_temp;
		output_temp = temp;
	}

	/*
	for(int k = 0; k < partition_size+2; k++){
		for(int l = 0; l < partition_size+2; l++){
			cout << input_temp[k][l] << " ";
		}
		cout << endl;
	}
	*/

	MPI_Finalize();
    				
    	for(int i = 0; i < partition_size+2; i++)
    	{
    		delete(input_temp[i]);
    		delete(output_temp[i]);
    	}
	delete(input_temp);
	delete(output_temp);

	return 0;	
}	

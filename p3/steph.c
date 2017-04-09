#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <string.h>

void print_result(int *r, int size);

int main(void)
{
	int i,
      j,
      k,
      size,
      comm_rank,
      comm_sz;
	char form[4],
	     flag[2];
	int *matrixA,
      *matrixB,
      *matrixC;
  double start, 
         end;

	// Start MPI, get the number of processes and the rank
	MPI_Init(NULL, NULL); 
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz); 
	MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);
  srand(time(NULL));

  if (comm_rank == 0) {	
    // Read input
		scanf("%s", form);
		scanf("%s", flag);
		scanf("%d", &size);

		// Allocate memory for a, b, and c matrices
		matrixA = (int*)calloc(size*size, sizeof(int));
		matrixB = (int*)calloc(size*size, sizeof(int));
		matrixC = (int*)calloc(size*size, sizeof(int));

		if (strcmp(flag,"I") == 0) {
			// Read in matrix a
			for (i = 0; i < size * size; i++)
					scanf("%i", &matrixA[i]);

			// Read in matrix b
			for (i = 0; i < size * size; i++)
					scanf("%i", &matrixB[i]);
		} else {
      // Generate random input
			for (i = 0; i < size * size; i++)
			{
				matrixA[i] = rand() % 101;
				matrixB[i] = rand() % 101;
			}
		}

		printf("running on %d processors\n", comm_sz);
	}

	// Synchronize
	MPI_Barrier(MPI_COMM_WORLD);

  // Set start time
	if (comm_rank == 0) start = MPI_Wtime();

	// Broadcast size of matrix to all processes
	MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);

	// Broadcast form  to all processes
	MPI_Bcast(&form, 4, MPI_CHAR, 0, MPI_COMM_WORLD);

	if (comm_rank != 0) {
    matrixB = (int*)calloc(size*size, sizeof(int));
    matrixC = (int*)calloc(size*size, sizeof(int));
	}

	// Broadcast b to all processes
	MPI_Bcast(matrixB, size * size, MPI_INT, 0, MPI_COMM_WORLD);
	
  // Variables to hold number of elements for each process
  int *elem_per_p = (int*)calloc(comm_sz, sizeof(int));
	int *dis = (int*)calloc(comm_sz, sizeof(int));
  int left_overs = size%comm_sz;
  int sum = 0;

  // Distribute elements
	for (i = 0; i < comm_sz; i++) {
		elem_per_p[i] = (size / comm_sz) * size;
    if (left_overs > 0) {
      elem_per_p[i]++;
      left_overs--;
    }
    dis[i] = sum;
    sum += elem_per_p[i];
  }
	
  // Malloc local_a and local_c	
  int *localMatrixA = (int*)calloc(elem_per_p[comm_rank], sizeof(int));
	int *localMatrixC = (int*)calloc(elem_per_p[comm_rank], sizeof(int));
	
	// Scatter a to all processes
	MPI_Scatterv(matrixA, elem_per_p, dis, MPI_INT, localMatrixA, elem_per_p[comm_rank], MPI_INT, 0, MPI_COMM_WORLD);
	
  // Preform appropriate calcultaion
	if (strcmp(form, "ijk") == 0) {
    sum = 0;
		for (i = 0; i < (elem_per_p[comm_rank] / size); i++) {
			for (j = 0; j < size; j++) {
				sum = 0;
				for (k = 0; k < size; k++) {
					sum += localMatrixA[i * size + k] * matrixB[k * size + j];
				}
        localMatrixC[i * size + j] = sum;
			}
		}
	} /*else if (strcmp(form, "ikj") == 0) {
		int num = 0;
    // Zero out local_c
		for (i = 0; i < elem_per_p[rank]; i++)
			local_c[i] = 0;

		for (i = 0; i < (elem_per_p[rank] / size); i++)
		{
			for (k = 0; k < size; k++)
			{
				num = local_a[i * size + k];
				for (j = 0; j < size; j++)
				{
					local_c[i * size + j] = local_c[i * size + j] + num * b[k * size + j];
				}
			}
		}
	}
	else if (strcmp(form, "kij") == 0)
	{
		int num = 0;

    // Zero out local_c
		for (i = 0; i < elem_per_p[rank]; i++)
			local_c[i] = 0;

		for (k = 0; k < size; k++)
		{
			for (i = 0; i < (elem_per_p[rank] / size); i++)
			{
				num = local_a[i * size + k];
				for (j = 0; j < size; j++)
				{
					local_c[i * size + j] = local_c[i * size + j] + num * b[k * size + j];
				}
			}
		}
	}*/
	MPI_Gatherv(localMatrixC, elem_per_p[comm_rank], MPI_INT, matrixC, elem_per_p, dis, MPI_INT, 0, MPI_COMM_WORLD);

	if (comm_rank == 0) {
		// Record the Finish Time
		end = MPI_Wtime();
		printf("elapsed time = %.6e seconds\n", end - start);
		if (strcmp(flag, "I") == 0)
			print_result(matrixC, size);
	}

	// Free Mem 	
 	free(matrixB);
 	free(matrixC);
 	free(elem_per_p);
 	free(dis);
 	free(localMatrixA);
 	free(localMatrixC);
 	/* Shut down MPI */
	MPI_Finalize();
 	return 0;
}

/**
 * Prints the contents of a Matrix
 * @param int* m 			The matrix that will be printed
 * @param int matrixSize	Size of the matrix
 */
void print_result(int *c, int size)
{
  int i = 0, j = 0;
	for (i = 0; i < size; i++) {
		for (j = 0; j < size; j++) {
			printf("%d ", c[i * size + j]);
    }
		printf("\n");
	}
}


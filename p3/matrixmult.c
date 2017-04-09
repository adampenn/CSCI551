#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <string.h>

void print_result(int *matrix, int size);
void get_input(char* form, char* flag, int* size);
void input_fill(int* matrix, int size);
void random_fill(int* matrix, int size);
void calculate_ijk(int* matrixA, int* matrixB, int* matrixC, int sendCount, int size);
void calculate_ikj(int* matrixA, int* matrixB, int* matrixC, int sendCount, int size);
void calculate_kij(int* matrixA, int* matrixB, int* matrixC, int sendCount, int size);

/*
 * Calculates the dot product of two matrixs
 *
 * Lets the user choose between three forms for calculating the dot product and
 * weather they want to enter the values or have them randomly generated.
 *
 * @return void    Nothing is returned
 */
int main() {

  // Declare variables for program
  int  i,
       size,
       comm_rank,
       comm_sz,
       *matrixA,
       *matrixB,
       *matrixC,
       *sendCount,
       *displs,
       extras,
       sum;
	char form[4],
	     flag[2];
  double start, 
         end;

  // Start MPI
  MPI_Init(NULL, NULL); 

  // Get the number of proccesses running
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

  // Get the current process's rank 
  MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);

  srand(time(NULL));

  // Do some setup if you are process 0
  if (comm_rank == 0) {	
  
    get_input(form, flag, &size);

		matrixA = (int*)calloc(size*size, sizeof(int));
		matrixB = (int*)calloc(size*size, sizeof(int));
		matrixC = (int*)calloc(size*size, sizeof(int));

    // Fill in matrixs
    if (strcmp(flag, "I") == 0) {
      input_fill(matrixA, size);
      input_fill(matrixB, size);
    } else {
      random_fill(matrixA, size);
      random_fill(matrixB, size);
    }

		printf("running on %d processors\n", comm_sz);
	}

	// Line up to start timing
	MPI_Barrier(MPI_COMM_WORLD);

  // Set start time
	if (comm_rank == 0) start = MPI_Wtime();

  // Send size and form to the other processes
  MPI_Bcast(&form, 4, MPI_CHAR, 0, MPI_COMM_WORLD);
	MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);

	if (comm_rank != 0) {
    matrixB = (int*)calloc(size*size, sizeof(int));
    matrixC = (int*)calloc(size*size, sizeof(int));
	}

	
  // Variables to hold number of elements for each process
  sendCount = (int*)calloc(comm_sz, sizeof(int));
  displs = (int*)calloc(comm_sz, sizeof(int));
  extras = size%comm_sz;
  sum = 0;

  // displstribute elements
	for (i = 0; i < comm_sz; i++) {
		sendCount[i] = (size / comm_sz) * size;
    if (extras > 0) {
      sendCount[i]++;
      extras--;
    }
    displs[i] = sum;
    sum += sendCount[i];
  }
	
  int *localMatrixA = (int*)calloc(sendCount[comm_rank], sizeof(int));
	int *localMatrixC = (int*)calloc(sendCount[comm_rank], sizeof(int));
	
	// Send matrixA and B to the other procceses
  MPI_Bcast(matrixB, size * size, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatterv(matrixA, sendCount, displs, MPI_INT, localMatrixA, sendCount[comm_rank], MPI_INT, 0, MPI_COMM_WORLD);
	
  // Calculate dot product
	if (strcmp(form, "ijk") == 0) {
    calculate_ijk(localMatrixA, matrixB, localMatrixC, sendCount[comm_rank], size);
  } else if (strcmp(form, "ikj") == 0) {
    calculate_ikj(localMatrixA, matrixB, localMatrixC, sendCount[comm_rank], size);
  }	else if (strcmp(form, "kij") == 0) {
    calculate_kij(localMatrixA, matrixB, localMatrixC, sendCount[comm_rank], size);
	}
	MPI_Gatherv(localMatrixC, sendCount[comm_rank], MPI_INT, matrixC, sendCount, displs, MPI_INT, 0, MPI_COMM_WORLD);

	if (comm_rank == 0) {
		end = MPI_Wtime();
		printf("elapsed time = %.6e seconds\n", end - start);
		if (strcmp(flag, "I") == 0)
			print_result(matrixC, size);
	}

	// Delete the memory when we are done with it
 	free(matrixB);
 	free(matrixC);
 	free(sendCount);
 	free(displs);
 	free(localMatrixA);
 	free(localMatrixC);

 	// Close MPI
	MPI_Finalize();
  
 	return 0;
}

/*
 * Prints a matrix
 *
 * Given a matrix it will print it with spaces between the indexs and new lines
 * between the rows
 *
 * @param  matrix  The matrix to print out
 * @param  size    The size of the matrix, only need one size sinces its n x n
 * @return void    Nothing is returned
 */
void print_result(int *matrix, int size)
{
  int i, j;
	for (i = 0; i < size; i++) {
		for (j = 0; j < size; j++) {
			printf("%d ", matrix[i * size + j]);
    }
		printf("\n");
	}
}

/*
 * Takes input from the user
 *
 * Gets input from the user about how they want to program ran.
 *
 * @param  form  Which form to calculate the dot product with
 * @param  flag  Wether the matrixs have random input or if the input is 
 *               given
 * @param  size  The size of the matrix, only need one size sinces its n x n
 * @return void  Nothing is returned
 */
void get_input(char* form, char* flag, int* size) {
  scanf("%s", form);
  scanf("%s", flag);
  scanf("%d", size);
}

/*
 * Fills in a matrix
 *
 * Takes the given matrix and fills it with input from the user.
 *
 * @param  matrix  The matrix to fill
 * @param  size    The size of the matrix, only need one size sinces its n x n
 * @return void    Nothing is returned
 */
void input_fill(int* matrix, int size) {
  int i;
  for (i = 0; i < size * size; i++) {
    scanf("%d", &matrix[i]);    
  }
}

/*
 * Fills in a matrix
 *
 * Takes the given matrix and fills it with random values. Assumes that random
 * was already seeded
 *
 * @param  matrix  The matrix to fill
 * @param  size    The size of the matrix, only need one size sinces its n x n
 * @return void    Nothing is returned
 */
void random_fill(int* matrix, int size) {
  int i;
  for (i = 0; i < size * size; i++) {
    matrix[i] = rand() % 101;
  }
}

/*
 * Calculates the dot product in IJK form
 *
 * Takes in three matrixs and calculates the dot product of the first two and stores
 * the reuslt into the third one.
 *
 * @param  matrixA    One of the matrixs being multiplied
 * @param  matrixB    One of the matrixs being multiplied
 * @param  matrixC    The matrix that holds the calculated dot product
 * @param  sendCount  The number of items (rows) that this function in calulating on
 * @param  size       The size of the matrix, only need one size sinces its n x n
 * @return void       Nothing is returned
 */
void calculate_ijk(int* matrixA, int* matrixB, int* matrixC, int sendCount, int size) {
  int i, j, k;
  for (i = 0; i < (sendCount/size); i++) {
    for (j = 0; j < size; j++) {
      matrixC[i*size+j] = 0;
      for (k = 0; k < size; k++) {
        matrixC[i*size+j] += matrixA[i*size+k] * matrixB[k*size+j];
      }
    }
  }
}

/*
 * Calculates the dot product in IKJ form
 *
 * Takes in three matrixs and calculates the dot product of the first two and stores
 * the reuslt into the third one.
 *
 * @param  matrixA    One of the matrixs being multiplied
 * @param  matrixB    One of the matrixs being multiplied
 * @param  matrixC    The matrix that holds the calculated dot product
 * @param  sendCount  The number of items (rows) that this function in calulating on
 * @param  size       The size of the matrix, only need one size sinces its n x n
 * @return void       Nothing is returned
 */
void calculate_ikj(int* matrixA, int* matrixB, int* matrixC, int sendCount, int size) {
  int i, j, k, tmp;
  for (i = 0; i < sendCount; i++) {
    matrixC[i] = 0;
  }
  for (i = 0; i < (sendCount/size); i++) {
    for (k = 0; k < size; k++) {
      tmp = matrixA[i*size+k];
      for (j = 0; j < size; j++) {
        matrixC[i*size+j] = matrixC[i*size+j] + tmp * matrixB[k*size+j];
      }
    }
  }
}

/**
 * Calculates the dot product in KIJ form
 *
 * Takes in three matrixs and calculates the dot product of the first two and stores
 * the reuslt into the third one.
 *
 * @param  matrixA    One of the matrixs being multiplied
 * @param  matrixB    One of the matrixs being multiplied
 * @param  matrixC    The matrix that holds the calculated dot product
 * @param  sendCount  The number of items (rows) that this function in calulating on
 * @param  size       The size of the matrix, only need one size sinces its n x n
 * @return void       Nothing is returned
 */
void calculate_kij(int* matrixA, int* matrixB, int* matrixC, int sendCount, int size) {
  int i, j, k, tmp;
  for (i = 0; i < sendCount; i++) {
      matrixC[i] = 0;    
  }
  for (k = 0; k < size; k++) {
    for (i = 0; i < (sendCount/size); i++) {
      tmp = matrixA[i*size+k];
      for (j = 0; j < size; j++) {
        matrixC[i * size + j] = matrixC[i * size + j] + tmp * matrixB[k * size + j];
      }
    }
  }
}

/* Adam J. Penn
 * Matrix Multiplication
 * CSCI 551
 * Febuary 13, 2017
 * CSU Chico
 */

#include "stdlib.h"
#include "stdio.h"
#include "time.h"
#include "mpi.h"
#include "string.h"

void setup(int* matrixA, int* matrixB, char *flag, char* form, int* size);
void calculate_dot(int* matrixA, int* matrixB, int* matrixC, int *size, char* form, int rows);
void print_result(int* matrixC, int *size, char *flag, double runTime);

int main() {
  //declaring variables
  int *size = (int*)calloc(1, sizeof(int));
  int my_rank, num_processes, *matrixA, *matrixB, *matrixC;
	char flag[2], form[4];
  double start, end;

  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

  int length = (*size)*(*size);
  printf("Length: %d", length);
  matrixA = (int*)calloc(length, sizeof(int));
  matrixB = (int*)calloc(length, sizeof(int));
  matrixC = (int*)calloc(length, sizeof(int));

  // Call the setup function to init the variables and read input
  if(my_rank == 0) {
    setup(matrixA, matrixB, flag, form, size);
    printf("\nrunning on %d cores.\n", num_processes);
	}
 
  MPI_Barrier(MPI_COMM_WORLD);
  if (my_rank == 0) { start = MPI_Wtime(); }
   
  // If we are not rank 0 we will need a local B and C
  // Figure out how to split up matrixB
  int i;
  int *fromB = (int*)malloc(num_processes * sizeof(int)); 
  if (my_rank == 0) {
    int leftOvers = ((*size)*(*size)) % num_processes;
    for (i = 0; i < num_processes; i++) {
      fromB[i] = (((*size) * (*size)) / num_processes);
      if (leftOvers > 0) {
        fromB[i]++;
        leftOvers--;
      }
      printf("Process %d fromB: %d\n", i, fromB[i]);
    }
  }
  
  printf("Before broadcasting fromB\n");
  MPI_Bcast(fromB, num_processes, MPI_INT, 0, MPI_COMM_WORLD);
  printf("Before broadcasting size\n");
  MPI_Bcast(size, 1, MPI_INT, 0, MPI_COMM_WORLD);
  printf("Before broadcasting form\n");
  MPI_Bcast(form, 4, MPI_CHAR, 0, MPI_COMM_WORLD);
  printf("Before broadcasting matrixA\n");
  MPI_Bcast(matrixB, (*size)*(*size), MPI_INT, 0, MPI_COMM_WORLD);
 
  int* localMatrixA = (int*)malloc(fromB[my_rank] * sizeof(int));
  
  printf("Before scattering matrixB\n");
  MPI_Scatter(matrixA, fromB[my_rank], MPI_INT, localMatrixA, fromB[my_rank], MPI_INT, 0, MPI_COMM_WORLD);
  printf("After scattering matrixB\n");
 
 
  
  // Make sure all the processes have what they need

  matrixC = (int*)malloc((*size) * (*size) * sizeof(int));
  int* localMatrixC = (int*)malloc(fromB[my_rank] * sizeof(int));

  printf("\nlocalB:\n");
  for (i = 0; i < fromB[my_rank]; i++) {
    printf("%d ", localMatrixA[i]);
  }
  printf("\n");
  
  if(my_rank == 0) {
    printf("\nB:\n");
    for (i = 0; i < (*size)*(*size); i++) {
      printf("%d ", matrixA[i]);
    }
    printf("\n");
  }


  //calulate the dot product
  printf("Before calling calcullate dot\n");
  calculate_dot(localMatrixA, matrixB, localMatrixC, size, form, fromB[my_rank]);
  printf("After calling calcullate dot\n");

  MPI_Gather(localMatrixC, fromB[my_rank], MPI_INT, matrixC,
             fromB[my_rank], MPI_INT, 0, MPI_COMM_WORLD);

  if (my_rank == 0) {
    end = MPI_Wtime();
    print_result(matrixC, size, flag, end-start);
  }
  
  MPI_Finalize();
 
  return 0;
}

void setup(int* matrixA, int* matrixB, char *flag, char* form, int* size) {
  // Setup the random seed
  srand(time(0));

  // Get input from user
  scanf("%3s", form);
  scanf("%s", flag);
  scanf("%d", size);
  
  int length = (*size) * (*size);

  //Setup matrixs
  int i;
  if (*flag == 'R') {
    for (i = 0; i < length; i++) {
      matrixA[i] = rand() % 101;
      matrixB[i] = rand() % 101;
    }
  } else if (*flag == 'I') {
    int input;
    // Input for A
    for (i = 0; i < (*size) * (*size); i++) {
        if(!scanf("%d", &input)){};
        matrixA[i]= input;
    }
    // Input for B
    for (i = 0; i < (*size) * (*size); i++) {
    //for (i = 0; i < (*size); i++) {
      //for (j = 0; j < (*size); j++) {
        if(!scanf("%d", &input)){};
        matrixB[i] = input;
      //}
    }
  }
}

/**
 * Calculates the dot product
 *
 * Takes in three matrixs and calculates the dot product of the first two and stores
 * the reuslt into the third one.
 *
 * @param  matrixA    One of the matrixs being multiplied
 * @param  matrixB    One of the matrixs being multiplied
 * @param  matrixC    The matrix that holds the calculated dot product
 * @param  size       The size of the matrix, only need one size sinces its n x n
 * @return void 			Nothing is returned
 */
void calculate_dot(int* matrixA, int* matrixB, int* matrixC, int* size, 
                   char* form, int rows)
{
  int i, j, k;
  
  // Calculate Dot product
  if (strcmp(form, "ijk") == 0) {
    for (i = 0; i < rows/(*size); i++) {
      for (j = 0; j < (*size); j++) {
        matrixC[i*(*size)+j] = 0;
        for (k = 0; k < (*size); k++) {
          matrixC[i*(*size)+j] += matrixA[i*(*size)+k] * matrixB[k*(*size)+j];
          //printf("%d ", matrixC[i*(*size)+j]);
        }
      }
    }
    /*
    for (i = 0; i < size; i++) {
      printf("%d ", matrixB[i]);
    }
    printf("\n");
    for (i = 0; i < size; i++) {
      for (j = 0; j < size; j++) {
        printf("%d ", matrixA[i+j]);
      }
    printf("\nC:\n");
    }
    int i;
    for (i = 0; i < size; i++) {
        printf("%d ", matrixC[i]);
    }
    printf("\n");
    */
  }
}

/**
 * Prints results
 *
 * Takes in the calculated dot product and prints it to the screen nicely formated
 *
 * @param  matrixC    The matrix that holds the calculated dot product
 * @param  size       The size of the matrix, only need one size sinces its n x n
 * @param  runTime    The calculated runtime of the program
 * @return void 			Nothing is returned
 */
void print_result(int* matrixC, int *size, char *flag, double runTime) {
	// Print out C
  int i;
	
  if ((*flag) == 'I') {
	  for (i = 0; i < (*size) * (*size); i++) {
      printf("%d ", matrixC[i]);
      if ((i+1) % (*size) == 0) {
	  	  printf("\n");
      }
	  }
  }
  printf("\nTime to complete dot product: %f\n", runTime);
}

#include "stdlib.h"
#include "mpi.h"
#include "time.h"
#include "string.h"
#include "stdio.h"

void print_matrix(int* matrix, int size, int time);

void get_input(char* form, char* flag, int* size, int comm_rank);

void random_fill(int* matrixA, int* matrixB, int size);

void input_fill(int* matrixA, int* matrixB, int size);

void calculate_ijk(int* matrixA, int* matrixB, int* matrixC, int size, int sendSize);

int main() {

  // Variables to be used with MPI
  int comm_sz, comm_rank;

  // Start MPI
  MPI_Init(NULL, NULL);

  // Get the number of processes running
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

  // Get the current process's rank
  MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);

  // Declare varibales for program
  char *form = (char*)calloc(4, sizeof(char)),
       *flag = (char*)calloc(2, sizeof(char));
  int  size,
       start,
       end,
       *matrixA = malloc(size*size*sizeof(int)),
       *matrixB = malloc(size*size*sizeof(int)),
       *matrixC = malloc(size*size*sizeof(int));

  // Get inital input from the user
  get_input(form, flag, &size, comm_rank);


  // Fill in the matrixs
  if (comm_rank == 0) {
    
    printf("Form: %s, Flag: %s, Size: %d\n", form, flag, size);
    
    if (strcmp(flag, "R") == 0) {
      random_fill(matrixA, matrixB, size);
    } else if (strcmp(flag, "I") == 0) {
      input_fill(matrixA, matrixB, size);
    }

    printf("running on %d processors\n", comm_sz);
  }
  int *sendcounts = malloc(comm_sz * sizeof(*sendcounts));

  // Create barrier and start timer
  MPI_Barrier(MPI_COMM_WORLD);
  if (comm_rank == 0) start = MPI_Wtime();

  // Calculate how much of matrixA to send
  int *displs = malloc(comm_sz * sizeof(*displs));

  // calculate send counts and displacements
  int extra = size%comm_sz, sum = 0;
  for (int i = 0; i < size; i++) {
    sendcounts[i] = size*size/comm_sz;
    if (extra > 0) {
      sendcounts[i]++;
      extra--;
    }

    displs[i] = sum;
    sum += sendcounts[i];
  } 

  // Send matrixA and matrixB to all processors
  MPI_Bcast(matrixB, size*size, MPI_INT, 0, MPI_COMM_WORLD);
  //MPI_Scatterv(matrixA, sendcounts, displs, MPI_INT, localMatrixA, sendcounts[comm_rank], MPI_INT, 0, MPI_COMM_WORLD);
  if (comm_rank == 0) {
    MPI_Scatter(matrixA, size*size/comm_sz, MPI_INT, MPI_IN_PLACE, size*size/comm_sz, MPI_INT, 0, MPI_COMM_WORLD);
  } else {
    MPI_Scatter(matrixA, size*size/comm_sz, MPI_INT, &matrixA[displs[comm_rank]], size*size/comm_sz, MPI_INT, 0, MPI_COMM_WORLD);

  }


  // Calculate dot product
  calculate_ijk(matrixA, matrixB, matrixC, size,
                sendcounts[comm_rank]/comm_sz);

  // Gather the results back to process 0
  //MPI_Gatherv(localMatrixC, sendcounts[comm_sz], MPI_INT, matrixC, sendcounts, displs, MPI_INT, 0, MPI_COMM_WORLD);
  if (comm_rank == 0) {
    MPI_Gather(MPI_IN_PLACE, size*size/comm_sz, MPI_INT, matrixC, size*size/comm_sz, MPI_INT, 0, MPI_COMM_WORLD);
  } else {
    MPI_Gather(&matrixC[displs[comm_rank]], size*size/comm_sz, MPI_INT, matrixC, size*size/comm_sz, MPI_INT, 0, MPI_COMM_WORLD);

  }
  // Print the result
  if (comm_rank == 0) {
    end = MPI_Wtime();
    print_matrix(matrixC, size, end-start);
  }

  MPI_Finalize();

}

void print_matrix(int* matrix, int size, int time) {
  int i, j;
  for (i = 0; i < size; i++) {
    for (j = 0; j < size; j++) {
      printf("%d ", matrix[i+j]);
    }
    printf("\n");
  }

  if(time != -1) printf("runtime %d\n", time); 

}

void get_input(char* form, char* flag, int* size, int comm_rank) {
  // Only setup if comm_rank is 0
  if (comm_rank == 0) {
    scanf("%s", form);
    scanf("%s", flag);
    scanf("%d", size);
  }

  // Broadcast variables
  MPI_Bcast(form, 4, MPI_CHAR, 0, MPI_COMM_WORLD);
  MPI_Bcast(flag, 2, MPI_CHAR, 0, MPI_COMM_WORLD);
  MPI_Bcast(size, 1, MPI_INT, 0, MPI_COMM_WORLD);
}

void random_fill(int* matrixA, int* matrixB, int size) {
  // Declare variables and seed random generator
  srand(time(0));
  int i;
  // Fill matrix
  for (i = 0; i < size*size; i++) {
    matrixA[i] = rand() % 101;
    matrixB[i] = rand() % 101;
  }
}

void input_fill(int* matrixA, int* matrixB, int size) {
  // Declare variables and seed random generator
  int i;
 
  // Fill matrix
  for (i = 0; i < size*size; i++) {
    scanf("%d", &matrixA[i]);
    scanf("%d", &matrixB[i]);
  }
}

void calculate_ijk(int* matrixA, int* matrixB, int* matrixC, int size, int sendSize) {
  // Declare variables
  int i, j, k;

  // Calculate dot product using ijk form
  for (i = 0; i < sendSize; i++) {
    for (j = 0; j < size; j++) {
      matrixC[i*size+j] = 0;
      for (k = 0; k < size; k++) {
        matrixC[i*size+j] += matrixA[i*size+k] * matrixB[k*size+j];
      }
    }
  }
}











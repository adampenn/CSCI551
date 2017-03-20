/* Adam J. Penn
 * Matrix Multiplication
 * CSCI 551
 * Febuary 13, 2017
 * CSU Chico
 */

#include "stdlib.h"
#include "stdio.h"
#include "time.h"

#define COCKS_PER_SEC CLOCKS_PER_SEC;

void calculate_dot(int** matrixA, int** matrixB, int** matrixC, int size);
void print_result(int** matrixC, int size, char flag, double runTime);

int main()
{
  //seed random value based off of time
  srand(time(0));
 
  //declaring variables
  int size, i, j, k, dot;
	char flag;
  clock_t start, end;
  double runTime;
 
	if(!scanf("%c", &flag)){}
  if(!scanf("%d", &size)){};
  start = clock();
	int** matrixA = (int**)malloc(size * sizeof(int*));
	int** matrixB = (int**)malloc(size * sizeof(int*));
	int** matrixC = (int**)malloc(size * sizeof(int*));

	for (i  = 0; i < size; i++) {
    matrixA[i] = (int*)malloc(size * sizeof(int));
    matrixB[i] = (int*)malloc(size * sizeof(int));
    matrixC[i] = (int*)malloc(size * sizeof(int));
	}

  // if random was chosen populate the matrixs randomly
  if (flag == 'R') {
    for (i = 0; i < size; i++) {
      for (j = 0; j <size; j++) {
        matrixA[i][j] = rand() % 101;
        matrixB[i][j] = rand() % 101;
      }
    }
	// else take input for matrixs	
  } else {
	  // Input for A
    for (i = 0; i < size; i++) {
      for (j = 0; j < size; j++) {
        int input;
        if(!scanf("%d", &input)){};
        matrixA[i][j] = input;
      }
    }
	  // Input for B
    for (i = 0; i < size; i++) {
      for (j = 0; j < size; j++) {
        int input;
        if(!scanf("%d", &input)){};
        matrixB[i][j] = input;
      }
    }
  }

  calculate_dot(matrixA, matrixB, matrixC, size);
  print_result(matrixC, size, flag, runTime);

  end = clock();
  runTime = ((double) (end - start)) / COCKS_PER_SEC;
 
  return 0;
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
void calculate_dot(int** matrixA, int** matrixB, int** matrixC, int size) {
  
  int i, j, k, dot;
  
  // Calculate Dot product
  for (i = 0; i < size; i++) {
    for (j = 0; j < size; j++) {
      dot = 0;
      for (k = 0; k < size; k++) {
        dot = dot + (matrixA[i][k] * matrixB[k][j]);
      }
      matrixC[i][j] = dot;
    }
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
void print_result(int** matrixC, int size, char flag, double runTime) {
	// Print out C
  int i, j;
	
  if (flag == 'I') {
	  for (i = 0; i < size; i++) {
      for (j = 0; j < size; j++) {
        printf("%d ", matrixC[i][j]);
      }
	  	printf("\n");
	  }
  }
  printf("Time to complete dot product: %f\n", runTime);
}


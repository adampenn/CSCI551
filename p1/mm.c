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

int main()
{

  int size, i, j, k, dot;
	char flag;
  clock_t start, end;
  double runTime;
 
  //seed random value based off of time
  srand(time(0));
  
	if(!scanf("%c", &flag)){}
  if(!scanf("%d", &size)){};
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

  start = clock();
  
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
 
  end = clock();
  runTime = ((double) (end - start)) / COCKS_PER_SEC;
 
	// Print out C
	if (flag == 'I') {
	  for (i = 0; i < size; i++) {
      for (j = 0; j < size; j++) {
        printf("%d ", matrixC[i][j]);
      }
	  	printf("\n");
	  }
  }

  printf("Time to complete dot product: %f\n", runTime);

  return 0;
}


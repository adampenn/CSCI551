#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void srand48();
double drand48();

// define as 1 to print out debug messages
#define debug 0

void fill_matrix(int size, double **matrix);
void forward_elimination(int size, double **matrix);
void print_matrix(int size, double **matrix);
void back_substitution(int size, double **matrix, double* result);
void print_results(int size, double* result, struct rusage usage);

/*
 * Does Gaussian Elimination on a matrix
 *
 * Gets input from the user if the size is less then 5x5 and does Gaussian
 * elimitation on the matrix. Prints the result to stdout.
 *
 * @return int  returns 0 if no errors occured
 */
int main() {
  // Declare variables for program
  int    size;
  double **matrix,
         *result;
  struct rusage usage;
  
  // Seed random number generator
  srand48(time(NULL));

  // Read in size
  scanf("%d", &size);
  
  // Create the matrixs we need based off of the size given
  matrix = (double**)calloc(size, sizeof(double*));
  result = (double*)calloc(size, sizeof(double));
  
  fill_matrix(size, matrix);

  #if debug
    printf("Printing orignal matrix\n");
    print_matrix(size, matrix);
  #else
    if (size <= 4)
      print_matrix(size, matrix);
  #endif
  
  forward_elimination(size, matrix);
  back_substitution(size, matrix, result);

  getrusage(RUSAGE_SELF, &usage); 

  print_results(size, result, usage);

  // Delete the memory when we are done with it
  //for (i = 0; i < size; i++)
  //  free(matrix[i]);
  //free(matrix);

  return 0;
}

/*
 * Prints the matrix
 *
 * Prints the given matrix with spaces between the indexs and a new line
 * between rows.
 *
 * @param  size    The size of the matrix ( size+1 incudes the b part )
 * @param  matrix  The matrix that needs to be printed
 * @return void    Nothing is returned
 */
void print_matrix(int size, double **matrix) {
  int i, j;
  for (i = 0; i < size; i++) {
    for (j = 0; j < size+1; j++)
      printf("%lf ", matrix[i][j]);
    printf("\n");
  }
}

/*
 * Applies forward elimination
 *
 * Takes the given matrix and does forward elimination on it so that it is
 * ready to be given to back_substitution so that we can get an answer.
 *
 * @param  size    The size of the matrix ( size+1 incudes the b part )
 * @param  matrix  The matrix that needs to have forward elimination done
 * @return void    Nothing is returned
 */
void forward_elimination(int size, double **matrix) {
  int i, j, k, swap, rowToSwap;
  double *temp, multiplier, diagnalValue;
  for (i = 0; i < size-1; i++) {
    diagnalValue = fabs(matrix[i][i]);
    #if debug
      printf("Diagnal Value: %lf\n", diagnalValue);
    #endif
    swap = 0;
    for (j = i+1; j < size; j++) {
      if(fabs(matrix[j][i] > diagnalValue)) {
        swap = 1;
        rowToSwap = j;
        #if debug
          printf("Should swap row %d with row %d\n", i, rowToSwap);
        #endif
      }
    }
    if (swap) {
      temp = matrix[i];
      matrix[i] = matrix[rowToSwap];
      matrix[rowToSwap] = temp;
      #if debug
        printf("Printing matrix after first swap\n");
        print_matrix(size, matrix);
      #endif
    }
    for (j = i+1; j < size; j++) {
      multiplier = matrix[j][i] / matrix[i][i];
      #if debug
        printf("multiplier: %lf\n", multiplier);
      #endif
      for (k = 0; k < size+1; k++)
        matrix[j][k] -= multiplier * matrix[i][k];
    }
    #if debug
      printf("Printing matrix after elimination\n");
      print_matrix(size, matrix);
    #endif
  }
}

/*
 * Fills in the matrix
 *
 * Decides whether to randomly fill in the matrix or to get input from the user
 * based of of how big n is.
 *
 * @param  size    The size of the matrix ( size+1 incudes the b part )
 * @param  matrix  The matrix that needs to be filled  
 * @return void    Nothing is returned
 */
void fill_matrix(int size, double **matrix) {
  int i, j;
  for (i = 0; i < size+1; i++)
    matrix[i] = calloc(size, sizeof(double));
  if (size <= 4)
    for (i = 0; i < size; i++)
      for (j = 0; j < size+1; j++)
      scanf("%lf", &matrix[i][j]); 
  else
    for (i = 0; i < size; i++)
      for (j = 0; j < size+1; j++)
        matrix[i][j] = drand48();
}

/*
 * Does the back substitution
 *
 * From the given matrix it preforms back subsititution, assumes that eh matrix
 * has already had forward elimitation done to it.
 *
 * @param  size    The size of the matrix ( size+1 incudes the b part )
 * @param  matrix  The matrix that needs to be solved
 * @param  result  The array to store the answer in
 * @return void    Nothing is returned
 */

void back_substitution(int size, double **matrix, double* result) {
  int i, j;
  
  // Deal with the case where there is only one variable
  result[size-1] = matrix[size-1][size] / matrix[size-1][size-1];

  for (i = size-2; i >= 0; i--) {
    for (j = i+1; j < size; j++)
      result[i] += matrix[i][j] * result[j];
    result[i] = (matrix[i][size] - result[i]) / matrix[i][i];
  }
}

void print_results(int size, double* result, struct rusage usage) {
  int i;
  double time = usage.ru_utime.tv_sec + usage.ru_utime.tv_usec/1000000;
  printf("User CPU Time: %lf\n", time);
  
  for (i = 0; i < size; i++)
    printf("%lf ", result[i]);
  printf("\n");
}


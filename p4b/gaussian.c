/*
 * @file gaussian.c
 * @author Adam J. Penn
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * This program does gaussian elimination on a matrixs A and b.
 * If the matrix is of size 5 or greater then it automatically
 * fills the matrix otherwise it takes input from the user.
 *
 */

#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <omp.h>

void srand48();
double drand48();

// Define as 1 to print out debug messages
#define debug 0

// init the variable that holds the number of threads
int threadCount = 0;


void fill_matrix(int size, double **matrix, double **orignalA);
void forward_elimination(int size, double **matrix);
void print_matrix(int size, double **matrix);
void back_substitution(int size, double **matrix, double *result);
void print_results(int size, double *result, struct rusage usage, double norm,
                   double runtime);
double calculate_euclidean(int size, double **matrix, double *result);

/*
 * Does Gaussian Elimination on a matrix
 *
 * Gets input from the user if the size is less then 5x5 and does Gaussian
 * elimination on the matrix. Prints the result to stdout.
 *
 * @return int  returns 0 if no errors occured
 */
int main(int argc, char **argv) {
  // Declare variables for program
  int    size,
         i,
         threadCount,
         j;
  double **matrix,
         **orignalA,
         *result,
         start,
         end,
         norm;
  struct rusage usage;

  // Start the timer
  start = omp_get_wtime();

  // Seed random number generator
  srand48(time(NULL));

  // Read in size
  if (argc != 2) {
    printf("NO SIZE GIVEN AS ARGUMENT\n");
    return 1;
  }
  size = atoi(argv[1]);
  #if debug
    printf("Size of matrix: %d\n", size);
  #endif

  // Set the number of threads
  #pragma omp parallel
    threadCount = omp_get_num_threads();
 
  //#if debug
      printf("Num of threads: %d\n", threadCount);
  //#endif
 
  // Create the matrixs we need based off of the size given
  matrix = (double**)_mm_malloc(size * sizeof(double*), 64);
  orignalA = (double**)_mm_malloc(size * sizeof(double*), 64);
  result = (double*)_mm_malloc(size * sizeof(double), 64);

  fill_matrix(size, matrix, orignalA);

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
  
  end = omp_get_wtime();

  norm = calculate_euclidean(size, orignalA, result);
  
  print_results(size, result, usage, norm, end - start);

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
      printf("%.10le ", matrix[i][j]);
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
void forward_elimination(int size, double ** restrict matrix) {
  int i, j, k, swap, rowToSwap;
  double *temp, multiplier, diagnalValue;

  #pragma vector aligned
  for (i = 0; i < size-1; i++) {
    diagnalValue = fabs(matrix[i][i]);
    #if debug
      printf("Diagnal Value: %lf\n", diagnalValue);
    #endif
    swap = 0;
    
    for (j = i+1; j < size; j++) {
      #if debug
        printf("Curent value: %lf Diagnal value: %lf\n", fabs(matrix[j][i]),
               diagnalValue);
      #endif
      if (fabs(matrix[j][i]) > diagnalValue) {
        swap = 1;
        rowToSwap = j;
        diagnalValue = fabs(matrix[j][i]);
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

    #pragma omp parallel for
    for (j = i+1; j < size; j++) {
      multiplier = matrix[j][i] / matrix[i][i];
      #if debug
        printf("multiplier: %lf\n", multiplier);
      #endif
      for (k = 0; k < size+1; k++)
        matrix[j][k] -= (multiplier * matrix[i][k]);
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
 * @param  size      The size of the matrix ( size+1 incudes the b part )
 * @param  matrix    The matrix that needs to be filled
 * @param  orignalA  The matrix to copy the orignal matrix into
 * @return void      Nothing is returned
 */
void fill_matrix(int size, double **__restrict__ matrix, double **__restrict__ orignalA) {
  int i, j;
  for (i = 0; i < size+1; i++) {
    matrix[i] = _mm_malloc(size * sizeof(double), 64);
    orignalA[i] = _mm_malloc(size * sizeof(double), 64);
  }
  if (size <= 4) {
    for (i = 0; i < size; i++)
      for (j = 0; j < size+1; j++) {
        if (!scanf("%lf", &matrix[i][j])) {
          printf("READING INDEX VALUE FAILED\n");
        }
        orignalA[i][j] = matrix[i][j];
      }
  } else {
    for (i = 0; i < size; i++)
      for (j = 0; j < size+1; j++)
        orignalA[i][j] = matrix[i][j] = drand48() * -2e6+ 1e6;
  }
}

/*
 * Does the back substitution
 *
 * From the given matrix it preforms back subsititution, assumes that eh matrix
 * has already had forward elimination done to it.
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

/*
 * Prints the results from the program running
 *
 * Takes the results and some usage stats and prints them for the user to see
 *
 * @param  size    The size of the reault matrix
 * @param  matrix  The matrix that holds the result from the gaussian 
 *                 elimination
 * @param  ussage  The struct that hold the usage stats
 * @return void    Nothing is returned
 */
void print_results(int size, double *result, struct rusage usage, double norm,
                   double runtime) {
  int i;
  printf("Run Time: %.6lf\n", runtime);
  printf("Max Resident Set: %ld\n", usage.ru_maxrss);
  printf("Minor Page Faults: %ld\n", usage.ru_minflt);
  printf("Major Page Faults: %ld\n", usage.ru_majflt);
  printf("l^2-norm: %.10le\n", norm);

  if (size <= 4) {
    for (i = 0; i < size; i++)
      printf("%.10le ", result[i]);
    printf("\n");
  }
}

/*
 * Calculates the euclidean norm
 *
 * Based off of the orginal matrix and the result matrix calculate the 
 * euclidean norm to see how much error was in your solution
 *
 * @param  size    The size of the reault matrix and the orginal matrix
 *                 ( size+1 incudes the b part )
 * @param  matrix  The matrix that holds the orginal A and b
 * @param  result  The matrix that holds the result from back subsititution
 * @return double  Returns the calculated euclidean norm
 */
double calculate_euclidean(int size, double **matrix, double *result) {
  int i, j, k;
  double norm = 0, *residule = (double*)_mm_malloc(size * sizeof(double), 64);

  #pragma vector aligned
  for (i = 0; i < size; i++) {
    for (j = 0; j < size; j++) {
      residule[i] = 0;
      for (k = 0; k < size; k++)
        residule[i] += matrix[i][k] * result[k];
    }
  }
  #if debug
    for (i = 0; i < size; i++)
      printf("%lf ", residule[i]);
    printf("\n");
  #endif

  for (i = 0; i < size; i++)
    norm += pow((residule[i]- matrix[i][size]), 2);
  #if debug
    printf("norm: %lf\n", norm);
  #endif
  return sqrt(norm);
}


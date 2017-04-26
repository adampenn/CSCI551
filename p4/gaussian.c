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
void back_substitution(int size, double **matrix, double *result);
void print_results(int size, double *result, struct rusage usage, double norm);
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
         j;
  double **matrix,
         **orignalA,
         *result,
         norm;
  struct rusage usage;

  // Seed random number generator
  srand48(time(NULL));

  // Read in size
  if (argc != 2) {
    printf("NO SIZE GIVEN AS ARGUMENT\n");
    return 1;
  }
  size = atoi(argv[1]);

  // Create the matrixs we need based off of the size given
  matrix = (double**)calloc(size, sizeof(double*));
  orignalA = (double**)calloc(size, sizeof(double*));
  result = (double*)calloc(size, sizeof(double));

  fill_matrix(size, matrix);

  // Make a copy of the matrix A
  for (i = 0; i < size; i++) {
    orignalA[i] = (double*)calloc(size+1, sizeof(double));
    for (j = 0; j < size+1; j++)
      orignalA[i][j] = matrix[i][j];
  }

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

  norm = calculate_euclidean(size, orignalA, result);

  print_results(size, result, usage, norm);

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
  if (size <= 4) {
    for (i = 0; i < size; i++)
      for (j = 0; j < size+1; j++)
        if (!scanf("%lf", &matrix[i][j])) {
          printf("READING INDEX VALUE FAILED\n");
        }
  } else {
    for (i = 0; i < size; i++)
      for (j = 0; j < size+1; j++)
        matrix[i][j] = drand48() * -2e6+ 1e6;
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
void print_results(int size, double *result, struct rusage usage, double norm) {
  int i;
  double userTime, systemTime;
  userTime = (double)usage.ru_utime.tv_sec +
             (double)usage.ru_utime.tv_usec / 1.0e6;
  systemTime = (double)usage.ru_stime.tv_sec +
               (double)usage.ru_stime.tv_usec / 1.0e6;
  printf("User CPU Time: %.6lf\n", userTime);
  printf("System CPU Time: %.6lf\n", systemTime);
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
  double norm = 0, *residule = calloc(size, sizeof(double));

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


#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int main() {
  int i, j, size = 3;
  double **matrix = (double**)calloc(size, sizeof(double*));
  double *result = (double*)calloc(size, sizeof(double));
  for (i = 0; i < size; i++)
    matrix[i] = calloc(size, sizeof(double));
  for (i = 0; i < size; i++)
    for (j = 0; j < size; j++)
      scanf("%lf", &matrix[i][j]); 
  for (i = 0; i < size; i++)
    scanf("%lf", &result[i]);

  int k;
  double *residule = calloc(size, sizeof(double));

  for (i = 0; i < size; i++) {
    for (j = 0; j < size; j++) {
      residule[i] = 0;
      for (k = 0; k < size; k++) {
        residule[i] += matrix[i][k] * result[k];
      }
    }
  }
  for (i = 0; i < size; i++) {
    printf("%lf ", residule[i]);
  }
  return 0;
}

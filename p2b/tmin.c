/* Adam J. Penn
 * Error in Numerical Integration
 * CSCI 551
 * March 1, 2017
 * CSU Chico
 */

#include "stdlib.h"
#include "stdio.h"
#include "time.h"
#include "math.h"
#include "mpi.h"

long double formula(long double slice);

long double integrate(long double a, long double b, long double t, int my_rank, int p);

int main() {
  
  // Decalare Variables
  int         my_rank, num_processes;
  long double a, b, n;
  long double absolute_error = .5 * pow(10, -14); // 0.5e-14
  long double current_error, partial_value, total_area,
              true_value = 4003.7209001513269868;
  double      start, end, total;
  
  // Start MPI
  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

  // Read in the values from the user
  if(my_rank == 0) {
    printf("Enter a, b, and n\n");
    scanf("%Lf%Lf%Lf", &a, &b, &n);
    printf("\nRunning on %d cores.\n", num_processes);
  }

  // Send out values to other porcesses / recive
  MPI_Bcast(&a, 1, MPI_LONG_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Bcast(&b, 1, MPI_LONG_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Bcast(&n, 1, MPI_LONG_DOUBLE, 0, MPI_COMM_WORLD);
   

  // Create a barrior for the processes to line up at
  MPI_Barrier(MPI_COMM_WORLD);
  
  // Start time timer for rank 0
  if (my_rank == 0) {
    start = MPI_Wtime();
  }

  // Start the integration
  partial_value = integrate(a, b, n, my_rank, num_processes);
 
  // Collect the results from the processes
  MPI_Reduce(&partial_value, &total_area, 1, MPI_LONG_DOUBLE, MPI_SUM, 0,
             MPI_COMM_WORLD); 

  // End the timer for rank 0
  if (my_rank == 0) {
    end = MPI_Wtime();
    total = end - start; 
  }

  // Output the results
  if (my_rank == 0) {
    current_error = fabs((true_value - total_area) / true_value);
    printf("Elapsed time = %.6e seconds\n", total);
    printf("With n = %.0Lf trapezoids, our estimate\n",n);
    printf("of the integral from %.6Lf to %.6Lf = %.13Le\n", a, b, total_area);
    printf("true value = %.19Le\n", true_value);
    printf("absolute relative true error = %.19Le\n", current_error);
    if (current_error < absolute_error) {
      printf("  is less than criteria = %.19Le\n", absolute_error);
    } else {
      printf("  is Not less than criteria = %.19Le\n", absolute_error);
    }
  }
  
  // End MPI
  MPI_Finalize();

  return 0;
}

/*
 * Calulates the value for the part of the curve passed in
 *
 * Takes in one value which represents the the part to be calulated and 
 * plugs it into the equation
 *
 * @param  slice        The value to be calculated
 * @return long double  The calulated value from the equation
 */
long double formula(long double slice) {
  return(cosl(slice/3)-2*cosl(slice/5)+5*sinl(slice/4)+8);
}

/*
 * Finds the area under the curve  
 *
 * Takes in the lower bound and the upper bound for the integration as well
 * as the number of trapazoids to divide it into
 *
 * @param  a            The lower bound
 * @param  b            The upper bound
 * @param  t            The number of trappazoids
 * @return long double  The calulated value from the equation
 */

long double integrate(long double a, long double b, long double n, int my_rank, int p) {
  // Decalare Variables
  long double h, slice, area, local_a, local_b;
  int i = 0, local_n;
  
  // Calculate local variables
  h = (b - a) / n;
  local_n = n/p;
  local_a = a + my_rank * local_n * h;
  local_b = local_a + local_n * h;
  
  // Calulate the area
  area = (formula(local_a) + formula(local_b)) / 2.0;
  for (i = 1; i < local_n; i++) {
    slice = local_a + i * h;
    area += formula(slice); 
  }
  area = h * area;

  // Return the calulated area
  return area;
}

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

long double formula(long double slice);

long double integrate(long double a, long double b, long double t);

int main() {
  // Decalare Variables
  long double a, b, t, increment = 1000;
  long double absolute_error = .5 * pow(10, -14); // 0.5e-14
  long double current_error, current_value,
              true_value = 4003.72090015132682659291;
  
  // Read in the values from the user
  printf("Enter lower, upper, number of traps to start with\n\n");
  scanf("%Lf%Lf%Lf", &a, &b, &t); // read in limits and start point
  
  // Account for the fact that it increments in the first itteration
  t -= increment;
  
  // Continue to increment t while error is higher then absolute error
  do {
    t += increment;
    current_value = integrate(a, b, t);

    // calulate error for current itteration
    current_error = fabs((true_value - current_value) / true_value);

    printf("T Value: %.0Lf   Integration Value: %4.13Le   Error Value: %0.19Le\n",
           t, current_value, current_error);

  } while (current_error > absolute_error);
  
  // Print out the final reults
  printf("Final T value: %.0Le\n", t);
  printf("Final Error Value: %.19Le\n", current_error);
  printf("Final Itegration Value: %4.13Le\n", current_value);

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

long double integrate(long double a, long double b, long double t) {
  // Decalare Variables
  long double h, slice, area = 0;
  int i = 0;
  
  // Calulate the area
  h = (b - a) / t;
  area = (formula(a) + formula(b)) / 2.0;
  for (i = 1; i < t; i++) {
    slice = a + i * h;
    area += formula(slice); 
  }
  area = h * area;

  // Return the calulated area
  return area;
}

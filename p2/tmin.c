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

int main()
{
  long double a, b; // the lower and upper limit respectivly limit
  long double absolute_error = .5 * pow(10, -14); // the absolute relative true error is 0.5e-14
  long double t; // the number of trapazoids to start with
  long double current_error;
  long double current_value, true_value = 4003.72090015132682659291;
  
  scanf("%Lf%Lf%Lf", &a, &b, &t); // read in limits and start point
  
  do {
    t += 1000;
    current_value = integrate(a, b, t);

    // calulate errog for current itteration
    current_error = fabs((true_value - current_value) / true_value);

    printf("T Value: %.0Lf   Integration Value: %4.13Lf   Error Value: %5.19Lf\n", t, current_value, current_error);

    // Loop ends when absolute true error is in range
  } while (current_error > absolute_error);

  printf("Final T value: %.0Lf\n", t);
  printf("Final Error Value: %.19Lf\n", current_error);
  printf("Final Itegration Value: %4.13Lf\n", current_value);

  return 0;
}

long double formula(long double slice) {
  return(cosl(slice/3)-2*cosl(slice/5)+5*sinl(slice/4)+8);
}

long double integrate(long double a, long double b, long double t) {
  long double h, slice, total = 0;
  int i = 0;
  
  h = (b - a) / t;
  total = (formula(a) + formula(b)) / 2.0;
  for (i = 1; i <= t-1; i++) {
    slice = a + i * h;
    total += formula(slice); 
  }
  total = h * total;

  return total;
}

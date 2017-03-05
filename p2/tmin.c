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

// integrate from 100 to 600 this function: cos(x/3)-2cos(x/5)+5sin(x/4)+8
long double formula(long double slice)
{
  long double value = cos(slice/3)-2*cos(slice/5)+5*sin(slice/4)+8;
  return(value);
}

int main()
{
  int i, t;
  double a, b, h;
  long double ans, so, se;
  scanf("%lf%lf%d",&a,&b,&t);
  if(!t%2==0) {
    t=t+1;
  }
  long double result[t], slice[t];
  h=(b-a)/t;
  for(i=0; i<=t; i++) {
    slice[i]=a+i*h;
    result[i]=formula(slice[i]);
  }
  so=0;
  se=0;
  for(i=1; i<t; i++) {
    if(i%2==1) {
      so=so+result[i];
    } else {
      se=se+result[i];
    }
  }
  ans=h/3*(result[0]+result[t]+4*so+2*se);
  printf("\n%4.20Le\n",ans);
  return 0;
}


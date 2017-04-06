#include "stdlib.h"
#include "stdio.h"
#include "time.h"
#include "mpi.h"
#include "string.h"

int main() {
  char form[4], flag[2];
  int *size = (int*)malloc(sizeof(int));
 
  scanf("%s", form);
  scanf("%s", flag);
  scanf("%d", size);

  printf("Form: %s, Flag: %s, Size: \n", form, flag);
}

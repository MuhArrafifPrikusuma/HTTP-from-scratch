#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[]) {
  volatile double a = 0.1;
  volatile double b = 0.2;
  volatile double sum = a + b;
  printf("%.18lf <- compile time. runtime -> %.18lf\n", 0.1 + 0.2, sum);
  return EXIT_SUCCESS;
}

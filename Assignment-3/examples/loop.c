/**
 * loop: add two vectors in parallel using dynamic load-balancing
 *
 * Usage: loop numberOfProcessors
 *
*/

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXLEN 1000

#define MAXPROC 8

int main(int argc, char *argv[]) {
  int numProcs, size;
  int a[MAXLEN], b[MAXLEN], c[MAXLEN];
  int i;

  /* Get thenumber of processors from the command line */
  if (argc < 2) {
    printf("Usage: loop numProcs\n");
    exit(1);
  }

  sscanf(argv[1], "%d", &numProcs);
  if (numProcs < 1 || numProcs > MAXPROC) {
    printf("Bad number of Processors (%d)\n", numProcs);
    exit(1);
  }

  /* Initialize the contexts of vector a and b */
  printf("Enter the size of the arrays: ");
  scanf("%d", &size);
  if (size < 0 || size > MAXLEN) {
    printf("Invalid length of vectors\n");
    exit(1);
  }

  printf("Enter the %d elements for vector a: ", size);
  for (i = 0; i < size; i++)
    if (scanf("%d", &a[i]) != 1) {
      printf("Fail to scan vector a\n");
      exit(1);
    }

  printf("Enter the %d elements for vector b: ", size);
  for (i = 0; i < size; i++)
    if (scanf("%d", &b[i]) != 1) {
      printf("Fail to scan vector b\n");
      exit(1);
    }

  /* Set the number of threads for the parallel region */
  omp_set_num_threads(numProcs);

/* Fork a team of threads to execute the for loop in parallel */
#pragma omp parallel for default(shared) private(i) schedule(dynamic)
  for (i = 0; i < size; i++) {
    c[i] = a[i] + b[i];

  } /* Implied barrier: all threads join master thread and terminate */

  printf("Result\n%10c%10c%10c\n", 'a', 'b', 'c');
  for (i = 0; i < size; i++)
    printf("%10d%10d%10d\n", a[i], b[i], c[i]);

  return 0;
}

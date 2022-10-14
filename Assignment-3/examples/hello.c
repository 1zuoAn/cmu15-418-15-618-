#include <omp.h>
#include <stdio.h>

int main() {
  int nthreads, tid;

  /* Check how many processors are available */
  printf("There are %d processors\n", omp_get_num_procs());

  /* Set the number of threads to 4 */
  omp_set_num_threads(4);

  /* Fork a team of threads giving them their own copies of variables */
  #pragma omp parallel private(nthreads, tid) 
  {
    /* Obtain and print thread id */
    tid = omp_get_thread_num();
    printf("Hello World from thread = %d\n", tid);

    /* Only master thread does this */
    if (tid == 0) {
      nthreads = omp_get_num_threads();
      printf("Number of threads = %d\n", nthreads);
    }

  } /* All threads join master thread and terminate */
}

/**
 * sqrt: calculate the square root of a set of random numbers
 *
 * Usage: sqrt numberOfProcessors
 *
 */

#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <sys/time.h>

#define MAXNUMBER 8

struct gm_t {
  int Index; /* Used by processors to find out which input to take */
  double input[MAXNUMBER];  /* Input vector */
  double output[MAXNUMBER]; /* Square root answer vector */

  omp_lock_t indexLock; /* Lock declaration for getting index. */
} * gm;

void SlaveStart(void);

int main(int argc, char *argv[]) {
  int i, numProcs;
  double number;
  struct timeval startInitTime, startCompTime, endCompTime;
  double totalTime, parallelTime;

  /* Record start of initialization time. */
  gettimeofday(&startInitTime, NULL);

  if (argc < 2) {
    printf("Usage: sqrt numProcs\n");
    exit(1);
  }

  /* Get thenumber of processors from the command line */
  sscanf(argv[1], "%d", &numProcs);
  if (numProcs < 1 || numProcs > MAXNUMBER) {
    printf("Bad number of Processors (%d)\n", numProcs);
    exit(1);
  }

  /* Allocate memory from heap (shared by default) */
  gm = (struct gm_t *)malloc(sizeof(struct gm_t));

  /* Initialise the index. */
  gm->Index = 0;

  /* Set up the input assignments */
  number = 1.0;
  for (i = 0; i < numProcs; i++) {
    number = number * 10.0;
    gm->input[i] = number;
  }

  /* Set the number of threads for the parallel region */
  omp_set_num_threads(numProcs);

  /* Initialize the lock */
  omp_init_lock(&gm->indexLock);

  /* Fork a team of threads */
  #pragma omp parallel
  {
    /* Only master thread does this */
    if (omp_get_thread_num() == 0)
      gettimeofday(&startCompTime, NULL);

    /* Every thread, including the master thread, executes SlaveStart */
    SlaveStart();
  }
  /* Implied barrier: all threads join master thread and terminate */

  /* Record final time. */
  gettimeofday(&endCompTime, NULL);

  /* Uninitialize the lock */
  omp_destroy_lock(&gm->indexLock);

  /* Output the result. */
  printf("\n");
  for (i = 0; i < numProcs; i++) {
    printf("The Square Root of %f is %f.\n", gm->input[i], gm->output[i]);
  }
  printf("\n");

  /* Print out total and parallel computation times. */
  parallelTime = (endCompTime.tv_sec - startCompTime.tv_sec) * 1000.0 +
                 (endCompTime.tv_usec - startCompTime.tv_usec) / 1000.0;

  totalTime = (endCompTime.tv_sec - startInitTime.tv_sec) * 1000.0 +
              (endCompTime.tv_usec - startInitTime.tv_usec) / 1000.0;

  printf("%d Processors:\n", numProcs);
  printf("   Parallel Time = %.3f millisecs\n", parallelTime);
  printf("      Total Time = %.3f millisecs\n", totalTime);

  /* Release memory on the heap */
  free(gm);

  return 0;
}

/**
 *  SlaveStart: this is the routine that all slaves execute after being born.
 *
 */
void SlaveStart() {
  int myIndex;

  /* Get an index, after first locking it. */
  omp_set_lock(&gm->indexLock);

  myIndex = gm->Index;
  gm->Index++;

  /* Unlock the Index */
  omp_unset_lock(&gm->indexLock);

  /* Do the work of square rooting */
  gm->output[myIndex] = sqrt(gm->input[myIndex]);

  /* Stop at the barrier to synchronize, not really necessary in this example */
  #pragma omp barrier
}

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

void transposition(int** matrix, int size) {
  int** result = malloc(size*sizeof(int*));
  for (int i=0; i<size; i++) {
    result[i] = malloc(size*sizeof(int));
    for (int j=0; j<size; j++) {
      result[i][j] = matrix[j][i];
    }
  }

  matrix = result;
}


int main(int argc, char* argv[]) {
  if (argc != 2) exit(1);

  int size = atoi(argv[1]);
  if (size<=0) exit(1);

  // initialize random arrays
  int** m1 = malloc(size*sizeof(int*));
  int** m2 = malloc(size*sizeof(int*));
  int** result = malloc(size*sizeof(int*));

  srand(time(NULL));
  for (int i=0; i<size; i++) {
    m1[i] = malloc(size*sizeof(int));
    m2[i] = malloc(size*sizeof(int));
    result[i] = malloc(size*sizeof(int));
    for (int j=0; j<size; j++) {
      m1[i][j] = rand();
      m2[i][j] = rand();
    }
  }
  
  printf("naive multiplication:\n");
  clock_t start = clock();
  //multiply
  for (int i=0; i<size; i++) {
    for (int j=0; j<size; j++) {
      int sum=0;
      for (int k=0; k<size;  k++) {
        sum += m1[i][k]*m2[k][j];
      }
      result[i][j] = sum;
    }
  }
  clock_t end = clock();
  double time_elapsed = (end - start)/(double)CLOCKS_PER_SEC;

  printf("%f time elapsed\n", time_elapsed);
  printf("optimised multiplication:\n");
  start = clock();

  transposition(m2,size);

  for (int i=0; i<size; i++) {
    for (int j=0; j<size; j++) {
      int sum=0;
      for (int k=0; k<size;  k++) {
        sum += m1[i][k]*m2[j][k];
      }
      result[i][j] = sum;
    }
  }

  end = clock();
  time_elapsed = (end - start)/(double)CLOCKS_PER_SEC;
  printf("%f time elapsed\n", time_elapsed);

  printf("loop nest optimised multiplication:\n");
  start = clock();

  transposition(m2,size);

  int block = 8;
  for (int ii=0; ii<size; ii+=block) {
    for (int j=0; j<size; j++) {
      for (int i=ii; i<ii+block && i<size; i++) {
        int sum=0;
        for (int k=0; k<size;  k++) {
          sum += m1[i][k]*m2[j][k];
        }
        result[i][j] = sum;
      }
    }
  }

  end = clock();
  time_elapsed = (end - start)/(double)CLOCKS_PER_SEC;
  printf("%f time elapsed\n", time_elapsed);

  return 0;
}

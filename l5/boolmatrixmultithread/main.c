#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<time.h>
#include<pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
  int matrixsize;
  bool** matrix1;
  bool** matrix2;
  bool** result;
  int calculatedrows;
} matrices_t;

matrices_t creatematrices(int size) {
  bool** matrix1 = malloc(size*sizeof(bool*));
  bool** matrix2 = malloc(size*sizeof(bool*));
  bool** result = malloc(size*sizeof(bool*));
  if (matrix1==NULL || matrix2==NULL || result==NULL) exit(1);
  
  srand(time(NULL));
  for (int i=0; i<size; i++) {
    matrix1[i]= malloc(size*sizeof(bool));
    matrix2[i]= malloc(size*sizeof(bool));
    result[i]= malloc(size*sizeof(bool));
    if (matrix1[i]==NULL || matrix2[i]==NULL || result[i]==NULL) exit(1);

    for (int j=0; j<size; j++) {
      matrix1[i][j] = rand()%2;
      matrix2[i][j] = rand()%2;
    }
  }

  matrices_t res= {matrixsize: size, matrix1: matrix1, matrix2: matrix2, result: result, calculatedrows: 0};
  return res;
}

void* multiplymatrixrow(void* args) {
  printf("thread started\n");
  matrices_t* m = (matrices_t*) args;
  while(m->calculatedrows < m->matrixsize) {

    pthread_mutex_lock(&mutex);
    int i = m->calculatedrows++;
    pthread_mutex_unlock(&mutex);

    for (int j=0; j < m->matrixsize; j++) {
      int sum=0;
      for(int sumcounter=0; sumcounter < m->matrixsize; sumcounter++) {
        sum |= (m->matrix1[i][sumcounter] &
            m->matrix2[sumcounter][j]);
        if (sum) break;
      }
      m->result[i][j] = sum;
    }
  }
}

int main(int argc, char* argv[]) {
  if (argc != 3) exit(1);
  int matrixsize = atoi(argv[1]);
  if (matrixsize<=0) exit(1);

  int threadsnum = atoi(argv[2]);
  if (threadsnum<=0) exit(1);

  // generating matrices
  matrices_t m = creatematrices(matrixsize);

  // multiplying
  pthread_t thread_ids[threadsnum];

  for (int i=0; i<threadsnum; i++) {
    pthread_create(thread_ids+i, NULL, multiplymatrixrow, (void*) &m);
  }
  for (int i=0; i<threadsnum; i++) {
    pthread_join(thread_ids[i], NULL);
  }

  //for (int i=0; i<m.matrixsize; i++) {
  //  for (int j=0; j<m.matrixsize; j++) {
  //    printf("%d ", m.matrix1[i][j]);
  //  }
  //  printf("\n");
  //}
  //printf("*\n");
  //for (int i=0; i<m.matrixsize; i++) {
  //  for (int j=0; j<m.matrixsize; j++) {
  //    printf("%d ", m.matrix2[i][j]);
  //  }
  //  printf("\n");
  //}
  //printf("=\n");

  //// print the result
  //for (int i=0; i<m.matrixsize; i++) {
  //  for (int j=0; j<m.matrixsize; j++) {
  //    printf("%d ", m.result[i][j]);
  //  }
  //  printf("\n");
  //}

  

  return  0;
}


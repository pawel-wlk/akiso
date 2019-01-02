#include <stdio.h>
#include <signal.h>

void sighandle(int sig) {
  printf("%d handled\n", sig);
}

int main (void) {
  for (int i=0; i<64; i++) {
    if (signal(i, sighandle) == SIG_ERR) {
      printf("cannot handle signal %d\n", i);
    }
  }
}

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void handler(int sig) {
  printf("%d received\n", sig);
}

int main (void) {
  signal(SIGUSR1, handler);

  pid_t pid = fork();
  if (pid == 0) {
    // child
    // wait for signals
    while(1);
  } else {
    // parent
    for(int i=0; i<30; i++) {
      //sleep(1);
      kill(pid, SIGUSR1);
    }
    printf("finshed sending signals\n");
    // wait for child
    pid_t waited = waitpid(pid, NULL, WUNTRACED); 
  }
}

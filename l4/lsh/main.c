#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>

#define BUFFER_SIZE 8
#define RL_BUFFER_SIZE 1024
#define MAX_PIPE_LENGTH 10

void  siginthandle(int signal) {}

void redirect (char** args) {
  for (int i=0; args[i] != NULL; i++) {
    if (strcmp(args[i], ">") == 0) {
      // redirect  stdout
      args[i] = NULL;
      freopen(args[i+1], "w+", stdout);
    } else if (strcmp(args[i], "2>") == 0) {
      // redirect stderr
      args[i] = NULL;
      freopen(args[i+1], "w+", stderr);
    } else  if (strcmp(args[i], "<") == 0) {
      // redirect stdin
      args[i] = NULL;
      freopen(args[i+1], "r", stdin);
    }
  }
}

char** split_line(char* line) {
  int bufsize = BUFFER_SIZE;
  if(line[0] == '\0') return NULL;
  // allocate working variables  and check if allocated correctly
  char** tokens = malloc(BUFFER_SIZE*sizeof(char*));
  char* token;
  if (!tokens) {
    fprintf(stderr, "lsh: memory error\n");
    exit(EXIT_FAILURE);
  }
  int position = 0;

  // tokenize line
  token = strtok(line, " ");
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position > bufsize) {
      bufsize += RL_BUFFER_SIZE;
      tokens = realloc(tokens, bufsize*sizeof(char));
      if (!tokens) {
        fprintf(stderr, "lsh: memory error\n");
        exit(1);
      }
    }

    token = strtok(NULL, " ");  // call for next token
  }

  tokens[position]  = NULL; //set last token as null for execvp

  return tokens;
}

int check_for_ampersand(char** args) {
  for (int i=0; args[i] != NULL; i++) {
    if (args[i][0] == '&') {
      args[i] = NULL;
      return 1;
    }
  }
  return 0;
}

char* read_line(void) {
  int bufsize = RL_BUFFER_SIZE;
  int pos = 0;
  char* buffer = malloc(sizeof(char)*bufsize);
  char c;
  
  if (!buffer) {
    fprintf(stderr, "lsh: memory error\n");
    exit(EXIT_FAILURE);
  }


  while (1) {
    // read character
    c = getc(stdin);

    if (c == EOF) {
      exit(0);
    } else if (c == '\n') {
      buffer[pos] = '\0';
      return buffer;
    } else buffer[pos] = c;

    pos++;

    if (pos > bufsize) {
      bufsize += RL_BUFFER_SIZE;
      buffer = realloc(buffer, bufsize*sizeof(char));
      if (!buffer) {
        fprintf(stderr, "lsh: memory error\n");
        exit(1);
      }
    }
  }
}

void execute_with_pipes(char** args, int ampersand) {
  if (!args[0]) return;
  // builtins
  // exit
  if (strcmp(args[0], "exit") == 0) {
    printf("Bye!!\n");
    exit(0);
  }
  // cd
  if (strcmp(args[0], "cd") == 0) {
    chdir(args[1]);
    return;
  }
  
  char*** commands = malloc(MAX_PIPE_LENGTH*sizeof(char**)); if (!commands) {
    fprintf(stderr, "memory error\n");
    exit(1);
  }

  // collect commands
  int command_no = 0;

  commands[command_no] = malloc(BUFFER_SIZE*sizeof(char*));
  if (!commands[command_no]) {
    fprintf(stderr, "memory error\n");
    exit(1);
  }

  int i;
  int new_command_pos=0;
  for (i=0; args[i]!=NULL; i++) {
    if (strcmp(args[i], "|") != 0) {
      commands[command_no][new_command_pos] = args[i];
      new_command_pos++;
    } else {
      commands[command_no][new_command_pos] = NULL;
      command_no++;
      new_command_pos = 0;
      commands[command_no] = malloc(BUFFER_SIZE*sizeof(char*));
      if (!commands[command_no]) {
        fprintf(stderr, "memory error\n");
        exit(1);
      }
    }
  }
  commands[command_no][i] = NULL;

  // initialize descriptors
  int pipes[command_no][2];
  for (int j=0; j<command_no; j++) {
    if (pipe(pipes[j]) < 0) {
      perror("lsh: piping error");
    }
  }


  // execute commands
  pid_t pids[command_no+1];
  // fork every process and attach descriptors
  for (int j=0; j<=command_no; j++) {
    if ((pids[j] = fork()) == 0) {
      if (j == 0) {
        // attach only output
        if (dup2(pipes[j][1], 1) == -1) {
          perror("lsh: dup2 error,only output");
          exit(1);
        }
      } else if (j == command_no) {
        // attach only input
        if (dup2(pipes[j-1][0], 0) == -1) {
          perror("lsh: dup2 error, only input");
          exit(1);
        }
      } else {
        // attach input and output
        // input
        if (dup2(pipes[j-1][0], 0) == -1) {
          perror("lsh: dup2 error, both");
          exit(1);
        }
        // output
        if (dup2(pipes[j][1], 1) == -1) {
          perror("lsh: dup2 error,both");
          exit(1);
        }
      }
      // close remaining fds FORK
      for (int k=0; k<command_no; k++) {
        close(pipes[k][0]);
        close(pipes[k][1]);
      }

      // EXECUTE COMMAND

      redirect(commands[j]);

      if (execvp(commands[j][0], commands[j]) == -1) {
        perror("lsh");
        exit(1);
      }

      //if (j==command_no && ampersand) {
      //  freopen("/dev/null", "w+", stdout);
      //}


      free(commands[j]);
      exit(0);
    }

    free(commands[j]);
  }
  // close remaining fds MAIN THREAD
  for (int j=0; j<command_no; j++) {
    close(pipes[j][0]);
    close(pipes[j][1]);
  }

  //  wait for  processes
  int j;
  for(j=0; j<command_no; j++) {
    if (pids[j] > 0) {
      waitpid(pids[j], NULL, 0);
    }
  }

  if (!ampersand) {
    waitpid(pids[j],NULL,0);
  }

  free(commands);
}

int main(void) {
  // preparation
  //
  // register handlers
  signal(SIGCHLD, SIG_IGN);
  signal(SIGINT, siginthandle);

  // main loop
  char* line;
  char** args;


  while(1) {
    printf("\e[1;32m┐༼ ͡° ͜ʖ ͡°༽┌\e[0;37m > ");
    char c = getc(stdin);
    if (c =='\n') {
      continue;
    } else {
      ungetc(c, stdin);
    }
    line = read_line();
    args = split_line(line);

    execute_with_pipes(args, check_for_ampersand(args));

    // free memory
    free(line);
    free(args);
  }

  return 0;
}

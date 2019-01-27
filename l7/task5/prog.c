#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void vulnerable_function(char* arg) {
  char buffer[256];
  strcpy(buffer, arg);
  printf("%s", buffer);
}

int main(int argc, char** argv) {
  vulnerable_function(argv[1]);
}


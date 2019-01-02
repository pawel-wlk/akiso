#include<stdio.h>

int main(){
  for (int i=0; i<256; i++) {
    printf("\e[38;5;%dmHello World!\n", i);
  }
  return 0;
}

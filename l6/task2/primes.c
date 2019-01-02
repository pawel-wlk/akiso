#include<stdio.h>
#include<stdbool.h>

#define SIZE 10000

bool is_prime(int n) {
  if (n==0 || n==1) return false;
  for (int i=2; i<=n/2; i++) {
    if (n%i == 0) {
      return false;
    }
  }
  return true;
}

int main() {
  for (int i=0; i<SIZE; i++) {
    if(is_prime(i)) 
      printf("%d ", i);
  }

  return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


int count_shift(char* number) {
  int counter = 0;
  while (*number !=  '\0') {
    if (*number == '.')
      return strlen(number) - counter;
    counter++;
    number++;
  }
  return 0;
}

void parse_number_to_arrays(char* number, char* integer, char* fraction) {
  int counter=0;
  bool dot_passed = false;
  while (*number != '\0') {
    if (*number == '.') {
      number++;
      counter = 0;
      dot_passed = true;
      continue;
    }
    if (dot_passed) {
      fraction[counter] = (*number >= '0' && *number <= '9') ? (*number) - '0' : 0;
    } else {
      integer[counter] = (*number >= '0' && *number <= '9') ? (*number) - '0' : 0;
    }

    number++;
    counter++;
  }
}

bool divide_by2(char* number, char* result, unsigned int length){
  char current_num = 0;
  bool remainder = 0;

  for (int i=0; i<length; i++) {
    current_num = remainder*10 + number[i];
    result[i] = current_num / 2;
    remainder = current_num % 2;
  }

  return remainder;
}

bool multiply_by2(char* number, char* result, unsigned int length) {
  bool carry = 0;

  for (int i=length-1; i>=0; i--) {
    result[i] = 2*number[i] + carry;
    carry = result[i] > 9;
    if (carry) result[i] -= 10;
  }

  return carry;
}
  



int main(int argc, char** argv) {
  if (argc != 3) {
    printf("Składnia: <liczba> <liczba cyfr po przecinku>\n");
	}

  int precision = atoi(argv[2]);
  int shift = count_shift(argv[1]);
  int length = (shift == 0) ? strlen(argv[1]) : strlen(argv[1])-1;
  char* integer = malloc(length * sizeof(char));
  char* fraction = malloc(length * sizeof(char));
  char* tmp = malloc(length * sizeof(char));

  parse_number_to_arrays(argv[1], integer, fraction);

  bool* binary_int = malloc(precision*sizeof(bool));

  for (int i=precision-1; i>=0; i-=2) {
    binary_int[i]   = divide_by2(integer, tmp, length-shift);
    binary_int[i-1] = divide_by2(tmp, integer, length-shift);
  }

  for (int i=0; i<precision; i++) {
    printf("%d", binary_int[i]);
  }
  printf(".");

  bool* binary_point = malloc(precision*sizeof(bool));

  for (int i=0; i<precision; i+=2) {
    binary_point[i]   = multiply_by2(fraction, tmp, shift);
    binary_point[i+1] = multiply_by2(tmp, fraction, shift);
  }

  for (int i=0; i<precision; i++) {
    printf("%d", binary_point[i]);
  }

  printf("\n");



  return 0;
}

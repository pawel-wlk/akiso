#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


int count_shift(char* number, unsigned int length) {
  int counter = 0;
  while (*number !=  '\0') {
    if (*number == '.')
      return length - counter;
    counter++;
    number++;
  }
  return 0;
}

void parse_number_to_array(char* number, char* fraction) {
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
      fraction[counter] = (*number) - '0';
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

unsigned int convert_integer(int value) {
  if (value==0 || value==1) return value;
  return value%2 + 10*convert_integer(value/2);
}
  



int main(int argc, char** argv) {
  if (argc != 3) {
    printf("Składnia: <liczba> <liczba cyfr po przecinku>\n");
    exit(0);
	}

  unsigned precision = atoi(argv[2]);
  unsigned shift = count_shift(argv[1], strlen(argv[1]));

  char* tmp = malloc(shift * sizeof(char));
  if (tmp == NULL) exit(0);
  char* fraction = malloc(shift * sizeof(char));
  if (fraction == NULL) exit(0);

  parse_number_to_array(argv[1], fraction);

  // convert integer part
  unsigned integer_part;
  sscanf(argv[1], "%d.", &integer_part);
  printf("%d.", convert_integer(integer_part));

  // convert fraction
  bool* binary_point = malloc(precision*sizeof(bool));
  if (binary_point == NULL) exit(0);
  for (int i=0; i<precision; i++) {
    if (i%2 == 0) {
      binary_point[i] = multiply_by2(fraction, tmp, shift);
    } else {
      binary_point[i] = multiply_by2(tmp, fraction, shift);
    }
  }

  for (int i=0; i<precision; i++) {
    printf("%d", binary_point[i]);
  }

  printf("\n");



  free(tmp);
  free(fraction);
  free(binary_point);
  return 0;
}

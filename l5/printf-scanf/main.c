#include <unistd.h>
#include <stdlib.h>

#define BUFSIZE 1024

char* inttostr(int n, int base) {
  char* buffer = malloc(BUFSIZE*sizeof(char));
  if (buffer == NULL) exit(1);

  char isnegative = n<0;
  if (isnegative) n=-n;

  int counter = 0;
  while (n != 0) {
    char digit = n % base;
    if (digit<10) {
      buffer[counter] = '0' + digit; // charcode shifting
    } else {
      buffer[counter] = 'a' + digit-10; // charcode shifting
    }
    counter++;
    n /= base;
  }

  if (isnegative) {
    buffer[counter] = '-';
    counter++;
  }
  // result has to be reversed
  char* reversed = malloc(counter*sizeof(char));
  if (reversed == NULL) exit(1);

  for (int i=0; i<counter; i++) {
    reversed[i] = buffer[counter-i-1];
  }

  reversed[counter]='\0';
  free(buffer);
  
  return reversed;
}

int strtoint(char* str, int base) {
  // works for integer bases up to 16
  int result=0;
  char isnegative = *str=='-';
  if (isnegative) str++;

  while (*str != '\0') {
    if (*str >= '0' && *str <= '9') {
      result = result*base + (*str - '0');
    } else if (*str >= 'a' && *str <= 'f') {
      result = result*base + (*str - 'a'+10);
    }
    str++;
  }

  if (isnegative) result = -result;

  return result;
}


void myprintf(char* template, ...) {
  void* nextarg = (void*)&template + sizeof(template);

  while (*template != '\0') {
    if (*template == '%') {
      template++;

      // determine format
      switch(*template) {
        case 'd':
          myprintf(inttostr(*(int*)nextarg, 10));

          nextarg+=sizeof(int);
          break;
        case 'x':
          myprintf(inttostr(*(int*)nextarg, 16));

          nextarg+=sizeof(int);
          break;
        case 'b':
          myprintf(inttostr(*(int*)nextarg, 2));

          nextarg+=sizeof(int);
          break;
        case 's':
          myprintf(*(char**)nextarg);
          nextarg+=sizeof(char*);
          break;
      }
    } else {
      write(1, template, sizeof(char));
    }

    template++;
  }
}
void myscanf(char* template, ...) {
  char* nextarg = (char*)&template + sizeof(template);


  while (*template != '\0') {
    if (*template == '%') {
      // allocate temporary variable
      char* tempstr = malloc(BUFSIZE*sizeof(char));
      if (tempstr == NULL) exit(1);

      // read input sign by sign
      int  i=-1;
      char* tmp = malloc(sizeof(char));
      if (tmp == NULL) exit(1);
      do {
        i++;
        read(0, tmp, sizeof(char));
        tempstr[i] = *tmp;
      } while (*tmp!='\n' && *tmp!=*(template+2));
      free(tmp);
          
      tempstr[i]='\0';

      //determine format
      template++;
      switch(*template) {
        case 'd': {
          int* res = (int*)(*(int*)nextarg);

          *res = strtoint(tempstr, 10);

          nextarg += sizeof(int*);
          break;
        }
        case 'b': {
          int* res = (int*)(*(int*)nextarg);

          *res = strtoint(tempstr, 2);

          nextarg += sizeof(int*);
          break;
        }
        case 'x': {
          int* res = (int*)(*(int*)nextarg);

          *res = strtoint(tempstr, 16);

          nextarg += sizeof(int*);
          break;
        }
        case 's': {
          char* res = (char*)(*(int*) nextarg);

          // literally rewrite the string
          int  i;
          for (i=0; tempstr[i] != '\0'; i++) {
            res[i]=tempstr[i];
          }
          res[i]='\0';

          nextarg += sizeof(char*);
          break;
        }

      }
      free(tempstr);
    } else {

    }
    template++;
  }
}

int main(void) {
  myprintf("xdd\n");
  myprintf("-123(10):%d\n", -123);
  myprintf("-10(2):%b\n", -10);
  myprintf("-27(x):%x\n", -27);
  myprintf("witam %s\n", "serdecznie");

  char s[100000];
  int d = 0;
  int b = 0;
  int x = 0;
  myprintf("podaj string:");
  myscanf("%s", s);
  myprintf("podaj inta:");
  myscanf("%d", &d);
  myprintf("podaj inta binarnego:");
  myscanf("%b", &b);
  myprintf("podaj inta hex:");
  myscanf("%x", &x);
  myprintf("%s, %d, %d, %d\n", s, d, b, x);
  myprintf("podaj dwa inty oddzielone przecinkiem:");
  myscanf("%d,%d", &d,&b);
  myprintf("%d, %d\n", d,b);

  return 0;
}


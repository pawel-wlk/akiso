#include <stdio.h> 
#include <string.h> //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h> //close 
#include <arpa/inet.h> //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#include <stdbool.h>
#include <netdb.h>
  
#define HOST "localhost" 
#define PORT 8888 
#define MAX_CLIENTS 30
  
int master_socket;
int addrlen;
int new_socket;
int client_socket[30];
char* client_nickname[30];
int activity;
int i;
int readlen;
int sd;
int max_sd;
struct sockaddr_in address;
char* str;
  
char* buffer;
  
//set of socket descriptors 
fd_set readfds; 
    
bool compare(char* str1, char* str2) {
  if (strlen(str1) != strlen(str2)) return false;
  for (int j=0; j<strlen(str1); j++) {
    if (str1[j] != str2[j]) return false;
  }
  return true;
}


char* getreceiver(char** message) {
  char* receiver = malloc(1024*sizeof(char));
  if (receiver == NULL) exit(EXIT_FAILURE);

  int j=0;

  while(**message != ' ' &&  **message != '\0') {
    receiver[j] = **message;
    (*message)++;
    j++;
  }
  if(**message!='\0') (*message)++;
  receiver[j] = '\0';

  return receiver;
}

char* user_list() {
  char *returnvalue = malloc((30*1024 + 40)*sizeof(char));
  if (returnvalue == NULL) exit(EXIT_FAILURE);
  strcpy(returnvalue, "USERS:\n");
  
  for (int j=0; j<MAX_CLIENTS; j++) {
    if (client_socket[j] != 0) {
      strcat(returnvalue, client_nickname[j]);
      strcat(returnvalue, "\n");
    }
  }
  strcat(returnvalue, "usage: nickname message\n");

  return returnvalue;
}

  
int main() { 
  buffer = malloc(1024*sizeof(char));
  if (buffer == NULL) exit(EXIT_FAILURE);
  //initialise all client_socket[] to 0
  for (i = 0; i < MAX_CLIENTS; i++) { 
    client_socket[i] = 0; 
  } 

  for (i=0; i<MAX_CLIENTS; i++) {
    client_nickname[i] = malloc(1024*sizeof(char));
    if (client_nickname[i] == NULL) exit(EXIT_FAILURE);
  }
    
  //create a master socket 
  if((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) { 
    perror("socket failed"); 
    exit(EXIT_FAILURE); 
  } 
  
  struct hostent* he;
  if (!(he = gethostbyname(HOST))) {
    perror("gethostbyname");
    exit(EXIT_FAILURE);
  }

  //type of socket created 
  address.sin_family = AF_INET; 
  address.sin_addr = *(struct in_addr*)(he->h_addr_list[0]);
  address.sin_port = htons(PORT); 
    
  //bind the socket to localhost port 8888 
  if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0) { 
    perror("bind failed"); 
    exit(EXIT_FAILURE); 
  } 
  printf("Listening on port %d \n", PORT); 
  printf("Listening on address %s \n", HOST); 
    
  // maximum of 3 pending connections for the master socket 
  if (listen(master_socket, 3) < 0) { 
    perror("listen"); 
    exit(EXIT_FAILURE); 
  } 
    
  //accept the incoming connection 
  addrlen = sizeof(address); 
  puts("Waiting for connections..."); 
    
  while(true) { 
    //clear the socket set 
    FD_ZERO(&readfds); 
  
    //add master socket to set 
    FD_SET(master_socket, &readfds); 
    max_sd = master_socket; 
      
    //add child sockets to set 
    for (i = 0 ; i < MAX_CLIENTS ; i++) { 
      //socket descriptor 
      sd = client_socket[i]; 
        
      if(sd > 0) 
        FD_SET(sd, &readfds); 
        
      if(sd > max_sd) 
        max_sd = sd; 
    } 
  
    activity = select(max_sd + 1, &readfds, NULL, NULL, NULL); 
  
    if ((activity < 0) && (errno!=EINTR)) { 
      printf("select error"); 
    } 
      
    if (FD_ISSET(master_socket, &readfds)) 
    { 
      if ((new_socket = accept(master_socket, 
          (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) { 
        perror("accept"); 
        exit(EXIT_FAILURE); 
      } 
      
      printf("New connection, socket fd is %d, ip is : %s, port : %d \n", 
          new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port)); 
    
      char *message = "Welcome to miniminitalk\nInsert your nickname: "; 
      send(new_socket, message, strlen(message), 0);

      if ((readlen = read(new_socket, buffer, 1024)) > 0) { 
        buffer[readlen-2]='\0';
      }

      for (i = 0; i < MAX_CLIENTS; i++) { 
        if (client_socket[i] == 0) { 
          client_socket[i] = new_socket; 
          strncpy(client_nickname[i], buffer,sizeof(buffer));

          printf("Adding to list of sockets as %d\n", i); 
          printf("with nickname %s\n", buffer);

          break; 
        } 
      } 

      char* users = user_list();
      send(new_socket, users, strlen(users), 0);
      free(users);
    } 
      
    for (i = 0; i < MAX_CLIENTS; i++) { 
      sd = client_socket[i]; 
        
      if (FD_ISSET(sd, &readfds)) { 
        if ((readlen = read(sd, buffer, 1024)) == 0) { 
          // disconnect
          getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addrlen); 
          printf("Host disconnected, ip %s, port %d \n", 
            inet_ntoa(address.sin_addr), ntohs(address.sin_port)); 
            
          close(sd); 
          client_socket[i] = 0; 
        } else { 
          // message
          buffer[readlen] = '\0'; 

          char* receiver = getreceiver(&buffer);

          str = malloc((2*1024+4)*sizeof(char));
          if (str==NULL) exit(EXIT_FAILURE);

          strcpy(str, "[");
          strcat(str, client_nickname[i]);
          strcat(str, "]: ");
          strcat(str, buffer);

          for (int j=0; j<MAX_CLIENTS; j++) {
            if (client_socket[j] != 0 && compare(client_nickname[j], receiver)) {
              send(client_socket[j], str, strlen(str), 0); 
            }
          }
          free(receiver);
        }
      }
    }
  }
    
  return 0; 
} 


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
	
#define TRUE 1 
#define FALSE 0 
#define PORT 8888 
#define MAX_CLIENTS 30

fd_set read_fds; // client file descriptors
struct sockaddr_in address; // struct to hold connection info
int addrlen;
char buffer[1024];

typedef struct {
  int fd;
  char* nickname;
} client_t;

void send_clients_list(client_t clients[], int fd) {
  for (int i=0; i<MAX_CLIENTS; i++) {
    if (FD_ISSET(clients[i].fd, &read_fds) && send(fd, clients[i].nickname, 
          strlen(clients[i].nickname), 0) < 0) {
      printf("Sending client %d with nickname %s\n", i, clients[i].nickname);
      perror("send");
    }
  }
}

int init_server() {
  // create master socket
  int master_socket;
  if((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("master socket creation");
    exit(1);
  }

  //bind master socket
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  addrlen = sizeof(address);

  if (bind(master_socket, (struct sockaddr*)&address,
        sizeof(address)) < 0) {
    perror("bind");
    exit(1);
  }

  // listen, max 3 pending clients
  if (listen(master_socket, 3) < 0) {
    perror("listen");
    exit(1);
  }

  printf("Listening on port %d\n", PORT);

  return master_socket;
}

void handle_client_message(client_t* client) {
  int readlen;
  if ((readlen=read(client->fd, buffer, 1024)) == 0) {
    getpeername(client->fd, (struct sockaddr*) &address, (socklen_t*)&addrlen);
    printf("client diconnected, ip %s, port %d\n",
        inet_ntoa(address.sin_addr), ntohs(address.sin_port));
    close(client->fd);
    client->fd = 0;
  } else if (readlen < 0) {
    perror("handle_client_message");
  } else {
    buffer[readlen] = '\0';
    printf(buffer);
    send(client->fd, ">chyba ty\n", strlen(">chyba ty\n"), 0);
  }
}

void accept_new_client (int master_socket, client_t clients[]) {
  int new_socket;
  if ((new_socket=accept(master_socket, 
          (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
    perror("accept");
    exit(1);
  }

  printf("New connection, ip: %s, port: %d\n",
      inet_ntoa(address.sin_addr), ntohs(address.sin_port));

  char* welcome_message = "welcome to miniminitalk server \nPlease enter your nickname: ";
  if (send(new_socket, welcome_message, 
        strlen(welcome_message), 0) < 0) {
    perror("send");
  }

  //int readlen;
  //if ((readlen=read(new_socket, buffer, 1024)) != 0) {
  //  buffer[readlen] = '\0';
  //  printf(buffer);
  //  char hello[64];
  //  sprintf(hello, "Welcome %s", buffer); 
  //  send(new_socket, hello, strlen(hello), 0);
  //}

  for (int i=0; i<MAX_CLIENTS; i++) {
    if (clients[i].fd == 0) {
      clients[i].fd = new_socket;
      //clients[i].nickname = buffer;
      //printf("Added client %d with nickname %s\n", i, buffer);
      break;
    }
  }
  FD_SET(new_socket, &read_fds);

  //send_clients_list(clients, new_socket);
}

void send_message(char* receiver, char* message, client_t clients[]) {
  int receiverfd;
  for (int i=0; i<MAX_CLIENTS; i++) {
    if (strcmp(receiver, clients[i].nickname)) {
      receiverfd = clients[i].fd;
      break;
    }
  }

  if (send(receiverfd, message, strlen(message), 0) < 0) {
    perror("send");
  }
}


int main(int argc , char *argv[]) {
  int i;// for loop iterator
  int master_socket = init_server();

  // initialize client sockets
  client_t clients[MAX_CLIENTS];
  for (i=0; i<MAX_CLIENTS; i++) clients[i].fd = 0;

  // MAIN LOOP
  while (TRUE) {
    // clear socket set
    FD_ZERO(&read_fds);
    // set master socket
    FD_SET(master_socket, &read_fds);
    int max_sd = master_socket;

    // add client sockets to set
    for (i=0; i<MAX_CLIENTS; i++) {
      if (clients[i].fd>0) {
        FD_SET(clients[i].fd, &read_fds);
      }

      // find highest descriptor
      if (clients[i].fd>max_sd) {
        max_sd = clients[i].fd;
      }
    }

      int activity = select(max_sd+1, &read_fds, NULL, NULL, NULL);
      if (activity<0 && (errno!=EINTR)) {
        printf("select error");
      }

      // incoming connection
      int new_socket;
      if (FD_ISSET(master_socket, &read_fds)) {
        //accept_new_client(master_socket, clients);
         if ((new_socket = accept(master_socket,  
                    (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)   
            {   
                perror("accept");   
                exit(EXIT_FAILURE);   
            }   
             
            //inform user of socket number - used in send and receive commands  
            printf("New connection , socket fd is %d , ip is : %s , port : %d  \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs 
                  (address.sin_port));   

            char* message = "hej\n";
           
            //send new connection greeting message  
            if( send(new_socket, message, strlen(message), 0) != strlen(message) )   
            {   
                perror("send");   
            }   
                 
            puts("Welcome message sent successfully");   
                 
            //add new socket to array of sockets  
            for (i = 0; i < MAX_CLIENTS; i++)   
            {   
                //if position is empty  
                if( clients[i].fd == 0 )   
                {   
                    clients[i].fd = new_socket;   
                    printf("Adding to list of sockets as %d\n" , i);   
                         
                    break;   
                }   
            }   
      }
      // handle already existing connections
      for (i=0; i<MAX_CLIENTS; i++) {
        // pass pointer to clients[i]
        //if (FD_ISSET(clients[i].fd, &read_fds)) {
        //  handle_client_message(clients + i);
        //}
        //Check if it was for closing , and also read the  
                //incoming message  
                int valread;
                if ((valread = read( clients[i].fd , buffer, 1024)) == 0)   
                {   
                    //Somebody disconnected , get his details and print  
                    getpeername(clients[i].fd , (struct sockaddr*)&address ,
                        (socklen_t*)&addrlen);   
                    printf("Host disconnected , ip %s , port %d \n" ,  
                          inet_ntoa(address.sin_addr) , ntohs(address.sin_port));   
                         
                    //Close the socket and mark as 0 in list for reuse  
                    close(clients[i].fd);   
                    clients[i].fd = 0;   
                }   
                     
                //Echo back the message that came in  
                else 
                {   
                    //set the string terminating NULL byte on the end  
                    //of the data read  
                    buffer[valread] = '\0';   
                    send(clients[i].fd , buffer , strlen(buffer) , 0 );   
                }   
      }
  }

  return 0;
}

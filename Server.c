// Server.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAXREQ 20
#define MAXQUEUE 5

char *getreq(char *inbuf, int len) {
  /* Get request char stream */
  printf("Server: ");              /* prompt */
  memset(inbuf,0,len);          /* clear for good measure */
  return fgets(inbuf,len,stdin); /* read up to a EOL */
}

/*Read from stdin and send to client*/
void* send_(void * consockfdv){
   int *consockfdp=(int*)consockfdv;
    int consockfd = *consockfdp;
    char sndbuf[MAXREQ];
    while(1){
        getreq(sndbuf, MAXREQ);     /* prompt */
        write(consockfd, sndbuf, strlen(sndbuf));
    }
    return NULL;
}

/* Read the received message from client and write to stdout */
void* receive_(void *consockfdv){
   int *consockfdp=(int*)consockfdv;
    int consockfd = *consockfdp;
    int n;
    char reqbuf[MAXREQ];
    while(1){
      memset(reqbuf,0, MAXREQ);
    n = read(consockfd,reqbuf,MAXREQ-1); /* Recv */
    
    if (n <= 0){
      printf("Client Disconnected...\nWaiting for connections...\n");
      return NULL;
   }
   printf("\nClient:");
   fflush(stdout);
   write(STDOUT_FILENO, reqbuf, n);
   printf("Server: ");
   fflush(stdout);
   //write(STDOUT_FILENO, reqbuf, n);          /* echo */
    }
    return NULL;
}

void server(int consockfd) {
   pthread_t send_thread;
   pthread_t receive_thread;
    pthread_create(&send_thread, NULL, send_, (void*)&consockfd);
    pthread_create(&receive_thread, NULL, receive_, (void*)&consockfd);
    //pthread_join(send_thread, NULL);
    pthread_join(receive_thread, NULL);
    pthread_cancel(send_thread);
   //std::thread send_thread(send_);
   //std::thread receive_thread(receive_);
    return;
}

int main() {

int lstnsockfd, consockfd, clilen, portno = 5033;
struct sockaddr_in serv_addr, cli_addr;

memset((char *) &serv_addr,0, sizeof(serv_addr));
serv_addr.sin_family      = AF_INET;
serv_addr.sin_addr.s_addr = INADDR_ANY;
serv_addr.sin_port        = htons(portno);

// Server protocol
/* Create Socket to receive requests*/
lstnsockfd = socket(AF_INET, SOCK_STREAM, 0);

/* Bind socket to port */
bind(lstnsockfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr));
printf("Bounded to port\n");
while (1) {
   printf("Listening for incoming connections\n");

/* Listen for incoming connections */
   listen(lstnsockfd, MAXQUEUE); 

   //clilen = sizeof(cl_addr);

/* Accept incoming connection, obtaining a new socket for it */
   consockfd = accept(lstnsockfd, (struct sockaddr *) &cli_addr,       
                      &clilen);
   printf("Accepted connection\n");

   server(consockfd);

   close(consockfd);
  }
close(lstnsockfd);
}

// Client.c
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>


#define MAXIN 20
#define MAXOUT 20

char *getreq(char *inbuf, int len) {
  /* Get request char stream */
  printf("Client: ");              /* prompt */
  memset(inbuf,0,len);          /* clear for good measure */
  return fgets(inbuf,len,stdin); /* read up to a EOL */
}

/*Read from stdin and send to server*/
void* send_(void* sockfdv){
    int *sockfdp=(int*)sockfdv;
    int sockfd = *sockfdp;
    char sndbuf[MAXIN];
    while(1){
        getreq(sndbuf, MAXIN);        /* prompt */
        int n = write(sockfd, sndbuf, strlen(sndbuf));
    }
    return NULL;
}

/* Read the received message from server and write to stdout */
void* receive_(void* sockfdv){
    int *sockfdp=(int*)sockfdv;
    int sockfd = *sockfdp;
    int n;
    char rcvbuf[MAXOUT];
    while(1){
        memset(rcvbuf,0,MAXOUT);                   /* clear */
        n=read(sockfd, rcvbuf, MAXOUT - 1);      /* receive */
        //printf("Received reply: %d",n);
        if(n==0)
        {
            //n=read(sockfd, rcvbuf, MAXOUT - 1);
          printf("Server is down. Exiting...\n");
          return NULL;
        }
        printf("\nServer:");
        fflush(stdout);
        write(STDOUT_FILENO, rcvbuf, n);          /* output */
        printf("Client: ");
        fflush(stdout);
        //printf("\n");
    }
        return NULL;
}

void* client(int sockfd) {
    pthread_t send_thread,receive_thread;
    pthread_create(&send_thread, NULL, send_, (void*)&sockfd);
    //std::thread send_thread(send_,sockfd);
    pthread_create(&receive_thread, NULL, receive_, (void*)&sockfd);
    //std::thread receive_thread(receive_,sockfd);
    //pthread_join(send_thread, NULL);
    pthread_join(receive_thread, NULL);
    pthread_cancel(send_thread);
    return NULL;
}

// Server address
struct hostent *buildServerAddr(struct sockaddr_in *serv_addr, char *serverIP, int portno) {
    /* Construct an address for remote server */
    memset((char *) serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr->sin_family = AF_INET;
    inet_aton(serverIP, &(serv_addr->sin_addr));
    serv_addr->sin_port = htons(portno);
}


int main() {
	//Client protocol
	char *serverIP = "10.8.14.63";
	int sockfd, portno = 5039;
	struct sockaddr_in serv_addr;
	
	buildServerAddr(&serv_addr, serverIP, portno);

	/* Create a TCP socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	/* Connect to server on port */
	if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) >= 0){
        printf("Connected to %s:%d\n",serverIP, portno);
        client(sockfd);
        close(sockfd);
    }
    else {
        printf("Server is not running\n");
    }

	/* Carry out Client-Server protocol */
	

	/* Clean up on termination */
	close(sockfd);
}
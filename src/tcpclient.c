#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<sys/types.h>
#include<arpa/inet.h> //inet_addr
#include <unistd.h> 
#include <netdb.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

pthread_t reader,writer;

void* readfrom(void* sock_desc){
  int sock = *(int*)sock_desc;
  char server_reply[2000];
  int read_size;
  while((read_size = recv(sock , server_reply , 1999 , 0))>0){
    
    if( read_size < 0)
      {
	puts("recv failed");
	return 0;	
      }
  
    
    puts(server_reply);
    
    fflush(stdout);    
    if((strcmp(server_reply,"server exit")==0)||(strcmp(server_reply,"exit")==0)){
      puts("closing client");
      pthread_cancel(writer);
      exit(0);
    }
   
    bzero(server_reply,2000);

  }
  return 0;
}
void* writeto(void* sock_desc){
  int sock = *(int*)sock_desc;
  char buffer[256];
  
  while(1){
    sleep(2);
    puts("");   
    puts("Enter message : ");
    
    bzero(buffer,256);
        
    // get a message from the client
    fgets(buffer,255,stdin);
    
   
 
    puts("");
    sleep(2);
    send(sock , buffer, strlen(buffer),0);
    
    bzero(buffer,256);
  
  }
}


int main(int argc , char *argv[])
{
  if(argc<3){
    return 1;
  }
    
  int portno;
  int sock;
  struct sockaddr_in server;
 
  struct hostent *serverIPAddress;
 
  //Create socket
  sock = socket(AF_INET , SOCK_STREAM , 0);
  if (sock == -1)
    {
      printf("Could not create socket");
    }
  puts("Socket created");
  bzero((char *) &server, sizeof(server));
  serverIPAddress = gethostbyname(argv[1]);
  printf("connecting to hostname: %s \n",argv[1]);
  if (serverIPAddress == NULL)
    {
      fprintf(stderr,"ERROR, no such host\n");
      exit(0);
    }

  bcopy((char *)serverIPAddress->h_addr, (char *)&server.sin_addr.s_addr, serverIPAddress->h_length);
  portno = atoi(argv[2]);
  server.sin_family = AF_INET;
  server.sin_port = htons(portno);
  printf("connecting to port %d\n",ntohs(server.sin_port));
  //Connect to remote server
  while (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
      perror("connect failed. Error,reconnect in two seconds");
      sleep(2);
    }
     
  puts("Connected\n");
  pthread_create(&reader,NULL,readfrom,(void*)&sock);
  pthread_create(&writer,NULL,writeto,(void*)&sock);

  pthread_join(reader,NULL);
  pthread_join(writer,NULL);

  close(sock);
  return 0;
}

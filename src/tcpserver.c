
#include "tcpserver.h"






bank* createbank(){

  bank* new = (bank*)malloc(sizeof(bank));
  if (pthread_mutex_init(&new->bankmute, NULL) != 0)
    {
      printf("\n mutex init failed\n");
      return NULL;
    }
  new->map = constructor(50);
  return new;


}
int createaccount(char* accountname,bank* thisbank){
  pthread_mutex_lock(&thisbank->bankmute);
  acctslots++;
  if(acctslots>20){
    acctslots--;
    return 1;
  }
  account* new = malloc(sizeof(account));
  new->balance = 0.0;
  pthread_mutex_init(&new->accountmute,NULL);
  if(get(accountname,thisbank->map)==NULL){

    set(accountname,new,thisbank->map);
    printf("account: %s created \n",accountname);

  }
  else{
    pthread_mutex_unlock(&thisbank->bankmute);
    return 1;
  }
  pthread_mutex_unlock(&thisbank->bankmute);

  return 0;
}
account*  accessacct(char* accountname,bank* thisbank){

  account* found;
  found = get(accountname,thisbank->map);
  return found;
}
void creditAct(account* acct,float amount){
  acct->balance += amount;
  return;
}

void debitAct(account* acct,float amount){
  acct->balance -= amount;
  return;
}

connectlist* createconnectlist(){
  connectlist* new;
  new = (connectlist*) malloc(sizeof(connectlist));
  new->head = NULL;
 
  return new;
}
void insertsock(connectlist* list,void* sock_desc){
  connectnode* new;
  
  new = (connectnode*) malloc(sizeof(connectnode));
  new->sock_desc = sock_desc;
 
  if(list->head == NULL){
    list->head = new;
    list->head->next= NULL;
     
  }
  else{
    new->next = list->head;
    list->head = new;
      
  }
  
}
void sigcHandler(int sig){
  if(sig == SIGINT){
    int sock;
    char *message;
    //Send some messages to the client
    message = "server exit";

    connectnode* ptr;
    ptr = globallist->head;
    while(ptr!=NULL){
      //Get the socket descriptor
      sock = *(int*)ptr->sock_desc;
      send(sock , message , strlen(message),0);
      close(sock);
      free(ptr->sock_desc);
      ptr = ptr->next;
    }
    exit(0);
  }
  

}
void* bankprint(void* banker){
  bank* thisbank = (bank*)banker;
  hashtable* index = thisbank->map;
  hashnode* hold;
  account* curr;
  int i;
  while(1){
    sleep(20);
    while(pthread_mutex_trylock(&thisbank->bankmute)!=0){
      puts("waiting to print");
      sleep(1);
    }
    puts("");
    puts("begin print");
    puts("");
    for(i=0;i<index->size;i++){
      hold = index->table[i];
      while(hold!=NULL){
        printf("account: %s\n",hold->key);
	curr = hold->value;
	if(pthread_mutex_trylock(&curr->accountmute)!=0){
	  printf("in session\n");
	}
	else{
	  printf("balance: %0.2f\n",curr->balance);
	  pthread_mutex_unlock(&curr->accountmute);
	}
	puts("");        
        hold = hold->next;
      }
    }
    puts("");
    puts("end bankserver print");
    pthread_mutex_unlock(&thisbank->bankmute);
  }


}
void *connection_handler(void *socket_desc)
{
  int ccount;
  CommandList* commando;
  //Get the socket descriptor
  int sock = *(int*)socket_desc;
  int read_size;
  char *message , client_message[2000], buffer[2000];
  float amt;   
  char* accountname = NULL;
  account* user = NULL;
  //Send some messages to the client
  message = "HEY you've connected to Vamsi and Armin's bank";
  send(sock , message , strlen(message),0);
     
    

  //Receive a message from client
  while( ((read_size = recv(sock , client_message , 2000 , 0)) > 0) )
    {
      
      if (read_size == 1){

	client_message[read_size] = '\0';
	write(sock,client_message,strlen(client_message));
      }
      //end of string marker (take newline char out)
      client_message[read_size-1] = '\0';
      
      
      commando = TokenizeString(client_message);
      ccount = commando->count;
      if(ccount == 0){
	message = "no command sent, retry";
	send(sock , message , strlen(message),0);
      }
      else if(ccount == 1){
	if (strcmp(commando->tail->command->token,"balance")==0){
	  message = "you want me to print out a balance";
	  send(sock , message , strlen(message),0);
	  if(user == NULL){
	    message = "no account in session";
	    send(sock , message , strlen(message),0);
	  }
	  else if(accountname!=NULL){
	    amt = user->balance;
	    snprintf(buffer,2000,"account: %s balance is :%0.2f",accountname,amt);
	    send(sock, buffer,strlen(buffer),0);
	    bzero(buffer,2000);
	        
	  }
	  else{
	    message = "error";
	    send(sock , message , strlen(message),0);
	  }
	}
	else if(strcmp(commando->tail->command->token,"finish")==0){
	  message = "finish an account sesh";
	  send(sock , message , strlen(message),0);
	  if (user == NULL){
	    message = "no open account sesh";
	    send(sock , message , strlen(message),0);
	  }
	  else{
	    pthread_mutex_unlock(&user->accountmute);
	    free(accountname);
	    accountname = NULL;
	    user = NULL;
	    message = "finished accounsesh";
	    send(sock , message , strlen(message),0);
	  }

	}
	else if(strcmp(commando->tail->command->token,"exit")==0){
          
	  if (user!=NULL){

            pthread_mutex_unlock(&user->accountmute);
	    message = "finished accounsesh";
            send(sock , message , strlen(message),0);
            free(accountname);
            accountname = NULL;
	    user = NULL;
          }
	  message = "exit";
          send(sock , message , strlen(message),0);

        }
	else{
	  message = "I didn't understand";
          send(sock , message , strlen(message),0);
	}
      }
      else if(ccount == 2){
	if (strcmp(commando->tail->next->command->token,"open")==0){
	    
          if(commando->tail->command->thistoken == word){
	    if (accountname == NULL){
	      snprintf(buffer,2000,"You want to open an account with name : %s",commando->tail->command->token);
	      send(sock, buffer,strlen(buffer),0);
	            
	      if(createaccount(commando->tail->command->token,mainbank)==1){
		message= "error either account already exists or bank is full";
		send(sock , message , strlen(message),0);

	      }
	      else{
		message = "made account";
		send(sock , message , strlen(message),0);
		bzero(buffer,2000);
	      }
	    }
	    else{
	      snprintf(buffer,2000,"You have yet to close account with name : %s",accountname);
              send(sock, buffer,strlen(buffer),0);
	      bzero(buffer,2000);
	    }

	  }
	  else{
	    message = "You wanna open an account but your second arg isn't a name"; 
	    send(sock , message , strlen(message),0);
	  }
        }
	
	else if(strcmp(commando->tail->next->command->token,"credit")==0){
	  if(commando->tail->command->thistoken == floatint){
	    sscanf(commando->tail->command->token,"%f",&amt);
	    snprintf(buffer,2000,"You want to credit account  by amount: %0.2f",amt);

	    if(user == NULL){
	      message = "no account in session";
	      send(sock , message , strlen(message),0);
       
	    }
	    else{
	      creditAct(user,amt);
	      message = "account credited";
	      send(sock , message , strlen(message),0);
	    }
	  }
	  else{
	    message = "You wanna credit an account but your second arg isn't a number";
	    send(sock , message , strlen(message),0);
	  }

	}
	else if (strcmp(commando->tail->next->command->token,"start")==0){
	  if(commando->tail->command->thistoken == word){
	    if (accountname == NULL){
	      accountname = (char*)malloc(sizeof(strlen(commando->tail->command->token)+1));
	      strcpy(accountname,commando->tail->command->token);
	           
	      snprintf(buffer,2000,"You want to start an account sesh with name : %s",accountname);
	           
	      send(sock, buffer,strlen(buffer),0);
	      pthread_mutex_lock(&mainbank->bankmute);	           
	      user = accessacct(accountname,mainbank);
	      pthread_mutex_unlock(&mainbank->bankmute);
	      bzero(buffer,2000);
	      if(user ==NULL){
		snprintf(buffer,2000,"couldn't find account with name : %s",accountname);
		free(accountname);
		accountname = NULL;
		send(sock, buffer,strlen(buffer),0);
		bzero(buffer,2000);
	      }
	      else {
		       
		while(pthread_mutex_trylock(&user->accountmute)!=0){
		  message = "waiting to use account: accountname";
		  send(sock,message,strlen(message),0);
		  sleep(5);
		}
		message = "succesfully started account session";
		send(sock,message,strlen(message),0);
	      }
	    }
	    else {
	      snprintf(buffer,2000,"You are still using account : %s",accountname);
	      send(sock, buffer,strlen(buffer),0);
	    }

	  }
	  else{
	    message = "You wanna start an account but your second arg isn't a name";
	    send(sock , message , strlen(message),0);
	  }
	}
	else if(strcmp(commando->tail->next->command->token,"debit")==0){
	  if(commando->tail->command->thistoken == floatint){
	    sscanf(commando->tail->command->token,"%f",&amt);
	    snprintf(buffer,2000,"You want to debit account by amount: %0.2f",amt);
	    send(sock,buffer,strlen(buffer),0);
	    if(user == NULL){
	      message = "no account in session";
	      send(sock , message , strlen(message),0);

	    }
	    else{
	      debitAct(user,amt);
	      if(user->balance<0.0){
		message = "too much debited";
		creditAct(user,amt);
		send(sock,message,strlen(message),0);
	      }
	      else{
		message = "account debited";
		send(sock , message , strlen(message),0);
	      }
	    }

	  }
	  else{
	    message = "You wanna debit an account but your second arg isn't a number";
	    send(sock , message , strlen(message),0);
	  }
	   
	}
	else{
	  message = "invalid input format, try again";
	  send(sock , message , strlen(message),0);

	}
	
      }
      else{
	message = "invalid input format, try again";
	send(sock , message , strlen(message),0);

      }
      //clear the message buffer
      commando = NULL;
      bzero(buffer,2000);
      bzero(client_message,2000);
    }
  

  if(read_size == 0)
    {
      puts("Client disconnected");
      if (user!=NULL){

	pthread_mutex_unlock(&user->accountmute);
	free(accountname);
	accountname = NULL;
	user = NULL;
      }

      fflush(stdout);
    }
  else if(read_size == -1)
    {
      perror("recv failed");
    }
         
  return 0;
} 

       

int main(int argc , char *argv[])
{
  acctslots = 0;
  mainbank = createbank();
  globallist=createconnectlist();
  int socket_desc , client_sock , clilen;
  int* new_sock;
  struct sockaddr_in server , client;    
  char ThisHost[80];
  pthread_t printthread;
  if(pthread_create( &printthread , NULL ,  bankprint , (void*)mainbank) < 0)
    {
      perror("could not create thread");
      return 1;
    }
  
  signal(SIGINT,sigcHandler); 
  
  gethostname(ThisHost, MAXHOSTNAME);
  printf("----TCP/Server running at host NAME: %s\n", ThisHost);
  //Create socket
  
  socket_desc = socket(AF_INET , SOCK_STREAM , 0);
  

  if (socket_desc == -1)
    {
      puts("Could not create socket");
    }
  puts("Socket created");

  //Prepare the sockaddr_in structure
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = 0;
 
  bzero((char *) &server, sizeof(server));   

  //Bind
  if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) != 0)
    {
      //print the error message
      perror("bind failed. Error");
      return 1;
    }
  
  puts("bind done");
  
  struct sockaddr_in sin;
  
  socklen_t len = sizeof(sin);
  
  if (getsockname(socket_desc, (struct sockaddr *)&sin, &len) == -1)
    perror("getsockname");
  else
    printf("port number %d\n", ntohs(sin.sin_port));   
  
  //Listen  
  listen(socket_desc , 5);
     
  //Accept and incoming connection
  puts("Waiting for incoming connections...");
  clilen = sizeof(struct sockaddr_in);
     
  
  
  while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&clilen)) )
    {
      puts("Connection accepted");
      pthread_t conn_thread;
     
      new_sock = malloc(sizeof(int));
      *new_sock = client_sock;

      insertsock(globallist,(void*)new_sock);

      if(pthread_create( &conn_thread , NULL ,  connection_handler , (void*)new_sock) < 0)
        {
	  perror("could not create thread");
	  return 1;
        }
      
      
      
      
    }
  
  if (client_sock < 0)
    {
      perror("accept failed");
      return 1;
    }
  
  return 0;
}
 

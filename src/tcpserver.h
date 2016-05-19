#ifndef TCPSERVER_H_
#define TCPSERVER_H_

#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with -pthread
#include<signal.h>
#include "tokenizer.h"
#include "hashtable.h"

#define MAXHOSTNAME 80 


typedef struct account{
  float balance;
  
  pthread_mutex_t accountmute;
}account;
typedef struct bank{
  hashtable* map;
  pthread_mutex_t bankmute;
}bank;

typedef struct connectlist{
  struct connectnode* head;


}connectlist;

typedef struct connectnode{
  void* sock_desc;
  struct  connectnode* next;
}connectnode;

bank* mainbank;
connectlist* globallist;
int acctslots;

bank* createbank();
int createaccount(char* accountname,bank* thisbank);
account* accessacct(char* accountname, bank* thisbank);
void creditAct(account* acct,float amount);
void debitAct(account* acct,float amount);
connectlist* createconnectlist();
void insertsock(connectlist* list,void* sock_desc);
void sigcHandler(int sig);
void* bankprint(void* banker);
void *connection_handler(void *socket_desc);


#endif

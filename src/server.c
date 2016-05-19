#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <libgen.h>
#include <pthread.h>
#include "tokenizer.h"
#include "hashtable.h"
#include "sorted-list.h"

typedef struct account{
  phtread_mutex_t accountmute1;
  float balance;
  int insession;
}account;
typedef struct bank{
  pthread_mutex_t bankmute0;
  hashtable* map;
}bank;

bank* createbank(){
  
  bank* new = (bank*)malloc(sizeof(bank));
  if (pthread_mutex_init(&new->bankmute0, NULL) != 0)
    {
      printf("\n mutex init failed\n");
      return NULL;
    }
  new->map = constructor(50);
  return new;
  
  
}
void createaccount(char* accountname,bank* thisbank){
  pthread_mutex_lock(&thisbank->bankmute0);
  
  account* new = malloc(sizeof(account));
  new->balance = 0.0;
  pthread_mutex_init(&new->accountmute1);
  if(get(accountname,thisbank->map)==NULL){
  
    set(accountname,new,thisbank->map);
    printf("account: %s created \n",accountname);

  }
  else{
    printf("account {%s} already exists \n",accountname);
  }  
  pthread_mutex_unlock(&thisbank->bankmute0);

  return;
}
account*  accessacct(char* accountname,bank* thisbank){
  
  account* found;
  found = get(accountname,thisbank->map);
  return found;
}

void creditAct(account* acct,int amount){
  acct->balance += amount;
  return;
}

void debitAct(hashnode* acct,int amount){
  acct->balance -= amount;
  return;
}


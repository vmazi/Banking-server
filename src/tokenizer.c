#include "tokenizer.h"



void PrintCommands (CommandList* list)
{
  if(list->tail == NULL)
    {
      return;
    }
  commandnode* ptr = list->tail->next;                          //delete a linked list
  commandnode* tmp;
  tmp = ptr;
  int count = list->count;
  if(ptr == NULL)
    {
      puts(list->tail->command->token);
      
      return;
    }
  while (count!=1)
    {

      ptr = ptr->next;
      puts(tmp->command->token);
      tmp = ptr;
      count--;
    }
  puts(ptr->command->token);
  return;
}

void  addcommand (TokenizerT* command,CommandList* list)
{
  commandnode* tmp = NULL;
  commandnode* new = NULL;
  new = (commandnode*)malloc(1*sizeof(commandnode));
  new -> command = command;
  new -> next = NULL;
  if (list->tail == NULL)
    {
      list->tail = new;                                     //insert into linked list
      list-> tail->next = NULL;
      list->count++;
      return;
    }
  else if (list->tail->next == NULL)
    {
      tmp =list->tail;
      list-> tail = new;
      list-> tail->next = tmp;
      tmp->next =list-> tail;
      list->count++;
      return;
    }
  else
    {
      tmp = list ->tail;
      list-> tail = new;
      list-> tail->next = tmp ->next;
      tmp->next = list->tail;
      list->count++;
      return;
    }
}
void deleteCommandList (CommandList* list)
{
  if(list->tail == NULL)
    {
      return;
    }
  commandnode* ptr = list->tail->next;                          //delete a linked list
  commandnode* tmp;
  tmp = ptr;
  int count = list->count;
  if(ptr == NULL)
    {
      TKDestroy(list->tail->command);
      free(list->tail);
      list->tail = NULL;
      list->count = 0;
      return;
    }
  while (count!=1)
    {

      ptr = ptr->next;
      TKDestroy(tmp->command);
      free(tmp);
      tmp = ptr;
      count--;
    }
  TKDestroy(ptr->command);
  free(ptr);
  ptr = NULL;
  tmp = NULL;
  list->tail = NULL;
  list->count = 0;
  return;
}


void  addEnd (char val,CLL* list)
{
  linknode* tmp = NULL;
  linknode* new = NULL;
  new = (linknode*)malloc(1*sizeof(linknode));
  new -> data = val;
  new -> next = NULL;
  if (list->tail == NULL)
    {
      list->tail = new;                                     //insert into linked list
      list-> tail->next = NULL;
      list->count++;
      return;
    }
  else if (list->tail->next == NULL)
    {
      tmp =list->tail;
      list-> tail = new;
      list-> tail->next = tmp;
      tmp->next =list-> tail;
      list->count++;
      return;
    }
  else
    {
      tmp = list ->tail;
      list-> tail = new;
      list-> tail->next = tmp ->next;
      tmp->next = list->tail;
      list->count++;
      return;
    }
}
char* CLLtoStr(CLL* list)
{
  linknode* point;
  int count = list->count;
  char* str = NULL;
  str = (char *) calloc(count,sizeof(char));
  int ptr;
  if(list->tail == NULL)
    {
      free(str);
      str = NULL;
      return NULL;
    }
  else if(list->tail->next == NULL)
    {
      point = list->tail;
    }
  else                                                       //turn linked list into a string
    {
      point = list->tail->next;
    }
  for(ptr = 0; ptr<count;ptr++)
    {
      str[ptr] = point->data;
      point = point->next;
    }
  return str;
}
void deleteCLL (CLL* list)
{
  if(list->tail == NULL)
    {
      return;
    }
  linknode* ptr = list->tail->next;                          //delete a linked list
  linknode* tmp;
  tmp = ptr;
  int count = list->count;
  if(ptr == NULL)
    {
      free(list->tail);
      list->tail = NULL;
      list->count = 0;
      return;
    }
  while (count!=1)
    {

      ptr = ptr->next;
      free(tmp);
      tmp = ptr;
      count--;
    }
  free(ptr);
  ptr = NULL;
  tmp = NULL;
  list->tail = NULL;
  list->count = 0;
  return;
}


TokenizerT *TKCreate( char * ts ) {
  CLL* list = (CLL*) malloc(sizeof(CLL));
  list->tail = NULL;
  list->count = 0;
  TokenizerT* new = (TokenizerT*)malloc(sizeof(TokenizerT));
  char * ptr= ts;
  char* temp;
  int i;
  if(ptr!=NULL&&isalpha(*ptr)){
    while(isalpha(*ptr)||isdigit(*ptr)){
      addEnd(*ptr,list);
      ptr++;
    }
    addEnd('\0',list);
    temp = CLLtoStr(list);
    for(i = 0; temp[i]; i++){
      temp[i] = tolower(temp[i]);
    }
    
    new->token = temp;
    new->thistoken = word;
    deleteCLL(list);
    return new;  
  
  }
else if(ptr!=NULL&&isdigit(*ptr)){
    while(isdigit(*ptr)){
      addEnd(*ptr,list);
      ptr++;
    }
    if(*ptr == '.'){
      addEnd(*ptr,list);
      ptr++;
    }
    while(isdigit(*ptr)){
      addEnd(*ptr,list);
      ptr++;
    }

    addEnd('\0',list);
    temp = CLLtoStr(list);

    new->token = temp;
    new->thistoken = floatint;
    deleteCLL(list);
    return new;

  }
 else if( ptr!= NULL&& *ptr == '.'){
   addEnd(*ptr,list);
   ptr++;
   while(isdigit(*ptr)){
     addEnd(*ptr,list);
     ptr++;
   }
   temp = CLLtoStr(list);
   new->token = temp;
   new->thistoken = floatint;
   deleteCLL(list);
   return new;
 }
 else{
   free(new);
   return NULL;
 }
}


void TKDestroy( TokenizerT * tk ) {
  free(tk->token);
  free(tk);
}


char *TKGetNextToken(char*here ) {
  char*  ptr = here;
  
  CLL* list = (CLL*) malloc(sizeof(CLL));                                        
  list->count = 0;
  while(*ptr != '\0'&&!(isalpha(*ptr)||isdigit(*ptr)||*ptr=='.')){ //Find next token
    ptr++;
  }
  if(isalpha(*ptr)||isdigit(*ptr)||*ptr=='.'){  //if found, return pointer to beginning
    return ptr;
  }
  else{      //else all tokens found, return null
    return NULL;
  }

}
CommandList* TokenizeString(char* string){
  char* ptr;
  TokenizerT* new;
  CommandList* commando = (CommandList*) calloc(1,sizeof(CommandList));
  int decount = 0;
  commando->count = 0;
  ptr = string;
  while(ptr!= NULL &&*ptr!='\0'){
    ptr = TKGetNextToken(ptr);  //find token
    new = TKCreate(ptr);       //use token
    if(new!=NULL){            //if token was found do this
      addcommand(new,commando);
      new = NULL;
    }  
    while(ptr!=NULL&& *ptr != '\0'&&(isalpha(*ptr)||isdigit(*ptr)||*ptr=='.')){  //get to end of token
      if(*ptr == '.'){
	decount++;
	if(decount>1){
	  decount = 0;
	  break;
	}
      }
      ptr++;
    }

  }
  return commando;
  
}


#ifndef TOKENIZER_H_
#define TOKENIZER_H_

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <libgen.h>






typedef struct CLL
{
  int count;
  struct linknode* tail;
}CLL;
typedef struct linknode
{
  char data;
  struct linknode* next;
}linknode;    

typedef enum{

  word,
  floatint 
} tokentype;

typedef struct TokenizerT {
  tokentype thistoken;
  char* token;
}TokenizerT;

typedef struct commandnode
{
  TokenizerT* command;
  struct commandnode* next;
}commandnode;

typedef struct CommandList
{
  int count;
  commandnode* tail;
}CommandList;


void PrintCommands (CommandList* list);
void  addcommand (TokenizerT* command,CommandList* list);
void deleteCommandList (CommandList* list);
void  addEnd (char val,CLL* list);
char* CLLtoStr(CLL* list);
TokenizerT *TKCreate( char * ts );
void TKDestroy( TokenizerT * tk );
char *TKGetNextToken( char* here );
CommandList* TokenizeString(char* string);
#endif

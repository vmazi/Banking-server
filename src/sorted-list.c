#include "sorted-list.h"

void  addHead (void* val, SortedList* list)
{
  
  listnode* tmp = NULL;
  listnode* new = NULL;
  new = (listnode*)malloc(1*sizeof(listnode));//allocate space thatll hold the node
  new -> data = val;
  new -> next = NULL;
  if (list->head == NULL)
    {

      list->head = new;
      list-> head->next = NULL;
      return;
    }
  
  else
    {
      tmp = list->head;
      list->head = new;
      list->head->next = tmp;
      return;
    }
}

int ListLength (listnode* list)
{
  listnode* temp = list;

  int i=0;
  while(temp!=NULL)
    {
      i++;
      temp=temp->next;


    }
  return i;
}



listnode* SelectPivot(listnode* list)//selects pivot from the middle of the list 
{

  int k, n, i = 0;
  n = ListLength(list);


  listnode* pivot=list;


  k=n/2;//middle index of list
  for (; i < k; ++i)//iterator thatll go till the middle of the list
    {
      pivot=pivot->next;
    }

  return pivot;//middle element
}


listnode* Quicksort(listnode* list,CompareFuncT cf)
{


  // Return NULL list
  if (ListLength(list) <= 1) return list;
  listnode* less=NULL, *more=NULL, *next, *end, *temp=NULL;

  // Select a middle pivot point
  listnode*  pivot = SelectPivot(list);

  // Remove pivot from list
  while(list !=NULL)
    {
      next = list->next;

      if(cf(list->data,pivot->data)!=0)
        {
          list->next=temp;
          temp = list;
         }
      list = next;
    }
  // Divide & Conq
  while(temp != NULL)
    {
      next = temp->next;
      if(cf(temp->data,pivot->data)<0)
        {
          temp->next = less;
          less = temp;
        }
      else
        {
          temp->next = more;
          more = temp;
        }
      temp = next;
    }
  // Recursive Calls
  less = Quicksort(less,cf);
  more = Quicksort(more,cf);

  // Merge
  if(more != NULL)
    {
      end = more;
      while(end->next != NULL){
        end=end->next;
      }
      pivot->next=less;
      end->next = pivot;
      return more;
    }
  else{
    pivot->next = less;
    return pivot;
  }

}




SortedList* SLCreate(CompareFuncT cf, DestructFuncT df){
  //Allocate space for a new list objct
  SortedList* new = (SortedList*)malloc(sizeof(SortedList));
  new->cf = cf;//set compare and destroy functions
  new->df = df;
  new->head = NULL;
  if(new != NULL){
    return new;
  }
  else{
    return NULL;
  }
}

//calls the destroy function and frees pointer along with data
void SLDestroy(SortedList* list){
  if(list == NULL) return;

  DestructFuncT df = list->df;
  listnode * ptr=list->head;
  listnode * tmp ;
  if(ptr == NULL){
    return;
  }  
  do{
    tmp = ptr;
    ptr = ptr->next;
    df(tmp->data);
    tmp->next = NULL;
    free(tmp);
  }while(ptr!=NULL);
  list->head = NULL;

}
int SLInsert(SortedList* list, void *newObj){

  CompareFuncT cf;

  if(list == NULL) return 0;
  cf = list->cf;
  if (list->head ==NULL){//if the list is empty
    addHead(newObj,list);
    return 1;
  }
  listnode* ptr;

  for(ptr = list->head;ptr!=NULL;ptr=ptr->next){//parse through list and see if object is already present
    if(cf(newObj,ptr->data)==0){
      return 0;
    }
  }

	//If not present
  addHead(newObj,list);
  list->head = Quicksort(list->head,cf);
  return 1;
}

void SLSort(SortedList* list){
  list->head = Quicksort(list->head,list->cf);
}

int SLRemove(SortedList* list, void *newObj){

  if(list == NULL){
    return 0;
  }
  CompareFuncT cf = list->cf;
  
  if(list->head ==NULL){
    return 0;
  }
  listnode* ptr;
  listnode* tmp;
//Advance parser to the location of the element to be removed, or end of list, whichever first
  for(ptr=list->head;ptr->next!=NULL&&cf(newObj,ptr->next->data)!=0;ptr=ptr->next)
    {

    }
//if the parser is at the end of the list, it means the element to be deleted doestn exist
  if(ptr->next==NULL){
    return 0;
  }
  else if(cf(ptr->next->data,newObj)==0){
    tmp = ptr->next;
    if(tmp->next ==NULL){
      ptr->next = NULL;
      tmp->data = NULL;
      free(tmp);
      return 1;
    }
    else{
      ptr->next = ptr->next->next;
      tmp->next = NULL;
      tmp->data = NULL;
      free(tmp);
      return 1;
    }
  }
  else{
    return 0;
  }
}


SortedListIterator* SLCreateIterator(SortedList* list){

  SortedListIterator* new = (SortedListIterator*)malloc(sizeof(SortedListIterator));
  new->ptr = list->head;
  return new;
}
void SLDestroyIterator(SortedListIterator* iter){
  iter->ptr = NULL;
  free(iter);


}


void * SLNextItem(SortedListIterator* iter){
  void* val;
  if(iter->ptr == NULL) return NULL;
  if(iter->ptr->next ==NULL){
    val = iter->ptr->data;
    iter->ptr = NULL;
    return val;
  }
  else{
    val = iter->ptr->data;
    iter->ptr = iter->ptr->next;
    return val;
  }
}
void * SLGetItem( SortedListIterator* iter ){
  if(iter->ptr == NULL) return NULL;
  else{
    return iter->ptr->data;
  }

}

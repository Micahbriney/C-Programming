
typedef struct node_st{
    int data;
    struct node_st *next;
} Node;

Node *merge_lists(Node *listA, Node *listB){
  Node *head;
  Node *sortHead;
  int sortFlag = 0;
    
  /* Case 1: Both lists are NULL */
  if(!listA && !listB){
    return '\0';
  }
    
  /* Case 2: List A is NULL */
  if(!listA){
    head = listB;
  }/* Case 3: List B is NULL */
  else if(!listB){
    head = listA;
  }
  else{ /* Case 4: Neither list is NULL */
    head = listA;
    while(listA->next){ /* Find end of List A */
      listA = listA->next;
    }
    listA->next = listB; /* Set end of List A's next to the head of List B */
  }
    

  while(!sortFlag){
    sortFlag = 1;
    sortHead = head;
    
    do {
      sortHead = sortHead->next;
      if(sortHead->data >= sortHead->next->data); /* Do nothing*/
      else{ /* XOR swap data */
	sortHead->data       = sortHead->data ^ sortHead->next->data;
	sortHead->next->data = sortHead->data ^ sortHead->next->data;
	sortHead->data       = sortHead->data ^ sortHead->next->data;
	sortFlag = 0;
      }
    } while (sortHead->next);
  }
  return tempHead;
}

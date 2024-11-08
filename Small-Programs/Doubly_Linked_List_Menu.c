#include <stdio.h>
#include <string.h>
#include <stdlib.h>

enum{PUSH_STRING = 1, PRINT_LIST, DELETE_ITEM, END_PROGRAM};

//Element structure
typedef struct listelement{
    struct listelement *next,*prev; //doubly linked navigation elements
    char text[1000];                //User input
}listElement;

void printMenu();
listElement* createNode(char* userStr);
void menu(int userSel);
void *addnewnode(char* userString);
void addToEnd(char* userStr);
void printList();

listElement* head = NULL;

int main(){
    int userNum;

    do{
        printMenu();
        scanf("%d", &userNum);

        menu(userNum);

    } while (userNum != 4);
    
    return 0;
}
//Print user menu
void printMenu(){
    puts("\n\n--------LINKED LIST MENU--------");
    puts("\nSelect option number:\n1. Push String\n2. Print List\n3. Delete Item\n4. End Program");
    fputs("\n--------------------------------\n:", stdout);
    return;
}

listElement* createNode(char* userStr){
    listElement* newNode = (listElement* )malloc(sizeof(listElement));//define and allocate space for new node
    
    strcpy(newNode->text, userStr);
    newNode->next = NULL;
    newNode->prev = NULL;
    return newNode;
}

void addToEnd(char* userStr){
    listElement* tempHead = head;
    listElement* newElement = createNode(userStr);
    // Case: Empty list. Set the head pointer.
    if (head == NULL){
        head = newElement;
        printf("\"%s\" added to the end of the list\n", userStr);
        return;
    }
    while(tempHead->next != NULL)// Find end of list
        tempHead = tempHead->next;
    tempHead->next = newElement;
    newElement->prev = tempHead;
    printf("\"%s\" added to the end of the list\n", userStr);

    return;
}
//TODO
//listElement *addToHead(char* userString)
//TODO
//listElement *addToIndex(char* userString)

void removeElement(char* text){
    listElement* tempHead = head;

    while (tempHead != NULL && strcmp(tempHead->text, text))// Find element to delete
        tempHead = tempHead->next;
    
    // Case: No match or empty list
    if (tempHead == NULL){
        printf("\n  !!STRING \"%s\" IS NOT IN LIST!!", text);
        return;
    }
    
    // Remove head
    if (tempHead->prev == NULL){
        if (tempHead->next == NULL){// Case: One item in list
            head = NULL;
            //free(tempHead);// Unsure if this is needed.
        }
        else{
            head = tempHead->next;// Case: More than one item in list
            head->prev = NULL;
        }

        free(tempHead);
        return;
    }
    
    // Remove tail
    if (tempHead->next == NULL){
        tempHead->prev->next = NULL;
        
        free(tempHead);
        return;
    }

    // Remove from middle
    if (tempHead->prev != NULL && tempHead->next != NULL){
        tempHead->prev->next = tempHead->next;
        tempHead->next->prev = tempHead->prev;

        free(tempHead);
        return;
    }
    return;
}


void printList(){
    listElement* tempHead = head;
    int currentElement = 1;
    fputs("\n   -----CONTENTS OF LIST-----",stdout);
    while(tempHead != NULL){
        printf("\n   %d. %s", currentElement, tempHead->text);
        tempHead = tempHead->next;
        currentElement++;
    }
    puts("\n   --------------------------");
    return;
}
//TODO
// void reverseprint(listElement *head)
// void fowardprintfromelement()
// void reveseprintfromelement()

void menu(int userSel){
    int userInput = userSel;
    char userStr[1000];
    
    if (userInput > 4){
        puts("  !!Invalid Selections!!");
        return;
    }
    
    switch (userInput)
    {
    case PUSH_STRING:
        fputs("Enter a string: ", stdout);
        scanf("%s", userStr);
        addToEnd(userStr);
        break;
    case PRINT_LIST:
        printList();
        break;
    case DELETE_ITEM:
        printList();
        fputs("\nEnter a string to remove from the list above: ", stdout);
        scanf("%s", userStr);
        removeElement(userStr);
        break;
    case END_PROGRAM:
        puts("Ending program...\n");
        break;
    default:
        break;
    }
    return;
}
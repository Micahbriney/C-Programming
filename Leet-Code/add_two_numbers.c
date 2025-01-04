/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     struct ListNode *next;
 * };
 */

 struct ListNode* createNewNode(int val);
 void insertNodeEnd(struct ListNode **head, int val);

struct ListNode* addTwoNumbers(struct ListNode* l1, struct ListNode* l2) {
    struct ListNode *head;
    int sum = 0;

    while(NULL != l1 || NULL != l2){
        if(NULL != l1){
            sum += l1->val;
            l1 = l1->next;
        }
        if(NULL != l2){
            sum += l2->val;
            l2 = l2->next;
        }
        insertNodeEnd(&head, sum % 10);
        sum = sum / 10;
    }

    if(sum > 0)
        insertNodeEnd(&head, sum % 10);

    return head;
}

struct ListNode* createNewNode(int val){
    struct ListNode* newNode;
    if(NULL == (newNode = (struct ListNode*)malloc(sizeof(struct ListNode)))){
        perror("createNewNode() struct ListNode newNode malloc()");
        exit(-1);
    }
    newNode->val = val;
    newNode->next = NULL;

    return newNode;
}

void insertNodeEnd(struct ListNode **head, int val){
    struct ListNode* newNode;
    newNode = createNewNode(val);

    if(NULL == *head){
        *head = newNode;
        return;
    }

    struct ListNode *temp = *head;
    while(NULL != temp->next){
        temp = temp->next;
    }
    temp->next = newNode;;
    
    return;
}
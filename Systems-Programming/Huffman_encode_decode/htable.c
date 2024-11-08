/**
 ***************************************************************************
 * @author : Micah Briney
 * @date   : 10-15-2022
 * @file   : htable.c
 * @brief  : htable will take in a provided file from the terminal or from
 *           stdin. The file will be read using unbuffered I/O and sent
 *           to stdout. In between the reading and writing stage a huffman
 *           table will be created. This is done by parsing the input one
 *           char at a time. The frequency of the char is put into an 
 *           array. The array is then sorted by frequency and then 
 *           alphabetically. The sorted array is then turned into a list
 *           of structs that hold the character, count, and pointers for
 *           the preceding list node. The stucts also contain room for
 *           child nodes and an encoded value of the char. After the list
 *           is created it will be converted to a huffman tree using a
 *           process where a super node is created from the first two
 *           nodes in the list. The super node contains a value of the 
 *           sum of the two chid node frequencies. The super node is then
 *           inserted back into the list making sure to maintain the 
 *           sorted feature. When the process is done all that remains is
 *           a tree. The next step is to determine the encode of the node
 *           this is done by traversing the tree untill a child leaf is 
 *           reached. The lenght of the encode is determined by the number
 *           of branches taken and where traversing left = 0 right = 1. 
 *           The encode is a representation of the traversal in binary chars
 *           ie a traveral of left right left would equal 010.
 *
 ***************************************************************************
 */

#include "htable.h"

int htable(int argc, char *argv[]){
  char *infilename;
  int infd;
  int outfd;
  int uniqchars = 0;
  /*int *encodedbuff;*/
  uint32_t freqArr[ARRSIZECHAR_][ARRSIZECOUNT_];
  hnode histogram[ARRSIZECHAR_];
  char encodedArr[ENCODESIZE_];
  listptr head;
  treeptr root;
  
  if(argc > 2) /* To many inputs.*/
    usage_htable_error(argv[0]);
  else if(argc == 2){ /* File or usage error*/
    infilename = argv[argc - 1];
    if(-1 == (infd = open(infilename, O_RDONLY))){
      perror("Open file");
      exit(EXIT_FAILURE);
    }
  }
  else{ /* Stdin or usage error*/
    infilename = "STDIN_FILENO";
    infd = STDIN_FILENO;
  }

  if(lseek(infd, 0, SEEK_CUR) == -1){ /* Check if file is seekable*/
    printf("File is empty ");
    /* TODO Handle empty file case: Output empty file?*/
  }

  /* ----------------------- TODO Error checking stdout */
  outfd = STDOUT_FILENO;

  /* Read input*/
  read_input(infd, freqArr);

  /* Find the number of unique charaters that exist in the file*/
  uniqchars = get_uniqnum(freqArr);

  /* Initilized an array of histogram nodes*/
  create_histogram(histogram);
  
  /* Create an array of histogram nodes*/
  build_histogram(histogram, freqArr);

  /* Sort histogram by count. They remain sorted alphabetically on tie*/
  qsort(histogram, uniqchars, sizeof(hnode), compare_count);

  /* Create list and return head*/
  head = init_list(histogram, uniqchars);

  /* Turn list into tree*/
  root = init_tree(head);

    
  /* For testing Tree & List order*/
  /*print_structures()*/

  print_bits(root, encodedArr);
  
  
  return 0;
}


/*
 *@fn: read_input
 *@brief: Read unbuffered input from either stdin or infile.
 *         First create a 2D array[row][col] to store the count at the 
 *         location of the char found. The first index represents the chars
 *         and the second index will hold the count of the chars found.
 *         ie. If a newline (ascii 10) is found then at freqArr[10][0] 
 *         increment the contents of the the column. 
*/
void read_input(int infd, uint32_t freqArr[][ARRSIZECOUNT_]){
  char buf[BUFFERSIZE_]; /* Read contents into buf*/
  int n, i;
  
  /* Scrub freqArr*/
  for(i = 0; i < ARRSIZECHAR_; i++)
    freqArr[i][0] = 0;   /* Zero out count*/
  
  while(0 < (n = read(infd, buf, BUFFERSIZE_))){ /* Read a chunk*/
    if(n < 0){ /* Problem with reading input file*/
      perror("Read infile");
      exit(EXIT_FAILURE);
    }
    
    /* Start building the frequency array*/
    for(i = 0; i < n; i++)
      freqArr[(int)buf[i]][0]++;/* Increment frequency @ location of char*/
  }
  return;
}


/*
 *@fn: get_uniqnum
 *@brief: This function will loop through the frequency array to find
 *         the total number of uniqchars found. 
*/
int get_uniqnum(uint32_t freqArr[][ARRSIZECOUNT_]){
  int uniqnum, i;

  uniqnum = 0;
  /* loop through 2D freqArr array looking for non zero columns*/
  for(i = 0; i < ARRSIZECHAR_; i++)
    if(freqArr[i][0] != 0) /* If a count is found*/
      uniqnum++; /* Increment uniq chars found*/

  return uniqnum;
}


/*
 *@fn: create_histogram
 *@brief: This function will create and initialize a histogram of
 *         the correct size using the uniqchars.  
*/
void create_histogram(hnode histogram[]){
  int i;

  for(i = 0; i < ARRSIZECHAR_; i++){
    histogram[i].count = 0;
    histogram[i].c = 0;
    histogram[i].next = NULL;
    histogram[i].left = NULL;
    histogram[i].right = NULL;
    histogram[i].codestr = NULL;
  }
  
  return;  
}


/*
 *@fn: build_histogram
 *@brief: Put the contents of the frequency array into the 
 *         hnode histogram. This will only occue when a count is not
 *         zero.
*/
void build_histogram(hnode histogram[],
		     uint32_t freqArr[][ARRSIZECOUNT_]){
  int i, j;
  
  for(i = 0, j = 0; i < ARRSIZECHAR_; i++){
    if(freqArr[i][0] != 0){
      histogram[j].count = freqArr[i][0];
      histogram[j++].c = i;
    }    
  }
  return;
}

/*
 *@fn: init_list
 *@brief: Create a list from the histogram.
*/
listptr init_list(hnode histogram[], int uniqchars){
  listptr head;
  listptr curr;
  int i;

  head = create_node();
    
  head = histogram;
  if(uniqchars == 1)
    return head;
  
  curr = histogram;
  
  for(i = 0;i < uniqchars; i++){
    curr->next = &histogram[i];
    if(curr->next->count != 0)
      curr = curr->next;
  }
  return head;
}

/*
 *@fn: compare_count
 *@brief: Compare function for qsort algorithm. It returns the smallest
 *         of the two values compared to order this from smallest to
 *         largest.
*/
int compare_count(const void *curr, const void *next){
  hnode *currptr = (hnode *)curr;
  hnode *nextptr = (hnode *)next;

  return -1 * (nextptr->count - currptr->count);  
}


/**
 *@fn: truncate_list
 *@brief: This function will truncate a list based on a provided limit.
 *@params: listptr, int
 *@return: none
 **/
listptr truncate_list(listptr head, int limit){
  listptr temp, nextptr;
  int i;

  temp = head;
  
  for(i = 0; i < limit; i++)
    if(temp->next)
      temp = temp->next;

  nextptr = temp->next;
  free_list(nextptr);
  temp->next = NULL;

  return head;
}

void free_list(listptr head){
  listptr prev;

  prev = head;
  
  while(head){
    if(head->next){
      head = head->next;
      free(prev);
      prev = head;
    }
    else
      free(head);
  }
  return;
}

/*
 *@fn: init_tree
 *@brief: Create a binary tree from the sorted list using Huffman 
 *         principles of less frequently charactes having a longer encode.
 *         And more frequently use characters having a shorter encode. The
 *         length of the encode is determined by how far down the tree the
 *         traversal will go. Left is a 0 bit and right is a 1 bit.
*/
treeptr init_tree(listptr head){
  treeptr root, newsupernode;
  listptr curr;
  curr = head;
  root = head;

  /* Case 1 Only one character found*/
  if(!curr->next)
    return root;

  /* Case 2 Only two characters found in file*/
  while(curr->next){
    if(!curr->next->next)
      return root = create_super_node(curr, curr->next);
    else{ /* Case 3, 4 more than 2 characters found*/
      /*while(head && head->next && head->next->next){*/
      newsupernode = create_super_node(curr, curr->next);
      curr = curr->next->next; /* Move head after super nodes right child*/
      insert_super_node(newsupernode, curr);
      /* if(head && head->next && head->next->next) */
      /*head = head->next->next; /\* Move head past the two old nodes*\/ */
      /*else
	head = root;*/
    }
    /* Finish with the last two in the list*/
  }
    
  return root = curr;
}


/*
 *@fn: create_node
 *@brief: Create an empty binary tree node.
*/
treeptr create_node(void){
  treeptr newnode;
  if(!(newnode = (hnodeptr)malloc(sizeof(hnode)))){
    perror("Malloc newsupernode");
    exit(EXIT_FAILURE);
  }
  
  newnode->count = 0;
  newnode->c = 0;
  newnode->next = NULL;
  newnode->left = NULL;
  newnode->right = NULL;
  newnode->codestr = NULL;
  
  return newnode;
}

/*
 *@fn: create_super_node
 *@brief: Create a super node from the current and next list nodes 
 *         if they exist.
*/
treeptr create_super_node(listptr curr, listptr next){
  treeptr newsupernode;

  newsupernode = create_node();
  newsupernode->count = curr->count + next->count; /* Store sum count*/
  newsupernode->left  = curr;
  newsupernode->right = next;
  
  return newsupernode;
}


/*treeptr insert_super_node(treeptr newsupernode, listptr head){*/
void insert_super_node(treeptr newsupernode, listptr head){
  listptr curr, prev = NULL;

  curr = head;
  
  if(newsupernode->count <= curr->count){ /* Case 3 Insert at head*/
    newsupernode->next = curr; /* Insert at start of list/tree*/
    head = newsupernode;       /* Point head back to head of list/tree*/   
  }
  else{ /* Case 4,5 Insert at some point after head of list*/
    /* Either we are at the end of the list, or between two nodes */
    while(curr->next && (newsupernode->count > curr->count)){
      prev = curr;
      curr = curr->next;
    }

    /* Case 4 put between prev and curr when curr->count is larger*/
    if(newsupernode->count <= curr->count){
      newsupernode->next = curr;
      prev->next = newsupernode;
    } /* Case 5 at end of list */
    else if(!curr->next){
      curr->next = newsupernode; /* Put new supernode at the end*/
    }
  }

  return;
}


/*
 * @fn: print_bits
 * @brief: 
 */
void print_bits(treeptr root, char encodedArr[]){
  int index, i;
  /* Scrub encodedArr*/
  for(i = 0; i < ENCODESIZE_; i++)
    encodedArr[i] = 0;   /* Zero out count*/
  index = 0;
  encode_post_order_traversal(root, index, encodedArr);

  print_pre_order_traversal(root, print_tree_table_code);
  
}

/*
 * @fn: encode_pre_order_traversal
 * @brief: The function will traverse the tree using a pre_order_traversal.
 *          for each left node it finds the the index will increment and
 *          the encode array will store a "0" at the index. The index 
 *          will then incremented for the next recursive call. If no more
 *          left nodes are found the it will store  the number of
 *          traversal calls in the encode_pre_order_traversal function. 
 *          It comes from the index variable found there. The encodeArr is
 *          an array that holds the encoded value created during the  
 *          pre_order_traversal process. This array has ENCODESIZE_ number 
 *          of elements which is why size is so important.
 * @params: treeptr root, int size, encodeArr
 */
void encode_post_order_traversal(treeptr root, int index, char encodeArr[]){
 
  if(root){
    if(root->left){
      encodeArr[index++] = T_LEFT;
      encode_post_order_traversal(root->left, index, encodeArr);
      --index;
    }
    if(root->right){
      encodeArr[index++] = T_RIGHT;
      encode_post_order_traversal(root->right, index, encodeArr);
      --index;
    }
    /* Root doesn't have left or right node. Therefore its not a supernode*/
    set_encode_str(root, index, encodeArr);      
    if(index > 0)
      --index;
    
  }
  return;
}

/*
 * @fn: set_encode_st
 * @brief: This function will copy the encode array value found by the
 *          encode_pre_order_traversal function to the tree node's codestr.
 *          The treeptr root is the current node that is looking to have
 *          its codestr added.The size value is calculated by the number of
 *          traversal calls in the encode_pre_order_traversal function. 
 *          It comes from the index variable found there. The encodeArr is
 *          an array that holds the encoded value created during the  
 *          pre_order_traversal process. This array has ENCODESIZE_ number 
 *          of elements which is why size is so important.
 * @params: treeptr root, int size, encodeArr
 */
void set_encode_str(treeptr root, int size, char encodeArr[]){

  /* Allocate space for the nodes codestr*/
  if(!(root->codestr = (char *)malloc((sizeof(char) * ENCODESIZE_)) + 1)){
    perror("Malloc codestr");
    exit(EXIT_FAILURE);
  }
  
  /* Copy size amount of encodeArr to root->codestr*/
  strncpy(root->codestr ,encodeArr, size + 1);

}

void print_tree_table_code(treeptr node){
  if(!(node->left) && !(node->right))
    printf(" 0x%02x: %s : %c\n", node->c, node->codestr, node->c);
  return;
}

/*
 *@fn: print_pre_order_traversal
 *@brief: This function will traverse the tree using a pre order traversal.
 *         When it does it will print the root and its children. Then it 
 *         will traverse left from the root and print its children. It will
 *         then return to the root and traverse right and print its
 *         children. This will continue printing from the top down 
 *         starting with the left branches and working right wards.
*/
void print_pre_order_traversal(treeptr root,
			       void (*print_tree_node)(treeptr node)){
  if(root){
    print_tree_node(root);
    if(root->left)
      print_pre_order_traversal(root->left, print_tree_node);
    if(root->right)
      print_pre_order_traversal(root->right, print_tree_node);
  }
  return;
}


void print_tree_node_full(treeptr node){ 
  printf("\t\t\tCount: %d | Letter: '%c' \n\n", node->count,
	 node->c ? node->c : '0');
  printf("Left: count: %d letter: '%c'",
	 node->left ? node->left->count : -1,
	 node->left ? node->left->c : '~');
  printf("\t\t Right: count: %d letter: %c\n\n",
	 node->right ? node->right->count : -1,
	 node->right ? node->right->c : '~');
  return;
}


void print_ordered_list(hnode histogram[]){
  int j;
  for(j = 0; j < ARRSIZECHAR_; j++){
    printf("{Count = %d, Char = %c}\n",histogram[j].count, histogram[j].c);
  }
}

void print_structures(treeptr root, hnode histogram[]){
  print_ordered_list(histogram);
  print_pre_order_traversal(root, print_tree_node_full);
}

/**
 *@fn: usage_error
 *@brief: This function will print usage errors when the user's input 
 *         does not match the usage rules.
 **/

void usage_hdecode_error(char *prog){
  fprintf(stderr,"Usage %s [ (infile | -) [outfile]].\n", prog);
  exit(EXIT_FAILURE);
}

void usage_hencode_error(char *prog){
  fprintf(stderr,"Usage %s infile [outfile].", prog);
  exit(EXIT_FAILURE);
}

void usage_htable_error(char *prog){
  fprintf(stderr,"Usage %s infile [outfile].", prog);
  exit(EXIT_FAILURE);
}


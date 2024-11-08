#ifndef WLH
#define WLH

typedef struct List_ *WordListPtr;

typedef struct List_{
  char* word;
  int count;
  WordListPtr next;
} List;

/* Create Functions*/
WordListPtr initWordList();
WordListPtr insertListNode(char *word, int count);
WordListPtr createListNode(char *word, int count);

/* Operation*/
void sort(WordListPtr wordlist);

/* get list*/
WordListPtr getWordListPtr();

  
#endif

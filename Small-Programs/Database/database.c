#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "readlongline.h"

#define CHUNK 1024
#define LAST_NAME_LEN 20
#define FIRST_NAME_LEN 20
#define EXTRA_SPACE 10
#define MAX_CMD_LEN 8
#define MAX_SUB_CMD_LEN 5
#define INPUT_LEN FIRST_NAME_LEN + MAX_CMD_LEN + MAX_SUB_CMD_LEN + EXTRA_SPACE
#define INPUT_BUFF INPUT_LEN
#define MAX_GRADE 6
#define MIN_GRADE 0
#define MAX_GPA 5.0
#define MIN_GPA 0.0

enum PrintFlag {NO, YES};
enum Validity {INVALID, VALID};
enum Commands {QUIT, STUDENT, TEACHER, BUS, GRADE, AVERAGE, INFO};
enum Fields {S_LAST, S_FIRST, S_GRADE, S_CLASSROOM, S_BUS, S_GPA, T_LAST, T_FIRST};

typedef struct Student {
  char student_last[LAST_NAME_LEN], student_first[FIRST_NAME_LEN];
  int grade, classroom, bus;
  float GPA;
  char teacher_last[LAST_NAME_LEN], teacher_first[FIRST_NAME_LEN];
} Student;

void printUserPrompt(void);
void printFlags(int *st_last_flag,
		            int *st_first_flag,
                int *grade_flag,
                int *class_flag,
                int *bus_flag,
                int *GPA_flag,
                int *tch_last_flag,
                int *tch_first_flag,
                char *normalizedCmd,
                char *lastname,
                char *subcmd,
                int grade);
void toLower(char *userInput, int size);
int isValidCommand(const char *cmd, const char *cmdWords[], int listSize);
void findStudent(int *found, Student *studentRecords, int size, char *lastname);
void findTeacher(int *found, Student *studentRecords, int size, char *lastname);
void findBus(int *found, Student *studentRecords, int size, int grade);
void findGrade(int *found, Student *studentRecords, int size, int grade);
float averageGPA(int *found, Student *studentRecords, int size);
void printInfo(Student *studentRecords, int size);
float findLowestGPA(int *found, Student *studentRecords, int size, int grade);
float findHighestGPA(int *found, Student *studentRecords, int size, int grade);
void printReport(int *found, 
                 Student *studentRecords, 
                 int size, 
                 int st_last_flag,
		             int st_first_flag,
                 int grade_flag,
                 int class_flag,
                 int bus_flag,
                 int GPA_flag,
                 int tch_last_flag,
                 int tch_first_flag);
void parseFile(Student *studentRecords);


int main(){
  // User input data
  char cmd[MAX_CMD_LEN];
  char field[FIRST_NAME_LEN];     // Unknown userinput type
  char lastname[LAST_NAME_LEN];
  char subcmd[MAX_SUB_CMD_LEN];
  // char words[WORDS_LEN];
  char words[INPUT_LEN];
  long int busOrGradeNum;      // Unknown purpose int
  // Print flags
  int st_last_flag;
  int st_first_flag;
  int grade_flag;
  int class_flag;
  int bus_flag;
  int GPA_flag;
  int tch_last_flag;
  int tch_first_flag;
  // Ctrl flags
  int validCmd;
  int validSubCmd;
  int quitCmd = NO;
  errno = 0;      // Used for zero case check of user input
  char *endPtr;   // Used to check if the field is an int or string
  int found[60];  // Location found = 1
  int sizeRecords;
  float avg = 0.0;
  const char *cmdWords[] = {
    "quit",
    "student",
    "teacher",
    "bus",
    "grade",
    "average",
    "info"
  };
    
  const char *subCmdWords[] = {
    "bus",
    "high",
    "low",
    ""
  };

  Student studentRecords[60] = {0};
  parseFile(studentRecords);
  
  // #define st_last_flag 1
  // #define st_first_flag 2
  // #define st_first_flag 4

  while(quitCmd == NO){
    // initilize flags
    st_last_flag = NO;
    st_first_flag = NO;
    grade_flag = NO;
    class_flag = NO;
    bus_flag = NO;
    GPA_flag = NO;
    tch_last_flag = NO;
    tch_first_flag = NO;
    validCmd = NO;
    validSubCmd = NO;
    quitCmd = NO;

    memset(found, 0, sizeof(found));
    memset(words, 0, sizeof(words));
    memset(cmd, 0, sizeof(cmd));
    memset(subcmd, 0, sizeof(subcmd));
    memset(lastname, 0, sizeof(lastname));
    memset(field, 0, sizeof(field));
      
    sizeRecords = sizeof(studentRecords) / sizeof(studentRecords[0]);
        
    // Print tool menu
    printUserPrompt();
        
    fgets(words, INPUT_BUFF, stdin);
        
    sscanf(words, "%s %s %s", cmd, field, subcmd);
        
    // Convert user input to lower case
    toLower(cmd, strlen(cmd));
    toLower(subcmd, strlen(subcmd));
        
    // Check for valid commands entered
    validCmd = isValidCommand(cmd, 
      cmdWords, 
      (sizeof(cmdWords) / sizeof(cmdWords[0])));
        
    // Check for program quit
    if(cmd[0] == cmdWords[QUIT][0] && validCmd == YES){
      quitCmd = YES;
      continue;
    }
        
    // Check for valid sub command entered
    validSubCmd = isValidCommand(subcmd, 
      subCmdWords, 
      (sizeof(subCmdWords) / sizeof(subCmdWords[0])));

    // Invalid input sentinal
    if(validCmd == NO || (validSubCmd == NO && subcmd[0] != '\0')){
      printf("\nYou entered: %sThis is not a valid command. Try again.\n",
	     words);
      continue;
    }
        
    // if field is an int save it as long int
    // This will save busOrGradeNum as 0 upon failure
    busOrGradeNum = strtol(field, &endPtr, 10);
        
    if (errno != 0) {
      perror("strtol");
      exit(EXIT_FAILURE);
    }
        
    // Type of field is String
    if (endPtr == field) {
      strcpy(lastname, field);
      busOrGradeNum = -1; // change strtol failure to -1
    }
        
    // Set print flags
    printFlags(&st_last_flag,
      &st_first_flag,
      &grade_flag,
      &class_flag,
      &bus_flag,
      &GPA_flag,
      &tch_last_flag,
      &tch_first_flag,
      cmd,
      lastname,
      subcmd,
      busOrGradeNum);
        
    switch (cmd[0]){
      case 's':
        if(subcmd[0] == 'b'){
          /*DO NOTHING;*/
        }
        findStudent(found, studentRecords, sizeRecords, lastname);
        break;
      case 't':
        findTeacher(found, studentRecords, sizeRecords, lastname);
        break;
      case 'b':
        findBus(found, studentRecords, sizeRecords, busOrGradeNum);
        break;
      case 'g':
        if(subcmd[0] == 'h'){
          findHighestGPA(found, studentRecords, sizeRecords, busOrGradeNum);
          break;
        }
        else if(subcmd[0] == 'l'){
          findLowestGPA(found, studentRecords, sizeRecords, busOrGradeNum);
          break;
        }
        findGrade(found, studentRecords, sizeRecords, busOrGradeNum);
        break;
      case 'a':
        avg = 0.0;
        if(busOrGradeNum > MAX_GRADE || busOrGradeNum < MIN_GRADE){
          printf("Grade %ld is an invalid grade. Try grades 0-6.\n", busOrGradeNum);
          continue;
        }
        findGrade(found, studentRecords, sizeRecords, busOrGradeNum);
        avg = averageGPA(found, studentRecords, sizeRecords);
        if(avg == 0.0){
          printf("No average found for grade %ld\n",busOrGradeNum);
          continue;
        }
        printf("Grade %ld average GPA: %0.2f\n", busOrGradeNum,avg);
        continue;
      case 'i':
        printInfo(studentRecords, sizeRecords);
        continue;
      default:
        break;
    }

    // Check for found records
    int j = 0;
    for(int i = 0; i < sizeRecords; i++){
      if(found[i] == 1){
        j++;
        break;
      }
    }

    if(j == 0){
      printf("No record(s) found\n");
      continue;
    }
        
    printReport(found, 
      studentRecords, 
      sizeRecords, 
      st_last_flag,
      st_first_flag,
      grade_flag,
      class_flag,
      bus_flag,
      GPA_flag,
      tch_last_flag,
      tch_first_flag);
        
  }
  return 0;
}


void printUserPrompt(void){
  //User Prompt
  printf("\n-----Welcome to the School Search tool.-----\n\n" 
	 "Please select one of the following options\n"
	 "• S[tudent]: <lastname> [B[us]]\n"
	 "• T[eacher]: <lastname>\n"
	 "• B[us]: <number>\n"
	 "• G[rade]: <number> [H[igh]|L[ow]]\n"
	 "• A[verage]: <number>\n"
	 "• I[nfo]\n"
	 "• Q[uit]\n\n-");
  return;
}


void printFlags(int *st_last_flag,
  int *st_first_flag,
  int *grade_flag,
  int *class_flag,
  int *bus_flag,
  int *GPA_flag,
  int *tch_last_flag,
  int *tch_first_flag,
  char *cmd,
  char *lastname,
  char *subcmd,
  int grade){
    
  switch (cmd[0]){
    case 's':
      if(subcmd[0] == 'b'){
        *st_last_flag = YES;
        *st_first_flag = YES;
        *grade_flag = NO;
        *class_flag = NO;
        *bus_flag = YES;
        *GPA_flag = NO;
        *tch_last_flag = NO;
        *tch_first_flag = NO;
            
        break;
      }
      *st_last_flag = YES;
      *st_first_flag = YES;
      *grade_flag = YES;
      *class_flag = YES;
      *bus_flag = NO;
      *GPA_flag = NO;
      *tch_last_flag = YES;
      *tch_first_flag = YES;
      break;
    case 't':
      *st_last_flag = YES;
      *st_first_flag = YES;
      *grade_flag = YES;
      *class_flag = NO;
      *bus_flag = NO;
      *GPA_flag = NO;
      *tch_last_flag = NO;
      *tch_first_flag = NO;   
      break;
    case 'b':
      *st_last_flag = YES;
      *st_first_flag = YES;
      *grade_flag = YES;
      *class_flag = YES;
      *bus_flag = NO;
      *GPA_flag = NO;
      *tch_last_flag = YES;
      *tch_first_flag = YES;
      break;
    case 'g':
      if(subcmd[0] == 'h'){
        *st_last_flag = YES;
        *st_first_flag = YES;
        *grade_flag = NO;
        *class_flag = NO;
        *bus_flag = YES;
        *GPA_flag = YES;
        *tch_last_flag = YES;
        *tch_first_flag = YES;
        // TODO Search for highest GPA function call
        break;
      }
      else if(subcmd[0] == 'l'){
        *st_last_flag = YES;
        *st_first_flag = YES;
        *grade_flag = NO;
        *class_flag = NO;
        *bus_flag = YES;
        *GPA_flag = YES;
        *tch_last_flag = YES;
        *tch_first_flag = YES;
        // TODO Search for lowest GPA function call
        break;
      }
      *st_last_flag = YES;
      *st_first_flag = YES;
      *grade_flag = NO;
      *class_flag = NO;
      *bus_flag = NO;
      *GPA_flag = NO;
      *tch_last_flag = NO;
      *tch_first_flag = NO;
      break;
    case 'a':
      *st_last_flag = NO;
      *st_first_flag = NO;
      *grade_flag = YES;
      *class_flag = NO;
      *bus_flag = NO;
      *GPA_flag = NO;
      *tch_last_flag = NO;
      *tch_first_flag = NO;
      break;
    case 'q':
      //End Program
      break;
    default:
      break;
  }
    
}

void toLower(char *cmd, int size){
    
  for(int i = 0; i < size; i++){
    if(isalpha(cmd[i])){
      cmd[i] = tolower((unsigned char)cmd[i]);
    }
    else if(ispunct(cmd[i])){
      cmd[i] = '\0';
    }
  }
  return;
}


int isValidCommand(const char *cmd, const char *cmdWords[], int listSize){
  int validCmd;

  for(int i = 0; i < listSize; i++){
    if(!strcmp(cmd, cmdWords[i])){
      validCmd = YES;
      break;
    }
    else if(strlen(cmd) == 1 && cmd[0] == cmdWords[i][0]){
      validCmd = YES;
      break;
    }
    else{
      validCmd = NO;
    }
  }
  return validCmd;
}


void findStudent(int *found, Student *studentRecords, int size, char *lastname){
    
  for(int i = 0; i < size; i++){
    if(!(strcmp(studentRecords[i].student_last, lastname))){
      found[i] = 1;
    }
  }
    
  return;
}


void findTeacher(int *found, Student *studentRecords, int size, char *lastname){
    
  for(int i = 0; i < size; i++){
    if(!(strcmp(studentRecords[i].teacher_last, lastname))){
      found[i] = 1;
    }
  }
  return;
}


void findBus(int *found, Student *studentRecords, int size, int bus){
 
  for(int i = 0; i < size; i++){
    if(studentRecords[i].bus == bus){
      found[i] = 1;
    }
  }
  return;
}


void findGrade(int *found, Student *studentRecords, int size, int grade){
        
  for(int i = 0; i < size; i++){
    if(studentRecords[i].grade == grade){
      found[i] = 1;
    }
  }
  return;
}


float averageGPA(int *found, Student *studentRecords, int size){
    
  float totalGPA = 0;
  int count = 0;
    
  for(int i = 0; i < size; i++){
    if(found[i] == 1){
      totalGPA += studentRecords[i].GPA;
      count++;
    }
  }

  if(count == 0 && totalGPA == 0){
    return totalGPA;
  }
    
  return totalGPA/count;
}


void printInfo(Student *studentRecords, int size){
    
  int gradeCounter[7] = {0};
  for(int i = 0; i < size; i++){
    gradeCounter[studentRecords[i].grade]++;
  }
    
  for(int i = 0; i < (sizeof(gradeCounter)/sizeof(gradeCounter[0])); i++){
    printf("Grade %d: %d\n",i, gradeCounter[i]);
  }
  return;
}


float findLowestGPA(int *found, Student *studentRecords, int size, int grade){

  findGrade(found, studentRecords, size, grade);
  
  float lowestGPA = MAX_GPA;
  // find lowest GPA
  for(int i = 0; i < size; i++){
    if(found[i] == 1){
      if(lowestGPA > studentRecords[i].GPA){
	      lowestGPA = studentRecords[i].GPA;
      }
    }
  }
  // Find students that match the lowest GPA
  for(int i = 0; i < size; i++){
    if(found[i] == 1){
      if(lowestGPA == studentRecords[i].GPA){
	      continue;
      }
      else{
	      found[i] = 0;
      }
    }
  }
  return lowestGPA;
}


float findHighestGPA(int *found, Student *studentRecords, int size, int grade){

  findGrade(found, studentRecords, size, grade);

  float highestGPA = MIN_GPA;
  // find highest GPA
  for(int i = 0; i < size; i++){
    if(found[i] == 1){
      if(highestGPA < studentRecords[i].GPA){
	      highestGPA = studentRecords[i].GPA;
      }
    }
  }
  // Find students that match the lowest GPA
  for(int i = 0; i < size; i++){
    if(found[i] == 1){
      if(highestGPA == studentRecords[i].GPA){
	      continue;
      }
      else{
	      found[i] = 0;
      }
    }
  }
  return highestGPA;
}


void printReport(int *found, Student *studentRecords, int size, 
		 int st_last_flag,
		 int st_first_flag,
		 int grade_flag,
		 int class_flag,
		 int bus_flag,
		 int GPA_flag,
		 int tch_last_flag,
		 int tch_first_flag){
  
  // TODO Solve for possible comma issues
  for(int i = 0; i < size; i++){
    if(found[i] == 1){
      if(st_last_flag){
	      printf("%s", studentRecords[i].student_last);
      }
      if(st_first_flag){
	      printf(",%s", studentRecords[i].student_first);
      }
      if(grade_flag){
	      printf(",%d", studentRecords[i].grade);
      }
      if(class_flag){
	      printf(",%d", studentRecords[i].classroom);
      }
      if(bus_flag){
	      printf(",%d", studentRecords[i].bus);
      }
      if(GPA_flag){
	      printf(",%0.2f", studentRecords[i].GPA);
      }
      if(tch_last_flag){
	      printf(",%s", studentRecords[i].teacher_last);
      }
      if(tch_first_flag){
	      printf(",%s", studentRecords[i].teacher_first);
      }
      printf("\n");
    }
  }
  printf("\n");
}


void parseFile(Student *studentRecords){

  // Read file
  FILE *fp = fopen("students.txt", "r");
  int i = 0;
  int fieldNum = 0;

  if(!fp){
    perror("fopen");
    exit(EXIT_FAILURE);
  }
 
  char *currLine;
  while(NULL != (currLine = readLongLine(fp))){
    
    char *data;
    char *endPtr;
    data = strtok(currLine, ",");
    while(data){
      switch(fieldNum){
        case S_LAST:
          strncpy(studentRecords[i].student_last, data, LAST_NAME_LEN);
          break;
        case S_FIRST:
          strncpy(studentRecords[i].student_first, data, FIRST_NAME_LEN);
          break;
        case S_GRADE:
          studentRecords[i].grade = strtol(data, &endPtr, 10);
          break;
        case S_CLASSROOM:
          studentRecords[i].classroom = strtol(data, &endPtr, 10);
          break;
        case S_BUS:
          studentRecords[i].bus = strtof(data, &endPtr);
          break;
        case S_GPA:
          studentRecords[i].GPA = strtof(data, &endPtr);
          break;
        case T_LAST:
          strncpy(studentRecords[i].teacher_last, data, LAST_NAME_LEN);
          break;
        case T_FIRST:
          strncpy(studentRecords[i].teacher_first, data, FIRST_NAME_LEN);
          break;
        default:
            //TODO error out. Change to default case?
        }
        data = strtok(NULL, ",");
        fieldNum++;
    }
    i++;
    fieldNum = 0;
  }

  fclose(fp);

  return;
}


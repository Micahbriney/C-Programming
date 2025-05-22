#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#define CHUNK 1024

enum PrintFlag {No, Yes};

typedef struct Student {
  char student_last[20], student_first[20];
  int grade, classroom, bus;
  float GPA;
  char teacher_last[20], teacher_first[20];
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
void findStudent(int *found, Student *studentRecords, int size, char *lastname);
void findTeacher(int *found, Student *studentRecords, int size, char *lastname);
void findBus(int *found, Student *studentRecords, int size, int grade);
void findGrade(int *found, Student *studentRecords, int size, int grade);
float averageGPA(int *found, Student *studentRecords, int size);
void printInfo(Student *studentRecords, int size);
float findLowestGPA(int *found, Student *studentRecords, int size, int grade);
float findHighestGPA(int *found, Student *studentRecords, int size, int grade);
void printReport(int *found, Student *studentRecords, int size, 
                int st_last_flag,
		        int st_first_flag,
                int grade_flag,
                int class_flag,
                int bus_flag,
                int GPA_flag,
                int tch_last_flag,
                int tch_first_flag);
void parseFile(Student *studentRecords);


int main()
{
  // User input data
  char cmd[20];
  char nameOrNum[20];     // Unknown userinput type
  char lastname[20];
  char subcmd[10];
  char words[50];
  long int busOrGradeNum;      // Unknown purpose int
  // Print flags
  int st_last_flag = 0;
  int st_first_flag = 0;
  int grade_flag = 0;
  int class_flag = 0;
  int bus_flag = 0;
  int GPA_flag = 0;
  int tch_last_flag = 0;
  int tch_first_flag = 0;
  // Ctrl flags
  int validCmd = 0;
  int validSubCmd = 0;
  int quitCmd = 0;
  errno = 0;      // Used for zero case check of user input
  char *endPtr;   // Used to check if nameOrNum is int or string
  int found[60]; // Location found = 1
  int sizeRecords;
  float avg;
  char cmdWords[7][8] = {
			 "quit",
			 "student",
			 "teacher",
			 "bus",
			 "grade",
			 "average",
			 "info"
  };
    
  char subCmdWords[4][5] = {
			    "bus",
			    "high",
			    "low",
			    ""
  };

  Student studentRecords[60] = {0};
  parseFile(studentRecords);
  
  while(quitCmd == 0){
      
    // initilize flags
    st_last_flag = 0;
    st_first_flag = 0;
    grade_flag = 0;
    class_flag = 0;
    bus_flag = 0;
    GPA_flag = 0;
    tch_last_flag = 0;
    tch_first_flag = 0;
    validCmd = 0;
    validSubCmd = 0;
    quitCmd = 0;

    memset(found, 0, sizeof(found));
    memset(words, 0, sizeof(words));
    memset(cmd, 0, sizeof(cmd));
    memset(subcmd, 0, sizeof(subcmd));
    memset(lastname, 0, sizeof(lastname));
    memset(nameOrNum, 0, sizeof(nameOrNum));
      
    sizeRecords = sizeof(studentRecords) / sizeof(studentRecords[0]);
        
    // Print tool menu
    printUserPrompt();
        
    fgets(words, 50, stdin);
        
    sscanf(words, "%s %s %s", cmd, nameOrNum, subcmd);
        
    // Convert user input to lower case
    toLower(cmd, strlen(cmd));
    toLower(subcmd, strlen(subcmd));
        
    // Check for valid commands entered
    for(int i = 0; i < (sizeof(cmdWords) / sizeof(cmdWords[0])); i++){
      if(!strcmp(cmd, cmdWords[i])){
	validCmd = 1;
	break;
      }
      else if(strlen(cmd) == 1 && cmd[0] == cmdWords[i][0]){
	validCmd = 1;
	break;
      }
      else{
	validCmd = 0;
      }
    }
        
    // Check for program quit
    if(cmd[0] == cmdWords[0][0] && validCmd == 1){
      quitCmd = 1;
      continue;
    }
        
    // Check for valid sub command entered
    for(int i = 0; i < (sizeof(subCmdWords) / sizeof(subCmdWords[0])); i++){
      if(!strcmp(subcmd, subCmdWords[i])){
	validSubCmd = 1;
	break;
      }
      else if(strlen(subcmd) == 1 && subcmd[0] == subCmdWords[i][0]){
	validSubCmd = 1;
	break;
      }
      else{
	validSubCmd = 0;
      }
    }
        
    // Invalid input sentinal
    if(validCmd == 0 || (validSubCmd == 0 && subcmd != 0)){
      printf("\nYou entered: %sThis is not a valid command. Try again.\n",
	     words);
      continue;
    }
        
    // if nameOrNum is an int save it as long int
    // This will save busOrGradeNum as 0 upon failure
    busOrGradeNum = strtol(nameOrNum, &endPtr, 10);
        
    if (errno != 0) {
      perror("strtol");
      exit(EXIT_FAILURE);
    }
        
    // Type of nameOrNum is String
    if (endPtr == nameOrNum) {
      strcpy(lastname, nameOrNum);
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
	       busOrGradeNum
	       );
        
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
      avg = 0;
      if(busOrGradeNum > 6 || busOrGradeNum < 0){
	printf("Grade %ld is an invalid grade. Try grades 0-6.\n", busOrGradeNum);
	continue;
      }
      findGrade(found, studentRecords, sizeRecords, busOrGradeNum);
      avg = averageGPA(found, studentRecords, sizeRecords);
      if(avg == 0){
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
        
    printReport(found, studentRecords, sizeRecords, 
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
      *st_last_flag = Yes;
      *st_first_flag = Yes;
      *grade_flag = No;
      *class_flag = No;
      *bus_flag = Yes;
      *GPA_flag = No;
      *tch_last_flag = No;
      *tch_first_flag = No;
          
      break;
    }
    *st_last_flag = Yes;
    *st_first_flag = Yes;
    *grade_flag = Yes;
    *class_flag = Yes;
    *bus_flag = 0;
    *GPA_flag = 0;
    *tch_last_flag = Yes;
    *tch_first_flag = Yes;
    break;
  case 't':
    *st_last_flag = Yes;
    *st_first_flag = Yes;
    *grade_flag = Yes;
    *class_flag = No;
    *bus_flag = No;
    *GPA_flag = No;
    *tch_last_flag = No;
    *tch_first_flag = No;   
    break;
  case 'b':
    *st_last_flag = Yes;
    *st_first_flag = Yes;
    *grade_flag = Yes;
    *class_flag = Yes;
    *bus_flag = 0;
    *GPA_flag = 0;
    *tch_last_flag = Yes;
    *tch_first_flag = Yes;
    break;
  case 'g':
    if(subcmd[0] == 'h'){
      *st_last_flag = Yes;
      *st_first_flag = Yes;
      *grade_flag = No;
      *class_flag = No;
      *bus_flag = Yes;
      *GPA_flag = Yes;
      *tch_last_flag = Yes;
      *tch_first_flag = Yes;
      // TODO Search for highest GPA function call
      break;
    }
    else if(subcmd[0] == 'l'){
      *st_last_flag = Yes;
      *st_first_flag = Yes;
      *grade_flag = No;
      *class_flag = No;
      *bus_flag = Yes;
      *GPA_flag = Yes;
      *tch_last_flag = Yes;
      *tch_first_flag = Yes;
      // TODO Search for lowest GPA function call
      break;
    }
    *st_last_flag = Yes;
    *st_first_flag = Yes;
    *grade_flag = No;
    *class_flag = No;
    *bus_flag = 0;
    *GPA_flag = 0;
    *tch_last_flag = No;
    *tch_first_flag = No;
    break;
  case 'a':
    *st_last_flag = No;
    *st_first_flag = No;
    *grade_flag = Yes;
    *class_flag = No;
    *bus_flag = No;
    *GPA_flag = No;
    *tch_last_flag = No;
    *tch_first_flag = No;
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
  
  float lowestGPA = 5.0;
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

  float highestGPA = 0.0;
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
  
  char s[CHUNK];
  int i = 0,  fieldNum = 0, size;
  // int numOfFields = 7;
  // open failure
  if(!fp){
    perror("fopen");
    exit(EXIT_FAILURE);
  }
 
  while(fgets(s, CHUNK, fp)){
    /* if(fieldNum < numOfFields){ */
    /*   fprintf(stderr,"\nFile invalid\n"); */
    /*   exit(EXIT_FAILURE); */
    /* } */
    fieldNum = 0;
    char *data = strtok(s, ",");
    char *endptr;
    while(data){
      if(fieldNum == 0){
	strcpy(studentRecords[i].student_last,  data);
      }
      if(fieldNum == 1){
	strcpy(studentRecords[i].student_first, data);
      }
      if(fieldNum == 2){
	studentRecords[i].grade = strtol(data, &endptr, 10);
      }
      if(fieldNum == 3){
	studentRecords[i].classroom = strtol(data, &endptr, 10);
      }
      if(fieldNum == 4){
	studentRecords[i].bus = strtol(data, &endptr, 10);
      }
      if(fieldNum == 5){
	studentRecords[i].GPA = strtof(data, &endptr);
      }
      if(fieldNum == 6){
	strcpy(studentRecords[i].teacher_last, data);
      }
      if(fieldNum == 7){
	strcpy(studentRecords[i].teacher_first, data);
	size = strlen(studentRecords[i].teacher_first) - 1;
	if(studentRecords[i].teacher_first[size] == '\n'){
	  studentRecords[i].teacher_first[size] = '\0';
	}
      }
      data = strtok(NULL, ",");
      fieldNum++;
    }
    i++;
  }
  fclose(fp);


  // free(s);
  // s = NULL;

  return;
}


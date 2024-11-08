// Prog for creating oddball test files. Such as files with null bits.
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char * argv[])
{
  int uniqnum = 0;
  int fd;
  int testnum = 45;  
  char *c;
     
  c = (char *)malloc(sizeof(int));
    
  *c = (char)uniqnum;
    
  fd = open(argv[1], O_RDWR);
  write(fd, c, 1);
    
  *c = 'a';
  write(fd, c, 1);

  if(testnum < 256){
    c[0] = '\0';
    c[1] = '\0';
    c[2] = '\0';
    c[3] = (char)testnum;
    /* write(fd, "\0\0\0", 3);
     *c = (char)testnum;*/
    write(fd, c, 4);
  }
  /*else if(*/
  
  /*write(fd, (char *)testnum, 1));*/
  return 0;
}

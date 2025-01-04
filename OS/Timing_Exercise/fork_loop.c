#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

/* Remember: In the parent process, fork() returns the process ID 
of the child process. In the child process, fork() returns 0 to 
indicate that it is the child process.*/
/* In the while loop the fork() will return 0 to the child process.
   This makes the while loop condiition false for the child process. 
   The parent will wait for the child process to complete. The child 
   process will print out the value of i. When the child terminates 
   then the print buffer will flush stdout and print to the screen.
   After the child terminates the parents wait function will no longer
   block and it will call printf with "Yo!". The string will be put into
   the buffer and i will increment.
   The process will then loop again but this time both child and parent 
   have the string "Yo!" in the stdout buffer. So this time when the
   child calls printf with i and terminates it will print out "Yo!1". 
   The parent will continue through the loop and its print buffer will now
   hold "Yo!Yo!". The parent/child forking and child terminating process 
   will continue until i = 5. At which point both parent and child will 
   have "Yo!Yo!Yo!Yo!Yo!" in the buffer. Then both parent and child will
   print out "Yo!Yo!Yo!Yo!Yo!5" because they will both fail the while loop.*/
int main(void){
    int i = 0;
    pid_t pid = 0;
    while(fork() && i < 5){
        wait(NULL);
        printf("Yo!");
        // printf("PID: %d, Yo! ",getpid());
        //fflush(NULL);
        i++;
    }
    
    printf("%d",i);
    // printf("PID: %d, %d ",getpid(), i);
    return 0;
}
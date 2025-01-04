#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#define ITER_MAX_ 1000
#define SLEEP 10.0

int main(void){
    char msg_p[] = "parent";
    char msg_c[] = "child";
    char nl[]    = "\n";
    int i = 0;

    for(; i < ITER_MAX_; i++){

        if(fork()){ /* Parent*/
            // printf("parent"); 
            // fflush(stdout);
            sleep(SLEEP);
            write(STDOUT_FILENO, msg_p, sizeof(msg_p) - 1);
            wait(NULL);
        }
        else{ /* Child*/
            // printf("child"); 
            // fflush(stdout);
            sleep(SLEEP);
            write(STDOUT_FILENO, msg_c, sizeof(msg_c) - 1);
            return 0;
        }

        sleep(SLEEP);
        // printf("\n"); 
        // fflush(stdout);
        write(STDOUT_FILENO, nl, sizeof(nl) - 1);

    }
    return 0;
}
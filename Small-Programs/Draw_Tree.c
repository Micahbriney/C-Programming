#include <stdio.h>

int main(){
    const int MAX_ROWS = 7;
    int numRows;
    
    do{
        printf("How many rows of leaves on your tree?\nType 0 (Zero) to end the program\n");
        scanf("%d", &numRows);
        
        while(numRows > MAX_ROWS || numRows < 2){
            if (numRows > MAX_ROWS) //Trees taller than 10 lines
                printf("Your tree is to tall.\nTrees don't grow past 7 rows of leaves.\nTry again.\n");
            else if(numRows < 0)    //Negative leaves is invalid
                printf("Enter the number of leaves not roots.\nTry again.\n");
            else if(numRows == 0){  //Sentinel to end the program
                printf("Ending program...");
                return 0;
            }
            else                    //Tree is to short
                printf("Your tree is a trunk.\nTry again.\n");
            scanf("%d", &numRows);
        }
        
        for(int i = 0; i < numRows; i++){
            for(int j = i + 1; j < numRows; j++) //Print spaces
                printf(" ");
            for(int j = 0; j < (2 * i + 1); j++) //Print leaves
                putchar('*');
            printf("\n");
        }

        for(int i = 0; i < 3; i++){
            for(int j = 0; j < numRows - 1; j++) //Print spaces for trunk
                printf(" ");
            printf("*\n");  //print the trunk
        }
    } while (numRows != 0);
    
    printf("Ending program...\n");
    return 0;
}
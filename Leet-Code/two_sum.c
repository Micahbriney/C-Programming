/**
 * Note: The returned array must be malloced, assume caller calls free().
 */
#define RETSIZE 2

// Method 1 Nested Loops
/*int* twoSum(int* nums, int numsSize, int target, int* returnSize) {
    int second = 0;   // Holds the second addend number to search for 
    int *set;         // Holds the indices of the two numbers
    if(NULL == (set = (int *)malloc(sizeof(int) * RETSIZE))){
        perror("twoSum() ret malloc");
        exit(-1);
    }

    for(int i = 0; i < numsSize; i++){
        second = target - nums[i];
        for(int k = i + 1; k < numsSize; k++){
            if(nums[k] == second){
                set[0] = i;
                set[1] = k;
                *returnSize = 2;
                return set;
            }
        }
    }
    // nums is an array 
    // target is an int
    // find the two numbers in nums that add up to the target
    return set;
}
*/

#include <limits.h>


#define PRIME       71 // Multiplier for hash function
#define NEG_NUM     1  // Negative index for simple index mapping
#define POS_NUM     0  // Positive index for simple index mapping
#define SET_FLAG    1  // Flag used when map item is populated
#define UNSET_FLAG  0  // For initilization
#define UNSET      -1  // For initilization
#define DUPLICATE   3  // Flag for when more than 2 duplicate num are found
#define NO_VAL      0

typedef struct HashElement{
    int setFlag;  // Flag for when index has been used. Used for collision check
    int val;      // num[i] value
    int index[2]; // Holds index for only 2. Duplicate are only an answer when < 3 
} HashElement;

    //Method 2 Hash map the nums array. 
    // if hashmap[i] != 0 check if hashmap[target-1] != 0. If true then done o(n)
int* twoSum(int* nums, int numsSize, int target, int* returnSize) {
    int indexSize = numsSize * 2 + PRIME;   // Arbitrarily large to avoid collisions
    HashElement hashMap[indexSize][2] = {}; // Simple index map. hashMap[1] for negs
    long long hash;                         // Hold hash val
    long long second = 0;                   // Hold hash val for desired 2nd num
    int *ret;                               // Store results to be returned

    for(int i = 0; i < indexSize; i++){ // Init struct array's index array
        hashMap[i][POS_NUM].setFlag = UNSET_FLAG;
        hashMap[i][POS_NUM].val = NO_VAL;
        hashMap[i][POS_NUM].index[0] = UNSET;
        hashMap[i][POS_NUM].index[1] = UNSET;
        hashMap[i][NEG_NUM].setFlag = UNSET_FLAG;
        hashMap[i][NEG_NUM].val = NO_VAL;
        hashMap[i][NEG_NUM].index[0] = UNSET;
        hashMap[i][NEG_NUM].index[1] = UNSET;
    }

    // Allocate space for result pointer.
    if(NULL == (ret = (int *)calloc(2, sizeof(int)))){
        perror("twoSum() hashMap calloc()");
        exit(-1);
    }

    for(int i = 0, pos; i < numsSize; i++){ // Map the input
        hash = (long long)nums[i] * PRIME % indexSize; // Calculate Hash
        // printf("hash val: %d\n", hash);
        // printf("Curr nums[i]: %d\n", nums[i]);
        // printf("i value to be stored in index[0 or 1]: %d\n", i);

        if(nums[i] > 0){
            // printf("Found a positive number\n");
            pos = POS_NUM;
        }
        else{
            hash *= -1;  // Convert the hash index to a positive
            pos = NEG_NUM;
        }
        // printf("Prev value found at hashMap[hash][1].index[0]: %d\n", hashMap[hash][0].index[0]);
        // printf("Prev value found at hashMap[hash][1].index[1]: %d\n", hashMap[hash][0].index[1]);
        // printf("Prev value found at hashMap[hash][1].index[0]: %d\n", hashMap[hash][1].index[0]);
        // printf("Prev value found at hashMap[hash][1].index[1]: %d\n", hashMap[hash][1].index[1]);

        if(hashMap[hash][pos].val != 0 && 
            hashMap[hash][pos].val != nums[i])
            printf("Collision!! While attempting to map a num"); 
            // TODO
        else if(hashMap[hash][pos].index[1] != UNSET ){ // More than  2 dups
            // printf("num at hashMap[hash][pos].index[1]: %d\n", hashMap[hash][pos].index[1]);
            // printf("More than 2 dupes found.");
            hashMap[hash][pos].setFlag = DUPLICATE;
            // hashMap[hash][pos].index[0] = UNSET;
            continue;
        }
        else{
            if(hashMap[hash][pos].index[0] == UNSET){
                // printf("index is UNSET. Setting it now.\n");
                hashMap[hash][pos].setFlag = SET_FLAG;
                hashMap[hash][pos].val = nums[i];
                hashMap[hash][pos].index[0] = i;
            }
            else
                hashMap[hash][pos].index[1] = i;
        }

        // printf("New value at hashMap[hash][1].index[0]: %d\n", hashMap[hash][0].index[0]);
        // printf("New value at hashMap[hash][1].index[1]: %d\n", hashMap[hash][0].index[1]);
        // printf("New value at hashMap[hash][1].index[0]: %d\n", hashMap[hash][1].index[0]);
        // printf("New value at hashMap[hash][1].index[1]: %d\n", hashMap[hash][1].index[1]);
    }

    for(int i = 0, pos; i < numsSize; i++){
        second = llabs(target - nums[i]) * PRIME % indexSize;
        // printf("First num: %d\n", nums[i]);
        // printf("Looking for second val num: %d\n", target - nums[i]);
        // printf("Looking for second val hash: %d\n", second);
        
        if((target - nums[i]) > 0)
            pos = POS_NUM;
        else
            pos = NEG_NUM;

        // Check if map entry populated with the expected value
        if(hashMap[second][pos].setFlag == SET_FLAG && 
           target - nums[i] == hashMap[second][pos].val){
            // Case where 2nd num needed is a duplicate but doesn't exist.
            if((target - nums[i]) == nums[i] && 
                hashMap[second][pos].index[1] == UNSET){
                // printf("Duplicate as the second num NOT found!");
                continue;
            }
            // Case where 2nd num needed is a duplicate and the number does exist.
            else if((target - nums[i]) == nums[i] && 
                    i != hashMap[second][pos].index[1]){
                ret[0] = i;
                ret[1] = hashMap[second][pos].index[1];
                // printf("Found the duplicate 2nd.\n");
                *returnSize = 2;
                break;
            }

            // printf("Found it!\n");
            // printf("i value: %d\n", i);
            ret[0] = i;
            ret[1] = hashMap[second][pos].index[0];
            *returnSize = 2;
            break;
        }
        else // collision So check at next populated map location
            printf("FIX ME. Collision when looking for num\n");
    }

    return ret;
}
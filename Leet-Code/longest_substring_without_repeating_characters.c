#define NEXT_LETTER_LEN  1
#define FOUND            1

int lengthOfLongestSubstring(char* s) {
    int alpha[256] = {0}; // Array of found letters
    int currLen = 0;      // Current length of substring
    int longest = 0;      // Length of longest found substring

    if(*s == '\0')            // Case: Empty string was passed
        return longest;
    else if(*(s + 1) == '\0') // Case where only one letter is sent
        return 1;

    while(*s != '\0'){
        if(alpha[*s] != 0){ // Case: Letter already occured in substring
            if(currLen > longest)                // Store longest string found
                longest = currLen;
            s = s - currLen + NEXT_LETTER_LEN;   // reset string to next letter
            currLen = 0;                         // reset counter
            memset(alpha, 0, sizeof(int) * 256); // Clear alpha array
        }
        else{ // Case: Unique letter in substring
            alpha[*s] = FOUND;                   // Mark substring letter
            s++;                                 // Increment s pointer
            currLen++;                           // Increment substring length counter
        }
    }

    if(currLen > longest) // Case : Last substring before null byte was longest
        return currLen;

    return longest;
}
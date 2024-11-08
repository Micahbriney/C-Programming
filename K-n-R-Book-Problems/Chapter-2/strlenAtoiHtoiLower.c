
#include <stdio.h>

int strLen(char s[]);
int atoI(char s[]);
int lower(int c);
int htoi(char s[]);
  
int main(int argc, char *argv[]){
  char arr[] = "0x1f4zc";
  int len;
  unsigned int hextoint;

  len = strLen(arr);

  hextoint = htoi(arr);

  printf("Hex String: %s. Int value: %d", arr, hextoint);
  printf("\n\"%s\" has a length of %d \n", arr, len);

}

/* strLen will find the length of the string parameter.
   This function has no error checking
   */
int strLen(char s[]){
  int i;

  i = 0;
  while(s[i] != '\0')
    ++i;
  return i;
}

/* atoI will change the contents of a char array to an integer.
   This has no error checking. It appears that any integer chars
   in the char array will only return the numbers at the beginning
   of the array.
   ie. "745 are numbers in 12745" will return 745.
   n = 10 * n (s[i] - '0') explained:
   (s[i] - '0') -- will return the integer value of the char found at s[i].
   10 * n       -- will move the previous integer to the tens place.
   Next add the above two. 
   This stores s[i] in the ones places and n in the tens place.
   store it in n and countinue the loop.
*/
int atoI(char s[]){
  int i, n;

  n = 0;
  for(i = 0; i >= '0' && i <= '9'; i++)
    n = 10 * n + (s[i] - '0');

  return n;
}

/* lower: convert c to lower case; ASCII only
   Function takes an integer parameter and returns the lower case
   if it is an upper case aplha char
   return c + 'a' - 'A' works as follows
   if c = 'D' = 68 and 'a' = 97 and 'A' = 65 and 'd' = 100
   'D' + 'a' = 68 + 97 = 165 (extended ascii char)
   165 - 65 = 100 = 'd'
*/
int lower(int c){
  if(c >= 'A' && c <= 'Z') /* Found upper case alpha char*/
    return c + 'a' - 'A';  /* Change the uppercase alpha to lower*/
  else                     /* No uppercase alpha found*/
    return c;              /* No change return*/
}

/* Function string of hexadecimal digits to equivalent integer value
   Allowable digits are 0 - 9, a - f AND A - F
   0-9
   a = 10
   b, c, d, e, f = 15
   0xff == 255 = 2^8
*/

int htoi(char s[]){
  int i;
  unsigned int ret;
  ret = 0;
  if(s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
    for(i = 2; s[i] != '\0'; i++){
      if(s[i] >= 'a' && s[i] <= 'f') /* Lower alpha*/
	ret = ret * 16 + 10 + s[i] - 'a';
      else if(s[i] >= 'A' && s[i] <= 'F') /* Upper alpha*/
	ret = ret * 16 + 10 + s[i] - 'A';
      else if(s[i] >= '0' && s[i] <= '9') /* Numeric*/
	ret = ret * 16 + s[i] - '0';
      else
	return 0;
    }
  return ret;
}

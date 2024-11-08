/* Program: dataTypeRanges.c
   Description: Print the ranges of all data types. Unfinished.

printf specifier format flags used:
    %d = signed decimal integer
    %u = unsigned decimal integer
    %f = decimal floating point
printf length format flags used:
    %h = short integer type
    %l = long integer type
    %ll = long long integer type (standard C, but not supported in Windows)
    %I64 = long long integer type (Windows only method)
    %L = long floating type (standard C, but not supported in Windows)
    Windows does not support long doubles types and stores them as doubles in memory. Source: https://docs.microsoft.com/en-us/cpp/c-language/type-long-double?view=msvc-160
*/

#include <limits.h>
#include <float.h>

int main(int argc, char *argv[]){
  /* ----- Integers ----- */
  int intrangemin, intrangemax;
  unsigned int uintrangemin, uintrangemax;
  short int shortrangemin, shortrangemax;
  unsigned short int ushortrangemin, ushortrangemax;
  long int longrangemin, longrangemax;
  unsigned long int ulongrangemin, ulongrangemax;
  
  /* ----- Chars ----- */
  char charrangemin, charrangemax;
  unsigned char ucharrangemin, ucharrangemax;

  /* ----- Floating point ----- */
  float floatrangemin, floatrangemax;
  unsigned float ufloatrangemin, ufloatrangemax;
  double doublerangemin, doublerangemax;
  unsigned double udoublerangemin, udoublerangemax;
  long double ldoublerangemin, ldoublerangemax;
  unsigned long double uldoublerangemin, uldoublerangemax;

 
  
  /* ----- Integers ----- */
  intrangemin = intrangemax = ~0;
  intrangemin >>= 1;
  uintrangemin = uintrangemax = ~0;
  shortrangemin = shortrangemax = ~0;
  ushortrangemin = ushortrangemax = ~0;
  longrangemin = longrangemax = ~0;
  ulongrangemin = ulongrangemax = ~0;

  printf("Int range: %d - $d", INT_MIN, INT_MAX);
  printf("Int range compute: %d - $d", intrangemin, intrangemax);

  printf("Unsigned Int range: %d - $d", 0, UINT_MAX);
  printf("Unsigned Int range compute: %d - $d", 0, UINT_MAX);

  printf("Int range: %d - $d", SHRT_MIN, SHRT_MAX);
  printf("Int range: %d - $d compute", SHRT_MIN, SHRT_MAX);

  printf("Int range: %d - $d", 0, USHRT_MAX);
  printf("Int range: %d - $d compute", 0, USHRT_MAX);

  printf("Int range: %d - $d", INT_MIN, INT_MAX);
  printf("Int range: %d - $d compute", INT_MIN, INT_MAX);
  
  /* ----- Chars ----- */
  charrangemin = charrangemax = ~0;
  ucharrangemin = ucharrangemax = ~0;

  /* ----- Floating point ----- */
  floatrangemin = floatrangemax = ~0;
  ufloatrangemin = ufloatrangemax = ~0;
  doublerangemin = doublerangemax = ~0;
  udoublerangemin = udoublerangemax = ~0;
  ldoublerangemin = ldoublerangemax = ~0;
  uldoublerangemin = uldoublerangemax = ~0;
  
 

  /* ----- Chars ----- */
  printf();
  
  /* ----- Floating point ----- */

  
}



#include <stdio.h>

int power(int m, int n);

int main(int argc, char *argv[]){
  int i;

  for(i = 0; i < 10; i++)
    printf("Exponent:%2d| Base 2: %3d| Base -3: %6d\n",
	    i, power(2, i), power(-3, i));
  return 0;  
}

/* power: raise base to the n-th power; n >= 0 */
int power(int base, int n){
  int i, p; /* p is the product*/

  p = 1; /* Init to 1*/
  for (i = 1; i <= n; ++i)
    p = p * base;
  return p;
}

/* This will print a table of
   and celciys and farenheight in 
   two columns
   Book Example pg 12 
   Exercise 1-3, 1-4, 1-5*/

#include <stdio.h>

void fahrenheitToCelsius();
void celsiusToFahrenheit();
void backwards();

int main(int argc, char *argv[]){

  fahrenheitToCelsius();
  celsiusToFahrenheit();
  backwards();
  
}

void fahrenheitToCelsius(){
  float fhr, cls;

  /* Increment by multiples of 20*/
  printf("Temperature conversion program.\n" 
         "Fahrenheit to Celsius table\n");
  for(int i = 0; i <= 300; i += 20){
    fhr = i;
    cls = 5.0 * (fhr - 32.0)  / 9.0;
    printf("%3.0f %6.1f\n", fhr, cls);
  }
}

void celsiusToFahrenheit(){
  float fhr, cls;
  
  /* Increment by multiples of 20*/
  printf("Temperature conversion program.\n" 
         "Celsius to Fahrenheit table\n");
  for(int i = 0; i <= 300; i += 20){
    cls = i;
    fhr = 9.0 * cls / 5.0 - 32.0;
    printf("%3.0f %6.1f\n", cls, fhr);
  }
}

void backwards(){
  float fhr, cls;
  
  /* Increment by multiples of 20*/
  printf("Temperature conversion program.\n" 
         "Celsius to Fahrenheit table High to low\n");
  for(int i = 300; i >= 0; i -= 20){
    cls = i;
    fhr = 9.0 * cls / 5.0 - 32.0;
    printf("%3.0f %6.1f\n", cls, fhr);
  }
}


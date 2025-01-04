char *intToRoman(int num){
 int i = 0, k = 0, length = 0;
 char *result;
 char *roman[13] = {
  "M", "CM", "D", "CD", 
  "C", "XC", "L", "XL", 
  "X", "IX", "V", "IV", "I"
 };
 int values[13] = {1000, 900, 500, 400, 
       100, 90, 50, 40, 
       10, 9, 5, 4, 1};
 printf("Test before calloc\n");
 if(NULL == (result = calloc(16, sizeof(char)))){
  perror("result calloc()\n");
  exit(1);
 }
	
 while(num > 0){
  
  if(num >= values[i]){
   
   printf("Value of I = %d\n", i);
   printf("Value of values[i] = %d\n", values[i]);
   printf("Value of K = %d\n", k);
   printf("Value of num = %d\n", num);
   printf("num > values[i]\n");
   strcat(result, roman[i]);
   k++;
   //result[k++] = roman[i][0];
   printf("Roman at i is %s\n", roman[i]);
   if(i % 2 != 0) // evens are 2 chars
    k++;
   printf("Result so far is %s\n", result);
   num -= values[i];
   printf("Value of new num = %d\n", num);
  }
  else
   i++;
 }
	
 printf("result = %s\n", result);
 result[k] = NULL;
 return result;
}

int main(int argc, char *argv[]){
 int num = 1;
 int num1 = 4;
 int num2 = 5;
 int num3 = 9;
 int num4 = 10;
 int num5 = 40;
 int num6 = 50;
 int num7 = 90;
 int num8 = 100;
 int num9 = 400;
 int num10 = 500;
 int num11 = 900;
 int num12 = 1000;
 int num13 = 3888;
 int num14 = 3999;
	
 printf("int %d= roman %s\n\n",num,intToRoman(num));
 printf("int %d= roman %s\n\n",num1,intToRoman(num1));
 printf("int %d= roman %s\n\n",num2,intToRoman(num2));
 printf("int %d= roman %s\n\n",num3,intToRoman(num3));
 printf("int %d= roman %s\n\n",num4,intToRoman(num4));
 printf("int %d= roman %s\n\n",num5,intToRoman(num5));
 printf("int %d= roman %s\n\n",num6,intToRoman(num6));
 printf("int %d= roman %s\n\n",num7,intToRoman(num7));
 printf("int %d= roman %s\n\n",num8,intToRoman(num8));
 printf("int %d= roman %s\n\n",num9,intToRoman(num9));
 printf("int %d= roman %s\n\n",num10,intToRoman(num10));
 printf("int %d= roman %s\n\n",num11,intToRoman(num11));
 printf("int %d= roman %s\n\n",num12,intToRoman(num12));
 printf("int %d= roman %s\n\n",num13,intToRoman(num13));
 printf("int %d= roman %s\n\n",num14,intToRoman(num14));
	
 return 0;
}

/* Mean function that takes an array of integers A and its length len */
/* The mean function returns the average of all elements of A as a double. */
double mean (int *A, int len){
  int i;
  double res = 0.0;
  for(i = 0; i < len; i++){
    res += A[i]; 
  }
  return res / len;
}

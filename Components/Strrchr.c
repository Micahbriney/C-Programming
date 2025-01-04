/* The strrchr() function returns a pointer to the last occurrence of the */
/* character c in the string s. */
/* The terminating null byte is considered part of the string, so that if c */
/* is specified as ’\0’, this function returns a pointer to the terminator. */
/* Statement appears to assume no nested Null bytes */
char *strrchr(const char *s, int c){
  const char *pt;
  pt = s;
    
  /* Case 1: char* s points to 0x0  */
  if(!pt){
    return '\0';
  }
    
  /* Case 2: s contains null and c is NULL */
  if(*pt == '\0' && !c){
    perror("char *p points to NULL");
    return (char *)pt;
  }/* Case 3: int c is passed as a NULL. Find terminator of char *s */
  else if (c == '\0'){
    while(*pt){         /* Go to NULL terminator */
      pt++;
    }
    return (char *)pt;
  }
    
  /* Case 4: Search char *s for c*/
  while(*pt){
    if(*pt == c){
      return (char *)pt; /* char c found in char *s*/
    }
    pt++;
  }

  /* Case 5: No match found */
  return '\0';
  /* Case: Embedded Null byte. NOT A CASE SEE ABOVE*/ 
}

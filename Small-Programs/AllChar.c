#include <stddef.h>
#include <stdio.h>
#define BUILD_BUG_ON_ZERO(e) (sizeof(struct { int:(-!!(e)); }))
#define __same_type(a, b) __builtin_types_compatible_p(typeof(a), typeof(b))
#define __must_be_array(a) BUILD_BUG_ON_ZERO(__same_type((a), &(a)[0]))
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]) + __must_be_array(arr))

void MakeFile(void){
  unsigned char ascii[255] = {0};
  for(int i=1; i < ARRAY_SIZE(ascii); ++i)
    {
      ascii[i] = ascii[i - 1] + 1;
    }

  FILE* fp = fopen("all_ascii.txt", "w");
  if (fp == NULL)
    {
      return;
    }
  fwrite(ascii, ARRAY_SIZE(ascii), 1, fp);
  fclose(fp);
}

int main(){
  MakeFile();
}


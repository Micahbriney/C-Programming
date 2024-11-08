/**
 * @author: Micah Briney
 * @date:   10/28/2022
 * @brief:  It is a pwd program only not so good and filled with invalid
 *          read of size 1 issues. It seems to work well enough. I wasn't
 *          able to produce a failed pwd result. Still need to iron out
 *          the issues noticed by valgrind. Probably issues with strlen and
 *          strncpy.
**/

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define SELF_ "."
#define PARENT_ ".."
#define NEWPARENT_ SELF_
#define FN_MAX_ 256

dev_t get_device_info(char *location);
ino_t get_inode_info(char *location);
char *search_directory(DIR *dir, ino_t selfino, dev_t selfdev);
size_t set_and_get_path_size(size_t pathsize);
void print_path(char *path[], int index);
void print_error(char *err, DIR *dir);

/**
 * @fn: main
 * @brief: Entry into the mypwd program
 * @param: int, char *
 * @return: int
**/
int main(int argc, char *argv[]){
  dev_t selfdev, parentdev; /* Store self, parent device number*/
  ino_t selfino, parentino; /* Store self, parent inode*/
  char *path[FN_MAX_];      /* Store path in reverse order*/
  int dirposition;          /* Store the number of dir changes*/
  DIR *dir;                 /* Hold the directory*/
  size_t size;              /* Hold size of Path*/
  int i;

  /* Check for errors in program execution*/
  if(argc > 1){ /* Unknown opts*/
    fprintf(stderr, "default Usage: %s\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  /* Zero out array*/
  for(i = 0; i < FN_MAX_; i++)
    path[i] = "\0";
  
  dirposition = 0;
  while(1){ /* Check forever. What could go wrong...*/

    /* Get alll of the link info*/
    selfdev   = get_device_info(SELF_);
    parentdev = get_device_info(PARENT_);
    selfino   = get_inode_info(SELF_);
    parentino = get_inode_info(PARENT_);

    /* Check if at root*/
    if(selfino == parentino &&
       selfdev == parentdev){ /* We made it to root!*/
      size = set_and_get_path_size(0);
      if(dirposition < FN_MAX_ &&
	 size < PATH_MAX + 1){
	print_path(path, dirposition);
      }
      else{ /* Path is too long*/
	fprintf(stderr, "Path too long!\n");
	if(dir)
	  closedir(dir);
	exit(EXIT_FAILURE);
      }
      break; /* Break out of while loop*/
    }
    else{ /* Not at root*/
      chdir(PARENT_); /* Change to parent directory*/

      if(!(dir = opendir(NEWPARENT_))) /* Open directory "."*/
	print_error("Opendir " NEWPARENT_, dir); /* It's dead, Jim.*/

      /* Build the path up*/
      path[dirposition++] = search_directory(dir, selfino, selfdev);
    }
  }
  return 0; 
}

/**
 * @fn: get_device_info
 * @brief: lstat the provided location and return the device number;
 * @param: char *
 * @return: dev_t
**/
dev_t get_device_info(char *location){
  struct stat sbdev;

  if(lstat(location, &sbdev))
    print_error(location, NULL);

  return sbdev.st_dev;
}

/**
 * @fn: get_inode_info
 * @brief: lstat the provided location and return the inode number;
 * @param: char *
 * @return: ino_t
**/
ino_t get_inode_info(char *location){
  struct stat sbino;

  if(lstat(location, &sbino))
    print_error(location, NULL);

  return sbino.st_ino;
}

/**
 * @fn: search_directory
 * @brief: This function will read the provided directory. It will then
 *         look to match the inode and device number parameters by 
 *         calling functions to lstat the d_name provided by the dirent.
 *         If no match is found the it will read the directory again
 *         repopulating the dirent with new directory data. This will
 *         continue till a match is found or NULL is returned.
 * @param: DIR, ino_t, dev_t
 * @return: char *
**/
char *search_directory(DIR *dir, ino_t selfino, dev_t selfdev){
  ino_t currino;
  dev_t currdev;
  struct dirent *ent;
  size_t size;
  char *path;

  if(errno)/* Some error found?. Effectivly sets errno to 0*/
    print_error("Something broke", dir); 
  while((ent = readdir(dir))){ /* Read */
    
    if(!(selfino == (currino = get_inode_info(ent->d_name))))
      continue; /* If inodes don't match skip getting device info*/

    currdev = get_device_info(ent->d_name);
    
    if(selfdev == currdev &&
       selfino == currino)  /* Found the route back*/
      break; /* Leave loop*/
  }

  if(!(ent) && errno)/* Error reading dir occured.*/
    print_error("Read Dir", dir);
  else if(!ent){ /* End of directory stream reached and no match found. */
    fprintf(stderr, "Might have jumped across file systems?");
    exit(EXIT_FAILURE);
  }

  if(closedir(dir)) /* Closing '.' directory failed*/
    print_error("Closing " NEWPARENT_ " directory fail.", NULL);

  size = 0;
  if(ent->d_name)
    size = strlen(ent->d_name);

  (void)set_and_get_path_size(size);

  if(!(path = (char *)malloc(sizeof(char) * size + 2))) /* Get space*/
    print_error("Malloc path", dir); /* Ruh-roh, Raggy*/
  path[0] = '/';  /* Add the directory separator*/
  if(size > 0)
    strncpy(++path, ent->d_name, size); /* Move past '/'. +1 for "/0"*/
  path[size] = '\0'; /* Terminate path string*/
  return --path; /* Move the path back to position 0*/
}

/**
 * @fn: set_and_get_path_size
 * @brief: Adjust and retrive the size of the total path.
 * @param: size_t
 * @return: size_t
**/
size_t set_and_get_path_size(size_t pathsize){ /* Set and get size of path*/
  static size_t size = 0;
  return size += pathsize;
}

/**
 * @fn: print_path
 * @brief: Print out the path of mypwd.
 * @param: char *, int
 * @return: void
**/
void print_path(char *path[], int index){ /* Print out pwd*/
  int i;

  for(i = (index - 1); i >= 0; i--){
    printf("%s", path[i]);
    free(path[i]);
  }
  printf("\n");
  
  return;
}

/**
 * @fn: print_error
 * @brief: This function handles printing out errors related to the checks
 *         performed above. It will also close the directory if one is
 *         provided
 * @param: char *, int
 * @return: void
**/
void print_error(char *err, DIR *dir){
  perror(err);
  if(dir)
    closedir(dir);
  exit(EXIT_FAILURE);
}

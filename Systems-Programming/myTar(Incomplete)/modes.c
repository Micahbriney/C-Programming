#include "options.h"
#include "modes.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <grp.h>
#include <pwd.h>

extern int errno;


/**
 *@fn: create_mode
 *@brief: This is one of the three action flag functions. This function 
           kicks off the creation of a tar archive. First a new file is 
	   created or an old one is truncated. It is give RW perms for
	   users, groups and others. Next if the check for file type.
	   This app only supports three mode types: Regular files, Symlinks
	   and directories. In verbose mode each entry will be listed as it
	   is added. Returns 0 on success and -1 on failure.
 *@params: char *tarfile, char *files[], int flags
 *@return: int
**/
int create_mode(char *tarfile, char *files[], int flags){
  int fd, i;
  struct stat sb;

  /* Open tarfile & truncate or create it with read & write permissions*/
  if(-1 == (fd = open(tarfile, O_RDWR | O_CREAT | O_TRUNC, ALLRWPERMS_))){
    perror("open/create tarfile");
    exit(EXIT_FAILURE);
  }

  i = 0;
  while(files[i]){
    if(-1 == lstat(files[i], &sb)){ /* Get the type for each dir/file/link*/
      perror("lstat file"); /* Print errno*/
      i++;
      errors(); /* Note error and keep going*/
      continue;
    }
    if(S_IFDIR == (sb.st_mode & S_IFMT)) /* Add the directory type*/
      create_directory(files[i++], flags);
    else if(S_IFREG == (sb.st_mode & S_IFMT)) /* Add the regular file type*/
      create_file(files[i++], flags);
    else if(S_IFLNK == (sb.st_mode & S_IFMT)) /* Add the symlink type*/
      create_link(files[i++], flags);
    else{
      fprintf(stderr, "File type not supported");
      errors();
    }
    
  }

  close(fd);

  if(errors())
    return -1;

  return 0;
}


/**
 *@fn: table_contents_mode
 *@brief: This is one of the three action flag functions. This function 
           will list the given archive file one line at a time. If a
	   filename is given on the command line then its total path and
	   all of its decendants will be listed. In verbose mode the extra
	   fileds will be displayed. These include Permissions, 
	   Owerners/groups, mtime and name.  Returns 0 on success and -1 
	   on failure.
 *@params: char *tarfile, char *files[], int flags
 *@return: int
**/
int table_contents_mode(char *tarfile, char *files[], int flags){
  return 0;
}


/**
 *@fn: extract_mode
 *@brief: This is one of the three action flag functions. This function 
           will list the given archive file one line at a time. If a
	   filename is given on the command line then its total path and
	   all of its decendants will be listed. In verbose mode the extra
	   fileds will be displayed. These include Permissions, 
	   Owerners/groups, mtime and name.  Returns 0 on success and -1 
	   on failure.
 *@params: char *tarfile, char *files[], int flags
 *@return: int
**/
int extract_mode(char *tarfile, char *files[], int flags){
  return 0;
}


/**
 *@fn: create_directory
 *@brief: This function will look through the current directory for sub 
 *         directories, files, and links to add to the tar file. This will
 *         be recirsive when sub directories are found otherwise it will
 *         just call create_file or create_link.
 *@params: char* filename, int flags
 *@return: nothing right now.
**/
void create_directory(char *dirname, int flags){
  struct ustarheader header;
  char paddedblock[BLOCKLEN_];
  DIR *dirp;
  struct dirent *ent;
  struct stat sb;

  /* Create a cleaned out header*/
  memset(&header, 0, sizeof(struct ustarheader));
  
  /* Create directory header block*/
  if(create_block(dirname, &header))
    return; /* Header block failed. The rest will probably fail?*/
  
  /* Pad the new directory block here*/
  /* Pad the header block here*/
  if(pad_block(paddedblock, &header)){
    fprintf(stderr, "Padding file %s's header block failed.", dirname);
    return;/* Padding failed*/
  }
  
  /* Verbose check here?*/
  /* Write the header block to the archive file here*/
  
  /* Get a DIR pointer to read the directory*/
  if(NULL == (dirp = opendir(dirname))){/* If found error keep going?*/
    perror("modes.c opendir");/* Somthing wrong, pathname, perms?*/
    errors(); /* Increment error counter*/
    return; /* Stop with this directory?*/
  }
  
  errno = 0;/* Reset errno for readdir error checking*/
  while(NULL != (ent = readdir(dirp))){/* Add all contents of directory*/
    if(!strcmp(PARENT_, ent->d_name))
      continue; /* Don't need to store the parent*/
    else if(!strcmp(SELF_, ent->d_name))
      continue; /* Don't need to store self link*/
    else{/* Figure out filetype*/
      if(-1 == (lstat(ent->d_name, &sb))){ /* lstat failed*/
	perror("lstat ent->d_name"); 
	continue; /* Check the remaining stuff*/
      }

      /* Ignore failed sub dir/file/lnks & keep looking for stuff*/
      switch(sb.st_mode & S_IFMT){/* Add sub directories/files/links*/
      case S_IFDIR: create_directory(ent->d_name, flags); break;
      case S_IFLNK: create_file(ent->d_name, flags); break;
      case S_IFREG: create_link(ent->d_name, flags); break;
      default: perror("Incompatable/unknown file type?\n");	
      }
      
    }
    /* For Testing*/
    /*printf("File\\Dir\\link Ent->d_name: %s\n", ent->d_name);*/
  }

  
  if(errno != 0)/* Error. Don't exit*/
    perror("modes.c readdir");/* Bad dirp?*/

  return;
}

/**
 *@fn: create_file
 *@brief: This helper function will help create file specific blocks
           and write them to the archive file. These include both the
	   header block any necessary data blocks.
 *@params: char* filename, int flags
 *@return: nothing right now.
**/
void create_file(char *filename, int flags){
  struct ustarheader header;
  char paddedblock[BLOCKLEN_];
  /* Create a cleaned out header*/
  memset(&header, 0, sizeof(struct ustarheader));
  
  /* Create directory header block*/
  if(create_block(filename, &header))
    return; /* Header block failed. The rest will probably fail?*/
  

  /* Pad the header block here*/
  if(pad_block(paddedblock, &header)){
    fprintf(stderr, "Padding file %s's header block failed.", filename);
    return;/* Padding failed*/
  }
  /* Verbose check here?*/

  /* Write it to the file*/

  /* Create data blocks*/
  /* Check if it needs padding*/
  /* Pad data blocks*/
  if(pad_block(paddedblock, &header)){
    fprintf(stderr, "Padding the file %s's data block failed.", filename);
    return;/* Padding failed*/
  }
  /* Write data blocks*/
  /* Loop until no more data*/

  
  return;
}

/**
 *@fn: create_link
 *@brief:  This helper function will help create link specific blocks and
            write them to the archive file.
 *@params: char* linkname, int flags
 *@return: nothing right now.
**/
void create_link(char *linkname, int flags){
  struct ustarheader header;
  char paddedblock[BLOCKLEN_];
  /* Create a cleaned out header*/
  memset(&header, 0, sizeof(struct ustarheader));
  
  /* Create symlink header block*/
  if(create_block(linkname, &header))
    return; /* Header block failed.*/
  
  /* Pad the header block here*/
  if(pad_block(paddedblock, &header)){
    fprintf(stderr, "Padding symlink %s block failed.", linkname);
    return;/* Padding failed*/
  }
  /* Verbose check here?*/

  /* Write it to the file*/
  
  return;
}

/**
 *@fn: create_block
 *@brief: This function will populated the ustarheader struct fields.
           The fullname path passed in is checked for length and split
	   along the first '/' found starting at 100 chars from the end
	   of the path. Latter 100 or so chars will be split into the name
	   while the first 155 or less chars will be put into the prefix 
	   field. There are cases where the fullname is greater than 100 
	   and there are no '/' within that portion. This block creation
	   will return -1 (failure). Other failures include fullname is too
	   long, readlink failed or truncated the fullname, filetype is
	   unknown/unsupported, getgrgid or getpwuid, 
 *@params: char *fullname, struct ustarheader *header
 *@return: int
**/
int create_block(char *fullname, struct ustarheader *header){
  /*struct ustarheader header;*/
  int namelen;
  struct stat sb;
  struct passwd *pwd;
  struct group *grd;
  /*char fullname[MAXFULLNAMELEN_+1];*/

  if(-1 == (lstat(fullname, &sb))){ /* lstat failed*/
    perror("lstat fullname"); /* This shouldn't fail*/
  }
  
  /* Create a cleaned out header*/
  /*  memset(&header, 0, sizeof(struct ustarheader));*/
  
  /* Start with harder to populate stuff*/
  /* Name first*/
  /* Can it fit in ustartheader name?*/
  namelen = strlen(fullname);
  if(namelen > MAXFULLNAMELEN_){/* Name is too long. Keep going?*/
    errors(); /* Note the error*/
    return - 1;
  }
  else if(namelen >= MAXNAMELEN_){/* Split the name if true.*/
    /* Split the name and store in Name and prefix fields*/
    if(!(split_name(header, fullname, namelen)))/* Something went wrong*/
      return -1;
  }
  else{/* namelen < MAXNAMELEN_ and can fit in the name field*/
    strncpy(header->name, fullname, namelen);
    header->name[namelen] = '\0'; /* Terminate the name*/
    header->prefix[0] = '\0'; /* Terminate prefix*/
  }

  /* Add directory/file/link type header info*/
  switch(sb.st_mode & S_IFMT){
  case S_IFDIR:
    header->size = 0; /* No size*/
    header->typeflag = DIRTYPE_;
    header->linkname[0] = '\0'; /*Redundant. Should be NULL from memset*/
    break;
  case S_IFLNK:
    header->size = 0; /* No size*/
    header->typeflag = SYMLINKTYPE_;
    /* Get info for the header linkname*/
    if( -1 == (namelen = readlink(fullname,
				  header->linkname,
				  MAXLINKNAMELEN_))){
      perror("Header.linkname"); /* Something wrong with symlink*/
      errors();
      return - 1;
    }
    else if(namelen == MAXLINKNAMELEN_ &&
	    !strcmp(header->linkname, fullname)){ /* Truncated fullname*/
      fprintf(stderr, "The symlink \"%s\" won't fit\n", fullname);
      errors(); /* Note an error occured. Keep going?*/
      return - 1;
    }
    else if(namelen < MAXLINKNAMELEN_)
      header->linkname[namelen] = '\0'; /* Add the NULL terminator*/
    break;
  case S_IFREG:
    header->size = sb.st_size;
    header->typeflag = REGFILETYPE_;
    header->linkname[0] = '\0'; /*Redundant. Should be NULL from memset*/
    break;
  default:
    fprintf(stderr, "Unsupported file type or unknown type.\n");
    errors(); /* Note an error occured. Keep going?*/
    return - 1;
    break;
  }

  /*Get the pw_name and gr_name*/
  errno = 0;
  if((NULL == (pwd = getpwuid(sb.st_uid))) ||
     (NULL == (grd = getgrgid(sb.st_gid)))){
    if(errno)
      perror("getgrgid or getpwuid.");
    else
      fprintf(stderr, "No match found?\n");
    errors(); /* Note any errors and keep goin?*/
    return - 1;
  }
  /* Check lengths of pw_name  and gr_name */
  if(strlen(pwd->pw_name) < UNAMEMAX_)
    strncpy(header->uname, pwd->pw_name, UNAMEMAX_);
  if(strlen(grd->gr_name) < GNAMEMAX_)
    strncpy(header->gname, grd->gr_name, GNAMEMAX_);
  
  /* Add the mode, uid, gid, mtime, */
  header->mode     = (ALLMODES_ & sb.st_mode);
  header->uid      = sb.st_uid;
  header->gid      = sb.st_gid;
  header->mtime    = sb.st_mtime;

  strncpy(header->magic, HEADERMAGIC_, MAGICMAX_); /* Copy "ustar\0"*/
  header->version[0] = '\0'; /* Version "00"*/
  header->version[1] = '\0'; 
  
  header->devmajor = 0; /*Redundant. Should be 0 from memset*/
  header->devminor = 0; /*Redundant. Should be 0 from memset*/
  
  return 0;
}

/**
 *@fn: split_name
 *@brief: If we are here then the file/dir/symlink is to long and won't
           fit into the name field. We need to search for a '/'. If there
	   is none and fullname == MAXFULLNAMELEN_ then store in 
	   header.name without a null terminator. Otherwise if fullname
	   is greater then error and dont save name. 0 for success, -1 for
	   failure.
 *@params: struct ustarheader *header, char *fullname, int len
 *@return: int
**/
int split_name(struct ustarheader *header, char *fullname, int len){
  int lenprefix;
  char *prefixend = NULL;


  /* Find the first '/' after 101 chars of the fullname */
  prefixend =  fullname + len; /* Go to last address. 1 char remains*/
  prefixend += (-MAXNAMELEN_ - 1); /* Move back max length name and '/'*/
  if((prefixend = strchr(prefixend, '/'))){ /* if '/' found, point there*/
    if((lenprefix = strlen(prefixend)) < PREFIXMAX_ ){
      /* Don't inclue the slash*/
      strncpy(header->prefix, prefixend, lenprefix); /* Copy the prefix*/
      if(lenprefix != PREFIXMAX_) /* If there is still space*/
	/* Don't include the slash*/
	header->prefix[lenprefix] = '\0'; /* Null terminate*/
    }
    /* Might have edge case with null terminating*/
    strncpy(header->name, prefixend + 1, len - lenprefix);/* 1 past'/'*/
  }
  else if(NULL == prefixend){ /*No '/' found and fullname is a too long*/
    errors();
    return -1;
  }
  
  return 0;
}


int pad_block(char *paddedblock, struct ustarheader *header){
  return 0;
}


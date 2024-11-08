#ifndef MODES_H_
#define MODES_H_

/* Permission macros*/
#define S_ISVTX          01000
#define ALLREADPERMS_    (S_IRUSR | S_IRGRP | S_IROTH)
#define ALLWRITEPERMS_   (S_IWUSR | S_IWGRP | S_IWOTH)
#define ALLEXECUTEPERMS_ (S_IXUSR | S_IXGRP | S_IXOTH)
#define ALLRWPERMS_      (ALLREADPERMS_ | ALLWRITEPERMS_)
#define ALLPERMS_        (ALRWPREMS_ | ALLEXECUTEPERMS_)


/* USTAR macros*/
#define MAXNAMELEN_ 100
#define MAXLINKNAMELEN_ 100
#define MAGICMAX_ 6
#define VERSIONMAX_ 2
#define UNAMEMAX_ 32
#define GNAMEMAX_ 32
#define PREFIXMAX_ 155
#define ALLMODES_ (S_ISUID | S_ISGID | S_ISVTX | ALLPERMS)
#define REGFILETYPE_ '\0'
#define SYMLINKTYPE_ '2'
#define DIRTYPE_     '5'

/* Other Macros*/
#define MAXFULLNAMELEN_ (MAXNAMELEN_ + PREFIXMAX_)
#define PARENT_ "."
#define SELF_ ".."
#define HEADERMAGIC_ "ustar\0"
#define HEADERVERSION_ "00"
#define BLOCKLEN_       512

struct ustarheader{
  char name[MAXNAMELEN_];         /* 0 100 bits. NUL-terminated if NUL fits*/
  mode_t mode;                    /* 100 8 bits*/
  uid_t uid;                      /* 108 8 bits*/
  gid_t gid;                      /* 116 8 bits*/
  size_t size;                    /* 124 12 bits*/
  time_t mtime;                   /* 136 12 bits*/
  unsigned int chksum;            /* 148 8 bits*/
  char typeflag;                  /* 156 1 bit*/
  char linkname[MAXLINKNAMELEN_]; /* 157 100 bits NUL-terminated if NUL fits*/
  char magic[MAGICMAX_];          /* 257 6 bits. “ustar” & NUL-terminated*/
  char version[VERSIONMAX_];      /* 263 2 bits must be “00” (zero-zero)*/
  char uname[UNAMEMAX_];          /* 265 32 bits NUL-terminated*/
  char gname[GNAMEMAX_];          /* 297 32 bits NUL-terminated*/
  int devmajor;                   /* 329 8 bits*/
  int devminor;                   /* 337 8 bits*/
  char prefix[PREFIXMAX_];        /* 345 155 bits NUL-terminated if NUL fits*/ 
};

int create_mode(char *tarfile, char *files[], int flags);
int table_contents_mode(char *tarfile, char *files[], int flags);
int extract_mode(char *tarfile, char *files[], int flags);
void create_directory(char *dirname, int flags); 
void create_file(char *filename, int flags);
void create_link(char *linkname, int flags);
int create_header(char *tarfile, char *files[]);

int create_block(char *fullname, struct ustarheader *header);

int split_name(struct ustarheader *header, char *fullname, int len);

int pad_block(char *paddedblock, struct ustarheader *header);

#endif

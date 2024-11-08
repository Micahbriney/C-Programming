#ifndef OPT_H_
#define OPT_H_

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>
#include <string.h>

/* ----- Masks ----- */
#define vMODEPOS_ 0
#define aMODEPOS_ 1
#define NMODEPOS_ 2
#define CLIENTMODEPOS_ 3
/* Modes*/
#define vMODE_      0x01 /* Verbose Mode */
#define aMODE_      0x02 /* Accept all Connnections mode */
#define NMODE_      0x04 /* Do not load ncurses */
#define CLIENTMODE_ 0x08 /* Load program into client mode */
#define HINIBBLE_   4
#define LOWNIBBLE_  4
#define INVALIDPORTNUM_ 0

/* getopt macros*/
#define MAXOPTS_ 3 /* Max number of opts: [v] [a] [N]*/
#define MAXARGS_ 6 /* Max # arguments: prog [v] [a] [N] [hostname] port*/
#define CLIENT_  (MAXARGS_ - MAXOPTS_ - 1) /* hostname & port provided*/
#define MINARGS_ 1 /* Min number of arguments: port number*/
#define OPTSTRING_ "vaN" /* Usage Options */

/* strtol macro*/
#define BASE_ 10

/* ----- Function Prototypes ----- */
unsigned int parse_options(int argc, char* argv[]);
void usage(char *argv);
void print_modes(int modes);
unsigned int get_port(const char *portStr);

#endif

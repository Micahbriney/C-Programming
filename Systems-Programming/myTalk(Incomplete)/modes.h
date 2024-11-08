#ifndef MODES_H_
#define MODES_H_

int server_mode(int port, int verbose, int accept);
int client_mode(char *servername, int port, int verbose);
void errorout(int fd, const char *error);

#endif

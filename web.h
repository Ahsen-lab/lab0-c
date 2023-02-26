#ifndef TINYWEB_H
#define TINYWEB_H

#include <netinet/in.h>

int web_open(int port);

char *web_recv(int fd, struct sockaddr_in *clientaddr);

void web_send(int out_fd, char *buffer);

typedef struct sockaddr SA;
int web_fd;
bool use_web_fd;

#endif

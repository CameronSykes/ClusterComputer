// Author: Cameron Sykes (cjsr4d@umsystem.edu)
// File: uici.h
// Purpose: Socket implementation functions

#ifndef CLUSTERCOMPUTER_UICI_H
#define CLUSTERCOMPUTER_UICI_H

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>

#include "uici.h"
#include "uiciname.h"
#include "clusterOutput.h"

typedef unsigned short u_port_t;
int u_open(u_port_t port);
int u_accept(int fd);
int u_connect(u_port_t port, char* host);
void r_close(int fd);
char* r_read(int fd);
void r_write(int fd, char* buff);

#endif //CLUSTERCOMPUTER_UICI_H

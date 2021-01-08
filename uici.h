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
int u_accept(int fd, char* hostn, int hostNameSize);
int u_connect(u_port_t port, char* hostNameSize);
void r_close(int fd);

#endif //CLUSTERCOMPUTER_UICI_H

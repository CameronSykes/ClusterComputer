// Author: Cameron Sykes (cjsr4d@umsystem.edu)
// File: uiciname.h
// Function: Name resolution functions

#ifndef CLUSTERCOMPUTER_UICINAME_H
#define CLUSTERCOMPUTER_UICINAME_H

#include <ctype.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define REENTRANT_NONE 0
#define REENTRANT_R 1
#define REENTRANT_MUTEX 2
#define REENTRANT_POSIX 3

void nameToAddr(char* name, in_addr_t* addr);
void addrToName(struct in_addr addr, char* name, int namelen);

#endif //CLUSTERCOMPUTER_UICINAME_H

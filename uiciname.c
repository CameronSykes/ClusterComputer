// Author: Cameron Sykes (cjsr4d@umsystem.edu)
// File: uiciname.h
// Purpose: Name resolution functions

#include "uiciname.h"
#include "clusterOutput.h"

#ifndef REENTRANCY
#define REENTRANCY REENTRANT_NONE
#endif // REENTRANCY

#if REENTRANCY==REENTRANT_MUTEX
#include <pthread.h>
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
#endif


#if REENTRANCY==REENTRANT_NONE

// A struct in_addr needs to be converted to a host name
void addrToName(struct in_addr addr, char* name, int namelen)
{
    struct hostent* hostptr;
    hostptr = gethostbyaddr((char*) &addr, 4, AF_INET);

    if(hostptr == NULL)
    {
        strncpy(name, inet_ntoa(addr), namelen - 1);
    }
    else
    {
        strncpy(name, hostptr->h_name, namelen - 1);
    }

    name[namelen - 1] = 0;
}

// Throws an errorMessage if the host can not be reached at that name
void nameToAddr(char* name, in_addr_t* addr)
{
    struct hostent* hp;

    if(isdigit((int) (*name)))
    {
        *addr = inet_addr(name);
    }
    else
    {
        hp = gethostbyname(name);
        if(hp == NULL)
        {
            errorMessage("gethostbyname() in nameToAddr() with REENTRANCY == REENTRANT_NONE");
        }
        
        memcpy((char*) addr, hp->h_addr_list[0], hp->h_length);
    }
}

#elif REENTRANCY==REENTRANT_R
#define GETHOST_BUFSIZE 8192

void addrToName(struct in_addr addr, char* name, int namelen)
{
    char buf[GETHOST_BUFSIZE];
    int h_error;
    struct hostent* hp;
    struct hostent result;
    
    hp = gethostbyaddr_r((char*) &addr, 4, AF_INET, &result, buf, GETHOST_BUFSIZE, &h_error);
    
    if(hp == NULL)
    {
        strncpy(name, inet_ntoa(addr), namelen - 1);
    }
    else
    {
        strncpy(name, hp->h_name, namelen - 1);
    }
    
    name[namelen - 1] = 0;
}

void nameToAddr(char* name, in_addr_t* addr)
{
    char buf[GETHOST_BUFSIZE];
    int h_error;
    struct hostent* hp;
    struct hostent result;
    
    if(isdigit((int) (*name)))
    {
        *addr = inet_addr(name);
    }
    else
    {
        hp = gethostbyname_r(name, &result, buf, GETHOST_BUFSIZE, &h_error);
        if(hp == NULL)
        {
            nameResolutionError("gethostbyname_r() in nameToAddr() with REENTRANCY == REENTRANT_R");
        }
        
        memcpy((char*)addr, hp->h_addr_list[0], hp->h_length);
    }
}

#elif REENTRANCY==REENTRANT_MUTEX

void addrToName(struct in_addr addr, char* name, int namelen)
{
    struct hostent* hostptr;
    
    if(pthread_mutex_lock(&mutex) == -1)
    {
        strncpy(name, inet_ntoa(addr), namelen - 1);
        name[namelen - 1] = 0;
        return;
    }
    
    hostptr = gethostbyaddr((char*) &addr, 4, AF_INET);
    if(hostptr == NULL)
    {
        strncpy(name, inet_ntoa(addr), namelen - 1);
    }
    else
    {
        strncpy(name, hostptr->h_name, namelen - 1);
    }
    
    pthread_mutex_unlock(&mutex);
    name[namelen - 1] = 0;
}

void nameToAddr(char* name, in_addr_t* addr)
{
    struct hostent* hp;
    
    if(isdigit((int)(*name)))
    {
        *addr = inet_addr(name);
    }
    else
    {
        if(pthread_mutex_lock(&mutex) == -1)
        {
            nameResolutionError("Could not lock the thread in nameToAddr() where REENTRANCY == REENTRANT_MUTEX");
        }
        
        hp = gethostbyname(name);
        if(hp == NULL)
        {
            pthread_mutex_unlock(&mutex);
            nameResolutionError("Could not unlock the thread in nameToAddr() where REENTRANCY == REENTRANT_MUTEX");
        }
        
        memcpy((char*) addr, hp->h_addr_list[0], hp->h_length);
        pthread_mutex_unlock(&mutex);
    }
}

#elif REENTRANCY==REENTRANT_POSIX

void addrToName(struct in_addr addr, char* name, int namelen)
{
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = 0;
    saddr.sin_addr = addr;
    
    if(getnameinfo((struct sockaddr*) &saddr, sizeof(saddr), name, namelen, NULL, 0, 0) != 0)
    {
        strncpy(name, inet_ntoa(addr), namelen - 1);
        name[namelen - 1] = 0;
    }
}

void nameToAddr(char* name, in_addr_t* addr)
{
    struct addrinfo hints;
    struct addrinfo* res;
    struct sockaddr_in *saddr;
    
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = PF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_addrlen = 0;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    
    if(getaddrinfo(name, NULL, &hints, &res) != 0)
    {
        nameResolutionError("Could not find host based on the given node and service");
    }
    
    saddr = (struct sockaddr_in*)(res->ai_addr);
    memcpy(addr, &saddr->sin_addr.s_addr, 4);
    freeaddrinfo(res);
}

#endif
// Author: Cameron Sykes (cjsr4d@umsystem.edu)
// File: uici.c
// Purpose: Socket implementation functions

#include "uici.h"

int u_open(u_port_t port)
{
    struct sockaddr_in server;
    int sock;
    int true = 1;
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1)
    {
        fprintf(stderr, "[%ld]: waiting for the first connection on port %d\n", (long)getpid(), (int)port);
        errorMessage("Failed to create listening endpoint");
    }
    
    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&true, sizeof(true)) == -1)
    {
        r_close(sock);
        errorMessage("Failed to create listening endpoint");
    }
    
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons((short) port);
    
    if((bind(sock, (struct sockaddr*) &server, sizeof(server))) == -1 || listen(sock, 50) == -1)
    {
        errorMessage("Failed to create listening endpoint");
    }
    
    return sock;
}

int u_accept(int fd, char* hostName, int hostNameSize)
{
    int len = sizeof(struct sockaddr);
    struct sockaddr_in netclient;
    int acceptVal;
    
    while((acceptVal = accept(fd, (struct sockaddr*)(&netclient), &len)) == -1 && (errno == EINTR));
    
    if(hostName == NULL || hostNameSize <= 0)
    {
        errorMessage("Failed to accept connections");
    }
    
    addrToName(netclient.sin_addr, hostName, hostNameSize);
    fprintf(stderr, "[%ld]: connected to %s\n", (long)getpid(), hostName);
    
    return acceptVal;
}

int u_connect(u_port_t port, char* hostName)
{
    int connectVal;
    struct sockaddr_in server;
    int sock;
    fd_set sockset;
    
    nameToAddr(hostName, &(server.sin_addr.s_addr));
    server.sin_port = htons((short) port);
    server.sin_family = AF_INET;
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1)
    {
        errorMessage("Failed to connect");
    }
    
    connectVal = connect(sock, (struct sockaddr*) &server, sizeof(server));
    if(connectVal == -1 || errno == EINTR || errno == EALREADY)
    {
        FD_ZERO(&sockset);
        FD_SET(sock, &sockset);
        while((connectVal = select(sock + 1, NULL, &sockset, NULL, NULL)) == -1)
        {
            FD_ZERO(&sockset);
            FD_SET(sock, &sockset);
        }
    }
    
    if(connectVal == -1)
    {
        r_close(sock);
        errorMessage("Failed to connect");
    }
    
    return sock;
}

void r_close(int fd)
{
    int closeVal;
    while(closeVal = close(fd), closeVal == -1 && errno == EINTR);
}
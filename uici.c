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
    
    fprintf(stderr, "[%ld]: Port %i is listening\n", (long)getpid(), port);
    
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
    fprintf(stderr, "[%ld]: Accepted connection request from %s\n", (long)getpid(), hostName);
    
    return acceptVal;
}

int u_connect(u_port_t port)
{
    int connectVal;
    struct sockaddr_in server;
    int sock;
    fd_set sockSet;
    
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
        FD_ZERO(&sockSet);
        FD_SET(sock, &sockSet);
        while((connectVal = select(sock + 1, NULL, &sockSet, NULL, NULL)) == -1)
        {
            FD_ZERO(&sockSet);
            FD_SET(sock, &sockSet);
        }
    }
    
    if(connectVal == -1)
    {
        r_close(sock);
        errorMessage("Failed to connect");
    }
    
    fprintf(stderr, "[%ld]: Connected\n", (long)getpid());
    
    return sock;
}

void r_close(int fd)
{
    int closeVal;
    while(closeVal = close(fd), closeVal == -1 && errno == EINTR);
}

// Purpose: Read a message at the socket, restarting if interrupted by a non-error signal
// Returns: char*       --- The message at that socket
// Params:  int fd      --- File descriptor, presumed to be connected and able to communicate
//          size_t size --- The number of bytes that read() will attempt to read
char* r_read(int fd, size_t size)
{
    ssize_t retval;
    int totalBytes = 0;
    char* buff = malloc(size);
    
    while(1)
    {
        retval = read(fd, buff, size);
        if(retval > 0)
        {
            // Characters have been read from the socket
            totalBytes += retval;
        }
        else
        {
            if(retval == -1)
            {
                // An error occurred while reading
                if(errno != EINTR)
                {
                    // The error was not caused by a non-error interruption
                    errorMessage("Could not read from that file descriptor");
                }
            }
            else if(retval == 0)
            {
                // End-of-line
                break;
            }
        }
    }
    
    // Output the total number of bytes read. Partially for sanity check
    fprintf(stderr, "Bytes read: %i\n", totalBytes);
    return buff;
}
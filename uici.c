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

// Purpose: Creates an endpoint file descriptor then connects the socket referred to by that file descriptor to the address referred to by the u_port_t port variable
// Returns: int             --- The file descriptor that refers to the socket created by socket.
// Params:  u_port_t port   --- The port to send messages to.
int u_connect(u_port_t port)
{
    int connectVal;
    struct sockaddr_in server;
    int sock;
    fd_set sockSet;
    
    // u_port_t port is wrapped up into a sockaddr_in struct for connect()
    server.sin_port = htons((short) port);
    server.sin_family = AF_INET;
    
    // Create a socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1)
    {
        errorMessage("Failed to connect");
    }
    
    // Connect to the socket via the port provided (see above, server.sin_port)
    connectVal = connect(sock, (struct sockaddr*) &server, sizeof(server));
    if(connectVal == -1 || errno == EINTR || errno == EALREADY)
    {
        // An error of some sort occurred
        FD_ZERO(&sockSet); // Init sockSet to 0/empty
        FD_SET(sock, &sockSet); // Add the created socket to the fd_set sockSet
        
        // Search for space to read or write in all of the sockets up to the socket numbered sock+1
        while((connectVal = select(sock + 1, NULL, &sockSet, NULL, NULL)) == -1 && errno == EINTR)
        {
            // No space was found. Reset sockSet and search again
            FD_ZERO(&sockSet);
            FD_SET(sock, &sockSet);
        }
    }
    
    if(connectVal == -1)
    {
        // The connection threw an error, close the socket, alert the user
        r_close(sock);
        errorMessage("Failed to connect");
    }
    
    // User feedback
    fprintf(stderr, "[%ld]: Connected\n", (long)getpid());
    return sock;
}

// Purpose: Close a socket, restarting if interrupted by a non-error signal
// Returns: void
// Params:  int fd  --- File descriptor to close
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
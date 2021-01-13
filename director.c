#include <stdlib.h>
#include "uici.h"

int main(int argc, char *argv[])
{
    u_port_t portNumber;
    int fd;
    
    if (argc != 2)
    {
       usage(1);
    }
    
    portNumber = atoi(argv[1]); // The user needs to pass the port that they wish to connect to
    fd = u_connect(portNumber); // Connect the port and return a file descriptor representing said port
    
    r_write(fd, "This is director, do you copy?"); // Write to the node
    fprintf(stderr, "[%ld]: Sent a message to node\n", (long)getpid()); // User feedback
    
    return 0;
}

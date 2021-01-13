#include <stdlib.h>
#include "uici.h"

int main(int argc, char *argv[])
{
    u_port_t portNumber;
    int fd;
    
    execName = argv[0];
    if (argc > 3)
    {
       usage();
    }
    
    portNumber = atoi(argv[2]);
    fd = u_connect(portNumber);
    
    r_write(fd, "This is director, do you copy?");
    fprintf(stderr, "[%ld]: Sent a message to node\n", (long)getpid());
    
    return 0;
}

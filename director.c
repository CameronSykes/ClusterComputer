#include <stdlib.h>
#include "uici.h"

int main(int argc, char *argv[])
{
    u_port_t portNumber;
    
    execName = argv[0];
    if (argc > 3)
    {
       usage();
    }
    
    portNumber = atoi(argv[2]);
    u_connect(portNumber);
    
    // read and/or write to the node
    // fprintf(stderr, "[%ld]: Sent a message to node\n", (long)getpid());
    
    return 0;
}

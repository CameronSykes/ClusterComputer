/* A simple server in the internet domain using TCP
   The port number is passed as an argument */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

#include "clusterOutput.h"
#include "uici.h"

int node(int argc, char *argv[])
{
    int spawned = 0; // Check on the number of children to stop fork bombs.
    int child;
    char buffer[256];
    u_port_t portNumber;
    int communication_fd;
    int listen_fd;
    
    if (argc < 2)
    {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    
    portNumber = (u_port_t) atoi(argv[1]);
    listen_fd = u_open(portNumber);
    
    while(1 && spawned < 20)
    {
        communication_fd = u_accept(listen_fd, buffer, 256);
        
        child = fork();
        spawned++;
        if(child == -1)
        {
            errorMessage("Failed to fork a child");
        }
        else if(child == 0)
        {
            r_close(listen_fd);
        }
        else
        {
            r_close(communication_fd);
        }
    }
    
    while(waitpid(-1, NULL, WNOHANG) > 0);
    return 0;
}

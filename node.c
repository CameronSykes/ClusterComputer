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
    u_port_t portNumber;
    int communication_fd;
    int listen_fd;
    char* message;
    
    if (argc < 2)
    {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    
    portNumber = (u_port_t) atoi(argv[1]);
    listen_fd = u_open(portNumber);
    
    while(spawned < 20)
    {
        communication_fd = u_accept(listen_fd);
        
        child = fork();
        spawned++;
        if(child == -1)
        {
            r_close(listen_fd);
            r_close(communication_fd);
            errorMessage("Failed to fork a child");
        }
        else if(child == 0)
        {
            // Child code
            message = r_read(listen_fd);
            fprintf(stderr, "Message received: %s\n", message);
            
            fprintf(stderr, "[%ld]: Closing the socket\n", (long)getpid());
            r_close(listen_fd);
        }
        else
        {
            // Parent code
            r_close(communication_fd);
            while(waitpid(-1, NULL, WNOHANG) > 0);
        }
    }
    
    return 0;
}

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
    
    if (argc != 2)
    {
        usage(0);
    }
    
    portNumber = (u_port_t) atoi(argv[1]); // The user passes the port that they wish to connect to
    listen_fd = u_open(portNumber); // Open the port for listening
    
    while(spawned < 1)
    {
        communication_fd = u_accept(listen_fd); // Accept the request for communication from the director
        
        // Begin fork
        child = fork();
        spawned++;
        if(child == -1)
        {
            // The sockets need to be closed if a fork fails
            r_close(listen_fd);
            r_close(communication_fd);
            errorMessage("Failed to fork a child");
        }
        else if(child == 0) // Child code
        {
            // Close the file descriptor used to open the port
            r_close(listen_fd);
            
            // Read from the socket
            message = r_read(communication_fd);
            fprintf(stderr, "Message received: %s\n", message); // Output read message
            
            // User feedback
            fprintf(stderr, "[%ld]: Closing the socket\n", (long)getpid());
            r_close(communication_fd);
            
            return 0;
        }
        else // Parent code
        {
            r_close(communication_fd); // Close the file descriptor used for communicating to the node
            while(waitpid(-1, NULL, WNOHANG) > 0); // Clean up zombies
        }
    }
    
    return 0;
}

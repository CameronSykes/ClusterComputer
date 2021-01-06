// Author: Cameron Sykes (cjsr4d@umsystem.edu)
// File: clusterOutput.c
// Purpose: Handle all of the output for usage or errors.

#include "clusterOutput.h"

void usage(int execType)
{
    char* usageStr = malloc(strlen("director [hostname] [port]"));
    if(execType == 1)
    {
        strcpy(usageStr, "director [hostname] [port]");
    }
    else if(execType == 2)
    {
        strcpy(usageStr, "node [port]");
    }
    else
    {
        perror("That executable option is invalid");
    }
    
    fprintf(stderr, "========== USAGE ==========\n%s\n%s\n", execName, usageStr);
    exit(EXIT_FAILURE);
}

void errorMessage(const char* errStr, int execType)
{
    char* errMsg = malloc(strlen("Error in ") + strlen(execName) + strlen(errStr) + 1);
    strcpy(errMsg, "Error in ");
    strcat(errMsg, execName);
    strcat(errMsg, errStr);
    
    perror(errMsg);
    usage(execType);
}
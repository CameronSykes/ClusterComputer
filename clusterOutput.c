// Author: Cameron Sykes (cjsr4d@umsystem.edu)
// File: clusterOutput.c
// Purpose: Handle all of the output for usage or errors.

#include "clusterOutput.h"

void usage(int execType)
{
    int usageLen = execType ? strlen("director [port]") + 1 : strlen("node [port]") + 1;
    
    char* usageStr = malloc(usageLen);
    if(execType)
    {
        strcpy(usageStr, "director ");
    }
    else
    {
        strcpy(usageStr, "node ");
    }
    strcat(usageStr, "[port]");
    
    fprintf(stderr, "========== USAGE ==========\n%s\n", usageStr);
    exit(EXIT_FAILURE);
}

void errorMessage(const char* errStr)
{
    char* errMsg = malloc(strlen("Error: ") + strlen(errStr) + 1);
    strcpy(errMsg, "Error: ");
    strcat(errMsg, errStr);
    
    perror(errMsg);
}
// Author: Cameron Sykes (cjsr4d@umsystem.edu)
// File: clusterOutput.h
// Purpose: Handle all of the output for usage or errors.

#ifndef CLUSTERCOMPUTER_CLUSTEROUTPUT_H
#define CLUSTERCOMPUTER_CLUSTEROUTPUT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* execName;

void usage();
void errorMessage(const char* errStr, int execType);

#endif //CLUSTERCOMPUTER_CLUSTEROUTPUT_H

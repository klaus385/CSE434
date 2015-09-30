/*******************************************************************
 * Authors: Katie Gosse, Emily Falkner
 * Course: CSE 434, Computer Networks
 * Semester: Fall 2015
 * Project Part: 1
 * Time Spent: 5 minutes
 * File Name: concurrentServer.h
 *****************************************************************/

#ifndef _CONCURRENTSERVER_H_
#define _CONCURRENTSERVER_H_

#include <stdio.h>      // used for input/output
#include <stdlib.h>     // contains many standard C functions, like exit()
#include <string.h>     // contains defintions for string manipulation
#include <unistd.h>     // gives access to POSIX for sys calls
#include <sys/types.h>  // contains definitions of numerous sys call data types
#include <sys/socket.h> // includes structure definitions for socket types
#include <netinet/in.h> // contains structures/constants for internet domain addresses

struct client_list 
{
	char *filename; // char array for the filename sent by client
	char op;   // File operation to perform on filename
	char cont; // Used for if the client wants to make another request (Y or N)
	FILE *fp;  // File pointer
};

void error (char *msg);

#endif /* _CONCURRENTSERVER_H_ */

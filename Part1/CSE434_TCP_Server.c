/*******************************************************************
 * Authors: Katie Gosse, Emily Faulkner
 * Course: CSE 434, Computer Networks
 * Semester: Fall 2015
 * Project Part: 1
 * Time Spent: ???
 * File Name: CSE434_TCP_Server.c
 * Description: ???
 *****************************************************************/

#include <stdio.h>      // used for input/output
#include <stdlib.h>     // contains many standard C functions, like exit()
#include <string.h>     // contains defintions for string manipulation
#include <unistd.h>	// gives access to POSIX for sys calls
#include <sys/types.h>  // contains definitions of numerous sys call data types
#include <sys/socket.h> // includes structure definitions for socket types
#include <netinet/in.h> // contains structures/constants for internet domain addresses

/*******************************************************************
 * Steps to create a server:
 * 1. Create socket using socket() sys call
 * 2. "Name" the socket using bind() system call
 * 3. Listen to connections using listen() sys call
 * 4. Wait for a client request to arrive
 * 5. Accept a connection using accept() sys call
 * 6. Send/receive data
 * 7. Repeat steps 4 - 7
 *****************************************************************/

void error (char *msg);

int main (int argc, char *argv[])
{
	// Variable lists - broken up for clarity
	int socket_fd, new_socket_fd; // File descripters - stores the returned values from socket() and accept()
	int  port_num; // Port number on which the server accepts connections
	int rw_num; // Return value for read()/write() calls
	char buffer[256]; // Characters read from the socket connection
	struct sockaddr_in server_addr, client_addr; // Structure for handling iternet addresses - see below
	socklen_t client_addr_sz; // Size of the address of the client (needed for accept()) - found in sys/socket.h
	
	/*******************************************************************
	 * Found in netinet/in.h
	 *
	 * struct sockaddr_in {
	 *	short		sin_family;   // Only use AF_INET
	 *	unsigned short  sin_port;
	 *	struct in_addr	sin_addr;
	 *	char		sin_zero[8];  // Not used, so zero out
	 * }
	 *
	 * struct in_addr {
	 *	unsigned long	s_addr;
	 * }
	 *****************************************************************/

	// Server needs one argument from command line - the port number
	if (argv[1] == NULL)
		error("ERROR: No port provided. Exiting now.");

	// Possible check for correct port number???????????????????????????????????????????????????????????
	
	// Grab port number from argv and convert to an int
	port_num = atoi(argv[1]);

	/*******************************************************************
	 * int socket (int domain, int type, int protocol)
	 * domain
	 *	communications domain in which the socket is created
	 *	e.g. AF_INET
	 * type
	 *	type of socket to  create, e.g. SOCK_STREAM
	 * protocol
	 *	the protocol to be used (put zero to use default protocol
	 *	for address family and type)
	 * return value
	 *	if successful, a non-negative int
	 *	else -1
	 *******************************************************************/
	// Create a socket or "doorway for communication" and check if the socket opened successfully
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0)
		error("ERROR: Unsuccessful socket opening. Exiting now.");

	// Make sure server_addr buffer is clear and set the struct variables
	memset((char *) &server_addr, '0', sizeof(server_addr));
	server_addr.sin_family = AF_INET; // Always use AF_INET
	server_addr.sin_port = htons(port_num); // Converts port_num from host byte order to network byte order
	server_addr.sin_addr.sin_addr = INADDR_ANY; // Sets the IP address of the server to the IP address of the host machine
		
	return 0;
}

/*******************************************************************
 * error: Prints the error message (if one exists) within msg
 * and then exits the program with and unsuccessful termination
 * code.
 *****************************************************************/
void error (char *msg)
{
	perror(msg);
	exit(EXIT_FAILIURE); 
}
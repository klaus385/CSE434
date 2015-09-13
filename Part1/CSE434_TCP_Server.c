/*******************************************************************
 * Authors: Katie Gosse, Emily Falkner
 * Course: CSE 434, Computer Networks
 * Semester: Fall 2015
 * Project Part: 1
 * Time Spent: ???
 * File Name: CSE434_TCP_Server.c
 * Description: ???
 *****************************************************************/

#include "CSE434_TCP_Server.h"

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

int main (int argc, char *argv[])
{
	// Variable lists - broken up for clarity
	int socket_fd, new_socket_fd; // File descripters - stores the returned values from socket() and accept()
	int  port_num; // Port number on which the server accepts connections
	int rw_num; // Return value for read()/write() calls
	int pid; // process ID of the child created by fork()
	int i; // Used in for loop 
	int closed = 0; // Keeps track if the connected was closed due to duplicate client
	int cid = 0; // Keeps track of where the client id is stored in the array
	int client_id[25]; // An inefficient but easy way to store up to 25 client ID numbers
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
	{
		error("ERROR: No port provided. Exiting now.");
	}
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
	//socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		error("ERROR: Unsuccessful socket opening. Exiting now.");
	}
	// Make sure server_addr buffer is clear and set the struct variables
	memset((char *) &server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET; // Always use AF_INET
	server_addr.sin_port = htons(port_num); // Converts port_num from host byte order to network byte order
	server_addr.sin_addr.s_addr = INADDR_ANY; // Sets the IP address of the server to the IP address of the host machine
	
	/*******************************************************************
	 * int bind (int sockfd, const struct sockaddr *addr, socklen_t addrlen);
	 * sockfd
	 *	socket file descripter, e.g. socket_fd
	 * *addr
	 *	the address to which the socket is bound
	 * addrlen
	 *	size of the address to which the socket is bound
	 * return value
	 *	if successful, 0
	 *	else -1
	 *******************************************************************/
	// Bind the socket to an address
	if (bind(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1)
	{
		error("ERROR: Unsuccessful socket binding. Exiting now.");
	}

	/*******************************************************************
	 * Concurrent Servers:
	 * 1. Listen for client request
	 * 2. Accept client request
	 * 3. Create a child process to handle client (forking)
	 * 4. Parent continues to listen
	 * 5. Repeat
	 *******************************************************************/
 
	/*******************************************************************
	 * int listen (int sockfd, int backlog);
	 * sockfd
	 *	socket file descripter, i.e. socket_fd
	 * backlog
	 *	max number of queued pending connections, usually set to 5
	 * return value
	 *	if successful, 0
	 *	else -1
	 * note:
	 *	sockfd is passive, meaning it is able to accept incoming
	 *	connection requests
	 *******************************************************************/
	// Have the server listen
	if (listen(socket_fd, 5) == -1)
	{
		error ("ERROR: Socket unable to listen. Exiting now.");
	}

	while(1)
	{
		/*******************************************************************
		 * int accept (int sockfd, struct sockaddr *addr, socklen_t *addrlen);
		 * sockfd
		 *	socket file descripter, i.e. socket_fd
		 * *addr
		 *	pointer to the client address
		 * *addrlen
		 *	pointer to the size of the client address
		 * return value
		 *	if succesful, non-negative integer
		 *	else -1
		 * note:
		 *	temporarily suspends the (listening) server from doing
		 *	anything until a client is connected
		 *******************************************************************/
		// Server accepts client
		client_addr_sz = sizeof(client_addr);
		new_socket_fd = accept(socket_fd, (struct sockaddr *) &client_addr, &client_addr_sz);
		
		// Throw an error if the connection was unsuccessful
		if (new_socket_fd == -1)
		{
			fprintf(stderr, "error code: %s", strerror(new_socket_fd));
	 		error("ERROR: Could not accept client. Exiting now.");
		}
		closed = 0;

		// Super cheap implementation of recording a client number
		// Would never do this for real world code
		memset(buffer, 0, sizeof(buffer));
		if (read(new_socket_fd, buffer, sizeof(buffer)) == -1)
		{
			error("ERROR: Could not read client number. Exiting now.");
		}
		
	//	printf("Client number is %s\n", buffer);

		for (i = 0; i < 25; ++i)
		{
			if (atoi(buffer) == client_id[i])
			{
				printf("Client already connected before. Terminating connection.\n");
				close(new_socket_fd);
				closed = 1;
				i = 25;
			}
		}
		client_id[cid] = atoi(buffer);

		/*******************************************************************
		 * int fork (void);
		 * return value
		 *	if successful, child process is given a PID, which is
		 *	returned to the parent and a 0 is returned to the child
		 *	else, -1 is returned to parent
		 *******************************************************************/
		// If the connection was successful, create a child process to handle the client
		// and check if child process was created
		if (closed == 0)
		{
			if ((pid = fork()) == -1)
			{
				error("ERROR: Could not create child process. Exiting now.");
			}
			else if (pid == 0)
			{
				/******************************************************************
				 * int close (int fd);
				 * fd
				 *	file descriptor, in this case a socket file descripter
				 * return value
				 *	if successful, 0
				 *	else -1
				 * note:
				 *	does not remove socket, only frees it up so it can be
				 *	reused (for future child processes)
				 *******************************************************************/
				// Close, or free, the listening socket (socket_fd)
				if (close(socket_fd) == -1)
				{
					error("ERROR: Could not close listening socket. Exiting now.");
				}
				
				memset(buffer, 0, sizeof(buffer));
				while (read(new_socket_fd, buffer, sizeof(buffer)) != -1)
				{
					if (buffer[0] != '\0' )
					{
						printf("Client (%d) says: %s", client_id[cid], buffer);
				
						if (rw_num = write(new_socket_fd, "Heard you loud and clear.", 25) == -1)
						{
							error("ERROR: Could not write to client. Exiting now.");
						}

						memset(buffer, 0, sizeof(buffer));
					}
				}
				
				++cid;
			}

			if (close(new_socket_fd) == -1)
			{
				error("ERROR: Could not close client socket. Exiting now.");
			}

			memset((char*) &client_addr, 0, client_addr_sz);
		}	
	}

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
	exit(1); 
}

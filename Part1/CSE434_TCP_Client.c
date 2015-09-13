/***********************************************
 * Authors: Katie Gosse, Emily Falkner
 * Course: CSE 434, Computer Networks
 * Semester: Fall 2015
 * Project Part: 1
 * Time Spent: ???
 * File Name: CSE434_TCP_Client.c
 * Description: ???
 **********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h> // Needed for socket system call
#include <sys/types.h>  // Contains numerous data types used in system calls
#include <netinet/in.h> // Needed for internet domain address constants/structs
#include <netdb.h>	// Needed to define the structure hostent

/***********************************************
 * Client Program Steps:
 * 1. Create a socket with socket() sys call
 * 2. Connect the socket to the address of the
 *    server using the connect() sys call
 * 3. Send/receive data.. i.e. read(), write()
 * 4. Close communication
 **********************************************/

//This function is used whenever a system call fails. Displays a message about the error then aborts
void error(const char *msg)
{
	perror(msg);
	exit(0);
}


int main (int argc, char *argv[])
{
	//Variable declarations
	int sockfd; //sockfd is a file descriptor
	int clientno; //clientno stores the client number
	int portno; //portno stores the port number on which the server accepts the connections.
	int n;

	//serv_addr will contain the address of the server
	struct sockaddr_in serv_addr;

	//variable server is a pointer to a structure of type hostent
	/*
	struct  hostent
	{
	  char    *h_name;        // official name of host
	  char    **h_aliases;    // alias list
	  int     h_addrtype;     // host address type
	  int     h_length;       // length of address
	  char    **h_addr_list;  // list of addresses from name server
	  #define h_addr  h_addr_list[0]  // address, for backward compatiblity
	};
	*/

	struct hostent *server;

	char buffer[256];

	if (argc != 4)
		error("ERROR: Need three arguments.");

    	//made changes
	clientno = atoi(argv[2]);
	portno = atoi(argv[3]); //atoi() function can be used to convert port number from string to int


	//create socket
	//takes 3 arguments - address domain, type of socet, protocol (zeror allows the OS to choose
	//the appropriate protocols based on type of socket)
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		error("ERROR OPENING SOCKET");
	}

	//gethostname() takes a name as an argument and returns a pointer to a hostent containing information about that host
	//name is taken as argument from user
	server = gethostbyname(argv[1]);

	//If hostent structure is NULL, the system could not locate host name
	if (server == NULL)
	{
		fprintf(stderr, "ERROR NO SUCH HOST\n");
		exit(0);
	}

	//set server address buffer with xeros using bzero or memset
	//two arguments - pointer to buffer and sizeof buffer
	memset((char*) &serv_addr, 0, sizeof(serv_addr));

	//sets the fields in serv_addr and for the address family
	serv_addr.sin_family = AF_INET;

	//copies length bytes from s1 to s2
	bcopy((char *)server->h_addr,
           (char *)&serv_addr.sin_addr.s_addr,
           server->h_length);

	//contain the port number
	serv_addr.sin_port = htons(portno);

	//connect is callled by the client to establish connection to the server
	//takes 3 argments: socket file descripter,address of the host that it wants to connect to, and the size of this address
	if(connect(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
	{
        	error("ERROR CONNECTING");
	}

	// Send ID to server
	if (write(sockfd, argv[2], sizeof(argv[2])) < 0)
	{
		perror("ERROR: Could not send client number to server.");
	}

	printf("Please enter the message: ");

	//After a connection a client has succesfully connected to the server initilize buffer using bzero()
	memset(buffer, 0, sizeof(buffer));

	//set buffer to the message entered on console at client end for a maximum of 255 characters
	fgets(buffer, 255, stdin);

	//write from the buffer into the socket
	n = write(sockfd, buffer, strlen(buffer));

	//check if write was sucessful
	if (n <0)
	{
        	error("ERROR WRITING TO SOCKET");
	}

	//server can read and write after connection has been established
	//everything writen by client will be read by server and vice versa
	memset(buffer, 0, 256);
	n = read(sockfd,buffer,255);
	if (n < 0)
	{
        	error("ERROR READING FROM SOCKET");
	}
	printf("%s\n",buffer);
	//close connections using file descriptrs
	//close(sockfd);
	
	return 0;
}

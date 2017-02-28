#include<io.h>
#include<stdio.h>
#include<winsock2.h>

#pragma comment(lib,"ws2_32.lib") //Winsock Library

// ToDo: put matching definitions and headings in to a header file and include it!
#define bool	int
#define true	1
#define false	0

#define CUSTOM_IP		"127.0.0.1"
#define CUSTOM_PORT		8888

bool startWinsock();
SOCKET getSocket();
bool bindSocket(SOCKET socket);
bool listenForConnections(SOCKET serverSocket, int queueSize);
bool sendDataToClient(SOCKET clientSocket, char *message);
SOCKET acceptConnection(SOCKET serverSocket);
bool exitProgram(SOCKET serverSocket);

int main(int argc, char *argv[])
{
	SOCKET serverSocket,
		   clientSocket;
	char *message = "Welcome to my server!";


	printf("Attempting to start WinSock... ");
	if (startWinsock() == false)
	{
		printf("FAILED : %d  \n\n", WSAGetLastError());
		return 0;
	}
	printf("SUCCESS \n");


	printf("Attempting to create a socket... ");
	if ((serverSocket = getSocket()) == INVALID_SOCKET)
	{
		printf("FAILED : %d \n\n", WSAGetLastError());
		return 0;
	}
	printf("SUCCESS \n");


	printf("Attempting to bind socket... ");
	if (bindSocket(serverSocket) == false)
	{
		printf("FAILED : %d \n\n", WSAGetLastError());
		return 0;
	}
	printf("SUCCESS \n");


	printf("Attempting to listen for connections... ");
	if (listenForConnections(serverSocket, 3) == false)
	{
		printf("FAILED : %d \n\n", WSAGetLastError());
		return 0;
	}
	printf("SUCCESS \n");


	printf("Waiting for connections... ");
	if ((clientSocket = acceptConnection(serverSocket)) == false)
	{
		printf("FAILED : %d \n\n", WSAGetLastError());
		return 0;
	}
	printf("CONNECTED \n");


	printf("Attempting to send data... ");
	if (sendDataToClient(clientSocket, message) == false)
	{
		printf("FAILED : %d \n\n", WSAGetLastError());
		return 0;
	}
	printf("SUCCESS \n");


	printf("Attempting to close everything... ");
	if (exitProgram(serverSocket) == false)
	{
		printf("FAILED : %d \n\n", WSAGetLastError());
		return 0;
	}
	printf("SUCCESS \n");

	return 0;
}

bool startWinsock()
{
	WSADATA winsockLibrary; // Windows Sockets initialization information

	/* The WSAStartup function is used to start or initialise winsock library.
	It takes 2 parameters; the first one is the version we want to load;
	the second one is a WSADATA structure which will hold additional information after winsock has been loaded. */
	// (MAKEWORD) The highest version of Windows Sockets specification that the caller can use.
	if (WSAStartup(MAKEWORD(2, 2), &winsockLibrary) != 0)
		return false;
	else
		return true;
}

SOCKET getSocket()
{
	/*  Address Family : AF_INET(this is IP version 4)
		Type : SOCK_STREAM(this means connection oriented TCP protocol)
		Protocol : 0[or IPPROTO_TCP, IPPROTO_UDP] */
	return socket(AF_INET, SOCK_STREAM, 0);
}

bool bindSocket(SOCKET socket)
{
	struct sockaddr_in server;
	server.sin_family = AF_INET;			// Address Family : AF_INET(this is IP version 4)
	server.sin_addr.s_addr = INADDR_ANY;	// Listen at any/all interfaces
	server.sin_port = htons(CUSTOM_PORT);	// Converts a u_short from host to TCP/IP network byte order

	if (bind(socket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
		return false;
	else
		return true;
}

bool listenForConnections(SOCKET serverSocket, int queueSize)
{
	if (listen(serverSocket, queueSize) == SOCKET_ERROR)
		return false;
	else
		return true;
}

SOCKET acceptConnection(SOCKET serverSocket)
{
	int sizeOfClient = sizeof(struct sockaddr_in);
	struct sockaddr_in client;
	int status = accept(serverSocket, (struct sockaddr *)&client, &sizeOfClient);
	if (status == INVALID_SOCKET)
		return INVALID_SOCKET;
	else
		return status;
}



bool sendDataToClient(SOCKET clientSocket, char *message)
{
	if (send(clientSocket, message, strlen(message), 0) < 0)
		return false;
	else
		return true;
}

bool exitProgram(SOCKET serverSocket)
{
	bool isSafe = true;
	if (closesocket(socket) == SOCKET_ERROR)
		isSafe = false;
	/* To deregister itself from a Windows Sockets implementation
	and allow the implementation to free any resources allocated
	on behalf of the application or DLL. */
	if (WSACleanup() == SOCKET_ERROR)
		isSafe = false;

	return isSafe;
}

#include<io.h>
#include<stdio.h>
#include<winsock2.h>

#pragma comment(lib,"ws2_32.lib") //Winsock Library

// ToDo: put matching definitions and headings in to a header file and include it!
#define bool  int
#define true  1
#define false 0

#define CUSTOM_IP			"127.0.0.1"
#define CUSTOM_PORT			8888

bool startWinsock();
SOCKET getSocket();
bool bindSocket(SOCKET socket);
SOCKET waitAndGetConnection(SOCKET serverSocket);
bool sendDataToClient(SOCKET clientSocket, char *message);

int main(int argc, char *argv[])
{
	SOCKET serverSocket;
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


	printf("Listening for connections... ");
	SOCKET clientSocket = waitAndGetConnection(serverSocket);
	if (clientSocket == INVALID_SOCKET)
	{
		printf("FAILED : %d \n\n", WSAGetLastError());
		return 0;
	}
	printf("SOMEONE CONNECTED \n");


	printf("Attempting to send data... ");
	if (sendDataToClient(clientSocket, message) == false)
	{
		printf("FAILED : %d \n\n", WSAGetLastError());
		return 0;
	}
	printf("SUCCESS \n");

	// ToDo: close connections?
	// ToDo: WSACleanup?
	// ToDo: read the website
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

	// ToDo: find out what the fuck this means
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(CUSTOM_PORT);

	if (bind(socket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
		return false;
	else
		return true;
}

SOCKET waitAndGetConnection(SOCKET serverSocket)
{
	// Listen for connections
	// ToDo: find out the meaning behind the number 3 (backlog?)
	listen(serverSocket, 3);

	int c = sizeof(struct sockaddr_in);
	struct sockaddr_in client;
	return  accept(serverSocket, (struct sockaddr *)&client, &c);
}

bool sendDataToClient(SOCKET clientSocket, char *message)
{
	if (send(clientSocket, message, strlen(message), 0) < 0)
		return false;
	else
		return true;
}
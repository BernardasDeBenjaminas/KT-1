#define _WINSOCK_DEPRECATED_NO_WARNINGS // Dėl IP konvertavimo iš string (nėra kitų metodų)

#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>

// dependency to the Ws2_32.lib library
#pragma comment(lib, "Ws2_32.lib")

#define bool  int
#define false 0
#define true  1

#define REPLY_BUFFER_SIZE	1000
#define CUSTOM_IP			"127.0.0.1"
#define CUSTOM_PORT			8888

bool startWinsock();
SOCKET getSocket();
bool connectToServer(char *ip, int port, SOCKET clientSocket);
bool sendDataToServer(SOCKET clientSocket, char *messageToServer);
bool receiveReplyFromServer(SOCKET clientSocket, char *messageFromServer);
void joinGame(SOCKET clientSocket);
bool exitProgram(SOCKET clientSocket);

int main()
{
	SOCKET clientSocket;


	printf("Attempting to start Winsock... ");
	if (startWinsock() == false)
	{
		printf("FAILED : %d \n\n", WSAGetLastError());
		return 0;
	}
	printf("SUCCESS \n");


	printf("Attempting to create a socket... ");
	clientSocket = getSocket();
	if (socket == INVALID_SOCKET)
	{
		printf("FAILED : %d \n\n", WSAGetLastError());
		return 0;
	}
	printf("SUCCESS \n");


	printf("Attempting to connect to server... ");
	if (connectToServer(CUSTOM_IP, CUSTOM_PORT, clientSocket) == false)
	{
		printf("FAILED : %d \n\n", WSAGetLastError());
		return 0;
	}
	printf("SUCCESS \n\n");

	
	joinGame(clientSocket);


	printf("Attempting to exit and cleanup... ");
	if (exitProgram(clientSocket) == false)
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

bool connectToServer(char *ip, int port, SOCKET clientSocket)
{
	struct sockaddr_in server;
	// The inet_addr function converts a string containing an IPv4 dotted-decimal address into a proper address for the IN_ADDR structure.
	server.sin_addr.s_addr = inet_addr(ip);
	// For IPv4
	server.sin_family = AF_INET;
	// The htons function can be used to convert an IP port number in host byte order to the IP port number in network byte order.
	server.sin_port = htons(port);

	if (connect(clientSocket, (struct sockaddr *)&server, sizeof(server)) < 0)
		return false;
	else
		return true;
}

bool sendDataToServer(SOCKET clientSocket, char *messageToServer)
{
	printf("Trying to send");
	int size = 0;
	if ((size = send(clientSocket, messageToServer, strlen(messageToServer), 0)) < 0)
		return false;
	else
		return true;
}

bool receiveReplyFromServer(SOCKET clientSocket, char *messageFromServer)
{
	int sizeReceived, allSizeReceived = 0;
	int sizeOfBuffer = REPLY_BUFFER_SIZE;
	while (sizeOfBuffer > 0)
	{
		sizeReceived = recv(clientSocket, messageFromServer, REPLY_BUFFER_SIZE, 0);
		allSizeReceived += sizeReceived;

		if (sizeReceived < 1)
			return false;

		// We received the zero symbol but it wasn't added?
		for (size_t i = 0; i < strlen(messageFromServer); i++)
			if (messageFromServer[i] == '\0');
				break;

		messageFromServer += sizeReceived;
		sizeOfBuffer -= sizeReceived;
	}
	messageFromServer[allSizeReceived] = '\0';
	return true;
}

void joinGame(SOCKET clientSocket)
{
	char messageFromServer[REPLY_BUFFER_SIZE];
	char *guessingLetter = NULL;
	char letter;

	while (true)
	{
		system("@cls||clear");

		if (receiveReplyFromServer(clientSocket, messageFromServer) != false)
			printf("%s \n", messageFromServer);
		else
			break;

		printf("My letter of choice: ");
		do
			letter = getchar();
		while (isspace(letter));

		guessingLetter = &letter;
		guessingLetter[1] = '\0';

		if (sendDataToServer(clientSocket, guessingLetter) == false)
			printf("FAILED : %d \n\n", WSAGetLastError());

	}
}

bool exitProgram(SOCKET socket)
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
#define _WINSOCK_DEPRECATED_NO_WARNINGS // Dėl IP konvertavimo iš string (nėra kitų metodų)
#include <stdio.h>
#include <WinSock2.h>

// dependency to the Ws2_32.lib library
#pragma comment(lib, "Ws2_32.lib")

#define bool  int
#define false 0
#define true  1

#define REPLY_BUFFER_SIZE	100
#define CUSTOM_IP			"127.0.0.1"
#define CUSTOM_PORT			8888

bool startWinsock();
SOCKET getSocket();
bool connectToServer(char *ip, int port, SOCKET mySocket);
bool sendDataToServer(SOCKET socket, const char *message);
bool receiveReplyFromServer(SOCKET socket, char *reply);
bool exitProgram(SOCKET clientSocket);

int main()
{
	SOCKET socket;
	int statusCode = 0;
	char *message = "Please let me in";
	char reply[REPLY_BUFFER_SIZE];


	printf("Attempting to start Winsock... ");
	if (startWinsock() == false)
	{
		printf("FAILED : %d \n\n", WSAGetLastError());
		return 0;
	}
	printf("SUCCESS \n");


	printf("Attempting to create a socket... ");
	socket = getSocket();
	if (socket == INVALID_SOCKET)
	{
		printf("FAILED : %d \n\n", WSAGetLastError());
		return 0;
	}
	printf("SUCCESS \n");


	printf("Attempting to connect to server... ");
	if (connectToServer(CUSTOM_IP, CUSTOM_PORT, socket) == false)
	{
		printf("FAILED : %d \n\n", WSAGetLastError());
		return 0;
	}
	printf("SUCCESS \n");


	printf("Attempting to send data... ");
	if (sendDataToServer(socket, message) == false)
	{
		printf("FAILED : %d \n\n", WSAGetLastError());
		return 0;
	}
	printf("SUCCESS \n");


	printf("Attempting to receive reply... ");
	if (receiveReplyFromServer(socket, reply) == false)
	{
		printf("FAILED : %d \n\n", WSAGetLastError());
		return 0;
	}
	printf("SUCCESS \n");
	printf("REPLY: %s \n", reply);


	printf("Attempting to exit and cleanup... ");
	if (exitProgram(socket) == false)
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

bool connectToServer(char *ip, int port, SOCKET socket)
{
	struct sockaddr_in server;
	// The inet_addr function converts a string containing an IPv4 dotted-decimal address into a proper address for the IN_ADDR structure.
	server.sin_addr.s_addr = inet_addr(ip);
	// For IPv4
	server.sin_family = AF_INET;
	// The htons function can be used to convert an IP port number in host byte order to the IP port number in network byte order.
	server.sin_port = htons(port);

	if (connect(socket, (struct sockaddr *)&server, sizeof(server)) < 0)
		return false;
	else
		return true;
}

bool sendDataToServer(SOCKET socket, const char *message)
{
	if (send(socket, message, strlen(message), 0) < 0)
		return false;
	else
		return true;
}

bool receiveReplyFromServer(SOCKET socket, char *reply)
{
	int replyStatus;
	if ((replyStatus = recv(socket, reply, REPLY_BUFFER_SIZE, 0)) == SOCKET_ERROR)
		return false;
	else
	{
		reply[replyStatus] = '\0';
		return true;
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
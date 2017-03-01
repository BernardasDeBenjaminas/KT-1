#include<stdio.h>
#include<stdlib.h>
#include<winsock2.h>

#pragma comment(lib,"ws2_32.lib") //Winsock Library

// ToDo: put matching definitions and headings in to a header file and include it!
#define bool	int
#define true	1
#define false	0

#define CUSTOM_IP			"127.0.0.1"
#define CUSTOM_PORT			8888
#define REPLY_BUFFER_SIZE	1000
#define MESSAGE_BUFFER_SIZE 1000

bool startWinsock();
SOCKET getSocket();
bool bindSocket(SOCKET socket);
bool listenForConnections(SOCKET serverSocket, int queueSize);
bool sendDataToClient(SOCKET clientSocket, char *messageToClient);
bool getDataFromClient(SOCKET clientSocket, char *messageFromClient);
SOCKET acceptConnection(SOCKET serverSocket);
bool startGame(SOCKET serverSocket);
bool sendIntroMessage(SOCKET clientSocket, int lengthOfGuessingWord, char *guessWord, int sizeOfGuessWord);
int checkPlayersGuess(char *messageFromClient, char *wordToGuess, char *guessWord, char *failedGuesses, int *livesLeft, int *lettersGuessed);
bool sendResponse(SOCKET clientSocket, bool isGuessCorrect, char *messageToClient, char *failedGuesses, char *guessWord, char *livesLeft);
bool lostTheGame(SOCKET clientSocket, char *messageToClient);
bool wonTheGame(SOCKET clientSocket, char *messageToClient);
bool exitProgram(SOCKET serverSocket);

int main(int argc, char *argv[])
{
	SOCKET serverSocket;


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

	
	startGame(serverSocket);
	

	/*printf("Waiting for connections... ");
	if ((clientSocket = acceptConnection(serverSocket)) == false)
	{
		printf("FAILED : %d \n\n", WSAGetLastError());
		return 0;
	}
	printf("CONNECTED \n");


	printf("Attempting to send data... ");
	if (sendDataToClient(clientSocket, messageToClient) == false)
	{
		printf("FAILED : %d \n\n", WSAGetLastError());
		return 0;
	}
	printf("SUCCESS \n");


	printf("Attempting to receive data... ");
	if (receiveReplyFromClient(clientSocket, messageFromClient) == false)
	{
		printf("FAILED : %d \n\n", WSAGetLastError());
		return 0;
	}
	printf("SUCCESS \n");
	printf("REPLY : %s \n", messageFromClient);*/


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

bool sendDataToClient(SOCKET clientSocket, char *messageToClient)
{
	int sizeSent;
	int lengthOfMessage = strlen(messageToClient);
	char *ptrToMessage = messageToClient;
	while (lengthOfMessage > 0)
	{
		sizeSent = send(clientSocket, messageToClient, strlen(messageToClient), 0);
		if (sizeSent < 1)
			return false;

		ptrToMessage += sizeSent;
		lengthOfMessage -= sizeSent;
	}
	return true;
}

bool getDataFromClient(SOCKET clientSocket, char *messageFromClient)
{
	int replyStatus;
	if ((replyStatus = recv(clientSocket, messageFromClient, strlen(messageFromClient), 0)) == SOCKET_ERROR)
		return false;
	else
	{
		messageFromClient[replyStatus] = '\0';
		return true;
	}
}

bool startGame(SOCKET serverSocket)
{
	int livesLeft = 5;
	int lettersGuessed = 0;
	char guessWord[500];		// Player's word (filled with questions marks and attempts)
	guessWord[0] = '\0';
	char failedGuesses[100] = "Your previous failed guesses: \0";			// Player's made guesses
	//failedGuesses[0] = '\0';
	

	bool isGuessCorrect = false;
	SOCKET clientSocket;
	char messageFromClient[REPLY_BUFFER_SIZE];
	char messageToClient[1000];
	messageToClient[0] = '\0';

	char *guessableWords[] = { "KANJONAS", "TARPEKLIS", "KATAPULTA", "INDELIS" };
	int choice = rand() % 3;
	char *wordToGuess = guessableWords[choice];


	printf("\n --- GAME STARTED --- \n");
	while ((clientSocket = acceptConnection(serverSocket)) != INVALID_SOCKET)
	{
		printf("\n -- PLAYER CONNECTED -- \n");

		if (sendIntroMessage(clientSocket, strlen(wordToGuess), guessWord, sizeof(guessWord)) == false)
			printf("There was a problem sending the intro message : %d\n\n", WSAGetLastError());

		while (true)
		{
			if (getDataFromClient(clientSocket, messageFromClient) != false)
			{
				isGuessCorrect = checkPlayersGuess(messageFromClient, wordToGuess, guessWord, failedGuesses, &livesLeft, &lettersGuessed);

				if (lettersGuessed == strlen(wordToGuess))
				{
					// Set something
					wonTheGame(clientSocket, messageToClient);
					// Clean up
				}

				else if (livesLeft > 0)
					sendResponse(clientSocket, isGuessCorrect, messageToClient, failedGuesses, guessWord, &livesLeft);

				else
				{
					// Set something
					lostTheGame(clientSocket, messageToClient);
					// Clean up
				}
			}
			else
				break;
		}
		printf(" -- PLAYER DISCONNECTED -- \n");
	}
	return true;
}

bool sendIntroMessage(SOCKET clientSocket, int lengthOfGuessingWord, char *guessWord, int sizeOfGuessWord)
{
	if (lengthOfGuessingWord < 6 || lengthOfGuessingWord > 10)
		return false;

	char introMessage[500];
	introMessage[0] = '\0';

	char *numberOfLetters = NULL,
		*msg1 = "\n\t\t=== WELCOME TO HANGMAN === \n\n",
		*msg2 = "The game has begun. \nA word has been assigned to you. \nHave fun. \nGuess a letter now: ";
	
	switch (lengthOfGuessingWord)
	{
		case 7:
			numberOfLetters = "??????? (7 letters)\n";
			strcat_s(guessWord, sizeOfGuessWord, numberOfLetters);
			break;
		case 8:
			numberOfLetters = "???????? (8 letters)\n";
			strcat_s(guessWord, sizeOfGuessWord, numberOfLetters);
			break;
		case 9:
			numberOfLetters = "????????? (9 letters)\n";
			strcat_s(guessWord, sizeOfGuessWord, numberOfLetters);
			break;
	}
	strcat_s(introMessage, sizeof(introMessage), msg1);
	strcat_s(introMessage, sizeof(introMessage), msg2);
	strcat_s(introMessage, sizeof(introMessage), numberOfLetters);

	if (sendDataToClient(clientSocket, introMessage) == false)
	{
		return false;
	}
	else
	{
		return true;
	}
}

int checkPlayersGuess(char *messageFromClient, char *wordToGuess, char *guessWord, char *failedGuesses, int *livesLeft, int *lettersGuessed)
{
	bool isRight = false;
	char guess[1] = { messageFromClient[0] };
	guess[0] = toupper(guess[0]);

	for (size_t i = 0; i < strlen(wordToGuess); i++)
	{
		if (wordToGuess[i] == guess[0])
		{
			isRight = true;
			guessWord[i] = guess[0];
			*lettersGuessed += 1;
		}
	}

	if (isRight == false)
	{
		*livesLeft -= 1;
		int stringLength = strlen(failedGuesses);
		failedGuesses[stringLength] = guess[0];
		failedGuesses[stringLength + 1] = ' ';
		//failedGuesses[stringLength + 2] = '\0';
		return false;
	}
	else
		return true;
}

bool sendResponse(SOCKET clientSocket, bool isGuessCorrect, char *messageToClient, char *failedGuesses, char *guessWord, char *livesLeft)
{
	if (*livesLeft == 0)
	{
		strcat_s(messageToClient, MESSAGE_BUFFER_SIZE, "\n\nOUT OF LIVES! Better luck next time..\n\n");
		sendDataToClient(clientSocket, messageToClient);
		messageToClient[0] = '\0';	// Empty the array

		return false;
	}

	strcat_s(messageToClient, MESSAGE_BUFFER_SIZE, "---------------");

	if (isGuessCorrect == true)
		strcat_s(messageToClient, MESSAGE_BUFFER_SIZE, "\nCorrect!\n");
	else
		strcat_s(messageToClient, MESSAGE_BUFFER_SIZE, "\nWrong!\n");

	strcat_s(messageToClient, MESSAGE_BUFFER_SIZE, failedGuesses);
	strcat_s(messageToClient, MESSAGE_BUFFER_SIZE, "\n");

	strcat_s(messageToClient, MESSAGE_BUFFER_SIZE, "Number of lives left: ");
	char livesLeftString[15];
	sprintf_s(livesLeftString, sizeof(livesLeftString),"%d", *livesLeft);
	strcat_s(messageToClient, MESSAGE_BUFFER_SIZE, livesLeftString);

	strcat_s(messageToClient, MESSAGE_BUFFER_SIZE, "\n");
	strcat_s(messageToClient, MESSAGE_BUFFER_SIZE, "\n");
	strcat_s(messageToClient, MESSAGE_BUFFER_SIZE, guessWord);
	strcat_s(messageToClient, MESSAGE_BUFFER_SIZE, "---------------");

	messageToClient[strlen(messageToClient)] = '\0';
	sendDataToClient(clientSocket, messageToClient);
	messageToClient[0] = '\0';	// Empty the array

	return true;
}

bool lostTheGame(SOCKET clientSocket, char *messageToClient)
{
	strcat_s(messageToClient, MESSAGE_BUFFER_SIZE, "\n\nOUT OF LIVES! Better luck next time..\n\n");

	if (sendDataToClient(clientSocket, messageToClient) == false)
		return false;

	messageToClient[0] = '\0';	// Empty the array
	return true;
}

bool wonTheGame(SOCKET clientSocket, char *messageToClient)
{
	strcat_s(messageToClient, MESSAGE_BUFFER_SIZE, "\n\nCONGRATULATIONS! You are the winner!\n\n");

	if (sendDataToClient(clientSocket, messageToClient) == false)
		return false;

	messageToClient[0] = '\0';	// Empty the array
	return true;
}

bool exitProgram(SOCKET serverSocket)
{
	bool isSafe = true;
	if (closesocket(serverSocket) == SOCKET_ERROR)
		isSafe = false;
	/* To deregister itself from a Windows Sockets implementation
	and allow the implementation to free any resources allocated
	on behalf of the application or DLL. */
	if (WSACleanup() == SOCKET_ERROR)
		isSafe = false;

	return isSafe;
}

/*******************************************************************************
 * A multi-threaded TCP/IP server application with non-blocking sockets
 ******************************************************************************/

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "Windows.h"		// Entire Win32 API...
 // #include "winsock2.h"	// ...or Winsock alone
#include "ws2tcpip.h"		// getaddrinfo()

// Tell the Visual Studio linker to include the following library in linking.
// Alternatively, we could add this file to the linker command-line parameters,
// but including it in the source code simplifies the configuration.
#pragma comment(lib, "ws2_32.lib")

#include <iostream>			// cout, cerr
#include <string>			// string

#include "taskqueue.h"

bool execute(SOCKET clientSocket);
void disconnect(SOCKET& listenerSocket);
enum CMDID {
	UNKNOWN = (unsigned char)0x0,
	REQ_QUIT = (unsigned char)0x1,
	REQ_ECHO = (unsigned char)0x2,
	RSP_ECHO = (unsigned char)0x3,
	REQ_LISTUSERS = (unsigned char)0x4,
	RSP_LISTUSERS = (unsigned char)0x5,
	CMD_TEST = (unsigned char)0x20,
	ECHO_ERROR = (unsigned char)0x30
};

int main()
{
	

	// -------------------------------------------------------------------------
	// Start up Winsock, asking for version 2.2.
	//
	// WSAStartup()
	// -------------------------------------------------------------------------

	// This object holds the information about the version of Winsock that we
	// are using, which is not necessarily the version that we requested.
	WSADATA wsaData{};

	// Initialize Winsock. You must call WSACleanup when you are finished.
	// As this function uses a reference counter, for each call to WSAStartup,
	// you must call WSACleanup or suffer memory issues.
	int errorCode = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (errorCode != NO_ERROR)
	{
		std::cerr << "WSAStartup() failed." << std::endl;
		return errorCode;
	}

	uint16_t port;
	std::cout << "Server Port Number: ";
	std::cin >> port;
	std::cout << std::endl;
	std::string portString = std::to_string(port);


	// -------------------------------------------------------------------------
	// Resolve own host name into IP addresses (in a singly-linked list).
	//
	// getaddrinfo()
	// -------------------------------------------------------------------------
	// Get local hostname
	char hostName[256];
	errorCode = gethostname(hostName, sizeof(hostName));
	if (errorCode)
	{
		std::cerr << "gethostname() failed with error: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return errorCode;
	}
	// Object hints indicates which protocols to use to fill in the info.
	addrinfo hints{};
	SecureZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;			// IPv4
	// For UDP use SOCK_DGRAM instead of SOCK_STREAM.
	hints.ai_socktype = SOCK_STREAM;	// Reliable delivery
	// Could be 0 for autodetect, but reliable delivery over IPv4 is always TCP.
	hints.ai_protocol = IPPROTO_TCP;	// TCP
	// Create a passive socket that is suitable for bind() and listen().
	hints.ai_flags = AI_PASSIVE;

	addrinfo* info = nullptr;
	errorCode = getaddrinfo(hostName, portString.c_str(), &hints, &info);
	if ((errorCode) || (info == nullptr))
	{
		std::cerr << "getaddrinfo() failed." << std::endl;
		WSACleanup();
		return errorCode;
	}
	// print server IP address and port number
	sockaddr_in* addr_in = reinterpret_cast<sockaddr_in*>(info->ai_addr);
	char ipStr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(addr_in->sin_addr), ipStr, sizeof(ipStr));
	std::cout << "Server IP Address: " << ipStr << std::endl;
	std::cout << "Server Port number: " << port << std::endl;

	// -------------------------------------------------------------------------
	// Create a socket and bind it to own network interface controller.
	//
	// socket()
	// bind()
	// -------------------------------------------------------------------------

	SOCKET listenerSocket = socket(
		hints.ai_family,
		hints.ai_socktype,
		hints.ai_protocol);
	if (listenerSocket == INVALID_SOCKET)
	{
		std::cerr << "socket() failed." << std::endl;
		freeaddrinfo(info);
		WSACleanup();
		return 1;
	}

	errorCode = bind(
		listenerSocket,
		info->ai_addr,
		static_cast<int>(info->ai_addrlen));
	if (errorCode != NO_ERROR)
	{
		std::cerr << "bind() failed." << std::endl;
		closesocket(listenerSocket);
		listenerSocket = INVALID_SOCKET;
	}

	freeaddrinfo(info);

	if (listenerSocket == INVALID_SOCKET)
	{
		std::cerr << "bind() failed." << std::endl;
		WSACleanup();
		return 2;
	}


	// -------------------------------------------------------------------------
	// Set a socket in a listening mode and accept 1 incoming client.
	//
	// listen()
	// accept()
	// -------------------------------------------------------------------------

	errorCode = listen(listenerSocket, SOMAXCONN);
	if (errorCode != NO_ERROR)
	{
		std::cerr << "listen() failed." << std::endl;
		closesocket(listenerSocket);
		WSACleanup();
		return 3;
	}

	{
		const auto onDisconnect = [&]() { disconnect(listenerSocket); };
		auto tq = TaskQueue<SOCKET, decltype(execute), decltype(onDisconnect)>{10, 20, execute, onDisconnect};
		while (listenerSocket != INVALID_SOCKET)
		{
			sockaddr clientAddress{};
			SecureZeroMemory(&clientAddress, sizeof(clientAddress));
			int clientAddressSize = sizeof(clientAddress);
			SOCKET clientSocket = accept(
				listenerSocket,
				&clientAddress,
				&clientAddressSize);
			if (clientSocket == INVALID_SOCKET)
			{
				break;
			}
			char client_ipStr[INET_ADDRSTRLEN];
			sockaddr_in* sockaddr_ipv4 = reinterpret_cast<sockaddr_in*>(&clientAddress);
			inet_ntop(AF_INET, &sockaddr_ipv4->sin_addr, client_ipStr, sizeof(client_ipStr));
			std::lock_guard<std::mutex> usersLock{ _stdoutMutex };
			std::cout << std::endl;
			std::cout << "Client IP Address: " << client_ipStr << std::endl;
			std::cout << "Client Port number: " << ntohs(sockaddr_ipv4->sin_port) << std::endl;
			tq.produce(clientSocket);
		}
	}

	// -------------------------------------------------------------------------
	// Clean-up after Winsock.
	//
	// WSACleanup()
	// -------------------------------------------------------------------------

	WSACleanup();
}

void disconnect(SOCKET& listenerSocket)
{
	if (listenerSocket != INVALID_SOCKET)
	{
		shutdown(listenerSocket, SD_BOTH);
		closesocket(listenerSocket);
		listenerSocket = INVALID_SOCKET;
	}
}

bool execute(SOCKET clientSocket)
{

	// -------------------------------------------------------------------------
	// Receive some text and send it back.
	//
	// recv()
	// send()
	// -------------------------------------------------------------------------

	constexpr size_t BUFFER_SIZE = 1000;
	char buffer[BUFFER_SIZE];
	bool stay = true;

	// Enable non-blocking I/O on a socket.
	u_long enable = 1;
	ioctlsocket(clientSocket, FIONBIO, &enable);

	while (true)
	{
		const int bytesReceived = recv(
			clientSocket,
			buffer,
			BUFFER_SIZE - 1,
			0);
		if (bytesReceived == SOCKET_ERROR)
		{
			size_t errorCode = WSAGetLastError();
			if (errorCode == WSAEWOULDBLOCK)
			{
				// A non-blocking call returned no data; sleep and try again.
				using namespace std::chrono_literals;
				std::this_thread::sleep_for(200ms);
				
				/*std::lock_guard<std::mutex> usersLock{ _stdoutMutex };
				std::cerr << "trying again..." << std::endl;*/
				continue;
			}
			std::lock_guard<std::mutex> usersLock{ _stdoutMutex };		
			std::cerr << "recv() failed." << std::endl;
			break;
		}
		if (bytesReceived == 0)
		{
			std::lock_guard<std::mutex> usersLock{ _stdoutMutex };
			std::cerr << "Graceful shutdown." << std::endl;
			break;
		}

		buffer[bytesReceived] = '\0';
		std::string text(buffer, bytesReceived);
		//std::lock_guard<std::mutex> usersLock{ _stdoutMutex };
		//std::cout
		//	<< "Text received:  " << text << "\n"
		//	<< "Bytes received: " << bytesReceived << "\n"
		//	<< std::endl;




		unsigned long hostLength{};
		unsigned char commandId = buffer[0];
		unsigned long des_ip{};
		unsigned short des_port{};
		//quit
		//if (commandId == 0x01)
		//{
		//	std::cout << "Quit command received. Bye from " << client_ipStr << ":" << ntohs(sockaddr_ipv4->sin_port) << std::endl;
		//	closesocket(clientSocket);
		//	awaitingClient = true;
		//	continue;
		//}

		//echo
		if (commandId == REQ_ECHO)
		{
			//get text message length
			unsigned long netOrder;
			memcpy(&netOrder, buffer + 7, 4);
			hostLength = ntohl(netOrder);

			memcpy(&netOrder, buffer + 1, 4);
			in_addr addr;
			addr.s_addr = netOrder;
			char des_ipStr[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &addr, des_ipStr, INET_ADDRSTRLEN);

			memcpy(&netOrder, buffer + 5, 2);
			des_port = ntohs(netOrder);

			std::lock_guard<std::mutex> usersLock{ _stdoutMutex };
			std::cout << des_ipStr <<" "<< des_port << std::endl;
			
			//if message length is invalid
			if (static_cast<int>(hostLength) <= 0)
			{
				std::lock_guard<std::mutex> usersLock{ _stdoutMutex };
				std::cout << "Error invalid message length" << std::endl;
				closesocket(clientSocket);
				break;
			}
			////if message is longer than buffer size
			//if (BUFFER_SIZE < hostLength + 5)
			//{
			//	std::string message(buffer + 5, bytesReceived - 5);
			//	std::cout << "[Echo Msg received] " << message;
			//	const int bytesSent = send(clientSocket, buffer, bytesReceived, 0);
			//	if (bytesSent == SOCKET_ERROR)
			//	{
			//		closesocket(clientSocket);
			//		awaitingClient = true;
			//		continue;
			//	}
			//	unsigned long totalBytesReceived = bytesReceived - 5;
			//	while (totalBytesReceived < hostLength)
			//		//loop until all bytes are received
			//	{
			//		char remainingBytes[BUFFER_SIZE];
			//		const int addbytesReceived = recv(clientSocket, remainingBytes, BUFFER_SIZE, 0);
			//		if (addbytesReceived == 0)
			//		{
			//			closesocket(clientSocket);
			//			awaitingClient = true;
			//			break;
			//		}
			//		if (addbytesReceived == SOCKET_ERROR)
			//		{
			//			closesocket(clientSocket);
			//			awaitingClient = true;
			//			break;
			//		}
			//		std::string message(remainingBytes, addbytesReceived);
			//		std::cout << message;
			//		totalBytesReceived += addbytesReceived;

			//		const int bytesSent = send(clientSocket, remainingBytes, addbytesReceived, 0);
			//		if (bytesSent == SOCKET_ERROR)
			//		{
			//			closesocket(clientSocket);
			//			awaitingClient = true;
			//			break;
			//		}
			//	}
			//	std::cout << std::endl;
			//}
			//else //if message is shorter than buffer size
			//{
				std::string message(buffer + 11, bytesReceived - 11);
				std::cout << "[Echo Msg received] " << message << std::endl;

				const int bytesSent = send(clientSocket, buffer, bytesReceived, 0);
				if (bytesSent == SOCKET_ERROR)
				{
					closesocket(clientSocket);
					break;
				}
			//}
		}
		else
		{
			std::lock_guard<std::mutex> usersLock{ _stdoutMutex };
			std::cout << "Error invalid command" << std::endl;
			closesocket(clientSocket);
			break;
		}




		if (text == "*")
		{
			std::lock_guard<std::mutex> usersLock{ _stdoutMutex };
			std::cout << "Requested to close the server!" << std::endl;
			stay = false;
			break;
		}

		//const int bytesSent = send(clientSocket, buffer, bytesReceived, 0);
		//if (bytesSent == SOCKET_ERROR)
		//{
		//	std::lock_guard<std::mutex> usersLock{ _stdoutMutex };
		//	std::cerr << "send() failed." << std::endl;
		//	break;
		//}
	}


	// -------------------------------------------------------------------------
	// Shut down and close sockets.
	//
	// shutdown()
	// closesocket()
	// -------------------------------------------------------------------------

	shutdown(clientSocket, SD_BOTH);
	closesocket(clientSocket);
	return stay;
}

/* Start Header
*****************************************************************/
/*!
\file echoserver.cpp
\author Wei Jingsong
\par jingsong.wei@digipen.edu
\date Jan 23 2024
\brief Basic Winsock Programming using TCP sockets. This is the
server code.

Copyright (C) 20xx DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
/*******************************************************************************
 * A simple TCP/IP server application
 ******************************************************************************/

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "Windows.h"  // Entire Win32 API...
					  // #include "winsock2.h"	// ...or Winsock alone
#include "ws2tcpip.h" // getaddrinfo()
#include <iphlpapi.h>
// Tell the Visual Studio linker to include the following library in linking.
// Alternatively, we could add this file to the linker command-line parameters,
// but including it in the source code simplifies the configuration.
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")
#include <iostream> // cout, cerr
#include <string>	// string
#include <vector>
#include <limits>
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

	// Object hints indicates which protocols to use to fill in the info.
	addrinfo hints{};
	SecureZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET; // IPv4
	// For UDP use SOCK_DGRAM instead of SOCK_STREAM.
	hints.ai_socktype = SOCK_STREAM; // Reliable delivery
	// Could be 0 for autodetect, but reliable delivery over IPv4 is always TCP.
	hints.ai_protocol = IPPROTO_TCP; // TCP
	// Create a passive socket that is suitable for bind() and listen().
	hints.ai_flags = AI_PASSIVE;

	addrinfo *info = nullptr;
	errorCode = getaddrinfo(nullptr, portString.c_str(), &hints, &info);
	if ((errorCode) || (info == nullptr))
	{
		std::cerr << "getaddrinfo() failed." << std::endl;
		WSACleanup();
		return errorCode;
	}

	// char hostName[256];
	// gethostname(hostName, 256);
	// std::cout << "Server IP Address: " << hostName << std::endl;

	// print server IP address and port number
	ULONG outBufLen = sizeof(IP_ADAPTER_ADDRESSES);
	PIP_ADAPTER_ADDRESSES pAddresses = (IP_ADAPTER_ADDRESSES *)malloc(outBufLen);
	if (GetAdaptersAddresses(AF_INET, 0, NULL, pAddresses, &outBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		free(pAddresses);
		pAddresses = (IP_ADAPTER_ADDRESSES *)malloc(outBufLen);
	}
	int c = 0;
	DWORD dwRetVal = GetAdaptersAddresses(AF_INET, 0, NULL, pAddresses, &outBufLen);
	if (dwRetVal == NO_ERROR)
	{
		for (PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses; pCurrAddresses != NULL; pCurrAddresses = pCurrAddresses->Next)
		{

			for (PIP_ADAPTER_UNICAST_ADDRESS pUnicast = pCurrAddresses->FirstUnicastAddress; pUnicast != NULL; pUnicast = pUnicast->Next)
			{
				if (pUnicast->Address.lpSockaddr->sa_family == AF_INET && c == 2)
				{
					char ipStr[INET_ADDRSTRLEN];
					sockaddr_in *sockaddr_ipv4 = reinterpret_cast<sockaddr_in *>(pUnicast->Address.lpSockaddr);
					inet_ntop(AF_INET, &sockaddr_ipv4->sin_addr, ipStr, sizeof(ipStr));
					std::cout << "Server IP Address: " << ipStr << std::endl;
				}
				c++;
			}
		}
	}
	else
	{
		std::cout << "GetAdaptersAddresses failed with error: " << dwRetVal << std::endl;
	}
	if (pAddresses)
	{
		free(pAddresses);
	}
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
	// -------------------------------------------------------------------------
	// Receive some text and send it back.
	//
	// recv()
	// send()
	// -------------------------------------------------------------------------
	
	SOCKET clientSocket;
	bool awaitingClient = true;
	constexpr size_t BUFFER_SIZE = 1000;
	char buffer[BUFFER_SIZE];
	sockaddr clientAddress{};
	SecureZeroMemory(&clientAddress, sizeof(clientAddress));
	int clientAddressSize = sizeof(clientAddress);
	char ipStr[INET_ADDRSTRLEN]{};
	sockaddr_in *sockaddr_ipv4{};
	while (true)
	{
		if (awaitingClient)
		{
			clientSocket = accept(
				listenerSocket,
				&clientAddress,
				&clientAddressSize);
			if (clientSocket == INVALID_SOCKET)
			{
				std::cerr << "accept() failed." << std::endl;
				closesocket(listenerSocket);
				WSACleanup();
				return 4;
			}
			awaitingClient = false;
			// print client IP address and port number
			sockaddr_ipv4 = reinterpret_cast<sockaddr_in *>(&clientAddress);
			inet_ntop(AF_INET, &sockaddr_ipv4->sin_addr, ipStr, sizeof(ipStr));
			std::cout << std::endl;
			std::cout << "Client IP Address: " << ipStr << std::endl;
			std::cout << "Client Port number: " << ntohs(sockaddr_ipv4->sin_port) << std::endl;
		}

		const int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);

		if (bytesReceived == 0)
		{
			// std::cout << "client " << ipStr << ":" << ntohs(sockaddr_ipv4->sin_port) << " is closing" << std::endl;
			closesocket(clientSocket);
			awaitingClient = true;
			continue;
		}
		else if (bytesReceived == SOCKET_ERROR)
		{
			closesocket(clientSocket);
			awaitingClient = true;
			continue;
		}
		else
		{
			unsigned long hostLength;
			buffer[bytesReceived] = '\0';
			unsigned char commandId = buffer[0];
			if (commandId == 0x01)
			{
				std::cout << "Quit command received. Bye from " << ipStr << ":" << ntohs(sockaddr_ipv4->sin_port) << std::endl;
				closesocket(clientSocket);
				awaitingClient = true;
				continue;
			}
			else if (commandId == 0x02)
			{
				unsigned long netLength;
				memcpy(&netLength, buffer + 1, 4); // Offset by 1 to skip command ID
				hostLength = ntohl(netLength);
				if (hostLength <= 0)
				{
					std::cout << "Error invalid message length" << std::endl;
					closesocket(clientSocket);
					awaitingClient = true;
					continue;
				}
				char *message = new char[hostLength + 1];
				if (BUFFER_SIZE - 1 < hostLength + 5)
				{
					memcpy(message, buffer + 5, bytesReceived);
					message[bytesReceived - 5] = '\0';
					unsigned long totalBytesReceived = bytesReceived - 5;
					while (totalBytesReceived < hostLength)
					{
						char remainingBytes[BUFFER_SIZE];
						const int addbytesReceived = recv(clientSocket, remainingBytes, BUFFER_SIZE - 1, 0);
						remainingBytes[addbytesReceived] = '\0';
						memcpy(message + strlen(message), remainingBytes, addbytesReceived + 1);
						totalBytesReceived += addbytesReceived;
					}
					std::cout << "[Echo Msg received] " << message << " " << hostLength << std::endl;
				}
				else
				{
					memcpy(message, buffer + 5, hostLength); // Offset by 5 to skip command ID and message length
					message[hostLength] = '\0';
					std::cout << "[Echo Msg received] " << message << " " << hostLength << std::endl;
				}
				char *sendMessage = new char[hostLength + 6];
				memcpy(sendMessage, buffer, 5);
				memcpy(sendMessage + 5, message, hostLength);
				const int bytesSent = send(clientSocket, sendMessage, hostLength + 5, 0);
				if (bytesSent == SOCKET_ERROR)
				{
					closesocket(clientSocket);
					awaitingClient = true;
					continue;
				}
				delete[] sendMessage;
				delete[] message;
			}
			else
			{
				std::cout << "Error invalid command" << std::endl;
				closesocket(clientSocket);
				awaitingClient = true;
				continue;
			}
		}
	}

	// -------------------------------------------------------------------------
	// Shut down and close sockets.
	//
	// shutdown()
	// closesocket()
	// -------------------------------------------------------------------------

	shutdown(clientSocket, SD_BOTH);
	shutdown(listenerSocket, SD_BOTH);

	closesocket(clientSocket);
	closesocket(listenerSocket);

	// -------------------------------------------------------------------------
	// Clean-up after Winsock.
	//
	// WSACleanup()
	// -------------------------------------------------------------------------

	WSACleanup();
}

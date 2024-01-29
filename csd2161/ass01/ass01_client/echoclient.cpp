/* Start Header
*****************************************************************/
/*!
\file echoclient.cpp
\author Wei Jingsong
\par jingsong.wei@digipen.edu
\date Jan 23 2024
\brief Basic Winsock Programming using TCP sockets. This is the
client code.

Copyright (C) 20xx DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
/*******************************************************************************
 * A simple TCP/IP client application
 ******************************************************************************/

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "Windows.h"  // Entire Win32 API...
					  // #include "winsock2.h"	// ...or Winsock alone
#include "ws2tcpip.h" // getaddrinfo()

// Tell the Visual Studio linker to include the following library in linking.
// Alternatively, we could add this file to the linker command-line parameters,
// but including it in the source code simplifies the configuration.
#pragma comment(lib, "ws2_32.lib")

#include <iostream> // cout, cerr
#include <string>	// string
#include <vector>	// vector
#include <limits>
#include <sstream>
// This function converts a string of hexadecimal digits into a string
std::string hexToString(const std::string &hex)
{
	std::string str;
	for (size_t i = 0; i < hex.length(); i += 2)
	{
		std::string byteString = hex.substr(i, 2);
		char byte = static_cast<char>(std::stoul(byteString, nullptr, 16));
		str.push_back(byte);
	}
	return str;
}

// This program requires one extra command-line parameter: a server hostname.
int main(int argc)
{
	std::string portString;
	std::string host;
	std::cout << "Server IP Address: ";
	std::cin >> host;
	std::cout << std::endl;
	std::cout << "Server Port Number: ";
	std::cin >> portString;
	std::cout << std::endl;
	// -------------------------------------------------------------------------
	// Start up Winsock, asking for version 2.2.
	//
	// WSAStartup()
	// -------------------------------------------------------------------------

	// This object holds the information about the version of Winsock that we
	// are using, which is not necessarily the version that we requested.
	WSADATA wsaData{};
	SecureZeroMemory(&wsaData, sizeof(wsaData));

	// Initialize Winsock. You must call WSACleanup when you are finished.
	// As this function uses a reference counter, for each call to WSAStartup,
	// you must call WSACleanup or suffer memory issues.
	int errorCode = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (errorCode != NO_ERROR)
	{
		std::cerr << "WSAStartup() failed." << std::endl;
		return errorCode;
	}

	// -------------------------------------------------------------------------
	// Resolve a server host name into IP addresses (in a singly-linked list).
	//
	// getaddrinfo()
	// -------------------------------------------------------------------------

	// Object hints indicates which protocols to use to fill in the info.
	addrinfo hints{};
	SecureZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;		 // IPv4
	hints.ai_socktype = SOCK_STREAM; // Reliable delivery
	// Could be 0 to autodetect, but reliable delivery over IPv4 is always TCP.
	hints.ai_protocol = IPPROTO_TCP; // TCP

	addrinfo *info = nullptr;
	errorCode = getaddrinfo(host.c_str(), portString.c_str(), &hints, &info);
	if ((errorCode) || (info == nullptr))
	{
		WSACleanup();
		return errorCode;
	}

	// -------------------------------------------------------------------------
	// Create a socket and attempt to connect to the first resolved address.
	//
	// socket()
	// connect()
	// -------------------------------------------------------------------------

	SOCKET clientSocket = socket(
		info->ai_family,
		info->ai_socktype,
		info->ai_protocol);
	if (clientSocket == INVALID_SOCKET)
	{
		freeaddrinfo(info);
		WSACleanup();
		return 2;
	}

	errorCode = connect(
		clientSocket,
		info->ai_addr,
		static_cast<int>(info->ai_addrlen));
	if (errorCode == SOCKET_ERROR)
	{
		std::cerr << "Unable to connect to server!" << std::endl;
		freeaddrinfo(info);
		closesocket(clientSocket);
		WSACleanup();
		return 3;
	}

	// -------------------------------------------------------------------------
	// Send some text.
	//
	// send()
	// -------------------------------------------------------------------------

	constexpr size_t BUFFER_SIZE = 1000;
	unsigned long commandID{};
	unsigned long textLength{};
	// ignore newline char left by key in ip and port
	std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
	while (true)
	{
		std::string input;
		std::getline(std::cin, input);

		if (input == "")
		{
			if (errorCode == SOCKET_ERROR)
			{
				closesocket(clientSocket);
				break;
			}
			closesocket(clientSocket);
			break;
		}
		char message[BUFFER_SIZE];
		// if quit
		if (input[0] == '/' && input[1] == 'q')
		{
			commandID = 1; // QUIT command
			message[0] = static_cast<char>(commandID);
			int bytesSent = send(
				clientSocket,
				message,
				static_cast<int>(1),
				0);
			if (errorCode == SOCKET_ERROR)
			{
				closesocket(clientSocket);
				break;
			}
			closesocket(clientSocket);
			break;
		}
		// if test
		else if (input[0] == '/' && input[1] == 't')
		{
			input.erase(0, 3);
			try
			{
				commandID = std::stoul(input.substr(0, 2));
			}
			catch (const std::out_of_range &e)
			{
				commandID = 0;
				
			}
			catch (const std::exception &e)
			{
				commandID = 0;
			}

			input.erase(0, 2);
			try
			{
				textLength = std::stoul(input.substr(0, 8));
			}
			catch (const std::out_of_range &e)
			{
				textLength = 0;
			}
			catch (const std::exception &e)
			{
				textLength = 0;
			}
			input.erase(0, 8);
			std::string text = hexToString(input);
			input = text;
			textLength = static_cast<unsigned long>(text.length());
		}
		else // if normal message
		{
			commandID = 2; // ECHO command
			textLength = static_cast<unsigned long>(input.length());
		}
		message[0] = static_cast<char>(commandID);
		unsigned long netLength = htonl(textLength);
		memcpy(message + 1, &netLength, 4);
		if (textLength <= BUFFER_SIZE - 5)
		{
			memcpy(message + 5, input.c_str(), textLength);
			int bytesSent = send(
				clientSocket,
				message,
				static_cast<int>(textLength + 5),
				0);
			if (bytesSent == SOCKET_ERROR)
			{
				if (errorCode == SOCKET_ERROR)
				{
					closesocket(clientSocket);
					break;
				}
				closesocket(clientSocket);
				break;
			}
		}
		else
		{
			memcpy(message + 5, input.c_str(), BUFFER_SIZE - 5);
			int bytesSent = send(
				clientSocket,
				message,
				static_cast<int>(BUFFER_SIZE),
				0);
			if (bytesSent == SOCKET_ERROR)
			{
				if (errorCode == SOCKET_ERROR)
				{
					closesocket(clientSocket);
					break;
				}
				closesocket(clientSocket);
				break;
			}
			input.erase(0, BUFFER_SIZE - 5);
			int remainTextLength = static_cast<int>(textLength - (BUFFER_SIZE - 5));
			while (remainTextLength > 0)
			{
				if (remainTextLength <= BUFFER_SIZE)
				{
					memcpy(message, input.c_str(), remainTextLength);
					bytesSent = send(
						clientSocket,
						message,
						remainTextLength,
						0);
					if (bytesSent == SOCKET_ERROR)
					{
						if (errorCode == SOCKET_ERROR)
						{
							closesocket(clientSocket);
							break;
						}
						closesocket(clientSocket);
						break;
					}
					remainTextLength = 0;
				}
				else
				{
					memcpy(message, input.c_str(), BUFFER_SIZE);
					bytesSent = send(
						clientSocket,
						message,
						BUFFER_SIZE,
						0);
					if (bytesSent == SOCKET_ERROR)
					{
						if (errorCode == SOCKET_ERROR)
						{
							closesocket(clientSocket);
							break;
						}
						closesocket(clientSocket);
						break;
					}
					input.erase(0, BUFFER_SIZE);
					remainTextLength -= BUFFER_SIZE;
				}
			}
		}

		char buffer[BUFFER_SIZE];
		const int bytesReceived = recv(
			clientSocket,
			buffer,
			BUFFER_SIZE,
			0);
		if (bytesReceived == SOCKET_ERROR)
		{
			errorCode = shutdown(clientSocket, SD_SEND);
			if (errorCode == SOCKET_ERROR)
			{
				closesocket(clientSocket);
				break;
			}

			closesocket(clientSocket);
			break;
		}
		else if (bytesReceived == 0)
		{
			errorCode = shutdown(clientSocket, SD_SEND);
			if (errorCode == SOCKET_ERROR)
			{
				closesocket(clientSocket);
				break;
			}
			closesocket(clientSocket);
			break;
		}
		else
		{
			std::string recvMessage(buffer + 5, bytesReceived - 5);
			std::cout << recvMessage;
		}
		unsigned long totalBytes{};
		memcpy(&totalBytes, buffer + 1, 4);
		totalBytes = ntohl(totalBytes);
		unsigned long recvBytes = static_cast<unsigned long>(bytesReceived - 5);
		while (recvBytes < totalBytes)
		{
			const int bytesReceived = recv(
				clientSocket,
				buffer,
				BUFFER_SIZE,
				0);
			if (bytesReceived == SOCKET_ERROR)
			{
				errorCode = shutdown(clientSocket, SD_SEND);
				if (errorCode == SOCKET_ERROR)
				{
					closesocket(clientSocket);
					break;
				}

				closesocket(clientSocket);
				break;
			}
			else if (bytesReceived == 0)
			{
				errorCode = shutdown(clientSocket, SD_SEND);
				if (errorCode == SOCKET_ERROR)
				{
					closesocket(clientSocket);
					break;
				}
				closesocket(clientSocket);
				break;
			}
			else
			{
				std::string recvMessage(buffer, bytesReceived);
				std::cout << recvMessage;
			}
			recvBytes += static_cast<unsigned long>(bytesReceived);
		}
		std::cout << std::endl;
	}

	// -------------------------------------------------------------------------
	// Clean-up after Winsock.
	//
	// WSACleanup()
	// -------------------------------------------------------------------------

	WSACleanup();
}

/* Start Header
*****************************************************************/
/*!
\file client.cpp
\author Wei Jingsong
\par jingsong.wei@digipen.edu
\date March 3 2024
\brief Multi-threading Winsock Programming using TCP sockets. This is the
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
#include <thread>
#include <iostream> // cout, cerr
#include <string>	// string
#include <vector>	// vector
#include <limits>
#include <sstream>
#include <algorithm> // For std::remove_if
#include <cctype> // For isspace
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
constexpr size_t BUFFER_SIZE = 1000;
int textLength{};
void processRecv(SOCKET clientSocket) {
	while (true) {

		char buffer[BUFFER_SIZE];
		const int bytesReceived = recv(
			clientSocket,
			buffer,
			BUFFER_SIZE,
			0);
		if (bytesReceived == SOCKET_ERROR)
		{
			size_t errorCode = WSAGetLastError();
			if (errorCode == WSAEWOULDBLOCK)
			{
				// A non-blocking call returned no data; sleep and try again.
				using namespace std::chrono_literals;
				std::this_thread::sleep_for(200ms);
				continue;
			}
			closesocket(clientSocket);
			break;
		}
		else if (bytesReceived == 0)
		{
			closesocket(clientSocket);
			break;
		}
		unsigned long netOrder{};
		unsigned char commandId = buffer[0];
		if (commandId == RSP_LISTUSERS) {
			std::cout << "==========RECV START==========" << std::endl;
			std::cout << "Users:" << std::endl;
			memcpy(&netOrder, buffer + 1, 2);
			unsigned short totalUsers = ntohs(static_cast<unsigned short>(netOrder));
			for (int i = 0; i < totalUsers; ++i) {
				memcpy(&netOrder, buffer + 3 + i * 6, 4);
				in_addr addr;
				addr.s_addr = netOrder;
				char ipStr[INET_ADDRSTRLEN];
				if (inet_ntop(AF_INET, &addr, ipStr, INET_ADDRSTRLEN) == nullptr) {
					break;
				}
				memcpy(&netOrder, buffer + 7 + i * 6, 2);
				unsigned short port = ntohs(static_cast<unsigned short>(netOrder));
				std::cout << ipStr << ":" << port << std::endl;
			}
			std::cout << "==========RECV END==========" << std::endl;
		}
		else if (commandId == ECHO_ERROR) {
			std::cout << "==========RECV START==========" << std::endl;
			std::cout << "Echo error" << std::endl;
			std::cout << "==========RECV END==========" << std::endl;
		}
		else if (commandId == REQ_ECHO)
		{
			std::cout << "==========RECV START==========" << std::endl;
			//get src ip and port
			memcpy(&netOrder, buffer + 1, 4);
			unsigned long ipULong = netOrder;
			in_addr addr;
			addr.s_addr = netOrder;
			char src_ipStr[INET_ADDRSTRLEN];
			if (inet_ntop(AF_INET, &addr, src_ipStr, INET_ADDRSTRLEN) == nullptr) {
				break;
			}
			memcpy(&netOrder, buffer + 5, 2);
			unsigned long src_port = netOrder;
			std::cout << std::string(src_ipStr) + ":" + std::to_string(ntohs(static_cast<unsigned short>(src_port))) << std::endl;
			//get message
			std::string recvMessage(buffer + 11, bytesReceived - 11);
			std::cout << recvMessage;

			unsigned long totalBytes{};
			memcpy(&totalBytes, buffer + 7, 4);
			//construct RSP_ECHO message
			char rspEchoMessage[BUFFER_SIZE]{};
			rspEchoMessage[0] = RSP_ECHO;
			memcpy(rspEchoMessage + 1, &ipULong, 4);
			memcpy(rspEchoMessage + 5, &src_port, 2);
			memcpy(rspEchoMessage + 7, &totalBytes, 4);
			memcpy(rspEchoMessage + 11, recvMessage.c_str(), recvMessage.length());
			//send RSP_ECHO message
			const int bytesSent = send(
				clientSocket,
				rspEchoMessage,
				static_cast<int>(bytesReceived),
				0);
#ifdef DEBUG_ASSIGNTMENT2_TEST
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(5000ms);
#endif
			if (bytesSent == SOCKET_ERROR)
			{
				closesocket(clientSocket);
				break;
			}

			totalBytes = ntohl(totalBytes);
			//if there is remaining message
			unsigned long recvBytes = static_cast<unsigned long>(bytesReceived - 11);
			//loop until all message is received
			while (recvBytes < totalBytes)
			{
				const int bytesReceived = recv(
					clientSocket,
					buffer,
					BUFFER_SIZE,
					0);
				if (bytesReceived == SOCKET_ERROR)
				{

					closesocket(clientSocket);
					break;
				}
				else if (bytesReceived == 0)
				{
					closesocket(clientSocket);
					break;
				}
				else
				{
					std::string recvMessage(buffer, bytesReceived);
					std::cout << recvMessage;
					const int bytesSent = send(
						clientSocket,
						buffer,
						static_cast<int>(bytesReceived),
						0);
#ifdef DEBUG_ASSIGNTMENT2_TEST
					using namespace std::chrono_literals;
					std::this_thread::sleep_for(5000ms);
#endif
					if (bytesSent == SOCKET_ERROR)
					{
						closesocket(clientSocket);
						break;
					}
				}
				recvBytes += static_cast<unsigned long>(bytesReceived);
			}
			std::cout << std::endl;
			std::cout << "==========RECV END==========" << std::endl;
			}
		else if (commandId == RSP_ECHO)
		{
			std::cout << "==========RECV START==========" << std::endl;
			//get src ip and port
			memcpy(&netOrder, buffer + 1, 4);
			in_addr addr;
			addr.s_addr = netOrder;
			char src_ipStr[INET_ADDRSTRLEN];
			if (inet_ntop(AF_INET, &addr, src_ipStr, INET_ADDRSTRLEN) == nullptr) {
				break;
			}
			memcpy(&netOrder, buffer + 5, 2);
			unsigned long src_port = netOrder;
			std::cout << std::string(src_ipStr) + ":" + std::to_string(ntohs(static_cast<unsigned short>(src_port))) << std::endl;
			//get message
			std::string recvMessage(buffer + 11, bytesReceived - 11);
			std::cout << recvMessage;
			unsigned long totalBytes{};
			memcpy(&totalBytes, buffer + 7, 4);
			totalBytes = ntohl(totalBytes);
			//if there is remaining message
			unsigned long recvBytes = static_cast<unsigned long>(bytesReceived - 11);
			//loop until all message is received
			while (recvBytes < totalBytes)
			{
				const int bytesReceived = recv(
					clientSocket,
					buffer,
					BUFFER_SIZE,
					0);
				if (bytesReceived == SOCKET_ERROR)
				{

					closesocket(clientSocket);
					break;
				}
				else if (bytesReceived == 0)
				{

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
			std::cout << "==========RECV END==========" << std::endl;
		}
		else {
			closesocket(clientSocket);
			break;
		}
		}
	}
//main function
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

	addrinfo* info = nullptr;
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

	// ignore newline char left by key in ip and port
	std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

	//receive message from server
	std::thread execute_recv(processRecv, clientSocket);
	//process user input
	while (true)
	{
		std::string input;
		std::getline(std::cin, input);

		if (input == "")
		{
			closesocket(clientSocket);
			break;
		}
		char message[BUFFER_SIZE]{};
		// if quit
		if (input[0] == '/' && input[1] == 'q')
		{
			message[0] = REQ_QUIT; // QUIT command
			int bytesSent = send(
				clientSocket,
				message,
				static_cast<int>(1),
				0);
#ifdef DEBUG_ASSIGNTMENT2_TEST
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(5000ms);
#endif
			if (bytesSent == SOCKET_ERROR)
			{
				closesocket(clientSocket);
				break;
			}
			closesocket(clientSocket);
			std::cerr << "disconnection..." << std::endl;
			break;
		}
		// if list users
		else if (input[0] == '/' && input[1] == 'l')
		{
			char message[1]{};
			message[0] = REQ_LISTUSERS;
			int bytesSent = send(
				clientSocket,
				message,
				static_cast<int>(1),
				0);
#ifdef DEBUG_ASSIGNTMENT2_TEST
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(5000ms);
#endif
			if (bytesSent == SOCKET_ERROR)
			{
				closesocket(clientSocket);
				break;
			}
		}
		// if REQ_ECHO
		else if (input[0] == '/' && input[1] == 'e')
		{
			input.erase(0, 3);
			message[0] = REQ_ECHO; // ECHO command
			size_t colonPos = input.find(':');
			size_t spacePos = input.find(' ', colonPos);

			if (colonPos != std::string::npos && spacePos != std::string::npos) {
				std::string ipStr = input.substr(0, colonPos);
				std::string portStr = input.substr(colonPos + 1, spacePos - colonPos - 1);
				std::string text = input.substr(spacePos + 1);
				// Remove any whitespace in ipStr
				ipStr.erase(std::remove_if(ipStr.begin(), ipStr.end(), isspace), ipStr.end());
				//convert ip and port
				in_addr addr;
				if (inet_pton(AF_INET, ipStr.c_str(), &addr) != 1) {
					closesocket(clientSocket);
					break;
				}
				unsigned long ipULong = addr.s_addr;
				unsigned short portUShort{};
				try {
					portUShort = static_cast<unsigned short>(std::stoul(portStr));
				}
				catch (const std::out_of_range& e) {
					(void)e;
				}
				catch (const std::invalid_argument& e) {
					(void)e;
				}
				portUShort = htons(portUShort);
				memcpy(message + 1, &ipULong, 4);
				memcpy(message + 5, &portUShort, 2);
				textLength = static_cast<int>(text.length());
				unsigned long netLength = htonl(static_cast<unsigned long>(textLength));
				memcpy(message + 7, &netLength, 4);
				//if message can be sent in one chunk
				if (textLength <= BUFFER_SIZE - 11) {
					memcpy(message + 11, text.c_str(), textLength);
					const int bytesSent = send(
						clientSocket,
						message,
						static_cast<int>(textLength + 11),
						0);
#ifdef DEBUG_ASSIGNTMENT2_TEST
					using namespace std::chrono_literals;
					std::this_thread::sleep_for(5000ms);
#endif
					if (bytesSent == SOCKET_ERROR)
					{
						closesocket(clientSocket);
						break;
					}
				}
				else //loop until all message is sent
				{
					memcpy(message + 11, text.c_str(), BUFFER_SIZE - 11);
					const int bytesSent = send(
						clientSocket,
						message,
						static_cast<int>(BUFFER_SIZE),
						0);
#ifdef DEBUG_ASSIGNTMENT2_TEST
					using namespace std::chrono_literals;
					std::this_thread::sleep_for(5000ms);
#endif
					if (bytesSent == SOCKET_ERROR)
					{
						closesocket(clientSocket);
						break;
					}
					text.erase(0, BUFFER_SIZE - 11);
					int remainTextLength = static_cast<int>(textLength - (BUFFER_SIZE - 11));
					while (remainTextLength > 0)
					{
						if (remainTextLength <= BUFFER_SIZE)
						{
							memcpy(message, text.c_str(), remainTextLength);
							const int bytesSent = send(
								clientSocket,
								message,
								remainTextLength,
								0);
#ifdef DEBUG_ASSIGNTMENT2_TEST
							using namespace std::chrono_literals;
							std::this_thread::sleep_for(5000ms);
#endif
							if (bytesSent == SOCKET_ERROR)
							{
								closesocket(clientSocket);
								break;
							}
							remainTextLength = 0;
						}
						else
						{
							memcpy(message, text.c_str(), BUFFER_SIZE);
							const int bytesSent = send(
								clientSocket,
								message,
								BUFFER_SIZE,
								0);
#ifdef DEBUG_ASSIGNTMENT2_TEST
							using namespace std::chrono_literals;
							std::this_thread::sleep_for(5000ms);
#endif
							if (bytesSent == SOCKET_ERROR)
							{
								closesocket(clientSocket);
								break;
							}
							text.erase(0, BUFFER_SIZE);
							remainTextLength -= BUFFER_SIZE;
						}
					}
						}

			}
			else {
				closesocket(clientSocket);
				break;
			}
			continue;
		}
		else {
			closesocket(clientSocket);
			std::cerr << "disconnection..." << std::endl;
			break;
		}
	}

	//wait for processRecv to finish before closing the socket
	if (execute_recv.joinable())
		execute_recv.join();
	// -------------------------------------------------------------------------
	// Clean-up after Winsock.
	//
	// WSACleanup()
	// -------------------------------------------------------------------------

	WSACleanup();
}

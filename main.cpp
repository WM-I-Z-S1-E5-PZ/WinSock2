//#define _WINSOCKAPI_
#define _WINSOCK_DEPRECATED_NO_WARNINGS
//#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

#define PORT "80"
#define BUFLEN 512

// If leave empty or ::1 - test localhost
//const char szHost[] = "::1";
// this is google part
//const char szHost[] = "2a00:1450:4006:804::200e";
//const char szHost[] = "2a00:1450:401b:803::200e";
//const char szHost[] = "2a00:1450:401b:806::200e";

// this is facebook part
//const char szHost[] = "2a03:2880:f003:c07:face:b00c::2";

// Some random IPv6
// this one doesn't work
//const char szHost[] = "fd83:4118:1d3:9bc2:0:0:0:0";
// this one too
const char szHost[] = "febc:580d:21fd:25d7:d6fc:2ea2:ff52:252";


using std::cout;
using std::endl;

int main() {

    // Initializing Winsock
    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(2, 2);
    // The WSAStartup function is called to initiate use of WS2_32.dll.
    int iResult = WSAStartup(wVersionRequested, &wsaData);
    if (iResult != NO_ERROR)
    {
        cout << "WSASturtup failed: " << iResult << endl;
        return 1;
    }
    else
    {
        cout << "========================= _WinSock status_ ======================\n";
        cout << "|\t\t\tThe WinSock dll found!\t\t\t|\n" << "|\t\t\tThe status: " << wsaData.szSystemStatus << "\t\t\t|" << endl;
        cout << "=================================================================\n\n";
    }

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
    {
        /* Tell the user that we could not find a usable WinSock DLL.*/
        printf("The dll do not support the Winsock version %u.%u!\n", LOBYTE(wsaData.wVersion), HIBYTE(wsaData.wVersion));
        WSACleanup();
        return 1;
    }
    else
    {
        cout << "========================= _System data_ =========================\n";
        cout << "|          The dll supports the Winsock version: " << (int)LOBYTE(wsaData.wVersion) << "." << (int)HIBYTE(wsaData.wVersion) << "            |" << endl;
        cout << "|          The highest version this dll can support: " << (int)LOBYTE(wsaData.wHighVersion) << "." << (int)HIBYTE(wsaData.wHighVersion) << "        |" << endl;
        cout << "=================================================================\n\n";
    }

    // Creating a socket (client)
    struct addrinfo* result = nullptr,
        * ptr = nullptr,
        hints;
    ZeroMemory(&hints, sizeof(hints));
    /*
        The address family was left unspecified (AF_UNSPEC),
        so the returned IP address could be either an IPv6 or IPv4 address for the server.

        If you want to connect using only IPv6 or IPv4,
        then the address family needs to be set to AF_INET6 for IPv6
        or AF_INET for IPv4 in the hints parameter.
        https://docs.microsoft.com/en-us/windows/win32/winsock/creating-a-socket-for-the-client
    */
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(szHost, PORT, &hints, &result);
    if (iResult != 0)
    {
        cout << "getaddrinfo failed: " << iResult << endl;
        WSACleanup();
        return 1;
    }
    SOCKET mySock = INVALID_SOCKET;
    // Attempt to connect to the first address returned by
    // the call to getaddrinfo
    ptr = result;
    // Create a SOCKET to connect to server
    mySock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    // Check for errors to ensure that the socket is a valid socket.
    if (mySock == INVALID_SOCKET)
    {
        cout << "Error at socket(): " << WSAGetLastError() << endl;
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
    // Connect to server
    iResult = connect(mySock, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        closesocket(mySock);
        mySock = INVALID_SOCKET;
    }
    freeaddrinfo(result);
    if (mySock == INVALID_SOCKET)
    {
        cout << "Unable to connect to server.\n";
        WSACleanup();
        return 1;
    }

    // Sending and Receiving Data
    int recvBufLen = BUFLEN;

    const char* sendBuf = "HEAD / HTTP/1.0\r\n\r\n";
    char recvBuf[BUFLEN];

    // Send an initial buffer
    iResult = send(mySock, sendBuf, (int)strlen(sendBuf), 0);
    if (iResult == SOCKET_ERROR)
    {
        cout << "Send failed: " << WSAGetLastError();
        closesocket(mySock);
        WSACleanup();
        return 1;
    }

    cout << "Bytes send: " << iResult << endl;

    // Shutdown the connection for sending since no more data will be sent
    // the client can still use the ConnectSocket for receiving data
    iResult = shutdown(mySock, SD_SEND);
    if (iResult == SOCKET_ERROR)
    {
        cout << "Shutdown failed: " << WSAGetLastError() << endl;
        closesocket(mySock);
        WSACleanup();
        return 1;
    }

    // Receive the data until the server closes the connection
    do
    {
        iResult = recv(mySock, recvBuf, recvBufLen, 0);
        if (iResult > 0)
        {
            cout << "Bytes received: " << iResult << endl;
        }
        else if (iResult == 0)
        {
            cout << "Connection close.\n";
        }
        else
        {
            cout << "Receive failed: " << WSAGetLastError();
        }
    } while (iResult > 0);

    // Disconnecting the Client
    // shutdown the send half of the connection since no more data will be sent
    iResult = shutdown(mySock, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        cout << "Shutdown failed: " << WSAGetLastError();
        closesocket(mySock);
        WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(mySock);
    WSACleanup();
    return 0;
}
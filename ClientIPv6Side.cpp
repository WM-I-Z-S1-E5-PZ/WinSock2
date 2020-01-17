#define WIND32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <iostream>
#include <string>

/*
	This code assumes that at the transport level, the system only supports
	one stream protocol (TCP) and one datagram protocol (UDP).  Therefore,
	specifying a socket type of SOCK_STREAM is equivalent to specifying TCP
	and specifying a socket type of SOCK_DGRAM is equivalent to specifying UDP.
*/

// Will use the loopback interface
#define DEFAULT_SERVER     NULL
// Accept either IPv4 or IPv6
#define DEFAULT_FAMILY		PF_UNSPEC
// TCP socket type
#define DEFAULT_SOCKTYPE	SOCK_STREAM
// Arbitrary, test port
#define DEFAULT_PORT		"2007"
// Number of extra bytes to send
#define DEFAULT_EXTRA		0
#define BUFFER_SIZE			65536

int main()
{
	char Buffer[BUFFER_SIZE], AddrName[NI_MAXHOST];
	char *Server = 
}
#include "UDPSocket.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h> 
#include <sys/socket.h>
#include <netinet/in.h>

UDPSocket::UDPSocket() {
	socket = ::socket(AF_INET, SOCK_DGRAM, 0);
	if (socket < 0) {
		printf("Error opening socket\n");
		exit(1);
	}
	FD_ZERO(&fdset);
	checkDelay.tv_sec = 0;
    checkDelay.tv_usec = 0;
}

UDPSocket::~UDPSocket() {
	::close(socket);
}

void UDPSocket::bind(uint16 port) {
	SocketAddress addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	if (::bind(socket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		printf("Error binding socket address\n");
		exit(1);
	}
}
bool UDPSocket::send(const void* data, uint32 size, SocketAddress to) {
	return ::sendto(socket, data, size, 0, (const struct sockaddr*)&to, sizeof(struct sockaddr_in)) >= 0;
}

bool UDPSocket::receive(void* data, uint32 maxSize, SocketAddress* from) {
	socklen_t fromlen = sizeof(struct sockaddr_in);
	struct sockaddr_in _from;
	int32 res = ::recvfrom(socket, data, maxSize, 0, (struct sockaddr*)&_from, &fromlen);
	if (from)
		*from = _from;
	return res >= 0;
}

bool UDPSocket::isDataAvailable() {
	FD_SET(socket, &fdset);
	int retval = select(4, &fdset, NULL, NULL, &checkDelay);

	if (retval < 0) {
		printf("Error select\n");
		return false;
	}else
		return FD_ISSET(socket, &fdset);
}

bool UDPSocket::isNonBlockingEnabled() {
	int opts = fcntl(socket, F_GETFL);
	if (opts < 0) {
		printf("Error fcntl\n");
		exit(1);
	}
	return opts & O_NONBLOCK;
}

void UDPSocket::setNonBlockingEnabled(bool value) {
	int opts = fcntl(socket, F_GETFL);
	if (opts < 0) {
		printf("Error fcntl\n");
		exit(1);
	}
	if (value)
		opts |= O_NONBLOCK;
	else
		opts &= ~O_NONBLOCK;
	fcntl(socket, F_SETFL, opts); 
}

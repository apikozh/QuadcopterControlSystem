#ifndef UDPSOCKET_H_
#define UDPSOCKET_H_

#include <sys/socket.h>
#include <netinet/in.h>

#include "types.h"

typedef sockaddr_in SocketAddress;

class UDPSocket {
private:
	int32 socket;
	fd_set fdset;
	timeval checkDelay;

public:
	UDPSocket();
	~UDPSocket();
	
	void bind(uint16 port);
	bool send(const void* data, uint32 size, SocketAddress to);
	bool receive(void* data, uint32 maxSize, SocketAddress* from);
	bool isDataAvailable();
	bool isNonBlockingEnabled();
	void setNonBlockingEnabled(bool value);
};

#endif
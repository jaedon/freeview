#pragma once
#include <string>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <list>

using namespace std;


class UdpSender
{
private:
	typedef struct
	{
		char*	buffer;
		int		length;
	} UDP_PACKET;

	typedef list<UDP_PACKET*>		PACKET_QUEUE;
	typedef PACKET_QUEUE::iterator	PACKET_QUEUE_IT;

protected:
	int					_sock;
	int					_port;
	struct sockaddr_in	_addr;

	pthread_t			_thread;
	int					_running;
	sem_t				_sema;
	PACKET_QUEUE		_packets;

public:
	UdpSender();
	~UdpSender();

	bool openUDP(string aIP, int aPort);
	bool sendSyncPacket(char* aBuffer, int aLength);
	bool sendAsyncPacket(char* aBuffer, int aLength);
	bool clsoeUDP();

private:
	static void* _senderThread(void* param);
	void* senderThread();
};

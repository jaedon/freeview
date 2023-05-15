#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "udp_sender.h"


UdpSender::UdpSender()
{
	_sock = -1;
	_running = false;
	memset(&_addr, 0, sizeof(_addr));

	sem_init(&_sema, 0, 1);
}

UdpSender::~UdpSender()
{
	if (_sock != -1)
	{
		close(_sock);
	}

	sem_destroy(&_sema);
}

bool UdpSender::openUDP(string aIP, int aPort)
{
	int ret;

	_sock = socket(AF_INET, SOCK_DGRAM /*| SOCK_NONBLOCK*/, 0);

	_addr.sin_family       = AF_INET;
	_addr.sin_addr.s_addr  = inet_addr(aIP.c_str());
	_addr.sin_port         = htons(aPort);

	_running = true;

	ret = pthread_create(&_thread, NULL, _senderThread, this);
	if (ret != 0)
	{
		_running = false;
		return false;
	}

	return (_sock != -1);
}

bool UdpSender::sendAsyncPacket(char* aBuffer, int aLength)
{
	UDP_PACKET* packet;

	if (_sock == -1)
	{
		return false;
	}

	packet = new UDP_PACKET;
	packet->buffer =  new char[aLength];
	packet->length = aLength;
	memcpy(packet->buffer, aBuffer, aLength);

	sem_wait(&_sema);
	_packets.push_front(packet);
	sem_post(&_sema);

	return true;
}

bool UdpSender::sendSyncPacket(char* aBuffer, int aLength)
{
	int ret;

	if (_sock == -1)
	{
		return false;
	}

	ret = sendto(_sock, aBuffer, aLength, 0, (struct sockaddr *)&_addr, sizeof(_addr));

	return (ret != -1);
}

bool UdpSender::clsoeUDP()
{
	void* pRet;

	if (_sock == -1)
	{
		return false;
	}

	close(_sock);
	_sock = -1;

	_running = false;
	pthread_join(_thread, &pRet);

	return true;
}


void* UdpSender::_senderThread(void* param)
{
	UdpSender* sender;

	sender = static_cast<UdpSender*>(param);

	return sender->senderThread();
}


void* UdpSender::senderThread()
{
	UDP_PACKET*		packet;
	PACKET_QUEUE_IT it;
	size_t sentBytes;

	printf("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	while(_running == true)
	{
		packet = NULL;
		sem_wait(&_sema);

		if (_packets.empty() == false)
		{
			packet = _packets.back();
			_packets.pop_back();
		}
		sem_post(&_sema);

		if (packet != NULL)
		{
			sentBytes = sendto(_sock, packet->buffer, packet->length, 0, (struct sockaddr *)&_addr, sizeof(_addr));
			if (sentBytes == (size_t)-1)
			{
				printf("[%s:%d] sendto error(errno:%d)\n", __FUNCTION__, __LINE__, errno);
			}

			delete[] packet->buffer;
			delete[] packet;
		}
		else
		{
			usleep(10);
		}
	}

	sem_wait(&_sema);
	for (it = _packets.begin() ; it != _packets.end() ; it++)
	{
		packet = *it;
		if (packet != NULL)
		{
			delete[] packet->buffer;
			delete[] packet;
		}
	}
	_packets.clear();
	sem_post(&_sema);

	printf("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return 0;
}

#include "int_streaming.h"
#include "int_include/int_pl_ringbuf_context.h"

#include <map>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>


//#define UDS_ONE_TIME_BUFFERING	/* 2015/6/17 mhkang: SAT>IP Streaming시 1번만 buffering 하는 설정 */
//#define UDS_TEST_FILE				/* 2015/6/17 mhkang: SAT>IP Streaming시 TS파일을 읽어서 Client에게 전달 테스트용  */
//#define UDS_TS_TRANSFER_TEST		/* 2015/6/19 mhkang: obama ==> homma TS 전달 테스트 */


#ifdef UDS_ONE_TIME_BUFFERING
#define UDS_BUFFER_TIME     500000			// (단위: micro second) : http://svn.humaxdigital.com:3000/issues/104038#note-23  이슈관련하여 buffering하지 않도록 수정됨(원래 값: 500000 micro second)
#else
#define UDS_BUFFER_TIME     0
#endif

#define UDS_PACKET_LENGTH   1316        // 188 x 7 byte. sat>ip spec에서 정의한  rtp packet안의 최대 ts 패킷수.


#define GET_TID()  syscall(4222)

using namespace std;


//#define DEBUG_CONSOLE_PRINT

#ifdef DEBUG_CONSOLE_PRINT
    #define HxLOG_Error   printf
    #define HxLOG_Info   printf
#endif

/* =================================== type definition ====================================== */
class UdsManager;

typedef map<int, bool>         SOCKET_MAP;  // int socketfd, int SendingFlag
typedef SOCKET_MAP::iterator   SOCKET_MAP_IT;


/* =================================== CPP Implementation ====================================== */
class UdsManager
{
public:
    UdsManager(int socketfd, Handle_t ringbuf);
    virtual ~UdsManager();

    bool SetRingBuf(Handle_t ringbuf);
    bool AddSocket(int socketfd);
    bool RemoveSocket(int socketfd);
    bool SetSendingFlag(int socketfd, bool bSend);
    bool SetSignalLock(bool signalLocked);

protected:
    void* UdsThread();


private:
    SOCKET_MAP  _sockmap;
    Handle_t    _ringbuf;
    int         _running;
    pthread_t   _thread;
    sem_t       _sema;
    bool        _signalLocked;
    int         _bufferingTime;
	bool		_bufferingFlag;
	int			_ownerSocket;
	bool		_ownerSendingFlag;

    void CleanUp();

    static void* _UdsThread(void* param);
};

UdsManager::UdsManager(int socketfd, Handle_t ringbuf)
{
    int ret;

    sem_init(&_sema, 0, 1);

    _signalLocked = false;
	_bufferingTime = UDS_BUFFER_TIME;
    _ringbuf = ringbuf;

	_ownerSocket = socketfd;
	_ownerSendingFlag = false;

    _running = true;
	_bufferingFlag = false;

    HxLOG_Info("[UDS] Create : socketfd:%d, ringbuf:%d \n", socketfd, ringbuf);
    ret = pthread_create(&_thread, NULL, _UdsThread, this);
    if (ret != 0)
    {
        HxLOG_Error("pthread_create failed(ret:%d)\n", ret);
    }
}

UdsManager::~UdsManager()
{
    CleanUp();

    sem_destroy(&_sema);
}


void* UdsManager::UdsThread()
{
    SOCKET_MAP_IT it;
    HERROR hErr;
    HUINT32 ulDataSize;
    HUINT32 ulReadSize;
    char* pucData = NULL;
    HxRingBuf_Status_t stStatus;
    struct timeval st, et;
    int sleepTime;
    int ret = 0;
    int lastError;
#ifdef UDS_TS_TRANSFER_TEST
	int sock;
	struct sockaddr_in addr;
	socklen_t socklen;
#endif

#ifdef UDS_TEST_FILE
	FILE* fp;

	fp = fopen("/kika.ts","rb");
	if (fp == NULL)
	{
		HxLOG_Error("[UDS] open file error(errno:%d)\n", errno);
		return 0;
	}

#endif

#ifdef UDS_TS_TRANSFER_TEST
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	socklen = sizeof(addr);
	memset(&addr, 0, socklen);
	addr.sin_family       = AF_INET;
	addr.sin_addr.s_addr  = inet_addr( "192.168.0.7");
	addr.sin_port         = htons(55555);

#endif

    ulReadSize = UDS_PACKET_LENGTH;   // 2015/3/25 mhkang: live555 모듈에서 188 x 7 단위로 읽어간다.
    pucData = new char[ulReadSize];

    while(_running == true)
    {
        sleepTime = 0;

        if (_ringbuf == HANDLE_NULL)
        {
//            HxLOG_Error("[UDS] ringbuf is null\n");
            sleepTime = 1000;
            goto LOOP_END;
        }

        if (_signalLocked == false)
        {
//            HxLOG_Error("[UDS] signal is not locked\n");
            sleepTime = 1000;
            goto LOOP_END;
        }
#ifndef UDS_TEST_FILE
		memset(&stStatus, 0, sizeof(stStatus));
        hErr = PL_RINGBUF_GetBufferingStatus(_ringbuf, &stStatus);
        if(ERR_OK != hErr)
        {
            HxLOG_Error("[UDS:%d] PL_RINGBUF_GetBufferingStatus failed(ringbuf:%d, hErr:%d)\n",GET_TID(), _ringbuf, hErr);
        }
        else
		{
            if (stStatus.ulUnreadSize < ulReadSize)
            {
				sleepTime = 10000;
	//			HxLOG_Error("unRead:%d\n", stStatus.ulUnreadSize);
                goto LOOP_END;
			}
	//		HxLOG_Debug("unRead:%d\n", stStatus.ulUnreadSize);


#ifdef UDS_TS_TRANSFER_TEST
			hErr = ::PL_RINGBUF_ReadData(_ringbuf, ulReadSize, (HUINT8*)pucData, &ulDataSize);
			if(ERR_OK != hErr || 0 == ulDataSize)
			{
				HxLOG_Error("[UDS:%d] can not read ts from ringbuf(%d), ulDataSize:%d, hErr:%d\n",GET_TID(), _ringbuf, ulDataSize, hErr);
				sleepTime = 10000;
				goto LOOP_END;
			}
			sendto(sock, pucData, ulDataSize, 0, (struct sockaddr *)&addr, socklen);
			usleep(10);
			continue;
#endif
        }


#ifdef UDS_ONE_TIME_BUFFERING
		if (_ownerSendingFlag == true)
		{
#endif
			// ringbuf 변경될 때마다 설정된 bufferinTime만큼 sleep 한다.
			if (_bufferingTime > 0 )
			{
				// buffering하기전에 기존 data clear
				//::PL_RINGBUF_ClearBuffer(_ringbuf);  // 2015/6/13 mhkang: http://svn.humaxdigital.com:3000/issues/104038#note-23  이슈때문에 RingBuffer Clear하지 않도록 수정함.
				usleep(_bufferingTime);
				_bufferingTime = 0;
			}

			hErr = ::PL_RINGBUF_ReadData(_ringbuf, ulReadSize, (HUINT8*)pucData, &ulDataSize);
			if(ERR_OK != hErr || 0 == ulDataSize)
			{
				HxLOG_Error("[UDS:%d] can not read ts from ringbuf(%d), ulDataSize:%d, hErr:%d\n",GET_TID(), _ringbuf, ulDataSize, hErr);
				sleepTime = 10000;
				goto LOOP_END;
			}
#ifdef UDS_ONE_TIME_BUFFERING
		}
#endif

#else
		ulDataSize = fread(pucData, 1, ulReadSize, fp);
		if (ulDataSize == 0)
		{
			fseek(fp, 0, SEEK_SET);
			ulDataSize = fread(pucData, 1, ulReadSize, fp);
		}
#endif


RETRY_OWNER_SEND:
		if (_ownerSendingFlag == true)
		{
			gettimeofday(&st, NULL);

			ret = write(_ownerSocket, pucData, ulDataSize);

			lastError = errno;
			if (ret == -1)
			{
				errno = 0;
				if (lastError == EAGAIN || lastError == EWOULDBLOCK)
				{
					if (_running == false)
					{
						/* 2015/3/24 mhkang: 종료가 감지되면 빠져나간다. */
						sem_post(&_sema);
						goto FUNC_END;
					}

					sem_post(&_sema); // 2015/3/27 mhkang: 여기서 sem_post하지 않으면 종료과정에 RemoveSocket 호출시 RemoveSocket안에서 sem_wait에서 무한 blocking될 수가 있다.
					/* 2015/4/7 mhkang: write timeout일때는 write/read buffer가 꽉 찰 경우이다. read하여 소진될때까지 timeout발생할 수 밖에 없다. 너무 자주 i/o 발생시키지 않도록 sleep해준다.
					  sleep 100 msec : HD 채널 재생시 약간 끊김현상 발생.
					  sleep 50 msec  : HD 채널 재생시 끊김현상 보이지 않음(테스트 결과값임. 튜닝 필요).
					*/
					usleep(50000);
					sem_wait(&_sema);

					/* blocking된 socket만 다시 보내야 할듯. 좀 더 고민해보자. */
					//HxLOG_Debug("[UDS] W: timeout\n");
					goto RETRY_OWNER_SEND;
				}
				else
				{
					HxLOG_Error("[UDS] writing error(%d)\n", lastError);
				}
			}
			else
			{

				if (stStatus.ulUnreadSize < 188 * 7 * 10)
				{
					usleep(5000);
				}
				else if (stStatus.ulUnreadSize < 188 * 7 * 100)
				{
					usleep(1000);
				}
				else if (stStatus.ulUnreadSize > 188 * 7 * 1000)
				{
					usleep(0);
				}
				else
				{
					// 아래 usleep 주는 것은 로그출력에 따라 다르게 동작함.
					// uds thread에서 로그출력안하면 200 msec 정도로 하고 로그출력하면 100 msec 정도.;;;
					// 경험치 값임.
					usleep(200); // 2015/6/17 mhkang: 200 micro second sleep 해주니 RTP 전송패턴 그래프에서 튀는 현상이 상당히 줄어듬. 500으로 하면 HD채널 streaming할 때 ringBuf overflow발생함(http://svn.humaxdigital.com:3000/issues/104038)
				}
			}

			gettimeofday(&et, NULL);
		}
		else
		{
			// 2015/3/27 mhkang: 전송flag가 false 인 상태. do nothing.
		}

RETRY_NON_OWNER_SEND:
		// SAT>IP Non-owner
		sem_wait(&_sema);
        for (it = _sockmap.begin() ; it != _sockmap.end() ; it++)
        {
            if (it->second == true) // bSendFlag is true
			{
                gettimeofday(&st, NULL);

				ret = write(it->first, pucData, ulDataSize);

                lastError = errno;
                if (ret == -1)
                {
                    errno = 0;
                    if (lastError == EAGAIN || lastError == EWOULDBLOCK)
                    {
						sem_post(&_sema); // 2015/3/27 mhkang: 여기서 sem_post하지 않으면 종료과정에 RemoveSocket 호출시 RemoveSocket안에서 sem_wait에서 무한 blocking될 수가 있다.
						/* 2015/4/7 mhkang: write timeout일때는 write/read buffer가 꽉 찰 경우이다. read하여 소진될때까지 timeout발생할 수 밖에 없다. 너무 자주 i/o 발생시키지 않도록 sleep해준다.
						  sleep 100 msec : HD 채널 재생시 약간 끊김현상 발생.
						  sleep 50 msec  : HD 채널 재생시 끊김현상 보이지 않음(테스트 결과값임. 튜닝 필요).
						*/

                        if (_running == false)
                        {
                            /* 2015/3/24 mhkang: 종료가 감지되면 빠져나간다. */
                            goto FUNC_END;
                        }

                        usleep(10000);

                        /* blocking된 socket만 다시 보내야 할듯. 좀 더 고민해보자. */
						//HxLOG_Debug("[UDS] writing timeout\n");
						goto RETRY_NON_OWNER_SEND;
                    }
                    else
                    {
                        HxLOG_Error("[UDS] writing error(%d)\n", lastError);
                    }
                }

                gettimeofday(&et, NULL);
            }
            else
            {
                // 2015/3/27 mhkang: 전송flag가 false 인 상태. do nothing.
            }
        }
		sem_post(&_sema);

LOOP_END:
        usleep(sleepTime);
    }

FUNC_END:
    delete[] pucData;

#ifdef UDS_TEST_FILE
	if (fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
#endif

#ifdef UDS_TS_TRANSFER_TEST
	close(sock);
#endif

    return 0;
}

void* UdsManager::_UdsThread(void* param)
{
    UdsManager* UdsMgr;

    UdsMgr = static_cast<UdsManager*>(param);

    return UdsMgr->UdsThread();
}

void UdsManager::CleanUp()
{
    SOCKET_MAP_IT it;
    void* pRet;

    HxLOG_Info("[UDS:%d] CleanUp (+)\n", GET_TID());

    _running = false;

    sem_wait(&_sema);

    _sockmap.clear();
    sem_post(&_sema);

    pthread_join(_thread, &pRet);

    HxLOG_Info("[UDS:%d] CleanUp (-)\n", GET_TID());
}

bool UdsManager::SetRingBuf(Handle_t ringbuf)
{
    HxLOG_Info("[UDS:%d] SetRingBuf(ringbuf:%d)\n",GET_TID(), ringbuf);

	if (_ringbuf == ringbuf)
	{
		return true;
	}

    sem_wait(&_sema);

    _ringbuf = ringbuf;
    sem_post(&_sema);

	if (_bufferingFlag == false) // 한번만 buffering하게
	{
		_bufferingTime = UDS_BUFFER_TIME;
		_bufferingFlag == true;
	}

    return true;
}

bool UdsManager::AddSocket(int socketfd)
{
    SOCKET_MAP_IT it;

    HxLOG_Info("[UDS:%d] AddSocket(socketfd:%d, ringbuf:%d)\n", GET_TID(), socketfd, _ringbuf);
    it = _sockmap.find(socketfd);


    if (it != _sockmap.end())
    {
        HxLOG_Error("Already exist socket(%d)\n", socketfd);
        return false;
    }

    sem_wait(&_sema);
    _sockmap.insert(make_pair(socketfd, false));
    sem_post(&_sema);

    return true;
}

bool UdsManager::RemoveSocket(int socketfd)
{
    SOCKET_MAP_IT it;

    HxLOG_Info("[UDS:%d] RemoveSocket(socketfd:%d)\n", GET_TID(), socketfd);
    sem_wait(&_sema);

    it = _sockmap.find(socketfd);
    if (it == _sockmap.end())
    {
        sem_post(&_sema);
        HxLOG_Error("Not found socket(%d)\n", socketfd);
        return false;
    }

    _sockmap.erase(it);

    sem_post(&_sema);

    HxLOG_Info("[UDS:%d] RemoveSocket(-)\n");

    return true;
}

bool UdsManager::SetSendingFlag(int socketfd, bool bSend)
{
    SOCKET_MAP_IT it;

    HxLOG_Info("[UDS:%d] SetSendingFlag(socketfd:%d, bSend:%d)\n", GET_TID(), socketfd, bSend);

	if (_ownerSocket == socketfd)
	{
		_ownerSendingFlag =bSend;
		return true;
	}

	sem_wait(&_sema);	// 2015/6/13 mhkang: non-owner의 sendingFlag 변경할 때는 반드시 sem lock해야함(crash가능성 있음).
    it = _sockmap.find(socketfd);
    if (it == _sockmap.end())
    {
		sem_post(&_sema);
        HxLOG_Error("Not found socket(%d)\n", socketfd);
        return false;
    }

    _sockmap.erase(it);
    _sockmap.insert(make_pair(socketfd, bSend)); // bSend 값을 변경 시키기 위해 삭제후 다시 insert

	sem_post(&_sema);

    return true;
}

bool UdsManager::SetSignalLock(bool signalLocked)
{
    HxLOG_Info("[UDS:%d] SetSignalLock(signal:%d)\n",GET_TID(), signalLocked);
    sem_wait(&_sema);

    _signalLocked = signalLocked;
    sem_post(&_sema);

    return true;
}

/* =================================== C API Wrapper ====================================== */
void*  UDS_CreateManager(int socketfd, Handle_t hringbuf)
{
    UdsManager* UdsMgr;

    UdsMgr =  new UdsManager(socketfd, hringbuf);

    return UdsMgr;
}

bool UDS_DestroyManager(void* UdsObj)
{
    UdsManager* UdsMgr;

    if (UdsObj == NULL)
    {
        HxLOG_Error("Invalid args(UdsObj is null)\n");
        return false;
    }

    UdsMgr = static_cast<UdsManager*>(UdsObj);

    delete UdsMgr;

    return true;
}

bool UDS_SetRingBuf(void* UdsObj, Handle_t hringbuf)
{
    UdsManager* UdsMgr;
    bool ret;

    if (UdsObj == NULL)
    {
        HxLOG_Error("Invalid args(UdsObj is null)\n");
        return false;
    }

    UdsMgr = static_cast<UdsManager*>(UdsObj);

    ret = UdsMgr->SetRingBuf(hringbuf);

    return ret;
}

bool UDS_AddSocket(void* UdsObj, int socketfd)
{
    UdsManager* UdsMgr;
    bool ret;


    if (UdsObj == NULL)
    {
        HxLOG_Error("Invalid args(UdsObj is null)\n");
        return false;
    }

    UdsMgr = static_cast<UdsManager*>(UdsObj);

    ret = UdsMgr->AddSocket(socketfd);

    return ret;
}


bool UDS_RemoveSocket(void* UdsObj, int socketfd)
{
    UdsManager* UdsMgr;
    bool ret;

    if (UdsObj == NULL)
    {
        HxLOG_Error("Invalid args(UdsObj is null)\n");
        return false;
    }

    UdsMgr = static_cast<UdsManager*>(UdsObj);

    ret = UdsMgr->RemoveSocket(socketfd);

    return ret;
}

bool UDS_SetSendingFlag(void* UdsObj, int socketfd, bool bSend)
{
    UdsManager* UdsMgr;
    bool ret;

    if (UdsObj == NULL)
    {
        HxLOG_Error("Invalid args(UdsObj is null)\n");
        return false;
    }

    UdsMgr = static_cast<UdsManager*>(UdsObj);

    ret = UdsMgr->SetSendingFlag(socketfd, bSend);

    return ret;
}

bool UDS_SetSignalLock(void* UdsObj, bool signalLocked)
{
    UdsManager* UdsMgr;
    bool ret;

    if (UdsObj == NULL)
    {
        HxLOG_Error("Invalid args(UdsObj is null)\n");
        return false;
    }

    UdsMgr = static_cast<UdsManager*>(UdsObj);

    ret = UdsMgr->SetSignalLock(signalLocked);

    return ret;
}


bool UDS_ClearSocketBuffer(int socketfd)
{
    struct timeval tv;
    fd_set readset;
    int ret = 0;
    char buffer[UDS_PACKET_LENGTH];

    while(true)
    {
        FD_ZERO(&readset);
        FD_SET(socketfd, &readset);

        tv.tv_sec = 0;
        tv.tv_usec = 10000; // 10 msec

        ret = select(socketfd + 1, &readset, NULL, NULL, &tv);
        if (ret > 0)
        {
            read(socketfd, buffer, UDS_PACKET_LENGTH);
            continue;
        }
        else if (ret == 0) // timeout
        {
            return true;
        }
        else
        {
            HxLOG_Warning("*************************************************\n");
            HxLOG_Warning("*                Connection close               *\n");
            HxLOG_Warning("*************************************************\n");
            return false;
        }
    }

    return false;
}

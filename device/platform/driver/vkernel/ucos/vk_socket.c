/*************************************************************
 * @file		hxsocket.h
 * @date		2012/03/13
 * @author		someone
 * @brief		HLIB Socket API

 * http://www.humaxdigital.com
 *************************************************************/

#define ___HEADER________________________________________________________________________________________

#include "vkernel.h"

#define ___DEFINE________________________________________________________________________________________

#define ___STATIC_VARIABLES______________________________________________________________________________

#define ___PRIVATE_FUNCTION_PROTOTYPE____________________________________________________________________

#define	___PRIVATE_API___________________________________________________________________________________

#define	___PUBLIC_API____________________________________________________________________________________

int VK_SOCKET_SERVER_Open(VK_SOCKET_TYPE_E eSocketType,
                          void *pServerPort,
                          HUINT32 *hSocket)
{
	(void)(eSocketType);
	(void)(pServerPort);
	(void)(hSocket);
	return VK_OK;
}

int VK_SOCKET_CLIENT_Open(VK_SOCKET_TYPE_E eSocketType,
	                      const HCHAR *pszIPAddr,
	                      void *targetPort,
	                      HUINT32 nTry,
	                      HUINT32 *hSocket)
{
	(void)(eSocketType);
	(void)(pszIPAddr);
	(void)(targetPort);
	(void)(nTry);
	(void)(hSocket);
	return VK_OK;
}

int VK_SOCKET_Close(HUINT32 sock)
{
	(void)(sock);	
	return VK_OK;
}

int VK_SOCKET_Listen(HUINT32 sock,
	                 HUINT32 timeout,
	                 VK_SOCKET_DataListener_Cb cbPacketReceived,
	                 void *userdata)
{
	(void)(sock);
	(void)(timeout);
	(void)(cbPacketReceived);
	(void)(userdata);
	return VK_OK;
}

int VK_SOCKET_Write(HUINT32 sock,
	                const void *pvData,
	                HUINT32 size,
	                HUINT32 reqid)
{
	(void)(sock);
	(void)(pvData);
	(void)(size);
	(void)(reqid);
	return VK_OK;
}

int VK_SOCKET_Init(void)
{
	return VK_OK;
}

void VK_SOCKET_PrintSockInfo(void)
{
}

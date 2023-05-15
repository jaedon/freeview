/*
*@ File: tc_hlib_socket.c
*@ Author: Peter, vdthe@humaxdigital.com
*@ Date: 06-Feb-2017 2:35 PM
*@ Description: TC for hlib socket, client and server
*/



#define ___HEADER______________________________________________________________________

#include "atkit.h"
#include "hlib.h"
#include "vkernel.h"

#define ___DEFINE______________________________________________________________________

#define	SOCKET_SERVER			"/home/atkit/userapp/.sockserver.app"
#define SERVER_MESSAGE			"SERVER_RESPONSE_MESSAGE"
#define CLIENT_MESSAGE			"SOCKET_CLIENT_MESSAGE"

#define ___STATIC_VARIABLES____________________________________________________________

static HxJSON_t s_manifest = NULL;
static HINT32	s_nServerHandle = 0;
static HINT32 	s_nClientHandle = 0;
#define ___STATIC_FUNCTION_____________________________________________________________

static void server_PacketReceived(void *userData,  HINT32 nSocketHandle, HUINT32 requestId, HINT32 size, void *pvData)
{
	void *data;
	HINT32 hErr = -1;
	HINT32 nPacket_size = 0;
	HINT32 clients[10];
	HUINT32 i = 0;
	HUINT32 n = 0;
/*  Reply client by its message */
	nPacket_size = size;
	data = pvData;

	n = HLIB_SOCKET_GetClients(s_nServerHandle, clients, 10);

	for(i = 0; i < n; i++)
	{
		hErr = HLIB_SOCKET_Write(clients[i], data, nPacket_size, 0);
		if(hErr < 0)
		{
			printf("[%s]TEST_FAIL: FAIL to HLIB_SOCKET_Write()-Err(%d) \n", __FUNCTION__, hErr);
			fflush(stdout);
			return;
		}
	}


printf("[Socket Server] Received packet size = %d, %d [clients] \n", nPacket_size, n);

}


static void server_StatusChanged(void *userData, HxSocket_Status_e eStatus, HINT32 nClientHandle)
{

	printf("[Socket Server] Status changed \n");
	fflush(stdout);

}

static void local_socket_server_listen(void *arg)
{
	HINT32 hErr = -1;

	while(1)
	{
		hErr = HLIB_SOCKET_Listen(s_nServerHandle, TRUE, server_PacketReceived, server_StatusChanged, NULL);
		if(hErr < 0)
		{
			printf("[%s] TEST_FAIL: Fail to HLIB_SOCKET_Listen()-Err(%d) \n", __FUNCTION__, hErr);
			fflush(stdout);
			return ;
		}

	}
	return ;

}


static void client_PacketReceived(void *userData,  HINT32 nSocketHandle, HUINT32 requestId, HINT32 size, void *pvData)
{

	HINT32 nPacket_size = size;
	printf("[Socket Client] Received packet size = %d \n", nPacket_size);
	fflush(stdout);

}
static void client_StatusChanged(void *userData, HxSocket_Status_e eStatus, HINT32 nClientHandle)
{

	printf("[Socket Client] Status changed \n");
	fflush(stdout);
}


HBOOL TC_SOCKET_SERVER_Open(HxJSON_t j_args)
{
	s_nServerHandle = HLIB_SOCKET_OpenServer(HxSOCKET_UDS, (void*)SOCKET_SERVER);
	if(s_nServerHandle == 0)
	{
		printf("[%s] TEST_FAIL: Fail to HLIB_SOCKET_OpenServer()-Err(%d)",__FUNCTION__,s_nServerHandle);
		fflush(stdout);
		return FALSE;
	}

	return TRUE;
}


HBOOL TC_SOCKET_SERVER_Listen(HxJSON_t j_args)
{
	HLONG	ulTaskId = 0;
	VK_TASK_Create(local_socket_server_listen, VK_TASK_PRIORITY_MW_REF, SIZE_256K, "tc_socket_server_listen", NULL, &ulTaskId, 0);
	VK_TASK_Start(ulTaskId);
	return TRUE;
}

HBOOL TC_SOCKET_CLIENT_Open(HxJSON_t j_args)
{
	HINT32 hErr = -1;
	HINT32 i = 0;
	HINT32 nLimitTc;
	HxJSON_t j_argument = NULL;

	j_argument = HLIB_JSON_Object_GetObject(j_args, "argument");
	if(j_argument != NULL)
	{
		nLimitTc = HLIB_JSON_Object_GetInteger(j_argument, "limit_tc");
	}

	for(i = 0; i < nLimitTc; i++)
	{
		s_nClientHandle = HLIB_SOCKET_OpenClient(HxSOCKET_UDS, (void*)SOCKET_SERVER , 1);
		if(s_nClientHandle == 0)
		{
			printf("[%s] TEST_FAIL: Fail to HLIB_SOCKET_OpenClient()-Err(%d)",__FUNCTION__, s_nClientHandle);
			fflush(stdout);
			return FALSE;
		}

		hErr = HLIB_SOCKET_Close(s_nClientHandle);
		if(hErr != ERR_OK)
		{
			printf("[%s] TEST_FAIL: Fail to HLIB_SOCKET_Close()-Err(%d)",__FUNCTION__, hErr);
			fflush(stdout);
			return FALSE;
		}
	}
	return TRUE;
}

HBOOL TC_SOCKET_CLIENT_ListenTimeOut(HxJSON_t j_args)
{
	HINT32 hErr = -1;
	HUINT32 nTimeout = 0;
	HxJSON_t j_argument = NULL;
	HUINT8 *pucClientMessage;
	j_argument = HLIB_JSON_Object_GetObject(j_args, "argument");
	if(j_argument != NULL)
	{
		nTimeout = HLIB_JSON_Object_GetInteger(j_argument, "timeout");
	}
	s_nClientHandle = HLIB_SOCKET_OpenClient(HxSOCKET_UDS, (void*)SOCKET_SERVER , 1);
	if(s_nClientHandle < 0)
	{
		printf("[%s] TEST_FAIL: Fail to HLIB_SOCKET_OpenClient()-Err(%d)",__FUNCTION__, s_nClientHandle);
		fflush(stdout);
		return FALSE;
	}
	pucClientMessage = (HUINT8*)calloc(50, sizeof(HUINT8));
	memcpy((void*)pucClientMessage,(void*)CLIENT_MESSAGE, strlen(CLIENT_MESSAGE));
	hErr = HLIB_SOCKET_Write(s_nClientHandle, (void*)pucClientMessage, 50, 0);
	if(hErr < 0)
	{

		printf("[%s] TEST_FAIL: Fail to HLIB_SOCKET_Write()-Err(%d)",__FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;

	}

	hErr = HLIB_SOCKET_ListenTimeout(s_nClientHandle, nTimeout, client_PacketReceived ,client_StatusChanged, NULL);
	if(hErr < 0)
	{
		printf("[%s] TEST_FAIL: Fail to HLIB_SOCKET_ListenTimeout()-Err(%d)",__FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;

	}
	hErr = HLIB_SOCKET_Close(s_nClientHandle);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FAIL: Fail to HLIB_SOCKET_Close()-Err(%d)",__FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}

	return TRUE;
}
void TC_HLIB_SOCKET_Init(HxJSON_t jManifest)
{

	printf("[%s]\n", __FUNCTION__);
	fflush(stdout);
	s_manifest = jManifest;
	ADD_TC(TC_SOCKET_SERVER_Open);
	ADD_TC(TC_SOCKET_SERVER_Listen);
	ADD_TC(TC_SOCKET_CLIENT_Open);
	ADD_TC(TC_SOCKET_CLIENT_ListenTimeOut);



}


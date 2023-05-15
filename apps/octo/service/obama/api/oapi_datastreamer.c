/**************************************************************
 *	@file		oapi_datastreamer.c
 *	This module for OCTO 2.0's Open API Interface
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2013/01/XX
 *	@author			humax
 *	@brief			OCTO Open API Interface
 **************************************************************/
#include 	<hlib.h>
#include	<obama_int.h>
#include	<oapi.h>
#include	<vkernel.h>

#include	"oapi_mem.h"





#define	CLIENT_DATASTREAMER_PORT			"/tmp/.datastreamer.dat"
STATIC	HINT32 	s_pstClientInfo = 0;
STATIC	void	(*s_cbPacketReceived)(HINT32 size, void *pvData) = NULL;


STATIC void		oapi_datastreamer_receivePacket(void *pvUser, HINT32 nSocketHandle, HUINT32 ulRequestId, HINT32 nSize, void *pvData)
{
	if (s_cbPacketReceived)
		s_cbPacketReceived(nSize, pvData);
}

STATIC void	oapi_datastreamer_listener(void *param)
{
	HLIB_SOCKET_Listen((HINT32)param, TRUE, oapi_datastreamer_receivePacket, NULL, NULL);
}

HERROR	OAPI_InitDataStreamer(void (*cbPacketReceived)(HINT32 size, void *pvData))
{
	HUINT32		ulTaskId;

	if (s_pstClientInfo == 0)
	{
		s_pstClientInfo = HLIB_EXTRA_OpenClient("datastream");//HLIB_SOCKET_OpenClient(HxSOCKET_UDS, CLIENT_DATASTREAMER_PORT, 0);
		VK_TASK_Create(oapi_datastreamer_listener, VK_TASK_PRIORITY_MW_REF, SIZE_32K, "oapids", (void*)s_pstClientInfo, (unsigned long*)&ulTaskId, 0);
		VK_TASK_Start(ulTaskId);
	}

	s_cbPacketReceived = cbPacketReceived;

	return ERR_OK;
}

HERROR	OAPI_ConnectDataStreamer(HCHAR *data_type)
{
	HINT32		stream;
	HCHAR		*appl_command;

	if (s_pstClientInfo == 0)
	{
		HxLOG_Critical("Data Streamer is not initialized!!!\n");
		return ERR_FAIL;
	}

	stream = HLIB_RWSTREAM_OpenEx(NULL, 0, OxMEM_Malloc_CB, OxMEM_Free_CB);
	HLIB_RWSTREAM_Print(stream, "{\"datastreamer\":{\"op_code\":\"connect\",\"datatype\":\"%s\"}}", data_type);
	appl_command = HLIB_RWSTREAM_CloseWithoutBuf(stream);

	HLIB_SOCKET_Write(s_pstClientInfo, (void*)appl_command, HxSTD_StrLen(appl_command) + 1, 0);

	OxMEM_Free(appl_command);

	return ERR_OK;
}

HERROR	OAPI_DisconnectDataStreamer(HCHAR *data_type)
{
	HINT32		stream;
	HCHAR		*appl_command;

	if (s_pstClientInfo == 0)
	{
		HxLOG_Critical("Data Streamer is not initialized!!!\n");
		return ERR_FAIL;
	}

	stream = HLIB_RWSTREAM_Open(NULL, 0);
	HLIB_RWSTREAM_Print(stream, "{\"datastreamer\":{\"op_code\":\"disconnect\",\"datatype\":\"%s\"}}", data_type);
	appl_command = HLIB_RWSTREAM_CloseWithoutBuf(stream);

	HLIB_SOCKET_Write(s_pstClientInfo, (void*)appl_command, HxSTD_StrLen(appl_command) + 1, 0);

	OxMEM_Free(appl_command);

	return ERR_OK;
}


/**************************************************************
*
*	http://www.humaxdigital.com
*
*	@data			2012/10/24
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ⓒ 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/


/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include <stdbool.h>
#include "hlib.h"
#include "vkernel.h"
#include "nova_ringbuf.h"
#include "nova_receiver.h"

/**********************************************************************************************************/
#define ____DEFINE________________________
/**********************************************************************************************************/
#define RINGBUF_DUMP_FILENAME	"/receive_ringbuf.ts"
#define STREAMING_UDS_NAME		"hommastreaming"

/**********************************************************************************************************/
#define ____TYPEDEF________________________
/**********************************************************************************************************/


/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/
#if defined(CONFIG_DEBUG)
static HBOOL					s_bDumpRingbuf = FALSE;
#endif


/**********************************************************************************************************/
#define ____IMPLEMENTATION_______________________
/**********************************************************************************************************/

class TsReceiver
{
public:
	TsReceiver();
	~TsReceiver();

	NOVA_ErrCode_e		init();
	NOVA_ErrCode_e		deinit();
	static TsReceiver*	instance();
	static TsReceiver*	checkInstance();

private:
	bool				_isRunning;
	HULONG				_sema;
	HULONG				_taskId;
	HINT32				_sock ;
	static TsReceiver*	_instance;

	static void		receiverTask(void *param);
	static void		onStatusChanged(void *userData, HxSocket_Status_e eStatus, HINT32 nClientHandle);
	static void		onPacketReceived(void *userData, HINT32 nSocket, HUINT32 ulRequestId, HINT32 nSize, void *pvData);
};

TsReceiver* TsReceiver::_instance = NULL;


TsReceiver::TsReceiver()
{
	HERROR hErr;

	_isRunning = false;
	_taskId = 0;
	_sock	= 0;

	hErr = VK_SEM_Create (&_sema, "NovaReceiverSema", VK_SUSPENDTYPE_PRIORITY);
	if (VK_OK != hErr)
	{
		HxLOG_Error (HxANSI_COLOR_RED("[%s:%d] VK_SEM_Create Semaphore failed!!!\n"), __FUNCTION__, __LINE__);
		_sema = 0;
	}
}

TsReceiver::~TsReceiver()
{
	if (_sema != 0)
	{
		VK_SEM_Destroy(_sema);
	}

	_instance = NULL;

	assert(_sock == 0);
	assert(_isRunning == false);
	assert(_taskId == 0);
}


TsReceiver* TsReceiver::checkInstance()
{
	return _instance;
}

TsReceiver* TsReceiver::instance()
{
	if (_instance == NULL)
	{
		_instance = new TsReceiver();
	}

	return _instance;
}



NOVA_ErrCode_e TsReceiver::init()
{
	NOVA_ErrCode_e  	ErrCode = NERR_ERROR;
	HERROR			hErr = ERR_OK;

	HxLOG_Info("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	if (_isRunning == true)
	{
		HxLOG_Warning("TsReceiver is already initialized\n");
		ErrCode = NERR_INIT;
		goto ErrBlock;
	}

	hErr = PL_RINGBUF_Init();
	if (hErr != ERR_OK)
	{
		HxLOG_Error (HxANSI_COLOR_RED("[%s:%d] PL_RINGBUF_Init failed(hErr:%d)\n"), __FUNCTION__, __LINE__, hErr);
		goto ErrBlock;
	}

	_sock = HLIB_EXTRA_OpenServer(STREAMING_UDS_NAME); // 2015/7/14 mhkang: homma process와 uds name 충돌발생한다. 수정이 필요하다.
	if(_sock == 0)
	{
		HxLOG_Error (HxANSI_COLOR_RED("[%s:%d] HLIB_EXTRA_OpenServer(%s) failed(errno:%d)\n"), __FUNCTION__, __LINE__, STREAMING_UDS_NAME, errno);
		ErrCode = NERR_OPEN;
		goto ErrBlock;
	}


	hErr = VK_TASK_Create (receiverTask, VK_TASK_PRIORITY_MW_REF, SIZE_16K, "NovaReceiverTask", this, &_taskId, 0);
	if (VK_OK != hErr)
	{
		HxLOG_Error (HxANSI_COLOR_RED ("[%s:%d] VK_TASK_Create error(hErr:%d)\n"), __FUNCTION__, __LINE__, hErr);
		ErrCode = NERR_CREATE;
		goto ErrBlock;
	}

	_isRunning = true;

	hErr = VK_TASK_Start(_taskId);
	if (VK_OK != hErr)
	{
		HxLOG_Error (HxANSI_COLOR_RED ("[%s:%d] VK_TASK_Start failed(hErr:%d)\n"), __FUNCTION__, __LINE__, hErr);
		ErrCode = NERR_START;
		goto ErrBlock;
	}

	HxLOG_Info("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return NERR_OK;

ErrBlock:
	if (_sock != 0)
	{
		HLIB_SOCKET_Close(_sock);
		_sock = 0;
	}

	_isRunning = false;

	if (_taskId != 0)
	{
		VK_TASK_Join(_taskId);
		_taskId = 0;
	}

	HxLOG_Info("[%s:%d] (-) Error\n", __FUNCTION__, __LINE__);

	return ErrCode;
}

NOVA_ErrCode_e TsReceiver::deinit()
{
	HxLOG_Info("[%s:%d] (+)\n", __FUNCTION__, __LINE__);


	PL_RINGBUF_DeInit();

	if (_sock != 0)
	{
		HLIB_SOCKET_Close(_sock);
		_sock = 0;
	}

	_isRunning = false;

	if (_taskId != 0)
	{
		VK_TASK_Join(_taskId);
		_taskId = 0;
	}

	HxLOG_Info("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return NERR_OK;
}

void TsReceiver::onPacketReceived(void *userData, HINT32 nSocket, HUINT32 ulRequestId, HINT32 nSize, void *pvData)
{
	HUINT8				*pucData = (HUINT8 *)pvData;
	HUINT32				ulWrittenSize = 0;
	Handle_t			hRingBuf = HANDLE_NULL;
	HERROR 				hErr	= ERR_OK;
	HINT32				nAddPos = 0;
	HINT32				nIndex	= 0;
	HINT32				nDataSize = 0;

	HxSTD_memcpy(&hRingBuf, (pucData+nAddPos), 4);
	nAddPos += 4;

	if(HANDLE_NULL == hRingBuf || 0 == hRingBuf)
	{
		HxLOG_Error (HxANSI_COLOR_RED("Invalid ringbuf [sock:%d, req:%d, size:%d]\r\n"), nSocket, ulRequestId, nSize);
		return;
	}

	HxSTD_memcpy(&nIndex, (pucData+nAddPos), 4);
	nAddPos += 4;

	HxSTD_memcpy(&nDataSize, (pucData+nAddPos), 4);
	nAddPos += 4;

	if(nDataSize != (nSize - nAddPos))
	{
		HxLOG_Error (HxANSI_COLOR_RED("Invalid length [hRingBuf:0x%x] Unmatched data size check! nDataSize(%d) receive size(%d)\r\n"), hRingBuf, nDataSize, (nSize - nAddPos));
	}

	/* 2015/8/28 mhkang: too many log
	printf("[TS]h:%d s:%d\n", hRingBuf, nDataSize);
	*/

#ifdef CONFIG_DEBUG
	if(TRUE == s_bDumpRingbuf)
	{
		HxFILE_t	fp = NULL;

		fp = HLIB_FILE_Open((const HUINT8*)RINGBUF_DUMP_FILENAME, (const HUINT8*)"ab");
		if(fp)
		{
			HLIB_FILE_Write(fp, (pucData + nAddPos), (nDataSize), 1);
			HLIB_FILE_Close(fp);
		}
	}
#endif

	hErr = PL_RINGBUF_WriteData(hRingBuf, (nDataSize), (pucData+nAddPos), &ulWrittenSize);
	if (ERR_OK != hErr)
	{
		HxLOG_Error (HxANSI_COLOR_RED("[%s:%d] PL_RINGBUF_WriteData(hRingBuf:0x%x) failed(err:%d)! [reqid:%d, size:%d, dataSize:%d, WrittenSize:%ld]\n"),__FUNCTION__, __LINE__,
					hRingBuf, hErr, ulRequestId, nSize, nDataSize, ulWrittenSize);
	}
 }

void TsReceiver::onStatusChanged(void *userData, HxSocket_Status_e eStatus, HINT32 nClientHandle)
{
	switch (eStatus)
	{
	case HxSOCKET_ADD_CLIENT:
		HxLOG_Print("HxSOCKET_ADD_CLIENT\r\n");
		break;

	case HxSOCKET_REMOVE_CLIENT:
		HxLOG_Print("HxSOCKET_REMOVE_CLIENT\r\n");
		break;

	case HxSOCKET_DISCONNECT_CLIENT:
		HxLOG_Print("HxSOCKET_DISCONNECT_CLIENT\r\n");
		break;

	default:
		break;
	}
}

void TsReceiver::receiverTask(void *param)
{
	TsReceiver* receiver;

	HxLOG_Info("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	receiver = static_cast<TsReceiver*>(param);
	if (receiver == NULL)
	{
		HxLOG_Error (HxANSI_COLOR_RED("[%s:%d]Invalid args\n"), __FUNCTION__, __LINE__);
		return;
	}

	if (receiver->_sock == 0)
	{
		HxLOG_Error (HxANSI_COLOR_RED("[%s:%d] streaming sock is 0\n"), __FUNCTION__, __LINE__);
		return;
	}

	while(receiver->_isRunning == TRUE)
	{

		HLIB_SOCKET_Listen(receiver->_sock, TRUE, onPacketReceived, onStatusChanged, receiver);
		VK_TASK_Sleep(1); /* Already socket blocked with SOCKET_FOREVER */
	}

	HxLOG_Info("[%s:%d] (-)\n", __FUNCTION__, __LINE__);
}


/**********************************************************************************************************/
#define ____DEBUG_FUNCTIONS_______________________
/**********************************************************************************************************/

#if defined(CONFIG_DEBUG)
static  HINT32   _plcmd_streaming_Ringbuf_dump(void *szArgList)
{
	s_bDumpRingbuf = (s_bDumpRingbuf == TRUE) ? FALSE : TRUE;
	if(TRUE == s_bDumpRingbuf)
	{
		HLIB_CMD_Printf("ringbuf dump enabled\n");
		HLIB_FILE_Delete((const HUINT8*)RINGBUF_DUMP_FILENAME);
	}
	else
	{
		HLIB_CMD_Printf("ringbuf dump disabled\n");
	}

	return HxCMD_OK;
}
#endif

extern void	NOVA_CMDRegister_StreamingReceive(const HCHAR *hCmdHandle)
{
#if defined(CONFIG_DEBUG)
	HLIB_CMD_RegisterWord( hCmdHandle, _plcmd_streaming_Ringbuf_dump, "dump_ringbuf", "toggle true/false", "received ringbuf dump");
#endif
}

/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/
NOVA_ErrCode_e NOVA_Receiver_Init(void)
{
	NOVA_ErrCode_e  ErrCode = NERR_ERROR;

	HxLOG_Info("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	if (TsReceiver::checkInstance() != NULL)
	{
		HxLOG_Error (HxANSI_COLOR_RED("[%s:%d] already initialized.\n"), __FUNCTION__, __LINE__);
		ErrCode = NERR_INIT;
	}
	else
	{
		ErrCode = TsReceiver::instance()->init();
	}

	HxLOG_Info("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ErrCode;
}

NOVA_ErrCode_e 	NOVA_Receiver_DeInit(void)
{
	NOVA_ErrCode_e ErrCode = NERR_ERROR;

	HxLOG_Info("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	if (TsReceiver::checkInstance() != NULL)
	{
		ErrCode = TsReceiver::instance()->deinit();
		delete TsReceiver::instance();
	}
	else
	{
		HxLOG_Error (HxANSI_COLOR_RED("[%s:%d] ts receiver was not created.\n"), __FUNCTION__, __LINE__);
		ErrCode = NERR_DEINIT;
	}

	HxLOG_Info("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ErrCode;
}


/*******************************************************************************
* File name : drv_channel_tsmf.c
* Author :
* description :
*
* Copyright (c)  by Humax Co., Ltd. Since 2011.
* All right reserved
*******************************************************************************/

/*******************************************************************************
* Header and definitions
*******************************************************************************/
/* global header files */
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<string.h>
#endif
#include <fcntl.h>
#include <sys/mman.h>

/* chip manufacture's  header files */
#include "nexus_platform.h"
#include "nexus_input_band.h"
#include "nexus_parser_band.h"
#include "nexus_pid_channel.h"
#include "nexus_packetsub.h"
#include "nexus_message.h"
#include "nexus_remux.h"
#include "nexus_memory.h"
#include "nexus_pid_channel_scrambling.h"
#include "berr.h"
#include "blst_squeue.h"

/* humax header files */
#include "linden_trace.h"
#include "htype.h"
#include "vkernel.h"
#include "taskdef.h"
#include "drv_err.h"
#include "drv_demux.h"
#include "di_err.h"
#include "di_channel_priv.h"

/*******************************************************************************
* Debug level
*******************************************************************************/
#define DEBUG_MODULE	TRACE_MODULE_DI_CHANNEL


/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#define DRV_CH_TSMF_PID 0x2F
#define DRV_CH_TSMF_FRAMESYNC_1 0x1A86
#define DRV_CH_TSMF_FRAMESYNC_2 0x0579

#define DRV_CH_TSMF_MSGQ_SIZE	 (MSGQ_SIZE_NORMAL)
#define DRV_CH_TSMF_TASK_PRIORITY	(USER_PRIORITY10)
#define DRV_CH_TSMF_TASK_STACK_SIZE	(NORMAL_TASK_STACK_SIZE)

#define DRV_CH_TSMF_NUM_OF_MONITOR_PARSER	(1)
#define MAX_PARSER_BAND_NUM				NEXUS_NUM_PARSER_BANDS

#define DRV_CH_TSMF_MANAGED_FILTER_SIZE 		(64*1024)

#define DRV_CH_TSMF_MONITOR_READY_TIME	(1500) /*1500ms*/

#define DRV_CH_TSMF_MONITOR_PERIOD	(500)	//500ms
#define DRV_CH_TSMF_SEARCH_TIMEOUT	(50)	//200ms
#define DRV_CH_TSMF_MONITOR_TIMEOUT	(50)	//200ms

typedef struct P_CH_TSMF_RESOURCE
{
	HUINT32	unDemuxId;
	HUINT32 unDemodID;
	HUINT32 unRequestID;
	HBOOL bEnabled;
	DRV_CH_TSMF_STATUS_e eStatus;
	NEXUS_PidChannelHandle pidHandle;
	NEXUS_MessageHandle msgHandle;
}P_CH_TSMF_RESOURCE_t;

typedef struct P_CH_TSMF_CONTROL_OBJECT
{
	HULONG ulTSEvtTaskId;
	HULONG ulTSEvtMsgId;
	HULONG ulTSEvtLock[CONFIG_NUM_OF_CAB_CHANNEL];

	HULONG ulTSMFTaskId;
	HULONG ulTSMFMsgId;
	HULONG ulTSMFTSEvtMsgId;
	HULONG ulTSMFLock;

	HULONG ulTSMFTimerId;
}P_CH_TSMF_CONTROL_OBJECT_t;

typedef enum P_CH_TSMF_MODE
{
	P_CH_TSMF_MODE_MONITORING = 0,
	P_CH_TSMF_MODE_MONITORING_FIRST,
	P_CH_TSMF_MODE_SEARCHING,
	P_CH_TSMF_MODE_SEARCHING_FIRST,
	P_CH_TSMF_MODE_DEBUGGING,
	P_CH_TSMF_MODE_UNKNOWN
} P_CH_TSMF_MODE_e;

typedef struct P_CH_TSMF
{
	pfnCH_TSMFNotify pfnTSMFNotify;
	P_CH_TSMF_RESOURCE_t stResource[CONFIG_NUM_OF_CAB_CHANNEL];
	P_CH_TSMF_CONTROL_OBJECT_t stControlObj;
	HBOOL bRequestSearch;
	HUINT8 ucSearchDemodId;
	HBOOL bSetSearchPath;
	HBOOL bMonSearch;
	HINT32 nDemuxId;
	HUINT8 ucMonDemodId;
}P_CH_TSMF_t;

typedef enum P_CH_TSMF_COMMAND
{
	P_CH_TSMF_COMMAND_SEARCH_TSMF = 0,
	P_CH_TSMF_COMMAND_ENABLE_MONITOR,
	P_CH_TSMF_COMMAND_CANCLE_SEARCH,
	P_CH_TSMF_COMMAND_DISABLE_MONITOR,
	P_CH_TSMF_COMMAND_NEW_ARRIVED_TSMF,
	P_CH_TSMF_COMMAND_DEBUG,
	P_CH_TSMF_COMMAND_MAX
} P_CH_TSMF_COMMAND_e;

typedef struct P_CH_TSEVT_MSG
{
	HUINT32 unReqID;
	HUINT32 unFilterIDX;
}P_CH_TSEVT_MSG_t;

typedef struct P_CH_TSMF_MSG
{
	P_CH_TSMF_COMMAND_e eCommand;
	HUINT8	ucTSMFIndex;
	HUINT32 unRequestID;
	HUINT32 unTSBufferAddress;
}P_CH_TSMF_MSG_t;


/*******************************************************************************
* Global variables and structures
*******************************************************************************/


/*******************************************************************************
* External variables and functions
*******************************************************************************/
extern HINT32 show_di_ch_tsmf;


/*******************************************************************************
* Static variables and structures
*******************************************************************************/
static P_CH_TSMF_t s_tTSMFInstance;
static HUINT8 s_ucNumMonitorParser = DRV_CH_TSMF_NUM_OF_MONITOR_PARSER;
static HUINT32 s_unRequestID;
static void *s_pFilterBuff;

#if (NEXUS_VERSION >= 1290)
static HUINT32 s_ulTsmfHandle[CONFIG_NUM_OF_CAB_CHANNEL];
#endif

/*******************************************************************************
* Static function prototypes
*******************************************************************************/
static DRV_Error P_CH_TSMF_PrepareTSHandling(void);
static void P_CH_TSMF_TSEvtTask(void* param);
static void P_CH_TSMF_TS_NotifyCB(void *context, int param);
static void P_CH_TSMF_TS_OverflowCB(void *context, int param);
static DRV_Error P_CH_TSMF_AllocTSFilter(HUINT8 ucTSMFIdx);
static DRV_Error P_CH_TSMF_InitTSMFModule(void);
static DRV_Error P_CH_TSMF_EnableTSFilter(HUINT8 ucTSMFIdx, HUINT32 unRequestID);
static DRV_Error P_CH_TSMF_DisableTSFilter(HUINT8 ucTSMFIdx);
static void P_CH_TSMF_MonTask(void *param);
static DRV_Error P_CH_TSMF_SetMTSIFPath(HUINT32 ulInputDeviceId, NEXUS_ParserBand eParserBand, HBOOL bPathEnable);


/*******************************************************************************
* function : DRV_CH_TSMF_Init
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_CH_TSMF_Init(void)
{
	DRV_Error eDrvRet;

	eDrvRet = P_CH_TSMF_PrepareTSHandling();
	if(eDrvRet != DRV_OK)
	{
		PrintError("P_CH_TSMF_PrepareTSHandling() error\n");
		return DRV_ERR;
	}

	eDrvRet = P_CH_TSMF_InitTSMFModule();
	if(eDrvRet != DRV_OK)
	{
		PrintError("P_CH_TSMF_PrepareTSHandling() error\n");
		return DRV_ERR;
	}

	// Using One Parserband for TSMF moniotring
	eDrvRet = P_CH_TSMF_AllocTSFilter(0);
	if(eDrvRet != DRV_OK)
	{
		PrintError("P_CH_TSMF_PrepareTSHandling() error\n");
		return DRV_ERR;
	}

	return DRV_OK;
}

/*******************************************************************************
* function : DRV_CH_TSMF_RegisterTSMFCallback
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_CH_TSMF_RegisterTSMFCallback (pfnCH_TSMFNotify pfnTSMFNotify)
{

	if((s_tTSMFInstance.pfnTSMFNotify != pfnTSMFNotify) && ( pfnTSMFNotify != NULL))
	{
		VK_SEM_Get(s_tTSMFInstance.stControlObj.ulTSMFLock);
		s_tTSMFInstance.pfnTSMFNotify = pfnTSMFNotify;
		VK_SEM_Release(s_tTSMFInstance.stControlObj.ulTSMFLock);
	}
	else
	{
		/* callback is already regitered */
	}
	return DRV_OK;
}

/*******************************************************************************
* function : DRV_CH_TSMF_RequestSearch
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_CH_TSMF_RequestSearch(HUINT32 nDemodId)
{
	P_CH_TSMF_MSG_t tsmfMsg;

	VK_SEM_Get(s_tTSMFInstance.stControlObj.ulTSMFLock);
	s_tTSMFInstance.bRequestSearch = TRUE;
	s_tTSMFInstance.stResource[nDemodId].eStatus = DRV_CH_TSMF_SEARCHING;
	VK_SEM_Release(s_tTSMFInstance.stControlObj.ulTSMFLock);

	tsmfMsg.eCommand = P_CH_TSMF_COMMAND_SEARCH_TSMF;
	tsmfMsg.ucTSMFIndex = (HUINT8)nDemodId;

	CH_DI_TSMF_Print(8,("DRV_CH_TSMF_RequestSearch() : ucMaxDemod(%d), RequestTSMFidx(%d)\n", CONFIG_NUM_OF_CAB_CHANNEL, tsmfMsg.ucTSMFIndex));

	VK_MSG_SendTimeout(s_tTSMFInstance.stControlObj.ulTSMFMsgId, &tsmfMsg, sizeof(P_CH_TSMF_MSG_t), 0);

	return DRV_OK;
}

/*******************************************************************************
* function : DRV_CH_TSMF_CancleSearch
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_CH_TSMF_CancleSearch(HUINT32 nDemodId)
{
	P_CH_TSMF_MSG_t tsmfMsg;

	tsmfMsg.eCommand = P_CH_TSMF_COMMAND_CANCLE_SEARCH;
	tsmfMsg.ucTSMFIndex = (HUINT8)nDemodId;

	CH_DI_TSMF_Print(8,("DRV_CH_TSMF_CancleSearch()!! , CancleTSMFidx(%d)\n", tsmfMsg.ucTSMFIndex));

	VK_MSG_SendTimeout(s_tTSMFInstance.stControlObj.ulTSMFMsgId, &tsmfMsg, sizeof(P_CH_TSMF_MSG_t), 0);

	return DRV_OK;
}

/*******************************************************************************
* function : DRV_CH_TSMF_EnableMonitor
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_CH_TSMF_EnableMonitor(void)
	{
	P_CH_TSMF_MSG_t tsmfMsg;

	tsmfMsg.eCommand = P_CH_TSMF_COMMAND_ENABLE_MONITOR;

	CH_DI_TSMF_Print(8,("DRV_CH_TSMF_EnableMonitor()!!\n"));

	VK_MSG_SendTimeout(s_tTSMFInstance.stControlObj.ulTSMFMsgId, &tsmfMsg, sizeof(P_CH_TSMF_MSG_t), 0);

	return DRV_OK;
}

/*******************************************************************************
* function : DRV_CH_TSMF_DisableMonitor
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_CH_TSMF_DisableMonitor(void)
{
	P_CH_TSMF_MSG_t tsmfMsg;

	tsmfMsg.eCommand = P_CH_TSMF_COMMAND_DISABLE_MONITOR;

	CH_DI_TSMF_Print(8,("DRV_CH_TSMF_DisableMonitor()!!\n"));

	VK_MSG_SendTimeout(s_tTSMFInstance.stControlObj.ulTSMFMsgId, &tsmfMsg, sizeof(P_CH_TSMF_MSG_t), 0);

	return DRV_OK;
}

/*******************************************************************************
* function : DRV_CH_TSMF_GetStatus
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_CH_TSMF_GetStatus(HUINT8 ucTSMFIdx, DRV_CH_TSMF_STATUS_e *peStatus)
{
	if((peStatus == NULL) || (ucTSMFIdx >= CONFIG_NUM_OF_CAB_CHANNEL))
	{
		return DRV_ERR;
	}
	else
	{
		VK_SEM_Get(s_tTSMFInstance.stControlObj.ulTSMFLock);
		*peStatus = s_tTSMFInstance.stResource[ucTSMFIdx].eStatus;
		VK_SEM_Release(s_tTSMFInstance.stControlObj.ulTSMFLock);

		CH_DI_TSMF_Print(0,("TSMF_STATUS[%d] = %s \n", ucTSMFIdx, s_tTSMFInstance.stResource[ucTSMFIdx].eStatus == DRV_CH_TSMF_IDLE ? "IDLE" : s_tTSMFInstance.stResource[ucTSMFIdx].eStatus == DRV_CH_TSMF_SEARCHING ? "SEARCHING" : s_tTSMFInstance.stResource[ucTSMFIdx].eStatus == DRV_CH_TSMF_RECEIVED ? "RECEIVED" : s_tTSMFInstance.stResource[ucTSMFIdx].eStatus == DRV_CH_TSMF_TIMEOUT ?"TIMEOUT":"WAIT_LIVEDEMOD"));
	}

	return DRV_OK;
}

#if (NEXUS_VERSION >= 1290)
/*******************************************************************************
* function :DRV_CH_TSMF_GetHandle
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_CH_TSMF_GetHandle(HUINT32 ulDemodId, HUINT32* pulTsmfHandle)
{
	if ((ulDemodId >= CONFIG_NUM_OF_CAB_CHANNEL) || (pulTsmfHandle == NULL))
	{
		CH_DI_TSMF_Print(0, ("Error! GetTsmfHandle() ulDemodId = %d, s_pulTsmfHandle[%d] is NULL\n", ulDemodId, ulDemodId));
		return DRV_ERR;
	}

	*pulTsmfHandle = s_ulTsmfHandle[ulDemodId];
	CH_DI_TSMF_Print(9, ("GetTsmfHandle() ulDemodId = %d, loaded pulTsmfHandle = %x\n", ulDemodId, *pulTsmfHandle));

	return DRV_OK;
}

/*******************************************************************************
* function :DRV_CH_TSMF_SetHandle
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_CH_TSMF_SetHandle(HUINT32 ulDemodId, HUINT32 ulTsmfHandle)
{
	if (ulDemodId >= CONFIG_NUM_OF_CAB_CHANNEL)
	{
		CH_DI_TSMF_Print(0, ("Error! SetTsmfHandle() ulDemodId = %d\n", ulDemodId));
		return DRV_ERR;
	}

	s_ulTsmfHandle[ulDemodId] = ulTsmfHandle;
	CH_DI_TSMF_Print(9, ("SetTsmfHandle() ulDemodId = %d, Saved pulTsmfHandle = %x\n", ulDemodId, s_ulTsmfHandle[ulDemodId]));

	return DRV_OK;
}
#endif

/*******************************************************************************
* function : P_CH_TSMF_PrepareTSHandling
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static DRV_Error P_CH_TSMF_PrepareTSHandling(void)
{
	HUINT32 err;
	HUINT8 ucIdx;

	for(ucIdx=0; ucIdx<s_ucNumMonitorParser; ucIdx++)
	{
		err = VK_SEM_Create(&s_tTSMFInstance.stControlObj.ulTSEvtLock[ucIdx], "TSEVT_SEM", VK_SUSPENDTYPE_FIFO);
		if(err)
		{
			PrintError("VK_SEM_Create() error, TSEVT_SEM\n");
			return DRV_ERR;
		}
	}

	if(VK_MSG_Create(DRV_CH_TSMF_MSGQ_SIZE, sizeof(P_CH_TSEVT_MSG_t),"TsEvtMsg",&s_tTSMFInstance.stControlObj.ulTSEvtMsgId,VK_SUSPENDTYPE_FIFO))
	{
		for(ucIdx=0; ucIdx<s_ucNumMonitorParser; ucIdx++)
		{
			VK_SEM_Destroy(s_tTSMFInstance.stControlObj.ulTSEvtLock[ucIdx]);
		}
		PrintError("VK_MSG_Create error\n");
		return DRV_ERR;
	}

	err = VK_TASK_Create(P_CH_TSMF_TSEvtTask,
						DRV_CH_TSMF_TASK_PRIORITY,
						DRV_CH_TSMF_TASK_STACK_SIZE,
						"TsEvtTask",
						NULL,
						&s_tTSMFInstance.stControlObj.ulTSEvtTaskId, FALSE);
	if( err != VK_OK )
	{
		VK_MSG_Destroy(s_tTSMFInstance.stControlObj.ulTSEvtMsgId);
		for(ucIdx=0; ucIdx<s_ucNumMonitorParser; ucIdx++)
		{
			VK_SEM_Destroy(s_tTSMFInstance.stControlObj.ulTSEvtLock[ucIdx]);
		}

		PrintError("Error(0x%x) in VK_TASK_Create(TsEvtTask)\n\r", err);
		return DRV_ERR;
	}

	err = VK_TASK_Start(s_tTSMFInstance.stControlObj.ulTSEvtTaskId);
	if(err != VK_OK)
	{
		VK_MSG_Destroy(s_tTSMFInstance.stControlObj.ulTSEvtMsgId);
		for(ucIdx=0; ucIdx<s_ucNumMonitorParser; ucIdx++)
		{
			VK_SEM_Destroy(s_tTSMFInstance.stControlObj.ulTSEvtLock[ucIdx]);
		}

		VK_TASK_Destroy(s_tTSMFInstance.stControlObj.ulTSEvtTaskId);
		PrintError("P_CH_PrepareTSHandling() Error(0x%x) in VK_TASK_Start(TsEvtTask)\n\r", err);
		return DRV_ERR;
	}

	return DRV_OK;
}


/*******************************************************************************
* function : P_CH_TSMF_TSEvtTask
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static void P_CH_TSMF_TSEvtTask(void* param)
{
	HUINT32 unMessage_length;
	HUINT32 unFilter_index;
	HUINT32 unRequestId;
	HUINT8 *pucBuffer;
	size_t size;
	NEXUS_Error eErr;
	int vk_ret;
	P_CH_TSEVT_MSG_t stTSEvtmsg;
	P_CH_TSMF_MSG_t stTSMFMsg;
	HUINT8 *pucTSMFBuff;

	BSTD_UNUSED(param);

	while(TRUE)
	{

		vk_ret = VK_MSG_Receive(s_tTSMFInstance.stControlObj.ulTSEvtMsgId, &stTSEvtmsg, sizeof(P_CH_TSEVT_MSG_t));
		if(vk_ret != VK_OK)
		{
			PrintError("P_CH_TSMF_TSEvtTask():VK_MSG_Receive returns error. (err=%ld)\n", vk_ret);
			continue;
		}
		unFilter_index = stTSEvtmsg.unFilterIDX;
		unRequestId = stTSEvtmsg.unReqID;

		CH_DI_TSMF_Print(10,("P_CH_TSMF_TSEvtTask(), filter_index = %d\n", unFilter_index));

		unMessage_length = 0;
		VK_SEM_Get(s_tTSMFInstance.stControlObj.ulTSEvtLock[0]);
		if((s_tTSMFInstance.stResource[unFilter_index].bEnabled == TRUE) && (s_tTSMFInstance.stResource[unFilter_index].unRequestID == unRequestId))
		{
		if((eErr=NEXUS_Message_GetBuffer(s_tTSMFInstance.stResource[0].msgHandle, (const void **)&pucBuffer, &size)))
		{
			VK_SEM_Release(s_tTSMFInstance.stControlObj.ulTSEvtLock[0]);
			PrintError("P_CH_TSMF_TSEvtTask(), NEXUS_Message_GetBuffer() return error (%ld)\n",eErr);
			continue;
		}

		if(!size)
		{
			/*size can be zero in normal condition. PrintError("P_DEMUX_SectionEventTask(), NEXUS_Message_GetBuffer() read size 0\n");*/
			VK_SEM_Release(s_tTSMFInstance.stControlObj.ulTSEvtLock[0]);
			continue;
		}

		unMessage_length = 188; /*hardcoded*/

		if(unMessage_length > size)
		{
			CH_DI_TSMF_Print(8,("P_CH_TSMF_TSEvtTask(), message length and read size are different %d-%d\n", unMessage_length, size));
			//continue;
		}

		CH_DI_TSMF_Print(9,("[ts_recv] : \n"));
		CH_DI_TSMF_Print(9,("%02x|%02x|%02x|%02x|%02x|%02x \n", pucBuffer[0], pucBuffer[1], pucBuffer[2], pucBuffer[3], pucBuffer[4], pucBuffer[5]));

		if((unMessage_length == size)&&(((pucBuffer[1]&0x1F)<<8 | pucBuffer[2]) == DRV_CH_TSMF_PID) && ((((pucBuffer[4]&0x1F)<<8 | pucBuffer[5]) == DRV_CH_TSMF_FRAMESYNC_1) || (((pucBuffer[4]&0x1F)<<8 | pucBuffer[5]) == DRV_CH_TSMF_FRAMESYNC_2)))
		{
			pucTSMFBuff = DD_MEM_Alloc(unMessage_length);
			VK_memcpy(pucTSMFBuff, pucBuffer, unMessage_length);

			NEXUS_Message_ReadComplete(s_tTSMFInstance.stResource[0].msgHandle, size);
			VK_SEM_Release(s_tTSMFInstance.stControlObj.ulTSEvtLock[0]);

			//tsmf callback alloc & copy & send msg
			stTSMFMsg.eCommand = P_CH_TSMF_COMMAND_NEW_ARRIVED_TSMF;
			stTSMFMsg.ucTSMFIndex = unFilter_index;
			stTSMFMsg.unRequestID = unRequestId;
			stTSMFMsg.unTSBufferAddress = (HUINT32)pucTSMFBuff;
			VK_MSG_SendTimeout(s_tTSMFInstance.stControlObj.ulTSMFTSEvtMsgId, &stTSMFMsg, sizeof(P_CH_TSMF_MSG_t), 0);
		}
		else
		{
			NEXUS_Message_ReadComplete(s_tTSMFInstance.stResource[0].msgHandle, size);
			VK_SEM_Release(s_tTSMFInstance.stControlObj.ulTSEvtLock[0]);
		}
	}
		else
		{
			if((eErr=NEXUS_Message_GetBuffer(s_tTSMFInstance.stResource[0].msgHandle, (const void **)&pucBuffer, &size)))
			{
				VK_SEM_Release(s_tTSMFInstance.stControlObj.ulTSEvtLock[0]);
				PrintError("P_CH_TSMF_TSEvtTask(), NEXUS_Message_GetBuffer() return error (%ld)\n",eErr);
				continue;
			}

			NEXUS_Message_ReadComplete(s_tTSMFInstance.stResource[0].msgHandle, size);
			VK_SEM_Release(s_tTSMFInstance.stControlObj.ulTSEvtLock[0]);
			PrintDebug("P_DEMUX_TsEventTask(), filter_index = %d is already stopped \n", unFilter_index);
		}
	}
}

/*******************************************************************************
* function : P_CH_TSMF_TS_NotifyCB
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static void P_CH_TSMF_TS_NotifyCB(void *context, int param)
{
	int ret;
	P_CH_TSEVT_MSG_t msg;

	msg.unFilterIDX = (HUINT32)context;
	msg.unReqID = (HUINT32)param;
	PrintDebug("P_CH_TSMF_TS_NotifyCB(), filter index = %d reqid=%ld\n", msg.unFilterIDX, msg.unReqID );
	ret = VK_MSG_SendTimeout(s_tTSMFInstance.stControlObj.ulTSEvtMsgId, &msg, sizeof(P_CH_TSEVT_MSG_t),0);
	if(ret == VK_TIMEOUT)
	{
		PrintError("Timeout occured! (filter_index = %d)\n",param);
	}
}

/*******************************************************************************
* function : P_CH_TSMF_TS_OverflowCB
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static void P_CH_TSMF_TS_OverflowCB(void *context, int param)
{
	PrintError("P_CH_TSMF_TS_OverflowCB(), context = 0x%lx, param = %ld\n", context, param);
}

/*******************************************************************************
* function : P_CH_TSMF_AllocTSFilter
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static DRV_Error P_CH_TSMF_AllocTSFilter(HUINT8 ucTSMFIdx)
{
	NEXUS_ParserBand parserBand;
	NEXUS_PidChannelHandle pidHandle;
	NEXUS_PidChannelSettings PidSettings;
	NEXUS_MessageSettings openSettings;
	NEXUS_MessageStartSettings startSettings;
	NEXUS_MessageHandle msgHandle;
	NEXUS_MemoryAllocationSettings memAllocationSettings;
	NEXUS_Error nexusError;

	VK_SEM_Get(s_tTSMFInstance.stControlObj.ulTSEvtLock[0]);
	parserBand = (NEXUS_ParserBand)s_tTSMFInstance.stResource[ucTSMFIdx].unDemuxId;

	if(s_pFilterBuff == NULL)
	{
	NEXUS_Memory_GetDefaultAllocationSettings(&memAllocationSettings);
	memAllocationSettings.alignment = 1024;
		nexusError = NEXUS_Memory_Allocate(DRV_CH_TSMF_MANAGED_FILTER_SIZE, &memAllocationSettings, &s_pFilterBuff);
	if(nexusError != BERR_SUCCESS)
	{
			VK_SEM_Release(s_tTSMFInstance.stControlObj.ulTSEvtLock[0]);
		PrintError(" Failed allocating from Nexus Heap. (%d)\n", nexusError);
		return DRV_ERR;
	}
	}

	NEXUS_PidChannel_GetDefaultSettings(&PidSettings);

	PidSettings.enabled = true;
	PidSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_MESSAGE_CAPABLE;

	pidHandle = NEXUS_PidChannel_Open(parserBand, DRV_CH_TSMF_PID, &PidSettings);
	if(pidHandle == NULL)
	{
		VK_SEM_Release(s_tTSMFInstance.stControlObj.ulTSEvtLock[0]);
		PrintError(" pid channel %d is not allocated or already enabled\n", ucTSMFIdx);
		return DRV_ERR;
	}
	else
	{
		s_tTSMFInstance.stResource[ucTSMFIdx].pidHandle = pidHandle;
	}

	NEXUS_Message_GetDefaultSettings(&openSettings);

	openSettings.dataReady.callback = P_CH_TSMF_TS_NotifyCB;
	openSettings.dataReady.context = 0;
	openSettings.dataReady.param = 0;
	openSettings.overflow.callback = P_CH_TSMF_TS_OverflowCB;
	openSettings.overflow.context = 0;
	openSettings.overflow.param = 0;
	openSettings.bufferSize =0;

	msgHandle = NEXUS_Message_Open(&openSettings);
	if(msgHandle == NULL)
	{
		NEXUS_PidChannel_Close(pidHandle);
		s_tTSMFInstance.stResource[ucTSMFIdx].pidHandle = NULL;
		VK_SEM_Release(s_tTSMFInstance.stControlObj.ulTSEvtLock[0]);
		PrintError(" NEXUS_Message_Open() return error\n");
		return DRV_ERR;
	}
	else
	{
		s_tTSMFInstance.stResource[ucTSMFIdx].msgHandle = msgHandle;
		NEXUS_Message_GetSettings(s_tTSMFInstance.stResource[0].msgHandle, &openSettings);
		openSettings.dataReady.callback = P_CH_TSMF_TS_NotifyCB;
		openSettings.dataReady.context = (void *)((HUINT32)ucTSMFIdx);
		openSettings.dataReady.param = 0xFFFFFFFF;
		openSettings.overflow.callback = P_CH_TSMF_TS_OverflowCB;
		openSettings.overflow.context = (void *)((HUINT32)ucTSMFIdx);
		openSettings.overflow.param = 0xFFFFFFFF;
		openSettings.bufferSize =0;
		NEXUS_Message_SetSettings(s_tTSMFInstance.stResource[0].msgHandle, &openSettings);

		NEXUS_Message_GetDefaultStartSettings(s_tTSMFInstance.stResource[0].msgHandle,&startSettings);
		startSettings.pidChannel = s_tTSMFInstance.stResource[0].pidHandle;
		startSettings.format = NEXUS_MessageFormat_eTs;
		startSettings.filterGroup = false;
		startSettings.bufferMode = NEXUS_MessageBufferMode_eOneMessage; //NEXUS_MessageBufferMode_eContinuous;	//NEXUS_MessageBufferMode_eOneMessage
	//	startSettings.bufferMode = NEXUS_MessageBufferMode_eContinuous;	//NEXUS_MessageBufferMode_eOneMessage
		startSettings.bank = -1;
		startSettings.psiCrcDisabled = false;
		startSettings.psfCrcDisabled = false;
		startSettings.buffer = s_pFilterBuff;
		startSettings.bufferSize = DRV_CH_TSMF_MANAGED_FILTER_SIZE;

		if(NEXUS_Message_Start(s_tTSMFInstance.stResource[0].msgHandle, &startSettings) != NEXUS_SUCCESS)
		{
			VK_SEM_Release(s_tTSMFInstance.stControlObj.ulTSEvtLock[0]);
			PrintError(" NEXUS_Message_Start() return error\n");
			return DRV_ERR;
		}
	}

	VK_SEM_Release(s_tTSMFInstance.stControlObj.ulTSEvtLock[0]);

	return DRV_OK;
}

/*******************************************************************************
* function : P_CH_TSMF_EnableTSFilter
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static DRV_Error P_CH_TSMF_EnableTSFilter(HUINT8 ucTSMFIdx, HUINT32 unRequestID)
{
	NEXUS_MessageSettings openSettings;

	VK_SEM_Get(s_tTSMFInstance.stControlObj.ulTSEvtLock[0]);


	NEXUS_Message_GetSettings(s_tTSMFInstance.stResource[0].msgHandle, &openSettings);
	openSettings.dataReady.callback = P_CH_TSMF_TS_NotifyCB;
	openSettings.dataReady.context = (void *)((HUINT32)ucTSMFIdx);
	openSettings.dataReady.param = unRequestID;
	openSettings.overflow.callback = P_CH_TSMF_TS_OverflowCB;
	openSettings.overflow.context = (void *)((HUINT32)ucTSMFIdx);
	openSettings.overflow.param = unRequestID;
	openSettings.bufferSize =0;
	NEXUS_Message_SetSettings(s_tTSMFInstance.stResource[0].msgHandle, &openSettings);

	s_tTSMFInstance.stResource[ucTSMFIdx].bEnabled = TRUE;
	VK_SEM_Release(s_tTSMFInstance.stControlObj.ulTSEvtLock[0]);

	return DRV_OK;
}

/*******************************************************************************
* function : P_CH_TSMF_DisableTSFilter
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static DRV_Error P_CH_TSMF_DisableTSFilter(HUINT8 ucTSMFIdx)
{

	VK_SEM_Get(s_tTSMFInstance.stControlObj.ulTSEvtLock[0]);
	s_tTSMFInstance.stResource[ucTSMFIdx].bEnabled = FALSE;

	VK_SEM_Release(s_tTSMFInstance.stControlObj.ulTSEvtLock[0]);
	return DRV_OK;
}

/*******************************************************************************
* function : P_CH_TSMF_Search
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static DRV_Error P_CH_TSMF_Search(HUINT8 ucTSMFIdx, P_CH_TSMF_MODE_e eMode)
{
	P_CH_TSMF_MSG_t tsmftsevtMsg;
	HUINT32 ulInputDevice_id;
	NEXUS_ParserBand parser_band;
	DRV_Error eDrvError;
	HUINT8* pucTSBuff;
	int vk_ret;
	unsigned long ulTimeout = 0xFFFFFFFF;
	int tick1, tick2;

	VK_SEM_Get(s_tTSMFInstance.stControlObj.ulTSMFLock);
	s_tTSMFInstance.stResource[ucTSMFIdx].eStatus = DRV_CH_TSMF_SEARCHING;
	VK_SEM_Release(s_tTSMFInstance.stControlObj.ulTSMFLock);

	ulInputDevice_id = s_tTSMFInstance.stResource[ucTSMFIdx].unDemodID;
	parser_band = (NEXUS_ParserBand)s_tTSMFInstance.stResource[ucTSMFIdx].unDemuxId;

	if((eMode == P_CH_TSMF_MODE_SEARCHING_FIRST)||(eMode == P_CH_TSMF_MODE_SEARCHING))
	{
		ulTimeout = DRV_CH_TSMF_SEARCH_TIMEOUT;
	}
	else
	{
		ulTimeout = DRV_CH_TSMF_MONITOR_TIMEOUT;
	}

	if((eMode == P_CH_TSMF_MODE_SEARCHING_FIRST)||(eMode == P_CH_TSMF_MODE_MONITORING_FIRST))
	{
	/* Set Path */
	eDrvError = P_CH_TSMF_SetMTSIFPath(ulInputDevice_id, parser_band, TRUE);
	if(eDrvError != DRV_OK)
	{
		PrintError(" Failed to set main ParserBand (%d)\n", eDrvError);
		return DRV_ERR;
	}

		if(eMode == P_CH_TSMF_MODE_SEARCHING_FIRST)
		{
			VK_SEM_Get(s_tTSMFInstance.stControlObj.ulTSMFLock);
			s_tTSMFInstance.bSetSearchPath = TRUE;
			VK_SEM_Release(s_tTSMFInstance.stControlObj.ulTSMFLock);
		}

		if(eMode == P_CH_TSMF_MODE_MONITORING_FIRST)
		{
			VK_SEM_Get(s_tTSMFInstance.stControlObj.ulTSMFLock);
			s_tTSMFInstance.bMonSearch = TRUE;
			VK_SEM_Release(s_tTSMFInstance.stControlObj.ulTSMFLock);
		}
	}

	s_tTSMFInstance.stResource[ucTSMFIdx].unRequestID = s_unRequestID++;

	/* Enable PID Channel */
	eDrvError = P_CH_TSMF_EnableTSFilter(ucTSMFIdx, s_tTSMFInstance.stResource[ucTSMFIdx].unRequestID);
	if(eDrvError != DRV_OK)
	{
		PrintError(" Failed to disable PID channel(%d)\n", eDrvError);
		return DRV_ERR;
	}

	CH_DI_TSMF_Print(7,(">>>>>>>>Change MTSIF Path to ulInputDevice_id(%d)\n", ulInputDevice_id));

	tick1 = VK_TIMER_GetSystemTick();
	CH_DI_TSMF_Print(7,(">> tick1 (%d)\n", tick1));

	while(TRUE)
	{
		vk_ret = VK_MSG_ReceiveTimeout(s_tTSMFInstance.stControlObj.ulTSMFTSEvtMsgId, &tsmftsevtMsg, sizeof(P_CH_TSMF_MSG_t), ulTimeout);
		if(vk_ret != VK_OK)
		{
			if(vk_ret != VK_TIMEOUT)
			{
				PrintError("VK_MSG_ReceiveTimeOut(tsmftsevtMsg) returns error. (err=%ld)\n", vk_ret);
				continue;
			}
			else
			{
				VK_SEM_Get(s_tTSMFInstance.stControlObj.ulTSMFLock);
				if(((eMode == P_CH_TSMF_MODE_MONITORING)|| (eMode == P_CH_TSMF_MODE_MONITORING_FIRST))&& (s_tTSMFInstance.bRequestSearch) && (s_tTSMFInstance.ucSearchDemodId == ulInputDevice_id))
				{
					CH_DI_TSMF_Print(7,("Current Monitoring and searching is Same(%d) Timeout\n", ulInputDevice_id));
				}
				else
				{
					CH_DI_TSMF_Print(7,(">>>>>>>>Search TSMF TimeOut DemodID(%d)\n", ulInputDevice_id));
				s_tTSMFInstance.stResource[ucTSMFIdx].eStatus = DRV_CH_TSMF_TIMEOUT;
					s_tTSMFInstance.bRequestSearch = FALSE;
				}
				VK_SEM_Release(s_tTSMFInstance.stControlObj.ulTSMFLock);
			}
		}
		else
		{
			tick2 = VK_TIMER_GetSystemTick();

			pucTSBuff = (HUINT8*)tsmftsevtMsg.unTSBufferAddress;
			if((tsmftsevtMsg.unRequestID != s_tTSMFInstance.stResource[ucTSMFIdx].unRequestID)||(tsmftsevtMsg.ucTSMFIndex != ucTSMFIdx))
			{
				CH_DI_TSMF_Print(8,(">>>>>>>>Skip TSMF Data !!!!!!!!!!!!(%d)\n", tsmftsevtMsg.unRequestID));
				DD_MEM_Free(pucTSBuff);
				continue;
			}
			else
			{

 		            /* Disable PID Channel for skip data */
		            eDrvError = P_CH_TSMF_DisableTSFilter(ucTSMFIdx);
		            if(eDrvError != DRV_OK)
		            {
			         PrintError(" Failed to disable PID channel(%d)\n", eDrvError);
			    }

				CH_DI_TSMF_Print(9,("[tsmf_recv-Demod(%d)] : \n", ulInputDevice_id));
				CH_DI_TSMF_Print(9, ("%02x|%02x|%02x|%02x|%02x|%02x|%02x|%02x|%02x|%02x \n ", pucTSBuff[0], pucTSBuff[1], pucTSBuff[2], pucTSBuff[3], pucTSBuff[4], pucTSBuff[5], pucTSBuff[6], pucTSBuff[7], pucTSBuff[8], pucTSBuff[9]));

				VK_SEM_Get(s_tTSMFInstance.stControlObj.ulTSMFLock);
				if(((eMode == P_CH_TSMF_MODE_MONITORING)||(eMode == P_CH_TSMF_MODE_MONITORING_FIRST))&& (s_tTSMFInstance.bRequestSearch) && (s_tTSMFInstance.ucSearchDemodId == ulInputDevice_id))
				{
					CH_DI_TSMF_Print(7,("Current Monitoring and searching is Same(%d)\n", ulInputDevice_id));
					DD_MEM_Free(pucTSBuff);
				}
				else
				{
					if((eMode == P_CH_TSMF_MODE_SEARCHING_FIRST)||(eMode == P_CH_TSMF_MODE_SEARCHING))
					{
						CH_DI_TSMF_Print(7,(">> tick2-1 (%d)\n", tick2-tick1));
					}
					else
					{
						CH_DI_TSMF_Print(7,(">> tick2-1 Monitor(%d)\n", tick2-tick1));
					}

				if(s_tTSMFInstance.pfnTSMFNotify != NULL)
				{
					PrintDebug("[tsmf_recv] Notify~~\n");
					s_tTSMFInstance.pfnTSMFNotify(ulInputDevice_id, pucTSBuff);
				}
				else
				{
					DD_MEM_Free(pucTSBuff);
				}

					s_tTSMFInstance.bRequestSearch = FALSE;
				s_tTSMFInstance.stResource[ucTSMFIdx].eStatus = DRV_CH_TSMF_RECEIVED;
				}
				VK_SEM_Release(s_tTSMFInstance.stControlObj.ulTSMFLock);
			}
		}

		/* Disable PID Channel */
		eDrvError = P_CH_TSMF_DisableTSFilter(ucTSMFIdx);
		if(eDrvError != DRV_OK)
		{
			PrintError(" Failed to disable PID channel(%d)\n", eDrvError);
			return DRV_ERR;
}
		break;
	}
	return DRV_OK;
}

/*******************************************************************************
* function : P_CH_TSMF_InitTSMFModule
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static DRV_Error P_CH_TSMF_InitTSMFModule(void)
{
	HUINT32 err;
	HUINT8 ucIdx;

	/* Init Resource */
	for(ucIdx = 0; ucIdx < CONFIG_NUM_OF_CAB_CHANNEL; ucIdx++)
	{
		if(s_ucNumMonitorParser != CONFIG_NUM_OF_CAB_CHANNEL)
		{
			s_tTSMFInstance.stResource[ucIdx].unDemuxId = MAX_PARSER_BAND_NUM-1;
		}
		else
		{
			s_tTSMFInstance.stResource[ucIdx].unDemuxId = (MAX_PARSER_BAND_NUM-1)-ucIdx;
		}
		s_tTSMFInstance.stResource[ucIdx].unDemodID = ucIdx;
		s_tTSMFInstance.stResource[ucIdx].bEnabled = FALSE;
		s_tTSMFInstance.stResource[ucIdx].eStatus = DRV_CH_TSMF_IDLE;
		s_tTSMFInstance.stResource[ucIdx].unRequestID = 0xFFFFFFFF;
	}

	s_tTSMFInstance.bRequestSearch = FALSE;
	s_tTSMFInstance.ucSearchDemodId = 0;
	s_tTSMFInstance.bSetSearchPath = FALSE;
	s_tTSMFInstance.bMonSearch = FALSE;
	s_tTSMFInstance.nDemuxId = -1;
	s_tTSMFInstance.ucMonDemodId = 0xFF;

	if(VK_MSG_Create(DRV_CH_TSMF_MSGQ_SIZE, sizeof(P_CH_TSMF_MSG_t),"TsmfTsEvtMsg",&s_tTSMFInstance.stControlObj.ulTSMFTSEvtMsgId, VK_SUSPENDTYPE_FIFO))
	{
		PrintError("P_CH_TSMF_InitTSMFModule(), VK_MSG_Create(TsmfTsEvtMsg) error\n");
		return DRV_ERR;
	}

	if(VK_MSG_Create(DRV_CH_TSMF_MSGQ_SIZE, sizeof(P_CH_TSMF_MSG_t),"TsmfMsg",&s_tTSMFInstance.stControlObj.ulTSMFMsgId, VK_SUSPENDTYPE_FIFO))
	{
		VK_MSG_Destroy(s_tTSMFInstance.stControlObj.ulTSMFTSEvtMsgId);
		PrintError("P_CH_TSMF_InitTSMFModule(), VK_MSG_Create(TsmfMsg) error\n");
		return DRV_ERR;
	}


	err = VK_SEM_Create(&s_tTSMFInstance.stControlObj.ulTSMFLock, "TSMF_SEM", VK_SUSPENDTYPE_FIFO);
	if(err)
	{
		VK_MSG_Destroy(s_tTSMFInstance.stControlObj.ulTSMFMsgId);
		VK_MSG_Destroy(s_tTSMFInstance.stControlObj.ulTSMFTSEvtMsgId);
		PrintError("VK_SEM_Create() error, TSMF_SEM\n");
		return DRV_ERR;
	}

	err = VK_TASK_Create(P_CH_TSMF_MonTask,
						DRV_CH_TSMF_TASK_PRIORITY,
						DRV_CH_TSMF_TASK_STACK_SIZE,
						"TsmfMonTask",
						NULL,
						&s_tTSMFInstance.stControlObj.ulTSMFTaskId, FALSE);
	if( err != VK_OK )
	{
		VK_MSG_Destroy(s_tTSMFInstance.stControlObj.ulTSMFMsgId);
		VK_MSG_Destroy(s_tTSMFInstance.stControlObj.ulTSMFTSEvtMsgId);
		VK_SEM_Destroy(s_tTSMFInstance.stControlObj.ulTSMFLock);
		PrintError("P_CH_TSMF_InitTSMFModule() Error(0x%x) in VK_TASK_Create(TsmfMonTask)\n\r", err);
		return DRV_ERR;
	}

	err = VK_TASK_Start(s_tTSMFInstance.stControlObj.ulTSMFTaskId);
	if(err != VK_OK)
	{
		VK_MSG_Destroy(s_tTSMFInstance.stControlObj.ulTSMFMsgId);
		VK_MSG_Destroy(s_tTSMFInstance.stControlObj.ulTSMFTSEvtMsgId);
		VK_SEM_Destroy(s_tTSMFInstance.stControlObj.ulTSMFLock);
		VK_TASK_Destroy(s_tTSMFInstance.stControlObj.ulTSMFTaskId);
		PrintError("P_CH_TSMF_InitTSMFModule() Error(0x%x) in VK_TASK_Start(TsmfMonTask)\n\r", err);
		return DRV_ERR;
	}

	return DRV_OK;
}


/*******************************************************************************
* function : P_CH_TSMF_MonTask
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static void P_CH_TSMF_MonTask(void *param)
{
	P_CH_TSMF_MSG_t tsmfMsg;
	DRV_Error eDrvError;
	int vk_ret;
	unsigned long ulMonitoringPeriod = 0xFFFFFFFF;
	HINT32 nCheckDemuxId;
	HINT32 nDemuxId;
	HUINT32 unInputBand;
	HUINT8 ucSearchIdx;
	HUINT8 ucIdx;
	P_CH_TSMF_MODE_e eMode;
	BSTD_UNUSED(param);

	while(TRUE)
	{
		vk_ret = VK_MSG_ReceiveTimeout(s_tTSMFInstance.stControlObj.ulTSMFMsgId, &tsmfMsg, sizeof(P_CH_TSMF_MSG_t), ulMonitoringPeriod);
		if(vk_ret != VK_OK)
		{
			if(vk_ret != VK_TIMEOUT)
			{
				PrintError("VK_MSG_ReceiveTimeout(tsmfMsg) returns error. (err=%ld)\n", vk_ret);
			continue;
		}
			else
				{
				if(ulMonitoringPeriod == DRV_CH_TSMF_MONITOR_READY_TIME)
				{


					ucSearchIdx = s_tTSMFInstance.ucSearchDemodId;

					if(s_tTSMFInstance.bSetSearchPath == TRUE)
					{
                                                CH_DI_TSMF_Print(0,("\n\nCurrent TSMF MONITOR but SEARCH Path existed -- disable SEARCH Path\n"));
						eDrvError = P_CH_TSMF_DisableTSFilter(ucSearchIdx);
						if(eDrvError != DRV_OK)
						{
							PrintError(" Failed to disable Search PID channel(%d)\n", eDrvError);
						}

						eDrvError = P_CH_TSMF_SetMTSIFPath(s_tTSMFInstance.stResource[ucSearchIdx].unDemodID, (NEXUS_ParserBand)s_tTSMFInstance.stResource[ucSearchIdx].unDemuxId, FALSE);
						if(eDrvError != DRV_OK)
						{
							PrintError(" Failed to disable Search ParserBand (%d)\n", eDrvError);
						}
					}

					VK_SEM_Get(s_tTSMFInstance.stControlObj.ulTSMFLock);
					s_tTSMFInstance.bSetSearchPath = FALSE;

					for(ucIdx = 0; ucIdx < CONFIG_NUM_OF_CAB_CHANNEL; ucIdx++)
					{
						s_tTSMFInstance.stResource[ucIdx].bEnabled = FALSE;
						s_tTSMFInstance.stResource[ucIdx].eStatus = DRV_CH_TSMF_IDLE;
					}
					VK_SEM_Release(s_tTSMFInstance.stControlObj.ulTSMFLock);


					/* Get live demod number */
					nCheckDemuxId = s_tTSMFInstance.nDemuxId + 1;
					eDrvError = DRV_DEMUX_GetLiveInputBand(nCheckDemuxId, &nDemuxId, &unInputBand);
					if(eDrvError != DRV_OK)
					{
						/* Check All demux */
						eDrvError = DRV_DEMUX_GetLiveInputBand(-1, &nDemuxId, &unInputBand);
						if(eDrvError != DRV_OK)
						{
                    		CH_DI_TSMF_Print(8,(" WAIT FOR Valid LIVE DEMUX(ALL DEMUX) --Failed to Get live inputband\n"));

							s_tTSMFInstance.nDemuxId = -1;
							continue;
						}
					}

					ucSearchIdx = (HUINT8)unInputBand;

					s_tTSMFInstance.nDemuxId = nDemuxId;
					s_tTSMFInstance.ucMonDemodId = ucSearchIdx;

					ulMonitoringPeriod = DRV_CH_TSMF_MONITOR_PERIOD;

                    CH_DI_TSMF_Print(8,("MONITOR Set Path - demod(%d)\n", ucSearchIdx));
					eDrvError = P_CH_TSMF_Search(ucSearchIdx, P_CH_TSMF_MODE_MONITORING_FIRST);
					if(eDrvError != DRV_OK)
					{
						PrintError(" Failed to MONITOR Search(%d)\n", eDrvError);
						continue;
					}
				}
				else
				{
					nCheckDemuxId = s_tTSMFInstance.nDemuxId + 1;
					eDrvError = DRV_DEMUX_GetLiveInputBand(nCheckDemuxId, &nDemuxId, &unInputBand);
					if(eDrvError != DRV_OK)
					{
						/* Check All Demux */
						eDrvError = DRV_DEMUX_GetLiveInputBand(-1, &nDemuxId, &unInputBand);
						if(eDrvError != DRV_OK)
						{
							s_tTSMFInstance.nDemuxId = -1;

							ucSearchIdx = s_tTSMFInstance.ucMonDemodId;

							VK_SEM_Get(s_tTSMFInstance.stControlObj.ulTSMFLock);
							s_tTSMFInstance.stResource[ucSearchIdx].eStatus = DRV_CH_TSMF_WAIT_LIVEDEMOD;
							VK_SEM_Release(s_tTSMFInstance.stControlObj.ulTSMFLock);
							continue;
						}
					}

					if(s_tTSMFInstance.ucMonDemodId != unInputBand)
					{
                        CH_DI_TSMF_Print(7,("\nLIVE DEMOD CHANGE (0x%x : 0x%x)!!\n", s_tTSMFInstance.ucMonDemodId, unInputBand));
						DRV_CH_TSMF_DisableMonitor();
						DRV_CH_TSMF_EnableMonitor();
						continue;
					}
					else
					{
						ucSearchIdx = s_tTSMFInstance.ucMonDemodId;

						eDrvError = P_CH_TSMF_Search(ucSearchIdx, P_CH_TSMF_MODE_MONITORING);
					if(eDrvError != DRV_OK)
					{
						PrintError(" Failed to disable PID channel(%d)\n", eDrvError);
						continue;
					}
					}
				}
					}
				}
		else
				{
			/*Request Search*/
			if(tsmfMsg.eCommand == P_CH_TSMF_COMMAND_SEARCH_TSMF)
				{
				ucSearchIdx = s_tTSMFInstance.ucMonDemodId;

				if(s_tTSMFInstance.bMonSearch == TRUE)
				{
					eDrvError = P_CH_TSMF_DisableTSFilter(ucSearchIdx);
					if(eDrvError != DRV_OK)
					{
						PrintError(" Failed to disable monitor PID channel(%d)\n", eDrvError);
					}
					eDrvError = P_CH_TSMF_SetMTSIFPath(s_tTSMFInstance.stResource[ucSearchIdx].unDemodID, (NEXUS_ParserBand)s_tTSMFInstance.stResource[ucSearchIdx].unDemuxId, FALSE);
					if(eDrvError != DRV_OK)
					{
						PrintError(" Failed to disable moninor ParserBand (%d)\n", eDrvError);
					}
				}

				VK_SEM_Get(s_tTSMFInstance.stControlObj.ulTSMFLock);
				s_tTSMFInstance.bMonSearch = FALSE;
				s_tTSMFInstance.ucMonDemodId = 0xFF;
				if(ucSearchIdx != tsmfMsg.ucTSMFIndex)
				{
					s_tTSMFInstance.stResource[ucSearchIdx].eStatus = DRV_CH_TSMF_IDLE;
				}
				VK_SEM_Release(s_tTSMFInstance.stControlObj.ulTSMFLock);

				ulMonitoringPeriod = 0xFFFFFFFF;

				if(s_tTSMFInstance.bSetSearchPath == TRUE)
				{
					if(s_tTSMFInstance.ucSearchDemodId != tsmfMsg.ucTSMFIndex)
					{
						CH_DI_TSMF_Print(0,("CHANGE PATH(%d ->%d) on Search TSMF\n", s_tTSMFInstance.ucSearchDemodId, tsmfMsg.ucTSMFIndex));
						ucSearchIdx = s_tTSMFInstance.ucSearchDemodId;
						eDrvError = P_CH_TSMF_DisableTSFilter(ucSearchIdx);
						if(eDrvError != DRV_OK)
						{
							PrintError(" Failed to disable Search PID channel(%d)\n", eDrvError);
						}

						eDrvError = P_CH_TSMF_SetMTSIFPath(s_tTSMFInstance.stResource[ucSearchIdx].unDemodID, (NEXUS_ParserBand)s_tTSMFInstance.stResource[ucSearchIdx].unDemuxId, FALSE);
						if(eDrvError != DRV_OK)
						{
							PrintError(" Failed to disable Search ParserBand (%d)\n", eDrvError);
						}

						VK_SEM_Get(s_tTSMFInstance.stControlObj.ulTSMFLock);
						s_tTSMFInstance.bSetSearchPath = FALSE;
						s_tTSMFInstance.ucSearchDemodId = tsmfMsg.ucTSMFIndex;
						VK_SEM_Release(s_tTSMFInstance.stControlObj.ulTSMFLock);

						eMode = P_CH_TSMF_MODE_SEARCHING_FIRST;
					}
					else
					{
				eMode = P_CH_TSMF_MODE_SEARCHING;
				}
				}
				else
				{
					s_tTSMFInstance.ucSearchDemodId = tsmfMsg.ucTSMFIndex;
					eMode = P_CH_TSMF_MODE_SEARCHING_FIRST;
				}

				ucSearchIdx = tsmfMsg.ucTSMFIndex;

                CH_DI_TSMF_Print(8,("SEARCH(%d) - demod(%d)\n",eMode, ucSearchIdx));
			eDrvError = P_CH_TSMF_Search(ucSearchIdx, eMode);
				if(eDrvError != DRV_OK)
				{
					PrintError(" Failed to disable PID channel(%d)\n", eDrvError);
					continue;
				}

			}
			else if(tsmfMsg.eCommand == P_CH_TSMF_COMMAND_CANCLE_SEARCH)
			{
				ulMonitoringPeriod = 0xFFFFFFFF;

				ucSearchIdx = tsmfMsg.ucTSMFIndex;

				if(s_tTSMFInstance.bSetSearchPath == TRUE)
				{
					eDrvError = P_CH_TSMF_DisableTSFilter(ucSearchIdx);
					if(eDrvError != DRV_OK)
					{
						PrintError(" Failed to disable Search PID channel(%d)\n", eDrvError);
					}

					if(ucSearchIdx == s_tTSMFInstance.ucSearchDemodId)
					{
					eDrvError = P_CH_TSMF_SetMTSIFPath(s_tTSMFInstance.stResource[ucSearchIdx].unDemodID, (NEXUS_ParserBand)s_tTSMFInstance.stResource[ucSearchIdx].unDemuxId, FALSE);
					if(eDrvError != DRV_OK)
					{
						PrintError(" Failed to disable Search ParserBand (%d)\n", eDrvError);
					}
				}
				}

				if(ucSearchIdx == s_tTSMFInstance.ucSearchDemodId)
				{
				VK_SEM_Get(s_tTSMFInstance.stControlObj.ulTSMFLock);
				s_tTSMFInstance.bSetSearchPath = FALSE;
				VK_SEM_Release(s_tTSMFInstance.stControlObj.ulTSMFLock);
				}

				continue;
			}
			else if(tsmfMsg.eCommand == P_CH_TSMF_COMMAND_ENABLE_MONITOR)
			{
				ulMonitoringPeriod = DRV_CH_TSMF_MONITOR_READY_TIME;

				continue;
			}
			else if(tsmfMsg.eCommand == P_CH_TSMF_COMMAND_DISABLE_MONITOR)
			{
				ulMonitoringPeriod = 0xFFFFFFFF;

				ucSearchIdx = s_tTSMFInstance.ucMonDemodId;

				if(s_tTSMFInstance.bMonSearch == TRUE)
				{
					eDrvError = P_CH_TSMF_DisableTSFilter(ucSearchIdx);
					if(eDrvError != DRV_OK)
					{
						PrintError(" Failed to disable monitor PID channel(%d)\n", eDrvError);
					}
					eDrvError = P_CH_TSMF_SetMTSIFPath(s_tTSMFInstance.stResource[ucSearchIdx].unDemodID, (NEXUS_ParserBand)s_tTSMFInstance.stResource[ucSearchIdx].unDemuxId, FALSE);
					if(eDrvError != DRV_OK)
					{
						PrintError(" Failed to disable moninor ParserBand (%d)\n", eDrvError);
					}
				}

				VK_SEM_Get(s_tTSMFInstance.stControlObj.ulTSMFLock);
				s_tTSMFInstance.bMonSearch = FALSE;
				s_tTSMFInstance.ucMonDemodId = 0xFF;
				if(ucSearchIdx < CONFIG_NUM_OF_CAB_CHANNEL)
				{
					s_tTSMFInstance.stResource[ucSearchIdx].eStatus = DRV_CH_TSMF_IDLE;
				}
				VK_SEM_Release(s_tTSMFInstance.stControlObj.ulTSMFLock);

				continue;
			}
			else
			{
				PrintError(" Unknown command : (%d)\n", tsmfMsg.eCommand);
				continue;
			}
		}
}
}

/*******************************************************************************
* function : P_CH_TSMF_SetMTSIFPath
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static DRV_Error P_CH_TSMF_SetMTSIFPath(HUINT32 ulInputDeviceId, NEXUS_ParserBand eParserBand, HBOOL bPathEnable)
{
	NEXUS_PlatformConfiguration tPlatformConfig;
	NEXUS_FrontendHandle pvFrontendHandle = NULL;
	NEXUS_FrontendUserParameters tUserParams;
	NEXUS_ParserBandSettings tParserBandSetting;
	NEXUS_Error eNexusRet;

	if(ulInputDeviceId >= NEXUS_MAX_FRONTENDS)
	{
		PrintError("Invalid frontend index (ulInputDeviceId=%d)\n", ulInputDeviceId);
		return DRV_ERR;
	}

	NEXUS_Platform_GetConfiguration(&tPlatformConfig);
	pvFrontendHandle = tPlatformConfig.frontend[ulInputDeviceId];
	if(pvFrontendHandle == NULL)
	{
		PrintError("Invalid frontend handle (pvFrontendHandle=%p)\n", pvFrontendHandle);
		return DRV_ERR;
	}

	NEXUS_Frontend_GetUserParameters(pvFrontendHandle, &tUserParams);
	NEXUS_ParserBand_GetSettings(eParserBand, &tParserBandSetting);
	if(tUserParams.isMtsif)
	{
		tParserBandSetting.sourceTypeSettings.inputBand =  DRV_DEMUX_GetUnusedInputBand(); /* Disable inputband */
		tParserBandSetting.sourceType = NEXUS_ParserBandSourceType_eMtsif;

		if(bPathEnable == TRUE)
		{
#if (NEXUS_VERSION >= 1290)
			tParserBandSetting.sourceTypeSettings.mtsif = NEXUS_Frontend_GetConnector(pvFrontendHandle);
#else
			tParserBandSetting.sourceTypeSettings.mtsif = pvFrontendHandle;
#endif
		}
		else
		{
			tParserBandSetting.sourceTypeSettings.mtsif = NULL;
		}
	}
	else
	{
		PrintError("Frontend can't support MTSIF\n");
		return DRV_ERR;
	}

	tParserBandSetting.transportType = NEXUS_TransportType_eTs;

	eNexusRet = NEXUS_ParserBand_SetSettings(eParserBand, &tParserBandSetting);
	if(eNexusRet != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_ParserBand_SetSettings fail (ret=%d)\n", eNexusRet);
		return DRV_ERR;
	}

	eNexusRet = NEXUS_Frontend_ReapplyTransportSettings(pvFrontendHandle);
	if(eNexusRet != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Frontend_ReapplyTransportSettings fail (ret=%d)\n", eNexusRet);
		return DRV_ERR;
	}

	return DRV_OK;
}


/* end of file */

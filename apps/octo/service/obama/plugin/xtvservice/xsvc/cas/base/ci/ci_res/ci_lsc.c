/***************************************************************
* $Workfile:   ci_lsc.c  $
* $Modtime:
* Auther : kkkim / hmkim
*
* Desc : Low Speed Communication (CI+ v1.3)
*
* Copyright (c) 2011 by Humax Co., Ltd.
* All right reserved
****************************************************************/

/***************************************************************
* #ifdef statements and include
***************************************************************/
#include <pal_ci.h>

#include <ci_plus.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>


/***************************************************************
* local definition
***************************************************************/
#define BUFFER_NUM 16			//CIPlus V1.3 spec에 따라 버퍼의 갯수는 16개로 고정 됨
#define IP_DESCRIPTOR_LENG 22	//CIPlus V1.3 spec에 따라 IP descriptor의 크기는 22byte가 되어야 함
#define IP_DESCRIPTOR_TAG 0xcf
#define HOSTNAME_DESCRIPTOR_TAG 0xcd

#if defined(CONFIG_DROP_ROOT_PRIVILEGES)//1
#if 0 //1 필요 없을 듯하여 막음. Test 후 필요시 추가 작업
#define	socket			PAL_SYS_Socket
#define	select			PAL_SYS_Select
#define	close			PAL_SYS_Close
#define	write			PAL_SYS_Write
#endif
#endif

/***************************************************************
* typedef
***************************************************************/
/* LSC task message */
typedef struct {
	unsigned short		usSsNo;
	unsigned long		ulApduTag;
	unsigned char		*pucApduData;
	unsigned long		ulApduLen;
} CI_LSC_TASK_MSG;

//Low Speed Communication ENUM
typedef enum
{
	eCiPlus_CommsCmd_Connect = 1,
	eCiPlus_CommsCmd_Disconnect,
	eCiPlus_CommsCmd_SetParams,
	eCiPlus_CommsCmd_EnquireStatus,
	eCiPlus_CommsCmd_GetNextBuffer
} CI_LSC_CMD;

typedef enum
{
	eCiPlus_Connection_Telephone = 1,
	eCiPlus_Connection_CableReturnChannel,
	eCiPlus_Connection_IP,
	eCiPlus_Connection_Hostname
} CI_LSC_CONNECT_TYPE;

typedef enum
{
	eCiPlus_IpProtocol_IPv4 = 1,
	eCiPlus_IpProtocol_IPv6
} CI_LSC_IP_VERSION;

typedef enum
{
	eCiPlus_Protocol_TCP = 1,
	eCiPlus_Protocol_UDP
} CI_LSC_PROTOCOL_TYPE;

typedef enum
{
	eCiPlus_CommsReply_ConnectAck = 1,
	eCiPlus_CommsReply_DisconnectAck,
	eCiPlus_CommsReply_SetParamsAck,
	eCiPlus_CommsReply_StatusReplyAck,
	eCiPlus_CommsReply_SendAck = 6
} CI_LSC_REPLY_TYPE;

typedef struct		// Comms Cmd IP_descriptor
{
	CI_LSC_PROTOCOL_TYPE		eProtocolType;
	unsigned short 				usDestinationPort;
	CI_LSC_IP_VERSION			eIpProtocolVer;
	unsigned char 				aucIpAddress[16];
} CI_LSC_IpDesc_t;

typedef struct		// Comms Cmd Hostname_descriptor
{
	CI_LSC_PROTOCOL_TYPE		eProtocolType;
	unsigned short 				usDestinationPort;
	unsigned char				*pucHostname;
} CI_LSC_HostnameDesc_t;

typedef struct
{
	HUINT16							usCiLscSsNo;

	CI_LSC_CONNECT_TYPE				eConnectionType;
	CI_LSC_IpDesc_t					stIpDesc;
	CI_LSC_HostnameDesc_t			stHostnameDesc;
	HBOOL							bConnectionStatus;
	HINT32							nSocketNum; /* fix for prevent #133254, #133255 */

	unsigned char					ucConnectionRetryCount;
	unsigned char					ucConnectionTimeout;

	unsigned short					usBufferSize;
	unsigned char					ucReceptionTimeout;			// 값 1은 10 ms을 나타냄.

	HBOOL							bBufferInitialized;

	unsigned char*					paucSendBuffer[BUFFER_NUM];
	unsigned char 					ucSendBufferFront;
	unsigned char 					ucSendBufferRear;
	unsigned char 					ucRcvBufferFront;
	unsigned char 					ucRcvBufferRear;
	unsigned char					ucSendBufferCount;
	unsigned char					ucRcvBufferCount;
	HBOOL							bRcvBufferFull;

	HINT16							sRcvMsgCount;
} CI_LSC_INSTANCE;


/***************************************************************
* static / external variables and structures
***************************************************************/
static unsigned long		s_ulCiLscMsgQId;
static unsigned long 		s_ulCiLscTaskId;

static unsigned long		s_ulCiLscRcvMsgQId;
static unsigned long 		s_ulCiLscRcvTaskId;

static HBOOL				s_bSsOpened;
static CI_LSC_INSTANCE		s_stCiLsc[PAL_MAX_CISLOT_NUMBER];


/***************************************************************
* static function prototypes
***************************************************************/
static void CiLsc_Task(void);
static void CiLsc_RcvTask(void);
CI_LSC_INSTANCE* CiLsc_GetConnectionInfo(CI_SLOT_ID usSlotId);
CI_LSC_INSTANCE* CiLsc_GetConnectionInfoBySsNo(HUINT16 usSsNo);
int CiLsc_Proc(unsigned short usSsNo, unsigned long ulApduTag, unsigned char *pucApduData, unsigned long ulApduLen);
int CiLsc_ProcCommand(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen);
int CiLsc_ProcSendMore(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen);
int CiLsc_ProcSendLast(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen);
int CiLsc_CommsReply(unsigned short usSsNo, CI_LSC_REPLY_TYPE eCommsReply, CI_LSC_INSTANCE* pstContext);
int CiLsc_EstablishConnection(unsigned short usSsNo, CI_LSC_INSTANCE* 	pstContext);
int CiLsc_EstablishIpConnection(CI_LSC_INSTANCE* pstContext);
int CiLsc_EstablishHostnameConnection(CI_LSC_INSTANCE* pstContext);
int CiLsc_TerminateConnection(unsigned short usSsNo, CI_LSC_INSTANCE* pstContext);
int CiLsc_TerminateIpConnection(CI_LSC_INSTANCE* pstContext);
int CiLsc_TerminateHostnameConnection(CI_LSC_INSTANCE* pstContext);
int CiLsc_BufferInit(unsigned short usBufferSize, CI_LSC_INSTANCE* pstContext);
int CiLsc_RcvBuffer(unsigned short usSsNo, unsigned char ucPhaseId, CI_LSC_INSTANCE* pstContext);
int CiLsc_RcvBufferUsingIpConnection(unsigned short usSsNo, unsigned char ucPhaseId, CI_LSC_INSTANCE* pstContext);
int CiLsc_SendBuffer(unsigned short usSsNo, CI_LSC_INSTANCE* pstContext, unsigned long ulDataLength);
int CiLsc_SendBufferUsingIpConnection(CI_LSC_INSTANCE* pstContext, unsigned char* pucSendData, int nDataLength);
unsigned char* CiLsc_MakeRcvApdu(unsigned short usBufferSize, unsigned char ucBufferRear, unsigned short usDataLength, unsigned char* pucData, unsigned short* pusApduLength);
unsigned char* CiLsc_SetConnection(unsigned char *pucApduData, unsigned long ulApduLen, CI_LSC_INSTANCE* 	pstContext);
unsigned char* CiLsc_SetIpConnection(unsigned char *pucApduData, CI_LSC_IpDesc_t* pstIpDesc);
unsigned char* CiLsc_SetHostnameConnection(unsigned char *pucApduData, unsigned long ulApduLen, CI_LSC_HostnameDesc_t* pstHostnameDesc);
int CiLsc_ConnectIpServer (int nSocketNum, const struct sockaddr *pstSocketAddr, int nSocketAddrLength, int nTimeout);


/***************************************************************
* function bodies
***************************************************************/
int CiLsc_Init(void)
{
	HUINT32	i=0;
	HxLOG_Info("[CILSC]---> CiLsc_Init()---> Low Speed Communication Resources are initialized. \n");

	for(i=0; i<PAL_MAX_CISLOT_NUMBER; i++)
	{
		HxSTD_memset(&s_stCiLsc[i], 0, sizeof(CI_LSC_INSTANCE));
		s_stCiLsc[i].usCiLscSsNo = 0;
	}

	s_ulCiLscMsgQId = 0;
	s_ulCiLscTaskId = 0;

	s_ulCiLscRcvMsgQId = 0;
	s_ulCiLscRcvTaskId = 0;

	s_bSsOpened = FALSE;

	return ERR_OK;
}

void CiLsc_TerminateProc(unsigned short usSsNo)
{
	int			i;
	CI_LSC_INSTANCE* pstContext;

	HxLOG_Info("SsNo[%d]...\n", usSsNo);

	pstContext = CiLsc_GetConnectionInfoBySsNo(usSsNo);

	if (pstContext->bConnectionStatus == TRUE)
	{
		CiLsc_TerminateConnection(usSsNo, pstContext);
	}

	if (pstContext->bBufferInitialized == TRUE)
	{
		for(i=0; i<BUFFER_NUM; i++)
		{
			if(pstContext->paucSendBuffer[i])
			{
				OxCAS_Free(pstContext->paucSendBuffer[i]);
				pstContext->paucSendBuffer[i] = NULL;
			}
		}
		pstContext->bBufferInitialized = FALSE;
	}

	pstContext->usCiLscSsNo = 0;
	HxSTD_memset(pstContext, 0, sizeof(CI_LSC_INSTANCE));

//	s_bSsOpened = FALSE;
}


static void CiLsc_Task(void)
{
	CI_LSC_TASK_MSG msg;

	while (1)
	{
		if (VK_MSG_Receive(s_ulCiLscMsgQId, &msg, sizeof(CI_LSC_TASK_MSG)) != VK_OK)
		{
			HxLOG_Error("[CILSC]---> CiLsc_Task()---> Error : fail to get msg !!!\n");
			continue;
		}

		if(msg.ulApduTag == 0 && msg.pucApduData == NULL && msg.ulApduLen == 0)
		{
			break;
		}

		if (CiLsc_Proc(msg.usSsNo, msg.ulApduTag, msg.pucApduData, msg.ulApduLen))
		{
			HxLOG_Error("[CILSC]---> CiLsc_Task()---> Error : CiCc_Proc(%d, 0x%lx) !!!\n", msg.usSsNo, msg.ulApduTag);
		}

		if (msg.pucApduData)
		{
			OxCAS_Free(msg.pucApduData);
			msg.pucApduData = NULL;
		}
	}

	if (s_ulCiLscMsgQId != 0)
	{
		HxLOG_Info("Destroy LSC MSG Q...\n");
		VK_MSG_Destroy(s_ulCiLscMsgQId);
		s_ulCiLscMsgQId = 0;
	}

	return;
}

static void CiLsc_RcvTask(void)
{
	CI_LSC_TASK_MSG msg;

	while (1)
	{
		if (VK_MSG_Receive(s_ulCiLscRcvMsgQId, &msg, sizeof(CI_LSC_TASK_MSG)) != VK_OK)
		{
			HxLOG_Error("[CILSC]---> CiLsc_Task()---> Error : fail to get msg !!!\n");
			continue;
		}

		if(msg.ulApduTag == 0 && msg.pucApduData == NULL && msg.ulApduLen == 0)
		{
			break;
		}

		if (CiLsc_Proc(msg.usSsNo, msg.ulApduTag, msg.pucApduData, msg.ulApduLen))
		{
			HxLOG_Error("[CILSC]---> CiLsc_Task()---> Error : CiCc_Proc(%d, 0x%lx) !!!\n", msg.usSsNo, msg.ulApduTag);
		}

		if (msg.pucApduData)
		{
			OxCAS_Free(msg.pucApduData);
			msg.pucApduData = NULL;
		}
	}

	if (s_ulCiLscRcvMsgQId != 0)
	{
		HxLOG_Info("Destroy LSC RCV MSG Q...\n");
		VK_MSG_Destroy(s_ulCiLscRcvMsgQId);
		s_ulCiLscRcvMsgQId = 0;
	}

	return;
}



int CiLsc_SsOpenedProc(unsigned short usSsNo)
{
	CI_SLOT_ID usSlotId;
	CI_LSC_INSTANCE* pstContext;

	HxLOG_Info("[CILSC]---> CiLsc_SsOpenedProc()---> SsNo[%d]...\n", usSsNo);

	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0)
	{
		HxLOG_Error("[CILSC]---> CiLsc_SsOpenedProc()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return -1;
	}

	pstContext = CiLsc_GetConnectionInfo(usSlotId);

	if (pstContext->usCiLscSsNo == 0)
	{
		pstContext->usCiLscSsNo = usSsNo;
	}
	else
	{
		HxLOG_Warning("LSC Session already Opend at Slot[%02x]\n", usSlotId);
	}

	if (s_bSsOpened == TRUE)		// 이미 Ss이 열린적 있어서 Task를 다시 생성 할 필요 없을 때 ERR_OK 를 return하며 종료.
		return ERR_OK;

	HxLOG_Info("[CILSC]---> CiLsc_SsOpenedProc()---> MSG and Task Create...\n");
	// create message queue and task
	if (VK_MSG_Create(PAL_CI_LSC_MSG_Q_SIZE, sizeof(CI_LSC_TASK_MSG), "qCILSC", &s_ulCiLscMsgQId, VK_SUSPENDTYPE_PRIORITY))
	{
		HxLOG_Error("[CILSC]---> CiLsc_Init()---> Error : VK_MSG_Create() !!!\n");
		goto ERROR;
	}
	if (VK_TASK_Create((void*)CiLsc_Task, PAL_CI_LSC_TASK_PRIORITY, PAL_CI_LSC_TASK_STACK_SIZE, "tCILSC", NULL, &s_ulCiLscTaskId, 0))
	{
		HxLOG_Error("[CILSC]---> CiLsc_Init()---> Error : VK_TASK_Create() !!!\n");
		goto ERROR;

	}

	if (VK_TASK_Start(s_ulCiLscTaskId))
	{
		HxLOG_Error("[CILSC]---> CiLsc_Init()---> Error : VK_TASK_Start() !!!\n");
		goto ERROR;

	}


	// create message queue and task
	if (VK_MSG_Create(PAL_CI_LSC_MSG_Q_SIZE, sizeof(CI_LSC_TASK_MSG), "qCILSCRCV", &s_ulCiLscRcvMsgQId, VK_SUSPENDTYPE_PRIORITY))
	{
		HxLOG_Error("[CILSC]---> CiLsc_Init()---> Error : VK_MSG_Create() !!!\n");
		goto ERROR;

	}
	if (VK_TASK_Create((void*)CiLsc_RcvTask, PAL_CI_LSC_TASK_PRIORITY, PAL_CI_LSC_TASK_STACK_SIZE, "tCILSCRCV", NULL, &s_ulCiLscRcvTaskId, 0))
	{
		HxLOG_Error("[CILSC]---> CiLsc_Init()---> Error : VK_TASK_Create() !!!\n");
		goto ERROR;

	}

	if (VK_TASK_Start(s_ulCiLscRcvTaskId))
	{
		HxLOG_Error("[CILSC]---> CiLsc_Init()---> Error : VK_TASK_Start() !!!\n");
		goto ERROR;

	}


	s_bSsOpened = TRUE;
	HxLOG_Info("LSC MSG ID: [%d], TASK ID:[%d] // LSC RCV MSG ID:[%d], TASK ID:[%d]\n\n", s_ulCiLscMsgQId, s_ulCiLscTaskId, s_ulCiLscRcvMsgQId, s_ulCiLscRcvTaskId);

	return ERR_OK;

ERROR:
	HxLOG_Error("ERROR:: Create MSG or TASK ERROR...\n");

	if (s_ulCiLscTaskId != 0)
	{
		VK_TASK_Stop(s_ulCiLscTaskId);
		VK_TASK_Destroy(s_ulCiLscTaskId);
		s_ulCiLscTaskId = 0;
		VK_TASK_Sleep(1);
	}

	if (s_ulCiLscMsgQId != 0)
	{
		VK_MSG_Destroy(s_ulCiLscMsgQId);
		s_ulCiLscMsgQId = 0;
	}

	if (s_ulCiLscRcvTaskId != 0)
	{
		VK_TASK_Stop(s_ulCiLscRcvTaskId);
		VK_TASK_Destroy(s_ulCiLscRcvTaskId);
		s_ulCiLscRcvTaskId = 0;
		VK_TASK_Sleep(1);
	}

	if (s_ulCiLscRcvMsgQId != 0)
	{
		VK_MSG_Destroy(s_ulCiLscRcvMsgQId);
		s_ulCiLscRcvMsgQId = 0;
	}

	return -1;
}

int CiLsc_SsClosedProc(unsigned short usSsNo)
{
	//CI_SLOT_ID	usSlotId;
	CI_LSC_INSTANCE* pstContext;
	CI_LSC_TASK_MSG msg;

	HxLOG_Info("[CILSC]---> CiLsc_SsClosedProc()---> SsNo[%d]...\n", usSsNo);

	pstContext = CiLsc_GetConnectionInfoBySsNo(usSsNo);

	if (pstContext->usCiLscSsNo != usSsNo)
	{
		/* fix for prevent #170115 */
		//HxLOG_Warning("Close additional LSC session on Slot[%02x]\n", usSlotId);
		HxLOG_Warning("Close additional LSC session on Slot\n");
		return ERR_OK;
	}

	// task 종료 하는 부분 -> task 가 한번 열리면 박스가 꺼질때까지 해당 task를 유지하도록 변경
	// (Connection이 이뤄진 상태에서 CAM을 뺏다가 다시 꽂으면 Queue full 나고 CAM이 인식 안되는 현상때문에 변경 함.)
	/*
	CI_LSC_TASK_MSG msg;

	msg.usSsNo = usSsNo;
	msg.ulApduTag = 0;
	msg.pucApduData = NULL;
	msg.ulApduLen = 0;

	if (VK_MSG_Send(s_ulCiLscMsgQId, &msg, sizeof(CI_LSC_TASK_MSG)))
	{
		HxLOG_Error("Error : fail to send msg !!!\n");
		return -1;
	}

	VK_TASK_Sleep(10);
	while(s_ulCiLscMsgQId != 0)
	{
		VK_TASK_Sleep(10);
	}

	if (s_ulCiLscTaskId != 0)
	{
		HxLOG_Info("Destroy LSC TASK...\n");
		VK_TASK_Destroy(s_ulCiLscTaskId);
		s_ulCiLscTaskId = 0;
	}

	if (VK_MSG_Send(s_ulCiLscRcvMsgQId, &msg, sizeof(CI_LSC_TASK_MSG)))
	{
		HxLOG_Error("Error : fail to send msg !!!\n");
		return -1;
	}

	VK_TASK_Sleep(10);
	while(s_ulCiLscRcvMsgQId != 0)
	{
		VK_TASK_Sleep(10);
	}

	if(s_ulCiLscRcvTaskId)
	{
		HxLOG_Info("Destroy LSC RCV TASK...\n");
		VK_TASK_Destroy(s_ulCiLscRcvTaskId);
		s_ulCiLscRcvTaskId = 0;
	}
	*/

	while (VK_MSG_ReceiveTimeout(s_ulCiLscRcvMsgQId, &msg, sizeof(CI_LSC_TASK_MSG), 10) != VK_TIMEOUT)
	{
		HxLOG_Warning("a Message at queue[s_ulCiLscRcvMsgQId]...\n");
	}

	while (VK_MSG_ReceiveTimeout(s_ulCiLscMsgQId, &msg, sizeof(CI_LSC_TASK_MSG), 10) != VK_TIMEOUT)
	{
		HxLOG_Warning("a Message at queue[s_ulCiLscRcvMsgQId]...\n");
	}

	CiLsc_TerminateProc(usSsNo);

	return ERR_OK;
}

int CiLsc_ApduProc(unsigned short usSsNo, unsigned long ulApduTag, unsigned char *pucApduData, unsigned long ulApduLen)
{
	CI_SLOT_ID		usSlotId;
	CI_LSC_TASK_MSG msg;
	int nRtnCode = ERR_OK;
	CI_LSC_INSTANCE* pstContext;

	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0)
	{
		HxLOG_Error("Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return -1;
	}

	pstContext = CiLsc_GetConnectionInfo(usSlotId);

	HxLOG_Info("[CILSC] Get into APDU Proc. APDU Tag [%06x]\n", ulApduTag);

	msg.usSsNo = usSsNo;
	msg.ulApduTag = ulApduTag;
	if (ulApduLen)
	{
		/* fix for prevent #128816 */
		if(NULL == pucApduData)
		{
			HxLOG_Error("pucApduData is NULL\n");
			return -1;
		}

		msg.pucApduData = (unsigned char *)OxCAS_Malloc(ulApduLen);
		if(NULL == msg.pucApduData)
		{
			HxLOG_Error("msg.pucApduData alloc is NULL\n");
			return -1;
		}

		HxSTD_memcpy(msg.pucApduData, pucApduData, ulApduLen);
		msg.ulApduLen = ulApduLen;
	}
	else
	{
		msg.pucApduData = NULL;
		msg.ulApduLen = 0;
	}

	if(ulApduTag == CI_COMMS_CMD)
	{
		/* fix for prevent #128816 */
		if(NULL == msg.pucApduData)
		{
			HxLOG_Error("msg.pucApduData is NULL\n");
			return -1;
		}

		if((CI_LSC_CMD)msg.pucApduData[0] == eCiPlus_CommsCmd_GetNextBuffer)
		{
			pstContext->sRcvMsgCount++;
			HxLOG_Info("sRcvMsgCount: [%d]\n", pstContext->sRcvMsgCount);

	if (VK_MSG_Send(s_ulCiLscRcvMsgQId, &msg, sizeof(CI_LSC_TASK_MSG)))
			{
				HxLOG_Error("[CILSC]---> CiLsc_ApduProc()---> Error : fail to send msg !!!\n");
				OxCAS_Free(msg.pucApduData);
				msg.pucApduData = NULL;
				return -1;
			}

			return ERR_OK;
		}
	}

	if (VK_MSG_Send(s_ulCiLscMsgQId, &msg, sizeof(CI_LSC_TASK_MSG)))
	{
		HxLOG_Error("[CILSC]---> CiLsc_ApduProc()---> Error : fail to send msg !!!\n");
		OxCAS_Free(msg.pucApduData);
		msg.pucApduData = NULL;
		return -1;
	}

	return nRtnCode;
}

int CiLsc_Proc(unsigned short usSsNo, unsigned long ulApduTag, unsigned char *pucApduData, unsigned long ulApduLen)
{
	int nRtnCode = ERR_OK;

	switch (ulApduTag)
	{
	case CI_COMMS_CMD:
		HxLOG_Info("CI_COMMS_CMD received (len=%ld)\n", ulApduLen);
		nRtnCode = CiLsc_ProcCommand(usSsNo, pucApduData, ulApduLen);
		break;

	case CI_COMMS_SEND_LAST:
		HxLOG_Info("CI_COMMS_SEND_LAST received (len=%ld)\n", ulApduLen);
		nRtnCode = CiLsc_ProcSendLast(usSsNo, pucApduData, ulApduLen);
		break;

	case CI_COMMS_SEND_MORE:
		HxLOG_Info("CI_COMMS_SEND_MORE received (len=%ld)\n", ulApduLen);
		nRtnCode = CiLsc_ProcSendMore(usSsNo, pucApduData, ulApduLen);
		break;

	default:
		nRtnCode = -1;
		HxLOG_Error("[CILSC]---> CiLsc_ApduProc()---> Error : Unknown Apdu Tag[0x%lx] !!!\n", ulApduTag);
	}

	HxLOG_Info("[CILSC]---> CiLsc_ApduProc()---> SsNo[%d], tag[0x%lx], len[%ld]---> %d\n", usSsNo, ulApduTag, ulApduLen, nRtnCode);

	return nRtnCode;
}

CI_LSC_INSTANCE* CiLsc_GetConnectionInfo(CI_SLOT_ID usSlotId)
{
	HxLOG_Info("[CILSC]---> CiLsc_GetConnectionInfo() \n");

	if (usSlotId >= PAL_CI_GetNumberOfModule())
		return 0;

	return &s_stCiLsc[usSlotId];
}

CI_LSC_INSTANCE* CiLsc_GetConnectionInfoBySsNo(HUINT16 usSsNo)
{
	HUINT32 i = 0;
	HxLOG_Info("CiLsc_GetConnectionInfoBySsNo() \n");

	for (i=0; i<PAL_MAX_CISLOT_NUMBER; i++)
	{
		if(s_stCiLsc[i].usCiLscSsNo == usSsNo)
		{
			return &s_stCiLsc[i];
		}
	}

	HxLOG_Error("ERROR: Check Session Number!! \n");
	return 0;
}

int CiLsc_ProcCommand(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	CI_LSC_CMD				eCommandId = 0;
	CI_LSC_REPLY_TYPE		eCommsReply;
	CI_LSC_INSTANCE			*pstContext;
	unsigned long			ulConnectionDesTag;
	unsigned long			ulConnectionDesLen;
	unsigned char			*pucConnectionDesData;
	unsigned char 			*pucData;
	unsigned char			ucPhaseId;
	CI_SLOT_ID				usSlotId;
	int						nErr = ERR_FAIL;

	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0)
	{
		HxLOG_Error("[CILSC]---> CiLsc_ProcCommand()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return -1;
	}

	if (pucApduData == NULL)
	{
		HxLOG_Error("[CILSC]---> CiLsc_ProcCommand()---> Error : Empty Apdu Data !!!\n");
		return -1;
	}

	pstContext = CiLsc_GetConnectionInfo(usSlotId);
	pucData = pucApduData;
	eCommandId = (CI_LSC_CMD)*pucData;			// SPEC 에서 CMD APDU의 첫번째 바이트는 command ID
	pucData++;

	HxLOG_Info("[CILSC]---> CiLsc_ProcCommand()---> Command ID: [%d] (1: Connect, 2: Disconnect, 3: Set Params, 4: Enquire Status, 5: Get Next Buffer )\n", eCommandId);

	switch (eCommandId)
	{
	case eCiPlus_CommsCmd_Connect:
	{
		if (pstContext->usCiLscSsNo != usSsNo)
		{
			HxLOG_Error("ERROR: Host support just one LSC session for each slot!!\n");
			eCommsReply = eCiPlus_CommsReply_ConnectAck;

			CiLsc_CommsReply(usSsNo, eCommsReply, pstContext);
			break;
		}

		if (pstContext->bConnectionStatus == TRUE)
		{
			CiLsc_TerminateConnection(usSsNo, pstContext);
			pstContext->bConnectionStatus = FALSE;
		}

		ulConnectionDesTag = CIRH_GetApduTag(pucData);
		ulConnectionDesLen = CIRH_GetApduDataLength(pucData);
		pucConnectionDesData = CIRH_GetApduData(pucData);

		HxLOG_Info("[CILSC]---> CiLsc_ProcCommand()---> Connection Descriptor Tag: [%06x], Connection Descriptor Length: [%ld] \n", ulConnectionDesTag, ulConnectionDesLen);

		if(ulConnectionDesTag == CI_CONNECTION_DESCRIPTOR)
		{
			pucData = CiLsc_SetConnection(pucConnectionDesData, ulConnectionDesLen, pstContext);
		}
		else
		{
			HxLOG_Error("[CILSC]---> CiLsc_ProcCommand()---> Error : Wrong CI Connection descriptor tag( The value has to be '9f8c01' )!!!\n");
			return -1;
		}

		if(pucData != NULL)									//NULL값일 경우에 SetConnection에서 셋팅이 제대로 되지 않은 경우임.
		{
			pstContext->ucConnectionRetryCount = *pucData;
			pucData++;
			pstContext->ucConnectionTimeout = *pucData;

			if(pstContext->ucConnectionRetryCount == 0)		// retry count가 0으로 설정 될 경우 connection 시도조차 하지 않으므로 한번은 하도록 값 변경.
				pstContext->ucConnectionRetryCount = 1;		// 만약 spec이나 인증에 위배 된다면 변경 필요.

			HxLOG_Info("[CILSC]---> CiLsc_ProcCommand()---> Connection Retry Count : [%d] \n", pstContext->ucConnectionRetryCount);
			HxLOG_Info("[CILSC]---> CiLsc_ProcCommand()---> Connection Timeout : [%d] \n", pstContext->ucConnectionTimeout);

			nErr = CiLsc_EstablishConnection(usSsNo, pstContext);
			if (nErr == ERR_OK)
			{
				pstContext->bConnectionStatus = TRUE;
			}
			else
			{
				pstContext->bConnectionStatus = FALSE;
			}
		}
		else
		{
			pstContext->bConnectionStatus = FALSE;
			HxLOG_Error("[CILSC]---> CiLsc_ProcCommand()---> Error : Connection Error !!!\n");
		}
		eCommsReply = eCiPlus_CommsReply_ConnectAck;

		CiLsc_CommsReply(usSsNo, eCommsReply, pstContext);
		break;
	}
	case eCiPlus_CommsCmd_Disconnect:
		CiLsc_TerminateConnection(usSsNo, pstContext);
		eCommsReply = eCiPlus_CommsReply_DisconnectAck;
		CiLsc_CommsReply(usSsNo, eCommsReply, pstContext);
		break;

	case eCiPlus_CommsCmd_SetParams:		// Comms Resource가 연결 되기 전에 SetParam을 받아서 해당 연결에 대한 버퍼 설정이 완료 되어야 함. 즉, param이 설정 되어야 connection이 가능 한것으로 파악 됨
		pstContext->usBufferSize = (unsigned short)*pucData << 8;
		pucData++;
		pstContext->usBufferSize += (unsigned short)*pucData;
		pucData++;
		pstContext->ucReceptionTimeout = *pucData;

		HxLOG_Info("[CILSC]---> CiLsc_ProcCommand()---> Buffer Size : [%d] \n", pstContext->usBufferSize);
		HxLOG_Info("[CILSC]---> CiLsc_ProcCommand()---> Reception Timeout : [%d] \n", pstContext->ucReceptionTimeout);

		CiLsc_BufferInit(pstContext->usBufferSize, pstContext);

		eCommsReply = eCiPlus_CommsReply_SetParamsAck;
		CiLsc_CommsReply(usSsNo, eCommsReply, pstContext);
		break;

	case eCiPlus_CommsCmd_EnquireStatus:
		eCommsReply = eCiPlus_CommsReply_StatusReplyAck;
		CiLsc_CommsReply(usSsNo, eCommsReply, pstContext);
		break;

	case eCiPlus_CommsCmd_GetNextBuffer:
		ucPhaseId = *pucData;
		HxLOG_Info("[CILSC]: CMD -> GetNextBuffer :: comms_phase_id [%d]\n", ucPhaseId);
		if (pstContext->bRcvBufferFull == TRUE)
		{
			pstContext->bRcvBufferFull = FALSE;
		}
		CiLsc_RcvBuffer(usSsNo, ucPhaseId, pstContext);
		break;

	default:
		break;
	}
	return 0;
}


int CiLsc_ProcSendMore(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	CI_LSC_REPLY_TYPE		eCommsReply = eCiPlus_CommsReply_SendAck;
	CI_LSC_INSTANCE* 		pstContext;
	unsigned char *			pucData;
	unsigned char			ucPhaseId;
	CI_SLOT_ID				usSlotId;


	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0)
	{
		HxLOG_Error("[CIHCI]---> CiLsc_ProcSendMore()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return -1;
	}

	pstContext = CiLsc_GetConnectionInfo(usSlotId);

	if (pucApduData == NULL)
	{
		HxLOG_Error("[CILSC]---> CiLsc_ProcSendMore()---> Error : Empty Apdu Data !!!\n");
		return -1;
	}

	pucData = pucApduData;
	ucPhaseId = *pucData;
	pucData++;

	HxLOG_Info("[CILSC]---> CiLsc_ProcSendMore()---> comms_send Phase ID: [%02x]\n", ucPhaseId);

	if ((pstContext->ucSendBufferRear + 1) % BUFFER_NUM == ucPhaseId)	// 비어있는 큐 버퍼에 순차적으로 데이터가 채워지는데, ucPhaseID는 스팩에서 나온 것 처럼 CAM쪽에서 버퍼 컨트롤 계산해서 알아서 잘 넘길 것이지만 확인하는 과정
	{
		pstContext->ucSendBufferRear = ucPhaseId;
	}
	else		// error:: 삽입해야 하는 위치가 잘못됨. 아마 CAM쪽이 제대로 설계 되었다면 이부분은 타지 않을 것 임!
	{
		HxLOG_Error("[CILSC]---> CiLsc_ProcSendLast()---> Error : Wrong buffer rear value or wrong phase id !!!\n");
		return -1;
	}

	if (pstContext->usBufferSize != ulApduLen - 1)
	{
		HxLOG_Error("[CILSC]---> CiLsc_ProcSendLast()---> Error : the value of sending data length using SEND_MORE APDU should be same with buffer size!!\n");
		return -1;
	}

	// CICAM에서 Buffer Overflow를 고려해서 Comms_Send APDU를 보내긴 하겠지만 방어적 코드로 Buffer Overflow도 고려 하도록 해야 할듯.

	HxSTD_memcpy(pstContext->paucSendBuffer[pstContext->ucSendBufferRear], pucData, ulApduLen-1);
	pstContext->ucSendBufferCount++;		//more로 들어오는 데이터 갯수 파악하기 위해... 항상 sendBuffer 수행 할 때마다 이 값을 0으로 초기화 시켜야 함.

	if (pstContext->eConnectionType == eCiPlus_Connection_IP)
	{
		if(pstContext->stIpDesc.eProtocolType == eCiPlus_Protocol_TCP)
		{
			HxLOG_Info("[CILSC]---> CiLsc_ProcSendMore()---> Sending buffer using TCP connection.\n");

			CiLsc_SendBuffer(usSsNo, pstContext, ulApduLen-1);
			pstContext->ucSendBufferCount = 0;
			CiLsc_CommsReply(usSsNo, eCommsReply, pstContext);
		}
		else if(pstContext->stIpDesc.eProtocolType == eCiPlus_Protocol_UDP)		// 검증되지 않음. UDP 쪽 Test 필요함.
		{
			// UDP는 buffer에 모든 data를 한번에 쌓아 둔 다음에 보내도록 하려는 의도가 있었지만, 버퍼 하나씩 바로 보내도 문제 없음.
			HxLOG_Info("sBuffer Rear[%d], sBuffer Front[%d], sBuffer Count[%d]\n", pstContext->ucSendBufferRear, pstContext->ucSendBufferFront, pstContext->ucSendBufferCount);
//			if(pstContext->ucSendBufferFront == pstContext->ucSendBufferRear || pstContext->ucSendBufferCount == BUFFER_NUM)
			{
				HxLOG_Info("[CILSC]---> CiLsc_ProcSendMore()---> Sending buffer using UDP connection.\n");

				CiLsc_SendBuffer(usSsNo, pstContext, ulApduLen-1);
				pstContext->ucSendBufferCount = 0;
				CiLsc_CommsReply(usSsNo, eCommsReply, pstContext);
			}
//			else
//			{
//				pstContext->ucSendBufferCount++;
//			}
		}
	}
	else if (pstContext->eConnectionType == eCiPlus_Connection_Telephone)
	{
		/* 전화 연결에 대한 처리 */
	}
	else if (pstContext->eConnectionType == eCiPlus_Connection_CableReturnChannel)
	{
		/* Cable Return Channel 연결에 대한 처리 */
	}
	else if (pstContext->eConnectionType == eCiPlus_Connection_Hostname)
	{
		/* Host name 에 대한 처리 */
		if(pstContext->stHostnameDesc.eProtocolType == eCiPlus_Protocol_TCP)
		{
			HxLOG_Info("Sending buffer using TCP connection.\n");

			CiLsc_SendBuffer(usSsNo, pstContext, ulApduLen-1);
			pstContext->ucSendBufferCount = 0;
			CiLsc_CommsReply(usSsNo, eCommsReply, pstContext);
		}
		else if(pstContext->stHostnameDesc.eProtocolType == eCiPlus_Protocol_UDP)		// 검증되지 않음. UDP 쪽 Test 필요함.
		{
			HxLOG_Info("Sending buffer using UDP connection.\n");

			CiLsc_SendBuffer(usSsNo, pstContext, ulApduLen-1);
			pstContext->ucSendBufferCount = 0;
			CiLsc_CommsReply(usSsNo, eCommsReply, pstContext);
		}
	}

	return 0;
}

int CiLsc_ProcSendLast(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	CI_LSC_REPLY_TYPE		eCommsReply = eCiPlus_CommsReply_SendAck;
	CI_LSC_INSTANCE* 	pstContext;
	unsigned char *			pucData;
	unsigned char				ucPhaseId;
	CI_SLOT_ID				usSlotId;

	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0)
	{
		HxLOG_Error("[CIHCI]---> CiLsc_ProcSendLast()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return -1;
	}

	pstContext = CiLsc_GetConnectionInfo(usSlotId);

	if (pucApduData == NULL)
	{
		HxLOG_Error("[CILSC]---> CiLsc_ProcSendLast()---> Error : Empty Apdu Data !!!\n");
		return -1;
	}

	pucData = pucApduData;

	ucPhaseId = *pucData;
	pucData++;

	HxLOG_Info("[CILSC]---> CiLsc_ProcSendLast()---> comms_send Phase ID: [%02x]\n", ucPhaseId);

	if ((pstContext->ucSendBufferRear + 1) % BUFFER_NUM == ucPhaseId)	// 비어있는 큐 버퍼에 순차적으로 데이터가 채워지는데, ucPhaseID는 스팩에서 나온 것 처럼 CAM쪽에서 버퍼 컨트롤 계산해서 알아서 잘 넘길 것이지만 확인하는 과정
	{
		pstContext->ucSendBufferRear = ucPhaseId;
	}
	else		// error:: 삽입해야 하는 위치가 잘못됨. 아마 CAM쪽이 제대로 설계 되었다면 이부분은 타지 않을 것 임!
	{
		HxLOG_Error("[CILSC]---> CiLsc_ProcSendLast()---> Error : Wrong buffer rear value or wrong phase id !!!\n");
		return -1;
	}

	// CICAM에서 Buffer Overflow를 고려해서 Comms_Send APDU를 보내긴 하겠지만 방어적 코드로 Buffer Overflow도 고려 하도록 해야 할듯.

	HxSTD_memcpy(pstContext->paucSendBuffer[pstContext->ucSendBufferRear], pucData, ulApduLen-1);
	pstContext->ucSendBufferCount++;		//more로 들어오는 데이터 갯수 파악하기 위해... 항상 sendBuffer 수행 할 때마다 이 값을 0으로 초기화 시켜야 함.

	if (pstContext->eConnectionType == eCiPlus_Connection_IP)
	{
		HxLOG_Info("[CILSC]---> CiLsc_ProcSendLast()---> Sending buffer using IP connection.\n");
		CiLsc_SendBuffer(usSsNo, pstContext, ulApduLen-1);
		pstContext->ucSendBufferCount = 0;
		CiLsc_CommsReply(usSsNo, eCommsReply, pstContext);
	}
	else if (pstContext->eConnectionType == eCiPlus_Connection_Telephone)
	{
		/* 전화 연결에 대한 처리 */
	}
	else if (pstContext->eConnectionType == eCiPlus_Connection_CableReturnChannel)
	{
		/* Cable Return Channel 연결에 대한 처리 */
	}
	else if (pstContext->eConnectionType == eCiPlus_Connection_Hostname)
	{
		/* Host name 에 대한 처리 */
		HxLOG_Info("Sending buffer using IP connection.\n");
		CiLsc_SendBuffer(usSsNo, pstContext, ulApduLen-1);
		pstContext->ucSendBufferCount = 0;
		CiLsc_CommsReply(usSsNo, eCommsReply, pstContext);
	}

	return 0;
}


int CiLsc_CommsReply(unsigned short usSsNo, CI_LSC_REPLY_TYPE eCommsReply, CI_LSC_INSTANCE* pstContext)
{
	unsigned char aucMsg[6];

	HxLOG_Info("[CILSC]---> CiLsc_CommsReply()---> [Send CommsReply APDU]---> Reply type: [%d] \n", eCommsReply);

	aucMsg[0] = (unsigned char)((CI_COMMS_REPLY >> 16) & 0xff);
	aucMsg[1] = (unsigned char)((CI_COMMS_REPLY >> 8) & 0xff);
	aucMsg[2] = (unsigned char)(CI_COMMS_REPLY & 0xff);
	aucMsg[3] = 0x02;

	switch(eCommsReply)
	{
	case eCiPlus_CommsReply_ConnectAck:
		aucMsg[4] = (unsigned char)eCommsReply;

		if (pstContext->usCiLscSsNo != usSsNo)
		{
			aucMsg[5] = 0xff;
		}
		else
		{
			if (pstContext->bConnectionStatus == TRUE)
				aucMsg[5] = 0x00;
			else
				aucMsg[5] = 0xff;
		}
		if (CIRH_SendApdu(usSsNo, aucMsg, 6) != ERR_OK)
		{
			HxLOG_Error("Fail to Send APDU... SsNo[%d] !!!\n", usSsNo);
			return ERR_FAIL;
		}
		break;
	case eCiPlus_CommsReply_DisconnectAck:
		aucMsg[4] = (unsigned char)eCommsReply;
		if (pstContext->bConnectionStatus == FALSE)
			aucMsg[5] = 0x00;
		else
			aucMsg[5] = 0xff;
		if (CIRH_SendApdu(usSsNo, aucMsg, 6) != ERR_OK)
		{
			HxLOG_Error("[CILSC]---> Fail to Send APDU... SsNo[%d] !!!\n", usSsNo);
			return ERR_FAIL;
		}
		break;
	case eCiPlus_CommsReply_SetParamsAck:
		aucMsg[4] = (unsigned char)eCommsReply;
		if (pstContext->usBufferSize == 0)				//일단 BufferSize가 0으로 셋팅 되어 있다는 것은 CommsReply에 들어오기 전 Buffer setting을 하는 부분에서 요구하는 사이즈 만큼의 버퍼를 할당 시켜주지 못한 것을 나타 냄. (스펙 내용 아니고, 개발자 임의로...)
		{
			aucMsg[5] = 0xfe;
			if (CIRH_SendApdu(usSsNo, aucMsg, 6) != ERR_OK)
			{
				HxLOG_Error("[CILSC]---> Fail to Send APDU... SsNo[%d] !!!\n", usSsNo);
				return ERR_FAIL;
			}
			return 0;
		}
		if (pstContext->ucReceptionTimeout == 0)			//1)reception timeout 값은 spec상 0이 될 수 없음. 2)network connection이 이뤄지기 전에 set params를 받아서 이미 셋팅이 완료 되어 있어야 함.(|| pstContext->bConnectionStatus == TRUE)
			aucMsg[5] = 0xff;
		else
			aucMsg[5] = 0x00;
		if (CIRH_SendApdu(usSsNo, aucMsg, 6) != ERR_OK)
		{
			HxLOG_Error("[CILSC]---> Fail to Send APDU... SsNo[%d] !!!\n", usSsNo);
			return ERR_FAIL;
		}
		break;
	case eCiPlus_CommsReply_StatusReplyAck:
		aucMsg[4] = (unsigned char)eCommsReply;
		if (pstContext->bConnectionStatus == TRUE)
		{
			aucMsg[5] = 1;		//1은 CIPlus Spec V1.3에 따라 Connected 됨을 나타 냄
		}
		else
		{
			aucMsg[5] = 0;		//0은 CIPlus Spec V1.3에 따라 Disconnected 됨을 나타 냄
		}
		if (CIRH_SendApdu(usSsNo, aucMsg, 6) != ERR_OK)
		{
			HxLOG_Error("[CILSC]---> Fail to Send APDU... SsNo[%d] !!!\n", usSsNo);
			return ERR_FAIL;
		}
		break;
	case eCiPlus_CommsReply_SendAck:
		aucMsg[4] = (unsigned char)eCommsReply;
		aucMsg[5] = pstContext->ucSendBufferFront;		// 나중에 command로 테스트 할때 확인 해보도록 할 것!

		if (CIRH_SendApdu(usSsNo, aucMsg, 6) != ERR_OK)
		{
			HxLOG_Error("[CILSC]---> Fail to Send APDU... SsNo[%d] !!!\n", usSsNo);
			return ERR_FAIL;
		}
		break;
	default:
		break;

	}
	HxLOG_Info("[CILSC]---> CiLsc_CommsReply()---> APDU Raw data: [%02x %02x %02x %02x %02x %02x] \n", aucMsg[0], aucMsg[1], aucMsg[2], aucMsg[3], aucMsg[4], aucMsg[5]);
	return 0;
}


int CiLsc_EstablishConnection(unsigned short usSsNo, CI_LSC_INSTANCE* 	pstContext)
{
	int	nErr = ERR_FAIL;

	switch(pstContext->eConnectionType)
	{
	case eCiPlus_Connection_Telephone:
		break;
	case eCiPlus_Connection_CableReturnChannel:
		break;
	case eCiPlus_Connection_IP:
		nErr = CiLsc_EstablishIpConnection(pstContext);
		break;
	case eCiPlus_Connection_Hostname:
		nErr = CiLsc_EstablishHostnameConnection(pstContext);
		break;
	default:
		break;
	}
	return nErr;
}

int CiLsc_EstablishHostnameConnection(CI_LSC_INSTANCE* pstContext)
{
//	CI_LSC_IpDesc_t					*pstIpDesc = &pstContext->stIpDesc;
	CI_LSC_HostnameDesc_t			*pstHostnameDesc = &pstContext->stHostnameDesc;
	struct hostent					*pstHostEnt;
	struct in_addr					stInAddr;
	struct sockaddr_in 				stServerAddr;
	int	i = 0, nResult = ERR_FAIL;
	long int	*add;

	pstHostEnt = gethostbyname(pstHostnameDesc->pucHostname);

	if (pstHostEnt == NULL)
	{
		HxLOG_Error("[CILSC] Error at gethostbyname()!!\n");
		HxLOG_Error("[CILSC] Error : [%d]\n", errno);
		return ERR_FAIL;
	}
	else
	{
		HxLOG_Info("[LSC] Official Host Name : [%s]\n", pstHostEnt->h_name);
		while(pstHostEnt->h_aliases[i] != NULL)
		{
			HxLOG_Info("[LSC] Aliases Name : [%s]\n", pstHostEnt->h_aliases[i]);
			i++;
		}
		HxLOG_Info("[LSC] Host address type : [%d]\n", pstHostEnt->h_addrtype);
		HxLOG_Info("[LSC] Length of Host Addr : [%d]\n", pstHostEnt->h_length);
		i = 0;

		while(*pstHostEnt->h_addr_list != NULL)
		{
			add = (long int *)*pstHostEnt->h_addr_list;

			stInAddr.s_addr = *add;
			++pstHostEnt->h_addr_list;
		}
	}

	if (pstHostnameDesc->eProtocolType == eCiPlus_Protocol_TCP)
	{
		if((pstContext->nSocketNum = socket(pstHostEnt->h_addrtype, SOCK_STREAM, 0)) < 0)
		{
			HxLOG_Error("[CILSC] Error : Can't create socket (TCP)!!!\n");
			return ERR_FAIL;
		}
	}
	else if (pstHostnameDesc->eProtocolType == eCiPlus_Protocol_UDP)
	{
		if((pstContext->nSocketNum = socket(pstHostEnt->h_addrtype, SOCK_DGRAM, 0)) < 0)
		{
			HxLOG_Error("[CILSC] Error : Can't create socket (UDP)!!!\n");
			return ERR_FAIL;
		}
	}
	else
	{
		HxLOG_Error("[CILSC] Error : Wrong protocol type!!!\n");
		return ERR_FAIL;
	}

	/* 연결할 서버의 소켓주소 구조체 작성 */
	bzero((char *)&stServerAddr, sizeof(stServerAddr));
	// TODO: gethostbyname() 결과에 따라 IPv6인 경우 이에 맞도록 setting / 현재는 IPv4만 처리 가능 함.
	stServerAddr.sin_family = pstHostEnt->h_addrtype;
	stServerAddr.sin_addr.s_addr = stInAddr.s_addr;
	HxLOG_Info("[CILSC]---> CiLsc_EstablishHostnameConnection()---> server Ip Addr: %x \n", stInAddr.s_addr);
	stServerAddr.sin_port = pstContext->stHostnameDesc.usDestinationPort;

	/* 연결요청 */
	for (i=0; i<pstContext->ucConnectionRetryCount; i++)
	{
		/* fix for prevent #133254 */
		nResult = CiLsc_ConnectIpServer(pstContext->nSocketNum, (struct sockaddr *)&stServerAddr, sizeof(stServerAddr), pstContext->ucConnectionTimeout);
		if(ERR_OK == nResult)
		{
			HxLOG_Info("[CILSC]---> CiLsc_EstablishHostnameConnection()---> Successfully connected to the server.\n");
			return ERR_OK;
		}
		else
		{
			HxLOG_Error("[CILSC]---> CiLsc_EstablishHostnameConnection()---> Error : Can't connect to server!!!\n");
			HxLOG_Info("[CILSC]---> CiLsc_EstablishHostnameConnection()---> [%d]th Connection Retry. \n", i+1);
		}
	}
	return ERR_FAIL;

}


int CiLsc_EstablishIpConnection(CI_LSC_INSTANCE* pstContext)
{
	struct sockaddr_in 	server_addr;
	unsigned long		ulServerIpAddr = 0;
	int				i = 0, nResult = ERR_FAIL;

	bzero((char *)&server_addr, sizeof(server_addr));

	if (pstContext->stIpDesc.eIpProtocolVer == eCiPlus_IpProtocol_IPv4)
	{
		if (pstContext->stIpDesc.eProtocolType == eCiPlus_Protocol_TCP)
		{
			if((pstContext->nSocketNum = socket(PF_INET, SOCK_STREAM, 0)) < 0)
			{
				HxLOG_Error("[CILSC]---> CiLsc_EstablishIpConnection()---> Error : Can't create socket (TCP)!!!\n");
				return ERR_FAIL;
			}
		}
		else if (pstContext->stIpDesc.eProtocolType == eCiPlus_Protocol_UDP)
		{
			if((pstContext->nSocketNum = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
			{
				HxLOG_Error("[CILSC]---> CiLsc_EstablishIpConnection()---> Error : Can't create socket (UDP)!!!\n");
				return ERR_FAIL;
			}
		}
		else
		{
			HxLOG_Error("[CILSC]---> CiLsc_EstablishIpConnection()---> Error : Wrong protocol type!!!\n");
			return ERR_FAIL;
		}
		server_addr.sin_family = AF_INET;
	}
	else if (pstContext->stIpDesc.eIpProtocolVer == eCiPlus_IpProtocol_IPv6)
	{
		if (pstContext->stIpDesc.eProtocolType == eCiPlus_Protocol_TCP)
		{
			if((pstContext->nSocketNum = socket(PF_INET6, SOCK_STREAM, 0)) < 0)
			{
				HxLOG_Error("[CILSC]---> CiLsc_EstablishIpConnection()---> Error : Can't create socket (TCP)!!!\n");
				return ERR_FAIL;
			}
		}
		else if (pstContext->stIpDesc.eProtocolType == eCiPlus_Protocol_UDP)
		{
			if((pstContext->nSocketNum = socket(PF_INET6, SOCK_DGRAM, 0)) < 0)
			{
				HxLOG_Error("[CILSC]---> CiLsc_EstablishIpConnection()---> Error : Can't create socket (UDP)!!!\n");
				return ERR_FAIL;
			}
		}
		else
		{
			HxLOG_Error("[CILSC]---> CiLsc_EstablishIpConnection()---> Error : Wrong protocol type!!!\n");
			return ERR_FAIL;
		}
		server_addr.sin_family = AF_INET6;
	}
	else
	{
		HxLOG_Error("[CILSC]---> CiLsc_EstablishIpConnection()---> Error : Wrong IP protocol version!!!\n");
		return ERR_FAIL;
	}

	ulServerIpAddr =
	#ifdef CONFIG_PROD_ENDIAN_LITTLE
	(pstContext->stIpDesc.aucIpAddress[12])|(pstContext->stIpDesc.aucIpAddress[13]<<8)|(pstContext->stIpDesc.aucIpAddress[14]<<16)|(pstContext->stIpDesc.aucIpAddress[15]<<24);
	#else
	(pstContext->stIpDesc.aucIpAddress[12]<<24)|(pstContext->stIpDesc.aucIpAddress[13]<<16)|(pstContext->stIpDesc.aucIpAddress[14]<<8)|(pstContext->stIpDesc.aucIpAddress[15]);
	#endif
	HxLOG_Info("[CILSC]---> CiLsc_EstablishIpConnection()---> Server IP Addr: %012x \n",ulServerIpAddr);

	/* 연결할 서버의 소켓주소 구조체 작성 */
	server_addr.sin_addr.s_addr = ulServerIpAddr;
	HxLOG_Info("[CILSC]---> CiLsc_EstablishIpConnection()---> server Ip Addr: %x \n", ulServerIpAddr);
	server_addr.sin_port = pstContext->stIpDesc.usDestinationPort;

	/* 연결요청 */
	for (i=0; i<pstContext->ucConnectionRetryCount; i++)
	{
		/* fix for prevent #133255 */
		nResult = CiLsc_ConnectIpServer(pstContext->nSocketNum, (struct sockaddr *)&server_addr, sizeof(server_addr), pstContext->ucConnectionTimeout);
		if(ERR_OK == nResult)
		{
			HxLOG_Info("[CILSC]---> CiLsc_EstablishIpConnection()---> Successfully connected to the server.\n");
			return ERR_OK;
		}
		else
		{
			HxLOG_Error("[CILSC]---> CiLsc_EstablishIpConnection()---> Error : Can't connect to server!!!\n");
			HxLOG_Info("[CILSC]---> CiLsc_EstablishIpConnection()---> [%d]th Connection Retry. \n", i+1);
		}
	}
	return ERR_FAIL;
}


int CiLsc_TerminateConnection(unsigned short usSsNo, CI_LSC_INSTANCE* pstContext)
{
	int						nErr = ERR_FAIL;

	while (1)
	{
		if (pstContext->sRcvMsgCount > 1)
		{
			VK_TASK_Sleep(50);
			HxLOG_Info("Wating for rcv data...\n");
		}
		else
		{
			break;
		}
	}

	if (pstContext->bConnectionStatus == TRUE)
	{
		switch(pstContext->eConnectionType)
		{
		case eCiPlus_Connection_Telephone:
			break;
		case eCiPlus_Connection_CableReturnChannel:
			break;
		case eCiPlus_Connection_IP:
			nErr = CiLsc_TerminateIpConnection(pstContext);
			break;
		case eCiPlus_Connection_Hostname:
			nErr = CiLsc_TerminateHostnameConnection(pstContext);
			break;
		default:
			break;
		}
	}
	else
	{
		nErr = ERR_OK;
	}

	if (nErr == ERR_OK)
	{
		pstContext->bConnectionStatus = FALSE;
	}

	return nErr;
}


int CiLsc_TerminateIpConnection(CI_LSC_INSTANCE* pstContext)
{
	int i = 0;

	/*socket 통신 이용해서 구현 해야 함 */
	close(pstContext->nSocketNum);
	pstContext->nSocketNum = 0;
	/*통신 종료 시 버퍼에 할당 되어있는 데이터 들도 모두 반환 하도록 할 것! */
	for(i=0; i<BUFFER_NUM; i++)
	{
		HxSTD_memset(pstContext->paucSendBuffer[i] , 0, sizeof(unsigned char) * pstContext->usBufferSize);
	}

	pstContext->ucRcvBufferFront 	= BUFFER_NUM - 1;
	pstContext->ucRcvBufferRear 	= BUFFER_NUM - 1;
	pstContext->ucRcvBufferCount 	= 0;
	pstContext->ucSendBufferFront	= BUFFER_NUM - 1;
	pstContext->ucSendBufferRear	= BUFFER_NUM - 1;
	pstContext->ucSendBufferCount 	= 0;

	pstContext->bConnectionStatus = FALSE;

	HxLOG_Info("[CILSC]---> CiLsc_TerminateIpConnection()---> Successfully disconnected!\n");
	return ERR_OK;
}

int CiLsc_TerminateHostnameConnection(CI_LSC_INSTANCE * pstContext)
{
	int i = 0;

	/*socket 통신 이용해서 구현 해야 함 */
	close(pstContext->nSocketNum);
	pstContext->nSocketNum = 0;
	/*통신 종료 시 버퍼에 할당 되어있는 데이터 들도 모두 반환 하도록 할 것! */
	for(i=0; i<BUFFER_NUM; i++)
	{
		HxSTD_memset(pstContext->paucSendBuffer[i] , 0, sizeof(unsigned char) * pstContext->usBufferSize);
	}

	pstContext->ucRcvBufferFront 	= BUFFER_NUM - 1;
	pstContext->ucRcvBufferRear 		= BUFFER_NUM - 1;
	pstContext->ucRcvBufferCount 	= 0;
	pstContext->ucSendBufferFront	= BUFFER_NUM - 1;
	pstContext->ucSendBufferRear		= BUFFER_NUM - 1;
	pstContext->ucSendBufferCount 	= 0;

	pstContext->bConnectionStatus 	= FALSE;

	if (pstContext->stHostnameDesc.pucHostname != NULL)
	{
		OxCAS_Free(pstContext->stHostnameDesc.pucHostname);
		pstContext->stHostnameDesc.pucHostname = NULL;
	}

	HxLOG_Info("[CILSC]---> CiLsc_TerminateHostnameConnection()---> Successfully disconnected!\n");
	return ERR_OK;
}


int CiLsc_BufferInit(unsigned short usBufferSize, CI_LSC_INSTANCE* pstContext)
{
	int i = 0;

	if (pstContext->bBufferInitialized == TRUE)
	{
		for(i=0; i<BUFFER_NUM; i++)
		{
			if(pstContext->paucSendBuffer[i])
			{
				OxCAS_Free(pstContext->paucSendBuffer[i]);
				pstContext->paucSendBuffer[i] = NULL;
			}
		}
		pstContext->bBufferInitialized = FALSE;
	}

	for(i=0; i<BUFFER_NUM; i++)
	{
		pstContext->paucSendBuffer[i] = (unsigned char*)OxCAS_Malloc(sizeof(unsigned char) * usBufferSize);
		HxSTD_memset(pstContext->paucSendBuffer[i] , 0, sizeof(unsigned char) * usBufferSize);
	}

	pstContext->ucRcvBufferFront 	= BUFFER_NUM - 1;
	pstContext->ucRcvBufferRear 	= BUFFER_NUM - 1;
	pstContext->ucRcvBufferCount 	= 0;
	pstContext->ucSendBufferFront	= BUFFER_NUM - 1;
	pstContext->ucSendBufferRear	= BUFFER_NUM - 1;
	pstContext->ucSendBufferCount 	= 0;

	pstContext->bBufferInitialized = TRUE;
	HxLOG_Info("[CILSC]---> CiLsc_BufferInit()---> Buffer Initialized!! \n");

	return 0;
}


int CiLsc_RcvBuffer(unsigned short usSsNo, unsigned char ucPhaseId, CI_LSC_INSTANCE* pstContext)
{
	CI_LSC_CONNECT_TYPE	eConnectionType = pstContext->eConnectionType;

	HxLOG_Info("[CILSC]---> CiLsc_RcvBuffer() \n");

	if (pstContext->bConnectionStatus == FALSE)
	{
		// 어떠한 리소스에라도 연결이 되어 있지 않다면 send 자체를 할 수 없다.
		HxLOG_Error("[CILSC]---> CiLsc_RcvBuffer()---> Error : Communication resource is not connected. \n");
		return ERR_FAIL;
	}

	if (ucPhaseId < 0 || ucPhaseId > 15)
	{
		HxLOG_Error("[CILSC]---> CiLsc_RcvBuffer()---> Error : Wrong Phase ID. \n");
		return ERR_FAIL;
	}


	/* 아래의 buffer size 및 reception timeout 에 대한 방어적은 코드는 CAM쪽에서 스펙상 오면 안되는 값(0)을 받았을때 대응 할 수 있도록 추가 하였음. */
	if (pstContext->usBufferSize == 0)
	{
		pstContext->usBufferSize = 254;				//SPEC 에 따른 default 값
		CiLsc_BufferInit(pstContext->usBufferSize, pstContext);
		HxLOG_Info("[CILSC]---> CiLsc_RcvBuffer()---> The buffer size was not initialized. So, the value of the size is 254 as default!\n");
	}

	if(pstContext->ucReceptionTimeout == 0 )
	{
		pstContext->ucReceptionTimeout = 10;		// Spec에따라 해당 값이 setting 되지 않았을 때, default: 10(100ms)으로 설정 됨.
		HxLOG_Info("[CILSC]---> CiLsc_RcvBuffer()---> Reception Timeout value can't be 0. So it is set 10 as default value!\n");
	}

	HxLOG_Info("[CILSC]---> CiLsc_RcvBuffer()---> Received Phase ID (CICAM -> Host) : [%d] (It means the front of queue) \n", ucPhaseId);
	HxLOG_Info("[CILSC]---> CiLsc_RcvBuffer()---> Connection Type : [%d] \n", eConnectionType);

	switch(eConnectionType)
	{
	case eCiPlus_Connection_Telephone:
		break;
	case eCiPlus_Connection_CableReturnChannel:
		break;
	case eCiPlus_Connection_IP:
	case eCiPlus_Connection_Hostname:
		CiLsc_RcvBufferUsingIpConnection(usSsNo, ucPhaseId, pstContext);
		break;
	default:
		break;
	}

	pstContext->sRcvMsgCount--;
	HxLOG_Info("sRcvMsgCount: [%d]\n", pstContext->sRcvMsgCount);

	return ERR_OK;
}


int CiLsc_RcvBufferUsingIpConnection(unsigned short usSsNo, unsigned char ucPhaseId, CI_LSC_INSTANCE* pstContext)
{
	unsigned short 	usBufferSize = pstContext->usBufferSize;
	unsigned char	ucReceptionTimeout;
	unsigned char*	pucApduData = NULL;
	unsigned char*	paucTempBuffer[BUFFER_NUM];
	unsigned short	usApduLength = 0;
	unsigned short	usDataLength = 0;
	int				nStatus;
	HERROR			hErr = ERR_OK;
 	fd_set			rset;
	struct timeval 	tval;

	pstContext->ucRcvBufferFront = ucPhaseId;
	pstContext->ucRcvBufferCount = 0;

	ucReceptionTimeout = pstContext->ucReceptionTimeout;

	HxLOG_Info("[CILSC]---> CiLsc_RcvBufferUsingIpConnection()\n");

	FD_ZERO(&rset);
	FD_SET(pstContext->nSocketNum, &rset);
	tval.tv_sec = 0;
	tval.tv_usec = ucReceptionTimeout * 10000;

	while(1)
	{
		if(pstContext->bConnectionStatus == FALSE)
			break;

		FD_ZERO(&rset);
		FD_SET(pstContext->nSocketNum, &rset);

		tval.tv_sec = 0;
		tval.tv_usec = 100000; //50000; 	// 50ms
		nStatus = select(pstContext->nSocketNum + 1, &rset, NULL, NULL, &tval);

		if (nStatus == 0)		// Timeout에 걸렸을 때
		{
			if(pstContext->sRcvMsgCount - 1 > 0)		// 지금 Msg를 소모 해도 다음에 더 처리해야 할 msg가 있을 때.
			{
				break;
			}
			else
			{
				continue;
			}
		}

		if(nStatus > 0 && FD_ISSET(pstContext->nSocketNum, &rset))
		{
			HxLOG_Info("...\n");

			if (pstContext->bRcvBufferFull == TRUE)
			{
				HxLOG_Info("Buffer overflow. \n");
				hErr = ERR_OK;
				goto EXIT;
			}

			if(pstContext->ucRcvBufferFront == ((pstContext->ucRcvBufferRear + 1) % BUFFER_NUM))
			{
				pstContext->bRcvBufferFull = TRUE;
			}

			pstContext->ucRcvBufferRear = (pstContext->ucRcvBufferRear + 1) % BUFFER_NUM;
			paucTempBuffer[pstContext->ucRcvBufferRear] = (unsigned char *) OxCAS_Malloc(sizeof(unsigned char) * usBufferSize);
			HxLOG_Info("Temp buffer is allocated... Buffer Size: [%d] \n", usBufferSize);

			usDataLength = (unsigned short)read(pstContext->nSocketNum, (void *)paucTempBuffer[pstContext->ucRcvBufferRear], (int)usBufferSize);
			HxLOG_Info("Readed buffer size from Server: [%d] \n", usDataLength);

			if (usDataLength > 0)
			{
				pucApduData = CiLsc_MakeRcvApdu(usBufferSize, pstContext->ucRcvBufferRear, usDataLength, paucTempBuffer[pstContext->ucRcvBufferRear], &usApduLength);
				HxLOG_Info("APDU Length: [%d]\n", usApduLength);
				HxLOG_Info("Phase ID: [%d]\n", pstContext->ucRcvBufferRear);

				if ( (pstContext->eConnectionType == eCiPlus_Connection_IP && pstContext->stIpDesc.eProtocolType == eCiPlus_Protocol_TCP)
					|| (pstContext->eConnectionType == eCiPlus_Connection_Hostname && pstContext->stHostnameDesc.eProtocolType == eCiPlus_Protocol_TCP) )
				{
					HxLOG_Info("Send COMMs_Rcv_APDU using TCP connection\n");
					if (CIRH_SendApdu(usSsNo, pucApduData, usApduLength) != ERR_OK)
					{
						HxLOG_Error("Fail to Send APDU... SsNo[%d] !!!\n", usSsNo);
						if (pucApduData)
						{
							OxCAS_Free(pucApduData);
							pucApduData = NULL;
						}
						if (paucTempBuffer[pstContext->ucRcvBufferRear])
						{
							OxCAS_Free(paucTempBuffer[pstContext->ucRcvBufferRear]);
							paucTempBuffer[pstContext->ucRcvBufferRear] = NULL;
						}
						hErr = ERR_FAIL;
						goto EXIT;
					}					
					if (paucTempBuffer[pstContext->ucRcvBufferRear])
					{
						OxCAS_Free(paucTempBuffer[pstContext->ucRcvBufferRear]);
						paucTempBuffer[pstContext->ucRcvBufferRear] = NULL;
					}
				}
				// TODO: UDP 에 대한 처리사항이 명확해지면 다시 확인해보고 코드 변경 필요!!
				else if ( (pstContext->eConnectionType == eCiPlus_Connection_IP && pstContext->stIpDesc.eProtocolType == eCiPlus_Protocol_UDP)
					|| (pstContext->eConnectionType == eCiPlus_Connection_Hostname && pstContext->stHostnameDesc.eProtocolType == eCiPlus_Protocol_UDP) )
				{
					HxLOG_Info("Send COMMs_Rcv_APDU using UDP connection\n");
					if (CIRH_SendApdu(usSsNo, pucApduData, usApduLength) != ERR_OK)
					{
						HxLOG_Error("Fail to Send APDU... SsNo[%d] !!!\n", usSsNo);
						if (pucApduData)
						{
							OxCAS_Free(pucApduData);
							pucApduData = NULL;
						}
						if (paucTempBuffer[pstContext->ucRcvBufferRear])
						{
							OxCAS_Free(paucTempBuffer[pstContext->ucRcvBufferRear]);
							paucTempBuffer[pstContext->ucRcvBufferRear] = NULL;
						}
						hErr = ERR_FAIL;
						goto EXIT;
					}
					if (paucTempBuffer[pstContext->ucRcvBufferRear])
					{
						OxCAS_Free(paucTempBuffer[pstContext->ucRcvBufferRear]);
						paucTempBuffer[pstContext->ucRcvBufferRear] = NULL;
					}

	//				pstContext->ucRcvBufferCount++;
	/*				else
					{
						if (usDataLength == usBufferSize)
						{
							pstContext->ucRcvBufferCount++;
						}
						else
						{
							for (i=0; pstContext->ucRcvBufferCount; i++)
							{
							}
							if (CIRH_SendApdu(usSsNo, pucApduData, usApduLength) != ERR_OK)
							{
								HxLOG_Error("[CILSC]---> Fail to Send APDU... SsNo[%d] !!!\n", usSsNo);
								return ERR_FAIL;
							}
							OxCAS_Free(paucTempBuffer[pstContext->ucRcvBufferRear]);
						}
					}
	*/
				}

				if (pucApduData)
				{
					OxCAS_Free(pucApduData);
					pucApduData = NULL;
				}

				if (usDataLength < usBufferSize)
				{
					HxLOG_Info("Last Rcv APDU is sended.\n");
					break;
				}
			}
			else
			{
				HxLOG_Info("No more data getting from server.\n");
				if(paucTempBuffer[pstContext->ucRcvBufferRear] != NULL)
				{
					OxCAS_Free(paucTempBuffer[pstContext->ucRcvBufferRear]);
					paucTempBuffer[pstContext->ucRcvBufferRear] = NULL;
				}
				pstContext->ucRcvBufferRear = pstContext->ucRcvBufferRear - 1;
				if (pstContext->ucRcvBufferRear == -1)
				{
					pstContext->ucRcvBufferRear  = 15;
				}

				// TODO: 아래 부분 검증 필요.
				// Select()에서 nStatus 가 1로 변경 사항이 있다고 나오는데, length가 0이라는 것은 connection이 끊겼음을 의미(?) 그렇다면 아래 내용이 맞음.
				// connection이 끊겼을 경우 CAM 쪽에 Reply 해줌.
				CiLsc_TerminateConnection(usSsNo, pstContext);
				CiLsc_CommsReply(usSsNo, eCiPlus_CommsReply_StatusReplyAck, pstContext);

				break;
			}
		}
		else
		{
			HxLOG_Info("CHECK!! \n");		// 만약 이 문장이 출력되면 왜 나왔는지 확인 해볼것!
			break;
		}
	}

	hErr = ERR_OK;

	EXIT:
	if (pucApduData)
		OxCAS_Free(pucApduData);

	return hErr;
}


int CiLsc_SendBuffer(unsigned short usSsNo, CI_LSC_INSTANCE* pstContext, unsigned long ulDataLength)
{
	unsigned char		ucFront 			= pstContext ->ucSendBufferFront;
	unsigned char		ucRear 			= pstContext ->ucSendBufferRear;
	unsigned char		ucBufferCount 	= pstContext ->ucSendBufferCount;
	unsigned short		usBufferSize 	= pstContext ->usBufferSize;
	unsigned long		ulFullDataLength;
	unsigned char* 		pucSendData = NULL;
	unsigned char*		pucTempPosition = NULL;
	int 				nCheckBufferLength;
	int					i = 0;

	CI_LSC_CONNECT_TYPE	eConnectionType = pstContext->eConnectionType;

	if (pstContext->bConnectionStatus == FALSE)
	{
		// 어떠한 리소스에라도 연결이 되어 있지 않다면 send 자체를 할 수 없다.
		HxLOG_Error("[CILSC]---> CiLsc_SendBuffer()---> Error : Communication resource is not connected. \n");
		return ERR_FAIL;
	}

	if (ucFront > ucRear)			//원형 큐이기 때문에 큐 삭제 되는 부분이 큐 삽입 되는 부분보다 높은 값을 가질 수 있기 때문에 이에 대한 처리.
	{
		nCheckBufferLength = ucRear + BUFFER_NUM - ucFront;
	}
	else
	{
		nCheckBufferLength = ucRear - ucFront;
	}

	if (ucBufferCount > nCheckBufferLength)
	{
		// error message
		HxLOG_Error("[CILSC]---> CiLsc_SendBuffer()---> Error : the number of continuous APDU data(SEND_MOREs) is cannot be bigger than datas in the buffer!! \n");
		return ERR_FAIL;
	}

	ulFullDataLength = (usBufferSize * (ucBufferCount-1)) + ulDataLength;

	HxLOG_Info("[CILSC]---> CiLsc_SendBuffer()---> Sending data size: [%d]  \n", ulFullDataLength);

	pucSendData = (unsigned char*)OxCAS_Malloc(sizeof(unsigned char) * ulFullDataLength);
	pucTempPosition = pucSendData;

	for( i = 0; i < ucBufferCount-1; i++)	// 여기에서 i < ucBufferCount까지 보내는데, 사실 이 값은 nCheckBufferLength 와 값이 같아야 할 것으로 생각되며, nCheckBufferLength 만큼 전송해야 할 것 같기도 함.
	{
		if (pstContext->ucSendBufferFront == pstContext->ucSendBufferRear )
		{
			HxLOG_Error("[CILSC]---> CiLsc_SendBuffer()---> Error : Buffer underflow \n");
			if(pucSendData)
			{
				OxCAS_Free(pucSendData);
				pucSendData = NULL;
			}
			return ERR_FAIL;
		}
		pstContext->ucSendBufferFront = (pstContext->ucSendBufferFront + 1 ) % BUFFER_NUM;
		HxSTD_memcpy(pucTempPosition, pstContext->paucSendBuffer[pstContext->ucSendBufferFront], usBufferSize);
		HxSTD_memset(pstContext->paucSendBuffer[pstContext->ucSendBufferFront], 0, usBufferSize);
		pucTempPosition += usBufferSize;
	}

	pstContext->ucSendBufferFront = (pstContext->ucSendBufferFront + 1 ) % BUFFER_NUM;
	HxSTD_memcpy(pucTempPosition, pstContext->paucSendBuffer[pstContext->ucSendBufferFront], ulDataLength);
	HxSTD_memset(pstContext->paucSendBuffer[pstContext->ucSendBufferFront], 0, usBufferSize);

	switch(eConnectionType)
	{
	case eCiPlus_Connection_Telephone:
		break;
	case eCiPlus_Connection_CableReturnChannel:
		break;
	case eCiPlus_Connection_IP:
	case eCiPlus_Connection_Hostname:
		CiLsc_SendBufferUsingIpConnection(pstContext, pucSendData, sizeof(unsigned char) * ulFullDataLength);
		break;
	default:
		break;
	}

	if(pucSendData)
	{
		OxCAS_Free(pucSendData);
		pucSendData = NULL;
	}

	return ERR_OK;
}


int CiLsc_SendBufferUsingIpConnection(CI_LSC_INSTANCE* pstContext, unsigned char* pucSendData, int nDataLength)
{
	/* 소켓통신을 통해서 데이터를 전송하도록 한다. */
	int nReturn;

	if ((nReturn = write(pstContext->nSocketNum, pucSendData, nDataLength)) < 0)
	{
		HxLOG_Error("[CILSC]---> CiLsc_SendBufferUsingIpConnection()---> Error : Error during sending data using IP connection.\n");
		return -1;
	}
	HxLOG_Info("[CILSC]---> CiLsc_SendBufferUsingIpConnection()---> The length of sended data: [%d].\n", nReturn);

#if 0
	int i;
	HxLOG_Info("[CILSC]---> CiLsc_SendBufferUsingIpConnection()---> Send APDU data: [ ");
	for ( i=0; i<nDataLength; i++)
	{
		HxLOG_Print("%02x ", pucSendData[i]);
	}
	HxLOG_Print("]\n");
#endif

	return 0;
}


unsigned char* CiLsc_MakeRcvApdu(unsigned short usBufferSize, unsigned char ucBufferRear, unsigned short usDataLength, unsigned char* pucData, unsigned short* pusApduLength)
{
	unsigned char*	pucCurPoint;
	unsigned short	usApduDataLength;
	unsigned char* 	pucApduData = NULL;
	unsigned char 	ucLenFieldSize;
	unsigned long	ulOffset = 0;

	HxLOG_Info("Make Receive APDU to send buffer to the CICAM.\n");

	usApduDataLength = usDataLength + 1;		//APDU로 만들 Data에 comms_phase_id(1byte) 값이 들어 간 것이 APDU의 DATA 영역이므로 그 길이는 data length +1이 됨.

	pucApduData = (unsigned char *) OxCAS_Malloc(8 + usApduDataLength);

	//	TODO: 바로 아래줄로 처리하면 받아오는 data가 더이상은 없지만 buffer size 랑 완전 같을 경우엔 LAST로 보내야 할 것을 MORE로 보낼 수 있으니 확인 해볼것!!
	if(usDataLength == usBufferSize)			// 받아온 data의 길이가 buffer size와 같다면 해당 내용을 CAM쪽에 More APDU로 보내야 한다.
	{
		HxLOG_Info("Make CI_COMMS_RCV_MORE APDU.\n");

		pucApduData[0] = (unsigned char)((CI_COMMS_RCV_MORE >> 16) & 0xff);
		pucApduData[1] = (unsigned char)((CI_COMMS_RCV_MORE >> 8) & 0xff);
		pucApduData[2] = (unsigned char)(CI_COMMS_RCV_MORE & 0xff);

		CIRH_ConvertApduLengthField(usApduDataLength, &pucApduData[3], &ucLenFieldSize);

		ulOffset = 3 + ucLenFieldSize;		// TAG 길이 3 + Length Field size

		pucApduData[ulOffset++] = ucBufferRear;
		pucCurPoint = &pucApduData[ulOffset++];
	}
	else
	{
		HxLOG_Info("Make CI_COMMS_RCV_LAST APDU.\n");

		pucApduData[0] = (unsigned char)((CI_COMMS_RCV_LAST >> 16) & 0xff);
		pucApduData[1] = (unsigned char)((CI_COMMS_RCV_LAST >> 8) & 0xff);
		pucApduData[2] = (unsigned char)(CI_COMMS_RCV_LAST & 0xff);
		CIRH_ConvertApduLengthField(usApduDataLength, &pucApduData[3], &ucLenFieldSize);

		ulOffset = 3 + ucLenFieldSize;		// TAG 길이 3 + Length Field size

		pucApduData[ulOffset++] = ucBufferRear;
		pucCurPoint = &pucApduData[ulOffset++];
	}

	HxSTD_memcpy(pucCurPoint, pucData, usDataLength);
	*pusApduLength = 3 + ucLenFieldSize + usApduDataLength;

	HxLOG_Info("APDU Length(which will be sended): [%d] \n", *pusApduLength);

	return pucApduData;
}


unsigned char* CiLsc_SetIpConnection(unsigned char *pucApduData, CI_LSC_IpDesc_t* pstIpDesc)
{
	unsigned char* pucData;

	pucData = pucApduData;

	HxLOG_Info("[CILSC]---> CiLsc_SetIpConnection()---> IP Descriptor tag: %02x \n", *pucData);
	if(*pucData == IP_DESCRIPTOR_TAG)
	{
		pucData++;
		if(*pucData != IP_DESCRIPTOR_LENG - 2)			//원래 IP Descriptor의 길이에서 tag와 leng 가 들어있는 2byte를 제외한 값이 들어가 있어야 올바른 데이터 임.
		{
			HxLOG_Error("[CILSC]---> CiLsc_SetIpConnection()---> Error :  Wrong IP descriptor !!!\n");
			return NULL;
		}
		pucData++;
		pstIpDesc->eIpProtocolVer = (CI_LSC_IP_VERSION)*pucData;
		pucData++;

#if 0
		HxLOG_Info("[CILSC]---> CiLsc_SetIpConnection()---> IP address(Raw Data):: [");
		for( i=0; i<16; i++)	// Spec에서 IP address를 총 16바이트에 걸쳐서 저장함
		{
			pstIpDesc->aucIpAddress[i] = (unsigned char)*pucData;
			HxLOG_Print("%02x ", pstIpDesc->aucIpAddress[i]);
			pucData++;
		}
		HxLOG_Print("]\n");
#endif

#ifdef CONFIG_PROD_ENDIAN_LITTLE
		pstIpDesc->usDestinationPort =  (unsigned short)*pucData;
		pucData++;
		pstIpDesc->usDestinationPort += (unsigned short)*pucData << 8;
		pucData++;
#else
		pstIpDesc->usDestinationPort = (unsigned short)*pucData << 8;
		pucData++;
		pstIpDesc->usDestinationPort += (unsigned short)*pucData;
		pucData++;
#endif

		pstIpDesc->eProtocolType = (CI_LSC_PROTOCOL_TYPE)*pucData;
		pucData++;
	}
	else
	{
		HxLOG_Error("[CILSC]---> CiLsc_SetIpConnection()---> Error :  Wrong IP descriptor Tag: [%02x] !!!\n", *pucData);
		return NULL;
	}
	HxLOG_Info("[CILSC]---> CiLsc_SetIpConnection()---> IP Protocol Version: [%d] \n", pstIpDesc->eIpProtocolVer);
	HxLOG_Info("[CILSC]---> CiLsc_SetIpConnection()---> IP Address: [%s] \n", pstIpDesc->aucIpAddress);
	HxLOG_Info("[CILSC]---> CiLsc_SetIpConnection()---> Destination Port: [%x] \n", pstIpDesc->usDestinationPort);
	HxLOG_Info("[CILSC]---> CiLsc_SetIpConnection()---> Protocol Type: [%d] \n", pstIpDesc->eProtocolType);
	return pucData;
}

unsigned char* CiLsc_SetHostnameConnection(unsigned char *pucApduData, unsigned long ulApduLen, CI_LSC_HostnameDesc_t* pstHostnameDesc)
{
	unsigned char* 	pucData;
	int				nHostnameLen = ulApduLen - 5;	// Hostname 길이는 전체 Desc 길이에서 5byte(tag(1byte), length(1byte), protocol type(1byte), port#(2byte))를 제외한 값.

	pucData = pucApduData;

	HxLOG_Info("[CILSC]---> CiLsc_SetHostnameConnection()---> Host name Descriptor tag: %02x \n", *pucData);
	if(*pucData == HOSTNAME_DESCRIPTOR_TAG)
	{
		pucData++;
		if(*pucData != ulApduLen - 2)			//Hostname Descriptor의 전체 길이에서 tag와 leng 가 들어있는 2byte를 제외한 값이 들어가 있어야 올바른 데이터 임.
		{
			HxLOG_Error("[CILSC]---> CiLsc_SetHostnameConnection()---> Error :  Wrong Hostname descriptor !!!\n");
			return NULL;
		}
		pucData++;

		pstHostnameDesc->eProtocolType = (CI_LSC_PROTOCOL_TYPE)*pucData;
		pucData++;

#ifdef CONFIG_PROD_ENDIAN_LITTLE
		pstHostnameDesc->usDestinationPort =  (unsigned short)*pucData;
		pucData++;
		pstHostnameDesc->usDestinationPort += (unsigned short)*pucData << 8;
		pucData++;
#else
		pstHostnameDesc->usDestinationPort = (unsigned short)*pucData << 8;
		pucData++;
		pstHostnameDesc->usDestinationPort += (unsigned short)*pucData;
		pucData++;
#endif
		pstHostnameDesc->pucHostname = (unsigned char *)OxCAS_Malloc(sizeof(unsigned char) * nHostnameLen + 1);
		HxSTD_memset(pstHostnameDesc->pucHostname, 0, nHostnameLen+1);
		HxSTD_memcpy(pstHostnameDesc->pucHostname, pucData, nHostnameLen);
		pucData = pucData + nHostnameLen;
	}
	else
	{
		HxLOG_Error("[CILSC]---> CiLsc_SetHostnameConnection()---> Error :  Wrong IP descriptor Tag: [%02x] !!!\n", *pucData);
		return NULL;
	}
	HxLOG_Info("[CILSC]---> CiLsc_SetHostnameConnection()---> Host Name: [%s] \n", pstHostnameDesc->pucHostname);
	HxLOG_Info("[CILSC]---> CiLsc_SetHostnameConnection()---> Destination Port: [%x] \n", pstHostnameDesc->usDestinationPort);
	HxLOG_Info("[CILSC]---> CiLsc_SetHostnameConnection()---> Protocol Type: [%d] \n", pstHostnameDesc->eProtocolType);
	return pucData;
}

unsigned char* CiLsc_SetConnection(unsigned char *pucApduData, unsigned long ulApduLen, CI_LSC_INSTANCE* 	pstContext)
{
	unsigned char* pucData = NULL;

	pstContext->eConnectionType = (CI_LSC_CONNECT_TYPE)pucApduData[0];
	HxLOG_Info("[CILSC]---> CiLsc_SetConnection()--->Connection Type: [%d] ( 1: Phone, 2: Cable return channel, 3: IP, 4: Hostname ) \n", pstContext->eConnectionType);

	HxLOG_Info("[CILSC]---> CiLsc_SetConnection()--->Buffer Size: [%d]\n", pstContext->usBufferSize);
	HxLOG_Info("[CILSC]---> CiLsc_SetConnection()--->Reception Timeout: [%d]\n", pstContext->ucReceptionTimeout);

	if (pstContext->usBufferSize == 0)
	{
		pstContext->usBufferSize = 254;			//SPEC 에 따른 default 값
		CiLsc_BufferInit(pstContext->usBufferSize, pstContext);
		HxLOG_Info("[CILSC]---> CiLsc_SetConnection()---> The buffer size was not initialized. So, the value of the size is 254 as default!\n");
	}

	if (pstContext->ucReceptionTimeout == 0)
	{
		pstContext->ucReceptionTimeout == 10;	//SPEC 에 따른 default 값
		HxLOG_Info("[CILSC]---> CiLsc_SetConnection()---> The reception timeout value has not initialized. So, the value is 10 as default value!\n");
	}

	switch(pstContext->eConnectionType)
	{
	case eCiPlus_Connection_Telephone:
		/* see DVC/SI specification and code it*/
		HxLOG_Error("[CILSC]---> CiLsc_SetConnection()---> Error :  Telephone connection is not supported on this STB!!!\n");
		break;
	case eCiPlus_Connection_CableReturnChannel:
		/* see specification and code it*/
		HxLOG_Error("[CILSC]---> CiLsc_SetConnection()---> Error :  Cable return channel is not supported on this STB!!!\n");
		break;
	case eCiPlus_Connection_IP:
		if(ulApduLen == IP_DESCRIPTOR_LENG + 1)		// Apdu Data에는 connection Type에 대해 1byte를 가지고 있고, 나머지는 IP Descriptor에 대한 내용이므로... +1을 해준다.
		{
			pucData = CiLsc_SetIpConnection(&pucApduData[1], &pstContext->stIpDesc);
		}
		else
		{
			HxLOG_Error("[CILSC]---> CiLsc_SetConnection()---> Error :  Wrong IP Descriptor Length !!!\n");
		}
		break;

	case eCiPlus_Connection_Hostname:
		pucData = CiLsc_SetHostnameConnection(&pucApduData[1], ulApduLen-1, &pstContext->stHostnameDesc);	// 두번째 인자는 Descriptor length를 나타냄.
		break;
	default:
		pucData = NULL;
		break;
	}
	return pucData;
}


int CiLsc_ConnectIpServer (int nSocketNum, const struct sockaddr *pstSocketAddr, int nSocketAddrLength, int nTimeout)
{
 	int				flags = 0;
	int				n = 0;
	int				error = 0;
 	socklen_t 		len;
 	fd_set			rset, wset;
	struct timeval 	tval;

	// set the socket as nonblocking IO
	flags = fcntl (nSocketNum, F_GETFL, 0);
	if(flags < 0)
		return ERR_FAIL;

	if(fcntl(nSocketNum, F_SETFL, flags | O_NONBLOCK) < 0)
		return ERR_FAIL;

	// we connect, but it will return soon
	n = connect(nSocketNum, (struct sockaddr *) pstSocketAddr, nSocketAddrLength);

	if(n < 0) {
		if (errno != EINPROGRESS && errno != EWOULDBLOCK)
		{
		   	return ERR_FAIL;
		}
	}
	else if (n == 0) {
		goto done;
	}

	FD_ZERO(&rset);
	FD_ZERO(&wset);
	FD_SET(nSocketNum, &rset);
	FD_SET(nSocketNum, &wset);

	tval.tv_sec = nTimeout;
	tval.tv_usec = 0;

	// We "select()" until connect() returns its result or timeout
	n = select(nSocketNum + 1, &rset, &wset, 0, nTimeout ? &tval : 0);
	if(n == 0) {
		errno = ETIMEDOUT;
		return ERR_FAIL;
	}

	if (FD_ISSET(nSocketNum, &rset) || FD_ISSET(nSocketNum, &wset)) {
		len = sizeof(error);
		if (getsockopt(nSocketNum, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
			return ERR_FAIL;
		}
	}
	else {
		return ERR_FAIL;
	}

	done:
	// We change the socket options back to blocking IO
	if (fcntl(nSocketNum, F_SETFL, flags) == -1) {
		return ERR_FAIL;
	}
	if (error)
	{
		close(nSocketNum);        /* just in case */
		HxLOG_Error("Socket connection ---> ERROR:: error num[  %d  ]\n", error);
		errno = error;
		return ERR_FAIL;
	}
	return ERR_OK;
}

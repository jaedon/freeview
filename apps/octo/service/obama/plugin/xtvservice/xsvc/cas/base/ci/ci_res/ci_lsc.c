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
#define BUFFER_NUM 16			//CIPlus V1.3 spec�� ���� ������ ������ 16���� ���� ��
#define IP_DESCRIPTOR_LENG 22	//CIPlus V1.3 spec�� ���� IP descriptor�� ũ��� 22byte�� �Ǿ�� ��
#define IP_DESCRIPTOR_TAG 0xcf
#define HOSTNAME_DESCRIPTOR_TAG 0xcd

#if defined(CONFIG_DROP_ROOT_PRIVILEGES)//1
#if 0 //1 �ʿ� ���� ���Ͽ� ����. Test �� �ʿ�� �߰� �۾�
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
	unsigned char					ucReceptionTimeout;			// �� 1�� 10 ms�� ��Ÿ��.

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

	if (s_bSsOpened == TRUE)		// �̹� Ss�� ������ �־ Task�� �ٽ� ���� �� �ʿ� ���� �� ERR_OK �� return�ϸ� ����.
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

	// task ���� �ϴ� �κ� -> task �� �ѹ� ������ �ڽ��� ���������� �ش� task�� �����ϵ��� ����
	// (Connection�� �̷��� ���¿��� CAM�� ���ٰ� �ٽ� ������ Queue full ���� CAM�� �ν� �ȵǴ� ���󶧹��� ���� ��.)
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
	eCommandId = (CI_LSC_CMD)*pucData;			// SPEC ���� CMD APDU�� ù��° ����Ʈ�� command ID
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

		if(pucData != NULL)									//NULL���� ��쿡 SetConnection���� ������ ����� ���� ���� �����.
		{
			pstContext->ucConnectionRetryCount = *pucData;
			pucData++;
			pstContext->ucConnectionTimeout = *pucData;

			if(pstContext->ucConnectionRetryCount == 0)		// retry count�� 0���� ���� �� ��� connection �õ����� ���� �����Ƿ� �ѹ��� �ϵ��� �� ����.
				pstContext->ucConnectionRetryCount = 1;		// ���� spec�̳� ������ ���� �ȴٸ� ���� �ʿ�.

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

	case eCiPlus_CommsCmd_SetParams:		// Comms Resource�� ���� �Ǳ� ���� SetParam�� �޾Ƽ� �ش� ���ῡ ���� ���� ������ �Ϸ� �Ǿ�� ��. ��, param�� ���� �Ǿ�� connection�� ���� �Ѱ����� �ľ� ��
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

	if ((pstContext->ucSendBufferRear + 1) % BUFFER_NUM == ucPhaseId)	// ����ִ� ť ���ۿ� ���������� �����Ͱ� ä�����µ�, ucPhaseID�� ���ѿ��� ���� �� ó�� CAM�ʿ��� ���� ��Ʈ�� ����ؼ� �˾Ƽ� �� �ѱ� �������� Ȯ���ϴ� ����
	{
		pstContext->ucSendBufferRear = ucPhaseId;
	}
	else		// error:: �����ؾ� �ϴ� ��ġ�� �߸���. �Ƹ� CAM���� ����� ���� �Ǿ��ٸ� �̺κ��� Ÿ�� ���� �� ��!
	{
		HxLOG_Error("[CILSC]---> CiLsc_ProcSendLast()---> Error : Wrong buffer rear value or wrong phase id !!!\n");
		return -1;
	}

	if (pstContext->usBufferSize != ulApduLen - 1)
	{
		HxLOG_Error("[CILSC]---> CiLsc_ProcSendLast()---> Error : the value of sending data length using SEND_MORE APDU should be same with buffer size!!\n");
		return -1;
	}

	// CICAM���� Buffer Overflow�� ����ؼ� Comms_Send APDU�� ������ �ϰ����� ����� �ڵ�� Buffer Overflow�� ��� �ϵ��� �ؾ� �ҵ�.

	HxSTD_memcpy(pstContext->paucSendBuffer[pstContext->ucSendBufferRear], pucData, ulApduLen-1);
	pstContext->ucSendBufferCount++;		//more�� ������ ������ ���� �ľ��ϱ� ����... �׻� sendBuffer ���� �� ������ �� ���� 0���� �ʱ�ȭ ���Ѿ� ��.

	if (pstContext->eConnectionType == eCiPlus_Connection_IP)
	{
		if(pstContext->stIpDesc.eProtocolType == eCiPlus_Protocol_TCP)
		{
			HxLOG_Info("[CILSC]---> CiLsc_ProcSendMore()---> Sending buffer using TCP connection.\n");

			CiLsc_SendBuffer(usSsNo, pstContext, ulApduLen-1);
			pstContext->ucSendBufferCount = 0;
			CiLsc_CommsReply(usSsNo, eCommsReply, pstContext);
		}
		else if(pstContext->stIpDesc.eProtocolType == eCiPlus_Protocol_UDP)		// �������� ����. UDP �� Test �ʿ���.
		{
			// UDP�� buffer�� ��� data�� �ѹ��� �׾� �� ������ �������� �Ϸ��� �ǵ��� �־�����, ���� �ϳ��� �ٷ� ������ ���� ����.
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
		/* ��ȭ ���ῡ ���� ó�� */
	}
	else if (pstContext->eConnectionType == eCiPlus_Connection_CableReturnChannel)
	{
		/* Cable Return Channel ���ῡ ���� ó�� */
	}
	else if (pstContext->eConnectionType == eCiPlus_Connection_Hostname)
	{
		/* Host name �� ���� ó�� */
		if(pstContext->stHostnameDesc.eProtocolType == eCiPlus_Protocol_TCP)
		{
			HxLOG_Info("Sending buffer using TCP connection.\n");

			CiLsc_SendBuffer(usSsNo, pstContext, ulApduLen-1);
			pstContext->ucSendBufferCount = 0;
			CiLsc_CommsReply(usSsNo, eCommsReply, pstContext);
		}
		else if(pstContext->stHostnameDesc.eProtocolType == eCiPlus_Protocol_UDP)		// �������� ����. UDP �� Test �ʿ���.
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

	if ((pstContext->ucSendBufferRear + 1) % BUFFER_NUM == ucPhaseId)	// ����ִ� ť ���ۿ� ���������� �����Ͱ� ä�����µ�, ucPhaseID�� ���ѿ��� ���� �� ó�� CAM�ʿ��� ���� ��Ʈ�� ����ؼ� �˾Ƽ� �� �ѱ� �������� Ȯ���ϴ� ����
	{
		pstContext->ucSendBufferRear = ucPhaseId;
	}
	else		// error:: �����ؾ� �ϴ� ��ġ�� �߸���. �Ƹ� CAM���� ����� ���� �Ǿ��ٸ� �̺κ��� Ÿ�� ���� �� ��!
	{
		HxLOG_Error("[CILSC]---> CiLsc_ProcSendLast()---> Error : Wrong buffer rear value or wrong phase id !!!\n");
		return -1;
	}

	// CICAM���� Buffer Overflow�� ����ؼ� Comms_Send APDU�� ������ �ϰ����� ����� �ڵ�� Buffer Overflow�� ��� �ϵ��� �ؾ� �ҵ�.

	HxSTD_memcpy(pstContext->paucSendBuffer[pstContext->ucSendBufferRear], pucData, ulApduLen-1);
	pstContext->ucSendBufferCount++;		//more�� ������ ������ ���� �ľ��ϱ� ����... �׻� sendBuffer ���� �� ������ �� ���� 0���� �ʱ�ȭ ���Ѿ� ��.

	if (pstContext->eConnectionType == eCiPlus_Connection_IP)
	{
		HxLOG_Info("[CILSC]---> CiLsc_ProcSendLast()---> Sending buffer using IP connection.\n");
		CiLsc_SendBuffer(usSsNo, pstContext, ulApduLen-1);
		pstContext->ucSendBufferCount = 0;
		CiLsc_CommsReply(usSsNo, eCommsReply, pstContext);
	}
	else if (pstContext->eConnectionType == eCiPlus_Connection_Telephone)
	{
		/* ��ȭ ���ῡ ���� ó�� */
	}
	else if (pstContext->eConnectionType == eCiPlus_Connection_CableReturnChannel)
	{
		/* Cable Return Channel ���ῡ ���� ó�� */
	}
	else if (pstContext->eConnectionType == eCiPlus_Connection_Hostname)
	{
		/* Host name �� ���� ó�� */
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
		if (pstContext->usBufferSize == 0)				//�ϴ� BufferSize�� 0���� ���� �Ǿ� �ִٴ� ���� CommsReply�� ������ �� Buffer setting�� �ϴ� �κп��� �䱸�ϴ� ������ ��ŭ�� ���۸� �Ҵ� �������� ���� ���� ��Ÿ ��. (���� ���� �ƴϰ�, ������ ���Ƿ�...)
		{
			aucMsg[5] = 0xfe;
			if (CIRH_SendApdu(usSsNo, aucMsg, 6) != ERR_OK)
			{
				HxLOG_Error("[CILSC]---> Fail to Send APDU... SsNo[%d] !!!\n", usSsNo);
				return ERR_FAIL;
			}
			return 0;
		}
		if (pstContext->ucReceptionTimeout == 0)			//1)reception timeout ���� spec�� 0�� �� �� ����. 2)network connection�� �̷����� ���� set params�� �޾Ƽ� �̹� ������ �Ϸ� �Ǿ� �־�� ��.(|| pstContext->bConnectionStatus == TRUE)
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
			aucMsg[5] = 1;		//1�� CIPlus Spec V1.3�� ���� Connected ���� ��Ÿ ��
		}
		else
		{
			aucMsg[5] = 0;		//0�� CIPlus Spec V1.3�� ���� Disconnected ���� ��Ÿ ��
		}
		if (CIRH_SendApdu(usSsNo, aucMsg, 6) != ERR_OK)
		{
			HxLOG_Error("[CILSC]---> Fail to Send APDU... SsNo[%d] !!!\n", usSsNo);
			return ERR_FAIL;
		}
		break;
	case eCiPlus_CommsReply_SendAck:
		aucMsg[4] = (unsigned char)eCommsReply;
		aucMsg[5] = pstContext->ucSendBufferFront;		// ���߿� command�� �׽�Ʈ �Ҷ� Ȯ�� �غ����� �� ��!

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

	/* ������ ������ �����ּ� ����ü �ۼ� */
	bzero((char *)&stServerAddr, sizeof(stServerAddr));
	// TODO: gethostbyname() ����� ���� IPv6�� ��� �̿� �µ��� setting / ����� IPv4�� ó�� ���� ��.
	stServerAddr.sin_family = pstHostEnt->h_addrtype;
	stServerAddr.sin_addr.s_addr = stInAddr.s_addr;
	HxLOG_Info("[CILSC]---> CiLsc_EstablishHostnameConnection()---> server Ip Addr: %x \n", stInAddr.s_addr);
	stServerAddr.sin_port = pstContext->stHostnameDesc.usDestinationPort;

	/* �����û */
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

	/* ������ ������ �����ּ� ����ü �ۼ� */
	server_addr.sin_addr.s_addr = ulServerIpAddr;
	HxLOG_Info("[CILSC]---> CiLsc_EstablishIpConnection()---> server Ip Addr: %x \n", ulServerIpAddr);
	server_addr.sin_port = pstContext->stIpDesc.usDestinationPort;

	/* �����û */
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

	/*socket ��� �̿��ؼ� ���� �ؾ� �� */
	close(pstContext->nSocketNum);
	pstContext->nSocketNum = 0;
	/*��� ���� �� ���ۿ� �Ҵ� �Ǿ��ִ� ������ �鵵 ��� ��ȯ �ϵ��� �� ��! */
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

	/*socket ��� �̿��ؼ� ���� �ؾ� �� */
	close(pstContext->nSocketNum);
	pstContext->nSocketNum = 0;
	/*��� ���� �� ���ۿ� �Ҵ� �Ǿ��ִ� ������ �鵵 ��� ��ȯ �ϵ��� �� ��! */
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
		// ��� ���ҽ����� ������ �Ǿ� ���� �ʴٸ� send ��ü�� �� �� ����.
		HxLOG_Error("[CILSC]---> CiLsc_RcvBuffer()---> Error : Communication resource is not connected. \n");
		return ERR_FAIL;
	}

	if (ucPhaseId < 0 || ucPhaseId > 15)
	{
		HxLOG_Error("[CILSC]---> CiLsc_RcvBuffer()---> Error : Wrong Phase ID. \n");
		return ERR_FAIL;
	}


	/* �Ʒ��� buffer size �� reception timeout �� ���� ������� �ڵ�� CAM�ʿ��� ����� ���� �ȵǴ� ��(0)�� �޾����� ���� �� �� �ֵ��� �߰� �Ͽ���. */
	if (pstContext->usBufferSize == 0)
	{
		pstContext->usBufferSize = 254;				//SPEC �� ���� default ��
		CiLsc_BufferInit(pstContext->usBufferSize, pstContext);
		HxLOG_Info("[CILSC]---> CiLsc_RcvBuffer()---> The buffer size was not initialized. So, the value of the size is 254 as default!\n");
	}

	if(pstContext->ucReceptionTimeout == 0 )
	{
		pstContext->ucReceptionTimeout = 10;		// Spec������ �ش� ���� setting ���� �ʾ��� ��, default: 10(100ms)���� ���� ��.
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

		if (nStatus == 0)		// Timeout�� �ɷ��� ��
		{
			if(pstContext->sRcvMsgCount - 1 > 0)		// ���� Msg�� �Ҹ� �ص� ������ �� ó���ؾ� �� msg�� ���� ��.
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
				// TODO: UDP �� ���� ó�������� ��Ȯ������ �ٽ� Ȯ���غ��� �ڵ� ���� �ʿ�!!
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

				// TODO: �Ʒ� �κ� ���� �ʿ�.
				// Select()���� nStatus �� 1�� ���� ������ �ִٰ� �����µ�, length�� 0�̶�� ���� connection�� �������� �ǹ�(?) �׷��ٸ� �Ʒ� ������ ����.
				// connection�� ������ ��� CAM �ʿ� Reply ����.
				CiLsc_TerminateConnection(usSsNo, pstContext);
				CiLsc_CommsReply(usSsNo, eCiPlus_CommsReply_StatusReplyAck, pstContext);

				break;
			}
		}
		else
		{
			HxLOG_Info("CHECK!! \n");		// ���� �� ������ ��µǸ� �� ���Դ��� Ȯ�� �غ���!
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
		// ��� ���ҽ����� ������ �Ǿ� ���� �ʴٸ� send ��ü�� �� �� ����.
		HxLOG_Error("[CILSC]---> CiLsc_SendBuffer()---> Error : Communication resource is not connected. \n");
		return ERR_FAIL;
	}

	if (ucFront > ucRear)			//���� ť�̱� ������ ť ���� �Ǵ� �κ��� ť ���� �Ǵ� �κк��� ���� ���� ���� �� �ֱ� ������ �̿� ���� ó��.
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

	for( i = 0; i < ucBufferCount-1; i++)	// ���⿡�� i < ucBufferCount���� �����µ�, ��� �� ���� nCheckBufferLength �� ���� ���ƾ� �� ������ �����Ǹ�, nCheckBufferLength ��ŭ �����ؾ� �� �� ���⵵ ��.
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
	/* ��������� ���ؼ� �����͸� �����ϵ��� �Ѵ�. */
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

	usApduDataLength = usDataLength + 1;		//APDU�� ���� Data�� comms_phase_id(1byte) ���� ��� �� ���� APDU�� DATA �����̹Ƿ� �� ���̴� data length +1�� ��.

	pucApduData = (unsigned char *) OxCAS_Malloc(8 + usApduDataLength);

	//	TODO: �ٷ� �Ʒ��ٷ� ó���ϸ� �޾ƿ��� data�� ���̻��� ������ buffer size �� ���� ���� ��쿣 LAST�� ������ �� ���� MORE�� ���� �� ������ Ȯ�� �غ���!!
	if(usDataLength == usBufferSize)			// �޾ƿ� data�� ���̰� buffer size�� ���ٸ� �ش� ������ CAM�ʿ� More APDU�� ������ �Ѵ�.
	{
		HxLOG_Info("Make CI_COMMS_RCV_MORE APDU.\n");

		pucApduData[0] = (unsigned char)((CI_COMMS_RCV_MORE >> 16) & 0xff);
		pucApduData[1] = (unsigned char)((CI_COMMS_RCV_MORE >> 8) & 0xff);
		pucApduData[2] = (unsigned char)(CI_COMMS_RCV_MORE & 0xff);

		CIRH_ConvertApduLengthField(usApduDataLength, &pucApduData[3], &ucLenFieldSize);

		ulOffset = 3 + ucLenFieldSize;		// TAG ���� 3 + Length Field size

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

		ulOffset = 3 + ucLenFieldSize;		// TAG ���� 3 + Length Field size

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
		if(*pucData != IP_DESCRIPTOR_LENG - 2)			//���� IP Descriptor�� ���̿��� tag�� leng �� ����ִ� 2byte�� ������ ���� �� �־�� �ùٸ� ������ ��.
		{
			HxLOG_Error("[CILSC]---> CiLsc_SetIpConnection()---> Error :  Wrong IP descriptor !!!\n");
			return NULL;
		}
		pucData++;
		pstIpDesc->eIpProtocolVer = (CI_LSC_IP_VERSION)*pucData;
		pucData++;

#if 0
		HxLOG_Info("[CILSC]---> CiLsc_SetIpConnection()---> IP address(Raw Data):: [");
		for( i=0; i<16; i++)	// Spec���� IP address�� �� 16����Ʈ�� ���ļ� ������
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
	int				nHostnameLen = ulApduLen - 5;	// Hostname ���̴� ��ü Desc ���̿��� 5byte(tag(1byte), length(1byte), protocol type(1byte), port#(2byte))�� ������ ��.

	pucData = pucApduData;

	HxLOG_Info("[CILSC]---> CiLsc_SetHostnameConnection()---> Host name Descriptor tag: %02x \n", *pucData);
	if(*pucData == HOSTNAME_DESCRIPTOR_TAG)
	{
		pucData++;
		if(*pucData != ulApduLen - 2)			//Hostname Descriptor�� ��ü ���̿��� tag�� leng �� ����ִ� 2byte�� ������ ���� �� �־�� �ùٸ� ������ ��.
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
		pstContext->usBufferSize = 254;			//SPEC �� ���� default ��
		CiLsc_BufferInit(pstContext->usBufferSize, pstContext);
		HxLOG_Info("[CILSC]---> CiLsc_SetConnection()---> The buffer size was not initialized. So, the value of the size is 254 as default!\n");
	}

	if (pstContext->ucReceptionTimeout == 0)
	{
		pstContext->ucReceptionTimeout == 10;	//SPEC �� ���� default ��
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
		if(ulApduLen == IP_DESCRIPTOR_LENG + 1)		// Apdu Data���� connection Type�� ���� 1byte�� ������ �ְ�, �������� IP Descriptor�� ���� �����̹Ƿ�... +1�� ���ش�.
		{
			pucData = CiLsc_SetIpConnection(&pucApduData[1], &pstContext->stIpDesc);
		}
		else
		{
			HxLOG_Error("[CILSC]---> CiLsc_SetConnection()---> Error :  Wrong IP Descriptor Length !!!\n");
		}
		break;

	case eCiPlus_Connection_Hostname:
		pucData = CiLsc_SetHostnameConnection(&pucApduData[1], ulApduLen-1, &pstContext->stHostnameDesc);	// �ι�° ���ڴ� Descriptor length�� ��Ÿ��.
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

/*
** di_ftp.c
**

**
** @defgroup
** @author
** @note
** @brief
** @file
*/


/*----------------------------------------------------------------------------------
** Headers
**--------------------------------------------------------------------------------*/
#include "vkernel.h"
#include "linden_trace.h"

#include "di_err.h"
#include "di_ftp.h"
#include "di_nvram.h"
#include "drv_nvram.h"


/*----------------------------------------------------------------------------------
** defines
**--------------------------------------------------------------------------------*/
#if defined (CONFIG_DEBUG)
#define PrintInfo		VK_printf
#define PrintDebug		VK_DBG_Print
#define PrintError		VK_printf
#else
#define PrintInfo(...)
#define PrintDebug(...)
#define PrintError(...)
#endif

#define FTP_RECV_TIMEOUT				60000 /* 60 sec */

#define SIZE_NET_PATH					0x100
#define SIZE_SEND_STR_MAX				0x100

#define ENET_MAX_PKT					1514
#define ENET_DIX_HEADER					14

	/* FTP Commands */
#define FTP_PORT_NUM					21
#define MAX_PACKET_SIZE 				(ENET_MAX_PKT-ENET_DIX_HEADER)
#define HOST_READY_NEW_USER 			220
#define HOST_READY_USER_PASSWD 			331
#define HOST_USER_LOGIN_SUCCESS 		230
#define HOST_CHANGE_DIR_SUCCESS 		250
#define HOST_USER_CMD_OK				200
#define HOST_ENTER_PASV_MODE 			227
#define HOST_SEND_FILE_STAUS 			213
#define HOST_FILE_STATUS_OK 			150 //about to open data connection.
#define HOST_CLOSE_DATA_CONNECTION 		226
#define HOST_SEND_FILE_STATUS 			213
#define HOST_SEND_LOGIN_FAILED 			530

#define CFE_ERR_HOSTUNKNOWN				(-20)
#define CFE_ERR_NOHANDLES				(-25)

#define TCPSTATUS_NOTCONN		0
#define TCPSTATUS_CONNECTING	1
#define TCPSTATUS_CONNECTED		2


/*----------------------------------------------------------------------------------
** typedef
**--------------------------------------------------------------------------------*/
typedef struct _mode_strct_
{
	HBOOL bLoopEnd;
	HBOOL bSetModeOK;
} SET_MODE_INFO, *pSET_MODE_INFO;

typedef enum tcp_polling
{
	POLLING_FOR_CONNECT = 0,   /* 0 */
	POLLING_FOR_CMD,
	POLLING_FOR_DATA
} TCP_POLL_CMD;


/*----------------------------------------------------------------------------------
** Variables
**--------------------------------------------------------------------------------*/
static HBOOL s_bTimeout = FALSE;
static FTP_INFO s_stFtpTransInfo;

/* FTP string */
static HCHAR user[] 	= "USER";
static HCHAR passwd[] 	= "PASS";
static HCHAR type[] 	= "TYPE I";
static HCHAR pasv[] 	= "PASV";
static HCHAR download[]= "RETR";
static HCHAR size[]	= "SIZE";
static HCHAR cwd[]		= "CWD";
static HCHAR quit[]	= "QUIT";


/*----------------------------------------------------------------------------------
** Function def.
**--------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------
** Functions
**--------------------------------------------------------------------------------*/
// 파일이름이 없는 경우도 추가 해야함.
// 일정 시간도안 원하는 패킷이 없을 경우QUIT 날릴것.
// Malloc Memory Free 할 것.
// Error Return 시 Socket Close 할 것.
static void P_FTP_Recieve_TimeoutCallback(HULONG ulTimerId, void* arg)
{
	(void)ulTimerId;
	(void)arg;

	s_bTimeout = TRUE;
}

static DRV_Error P_FTP_Polling_RecvPort(HINT32 nSocket, HINT32 *pnStatus, HINT32 *pnReady, HINT32 nWaitMode)
{
	HINT32 nConnflag=0;
	HINT32 nRxReady=0;
	HULONG ulTimerId;

	s_bTimeout = FALSE;
	VK_TIMER_EventAfter(FTP_RECV_TIMEOUT, P_FTP_Recieve_TimeoutCallback, (void *)NULL, (HINT32)NULL, (HULONG *)&ulTimerId);

	/* Wait 30 sec  */
	while(!s_bTimeout)
	{
//		VK_SYSTEM_Poll();
		VK_TASK_Sleep(10);
		VK_TCP_status(nSocket, &nConnflag, &nRxReady, NULL);

		if(nWaitMode == POLLING_FOR_CONNECT)
		{
			if (nConnflag == TCPSTATUS_CONNECTED)
			{
				*pnStatus = nConnflag;
				VK_TIMER_Cancel(ulTimerId);
				return DRV_OK;
			}
		}
		else if(nWaitMode == POLLING_FOR_CMD)
		{
			if (nConnflag != TCPSTATUS_CONNECTED)
			{
				*pnReady = NULL;
				*pnStatus = nConnflag;
				VK_TIMER_Cancel(ulTimerId);
				PrintError("[P_FTP_Polling_RecvPort] Remote host is no longer connected. \n");
				return DRV_ERR;

			}

			if (nRxReady > 0)
			{
				*pnReady = nRxReady;
				*pnStatus = nConnflag;
				VK_TIMER_Cancel(ulTimerId);
				return DRV_OK;
			}
		}
		else
		{
			*pnStatus = nConnflag;
			PrintError("[P_FTP_Polling_RecvPort] Unknown polling cmd. \n");
			VK_TIMER_Cancel(ulTimerId);
			return DRV_OK;
		}
	}

	/* Timeout */
	PrintError("[P_FTP_Polling_RecvPort] Timeout nWaitMode=%d !!! \n", nWaitMode);
	*pnStatus = TCPSTATUS_NOTCONN;

	return DRV_ERR;
}

static DRV_Error P_FTP_Connect_HostPort(HINT32 *pnReturnSocket, HUINT8 *ucHostaddr, HUINT16 usHostPort)
{
	HINT32 nRet=0;
	HINT32 nConnflag=0;
	HINT32 nCmdSocket=0;

	if(ucHostaddr == NULL)
	{
		PrintError("[P_FTP_Connect_HostPort] Param is NULL !!! \n");
		return DRV_ERR;
	}

	nCmdSocket = VK_TCP_socket();
	if (nCmdSocket< 0)
	{
		PrintError("[P_FTP_Connect_HostPort] Could not create TCP socket \n");
		return DRV_ERR;
	}
	else
	{
		*pnReturnSocket = nCmdSocket;
	}

	nRet = VK_TCP_connect(nCmdSocket, ucHostaddr, usHostPort);
	if (nRet < 0)
	{
		PrintError("[P_FTP_Connect_HostPort] Could not connect to host %I \n", ucHostaddr);
		VK_TCP_close(nCmdSocket);
		return DRV_ERR;
	}

	P_FTP_Polling_RecvPort(nCmdSocket, &nConnflag, NULL, POLLING_FOR_CONNECT);

	if (nConnflag != TCPSTATUS_CONNECTED)
	{
		PrintError("[P_FTP_Connect_HostPort] Could not connect to remote host \n");
		VK_TCP_close(nCmdSocket);
		return DRV_ERR;
	}

	return DRV_OK;
}

static DRV_Error P_FTP_Sent_Cmd(HCHAR *pCmdStr, HCHAR *pDataStr)
{
	HCHAR pSendStr[SIZE_SEND_STR_MAX];
	HUINT32 ulStrSize = 0;

	if(pCmdStr == NULL)
	{
		PrintError("[P_FTP_Sent_Cmd] pCmdStr is NULL !!! \n");
		return DRV_ERR;
	}

	ulStrSize = VK_strlen(pCmdStr);
	if(pDataStr != NULL)
	{
		ulStrSize += VK_strlen(pDataStr);
	}

	if((SIZE_SEND_STR_MAX-4) <=ulStrSize)
	{
		PrintError("[P_FTP_Sent_Cmd] Sending string is bigger than buffer !!! \n");
		return DRV_OK;
	}

	if(pDataStr == NULL)
	{
		VK_sprintf(pSendStr, "%s\r\n", pCmdStr);
		VK_TCP_send(s_stFtpTransInfo.ulCmdSocket, (HUINT8 *)pSendStr, VK_strlen(pSendStr));
	}
	else
	{
		VK_sprintf(pSendStr, "%s %s\r\n", pCmdStr, pDataStr);
		VK_TCP_send(s_stFtpTransInfo.ulCmdSocket, (HUINT8 *)pSendStr, VK_strlen(pSendStr));
	}

	return DRV_OK;
}

static HINT32 P_FTP_Recieve_Cmd(HUINT8 *pRxDataBuff, HUINT32 uiSize)
{
	HINT32 nRxDataSize = 0;

	if(s_stFtpTransInfo.ulCmdSocket == CFE_ERR_NOHANDLES)
	{
		PrintError("[P_FTP_Recieve_Cmd] CMD Socket isn't created !!! \n");
		return -1;
	}

	nRxDataSize = VK_TCP_recv(s_stFtpTransInfo.ulCmdSocket, pRxDataBuff, uiSize);

	return nRxDataSize;
}

static HINT32 P_FTP_Recieve_Data(HUINT8 *pRxDataBuff, HUINT32 uiSize)
{
	HINT32 nRxDataSize = 0;

	if(s_stFtpTransInfo.ulDataSocket == CFE_ERR_NOHANDLES)
	{
		PrintError("[P_FTP_Recieve_Data] Data Socket isn't created !!! \n");
		return -1;
	}

	nRxDataSize = VK_TCP_recv(s_stFtpTransInfo.ulDataSocket, pRxDataBuff, uiSize);

	return nRxDataSize;
}

static DRV_Error P_FTP_Get_ReplyCode(HUINT8 *aRxDataBuff, HINT32 *iReplyCode)
{
	HUINT8 aucReplyCodeString[4];

	if(aRxDataBuff == NULL || iReplyCode == NULL)
	{
		PrintError("[P_FTP_Get_ReplyCode] Param is NULL !!! \n");
		return DRV_ERR;
	}

	VK_strncpy((HCHAR *)aucReplyCodeString, (HCHAR *)aRxDataBuff, 3);
	aucReplyCodeString[3] = NULL;
	*iReplyCode = VK_atoi((HCHAR *)aucReplyCodeString);

	return DRV_OK;
}

static DRV_Error P_FTP_Parse_FtpOtaInfo(DI_FTP_OTA_SETTING_T *pOtaInfo)
{
	if(pOtaInfo ==NULL)
	{
		PrintError("[P_FTP_Parse_FtpOtaInfo] pOtaInfo is NULL !!! \n");
		return DRV_ERR;
	}

	/* Get IP address */
	s_stFtpTransInfo.usPortNum	= FTP_PORT_NUM;
	VK_memcpy(s_stFtpTransInfo.aServerIp, pOtaInfo->aServerIp, NET_ADDR_LENGTH);
	VK_strncpy((HCHAR *)s_stFtpTransInfo.aUserName, (HCHAR *)pOtaInfo->aUserName, FTP_USER_LENGTH);
	VK_strncpy((HCHAR *)s_stFtpTransInfo.aPasswd, (HCHAR *)pOtaInfo->aPasswd, FTP_PASSWD_LENGTH);
	VK_strncpy((HCHAR *)s_stFtpTransInfo.aDirPath, (HCHAR *)pOtaInfo->aDirPath, FTP_DIR_PATH_LENGTH);
	VK_strncpy((HCHAR *)s_stFtpTransInfo.aFileName, (HCHAR *)pOtaInfo->aFileName, FTP_FILE_NAME_LENGTH);

	PrintDebug("[P_FTP_Parse_FtpOtaInfo] IP : %d.%d.%d.%d\n", s_stFtpTransInfo.aServerIp[0], s_stFtpTransInfo.aServerIp[1], s_stFtpTransInfo.aServerIp[2], s_stFtpTransInfo.aServerIp[3]);
	PrintDebug("[P_FTP_Parse_FtpOtaInfo] Port Number : %d \n", s_stFtpTransInfo.usPortNum);
	PrintDebug("[P_FTP_Parse_FtpOtaInfo] User Name : %s \n", s_stFtpTransInfo.aUserName);
	PrintDebug("[P_FTP_Parse_FtpOtaInfo] Passwd : %s \n", s_stFtpTransInfo.aPasswd);
	PrintDebug("[P_FTP_Parse_FtpOtaInfo] DIR : %s \n", s_stFtpTransInfo.aDirPath);
	PrintDebug("[P_FTP_Parse_FtpOtaInfo] File Name : %s \n", s_stFtpTransInfo.aFileName);

	return DRV_OK;
}

static DRV_Error P_FTP_Parse_ReplyCode(HINT32 nSocket, HINT32 iReplyCode, HUINT8 *ucRcvBuffer, pSET_MODE_INFO SetModeInfo)
{
	HCHAR *pStartOffset;
	HCHAR *pEndOffset;
	HCHAR *pStrBuf;
	HINT32 iToken=0;
	unsigned long ulNewPort;

	HAPPY(nSocket);

	switch(iReplyCode)
	{
		case HOST_READY_NEW_USER:
			P_FTP_Sent_Cmd(user, (HCHAR *)s_stFtpTransInfo.aUserName);
			PrintDebug("[P_FTP_Parse_ReplyCode] SEND_NEW_USER \n");
			break;

		case HOST_READY_USER_PASSWD:
			P_FTP_Sent_Cmd(passwd, (HCHAR *)s_stFtpTransInfo.aPasswd);
			PrintDebug("[P_FTP_Parse_ReplyCode] SEND_USER_PASSWD \n");
			break;

		case HOST_USER_LOGIN_SUCCESS:
			P_FTP_Sent_Cmd(cwd, (HCHAR *)s_stFtpTransInfo.aDirPath);
			PrintDebug("[P_FTP_Parse_ReplyCode] SEND_CHANGE_DIR \n");
			break;

		case HOST_SEND_LOGIN_FAILED:
			SetModeInfo->bLoopEnd = TRUE;
			SetModeInfo->bSetModeOK =FALSE;
			PrintError("[P_FTP_Parse_ReplyCode] Login Failed !\n");
			break;

		case HOST_CHANGE_DIR_SUCCESS:
			P_FTP_Sent_Cmd(type, (HCHAR *)NULL);
			PrintDebug("[P_FTP_Parse_ReplyCode] SEND_CHANGE_TYPE I \n");
			break;

		case HOST_USER_CMD_OK:
			if(s_stFtpTransInfo.ulFileSize == 0)
			{
				P_FTP_Sent_Cmd(size, (HCHAR *)s_stFtpTransInfo.aFileName);
				PrintDebug("[P_FTP_Parse_ReplyCode] SEND_REQUEST_FILE_SIZE \n");
			}
			else
			{
				P_FTP_Sent_Cmd(pasv, (HCHAR *)NULL);
				PrintDebug("[P_FTP_Parse_ReplyCode] SEND_REQUEST_PASV_MODE !!! \n");
			}
			break;

		case HOST_SEND_FILE_STATUS:
			s_stFtpTransInfo.ulFileSize = VK_atoi((HCHAR *)&ucRcvBuffer[4]);
			PrintDebug("[P_FTP_Parse_ReplyCode] FILE Size : %d \n", s_stFtpTransInfo.ulFileSize);

			P_FTP_Sent_Cmd(type, (HCHAR *)NULL);
			PrintDebug("[P_FTP_Parse_ReplyCode] SEND_CHANGE_TYPE I \n");
			break;

		case HOST_ENTER_PASV_MODE:
			s_stFtpTransInfo.usPasvPortNum = 0;
			pStartOffset = VK_strchr((HCHAR *)ucRcvBuffer, '(');
			if(pStartOffset == NULL)
			{
				PrintDebug("[P_FTP_Parse_ReplyCode] HOST_ENTER_PASV_MODE pStartOffse tinfo is NULL \n");
				return DRV_ERR;
			}
			pStartOffset++;
			while(iToken < 6)
			{
				if(iToken == 5)
				{
					pEndOffset = VK_strchr(pStartOffset, ')');
					if(pEndOffset == NULL)
					{
						PrintDebug("[P_FTP_Parse_ReplyCode] HOST_ENTER_PASV_MODE pEndOffset info is NULL \n");
						return DRV_ERR;
					}
				}
				else
				{
					pEndOffset = VK_strchr(pStartOffset, ',');
					if(pEndOffset == NULL)
					{
						PrintDebug("[P_FTP_Parse_ReplyCode] HOST_ENTER_PASV_MODE pEndOffset info is NULL \n");
						return DRV_ERR;
					}
				}

				if(iToken == 4)
				{
					pStrBuf = VK_MEM_Alloc(pEndOffset-pStartOffset+1);
					VK_strncpy(pStrBuf, pStartOffset, pEndOffset-pStartOffset);
					pStrBuf[pEndOffset-pStartOffset] = NULL;
					ulNewPort= VK_atoi(pStrBuf);
					s_stFtpTransInfo.usPasvPortNum = (256*ulNewPort);
					VK_MEM_Free(pStrBuf);
				}
				else if(iToken == 5)
				{
					pStrBuf = VK_MEM_Alloc(pEndOffset-pStartOffset+1);
					VK_strncpy(pStrBuf, pStartOffset, pEndOffset-pStartOffset);
					pStrBuf[pEndOffset-pStartOffset] = NULL;
					ulNewPort = VK_atoi(pStrBuf);
					s_stFtpTransInfo.usPasvPortNum += ulNewPort;
					VK_MEM_Free(pStrBuf);
				}
				else
				{
				}

				iToken++;
				pStartOffset = pEndOffset+1;
			}

			SetModeInfo->bLoopEnd = TRUE;
			SetModeInfo->bSetModeOK = TRUE;
			PrintDebug("[P_FTP_Parse_ReplyCode] PASV Port Num %d \n", s_stFtpTransInfo.usPasvPortNum);
			break;

		default:
			PrintDebug("[P_FTP_Parse_ReplyCode] Received not wanted MSG !!\n");
			break;
	}

	return DRV_OK;
}


HINT32 P_FTP_ParseIpAddr(const HCHAR *ipaddr, HUINT8 *dest)
{
	HINT32 a, b, c, d;
	HCHAR *x;

	/* make sure it's all digits and dots. */
	x = (HCHAR *)ipaddr;
	while (*x) {
		if ((*x == '.') || ((*x >= '0') && (*x <= '9'))) {
			x++;
			continue;
		}
		return -1;
	}

	x = (HCHAR *)ipaddr;
	a = VK_atoi(ipaddr);
	x = VK_strchr(x, '.');
	if (!x)
		return -1;
	b = VK_atoi(x + 1);
	x = VK_strchr(x + 1, '.');
	if (!x)
		return -1;
	c = VK_atoi(x + 1);
	x = VK_strchr(x + 1, '.');
	if (!x)
		return -1;
	d = VK_atoi(x + 1);

	if ((a < 0) || (a > 255))
		return -1;
	if ((b < 0) || (b > 255))
		return -1;
	if ((c < 0) || (c > 255))
		return -1;
	if ((d < 0) || (d > 255))
		return -1;

	dest[0] = (HUINT8) a;
	dest[1] = (HUINT8) b;
	dest[2] = (HUINT8) c;
	dest[3] = (HUINT8) d;

	return 0;
}


#define _______________________________________________________________________
#define ____GLOBAL_FUNCTION_BODY____
DI_ERR_CODE DI_FTP_init(void)
{
	s_stFtpTransInfo.ulCmdSocket	= CFE_ERR_NOHANDLES;
	s_stFtpTransInfo.ulDataSocket	= CFE_ERR_NOHANDLES;
	s_stFtpTransInfo.usPortNum		=CFE_ERR_HOSTUNKNOWN;
	s_stFtpTransInfo.usPasvPortNum 	= CFE_ERR_HOSTUNKNOWN;
	s_stFtpTransInfo.ulFileSize		= 0;

	VK_MEM_Memset(s_stFtpTransInfo.aServerIp, 0x0, NET_ADDR_LENGTH);
	VK_MEM_Memset(s_stFtpTransInfo.aUserName, 0x0, FTP_USER_LENGTH);
	VK_MEM_Memset(s_stFtpTransInfo.aPasswd, 0x0, FTP_PASSWD_LENGTH);
	VK_MEM_Memset(s_stFtpTransInfo.aDirPath, 0x0, FTP_DIR_PATH_LENGTH);
	VK_MEM_Memset(s_stFtpTransInfo.aFileName, 0x0, FTP_FILE_NAME_LENGTH);

	return DI_ERR_OK;
}

DI_ERR_CODE DI_FTP_open(DI_FTP_OTA_SETTING_T *pOtaInfo)
{
	DRV_Error nDrvErr = DRV_OK;
	HINT32 nConnflag=0;
	HINT32 nCmdSocket=0;
	HINT32 ulDataSocket=0;
	HINT32 nRxReady=0;
	HINT32 nRxDataSize=0;
	HUINT8 *pRxDataBuff;
	HINT32 iReplyCodeDec=0;
	SET_MODE_INFO SetModeInfo;

	/* Target Server : FileZilla FTP Server */

	/* Parse OTA Info */
	nDrvErr = P_FTP_Parse_FtpOtaInfo(pOtaInfo);
	if (DRV_OK != nDrvErr)
	{
		PrintError("[DRV_FTP_open] FTP cmd connection failed ! \n");
		return DI_ERR_ERROR;
	}

	/* Open Command port */
	nDrvErr = P_FTP_Connect_HostPort(&nCmdSocket, s_stFtpTransInfo.aServerIp, s_stFtpTransInfo.usPortNum);
	if (DRV_OK != nDrvErr)
	{
		PrintError("[DRV_FTP_open] FTP cmd connection failed  to %s %d \n", s_stFtpTransInfo.aServerIp, s_stFtpTransInfo.usPortNum);
		return DI_ERR_ERROR;
	}
	else
	{
		s_stFtpTransInfo.ulCmdSocket = nCmdSocket;
		PrintDebug("[DRV_FTP_open] Connected to remote host.\n");
	}

	/* Set FTP mode */
	pRxDataBuff = VK_MEM_Alloc((unsigned long )MAX_PACKET_SIZE);
	if(pRxDataBuff == NULL)
	{
		nDrvErr = P_FTP_Sent_Cmd(quit, (HCHAR *)NULL);
		VK_TCP_close(nCmdSocket);
		PrintError("[DRV_FTP_open] Allocation failed !!! \n");
		return DI_ERR_ERROR;
	}
	VK_MEM_Memset(pRxDataBuff, 0x0, MAX_PACKET_SIZE);

	SetModeInfo.bLoopEnd = FALSE;
	SetModeInfo.bSetModeOK = FALSE;

	while(1)
	{
		P_FTP_Polling_RecvPort(nCmdSocket, &nConnflag, &nRxReady, POLLING_FOR_CMD);

		if (nConnflag != TCPSTATUS_CONNECTED)
		{
			nDrvErr = P_FTP_Sent_Cmd(quit, (HCHAR *)NULL);
			VK_MEM_Free(pRxDataBuff);
			VK_TCP_close(nCmdSocket);
			PrintError("[DI_FTP_open] Remote host is no longer connected. \n");
			return DI_ERR_ERROR;
		}

		if (nRxReady > 0)
		{
			nRxDataSize = P_FTP_Recieve_Cmd(pRxDataBuff, MAX_PACKET_SIZE);
			if (nRxDataSize > 0)
			{
				nDrvErr = P_FTP_Get_ReplyCode(pRxDataBuff, &iReplyCodeDec);
				nDrvErr = P_FTP_Parse_ReplyCode(nCmdSocket, iReplyCodeDec, pRxDataBuff, &SetModeInfo);
			}

			if (nRxDataSize < 0)
			{
				PrintError("[DI_FTP_open] TCP read error. \n");
			}
		}

		if(SetModeInfo.bLoopEnd == TRUE)
		{
			PrintError("[DI_FTP_open] FTP Transmission setting OK \n");
			break;
		}
	}

	/* Check Mode set result */
	if(SetModeInfo.bSetModeOK == TRUE)
	{
		VK_MEM_Free(pRxDataBuff);
	}
	else
	{
		nDrvErr = P_FTP_Sent_Cmd(quit, (HCHAR *)NULL);
		VK_MEM_Free(pRxDataBuff);
		VK_TCP_close(nCmdSocket);
		PrintError("[DRV_FTP_open] Set FTP mode failed !!! \n");
		return DI_ERR_ERROR;
	}

	/* Open Data port */
	nDrvErr = P_FTP_Connect_HostPort(&ulDataSocket, s_stFtpTransInfo.aServerIp, s_stFtpTransInfo.usPasvPortNum);
	if (DRV_OK != nDrvErr)
	{
		nDrvErr = P_FTP_Sent_Cmd(quit, (HCHAR *)NULL);
		VK_TCP_close(nCmdSocket);
		PrintError("[DRV_FTP_open] FTP data connection failed  to %s %d \n",
			s_stFtpTransInfo.aServerIp, s_stFtpTransInfo.usPasvPortNum);
		return DI_ERR_ERROR;
	}
	else
	{
		s_stFtpTransInfo.ulDataSocket = ulDataSocket;
		PrintDebug("[DRV_FTP_open] Connected to remote host.\n");
	}

	/* Request Data Send */
	nDrvErr = P_FTP_Sent_Cmd(download, (HCHAR *)s_stFtpTransInfo.aFileName);
	if (DRV_OK != nDrvErr)
	{
		PrintError("[DI_FTP_open] error %08X\n", nDrvErr);
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;

}

HUINT32 DI_FTP_read(HUINT8 *buf, HUINT32 ullen)
{
	HINT32 nRxDataSize=0;
	HUINT32 iRequestSize=ullen;
	HUINT32 iReadTotal=0;
	HUINT32 ulTimerId;

	s_bTimeout = FALSE;
	VK_TIMER_EventAfter(FTP_RECV_TIMEOUT, P_FTP_Recieve_TimeoutCallback, (void *)NULL, (HINT32)NULL, (HULONG *)&ulTimerId);

	while(!s_bTimeout)
	{
		nRxDataSize = P_FTP_Recieve_Data(buf, iRequestSize);
		iReadTotal += nRxDataSize;
		s_stFtpTransInfo.ulFileSize -= nRxDataSize;

		if(ullen == iReadTotal || s_stFtpTransInfo.ulFileSize == 0)
		{
			VK_TIMER_Cancel(ulTimerId);
			return iReadTotal;
		}
		else
		{
			iRequestSize -= nRxDataSize;
			buf +=nRxDataSize;;
		}

		if(nRxDataSize < 0)
		{
			VK_TASK_Sleep(10);
		}
	}

	PrintError("[DI_FTP_read] Remote host is no longer connected. \n");
	return 0;
}

HUINT32 DI_FTP_write(void)
{
	HUINT32 iWriteTotal=0;

	// extension for send file. not supported now.
	return iWriteTotal;
}

DI_ERR_CODE DI_FTP_seek(void)
{
	// extension for file control. not supported now.
	return DI_ERR_OK;
}

DI_ERR_CODE DI_FTP_close(void)
{
	DRV_Error nDrvErr = DRV_OK;

	nDrvErr = P_FTP_Sent_Cmd(quit, (HCHAR *)NULL);
	if (DRV_OK != nDrvErr)
	{
		PrintError("[DI_FTP_close] error %08X\n", nDrvErr);
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_FTP_uninit(void)
{
	VK_TCP_close(s_stFtpTransInfo.ulCmdSocket);
	VK_TCP_close(s_stFtpTransInfo.ulDataSocket);

	return DI_ERR_OK;
}

DI_ERR_CODE DI_FTP_setFtpOtaPath(HUINT8 *pOtaInfo)
{
	DRV_Error nDrvErr = DRV_OK;
	HINT32	i, res;
	HCHAR	*pStartIndex=0, *pEndIndex=0;
	HUINT8	aInfoBuff[SIZE_NET_PATH];
	DI_FTP_OTA_SETTING_T *pFtpOtaSet;

	if(pOtaInfo ==NULL)
	{
		PrintError("[DI_FTP_setFtpOtaPath] pOtaInfo is NULL !!! \n");
		return DI_ERR_ERROR;
	}

	/* Alloc OTA Struct */
	pFtpOtaSet = VK_MEM_Alloc(sizeof(DI_FTP_OTA_SETTING_T));
	 VK_MEM_Memset(pFtpOtaSet, 0x0, sizeof(DI_FTP_OTA_SETTING_T));

	/* Get IP address */
	VK_MEM_Memset(aInfoBuff, 0, SIZE_NET_PATH);
	for (i=0; i<SIZE_NET_PATH; i++)
	{
		if (pOtaInfo[i] == ':' || pOtaInfo[i] == 0)
		{
			pStartIndex = (HCHAR *)pOtaInfo+i+1;
			break;
		}
		aInfoBuff[i] = pOtaInfo[i];
	}

	res = P_FTP_ParseIpAddr((HCHAR *)aInfoBuff, pFtpOtaSet->aServerIp);
	if (res != 0)
	{
		VK_MEM_Free(pFtpOtaSet);
		PrintError("[DI_FTP_setFtpOtaPath] error %08X : parseipaddr \n", res);
		return DI_ERR_ERROR;
	}
	PrintDebug("[DI_FTP_setFtpOtaPath] IP : %d.%d.%d.%d\n", pFtpOtaSet->aServerIp[0], pFtpOtaSet->aServerIp[1], pFtpOtaSet->aServerIp[2], pFtpOtaSet->aServerIp[3]);

	/* Get Port Number */
#if 0
	pEndIndex = strchr(pStartIndex,':');
	pPortNum = DD_MEM_Alloc(pEndIndex-pStartIndex+1);
	strncpy(pPortNum, pStartIndex, pEndIndex-pStartIndex);
	pPortNum[pEndIndex-pStartIndex] = NULL;
	pFtpOtaSet->usPortNum = atoi(pPortNum);
	VK_MEM_Free((void *)pPortNum);
	pStartIndex = pEndIndex+1;
	PrintDebug("[DI_ETH_SetFtpOtaPath] Port Number : %d \n", pFtpOtaSet->usPortNum);
#endif

	/* Get User Name */
	if(pStartIndex == 0 || pStartIndex == NULL)
	{
		VK_MEM_Free(pFtpOtaSet);
		PrintError("[DI_FTP_setFtpOtaPath] User Name is null\n");
		return DI_ERR_ERROR;
	}

	pEndIndex = VK_strchr(pStartIndex,':');
	VK_strncpy((HCHAR *)pFtpOtaSet->aUserName, pStartIndex, (HUINT32)(pEndIndex-pStartIndex));
	pStartIndex = pEndIndex+1;
	PrintDebug("[DI_FTP_setFtpOtaPath] User Name : %s \n", pFtpOtaSet->aUserName);

	/* Get Passwd */
	pEndIndex = VK_strchr(pStartIndex,':');
	VK_strncpy((HCHAR *)pFtpOtaSet->aPasswd, pStartIndex, (HUINT32)(pEndIndex-pStartIndex));
	pStartIndex = pEndIndex+1;
	PrintDebug("[DI_FTP_setFtpOtaPath] Passwd : %s \n", pFtpOtaSet->aPasswd);

	/* Get DIR */
	pEndIndex = VK_strchr(pStartIndex,':');
	VK_strncpy((HCHAR *)pFtpOtaSet->aDirPath, pStartIndex, (HUINT32)(pEndIndex-pStartIndex));
	pStartIndex = pEndIndex+1;
	PrintDebug("[DI_FTP_setFtpOtaPath] DIR : %s \n", pFtpOtaSet->aDirPath);

	/* Get File Name */
	if((FTP_FILE_NAME_LENGTH -1) < VK_strlen(pStartIndex))
	{
		PrintError("[DI_FTP_setFtpOtaPath] Length of file name is overflow \n");
		VK_MEM_Free(pFtpOtaSet);
		return DI_ERR_ERROR;
	}
	else
	{
		VK_strncpy((HCHAR *)pFtpOtaSet->aFileName, pStartIndex, VK_strlen(pStartIndex));
		PrintDebug("[DI_FTP_setFtpOtaPath] File Name : %s \n", pFtpOtaSet->aFileName);
	}

	/* Write to NVRAM */
	nDrvErr = DRV_NVRAM_SetField(DI_NVRAM_FIELD_OTA_FTP_INFO, 0, (void *)pFtpOtaSet, sizeof(DI_FTP_OTA_SETTING_T));
	if (DRV_OK != nDrvErr)
	{
		PrintError("[DI_FTP_setFtpOtaPath] error %08X : DI_EEPROM_SetField\n", nDrvErr);
		VK_MEM_Free(pFtpOtaSet);
		return DI_ERR_ERROR;
	}

	/* Free OTA Struct */
	VK_MEM_Free(pFtpOtaSet);

	return DI_ERR_OK;
}

DI_ERR_CODE DI_FTP_getFtpOtaPath(DI_FTP_OTA_SETTING_T *pszOtaPath)
{
	DRV_Error nDrvErr = DRV_OK;

	nDrvErr = DRV_NVRAM_GetField(DI_NVRAM_FIELD_OTA_FTP_INFO, 0, (void *)pszOtaPath, sizeof(DI_FTP_OTA_SETTING_T));
	if (DRV_OK != nDrvErr)
	{
		PrintError("[DI_FTP_getFtpOtaPath] error %08X : DI_EEPROM_GetField\n", nDrvErr);
		return DI_ERR_ERROR;
	}

	PrintDebug("[DI_FTP_getFtpOtaPath] OTA PATH : %d %d %d %d : %s : %s : %s : %s \n",
		pszOtaPath->aServerIp[0],
		pszOtaPath->aServerIp[1],
		pszOtaPath->aServerIp[2],
		pszOtaPath->aServerIp[3],
		pszOtaPath->aUserName,
		pszOtaPath->aPasswd,
		pszOtaPath->aDirPath,
		pszOtaPath->aFileName);

	return DI_ERR_OK;
}

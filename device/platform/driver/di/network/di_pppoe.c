/*******************************************************************************
* File name : di_pppoe.c
* Author :
* description :
*
* Copyright (c)  by Humax Co., Ltd. Since 2012.
* All right reserved
*******************************************************************************/

/*******************************************************************************
* Header and definitions
*******************************************************************************/
/* global header files */
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <resolv.h>

#include <linux/sockios.h>
#include <linux/if.h>
#include <errno.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <fcntl.h>

/* chip manufacture's  header files */

/* humax header files */
#include "htype.h"
#include "taskdef.h"
#include "vkernel.h"
#include "nvram_map.h"
#include "di_err.h"
#include "di_uart.h"
#include "di_nvram.h"
#include "di_pppoe.h"
#include "drv_err.h"
#include "drv_network.h"
#include "drv_pppoe.h"

/*******************************************************************************
* Debug level
*******************************************************************************/
//#define PPPOE_DEBUG
#ifdef PPPOE_DEBUG
#define PrintDebug(fmt, ...)	VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintError(fmt, ...)	VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter()			VK_printf("(+)%s\n", __FUNCTION__)
#define PrintExit()				VK_printf("(-)%s\n", __FUNCTION__)
#else
#define PrintDebug(fmt, ...)
#define PrintError(fmt, ...)	VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter()
#define PrintExit()
#endif

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/

#define	PPPOE_TASK_PRIORITY			USER_PRIORITY5
#define PPPOE_TASK_STACK_SIZE		BASIC_TASK_STACK_SIZE


const HUINT8 s_aucMacNull[MAC_ADDR_LEN] = {0, };
const HUINT8 s_aucIpv4Null[IPV4_ADDR_LEN] = {0, };
const HUINT8 s_aucIpv6Null[IPV6_ADDR_LEN] = {0, };

#define IS_VALID_MAC_ADDR(mac_addr) 	VK_memcmp(s_aucMacNull, mac_addr, MAC_ADDR_LEN)
#define IS_VALID_IPV4_ADDR(ipv4_addr) 	VK_memcmp(s_aucIpv4Null, ipv4_addr, IPV4_ADDR_LEN)
#define IS_VALID_IPV6_ADDR(ipv6_addr) 	VK_memcmp(s_aucIpv6Null, ipv6_addr, IPV4_ADDR_LEN)

#define UNREFENCED_PARAM(x) 		(x=x)

#define	PPPOE_ID_FILE01		"/etc/ppp/pap-secrets"
#define	PPPOE_ID_FILE02		"/etc/ppp/chap-secrets"
#define	PPPOE_ID_01_TEMP	"/var/lib/humaxtv/ppp/pap-secrets.temp"
#define	PPPOE_ID_01_TEMP_RW	"/var/lib/humaxtv/ppp/pap-secrets"
#define	PPPOE_ID_02_TEMP	"/var/lib/humaxtv/ppp/chap-secrets.temp"
#define	PPPOE_ID_02_TEMP_RW	"/var/lib/humaxtv/ppp/chap-secrets"
#define	PPPOE_CONF			"/etc/ppp/pppoe.conf"
#define	PPPOE_CONF_TEMP		"/var/lib/humaxtv/ppp/pppoe.conf.temp"
#define	PPPOE_CONF_TEMP_RW	"/var/lib/humaxtv/ppp/pppoe.conf"
#define	PPPOE_CONF_ID_PATTERN	"USER='%s'\n"
#define	PPPOE_CONF_DEV_PATTERN	"ETH='%s'\n"
#define	PPPOE_IDPW_PATTERN	"\"%s\"\t*\t\"%s\"\n"
#define PPPOE_DEV_IF_NAME	"ppp0"
#define PPPOE_RESULT_FILE	"/tmp/adsl_cmd.tmp"
#define PPPOE_TIMEOUT		(30)

#define	GET_NET_DEV_SEM(DevId)								\
do {											\
	if(VK_SEM_Get(s_stPPPoEInst->ulSemaId) != VK_OK)			\
	{										\
		PrintError("[%s:%d] VK_SEM_Get Error!\n", __FUNCTION__, __LINE__);	\
		return DI_ERR_ERROR;							\
	}										\
} while(0)
#define	REL_NET_DEV_SEM(DevId) 								\
do {											\
	if(VK_SEM_Release(s_stPPPoEInst->ulSemaId) != VK_OK)			\
	{										\
		PrintError("[%s:%d] VK_SEM_Release Error!\n", __FUNCTION__, __LINE__);	\
		return DI_ERR_ERROR;							\
	}										\
} while(0)

typedef struct
{
	HUINT32			ulMsgId;
	HUINT32			ulNotifyTaskId;
	HUINT32			ulSemaId;

	HUINT32			ulExecTaskId;
	DI_PPPOE_INFO_t	stPPPoEInfo;
	pfnDI_PPPOE_EVENT_CALLBACK pfnEventCallback[DI_PPPOE_EVENT_MAX];
} P_PPPOE_INSTANCE_t;

static P_PPPOE_INSTANCE_t * s_stPPPoEInst = NULL;


/*******************************************************************************
* function : P_PPPOE_IsConnected
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
void P_PPPOE_IsConnected(HBOOL *pbConnected)
{
	int		fd;
	char	dev_path[256];
	char	link_status;
	HBOOL	connected = FALSE;

#if 0
	/* TODO: connect status is checked through ioctl with pppoe flags (like PPPIOCATTCHAN or another) */
#else
	VK_snprintf(dev_path, sizeof(dev_path)-1, "/sys/class/net/%s/carrier", PPPOE_DEV_IF_NAME);
	fd = open(dev_path, O_RDONLY);
	if(fd >= 0)
	{
		if(read(fd, &link_status, sizeof(char)) > 0)
		{
			connected = (link_status == '1') ? TRUE : FALSE;
		}
		close(fd);
	}
#endif

	*pbConnected = connected;
}

/*******************************************************************************
* function : P_PPPOE_ExecDisconnectTask
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
void P_PPPOE_ExecDisconnectTask(void *pvParam)
{
	P_PPPOE_INSTANCE_t *pstInstance;
	DI_PPPOE_CALLBACK_DATA_t stCallbackData;
	HCHAR szCmdBuf[128];
	int vkErr;
	HULONG ulCurrentTaskId;

	UNREFENCED_PARAM(pvParam);

	pstInstance = s_stPPPoEInst;
	ulCurrentTaskId = pstInstance->ulExecTaskId;

	VK_snprintf(szCmdBuf, 128, "adsl_cmd adsl-stop");

	vkErr = VK_SYSTEM_Command(szCmdBuf);
	if(!vkErr)
	{
		P_PPPOE_IsConnected(&pstInstance->stPPPoEInfo.bConnected);
		if(pstInstance->stPPPoEInfo.bConnected == FALSE)
		{
			stCallbackData.eEvent = DI_PPPOE_EVENT_DISCONNECT_SUCCESS;
		}
		else
		{
			stCallbackData.eEvent = DI_PPPOE_EVENT_DISCONNECT_FAIL;
		}
	}
	else
	{
		stCallbackData.eEvent = DI_PPPOE_EVENT_DISCONNECT_FAIL;
		/* connected status is not changed */
	}

	VK_MSG_SendTimeout(pstInstance->ulMsgId, &stCallbackData, sizeof(DI_PPPOE_CALLBACK_DATA_t), 0);

	pstInstance->ulExecTaskId = 0;
	VK_TASK_Destroy(ulCurrentTaskId);	/* async task destroy */

	while(1)	/* wait until self task detroy */
	{
		VK_TASK_Sleep(0xFFFFFFFF);
	}
}

/*******************************************************************************
* function : P_PPPOE_Disconnect
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error P_PPPOE_Disconnect(HUINT32 ulDevId)
{
	int vkErr;

	UNREFENCED_PARAM(ulDevId);

	if(s_stPPPoEInst->ulExecTaskId)
	{
		PrintError("[%s:%d] : the PPPoE was already executed!!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	vkErr = VK_TASK_Create(P_PPPOE_ExecDisconnectTask,
				PPPOE_TASK_PRIORITY,
				PPPOE_TASK_STACK_SIZE,
				"P_PPPOE_ExecDisconnectTask",
				(void*) 0,
				(unsigned long*) &s_stPPPoEInst->ulExecTaskId,
				TRUE);
	if(vkErr != VK_OK)
	{
		PrintError("[%s:%d] VK_TASK_Create Error!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	vkErr = VK_TASK_Start(s_stPPPoEInst->ulExecTaskId);
	if(vkErr != VK_OK)
	{
		PrintError("[%s:%d] VK_TASK_Start Error!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	return DRV_OK;
}

/*******************************************************************************
* function : P_PPPOE_ExecConnectTask
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
void P_PPPOE_ExecConnectTask(void *pvParam)
{
	P_PPPOE_INSTANCE_t *pstInstance;
	DI_PPPOE_CALLBACK_DATA_t stCallbackData;
	HCHAR szCmdBuf[128];
	int vkErr;
	HULONG ulCurrentTaskId;

	pstInstance = (P_PPPOE_INSTANCE_t *)pvParam;
	ulCurrentTaskId = pstInstance->ulExecTaskId;

	VK_snprintf(szCmdBuf, 128, "adsl_cmd adsl-start %d", pstInstance->stPPPoEInfo.ulWaitSeconds);

	vkErr = VK_SYSTEM_Command(szCmdBuf);
	if(!vkErr)
	{
		P_PPPOE_IsConnected(&pstInstance->stPPPoEInfo.bConnected);
		if(pstInstance->stPPPoEInfo.bConnected == TRUE)
		{
			stCallbackData.eEvent = DI_PPPOE_EVENT_CONNECT_SUCCESS;
		}
		else
		{
			stCallbackData.eEvent = DI_PPPOE_EVENT_CONNECT_FAIL;
		}
	}
	else
	{
		stCallbackData.eEvent = DI_PPPOE_EVENT_CONNECT_FAIL;
		pstInstance->stPPPoEInfo.bConnected = FALSE;
	}

	VK_MSG_SendTimeout(pstInstance->ulMsgId, &stCallbackData, sizeof(DI_PPPOE_CALLBACK_DATA_t), 0);

	pstInstance->ulExecTaskId = 0;
	VK_TASK_Destroy(ulCurrentTaskId);	/* async task destroy */

	while(1)	/* wait until self task detroy */
	{
		VK_TASK_Sleep(0xFFFFFFFF);
	}
}

/*******************************************************************************
* function : P_PPPOE_Connect
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error P_PPPOE_Connect(HUINT32 ulDevId, DI_PPPOE_INFO_t *pstParams)
{
	int vkErr;

	if(s_stPPPoEInst->ulExecTaskId)
	{
		PrintError("[%s:%d] : the PPPoE was already executed!!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	UNREFENCED_PARAM(ulDevId);
	//P_PPPOE_Disconnect(ulDevId);

	VK_memcpy(&s_stPPPoEInst->stPPPoEInfo, pstParams, sizeof(DI_PPPOE_INFO_t));
	s_stPPPoEInst->stPPPoEInfo.ulWaitSeconds = PPPOE_TIMEOUT;

	vkErr = VK_TASK_Create(P_PPPOE_ExecConnectTask,
				PPPOE_TASK_PRIORITY,
				PPPOE_TASK_STACK_SIZE,
				"P_PPPOE_ExecConnectTask",
				(void*) s_stPPPoEInst,
				(unsigned long*) &s_stPPPoEInst->ulExecTaskId,
				TRUE);
	if(vkErr != VK_OK)
	{
		PrintError("[%s:%d] VK_TASK_Create Error!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	vkErr = VK_TASK_Start(s_stPPPoEInst->ulExecTaskId);
	if(vkErr != VK_OK)
	{
		PrintError("[%s:%d] VK_TASK_Start Error!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	return DRV_OK;
}

/*******************************************************************************
* function : P_PPPOE_SetInfo
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error P_PPPOE_SetInfo(HUINT32 ulDevId, DI_PPPOE_INFO_t *pstPPPoEInfo)
{
	int				i = 0;
	FILE			*dirtyFile = NULL;
	FILE			*tempFile = NULL;
	unsigned char	*p[] = {PPPOE_ID_FILE01, PPPOE_ID_FILE02, };
	unsigned char	*t0[] = {PPPOE_ID_01_TEMP, PPPOE_ID_02_TEMP, };
	unsigned char	*t1[] = {PPPOE_ID_01_TEMP_RW, PPPOE_ID_02_TEMP_RW, };

	HINT32 nReadbyte = 0;
	HUINT8 buff[128] = {0, };

	UNREFENCED_PARAM(ulDevId);

	/* pppoe.conf */
	dirtyFile = fopen(PPPOE_CONF, "r+");
	if(dirtyFile == NULL)
	{
		PrintError("[%s:%d] file(%s) is not.\n",__FUNCTION__, __LINE__, PPPOE_CONF);
	}
	else
	{
		tempFile = fopen(PPPOE_CONF_TEMP, "w");
		if(tempFile == NULL)
		{
			fclose(dirtyFile);
			PrintError("[%s:%d] file(%s) is not.\n", __FUNCTION__, __LINE__, PPPOE_CONF_TEMP);
		}
		else
		{
			/* read file */
			fseek(dirtyFile, 0, SEEK_SET);
			VK_memset(buff, 0, sizeof(unsigned char)*128);
			while(fgets(buff, 128, dirtyFile))
			{
				nReadbyte = VK_strlen(buff);
				if(VK_strstr(buff, "USER="))
				{
					VK_memset(buff, 0, sizeof(unsigned char)*128);
					VK_snprintf(buff, 128, PPPOE_CONF_ID_PATTERN, pstPPPoEInfo->aucId);
				}
				fwrite(buff, VK_strlen(buff), 1, tempFile);
				PrintDebug("[%s:%d] copy buff[%s]\n", __FUNCTION__, __LINE__, buff);

				VK_memset(buff, 0, sizeof(unsigned char)*128);
			}

			fclose(tempFile);
			fclose(dirtyFile);

			rename(PPPOE_CONF_TEMP, PPPOE_CONF_TEMP_RW);
		}
	}

	/* secret files */
	for(i = 0; i < 2; i++)
	{
		dirtyFile = fopen(p[i], "r+");
		PrintDebug("[%s:%d] Set ID_PW file(%s)\n", __FUNCTION__, __LINE__, p[i]);
		if(dirtyFile == NULL)
		{
			PrintDebug("[%s:%d] file(%s) is not.\n", __FUNCTION__, __LINE__, p[i]);
			continue;
		}
		else
		{
			HINT32 nReadbyte = 0;
			HUINT8 buff[128] = {0, };
			HBOOL	bIsFound = FALSE;

			tempFile = fopen(t0[i], "w");
			if(tempFile == NULL)
			{
				fclose(dirtyFile);
				PrintDebug("[%s:%d] file(%s) is not.\n", __FUNCTION__, __LINE__, t0[i]);
				continue;
			}
			else
			{
				/* read file */
				fseek(dirtyFile, 0, SEEK_SET);
				VK_memset(buff, 0, sizeof(unsigned char)*128);
				while(fgets(buff, 128, dirtyFile))
				{
					nReadbyte = VK_strlen(buff);
					if(VK_strstr(buff, pstPPPoEInfo->aucId))
					{
						/* id, pw를 기록한다.  */
						VK_memset(buff, 0, sizeof(unsigned char)*128);
						VK_snprintf(buff, 128, PPPOE_IDPW_PATTERN, pstPPPoEInfo->aucId, pstPPPoEInfo->aucPassword);
						bIsFound = TRUE;
					}
					fwrite(buff, VK_strlen(buff), 1, tempFile);
					PrintDebug("[%s:%d] copy buff[%s]\n", __FUNCTION__, __LINE__, buff);

					VK_memset(buff, 0, sizeof(unsigned char)*128);
				}

				if(bIsFound == FALSE)
				{
					/* ID, pw를 기록한다.  */
					VK_memset(buff, 0, sizeof(unsigned char)*128);
					VK_snprintf(buff, 128, PPPOE_IDPW_PATTERN, pstPPPoEInfo->aucId, pstPPPoEInfo->aucPassword);
					fwrite(buff, VK_strlen(buff), 1, tempFile);
				}

				PrintDebug("[%s:%d]id pw pair is [%s].\n", __FUNCTION__, __LINE__, buff);

				fclose(tempFile);
				fclose(dirtyFile);

				rename(t0[i], t1[i]);
			}
		}
	}

	pstPPPoEInfo->ulWaitSeconds = PPPOE_TIMEOUT;

	return DI_ERR_OK;
}

/*******************************************************************************
* function : P_PPPOE_GetInfo
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error P_PPPOE_GetInfo(HUINT32 ulDevId, DI_PPPOE_INFO_t *pstPPPoEInfo)
{
	FILE	*dirtyFile = NULL;
	HUINT8	ID[PPPOE_ID_LEN];
	HBOOL	bFound = FALSE;

	UNREFENCED_PARAM(ulDevId);

	PrintEnter();

	if(pstPPPoEInfo == NULL)
	{
		PrintError("[%s:%d] Invalid Device ID!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

 	VK_memset(pstPPPoEInfo, 0, sizeof(DI_PPPOE_INFO_t));
	pstPPPoEInfo->ulWaitSeconds = PPPOE_TIMEOUT;
	pstPPPoEInfo->bConnected = s_stPPPoEInst->stPPPoEInfo.bConnected;

	/* pppoe.conf */
	dirtyFile = fopen(PPPOE_CONF, "r");
	if(dirtyFile == NULL)
	{
		PrintError("[%s:%d] file(%s) is not.\n", __FUNCTION__, __LINE__,PPPOE_CONF);
		VK_strncpy(pstPPPoEInfo->aucId, "unknown", VK_strlen("unknown"));
		VK_strncpy(pstPPPoEInfo->aucPassword, "unknown", VK_strlen("unknown"));
	}
	else
	{
		HINT32 nReadbyte = 0;
		HUINT8 buff[128] = {0, };
		HUINT8 *pHeadQuotePos = 0;
		HUINT8 *pTailQuotePos = 0;
		HINT32 nSize = 0;

		/* read file */
		fseek(dirtyFile, 0, SEEK_SET);
		VK_memset(buff, 0, sizeof(unsigned char)*128);
		while(fgets(buff, 128, dirtyFile))
		{
			nReadbyte = VK_strlen(buff);
			if(VK_strstr(buff, "USER="))
			{
				VK_memset(ID, 0, sizeof(unsigned char)*64);
				if((pHeadQuotePos = VK_strchr(buff, '\'')) != NULL)
				{
					pTailQuotePos = VK_strchr(pHeadQuotePos+1, '\'');
					nSize = pTailQuotePos - pHeadQuotePos - 1;
					VK_strncpy(ID, pHeadQuotePos+1, nSize);
					bFound = TRUE;
					if(nSize+1 > PPPOE_ID_LEN)
					{
						fclose(dirtyFile);
						/* even if id doesn't exist, information field has to fill out */
						VK_strncpy(pstPPPoEInfo->aucId, "unknown", VK_strlen("unknown"));
						VK_strncpy(pstPPPoEInfo->aucPassword, "unknown", VK_strlen("unknown"));
						return DRV_OK;
						//return DI_ERR_ERROR;
					}
					VK_strncpy(pstPPPoEInfo->aucId, ID, nSize);
					break;
				}
			}
		}

		fclose(dirtyFile);
	}

	dirtyFile = fopen(PPPOE_ID_FILE02, "r");
	PrintDebug("[%s] Set ID_PW file(%s)\n", __FUNCTION__, PPPOE_ID_FILE02);
	if(dirtyFile == NULL)
	{
		PrintError("[%s:%d] file(%s) is not.\n", __FUNCTION__,__LINE__,PPPOE_ID_FILE02);
		VK_strncpy(pstPPPoEInfo->aucId, "unknown", VK_strlen("unknown"));
		VK_strncpy(pstPPPoEInfo->aucPassword, "unknown", VK_strlen("unknown"));
		return DRV_OK;
		//return DI_ERR_ERROR;
	}
	else if(bFound == FALSE)
	{
		fclose(dirtyFile);
		PrintError("[%s:%d] can not find USER ID.\n",__FUNCTION__,__LINE__);
		VK_strncpy(pstPPPoEInfo->aucId, "unknown", VK_strlen("unknown"));
		VK_strncpy(pstPPPoEInfo->aucPassword, "unknown", VK_strlen("unknown"));
		return DRV_OK;
		//return DI_ERR_ERROR;
	}
	else
	{
		HINT32 nReadbyte = 0;
		HINT32 nCntOfQuote = 0;
		HUINT8 buff[128] = {0, };
		HUINT8 *pHeadQuotePos = 0;
		HUINT8 *pTailQuotePos = 0;
		HINT32 nSize = 0;

		bFound = FALSE;

		/* read file */
		fseek(dirtyFile, 0, SEEK_SET);
		VK_memset(buff, 0, sizeof(unsigned char)*128);
		while(fgets(buff, 128, dirtyFile))
		{
			nReadbyte = VK_strlen(buff);
			if((pHeadQuotePos = VK_strstr(buff, ID)) != NULL)
			{
				while((pHeadQuotePos = VK_strchr(pHeadQuotePos+1, '\"')) != NULL)
				{
					nCntOfQuote++;
					if(nCntOfQuote == 2)
					{
						pTailQuotePos = VK_strchr(pHeadQuotePos+1, '\"');
						nSize = pTailQuotePos - pHeadQuotePos - 1;
						VK_strncpy(ID, pHeadQuotePos+1, nSize);
						bFound = TRUE;
						if(nSize+1 > PPPOE_PW_LEN)
						{
							fclose(dirtyFile);
							VK_strncpy(pstPPPoEInfo->aucId, "unknown", VK_strlen("unknown"));
							VK_strncpy(pstPPPoEInfo->aucPassword, "unknown", VK_strlen("unknown"));
							return DRV_OK;
							//return DI_ERR_ERROR;
						}
						VK_strncpy(pstPPPoEInfo->aucPassword, ID, nSize);
						break;
					}
				}
			}
		}

		fclose(dirtyFile);
	}

	if(bFound == FALSE)
	{
		PrintError("[%s:%d] can not find USER PASSWORD.\n",__FUNCTION__,__LINE__);
		VK_strncpy(pstPPPoEInfo->aucId, "unknown", VK_strlen("unknown"));
		VK_strncpy(pstPPPoEInfo->aucPassword, "unknown", VK_strlen("unknown"));
		return DRV_OK;
		//return DI_ERR_ERROR;
	}

	return DRV_OK;
}

static void P_PPPOE_EventNotifyTask(void *pvParam)
{
	VK_ERR_CODE eVkErr;
	DI_PPPOE_CALLBACK_DATA_t stCallbackData;

	UNREFENCED_PARAM(pvParam);

	while(1)
	{
		eVkErr = VK_MSG_Receive(s_stPPPoEInst->ulMsgId, &stCallbackData, sizeof(DI_PPPOE_CALLBACK_DATA_t));
		if (eVkErr != VK_OK)
		{
			PrintError("[%s:%d] VK_MSG_Receive Error!\n", __FUNCTION__, __LINE__);
			continue;
		}

		/* TODO: how to use device id */

		if(stCallbackData.eEvent >= DI_PPPOE_EVENT_MAX)
		{
			PrintError("[%s:%d] Invalid Event!\n", __FUNCTION__, __LINE__);
			continue;
		}

		if(s_stPPPoEInst->pfnEventCallback[stCallbackData.eEvent])
			s_stPPPoEInst->pfnEventCallback[stCallbackData.eEvent](&stCallbackData);

	}
}

DRV_Error DRV_PPPOE_GetLinkStatus(HUINT32 ulDevId, HBOOL *pbConnect)
{
	if(pbConnect == NULL)
		return DRV_ERR;

	UNREFENCED_PARAM(ulDevId);

	PrintEnter();

	P_PPPOE_IsConnected(pbConnect);

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_PPPOE_Init(void)
{
	HINT32	nVkErr;

	/* malloc for network instance */
	s_stPPPoEInst = (P_PPPOE_INSTANCE_t *)VK_MEM_Alloc(sizeof(P_PPPOE_INSTANCE_t));
	VK_MEM_Memset(s_stPPPoEInst, 0, sizeof(P_PPPOE_INSTANCE_t));

	nVkErr = VK_SEM_Create((unsigned long*) &s_stPPPoEInst->ulSemaId, "PPPoE", VK_SUSPENDTYPE_FIFO);
	if(nVkErr != VK_OK)
	{
		PrintError("[%s] Creating PPPoE VK_SEM_Create failed!\n",__func__);
		return DRV_ERR;
	}

	nVkErr = VK_MSG_Create(MSGQ_SIZE_NORMAL, sizeof(DI_PPPOE_CALLBACK_DATA_t),"PPPoEMsgQ",(unsigned long*) &s_stPPPoEInst->ulMsgId, VK_SUSPENDTYPE_FIFO);
	if(nVkErr != VK_OK)
	{
		PrintError("[%s] Creating PPPoE VK_MSG_Create failed!\n",__func__);
		VK_SEM_Destroy(s_stPPPoEInst->ulSemaId);
		return DRV_ERR;
	}

	nVkErr = VK_TASK_Create(P_PPPOE_EventNotifyTask, PPPOE_TASK_PRIORITY, PPPOE_TASK_STACK_SIZE, "P_PPPOE_EventNotifyTask", (void *)0, (unsigned long*) &s_stPPPoEInst->ulNotifyTaskId, FALSE);
	if(nVkErr != VK_OK)
	{
		PrintError("[%s] Creating PPPoE VK_TASK_Create failed!\n",__func__);
		VK_SEM_Destroy(s_stPPPoEInst->ulSemaId);
		VK_MSG_Destroy(s_stPPPoEInst->ulMsgId);
		return DRV_ERR;
	}

	nVkErr = VK_TASK_Start(s_stPPPoEInst->ulNotifyTaskId);
	if(nVkErr != VK_OK)
	{
		PrintError("[%s] Creating PPPoE VK_TASK_Create failed!\n",__func__);
		VK_SEM_Destroy(s_stPPPoEInst->ulSemaId);
		VK_MSG_Destroy(s_stPPPoEInst->ulMsgId);
		VK_TASK_Destroy(s_stPPPoEInst->ulNotifyTaskId);
		return DRV_ERR;
	}

	PrintDebug("[%s:%d] Initialized Successfully \n", __FUNCTION__, __LINE__);

	return DRV_OK;
}

/*******************************************************************************
* function : DI_PPPOE_RegisterEventCallback
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DI_ERR_CODE DI_PPPOE_RegisterEventCallback(HUINT32 ulDevId, DI_PPPOE_NOTIFY_EVENT_e eEvent, pfnDI_PPPOE_EVENT_CALLBACK pfnEventCallback)
{
	UNREFENCED_PARAM(ulDevId);

	PrintEnter();

	if(eEvent >= DI_PPPOE_EVENT_MAX)
	{
		PrintError("[%s] Fail DI_PPPOE_RegisterEventCallback failed!\n",__func__);
		return DI_ERR_ERROR;
	}

	GET_NET_DEV_SEM(ulDevId);

	s_stPPPoEInst->pfnEventCallback[eEvent] = pfnEventCallback;

	REL_NET_DEV_SEM(ulDevId);

	PrintExit();

	return DI_ERR_OK;
}

/*******************************************************************************
* function : DI_PPPOE_Disconnect
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DI_ERR_CODE DI_PPPOE_Disconnect(HUINT32 ulDevId)
{
	DRV_Error eDrvErr;
	DI_ERR_CODE eDiErr = DI_ERR_OK;

	PrintEnter();

	GET_NET_DEV_SEM(ulDevId);

	eDrvErr = P_PPPOE_Disconnect(ulDevId);
	if(eDrvErr != DRV_OK)
	{
		PrintError("[%s:%d] P_PPPOE_Disconnect Error!\n", __FUNCTION__, __LINE__);
		eDiErr = DI_ERR_ERROR;
	}

	REL_NET_DEV_SEM(ulDevId);

	PrintExit();

	return eDiErr;
}

/*******************************************************************************
* function : DI_PPPOE_Connect
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DI_ERR_CODE DI_PPPOE_Connect(HUINT32 ulDevId, DI_PPPOE_INFO_t *pstPPPoEInfo)
{
	DRV_Error eDrvErr;
	DI_ERR_CODE eDiErr = DI_ERR_OK;

	PrintEnter();

	GET_NET_DEV_SEM(ulDevId);

	eDrvErr = P_PPPOE_Connect(ulDevId, pstPPPoEInfo);
	if(eDrvErr != DRV_OK)
	{
		PrintError("[%s:%d] P_PPPOE_Connect Error!\n", __FUNCTION__, __LINE__);
		eDiErr = DI_ERR_ERROR;
	}

	REL_NET_DEV_SEM(ulDevId);

	PrintExit();

	return eDiErr;
}

/*******************************************************************************
* function : DI_PPPOE_SetInfo
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DI_ERR_CODE DI_PPPOE_SetInfo(HUINT32 ulDevId, DI_PPPOE_INFO_t *pstPPPoEInfo)
{
	DRV_Error eDrvErr;
	DI_ERR_CODE eDiErr = DI_ERR_OK;

	PrintEnter();

	if(pstPPPoEInfo == NULL)
	{
		PrintError("[%s:%d] Invalid Parameter\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	if(pstPPPoEInfo->aucId[0] == '\0' || pstPPPoEInfo->aucPassword[0] == '\0')
	{
		PrintError("[%s:%d] id or password incorrect\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	GET_NET_DEV_SEM(ulDevId);

	eDrvErr = P_PPPOE_SetInfo(ulDevId, pstPPPoEInfo);
	if(eDrvErr != DRV_OK)
	{
		PrintError("[%s:%d] P_PPPOE_SetInfo Error!\n", __FUNCTION__, __LINE__);
		eDiErr = DI_ERR_ERROR;
	}

	REL_NET_DEV_SEM(ulDevId);

	PrintExit();

	return eDiErr;
}

/*******************************************************************************
* function : DI_PPPOE_GetInfo
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DI_ERR_CODE DI_PPPOE_GetInfo(HUINT32 ulDevId, DI_PPPOE_INFO_t *pstPPPoEInfo)
{
	DRV_Error eDrvErr;
	DI_ERR_CODE eDiErr = DI_ERR_OK;

	PrintEnter();

	if(pstPPPoEInfo == NULL)
	{
		PrintError("[%s:%d] Invalid Parameter\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	GET_NET_DEV_SEM(ulDevId);

	eDrvErr = P_PPPOE_GetInfo(ulDevId, pstPPPoEInfo);
	if(eDrvErr != DRV_OK)
	{
		PrintError("[%s:%d] P_PPPOE_GetInfo Error!\n", __FUNCTION__, __LINE__);
		eDiErr = DI_ERR_ERROR;
	}

	REL_NET_DEV_SEM(ulDevId);

	PrintExit();

	return eDiErr;
}


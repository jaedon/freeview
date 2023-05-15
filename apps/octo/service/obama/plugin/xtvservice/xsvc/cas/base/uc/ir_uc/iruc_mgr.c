/*
 * (c) 2011-2012 Humax Co., Ltd.
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
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

#include "octo_common.h"

#include <db_svc.h>
#include <iruc_mgr.h>
#include <iruc_drv.h>
#include <ir_ctrl.h>

#include <db_svc.h> //DbSvc_TsInfo_t
#include <_svc_cas_sub_dev_api.h>

//#include <_svc_cas_err.h>
//#include <_svc_cas_mgr.h>
//#include <_svc_cas_mgr_common.h>

#include "_svc_cas_mgr_main.h"
#include "_svc_cas_sub_svc_api.h"
#include "util.h" //for api :get 12 bit."


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



/********************************************************************
*	External variables
********************************************************************/

/********************************************************************
*	External function prototypes
********************************************************************/
void CAS_IRUC_Main_ProcessMsg(HMSG *msg);


/********************************************************************
*	Global variables
********************************************************************/
// TODO: temporal type definition. jichoi.

//typedef Handle_t	CAI_HANDLE;
//CAI_HANDLE		g_hIrCai = (CAI_HANDLE)NULL;

//TODO: definitions shall be fixed.
STATIC IRUC_ScInst_t 	s_IrScInst; //[HUMAX_VD_SC_SLOT_COUNT];

static SvcCas_Context_t			s_astCasIrContext[CAS_MAX_NUM_OF_ACTION];
/********************************************************************
*	Static variables
********************************************************************/


/********************************************************************
*	Static functions Prototypes
*	The following functions will be used as inner functions
********************************************************************/
static HERROR local_cas_iruc_Mgr_InitModule(void);
static void local_cas_iruc_Mgr_StartService(Handle_t hAction, Handle_t hSvc, SvcCas_ActionType_e eActType);// Action Type 전달 해준다.
static void local_cas_iruc_Mgr_StopService(Handle_t hAction, Handle_t hNextSvc);
static void local_cas_iruc_Mgr_IamSelected(Handle_t hAction, HUINT32 ulCasInstanceId, HBOOL bConnect);
static void local_cas_iruc_Mgr_SetPID(Handle_t hAction, SvcCas_SetPidInfo_t *pstCaDscInfo);
static void local_cas_iruc_Mgr_UpdatePmt(Handle_t hAction, HUINT8 *pucPmt);
static void local_cas_iruc_Mgr_UpdateCat(Handle_t hAction, HUINT16 usUniqueId, HUINT8 *pucCat);
static void local_UcMgr_UpdateSdt(Handle_t hAction, HUINT16 usUniqueId, HUINT8 *pucSdt);

void local_cas_iruc_SendMsg(HUINT16 msg_channel, HUINT16 msg_type, void* pMsg);


#define ___LOCAL_FUNCS_FOR_INITIALIZATION___

//#define	IRUC_SERIAL_COMMUNICATION
#if defined(IRUC_SERIAL_COMMUNICATION)	//  ewlee 20100507
#include <stdio.h>
#include <sys/ioctl.h>
//#include <sys/poll.h>
#include <sys/signal.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include "UniversalClient_API.h"

//#define	ASYNC_SERIAL_COMM

#define SERIAL_IRUC_PS_FILE_NAME_LEN_MAX 	(128)
#define SERIAL_IRUC_PS_FILE_JFFS2_PATH		"/var/lib/humaxtv/cas/irdeto_universal/"
#define SERIAL_IRUC_PS_FILE 			"SerialData.txt"


int local_cas_iruc_SerialDataWrite(unsigned char *pData, int offset, int nLen)
{
	PAL_FS_UINT32	hFileHandle;
	PAL_FS_INT64	nPosOffset;
	PAL_FS_UINT32	ulWrittenSize;
	HINT32	nErrCode;
	HUINT8	ucFileName[SERIAL_IRUC_PS_FILE_NAME_LEN_MAX];

	if (pData == NULL)
		return 0;

	if (svc_cas_DevFsIsDirExist(SERIAL_IRUC_PS_FILE_JFFS2_PATH) != TRUE)
	{
		if (svc_cas_DevFsMkdir(SERIAL_IRUC_PS_FILE_JFFS2_PATH) != ERR_OK)
			return 0;
	}
	sprintf(ucFileName, "%s%s", SERIAL_IRUC_PS_FILE_JFFS2_PATH, SERIAL_IRUC_PS_FILE);

	nErrCode = svc_cas_DevFsOpen(0, ucFileName, PAL_FS_MODE_WRITE, &hFileHandle);
	if (nErrCode)
	{
		return 0;
	}

	nErrCode = svc_cas_DevFsLSeek(hFileHandle, offset, PAL_FS_POS_START, &nPosOffset);
	if (nErrCode)
	{
		return 0;
	}

	nErrCode = svc_cas_DevFsWrite(hFileHandle, pData, nLen, &ulWrittenSize);
	if (nErrCode)
	{
		return 0;
	}

	svc_cas_DevFsClose(hFileHandle);

	//PAL_FS_Flush(void)
	return 1;
}

#define BAUDRATE B115200
#define MODEMDEVICE "/dev/ttyS0"

int		g_SerialFd = -1;
struct termios g_oldtio, g_newtio;
char g_readbuf[4096];//, g_writebuf[256];
int g_nreadindex = 0, g_nprocessindex = 0;
static HUINT32 s_SendId, s_ReceiveId;
HUINT32	g_SemID = 0;
int	g_nIsEnd = 0;
int	g_nOffset = 0;


#if defined(ASYNC_SERIAL_COMM)
int	g_WaitFlag = 1;
void local_cas_iruc_signal_handler_IO (int status);
#endif


#define	REQUEST_PING					0x01
#define	RESPONSE_PING					0x02
#define	REQUEST_TSC_CHIP_RECORD			0x03
#define	RESPONSE_TSC_CHIP_RECORD		0x04
#define	SEND_CSSK						0x05
#define	RECEIVE_CSSK					0x06
#define	SEND_EVENCW						0x07
#define	RECEIVE_EVENCW					0x08
#define	SEND_ODDCW						0x09
#define	RECEIVE_ODDCW					0x0A
#define	REQUEST_START_TEST				0x0B
#define	RESPONSE_START_TEST				0x0C
#define	END_PROTOCOL					0x0D

#define	HEADER_FIRST_BYTE				0x48	// 'H'
#define	HEADER_SECOND_BYTE				0x55	// 'U'
#define	HEADER_THIRD_BYTE				0x43	// 'C'

#define	STATE_CREATE_TSC_CHIP_RECORD		0x90
#define	STATE_WAIT_FOR_TSC_CHIP_RECORD		0x91
#define	STATE_RECEIVE_TSC_CHIP_RECORD		0x92
#define	STATE_END_TSC_CHIP_RECORD			0x93
#define	STATE_GET_STB_INFO					0x94
#define	STATE_SEND_CSSK						0x95
#define	STATE_WAIT_FOR_CSSK_RESPONSE		0x96
#define	STATE_SEND_EVENCW					0x97
#define	STATE_WAIT_FOR_EVENCW_RESPONSE		0x98
#define	STATE_SEND_ODDCW					0x99
#define	STATE_WAIT_FOR_ODDCW_RESPONSE		0x9A
#define	STATE_START_TEST					0x9B
#define	STATE_RECEIVE_DESCRAMBLE_STATUS		0x9C


typedef struct stTSCChipRecord
{
	unsigned short nSTBSNLen;
	unsigned char *pSTBSN;
	unsigned short nCSSNLen;
	unsigned char *pCSSN;
	unsigned int rsaMode;
	unsigned int jtag;
	unsigned int cwMode;
	unsigned short lockId;
	unsigned short systemId;
	unsigned short variant;
	unsigned short keyVersion;
	unsigned short signatureVersion;
	unsigned short manufacturerId;
	unsigned short loaderVersion;
	unsigned short hardwareVersion;
	unsigned short nPrivateDataLen;
	unsigned char *pPrivateData;
}TSC_CHIPRECORD;

typedef struct stDescrambleKey
{
	unsigned short nCSSKLen;
	unsigned char *pCSSK;
	unsigned short nEvenKeyLen;
	unsigned char *pEvenKey;
	unsigned short nOddKeyLen;
	unsigned char *pOddKey;
}DESCRAMBLE_KEY;

#if defined(ASYNC_SERIAL_COMM)
void local_cas_iruc_signal_handler_IO (int status)
 {
 	char	buf[1024];
	memset(buf, 0, 1024);
	sprintf(buf, "Receive Signal for reading data\n");
	local_cas_iruc_SerialDataWrite(buf, 0, 32);
    g_WaitFlag = 0;
 }
#endif

char local_cas_iruc_GetCheckSum(char *input, int size)
{
	char checkSum = 0x00;
	int i = 0;
	for(i=0; i<size; i++)
	{
		checkSum = checkSum ^ input[i];
	}

	return checkSum;
}

int local_cas_iruc_OpenSerialPort()
{
#if defined(ASYNC_SERIAL_COMM)
	struct sigaction saio;

	g_SerialFd = open(MODEMDEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK);
 	if (g_SerialFd <0) {
    		HxLOG_Error("&&&&&&&&&&&&&&& Failed to open serial port\n");
		return 0;
  	}

	memset(&saio, 0, sizeof(struct sigaction));
	saio.sa_handler = local_cas_iruc_signal_handler_IO;
//	saio.sa_mask = 0;
	saio.sa_flags = 0;
	saio.sa_restorer = NULL;
	sigaction(SIGIO,&saio,NULL);

	fcntl(g_SerialFd, F_SETOWN, getpid());
	fcntl(g_SerialFd, F_SETFL, FNDELAY);

  	tcgetattr(g_SerialFd,&g_oldtio);

  	bzero(&g_newtio, sizeof(g_newtio));
  	g_newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  	g_newtio.c_iflag = IGNPAR;
  	g_newtio.c_oflag = 0;
   	g_newtio.c_lflag = 0;
   	g_newtio.c_cc[VTIME] = 0;
  	g_newtio.c_cc[VMIN]  = 1;

  	tcflush(g_SerialFd, TCIFLUSH);
  	tcsetattr(g_SerialFd,TCSANOW,&g_newtio);
#else
	g_SerialFd = open(MODEMDEVICE, O_RDWR | O_NOCTTY);
 	if (g_SerialFd <0) {
    		HxLOG_Error("&&&&&&&&&&&&&&& Failed to open serial port\n");
		return 0;
  	}

  	tcgetattr(g_SerialFd,&g_oldtio);

  	bzero(&g_newtio, sizeof(g_newtio));
  	g_newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  	g_newtio.c_iflag = IGNPAR;
  	g_newtio.c_oflag = 0;

  	g_newtio.c_lflag = 0;

  	g_newtio.c_cc[VTIME] = 0;
  	g_newtio.c_cc[VMIN]  = 1;

  	tcflush(g_SerialFd, TCIFLUSH);
  	tcsetattr(g_SerialFd,TCSANOW,&g_newtio);
#endif
	return 1;
}

int local_cas_iruc_CloseSerialPort()
{
	tcsetattr(g_SerialFd,TCSANOW,&g_oldtio);
	close(g_SerialFd);
}

int local_cas_iruc_GetTSCChipRecord(TSC_CHIPRECORD *pstTscChipRecord)
{
	int	idx = 0;
	uc_bytes ucCSSN, ucPrivateData;
	uc_device_security_state	ucDeviceSecurity;
	uc_device_platform_identifiers	ucDevicePlatform;
	if (pstTscChipRecord == NULL)
		return 0;

	// ewlee 20100416 - getting serial number is not implemented yet
	pstTscChipRecord->nSTBSNLen = 14;
	pstTscChipRecord->pSTBSN = (unsigned char*) HLIB_STD_MemAlloc(14 * sizeof(unsigned char));
	for (idx = 0; idx < 14; idx++)
		pstTscChipRecord->pSTBSN[idx] = idx + 1;

	ucCSSN.length = IRUC_FLASH_SECURE_ID_SIZE;
	ucCSSN.bytes = (uc_byte*) HLIB_STD_MemAlloc(IRUC_FLASH_SECURE_ID_SIZE * sizeof(uc_byte));
	UniversalClientSPI_Device_GetCSSN(&ucCSSN);

	ucPrivateData.length = IRUC_FLASH_PRIVATEDATA_SIZE;
	ucPrivateData.bytes = (uc_byte*) HLIB_STD_MemAlloc(IRUC_FLASH_PRIVATEDATA_SIZE * sizeof(uc_byte));
	UniversalClientSPI_Device_GetPrivateData(&ucPrivateData);

	UniversalClient_GetLockId((uc_uint16*)&pstTscChipRecord->lockId);

	if (UniversalClientSPI_Device_GetSecurityState(&ucDeviceSecurity) == UC_ERROR_SUCCESS)
	{
		pstTscChipRecord->rsaMode = ucDeviceSecurity.rsaMode;
		pstTscChipRecord->jtag = ucDeviceSecurity.jtag;
		pstTscChipRecord->cwMode = ucDeviceSecurity.cwMode;
	}
	else
	{
		pstTscChipRecord->rsaMode = 55;
		pstTscChipRecord->jtag = 12;
		pstTscChipRecord->cwMode = 66;
	}

	if (UniversalClientSPI_Device_GetPlatformIdentifiers(&ucDevicePlatform) == UC_ERROR_SUCCESS)
	{
		pstTscChipRecord->hardwareVersion = ucDevicePlatform.hardwareVersion;
		pstTscChipRecord->keyVersion = ucDevicePlatform.keyVersion;
		pstTscChipRecord->loaderVersion = ucDevicePlatform.loaderVersion;
		pstTscChipRecord->manufacturerId = ucDevicePlatform.manufacturerId;
		pstTscChipRecord->signatureVersion = ucDevicePlatform.signatureVersion;
		pstTscChipRecord->systemId = ucDevicePlatform.systemId;
		pstTscChipRecord->variant = ucDevicePlatform.variant;
	}
	else
	{
		pstTscChipRecord->hardwareVersion = 11;
		pstTscChipRecord->keyVersion = 13;
		pstTscChipRecord->loaderVersion = 14;
		pstTscChipRecord->manufacturerId = 15;
		pstTscChipRecord->signatureVersion = 16;
		pstTscChipRecord->systemId = 17;
		pstTscChipRecord->variant = 18;
	}

	pstTscChipRecord->nCSSNLen = IRUC_FLASH_SECURE_ID_SIZE;
	pstTscChipRecord->pCSSN = (unsigned char*) HLIB_STD_MemAlloc(IRUC_FLASH_SECURE_ID_SIZE * sizeof(unsigned char));
	pstTscChipRecord->pCSSN[0] = ucCSSN.bytes[0];
	pstTscChipRecord->pCSSN[1] = ucCSSN.bytes[1];
	pstTscChipRecord->pCSSN[2] = ucCSSN.bytes[2];
	pstTscChipRecord->pCSSN[3] = ucCSSN.bytes[3];

	pstTscChipRecord->nPrivateDataLen = IRUC_FLASH_PRIVATEDATA_SIZE;
	pstTscChipRecord->pPrivateData = (unsigned char*) HLIB_STD_MemAlloc(IRUC_FLASH_PRIVATEDATA_SIZE * sizeof(unsigned char));
	memcpy(pstTscChipRecord->pPrivateData, ucPrivateData.bytes, IRUC_FLASH_PRIVATEDATA_SIZE);
//	for (idx = 0; idx < 15; idx++)
//		pstTscChipRecord->pPrivateData[idx] = idx+1;

	HLIB_STD_MemFree(ucCSSN.bytes);
	HLIB_STD_MemFree(ucPrivateData.bytes);

	return 1;
}

int local_cas_iruc_SetCSSK(uc_bytes *pucCSSK, char *pszCSSK, int nCSSKLen)
{
//	int			nRet = 1;
//	uc_result	ucRet = UC_ERROR_SUCCESS;

	if (pucCSSK == NULL || pszCSSK == NULL || nCSSKLen <= 0)
		return 0;

	pucCSSK->length = nCSSKLen;
	pucCSSK->bytes = (uc_byte*) HLIB_STD_MemAlloc(nCSSKLen * sizeof(uc_byte));
	memcpy(pucCSSK->bytes, pszCSSK, nCSSKLen);

	return 1;
//	ucRet = UniversalClientSPI_Device_SetCSSK(&ucCSSK);
//	if (ucRet != UC_ERROR_SUCCESS)
//		nRet = 0;

//	return nRet;
}

int local_cas_iruc_SetDescrambleKey(uc_key_info *pucKeyInfo,  char *pszKey, int nKeyLen)
{
//	uc_key_info	ucKeyInfo;
//	uc_bytes	ucKey;
//	int			nRet = 1;
//	uc_result	ucRet = UC_ERROR_SUCCESS;

	if (pucKeyInfo == NULL || pszKey == NULL || nKeyLen <= 0 || pucKeyInfo->pDescramblingKey == NULL)
		return 0;

	pucKeyInfo->pDescramblingKey->length = nKeyLen;
	pucKeyInfo->pDescramblingKey->bytes = (uc_byte*) HLIB_STD_MemAlloc(nKeyLen * sizeof(uc_byte));
	memcpy(pucKeyInfo->pDescramblingKey->bytes, pszKey, nKeyLen);
	pucKeyInfo->descramblingKeyAlgorithm = UC_DK_ALGORITHM_DVB_CSA;
	pucKeyInfo->descramblingKeyProtection = UC_DK_PROTECTION_TDES;

	return 1;

//	if (nType == 0)
//		ucRet = UniversalClientSPI_Stream_SetDescramblingKey(100, &ucKeyInfo, 0);
//	else if (nType == 1)
//		ucRet = UniversalClientSPI_Stream_SetDescramblingKey(100, &ucKeyInfo, 1);
//	else
//		nRet = 0;

//	if (ucRet != UC_ERROR_SUCCESS)
//		nRet = 0;

//	return nRet;
}

int local_cas_iruc_StartDescrambleTest(uc_bytes *pucCSSK, uc_key_info *pEvenKeyInfo, uc_key_info *pOddKeyInfo)
{
	uc_result	ucRet = UC_ERROR_SUCCESS;

	ucRet = UniversalClient_SetTestingKey(pucCSSK, pEvenKeyInfo, pOddKeyInfo);

	if (ucRet != UC_ERROR_SUCCESS)
		return 0;

	return 1;
}

int local_cas_iruc_SendResponseToPC(int fd, int nCmd, char *pszKey, int nKeyLen)
{
	int nLen = 0, idx = 0;
	char szData[1024];
	TSC_CHIPRECORD		stChipRecord;
	unsigned short usDescrambleResult = 0;
	if (fd <= 0)
		return 0;

	memset(szData, 0, 1024);
	szData[0] = HEADER_FIRST_BYTE;
	szData[1] = HEADER_SECOND_BYTE;
	szData[2] = HEADER_THIRD_BYTE;
	szData[3] = nCmd;

	switch (nCmd)
	{
	case RESPONSE_PING:
		break;
	case RESPONSE_TSC_CHIP_RECORD:
		local_cas_iruc_GetTSCChipRecord(&stChipRecord);
		nLen = (sizeof(unsigned short) * 11) + (sizeof(unsigned int) * 3) + stChipRecord.nSTBSNLen + stChipRecord.nCSSNLen + stChipRecord.nPrivateDataLen;
		szData[4] = (char) nLen;

		idx = 5;
		memcpy(&szData[idx], &stChipRecord.nSTBSNLen, sizeof(unsigned short));
		idx += sizeof(unsigned short);
		memcpy(&szData[idx], stChipRecord.pSTBSN, stChipRecord.nSTBSNLen);
		idx += stChipRecord.nSTBSNLen;
		memcpy(&szData[idx], &stChipRecord.nCSSNLen, sizeof(unsigned short));
		idx += sizeof(unsigned short);
		memcpy(&szData[idx], stChipRecord.pCSSN, stChipRecord.nCSSNLen);
		idx += stChipRecord.nCSSNLen;
		memcpy(&szData[idx], &stChipRecord.rsaMode, sizeof(unsigned int));
		idx += sizeof(unsigned int);
		memcpy(&szData[idx], &stChipRecord.jtag, sizeof(unsigned int));
		idx += sizeof(unsigned int);
		memcpy(&szData[idx], &stChipRecord.cwMode, sizeof(unsigned int));
		idx += sizeof(unsigned int);
		memcpy(&szData[idx], &stChipRecord.lockId, sizeof(unsigned short));
		idx += sizeof(unsigned short);
		memcpy(&szData[idx], &stChipRecord.systemId, sizeof(unsigned short));
		idx += sizeof(unsigned short);
		memcpy(&szData[idx], &stChipRecord.variant, sizeof(unsigned short));
		idx += sizeof(unsigned short);
		memcpy(&szData[idx], &stChipRecord.keyVersion, sizeof(unsigned short));
		idx += sizeof(unsigned short);
		memcpy(&szData[idx], &stChipRecord.signatureVersion, sizeof(unsigned short));
		idx += sizeof(unsigned short);
		memcpy(&szData[idx], &stChipRecord.manufacturerId, sizeof(unsigned short));
		idx += sizeof(unsigned short);
		memcpy(&szData[idx], &stChipRecord.loaderVersion, sizeof(unsigned short));
		idx += sizeof(unsigned short);
		memcpy(&szData[idx], &stChipRecord.hardwareVersion, sizeof(unsigned short));
		idx += sizeof(unsigned short);
		memcpy(&szData[idx], &stChipRecord.nPrivateDataLen, sizeof(unsigned short));
		idx += sizeof(unsigned short);
		memcpy(&szData[idx], stChipRecord.pPrivateData, stChipRecord.nPrivateDataLen);
		idx += stChipRecord.nPrivateDataLen;

		szData[5 + nLen] = local_cas_iruc_GetCheckSum((char*) (&szData[5]), nLen);
		write(fd, szData, 5 + nLen + 1);

		if (stChipRecord.pSTBSN != NULL)
			HLIB_STD_MemFree(stChipRecord.pSTBSN);
		if (stChipRecord.pCSSN != NULL)
			HLIB_STD_MemFree(stChipRecord.pCSSN);
		if (stChipRecord.pPrivateData != NULL)
			HLIB_STD_MemFree(stChipRecord.pPrivateData);
		break;
	case RECEIVE_CSSK:
	case RECEIVE_EVENCW:
	case RECEIVE_ODDCW:
		if (pszKey == NULL || nKeyLen <= 0)
			return 0;
		szData[4] = (char) nKeyLen;
		memcpy(&szData[5], pszKey, nKeyLen);
		szData[5 + nKeyLen] = local_cas_iruc_GetCheckSum((char*) (&szData[5]), nKeyLen);
		idx = write(fd, szData, 5 + nKeyLen + 1);
	//	memset(szData, 0, 1024);
	//	sprintf(szData, "Send Key data OK(%02X-%02d)\n", nCmd, idx);
	//	local_cas_iruc_SerialDataWrite(szData, g_nOffset, 24);
	//	g_nOffset += 24;
		break;
	case RESPONSE_START_TEST:
		nLen = sizeof(unsigned short);
		szData[4] = (char) nLen;

	//	if ( OxMGR_WTV_GetState(MGR_ACTION_GetMainActionId()) == 12 )
			usDescrambleResult = 1;
	//	else
	//		usDescrambleResult = 0;
		memcpy(&szData[5], &usDescrambleResult, sizeof(unsigned short));
		szData[5 + nLen] = local_cas_iruc_GetCheckSum((char*) (&szData[5]), nLen);
		idx = write(fd, szData, 5 + nLen + 1);
	//	memset(szData, 0, 1024);
	//	sprintf(szData, "Send result OK(%02d)\n", idx);
	//	local_cas_iruc_SerialDataWrite(szData, g_nOffset, 19);
	//	g_nOffset += 19;
		break;
	}

	return 1;
}

static void local_cas_iruc_SerialSendThread(void *args)
{
	int nIndex = 0, nLen;
	char checksum;

	uc_bytes cssk;
	uc_key_info evenDescramblingKeyInfo;
	uc_key_info oddDescramblingKeyInfo;
	uc_bytes evenCw;
	uc_bytes oddCw;

//	int	nIsEnd = 0;
	if (g_SerialFd < 0)
		return;

	HxLOG_Print("&&&&&&&&&&&&&&& Start local_cas_iruc_SerialSendThread\n");

//	local_cas_iruc_SerialDataWrite("Start Send thread\n", g_nOffset, 18);
//	g_nOffset += 18;

	memset(&cssk, 0, sizeof(uc_bytes));
	memset(&evenCw, 0, sizeof(uc_bytes));
	memset(&oddCw, 0, sizeof(uc_bytes));
	memset(&evenDescramblingKeyInfo, 0, sizeof(uc_key_info));
	memset(&oddDescramblingKeyInfo, 0, sizeof(uc_key_info));

	while (1)
	{
		if (g_nIsEnd == 1)
			break;
		if (g_nreadindex == g_nprocessindex)
		{
			usleep(500);
			continue;
		}

		nIndex = g_nprocessindex;
		VK_SEM_Get(g_SemID);
		if (g_readbuf[nIndex] == HEADER_FIRST_BYTE && g_readbuf[nIndex + 1] == HEADER_SECOND_BYTE && g_readbuf[nIndex + 2] == HEADER_THIRD_BYTE)
		{
			g_nprocessindex += 3;
			if (g_readbuf[g_nprocessindex] == REQUEST_TSC_CHIP_RECORD)
			{
				g_nprocessindex++;
				nLen = (int) (g_readbuf[g_nprocessindex++]);
				checksum = local_cas_iruc_GetCheckSum((char *) (&g_readbuf[g_nprocessindex]), nLen);
				if (checksum == g_readbuf[g_nprocessindex + nLen])
				{
					if (local_cas_iruc_SendResponseToPC(g_SerialFd, RESPONSE_TSC_CHIP_RECORD, NULL, 0) == 1)
						g_nprocessindex += nLen + 1;
				}
			}
			else if (g_readbuf[g_nprocessindex] == SEND_CSSK)
			{
				g_nprocessindex++;
				nLen = (int) (g_readbuf[g_nprocessindex++]);
				checksum = local_cas_iruc_GetCheckSum((char *) (&g_readbuf[g_nprocessindex]), nLen);
				if (checksum == g_readbuf[g_nprocessindex + nLen])
				{
					char	*pszKey;
					pszKey = (char*) HLIB_STD_MemAlloc(nLen * sizeof(char));
					memcpy(pszKey, &g_readbuf[g_nprocessindex], nLen);

					if (local_cas_iruc_SetCSSK(&cssk, pszKey, nLen) == 0)
					{
						char szData[1024];
						memset(szData, 0, 1024);
						sprintf(szData, "Failed to set CSSK\n");
						local_cas_iruc_SerialDataWrite(szData, g_nOffset, 19);
						g_nOffset += 19;
					}

					if (local_cas_iruc_SendResponseToPC(g_SerialFd, RECEIVE_CSSK, pszKey, nLen) == 1)
						g_nprocessindex += nLen + 1;
					HLIB_STD_MemFree(pszKey);
				}
			}
			else if (g_readbuf[g_nprocessindex] == SEND_EVENCW)
			{
				g_nprocessindex++;
				nLen = (int) (g_readbuf[g_nprocessindex++]);
				checksum = local_cas_iruc_GetCheckSum((char *) (&g_readbuf[g_nprocessindex]), nLen);
				if (checksum == g_readbuf[g_nprocessindex + nLen])
				{
					char	*pszKey;
					pszKey = (char*) HLIB_STD_MemAlloc(nLen * sizeof(char));
					memcpy(pszKey, &g_readbuf[g_nprocessindex], nLen);

					evenDescramblingKeyInfo.pDescramblingKey = &evenCw;
					if (local_cas_iruc_SetDescrambleKey(&evenDescramblingKeyInfo, pszKey, nLen) == 0)
					{
						char szData[1024];
						memset(szData, 0, 1024);
						sprintf(szData, "Failed to set even key\n");
						local_cas_iruc_SerialDataWrite(szData, g_nOffset, 23);
						g_nOffset += 23;
					}

					if (local_cas_iruc_SendResponseToPC(g_SerialFd, RECEIVE_EVENCW, pszKey, nLen) == 1)
						g_nprocessindex += nLen + 1;
					HLIB_STD_MemFree(pszKey);
				}
			}
			else if (g_readbuf[g_nprocessindex] == SEND_ODDCW)
			{
				g_nprocessindex++;
				nLen = (int) (g_readbuf[g_nprocessindex++]);
				checksum = local_cas_iruc_GetCheckSum((char *) (&g_readbuf[g_nprocessindex]), nLen);
				if (checksum == g_readbuf[g_nprocessindex + nLen])
				{
					char	*pszKey;
					pszKey = (char*) HLIB_STD_MemAlloc(nLen * sizeof(char));
					memcpy(pszKey, &g_readbuf[g_nprocessindex], nLen);

					oddDescramblingKeyInfo.pDescramblingKey = &oddCw;
					if (local_cas_iruc_SetDescrambleKey(&oddDescramblingKeyInfo, pszKey, nLen) == 0)
					{
						char szData[1024];
						memset(szData, 0, 1024);
						sprintf(szData, "Failed to set odd key\n");
						local_cas_iruc_SerialDataWrite(szData, g_nOffset, 22);
						g_nOffset += 22;
					}

					if (local_cas_iruc_SendResponseToPC(g_SerialFd, RECEIVE_ODDCW, pszKey, nLen) == 1)
						g_nprocessindex += nLen + 1;
					HLIB_STD_MemFree(pszKey);
				}
			}
			else if (g_readbuf[g_nprocessindex] == REQUEST_START_TEST)
			{
				g_nprocessindex++;
				nLen = (int) (g_readbuf[g_nprocessindex++]);
				checksum = local_cas_iruc_GetCheckSum((char *) (&g_readbuf[g_nprocessindex]), nLen);
				if (checksum == g_readbuf[g_nprocessindex + nLen])
				{
					if (local_cas_iruc_StartDescrambleTest(&cssk, &evenDescramblingKeyInfo, &oddDescramblingKeyInfo) == 1)
					{
						if (local_cas_iruc_SendResponseToPC(g_SerialFd, RESPONSE_START_TEST, NULL, 0) == 1)
							g_nprocessindex += nLen + 1;
					}
					else
					{
						char szData[1024];
						memset(szData, 0, 1024);
						sprintf(szData, "Failed to start test\n");
						local_cas_iruc_SerialDataWrite(szData, g_nOffset, 21);
						g_nOffset += 21;
					}
				}
			}
			else if (g_readbuf[g_nprocessindex] == END_PROTOCOL)
			{
				g_nprocessindex++;
				g_nIsEnd = 1;
			}
			VK_SEM_Release(g_SemID);
		}
		else
		{
			g_nprocessindex++;
			VK_SEM_Release(g_SemID);
			continue;
		}
	}
//	VK_TASK_Destroy(s_ReceiveId);
//	VK_SEM_Destroy(g_SemID);
	usleep(100);
	local_cas_iruc_CloseSerialPort();

	if (cssk.bytes != NULL)
		HLIB_STD_MemFree(cssk.bytes);
	if (evenCw.bytes != NULL)
		HLIB_STD_MemFree(evenCw.bytes);
	if (oddCw.bytes != NULL)
		HLIB_STD_MemFree(oddCw.bytes);
	HxLOG_Print("&&&&&&&&&&&&&&& End local_cas_iruc_SerialSendThread\n");

//	local_cas_iruc_SerialDataWrite("End Send thread\n", g_nOffset, 16);
//	g_nOffset += 16;
}

static void local_cas_iruc_SerialReceiveThread(void *args)
{
#if defined(ASYNC_SERIAL_COMM)
	int nResult = 0, offset = 0;
	char	readbuf[1024];
	if (g_SerialFd < 0)
		return;

	HxLOG_Print("&&&&&&&&&&&&&&& Start local_cas_iruc_SerialReceiveThread\n");

	while (1)
	{
		if (g_WaitFlag == 0)
		{
			memset(readbuf, 0, 1024);
			VK_SEM_Get(g_SemID);
			nResult = read(g_SerialFd, readbuf, 1024);
			if (nResult > 0)
			{
				local_cas_iruc_SerialDataWrite(readbuf, offset, nResult);
				offset += nResult;
				if ( (g_nreadindex + nResult) >= 4096)
				{
					g_nreadindex = g_nprocessindex = 0;
					memcpy(&g_readbuf[g_nreadindex], readbuf, nResult);
					g_nreadindex = nResult - 1;
				}
				else
				{
					memcpy(&g_readbuf[g_nreadindex], readbuf, nResult);
					g_nreadindex += nResult;
				}
			}
			VK_SEM_Release(g_SemID);
			g_WaitFlag = 1;
		}
		usleep(500);
	}
#else
	int nResult = 0, idx;
	char	readbuf[1024], temp[2048];
	int	nPos = 0, nRetLen = 0;
	if (g_SerialFd < 0)
		return;

	HxLOG_Print("&&&&&&&&&&&&&&& Start local_cas_iruc_SerialReceiveThread\n");

//	local_cas_iruc_SerialDataWrite("Start Receive thread\n", g_nOffset, 21);
//	g_nOffset += 21;

	while (1)
	{
		memset(readbuf, 0, 1024);
		nResult = 0;
		if (g_nIsEnd == 1)
			break;
		VK_SEM_Get(g_SemID);
		if (g_nIsEnd == 1)
		{
			VK_SEM_Release(g_SemID);
		//	VK_SEM_Destroy(g_SemID);
			break;
		}
		nResult = read(g_SerialFd, readbuf, 1024);
		if (g_nIsEnd == 1)
		{
			VK_SEM_Release(g_SemID);
		//	VK_SEM_Destroy(g_SemID);
			break;
		}
		if (nResult > 0)
		{
		//	local_cas_iruc_SerialDataWrite(readbuf, g_nOffset, nResult);
		//	g_nOffset += nResult;
		//	memset(temp, 0, 2048);
		//	nPos= nRetLen = 0;
		//	for (idx = 0; idx < nResult; idx++)
		//	{
		//		nRetLen = sprintf(temp + nPos, "0x%02X ", readbuf[idx]);
		//		nPos += nRetLen;
		//	}
		//	temp[nPos] = '\n';
		//	local_cas_iruc_SerialDataWrite(temp, g_nOffset, nPos + 1);
		//	g_nOffset += nPos + 1;
			if ( (g_nreadindex + nResult) >= 4096)
			{
				g_nreadindex = g_nprocessindex = 0;
				memcpy(&g_readbuf[g_nreadindex], readbuf, nResult);
				g_nreadindex = nResult - 1;
			}
			else
			{
				memcpy(&g_readbuf[g_nreadindex], readbuf, nResult);
				g_nreadindex += nResult;
			}
			nResult = read(g_SerialFd, readbuf, 1024);
			if (g_nIsEnd == 1)
			{
				VK_SEM_Release(g_SemID);
			//	VK_SEM_Destroy(g_SemID);
				break;
			}
			if (nResult > 0)
			{
			//	local_cas_iruc_SerialDataWrite(readbuf, g_nOffset, nResult);
			//	g_nOffset += nResult;
			//	memset(temp, 0, 2048);
			//	nPos = nRetLen = 0;
			//	for (idx = 0; idx < nResult; idx++)
			//	{
			//		nRetLen = sprintf(temp + nPos, "0x%02X ", readbuf[idx]);
			//		nPos += nRetLen;
			//	}
			//	temp[nPos] = '\n';
			//	local_cas_iruc_SerialDataWrite(temp, g_nOffset, nPos + 1);
			//	g_nOffset += nPos + 1;
				if ( (g_nreadindex + nResult) >= 4096)
				{
					g_nreadindex = g_nprocessindex = 0;
					memcpy(&g_readbuf[g_nreadindex], readbuf, nResult);
					g_nreadindex = nResult - 1;
				}
				else
				{
					memcpy(&g_readbuf[g_nreadindex], readbuf, nResult);
					g_nreadindex += nResult;
				}
			}
		}
		VK_SEM_Release(g_SemID);
		usleep(500);
	}
	g_nIsEnd = 0;
#endif
	VK_SEM_Destroy(g_SemID);
	usleep(500);
//	local_cas_iruc_SerialDataWrite("End Receive thread\n", g_nOffset, 19);
//	g_nOffset += 19;
	HxLOG_Print("&&&&&&&&&&&&&&& End local_cas_iruc_SerialReceiveThread\n");
}
#endif	// IRUC_SERIAL_COMMUNICATION

HUINT32 CAS_IRUC_MGR_Notify_InstanceChanged(HUINT16 usSlotId, HUINT16 *pusCaSysId, HUINT8 ucNumCaSysId, HBOOL bAvailable)
{
#if (CONFIG_MW_CAS_NUM_EMBEDDED > 1)
	HUINT32 i;
#endif
	HUINT32	ret;

	HxLOG_Info("ucNumCaSysId=0x%d\n", ucNumCaSysId);

#if (CONFIG_MW_CAS_NUM_EMBEDDED <= 1)
	s_IrScInst.usSlotId= usSlotId;
  #if defined(IR_SC_IN_OUT_NOTI_TO_CASMGR)
	s_IrScInst.usNumCaSysId= ucNumCaSysId;	/* Cas Mgr에서는 이 값이 0이면 sc 빠진 것으로 처리한다. 이를 고려하기 위함. */
  #else
	s_IrScInst.usNumCaSysId= 1;
  #endif  // #if defined(IR_SC_IN_OUT_NOTI_TO_CASMGR)
	s_IrScInst.ausCaSysId[0] = 0x600;
	bAvailable = TRUE;
#else
	s_IrScInst.usSlotId= usSlotId;
	s_IrScInst.usNumCaSysId= ucNumCaSysId;

	if(pusCaSysId == NULL || ucNumCaSysId == 0)
	{//card removed
		for(i=0; i<MAX_SC_SECTOR_COUNT; i++ )
		{
			s_IrScInst.ausCaSysId[i] = 0 ;
		}
//		bAvailable = FALSE;
	}
	else	//card inserted
	{
		for(i=0; i<ucNumCaSysId; i++ )
		{
			// TODO: CA instance  버그 수정 전 가지 마스킹 사용 할 것
			#if 0 //good
			s_IrScInst.ausCaSysId[i] = pusCaSysId[i] ;
			#else// bad
			s_IrScInst.ausCaSysId[i] = pusCaSysId[i] & 0xFF00 ;
			#endif
		}
//		bAvailable = TRUE;
	}
#endif
	ret = svc_cas_MgrSubUpdateCasInfo(	s_IrScInst.hCasDevice,
								eDxCAS_GROUPID_IR,
								s_IrScInst.usSlotId,
								s_IrScInst.ausCaSysId,
								s_IrScInst.usNumCaSysId,
								IR_HUMAX_MAX_SERVICE,
								 bAvailable
								);

	if(ret != ERR_OK)
	{
		HxLOG_Print("\tERROR CAS_IR_MGR_Notify_InstanceChanged ret=0x%x\n", ret);
		return ret;
	}

	return ERR_OK;
}

HERROR xsvc_cas_IrUcMgrInit(void)
{
#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
	HINT32	nItemValue;
	HERROR	hErr = ERR_OK;
#endif
	HxLOG_Print("\n");

//	HxSTD_memset(&s_IrUcScInst, 0, sizeof(IRUC_ScInst_t) );//* HUMAX_VD_SC_SLOT_COUNT);

	/*step2 : IR DRV init */
	/*Create CA TASK and UC init */

	CAS_IRUC_DRV_Init();

	/*Create UCCA init */
	CAS_IRUC_Client_Init();

	//step3. Session manager Init
	local_cas_iruc_Mgr_InitModule();

	//step 4. register ctrl list
	CA_IR_RegisterCtrlList();

	CAS_IRUC_MGR_Notify_InstanceChanged(0, NULL, 0, 0);

#if defined(IRUC_SERIAL_COMMUNICATION)
	// ewlee 20100317 - serial test
	HxLOG_Error("&&&&&&&&&&&&&&& Start to test serial port\n");
//	SerialTest();
	if (local_cas_iruc_OpenSerialPort() > 0)
//	if (0)
	{
		char task_name[32];
//		HUINT32	task_id;
		HUINT32	res = VK_ERROR;
		memset(task_name, 0, 32);

	//	local_cas_iruc_SerialDataWrite("Open serial port\n", g_nOffset, 17);
	//	g_nOffset += 17;

		res = VK_SEM_CreateWithCount(&g_SemID, 1, "Serial_Sem", VK_SUSPENDTYPE_FIFO);
		if (res != VK_OK)
		{
			local_cas_iruc_CloseSerialPort();
			HxLOG_Print("@@@@@@@@ Failed to create semaphore\n");
			return;
		}
		sprintf(task_name, "SERIAL_RECEIVE" );
		res = VK_TASK_Create( (void *) local_cas_iruc_SerialReceiveThread, (MW_CAS_TASK_PRIORITY+3), MW_CAS_TASK_STACK_SIZE, task_name, NULL, (unsigned long *)&s_ReceiveId, 0);
		if (res == VK_OK)
			VK_TASK_Start(s_ReceiveId);

		sprintf(task_name, "SERIAL_SEND" );
		res = VK_TASK_Create( (void *) local_cas_iruc_SerialSendThread, (MW_CAS_TASK_PRIORITY+3), MW_CAS_TASK_STACK_SIZE, task_name, NULL, (unsigned long *)&s_SendId, 0);
		if (res == VK_OK)
			VK_TASK_Start(s_SendId);
	}
//	else
//	{
//		local_cas_iruc_SerialDataWrite("Failed to open serial port\n", g_nOffset, 27);
//		g_nOffset += 27;
//	}
	HxLOG_Error("&&&&&&&&&&&&&&& Stop to test serial port\n");
#endif

#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_VIRTUAL_SECURECW_SETTING, (HUINT32 *)&nItemValue, 0);
	if(hErr != ERR_OK)	HxLOG_Error(" Menu Item Value Error t\n");

	CAS_IR_SECURECHIP_SetVirtualCWMode(nItemValue);
#endif
	return ERR_OK;
}

static HERROR local_cas_iruc_Mgr_InitModule(void)
{
	HERROR nResult = ERR_FAIL;
	SvcCas_CbFuncList_t cbList;
	Handle_t hCasDevice = HANDLE_NULL;

	HxLOG_Print("\r\n");

	HxSTD_memset(&cbList, 0x00, sizeof(SvcCas_CbFuncList_t));
	cbList.pfnCbStartSession = local_cas_iruc_Mgr_StartService;
	cbList.pfnCbStopSession = local_cas_iruc_Mgr_StopService;
//	cbList.pfnCbPauseSession = NULL;
//	cbList.pfnCbResumeSession = NULL;
	cbList.pfnCbSetDSC = local_cas_iruc_Mgr_SetPID;

	cbList.pfnCbInstSelected = local_cas_iruc_Mgr_IamSelected;

	cbList.pfnCbUpdatePmt = local_cas_iruc_Mgr_UpdatePmt;
	cbList.pfnCbUpdateCat = local_cas_iruc_Mgr_UpdateCat;

	nResult = svc_cas_MgrSubRegisterCb(eCasInst_Embedded, eDxCAS_GROUPID_IR, 0, &cbList, 1, &hCasDevice);
	if (nResult != ERR_OK)
	{
		HxLOG_Print("\n[%s:%d]\n",__FUNCTION__,__LINE__);
		return ERR_FAIL;
	}

//	s_IrUcScInst.hCasDevice = hCasDevice; // Instance Table Index
	HxLOG_Print("hCasDevice (0x%X)\n", hCasDevice);

	return ERR_OK;
}


#define ___LOCAL_SESSION_MANAGE_________
static void local_cas_iruc_Mgr_GenerateIrSessionHandle(Handle_t in_hAction, Handle_t *out_pIrUcSessionHandle)
{
	SvcCas_Context_t *pContext=NULL;

	/*It can be changed with pure Action ID */
	pContext = CAS_IRUC_MGR_GetContextByAction(in_hAction);
	if (in_hAction == HANDLE_NULL || pContext == NULL)
	{
		HxLOG_Print(" Handle is invalid\n");
	}


	if(pContext)
	{
	      *out_pIrUcSessionHandle = (in_hAction & 0xFF000000) | GET_SERVICE_ID(pContext->hSvc) ;
    }
}

static void local_cas_iruc_Mgr_StartService(Handle_t hAction, Handle_t hSvc, SvcCas_ActionType_e eActType)	// Action Type 전달 해준다.
{
	CAS_CAI_IF_msgPID *pMsg;
	HUINT32		i;

	HxLOG_Print("[%s] hAction=0x%X, Svc=0x%X, eActType=%d)\n",__FUNCTION__, hAction, hSvc, eActType);

	pMsg = (CAS_CAI_IF_msgPID *)OxCAS_Malloc(sizeof(CAS_CAI_IF_msgPID));
	if (pMsg == NULL)
	{
		HxLOG_Print("\n[%s:%d]\n",__FUNCTION__,__LINE__);
		return;
	}

	local_cas_iruc_Mgr_GenerateIrSessionHandle(hAction, &(pMsg->hIrSvc) );
	pMsg->hAct= hAction;
	pMsg->hSvc= hSvc;
	pMsg->eActType= eActType;

	for(i=0; i<eIR_COMPONENT_MAX; i++)
	{
		pMsg->usPID_list[i] = IRDRV_NULL_PID;
	}

	local_cas_iruc_SendMsg(eIR_MSG_CLASS_CAI, CASMGR_CAI_START_SERVICE, (void *)pMsg);

}

static void local_cas_iruc_Mgr_StopService(Handle_t hAction, Handle_t hNextSvc)
{
	HUINT32 i;
	CAS_CAI_IF_msgPID *pPidMsg;
	CAS_CAI_IF_msgTS *pTsMsg;
	DbSvc_TsInfo_t stTsInfo;


	HxLOG_Print("hAction=0x%X, hNextSvc=0x%X)\n", hAction, hNextSvc);

	pPidMsg = (CAS_CAI_IF_msgPID *)OxCAS_Malloc(sizeof(CAS_CAI_IF_msgPID));
	if (pPidMsg == NULL)
	{
		HxLOG_Print("\n[%s:%d]\n",__FUNCTION__,__LINE__);
		return;
	}

 	local_cas_iruc_Mgr_GenerateIrSessionHandle(hAction, &(pPidMsg->hIrSvc) );

	/*Stop TS */
	// TP 전환 혹은 service 완전 stop시 EMM filtering stop
	// 1.service 완전 stop (menu 진입, standby 진입 등)
	if( hNextSvc == (Handle_t) NULL)
	{
		pPidMsg->usTsUniqueId= INVALID_TS_UNIQUE_ID;
		pPidMsg->hSvc	= HANDLE_NULL;
	}
	else if (DB_SVC_GetTsInfoByServiceHandle(hNextSvc, &stTsInfo) == ERR_OK)
	{
		pPidMsg->usTsUniqueId = stTsInfo.usUniqueId;
		pPidMsg->hSvc	= HANDLE_NULL; /* no meaning */
	}
	else
	{
		// TODO: DB에 기술되지 않은 서비스, Ir_drv로 일단 넣는다.
		pPidMsg->usTsUniqueId= INVALID_TS_UNIQUE_ID;
		pPidMsg->hSvc	= HANDLE_NULL;
		// jichoi HxLOG_Print("\n[%s:%d]\n",__FUNCTION__,__LINE__);
	}

	/*Stop Service */
	for(i=0; i<eIR_COMPONENT_MAX; i++)
	{
		pPidMsg->usPID_list[i] = IRDRV_NULL_PID;
	}

	local_cas_iruc_SendMsg(eIR_MSG_CLASS_CAI, CASMGR_CAI_STOP_SERVICE, (void *)pPidMsg);

	// tp management.
	pTsMsg = (CAS_CAI_IF_msgTS *)OxCAS_Malloc(sizeof(CAS_CAI_IF_msgTS));

	if(hNextSvc == (Handle_t )NULL)
	{
		pTsMsg->hAction = hAction;
		pTsMsg->usTsUniqueId = INVALID_TS_UNIQUE_ID;
	}
	else
	{
		if (DB_SVC_GetTsInfoByServiceHandle(hNextSvc, &stTsInfo) == ERR_OK)
		{
			pTsMsg->hAction = hAction;
			pTsMsg->usTsUniqueId = stTsInfo.usUniqueId;
		}
		else
		{
			HxLOG_Print("DB_SVC_GetTsInfoByServiceHandle, Cannot find TsInfo ");

			// TODO: 일단 현재의 TP를 cut 한다.
			pTsMsg->hAction = hAction;
			pTsMsg->usTsUniqueId = INVALID_TS_UNIQUE_ID;
		}
	}
	local_cas_iruc_SendMsg(eIR_MSG_CLASS_CAI, CASMGR_CAI_STOP_TS, (void *)pTsMsg);
}


static void local_cas_iruc_Mgr_IamSelected(Handle_t hAction, HUINT32 ulCasInstanceId, HBOOL bConnect)
{
	CAS_CAI_IF_msgCAINST_SELECT	*pMsg;

	HxLOG_Print("hAction=0x%X, ulCasInstanceId=0x%X, bConnect=%d)\n", hAction, ulCasInstanceId, bConnect);

	pMsg = (CAS_CAI_IF_msgCAINST_SELECT *)OxCAS_Malloc(sizeof(CAS_CAI_IF_msgCAINST_SELECT));
	if (pMsg == NULL)
	{
		HxLOG_Print("\n[%s:%d]\n",__FUNCTION__,__LINE__);
		return;
	}

  	local_cas_iruc_Mgr_GenerateIrSessionHandle(hAction, &(pMsg->hIrSvc) );

	pMsg->ulCasInstanceId = ulCasInstanceId;
	pMsg->IsSelect = bConnect;

	local_cas_iruc_SendMsg(eIR_MSG_CLASS_CAI, CASMGR_CAINST_SELECTDESELECT, (void *)pMsg);

}

static void local_cas_iruc_Mgr_SetPID(Handle_t hAction, SvcCas_SetPidInfo_t *pstCaDscInfo)
{
	HUINT8	i;
	CAS_CAI_IF_msgPIDList *pMsg;

	HxLOG_Print("hAction=0x%X, pstCaDscInfo=0x%X\n", hAction, (HUINT32) pstCaDscInfo );

#if defined(CONFIG_DEBUG)
	HxLOG_Print("pstCaDscInfo->ulPidNum = %d\n", pstCaDscInfo->ulPidNum);

	for(i=0; i<pstCaDscInfo->ulPidNum; i++)
	{
		HxLOG_Print("[%s] ES type= %d, PID= 0x%X\n",__FUNCTION__, pstCaDscInfo->stPidInfo[i].esType, pstCaDscInfo->stPidInfo[i].usPid);
	}
#endif

	pMsg = (CAS_CAI_IF_msgPIDList *)OxCAS_Malloc(sizeof(CAS_CAI_IF_msgPIDList));
	if (pMsg == NULL)
	{
		HxLOG_Print("\n[%s:%d]\n",__FUNCTION__,__LINE__);
		return;
	}

  	local_cas_iruc_Mgr_GenerateIrSessionHandle(hAction, &(pMsg->hIrSvc) );

	for( i = 0; i < IR_SVC_MAX_PID; i++ )
	{
		pMsg->astPidInfo[i].ucType = eEsType_None;
		pMsg->astPidInfo[i].usPID = IRDRV_NULL_PID;
	}

	pMsg->ulPidInfoCnt = pstCaDscInfo->ulPidNum;

	for( i=0; i<pstCaDscInfo->ulPidNum ; i++)
	{
		pMsg->astPidInfo[i].usPID = pstCaDscInfo->stPidInfo[i].usPid;
		pMsg->astPidInfo[i].ucType= pstCaDscInfo->stPidInfo[i].esType;

	}

	local_cas_iruc_SendMsg(eIR_MSG_CLASS_CAI, CASMGR_CAI_SET_PID, (void *)pMsg);

}

static void local_cas_iruc_Mgr_UpdatePmt(Handle_t hAction, HUINT8 *pucPmt)
{
	CAS_CAI_IF_msgPMT *pMsg;
	HUINT16			usPmtLen;

	usPmtLen = get12bit(pucPmt + 1) + 3;

	HxLOG_Print("hAction=0x%X, pucPmt= 0x%X, usPmtLen=%d\n", hAction, (HUINT32) pucPmt, usPmtLen);

	pMsg = (CAS_CAI_IF_msgPMT *)OxCAS_Malloc(sizeof(CAS_CAI_IF_msgPMT));
	if (pMsg == NULL)
	{
		HxLOG_Print("\n[%s:%d]\n",__FUNCTION__,__LINE__);
		return;
	}

 	local_cas_iruc_Mgr_GenerateIrSessionHandle(hAction, &(pMsg->hIrSvc) );

	pMsg->usLength = usPmtLen;

	// Message받아서 처리하는 부분에서 FREE하기.. !!!!
	pMsg->pRawData = (HUINT8 *)OxCAS_Malloc(usPmtLen);
	if (pMsg->pRawData == NULL)
	{
		HxLOG_Print("\n[%s:%d]\n",__FUNCTION__,__LINE__);
		OxCAS_Free(pMsg);
		pMsg = NULL;
		return;
	}
	else
	{
		HxSTD_memcpy(pMsg->pRawData, pucPmt, usPmtLen);
		local_cas_iruc_SendMsg(eIR_MSG_CLASS_CAI, CASMGR_SI_PMT_UPDATE, (void *)pMsg);
	}
}
static void local_cas_iruc_Mgr_UpdateCat(Handle_t hAction, HUINT16 usUniqueId, HUINT8 *pucCat)
{
	CAS_CAI_IF_msgCAT *pMsg;
	HUINT16 	usCatLen;

	usCatLen = get12bit(pucCat + 1) + 3;

	HxLOG_Print("hAction=0x%X, usUniqueId= 0x%X,pucCat=%X\n", hAction, usUniqueId,(HUINT32) pucCat );

	pMsg = (CAS_CAI_IF_msgCAT *)OxCAS_Malloc(sizeof(CAS_CAI_IF_msgCAT));
	if (pMsg == NULL)
	{
		HxLOG_Print("\n[%s:%d]\n",__FUNCTION__,__LINE__);
		return;
	}

	pMsg->hAction= hAction;
	pMsg->usTsUniqueId = usUniqueId;
	pMsg->usLength = usCatLen;

	// Message받아서 처리하는 부분에서 FREE하기.. !!!!
	pMsg->pRawData = (HUINT8 *)OxCAS_Malloc(usCatLen);
	if (pMsg->pRawData == NULL)
	{
		HxLOG_Print("\n[%s:%d]\n",__FUNCTION__,__LINE__);
		OxCAS_Free(pMsg);
		pMsg = NULL;
		return;
	}

	HxSTD_memcpy(pMsg->pRawData, pucCat, usCatLen);
	local_cas_iruc_SendMsg(eIR_MSG_CLASS_CAI, CASMGR_SI_CAT_UPDATE, (void *)pMsg);

}


#define ___API_FOR_IRUC_DRV___________________________

/********************************************************************
*	function name :	 local_cas_iruc_MGR_Notify_InstanceChanged
*	input : void(None)
*	output : None
*	description : blah blah blah
********************************************************************/
#if 0
HUINT32 local_cas_iruc_MGR_Notify_InstanceChanged(HUINT16 usSlotId, HUINT16 *pusCaSysId, HUINT8 ucNumCaSysId, HBOOL bAvailable)
{
	HUINT32 i;
	HUINT32	ret;

	HxLOG_Print("ucNumCaSysId=0x%d\n", ucNumCaSysId);

#if (CONFIG_MW_CAS_NUM_EMBEDDED <= 1)
	s_IrUcScInst.usSlotId= usSlotId;
	s_IrUcScInst.usNumCaSysId= 1;
	s_IrUcScInst.ausCaSysId[0] = 0x600;
	bAvailable = TRUE;
#else
	s_IrUcScInst.usSlotId= usSlotId;
	s_IrUcScInst.usNumCaSysId= ucNumCaSysId;

	if(pusCaSysId == NULL || ucNumCaSysId == 0)
	{//card removed
		for(i=0; i<MAX_SC_SECTOR_COUNT; i++ )
		{
			s_IrUcScInst.ausCaSysId[i] = 0 ;
		}
//		bAvailable = FALSE;
	}
	else	//card inserted
	{
		for(i=0; i<ucNumCaSysId; i++ )
		{
			// TODO: CA instance  버그 수정 전 가지 마스킹 사용 할 것
			#if 0 //good
			s_IrUcScInst.ausCaSysId[i] = pusCaSysId[i] ;
			#else// bad
			s_IrUcScInst.ausCaSysId[i] = pusCaSysId[i] & 0xFF00 ;
			#endif
		}
//		bAvailable = TRUE;
	}
#endif
	ret = svc_cas_MgrSubUpdateCasInfo(	s_IrUcScInst.hCasDevice,
								eDxCAS_GROUPID_IR,
								s_IrUcScInst.usSlotId,
								s_IrUcScInst.ausCaSysId,
								s_IrUcScInst.usNumCaSysId,
								IR_HUMAX_MAX_SERVICE,
								 bAvailable
								);

	if(ret != ERR_OK)
	{
		HxLOG_Error("\tERROR local_cas_iruc_MGR_Notify_InstanceChanged ret=0x%x\n", ret);
		return ret;
	}

	return ERR_OK;
}
#endif

void local_cas_iruc_SendMsg(HUINT16 msg_channel, HUINT16 msg_type, void* pMsg)
{
#ifdef UC_MGR_CONFIG_NO_CA_TASK
// TODO: Must be used with MUTEX
	HMSG		msg;

	HxLOG_Print("msg channel(0x%X) : type(0x%X)\n", (HUINT32) msg_channel, (HUINT32)msg_type );

	msg.usMsgClass	= msg_channel;
	msg.usParam		= msg_type;
	msg.ulParam		= (HUINT32)pMsg;

	CAS_IRUC_Main_ProcessMsg( &msg);
#else
#error "Not implemeted task "
//	VK_MSG_Send( s_ulCaTaskQId, &msg, sizeof(HMSG) );

	HMSG		msg;

	HxLOG_Print("msg channel(0x%X) : type(0x%X)\n", (HUINT32) msg_channel, (HUINT32)msg_type );

	msg.usMsgClass	= msg_channel;
	msg.usParam		= msg_type;
	msg.ulParam		= (HUINT32)pMsg;

#endif

}

static SvcCas_Context_t * local_cas_iruc_Mgr_GetContextByAction(Handle_t hAction)
{
	HUINT32 ulActionId = 0;

	if (hAction == HANDLE_NULL)
	{
		HxLOG_Error("handle is null. \n");
		return (SvcCas_Context_t *)NULL;
	}

	ulActionId = svc_cas_SubSvcRmGetActionId(hAction);
	if(ulActionId >= CAS_MAX_NUM_OF_ACTION)
	{
		HxLOG_Error("action id is wrong. \n");
		return (SvcCas_Context_t *)NULL;
	}

	return &s_astCasIrContext[ulActionId];

}

SvcCas_Context_t * CAS_IRUC_MGR_GetContextByAction(Handle_t hAction)
{
	return local_cas_iruc_Mgr_GetContextByAction(hAction);
}

#if defined(CONFIG_DEBUG)
static int local_cas_iruc_RunVM(void *szArgList)
{
#if defined(CONFIG_MW_CAS_IRDETO_UCAS_VM_VAILDATOR)
	HCHAR	*pszArg;
	HUINT32 ulCount;

	pszArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if(pszArg == NULL)
	{
		HxLOG_Error("pszArg NULL\n");
		return HxCMD_ERR;
	}

	ulCount = HLIB_STD_StrToINT32(pszArg);
	if(ulCount == 0 || ulCount > 100)
	{
		ulCount = 100;
	}

	HxLOG_Warning(HxANSI_COLOR_YELLOW("##################################")"\n");
	HxLOG_Warning(HxANSI_COLOR_YELLOW("####### VM VAILDATOR START #######")"\n");
	HxLOG_Warning(HxANSI_COLOR_YELLOW("##################################")"\n");

	Run(CAS_JFFS2_PATH"/irUC/SecureCore.bin", ulCount);

	HxLOG_Warning(HxANSI_COLOR_YELLOW("##################################")"\n");
	HxLOG_Warning(HxANSI_COLOR_YELLOW("######## VM VAILDATOR END ########")"\n");
	HxLOG_Warning(HxANSI_COLOR_YELLOW("##################################")"\n");
#else
	HxLOG_Error(HxANSI_COLOR_YELLOW("need to enable CONFIG_MW_CAS_IRDETO_UCAS_VM_VAILDATOR")"\n");
	HxLOG_Error(HxANSI_COLOR_YELLOW("need to enable CONFIG_MW_CAS_IRDETO_UCAS_VM_VAILDATOR")"\n");
	HxLOG_Error(HxANSI_COLOR_YELLOW("need to enable CONFIG_MW_CAS_IRDETO_UCAS_VM_VAILDATOR")"\n");
#endif

	return 0;
}

static int local_cas_iruc_Cipher(void *szArgList)
{
#if (0)
#define IRUC_TEST_BUFFER_SIZE	256
	HCHAR *pszArg = NULL;
	HUINT8 aucKey[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
	HUINT8 aucPlain[IRUC_TEST_BUFFER_SIZE] = {0,};
	HUINT8 aucEncrypt[IRUC_TEST_BUFFER_SIZE] = {0,};
	HUINT8 aucDecrypt[IRUC_TEST_BUFFER_SIZE] = {0,};
	SvcCas_Crypt_t stCrypt;
	HINT32 i;

	pszArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if(pszArg == NULL)
	{
		HxLOG_Error("pszArg NULL\n");
		return HxCMD_ERR;
	}

	HLIB_STD_StrLower(pszArg);

	if(HxSTD_StrCmp(pszArg, "syskey") == 0)
	{
		stCrypt.etCryptKey = eCAS_CRYPT_DEFAULT_SYSTEM_KEY;
		stCrypt.pKey = NULL;
		stCrypt.ulKeySize = 0;
	}
	else if(HxSTD_StrCmp(pszArg, "userkey") == 0)
	{
		stCrypt.etCryptKey = eCAS_CRYPT_USER_KEY;
		stCrypt.pKey = aucKey;
		stCrypt.ulKeySize = 16;
	}
	else
	{
		HxLOG_Error("Unknown cipher keytype.\n");
		return HxCMD_ERR;
	}

	for(i = 0 ; i < IRUC_TEST_BUFFER_SIZE ; i++)
	{
		aucPlain[i] = i;
	}

	stCrypt.etCryptMode = eCAS_CRYPT_AES;
	stCrypt.ulBuffSize = IRUC_TEST_BUFFER_SIZE;

	stCrypt.pSrcBuffer = aucPlain;
	stCrypt.pDstBuffer = aucEncrypt;

	if(svc_cas_PalPortSysEncryptData(&stCrypt) != ERR_OK)
	{
		HxLOG_Error("svc_cas_DevPortSysEncrypt() error\n");
		return HxCMD_ERR;
	}

	stCrypt.pSrcBuffer = aucEncrypt;
	stCrypt.pDstBuffer = aucDecrypt;

	if(svc_cas_DevPortSysDecryptData(&stCrypt) != ERR_OK)
	{
		HxLOG_Error("svc_cas_DevPortSysDecrypt() error\n");
		return HxCMD_ERR;
	}

	if(HxSTD_memcmp(aucPlain, aucDecrypt, IRUC_TEST_BUFFER_SIZE) != 0)
	{
		HxLOG_Error("compare error.\n");
		return HxCMD_ERR;
	}

	HxLOG_Warning("success.\n");
#endif
	return HxCMD_OK;
}

void CMD_Register_SVC_CAS_IrdetoCmd(HxCMD_Handle_t hCasCmdHandle)
{
	HLIB_CMD_RegisterWord(hCasCmdHandle,
						local_cas_iruc_RunVM,
						"runVM",						/* keyword */
						"command : runVM [COUNT]",	/* help   */
						"runVM");	/* usage  */

	HLIB_CMD_RegisterWord(hCasCmdHandle,
						local_cas_iruc_Cipher,
						"cipher",						/* keyword */
						"command : cipher",	/* help   */
						"cipher");	/* usage  */
}
#endif


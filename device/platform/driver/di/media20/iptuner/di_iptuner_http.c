/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_http.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:	SungYong Jeon
 * Current Author:	$Author: junsy@humaxdigital.com $
 * Date:			$Date:   Tue May 20 21:49:01 KST 2009 $
 * File Description:	     HTTP DNP Receiver & HTTP VFIO for Streaming
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2009 HUMAX Co., Ltd. 								*/
/* All rights reserved.												*/
/*******************************************************************/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* Start Including Header Files */

#include "di_iptuner_config.h"

#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <inttypes.h>	 /** for 64bit MACRO ex>PRId64 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "htype.h"
#include "di_uart.h"

#include "vkernel.h"
#include "taskdef.h"

#include "di_iptuner20.h"
#include "di_iptuner_dnp.h"
#include "di_iptuner_pool.h"
#include "di_iptuner_http.h"
#include "di_iptuner_underflow.h"

#include "util_sys.h"

#include "vfio20.h"
#include "stream_http.h"
#include "adp_buffermgr.h"

#include "drv_drm.h"

#include "bfile_types.h"
#ifndef CONFIG_PRODUCT_IMAGE_HWTEST
#include "DtcpApi.h"
#include "DtcpContentManagementTypes.h"
#include "DtcpAppLib.h"
#endif
/* End Including Headers */
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define DI_IPTUNER_HTTP_DEBUG

/* DEBUG DEFINE */
#ifdef IPTUNER_ALL_DEBUG

#ifdef DI_IPTUNER_HTTP_DEBUG
#define PrintData		DI_UART_Print
#define PrintDebug		DI_UART_Print
#define PrintEnter		DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit		DI_UART_Print("(-)%s\n", __FUNCTION__)
#define PrintError		DI_UART_Print
#else
#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#define PrintError		DI_UART_Print
#endif

#else

#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintError		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#endif /* END DEBUG DEFINE */

#define UNUSED_PARAM(x)  ((void)x)

#define NEXUS_ALIGN_BUFFER_SIZE (1024*10)
#ifdef CONFIG_DLNA_CERTIFICATION
#define DI_IPTUNER_HTTP_SEEK_RETRY_COUNT (5)
#else
#define DI_IPTUNER_HTTP_SEEK_RETRY_COUNT (20)
#endif

#define	TUNER_RT_TASK_STACK_SIZE	(256*1024)
#define	TUNER_RT_MSG_QUEUE_SIZE		(20)
#define	TUNER_RT_MSG_QUEUE_TIMEOUT	(10)

#define	TUNER_RT_TRIGGERMSG_READ_START	(1)
#define	TUNER_RT_TRIGGERMSG_READ_END	(2)

typedef enum
{
	RT_BUFFER_TYPE_DATA = 0,
	RT_BUFFER_TYPE_INDEX = 1,
	RT_BUFFER_TYPE_PROBE = 2,
} RT_BUFFER_TYPE_e;

#ifdef CONFIG_DLNA_CERTIFICATION
#define USE_SIMPLE_HTTP_CONTROLLER
#endif

#define HTTP_DTCP_HEADER_LENGTH			(14)

/* End #define */
/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */

struct	P_STREAM_DLNA_Flags_t
{
	unsigned int	Flags_Bit0	:1 ;
	unsigned int	Flags_Bit1	:1 ;
	unsigned int	Flags_Bit2	:1 ;
	unsigned int	Flags_Bit3	:1 ;
	unsigned int	Flags_Bit4	:1 ;
	unsigned int	Flags_Bit5	:1 ;
	unsigned int	Flags_Bit6	:1 ;
	unsigned int	Flags_Bit7	:1 ;
	unsigned int	Flags_Bit8	:1 ;
	unsigned int	Flags_Bit9	:1 ;
	unsigned int	Flags_Bit10	:1 ;
	unsigned int	Flags_Bit11	:1 ;
	unsigned int	Flags_Bit12	:1 ;
	unsigned int	Flags_Bit13	:1 ;
	unsigned int	Flags_Bit14	:1 ;
	unsigned int	Flags_Bit15	:1 ;
	unsigned int	Flags_Bit16	:1 ;
	unsigned int	Flags_Bit17	:1 ;
	unsigned int	Flags_Bit18	:1 ;
	unsigned int	Flags_Bit19	:1 ;
	unsigned int	Flags_Bit20	:1 ;
	unsigned int	Flags_Bit21	:1 ;
	unsigned int	Flags_Bit22	:1 ;
	unsigned int	Flags_Bit23	:1 ;
	unsigned int	Flags_Bit24	:1 ;
	unsigned int	Flags_Bit25	:1 ;
	unsigned int	Flags_Bit26	:1 ;
	unsigned int	Flags_Bit27	:1 ;
	unsigned int	Flags_Bit28	:1 ;
	unsigned int	Flags_Bit29	:1 ;
	unsigned int	Flags_Bit30	:1 ;
	unsigned int	Flags_Bit31	:1 ;
} ;

typedef struct P_STREAM_DLNA_Flags_t* STREAM_DLNA_FlagBits;

/* End typedef */

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
/* Start global variable */
/* End global variable */

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
/* Start static variable */
#if 0
static HBOOL P_STREAM_HTTP_UseHttpsProtocol(STREAM_ACCESS_t *pAccess)
{
	if ( pAccess == NULL ) return FALSE;
	if ( pAccess->psz_path == NULL ) return FALSE;

	return ( VK_strncmp(pAccess->psz_path, "https", 5) == 0 ) ? TRUE : FALSE;
}
#endif

/* End static variable */
/*******************************************************************/
/************************ static funtions **************************/
/*******************************************************************/
#if __________p__________
#endif

#if __________task__________
#endif

#if __________drv__________
#endif

#if __________di__________
#endif

#if 0
static HUINT64 P_STREAM_HTTP_FillBufferBlock(DI_IPTUNER_t* pHTuner, STREAM_ACCESS_t *pAccess, ADP_BUFFERMGR_t* pBufferMgr);
#endif

/*****************************
 * Callback for ERROR_EVENT
 *****************************/
void P_HTTP_ResponseCallback (void *context, int param1, char* param2)
{
	DI_IPTUNER_t *pHTuner = context;
	DI_MEDIA_ERROR_REASON_e eErrorReason;
	HINT32 bValid = TRUE;

	PrintEnter;

	UNUSED_PARAM(param2);

	if(pHTuner==NULL)
	{
		PrintError("%s (%d) Error> invalid context.\n",__FUNCTION__,__LINE__);
		return;
	}

	if(pHTuner->tErrorEventCallback!=NULL)
	{
		switch (param1)
		{
			case HTTP_STATUS_CREATED:
				eErrorReason = eMEDIA_ERROR_R_SERVER_HTTP_STATUS_CREATED;
				break;
			case HTTP_STATUS_MULTIPLE_CHOICE:
				eErrorReason = eMEDIA_ERROR_R_SERVER_HTTP_STATUS_MULTIPLE_CHOICE;
				break;
			case HTTP_STATUS_UNUSED:
				eErrorReason = eMEDIA_ERROR_R_SERVER_HTTP_STATUS_UNUSED;
				break;
			case HTTP_STATUS_BAD_REQUEST          :
				eErrorReason = eMEDIA_ERROR_R_SERVER_HTTP_STATUS_BAD_REQUEST;
				break;
			case HTTP_STATUS_UNAUTHORIZED         :
				eErrorReason = eMEDIA_ERROR_R_SERVER_HTTP_STATUS_UNAUTHORIZED;
				break;
			case HTTP_STATUS_PAYMENT_REQUIRED     :
				eErrorReason = eMEDIA_ERROR_R_SERVER_HTTP_STATUS_PAYMENT_REQUIRED;
				break;
			case HTTP_STATUS_FORBIDDEN            :
				eErrorReason = eMEDIA_ERROR_R_SERVER_HTTP_STATUS_FORBIDDEN;
				break;
			case HTTP_STATUS_NOT_FOUND            :
				eErrorReason = eMEDIA_ERROR_R_SERVER_HTTP_STATUS_NOT_FOUND;
				break;
			case HTTP_STATUS_METHOD_NOT_ALLOWED   :
				eErrorReason = eMEDIA_ERROR_R_SERVER_HTTP_STATUS_METHOD_NOT_ALLOWED;
				break;
			case HTTP_STATUS_NOT_ACCEPTABLE       :
				eErrorReason = eMEDIA_ERROR_R_SERVER_HTTP_STATUS_NOT_ACCEPTABLE;
				break;
			case HTTP_STATUS_PROXY_AUTH_REQUIRED  :
				eErrorReason = eMEDIA_ERROR_R_SERVER_HTTP_STATUS_PROXY_AUTH_REQUIRED;
				break;
			case HTTP_STATUS_REQUEST_TIMEOUT      :
				eErrorReason = eMEDIA_ERROR_R_SERVER_HTTP_STATUS_REQUEST_TIMEOUT;
				break;
			case HTTP_STATUS_CONFILICT            :
				eErrorReason = eMEDIA_ERROR_R_SERVER_HTTP_STATUS_CONFILICT;
				break;
			case HTTP_STATUS_GONE                 :
				eErrorReason = eMEDIA_ERROR_R_SERVER_HTTP_STATUS_GONE;
				break;
			case HTTP_STATUS_LENGTH_REQUIRED      :
				eErrorReason = eMEDIA_ERROR_R_SERVER_HTTP_STATUS_LENGTH_REQUIRED;
				break;
			case HTTP_STATUS_PRECONDITION_FAILED      :
				eErrorReason = eMEDIA_ERROR_R_SERVER_HTTP_STATUS_PRECONDITION_FAILED;
				break;
			case HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE :
				eErrorReason = eMEDIA_ERROR_R_SERVER_HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE;
				break;
			case HTTP_STATUS_REQUEST_URI_TOO_LARGE    :
				eErrorReason = eMEDIA_ERROR_R_SERVER_HTTP_STATUS_REQUEST_URI_TOO_LARGE;
				break;
			case HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE   :
				eErrorReason = eMEDIA_ERROR_R_SERVER_HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE;
				break;
			case HTTP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE :
				eErrorReason = eMEDIA_ERROR_R_SERVER_HTTP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE;
				break;
			case HTTP_STATUS_EXPECTATION_FAILED       :
				eErrorReason = eMEDIA_ERROR_R_SERVER_HTTP_STATUS_EXPECTATION_FAILED;
				break;
			case HTTP_STATUS_INTERNAL_SERVER_ERROR    :
				eErrorReason = eMEDIA_ERROR_R_SERVER_HTTP_STATUS_INTERNAL_SERVER_ERROR;
				break;
			case HTTP_STATUS_NOT_IMPLEMENTED          :
				eErrorReason = eMEDIA_ERROR_R_SERVER_HTTP_STATUS_NOT_IMPLEMENTED;
				break;
			case HTTP_STATUS_BAD_GATEWAY              :
				eErrorReason = eMEDIA_ERROR_R_SERVER_HTTP_STATUS_BAD_GATEWAY;
				break;
			case HTTP_STATUS_SERVICE_UNAVAILABLE      :
				eErrorReason = eMEDIA_ERROR_R_SERVER_HTTP_STATUS_SERVICE_UNAVAILABLE;
				break;
			case HTTP_STATUS_GATEWAY_TIMEOUT          :
				eErrorReason = eMEDIA_ERROR_R_SERVER_HTTP_STATUS_GATEWAY_TIMEOUT;
				break;
			case HTTP_STATUS_MHEG_UNKNOWN:
				eErrorReason = eMEDIA_ERROR_R_SERVER_HTTP_STATUS_MHEG_UNKNOWN;
				break;
			default :
				bValid = FALSE;
				break;
		}

		if(bValid == TRUE)
		{
			pHTuner->tErrorEventCallback ((HUINT32)pHTuner, eMEDIA_ERROR_SERVER, eErrorReason);
			PrintDebug("%s (%d) Response Code [ HTTP %d ] (pHTuner=%u) \n"
					,__FUNCTION__
					,__LINE__
					,param1
					,(HUINT32) pHTuner);
		}
	}

	PrintExit;
	return;
}

void P_STREAM_HTTP_TlsCertificateCallback (void *context, void *userdata, void *verifyhandle, HINT32 *result)
{
	DI_IPTUNER_t *pHTuner = context;

	PrintEnter;

	if(pHTuner==NULL)
	{
		PrintError("%s (%d) Error> invalid context.\n",__FUNCTION__,__LINE__);
		return;
	}

	if(pHTuner->cbTlsCertificate.cbVerify != NULL)
	{
		pHTuner->cbTlsCertificate.cbVerify((HUINT32)pHTuner, userdata, verifyhandle, result);
	}

	PrintExit;

	return;
}

STREAM_DLNA_FlagBits P_STREAM_DLNA_FlagBits(unsigned long *value)
{
	STREAM_DLNA_FlagBits tRetValue = (STREAM_DLNA_FlagBits)value;

	return(tRetValue);
}

#if 0
/*
 * ProtocolInfo의 정보를 Parsing한다.
 */
static DI_ERR_CODE P_STREAM_DLNA_Protocolinfo_Parser (DI_IPTUNER_t *pHTuner, HINT8 *ProtocolInfo)
{
	HUINT32			ulRet = ERR_OK;
	char 			*pParser = NULL;
	unsigned int	i = 0;
	char			szPrimaryFlags[9];
	unsigned long	lPrimaryFlags = 0;
	bool			bFowardTrick = FALSE;

	PrintEnter;

	if ( NULL == pHTuner )
	{
		PrintError ("%s (%d) Error>  invalid tuner handle (NULL) !\n",__FUNCTION__,__LINE__);
		return DI_ERR_INVALID_PARAM;
	}

	if ( NULL == ProtocolInfo )
	{
		PrintError ("%s (%d) Error>  invalid Protocol Info (NULL) !\n",__FUNCTION__,__LINE__);
		return DI_ERR_INVALID_PARAM;
	}
	else
	{
		pHTuner->tDlnaSetting.bDlna = TRUE;
	}

	/* OP Code */
	pParser = VK_strstr((char *)ProtocolInfo,"ORG_OP=");
	if(pParser != NULL)
	{
		pParser = pParser+7;
		if((pParser[0] == '0') && (pParser[1] == '1'))
		{
			pHTuner->tDlnaSetting.eSeekMode = DI_IPTUNER_DLNA_SEEKMODE_BYTESEEK;
		}
		else if((pParser[0] == '1') && (pParser[1] == '1'))
		{
			pHTuner->tDlnaSetting.eSeekMode = DI_IPTUNER_DLNA_SEEKMODE_BYTESEEK;
		}
		else if((pParser[0] == '1') && (pParser[1] == '0'))
		{
			pHTuner->tDlnaSetting.eSeekMode = DI_IPTUNER_DLNA_SEEKMODE_TIMESEEK;
		}
		else
		{
			pHTuner->tDlnaSetting.eSeekMode = DI_IPTUNER_DLNA_SEEKMODE_NONE;
		}
	}
	else
	{
		pHTuner->tDlnaSetting.eSeekMode = DI_IPTUNER_DLNA_SEEKMODE_BYTESEEK;
	}

	PrintData ("[%s] eSeekMode = %d\n",__FUNCTION__, pHTuner->tDlnaSetting.eSeekMode);

	/* Conversion Indication */
	pParser = VK_strstr((char *)ProtocolInfo,"ORG_CI=");
	if(pParser != NULL)
	{
		pParser = pParser+7;
		if(pParser[0] == '1')
		{
			pHTuner->tDlnaSetting.eConversion = DI_IPTUNER_DLNA_CONVERSION_TRANSCODED;
		}
		else
		{
			pHTuner->tDlnaSetting.eConversion = DI_IPTUNER_DLNA_CONVERSION_NONE;
		}
	}
	else
	{
		pHTuner->tDlnaSetting.eConversion = DI_IPTUNER_DLNA_CONVERSION_NONE;
	}

	PrintData ("[%s] eConversion = %d\n",__FUNCTION__, pHTuner->tDlnaSetting.eConversion);

	/* Play Speed */
	pParser = VK_strstr((char *)ProtocolInfo,"ORG_PS=");
	if(pParser != NULL)
	{
		VK_MEM_Memset(&pHTuner->tDlnaSetting.tPlaySpeed, 0x0, sizeof(pHTuner->tDlnaSetting.tPlaySpeed));

		pHTuner->tDlnaSetting.tPlaySpeed.bForward1x = TRUE;
		pHTuner->tDlnaSetting.eSpeed = DI_IPTUNER_DLNA_SPEED_NORMAL;

		pParser = pParser+7;
		bFowardTrick = TRUE;

		PrintData ("### [%s] Start PlaySpeed Parsing ###\n",__FUNCTION__);
		for(i = 0; i<20; i++)
		{
			if(VK_strlen(pParser) < i)
			{
				break;
			}

			if(pParser[i] == ';')
			{
				break;
			}
			else if(pParser[i] == ',')
			{
				bFowardTrick = TRUE;
				continue;
			}
			else
			{
				if(pParser[i] == '-')
				{
					bFowardTrick = FALSE;
					PrintData ("### [%s] Backward PlaySpeed Parsing ###\n",__FUNCTION__);
					continue;
				}

				if((pParser[i] == '1') && (VK_strlen(pParser) >= i+2))
				{
					if(pParser[i+1] == '/')
					{
						if(pParser[i+2] == '2')
						{
							if(bFowardTrick == TRUE)
							{
								pHTuner->tDlnaSetting.tPlaySpeed.bForward1_2x = TRUE;
							}
							else
							{
								pHTuner->tDlnaSetting.tPlaySpeed.bBackward1_2x = TRUE;
							}

							i = i+2;
							PrintData ("### [%s] Complete (+-)1/2 PlaySpeed Parsing ###\n",__FUNCTION__);
						}
						else if(pParser[i+2] == '4')
						{
							if(bFowardTrick == TRUE)
							{
								pHTuner->tDlnaSetting.tPlaySpeed.bForward1_4x = TRUE;
							}
							else
							{
								pHTuner->tDlnaSetting.tPlaySpeed.bBackward1_4x = TRUE;
							}

							i = i+2;
							PrintData ("### [%s] Complete (+-)1/4 PlaySpeed Parsing ###\n",__FUNCTION__);
						}
						else
						{
							i = i+2;
						}
					}
					else
					{
						if(bFowardTrick != TRUE)
						{
							pHTuner->tDlnaSetting.tPlaySpeed.bBackward1x = TRUE;
							PrintData ("### [%s] Complete -1 PlaySpeed Parsing ###\n",__FUNCTION__);
						}
					}
				}
				else if(pParser[i] == '1')
				{
					if(bFowardTrick != TRUE)
					{
						pHTuner->tDlnaSetting.tPlaySpeed.bBackward1x = TRUE;
						PrintData ("### [%s] Complete -1 PlaySpeed Parsing ###\n",__FUNCTION__);
					}
				}
				else if(pParser[i] == '2')
				{
					if(bFowardTrick == TRUE)
					{
						pHTuner->tDlnaSetting.tPlaySpeed.bForward2x = TRUE;
					}
					else
					{
						pHTuner->tDlnaSetting.tPlaySpeed.bBackward2x = TRUE;
					}
					PrintData ("### [%s] Complete (+-)2 PlaySpeed Parsing ###\n",__FUNCTION__);
				}
				else if(pParser[i] == '4')
				{
					if(bFowardTrick == TRUE)
					{
						pHTuner->tDlnaSetting.tPlaySpeed.bForward4x = TRUE;
					}
					else
					{
						pHTuner->tDlnaSetting.tPlaySpeed.bBackward4x = TRUE;
					}
					PrintData ("### [%s] Complete (+-)4 PlaySpeed Parsing ###\n",__FUNCTION__);
				}
			}
		}
	}
	else
	{
		pHTuner->tDlnaSetting.eSpeed = DI_IPTUNER_DLNA_SPEED_INVALID;
	}

	PrintData ("### [%s] End PlaySpeed Parsing ###\n",__FUNCTION__);

	/* Primary Flags */
	pParser = VK_strstr((char *)ProtocolInfo,"ORG_FLAGS=");
	if(pParser != NULL)
	{
		VK_MEM_Memset(szPrimaryFlags, 0x0, sizeof(szPrimaryFlags));

		pParser = pParser+10;
		for(i = 0; i<8; i++)
		{
			if(pParser[i] == ';')
			{
				break;
			}
			else
			{
				szPrimaryFlags[i] = pParser[i];
			}
		}

		lPrimaryFlags = strtoul(szPrimaryFlags,NULL,16);

		/*
		 * DLNA v1.5 Versioning Flag
		*/
		if(P_STREAM_DLNA_FlagBits(&lPrimaryFlags)->Flags_Bit20)
		{
			PrintData ("### [%s] DLNA Version 1.5 ###\n",__FUNCTION__);
			pHTuner->tDlnaSetting.eDlnaVersion = DI_IPTUNER_DLNA_VERSION_15;
		}
		else
		{
			PrintData ("### [%s] DLNA Version 1.0 ###\n",__FUNCTION__);
			pHTuner->tDlnaSetting.eDlnaVersion = DI_IPTUNER_DLNA_VERSION_10;
		}

		if(pHTuner->tDlnaSetting.eDlnaVersion == DI_IPTUNER_DLNA_VERSION_15)
		{
			/*
			 * Sender Paced Flag
			 * False = The Content Source is not the Content Clock Source
			 * True = The Content Source is the Content Clock Source
			*/
			pHTuner->tDlnaSetting.bSenderPaced = P_STREAM_DLNA_FlagBits(&lPrimaryFlags)->Flags_Bit31;
			PrintData ("### [%s] bSenderPaced =%d ###\n",__FUNCTION__, pHTuner->tDlnaSetting.bSenderPaced);
			/*
			 * Limited Operations Flag (Time Base)
			 * False = Not Limited Random Access Data Availablility (Full Random Access or Not Support Random Access)
			 * True = Limited Random Access Data Availablility
			*/
			pHTuner->tDlnaSetting.bLimitedTimeBasedSeek = P_STREAM_DLNA_FlagBits(&lPrimaryFlags)->Flags_Bit30;
			PrintData ("### [%s] bLimitedTimeBasedSeek =%d ###\n",__FUNCTION__, pHTuner->tDlnaSetting.bLimitedTimeBasedSeek);

			/*
			 * Limited Operations Flag (Range Base)
			 * False = Not Limited Random Access Data Availablility (Full Random Access or Not Support Random Access)
			 * True = Limited Random Access Data Availablility
			*/
			pHTuner->tDlnaSetting.bLimitedRangeBasedSeek = P_STREAM_DLNA_FlagBits(&lPrimaryFlags)->Flags_Bit29;
			PrintData ("### [%s] bLimitedRangeBasedSeek =%d ###\n",__FUNCTION__, pHTuner->tDlnaSetting.bLimitedRangeBasedSeek);

			/*
			 * DLNA PlayContainer Flag (for DMR Devices(Optional))
			 * False = Not support DLNA PlayContainer URI Operation
			 * True = Support DLNA PlayContainer URI Operation
			*/
			pHTuner->tDlnaSetting.bDlnaPlayContainer = P_STREAM_DLNA_FlagBits(&lPrimaryFlags)->Flags_Bit28;
			PrintData ("### [%s] bDlnaPlayContainer =%d ###\n",__FUNCTION__, pHTuner->tDlnaSetting.bDlnaPlayContainer);

			/*
			 * UCDAM S0(Begin Position) Increasing Flag
			 * False = The S0 Data boundary is fixed.
			 * True = The S0 Data boundary increase with time.
			*/
			pHTuner->tDlnaSetting.bS0_Increasing = P_STREAM_DLNA_FlagBits(&lPrimaryFlags)->Flags_Bit27;
			PrintData ("### [%s] bS0_Increasing =%d ###\n",__FUNCTION__, pHTuner->tDlnaSetting.bS0_Increasing);

			/*
			 * UCDAM SN(End Position) Increasing Flag
			 * False = The SN Data boundary is fixed.
			 * True = The SN Data boundary increase with time.
			*/
			pHTuner->tDlnaSetting.bS1_Increasing = P_STREAM_DLNA_FlagBits(&lPrimaryFlags)->Flags_Bit26;
			PrintData ("### [%s] bS1_Increasing =%d ###\n",__FUNCTION__, pHTuner->tDlnaSetting.bS1_Increasing);

			/*
			 * Pause media opeation support for RTP Serving Endpoints
			 * False = Not support RTP Pause Operation.
			 * True = Support RTP Pause Operation.
			*/
			pHTuner->tDlnaSetting.bRtspPause = P_STREAM_DLNA_FlagBits(&lPrimaryFlags)->Flags_Bit25;
			PrintData ("### [%s] bRtspPause =%d ###\n",__FUNCTION__, pHTuner->tDlnaSetting.bRtspPause);

			/*
			 * Streaming Mode Flag
			 * False = Interactive or Background Mode.
			 * True = Streaming Mode.
			*/
			pHTuner->tDlnaSetting.bStreamingTransfer = P_STREAM_DLNA_FlagBits(&lPrimaryFlags)->Flags_Bit24;
			PrintData ("### [%s] bStreamingTransfer =%d ###\n",__FUNCTION__, pHTuner->tDlnaSetting.bStreamingTransfer);

			/*
			 * Interactive Mode Flag
			 * False = Streaming or Background Mode.
			 * True = Interactive Mode.
			*/
			pHTuner->tDlnaSetting.bInteractiveTransfer = P_STREAM_DLNA_FlagBits(&lPrimaryFlags)->Flags_Bit23;
			PrintData ("### [%s] bInteractiveTransfer =%d ###\n",__FUNCTION__, pHTuner->tDlnaSetting.bInteractiveTransfer);

			/*
			 * Background Mode Flag
			 * False = Streaming or Interactive Mode.
			 * True = Background Mode.
			*/
			pHTuner->tDlnaSetting.bBackgroundTransfer = P_STREAM_DLNA_FlagBits(&lPrimaryFlags)->Flags_Bit22;
			PrintData ("### [%s] bBackgroundTransfer =%d ###\n",__FUNCTION__, pHTuner->tDlnaSetting.bBackgroundTransfer);

			/*
			 * HTTP Connection Stalling Flag
			 * False = Not support HTTP Stalling.
			 * True = Support HTTP Stalling(Pause and Pause-Release).
			*/
			pHTuner->tDlnaSetting.bConnectionStall = P_STREAM_DLNA_FlagBits(&lPrimaryFlags)->Flags_Bit21;
			PrintData ("### [%s] bConnectionStall =%d ###\n",__FUNCTION__, pHTuner->tDlnaSetting.bConnectionStall);

			/*
			 * Link Protected Content Flag
			 * False = Not protected Content.
			 * True = Link Protected Content.
			*/
			pHTuner->tDlnaSetting.bLinkProtection = P_STREAM_DLNA_FlagBits(&lPrimaryFlags)->Flags_Bit16;
			PrintData ("### [%s] bLinkProtection =%d ###\n",__FUNCTION__, pHTuner->tDlnaSetting.bLinkProtection);

			/*
			 * Cleartext Byte Full Data Seek Flag
			 * False = Not protected Content.
			 * True = Link Protected Content.
			*/
			pHTuner->tDlnaSetting.bClearTextFullSeek = P_STREAM_DLNA_FlagBits(&lPrimaryFlags)->Flags_Bit15;
			PrintData ("### [%s] bClearTextFullSeek =%d ###\n",__FUNCTION__, pHTuner->tDlnaSetting.bClearTextFullSeek);

			/*
			 * Cleartext Byte Limited Data Seek Flag
			 * False = Not protected Content.
			 * True = Link Protected Content.
			*/
			pHTuner->tDlnaSetting.bClearTextLimitedSeek = P_STREAM_DLNA_FlagBits(&lPrimaryFlags)->Flags_Bit14;
			PrintData ("### [%s] bClearTextLimitedSeek =%d ###\n",__FUNCTION__, pHTuner->tDlnaSetting.bClearTextLimitedSeek);
		}
		else
		{
			/*
			 * HTTP Connection Stalling Flag
			 * False = Not support HTTP Stalling.
			 * True = Support HTTP Stalling(Pause and Pause-Release).
			*/
			pHTuner->tDlnaSetting.bConnectionStall = P_STREAM_DLNA_FlagBits(&lPrimaryFlags)->Flags_Bit21;
		}
	}

	PrintExit;

	return ulRet;
}
#endif

#ifndef CONFIG_PRODUCT_IMAGE_HWTEST
int P_STREAM_HTTP_DTCP_PCP_HeaderRead (DI_IPTUNER_t *pHTuner, STREAM_ACCESS_t *pAccess)
{
	int 				nDtcpRet =0;
	HUINT8				*pPCPHeaderbuffer = NULL;
	HINT32				i_PCPDataReadSize = 0;
	HINT32				i_RealDataReadSize = 0;
	HINT32				i_DataReadSize_tmp = 0;
	unsigned char		cPCPtHeader[ HTTP_DTCP_HEADER_LENGTH];
	EnExtendedEmi		tExtendedEmi = 0;
	unsigned int		nContentSize = 0;
	HTTP_ACCESS_SYS_t   *pHttpSys = NULL;
	int 				nRetryCount =0;
	HINT64				ulTempOffset = 0;

	PrintEnter;

	if(pHTuner==NULL)
	{
		PrintError("%s (%d) Error> invalid context.\n",__FUNCTION__,__LINE__);
		return DI_IPT_EGENERIC;
	}

	if(pAccess==NULL)
	{
		PrintError("%s (%d) Error> invalid context.\n",__FUNCTION__,__LINE__);
		return DI_IPT_EGENERIC;
	}

RETRY_READHEADER:

	VK_MEM_Memset(&cPCPtHeader, 0x0, HTTP_DTCP_HEADER_LENGTH);

	pHttpSys = (HTTP_ACCESS_SYS_t*)pAccess->p_sys;

	if(pAccess->t_dtcpip.psz_pcp_packethandle != NULL)
	{
		nDtcpRet = DtcpApi_ClosePacket((DtcpPacketHandle)pAccess->t_dtcpip.psz_pcp_packethandle);
		if(nDtcpRet != 0)
		{
			PrintError("######### [%s] Fail DtcpApi_ClosePacket #######\n", __FUNCTION__);
		}
		else
		{
			pAccess->t_dtcpip.psz_pcp_packethandle = NULL;
		}
	}

	pPCPHeaderbuffer = DD_MEM_Alloc(HTTP_DTCP_HEADER_LENGTH);
	if(pPCPHeaderbuffer != NULL)
	{
		VK_MEM_Memset(pPCPHeaderbuffer, 0x0, HTTP_DTCP_HEADER_LENGTH);

		i_PCPDataReadSize = HTTP_DTCP_HEADER_LENGTH;
		i_RealDataReadSize = 0;
		do
		{
			if(nRetryCount != 0)
			{
				ulTempOffset = pAccess->info.i_pos;
				(void)STREAM_HTTP_Seek(pAccess,0);
				(void)STREAM_HTTP_Seek(pAccess,ulTempOffset);
			}

			i_DataReadSize_tmp = STREAM_HTTP_Read ( pAccess, pPCPHeaderbuffer+i_RealDataReadSize, i_PCPDataReadSize - i_RealDataReadSize );
			if(i_DataReadSize_tmp < 0)
			{
				break;
			}

			if (i_DataReadSize_tmp == 0)
			{
				/* If an application requests destruction of http session like DI_IPTUNER_StopBuffering ,
				 *  this function should be returned immediately.
				 * If not so, the session may be blocked by retrying logic. Be careful.
				 */
				if(pAccess->b_immediately_exit == TRUE)
				{
					pAccess->b_retrying = FALSE;
					pAccess->b_alive = FALSE;
					break;
				}

				if(pAccess->info.b_eof == TRUE) /* if chunked-TE and last-chunk, */
				{
					PrintDebug("%s(%d) Debug> EndOfStream Occured in Steram HTTP Controller !!\n", __FUNCTION__, __LINE__);
					break;
				}
			}

			i_RealDataReadSize += i_DataReadSize_tmp;
		}
		while (i_RealDataReadSize < i_PCPDataReadSize);

		if(i_PCPDataReadSize < HTTP_DTCP_HEADER_LENGTH)
		{
			PrintDebug("### %s (%d) i_ReadDataSize=%d ###\n",__FUNCTION__, __LINE__, i_PCPDataReadSize);
			DD_MEM_Free(pPCPHeaderbuffer);

			PrintError("%s (%d) Error> invalid context.\n",__FUNCTION__,__LINE__);
			return DI_IPT_EGENERIC;
		}
		else if(i_PCPDataReadSize == HTTP_DTCP_HEADER_LENGTH)
		{
			PrintDebug("### %s (%d) AkeHandle=x%x, i_ReadDataSize=%d ###\n",__FUNCTION__, __LINE__, pHTuner->tDtcpIpSetting.AkeHandle, i_PCPDataReadSize);
			VK_MEM_Memcpy(cPCPtHeader, pPCPHeaderbuffer, HTTP_DTCP_HEADER_LENGTH);
			nDtcpRet = DtcpApi_ConsumePacketHeader(pHTuner->tDtcpIpSetting.AkeHandle, cPCPtHeader, &tExtendedEmi, &nContentSize, (DtcpPacketHandle *)&pAccess->t_dtcpip.psz_pcp_packethandle);
			if(nDtcpRet != 0)
			{
				PrintError("### %s (%d) Fail DtcpApi_ConsumePacketHeader(nDtcpRet=%d) ###\n",__FUNCTION__, __LINE__, nDtcpRet);

				DD_MEM_Free(pPCPHeaderbuffer);
				pPCPHeaderbuffer = NULL;

				if(((nDtcpRet == -601) || (nDtcpRet == -604)) && (nRetryCount < 2))
				{
					PrintError("### %s (%d) Retry Ake(%d) ###\n",__FUNCTION__,__LINE__, nRetryCount);
					(void)DtcpAppLib_CloseAke(pHTuner->tDtcpIpSetting.AkeHandle);
					pHTuner->tDtcpIpSetting.AkeHandle = NULL;

					nDtcpRet = DtcpAppLib_DoAke(pHTuner->tDtcpIpSetting.pcDTCP1HOST, VK_atoi(pHTuner->tDtcpIpSetting.pcDTCP1PORT), &pHTuner->tDtcpIpSetting.AkeHandle);
					if(nDtcpRet != 0)
					{
						PrintError("### %s (%d) Ake Fail(%d) ###\n",__FUNCTION__,__LINE__, nDtcpRet);
						pHTuner->tDtcpIpSetting.AkeHandle = NULL;
						return DI_IPT_EGENERIC;
					}
					else
					{
						nRetryCount++;
						VK_TASK_Sleep(100);
						goto RETRY_READHEADER;
					}
				}
			}
			else
			{
				PrintDebug("### %s (%d) tExtendedEmi=%d, nContentSize=%d ###\n",__FUNCTION__, __LINE__, tExtendedEmi, nContentSize);
				if((tExtendedEmi != 0) || (nContentSize != 0))
				{
					pAccess->info.i_pos = pAccess->info.i_pos - HTTP_DTCP_HEADER_LENGTH;
					if(pHttpSys != NULL)
					{
						pHttpSys->i_remaining = pHttpSys->i_remaining + HTTP_DTCP_HEADER_LENGTH;
					}
				}

				pAccess->t_dtcpip.b_dtcp_pcpparsing = TRUE;
				pAccess->t_dtcpip.i_extendedemi = tExtendedEmi;
				pAccess->t_dtcpip.i_pcp_contentlength = nContentSize;
				pAccess->t_dtcpip.i_pcp_contentreadlength = 0;

				DD_MEM_Free(pPCPHeaderbuffer);
				pPCPHeaderbuffer = NULL;
			}
		}
	}
	else
	{
		PrintError("%s (%d) Error> invalid context.\n",__FUNCTION__,__LINE__);
		return DI_IPT_EGENERIC;
	}

	PrintExit;

	return DI_IPT_SUCCESS;
}

int P_STREAM_HTTP_DTCP_PCP_ContentRead (DI_IPTUNER_t *pHTuner, STREAM_ACCESS_t *pAccess, HUINT8 *p_buffer, size_t i_len )
{
	int 				nDtcpRet =0;
	HUINT8				*pPCPContentbuffer = NULL;
	HUINT8				*pContentTempbuffer = NULL;
	HINT32				i_DataReadSize = 0;
	HINT32				i_RealDataReadSize = 0;
	HINT32				i_DataReadSize_tmp = 0;
	int 				nRemainDataLength =0;
	size_t				i_TotalReadSize = 0;

	PrintEnter;

	if(pHTuner==NULL)
	{
		PrintError("%s (%d) Error> invalid context.\n",__FUNCTION__,__LINE__);
		return DI_IPT_EGENERIC;
	}

	if(pAccess==NULL)
	{
		PrintError("%s (%d) Error> invalid context.\n",__FUNCTION__,__LINE__);
		return DI_IPT_EGENERIC;
	}

	if(p_buffer==NULL)
	{
		PrintError("%s (%d) Error> invalid context.\n",__FUNCTION__,__LINE__);
		return DI_IPT_EGENERIC;
	}

	nRemainDataLength = i_len%DTCP_BASE_DECRYPT_BLOCK_SIZE;

	if(nRemainDataLength != 0)
	{
		i_TotalReadSize = i_len + (DTCP_BASE_DECRYPT_BLOCK_SIZE - nRemainDataLength);
	}
	else
	{
		i_TotalReadSize = i_len;
	}

	pPCPContentbuffer = DD_MEM_Alloc(i_TotalReadSize);
	if(pPCPContentbuffer != NULL)
	{
		VK_MEM_Memset(pPCPContentbuffer, 0x0, i_TotalReadSize);
		PrintDebug("### %s (%d) i_len =%d ###\n",__FUNCTION__, __LINE__, i_TotalReadSize);

		i_DataReadSize = i_len;

		i_RealDataReadSize = 0;
		do
		{
			i_DataReadSize_tmp = STREAM_HTTP_Read ( pAccess, (HUINT8 *)pPCPContentbuffer+i_RealDataReadSize, i_DataReadSize - i_RealDataReadSize );
			if(i_DataReadSize_tmp < 0)
			{
				break;
			}

			if (i_DataReadSize_tmp == 0)
			{
				/* If an application requests destruction of http session like DI_IPTUNER_StopBuffering ,
				 *  this function should be returned immediately.
				 * If not so, the session may be blocked by retrying logic. Be careful.
				 */
				if(pAccess->b_immediately_exit == TRUE)
				{
					pAccess->b_retrying = FALSE;
					pAccess->b_alive = FALSE;
					break;
				}

				if(pAccess->info.b_eof == TRUE) /* if chunked-TE and last-chunk, */
				{
					PrintDebug("%s(%d) Debug> EndOfStream Occured in Steram HTTP Controller !!\n", __FUNCTION__, __LINE__);
					break;
				}
			}

			i_RealDataReadSize += i_DataReadSize_tmp;
		}
		while (i_RealDataReadSize < i_DataReadSize);

		i_DataReadSize = i_RealDataReadSize;

		if(i_DataReadSize > 0)
		{
			if(i_TotalReadSize > i_len)
			{
				pContentTempbuffer = DD_MEM_Alloc(i_TotalReadSize);
				if(pContentTempbuffer != NULL)
				{
					VK_MEM_Memset(pContentTempbuffer, 0x0, sizeof(i_TotalReadSize) );
				}
				else
				{
					DD_MEM_Free(pPCPContentbuffer);
					pPCPContentbuffer = NULL;

					PrintError("%s (%d) Error> invalid context.\n",__FUNCTION__,__LINE__);
					PrintExit;
					return DI_IPT_EGENERIC;
				}

				nDtcpRet = DtcpApi_DecryptData((DtcpPacketHandle)pAccess->t_dtcpip.psz_pcp_packethandle, pPCPContentbuffer, pContentTempbuffer, i_TotalReadSize);

				/* 요청한 Size만큼 Read되지 않은 경우 처리 */
				if(i_len > (size_t)i_DataReadSize)
				{
					VK_MEM_Memcpy(p_buffer, pContentTempbuffer, i_DataReadSize);
				}
				else
				{
					VK_MEM_Memcpy(p_buffer, pContentTempbuffer, i_len);
				}
			}
			else
			{
				if(i_len > (size_t)i_DataReadSize)
				{
					pContentTempbuffer = DD_MEM_Alloc(i_len);
					if(pContentTempbuffer != NULL)
					{
						VK_MEM_Memset(pContentTempbuffer, 0x0, sizeof(i_len) );
					}
					else
					{
						DD_MEM_Free(pPCPContentbuffer);
						pPCPContentbuffer = NULL;

						PrintError("%s (%d) Error> invalid context.\n",__FUNCTION__,__LINE__);
						PrintExit;
						return DI_IPT_EGENERIC;
					}

					/* 요청한 Size만큼 Read되지 않은 경우 처리 */
					nDtcpRet = DtcpApi_DecryptData((DtcpPacketHandle)pAccess->t_dtcpip.psz_pcp_packethandle, pPCPContentbuffer, pContentTempbuffer, i_len);
					VK_MEM_Memcpy(p_buffer, pContentTempbuffer, i_DataReadSize);
				}
				else
				{
					nDtcpRet = DtcpApi_DecryptData((DtcpPacketHandle)pAccess->t_dtcpip.psz_pcp_packethandle, pPCPContentbuffer, p_buffer, i_DataReadSize);
				}
			}

			if(nDtcpRet != 0)
			{
				PrintError("%s(%d) Error> Fail DtcpApi_DecryptData(nDtcpRet=%d)\n", __FUNCTION__, __LINE__, nDtcpRet);
				PrintError("### %s (%d) i_len =%d ###\n",__FUNCTION__, __LINE__, i_len);
				PrintError("### %s (%d) i_DataReadSize =%d ###\n",__FUNCTION__, __LINE__, i_DataReadSize);
				PrintError("### %s (%d) tExtendedEmi=%d, nContentSize=%u ###\n",
					__FUNCTION__, __LINE__, pAccess->t_dtcpip.i_extendedemi, pAccess->t_dtcpip.i_pcp_contentlength);
				i_DataReadSize = 0;
			}
			else
			{
				pAccess->t_dtcpip.i_pcp_contentreadlength += i_DataReadSize;
			}
		}
		else
		{
			i_DataReadSize = i_DataReadSize_tmp;
		}

		if(pContentTempbuffer != NULL)
		{
			DD_MEM_Free(pContentTempbuffer);
			pContentTempbuffer = NULL;
		}

		DD_MEM_Free(pPCPContentbuffer);
		pPCPContentbuffer = NULL;
	}
	else
	{
		PrintError("%s(%d) Error> Fail Memory Alloc\n", __FUNCTION__, __LINE__);
	}

	PrintExit;

	PrintDebug("### %s (%d) i_DataReadSize =%d ###\n",__FUNCTION__, __LINE__, i_DataReadSize);

	return i_DataReadSize;
}

int P_STREAM_HTTP_DTCP_Read (DI_IPTUNER_t *pHTuner, STREAM_ACCESS_t *pAccess, HUINT8 *p_buffer, size_t i_len )
{
	int 				nPcpParsingRet =0;
	HINT32				i_DataReadSize = 0;

	PrintEnter;

	if(pHTuner==NULL)
	{
		PrintError("%s (%d) Error> invalid context.\n",__FUNCTION__,__LINE__);
		return DI_IPT_EGENERIC;
	}

	if(pAccess==NULL)
	{
		PrintError("%s (%d) Error> invalid context.\n",__FUNCTION__,__LINE__);
		return DI_IPT_EGENERIC;
	}

	if(p_buffer==NULL)
	{
		PrintError("%s (%d) Error> invalid context.\n",__FUNCTION__,__LINE__);
		return DI_IPT_EGENERIC;
	}

	if(pAccess->t_dtcpip.b_dtcp_pcpparsing == FALSE)
	{
		nPcpParsingRet = P_STREAM_HTTP_DTCP_PCP_HeaderRead(pHTuner, pAccess);
		if(nPcpParsingRet == DI_IPT_SUCCESS)
		{
			if((pAccess->t_dtcpip.i_pcp_contentreadlength + i_len) > pAccess->t_dtcpip.i_pcp_contentlength)
			{
				i_DataReadSize = P_STREAM_HTTP_DTCP_PCP_ContentRead(pHTuner, pAccess, p_buffer, (pAccess->t_dtcpip.i_pcp_contentlength - pAccess->t_dtcpip.i_pcp_contentreadlength));
				if(i_DataReadSize <= 0)
				{
					PrintError("%s (%d) Error> Fail P_STREAM_HTTP_DTCP_PCP_ContentRead\n",__FUNCTION__,__LINE__);
				}

				nPcpParsingRet = P_STREAM_HTTP_DTCP_PCP_HeaderRead(pHTuner, pAccess);
				if(nPcpParsingRet == DI_IPT_SUCCESS)
				{
					i_DataReadSize += P_STREAM_HTTP_DTCP_PCP_ContentRead(pHTuner, pAccess, p_buffer+i_DataReadSize, i_len-i_DataReadSize);
				}
				else
				{
					PrintError("%s (%d) Error> Fail P_STREAM_HTTP_DTCP_PCP_HeaderRead\n",__FUNCTION__,__LINE__);
				}
			}
			else
			{
				i_DataReadSize = P_STREAM_HTTP_DTCP_PCP_ContentRead(pHTuner, pAccess, p_buffer, i_len);
				if(i_DataReadSize <= 0)
				{
					PrintError("%s (%d) Error> Fail P_STREAM_HTTP_DTCP_PCP_ContentRead\n",__FUNCTION__,__LINE__);
				}
			}
		}
		else
		{
			PrintError("%s(%d) Error> Fail P_STREAM_HTTP_DTCP_PCP_HeaderRead\n",__FUNCTION__,__LINE__);
			pHTuner->bExitRead = TRUE;
			return DI_IPT_EGENERIC;
		}
	}
	else
	{
		if(pAccess->t_dtcpip.i_pcp_contentreadlength == pAccess->t_dtcpip.i_pcp_contentlength)
		{
			nPcpParsingRet = P_STREAM_HTTP_DTCP_PCP_HeaderRead(pHTuner, pAccess);
			if(nPcpParsingRet == DI_IPT_SUCCESS)
			{
				i_DataReadSize = P_STREAM_HTTP_DTCP_PCP_ContentRead(pHTuner, pAccess, p_buffer, i_len);
				if(i_DataReadSize <= 0)
				{
					PrintError("%s (%d) Error> Fail P_STREAM_HTTP_DTCP_PCP_ContentRead(%u, %u)\n",__FUNCTION__,__LINE__, pAccess->t_dtcpip.i_pcp_contentlength, pAccess->t_dtcpip.i_pcp_contentreadlength);
					PrintError("%s(%d) Error> Position (Current : %"PRId64", File Length : %"PRId64")\n",__FUNCTION__,__LINE__,pAccess->info.i_pos,pAccess->info.i_size);
				}
			}
			else
			{
				PrintError("%s (%d) Error> Fail P_STREAM_HTTP_DTCP_PCP_HeaderRead\n",__FUNCTION__,__LINE__);
				pHTuner->bExitRead = TRUE;
				return DI_IPT_EGENERIC;
			}
		}
		else if((pAccess->t_dtcpip.i_pcp_contentreadlength + i_len) > pAccess->t_dtcpip.i_pcp_contentlength)
		{
			i_DataReadSize = P_STREAM_HTTP_DTCP_PCP_ContentRead(pHTuner, pAccess, p_buffer, (pAccess->t_dtcpip.i_pcp_contentlength - pAccess->t_dtcpip.i_pcp_contentreadlength));
			if(i_DataReadSize < 0)
			{
				PrintError("%s(%d) Error> Fail P_STREAM_HTTP_DTCP_PCP_ContentRead(%ud, %ud)\n",__FUNCTION__,__LINE__, pAccess->t_dtcpip.i_pcp_contentlength, pAccess->t_dtcpip.i_pcp_contentreadlength);
				PrintError("%s(%d) Error> Position (Current : %"PRId64", File Length : %"PRId64")\n",__FUNCTION__,__LINE__,pAccess->info.i_pos,pAccess->info.i_size);
				return DI_IPT_EGENERIC;
			}
			else if(i_DataReadSize == 0)
			{
				PrintError("%s(%d) Error> Fail P_STREAM_HTTP_DTCP_PCP_ContentRead(timeout: %ud, %ud)\n",__FUNCTION__,__LINE__, pAccess->t_dtcpip.i_pcp_contentlength, pAccess->t_dtcpip.i_pcp_contentreadlength);
				PrintError("%s(%d) Error> Position (Current : %"PRId64", File Length : %"PRId64")\n",__FUNCTION__,__LINE__,pAccess->info.i_pos,pAccess->info.i_size);
			}
			/* else */

			nPcpParsingRet = P_STREAM_HTTP_DTCP_PCP_HeaderRead(pHTuner, pAccess);
			if(nPcpParsingRet == DI_IPT_SUCCESS)
			{
				i_DataReadSize += P_STREAM_HTTP_DTCP_PCP_ContentRead(pHTuner, pAccess, p_buffer+i_DataReadSize, i_len-i_DataReadSize);
				if(i_DataReadSize <= 0)
				{
					PrintError("%s (%d) Error> Fail P_STREAM_HTTP_DTCP_PCP_ContentRead(%ud, %ud)\n",__FUNCTION__,__LINE__, pAccess->t_dtcpip.i_pcp_contentlength, pAccess->t_dtcpip.i_pcp_contentreadlength);
					PrintError("%s(%d) Error> Position (Current : %"PRId64", File Length : %"PRId64")\n",__FUNCTION__,__LINE__,pAccess->info.i_pos,pAccess->info.i_size);
				}
			}
			else
			{
				PrintError("%s (%d) Error> Fail P_STREAM_HTTP_DTCP_PCP_HeaderRead\n",__FUNCTION__,__LINE__);
				return DI_IPT_EGENERIC;
			}
		}
		else
		{
			i_DataReadSize = P_STREAM_HTTP_DTCP_PCP_ContentRead(pHTuner, pAccess, p_buffer, i_len);
			if(i_DataReadSize < 0)
			{
				PrintError("%s (%d) Error> Fail P_STREAM_HTTP_DTCP_PCP_ContentRead\n",__FUNCTION__,__LINE__);
				return DI_IPT_EGENERIC;
			}
			else if(i_DataReadSize == 0)
			{
				PrintError("%s (%d) Error> Fail P_STREAM_HTTP_DTCP_PCP_ContentRead(timeout)\n",__FUNCTION__,__LINE__);
			}
			/* else */
		}
	}

	if(i_DataReadSize > (HINT32)i_len)
	{
		PrintError("%s (%d) Error> Invalid Read Size(Req_size=%d, Real_size=%d)\n",__FUNCTION__,__LINE__, i_len, i_DataReadSize);
		i_DataReadSize = -1;
	}

	PrintExit;

	PrintDebug("### %s (%d) i_DataReadSize =%d ###\n",__FUNCTION__, __LINE__, i_DataReadSize);

	return i_DataReadSize;
}
#endif

static HUINT64 P_STREAM_HTTP_ReadFromBuffer(DI_IPTUNER_t* pHTuner, ADP_BUFFERMGR_t* pBufferMgr, void *pPtr, HUINT64 ullOffset, HUINT64 unReadSize)
{
	HUINT32 unBlockId = 0;
	HUINT64 ullBlockOffset = 0;
	size_t	sz_forRead = 0;
	size_t	sz_Real = 0;
	ADP_BUFFERMGR_BlockSnapshot_t tSnapshot;
	if(pBufferMgr == NULL)
	{
		PrintError("######### [%s:%d] Critical Section: Handle is NULL", __FUNCTION__,__LINE__);
		return 0;
	}
	if(pHTuner->ulRealTimeTaskSema != 0 && VK_SEM_Get(pHTuner->ulRealTimeTaskSema) != VK_OK)
	{
		PrintError("%s(%d) Error> can't get ulRealTimeTaskSema\n"
								,__FUNCTION__
								,__LINE__);
		PrintExit;
		return 0;
	}
#if 1
	while(ADP_BUFFERMGR_FindBlockByOffset(pBufferMgr,ullOffset,&unBlockId,&ullBlockOffset) == ADP_BUFFER_OK)
#else
	while(ADP_BUFFERMGR_FindBlock(pBufferMgr,unCategory,0,Current_Pos,&unBlockId,&ullBlockOffset) == ADP_BUFFER_OK)
#endif
	{
	/*	if(pHTuner->bExitRead == TRUE)
		{
			PrintDebug("%s(%d) : handle (0x%x) Breaking Loop\n",__FUNCTION__,__LINE__, pHTuner);
			goto read_done;
		}*/
		if(ADP_BUFFERMGR_SetReadPos(pBufferMgr, unBlockId, ullBlockOffset) != ADP_BUFFER_OK)
		{
			PrintError("%s(%d) ADP_BUFFERMGR_SetReadPos Fail\n",__FUNCTION__,__LINE__);
			break;
		}
		VK_MEM_Memset(&tSnapshot,0x00,sizeof(ADP_BUFFERMGR_BlockSnapshot_t));
		if(ADP_BUFFERMGR_GetReaderSnapshot(pBufferMgr, &tSnapshot) != ADP_BUFFER_OK)
		{
			PrintError("%s(%d) ADP_BUFFERMGR_GetReadBlockStatus Fail\n",__FUNCTION__,__LINE__);
			break;
		}
		if(unReadSize > tSnapshot.ulReadableSize)
		{
			sz_forRead = tSnapshot.ulReadableSize;
		}
		else
		{
			sz_forRead = unReadSize;
		}
		sz_forRead = ADP_BUFFERMGR_Read(pBufferMgr,(HUINT8 *)pPtr+sz_Real,sz_forRead);
		unReadSize -= sz_forRead;
		sz_Real += sz_forRead;
		ullOffset += sz_forRead;
		if(unReadSize <= 0)
		{
			PrintDebug("%s(%d) : handle (0x%x) Read From Buffer(%d)\n",__FUNCTION__,__LINE__,pHTuner, sz_Real);
			goto read_done;
		}
	}
read_done:
	if(pHTuner->ulRealTimeTaskSema != 0)
	{
		(void)VK_SEM_Release(pHTuner->ulRealTimeTaskSema);
	}
	return sz_Real;
}

#if 0
static HUINT64 P_STREAM_HTTP_FillBufferBlock(DI_IPTUNER_t* pHTuner, STREAM_ACCESS_t *pAccess, ADP_BUFFERMGR_t* pBufferMgr)
{
	HUINT8 *pBufferForFilled = NULL;
	HINT32  n_ReturnedValue = 0;
	HINT32 	ReadFail_retrycount =0;
	HUINT64 ullWriteSize = 0;
	int 	error_Timeoutcount =0;
	size_t	sz_Real = 0;
	size_t	sz_Try = 0;
	ADP_BUFFERMGR_BlockSnapshot_t tSnapshot;

	if(pAccess == NULL || pBufferMgr == NULL)
	{
		PrintError("######### [%s:%d] Critical Section: Handle is NULL", __FUNCTION__,__LINE__);
		return 0;
	}

	VK_MEM_Memset(&tSnapshot,0x00,sizeof(ADP_BUFFERMGR_BlockSnapshot_t));

	if(ADP_BUFFERMGR_GetWriterSnapshot(pBufferMgr,&tSnapshot) != ADP_BUFFER_OK)
	{
		return 0;
	}
	if( tSnapshot.ulWritableSize == 0 )
	{
		return 0;
	}
	sz_Try = tSnapshot.ulWritableSize;
	pBufferForFilled = VK_MEM_Alloc(sz_Try);
	if(pBufferForFilled == NULL)
	{
		return 0;
	}
	if(pAccess->info.i_size > 0 &&  pAccess->info.i_pos > 0 && ((HUINT64)pAccess->info.i_size < (HUINT64)pAccess->info.i_pos + sz_Try))
	{
		sz_Try = pAccess->info.i_size - pAccess->info.i_pos;
	}
	if(sz_Try == 0)
	{
		goto read_done;
	}

	do
	{
		if(pHTuner->tDtcpIpSetting.bDtcpIp == TRUE)
		{
			n_ReturnedValue = P_STREAM_HTTP_DTCP_Read (pHTuner, pAccess, pBufferForFilled+sz_Real, sz_Try );
		}
		else
		{
			n_ReturnedValue = STREAM_HTTP_Read (pAccess, pBufferForFilled+sz_Real, sz_Try );
		}

		if ((n_ReturnedValue == 0) || (n_ReturnedValue == DI_IPT_ETIMEOUT))
		{
			/* If an application requests destruction of http session like DI_IPTUNER_StopBuffering ,
			 *  this function should be returned immediately.
			 * If not so, the session may be blocked by retrying logic. Be careful.
			 */
			if(pAccess->b_immediately_exit == TRUE)
			{
				goto read_done;
			}

			if(pAccess->info.b_eof == TRUE) /* if chunked-TE and last-chunk, */
			{
				PrintDebug("%s(%d) Debug> EndOfStream Occured in Steram HTTP Controller !!\n", __FUNCTION__, __LINE__);
				break;
			}
			/* Something happened in STREAM Controller , Let's try again */
			else
			{
				if(error_Timeoutcount != 1)
				{
					error_Timeoutcount++;
					continue;
				}
				else
				{
					error_Timeoutcount = 0;
				}
			}
		}

		if(n_ReturnedValue < 0)
		{
			goto read_done;
		}
		else
		{
			sz_Try -= n_ReturnedValue;
			sz_Real += n_ReturnedValue;
		}
		if((ReadFail_retrycount == 100)&&(sz_Real == 0))
		{
			PrintError("##### %s(%d) Retry Fail(Read Size = 0)(RetryCount=%d) #####\n\n", __FUNCTION__, __LINE__, ReadFail_retrycount);
			ReadFail_retrycount = 0;
			goto read_done;
		}

		if((ReadFail_retrycount == 300)&&(sz_Real > 0))
		{
			PrintError("##### %s(%d) Retry Fail(Read Size != 0)(RetryCount=%d) #####\n\n", __FUNCTION__, __LINE__, ReadFail_retrycount);
			ReadFail_retrycount = 0;
			goto read_done;
		}
		ReadFail_retrycount++;
	}
	while (sz_Try > 0);

read_done:
	if(sz_Real > 0 && pBufferForFilled != NULL)
	{
		ullWriteSize = ADP_BUFFERMGR_Write(pBufferMgr,pBufferForFilled,sz_Real);
	}
	if(pBufferForFilled != NULL)
	{
		VK_MEM_Free(pBufferForFilled);
		pBufferForFilled = NULL;
	}
	return ullWriteSize;
}
#endif

static int P_STREAM_HTTP_SeekForRealTime(DI_IPTUNER_t* pHTuner ,STREAM_ACCESS_t *pAccess, off_t offset)
{
	int ret = DI_IPT_SUCCESS;
	HINT32 nSeekRetryCount = 0;
	RT_STREAMING_INSTANCE_t *pInstance=NULL;
	if( pHTuner == NULL || pAccess == NULL)
	{

		PrintError ("######### [%s:%d] Critical Section: Handle is NULL\n",__FUNCTION__,__LINE__);
		ret = VFIO_ERROR_BAD_PARAMETER;
		goto seek_done;
	}
	pInstance = pHTuner->pRealTimeInstance;
	if(pInstance == NULL)
	{
		PrintError("######### [%s:%d] Critical Section: RealTime Instance is NULL\n",__FUNCTION__,__LINE__);
		ret = VFIO_ERROR_BAD_PARAMETER;
		goto seek_done;
	}
	if( pAccess->info.i_size < offset )
	{
		ret = VFIO_ERROR_BAD_PARAMETER;
		goto seek_done;
	}

	while( DI_IPT_SUCCESS != STREAM_HTTP_Seek ( pAccess , offset ))
	{
		nSeekRetryCount++;
		PrintError ("%s(%d) Error> STREAM_HTTP_Seek fail... Retrying seek request (%d) times..\n"
		,__FUNCTION__
		,__LINE__
		,nSeekRetryCount);

		if ( FALSE == UTIL_SYS_GetLinkStatus () )
		{
			PrintError ("%s (%d) Debug> Network Cable unplugged !!\n",__FUNCTION__,__LINE__);
			ret = VFIO_ERROR_IN_DEVICE;
			goto seek_done;
		}

		if (pHTuner->bProbeCancel == TRUE)
		{
			PrintError ("%s (%d) Debug> PROBE Cancel !!\n",__FUNCTION__,__LINE__);
			ret = VFIO_ERROR_IN_DEVICE;
			goto seek_done;
		}
		if(pHTuner->bExitRead == TRUE && pInstance->bDataReadBlock == TRUE)
		{
			PrintDebug("%s(%d) : handle (0x%x) Breaking Loop\n",__FUNCTION__,__LINE__, pHTuner);
			goto seek_done;
		}
		if(pHTuner->bRealTimeTaskQuit == TRUE)
		{
			PrintDebug("%s(%d) : handle (0x%x) Breaking Loop\n",__FUNCTION__,__LINE__, pHTuner);
			goto seek_done;
		}
		if(pAccess->b_immediately_exit == TRUE)
		{
			PrintDebug("%s(%d) : handle (0x%x) Breaking Loop\n",__FUNCTION__,__LINE__, pHTuner);
			goto seek_done;
		}
		if(nSeekRetryCount > DI_IPTUNER_HTTP_SEEK_RETRY_COUNT)
		{
			PrintError ("%s(%d) Error> STREAM_HTTP_Seek Retrying fail. Last Retrying count(%d) ...\n"
				,__FUNCTION__
				,__LINE__
				,nSeekRetryCount);
			ret = VFIO_ERROR_FAIL;
			goto seek_done;
		}
	}
seek_done:
	return ret;
}

static size_t P_STREAM_HTTP_NetReadAndFillBuffer(DI_IPTUNER_t* pHTuner, STREAM_ACCESS_t *pAccess, ADP_BUFFERMGR_t* pBufferMgr, ADP_BUFFER_UserData_t *pUserData, HUINT32 unReadSize, void *pPtr, HUINT32 unBufferSize, HBOOL bRetain, HUINT64 ullBefore, HBOOL bPassThrough)
{
	HUINT8 *pBufferForFilled = NULL;
	HINT32  n_ReturnedValue = 0;
	HUINT64 ullWriteSize = 0;
	int 	error_Timeoutcount =0;
	size_t	sz_Try = 0;
	size_t	sz_Pos = 0;
	size_t	sz_TotalRead = 0;
	ssize_t sz_BufferSize = BROADCOM_PLAYBACK_READ_BLOCK;
	size_t	sz_forRead = sz_BufferSize;
	HBOOL bOpend = TRUE;
	HBOOL bBufferFull = FALSE;
	ADP_BUFFERMGR_STATUS_t tStatus;
	RT_STREAMING_INSTANCE_t *pInstance=NULL;
	HUINT32 unRetain = 0;
	PrintEnter;
	if(pHTuner == NULL)
	{
		PrintError("######### [%s:%d] Critical Section: Tuner Handle is NULL", __FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}

	pInstance = pHTuner->pRealTimeInstance;
	if(pInstance == NULL)
	{
		PrintError("######### [%s:%d] Critical Section: RealTime Instance is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}

	if(pAccess == NULL)
	{
		PrintError("######### [%s:%d] Critical Section: Handle is NULL", __FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}

	if(pUserData == NULL)
	{
		PrintError("######### [%s:%d] Critical Section: Handle is NULL", __FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}

	if(pPtr != NULL && unBufferSize == 0)
	{
		PrintError("######### [%s:%d] Critical Section: Buffer Size is Zero", __FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}

	sz_Try = unReadSize;
	pBufferForFilled = VK_MEM_Alloc(sz_BufferSize);
	if(pBufferForFilled == NULL)
	{
		PrintExit;
		return 0;
	}

	if(pAccess->info.i_size > 0 &&  pAccess->info.i_pos > 0 && ((HUINT64)pAccess->info.i_size < (HUINT64)pAccess->info.i_pos + sz_Try))
	{
		sz_Try = pAccess->info.i_size - pAccess->info.i_pos;
	}

	if(sz_Try == 0)
	{
		if(pBufferForFilled != NULL)		{
			DD_MEM_Free(pBufferForFilled);
		}
		PrintExit;
		return 0;
	}

	if(pHTuner->ulRealTimeTaskSema != 0 && VK_SEM_Get(pHTuner->ulRealTimeTaskSema) != VK_OK)
	{
		PrintError("%s(%d) Error> can't get ulRealTimeTaskSema\n"
								,__FUNCTION__
								,__LINE__);
		if(pBufferForFilled != NULL)		{
			DD_MEM_Free(pBufferForFilled);
		}
		PrintExit;
		return 0;
	}

	if(ADP_BUFFERMGR_GetStatus(pBufferMgr,&tStatus) != ADP_BUFFER_OK)
	{
		PrintError("%s(%d) Error> can't get Buffer Manager Status\n"
								,__FUNCTION__
								,__LINE__);
		bOpend = FALSE;
		goto read_done;
	}

	if( tStatus.ulBlockSize == 0)
	{
		PrintError("%s(%d) Error> Block Size is Zero !!\n"
								,__FUNCTION__
								,__LINE__);
		bOpend = FALSE;
		goto read_done;
	}

	if(pHTuner->bExitRead == TRUE && pInstance->bDataReadBlock == TRUE)
	{
		PrintDebug("%s(%d) : handle (0x%x) Breaking for VFIO Read\n",__FUNCTION__,__LINE__, pHTuner);
		bOpend = FALSE;
		goto read_done;
	}

	if(pHTuner->bRealTimeTaskQuit == TRUE)
	{
		PrintDebug("%s(%d) : handle (0x%x) Breaking for Buffering Task\n",__FUNCTION__,__LINE__, pHTuner);
		bOpend = FALSE;
		goto read_done;
	}

	if(pBufferMgr != NULL)
	{
		if(ADP_BUFFERMGR_Open(pBufferMgr,pUserData) != ADP_BUFFER_OK)
		{
			if(bRetain == TRUE)
			{
				unRetain = 0;
				if( sz_Try > tStatus.ulBlockSize )
				{
					unRetain = sz_Try / tStatus.ulBlockSize;
					if((tStatus.ulBlockSize * unRetain) < sz_Try)
					{
						unRetain++;
					}
				}
				else
				{
					unRetain = 1;
				}
				if(ADP_BUFFERMGR_RetainBlock(pBufferMgr, pUserData->ulCategory, pUserData->ulSegId,ullBefore,unRetain) > 0
				/*ADP_BUFFERMGR_RetainUninterestedBlock(pBufferMgr, pUserData->ulCategory, pUserData->ulSegId,ullBefore) > 0*/)
				{
					if(ADP_BUFFERMGR_Open(pBufferMgr,pUserData) != ADP_BUFFER_OK)
					{
						PrintDebug("##### %s(%d) Can't Open Block\n\n", __FUNCTION__, __LINE__);
						bOpend = FALSE;
					}
				}
				else
				{
					PrintDebug("##### %s(%d) Can't Retain Block\n\n", __FUNCTION__, __LINE__);
					bOpend = FALSE;
				}
			}
			else
			{
				PrintDebug("##### %s(%d) Can't Open Block\n\n", __FUNCTION__, __LINE__);
				bOpend = FALSE;
			}
		}
	}
	else
	{
		bOpend = FALSE;
	}

	if(bOpend == FALSE && bPassThrough == FALSE	&& pPtr == NULL)
	{
		goto read_done;
	}

	do
	{
		if(pHTuner->bExitRead == TRUE && pInstance->bDataReadBlock == TRUE)
		{
			PrintDebug("%s(%d) : handle (0x%x) Breaking Loop\n",__FUNCTION__,__LINE__, pHTuner);
			goto read_done;
		}
		if(pHTuner->bRealTimeTaskQuit == TRUE)
		{
			PrintDebug("%s(%d) : handle (0x%x) Breaking Loop for Buffering Task\n",__FUNCTION__,__LINE__, pHTuner);
			goto read_done;
		}
		if(pAccess->b_immediately_exit == TRUE)
		{
			PrintDebug("%s(%d) : handle (0x%x) Breaking Loop\n",__FUNCTION__,__LINE__, pHTuner);
			goto read_done;
		}
		if( sz_Pos == sz_forRead)
		{
			sz_Pos = 0;
		}
		if(sz_Try < sz_forRead)
		{
			sz_forRead = sz_Try;
		}

#ifndef CONFIG_PRODUCT_IMAGE_HWTEST
		if(pHTuner->tDtcpIpSetting.bDtcpIp == TRUE)
		{
			n_ReturnedValue = P_STREAM_HTTP_DTCP_Read (pHTuner, pAccess, pBufferForFilled + sz_Pos, sz_forRead );
		}
		else
#endif
		{
			n_ReturnedValue = STREAM_HTTP_Read (pAccess, pBufferForFilled + sz_Pos, sz_forRead);
		}
		if ((n_ReturnedValue == 0) || (n_ReturnedValue == DI_IPT_ETIMEOUT))
		{
			/* If an application requests destruction of http session like DI_IPTUNER_StopBuffering ,
			 *  this function should be returned immediately.
			 * If not so, the session may be blocked by retrying logic. Be careful.
			 */

			if(pHTuner->bExitRead == TRUE && pInstance->bDataReadBlock == TRUE)
			{
				PrintDebug("%s(%d) : handle (0x%x) Breaking Loop\n",__FUNCTION__,__LINE__, pHTuner);
				goto read_done;
			}
			if(pHTuner->bRealTimeTaskQuit == TRUE)
			{
				PrintDebug("%s(%d) : handle (0x%x) Breaking Loop for Buffering Task \n",__FUNCTION__,__LINE__, pHTuner);
				goto read_done;
			}
			if(pAccess->b_immediately_exit == TRUE)
			{
				PrintDebug("%s(%d) : handle (0x%x) Breaking Loop\n",__FUNCTION__,__LINE__, pHTuner);
				goto read_done;
			}
			if(pAccess->info.b_eof == TRUE) /* if chunked-TE and last-chunk, */
			{
				PrintDebug("%s(%d) Debug> EndOfStream Occured in Steram HTTP Controller !!\n", __FUNCTION__, __LINE__);
				break;
			}
			/* Something happened in STREAM Controller , Let's try again */
			else
			{
				if(error_Timeoutcount < 3)
				{
					error_Timeoutcount++;
					PrintError("%s(%d) : handle (0x%x) Time Out\n",__FUNCTION__,__LINE__, pHTuner);
					continue;
				}
				else
				{
					error_Timeoutcount = 0;
					PrintError("%s(%d) Error> Read Fail!! (Time Out)\n Try to Reconnect...\n", __FUNCTION__, __LINE__);
					if(DI_IPT_SUCCESS != P_STREAM_HTTP_SeekForRealTime(pHTuner,pAccess,pAccess->info.i_pos))
					{
						PrintError ("%s(%d) Error> Reconnect fail... \n",__FUNCTION__,__LINE__);
						sz_TotalRead = -1;
						goto read_done;
					}
					continue;
				}
			}
		}
		else if(n_ReturnedValue < 0)
		{
			PrintError("%s(%d) Error> Read Fail!! (Socket Error)\n Try to Reconnect...\n", __FUNCTION__, __LINE__);
			if(DI_IPT_SUCCESS != P_STREAM_HTTP_SeekForRealTime(pHTuner,pAccess,pAccess->info.i_pos))
			{
				PrintError ("%s(%d) Error> Reconnect fail... \n",__FUNCTION__,__LINE__);
				sz_TotalRead = -1;
//				ret = VFIO_ERROR_FAIL;
				goto read_done;
			}
			continue;
		}
		else
		{
			HINT32 nPosForBuffer = 0;
			HINT32 nSize = 0;
			if(pPtr != NULL && unBufferSize > sz_TotalRead)
			{
				if(sz_TotalRead + n_ReturnedValue > unBufferSize)
				{
					nSize = unBufferSize - sz_TotalRead;
				}
				else
				{
					nSize = n_ReturnedValue;
				}
				VK_MEM_Memcpy((HUINT8*)pPtr + sz_TotalRead, pBufferForFilled + sz_Pos, n_ReturnedValue);
			}
			sz_Try -= n_ReturnedValue;
			sz_TotalRead += n_ReturnedValue;
//			PrintDebug("%s(%d) Debug> HTTP Left : %d \n", __FUNCTION__, __LINE__,sz_Try);
			if(	bOpend == TRUE && bBufferFull == FALSE)
			{
				do
				{
					if(pHTuner->bExitRead == TRUE && pInstance->bDataReadBlock == TRUE)
					{
						PrintDebug("%s(%d) : handle (0x%x) Breaking Loop\n",__FUNCTION__,__LINE__, pHTuner);
						goto read_done;
					}

					if(pHTuner->bRealTimeTaskQuit == TRUE)
					{
						PrintDebug("%s(%d) : handle (0x%x) Breaking Loop for Buffering Task\n",__FUNCTION__,__LINE__, pHTuner);
						goto read_done;
					}

					if(pAccess->b_immediately_exit == TRUE)
					{
						PrintDebug("%s(%d) : handle (0x%x) Breaking Loop\n",__FUNCTION__,__LINE__, pHTuner);
						goto read_done;
					}

					ullWriteSize = ADP_BUFFERMGR_Write(pBufferMgr,pBufferForFilled + (sz_Pos+nPosForBuffer),n_ReturnedValue - nPosForBuffer);
					if((n_ReturnedValue - nPosForBuffer) != (HINT32)ullWriteSize)
					{
						if(bRetain == TRUE)
						{
							unRetain = 0;
							if( sz_Try > tStatus.ulBlockSize )
							{
								unRetain = sz_Try / tStatus.ulBlockSize;
								if((tStatus.ulBlockSize * unRetain) < sz_Try)
								{
									unRetain++;
								}
							}
							else
							{
								unRetain = 1;
							}

							if(ADP_BUFFERMGR_RetainBlock(pBufferMgr, pUserData->ulCategory, pUserData->ulSegId,ullBefore,unRetain) == 0
							/* ADP_BUFFERMGR_RetainUninterestedBlock(pBufferMgr, pUserData->ulCategory, pUserData->ulSegId,ullBefore) == 0 */)
							{
								PrintDebug("##### %s(%d) Can't Retain Block\n\n", __FUNCTION__, __LINE__);
								if(bPassThrough == FALSE && unReadSize == sz_TotalRead)
								{
									goto read_done;
								}
								else
								{
									bBufferFull = TRUE;
									break;
								}
							}
						}
						else
						{
							bBufferFull = TRUE;
							break;
						}
					}

					nPosForBuffer += (HINT32) ullWriteSize;
					if(n_ReturnedValue < nPosForBuffer)
					{
						PrintError ("%s (%d) Warn> Over Wrote !!!\n",__FUNCTION__,__LINE__);
						break;
					}
				} while(n_ReturnedValue != nPosForBuffer);
			}
			else
			{
				if(bPassThrough == FALSE && unBufferSize == sz_TotalRead)
				{
					goto read_done;
				}
			}
		}
#if 0
		if((ReadFail_retrycount == 100)&&(sz_Try > 0))
		{
			PrintError("##### %s(%d) Retry Fail(Read Size = 0)(RetryCount=%d) #####\n\n", __FUNCTION__, __LINE__, ReadFail_retrycount);
			ReadFail_retrycount = 0;
			goto read_done;
		}
#endif

	}
	while (sz_Try > 0);
read_done:

	if( bOpend == TRUE)
	{
#if 0
		if(bPassThrough == FALSE && bBufferFull == FALSE)
		{
			ullWriteSize = P_STREAM_HTTP_FillBufferBlock(pHTuner, pAccess,pBufferMgr);
		}
#endif
		if(ADP_BUFFERMGR_Close(pBufferMgr) != ADP_BUFFER_OK)
		{
			PrintError("##### %s(%d) ADP_BUFFERMGR_Close Fail\n\n", __FUNCTION__, __LINE__);
		}
	}
	if(pBufferForFilled != NULL)
	{
		VK_MEM_Free(pBufferForFilled);
		pBufferForFilled = NULL;
	}
	if(pHTuner->ulRealTimeTaskSema != 0)
	{
		(void)VK_SEM_Release(pHTuner->ulRealTimeTaskSema);
	}
	PrintExit;
	return sz_TotalRead;
}

static void P_RealTimeBufferingTask (void *pParam)
{
	DI_IPTUNER_Msg_t s_Msg;
	HINT32 nVKRet;
	DI_IPTUNER_t *pHTuner = (DI_IPTUNER_t*) pParam;
	STREAM_ACCESS_t *pAccess = NULL;
	ADP_BUFFERMGR_t *pBufferMgr=NULL;
	RT_STREAMING_INSTANCE_t *pInstance=NULL;
	ADP_BUFFER_UserData_t tUserData;
	HUINT32 unCategory = RT_BUFFER_TYPE_DATA;
	HUINT32 unTriggerTick = 0;
	HUINT32 unWakeUpTick = 0;
	HUINT32 unBlockId = 0;
	HUINT64 ullBlockOffset = 0;
	HUINT64 ullPos = 0;

	PrintEnter;

	if (pHTuner == NULL)
	{
		PrintDebug ("%s (%d) : P_RealTimeBufferingTask unavailable (pHTuner handle is null)!!!\n",__FUNCTION__,__LINE__);
		goto end;
	}
	pInstance = pHTuner->pRealTimeInstance;
	if(pInstance == NULL)
	{
		PrintError ("%s (%d) Error> RealTime Instance is NULL\n",__FUNCTION__,__LINE__);
		goto end;
	}
	pBufferMgr = pInstance->pBufferMgr;
	if(pBufferMgr == NULL)
	{
		PrintError ("%s (%d) Error>  Buffer Manager is NULL\n",__FUNCTION__,__LINE__);
		goto end;
	}
	if((STREAM_ACCESS_t *)pHTuner->pHStreamData != NULL)
	{
		pAccess = (STREAM_ACCESS_t *)pHTuner->pHStreamData;
	}
	else
	{
		PrintError ("%s(%d) Error> tHandle->pHStream is null \n",__FUNCTION__,__LINE__);
		goto end;
	}
	pHTuner->bRealTimeTaskFinished = FALSE;

	while ( TRUE != pHTuner->bRealTimeTaskQuit )
	{
		nVKRet = VK_MSG_ReceiveTimeout (pHTuner->ulRealTimeTaskMsgQueue, &s_Msg, sizeof(DI_IPTUNER_Msg_t), TUNER_RT_MSG_QUEUE_TIMEOUT);
		if ( nVKRet == VK_OK )
		{
			if (s_Msg.unParam1 != RT_BUFFER_TYPE_PROBE && s_Msg.unMsg == TUNER_RT_TRIGGERMSG_READ_START )
			{
				unWakeUpTick = 0;
				unTriggerTick = 0;
				ullPos = 0;
//				PrintDebug ("%s(%d) : TUNER_RT_TRIGGERMSG_READ_START Received TUNER (%u) \n",__FUNCTION__,__LINE__, (HUINT32)pHTuner);
				continue;
			}
			if (s_Msg.unParam1 != RT_BUFFER_TYPE_PROBE && s_Msg.unMsg == TUNER_RT_TRIGGERMSG_READ_END )
			{
//				PrintDebug ("%s(%d) : TUNER_RT_TRIGGERMSG_READ_END Received TUNER (%u) \n",__FUNCTION__,__LINE__, (HUINT32)pHTuner);
				unTriggerTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */
				continue;
			}
			else
			{
				continue;
			}
		}
		else
		{
			unWakeUpTick = VK_TIMER_GetSystemTick(); /* 1000 tick == 1 sec */
			if(unTriggerTick != 0 && unWakeUpTick > unTriggerTick
			&& unWakeUpTick - unTriggerTick > 1000)  // TODO: Time Diff Default 정리할 것.
			{
				if (pInstance->bDataReadBlock == TRUE)
				{
					continue;
				}
				if (VK_SEM_Get(pAccess->ulStreamSema) != VK_OK)
				{
					PrintError("%s(%d) Error> can't get ulStreamSema (tuner %u)\n"
					,__FUNCTION__
					,__LINE__
					,(HUINT32)pHTuner);
					goto end;
				}
#if 1
				if(ADP_BUFFERMGR_FindBlockByOffset(pBufferMgr,pAccess->info.i_pos + ullPos,&unBlockId,&ullBlockOffset) == ADP_BUFFER_FAIL)
				{
					HUINT64 ullSize = 0;
					if(pAccess->info.i_pos + (HINT64)ullPos < pAccess->info.i_size)
					{
						if((HUINT64)pAccess->info.i_pos + ullPos < pInstance->ullCurrent_PlayOffset)
						{
							PrintError ("%s(%d) : the offset of access handle is need to be updated. \n",__FUNCTION__,__LINE__ );
							PrintError ("%s(%d) : Jump to Current Play Offset %llu\n",__FUNCTION__,__LINE__,pInstance->ullCurrent_PlayOffset);
							#if 1
							if(DI_IPT_SUCCESS != P_STREAM_HTTP_SeekForRealTime(pHTuner,pAccess,pInstance->ullCurrent_PlayOffset))
							{
								PrintError ("%s(%d) Error> Update fail... \n",__FUNCTION__,__LINE__);
								(void)VK_SEM_Release (pAccess->ulStreamSema);
								continue;
							}
							#endif
						}
						else if ( ullPos > 0 )
						{
							PrintError ("%s(%d) : the offset of access handle is need to be updated. \n",__FUNCTION__,__LINE__ );
							PrintError ("%s(%d) : Jump to Empty Area %llu \n",__FUNCTION__,__LINE__,pAccess->info.i_pos + ullPos);
							#if 1
							if(DI_IPT_SUCCESS != P_STREAM_HTTP_SeekForRealTime(pHTuner,pAccess,pAccess->info.i_pos + ullPos))
							{
								PrintError ("%s(%d) Error> Update fail... \n",__FUNCTION__,__LINE__);
								(void)VK_SEM_Release (pAccess->ulStreamSema);
								continue;
							}
							#endif
						}
						ullPos = 0;
//						PrintError ("%s(%d) : Wake Up Buffering (%u) \n",__FUNCTION__,__LINE__, (HUINT32)pHTuner);
						VK_MEM_Memset(&tUserData,0x00,sizeof(ADP_BUFFER_UserData_t));
						tUserData.ulCategory = unCategory;
						tUserData.ullStartOffset = (HUINT64)pAccess->info.i_pos;
						ullSize = P_STREAM_HTTP_NetReadAndFillBuffer(pHTuner
											,pAccess
											,pBufferMgr
											,&tUserData
											,BROADCOM_PLAYBACK_READ_BLOCK  /* Read Size */
											,NULL /* Ptr Buffer */
											,0 /* Ptr Buffer Size */
											,TRUE /* Buffer Retain Flag */
											,pInstance->ullCurrent_PlayOffset /* Retain Base Offset */
											,FALSE /* Don't Pass Through */
											);
						if(ullSize > 0)
						{
							PrintError ("%s(%d) : Buffering (%llu From %llu) \n",__FUNCTION__,__LINE__,ullSize,(HUINT64)pAccess->info.i_pos);
						}

						if (pAccess->b_immediately_exit == TRUE)
						{
							PrintError ("%s(%d) : Exit by ProbeCancel\n",__FUNCTION__,__LINE__);
							goto end;
						}
					}
					else
					{
						ullPos = 0;
					}
				}
				else
				{
					ullPos += BROADCOM_PLAYBACK_READ_BLOCK;
					if(pAccess->info.i_pos + (HINT64)ullPos > pAccess->info.i_size)
					{
						ullPos	= 0;
					}
				}
#endif
				(void)VK_SEM_Release (pAccess->ulStreamSema);
			}
		}
	} /* endof while */
	/* else { continue } */

end:
	PrintError ("%s(%d) : Debug> Buffering Task Finished.\n",__FUNCTION__,__LINE__);
	if(pHTuner != NULL)
	{
		pHTuner->bRealTimeTaskFinished = TRUE;
	}
	PrintExit;
	return;
}

static HINT32 P_STREAM_HTTP_CreateBufferingThread (DI_IPTUNER_t* pHTuner)
{
	HUINT8 strName[32] = {0};
	DI_IPTUNER_Msg_t s_Msg;
	HINT32 nVKRet;

	PrintEnter;
	if(pHTuner == NULL)
	{
		PrintError	("%s (%d) Error>  pHTuner is NULL !\n",__FUNCTION__,__LINE__);
		return -1;
	}
	if ( TRUE != pHTuner->bRealTimeTaskInit )
	{
		/* create & init message que */
		VK_snprintf((char*)strName,VK_strlen("RT_TASKMSGQ_")+10, "RT_TASKMSGQ_%p", (void*)pHTuner);
		nVKRet = VK_MSG_Create(TUNER_RT_MSG_QUEUE_SIZE, sizeof(s_Msg), (const char*)strName, &pHTuner->ulRealTimeTaskMsgQueue, VK_SUSPENDTYPE_FIFO);
		if ( VK_OK != nVKRet )
		{
			PrintError ("%s (%d) Error> STREAM Task msgQ creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet );
			return -1;
		}	/* Semaphore for shared data between main_task and timer_task */
		VK_MEM_Memset(strName,0x00,32);
		VK_snprintf((char*)strName,VK_strlen("RT_TASKSEM_")+10, "RT_TASKSEM_%p", (void*)pHTuner);
		nVKRet = VK_SEM_Create(&pHTuner->ulRealTimeTaskSema, (const char*)strName, VK_SUSPENDTYPE_PRIORITY);
		if ( VK_OK != nVKRet )
		{
			PrintError ("%s (%d) Error> STREAM Timer sema creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet );
			return -1;
		}

		VK_MEM_Memset(strName,0x00,32);
		VK_snprintf((char*)strName,VK_strlen("RT_TASK_")+10, "RT_TASK_%p", (void*)pHTuner);
		/** Create Download Task */
		nVKRet = VK_TASK_Create ( P_RealTimeBufferingTask
							, PLAYBACK_LOAD_TASK_PRIORITY	// Buffering TASK Priority 는 Playback 만큼 또는 그 이상 높아야 한다.
							, TUNER_RT_TASK_STACK_SIZE
							, (const char*)strName
							, (void*)pHTuner
							, &pHTuner->ulRealTimeTaskId
							, 0);
		if ( VK_OK != nVKRet )
		{
			PrintError ("%s (%d) Error> STREAM Tuner task creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet);
			return -1;
		}
		/** Start Download Task */
		pHTuner->bRealTimeTaskQuit = FALSE;
		pHTuner->bRealTimeTaskInit = TRUE;
		VK_TASK_Start(pHTuner->ulRealTimeTaskId);
		PrintError ("%s (%d) Debug Task START!\n",__FUNCTION__,__LINE__);
	}
	else
	{
		PrintDebug  ("%s (%d) Debug>  Already initialized !\n",__FUNCTION__,__LINE__);
	}
	PrintExit;
	return 0;
}

static HINT32 P_STREAM_HTTP_DestroyBufferingThread (DI_IPTUNER_t* pHTuner)
{
	PrintEnter;
	if(pHTuner == NULL)
	{
		PrintError("%s(%d) : Error> Tuner Handle is NULL\n",__FUNCTION__,__LINE__);
		return -1;
	}

	if(pHTuner->bRealTimeTaskInit == FALSE)
	{
		return 0;
	}
	pHTuner->bRealTimeTaskQuit = TRUE;
	while(pHTuner->bRealTimeTaskFinished != TRUE)
	{
		VK_TASK_Sleep(100);
	}
	VK_TASK_Sleep(100);
	(void) VK_TASK_Destroy (pHTuner->ulRealTimeTaskId);
	pHTuner->bRealTimeTaskInit = FALSE;
	PrintError ("%s(%d) : Debug> Buffering Task Destroied.\n",__FUNCTION__,__LINE__);
	PrintExit;
	return 0;
}

/** Open From User Application and bmedia*/
int DI_STREAM_HTTP_Open  (const HINT8 *pUrl, const HINT8 *mode, VFIO_Handle_t *ptHandle )
{
	HINT32 nRet = DI_IPT_SUCCESS;
	HINT32 vRet = VFIO_ERROR_SUCCESS;
	HTTP_ACCESS_SYS_t *pstHttpSys=NULL;
#if SUPPORT_HUMAX_COOKIE
	char *psCookie = NULL;
	URL_t tUrl;
#endif
	DI_IPTUNER_t *pHTuner=NULL;
	STREAM_ACCESS_t *pAccess=NULL;
	ADP_BUFFERMGR_t* pBufferMgr = NULL;
	RT_STREAMING_INSTANCE_t *pInstance=NULL;

	UNUSED_PARAM (mode);
	PrintEnter;

	/* valid check */
	if(pUrl==NULL)
	{
		ptHandle->ucAvailable = 0;
		PrintError("%s(%d) : Error> invalid pUrl(%s)\n",__FUNCTION__,__LINE__,pUrl);
		return VFIO_ERROR_IN_DEVICE;
	}

	/* Searching IPTUNER handle using URL String */
	pHTuner = DI_IPTUNER_POOL_FindByUrl (pUrl);
	if(pHTuner == NULL)
	{
		ptHandle->ucAvailable = 0;
		PrintError("%s(%d) : Error> can't find iptuner handler by this URL(%s)\n",__FUNCTION__,__LINE__,pUrl);
		return VFIO_ERROR_IN_DEVICE;
	}
	else
	{
		// TODO: Configuration 사용하여  정리 할 것 by  ykkim3
		ADP_BUFFERMGR_CONFIG_t tConfig;
		tConfig.eType = ADP_BUFFERTYPE_MEMORY;
		tConfig.eMode = ADP_BUFFERMODE_FRAGMENTED;
		tConfig.ullBufferSize = pHTuner->uiProbebufferSize;
		tConfig.ulBlockSize = pHTuner->uiBlockSize;

		if(pHTuner->pRealTimeInstance != NULL)
		{
			pInstance = (RT_STREAMING_INSTANCE_t*)pHTuner->pRealTimeInstance;
			if(pInstance->pUrl != NULL)
			{
				VK_MEM_Free(pInstance->pUrl);
				pInstance->pUrl = NULL;
			}
			pBufferMgr = pInstance->pBufferMgr;
			if(pBufferMgr != NULL)
			{
				ADP_BUFFERMGR_Destroy(pBufferMgr);
			}
			if(pHTuner->pRealTimeInstance != NULL)
			{
				VK_MEM_Free(pHTuner->pRealTimeInstance);
				pHTuner->pRealTimeInstance = NULL;
			}
			pBufferMgr = NULL;
			pInstance = NULL;
		}

		pHTuner->pRealTimeInstance = (RT_STREAMING_INSTANCE_t*)VK_MEM_Alloc(sizeof(RT_STREAMING_INSTANCE_t));
		if(pHTuner->pRealTimeInstance == NULL)
		{
			ptHandle->ucAvailable = 0;
			PrintError("%s(%d) : Error> Malloc Fail\n",__FUNCTION__,__LINE__);
			return VFIO_ERROR_IN_DEVICE;
		}
		else
		{
			VK_MEM_Memset(pHTuner->pRealTimeInstance,0x00,sizeof(RT_STREAMING_INSTANCE_t));
		}
		pInstance = (RT_STREAMING_INSTANCE_t*)pHTuner->pRealTimeInstance;
		pInstance->pBufferMgr = ADP_BUFFERMGR_Create(&tConfig);
		if(pInstance->pBufferMgr == NULL)
		{
			ptHandle->ucAvailable = 0;
			PrintError("%s(%d) : Error> ADP_BUFFERMGR_Create Fail\n",__FUNCTION__,__LINE__);
			if(pHTuner->pRealTimeInstance != NULL)
			{
				VK_MEM_Free(pHTuner->pRealTimeInstance);
				pHTuner->pRealTimeInstance = NULL;
			}
			return VFIO_ERROR_IN_DEVICE;
		}
	}
	pBufferMgr = pInstance->pBufferMgr;

	/* Make Stream Core Handler */
	pAccess = (STREAM_ACCESS_t*)DD_MEM_Alloc(sizeof(STREAM_ACCESS_t));
	if(pAccess == NULL)
	{
		PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
		ptHandle->ucAvailable = 0;

		nRet = VFIO_ERROR_IN_DEVICE;
		PrintExit;
		return nRet;
	}
	else
	{
		VK_MEM_Memset(pAccess, 0x0, sizeof(STREAM_ACCESS_t));
	}

	/*
	 * We must store a STREAM Handler in iptuner handle .
	 * If not so, we can't know correct stream handle in vfio function set.(VFIO_Read/VFIO_xxx etc)
	 *
	 * iptuner_handler --  VFIO Handler (include file and socket descriptor)
	 *                 --  STREAM Handler ( include protocol controller)
	 */
	ptHandle->pTunerHandle = (void*) pHTuner;
//	pHTuner->pHStream = (void*) pAccess;

#if SUPPORT_HUMAX_COOKIE
	VK_MEM_Memset(&tUrl, 0x0, sizeof(tUrl));
	UTIL_UrlParse(&tUrl, (const char *)pUrl, 0);
	if(tUrl.psz_cookie != NULL)
	{
		psCookie = VK_StrDup((void *)tUrl.psz_cookie);
	}
	UTIL_UrlClean(&tUrl);
#endif

	pAccess->psz_path = VK_StrDup ( (void *) UTIL_GetOriginalURL ((const char *)pUrl) );
#if SUPPORT_HUMAX_COOKIE
	if(psCookie != NULL)
	{
		pAccess->psz_cookie = psCookie;
	}
#endif

	if( pAccess->psz_path != NULL )
	{
		if((NULL != VK_strstr((char *)pAccess->psz_path, "https")) && (NULL != VK_strstr((char *)pAccess->psz_path, "#http")))
		{
			/* Marlin DRM Compound URI */
			int dResult = DRV_OK;
			DRM_Info_t drm_info;
			dResult = DRV_DRM_GetInfo(pHTuner, &drm_info);
			if(dResult != DRV_OK)
			{
				PrintError("DRV_DRM_GetInfo failed!\n");
				vRet = VFIO_ERROR_FAIL;
				goto OPEN_ERROR;
			}
			pHTuner->tMediaInfo.eDrmType = DI_MEDIA_DRM_TYPE_MARLIN;
			drm_info.drm_type=DI_DRM_TYPE_MARLIN;
			drm_info.drm.marlin.msg.MessageURL = (HUINT8*)pAccess->psz_path;

			dResult = DRV_DRM_ResigterCallback(pHTuner, &drm_info);
			if(dResult != DRV_OK)
			{
				PrintError("DRV_DRM_ResigterCallback failed!\n");
				vRet = VFIO_ERROR_FAIL;
				goto OPEN_ERROR;
			}

			dResult = DRV_DRM_GetLicence(pHTuner);
			if(dResult != DRV_OK)
			{
				PrintError("DRV_DRM_GetLicence failed!\n");
				vRet = VFIO_ERROR_FAIL;
				goto OPEN_ERROR;
			}
		}
	}

	if(pHTuner->pTls_ca_name != NULL)
	{
		pAccess->psz_ca_name = VK_StrDup ( pHTuner->pTls_ca_name);
	}

	if(pHTuner->pTls_ca_path != NULL)
	{
		pAccess->psz_ca_path = VK_StrDup ( pHTuner->pTls_ca_path);
	}

	if(pHTuner->pTls_certi_path != NULL)
	{
		pAccess->psz_certi_path = VK_StrDup ( pHTuner->pTls_certi_path);
	}

	if(pHTuner->pTls_prikey_path != NULL)
	{
		pAccess->psz_prikey_path = VK_StrDup ( pHTuner->pTls_prikey_path);
	}

	if(pHTuner->pTls_pripw != NULL)
	{
		pAccess->psz_prikey_passwd = VK_StrDup ( pHTuner->pTls_pripw);
	}

	if(pHTuner->cbTlsCertificate.cbVerify != NULL)
	{
		pAccess->cbTlsCerficate = (STREAM_VerifyExternalCallback_t*) DD_MEM_Alloc(sizeof(STREAM_VerifyExternalCallback_t));
		if(pAccess->cbTlsCerficate)
		{
			VK_memset (pAccess->cbTlsCerficate, 0, sizeof(STREAM_VerifyExternalCallback_t));
			pAccess->cbTlsCerficate->callback = &P_STREAM_HTTP_TlsCertificateCallback;
			pAccess->cbTlsCerficate->context  = (void*) pHTuner;
			pAccess->cbTlsCerficate->userdata = pHTuner->cbTlsCertificate.pUserdata;
			PrintDebug ("%s(%d) Register Tls Certificate callback (%u)\n",__FUNCTION__,__LINE__,(HUINT32)pAccess->cbTlsCerficate);
		}
		else
		{
			PrintError("%s(%d) Fail Register Tls Certificate Callback\n",__FUNCTION__,__LINE__);
		}
	}

	if(pAccess->psz_path == NULL)
	{
		PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
		ptHandle->ucAvailable = 0;
		vRet = VFIO_ERROR_IN_DEVICE;
		goto OPEN_ERROR;
	}

	/* support YouView Specific UserAgent String */
	if(pHTuner->pUserAgentString != NULL)
	{
		pAccess->psz_user_agent = VK_StrDup ((void *)pHTuner->pUserAgentString);
		if(pAccess->psz_user_agent == NULL)
		{
			PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
			ptHandle->ucAvailable = 0;
			vRet = VFIO_ERROR_IN_DEVICE;
			goto OPEN_ERROR;
		}
	}
	else /* error case : using default HUMAX UserAgent */
	{
		pAccess->psz_user_agent = VK_StrDup ((void *)DI_IPTUNER_USER_AGENT_STRING);
	}

	pAccess->callback = (STREAM_CallbackDesc*) DD_MEM_Alloc(sizeof(STREAM_CallbackDesc));
	if(pAccess->callback)
	{
		VK_memset (pAccess->callback, 0, sizeof(STREAM_CallbackDesc));
		pAccess->callback->callback = &P_HTTP_ResponseCallback;
		pAccess->callback->context  = (void*) pHTuner;
		PrintDebug ("%s(%d) Open callback (%u)\n",__FUNCTION__,__LINE__,(HUINT32)pAccess->callback);
		PrintDebug ("%s(%d) Open callback->callback (%u)\n",__FUNCTION__,__LINE__,(HUINT32)pAccess->callback->callback);
		PrintDebug ("%s(%d) >>>> Open callback->context  (%u)\n",__FUNCTION__,__LINE__,(HUINT32)pAccess->callback->context);
	}
	else
	{
		PrintError("%s(%d) Warning> can't allocate STREAM event callbacks\n",__FUNCTION__,__LINE__);
		PrintError("%s(%d) Warning> The backoff event can't be generated in stream core\n",__FUNCTION__,__LINE__);
		PrintError("%s(%d) Warning> Anyway continue...\n",__FUNCTION__,__LINE__);
	}

	/* Set DLNA Feature */
	if(pHTuner->tDlnaSetting.bDlna == TRUE)
	{
		if(pHTuner->tDlnaSetting.eSeekMode == DI_IPTUNER_DLNA_SEEKMODE_BYTESEEK)
		{
			pAccess->t_dlna.b_byte_seek = TRUE;
			pAccess->t_dlna.b_time_seek = FALSE;
		}
		else if(pHTuner->tDlnaSetting.eSeekMode == DI_IPTUNER_DLNA_SEEKMODE_TIMESEEK)
		{
			pAccess->t_dlna.b_byte_seek = FALSE;
			pAccess->t_dlna.b_time_seek = TRUE;
		}
		else
		{
			pAccess->t_dlna.b_byte_seek = FALSE;
			pAccess->t_dlna.b_time_seek = FALSE;
		}

		if(pHTuner->tDlnaSetting.eSpeed == TRUE)
		{
			pAccess->t_dlna.i_server_trickmode= TRUE;
		}
		else
		{
			pAccess->t_dlna.i_server_trickmode = FALSE;
		}

		if((pHTuner->tDlnaSetting.bLimitedTimeBasedSeek == TRUE) || (pHTuner->tDlnaSetting.bLimitedRangeBasedSeek == TRUE))
		{
			pAccess->t_dlna.b_limited_seek = TRUE;
		}
		else
		{
			pAccess->t_dlna.b_limited_seek = FALSE;
		}

		if(pHTuner->tDlnaSetting.nByteRate > 0 )
		{
			pAccess->t_dlna.i_byterate = pHTuner->tDlnaSetting.nByteRate;
		}
		else
		{
			pAccess->t_dlna.i_byterate = 0;
		}

		if(pHTuner->tDlnaSetting.unDuration > 0 )
		{
			pAccess->t_dlna.i_duration = pHTuner->tDlnaSetting.unDuration;
		}
		else
		{
			pAccess->t_dlna.i_duration = 0;
		}

		pAccess->t_dlna.b_dlna = TRUE;
	}

	if(DI_IPT_SUCCESS != STREAM_HTTP_Open (pAccess))
	{
		PrintError("%s(%d) : Error> HTTP_Open Fail.\n",__FUNCTION__,__LINE__);
		pAccess->b_alive = TRUE;
		ptHandle->ucAvailable = 0;

		vRet = VFIO_ERROR_IN_DEVICE;
	}
	else
	{
		pAccess->t_dtcpip.psz_pcp_packethandle = NULL;

		if(VK_strcmp(pAccess->psz_path, (void *) UTIL_GetOriginalURL ((const char *)pUrl)))
		{
			if(pInstance->pUrl != NULL)
			{
				VK_MEM_Free(pInstance->pUrl);
				pInstance->pUrl = NULL;
			}
			pInstance->pUrl = VK_StrDup((void *)pAccess->psz_path);
			PrintError("### %s(%d) : Redirection Url=%s ###\n",__FUNCTION__,__LINE__, pInstance->pUrl);
		}

		/*************************
		 * Succesful Open
		 *************************/
		PrintData ("%s(%d) : i_update       = %d \n",__FUNCTION__,__LINE__,pAccess->info.i_update);
		PrintData ("%s(%d) : i_size         = %"PRId64"\n",__FUNCTION__,__LINE__,pAccess->info.i_size);
		PrintData ("%s(%d) : i_pos          = %"PRId64"\n",__FUNCTION__,__LINE__,pAccess->info.i_pos);
		PrintData ("%s(%d) : b_eof          = %d \n",__FUNCTION__,__LINE__,pAccess->info.b_eof);
		PrintData ("%s(%d) : i_title        = %d \n",__FUNCTION__,__LINE__,pAccess->info.i_title);
		PrintData ("%s(%d) : i_seekpoint    = %d \n",__FUNCTION__,__LINE__,pAccess->info.i_seekpoint);
		PrintData ("%s(%d) : b_prebuffered  = %d \n",__FUNCTION__,__LINE__,pAccess->info.b_prebuffered);

		if((pAccess->info.i_size > 0) && (pAccess->info.i_size < 4*1024))
		{
			/******************
			 * Abnormal Case
			 ******************/
			PrintError("%s(%d) : Error> HTTP_Open Fail.\n",__FUNCTION__,__LINE__);
			PrintError("%s(%d) : Error> Invalid File Size(File Size = %"PRId64".\n",__FUNCTION__,__LINE__, pAccess->info.i_size);
			pAccess->b_alive = TRUE;
			ptHandle->ucAvailable = 0;

			// TODO: Redirection URL

			/* else { continue; } */

			vRet = VFIO_ERROR_IN_DEVICE;
		}
		else
		{
			pstHttpSys = (HTTP_ACCESS_SYS_t*) pAccess->p_sys;
			ptHandle->ucAvailable = 1;
			ptHandle->tFile.callback.unHandle = (HUINT32) pstHttpSys->fd; /** http socket descriptor */
			pHTuner->ulDNPFileLength = pAccess->info.i_size;
			PrintData ("%s(%d) : callback unhandle=%d\n",__FUNCTION__,__LINE__, ptHandle->tFile.callback.unHandle);


			/* InetRadio structure in Tuner is filled here */
			if(pstHttpSys->b_inet_radio == TRUE)
			{
				pHTuner->eInetRadioType = DI_IPTUNER_INETRADIO_TYPE_GENERIC;
				pHTuner->tInetRadioInfo.eInetRadioCodec = pstHttpSys->e_inetradio_codec;
				pHTuner->tInetRadioInfo.nReadUnit = 0;
				pHTuner->tInetRadioInfo.nInetRadioBitrate = pstHttpSys->i_icy_br;

				if (pstHttpSys->b_icecast == TRUE)
				{
					pHTuner->eInetRadioType = DI_IPTUNER_INETRADIO_TYPE_ICY;
					pHTuner->tInetRadioInfo.nReadUnit = pstHttpSys->i_icy_meta;
				}
			}
			else
			{
				pHTuner->eInetRadioType = DI_IPTUNER_INETRADIO_TYPE_NONE;
			}

			pHTuner->bHttpChunked = pstHttpSys->b_chunked;
			pHTuner->bRangeAccepted = pstHttpSys->b_range_accepted;

			PrintData ("%s(%d) : callback handle=0x%x\n",__FUNCTION__,__LINE__, ptHandle->tFile.callback.unHandle);

			nRet = VK_SEM_Create(&pAccess->ulStreamSema,"httpSema", DEFAULT_SUSPEND_TYPE);
			if( nRet != VK_OK )
			{
				PrintError("%s(%d) Error(0x%x) \n\r",__FUNCTION__,__LINE__, nRet);
				vRet = VFIO_ERROR_IN_DEVICE;
			}
			else
			{
				pHTuner->nProbe = (HUINT32) pstHttpSys->fd;
				pHTuner->pHStream = (void*) pAccess;
			}
		}
	}

OPEN_ERROR:
	if( vRet != VFIO_ERROR_SUCCESS)
	{
		PrintError("%s(%d) Error(0x%x) \n\r",__FUNCTION__,__LINE__, nRet);

		if(pAccess != NULL)
		{
			(void)STREAM_HTTP_Close (pAccess);

			if(pAccess->psz_path != NULL)
			{
				DD_MEM_Free(pAccess->psz_path);
				pAccess->psz_path =NULL;
			}
			/* else { continue; } */

			if(pAccess->psz_user_agent != NULL)
			{
				VK_MEM_Free(pAccess->psz_user_agent);
				pAccess->psz_user_agent =NULL;
			}
			/* else { continue; } */

#if SUPPORT_HUMAX_COOKIE
			if(pAccess->psz_cookie != NULL)
			{
				DD_MEM_Free(pAccess->psz_cookie);
				pAccess->psz_cookie =NULL;
			}
			/* else { continue; } */
#endif
			DD_MEM_Free(pAccess);
			pAccess = NULL;
		}
		PrintExit;
		return VFIO_ERROR_IN_DEVICE;
	}

	PrintExit;
	return vRet;
}

/** */
int DI_STREAM_HTTP_Close (VFIO_Handle_t tHandle)
{
	HINT32 vRet = VFIO_ERROR_SUCCESS;
	DI_IPTUNER_t *pHTuner=NULL;
	STREAM_ACCESS_t *pAccess=NULL;
	ADP_BUFFERMGR_t *pBufferMgr=NULL;
	RT_STREAMING_INSTANCE_t *pInstance=NULL;
	unsigned int *pFd = NULL;
	PrintEnter;
	/* Searching IPTUNER handle using URL String */
	pHTuner = tHandle.pTunerHandle;
	if (pHTuner == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Error>  Invalid Tuner Handle in VFIO Handle\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}

	pInstance = pHTuner->pRealTimeInstance;
	if(pInstance != NULL)
	{
		pBufferMgr = pInstance->pBufferMgr;
	}

	if((unsigned int)pHTuner->nData == tHandle.tFile.callback.unHandle)
	{
		if((STREAM_ACCESS_t *)pHTuner->pHStreamData != NULL)
		{
			pAccess = (STREAM_ACCESS_t *)pHTuner->pHStreamData;
			pHTuner->pHStreamData = NULL;
			pFd = &pHTuner->nData;
		}
		else
		{
			PrintError ("%s(%d) Error> tHandle->pHStream is null \n",__FUNCTION__,__LINE__);
			PrintExit;
			return VFIO_ERROR_IN_DEVICE;
		}

		if(pHTuner->tDlnaSetting.eSeekMode != DI_IPTUNER_DLNA_SEEKMODE_TIMESEEK)
		{
			(void)P_STREAM_HTTP_DestroyBufferingThread(pHTuner);
			if ((pHTuner->bHttpChunked == TRUE) && (pHTuner->bRangeAccepted == FALSE))
			{
				PrintDebug("%s(%d) HTTP chunked live streaming use CLS monitor, not UnderflowMonitor!\n",__FUNCTION__,__LINE__);
				;
			}
			else
			{
				DI_IPTUNER_UnderflowMonitorTerminateThread(&tHandle);
			}
		}
	}
	else if((unsigned int)pHTuner->nIndex == tHandle.tFile.callback.unHandle)
	{
		if((STREAM_ACCESS_t *)pHTuner->pHStreamIndex != NULL)
		{
			pAccess = (STREAM_ACCESS_t *)pHTuner->pHStreamIndex;
			pHTuner->pHStreamIndex = NULL;
			pFd = &pHTuner->nIndex;
		}
		else
		{
			PrintError ("%s(%d) Error> tHandle->pHStream is null \n",__FUNCTION__,__LINE__);
			PrintExit;
			return VFIO_ERROR_IN_DEVICE;
		}
	}
	else
	{
		if((STREAM_ACCESS_t *)pHTuner->pHStream != NULL)
		{
			pAccess = (STREAM_ACCESS_t *)pHTuner->pHStream;
			pHTuner->pHStream = NULL;
			pFd = &pHTuner->nProbe;
		}
		else
		{
			PrintError ("%s(%d) Error> tHandle->pHStream is null \n",__FUNCTION__,__LINE__);
			PrintExit;
			return VFIO_ERROR_IN_DEVICE;
		}
	}

	{
		if(VK_SEM_Get(pAccess->ulStreamSema) != VK_OK)
		{
			PrintError("%s(%d) Error> can't get ulStreamSema (tuner %d) (unHandle %d), Maybe this tuner wasn't opened.\n"
					,__FUNCTION__
					,__LINE__
					,(HUINT32)tHandle.pTunerHandle
					,tHandle.tFile.callback.unHandle);
			return VFIO_ERROR_FAIL;
		}

		PrintDebug ("%s(%d) : close s_PlayerHandle.\n",__FUNCTION__,__LINE__);

		if(pAccess->b_alive != FALSE)
		{
			(void) STREAM_HTTP_Close (pAccess);

			pAccess->b_alive = FALSE;

			if(pAccess->psz_path != NULL)
			{
				DD_MEM_Free(pAccess->psz_path);
				pAccess->psz_path =NULL;
			}
			/* else { continue; } */

			if(pAccess->callback != NULL)
			{
				DD_MEM_Free (pAccess->callback);
				pAccess->callback = NULL;
			}

#if SUPPORT_HUMAX_COOKIE
			if(pAccess->psz_cookie != NULL)
			{
				DD_MEM_Free(pAccess->psz_cookie);
				pAccess->psz_cookie =NULL;
			}
			/* else { continue; } */
#endif

			if(pAccess->psz_ca_name != NULL)
			{
				DD_MEM_Free(pAccess->psz_ca_name);
				pAccess->psz_ca_name = NULL;
			}

			if(pAccess->psz_ca_path != NULL)
			{
				DD_MEM_Free(pAccess->psz_ca_path);
				pAccess->psz_ca_path = NULL;
			}

			if(pAccess->psz_certi_path != NULL)
			{
				DD_MEM_Free(pAccess->psz_certi_path);
				pAccess->psz_certi_path = NULL;
			}

			if(pAccess->psz_prikey_path != NULL)
			{
				DD_MEM_Free(pAccess->psz_prikey_path);
				pAccess->psz_prikey_path = NULL;
			}

			if(pAccess->psz_prikey_passwd != NULL)
			{
				DD_MEM_Free(pAccess->psz_prikey_passwd);
				pAccess->psz_prikey_passwd = NULL;
			}

			if(pAccess->cbTlsCerficate != NULL)
			{
				pAccess->cbTlsCerficate->callback = NULL;
				DD_MEM_Free(pAccess->cbTlsCerficate);
				pAccess->cbTlsCerficate = NULL;
			}
#ifndef CONFIG_PRODUCT_IMAGE_HWTEST
			if(pAccess->t_dtcpip.psz_pcp_packethandle != NULL)
			{
				(void)DtcpApi_ClosePacket((DtcpPacketHandle)pAccess->t_dtcpip.psz_pcp_packethandle);
				pAccess->t_dtcpip.psz_pcp_packethandle = NULL;
			}
#endif
		}
		else
		{
			PrintDebug ("%s(%d) : HTTP Session is already closed.\n",__FUNCTION__,__LINE__);
		}

		*pFd = 0;
		(void)VK_SEM_Release (pAccess->ulStreamSema);
	}

	(void)VK_SEM_Destroy (pAccess->ulStreamSema);

	pAccess->ulStreamSema = 0;

	if(pAccess != NULL)
	{
		DD_MEM_Free (pAccess);
		pAccess = NULL;
	}

	PrintExit;
	return vRet;
}

/** */

int DI_STREAM_HTTP_Read (VFIO_Handle_t tHandle, void *ptr, size_t size, size_t nmemb, ssize_t *ptReadSize)
{
	ssize_t sz_Real=0;
	size_t sz_Read=0;
//	size_t sz_forRead=0;
	size_t sz_Try = size*nmemb;
	/* warning: unused variable */
	int 	ret = VFIO_ERROR_SUCCESS;
	HUINT64 Current_Pos =0;
	HUINT32 unCategory = 0;
	HUINT64 *pullOffset = NULL;
	DI_IPTUNER_t *pHTuner = NULL;
	STREAM_ACCESS_t *pAccess = NULL;
	ADP_BUFFERMGR_t *pBufferMgr=NULL;
	RT_STREAMING_INSTANCE_t *pInstance=NULL;
	ADP_BUFFER_UserData_t tUserData;
	HBOOL bRetain = FALSE;
	DI_IPTUNER_Msg_t s_Msg;
	PrintEnter;
	/* Searching IPTUNER handle using URL String */
	pHTuner = tHandle.pTunerHandle;
	if (pHTuner == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Error>  Invalid Tuner Handle in VFIO Handle\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	if(pHTuner->bProbeCancel == TRUE)
	{
		PrintDebug ("[%s] Probe Canceled!\n", __FUNCTION__);
		*ptReadSize = -1;
		return VFIO_ERROR_SUCCESS;
	}
	pInstance = pHTuner->pRealTimeInstance;
	if(pInstance == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Error> RealTime Instance is NULL\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	pBufferMgr = pInstance->pBufferMgr;
	if(pBufferMgr == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Error>  Buffer Manager is NULL\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	/**
	* VFIO Handle Type 에 따른 Handle 및 현재 Offset Setting
	*/
	if((unsigned int)pHTuner->nData == tHandle.tFile.callback.unHandle)
	{
		if((STREAM_ACCESS_t *)pHTuner->pHStreamData != NULL)
		{
			pAccess = (STREAM_ACCESS_t *)pHTuner->pHStreamData;
		}
		else
		{
			PrintError ("%s(%d) Error> tHandle->pHStream is null \n",__FUNCTION__,__LINE__);
			PrintExit;
			return VFIO_ERROR_IN_DEVICE;
		}
		pInstance->bDataReadBlock = TRUE;
		bRetain = TRUE;
		unCategory = RT_BUFFER_TYPE_DATA;
		pullOffset = &pInstance->ullCurrent_PlayOffset;
	}
	else if((unsigned int)pHTuner->nIndex == tHandle.tFile.callback.unHandle)
	{
		if((STREAM_ACCESS_t *)pHTuner->pHStreamIndex != NULL)
		{
			pAccess = (STREAM_ACCESS_t *)pHTuner->pHStreamIndex;
		}
		else
		{
			PrintError ("%s(%d) Error> tHandle->pHStream is null \n",__FUNCTION__,__LINE__);
			PrintExit;
			return VFIO_ERROR_IN_DEVICE;
		}
		unCategory = RT_BUFFER_TYPE_INDEX;
		pullOffset = &pInstance->ullCurrent_IndexOffset;
	}
	else
	{
		if((STREAM_ACCESS_t *)pHTuner->pHStream != NULL)
		{
			pAccess = (STREAM_ACCESS_t *)pHTuner->pHStream;
		}
		else
		{
			PrintError ("%s(%d) Error> tHandle->pHStream is null \n",__FUNCTION__,__LINE__);
			PrintExit;
			return VFIO_ERROR_IN_DEVICE;
		}
		unCategory = RT_BUFFER_TYPE_PROBE;
		pullOffset = &pInstance->ullCurrent_ProbeOffset;
	}
	if (VK_SEM_Get(pAccess->ulStreamSema) != VK_OK)
	{
		PrintError("%s(%d) Error> can't get ulStreamSema (tuner %d) (unHandle %d)\n"
		,__FUNCTION__
		,__LINE__
		,(HUINT32)tHandle.pTunerHandle
		,tHandle.tFile.callback.unHandle);
		return VFIO_ERROR_FAIL;
	}

	if(pHTuner->tDlnaSetting.eSeekMode != DI_IPTUNER_DLNA_SEEKMODE_TIMESEEK)
	{
		if(unCategory != RT_BUFFER_TYPE_PROBE)
		{
			(void) VK_memset (&s_Msg, 0x0, sizeof(s_Msg));
			s_Msg.unMsg = TUNER_RT_TRIGGERMSG_READ_START;
			s_Msg.unParam1 = unCategory;
			(void) VK_MSG_Send ( pHTuner->ulRealTimeTaskMsgQueue , (HUINT32 *)&s_Msg, sizeof(DI_IPTUNER_Msg_t));
		}
	}
	if(pullOffset == NULL)
	{
		PrintError("%s(%d) Error> can't get current offset\n",__FUNCTION__,__LINE__);
		ret = VFIO_ERROR_FAIL;
		goto read_done;
	}
	if(pAccess->info.i_size > 0 && ((HUINT64)pAccess->info.i_size < (*pullOffset + sz_Try)))
	{
		PrintError("%s(%d) Error> Not enough data (Left : %"PRId64", Try Read : %d, File Length : %"PRId64")\n",__FUNCTION__,__LINE__,(pAccess->info.i_size - *pullOffset),sz_Try,pAccess->info.i_size);
		sz_Try = pAccess->info.i_size - *pullOffset;
		PrintError("%s(%d) Error> Read Size Changed (%d)\n",__FUNCTION__,__LINE__,sz_Try);
		if(sz_Try <= 0)
		{
			PrintError("%s(%d) Error> Invalid Read Size\n",__FUNCTION__,__LINE__);
			ret = VFIO_ERROR_FAIL;
			goto read_done;
		}
	}

	Current_Pos = *pullOffset;

	if(pHTuner->tDlnaSetting.eSeekMode != DI_IPTUNER_DLNA_SEEKMODE_TIMESEEK)
	{
		/**
		* Buffer Manager 내에 현재 Offset에 해당하는 데이터가 존재하는 지 탐색 후
		* 가용한 데이터 복사
		*/
		if(pBufferMgr != NULL)
		{
			sz_Real = P_STREAM_HTTP_ReadFromBuffer(pHTuner,pBufferMgr,ptr,Current_Pos,sz_Try);
			Current_Pos += sz_Real;
			sz_Try -= sz_Real;
			if(sz_Try <= 0)
			{
				PrintError("%s(%d) : handle (0x%x) Read From Buffer(%d)\n",__FUNCTION__,__LINE__, tHandle.tFile.callback.unHandle,sz_Real);
				goto read_done;
			}
		}
	}

	/**
	* Buffer Manager 로 부터 데이터 복사 후 잔여 데이터를 네트워크를 통해 얻는다.
	* 전송 받기 전에 현재 Access Handle의 Offset 과 다운 받을 Offset이 같은지 확인 후 같지 않은 경우
	*
	* Case 1 : Offset 간의 Gap이 특정 값 이하인 경우 Reconnect 하지 않고 Data를 Read하여 Offset을 이동한다.
	* Case 2 : 1에 해당하지 않는 경우 Reconnect를 이용한 Seek을 시도한다.
	*/
	if(unCategory == RT_BUFFER_TYPE_DATA && pHTuner->bExitRead == TRUE)
	{
		PrintError("%s(%d) : handle (0x%x) Breaking Read\n",__FUNCTION__,__LINE__, pHTuner);
		pHTuner->bExitRead = FALSE;
		sz_Real = BFILE_ERROR_NO_DATA;
		goto read_done;
	}
	else if(unCategory == RT_BUFFER_TYPE_INDEX && pHTuner->bExitIndexRead == TRUE)
	{
		PrintError("%s(%d) : handle (0x%x) Breaking Read\n",__FUNCTION__,__LINE__, pHTuner);
		pHTuner->bExitIndexRead = FALSE;
		sz_Real = BFILE_ERROR_NO_DATA;
		goto read_done;
	}
	PrintDebug("%s(%d) : handle (0x%x) tryRead(%d) Buffered(%d) from %llu(%llu)\n",__FUNCTION__,__LINE__, tHandle.tFile.callback.unHandle,sz_Try, sz_Real, pAccess->info.i_pos,Current_Pos);
	if(pAccess->info.i_pos >= 0 && (HUINT64)pAccess->info.i_pos != Current_Pos)
	{
		if(pAccess->info.i_size > 0 && (Current_Pos > (HUINT64)pAccess->info.i_size))
		{
			PrintError("%s(%d) Position is INVALID\n",__FUNCTION__,__LINE__);
			Current_Pos -= (pAccess->info.i_size - sz_Try);
		}
		/**
		* Case #1 : Offset 간의 Gap이 특정 값 이하인 경우 Reconnect 하지 않고 Data를 Read하여 Offset을 이동한다.
		*          Gap에 대한 기준 필요.
		*/
		/* if((unsigned int)pHTuner->nData == tHandle.tFile.callback.unHandle) */
		{
			if(pHTuner->tDlnaSetting.eSeekMode != DI_IPTUNER_DLNA_SEEKMODE_TIMESEEK)
			{
				if((HUINT64)(pAccess->info.i_pos + BROADCOM_PLAYBACK_READ_BLOCK*30) > Current_Pos && Current_Pos > (HUINT64)pAccess->info.i_pos )
				{
					HINT64 ullResSeek = 0;
					PrintDebug("%s(%d) : handle (0x%x) Read for Seek %llu to %llu (Read : %llu)\n",__FUNCTION__,__LINE__, tHandle.tFile.callback.unHandle,pAccess->info.i_pos,Current_Pos,Current_Pos-pAccess->info.i_pos);
					VK_MEM_Memset(&tUserData,0x00,sizeof(ADP_BUFFER_UserData_t));
					tUserData.ulCategory = unCategory;
					tUserData.ullStartOffset = (HUINT64)pAccess->info.i_pos;
					ullResSeek = P_STREAM_HTTP_NetReadAndFillBuffer(pHTuner
									,pAccess
									,pBufferMgr
									,&tUserData
									,Current_Pos-pAccess->info.i_pos /* Read Size */
									,NULL /* Ptr Buffer */
									,0 /* Ptr Buffer Size */
									,bRetain /* Buffer Retain Flag */
									,0 /* Retain Base Offset */
									,TRUE /* Pass Through Flag */
									);
					if(ullResSeek < 0)
					{
						sz_Real = -1;
						ret = VFIO_ERROR_IN_DEVICE;
						goto read_error;
					}
	//				PrintError("Result %llu to %llu (Read : %llu)\n",pAccess->info.i_pos,Current_Pos,ullResOffset);
				}
			}
			else
			{
				if((HUINT64)(pAccess->info.i_pos + BROADCOM_PLAYBACK_READ_BLOCK*2) > Current_Pos && Current_Pos > (HUINT64)pAccess->info.i_pos )
				{
					HINT64 ullResSeek = 0;
					PrintDebug("%s(%d) : handle (0x%x) Read for Seek %llu to %llu (Read : %llu)\n",__FUNCTION__,__LINE__, tHandle.tFile.callback.unHandle,pAccess->info.i_pos,Current_Pos,Current_Pos-pAccess->info.i_pos);
					VK_MEM_Memset(&tUserData,0x00,sizeof(ADP_BUFFER_UserData_t));
					tUserData.ulCategory = unCategory;
					tUserData.ullStartOffset = (HUINT64)pAccess->info.i_pos;
					ullResSeek = P_STREAM_HTTP_NetReadAndFillBuffer(pHTuner
									,pAccess
									,pBufferMgr
									,&tUserData
									,Current_Pos-pAccess->info.i_pos /* Read Size */
									,NULL /* Ptr Buffer */
									,0 /* Ptr Buffer Size */
									,bRetain /* Buffer Retain Flag */
									,0 /* Retain Base Offset */
									,TRUE /* Pass Through Flag */
									);
					if(ullResSeek < 0)
					{
						sz_Real = -1;
						ret = VFIO_ERROR_IN_DEVICE;
						goto read_error;
					}
	//				PrintError("Result %llu to %llu (Read : %llu)\n",pAccess->info.i_pos,Current_Pos,ullResOffset);
				}
			}
		}
		/**
		* Case #2 : #1에 해당하지 않는 경우 Re-connect를 이용한 Seek을 시도한다.
		*/
		if(unCategory == RT_BUFFER_TYPE_DATA && pHTuner->bExitRead == TRUE)
		{
			PrintError("%s(%d) : handle (0x%x) Breaking Read\n",__FUNCTION__,__LINE__, pHTuner);
			pHTuner->bExitRead = FALSE;
			sz_Real = BFILE_ERROR_NO_DATA;
			goto read_done;
		}
		else if(unCategory == RT_BUFFER_TYPE_INDEX && pHTuner->bExitIndexRead == TRUE)
		{
			PrintError("%s(%d) : handle (0x%x) Breaking Read\n",__FUNCTION__,__LINE__, pHTuner);
			pHTuner->bExitIndexRead = FALSE;
			sz_Real = BFILE_ERROR_NO_DATA;
			goto read_done;
		}
		if((HUINT64)pAccess->info.i_pos != Current_Pos)
		{
			PrintDebug("%s(%d) : handle (0x%x) Real Seek (Read : %llu)\n",__FUNCTION__,__LINE__, tHandle.tFile.callback.unHandle, Current_Pos);
			if(DI_IPT_SUCCESS != P_STREAM_HTTP_SeekForRealTime(pHTuner,pAccess,Current_Pos))
			{
				PrintError ("%s(%d) Error> STREAM_HTTP_Seek fail... \n",__FUNCTION__,__LINE__);
				ret = VFIO_ERROR_FAIL;
				goto read_done;
			}
		}
	}
	/**
	* Buffer Manager 로 부터 데이터 복사 후 남은 잔여 데이터를 네트워크를 통해 얻는다.
	*/
	if(unCategory == RT_BUFFER_TYPE_DATA && pHTuner->bExitRead == TRUE)
	{
		PrintError("%s(%d) : handle (0x%x) Breaking Read\n",__FUNCTION__,__LINE__, pHTuner);
		pHTuner->bExitRead = FALSE;
		sz_Real = BFILE_ERROR_NO_DATA;
		goto read_done;
	}
	else if(unCategory == RT_BUFFER_TYPE_INDEX && pHTuner->bExitIndexRead == TRUE)
	{
		PrintError("%s(%d) : handle (0x%x) Breaking Read\n",__FUNCTION__,__LINE__, pHTuner);
		pHTuner->bExitIndexRead = FALSE;
		sz_Real = BFILE_ERROR_NO_DATA;
		goto read_done;
	}
	VK_MEM_Memset(&tUserData,0x00,sizeof(ADP_BUFFER_UserData_t));
	tUserData.ulCategory = unCategory;
	tUserData.ullStartOffset = Current_Pos;
	sz_Read	= P_STREAM_HTTP_NetReadAndFillBuffer(pHTuner
						,pAccess
						,pBufferMgr
						,&tUserData
						,sz_Try /* Read Size */
						,(HUINT8*)ptr+sz_Real /* Ptr Buffer */
						,sz_Try /* Ptr Buffer Size */
						,bRetain /* Buffer Retain Flag */
						,0 /* Retain Base Offset */
						,FALSE /* Don't Pass Through */
						);
	if(sz_Read > 0)
	{
		sz_Real += sz_Read;
	}
	else
	{
		sz_Real = -1;
		ret = VFIO_ERROR_IN_DEVICE;
		goto read_error;
	}
read_done:
	if( sz_Real > 0 )
	{
		*pullOffset = ( *pullOffset + sz_Real );
	}
	*ptReadSize = sz_Real;
	PrintDebug("%s(%d) : handle (0x%x) tryRead(%d) realRead(%d)\n",__FUNCTION__,__LINE__, tHandle.tFile.callback.unHandle,size*nmemb, sz_Real);

	if(pHTuner->tDlnaSetting.eSeekMode != DI_IPTUNER_DLNA_SEEKMODE_TIMESEEK)
	{
		if(unCategory != RT_BUFFER_TYPE_PROBE)
		{
			(void) VK_memset (&s_Msg, 0x0, sizeof(s_Msg));
			s_Msg.unMsg = TUNER_RT_TRIGGERMSG_READ_END;
			s_Msg.unParam1 = unCategory;
			(void) VK_MSG_Send ( pHTuner->ulRealTimeTaskMsgQueue, (HUINT32 *)&s_Msg, sizeof(DI_IPTUNER_Msg_t));
		}
	}
read_error:
	if((sz_Real <= 0) || (ret != VFIO_ERROR_SUCCESS))
	{
		*ptReadSize = sz_Real;
		PrintError ("%s(%d) Error> Fail Data Read(ret=%d,*ptReadSize=%d) \n",__FUNCTION__,__LINE__, ret, *ptReadSize);
	}

	(void)VK_SEM_Release (pAccess->ulStreamSema);
	if(unCategory == RT_BUFFER_TYPE_DATA)
	{
		pInstance->bDataReadBlock = FALSE;
	}
	PrintExit;
	return ret;
}

/** This Function called by bmedia probe only.*/
int DI_STREAM_HTTP_Seek (VFIO_Handle_t tHandle, off_t offset, int whence) /* Consider boundless live streaming */
{
	DI_IPTUNER_t *pHTuner = NULL;
	STREAM_ACCESS_t *pAccess = NULL;
	RT_STREAMING_INSTANCE_t *pInstance=NULL;

	PrintEnter;

	/* Searching IPTUNER handle using URL String */
	pHTuner = tHandle.pTunerHandle;
	if (pHTuner == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Error>  Invalid Tuner Handle in VFIO Handle\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}

	if(pHTuner->bProbeCancel == TRUE)
	{
		PrintDebug ("[%s] Probe Canceled!\n", __FUNCTION__);
		return VFIO_ERROR_SUCCESS;
	}

	pInstance = pHTuner->pRealTimeInstance;
	if(pInstance == NULL || pInstance->pBufferMgr == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Error>  RealTime Instance is NULL\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	if((STREAM_ACCESS_t *)pHTuner->pHStream != NULL)
	{
		pAccess = (STREAM_ACCESS_t *)pHTuner->pHStream;
	}
	else
	{
		PrintError ("%s(%d) Error> tHandle->pHStream is null \n",__FUNCTION__,__LINE__);
		PrintExit;
		return VFIO_ERROR_IN_DEVICE;
	}

	PrintDebug("%s(%d) : handle (0x%x) Seek to %"PRId64" (whence :%d) Total Size %"PRId64"\n",__FUNCTION__,__LINE__,pAccess, offset,whence,pAccess->info.i_size);

	if(VK_SEM_Get(pAccess->ulStreamSema) != VK_OK)
	{
		PrintError("%s(%d) Error> can't get ulStreamSema (tuner %d) (unHandle %d), Maybe this tuner wasn't opened.\n"
				,__FUNCTION__
				,__LINE__
				,(HUINT32)tHandle.pTunerHandle
				,tHandle.tFile.callback.unHandle);
		return VFIO_ERROR_FAIL;
	}

	if(whence == SEEK_END)
		offset = (off_t)pAccess->info.i_size-offset;

	if(pAccess->info.i_size < offset)
	{
		offset = (off_t)pAccess->info.i_size;
	}

#if 0 /* HUMAX DMS MP3문제로 인해 Workaround 처리함. */
	if((off_t)pAccess->info.i_size == offset)
	{
		ptInstance->ullCurrent_PlayCurPos = offset;
		(void)VK_SEM_Release (pAccess->ulStreamSema);
		PrintExit;
		return offset;
	}
#endif

	PrintDebug("== Probe Seek\n");

	pInstance->ullCurrent_ProbeOffset = offset;

	(void)VK_SEM_Release (pAccess->ulStreamSema);
	PrintExit;
	return VFIO_ERROR_SUCCESS;
}

/** */
int DI_STREAM_HTTP_Tell (VFIO_Handle_t tHandle, off_t *plCurOffset)
{
	HINT32 nRet = VFIO_ERROR_SUCCESS;
	DI_IPTUNER_t *pHTuner = NULL;
	RT_STREAMING_INSTANCE_t *pInstance=NULL;

	PrintEnter;

	/* Searching IPTUNER handle using URL String */
	pHTuner = tHandle.pTunerHandle;
	if (pHTuner == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Error>  Invalid Tuner Handle in VFIO Handle\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}

	pInstance = pHTuner->pRealTimeInstance;
	if(pInstance == NULL || pInstance->pBufferMgr == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Error>  RealTime Instance is NULL\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	if((unsigned int)pHTuner->nData == tHandle.tFile.callback.unHandle)
	{
		*plCurOffset = pInstance->ullCurrent_PlayOffset;
	}
	else if((unsigned int)pHTuner->nIndex == tHandle.tFile.callback.unHandle)
	{
		*plCurOffset = pInstance->ullCurrent_IndexOffset;
	}
	else
	{
		*plCurOffset = pInstance->ullCurrent_ProbeOffset;
	}

//	PrintDebug ("%s(%d) : i_pos (%"PRId64") nRet(%d)\n",__FUNCTION__,__LINE__,*plCurOffset, nRet);
//	PrintExit;
	return nRet;
}

/** if EOF is true, must be return -1 like feof*/
int DI_STREAM_HTTP_IsEOF (VFIO_Handle_t tHandle)
{
	HINT32 nRet = VFIO_ERROR_SUCCESS;
	DI_IPTUNER_t *pHTuner = NULL;
	STREAM_ACCESS_t *pAccess=NULL;

	PrintEnter;

	/* Searching IPTUNER handle using URL String */
	pHTuner = tHandle.pTunerHandle;
	if (pHTuner == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Error>  Invalid Tuner Handle in VFIO Handle\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}

	if((unsigned int)pHTuner->nData == tHandle.tFile.callback.unHandle)
	{
		if((STREAM_ACCESS_t *)pHTuner->pHStreamData != NULL)
		{
			pAccess = (STREAM_ACCESS_t *)pHTuner->pHStreamData;
		}
		else
		{
			PrintError ("%s(%d) Error> tHandle->pHStream is null \n",__FUNCTION__,__LINE__);
			PrintExit;
			return VFIO_ERROR_IN_DEVICE;
		}

	    if(TRUE == pAccess->info.b_eof)
		{
			nRet = -1; /* must be -1 like feof*/
		}
	}
	else if((unsigned int)pHTuner->nIndex == tHandle.tFile.callback.unHandle)
	{
		if((STREAM_ACCESS_t *)pHTuner->pHStreamIndex != NULL)
		{
			pAccess = (STREAM_ACCESS_t *)pHTuner->pHStreamIndex;
		}
		else
		{
			PrintError ("%s(%d) Error> tHandle->pHStream is null \n",__FUNCTION__,__LINE__);
			PrintExit;
			return VFIO_ERROR_IN_DEVICE;
		}

	    if(TRUE == pAccess->info.b_eof)
		{
			nRet = -1; /* must be -1 like feof*/
		}
	}
	else
	{
		if((STREAM_ACCESS_t *)pHTuner->pHStream != NULL)
		{
			pAccess = (STREAM_ACCESS_t *)pHTuner->pHStream;
		}
		else
		{
			PrintError ("%s(%d) Error> tHandle->pHStream is null \n",__FUNCTION__,__LINE__);
			PrintExit;
			return VFIO_ERROR_IN_DEVICE;
		}

	    if(TRUE == pAccess->info.b_eof)
		{
			nRet = -1; /* must be -1 like feof*/
		}
	}

	PrintExit;
	return nRet;
}

/* Open From Nexus */

int DI_STREAM_HTTP_OpenFromNexus (const HINT8 *pUrl, const unsigned int flags, VFIO_Handle_t *ptHandle)
{
	HINT32 nRet = DI_IPT_SUCCESS;
	HTTP_ACCESS_SYS_t *pstHttpSys=NULL;
#if SUPPORT_HUMAX_COOKIE
	char *psCookie = NULL;
	URL_t tUrl;
#endif
	DI_IPTUNER_t *pHTuner=NULL;
	STREAM_ACCESS_t *pAccess=NULL;

	ADP_BUFFERMGR_t* pBufferMgr = NULL;
	RT_STREAMING_INSTANCE_t *pInstance=NULL;

	UNUSED_PARAM (flags);

	PrintEnter;

	/* valid check */
	if(pUrl==NULL)
	{
		ptHandle->ucAvailable = 0;
		PrintError("%s(%d) : Error> invalid pUrl(%s)\n",__FUNCTION__,__LINE__,pUrl);
		return VFIO_ERROR_IN_DEVICE;
	}

	/* Searching IPTUNER handle using URL String */
	pHTuner = DI_IPTUNER_POOL_FindByUrl (pUrl);
	if(pHTuner == NULL)
	{
		ptHandle->ucAvailable = 0;
		PrintError ("%s(%d) : Error> can't find iptuner handler by this URL(%s)\n",__FUNCTION__,__LINE__,pUrl);
		return VFIO_ERROR_IN_DEVICE;
	}
	pInstance = pHTuner->pRealTimeInstance;
	if(pInstance == NULL)
	{
		ptHandle->ucAvailable = 0;
		PrintError ("%s (%d) Error> RealTime Instance is NULL\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	pBufferMgr = pInstance->pBufferMgr;
	if(pBufferMgr == NULL)
	{
		ptHandle->ucAvailable = 0;
		PrintError ("%s (%d) Error>  Buffer Manager is NULL\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	pAccess = (STREAM_ACCESS_t*) DD_MEM_Alloc(sizeof(STREAM_ACCESS_t));
	if(pAccess == NULL)
	{
		PrintError("%s(%d) Error> can't allocate STREAM handler\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	else
	{
		VK_MEM_Memset(pAccess,0,sizeof(STREAM_ACCESS_t));
	}

#if SUPPORT_HUMAX_COOKIE
	VK_MEM_Memset(&tUrl, 0x0, sizeof(tUrl));
	UTIL_UrlParse(&tUrl, (const char *)pUrl, 0);
	if(tUrl.psz_cookie != NULL)
	{
		psCookie = VK_StrDup((void *)tUrl.psz_cookie);
	}
	UTIL_UrlClean(&tUrl);
#endif
	ptHandle->pTunerHandle = (void*) pHTuner;

	/* Stream Buffering 시 Probe 를 위한 Open 과 Playback을 위한 Open 을 구분하기 위한 용도 */
	if(pHTuner->nData == 0) 	/* open for Player */
	{
		// TODO: Configuration 사용하여  정리 할 것 by  ykkim3
		ADP_BUFFERMGR_Resize(pBufferMgr,pHTuner->uiRambufferSize);
		pHTuner->pHStreamData = (void*) pAccess;
		PrintDebug("openFromNexus for Player\n\n");
	}
	else if((pHTuner->nData != 0) && (pHTuner->nIndex == 0)) 	/* open for Indexer */
	{
		pHTuner->pHStreamIndex = (void*) pAccess;
		PrintDebug("openFromNexus for Indexer\n\n");
	}
	else
	{
		PrintError("%s (%d) : Invalid Open.\n",__FUNCTION__,__LINE__);
#if SUPPORT_HUMAX_COOKIE
		if(psCookie != NULL)
		{
			DD_MEM_Free(psCookie);
			psCookie = NULL;
		}
#endif
		if(pAccess != NULL)
		{
			DD_MEM_Free(pAccess);
		}
		return VFIO_ERROR_IN_DEVICE;
	}

	{
		/* open for player */
		/** psz_path must be free */
		if(pInstance->pUrl != NULL)
		{
			pAccess->psz_path = VK_StrDup((void *)pInstance->pUrl);
		}
		else
		{
			/* open for player */
			/** psz_path must be free */
			pAccess->psz_path = VK_StrDup ((void *) UTIL_GetOriginalURL ((const char *)pUrl) );
		}

#if SUPPORT_HUMAX_COOKIE
		if(psCookie != NULL)
		{
			pAccess->psz_cookie = psCookie;
		}
#endif

		pAccess->callback = (STREAM_CallbackDesc*) DD_MEM_Alloc(sizeof(STREAM_CallbackDesc));
		if(pAccess->callback)
		{
			VK_memset (pAccess->callback, 0, sizeof(STREAM_CallbackDesc));
			pAccess->callback->callback = &P_HTTP_ResponseCallback;
			pAccess->callback->context  = (void*) pHTuner;
			PrintDebug ("%s(%d) Open callback (%u)\n",__FUNCTION__,__LINE__,(HUINT32)pAccess->callback);
			PrintDebug ("%s(%d) Open callback->callback (%u)\n",__FUNCTION__,__LINE__,(HUINT32)pAccess->callback->callback);
			PrintDebug("%s(%d) >>>> Open callback->context  (%u)\n",__FUNCTION__,__LINE__,(HUINT32)pAccess->callback->context);
		}
		else
		{
			PrintError("%s(%d) Warning> can't allocate STREAM event callbacks\n",__FUNCTION__,__LINE__);
			PrintError("%s(%d) Warning> The backoff event can't be generated in stream core\n",__FUNCTION__,__LINE__);
			PrintError("%s(%d) Warning> Anyway continue...\n",__FUNCTION__,__LINE__);
		}

		if(pHTuner->pTls_ca_name != NULL)
		{
			pAccess->psz_ca_name = VK_StrDup ( pHTuner->pTls_ca_name);
		}

		if(pHTuner->pTls_ca_path != NULL)
		{
			pAccess->psz_ca_path = VK_StrDup ( pHTuner->pTls_ca_path);
		}

		if(pHTuner->pTls_certi_path != NULL)
		{
			pAccess->psz_certi_path = VK_StrDup ( pHTuner->pTls_certi_path);
		}

		if(pHTuner->pTls_prikey_path != NULL)
		{
			pAccess->psz_prikey_path = VK_StrDup ( pHTuner->pTls_prikey_path);
		}

		if(pHTuner->pTls_pripw != NULL)
		{
			pAccess->psz_prikey_passwd = VK_StrDup ( pHTuner->pTls_pripw);
		}

		if(pHTuner->cbTlsCertificate.cbVerify != NULL)
		{
			pAccess->cbTlsCerficate = (STREAM_VerifyExternalCallback_t*) DD_MEM_Alloc(sizeof(STREAM_VerifyExternalCallback_t));
			if(pAccess->cbTlsCerficate)
			{
				VK_memset (pAccess->cbTlsCerficate, 0, sizeof(STREAM_VerifyExternalCallback_t));
				pAccess->cbTlsCerficate->callback = &P_STREAM_HTTP_TlsCertificateCallback;
				pAccess->cbTlsCerficate->context  = (void*) pHTuner;
				pAccess->cbTlsCerficate->userdata = pHTuner->cbTlsCertificate.pUserdata;
				PrintDebug ("%s(%d) Register Tls Certificate callback (%u)\n",__FUNCTION__,__LINE__,(HUINT32)pAccess->cbTlsCerficate);
			}
			else
			{
				PrintError("%s(%d) Fail Register Tls Certificate Callback\n",__FUNCTION__,__LINE__);
			}
		}

		if(pAccess->psz_path == NULL)
		{
			PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
			nRet = VFIO_ERROR_IN_DEVICE;
		}
		else
		{
#if 1
			pAccess->psz_user_agent = VK_StrDup ((void *)DI_IPTUNER_USER_AGENT_STRING);

			/* Set DLNA Feature */
			if(pHTuner->tDlnaSetting.bDlna == TRUE)
			{
				if(pHTuner->tDlnaSetting.eSeekMode == DI_IPTUNER_DLNA_SEEKMODE_BYTESEEK)
				{
					pAccess->t_dlna.b_byte_seek = TRUE;
					pAccess->t_dlna.b_time_seek = FALSE;
				}
				else if(pHTuner->tDlnaSetting.eSeekMode == DI_IPTUNER_DLNA_SEEKMODE_TIMESEEK)
				{
					pAccess->t_dlna.b_byte_seek = FALSE;
					pAccess->t_dlna.b_time_seek = TRUE;
				}
				else
				{
					pAccess->t_dlna.b_byte_seek = FALSE;
					pAccess->t_dlna.b_time_seek = FALSE;
				}

				if(pHTuner->tDlnaSetting.eSpeed == TRUE)
				{
					pAccess->t_dlna.i_server_trickmode= TRUE;
				}
				else
				{
					pAccess->t_dlna.i_server_trickmode = FALSE;
				}

				if((pHTuner->tDlnaSetting.bLimitedTimeBasedSeek == TRUE) || (pHTuner->tDlnaSetting.bLimitedRangeBasedSeek == TRUE))
				{
					pAccess->t_dlna.b_limited_seek = TRUE;
				}
				else
				{
					pAccess->t_dlna.b_limited_seek = FALSE;
				}

				if(pHTuner->tDlnaSetting.nByteRate > 0 )
				{
					pAccess->t_dlna.i_byterate = pHTuner->tDlnaSetting.nByteRate;
				}
				else
				{
					pAccess->t_dlna.i_byterate = 0;
				}

				if(pHTuner->tDlnaSetting.unDuration > 0 )
				{
					pAccess->t_dlna.i_duration = pHTuner->tDlnaSetting.unDuration;
				}
				else
				{
					pAccess->t_dlna.i_duration = 0;
				}

				pAccess->t_dlna.b_dlna = TRUE;
			}

			nRet = STREAM_HTTP_Open (pAccess);
			if(nRet == DI_IPT_SUCCESS)
			{
				pstHttpSys = (HTTP_ACCESS_SYS_t *)pAccess->p_sys;
				ptHandle->tFile.callback.unHandle = (HUINT32) pstHttpSys->fd;
				if(pHTuner->nData == 0) 	/* open for Player */
				{
					pHTuner->nData = (HUINT32) pstHttpSys->fd;
				}
				else if((pHTuner->nData != 0) && (pHTuner->nIndex == 0)) 	/* open for Indexer */
				{
					pHTuner->nIndex = (HUINT32) pstHttpSys->fd;
				}

				ptHandle->ucAvailable = 1;

				pAccess->t_dtcpip.psz_pcp_packethandle = NULL;

				PrintData ("%s(%d) : callback handle=0x%x\n",__FUNCTION__,__LINE__, ptHandle->tFile.callback.unHandle);
			}
#endif
		}

		if(nRet != VFIO_ERROR_SUCCESS)
		{
			PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
			ptHandle->ucAvailable = 0;

			if(pAccess->callback != NULL)
			{
				DD_MEM_Free (pAccess->callback);
				pAccess->callback = NULL;
			}

			if(pAccess->psz_path != NULL)
			{
				DD_MEM_Free(pAccess->psz_path);
				pAccess->psz_path =NULL;
			}
			/* else { continue; } */
#if SUPPORT_HUMAX_COOKIE
			if(pAccess->psz_cookie != NULL)
			{
				DD_MEM_Free(pAccess->psz_cookie);
				pAccess->psz_cookie =NULL;
			}
			/* else { continue; } */
#endif

			if(pAccess->psz_ca_name != NULL)
			{
				DD_MEM_Free(pAccess->psz_ca_name);
				pAccess->psz_ca_name = NULL;
			}

			if(pAccess->psz_ca_path != NULL)
			{
				DD_MEM_Free(pAccess->psz_ca_path);
				pAccess->psz_ca_path = NULL;
			}

			if(pAccess->psz_certi_path != NULL)
			{
				DD_MEM_Free(pAccess->psz_certi_path);
				pAccess->psz_certi_path = NULL;
			}

			if(pAccess->psz_prikey_path != NULL)
			{
				DD_MEM_Free(pAccess->psz_prikey_path);
				pAccess->psz_prikey_path = NULL;
			}

			if(pAccess->psz_prikey_passwd != NULL)
			{
				DD_MEM_Free(pAccess->psz_prikey_passwd);
				pAccess->psz_prikey_passwd = NULL;
			}

			if(pAccess->cbTlsCerficate != NULL)
			{
				pAccess->cbTlsCerficate->callback = NULL;
				DD_MEM_Free(pAccess->cbTlsCerficate);
				pAccess->cbTlsCerficate = NULL;
			}

			if(pAccess != NULL)
			{
				(void)STREAM_HTTP_Close (pAccess);
				VK_MEM_Free(pAccess);
				pAccess = NULL;
			}
			nRet = VFIO_ERROR_IN_DEVICE;
			PrintExit;
			return nRet;
		}
		//---------------------------------------
	}

	if( VK_OK != VK_SEM_Create(&pAccess->ulStreamSema,"httpSema", DEFAULT_SUSPEND_TYPE))
	{
		(void)STREAM_HTTP_Close (pAccess);
		if(pAccess->psz_path != NULL)
		{
			VK_MEM_Free(pAccess->psz_path);
			pAccess->psz_path =NULL;
		}

		if(pAccess->psz_user_agent != NULL)
		{
			VK_MEM_Free  (pAccess->psz_user_agent);
			pAccess->psz_user_agent =NULL;
		}
		/* else { continue; } */
#if SUPPORT_HUMAX_COOKIE
		if(pAccess->psz_cookie != NULL)
		{
			DD_MEM_Free(pAccess->psz_cookie);
			pAccess->psz_cookie =NULL;
		}
		/* else { continue; } */
#endif

		if(pAccess->callback != NULL)
		{
			DD_MEM_Free (pAccess->callback);
			pAccess->callback = NULL;
		}

		if(pAccess->psz_ca_name != NULL)
		{
			DD_MEM_Free(pAccess->psz_ca_name);
			pAccess->psz_ca_name = NULL;
		}

		if(pAccess->psz_ca_path != NULL)
		{
			DD_MEM_Free(pAccess->psz_ca_path);
			pAccess->psz_ca_path = NULL;
		}

		if(pAccess->psz_certi_path != NULL)
		{
			DD_MEM_Free(pAccess->psz_certi_path);
			pAccess->psz_certi_path = NULL;
		}

		if(pAccess->psz_prikey_path != NULL)
		{
			DD_MEM_Free(pAccess->psz_prikey_path);
			pAccess->psz_prikey_path = NULL;
		}

		if(pAccess->psz_prikey_passwd != NULL)
		{
			DD_MEM_Free(pAccess->psz_prikey_passwd);
			pAccess->psz_prikey_passwd = NULL;
		}

		if(pAccess->cbTlsCerficate != NULL)
		{
			pAccess->cbTlsCerficate->callback = NULL;
			DD_MEM_Free(pAccess->cbTlsCerficate);
			pAccess->cbTlsCerficate = NULL;
		}

		PrintError ("%s(%d) Error> can't create semaphore httpSema",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}

	if(pHTuner->tDlnaSetting.eSeekMode != DI_IPTUNER_DLNA_SEEKMODE_TIMESEEK)
	{
		(void)P_STREAM_HTTP_CreateBufferingThread(pHTuner);
		if ((pHTuner->bHttpChunked == TRUE) && (pHTuner->bRangeAccepted == FALSE))
		{
			PrintDebug("%s(%d) HTTP chunked live streaming use CLS monitor, not UnderflowMonitor!\n",__FUNCTION__,__LINE__);
			;
		}
		else
		{
			if((unsigned int)pHTuner->nData == ptHandle->tFile.callback.unHandle)
			{
				DI_IPTUNER_UnderflowMonitorInitThread(ptHandle);
			}
		}
	}

	PrintExit;
	return VFIO_ERROR_SUCCESS;
}

/* Seek From Nexus : Long jump 인 경우 어느정도는 block 되어도 된다.
 * 단 Read 함수에서는 block 되면 waiting IO error, 0 return 시 endofStream, -1 return시 errorofstream 이 뜬다.*/

off_t DI_STREAM_HTTP_SeekForNexus (VFIO_Handle_t tHandle, off_t offset, int whence) /* Consider unbound live streaming */
{
	HINT32 nRet = VFIO_ERROR_SUCCESS;
	DI_IPTUNER_t *pHTuner=NULL;
	STREAM_ACCESS_t *pAccess=NULL;
	RT_STREAMING_INSTANCE_t *pInstance=NULL;

	PrintEnter;

	UNUSED_PARAM(whence);

	/* Searching IPTUNER handle using URL String */
	pHTuner = tHandle.pTunerHandle;
	if (pHTuner == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Error>  Invalid Tuner Handle in VFIO Handle\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}

	if(pHTuner->bProbeCancel == TRUE)
	{
		PrintDebug ("[%s] Probe Canceled!\n", __FUNCTION__);
		return VFIO_ERROR_SUCCESS;
	}
	pInstance = pHTuner->pRealTimeInstance;
	if(pInstance == NULL || pInstance->pBufferMgr == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Error>  RealTime Instance is NULL\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	if((unsigned int)pHTuner->nData == tHandle.tFile.callback.unHandle)
	{
		if((STREAM_ACCESS_t *)pHTuner->pHStreamData != NULL)
		{
			pAccess = (STREAM_ACCESS_t *)pHTuner->pHStreamData;
		}
		else
		{
			PrintError ("%s(%d) Error> tHandle->pHStream is null \n",__FUNCTION__,__LINE__);
			PrintExit;
			return VFIO_ERROR_IN_DEVICE;
		}
	}
	else if((unsigned int)pHTuner->nIndex == tHandle.tFile.callback.unHandle)
	{
		if((STREAM_ACCESS_t *)pHTuner->pHStreamIndex != NULL)
		{
			pAccess = (STREAM_ACCESS_t *)pHTuner->pHStreamIndex;
		}
		else
		{
			PrintError ("%s(%d) Error> tHandle->pHStream is null \n",__FUNCTION__,__LINE__);
			PrintExit;
			return VFIO_ERROR_IN_DEVICE;
		}
	}
	else
	{
		PrintError ("%s(%d) Error> Invalid Handle \n",__FUNCTION__,__LINE__);
		PrintExit;
		return VFIO_ERROR_IN_DEVICE;
	}
	if(VK_SEM_Get(pAccess->ulStreamSema) != VK_OK)
	{
		PrintError("%s(%d) Error> can't get ulStreamSema (tuner %d) (unHandle %d), Maybe this tuner wasn't opened.\n"
				,__FUNCTION__
				,__LINE__
				,(HUINT32)tHandle.pTunerHandle
				,tHandle.tFile.callback.unHandle);
		return VFIO_ERROR_FAIL;
	}

	if(whence == SEEK_END)
		offset = (off_t)pAccess->info.i_size-offset;

	if(pAccess->info.i_size < offset)
	{
		offset = (off_t)pAccess->info.i_size;
	}

	if((unsigned int)pHTuner->nData == tHandle.tFile.callback.unHandle)
	{
		pInstance->ullCurrent_PlayOffset = offset;
	}
	else if((unsigned int)pHTuner->nIndex == tHandle.tFile.callback.unHandle)
	{
		pInstance->ullCurrent_IndexOffset = offset;
	}
	else
	{
		PrintError ("%s(%d) Error> STREAM_HTTP_Seek fail \n",__FUNCTION__,__LINE__);
		PrintExit;
		nRet = -1;
	}
	(void)VK_SEM_Release (pAccess->ulStreamSema);
	return offset;
}

/* Bounds From Nexus */
int DI_STREAM_HTTP_BoundsForNexus (VFIO_Handle_t tHandle, off_t *file_size)
{
	HINT32 nRet = VFIO_ERROR_SUCCESS;
	DI_IPTUNER_t        *pHTuner  = NULL;
	STREAM_ACCESS_t *pAccess=NULL;

	PrintEnter;

	/* Searching IPTUNER handle using URL String */
	pHTuner = tHandle.pTunerHandle;

	if (pHTuner == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Error>  Invalid Tuner Handle in VFIO Handle\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}

	if(pHTuner->bProbeCancel == TRUE)
	{
		PrintDebug ("[%s] Probe Canceled!\n", __FUNCTION__);
		*file_size = 0;
		return VFIO_ERROR_SUCCESS;
	}

	if((unsigned int)pHTuner->nData == tHandle.tFile.callback.unHandle)
	{
		if((STREAM_ACCESS_t *)pHTuner->pHStreamData != NULL)
		{
			pAccess = (STREAM_ACCESS_t *)pHTuner->pHStreamData;
		}
		else
		{
			PrintError ("%s(%d) Error> tHandle->pHStream is null \n",__FUNCTION__,__LINE__);
			PrintExit;
			return VFIO_ERROR_IN_DEVICE;
		}

		*file_size = pAccess->info.i_size;
	}
	else if((unsigned int)pHTuner->nIndex == tHandle.tFile.callback.unHandle)
	{
		if((STREAM_ACCESS_t *)pHTuner->pHStreamIndex != NULL)
		{
			pAccess = (STREAM_ACCESS_t *)pHTuner->pHStreamIndex;
		}
		else
		{
			PrintError ("%s(%d) Error> tHandle->pHStream is null \n",__FUNCTION__,__LINE__);
			PrintExit;
			return VFIO_ERROR_IN_DEVICE;
		}

		*file_size = pAccess->info.i_size;
	}
	else
	{
		if((STREAM_ACCESS_t *)pHTuner->pHStream != NULL)
		{
			pAccess = (STREAM_ACCESS_t *)pHTuner->pHStream;
		}
		else
		{
			PrintError ("%s(%d) Error> tHandle->pHStream is null \n",__FUNCTION__,__LINE__);
			PrintExit;
			return VFIO_ERROR_IN_DEVICE;
		}

		*file_size = pAccess->info.i_size;
	}

#if 0
	PrintData ("%s(%d) : i_size=%"PRId64"\n",__FUNCTION__,__LINE__, stHttpAccess.info.i_size);
#endif

	PrintExit;
	return nRet;
}

void DI_STREAM_HTTP_UnregisterCallback (const char *pUrl)
{
	HINT32 nRet=0;
	HUINT32 nWaitingTime = 0;
	DI_IPTUNER_t *pHTuner=NULL;

	PrintEnter;

	/* Searching IPTUNER handle using URL String */
	pHTuner = DI_IPTUNER_POOL_FindByUrl ((const HINT8 *)pUrl);
	if(pHTuner == NULL)
	{
		PrintError("%s(%d) : Error> can't find iptuner handler by this URL(%s)\n",__FUNCTION__,__LINE__,pUrl);
		return;
	}

	while((pHTuner->nData != 0) || (pHTuner->nIndex != 0))
	{
		if(nWaitingTime != 5000)
		{
			PrintDebug("[%s] wait cloase(DI_STREAM_HTTP_UnregisterCallback)\n", __FUNCTION__);
			VK_TASK_Sleep(100);
			nWaitingTime=nWaitingTime+100;
		}
		else
		{
			PrintError("Fail close DI_STREAM_HTTP_UnregisterCallback\n");
			break;
		}
	}

	PrintDebug("[%s] PlayerHandle=%d, IndexHandle=%d \n", __FUNCTION__, pHTuner->nData, pHTuner->nIndex);

	nRet = VFIO_UnregisterCallback (pUrl);
	if(nRet<0)
	{
		PrintError("%s(%d) Error> DI_STREAM_HTTP_UnregisterCallback Fail (%s)\n",__FUNCTION__,__LINE__,pUrl);
	}

	PrintExit;
	return;
}

DI_ERR_CODE DI_STREAM_HTTP_Control ( DI_IPTUNER_CMD_e cmd, HUINT64 val, HUINT64 val2 )
{
	DI_ERR_CODE nRet=DI_ERR_OK;
	PrintEnter;

/* XXX : This function does not work on HTTP Controller yet. So .... */
#if 0
	nRet = STREAM_HTTP_Control (&stHttpAccess, cmd , val, val2);
	if(nRet != DI_ERR_OK)
	{
		PrintError("%s (%d) Error> STREAM_HTTP_Control Fail \n",__FUNCTION__,__LINE__);
	}
#else
	UNUSED_PARAM(cmd);
	UNUSED_PARAM(val);
	UNUSED_PARAM(val2);
#endif

	PrintExit;

	return nRet;
}

/* Please Check this : XXX */
int DI_STREAM_HTTP_ChangeUrl (VFIO_Handle_t tHandle, const HINT8 *pUrl)
{
	HTTP_ACCESS_SYS_t   *pHttpSys = NULL;
	STREAM_ACCESS_t     *pAccess  = NULL;
	DI_IPTUNER_t        *pHTuner  = NULL;

	PrintEnter;

	/* Searching IPTUNER handle using URL String */
#if 0
	pHTuner = DI_IPTUNER_POOL_FindByVfioHandle (&tHandle);
#else
	pHTuner = tHandle.pTunerHandle;
#endif
	if (pHTuner == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Error>  Invalid Tuner Handle in VFIO Handle\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}

	/* Now we just get the Stream handler */
	pAccess = (STREAM_ACCESS_t *)pHTuner->pHStream;
	if (NULL == pAccess)
	{
		PrintError("%s(%d) Error> Tuner handle doesn't include correct STREAM Handler. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return VFIO_ERROR_FAIL;
	}

	if(pAccess->psz_path == NULL)
	{
		return VFIO_ERROR_FAIL;
	}

	pHttpSys = (HTTP_ACCESS_SYS_t*)pAccess->p_sys;
	UTIL_UrlParse ( &pHttpSys->url, (const char *)pUrl, 0 );
	DI_STREAM_HTTP_Close (tHandle);
	DI_STREAM_HTTP_Open (pUrl,NULL,&tHandle);

	PrintDebug("== Set Url : %s\n",pUrl);

	return VFIO_ERROR_SUCCESS;
}

/* Back-Off Algo. */
int DI_STREAM_HTTP_ForcedExit ( void* pHStream )
{
	STREAM_ACCESS_t *pAccess = NULL;
	PrintEnter;
	if(pHStream == NULL)
	{
		// PrintError ("%s (%d) Error> Stream Access Handle is NULL.\n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pAccess = (STREAM_ACCESS_t*)pHStream;
	pAccess->b_immediately_exit = TRUE;
	while( pAccess->b_retrying )
	{
		PrintError ("%s (%d) Debug> Waiting......until stopping HTTP Stream_Core Retry logic(Back-Off).!\n",__FUNCTION__,__LINE__);
		VK_TASK_Sleep(100);
	}
	PrintExit;
	return DI_ERR_OK;
}

/** Open From User Application and bmedia (Probe)*/
int DI_STREAM_SIMPLE_HTTP_Open  (const HINT8 *pUrl, const HINT8 *mode, VFIO_Handle_t *ptHandle )
{
	HINT32 vRet = VFIO_ERROR_SUCCESS;
	HTTP_ACCESS_SYS_t *pstHttpSys=NULL;
#if SUPPORT_HUMAX_COOKIE
	char *psCookie = NULL;
	URL_t tUrl;
#endif
	DI_IPTUNER_t *pHTuner=NULL;
	STREAM_ACCESS_t *pAccess=NULL;

	UNUSED_PARAM (mode);
	PrintEnter;

	/* valid check */
	if(pUrl==NULL)
	{
		ptHandle->ucAvailable = 0;
		PrintError("%s(%d) : Error> invalid pUrl(%s)\n",__FUNCTION__,__LINE__,pUrl);
		return VFIO_ERROR_IN_DEVICE;
	}

	/* Searching IPTUNER handle using URL String */
	pHTuner = DI_IPTUNER_POOL_FindByUrl (pUrl);
	if(pHTuner == NULL)
	{
		ptHandle->ucAvailable = 0;
		PrintError("%s(%d) : Error> can't find iptuner handler by this URL(%s)\n",__FUNCTION__,__LINE__,pUrl);
		return VFIO_ERROR_IN_DEVICE;
	}

	/* Make Stream Core Handler */
	pAccess = (STREAM_ACCESS_t*)DD_MEM_Alloc(sizeof(STREAM_ACCESS_t));
	if(pAccess == NULL)
	{
		PrintError("%s(%d) Error> can't allocate STREAM handler\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	VK_memset(pAccess,0,sizeof(STREAM_ACCESS_t));

	/*
	 * We must store a STREAM Handler in iptuner handle .
	 * If not so, we can't know correct stream handle in vfio function set.(VFIO_Read/VFIO_xxx etc)
	 *
	 * iptuner_handler --  VFIO Handler (include file and socket descriptor)
	 *                 --  STREAM Handler ( include protocol controller)
	 */
	ptHandle->pTunerHandle = (void*) pHTuner;
	pHTuner->pHStream = (void*) pAccess;

#if SUPPORT_HUMAX_COOKIE
	VK_MEM_Memset(&tUrl, 0x0, sizeof(tUrl));
	UTIL_UrlParse(&tUrl, (const char *)pUrl, 0);
	if(tUrl.psz_cookie != NULL)
	{
		psCookie = VK_StrDup(tUrl.psz_cookie);
	}
	UTIL_UrlClean(&tUrl);
#endif

	pAccess->psz_path = VK_StrDup ( (void *) UTIL_GetOriginalURL ((const char *)pUrl) );
#if SUPPORT_HUMAX_COOKIE
	if(psCookie != NULL)
	{
		pAccess->psz_cookie = psCookie;
	}
#endif

	if(pAccess->psz_path == NULL)
	{
		PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
		ptHandle->ucAvailable = 0;
		vRet = VFIO_ERROR_IN_DEVICE;
		return vRet;
	}

	/* support YouView Specific UserAgent String */
	if(pHTuner->pUserAgentString != NULL)
	{
		pAccess->psz_user_agent = VK_StrDup ((void *)pHTuner->pUserAgentString);
		if(pAccess->psz_user_agent == NULL)
		{
			PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
			ptHandle->ucAvailable = 0;
			vRet = VFIO_ERROR_IN_DEVICE;
			return vRet;
		}
	}
	else /* error case : using default HUMAX UserAgent */
	{
		pAccess->psz_user_agent = VK_StrDup ((void *)DI_IPTUNER_USER_AGENT_STRING);
	}

	if(pHTuner->pTls_ca_name != NULL)
	{
		pAccess->psz_ca_name = VK_StrDup ( pHTuner->pTls_ca_name);
	}

	if(pHTuner->pTls_ca_path != NULL)
	{
		pAccess->psz_ca_path = VK_StrDup ( pHTuner->pTls_ca_path);
	}

	if(pHTuner->pTls_certi_path != NULL)
	{
		pAccess->psz_certi_path = VK_StrDup ( pHTuner->pTls_certi_path);
	}

	if(pHTuner->pTls_prikey_path != NULL)
	{
		pAccess->psz_prikey_path = VK_StrDup ( pHTuner->pTls_prikey_path);
	}

	if(pHTuner->pTls_pripw != NULL)
	{
		pAccess->psz_prikey_passwd = VK_StrDup ( pHTuner->pTls_pripw);
	}

	if(pHTuner->cbTlsCertificate.cbVerify != NULL)
	{
		pAccess->cbTlsCerficate = (STREAM_VerifyExternalCallback_t*) DD_MEM_Alloc(sizeof(STREAM_VerifyExternalCallback_t));
		if(pAccess->cbTlsCerficate)
		{
			VK_memset (pAccess->cbTlsCerficate, 0, sizeof(STREAM_VerifyExternalCallback_t));
			pAccess->cbTlsCerficate->callback = &P_STREAM_HTTP_TlsCertificateCallback;
			pAccess->cbTlsCerficate->context  = (void*) pHTuner;
			pAccess->cbTlsCerficate->userdata = pHTuner->cbTlsCertificate.pUserdata;
			PrintDebug ("%s(%d) Register Tls Certificate callback (%u)\n",__FUNCTION__,__LINE__,(HUINT32)pAccess->cbTlsCerficate);
		}
		else
		{
			PrintError("%s(%d) Fail Register Tls Certificate Callback\n",__FUNCTION__,__LINE__);
		}
	}

	/* Set DLNA Feature */
	if(pHTuner->tDlnaSetting.bDlna == TRUE)
	{
		if(pHTuner->tDlnaSetting.eSeekMode == DI_IPTUNER_DLNA_SEEKMODE_BYTESEEK)
		{
			pAccess->t_dlna.b_byte_seek = TRUE;
			pAccess->t_dlna.b_time_seek = FALSE;
		}
		else if(pHTuner->tDlnaSetting.eSeekMode == DI_IPTUNER_DLNA_SEEKMODE_TIMESEEK)
		{
			pAccess->t_dlna.b_byte_seek = FALSE;
			pAccess->t_dlna.b_time_seek = TRUE;
		}
		else
		{
			pAccess->t_dlna.b_byte_seek = FALSE;
			pAccess->t_dlna.b_time_seek = FALSE;
		}

		if(pHTuner->tDlnaSetting.eSpeed == TRUE)
		{
			pAccess->t_dlna.i_server_trickmode= TRUE;
		}
		else
		{
			pAccess->t_dlna.i_server_trickmode = FALSE;
		}

		if((pHTuner->tDlnaSetting.bLimitedTimeBasedSeek == TRUE) || (pHTuner->tDlnaSetting.bLimitedRangeBasedSeek == TRUE))
		{
			pAccess->t_dlna.b_limited_seek = TRUE;
		}
		else
		{
			pAccess->t_dlna.b_limited_seek = FALSE;
		}

		if(pHTuner->tDlnaSetting.nByteRate > 0 )
		{
			pAccess->t_dlna.i_byterate = pHTuner->tDlnaSetting.nByteRate;
		}
		else
		{
			pAccess->t_dlna.i_byterate = 0;
		}

		if(pHTuner->tDlnaSetting.unDuration > 0 )
		{
			pAccess->t_dlna.i_duration = pHTuner->tDlnaSetting.unDuration;
		}
		else
		{
			pAccess->t_dlna.i_duration = 0;
		}

		pAccess->t_dlna.b_dlna = TRUE;
	}

	if ( DI_IPT_SUCCESS != STREAM_HTTP_Open (pAccess))
	{
		/***********************
		 * STREAM Open fails
		 ***********************/
		pAccess->b_alive = TRUE;
		ptHandle->ucAvailable = 0;

		if(pAccess->psz_path != NULL)
		{
			VK_MEM_Free(pAccess->psz_path);
			pAccess->psz_path =NULL;
		}
		if(pAccess->psz_user_agent != NULL)
		{
			VK_MEM_Free(pAccess->psz_user_agent);
			pAccess->psz_user_agent =NULL;
		}
		/* else { continue; } */
#if SUPPORT_HUMAX_COOKIE
		if(pAccess->psz_cookie != NULL)
		{
			DD_MEM_Free(pAccess->psz_cookie);
			pAccess->psz_cookie =NULL;
		}
		/* else { continue; } */
#endif

		if(pAccess->psz_ca_name != NULL)
		{
			DD_MEM_Free(pAccess->psz_ca_name);
			pAccess->psz_ca_name = NULL;
		}

		if(pAccess->psz_ca_path != NULL)
		{
			DD_MEM_Free(pAccess->psz_ca_path);
			pAccess->psz_ca_path = NULL;
		}

		if(pAccess->psz_certi_path != NULL)
		{
			DD_MEM_Free(pAccess->psz_certi_path);
			pAccess->psz_certi_path = NULL;
		}

		if(pAccess->psz_prikey_path != NULL)
		{
			DD_MEM_Free(pAccess->psz_prikey_path);
			pAccess->psz_prikey_path = NULL;
		}

		if(pAccess->psz_prikey_passwd != NULL)
		{
			DD_MEM_Free(pAccess->psz_prikey_passwd);
			pAccess->psz_prikey_passwd = NULL;
		}

		if(pAccess->cbTlsCerficate != NULL)
		{
			pAccess->cbTlsCerficate->callback = NULL;
			DD_MEM_Free(pAccess->cbTlsCerficate);
			pAccess->cbTlsCerficate = NULL;
		}

		PrintError("%s(%d) : Error> HTTP_Open Fail.\n",__FUNCTION__,__LINE__);
		vRet = VFIO_ERROR_IN_DEVICE;
	}
	else
	{
		/*************************
		 * Succesful Open
		 *************************/
		PrintData ("%s(%d) : i_update       = %d \n",__FUNCTION__,__LINE__,pAccess->info.i_update);
		PrintData ("%s(%d) : i_size         = %"PRId64"\n",__FUNCTION__,__LINE__,pAccess->info.i_size);
		PrintData ("%s(%d) : i_pos          = %"PRId64"\n",__FUNCTION__,__LINE__,pAccess->info.i_pos);
		PrintData ("%s(%d) : b_eof          = %d \n",__FUNCTION__,__LINE__,pAccess->info.b_eof);
		PrintData ("%s(%d) : i_title        = %d \n",__FUNCTION__,__LINE__,pAccess->info.i_title);
		PrintData ("%s(%d) : i_seekpoint    = %d \n",__FUNCTION__,__LINE__,pAccess->info.i_seekpoint);
		PrintData ("%s(%d) : b_prebuffered  = %d \n",__FUNCTION__,__LINE__,pAccess->info.b_prebuffered);
#ifndef CONFIG_DLNA_CERTIFICATION
		pHTuner->bHttpChunked = pstHttpSys->b_chunked;
		pHTuner->bRangeAccepted = pstHttpSys->b_range_accepted;
#endif
		if((pAccess->info.i_size > 0) && (pAccess->info.i_size < 4*1024))
		{
			/******************
			 * Abnormal Case
			 ******************/
			PrintError("%s(%d) : Error> HTTP_Open Fail.\n",__FUNCTION__,__LINE__);
			PrintError("%s(%d) : Error> Invalid File Size(File Size = %"PRId64".\n",__FUNCTION__,__LINE__, pAccess->info.i_size);
			pAccess->b_alive = TRUE;
			ptHandle->ucAvailable = 0;

			(void)STREAM_HTTP_Close (pAccess);

			if(pAccess->psz_path != NULL)
			{
				VK_MEM_Free(pAccess->psz_path);
				pAccess->psz_path =NULL;
			}

			if(pAccess->psz_user_agent != NULL)
			{
				VK_MEM_Free(pAccess->psz_user_agent);
				pAccess->psz_user_agent =NULL;
			}

			/* else { continue; } */
#if SUPPORT_HUMAX_COOKIE
			if(pAccess->psz_cookie != NULL)
			{
				DD_MEM_Free(pAccess->psz_cookie);
				pAccess->psz_cookie =NULL;
			}
			/* else { continue; } */
#endif

			if(pAccess->psz_ca_name != NULL)
			{
				DD_MEM_Free(pAccess->psz_ca_name);
				pAccess->psz_ca_name = NULL;
			}

			if(pAccess->psz_ca_path != NULL)
			{
				DD_MEM_Free(pAccess->psz_ca_path);
				pAccess->psz_ca_path = NULL;
			}

			if(pAccess->psz_certi_path != NULL)
			{
				DD_MEM_Free(pAccess->psz_certi_path);
				pAccess->psz_certi_path = NULL;
			}

			if(pAccess->psz_prikey_path != NULL)
			{
				DD_MEM_Free(pAccess->psz_prikey_path);
				pAccess->psz_prikey_path = NULL;
			}

			if(pAccess->psz_prikey_passwd != NULL)
			{
				DD_MEM_Free(pAccess->psz_prikey_passwd);
				pAccess->psz_prikey_passwd = NULL;
			}

			if(pAccess->cbTlsCerficate != NULL)
			{
				pAccess->cbTlsCerficate->callback = NULL;
				DD_MEM_Free(pAccess->cbTlsCerficate);
				pAccess->cbTlsCerficate = NULL;
			}

			vRet = VFIO_ERROR_IN_DEVICE;
		}
		else
		{
			/******************
			 * Normal Case
			 ******************/
			pstHttpSys = (HTTP_ACCESS_SYS_t*) pAccess->p_sys;
#ifdef CONFIG_DLNA_CERTIFICATION
			pHTuner->bHttpChunked = pstHttpSys->b_chunked;
			pHTuner->bRangeAccepted = pstHttpSys->b_range_accepted;
#endif
			ptHandle->ucAvailable = 1;
			ptHandle->tFile.callback.unHandle = (HUINT32) pstHttpSys->fd; /** http socket descriptor */
			pHTuner->ulDNPFileLength = pAccess->info.i_size;
			PrintData ("%s(%d) : callback unhandle=%d\n",__FUNCTION__,__LINE__, ptHandle->tFile.callback.unHandle);

			/* InetRadio structure in Tuner is filled here */
			if(pstHttpSys->b_inet_radio == TRUE)
			{
				pHTuner->eInetRadioType = DI_IPTUNER_INETRADIO_TYPE_GENERIC;
				pHTuner->tInetRadioInfo.eInetRadioCodec = pstHttpSys->e_inetradio_codec;
				pHTuner->tInetRadioInfo.nInetRadioBitrate = pstHttpSys->i_icy_br;

				if (pstHttpSys->b_icecast == TRUE)
				{
					pHTuner->eInetRadioType = DI_IPTUNER_INETRADIO_TYPE_ICY;
				}
			}
			else
			{
				pHTuner->eInetRadioType = DI_IPTUNER_INETRADIO_TYPE_NONE;
			}
			pHTuner->bHttpChunked = pstHttpSys->b_chunked;

			/* Create Semaphore to control a vfio functions exclusively.*/
			if ( VK_OK != VK_SEM_Create(&pAccess->ulStreamSema,"httpSema", DEFAULT_SUSPEND_TYPE))
			{
				/**************************
				 * Semaphore creation fail
				 **************************/
				(void)STREAM_HTTP_Close (pAccess);

				if(pAccess->psz_path != NULL)
				{
					VK_MEM_Free(pAccess->psz_path);
					pAccess->psz_path =NULL;
				}

				if(pAccess->psz_user_agent != NULL)
				{
					VK_MEM_Free(pAccess->psz_user_agent);
					pAccess->psz_user_agent =NULL;
				}
				/* else { continue; } */
#if SUPPORT_HUMAX_COOKIE
				if(pAccess->psz_cookie != NULL)
				{
					DD_MEM_Free(pAccess->psz_cookie);
					pAccess->psz_cookie =NULL;
				}
				/* else { continue; } */
#endif

				if(pAccess->psz_ca_name != NULL)
				{
					DD_MEM_Free(pAccess->psz_ca_name);
					pAccess->psz_ca_name = NULL;
				}

				if(pAccess->psz_ca_path != NULL)
				{
					DD_MEM_Free(pAccess->psz_ca_path);
					pAccess->psz_ca_path = NULL;
				}

				if(pAccess->psz_certi_path != NULL)
				{
					DD_MEM_Free(pAccess->psz_certi_path);
					pAccess->psz_certi_path = NULL;
				}

				if(pAccess->psz_prikey_path != NULL)
				{
					DD_MEM_Free(pAccess->psz_prikey_path);
					pAccess->psz_prikey_path = NULL;
				}

				if(pAccess->psz_prikey_passwd != NULL)
				{
					DD_MEM_Free(pAccess->psz_prikey_passwd);
					pAccess->psz_prikey_passwd = NULL;
				}

				if(pAccess->cbTlsCerficate != NULL)
				{
					pAccess->cbTlsCerficate->callback = NULL;
					DD_MEM_Free(pAccess->cbTlsCerficate);
					pAccess->cbTlsCerficate = NULL;
				}

				PrintError ("%s(%d) Error> can't create semaphore httpSema",__FUNCTION__,__LINE__);
				return VFIO_ERROR_IN_DEVICE;
			}

			/*************************************************************
			 * A decision of TIMELINE methods from YouView Content Server
			 * 주의 !!
			 *     timeline 결정은 probing time 에만 시도해야한다.
			 *     본 vfio open function 은 어디서든 항상 불리는 function 이므로
			 *     status 를 비교하지 않으면 호출될때마다 timeline 이 바뀌게된다.
			 *************************************************************/
			if (pHTuner->eDownloadType != DI_IPTUNER_DOWNLOADING)
			{
				if(pHTuner->pIndexTimeLineUrl )
				{
					DD_MEM_Free (pHTuner->pIndexTimeLineUrl);
					pHTuner->pIndexTimeLineUrl = NULL;
				}

				if(pHTuner->pIndexRoom)
				{
					DD_MEM_Free (pHTuner->pIndexRoom);
					pHTuner->pIndexRoom = NULL;
					pHTuner->nIndexRoomSize=0;
				}

				if(pAccess->psz_index_file != NULL)
				{
					pHTuner->eTimeLine = DI_IPTUNER_TIMELINE_INDEX_FILE;
					pHTuner->pIndexTimeLineUrl = VK_StrDup (pAccess->psz_index_file);
					PrintError ("%s(%d) Info> Using X-IndexFileHeader(%s) for iptuner-timeline \n"
							,__FUNCTION__
							,__LINE__,
							pAccess->psz_index_file);
				}
				else if(pAccess->psz_bps_value != NULL)
				{
					pHTuner->eTimeLine = DI_IPTUNER_TIMELINE_BYTESPERSECOND;
					pHTuner->nTimeLineBPS = (HUINT32) VK_atoi (pAccess->psz_bps_value);
					PrintError ("%s(%d) Info> Using X-BytesPerSecond Value (%d) for iptuner-timeline \n"
							,__FUNCTION__
							,__LINE__
							,pHTuner->nTimeLineBPS);
				}
				else
				{
					pHTuner->eTimeLine = DI_IPTUNER_TIMELINE_STREAM;
					PrintError ("%s(%d) Info> Using StreamItself for iptuner-timeline \n",__FUNCTION__,__LINE__);
				}
			}
		} /* normal end */
	}

	PrintExit;
	return vRet;
}

/** */
int DI_STREAM_SIMPLE_HTTP_Close (VFIO_Handle_t tHandle)
{
	HINT32 vRet = VFIO_ERROR_SUCCESS;
	DI_IPTUNER_t *pHTuner = NULL;
	STREAM_ACCESS_t *pAccess = NULL;

	PrintEnter;

	/* Searching IPTUNER handle using URL String */
	pHTuner = tHandle.pTunerHandle;

	if (pHTuner == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Error>  Invalid Tuner Handle in VFIO Handle\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}

	/* Now we just get the Stream handler */
	pAccess = (STREAM_ACCESS_t *)pHTuner->pHStream;
	if (NULL == pAccess)
	{
		PrintError("%s(%d) Error> Tuner handle doesn't include correct STREAM Handler. Check This !!\n"
			, __FUNCTION__
			, __LINE__);
		return VFIO_ERROR_FAIL;
	}

	/* In multi-open case, the http session should be closed only once. */
	if(pAccess->b_alive == FALSE)
	{
		PrintDebug ("%s(%d) : HTTP Session is already closed.\n",__FUNCTION__,__LINE__);
		goto success;
	}

	if(VK_SEM_Get(pAccess->ulStreamSema) != VK_OK)
	{
		PrintError("%s(%d) Error> can't get ulStreamSema (tuner %d) (unHandle %d), Maybe this tuner wasn't opened.\n"
				,__FUNCTION__
				,__LINE__
				,(HUINT32)tHandle.pTunerHandle
				,tHandle.tFile.callback.unHandle);
		return VFIO_ERROR_FAIL;
	}

	(void) STREAM_HTTP_Close ( pAccess );

	pAccess->b_alive = FALSE;

	if(pAccess->psz_path != NULL)
	{
		VK_MEM_Free (pAccess->psz_path);
		pAccess->psz_path =NULL;
	}

	/* Error Event Callback */
	if(pAccess->callback != NULL)
	{
		DD_MEM_Free (pAccess->callback);
		pAccess->callback = NULL;
	}

	if(pAccess->psz_user_agent != NULL)
	{
		VK_MEM_Free(pAccess->psz_user_agent);
		pAccess->psz_user_agent =NULL;
	}
	/* else { continue; } */

#if SUPPORT_HUMAX_COOKIE
	if(pAccess->psz_cookie != NULL)
	{
		DD_MEM_Free(pAccess->psz_cookie);
		pAccess->psz_cookie =NULL;
	}
	/* else { continue; } */
#endif

	if(pAccess->psz_ca_name != NULL)
	{
		DD_MEM_Free(pAccess->psz_ca_name);
		pAccess->psz_ca_name = NULL;
	}

	if(pAccess->psz_ca_path != NULL)
	{
		DD_MEM_Free(pAccess->psz_ca_path);
		pAccess->psz_ca_path = NULL;
	}

	if(pAccess->psz_certi_path != NULL)
	{
		DD_MEM_Free(pAccess->psz_certi_path);
		pAccess->psz_certi_path = NULL;
	}

	if(pAccess->psz_prikey_path != NULL)
	{
		DD_MEM_Free(pAccess->psz_prikey_path);
		pAccess->psz_prikey_path = NULL;
	}

	if(pAccess->psz_prikey_passwd != NULL)
	{
		DD_MEM_Free(pAccess->psz_prikey_passwd);
		pAccess->psz_prikey_passwd = NULL;
	}

	if(pAccess->cbTlsCerficate != NULL)
	{
		pAccess->cbTlsCerficate->callback = NULL;
		DD_MEM_Free(pAccess->cbTlsCerficate);
		pAccess->cbTlsCerficate = NULL;
	}

	(void)VK_SEM_Release (pAccess->ulStreamSema);

	(void)VK_SEM_Destroy (pAccess->ulStreamSema);

	pAccess->ulStreamSema = 0;

success:
	if(pHTuner->pHStream != NULL)
	{
		DD_MEM_Free (pHTuner->pHStream);
		pHTuner->pHStream = NULL;
	}

	PrintExit;
	return vRet;
}

/** */
int DI_STREAM_SIMPLE_HTTP_Read (VFIO_Handle_t tHandle, void *ptr, size_t size, size_t nmemb, ssize_t *ptReadSize)
{
	size_t sz_Real = 0;
	size_t sz_Try = size * nmemb;
	int ret = VFIO_ERROR_SUCCESS;
	HINT32 n_ReturnedValue = 0;
	HINT32 	nRetryStreamReadCount =0;

	DI_IPTUNER_t *pHTuner = NULL;
	STREAM_ACCESS_t *pAccess = NULL;

	/* Getting IPTUNER handle from VFIO Handle */
	pHTuner = tHandle.pTunerHandle;

	if (pHTuner == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Error> Invalid Tuner Handle in VFIO Handle\n"
			,__FUNCTION__
			,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}

	/* To cancel probe. */
	if (pHTuner->bProbeCancel == TRUE)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Debug> PROBE Cancel !!\n"
			,__FUNCTION__
			,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}

	/* Now we just get the Stream handler */
	pAccess = (STREAM_ACCESS_t *)pHTuner->pHStream;
	if (NULL == pAccess)
	{
		PrintError("%s(%d) Error> Tuner handle doesn't include correct STREAM Handler. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return VFIO_ERROR_FAIL;
	}
#ifdef CONFIG_DLNA_CERTIFICATION
	if (pHTuner->bDisconnectedForPause)
	{
		*ptReadSize = sz_Try;
		ret = VFIO_ERROR_SUCCESS;
		VK_memset(ptr,0x0,sz_Try);
		PrintError("%s(%d) DEBUG> Read on disconnected\n"
		, __FUNCTION__, __LINE__);
		return ret;
	}
#endif
	if (VK_SEM_Get(pAccess->ulStreamSema) != VK_OK)
	{
		PrintError("%s(%d) Error> can't get ulStreamSema (tuner %d) (unHandle %d)\n"
		,__FUNCTION__
		,__LINE__
		,(HUINT32)tHandle.pTunerHandle
		,tHandle.tFile.callback.unHandle);
		return VFIO_ERROR_FAIL;
	}

	if(pAccess->info.i_size < pAccess->info.i_pos + sz_Try)
	{
		sz_Try = pAccess->info.i_size - pAccess->info.i_pos;
	}

	if(sz_Try > size*nmemb)
	{
		PrintDebug("######### [%s(%d)] Error> Size Invalid(%u, %"PRId64",%"PRId64") #######\n", __FUNCTION__,__LINE__, sz_Try, pAccess->info.i_size, pAccess->info.i_pos);
		sz_Try = size*nmemb;
	}

	do
	{
		nRetryStreamReadCount = 0;
retry_read:
#ifndef CONFIG_PRODUCT_IMAGE_HWTEST
		if(pHTuner->tDtcpIpSetting.bDtcpIp == TRUE)
		{
			n_ReturnedValue = P_STREAM_HTTP_DTCP_Read (pHTuner, pAccess, (HUINT8 *)ptr+sz_Real, sz_Try - sz_Real );
		}
		else
#endif
		{
			n_ReturnedValue = STREAM_HTTP_Read ( pAccess, (HUINT8 *)ptr+sz_Real, sz_Try - sz_Real );
		}

		/* Read Error : should be exit immediately */
		if (n_ReturnedValue == 0 || n_ReturnedValue == DI_IPT_ETIMEOUT)
		{
			/* If an application requests destruction of http session like DI_IPTUNER_StopBuffering ,
			 *  this function should be returned immediately.
			 * If not so, the session may be blocked by retrying logic. Be careful.
			 */
			if(pAccess->b_immediately_exit == TRUE)
			{
				pAccess->b_retrying = FALSE;
				pAccess->b_alive = FALSE;
				ret = VFIO_ERROR_IN_DEVICE;

				PrintError("%s(%d) Error> STREAM_HTTP_Read Fail!!\n", __FUNCTION__, __LINE__);
				goto stream_read_error;
			}

			if(pAccess->info.b_eof == TRUE) /* if chunked-TE and last-chunk, */
			{
				PrintDebug("%s(%d) Debug> EndOfStream Occured in Steram HTTP Controller !!\n", __FUNCTION__, __LINE__);
				break;
			}
			/* Something happened in STREAM Controller , Let's try again */
			else
			{
				nRetryStreamReadCount ++;
				if(nRetryStreamReadCount > 100)
				{
					*ptReadSize = sz_Real;
					nRetryStreamReadCount = 0;
					PrintError("%s(%d) Error> Retry Time Over (%d)!!\n", __FUNCTION__, __LINE__, nRetryStreamReadCount );
					goto stream_read_error;
				}

				PrintError("%s(%d) Retry> (RetryCount=%d)...\n", __FUNCTION__, __LINE__, nRetryStreamReadCount );
				goto retry_read;
			}

		}
		else if (n_ReturnedValue < 0)
		{
			*ptReadSize = sz_Real;
			ret = VFIO_ERROR_IN_DEVICE;
			PrintError("%s(%d) Error> STREAM_HTTP_Read Fail!!\n", __FUNCTION__, __LINE__);
			goto stream_read_error;
		}
		/* EOF or Error */
		sz_Real += n_ReturnedValue;
	}
	while (sz_Real < sz_Try);

	*ptReadSize = sz_Real;

stream_read_error:

	(void)VK_SEM_Release (pAccess->ulStreamSema);

	return ret;
}

/** This Function called by bmedia probe only.*/
int DI_STREAM_SIMPLE_HTTP_Seek (VFIO_Handle_t tHandle, off_t offset, int whence)
{
	DI_IPTUNER_t *pHTuner = NULL;
	STREAM_ACCESS_t *pAccess = NULL;
	HINT32 nSeekRetryCount = 0;
//	PrintEnter;

	/* Getting IPTUNER handle from VFIO Handle */
	pHTuner = tHandle.pTunerHandle;

	if (pHTuner == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Error> Invalid Tuner Handle in VFIO Handle\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}

	/* To cancel probe */
	if (pHTuner->bProbeCancel == TRUE)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Debug> PROBE Cancel !!\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}

	/* Now we just get the Stream handler */
	pAccess = (STREAM_ACCESS_t *)pHTuner->pHStream;
	if (NULL == pAccess)
	{
		PrintError("%s(%d) Error> Tuner handle doesn't include correct STREAM Handler. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return VFIO_ERROR_FAIL;
	}

	if ( VK_SEM_Get(pAccess->ulStreamSema) != VK_OK)
	{
		PrintError("%s(%d) Error> can't get ulStreamSema (tuner %d) (unHandle %d)\n"
		,__FUNCTION__
		,__LINE__
		,(HUINT32)tHandle.pTunerHandle
		,tHandle.tFile.callback.unHandle);
		return VFIO_ERROR_FAIL;
	}

	PrintData ("%s(%d) : Tuner(%u) offset=%"PRId64", whence=%d\n"
			,__FUNCTION__
			,__LINE__
			, (unsigned int)pHTuner
			, offset
			, whence);


	/* POSIX Seek operation */

	if(whence == SEEK_END)
		offset = (off_t)pAccess->info.i_size-offset;

	PrintDebug("== Probe Seek\n");

/***********************************
*	If the connection is lost,
*	i_pos has advanced but the actual pointer didn't move yet.
***********************************/
#ifdef CONFIG_DLNA_CERTIFICATION
	if(pAccess->info.i_pos == offset)
	{
		(void)VK_SEM_Release (pAccess->ulStreamSema);
		return VFIO_ERROR_SUCCESS;
	}
#endif

	/* YouView Back-Off */
	/* Retrying ... */
	while( DI_IPT_SUCCESS != STREAM_HTTP_Seek ( pAccess , offset ))
	{
		nSeekRetryCount++;
		PrintError ("%s(%d) Error> STREAM_HTTP_Seek fail... Retrying seek request (%d) times..\n"
		,__FUNCTION__
		,__LINE__
		,nSeekRetryCount);

		if ( FALSE == UTIL_SYS_GetLinkStatus () )
		{
			PrintError ("%s (%d) Debug> Network Cable unplugged !!\n",__FUNCTION__,__LINE__);
			(void)VK_SEM_Release (pAccess->ulStreamSema);
			return VFIO_ERROR_IN_DEVICE;
		}

		if (pHTuner->bProbeCancel == TRUE)
		{
			PrintError ("%s (%d) Debug> PROBE Cancel !!\n",__FUNCTION__,__LINE__);
			(void)VK_SEM_Release (pAccess->ulStreamSema);
			return VFIO_ERROR_IN_DEVICE;
		}

		if(nSeekRetryCount > DI_IPTUNER_HTTP_SEEK_RETRY_COUNT)
		{
			PrintError ("%s(%d) Error> STREAM_HTTP_Seek Retrying fail. Last Retrying count(%d) ...\n"
				,__FUNCTION__
				,__LINE__
				,nSeekRetryCount);
			(void)VK_SEM_Release (pAccess->ulStreamSema);
			return VFIO_ERROR_FAIL;
		}
	}

	(void)VK_SEM_Release (pAccess->ulStreamSema);

	return VFIO_ERROR_SUCCESS;
}

/** */
int DI_STREAM_SIMPLE_HTTP_Tell (VFIO_Handle_t tHandle, off_t *plCurOffset)
{
	DI_IPTUNER_t *pHTuner = NULL;
	STREAM_ACCESS_t *pAccess = NULL;

//	PrintEnter;

	/* Getting IPTUNER handle from VFIO Handle*/
	pHTuner = tHandle.pTunerHandle;
	if (pHTuner == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Error>  Invalid Tuner Handle in VFIO Handle\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}

	/* Now we just get the Stream handler */
	pAccess = (STREAM_ACCESS_t *)pHTuner->pHStream;
	if (NULL == pAccess)
	{
		PrintError("%s(%d) Error> Tuner handle doesn't include correct STREAM Handler. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return VFIO_ERROR_FAIL;
	}

	*plCurOffset = pAccess->info.i_pos;
//	PrintDebug ("%s(%d) : i_pos (%"PRId64") nRet(%d)\n",__FUNCTION__,__LINE__,*plCurOffset, nRet);

	return VFIO_ERROR_SUCCESS;
}

/** if EOF is true, must be return -1 like feof*/
int DI_STREAM_SIMPLE_HTTP_IsEOF (VFIO_Handle_t tHandle)
{
	HINT32 nRet = VFIO_ERROR_SUCCESS;
	DI_IPTUNER_t *pHTuner = NULL;
	STREAM_ACCESS_t *pAccess = NULL;

//	PrintEnter;
	/* Getting IPTUNER handle from VFIO Handle */
	pHTuner = tHandle.pTunerHandle;
	if (pHTuner == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Error>  Invalid Tuner Handle in VFIO Handle\n",__FUNCTION__,__LINE__);
		nRet = 0;
		return nRet;
	}

	/* Now we just get the Stream handler */
	pAccess = (STREAM_ACCESS_t *)pHTuner->pHStream;
	if (NULL == pAccess)
	{
		PrintError("%s(%d) Error> Tuner handle doesn't include correct STREAM Handler. Check This !!\n"
		, __FUNCTION__, __LINE__);
		nRet = 0;
		return nRet;
	}

    if(TRUE == pAccess->info.b_eof)
	{
		nRet = -1; /* must be -1 like feof*/
	}

//	PrintExit;
	return nRet;
}

/* Open From Nexus */
int DI_STREAM_SIMPLE_HTTP_OpenFromNexus (const HINT8 *pUrl, const unsigned int flags, VFIO_Handle_t *ptHandle)
{
	HTTP_ACCESS_SYS_t *pstHttpSys=NULL;
	DI_IPTUNER_t *pHTuner=NULL;
	STREAM_ACCESS_t *pAccess=NULL;
#if SUPPORT_HUMAX_COOKIE
	char *psCookie = NULL;
	URL_t tUrl;
#endif

	UNUSED_PARAM (flags);

	PrintEnter;

	/* valid check */
	if(pUrl==NULL)
	{
		ptHandle->ucAvailable = 0;
		PrintError("%s(%d) : Error> invalid pUrl(%s)\n",__FUNCTION__,__LINE__,pUrl);
		return VFIO_ERROR_IN_DEVICE;
	}

	/* Searching IPTUNER handle using URL String */
	pHTuner = DI_IPTUNER_POOL_FindByUrl (pUrl);
	if(pHTuner == NULL)
	{
		ptHandle->ucAvailable = 0;
		PrintError ("%s(%d) : Error> can't find iptuner handler by this URL(%s)\n",__FUNCTION__,__LINE__,pUrl);
		return VFIO_ERROR_IN_DEVICE;
	}
	pAccess = (STREAM_ACCESS_t*) DD_MEM_Alloc(sizeof(STREAM_ACCESS_t));
	if(pAccess == NULL)
	{
		PrintError("%s(%d) Error> can't allocate STREAM handler\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	VK_memset(pAccess,0,sizeof(STREAM_ACCESS_t));

	/*
	 * We must store a STREAM Handler in iptuner handle .
	 * If not so, we can't know correct stream handle in vfio function set.(VFIO_Read/VFIO_xxx etc)
	 *
	 * iptuner_handler --  VFIO Handler (include file and socket descriptor)
	 *                 --  STREAM Handler ( include protocol controller)
	 */
	ptHandle->pTunerHandle = (void*) pHTuner;
	pHTuner->pHStream = (void*) pAccess;


#if SUPPORT_HUMAX_COOKIE
	VK_MEM_Memset(&tUrl, 0x0, sizeof(tUrl));
	UTIL_UrlParse(&tUrl, (const char *)pUrl, 0);
	if(tUrl.psz_cookie != NULL)
	{
		psCookie = VK_StrDup(tUrl.psz_cookie);
	}
	UTIL_UrlClean(&tUrl);
#endif

	/* open for player */
	/** psz_path must be free */
	pAccess->psz_path = VK_StrDup ((void *) UTIL_GetOriginalURL ((const char *)pUrl) );
#if SUPPORT_HUMAX_COOKIE
	if(psCookie != NULL)
	{
		pAccess->psz_cookie = psCookie;
	}
#endif
	if(pAccess->psz_path == NULL)
	{
		PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
		ptHandle->ucAvailable = 0;

		/* else { continue; } */
#if SUPPORT_HUMAX_COOKIE
		if(pAccess->psz_cookie != NULL)
		{
			DD_MEM_Free(pAccess->psz_cookie);
			pAccess->psz_cookie =NULL;
		}
		/* else { continue; } */
#endif
		return VFIO_ERROR_IN_DEVICE;
	}

	/* support YouView Specific UserAgent String */
	if(pHTuner->pUserAgentString != NULL)
	{
		pAccess->psz_user_agent = VK_StrDup ((void *)pHTuner->pUserAgentString);
		if(pAccess->psz_user_agent == NULL)
		{
			PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
			ptHandle->ucAvailable = 0;
#if SUPPORT_HUMAX_COOKIE
			if(pAccess->psz_cookie != NULL)
			{
				DD_MEM_Free(pAccess->psz_cookie);
				pAccess->psz_cookie =NULL;
			}
			/* else { continue; } */
#endif
			return VFIO_ERROR_IN_DEVICE;
		}
	}
	else /* error case : using default HUMAX UserAgent */
	{
		pAccess->psz_user_agent = VK_StrDup ((void *)DI_IPTUNER_USER_AGENT_STRING);
	}

	/* Error Event Callback */
	pAccess->callback = (STREAM_CallbackDesc*) DD_MEM_Alloc(sizeof(STREAM_CallbackDesc));
	if(pAccess->callback)
	{
		VK_memset (pAccess->callback, 0, sizeof(STREAM_CallbackDesc));
		pAccess->callback->callback = &P_HTTP_ResponseCallback;
		pAccess->callback->context  = (void*) pHTuner;
		PrintDebug ("%s(%d) Open callback (%u)\n",__FUNCTION__,__LINE__,(HUINT32)pAccess->callback);
		PrintDebug ("%s(%d) Open callback->callback (%u)\n",__FUNCTION__,__LINE__,(HUINT32)pAccess->callback->callback);
		PrintDebug("%s(%d) >>>> Open callback->context  (%u)\n",__FUNCTION__,__LINE__,(HUINT32)pAccess->callback->context);
	}
	else
	{
		PrintError("%s(%d) Warning> can't allocate STREAM event callbacks\n",__FUNCTION__,__LINE__);
		PrintError("%s(%d) Warning> The backoff event can't be generated in stream core\n",__FUNCTION__,__LINE__);
		PrintError("%s(%d) Warning> Anyway continue...\n",__FUNCTION__,__LINE__);
	}

	if(pHTuner->pTls_ca_name != NULL)
	{
		pAccess->psz_ca_name = VK_StrDup ( pHTuner->pTls_ca_name);
	}

	if(pHTuner->pTls_ca_path != NULL)
	{
		pAccess->psz_ca_path = VK_StrDup ( pHTuner->pTls_ca_path);
	}

	if(pHTuner->pTls_certi_path != NULL)
	{
		pAccess->psz_certi_path = VK_StrDup ( pHTuner->pTls_certi_path);
	}

	if(pHTuner->pTls_prikey_path != NULL)
	{
		pAccess->psz_prikey_path = VK_StrDup ( pHTuner->pTls_prikey_path);
	}

	if(pHTuner->pTls_pripw != NULL)
	{
		pAccess->psz_prikey_passwd = VK_StrDup ( pHTuner->pTls_pripw);
	}

	if(pHTuner->cbTlsCertificate.cbVerify != NULL)
	{
		pAccess->cbTlsCerficate = (STREAM_VerifyExternalCallback_t*) DD_MEM_Alloc(sizeof(STREAM_VerifyExternalCallback_t));
		if(pAccess->cbTlsCerficate)
		{
			VK_memset (pAccess->cbTlsCerficate, 0, sizeof(STREAM_VerifyExternalCallback_t));
			pAccess->cbTlsCerficate->callback = &P_STREAM_HTTP_TlsCertificateCallback;
			pAccess->cbTlsCerficate->context  = (void*) pHTuner;
			pAccess->cbTlsCerficate->userdata = pHTuner->cbTlsCertificate.pUserdata;
			PrintDebug ("%s(%d) Register Tls Certificate callback (%u)\n",__FUNCTION__,__LINE__,(HUINT32)pAccess->cbTlsCerficate);
		}
		else
		{
			PrintError("%s(%d) Fail Register Tls Certificate Callback\n",__FUNCTION__,__LINE__);
		}
	}

	/* Set DLNA Feature */
	if(pHTuner->tDlnaSetting.bDlna == TRUE)
	{
		if(pHTuner->tDlnaSetting.eSeekMode == DI_IPTUNER_DLNA_SEEKMODE_BYTESEEK)
		{
			pAccess->t_dlna.b_byte_seek = TRUE;
			pAccess->t_dlna.b_time_seek = FALSE;
		}
		else if(pHTuner->tDlnaSetting.eSeekMode == DI_IPTUNER_DLNA_SEEKMODE_TIMESEEK)
		{
			pAccess->t_dlna.b_byte_seek = FALSE;
			pAccess->t_dlna.b_time_seek = TRUE;
		}
		else
		{
			pAccess->t_dlna.b_byte_seek = FALSE;
			pAccess->t_dlna.b_time_seek = FALSE;
		}

		if(pHTuner->tDlnaSetting.eSpeed == TRUE)
		{
			pAccess->t_dlna.i_server_trickmode= TRUE;
		}
		else
		{
			pAccess->t_dlna.i_server_trickmode = FALSE;
		}

		if((pHTuner->tDlnaSetting.bLimitedTimeBasedSeek == TRUE) || (pHTuner->tDlnaSetting.bLimitedRangeBasedSeek == TRUE))
		{
			pAccess->t_dlna.b_limited_seek = TRUE;
		}
		else
		{
			pAccess->t_dlna.b_limited_seek = FALSE;
		}

		if(pHTuner->tDlnaSetting.nByteRate > 0 )
		{
			pAccess->t_dlna.i_byterate = pHTuner->tDlnaSetting.nByteRate;
		}
		else
		{
			pAccess->t_dlna.i_byterate = 0;
		}

		if(pHTuner->tDlnaSetting.unDuration > 0 )
		{
			pAccess->t_dlna.i_duration = pHTuner->tDlnaSetting.unDuration;
		}
		else
		{
			pAccess->t_dlna.i_duration = 0;
		}

		pAccess->t_dlna.b_dlna = TRUE;
	}

	/* Try to open the STREAM */
	if (DI_IPT_SUCCESS != STREAM_HTTP_Open (pAccess) )
	{
		ptHandle->ucAvailable = 0;

		if(pAccess->psz_path != NULL)
		{
			VK_MEM_Free(pAccess->psz_path);
			pAccess->psz_path =NULL;
		}

		if(pAccess->psz_user_agent != NULL)
		{
			VK_MEM_Free  (pAccess->psz_user_agent);
			pAccess->psz_user_agent =NULL;
		}
		/* else { continue; } */
#if SUPPORT_HUMAX_COOKIE
		if(pAccess->psz_cookie != NULL)
		{
			DD_MEM_Free(pAccess->psz_cookie);
			pAccess->psz_cookie =NULL;
		}
		/* else { continue; } */
#endif

		/* Error Event Callback */
		if(pAccess->callback != NULL)
		{
			DD_MEM_Free (pAccess->callback);
			pAccess->callback = NULL;
		}

		if(pAccess->psz_ca_name != NULL)
		{
			DD_MEM_Free(pAccess->psz_ca_name);
			pAccess->psz_ca_name = NULL;
		}

		if(pAccess->psz_ca_path != NULL)
		{
			DD_MEM_Free(pAccess->psz_ca_path);
			pAccess->psz_ca_path = NULL;
		}

		if(pAccess->psz_certi_path != NULL)
		{
			DD_MEM_Free(pAccess->psz_certi_path);
			pAccess->psz_certi_path = NULL;
		}

		if(pAccess->psz_prikey_path != NULL)
		{
			DD_MEM_Free(pAccess->psz_prikey_path);
			pAccess->psz_prikey_path = NULL;
		}

		if(pAccess->psz_prikey_passwd != NULL)
		{
			DD_MEM_Free(pAccess->psz_prikey_passwd);
			pAccess->psz_prikey_passwd = NULL;
		}

		if(pAccess->cbTlsCerficate != NULL)
		{
			pAccess->cbTlsCerficate->callback = NULL;
			DD_MEM_Free(pAccess->cbTlsCerficate);
			pAccess->cbTlsCerficate = NULL;
		}

		PrintError("%s (%d) : Error> STREAM_HTTP_Open Fail.\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	else
	{
		pstHttpSys = (HTTP_ACCESS_SYS_t *)pAccess->p_sys;
		ptHandle->tFile.callback.unHandle = (HUINT32) pstHttpSys->fd;
		ptHandle->ucAvailable = 1;
		PrintData ("%s(%d) : callback handle=0x%x\n",__FUNCTION__,__LINE__, ptHandle->tFile.callback.unHandle);
	}

	if( VK_OK != VK_SEM_Create(&pAccess->ulStreamSema,"httpSema", DEFAULT_SUSPEND_TYPE))
	{
		(void)STREAM_HTTP_Close (pAccess);
		if(pAccess->psz_path != NULL)
		{
			VK_MEM_Free(pAccess->psz_path);
			pAccess->psz_path =NULL;
		}

		if(pAccess->psz_user_agent != NULL)
		{
			VK_MEM_Free  (pAccess->psz_user_agent);
			pAccess->psz_user_agent =NULL;
		}
		/* else { continue; } */
#if SUPPORT_HUMAX_COOKIE
		if(pAccess->psz_cookie != NULL)
		{
			DD_MEM_Free(pAccess->psz_cookie);
			pAccess->psz_cookie =NULL;
		}
		/* else { continue; } */
#endif

		/* Error Event Callback */
		if(pAccess->callback != NULL)
		{
			DD_MEM_Free (pAccess->callback);
			pAccess->callback = NULL;
		}

		if(pAccess->psz_ca_name != NULL)
		{
			DD_MEM_Free(pAccess->psz_ca_name);
			pAccess->psz_ca_name = NULL;
		}

		if(pAccess->psz_ca_path != NULL)
		{
			DD_MEM_Free(pAccess->psz_ca_path);
			pAccess->psz_ca_path = NULL;
		}

		if(pAccess->psz_certi_path != NULL)
		{
			DD_MEM_Free(pAccess->psz_certi_path);
			pAccess->psz_certi_path = NULL;
		}

		if(pAccess->psz_prikey_path != NULL)
		{
			DD_MEM_Free(pAccess->psz_prikey_path);
			pAccess->psz_prikey_path = NULL;
		}

		if(pAccess->psz_prikey_passwd != NULL)
		{
			DD_MEM_Free(pAccess->psz_prikey_passwd);
			pAccess->psz_prikey_passwd = NULL;
		}

		if(pAccess->cbTlsCerficate != NULL)
		{
			pAccess->cbTlsCerficate->callback = NULL;
			DD_MEM_Free(pAccess->cbTlsCerficate);
			pAccess->cbTlsCerficate = NULL;
		}

		PrintError ("%s(%d) Error> can't create semaphore httpSema",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	//---------------------------------------

	PrintExit;
	return VFIO_ERROR_SUCCESS;
}

/* Seek From Nexus : Long jump 인 경우 어느정도는 block 되어도 된다.
 * 단 Read 함수에서는 block 되면 waiting IO error, 0 return 시 endofStream, -1 return시 errorofstream 이 뜬다.*/
off_t DI_STREAM_SIMPLE_HTTP_SeekForNexus (VFIO_Handle_t tHandle, off_t offset, int whence)
{
	HINT32 nRet;
	DI_IPTUNER_t        *pHTuner  = NULL;
	STREAM_ACCESS_t     *pAccess  = NULL;

	PrintEnter;

	/* Getting IPTUNER handle from VFIO Handle */
	pHTuner = tHandle.pTunerHandle;
	if (pHTuner == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Error>  Invalid Tuner Handle in VFIO Handle\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}

	/* Now we just get the Stream handler */
	pAccess = (STREAM_ACCESS_t *)pHTuner->pHStream;
	if (NULL == pAccess)
	{
		PrintError("%s(%d) Error> Tuner handle doesn't include correct STREAM Handler. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return VFIO_ERROR_FAIL;
	}
#ifdef CONFIG_DLNA_CERTIFICATION
	if(pHTuner->tDlnaSetting.bDlna == TRUE && pHTuner->tDlnaSetting.eSeekMode == DI_IPTUNER_DLNA_SEEKMODE_NONE)
	{
		return offset;
	}
#endif
	if(VK_SEM_Get(pAccess->ulStreamSema) != VK_OK)
	{
		PrintError("%s(%d) Error> can't get ulStreamSema (tuner %d) (unHandle %d)\n"
		,__FUNCTION__
		,__LINE__
		,(HUINT32)tHandle.pTunerHandle
		,tHandle.tFile.callback.unHandle);
		return VFIO_ERROR_FAIL;
	}

	PrintData ("%s(%d) : Tuner(%u) offset=%"PRId64", whence=%d\n"
	,__FUNCTION__
	,__LINE__
	, (unsigned int)pHTuner
	, offset
	, whence);

	if(whence == SEEK_END)
	{
		offset = (off_t)pAccess->info.i_size-offset;
	}
#ifdef CONFIG_DLNA_CERTIFICATION
	if(pAccess->info.i_pos == offset)
	{
		(void)VK_SEM_Release (pAccess->ulStreamSema);
		PrintExit;
		return offset;
	}
#endif
	nRet = STREAM_HTTP_Seek ( pAccess, offset );
	if(nRet != DI_IPT_SUCCESS)
	{
		PrintError ("%s(%d) Error> STREAM_HTTP_Seek fail \n",__FUNCTION__,__LINE__);
		(void)VK_SEM_Release (pAccess->ulStreamSema);
		return -1;
	}

	PrintExit;
	(void)VK_SEM_Release (pAccess->ulStreamSema);
	return offset;
}

/* Bounds From Nexus */
int DI_STREAM_SIMPLE_HTTP_BoundsForNexus (VFIO_Handle_t tHandle, off_t *file_size)
{
	HINT32 nRet = VFIO_ERROR_SUCCESS;
	DI_IPTUNER_t        *pHTuner  = NULL;
	STREAM_ACCESS_t     *pAccess  = NULL;

#if 0
	PrintEnter;
#endif

	/* Getting IPTUNER handle from VFIO Handle */
	pHTuner = tHandle.pTunerHandle;
	if (pHTuner == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Error>  Invalid Tuner Handle in VFIO Handle\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}

	/* To cancel probe */
	if (pHTuner->bProbeCancel == TRUE)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Debug>  PROBE Cancel !!\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}

	/* Now we just get the Stream handler */
	pAccess = (STREAM_ACCESS_t *)pHTuner->pHStream;
	if (NULL == pAccess)
	{
		PrintError("%s(%d) Error> Tuner handle doesn't include correct STREAM Handler. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return VFIO_ERROR_FAIL;
	}

	*file_size = pAccess->info.i_size;

#if 0
	PrintData ("%s(%d) : i_size=%"PRId64"\n",__FUNCTION__,__LINE__, pAccess->info.i_size);
#endif

#if 0
	PrintExit;
#endif
	return nRet;
}

#ifdef CONFIG_DLNA_CERTIFICATION
DI_ERR_CODE DI_IPTUNER_HTTP_DisconnectForPause (DI_IPTUNER_t *pHTuner )
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	STREAM_ACCESS_t *pAccess = NULL;

	PrintEnter;

	if ( NULL == pHTuner )
	{
		PrintError ("%s (%d) Error>  invalid tuner handle (NULL) !\n",__FUNCTION__,__LINE__);
		return DI_ERR_INVALID_PARAM;
	}
	pAccess = (STREAM_ACCESS_t *)pHTuner->pHStream;
	if (NULL == pAccess)
	{
		PrintError("%s(%d) Error> Tuner handle doesn't include correct STREAM Handler. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return VFIO_ERROR_FAIL;
	}

	if (VK_SEM_Get(pAccess->ulStreamSema) != VK_OK)
	{
		PrintError("%s(%d) Error> can't get ulStreamSema\n"
		,__FUNCTION__
		,__LINE__
		);
		return VFIO_ERROR_FAIL;
	}
	if(!pHTuner->bDisconnectedForPause)
	{
		(void) STREAM_HTTP_Disconnect (pAccess);
		pHTuner->bDisconnectedForPause = TRUE;
	}

	(void)VK_SEM_Release (pAccess->ulStreamSema);

	PrintExit;
	return nRet;
}

DI_ERR_CODE DI_IPTUNER_HTTP_ConnectForResume (DI_IPTUNER_t *pHTuner )
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	STREAM_ACCESS_t *pAccess = NULL;
	HINT32 nSeekRetryCount = 0;

	PrintEnter;

	if ( NULL == pHTuner )
	{
		PrintError ("%s (%d) Error>  invalid tuner handle (NULL) !\n",__FUNCTION__,__LINE__);
		return DI_ERR_INVALID_PARAM;
	}
	pAccess = (STREAM_ACCESS_t *)pHTuner->pHStream;
	if (NULL == pAccess)
	{
		PrintError("%s(%d) Error> Tuner handle doesn't include correct STREAM Handler. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return VFIO_ERROR_FAIL;
	}

	if (VK_SEM_Get(pAccess->ulStreamSema) != VK_OK)
	{
		PrintError("%s(%d) Error> can't get ulStreamSema\n"
		,__FUNCTION__
		,__LINE__
		);
	}
	if(pHTuner->bDisconnectedForPause)
	{
		while( DI_IPT_SUCCESS != STREAM_HTTP_Seek ( pAccess ,  pAccess->info.i_pos ))
		{
			nSeekRetryCount++;
			PrintError ("%s(%d) Error> STREAM_HTTP_Seek fail... Retrying seek request (%d) times..\n"
							,__FUNCTION__
							,__LINE__
							,nSeekRetryCount);

			if ( FALSE == UTIL_SYS_GetLinkStatus () )
			{
				PrintError ("%s (%d) Debug> Network Cable unplugged !!\n",__FUNCTION__,__LINE__);
				break;
			}

			if (pHTuner->bProbeCancel == TRUE)
			{
				PrintError ("%s (%d) Debug> PROBE Cancel !!\n",__FUNCTION__,__LINE__);
				break;
			}
			if(pAccess->b_immediately_exit == TRUE)
			{
				PrintDebug("%s(%d) : handle (0x%x) Breaking Loop\n",__FUNCTION__,__LINE__, pHTuner);
				break;
			}
			if(nSeekRetryCount > DI_IPTUNER_HTTP_SEEK_RETRY_COUNT)
			{
					PrintError ("%s(%d) Error> STREAM_HTTP_Seek Retrying fail. Last Retrying count(%d) ...\n"
								,__FUNCTION__
								,__LINE__
								,nSeekRetryCount);
				break;
			}
		}

		pHTuner->bDisconnectedForPause = FALSE;
	}

	(void)VK_SEM_Release (pAccess->ulStreamSema);

	PrintExit;
	return nRet;
}
#endif

int DI_STREAM_HTTP_GetSmallFile ( const HINT8 *pUrl, const HINT8 *pUserAgentString , HUINT8 **pBuf, HINT32 *pSize)
{
	HINT32 nRet = DI_IPT_SUCCESS;
	STREAM_ACCESS_t *pAccess=NULL;
	HUINT8 *pReadBuffer=NULL;
	size_t sz_Real = 0;
	size_t sz_Try = 0;
	HINT32 nRead = 0;
	HINT32 nRetryCount =0;

	PrintEnter;

	if(!pUrl)
	{
		PrintError("%s(%d) : Error> URL has no data(NULL).\n",__FUNCTION__,__LINE__);
		return DI_IPT_EEXIT;
	}

	PrintError("%s(%d) Info> Downloading : %s \n",__FUNCTION__,__LINE__,pUrl);

	if(NULL == VK_strstr((char *)pUrl, "http://"))
	{
		PrintError("%s(%d) : Error> malformed URL. (%s).\n",__FUNCTION__,__LINE__, pUrl);
		return DI_IPT_EEXIT;
	}

	/***********************************
	 * Open HTTP Session using pUrl
	 ***********************************/
	pAccess = DD_MEM_Alloc(sizeof(STREAM_ACCESS_t));
	if(pAccess == NULL)
	{
		PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
		return DI_IPT_EEXIT;
	}

	VK_MEM_Memset(pAccess, 0x0, sizeof(STREAM_ACCESS_t));

	pAccess->psz_path = VK_StrDup ((void *) UTIL_GetOriginalURL ((const char*)pUrl) );
	if(!pAccess->psz_path)
	{
		PrintError("%s(%d) : Error> Fail VK_StrDup.\n",__FUNCTION__,__LINE__);
		VK_MEM_Free (pAccess);
		pAccess = NULL;
		return DI_IPT_EEXIT;
	}

	/* support YouView Specific UserAgent String */
	if(pUserAgentString != NULL)
	{
		pAccess->psz_user_agent = VK_StrDup ((void*)pUserAgentString);
	}
	else
	{
		/* using default agent string */
		pAccess->psz_user_agent = VK_StrDup ((void*)DI_IPTUNER_USER_AGENT_STRING);
	}

	nRet = STREAM_HTTP_Open (pAccess);
	if(nRet != DI_IPT_SUCCESS)
	{
		PrintError("%s(%d) : Error> STREAM_HTTP_Open Fail.\n",__FUNCTION__,__LINE__);
		goto open_error;
	}

	sz_Try = pAccess->info.i_size; /* total size */

	/* chck validation */
	if ( sz_Try <= 0 || sz_Try > 1*1024*1024 )
	{
		PrintError ("%s(%d) : Error > size of file is invalid !\n",__FUNCTION__,__LINE__);
		goto open_error;
	}

	/* allocate heap memory */
	pReadBuffer = (HUINT8*) DD_MEM_Alloc (sz_Try);
	if(pReadBuffer == NULL)
	{
		PrintError ("%s(%d) : Error > VK_malloc fail. !\n",__FUNCTION__,__LINE__);
		goto open_error;
	}
	VK_MEM_Memset (pReadBuffer, 0x0, sz_Try);

	/****************************************
	 * now ready for download.
	 ****************************************/
	do
	{
		nRetryCount = 0;
retry_read:
		nRead = STREAM_HTTP_Read ( pAccess, (HUINT8 *)pReadBuffer+sz_Real, sz_Try - sz_Real );
		/* Read Error : should be exit immediately */
		if (nRead < 0)
		{
			PrintError("%s(%d) Error> STREAM_HTTP_Read Fail!!\n", __FUNCTION__, __LINE__);
			goto read_error;
		}
		/* EOF or Error */
		else if (nRead == 0)
		{
			if(pAccess->info.b_eof == TRUE)
			{
				PrintDebug("%s(%d) Debug> Stream Read canceled or stopped by user !!\n", __FUNCTION__, __LINE__);
				break;
			}
			/* Something happened in STREAM Controller , Let's try again */
			else
			{
				nRetryCount ++;
				if(nRetryCount > 100)
				{
					nRetryCount = 0;
					PrintError("%s(%d) Error> Retry Time Over (%d)!!\n", __FUNCTION__, __LINE__, nRetryCount );
					goto read_error;
				}

				PrintError("%s(%d) Retry> (RetryCount=%d)...\n", __FUNCTION__, __LINE__, nRetryCount );
				goto retry_read;
			}
		}
		/* Summarize */
		sz_Real += nRead;
		PrintDebug ("%s(%d) Debug> %d bytes download \n",__FUNCTION__,__LINE__,sz_Real);
	}
	while (sz_Real == sz_Try);

	*pSize = sz_Real;
	*pBuf = pReadBuffer;

	STREAM_HTTP_Close (pAccess);

	PrintExit;
	return DI_IPT_SUCCESS;

open_error:
	*pSize = 0;
	(void) STREAM_HTTP_Close (pAccess);
	if( pAccess->psz_path != NULL)
	{
		VK_MEM_Free (pAccess->psz_path);
		pAccess->psz_path = NULL;
	}

	if( pAccess->psz_user_agent != NULL)
	{
		VK_MEM_Free (pAccess->psz_user_agent);
		pAccess->psz_user_agent = NULL;
	}

	if(pAccess!=NULL)
	{
		VK_MEM_Free (pAccess);
		pAccess = NULL;
	}

	PrintExit;
	return DI_IPT_EEXIT;

read_error:
	*pSize = 0;
	(void) STREAM_HTTP_Close (pAccess);
	if( pAccess->psz_path != NULL)
	{
		VK_MEM_Free (pAccess->psz_path);
		pAccess->psz_path = NULL;
	}

	if( pAccess->psz_user_agent != NULL)
	{
		VK_MEM_Free (pAccess->psz_user_agent);
		pAccess->psz_user_agent = NULL;
	}

	if(pReadBuffer != NULL)
	{
		DD_MEM_Free (pReadBuffer);
		pReadBuffer = NULL;
	}

	if(pAccess!=NULL)
	{
		VK_MEM_Free (pAccess);
		pAccess = NULL;
	}

	PrintExit;
	return DI_IPT_EEXIT;
}

#if defined(USE_SIMPLE_HTTP_CONTROLLER)
void DI_STREAM_HTTP_RegisterCallback (const char *pUrl)
{
	VFIO_Callback_t stCBFunction;
	/* 1. VFIO에 callback 함수 등록 */
	(void)VK_memset (&stCBFunction, 0x0, sizeof(VFIO_Callback_t));

	stCBFunction.Open 			= (VFIOOpenCB)	DI_STREAM_SIMPLE_HTTP_Open;
	stCBFunction.Close			= (VFIOCloseCB)	DI_STREAM_SIMPLE_HTTP_Close;
	stCBFunction.Read			= (VFIOReadCB)	DI_STREAM_SIMPLE_HTTP_Read;
	stCBFunction.Seek			= (VFIOSeekCB) 	DI_STREAM_SIMPLE_HTTP_Seek;
	stCBFunction.Tell			= (VFIOTellCB) 	DI_STREAM_SIMPLE_HTTP_Tell;
	stCBFunction.IsEOF			= (VFIOIsEOFCB) DI_STREAM_SIMPLE_HTTP_IsEOF;
	stCBFunction.OpenNexus		= (VFIOOpenFromNexusCB)	 DI_STREAM_SIMPLE_HTTP_OpenFromNexus;
	stCBFunction.SeekNexus		= (VFIOSeekForNexusCB) 	 DI_STREAM_SIMPLE_HTTP_SeekForNexus;
	stCBFunction.BoundsNexus	= (VFIOBoundsForNexusCB) DI_STREAM_SIMPLE_HTTP_BoundsForNexus;

	(void) VFIO_RegisterCallback (pUrl, stCBFunction);

	return;
}
#else
void DI_STREAM_HTTP_RegisterCallback (const char *pUrl)
{
	VFIO_Callback_t stCBFunction;
	/* 1. VFIO에 callback 함수 등록 */
	(void)VK_memset (&stCBFunction, 0x0, sizeof(VFIO_Callback_t));

	stCBFunction.Open 			= (VFIOOpenCB)	DI_STREAM_HTTP_Open;
	stCBFunction.Close			= (VFIOCloseCB)	DI_STREAM_HTTP_Close;
	stCBFunction.Read			= (VFIOReadCB)	DI_STREAM_HTTP_Read;
	stCBFunction.Seek			= (VFIOSeekCB) 	DI_STREAM_HTTP_Seek;
	stCBFunction.Tell			= (VFIOTellCB) 	DI_STREAM_HTTP_Tell;
	stCBFunction.IsEOF			= (VFIOIsEOFCB) DI_STREAM_HTTP_IsEOF;
	stCBFunction.OpenNexus		= (VFIOOpenFromNexusCB)	 DI_STREAM_HTTP_OpenFromNexus;
	stCBFunction.SeekNexus		= (VFIOSeekForNexusCB) 	 DI_STREAM_HTTP_SeekForNexus;
	stCBFunction.BoundsNexus	= (VFIOBoundsForNexusCB) DI_STREAM_HTTP_BoundsForNexus;

	(void) VFIO_RegisterCallback (pUrl, stCBFunction);

	return;
}
#endif

#if defined (CONFIG_PLAYREADY)
#define ASF_GUID_LENGTH 16
#define ASF_OBJECT_SIZE_LENGTH 8
HBOOL DI_STREAM_HTTP_GetASFWrmHeader(DI_IPTUNER_t* pHTuner)
{
	HBOOL bValidAsfHeader=TRUE;
	HINT32 iResult;
	HUINT32 uiIndex=0;
	HUINT32 uiOffset=0;
	HUINT16 usRecordCount=0;
	HUINT16 usRecordType=0;
	HUINT16 usRecordSize=0;
	HUINT32 uiDataSize=0;
	HUINT64 ullHeaderSize=0;
	HUINT8* pucBuf=NULL;
	long lReadResult=0;
	size_t sz_Try=0;
	size_t sz_Total=0;
	STREAM_ACCESS_t tAccess;
	HUINT8 ucAsfHeaderObject[ASF_GUID_LENGTH] = {0x30, 0x26, 0xB2, 0x75, 0x8E, 0x66, 0xCF, 0x11, 0xA6, 0xD9, 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C};
	HUINT8 ucAsfPlayReadyHeaderObject[ASF_GUID_LENGTH] = {0x10, 0x70, 0x63, 0xF4, 0xC3, 0x03, 0xCD, 0x42, 0xB9, 0x32, 0xB4, 0x8A, 0xDF, 0x3A, 0x6A, 0x54};
	HUINT8 ucAsfExtendedContentEncryptionObject[ASF_GUID_LENGTH] = {0x14, 0xE6, 0x8A, 0x29, 0x22, 0x26, 0x17, 0x4C, 0xB9, 0x35, 0xDA, 0xE0, 0x7E, 0xE9, 0x28, 0x9C};

	if(pHTuner == NULL)
	{
		PrintError("invalid parameter!\n");
		return FALSE;
	}

/*************************
*	1. Open the connection
*************************/
	VK_MEM_Memset(&tAccess,0,sizeof(STREAM_ACCESS_t));
	tAccess.psz_path = VK_StrDup (pHTuner->pUrl);
	if(tAccess.psz_path == NULL)
	{
		PrintError("VK_StrDup failed!\n");
		return FALSE;
	}

	iResult = STREAM_HTTP_Open(&tAccess);
	if(iResult != DI_IPT_SUCCESS)
	{
		PrintError("STREAM_HTTP_Open failed!\n");
		VK_MEM_Free(tAccess.psz_path);
		tAccess.psz_path = NULL;
		return FALSE;
	}

/*************************
*	2. Read the header
*	ASF_Header_Object GUID:
*	75B22630-668E-11CF-A6D9-00AA0062CE6C
*************************/
	pucBuf = VK_MEM_Alloc(ASF_GUID_LENGTH + ASF_OBJECT_SIZE_LENGTH);
	if(pucBuf == NULL)
	{
		PrintError("Out of Memory!\n");
		STREAM_HTTP_Close(&tAccess);
		return FALSE;
	}

	sz_Try = (ASF_GUID_LENGTH + ASF_OBJECT_SIZE_LENGTH);
	sz_Total = 0;
	while(sz_Total < (ASF_GUID_LENGTH + ASF_OBJECT_SIZE_LENGTH))
	{
		lReadResult = STREAM_HTTP_Read(&tAccess, (HUINT8 *)pucBuf, sz_Try);
		sz_Total += lReadResult;
		sz_Try -= lReadResult;
	}

	for(uiIndex=0; uiIndex<ASF_GUID_LENGTH; uiIndex++)
	{
		if( pucBuf[uiIndex] != ucAsfHeaderObject[uiIndex])
		{
			bValidAsfHeader=FALSE;
		}
	}

	if(bValidAsfHeader == TRUE)
	{
		VK_MEM_Memcpy(&ullHeaderSize, pucBuf+ASF_GUID_LENGTH, 8);
	}
	VK_MEM_Free(pucBuf);
	pucBuf = NULL;

/************************
*	3. Retrieve the WRM Header
*************************/
	if(ullHeaderSize != 0)
	{
		pucBuf = VK_MEM_Alloc(ullHeaderSize);
		if(pucBuf == NULL)
		{
			PrintError("Out of Memory!\n");
			STREAM_HTTP_Close(&tAccess);
			return FALSE;
		}

		iResult = STREAM_HTTP_Seek(&tAccess, 0);
		if(iResult != DI_IPT_SUCCESS)
		{
			PrintError("STREAM_HTTP_Seek failed!\n");
			VK_MEM_Free(pucBuf);
			STREAM_HTTP_Close(&tAccess);
			return FALSE;
		}

		sz_Try = ullHeaderSize;
		sz_Total = 0;
		while(sz_Total < ullHeaderSize)
		{
			lReadResult = STREAM_HTTP_Read(&tAccess, (HUINT8 *)pucBuf+sz_Total, sz_Try);
			sz_Total += lReadResult;
			sz_Try -= lReadResult;
		}

		VK_MEM_Memcpy(&pHTuner->tMediaInfo.ullAsfPreroll, pucBuf+80, 8);
		VK_MEM_Memcpy(&pHTuner->tMediaInfo.uiAsfPacketSize, pucBuf+96, 4);

/*************************
*	PlayReady Header GUID
*************************/
		uiIndex = 0;
		for(uiOffset=0; uiOffset < ullHeaderSize; uiOffset++)
		{
			for(uiIndex=0; uiIndex < ASF_GUID_LENGTH; uiIndex++)
			{
				if(pucBuf[uiOffset+uiIndex] != ucAsfPlayReadyHeaderObject[uiIndex])
				{
					break;
				}
			}
			if(uiIndex == ASF_GUID_LENGTH)
			{
				PrintDebug("PlayReady header is found!\n");
				pHTuner->tMediaInfo.eDrmType = DI_MEDIA_DRM_TYPE_PLAYREADY;
				break;
			}

			for(uiIndex=0; uiIndex < ASF_GUID_LENGTH; uiIndex++)
			{
				if(pucBuf[uiOffset+uiIndex] != ucAsfExtendedContentEncryptionObject[uiIndex])
				{
					break;
				}
			}
			if(uiIndex == ASF_GUID_LENGTH)
			{
				PrintDebug("ASF extended content encryption object is found!\n");
				pHTuner->tMediaInfo.eDrmType = DI_MEDIA_DRM_TYPE_WMDRM;
				break;
			}
		}

		if(uiIndex != 16)
		{
			PrintDebug ("Encryption guid abscent!\n");
			VK_MEM_Free(pucBuf);
			pucBuf = NULL;
			STREAM_HTTP_Close(&tAccess);
			return FALSE;
		}

		uiOffset += 16;

		if(pHTuner->tMediaInfo.eDrmType == DI_MEDIA_DRM_TYPE_WMDRM)
		{
/*************************
*	Extended Content Encryption Object
*	(Object Size 64bits)
*************************/
			uiOffset += 8;

/*************************
*	Extended Content Encryption Object
*	(Data Size 32bits)
*************************/
			uiDataSize = pucBuf[uiOffset] | pucBuf[uiOffset+1]<<8 | pucBuf[uiOffset+2]<<16 | pucBuf[uiOffset+3]<<24;
			uiOffset += 4;

			if(pHTuner->tMediaInfo.pucWrmHeader != NULL)
			{
				VK_MEM_Free(pHTuner->tMediaInfo.pucWrmHeader);
				pHTuner->tMediaInfo.pucWrmHeader = NULL;
			}

			pHTuner->tMediaInfo.pucWrmHeader = VK_MEM_Alloc(uiDataSize);
			if(pHTuner->tMediaInfo.pucWrmHeader == NULL)
			{
				PrintError("Out of Memory!\n");
				VK_MEM_Free(pucBuf);
				pucBuf = NULL;
				STREAM_HTTP_Close(&tAccess);
				return FALSE;
			}

			VK_MEM_Memcpy(pHTuner->tMediaInfo.pucWrmHeader, &pucBuf[uiOffset], uiDataSize);
			pHTuner->tMediaInfo.nWrmHeaderLen = uiDataSize;
			VK_MEM_Free(pucBuf);
			pucBuf = NULL;
			STREAM_HTTP_Close(&tAccess);
			return TRUE;
		}
		else
		{
/*************************
*	PSI object (System Version 32bits)
*	PSI object (Data Size 32bits)
*************************/
			uiOffset += 8;

/*************************
*	PlayReady Header Objects (Length 32bits)
*	PlayReady Header Objects (Record Count 16bits)
*************************/
			usRecordCount = pucBuf[uiOffset+4] | (pucBuf[uiOffset+5]<<8);
			uiOffset += 6;

/*************************
*	PlayReady Header Objects (Record Type 16bits)
*	PlayReady Header Objects (Record Length 16bits)
*	PlayReady Header Objects (Record Value)
*************************/
			for(uiIndex=0; uiIndex<usRecordCount; uiIndex++)
			{
				usRecordType = pucBuf[uiOffset] | (pucBuf[uiOffset+1]<<8);
				usRecordSize = pucBuf[uiOffset+2] | (pucBuf[uiOffset+3]<<8);
				uiOffset+=4;
				if(usRecordType == 0x1)
				{
					PrintDebug("WRM Header Found!\n");
					break;
				}
				else
				{
					uiOffset+=usRecordSize;
				}
			}

			if(uiIndex == usRecordCount)
			{
				PrintDebug("WRM Header abscent!\n");
				VK_MEM_Free(pucBuf);
				pucBuf = NULL;
				STREAM_HTTP_Close(&tAccess);
				return FALSE;
			}

			if(pHTuner->tMediaInfo.pucWrmHeader != NULL)
			{
				VK_MEM_Free(pHTuner->tMediaInfo.pucWrmHeader);
				pHTuner->tMediaInfo.pucWrmHeader = NULL;
			}

			pHTuner->tMediaInfo.pucWrmHeader = VK_MEM_Alloc(usRecordSize);
			if(pHTuner->tMediaInfo.pucWrmHeader == NULL)
			{
				PrintError("Out of Memory!\n");
				VK_MEM_Free(pucBuf);
				pucBuf = NULL;
				STREAM_HTTP_Close(&tAccess);
				return FALSE;
			}

			VK_MEM_Memcpy(pHTuner->tMediaInfo.pucWrmHeader, &pucBuf[uiOffset], usRecordSize);
			pHTuner->tMediaInfo.nWrmHeaderLen = usRecordSize;
			VK_MEM_Free(pucBuf);
			pucBuf = NULL;
			STREAM_HTTP_Close(&tAccess);
			return TRUE;
		}
	}

	STREAM_HTTP_Close(&tAccess);
	return FALSE;
}

HBOOL DI_STREAM_HTTP_GetSimpleIndex(DI_IPTUNER_t* pHTuner)
{
	HUINT8* pucBuf=NULL;
	HUINT8* pucIndex=NULL;
	HUINT32 uiIndex=0;
	HINT32 iResult;
	STREAM_ACCESS_t tAccess;
	long lReadResult=0;
	size_t sz_Try=0;
	size_t sz_Total=0;
	HUINT8 ucAsfDataObject[ASF_GUID_LENGTH] = {0x36, 0x26, 0xB2, 0x75, 0x8E, 0x66, 0xCF, 0x11, 0xD9, 0xA6, 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C};
	HUINT8 ucAsfSimpleIndexObject[ASF_GUID_LENGTH] = {0x90, 0x08, 0x00, 0x33, 0xB1, 0xE5, 0xCF, 0x11, 0x89, 0xF4, 0x00, 0xA0, 0xC9, 0x03, 0x49, 0xCB};
	HBOOL bValidAsfIndex=TRUE;
	HBOOL bValidAsfData=TRUE;
	HUINT64 ullIndexSize=0;
	HUINT64 ullFileSize=0;
	HUINT64 ullOffset=0;
	HUINT64 ullObjectSize=0;
	HBOOL bResult;

	if(pHTuner == NULL)
	{
		PrintError("invalid parameter!\n");
		return FALSE;
	}

/*************************
*	1. Open the connection
*************************/
	VK_MEM_Memset(&tAccess,0,sizeof(STREAM_ACCESS_t));
	tAccess.psz_path = VK_StrDup (pHTuner->pUrl);
	if(tAccess.psz_path == NULL)
	{
		PrintError("VK_StrDup failed!\n");
		return FALSE;
	}

	iResult = STREAM_HTTP_Open(&tAccess);
	if(iResult != DI_IPT_SUCCESS)
	{
		PrintError("STREAM_HTTP_Open failed!\n");
		VK_MEM_Free(tAccess.psz_path);
		return FALSE;
	}
	ullFileSize = tAccess.info.i_size;

	pucBuf = VK_MEM_Alloc(ASF_GUID_LENGTH+ASF_OBJECT_SIZE_LENGTH);
	if(pucBuf == NULL)
	{
		PrintError("Out of Memory!\n");
		STREAM_HTTP_Close(&tAccess);
		VK_MEM_Free(tAccess.psz_path);
		return FALSE;
	}

	while(ullOffset < ullFileSize)
	{
/*************************
*	Check GUID (16 Bytes)
*************************/
		sz_Try = (ASF_GUID_LENGTH + ASF_OBJECT_SIZE_LENGTH);
		sz_Total = 0;
		while(sz_Total < (ASF_GUID_LENGTH + ASF_OBJECT_SIZE_LENGTH))
		{
			lReadResult = STREAM_HTTP_Read(&tAccess, (HUINT8 *)pucBuf, sz_Try);
			sz_Total += lReadResult;
			sz_Try -= lReadResult;
		}
		VK_MEM_Memcpy(&ullObjectSize, pucBuf, 8);

		for(uiIndex=0; uiIndex<ASF_GUID_LENGTH; uiIndex++)
		{
			if( pucBuf[uiIndex] != ucAsfSimpleIndexObject[uiIndex])
			{
				bValidAsfIndex=FALSE;
			}

			if( pucBuf[uiIndex] != ucAsfDataObject[uiIndex])
			{
				bValidAsfData=FALSE;
			}
		}

/*************************
*	parse the data offset
*************************/
		if(bValidAsfData == TRUE)
		{
			pHTuner->tMediaInfo.ullAsfDataOffset = ullObjectSize;
		}
		else
		{
			bValidAsfData = TRUE;
		}

		if(bValidAsfIndex == TRUE)
		{
/*************************
*	parse the simple index
*************************/
			pucIndex = VK_MEM_Alloc(ullObjectSize);
			if(pucIndex == NULL)
			{
				PrintError("Out of memory!\n");
				bResult = FALSE;
				goto ParsingDone;
			}

			iResult = STREAM_HTTP_Seek(&tAccess, ullOffset);
			if(iResult != DI_IPT_SUCCESS)
			{
				PrintError("STREAM_HTTP_Seek failed!\n");
				bResult = FALSE;
				goto ParsingDone;
			}

			sz_Try = ullObjectSize;
			sz_Total = 0;
			while(sz_Total < ullIndexSize)
			{
				lReadResult = STREAM_HTTP_Read(&tAccess, (HUINT8 *)pucIndex+sz_Total, sz_Try);
				sz_Total += lReadResult;
				sz_Try -= lReadResult;
			}

			VK_MEM_Memcpy(&pHTuner->tMediaInfo.ullAsfIndexTimeInterval, pucIndex+40, 8);
			VK_MEM_Memcpy(&pHTuner->tMediaInfo.uiIndexEntriesCount, pucIndex+52, 4);

			pHTuner->tMediaInfo.pucIndex = VK_MEM_Alloc(6 * pHTuner->tMediaInfo.uiIndexEntriesCount);
			if(pHTuner->tMediaInfo.pucIndex == NULL)
			{
				PrintError("out of memory!\n");
				bResult = FALSE;
				goto ParsingDone;
			}
			VK_MEM_Memcpy(pHTuner->tMediaInfo.pucIndex, pucIndex+56, 6*pHTuner->tMediaInfo.uiIndexEntriesCount);
			break;
		}
		else
		{
/*************************
*	skip all the other objects
*************************/
			ullOffset += ullObjectSize;
			if(ullOffset < ullFileSize)
			{
				iResult = STREAM_HTTP_Seek(&tAccess, ullOffset);
				if(iResult != DI_IPT_SUCCESS)
				{
					PrintError("STREAM_HTTP_Seek failed!\n");
					bResult = FALSE;
					goto ParsingDone;
				}
			}
			bValidAsfData = TRUE;
		}
	}

ParsingDone:
	STREAM_HTTP_Close(&tAccess);
	VK_MEM_Free(tAccess.psz_path);
	VK_MEM_Free(pucBuf);
	VK_MEM_Free(pucIndex);

	return bResult;
}

DI_ERR_CODE DI_STREAM_HTTP_GetStreamInfo(DI_IPTUNER_t* pHTuner, HINT64 *file_pos, HINT64 *file_size)
{
	STREAM_ACCESS_t     *pAccess  = NULL;

	*file_pos = *file_size = 0;
	if (pHTuner == NULL)
	{
		PrintError ("%s (%d) Error>  Invalid Tuner Handle\n",__FUNCTION__,__LINE__);
		return DI_ERR_INVALID_PARAM;
	}

	/* Now we just get the Stream handler */
	pAccess = (STREAM_ACCESS_t *)pHTuner->pHStream;
	if (NULL == pAccess)
	{
		PrintError("%s(%d) Error> Tuner handle doesn't include correct STREAM Handler. Check This!\n", __FUNCTION__, __LINE__);
		return DI_ERR_INVALID_PARAM;
	}

	*file_pos = pAccess->info.i_pos;
	*file_size = pAccess->info.i_size;
	PrintData ("%s(%d) : i_pos=%"PRId64", i_size=%"PRId64"\n",__FUNCTION__,__LINE__, *file_pos, *file_size);
	return ERR_OK;
}

#endif

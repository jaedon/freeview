/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_dlna.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:		Dongho Kim $
 * Current Author:	$Author: kimdh1@humaxdigital.com $
 * Date:			$Date: 7 15 10:20:48 KST 2009 $
 * File Description:	DLNA PLAY
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
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <inttypes.h>

#include "bstd.h"
#include "vfio.h"

#include "vkernel.h"
#include "taskdef.h"
#include "di_uart.h"

#include "di_media.h"

#include "util_url.h"
#include "di_iptuner.h"
#include "di_iptuner_dlna.h"
#include "di_iptuner_config.h"
/* End Including Headers */

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */

//#define DI_IPTUNER_DLNA_DEBUG
//#define DI_IPTUNER_DLNA_CRITICAL_DEBUG

#ifdef IPTUNER_ALL_DEBUG

#ifdef DI_IPTUNER_DLNA_DEBUG
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
#endif

#define UNUSED_PARAM(x)		((void)x)


/* End #define */

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */
static HULONG s_ulDLNASema;


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
static HUINT8						s_ucIsInitialized = FALSE;

static URL_t						stURL;

static DLNA_FileFunction_t			s_tDLNAFileFn;

/* End static variable */


/*******************************************************************/
/************************ static funtions **************************/
/*******************************************************************/
#if __________task__________
#endif

#if __________p__________
#endif
static HINT32 P_IPTUNER_InitDLNA (void)
{
	HINT32 nVKRet;

	PrintEnter;

	if ( TRUE != s_ucIsInitialized )
	{
/* TODO : check */
		nVKRet = VK_SEM_Create(&s_ulDLNASema, "DNPTunerSema", VK_SUSPENDTYPE_PRIORITY);
		if ( VK_OK != nVKRet )
		{
			PrintError ("[%s] [%d] DNP Timer sema creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet );
			return -1;
		}

		s_ucIsInitialized = TRUE;
	}
	else
	{
		PrintError  ("%s (%d) Error>  Already initialized !\n",__FUNCTION__,__LINE__);
	}

	PrintExit;

	return 0;
}


/** VFIO (for DLNA) Open Function */
static int P_IPTUNER_OpenDLNA (HINT8 *path, HINT8 *mode, VFIO_Handle_t *ptHandle)
{
	int		ret=VFIO_ERROR_SUCCESS;
	DI_ERR_CODE 				nRet = DI_ERR_OK;
	char	*pucIsHTTP=NULL;
	DI_IPTUNER_DLNA_FILEDESC tFileDescription;
	DI_IPTUNER_DLNA_FILEHANDLE	hDLNAHandle=NULL;

	UNUSED_PARAM (mode);

	PrintEnter;

	VK_memset(&tFileDescription, 0x0, sizeof(tFileDescription));

	PrintData ("[%s] [%d] path=%s, mode=%s\n",__FUNCTION__,__LINE__, path, mode);

	if(s_tDLNAFileFn.Open != NULL)
	{
		tFileDescription.url = (char *)path;

		pucIsHTTP = VK_strstr((char *)path, "http://");
		if(pucIsHTTP != NULL)
		{
			tFileDescription.access_type = DLNA_FILEACCESS_HTTP;
		}

		nRet = s_tDLNAFileFn.Open(&tFileDescription, &hDLNAHandle);
		if ( nRet != DI_ERR_OK )
		{
			ptHandle->ucAvailable = 0;
			ret = VFIO_ERROR_IN_DEVICE;
			PrintError ("%s (%d) Error>  %s open Error, please check file path\n",__FUNCTION__,__LINE__,path);
		}
		else
		{
			ptHandle->ucAvailable = 1;
			ptHandle->tFile.callback.unHandle = (HUINT32)hDLNAHandle;

			PrintData ("%s (%d) callback handle=0x%x\n",__FUNCTION__,__LINE__, ptHandle->tFile.callback.unHandle);
		}
	}
	else
	{
		PrintData ("%s (%d) Unregist Open Function\n",__FUNCTION__,__LINE__);
	}

	PrintExit;

	return ret;
}

/** VFIO (for DLNA) Close Function */
static int P_IPTUNER_CloseDLNA (VFIO_Handle_t tHandle)
{
	int		ret=VFIO_ERROR_SUCCESS;
	DI_IPTUNER_DLNA_FILEHANDLE	hDLNAHandle=NULL;

	PrintEnter;

	if(s_tDLNAFileFn.Close != NULL)
	{
		tHandle.ucAvailable = 0;

		hDLNAHandle = (DI_IPTUNER_DLNA_FILEHANDLE)tHandle.tFile.callback.unHandle;

		if(s_tDLNAFileFn.Close(hDLNAHandle) != 0)
		{
			ret = VFIO_ERROR_FAIL;
		}
	}
	else
	{
		PrintData ("%s (%d) Unregist Close Function\n",__FUNCTION__,__LINE__);
	}

	PrintExit;

	return ret;
}

/** VFIO (for DLNA) Read Function */
static int P_IPTUNER_ReadDLNA (VFIO_Handle_t tHandle, void *ptr, size_t size, size_t nmemb, size_t *ptReadSize)
{
	int		ret=VFIO_ERROR_SUCCESS;
	DI_IPTUNER_DLNA_FILEHANDLE	hDLNAHandle=NULL;

	PrintEnter;

	PrintData ("%s (%d) size=%d, nmemb=%d\n",__FUNCTION__,__LINE__, size, nmemb);

	if(s_tDLNAFileFn.Read != NULL)
	{
		hDLNAHandle = (DI_IPTUNER_DLNA_FILEHANDLE)tHandle.tFile.callback.unHandle;

		if(s_tDLNAFileFn.Read(hDLNAHandle, ptr, (HUINT32)(size*nmemb), (HUINT32 *)ptReadSize) != 0)
		{
			ret = VFIO_ERROR_FAIL;
		}

		PrintData("%s (%d) ret(%d), readCnt(%d)\n",__FUNCTION__,__LINE__, ret, *ptReadSize);
	}
	else
	{
		PrintData ("%s (%d) Unregist Read Function\n",__FUNCTION__,__LINE__);
		ret = VFIO_ERROR_FAIL;
	}

	PrintExit;

	return ret;
}

/** VFIO (for DLNA) Seek Function */
static int P_IPTUNER_SeekDLNA (VFIO_Handle_t tHandle, off_t offset, int whence)
{
	int	ret=VFIO_ERROR_SUCCESS;
	int	curOption = 0;
	DI_IPTUNER_DLNA_FILEHANDLE	hDLNAHandle=NULL;

	PrintEnter;

	PrintData ("%s (%d) offset=%"PRId64", whence=%d\n",__FUNCTION__,__LINE__,offset, whence);

	if(s_tDLNAFileFn.Seek != NULL)
	{
		switch(whence)
		{
			case SEEK_CUR:
				curOption = DLNA_FILESEEK_CUR;
				break;
			case SEEK_END:
				curOption = DLNA_FILESEEK_END;
				break;
			case SEEK_SET:
				curOption = DLNA_FILESEEK_SET;
				break;
			default:
				PrintError ("%s (%d) Invalid Param(whence=%d)\n",__FUNCTION__,__LINE__,whence);
				PrintExit;
				return VFIO_ERROR_FAIL;
		}

		hDLNAHandle = (DI_IPTUNER_DLNA_FILEHANDLE)tHandle.tFile.callback.unHandle;
		(void)s_tDLNAFileFn.Seek(hDLNAHandle, (HINT64)offset, (HINT32)curOption);
	}
	else
	{
		PrintData ("%s (%d) Unregist Seek Function\n",__FUNCTION__,__LINE__);
	}

	PrintExit;

	return ret;
}

/** VFIO (for DLNA) Tell Function */
static int P_IPTUNER_TellDLNA (VFIO_Handle_t tHandle, off_t *plCurOffset)
{
	int		ret=VFIO_ERROR_SUCCESS;
	DI_IPTUNER_DLNA_FILEHANDLE	hDLNAHandle=NULL;

	PrintEnter;

	if(s_tDLNAFileFn.Tell != NULL)
	{
		hDLNAHandle = (DI_IPTUNER_DLNA_FILEHANDLE)tHandle.tFile.callback.unHandle;

		if(s_tDLNAFileFn.Tell(hDLNAHandle, (HUINT64 *)plCurOffset) != 0)
		{
			ret = VFIO_ERROR_FAIL;
		}
	}
	else
	{
		PrintData ("%s (%d) Unregist Tell Function\n",__FUNCTION__,__LINE__);
	}

	PrintExit;

	return ret;
}

/** VFIO (for DLNA) IsEOF Function */
static int P_IPTUNER_IsEOFDLNA (VFIO_Handle_t tHandle)
{
	int			ret=VFIO_ERROR_SUCCESS;
	DI_IPTUNER_DLNA_FILEHANDLE	hDLNAHandle=NULL;

	PrintEnter;

	if(s_tDLNAFileFn.IsEOF != NULL)
	{
		hDLNAHandle = (DI_IPTUNER_DLNA_FILEHANDLE)tHandle.tFile.callback.unHandle;

		if(s_tDLNAFileFn.IsEOF(hDLNAHandle) != 0)
		{
			ret = VFIO_ERROR_FAIL;
		}
	}
	else
	{
		PrintData ("%s (%d) Unregist IsEOF Function\n",__FUNCTION__,__LINE__);
	}

	PrintExit;

	return ret;
}

/** VFIO (for DLNA) OpenFromNexus Function */
static int P_IPTUNER_OpenDLNAFromNexus (HINT8 *path, const unsigned int flags, VFIO_Handle_t *ptHandle)
{
	int		ret=VFIO_ERROR_SUCCESS;
	DI_ERR_CODE 				nRet = DI_ERR_OK;
	char	*pucIsHTTP=NULL;
	DI_IPTUNER_DLNA_FILEDESC tFileDescription;
	DI_IPTUNER_DLNA_FILEHANDLE	hDLNAHandle=NULL;

	UNUSED_PARAM (flags);

	PrintEnter;

	VK_memset(&tFileDescription, 0x0, sizeof(tFileDescription));

	PrintData ("%s (%d)  path=%s, flags=%d\n",__FUNCTION__,__LINE__, path, flags);

	if(s_tDLNAFileFn.OpenForNexus != NULL)
	{
		tFileDescription.url = (char *)path;

		pucIsHTTP = VK_strstr((char *)path, "http://");
		if(pucIsHTTP != NULL)
		{
			tFileDescription.access_type = DLNA_FILEACCESS_HTTP;
		}

		nRet = s_tDLNAFileFn.OpenForNexus(&tFileDescription, &hDLNAHandle);
		if ( nRet != DI_ERR_OK )
		{
			ptHandle->ucAvailable = 0;
			ret = VFIO_ERROR_IN_DEVICE;
			PrintError ("%s (%d) Error>  %s open Error, please check file path\n",__FUNCTION__,__LINE__,path);
		}
		else
		{
			ptHandle->ucAvailable = 1;
			ptHandle->tFile.callback.unHandle = (HUINT32)hDLNAHandle;
			PrintData ("%s (%d) callback handle=0x%x\n",__FUNCTION__,__LINE__, ptHandle->tFile.callback.unHandle);
		}
	}
	else
	{
		PrintData ("%s (%d) Unregist Open Function\n",__FUNCTION__,__LINE__);
	}

	PrintExit;

	return ret;
}

/** VFIO (for DLNA) SeekFromNexus Function */
static off_t P_IPTUNER_SeekDLNAForNexus (VFIO_Handle_t tHandle, off_t offset, int whence)
{
	int	curOption;
	DI_IPTUNER_DLNA_FILEHANDLE	hDLNAHandle=NULL;
	off_t nOffsetRet=0;

	PrintEnter;

	PrintData ("%s (%d) offset=%"PRId64", whence=%d\n",__FUNCTION__,__LINE__,offset, whence);

	if(s_tDLNAFileFn.SeekForNexus != NULL)
	{
		switch(whence)
		{
			case SEEK_CUR:
				curOption = DLNA_FILESEEK_CUR;
				break;
			case SEEK_END:
				curOption = DLNA_FILESEEK_END;
				break;
			case SEEK_SET:
				curOption = DLNA_FILESEEK_SET;
				break;
			default:
				PrintError ("%s (%d) Invalid Param(whence=%d)\n",__FUNCTION__,__LINE__,whence);
				PrintExit;
				return -1;
		}

		hDLNAHandle = (DI_IPTUNER_DLNA_FILEHANDLE)tHandle.tFile.callback.unHandle;

		nOffsetRet=s_tDLNAFileFn.SeekForNexus(hDLNAHandle, (HINT64)offset, (HINT32)curOption);
	}
	else
	{
		PrintData ("%s (%d) Unregist Seek Function\n",__FUNCTION__,__LINE__);
	}

	PrintExit;

	return nOffsetRet;
}

/** VFIO (for DLNA) BoundsFromNexus Function */
static int P_IPTUNER_BoundsDLNAForNexus(VFIO_Handle_t tHandle, off_t *file_size)
{
	int		ret=VFIO_ERROR_SUCCESS;
	DI_IPTUNER_DLNA_FILEHANDLE	hDLNAHandle=NULL;

	UNUSED_PARAM(tHandle);

	if(s_tDLNAFileFn.GetFileSize != NULL)
	{
		hDLNAHandle = (DI_IPTUNER_DLNA_FILEHANDLE)tHandle.tFile.callback.unHandle;

		if(s_tDLNAFileFn.BoundsForNexus(hDLNAHandle, (HINT64 *)file_size) != 0)
		{
			ret = VFIO_ERROR_FAIL;
		}
	}
	else
	{
		PrintData ("%s (%d) Unregist GetFileSize Function\n",__FUNCTION__,__LINE__);
	}

	return ret;
}

static HINT32 P_IPTUNER_TermDLNA (void)
{
	PrintEnter;

	if ( TRUE == s_ucIsInitialized )
	{
/* TODO : check */
		(void)VK_SEM_Destroy(s_ulDLNASema);
		s_ucIsInitialized = FALSE;
	}
	else
	{
		PrintError  ("%s (%d) Error>  Not initialized !\n",__FUNCTION__,__LINE__);
	}

	PrintExit;

	return 0;
}

#if __________di__________
#endif
/*******************************************************************/
/************************ public funtions **************************/
/*******************************************************************/
DI_ERR_CODE DI_IPTUNER_OpenDLNA (DI_IPTUNER_Index_e nTunerIndex, HINT8 *pUrl, DI_MEDIA_INFO_t *pstMediaInfo)
{
	HINT32	nRet = DI_ERR_OK;
	HINT8 	*tmpUrl=NULL;
	HINT32	nUrlLen;

	DI_IPTUNER_Msg_t s_Msg;
	VFIO_Callback_t	stCBFunction;

	PrintEnter;

	UNUSED_PARAM(nTunerIndex);

	VK_memset(&stURL,0,sizeof(URL_t));

	if ( TRUE != s_ucIsInitialized )
	{
		(void)P_IPTUNER_InitDLNA();
	}
	/* else - { continue } */

	if ( NULL == pUrl )
	{
		PrintError ("%s (%d) Error>  pUrl is NULL !\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_INVALID_PARAM;
		return nRet;
	}
	else if (pstMediaInfo == NULL)
	{
		PrintError ("%s (%d) Error> pstMediaInfo is NULL !\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_INVALID_PARAM;
		return nRet;
	}
	/* else - { continue } */

//	PrintData ("%s (%d)  pUrl=%s, nTunerIndex=%d\n",__FUNCTION__,__LINE__, pUrl, nTunerIndex);

	/* 1. VFIO에 callback 함수 등록 */
	(void)VK_memset(&stCBFunction, 0x0, sizeof(VFIO_Callback_t));

	stCBFunction.Open 			= (VFIOOpenCB)  P_IPTUNER_OpenDLNA;
	stCBFunction.Close			= (VFIOCloseCB) P_IPTUNER_CloseDLNA;
	stCBFunction.Read			= (VFIOReadCB)  P_IPTUNER_ReadDLNA;
	stCBFunction.Seek			= (VFIOSeekCB)  P_IPTUNER_SeekDLNA;
	stCBFunction.Tell			= (VFIOTellCB)  P_IPTUNER_TellDLNA;
	stCBFunction.IsEOF			= (VFIOIsEOFCB) P_IPTUNER_IsEOFDLNA;
	stCBFunction.OpenNexus		= (VFIOOpenFromNexusCB) P_IPTUNER_OpenDLNAFromNexus;
	stCBFunction.SeekNexus		= (VFIOSeekForNexusCB) P_IPTUNER_SeekDLNAForNexus;
	stCBFunction.BoundsNexus	= (VFIOBoundsForNexusCB) P_IPTUNER_BoundsDLNAForNexus;
	(void) VFIO_RegisterCallback (VFIO_CALLBACK_TYPE_DLNA, stCBFunction);

	/* 2. Parse URL */
	(void) VK_memset(&s_Msg, 0x0, sizeof(s_Msg));

	/** IPTUNER DLNA 를 Open할때에는 [DLNA]를 붙이지 않음. VFIO에서만 구별을 위해 [DLNA]를 붙이나
	 * 추후 VFIO에서 현재 Type을 알아서 판단하도록 수정이 필요함. */
	(void) UTIL_UrlParse (&stURL,(const char*)pUrl,  0);

	/*XXX: For TEST DI_MEDIA_Probe를 위해서는 URL앞에 [DLNA]를 붙여 줘야 한다.*/
	nUrlLen = VK_strlen((char *)pUrl)+sizeof("[DLNA]");

	tmpUrl = (HINT8*)DD_MEM_Alloc (nUrlLen);
	(void) VK_memset (tmpUrl,0,nUrlLen);
	nRet = VK_snprintf((char *)tmpUrl,nUrlLen,"[DLNA]%s",pUrl);
	if(nRet<0)
	{
		PrintError ("%s (%d) Error> can't make url for dlna : %d\n",__FUNCTION__,__LINE__, nRet);
		nRet = DI_ERR_ERROR;
	}
	else
	{
		/* 5. Probe Media */
		nRet = DI_MEDIA_Probe ((HUINT8*)tmpUrl, pstMediaInfo);
		if (nRet != DI_ERR_OK)
		{
			PrintError ("%s (%d) Error> DI_MEDIA_Probe : %d\n",__FUNCTION__,__LINE__, nRet);
		}
#ifdef DI_IPTUNER_DLNA_DEBUG
		else
		{
			(void) DI_MEDIA_PrintProbeInfo (pstMediaInfo);
		}
		/* else - { continue } */
#endif
	}

	DD_MEM_Free (tmpUrl);

	UTIL_UrlClean (&stURL);
	PrintExit;

	return nRet;
}


DI_ERR_CODE DI_IPTUNER_CloseDLNA (DI_IPTUNER_Index_e nTunerIndex)
{
	PrintEnter;

	UNUSED_PARAM (nTunerIndex);
	UTIL_UrlClean (&stURL);

	(void)VFIO_UnregisterCallback (VFIO_CALLBACK_TYPE_DLNA);

	(void)P_IPTUNER_TermDLNA();

	PrintExit;

	return DI_ERR_OK;
}

DI_ERR_CODE DI_IPTUNER_ControlDLNA (DI_IPTUNER_Index_e nTunerIndex,  DI_IPTUNER_CMD_e cmd, HINT32 val)
{
	UNUSED_PARAM (nTunerIndex);
	UNUSED_PARAM (cmd);
	UNUSED_PARAM (val);
	return DI_ERR_OK;
}

/**
 * @brief 	DLNA File Function을 Install한다.
 * @param 	stDLNAFileFn		DLNA File Function List
 * @return 	DI_ERR_CODE
 * @date 	2009/07/15
 * @author 	kimdh1@humaxdigital.com
 * @remark
 * @see
 */
DI_ERR_CODE DI_IPTUNER_DLNA_InstallDLNAFunction (DLNA_FileFunction_t stDLNAFileFn)
{
	PrintEnter;

	s_tDLNAFileFn.Open = stDLNAFileFn.Open;
	s_tDLNAFileFn.Close = stDLNAFileFn.Close;
	s_tDLNAFileFn.Read = stDLNAFileFn.Read;
	s_tDLNAFileFn.Seek = stDLNAFileFn.Seek;
	s_tDLNAFileFn.Tell = stDLNAFileFn.Tell;
	s_tDLNAFileFn.IsEOF = stDLNAFileFn.IsEOF;
	s_tDLNAFileFn.GetFileSize = stDLNAFileFn.GetFileSize;
	s_tDLNAFileFn.OpenForNexus = stDLNAFileFn.OpenForNexus;
	s_tDLNAFileFn.SeekForNexus = stDLNAFileFn.SeekForNexus;
	s_tDLNAFileFn.BoundsForNexus = stDLNAFileFn.BoundsForNexus;

	PrintExit;

	return DI_ERR_OK;
}



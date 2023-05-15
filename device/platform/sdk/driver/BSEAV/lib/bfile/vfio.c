/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   vfio.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:	Kim,Jeongsu
 * Current Author:	$Author: Jeon Sung Yong $
 * Date:			$Date: Mon Apr 13 18:32:48 KST 2009 $
 * File Description:	Virtual File I/O
 * Module: 			
 * Remarks:		
 */	

/*******************************************************************/ 	
/* Copyright (c) 2009 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* Start Including Header Files */
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "vfio.h"

/* End Including Headers */


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
#ifndef VFIO_DEBUG
#define VFIO_DEBUG 	0
#endif

#ifndef	__REMOVE_ALL_DEBUG__
#if VFIO_DEBUG
#define	PRINT_DATA(x)			printf x
#define	PRINT_FUNC(x)			printf x
#define	PRINT_WARN(x)			printf x
#define	PRINT_ERROR(x)			printf x
#else
#define	PRINT_DATA(x)
#define	PRINT_FUNC(x)
#define	PRINT_WARN(x)
#define	PRINT_ERROR(x)			printf x
#endif
#else   /* __REMOVE_ALL_DEBUG__ */
#define PRINT_DATA(x)
#define PRINT_FUNC(x)
#define PRINT_WARN(x)
#define PRINT_ERROR(x)
#endif  /* end of __REMOVE_ALL_DEBUG__ */

#if	_FILE_OFFSET_BITS==64
#define OFF_T_FORMAT "%lld"
#define OFF_T_FORMAT_CAST long long
#else
#define OFF_T_FORMAT "%ld"
#define OFF_T_FORMAT_CAST long
#endif


/* End #define */


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */

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
static VFIO_Callback_t		s_stCallbackFn[VFIO_CALLBACK_TYPE_MAX];

/* End static variable */


void VFIO_RegisterCallback (VFIO_CALLBACK_TYPE_e eCBtype, VFIO_Callback_t stVfioCallbackFn)
{
	PRINT_FUNC(("[++VFIO_RegisterCallback] callback type=%d\n", eCBtype));

#if	1
	(void)memcpy(&s_stCallbackFn[eCBtype], &stVfioCallbackFn, sizeof(VFIO_Callback_t));
#else
	s_stCallbackFn[eCBtype].Open 			= stVfioCallbackFn.Open;
	s_stCallbackFn[eCBtype].Close			= stVfioCallbackFn.Close;
	s_stCallbackFn[eCBtype].Read			= stVfioCallbackFn.Read;
	s_stCallbackFn[eCBtype].Seek			= stVfioCallbackFn.Seek;
	s_stCallbackFn[eCBtype].Tell			= stVfioCallbackFn.Tell;
	s_stCallbackFn[eCBtype].IsEOF			= stVfioCallbackFn.IsEOF;
	s_stCallbackFn[eCBtype].OpenNexus		= stVfioCallbackFn.OpenNexus;
	s_stCallbackFn[eCBtype].SeekNexus		= stVfioCallbackFn.SeekNexus;
	s_stCallbackFn[eCBtype].BoundsNexus	= stVfioCallbackFn.BoundsNexus;
#endif

	PRINT_FUNC(("[--VFIO_RegisterCallback]\n"));
}

void VFIO_UnregisterCallback (VFIO_CALLBACK_TYPE_e eCBtype)
{
	PRINT_FUNC(("[++VFIO_UnregisterCallback] callback type=%d\n", eCBtype));

	(void)memset(&s_stCallbackFn[eCBtype], 0x0, sizeof(VFIO_Callback_t));
	
	PRINT_FUNC(("[--VFIO_UnregisterCallback]\n"));
}

/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
/* Function Name:		VFIO_Open	
 * Function Description:
 * Parameter:				
 * Return Value:			
 */
int VFIO_Open (const char *path, const char *mode, VFIO_Handle_t *ptHandle)
{
	int			ret=VFIO_ERROR_SUCCESS;
	char		*pucIsLocalFile=NULL;
	char		*pucIsDlnaCallback=NULL;
	char		*pucIsDnPCallback=NULL;
	char		*pucIsRtspCallback=NULL;
	char		*pucIsMmsCallback=NULL;
	char		*pucIsHttpCallback=NULL;
	char		*pucISFilePath=NULL;
	FILE		*pFileHandle=NULL;

	if ( NULL == path )
	{
		PRINT_ERROR(("[+-VFIO_Open] Error - path is NULL\n"));
		return VFIO_ERROR_BAD_PARAMETER;
	}
	else if ( NULL == ptHandle)
	{
		PRINT_ERROR(("[+-VFIO_Open] Error - ptHandle is NULL\n"));
		return VFIO_ERROR_BAD_PARAMETER;
	}
	/* else {continue;} */

	PRINT_FUNC(("[++VFIO_Open] path=%s, mode=%s\n", path, mode));

	/*
	 * Callback 등록 여부에 따라 callback이 등록되어 있으면 callback 함수를 호출하고,
	 * Callback이 등록되어 있지 않으면 linux basic file IO 함수를 사용하는 구조로 변경 필요
	 */
	pucIsLocalFile = strstr(path, "file://");
	pucISFilePath	= strstr(path,"://");
	/* TODO : not supported yet.
	pucIsLocalFile = strstr(path, "p2p://");
	pucIsLocalFile = strstr(path, "smb://");
	*/
	if ( (NULL != pucIsLocalFile) || (NULL == pucISFilePath) )
	{
		/* local file */
		ptHandle->eType = VFIO_TYPE_LOCAL;

		if ( NULL == mode )
		{
			PRINT_ERROR(("[--VFIO_Open] Error - mode is NULL\n"));
			return VFIO_ERROR_BAD_PARAMETER;
		}
		else
		{
			if (NULL==pucISFilePath)
			{
				pFileHandle = fopen(path, mode);	/* file path 이므로 그대로 사용 */
			}
			else
			{
				pFileHandle = fopen(&path[6], mode);	/* path에서 "file:/"를 제거하기 위해 &path[5]를 사용함 */
			}
			
			if ( NULL == pFileHandle)
			{
				ptHandle->ucAvailable = 0;
				ret = VFIO_ERROR_IN_DEVICE;
			}
			else
			{
				ptHandle->ucAvailable = 1;
				ptHandle->tFile.local.fin = pFileHandle;
				
				PRINT_DATA(("[VFIO_Open] Local handle=0x%x\n", ptHandle->tFile.local.fin));
			}
		}
	}
	else
	{
		ptHandle->eType = VFIO_TYPE_CALLBACK;
		
		pucIsDlnaCallback = strstr(path, "[DLNA]");	/* Note :: "[DLNA]"는 URL을 전달하는 초기 함수에서 반드시 추가해 주어야 함 */
		if ( NULL != pucIsDlnaCallback )
		{
			/* donwload and play */
			ptHandle->eCBtype = VFIO_CALLBACK_TYPE_DLNA;
		}
		else
		{
			pucIsDnPCallback = strstr(path, "[dnp]");	/* Note :: "[dnp]"는 URL을 전달하는 초기 함수에서 반드시 추가해 주어야 함 */
			if ( NULL != pucIsDnPCallback )
			{
				/* donwload and play */
				ptHandle->eCBtype = VFIO_CALLBACK_TYPE_DNP;
			}
			else
			{
				pucIsRtspCallback = strstr(path,"rtsp://");
				if ( NULL != pucIsRtspCallback )
				{
					/* RTSP Streaming */
					ptHandle->eCBtype = VFIO_CALLBACK_TYPE_STREAMING_RTSP;
				}
				else 
				{
					pucIsMmsCallback = strstr(path,"mms://");
					if ( NULL != pucIsMmsCallback )
					{
						/* MMS Streaming */
						ptHandle->eCBtype = VFIO_CALLBACK_TYPE_STREAMING_MMS;
					}
					else
					{
						pucIsHttpCallback = strstr(path,"http://");
						if ( NULL != pucIsHttpCallback )
						{
							/* HTTP Streaming */
							ptHandle->eCBtype = VFIO_CALLBACK_TYPE_STREAMING_HTTP;
							if(NULL != strstr(path,".ism/Manifest") || NULL != strstr(path,".ism/manifest")
							|| NULL != strstr(path,".isml/Manifest") || NULL != strstr(path,".isml/manifest"))
							
							{
								ptHandle->eCBtype = VFIO_CALLBACK_TYPE_STREAMING_SSTR;
							}
							else if(NULL != strstr(path,".m3u8"))
							{
								ptHandle->eCBtype = VFIO_CALLBACK_TYPE_STREAMING_HLS;
							}
							else
							{
								ptHandle->eCBtype = VFIO_CALLBACK_TYPE_STREAMING_HTTP;
							}
						}
						else
						{
							pucIsHttpCallback = strstr(path,"https://");
							if ( NULL != pucIsHttpCallback )
							{
								/* HTTPS Streaming */
								ptHandle->eCBtype = VFIO_CALLBACK_TYPE_STREAMING_HTTP;
								if(NULL != strstr(path,".ism/Manifest") || NULL != strstr(path,".ism/manifest")
								|| NULL != strstr(path,".isml/Manifest") || NULL != strstr(path,".isml/manifest"))
								
								{
									ptHandle->eCBtype = VFIO_CALLBACK_TYPE_STREAMING_SSTR;
								}
								else if(NULL != strstr(path,".m3u8"))
								{
									ptHandle->eCBtype = VFIO_CALLBACK_TYPE_STREAMING_HLS;
								}
								else
								{
									ptHandle->eCBtype = VFIO_CALLBACK_TYPE_STREAMING_HTTP;
								}
							}
							else
							{
							PRINT_ERROR(("[VFIO_Open] Error - Unsupported URI [%s] !\n", path));
							ptHandle->ucAvailable = 0;
							ret = VFIO_ERROR_IN_DEVICE;
							return ret;
						}
					}
				}
			}
		}
		}

		if ( NULL != s_stCallbackFn[ptHandle->eCBtype].Open )
		{
			if ( VFIO_CALLBACK_TYPE_DNP == ptHandle->eCBtype )
			{
				/* Remove "[dnp]" in URL String */
				ret = s_stCallbackFn[ptHandle->eCBtype].Open(&path[5], mode, ptHandle);
			}
			else if ( VFIO_CALLBACK_TYPE_DLNA == ptHandle->eCBtype )
			{
				/* Remove "[DLNA]" in URL String */
				ret = s_stCallbackFn[ptHandle->eCBtype].Open(&path[6], mode, ptHandle);
			}
			/* Other Streaming Protocol Callback */
			else
			{
				ret = s_stCallbackFn[ptHandle->eCBtype].Open(path, mode, ptHandle);
			}

			if ( VFIO_ERROR_SUCCESS == ret )
			{
				ptHandle->ucAvailable = 1;
			}
			else
			{
				PRINT_ERROR(("-VFIO_Open] Error - Open callback !\n"));
				ptHandle->ucAvailable = 0;
				ret = VFIO_ERROR_IN_DEVICE;
			}
		}
		else
		{
			PRINT_ERROR(("[VFIO_Open] Error - Open callback is NULL !\n"));
			ptHandle->ucAvailable = 0;
			ret = VFIO_ERROR_IN_DEVICE;
		}
	}

	PRINT_FUNC(("[--VFIO_Open] ret(%d)\n", ret));

	return ret;
}

/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
/* Function Name:		VFIO_Close	
 * Function Description:
 * Parameter:				
 * Return Value:			
 */
int VFIO_Close (VFIO_Handle_t tHandle)
{
	int		ret=VFIO_ERROR_SUCCESS;

	if ( 0 == tHandle.ucAvailable )
	{
		PRINT_ERROR(("[+-VFIO_Close] Error - tHandle is not available \n"));
		return VFIO_ERROR_INVALID_HANDLE;
	}
	/* else {continue;} */

	PRINT_FUNC(("[++VFIO_Close]\n"));

	tHandle.ucAvailable = 0;
	if ( VFIO_TYPE_LOCAL == tHandle.eType )
	{
		PRINT_DATA(("[VFIO_Close] Local handle=0x%x\n", tHandle.tFile.local.fin));
		(void)fclose (tHandle.tFile.local.fin);
		tHandle.tFile.local.fin = NULL;
	}
	else if ( VFIO_TYPE_LOCAL_NEXUS == tHandle.eType )
	{
		(void)close(tHandle.tFile.local_nexus.fd);
		tHandle.tFile.local_nexus.fd = -1;
	}
	else if ( VFIO_TYPE_CALLBACK == tHandle.eType )
	{
		if ( NULL !=  s_stCallbackFn[tHandle.eCBtype].Close )
		{
			ret = s_stCallbackFn[tHandle.eCBtype].Close (tHandle);
		}
		else
		{
			PRINT_ERROR(("[VFIO_Close] Error - Close callback is NULL !\n"));
			ret = VFIO_ERROR_FAIL;
		}
	}
	else
	{
		PRINT_ERROR(("[VFIO_Close] Error - invalid tHandle.eType(%d) !\n", tHandle.eType));
		ret = VFIO_ERROR_FAIL;
	}

	PRINT_FUNC(("[--VFIO_Close] ret(%d)\n", ret));

	return ret;
}

/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
/* Function Name:		VFIO_Read	
 * Function Description:
 * Parameter:				
 * Return Value:			
 */
/* TODO : comment by junsy ( param 5 => size_t --> ssize_t ??) 
 *        size_t : unsigned  vs ssize_t : signed
 *        ( fread vs read )
 */
int VFIO_Read (VFIO_Handle_t tHandle, void *ptr, size_t size, size_t nmemb, size_t *ptReadSize)
{
	int		ret=VFIO_ERROR_SUCCESS;
	size_t	readCnt=0;
	/* for samba
	int		retSamba=0;
	*/
	unsigned int	unRequestSize=(unsigned int)(size*nmemb);	/* TODO :: need to check overflow, (2G 초과 file) */
	/*
	unsigned int	unReadSize=0;
	*/

	if ( 0 == tHandle.ucAvailable )
	{
		PRINT_ERROR(("[+-VFIO_Read] Error - tHandle is not available \n"));
		return VFIO_ERROR_INVALID_HANDLE;
	}
	else if ( NULL == ptr )
	{
		PRINT_ERROR(("[+-VFIO_Read] Error - ptr is NULL\n"));
		return VFIO_ERROR_BAD_PARAMETER;
	}
	else if ( NULL == ptReadSize )
	{
		PRINT_ERROR(("[+-VFIO_Read] Error - ptReadSize is NULL\n"));
		return VFIO_ERROR_BAD_PARAMETER;
	}
	/* else {continue;} */

	PRINT_FUNC(("[++VFIO_Read] size=%d, nmemb=%d\n", size, nmemb));

	if ( VFIO_TYPE_LOCAL == tHandle.eType )
	{
		PRINT_DATA(("[VFIO_Read] VFIO_TYPE_LOCAL Local handle=0x%x\n", tHandle.tFile.local.fin));
		readCnt = fread(ptr, 1, unRequestSize, tHandle.tFile.local.fin);
		if( readCnt < 0 )
		{
			PRINT_ERROR(("[VFIO_Read] Error - read() at NEXUS Local layer\n"));
			ret = VFIO_ERROR_IN_DEVICE;
			*ptReadSize = 0;
		}
		else
		{
			*ptReadSize = readCnt;
		}
	}
	else if ( VFIO_TYPE_LOCAL_NEXUS == tHandle.eType )
	{
		PRINT_DATA(("[VFIO_Read] VFIO_TYPE_LOCAL_NEXUS NEXUS Local handle=0x%x\n", tHandle.tFile.local_nexus.fd));
		/* VFIO_TYPE_LOCAL_NEXUS인경우 O_DIRECT 로 open 된 descriptor 인경우
		 * tHandle.tFile.local_nexus.fd 에 대한 fcntl 을 상위에서 사용한다.
		 */
		readCnt = read(tHandle.tFile.local_nexus.fd, ptr, unRequestSize);
		if ( readCnt < 0 )
		{
			PRINT_ERROR(("[VFIO_Read] Error - read() at NEXUS layer\n"));
			ret = VFIO_ERROR_IN_DEVICE;
			*ptReadSize = 0;
		}
		else
		{
			*ptReadSize = readCnt;
		}
	}
	else if ( VFIO_TYPE_CALLBACK == tHandle.eType )
	{
		if ( NULL !=  s_stCallbackFn[tHandle.eCBtype].Read )
		{
			ret = s_stCallbackFn[tHandle.eCBtype].Read (tHandle, ptr, size, nmemb, ptReadSize);
		}
		else
		{
			PRINT_ERROR(("[VFIO_Read] Error - Read callback is NULL !\n"));
			ret = VFIO_ERROR_FAIL;
		}
	}
	else
	{
		PRINT_ERROR(("[VFIO_Read] Error - invalid tHandle.eType(%d) !\n", tHandle.eType));
		ret = VFIO_ERROR_FAIL;
	}

	PRINT_FUNC(("[--VFIO_Read] ret(%d), readCnt(%d)\n", ret, *ptReadSize));

	return ret;
}

/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
/* Function Name:		VFIO_Write	
 * Function Description:
 * Parameter:				
 * Return Value:			
 */
int VFIO_Write (VFIO_Handle_t tHandle, const void *ptr, size_t size, size_t nmemb, size_t *ptWrittenSize)
{
	int		ret=VFIO_ERROR_SUCCESS;

	if ( 0 == tHandle.ucAvailable )
	{
		PRINT_ERROR(("[+-VFIO_Write] Error - tHandle is not available \n"));
		return VFIO_ERROR_INVALID_HANDLE;
	}
	else if ( NULL == ptr )
	{
		PRINT_ERROR(("[+-VFIO_Write] Error - ptr is NULL\n"));
		return VFIO_ERROR_BAD_PARAMETER;
	}
	else if ( NULL == ptWrittenSize )
	{
		PRINT_ERROR(("[+-VFIO_Write] Error - ptWrittenSize is NULL\n"));
		return VFIO_ERROR_BAD_PARAMETER;
	}	/* else {continue;} */

	PRINT_FUNC(("[++VFIO_Write] size=%d, nmemb=%d\n", size, nmemb));

	PRINT_ERROR(("[VFIO_Write] Error - NOT Support !!!\n"));

	PRINT_FUNC(("[--VFIO_Write] ret(%d)\n", ret));

	return ret;
}

/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
/* Function Name:		VFIO_Seek	
 * Function Description:
 * Parameter:				
 * Return Value:			
 */
int VFIO_Seek (VFIO_Handle_t tHandle, off_t offset, int whence)
{
	int		ret=VFIO_ERROR_SUCCESS;

	if ( 0 == tHandle.ucAvailable )
	{
		PRINT_ERROR(("[+-VFIO_Seek] Error - tHandle is not available \n"));
		return VFIO_ERROR_INVALID_HANDLE;
	}
	/* else {continue;} */

	PRINT_FUNC(("[++VFIO_Seek] offset="OFF_T_FORMAT", whence=%d\n", offset, whence));

	if ( VFIO_TYPE_LOCAL == tHandle.eType )
	{
		PRINT_DATA(("[VFIO_Seek] Local handle=0x%x\n", tHandle.tFile.local.fin));
		ret = fseeko(tHandle.tFile.local.fin, offset, whence);
	}
	else if ( VFIO_TYPE_CALLBACK == tHandle.eType )
	{
		if ( NULL !=  s_stCallbackFn[tHandle.eCBtype].Seek )
		{
			ret = s_stCallbackFn[tHandle.eCBtype].Seek (tHandle, offset, whence);
		}
		else
		{
			PRINT_ERROR(("[VFIO_Seek] Error - Seek callback is NULL !\n"));
			ret = VFIO_ERROR_FAIL;
		}
	}
	else
	{
		PRINT_ERROR(("[VFIO_Seek] Error - invalid tHandle.eType(%d) !\n", tHandle.eType));
		ret = VFIO_ERROR_FAIL;
	}
	/* else {continue;} */

	PRINT_FUNC(("[--VFIO_Seek] ret(%d)\n", ret));

	return ret;
}

/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
/* Function Name:		VFIO_Tell	
 * Function Description:
 * Parameter:				
 * Return Value:			
 */
int VFIO_Tell (VFIO_Handle_t tHandle, off_t *plCurOffset)
{
	int		ret=VFIO_ERROR_SUCCESS;

	if ( 0 == tHandle.ucAvailable )
	{
		PRINT_ERROR(("[+-VFIO_Tell] Error - tHandle is not available \n"));
		return VFIO_ERROR_INVALID_HANDLE;
	}
	else if ( NULL == plCurOffset )
	{
		PRINT_ERROR(("[+-VFIO_Tell] Error - plCurOffset is NULL\n"));
		return VFIO_ERROR_BAD_PARAMETER;
	}
	/* else {continue;} */

	PRINT_FUNC(("[++VFIO_Tell]\n"));

	if ( VFIO_TYPE_LOCAL == tHandle.eType )
	{
		PRINT_DATA(("[VFIO_Tell] Local handle=0x%x\n", tHandle.tFile.local.fin));
		*plCurOffset = ftello(tHandle.tFile.local.fin);
	}
	else if ( VFIO_TYPE_LOCAL_NEXUS == tHandle.eType )
	{
		PRINT_DATA(("[VFIO_Tell] NEXUS Local handle=0x%x\n", tHandle.tFile.local_nexus.fd));
	}
	else if ( VFIO_TYPE_CALLBACK == tHandle.eType )
	{
		if ( NULL !=  s_stCallbackFn[tHandle.eCBtype].Tell )
		{
			ret = s_stCallbackFn[tHandle.eCBtype].Tell (tHandle, plCurOffset);
		}
		else
		{
			PRINT_ERROR(("[VFIO_Tell] Error - Tell callback is NULL !\n"));
			ret = VFIO_ERROR_FAIL;
		}
	}
	else
	{
		PRINT_ERROR(("[VFIO_Tell] Error - invalid tHandle.eType(%d) !\n", tHandle.eType));
		ret = VFIO_ERROR_FAIL;
	}

	PRINT_FUNC(("[--VFIO_Tell] ret(%d), curOffset="OFF_T_FORMAT"\n", ret, *plCurOffset));

	return ret;
}

/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
/* Function Name:		VFIO_IsEOF	
 * Function Description:
 * Parameter:				
 * Return Value:			
 */
int VFIO_IsEOF (VFIO_Handle_t tHandle)
{
	int	ret=VFIO_ERROR_SUCCESS;

	if ( 0 == tHandle.ucAvailable )
	{
		PRINT_ERROR(("[+-VFIO_IsEOF] Error - tHandle is not available \n"));

		/* TODO :: feof()의 경우 error return을 '-1'로 해야 함 */
		return -1;
	}
	/* else {continue;} */

	PRINT_FUNC(("[++VFIO_IsEOF]\n"));

	if ( VFIO_TYPE_LOCAL == tHandle.eType )
	{
		PRINT_DATA(("[VFIO_IsEOF] Local handle=0x%x\n", tHandle.tFile.local.fin));
		ret = feof(tHandle.tFile.local.fin);
	}
	else if ( VFIO_TYPE_LOCAL_NEXUS == tHandle.eType )
	{
		PRINT_DATA(("[VFIO_IsEOF] NEXUS Local handle=0x%x\n", tHandle.tFile.local_nexus.fd));
	}
	else if ( VFIO_TYPE_CALLBACK == tHandle.eType )
	{
		if ( NULL !=  s_stCallbackFn[tHandle.eCBtype].IsEOF )
		{
			ret = s_stCallbackFn[tHandle.eCBtype].IsEOF(tHandle);
		}
		else
		{
			PRINT_ERROR(("[VFIO_IsEOF] Error - IsEOF callback is NULL !\n"));
			ret = VFIO_ERROR_FAIL;
		}
	}
	else
	{
		PRINT_ERROR(("[VFIO_IsEOF] Error - invalid tHandle.eType(%d) !\n", tHandle.eType));
		ret = VFIO_ERROR_FAIL;
	}

	PRINT_FUNC(("[--VFIO_IsEOF]\n"));

	return ret;
}


/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
/* Function Name:		VFIO_OpenFromNexus	
 * Function Description:
 * Parameter:				
 * Return Value:			
 */
int VFIO_OpenFromNexus (const char *path, const unsigned int flags, VFIO_Handle_t *ptHandle)
{
	int			ret=VFIO_ERROR_SUCCESS;
	char		*pucIsLocalFile=NULL;
	char		*pucIsDlnaCallback=NULL;
	char		*pucIsDnPCallback=NULL;
	char		*pucIsRtspCallback=NULL;
	char		*pucIsMmsCallback=NULL;
	char		*pucIsHttpCallback=NULL;
	char		*pucISFilePath=NULL;
	int			fd;
	int 		oflags = 0;

	if ( NULL == path )
	{
		PRINT_ERROR(("[+-VFIO_OpenFromNexus] Error - path is NULL\n"));
		return VFIO_ERROR_BAD_PARAMETER;
	}
	else if ( NULL == ptHandle)
	{
		PRINT_ERROR(("[+-VFIO_OpenFromNexus] Error - ptHandle is NULL\n"));
		return VFIO_ERROR_BAD_PARAMETER;
	}
	/* else {continue;} */

	PRINT_FUNC(("[++VFIO_OpenFromNexus] path=%s, flags=0x%x, direct=%s\n", path, flags, flags&O_DIRECT?"true":"false"));

	/*
	 * Callback 등록 여부에 따라 callback이 등록되어 있으면 callback 함수를 호출하고,
	 * Callback이 등록되어 있지 않으면 linux basic file IO 함수를 사용하는 구조로 변경 필요
	 */
	pucIsLocalFile = strstr(path, "file://");
	pucISFilePath	= strstr(path,"://");
	/* TODO : not supported yet.
	pucIsLocalFile = strstr(path, "p2p://");
	pucIsLocalFile = strstr(path, "smb://");
	*/
	if ( (NULL != pucIsLocalFile) || (NULL == pucISFilePath) )
	{
		/* local file */
		ptHandle->eType = VFIO_TYPE_LOCAL_NEXUS;

		if (NULL==pucISFilePath)
		{
			/* file path 이므로 path는 수정없이 그대로 사용하며
			 * VFIO_TYPE_LOCAL_NEXUS 인경우는 flags를 수정없이 전달한다.
			 */
#if 1 /* Support DIRECT_IO Flags in OpenFrom Nexus */
			fd = open(path, flags, 0666);	
#else
			fd = open(path, O_RDONLY, 0666);	

#endif
		}
		else
		{
			fd = open(&path[6], O_RDONLY, 0666);
		}
		if ( fd < 0 )
		{
			ptHandle->ucAvailable = 0;
			ret = VFIO_ERROR_IN_DEVICE;
		}
		else
		{
			/* set close-on-exec flag on */
			oflags = fcntl(fd, F_GETFD, 0);
			if(!(oflags & FD_CLOEXEC))
			{
				oflags |= FD_CLOEXEC;
				fcntl(fd, F_SETFD, oflags);
			}

			ptHandle->ucAvailable = 1;
			ptHandle->tFile.local_nexus.fd = fd;
			
			PRINT_DATA(("[VFIO_OpenFromNexus] Nexus Local handle=0x%x\n", ptHandle->tFile.local_nexus.fd));
		}
	}
	else
	{
		ptHandle->eType = VFIO_TYPE_CALLBACK;
		
		pucIsDlnaCallback = strstr(path, "[DLNA]");	/* Note :: "[DLNA]"는 URL을 전달하는 초기 함수에서 반드시 추가해 주어야 함 */
		if ( NULL != pucIsDlnaCallback )
		{
			/* donwload and play */
			ptHandle->eCBtype = VFIO_CALLBACK_TYPE_DLNA;
		}
		else
		{
			pucIsDnPCallback = strstr(path, "[dnp]");	/* Note :: "[dnp]"는 HMedia_Open/OpenEx() 함수에서 추가해주어야 함. */
			if ( NULL != pucIsDnPCallback )
			{
				/* donwload and play */
				ptHandle->eCBtype = VFIO_CALLBACK_TYPE_DNP;
			}
			else
			{
				pucIsRtspCallback = strstr(path,"rtsp://");
				if ( NULL != pucIsRtspCallback )
				{
					/* RTSP Streaming */
					ptHandle->eCBtype = VFIO_CALLBACK_TYPE_STREAMING_RTSP;
				}
				else 
				{
					pucIsMmsCallback = strstr(path,"mms://");
					if ( NULL != pucIsMmsCallback )
					{
						/* MMS Streaming */
						ptHandle->eCBtype = VFIO_CALLBACK_TYPE_STREAMING_MMS;
					}
					else
					{
						pucIsHttpCallback = strstr(path,"http://");
						if ( NULL != pucIsHttpCallback )
						{
							/* HTTP Streaming */
							if(NULL != strstr(path,".ism/Manifest") || NULL != strstr(path,".ism/manifest")
							|| NULL != strstr(path,".isml/Manifest") || NULL != strstr(path,".isml/manifest"))
							{
								ptHandle->eCBtype = VFIO_CALLBACK_TYPE_STREAMING_SSTR; /* for SSTR */
							}
							else if(NULL != strstr(path,".m3u8"))
							{
								ptHandle->eCBtype = VFIO_CALLBACK_TYPE_STREAMING_HLS;
							}
							else
							{
								ptHandle->eCBtype = VFIO_CALLBACK_TYPE_STREAMING_HTTP;
							}
						}
						else
						{
							pucIsHttpCallback = strstr(path,"https://");
							if ( NULL != pucIsHttpCallback )
							{
								/* HTTPS Streaming */
								ptHandle->eCBtype = VFIO_CALLBACK_TYPE_STREAMING_HTTP;
								if(NULL != strstr(path,".ism/Manifest") || NULL != strstr(path,".ism/manifest")
								|| NULL != strstr(path,".isml/Manifest") || NULL != strstr(path,".isml/manifest"))
								
								{
									ptHandle->eCBtype = VFIO_CALLBACK_TYPE_STREAMING_SSTR;
								}
								else if(NULL != strstr(path,".m3u8"))
								{
									ptHandle->eCBtype = VFIO_CALLBACK_TYPE_STREAMING_HLS;
								}
								else
								{
									ptHandle->eCBtype = VFIO_CALLBACK_TYPE_STREAMING_HTTP;
								}
							}
							else
							{
							PRINT_ERROR(("[VFIO_Open] Error - Unsupported URI [%s] !\n", path));
							ptHandle->ucAvailable = 0;
							ret = VFIO_ERROR_IN_DEVICE;
							return ret;
						}
					}
				}
			}
		}
		}

		if ( NULL != s_stCallbackFn[ptHandle->eCBtype].OpenNexus )
		{
			if ( VFIO_CALLBACK_TYPE_DNP == ptHandle->eCBtype )
			{
				/* Remove "[dnp]" in URL String */
				ret = s_stCallbackFn[ptHandle->eCBtype].OpenNexus(&path[5], flags, ptHandle);
			}
			else if ( VFIO_CALLBACK_TYPE_DLNA == ptHandle->eCBtype )
			{
				/* Remove "[DLNA]" in URL String */
				ret = s_stCallbackFn[ptHandle->eCBtype].OpenNexus(&path[6], flags, ptHandle);
			}
			/* Other Streaming Protocol Callback */
			else
			{
				ret = s_stCallbackFn[ptHandle->eCBtype].OpenNexus(path, flags, ptHandle);
			}
			if ( VFIO_ERROR_SUCCESS == ret )
			{
				ptHandle->ucAvailable = 1;
			}
			else
			{
				PRINT_ERROR(("-VFIO_OpenFromNexus] Error - OpenNexus callback !\n"));
				ptHandle->ucAvailable = 0;
				ret = VFIO_ERROR_IN_DEVICE;
			}
		}
		else
		{
			PRINT_ERROR(("[VFIO_OpenFromNexus] Error - OpenNexus callback is NULL !\n"));
			ptHandle->ucAvailable = 0;
			ret = VFIO_ERROR_IN_DEVICE;
		}
	}
	
	PRINT_FUNC(("[--VFIO_OpenFromNexus] ret(%d)\n", ret));

	return ret;
}

/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
/* Function Name:		VFIO_SeekForNexus	
 * Function Description:
 * Parameter:				
 * Return Value:			
 */
off_t VFIO_SeekForNexus (VFIO_Handle_t tHandle, off_t offset, int whence)
{
	off_t		curOffset=-1;

	if ( 0 == tHandle.ucAvailable )
	{
		PRINT_ERROR(("[+-VFIO_SeekForNexus] Error - tHandle is not available \n"));
		return -1;
	}
	/* else {continue;} */

	PRINT_FUNC(("[++VFIO_SeekForNexus] offset="OFF_T_FORMAT", whence=%d\n", (OFF_T_FORMAT_CAST)offset, whence));

	if ( VFIO_TYPE_LOCAL_NEXUS == tHandle.eType )
	{
		PRINT_DATA(("[VFIO_SeekForNexus] NEXUS Local handle=0x%x\n", tHandle.tFile.local_nexus.fd));
		curOffset = lseek(tHandle.tFile.local_nexus.fd, offset, whence);
	}
	else if ( VFIO_TYPE_CALLBACK == tHandle.eType )
	{
		if ( NULL !=  s_stCallbackFn[tHandle.eCBtype].SeekNexus )
		{
			curOffset = s_stCallbackFn[tHandle.eCBtype].SeekNexus(tHandle, offset, whence);
		}
		else
		{
			PRINT_ERROR(("[VFIO_SeekForNexus] Error - SeekNexus callback is NULL !\n"));
		}
	}
	else
	{
		PRINT_ERROR(("[VFIO_SeekForNexus] Error - invalid tHandle.eType(%d) !\n", tHandle.eType));
	}

	PRINT_FUNC(("[--VFIO_SeekForNexus] curOffset="OFF_T_FORMAT"\n", (OFF_T_FORMAT_CAST)curOffset));

	return curOffset;
}

/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
/* Function Name:		VFIO_BoundsForNexus	
 * Function Description:
 * Parameter:				
 * Return Value:			
 */
int  VFIO_BoundsForNexus(VFIO_Handle_t tHandle, off_t *file_size)
{
	int		ret=VFIO_ERROR_SUCCESS;
	struct stat	sb;
	off_t cur=0, last=0;
	int rc;

	if ( 0 == tHandle.ucAvailable )
	{
		PRINT_ERROR(("[+-VFIO_BoundsForNexus] Error - tHandle is not available \n"));
		return VFIO_ERROR_INVALID_HANDLE;
	}
	/* else {continue;} */

	PRINT_FUNC(("[++VFIO_BoundsForNexus]\n"));

	if ( VFIO_TYPE_LOCAL_NEXUS == tHandle.eType )
	{
		PRINT_DATA(("[VFIO_BoundsForNexus] NEXUS Local handle=0x%x\n", tHandle.tFile.local_nexus.fd));
		
		if (fstat(tHandle.tFile.local_nexus.fd, &sb)<0)
		{
			PRINT_ERROR(("[VFIO_BoundsForNexus] Error - fstat()\n"));
			ret = VFIO_ERROR_IN_DEVICE;

			*file_size = 0;
		}
		else
		{
			*file_size = sb.st_size;
		}
	}
	else if ( VFIO_TYPE_CALLBACK == tHandle.eType )
	{
		if ( NULL !=  s_stCallbackFn[tHandle.eCBtype].BoundsNexus )
		{
			ret = s_stCallbackFn[tHandle.eCBtype].BoundsNexus(tHandle, file_size);
		}
		else
		{
			PRINT_ERROR(("[VFIO_BoundsForNexus] Error - BoundsNexus callback is NULL !\n"));
		}
	}
	else
	{
		/* TODO :: */
		/*
		unsigned long long	unFileSize=0;
		*/
			
		PRINT_DATA(("[VFIO_BoundsForNexus] handle type(%d), callback type(%d)\n", tHandle.eType, tHandle.eCBtype));

		/* TODO :: */
		*file_size = 0;
		rc = VFIO_Tell(tHandle, &cur);
		if (rc<0)
		{
			PRINT_ERROR(("[VFIO_BoundsForNexus] Error -  VFIO_Tell()=%d - pos1\n", rc));
			return -1;
		}

		rc = VFIO_Seek (tHandle, 0, SEEK_END);
		if (rc<0)
		{
			PRINT_ERROR(("[VFIO_BoundsForNexus] Error -  VFIO_Seek()=%d - pos1\n", rc));
			return rc;
		}

		rc = VFIO_Tell(tHandle, &last);
		if (rc<0)
		{
			PRINT_ERROR(("[VFIO_BoundsForNexus] Error -  VFIO_Tell()=%d - pos2\n", rc));
			return -1;
		}

		*file_size = (off_t)last;

		rc = VFIO_Seek (tHandle, cur, SEEK_SET);
		if (rc<0)
		{
			PRINT_ERROR(("[VFIO_BoundsForNexus] Error -  VFIO_Seek()=%d - pos2\n", rc));
			return rc;
		}
	}

	PRINT_FUNC(("[--VFIO_BoundsForNexus] ret(%d)\n", ret));

	return ret;
}

#endif

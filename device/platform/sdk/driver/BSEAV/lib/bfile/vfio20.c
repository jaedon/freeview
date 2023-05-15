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
#include <inttypes.h> /* for PRId64 */

#include "vfio20.h"

/* End Including Headers */


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
#ifndef VFIO_DEBUG
#define VFIO_DEBUG 	0
#define DEBUG_IO 	0
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

#define VFIO_POOL_MAX		(100)
#define TRUE	1
#define FALSE	0

typedef unsigned char VBOOL;

static VFIO_POOL_t			stVfioItem[VFIO_POOL_MAX];

/* End static variable */

VFIO_POOL_t *INT_SearchItemByUrl (const char *it)
{
	int i=0;

	if(it==NULL)
	{
		return NULL;
	}

#if DEBUG_IO
	printf("%s(%d) : Enter \n",__FUNCTION__,__LINE__);
#endif
	for(i=0;i<VFIO_POOL_MAX;i++)
	{
/*DRAGON TEST		if( stVfioItem[i].isAvailable == TRUE) */
		if(stVfioItem[i].pUrl != NULL)
		{
			if(!strcmp(it, stVfioItem[i].pUrl))
			{
				/* searched !!*/
				break;
			}
		}
	}

	if (i != VFIO_POOL_MAX)
	{
#if DEBUG_IO
		if((stVfioItem[i].pUrl!=NULL) && (stVfioItem[i].pHandle!=NULL))
		{
			printf("%s(%d) : Got It !! URL[%s] pTunerHandle(%d)\n"
					,__FUNCTION__
					,__LINE__
					,stVfioItem[i].pUrl
					,(unsigned int)stVfioItem[i].pHandle->pTunerHandle);
		}
#endif
		return &stVfioItem[i];
	}

#if DEBUG_IO
	printf("%s(%d) : Exit \n",__FUNCTION__,__LINE__);
#endif
	return NULL;

}

/* open 은 동시에 세가지 vfio handle 을 통해 이루어 질 수 있다.
 * PROBE/INDEX/DATA 모두 다른 vfio handle 을 가지지만
 * open 시 VFIO CAllback (OPEN or OPEN NExUS) 에서
 * 동일한 tuner handle 을 등록하므로
 * vfio handle 전달을 위한 callback set 을 찾기 위한 용도로만
 * VFIO_POOL item 에 vfio handle 을 전달한다.
 *
 * TODO : Open/OpenNexus 시 Item 으로 TunerHandle 만 등록하고
 * Read/Seek 등에서는 vfioHandle 의 tuner handle 만으로
 * Callback set 을 찾도록 수정할 예정이다.
 */

VFIO_POOL_t *INT_SearchItemByVfioHandle (VFIO_Handle_t it)
{
	int i=0;

#if DEBUG_IO
	printf("%s(%d) : Enter (search it.pTunerHandle(%d))\n",__FUNCTION__,__LINE__,(unsigned int)it.pTunerHandle);
#endif
	for(i=0;i<VFIO_POOL_MAX;i++)
	{
		if(stVfioItem[i].isAvailable == TRUE)
		{
			if( (unsigned int)it.pTunerHandle ==(unsigned int) stVfioItem[i].pHandle->pTunerHandle)
			{
				/* searched !!*/
#if DEBUG_IO
				printf("%s(%d) : Got It !! URL[%s]\n",__FUNCTION__,__LINE__,stVfioItem[i].pUrl);
#endif
				break;
			}
		}
	}

	if (i != VFIO_POOL_MAX)
	{
		return &stVfioItem[i];
	}

#if DEBUG_IO
	printf("%s(%d) : Exit \n",__FUNCTION__,__LINE__);
#endif
	return NULL;
}

int INT_InsertVfioCallback (const char *pUrl, VFIO_Callback_t *it )
{
	int i=0;

	if(pUrl == NULL || it == NULL)
	{
		return -1;
	}

#if DEBUG_IO
	printf("%s(%d) : Enter insert(%s)\n",__FUNCTION__,__LINE__,pUrl);
#endif

	/***********************************
	 * Make & Insertion of VfioPoolItem
	 ***********************************/
	for (i=0;i<VFIO_POOL_MAX;i++)
	{
		if (stVfioItem[i].isAvailable == FALSE)
		{
#if 1
			stVfioItem[i].stCallbackFn.Open         = it->Open;
			stVfioItem[i].stCallbackFn.Close        = it->Close;
			stVfioItem[i].stCallbackFn.Read         = it->Read;
			stVfioItem[i].stCallbackFn.Seek         = it->Seek;
			stVfioItem[i].stCallbackFn.Tell         = it->Tell;
			stVfioItem[i].stCallbackFn.IsEOF        = it->IsEOF;
			stVfioItem[i].stCallbackFn.OpenNexus    = it->OpenNexus;
			stVfioItem[i].stCallbackFn.SeekNexus    = it->SeekNexus;
			stVfioItem[i].stCallbackFn.BoundsNexus  = it->BoundsNexus;
#else
			(void)memcpy(&stVfioItem[i].stCallbackFn, &it, sizeof(VFIO_Callback_t));
#endif
			stVfioItem[i].isAvailable = TRUE;
			stVfioItem[i].pUrl = strdup (pUrl);
#if DEBUG_IO
			/* Just Printing all item */
			for (i=0;i<VFIO_POOL_MAX;i++)
			{
				if((stVfioItem[i].pUrl != NULL)) /* TODO */
				{
					printf("%2d : URL [%s], pVfioHandle[%d], pTunerHandle[%d]\n"
							, i
							, stVfioItem[i].pUrl
							, (stVfioItem[i].pHandle!=NULL)?(unsigned int)stVfioItem[i].pHandle : NULL
							, (stVfioItem[i].pHandle!=NULL)?(unsigned int)stVfioItem[i].pHandle->pTunerHandle: NULL 
							);
				}
			}
			printf("%s(%d) : Exit \n",__FUNCTION__,__LINE__);
#endif
			return 0;
		}
	}
#if DEBUG_IO

	printf("%s(%d) Error> Exit \n",__FUNCTION__,__LINE__);
#endif
	return -1;
}

/*
 *  install vfio callback with URL (This URL is key that can search the unique VFIO Callback Set).
 */
int VFIO_RegisterCallback ( const char *pUrl, VFIO_Callback_t stVfioCallbackFn)
{

	PRINT_FUNC(("%s(%d): Register Callback for [%s]\n",__FUNCTION__,__LINE__,pUrl));

	/* check validation */
	if ( INT_SearchItemByUrl(pUrl) ) 
	{
		PRINT_ERROR(("%s(%d): It is already exist in VFIO_POOL. It couldn't insert in the pool !!\n",__FUNCTION__,__LINE__));
		return -1;
	}

	/* install vfio set */
	if ( INT_InsertVfioCallback (pUrl, &stVfioCallbackFn) ) 
	{
		PRINT_ERROR(("%s(%d): Can't insert Item in VFIO_POOL !!\n",__FUNCTION__,__LINE__));
		return -1;
	}

	return 0;
}

int VFIO_UnregisterCallback ( const char *pUrl )
{
	VFIO_POOL_t *item = NULL;

	PRINT_FUNC(("%s(%d): Unregister Callback for [%s]\n",__FUNCTION__,__LINE__,pUrl));

	item = INT_SearchItemByUrl (pUrl);
	if ( item == NULL ) 
	{
		PRINT_ERROR(("%s(%d): It isn't in the VFIO_POOL !!\n",__FUNCTION__,__LINE__));
		return -1;
	}

	/* delete temp */
	if(item->pUrl)
	{
		free (item->pUrl);
		item->isAvailable = FALSE;
	}
	(void)memset(item, 0x0, sizeof(VFIO_POOL_t));

	return 0;
}

VBOOL P_IsLocalFilePath (const char *pPath)
{
	if (strstr (pPath, "://"))
	{
		/* dvr:/// */
		if(strstr (pPath, "dvr://"))
		{
			/* Downloader with SmartBuffering */
			return FALSE;
		}
		/* file:/// */
		else if (strstr (pPath,"file://"))
		{
			return TRUE;
		}
		/* http://, mms:// */
		else
		{
			return FALSE;
		}
	}

	/* e.g> "/mnt/hd2/download0" */

	return TRUE;
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
	int			ret = VFIO_ERROR_SUCCESS;
	char 		*pUrl = NULL;

	VFIO_POOL_t	*pVfioPoolItem      = NULL;
	VBOOL 		bIsFilePath  	= FALSE;
	FILE		*pFileHandle    = NULL;

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
	bIsFilePath	= P_IsLocalFilePath (path);

	if ( bIsFilePath )
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
			if(NULL == strstr(path,"://"))
			{
				pFileHandle = fopen(path, mode);	/* file path 이므로 그대로 사용 */
			}
#if 0
			else if (strstr(path,"dvr://"))
			{
				pFileHandle = fopen(&path[5], mode);	/* path에서 "dvr:/"를 제거하기 위해 &path[5]를 사용함 */
			}	
#endif
			else if (strstr(path,"file://"))
			{
				pUrl = strstr(path,"file://");
				pFileHandle = fopen(pUrl+6, mode);	/* path에서 "file:/"를 제거하기 위해 &path[5]를 사용함 */
			}
			else
			{
				PRINT_ERROR(("[--VFIO_Open] Error - invalid file prefix.\n"));
				return VFIO_ERROR_BAD_PARAMETER;
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
				
				PRINT_DATA(("[VFIO_Open] Local handle=%p\n", (void*)ptHandle->tFile.local.fin));
			}
		}
	}
	else
	{
		/**********************************************************
		 * Search vfio callback set by path
		 **********************************************************/
		ptHandle->eType = VFIO_TYPE_CALLBACK;

		pVfioPoolItem = INT_SearchItemByUrl (path);
		if(pVfioPoolItem == NULL) 
		{
			ptHandle->ucAvailable = 0;
			return VFIO_ERROR_IN_DEVICE;
		}

		/**********************************************************
		 * make a link between POOL and VFIO_Handle for Closing
		 **********************************************************/
		pVfioPoolItem->pHandle = ptHandle;

		/**********************************************************
		 * Call the Open Callback and return .
		 **********************************************************/
		if ( NULL != pVfioPoolItem->stCallbackFn.Open )
		{
			ret = pVfioPoolItem->stCallbackFn.Open (path, mode, ptHandle);
			if ( VFIO_ERROR_SUCCESS == ret )
			{
				ptHandle->ucAvailable = 1;
			}
			else
			{
				PRINT_ERROR(("[-VFIO_Open] Error - Open callback !\n"));
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
	VFIO_POOL_t	*pVfioPoolItem      = NULL;

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
		PRINT_DATA(("[VFIO_Close] Local handle=%p\n", (void*)tHandle.tFile.local.fin));
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
		/****************************************
		 * Search item by tHandle
		 ****************************************/
		pVfioPoolItem = INT_SearchItemByVfioHandle (tHandle);
		if (NULL == pVfioPoolItem)
		{
			PRINT_ERROR(("[VFIO_Close] Error - can't search VfioPool by Handle !\n"));
#if DEBUG_IO
			printf("tHandle.pDnpUserData %p\n",tHandle.pDnpUserData);
			printf("tHandle.tFile.callback.unHandle %d\n",tHandle.tFile.callback.unHandle);
			printf("tHandle.pTunerHandle %d\n",(unsigned int)tHandle.pTunerHandle);
#endif
			return VFIO_ERROR_FAIL;
		}

		if ( NULL !=  pVfioPoolItem->stCallbackFn.Close )
		{
			ret = pVfioPoolItem->stCallbackFn.Close (tHandle);
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
int VFIO_Read (VFIO_Handle_t tHandle, void *ptr, size_t size, size_t nmemb, ssize_t *ptReadSize)
{
	int		ret=VFIO_ERROR_SUCCESS;
	ssize_t	readCnt=0;
	VFIO_POOL_t	*pVfioPoolItem      = NULL;
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
		PRINT_DATA(("[VFIO_Read] VFIO_TYPE_LOCAL Local handle=%p\n", (void*) tHandle.tFile.local.fin));
		readCnt = fread(ptr, 1, unRequestSize, tHandle.tFile.local.fin);
		if( readCnt < 0 )
		{
			PRINT_ERROR(("[VFIO_Read] Error - read() at NEXUS Local layer\n"));
			ret = VFIO_ERROR_IN_DEVICE;
			*ptReadSize = -1;
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
			*ptReadSize = -1;
		}
		else
		{
			*ptReadSize = (ssize_t)readCnt;
		}
	}
	else if ( VFIO_TYPE_CALLBACK == tHandle.eType )
	{
		/****************************************
		 * Search item by tHandle
		 ****************************************/
		pVfioPoolItem = INT_SearchItemByVfioHandle (tHandle);
		if (NULL == pVfioPoolItem)
		{
			PRINT_ERROR(("[VFIO_Read] Error - can't search VfioPool by Handle !\n"));
			return VFIO_ERROR_FAIL;
		}

		if ( NULL !=  pVfioPoolItem->stCallbackFn.Read )
		{
			ret =  pVfioPoolItem->stCallbackFn.Read (tHandle, ptr, size, nmemb, ptReadSize);
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
int VFIO_Write (VFIO_Handle_t tHandle, const void *ptr, size_t size, size_t nmemb, ssize_t *ptWrittenSize)
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
	VFIO_POOL_t	*pVfioPoolItem      = NULL;

	if ( 0 == tHandle.ucAvailable )
	{
		PRINT_ERROR(("[+-VFIO_Seek] Error - tHandle is not available \n"));
		return VFIO_ERROR_INVALID_HANDLE;
	}
	/* else {continue;} */

	PRINT_FUNC(("[++VFIO_Seek] offset=%"PRId64", whence=%d\n", offset, whence));

	if ( VFIO_TYPE_LOCAL == tHandle.eType )
	{
		PRINT_DATA(("[VFIO_Seek] Local handle=%p\n", (void*)tHandle.tFile.local.fin));
		ret = fseeko(tHandle.tFile.local.fin, offset, whence);
	}
	else if ( VFIO_TYPE_CALLBACK == tHandle.eType )
	{
		/****************************************
		 * Search item by tHandle
		 ****************************************/
		pVfioPoolItem = INT_SearchItemByVfioHandle (tHandle);
		if (NULL == pVfioPoolItem)
		{
			PRINT_ERROR(("[VFIO_Seek] Error - can't search VfioPool by Handle !\n"));
			return VFIO_ERROR_FAIL;
		}

		if ( NULL !=  pVfioPoolItem->stCallbackFn.Seek )
		{
			ret = pVfioPoolItem->stCallbackFn.Seek (tHandle, offset, whence);
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
	VFIO_POOL_t	*pVfioPoolItem      = NULL;

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
		PRINT_DATA(("[VFIO_Tell] Local handle=%p\n", (void*) tHandle.tFile.local.fin));
		*plCurOffset = ftello(tHandle.tFile.local.fin);
	}
	else if ( VFIO_TYPE_LOCAL_NEXUS == tHandle.eType )
	{
		PRINT_DATA(("[VFIO_Tell] NEXUS Local handle=0x%x\n", tHandle.tFile.local_nexus.fd));
	}
	else if ( VFIO_TYPE_CALLBACK == tHandle.eType )
	{
		/****************************************
		 * Search item by tHandle
		 ****************************************/
		pVfioPoolItem = INT_SearchItemByVfioHandle (tHandle);
		if (NULL == pVfioPoolItem)
		{
			PRINT_ERROR(("[VFIO_Tell] Error - can't search VfioPool by Handle !\n"));
			return VFIO_ERROR_FAIL;
		}

		if ( NULL !=  pVfioPoolItem->stCallbackFn.Tell )
		{
			ret = pVfioPoolItem->stCallbackFn.Tell (tHandle, plCurOffset);
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

	PRINT_FUNC(("[--VFIO_Tell] ret(%d), curOffset=%"PRId64"\n", ret, *plCurOffset));

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
	VFIO_POOL_t	*pVfioPoolItem      = NULL;

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
		PRINT_DATA(("[VFIO_IsEOF] Local handle=%p\n", (void*)tHandle.tFile.local.fin));
		ret = feof(tHandle.tFile.local.fin);
	}
	else if ( VFIO_TYPE_LOCAL_NEXUS == tHandle.eType )
	{
		PRINT_DATA(("[VFIO_IsEOF] NEXUS Local handle=0x%x\n", tHandle.tFile.local_nexus.fd));
	}
	else if ( VFIO_TYPE_CALLBACK == tHandle.eType )
	{
		/****************************************
		 * Search item by tHandle
		 ****************************************/
		pVfioPoolItem = INT_SearchItemByVfioHandle (tHandle);
		if (NULL == pVfioPoolItem)
		{
			PRINT_ERROR(("[VFIO_IsEOF] Error - can't search VfioPool by Handle !\n"));
			return VFIO_ERROR_FAIL;
		}

		if ( NULL !=  pVfioPoolItem->stCallbackFn.IsEOF )
		{
			ret = pVfioPoolItem->stCallbackFn.IsEOF(tHandle);
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
	char 		*pUrl = NULL;

	VFIO_POOL_t	*pVfioPoolItem      = NULL;

	VBOOL		bIsFilePath =FALSE;

	int			fd=-1;
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
	bIsFilePath	= P_IsLocalFilePath (path);

	if ( bIsFilePath )
	{
		/* local file */
		ptHandle->eType = VFIO_TYPE_LOCAL_NEXUS;

		if(NULL == strstr(path,"://"))
		{
			fd = open(path, flags, 0666);	
		}
#if 0
		else if (strstr(path,"dvr://"))
		{
			fd = open(&path[5], O_RDONLY, 0666);	
		}	
#endif
		else if (strstr(path,"file://"))
		{
			pUrl = strstr(path,"file://");			
			fd = open(pUrl+6, O_RDONLY, 0666);	
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
		/**********************************************************
		 * Search vfio callback set by path
		 **********************************************************/
		ptHandle->eType = VFIO_TYPE_CALLBACK;

		pVfioPoolItem = INT_SearchItemByUrl (path);
		if(pVfioPoolItem == NULL) 
		{
			ptHandle->ucAvailable = 0;
			return VFIO_ERROR_IN_DEVICE;
		}

		/**********************************************************
		 * make a link between POOL and VFIO_Handle for Closing
		 **********************************************************/
		pVfioPoolItem->pHandle = ptHandle;

		/**********************************************************
		 * Call the Open Callback and return .
		 **********************************************************/
		if ( NULL != pVfioPoolItem->stCallbackFn.OpenNexus )
		{
			ret = pVfioPoolItem->stCallbackFn.OpenNexus (path, flags, ptHandle);
			if ( VFIO_ERROR_SUCCESS == ret )
			{
				ptHandle->ucAvailable = 1;
#if DEBUG_IO
				printf("tHandle.pDnpUserData %p\n",ptHandle->pDnpUserData);
				printf("tHandle.tFile.callback.unHandle %d\n",ptHandle->tFile.callback.unHandle);
				printf("tHandle.pTunerHandle %d\n",(unsigned int)ptHandle->pTunerHandle);
#endif
			}
			else
			{
				PRINT_ERROR(("[-VFIO_OpenFromNexus] Error - OpenNexus callback !\n"));
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
	VFIO_POOL_t	*pVfioPoolItem      = NULL;
	off_t		curOffset=-1;

	if ( 0 == tHandle.ucAvailable )
	{
		PRINT_ERROR(("[+-VFIO_SeekForNexus] Error - tHandle is not available \n"));
		return -1;
	}
	/* else {continue;} */

	PRINT_FUNC(("[++VFIO_SeekForNexus] offset=%"PRId64", whence=%d\n", offset, whence));

	if ( VFIO_TYPE_LOCAL_NEXUS == tHandle.eType )
	{
		PRINT_DATA(("[VFIO_SeekForNexus] NEXUS Local handle=0x%x\n", tHandle.tFile.local_nexus.fd));
		curOffset = lseek(tHandle.tFile.local_nexus.fd, offset, whence);
	}
	else if ( VFIO_TYPE_CALLBACK == tHandle.eType )
	{
		/****************************************
		 * Search item by tHandle
		 ****************************************/
		pVfioPoolItem = INT_SearchItemByVfioHandle (tHandle);
		if (NULL == pVfioPoolItem)
		{
			PRINT_ERROR(("[VFIO_SeekForNexus] Error - can't search VfioPool by Handle !\n"));
#if DEBUG_IO
			printf("tHandle.pDnpUserData %p\n",tHandle.pDnpUserData);
			printf("tHandle.tFile.callback.unHandle %d\n",tHandle.tFile.callback.unHandle);
			printf("tHandle.pTunerHandle %d\n",(unsigned int)tHandle.pTunerHandle);
#endif
			return VFIO_ERROR_FAIL;
		}

		if ( NULL !=  pVfioPoolItem->stCallbackFn.SeekNexus )
		{
			curOffset = pVfioPoolItem->stCallbackFn.SeekNexus (tHandle, offset, whence);
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

	PRINT_FUNC(("[--VFIO_SeekForNexus] curOffset=%"PRId64"\n", curOffset));

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
	VFIO_POOL_t	*pVfioPoolItem      = NULL;

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
		/****************************************
		 * Search item by tHandle
		 ****************************************/
		pVfioPoolItem = INT_SearchItemByVfioHandle (tHandle);
		if (NULL == pVfioPoolItem)
		{
			PRINT_ERROR(("[VFIO_BoundsForNexus] Error - can't search VfioPool by Handle !\n"));
			return VFIO_ERROR_FAIL;
		}

		if ( NULL !=  pVfioPoolItem->stCallbackFn.BoundsNexus )
		{
			ret = pVfioPoolItem->stCallbackFn.BoundsNexus (tHandle, file_size);
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
			
		PRINT_DATA(("[VFIO_BoundsForNexus] handle type(%d)\n", tHandle.eType));

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

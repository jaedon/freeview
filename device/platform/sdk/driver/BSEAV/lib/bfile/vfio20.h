/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   vfio.h  $
 * Version:			$Revision:   1.0  $
 * Original Author:	Kim,Jeongsu
 * Current Author:	$Author:   	Jeon Sung Yong  $
 * Date:			$Date: 		Mon Apr 13 18:32:48 KST 2009 $
 * File Description:	Virtual FILE IO Header
 * Module: 			
 * Remarks:		
 */																										

/*******************************************************************/ 	
/* Copyright (c) 2009 HUMAX Co., Ltd. 								*/
/* All rights reserved.												*/
/*******************************************************************/

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)

#ifndef	__VFIO20_H__
#define	__VFIO20_H__

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
/* End Including Headers */


/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
/* Start Extern variablee */

/* End Extern variable */


/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
/* Start Macro definition */
#define VFIO_ERROR_SUCCESS					0
#define VFIO_ERROR_FAIL						(-1)
#define VFIO_ERROR_BAD_PARAMETER			(-2)	/* Bad parameter passed       */
#define VFIO_ERROR_INVALID_HANDLE			(-3)	/* Handle is not valid        */
#define VFIO_ERROR_IN_DEVICE				(-4)	/* Error occured in device */

/* End Macro definition */


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */

typedef enum {
	VFIO_TYPE_LOCAL = 1,
	VFIO_TYPE_LOCAL_NEXUS,
	VFIO_TYPE_CALLBACK
} VFIO_TYPE_e;

typedef enum {
	VFIO_OPEN_DATA = 1,
	VFIO_OPEN_INDEX,
	VFIO_OPEN_PROBE
} VFIO_OPEN_TYPE_e;

typedef union {
	struct {
		FILE	*fin;
	} local;
	struct {
		int		fd;
	} local_nexus;
	struct {
		unsigned int		unHandle;
	} callback;
} VFIO_File_t;

typedef struct {
	VFIO_TYPE_e				eType;
	unsigned char			ucAvailable;
	VFIO_File_t				tFile;
	void 					*pDnpUserData;
	void					*pTunerHandle;
	VFIO_OPEN_TYPE_e		eOpenType;
} VFIO_Handle_t;

typedef int (*VFIOOpenCB) (const char *path, const char *mode, VFIO_Handle_t *ptHandle);
typedef int (*VFIOCloseCB) (VFIO_Handle_t unFileHandle);

typedef int (*VFIOReadCB) (VFIO_Handle_t tHandle, void *ptr, size_t size, size_t nmemb, ssize_t *ptReadSize);
typedef int (*VFIOSeekCB) (VFIO_Handle_t tHandle, off_t offset, int whence);
typedef int (*VFIOTellCB) (VFIO_Handle_t tHandle, off_t *plCurOffset);
typedef int (*VFIOIsEOFCB) (VFIO_Handle_t tHandle);

typedef int (*VFIOOpenFromNexusCB) (const char *path, const unsigned int flags, VFIO_Handle_t *ptHandle);
typedef off_t (*VFIOSeekForNexusCB) (VFIO_Handle_t tHandle, off_t offset, int whence);
typedef int (*VFIOBoundsForNexusCB)(VFIO_Handle_t tHandle, off_t *file_size);

typedef struct {
	VFIOOpenCB	Open;
	VFIOCloseCB	Close;
	VFIOReadCB	Read;
	VFIOSeekCB	Seek;
	VFIOTellCB	Tell;
	VFIOIsEOFCB	IsEOF;
	VFIOOpenFromNexusCB		OpenNexus;
	VFIOSeekForNexusCB		SeekNexus;
	VFIOBoundsForNexusCB	BoundsNexus;
} VFIO_Callback_t;


typedef struct {
	int isAvailable;
	char *pUrl;
	VFIO_Handle_t		*pHandle;
	VFIO_Callback_t 	stCallbackFn;
} VFIO_POOL_t;

/* End typedef */


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* Start global function prototypes */
int VFIO_RegisterCallback (const char *pUrl, VFIO_Callback_t stVfioCallbackFn);
int VFIO_UnregisterCallback (const char *pUrl);

int VFIO_Open (const char *path, const char *mode, VFIO_Handle_t *ptHandle);
int VFIO_Close (VFIO_Handle_t unFileHandle);

/* TODO :: size_t가 long file(> 2G)을 지원할 경우 UINT64가 되는지 확인 필요 */
int VFIO_Read (VFIO_Handle_t tHandle, void *ptr, size_t size, size_t nmemb, ssize_t *ptReadSize);
int VFIO_Write (VFIO_Handle_t tHandle, const void *ptr, size_t size, size_t nmemb, ssize_t *ptWrittenSize);

int VFIO_Seek (VFIO_Handle_t tHandle, off_t offset, int whence);
int VFIO_Tell (VFIO_Handle_t tHandle, off_t *plCurOffset);

int VFIO_IsEOF (VFIO_Handle_t tHandle);


int VFIO_OpenFromNexus (const char *path, const unsigned int flags, VFIO_Handle_t *ptHandle);
off_t VFIO_SeekForNexus (VFIO_Handle_t tHandle, off_t offset, int whence);
int VFIO_BoundsForNexus(VFIO_Handle_t tHandle, off_t *file_size);

/* End global function prototypes */

#endif /* !__VFIO_H__ */

#endif

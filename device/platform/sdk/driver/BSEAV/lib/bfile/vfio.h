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

#ifndef	__VFIO_H__
#define	__VFIO_H__

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#include <stdio.h>
#include <unistd.h>
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
/* For Multi Download Set */
typedef enum {
	VFIO_CALLBACK_TYPE_DNP,
	VFIO_CALLBACK_TYPE_STREAMING_RTSP,
	VFIO_CALLBACK_TYPE_STREAMING_MMS,
	VFIO_CALLBACK_TYPE_STREAMING_HTTP,
	VFIO_CALLBACK_TYPE_STREAMING_SSTR,
	VFIO_CALLBACK_TYPE_STREAMING_HLS,
	VFIO_CALLBACK_TYPE_DLNA,
	/* TODO : not supported yet. 
		VFIO_CALLBACK_TYPE_P2P,
		VFIO_CALLBACK_TYPE_SAMBA,
   	*/
	VFIO_CALLBACK_TYPE_MAX
} VFIO_CALLBACK_TYPE_e;

typedef enum {
	VFIO_TYPE_LOCAL = 1,
	VFIO_TYPE_LOCAL_NEXUS,
	VFIO_TYPE_CALLBACK
} VFIO_TYPE_e;

typedef union {
	struct {
		FILE		*fin;
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
	VFIO_CALLBACK_TYPE_e	eCBtype;
	unsigned char					ucAvailable;
	VFIO_File_t				tFile;
	void *					pDnpUserData;
} VFIO_Handle_t;

typedef int (*VFIOOpenCB) (const char *path, const char *mode, VFIO_Handle_t *ptHandle);
typedef int (*VFIOCloseCB) (VFIO_Handle_t unFileHandle);

typedef int (*VFIOReadCB) (VFIO_Handle_t tHandle, void *ptr, size_t size, size_t nmemb, size_t *ptReadSize);
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

/* End typedef */


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* Start global function prototypes */
void VFIO_RegisterCallback (VFIO_CALLBACK_TYPE_e eCBtype, VFIO_Callback_t stVfioCallbackFn);
void VFIO_UnregisterCallback (VFIO_CALLBACK_TYPE_e eCBtype);

int VFIO_Open (const char *path, const char *mode, VFIO_Handle_t *ptHandle);
int VFIO_Close (VFIO_Handle_t unFileHandle);

/* TODO :: size_t가 long file(> 2G)을 지원할 경우 UINT64가 되는지 확인 필요 */
int VFIO_Read (VFIO_Handle_t tHandle, void *ptr, size_t size, size_t nmemb, size_t *ptReadSize);
int VFIO_Write (VFIO_Handle_t tHandle, const void *ptr, size_t size, size_t nmemb, size_t *ptWrittenSize);

int VFIO_Seek (VFIO_Handle_t tHandle, off_t offset, int whence);
int VFIO_Tell (VFIO_Handle_t tHandle, off_t *plCurOffset);

int VFIO_IsEOF (VFIO_Handle_t tHandle);


int VFIO_OpenFromNexus (const char *path, const unsigned int flags, VFIO_Handle_t *ptHandle);
off_t VFIO_SeekForNexus (VFIO_Handle_t tHandle, off_t offset, int whence);
int VFIO_BoundsForNexus(VFIO_Handle_t tHandle, off_t *file_size);

/* End global function prototypes */

#endif /* !__VFIO_H__ */

#endif

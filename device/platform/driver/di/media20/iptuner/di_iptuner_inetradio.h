/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/*	File Name:		 $Workfile:   di_iptuner_inetradio.h  $
 * Version:		 $Revision:   $
 * Original Author: $
 * Current Author:  $Author: parkjh4@humaxdigital.com $
 * Date:			 $Date: 
 * File Description:	 Internet Radio
 * Module:
 * Remarks:
*/ 

/**
 * @ingroup INTERNET_RADIO
 */

/**
 * @author 
 * @date 30 Jul 2012
 */

/**
 * @note
 * Copyright (C) 2012 Humax Corporation. All Rights Reserved. <br>
 * This software is the confidential and proprietary information
 * of Humax Corporation. You may not use or distribute this software
 * except in compliance with the terms and conditions of any applicable license
 * agreement in writing between Humax Corporation and you.
 */

/*@{*/

/**
 * @file di_iptuner_inetradio.h
 */

#ifndef	__DI_IPTUNER_INETRADIO_H__
#define	__DI_IPTUNER_INETRADIO_H__

#ifdef	__cplusplus
extern "C" {
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */

#include "di_iptuner_config.h"

#include "htype.h"
#include "di_err.h"
#include "di_media20.h"
#include "vfio20.h"
#include "di_iptuner20.h"
/* End Including Headers*/


/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
/* Start Extern variablee */

/* End Extern variable */


/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
/* Start Macro definition */
#define MAX_READ_BUFFER_SIZE_IN_BUFFER (1000)
#define INETRADIO_GENERIC_READ_SIZE (16*1024)
#define MAX_TITLE_LENGTH (256)
/* End Macro definition */


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */

/* End typedef */


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/

/* Start global function prototypes */

/*! 
 *	\brief Probe function for internet radio
 *	\param pHTuner : Tuner
 *	\param stMediaInfo : Media info structure
 *	\param bNeedHttpConnect : if already connected to destination, this shall be false.
 *	\return Error code
 *	\see 
 */
DI_ERR_CODE DI_STREAM_INETRADIO_Probe(DI_IPTUNER_t *pHTuner, DI_MEDIA_INFO_t *stMediaInfo, HBOOL bNeedHttpConnect);

/** Internet radio generic VFIO */
DI_ERR_CODE DI_STREAM_INETRADIO_Generic_Open(const HINT8 *path, const HINT8 *mode, VFIO_Handle_t *ptHandle);
DI_ERR_CODE DI_STREAM_INETRADIO_Generic_Close(VFIO_Handle_t tHandle);
DI_ERR_CODE DI_STREAM_INETRADIO_Generic_Read(VFIO_Handle_t tHandle, void *ptr, size_t size, size_t nmemb, ssize_t *ptReadSize);
DI_ERR_CODE DI_STREAM_INETRADIO_Generic_Seek(VFIO_Handle_t tHandle, off_t offset, int whence);
DI_ERR_CODE DI_STREAM_INETRADIO_Generic_Tell(VFIO_Handle_t tHandle, off_t *plCurOffset);
DI_ERR_CODE DI_STREAM_INETRADIO_Generic_IsEOF(VFIO_Handle_t tHandle);
DI_ERR_CODE DI_STREAM_INETRADIO_Generic_OpenFromNexus(const HINT8 *path, const unsigned int flags, VFIO_Handle_t *ptHandle);
off_t DI_STREAM_INETRADIO_Generic_SeekForNexus (VFIO_Handle_t tHandle, off_t offset, int whence);
DI_ERR_CODE DI_STREAM_INETRADIO_Generic_BoundsForNexus(VFIO_Handle_t tHandle, off_t *file_size);

/** ICY VFIO read */
DI_ERR_CODE DI_STREAM_INETRADIO_ICY_Read(VFIO_Handle_t tHandle, void *ptr, size_t size, size_t nmemb, ssize_t *ptReadSize);


/** Generic VFIO Registration */
void DI_STREAM_INETRADIO_Generic_RegisterCallback (const char *pUrl);
void DI_STREAM_INETRADIO_Generic_UnregisterCallback (const char *pUrl);

/** ICY VFIO Registeration */
void DI_STREAM_INETRADIO_ICY_RegisterCallback (const char *pUrl);
void DI_STREAM_INETRADIO_ICY_UnregisterCallback (const char *pUrl);



/* End global function prototypes */


#ifdef	__cplusplus
}
#endif

#endif /* !__DI_IPTUNER_INETRADIO_H__ */

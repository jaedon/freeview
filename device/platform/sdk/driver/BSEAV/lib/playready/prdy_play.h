/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: prdy_play.h $
 * $brcm_Revision: 5 $
 * $brcm_Date: 8/15/12 12:30p $
 *
 * Module Description:
 *
 * Playready library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/playready/prdy_play.h $
 * 
 * 5   8/15/12 12:30p hugost
 * SW7425-3573: Moved Drm_Initialize() call to bdrm_internal_create
 * 
 * 4   11/1/11 9:51a hugost
 * SW7425-1625: Added MP4 fragment support
 * 
 * 3   10/25/11 3:19p piyushg
 * SW7425-1607: Added file header to track revision history.
 ***************************************************************************/

#ifndef __DRMPLAY_H__
#define __DRMPLAY_H__
#if 0
#include <drmcommon.h>
#endif
struct DRM_APP_CONTEXT;
struct DRM_DECRYPT_CONTEXT;

/* called from the ASF parser to start grabbing of the license
*/
DRM_RESULT bdrm_lookup_license (
    DRM_BYTE* pbXML,                            /* [in] buffer with xml V2 WMDRM license header */ 
    DRM_DWORD cbXML,                            /* [in] byte count in V2 license header */
    DRM_APP_CONTEXT* pAppContext,               /* [in] resulting manager context */
    DRM_DECRYPT_CONTEXT** outDecryptCtx,        /* [out] resulting decrypt context */
    bdrm_license_t* license,
    DRMPFNPOLICYCALLBACK policy_cback,          /* [in] pointer to the policy callback */
    void * policy_ctx                           /* [in] pointer to the policy callback context */
   );

/* called from the drmcore, to destroy the live license instance
*/
DRM_RESULT bdrm_destroy_license (
    bdrm_license_t license                              /* [in] license instance handle */
    );

/* play content function. original implementation
*/
int bdrm_play_content (
	DRM_CONST_STRING *pdstrContentFilename,             /* [in] */
	DRM_CONST_STRING *pdstrOutputFileName,              /* [in] */
	DRM_CONST_STRING *pdstrDataStoreFile                /* [in] */
	);

#endif /* __DRMPLAY_H__ */

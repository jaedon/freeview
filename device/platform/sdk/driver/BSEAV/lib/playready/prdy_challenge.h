/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: prdy_challenge.h $
 * $brcm_Revision: 5 $
 * $brcm_Date: 10/4/12 10:48a $
 *
 * Module Description:
 *
 * Playready library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/playready/prdy_challenge.h $
 * 
 * 5   10/4/12 10:48a hugost
 * SW7425-4032: Add custom data support to challenge generation
 * 
 * 4   11/1/11 9:50a hugost
 * SW7425-1625: Added MP4 fragment support
 * 
 * 3   10/25/11 3:18p piyushg
 * SW7425-1607: Added file header to track revision history.
 ***************************************************************************/

#ifndef __DRM_CHALLENGE_H__
#define __DRM_CHALLENGE_H__

/* function used to generate the actual challenge to be sent to the server and return
** the two resulting  contexts : the manager context and the drm crypt context.
*/
DRM_RESULT bdrm_generate_lic_challenge (
    DRM_APP_CONTEXT *poManagerContext, 	        /* [in] drm manager context */
    DRM_CHAR *pCustomData,                      /* [in] custom data */
    uint32_t cchCustomDataLen,                  /* [in] length of the custom data string */
    DRM_CHAR** ch_url,                          /* [out] challenge url */
    DRM_CHAR** ch_data,                         /* [out] challenge data, base64 encoded */
    uint32_t* ch_len                            /* [out] length of the challenge */
    );


#ifdef PRDY_ROBUSTNESS
DRM_RESULT
bdrm_generate_client_info(DRM_APP_CONTEXT *poManagerContext, DRM_CHAR** ci_xml, uint32_t* ci_length);

void bdrm_free_client_info(DRM_CHAR* ci_xml);
#endif

#endif /* __DRM_CHALLENGE_H__ */

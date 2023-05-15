/***************************************************************************
 *     Copyright (c) 2002-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: prdy_response.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 10/25/11 3:19p $
 *
 * Module Description:
 *
 * Playready library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/playready/prdy_response.h $
 * 
 * 2   10/25/11 3:19p piyushg
 * SW7425-1607: Added file header to track revision history.
 ***************************************************************************/

#ifndef __DRMRESPONSE_H__
#define __DRMRESPONSE_H__

/* processes the response as it is being sent from the license server.
** This is an XML blob that contains the encrypted license. Only the 
** rightful recipient should be able to obtain a valid license.
*/
DRM_RESULT bdrm_process_response (
    DRM_APP_CONTEXT* poManagerContext,              /* [in] drm manager context */
    DRM_BYTE* pbData,                                   /* [in] response data, as obtained from the server */
    DRM_DWORD dw                                        /* [in] size of the response data */
    );

/* loads a sample response file into a DRM_BYTE buffer. Use this with
** tests to verify if the response processing function works well.
*/
DRM_RESULT bdrm_util_load_response (
    DRM_WCHAR *pwszResponseFilename,                    /* [in] response file filename */
    DRM_BYTE** pbData,                                  /* [out] placeholder for response data */
    uint32_t* dw                                        /* [out] length of response data */
    );

#endif /* __DRMRESPONSE_H__ */



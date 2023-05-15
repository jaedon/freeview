/***************************************************************************
 *     Copyright (c) 2002-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: prdy_secure_clk.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 10/25/11 3:19p $
 *
 * Module Description:
 *
 * Playready library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/playready/prdy_secure_clk.h $
 * 
 * 2   10/25/11 3:19p piyushg
 * SW7425-1607: Added file header to track revision history.
 ***************************************************************************/

#ifndef PRDY_SECURE_CLK_H_
#define PRDY_SECURE_CLK_H_

#include <drmcommon.h>

/*********************************************************************************
Function name:  DRM_RESULT bdrm_clk_generate_challenge()
Input parameters:    
    poManagerContext
        Type:   DRM_MANAGER_CONTEXT *
        Purpose: Opaque DRM manager context returned from DRM_MGR_Initialize
        
Output Parameters:   
    ch_url 
        Type:   DRM_CHAR **
        Purpose: user allocated buffer to get URL in WCHAR (null terminated)

    ch_data 
        Type:   DRM_BYTE **
        Purpose: user allocated buffer to be used as output buffer for challenge data
        
Input / Output Parameters:   
    url_len 
        Type:   DRM_DWORD *
        Purpose:    Character count of ch_url.  Can be NULL if URL is not required

    ch_len    
        Type:   DRM_DWORD *
        Purpose:    size of challenge data

Returns:    On Success:                 DRM_SUCCESS 
            On Failure:                 other
*************************************************************************/
DRM_RESULT bdrm_secure_clk_gen_challenge (  DRM_APP_CONTEXT *poManagerContext,          /* [in] drm manager context */
                                            DRM_CHAR** ch_url,       /* [out] challenge url */
                                            DRM_BYTE** ch_data,      /* [out] challenge data */
                                            uint32_t* url_len,       /* [out] length of the url challenge */
                                            uint32_t* ch_len);       /* [out] length of the challenge data*/


/****************************************************************************
Function name:  DRM_RESULT bdrm_clk_process_response()
Input parameters:   
    poManagerContext
        Type:   DRM_MANAGER_CONTEXT *
        Purpose: Opaque DRM manager context returned from DRM_MGR_Initialize

    pbResponse  
        Type:   DRM_BYTE *
        Purpose: response string received from server

    cbResponse  
        Type:   DRM_WORD
        Purpose: size of pbResponse in bytes

Output Parameters:
    result
        Type: DRM_RESULT
        Purpose: Pointer to a DRM_RESULT indicating the error from the clock service, so it can be included in the response

Returns:    On Success:                 DRM_SUCCESS 
            On Failure:                 other
        
*****************************************************************************/ 
DRM_RESULT bdrm_secure_clk_process_response(DRM_APP_CONTEXT* poManagerContext,   /* [in] drm manager context */
                                            DRM_BYTE *pbResponse,   /* [in] response string received from server */
                                            DRM_DWORD cbResponse,   /* [in] size of pbResponse in bytes */
                                            DRM_RESULT *result);

/****************************************************************************
Function name:  DRM_RESULT bdrm_get_secure_clk_status()
Input parameter:  
    poManagerContext
        Type:   DRM_MANAGER_CONTEXT *
        Purpose: Opaque DRM manager context returned from DRM_MGR_Initialize
        
Output parameters:   
    pwszSecureTime  
        Type: DRM_BYTE  *
        Purpose: Pointer to a user-allocated buffer to hold the secure time. If NULL is specified, the required buffer size is returned.

    pdwFlags    
        Type:   DRM_WORD *
        Purpose: Pointer to flag indicating the state of the secure clock, which can be one of the following values.
            DRM_CLK_NOT_SET (value = 0)
            DRM_CLK_SET (value = 1)
            DRM_CLK_NEEDS_REFRESH (value = 2)

    pbSecureTimeWithStatus
        Purpose: pointer to BYTE buffer to get Secure time and flag status in XML format. Optional 
    
    pcbSecureTimeWithStatus
        Purpose: size of buffer. Optional

Input/ Output parameter:    
    pcchSecureTime  
        Type:  DRM_WORD *
        Purpose: On input, this parameter should point to the current buffer size of pwszSecureTime. 
                 On output, it will point to the number of characters that were placed in pwszSecureTime.
*****************************************************************************/
DRM_RESULT bdrm_get_secure_clk_status(DRM_APP_CONTEXT* poManagerContext,   /* [in] drm manager context */
                                      DRM_CHAR **pwszSecureTime,   /* [out] Pointer to a user-allocated buffer to hold the secure time */
                                      DRM_DWORD *pcchSecureTime,   /* [in/out] see above header comments */
                                      DRM_DWORD *pdwFlags,    /* [in] Pointer to flag indicating the state of the secure clock */
                                      DRM_BYTE  *pbSecureTimeWithStatus,
                                      DRM_DWORD *pcbSecureTimeWithStatus);


#endif /*PRDY_SECURE_CLK_H_*/

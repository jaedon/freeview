/***************************************************************************
*     Copyright (c) 2011-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: prdy_secure_clk.c $
* $brcm_Revision: 3 $
* $brcm_Date: 7/17/12 4:13p $
*
* Module Description:
*
* DRM library
* 
* Revision History:
*
* $brcm_Log: /BSEAV/lib/playready/prdy_secure_clk.c $
* 
* 3   7/17/12 4:13p piyushg
* SW7425-3281: Add playready kernel mode support and nexus drm
* implementation.
* 
* SW7425-2317/1   2/28/12 10:01p piyushg
* Added file element "*.a".
* Added file element "certs.a".
* Added file element "cmdlnpars.a".
* Added file element "constants.a".
* Added file element "domain.a".
* Added file element "drmbignum.a".
* Added file element "drmblackbox.a".
* Added file element "drmcore.a".
* Added file element "drmcrypto.a".
* Added file element "drmdevcert.a".
* Added file element "drmdevicedevcert.a".
* Added file element "drmmanager.a".
* Added file element "drmmetering.a".
* Added file element "drmpkcrypto.a".
* Added file element "drmsecureclock.a".
* Added file element "drmsha256.a".
* Added file element "drmsynclist.a".
* Added file element "drmtoolscommon.a".
* Added file element "drmxmr.a".
* Added file element "ndt.a".
* Added file element "ndtasf.a".
* Added file element "ndtmgr.a".
* Added file element "noncestore.a".
* Added file element "oem_ansi.a".
* Added file element "oem_common.a".
* Added file element "wmdrmcert.a".
* Added directory element "playreadylinuxkernel".
* Added file element "Makefile.wmdrmpd".
* SW7425-2317: Initial checkin for Nexus DRM usrmode and kernelmode
* support
* 
* 2   10/4/11 4:47p piyushg
* SW7425-1376: Fix compilation warnings.
* 
* 1   9/27/11 7:25p piyushg
* SW7425-1358: Make playready compliant with robustness rules and
* maintain one branch for playready with
* robustness and without. Use PRDY_ROBUSTNESS_ENABLE=y along with
* MSDRM_PRDY_SUPPORT=y to have playready with robustness.
* 
* dtv_v3.0/1   5/11/11 4:16p hugost
* Merge Playready Robustness rules into dtv branch
* 
* dev_dtv_v3.0_SWDTV-6935/1   5/11/11 11:17a hugost
* Added secure clock.
* 
* fix_bdvd_v3.0_SWBLURAY-25587/1   4/29/11 11:00a hugost
* Added Secure Clock Support.
* 
*******************************************************************************/

#include <drmcommon.h>
#include <drmutilities.h>
#include <drmcrt.h>
#include <drmcontextsizes.h>
#include <drmmanager.h>
#include <drmsecureclock.h>
#ifndef __KERNEL__
#include <stdio.h>
#endif

#include "bstd.h"                   /* BRCM standard includes */


BDBG_MODULE(bdrm_secureclock);        /* debug handle */

DRM_RESULT bdrm_secure_clk_gen_challenge (  DRM_APP_CONTEXT *poManagerContext,          /* [in] drm manager context */
                                            DRM_CHAR** ch_url,       /* [out] challenge url */
                                            DRM_BYTE** ch_data,      /* [out] challenge data */
                                            uint32_t* url_len,       /* [out] length of the url challenge */
                                            uint32_t* ch_len)        /* [out] length of the challenge data*/
{
    DRM_RESULT dr = DRM_SUCCESS;
#if DRM_SUPPORT_SECURE_CLOCK
    DRM_DWORD   cchUrl       = 0;
    DRM_DWORD   cchChallenge = 0;
    DRM_WCHAR  *pwszUrl      = NULL; 
    DRM_BYTE   *pszChallenge = NULL;
    
    BDBG_MSG(("bdrm_secure_clk_gen_challenge - entered function"));
    
    if(ch_url == NULL) {
        dr = DRM_E_INVALIDARG;
        goto ErrorExit;
    }

    dr =  Drm_SecureClock_GenerateChallenge(poManagerContext, /* determine length of challenge */
                                        NULL, &cchUrl, 
                                        NULL, &cchChallenge);

    if(dr != DRM_E_BUFFERTOOSMALL){                          /* alloc buffers accordingly */
        BDBG_ERR(("bdrm_secure_clk_gen_challenge - return code not DRM_E_BUFFERTOOSMALL"));
        goto ErrorExit;
    }
    
    BDBG_MSG(("bdrm_secure_clk_gen_challenge - Allocating '%u' bytes to 'pwszUrl' and '%u' bytes to 'pszChallenge'", cchUrl, cchChallenge));    

    ChkMem(pwszUrl = (DRM_WCHAR*) Oem_MemAlloc(cchUrl * SIZEOF (DRM_WCHAR)));
    ChkMem(*ch_url = (DRM_CHAR*) Oem_MemAlloc(cchUrl * SIZEOF (DRM_CHAR)));
    ChkMem(*ch_data = pszChallenge = (DRM_BYTE*) Oem_MemAlloc(cchChallenge));
    
    BDBG_MSG(("bdrm_secure_clk_gen_challenge - After Allocating bytes: '*ch_url' = '%p' '*ch_data' = '%p', 'pszChallenge' = '%p'...", (*ch_url), (*ch_data), pszChallenge));
    
    ChkDR( Drm_SecureClock_GenerateChallenge(poManagerContext,/* retreive the actual challenge */
                                        pwszUrl, &cchUrl, 
                                        pszChallenge, &cchChallenge));

    //set the sizes to return to the caller
    *url_len = cchUrl;
    *ch_len = cchChallenge;

    // transform the URL from wide-char to ANSI 
    DRM_UTL_DemoteUNICODEtoASCII(pwszUrl, *ch_url, cchUrl); 

ErrorExit:
    if (pwszUrl != NULL){/* clean up temp data */
        BDBG_MSG(("bdrm_secure_clk_gen_challenge - Freeing 'pwszUrl' buffer..."));
        Oem_MemFree(pwszUrl);
        pwszUrl = NULL;
    }

    if(dr != DRM_SUCCESS){
        Oem_MemFree(*ch_url);
        Oem_MemFree(*ch_data );
        *ch_data = NULL;
        *ch_url = NULL;
    }

#else 
    BSTD_UNUSED(poManagerContext);
    BSTD_UNUSED(ch_url);
    BSTD_UNUSED(ch_data);
    BSTD_UNUSED(url_len);
    BSTD_UNUSED(ch_len);
    BDBG_ERR(("bdrm_secure_clk_gen_challenge - 'DRM_SUPPORT_SECURE_CLOCK' NOT DEFINED!!!!!"));
    dr = DRM_E_NOTIMPL;
#endif 

    return (dr);
}

DRM_RESULT bdrm_secure_clk_process_response(DRM_APP_CONTEXT* poManagerContext,   /* [in] drm manager context */
                                            DRM_BYTE *pbResponse,   /* [in] response string received from server */
                                            DRM_DWORD cbResponse,   /* [in] size of pbResponse in bytes */
                                            DRM_RESULT *result)  
{
    DRM_RESULT dr = DRM_SUCCESS;
    
#if DRM_SUPPORT_SECURE_CLOCK
    dr = Drm_SecureClock_ProcessResponse(poManagerContext, pbResponse, cbResponse, result);
#else
    BSTD_UNUSED(poManagerContext);
    BSTD_UNUSED(pbResponse);
    BSTD_UNUSED(cbResponse);
    BSTD_UNUSED(result);
    BDBG_ERR(("bdrm_secure_clk_process_response - 'DRM_SUPPORT_SECURE_CLOCK' NOT DEFINED!!!!!"));
    dr = DRM_E_NOTIMPL;
#endif 

    return (dr);
}




DRM_RESULT bdrm_get_secure_clk_status(DRM_APP_CONTEXT* poManagerContext,   /* [in] drm manager context */
                                      DRM_CHAR **pwszSecureTime,   /* [out] Pointer to a user-allocated buffer to hold the secure time */
                                      DRM_DWORD *pcchSecureTime,   /* [in/out] see above header comments */
                                      DRM_DWORD *pdwFlags,    /* [in] Pointer to flag indicating the state of the secure clock */
                                      DRM_BYTE  *pbSecureTimeWithStatus,
                                      DRM_DWORD *pcbSecureTimeWithStatus)   
{
    DRM_RESULT dr = DRM_SUCCESS;

#if DRM_SUPPORT_SECURE_CLOCK
    DRM_WCHAR  *meta_pwszSecureTime      = NULL; 

    *pcchSecureTime = 0;
    *pcbSecureTimeWithStatus = 0;
    dr = Drm_SecureClock_GetValue(poManagerContext, 
                                    NULL, pcchSecureTime, 
                                    pdwFlags, 
                                    NULL, pcbSecureTimeWithStatus);
    if(dr != DRM_E_BUFFERTOOSMALL)
    {
        BDBG_ERR(("bdrm_get_secure_clk_status - return code not DRM_E_BUFFERTOOSMALL (%u)", dr));
        goto ErrorExit;
    }

    BDBG_MSG(("bdrm_get_secure_clk_status - buffer size to allocate '%u'", (*pcchSecureTime) ));
    ChkMem(meta_pwszSecureTime = (DRM_WCHAR*) Oem_MemAlloc((*pcchSecureTime) * SIZEOF (DRM_DWORD)));


    dr = Drm_SecureClock_GetValue(poManagerContext, 
                                    meta_pwszSecureTime, pcchSecureTime, 
                                    pdwFlags, 
                                    pbSecureTimeWithStatus, pcbSecureTimeWithStatus);
    if(dr == DRM_SUCCESS){
        DRM_UTL_DemoteUNICODEtoASCII(meta_pwszSecureTime, *pwszSecureTime, (*pcchSecureTime));  /* transform from wide-char to ANSI */        
    }
    else{
        BDBG_ERR(("bdrm_get_secure_clk_status - failure in SDK for getting secure clock status (%u)", dr));
    }

ErrorExit:
    if (meta_pwszSecureTime != NULL)                                    /* clean up temp data */
        Oem_MemFree(meta_pwszSecureTime);

#else 
    BSTD_UNUSED(poManagerContext);
    BSTD_UNUSED(pwszSecureTime);
    BSTD_UNUSED(pcchSecureTime);
    BSTD_UNUSED(pdwFlags);
    BSTD_UNUSED(pbSecureTimeWithStatus);
    BSTD_UNUSED(pcbSecureTimeWithStatus);
    BDBG_ERR(("bdrm_get_secure_clk_status - 'DRM_SUPPORT_SECURE_CLOCK' NOT DEFINED!!!!!"));
    dr = DRM_E_NOTIMPL;
#endif

    return dr;
}


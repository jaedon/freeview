/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: prdy_challenge.c $
 * $brcm_Revision: 11 $
 * $brcm_Date: 10/15/12 3:13p $
 *
 * Module Description:
 *
 * Playready library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/playready/prdy_challenge.c $
 * 
 * 11   10/15/12 3:13p hugost
 * SW7425-4020: Pass id to Oem_Create
 * 
 * 10   10/4/12 10:48a hugost
 * SW7425-4032: Add custom data support to challenge generation
 * 
 * 9   7/17/12 4:13p piyushg
 * SW7425-3281: Add playready kernel mode support and nexus drm
 * implementation.
 * 
 * SW7425-2317/4   7/13/12 5:45p piyushg
 * SW7425-3281: Fix bunch of compile time warnings in both usermode and
 * kernelmode
 * 
 * SW7425-2317/3   4/27/12 2:10p hugost
 * Mutli-process integration
 * 
 * SW7425-2317/2   3/7/12 4:30p piyushg
 * SW7425-2317: Checkin for Nexus DRM usrmode and kernelmode support with
 * stack overflow and kernel freeze issues resolved.
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
 * 8   11/1/11 9:50a hugost
 * SW7425-1625: Added MP4 fragment support
 * 
 * 7   10/25/11 3:14p piyushg
 * SW7425-1607: Added file header to track revision history.
 ***************************************************************************/

#include <drmcommon.h>
#include <drmutilities.h>
#include <drmcrt.h>
#include <drmcontextsizes.h>
#include <drmmanager.h>
#include <oemcommon.h>

#ifdef PRDY_ROBUSTNESS
#include <oem_brcm.h>
#include "nexus_otpmsp.h"

extern void bdrm_read_otp_id_priv(NEXUS_ReadOtpIO *pOtpid);

#endif

#include <drmcmdlnpars.h>
#ifndef __KERNEL__
#include <stdio.h>
#include <wchar.h>
#endif
#include "bstd.h"                   /* BRCM standard includes */

BDBG_MODULE(bdrm_challenge);        /* debug handle */

#ifdef PRDY_ROBUSTNESS
DRM_RESULT
bdrm_generate_client_info(DRM_APP_CONTEXT *poManagerContext, DRM_CHAR** ci_xml, uint32_t* ci_length)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  cch_ci = 0;
    DRM_BYTE*  tmp_ci_xml = NULL;

    if(poManagerContext == NULL || ci_xml == NULL || ci_length == NULL){
        BDBG_ERR(("bdrm_generate_client_info -  invalid parameter\n"));
        dr = DRM_E_INVALIDARG;
        goto ErrorExit;
    }

    /* Figure out how much memory we need to store the client info */
    dr = Drm_Device_GetProperty(poManagerContext, DRM_DGP_CLIENT_INFO, NULL, &cch_ci);
    if(dr != DRM_E_BUFFERTOOSMALL){                          /* alloc buffers accordingly */
        BDBG_ERR(("bdrm_generate_client_info -  Drm_Device_GetProperty() failed, couldn't determined the amount of memory needed to hold the client info\n"));
        goto ErrorExit;
    }
 
    /* Allocate enough memory to store the client info. */
    ChkMem((*ci_xml) = (DRM_CHAR*) Oem_MemAlloc(cch_ci * SIZEOF (DRM_BYTE)));
    ChkMem(tmp_ci_xml = (DRM_BYTE*) Oem_MemAlloc(cch_ci * SIZEOF (DRM_BYTE)));

    BDBG_MSG(("bdrm_generate_client_info() - calling Drm_Device_GetProperty\n"));
    ChkDR(Drm_Device_GetProperty(poManagerContext, DRM_DGP_CLIENT_INFO, tmp_ci_xml, &cch_ci));
    BDBG_MSG(("bdrm_generate_client_info() - Drm_Device_GetProperty() succeeded\n"));

    /* DRM_DGP_CLIENT_INFO is UNICODE blob which is NOT NULL terminated */
    DRM_UTL_DemoteUNICODEtoASCII((DRM_WCHAR *)tmp_ci_xml, (*ci_xml), cch_ci);  /* transform from wide-char to ANSI */
    *ci_length = (cch_ci / sizeof(DRM_WCHAR));

ErrorExit:

    /* Clean up temporary buffer used for UNICODE to ANSI conversion. */
    if(tmp_ci_xml != NULL) Oem_MemFree(tmp_ci_xml);

    if(dr != DRM_SUCCESS){
        BDBG_ERR(("bdrm_generate_client_info -  exiting with an error.\n"));
        if(*ci_xml != NULL) Oem_MemFree(*ci_xml);
    }

    return (dr);
}

void bdrm_free_client_info(DRM_CHAR* ci_xml)
{
    if(ci_xml != NULL) Oem_MemFree(ci_xml);
}
#endif

/* generate a license challenge, based on a license manager context.
   output consists of an url for the license script, and the challenge blob.
*/
DRM_RESULT
bdrm_generate_lic_challenge(DRM_APP_CONTEXT *poManagerContext, 
                            DRM_CHAR *pCustomData,
                            uint32_t customDataLen,
                            DRM_CHAR** ch_url, 
                            DRM_CHAR** ch_data,
                            uint32_t* ch_len)
{
    DRM_CHAR   *rgchURL = NULL;
    DRM_DWORD   cchUrl       = 0,
                cchChallenge = 0;
    DRM_DWORD  *pcchUrl = NULL;
    DRM_WCHAR  *pwszUrl      = NULL; 

    DRM_BYTE   *pszChallenge = NULL;
    DRM_RESULT dr = DRM_SUCCESS;
    
    BDBG_MSG(("generating lic challenge dr = %X, cchChallenge=%d", 
              dr, cchChallenge));

    if(ch_url != NULL){
        pcchUrl = &cchUrl;
    }

    dr = Drm_LicenseAcq_GenerateChallenge (
        poManagerContext, /* determine length of challenge */
        NULL,
        0,
        NULL,
        pCustomData,
        customDataLen,
        NULL,
        pcchUrl,
        NULL,
        0,
        NULL, 
        &cchChallenge);

    if(dr != DRM_E_BUFFERTOOSMALL){                          /* alloc buffers accordingly */
        BDBG_MSG(("%s - first call to Drm_LicenseAcq_GenerateChallenge() failed, dr = %x", __FUNCTION__, dr));
        goto ErrorExit;
    }
    
    if(ch_url != NULL) {
        ChkMem(pwszUrl = (DRM_WCHAR*) Oem_MemAlloc(cchUrl * SIZEOF (DRM_WCHAR)));
        ChkMem(*ch_url = rgchURL = (DRM_CHAR*) Oem_MemAlloc(cchUrl * SIZEOF (DRM_CHAR)));
    }

    if((ch_len != NULL) && (ch_data != NULL)) {
        ChkMem(pszChallenge = (DRM_BYTE*) Oem_MemAlloc(cchChallenge * SIZEOF(DRM_CHAR)));
        *ch_data = (DRM_CHAR*)pszChallenge;
        *ch_len = cchChallenge;
    }

    dr = Drm_LicenseAcq_GenerateChallenge(poManagerContext,/* retreive the actual challenge */
        NULL,
        0,
        NULL,
        pCustomData,
        customDataLen,
        rgchURL,  /* Netflix no url case */
        pcchUrl,  /* Netflix no url case */
        NULL,
        0, 
        pszChallenge, 
        &cchChallenge);

     *ch_len = cchChallenge; /*adjust the challenge data size to actual size */

     if (pwszUrl != NULL)                                    /* clean up temp data */
        Oem_MemFree(pwszUrl);

#ifdef __CTX_CLEAN_ON_EXIT__
    if (pszChallenge != NULL)
        Oem_MemFree(pszChallenge);

    if (poManagerContext != NULL) {
        Drm_Uninitialize(poManagerContext);
        Oem_MemFree(poManagerContext);
    }
#endif /* __CTX_CLEAN_ON_EXIT */

    return (dr);

ErrorExit:
    if (pwszUrl != NULL)
        Oem_MemFree(pwszUrl);

    if (pszChallenge != NULL)
        Oem_MemFree(pszChallenge);

    if (rgchURL != NULL)
        Oem_MemFree(rgchURL);

    return (dr);
}

/* generates a license challenge based on the contents of the xml
 * blob within the asf, outputs to a file
 * */
int
bdrm_generate_file_lic_challenge(DRM_CONST_STRING *pdstrContentFilename, 
        DRM_CONST_STRING *pdstrOutputFileName, 
        DRM_CONST_STRING *f_pdstrFileURL,
        DRM_CONST_STRING *pdstrDataStoreFile)
{
    int ret = 0; DRM_BYTE *pbXML = NULL;
    DRM_APP_CONTEXT *poManagerContext = NULL;
    DRM_VOID* f_pOEMContext = NULL;
    DRM_CHAR rgchURL[1024];
    const DRM_CONST_STRING *rgpdstrRights [1] = { 0 };
    DRM_DWORD cbRead = 0, cbXML = 0, cbWritten = 0, cchUrl = 0,
              cchChallenge = 0;
    
    DRM_RESULT dr = DRM_SUCCESS; OEM_FILEHDL hfileURL = OEM_INVALID_HANDLE_VALUE;
    OEM_FILEHDL hfileIn = OEM_INVALID_HANDLE_VALUE, hfileOut = OEM_INVALID_HANDLE_VALUE;    
    DRM_WCHAR  *pwszUrl = NULL; DRM_CHAR *pszChallenge = NULL; 
    
    BSTD_UNUSED (pdstrContentFilename);                             
    ChkMem(poManagerContext = Oem_MemAlloc (SIZEOF (DRM_APP_CONTEXT)));
    ZEROMEM(poManagerContext, SIZEOF (DRM_APP_CONTEXT));    

    rgpdstrRights [0] = &g_dstrWMDRM_RIGHT_PLAYBACK;

    #ifdef PRDY_ROBUSTNESS
    {
        NEXUS_ReadOtpIO  otp_id;
        bdrm_read_otp_id_priv(&otp_id);
        f_pOEMContext = Oem_Create(&otp_id);
    }
    ChkMem(f_pOEMContext);
	#endif

    ChkDR (Drm_Initialize(poManagerContext, f_pOEMContext, pdstrDataStoreFile));
    ChkDR (Drm_Content_SetProperty(poManagerContext, DRM_CSP_AUTODETECT_HEADER, pbXML, cbXML));/* set v2 header from asf */
    dr = Drm_LicenseAcq_GenerateChallenge (poManagerContext, /* determine length of challenge */
                rgpdstrRights,
                1,
                NULL, 
                NULL,
                0,
                rgchURL,
                &cchUrl,
                NULL,
                0,
                NULL,
                &cchChallenge);

    if(dr != DRM_E_BUFFERTOOSMALL)
        goto ErrorExit;
    
    ChkMem(pwszUrl = (DRM_WCHAR*)Oem_MemAlloc(cchUrl * SIZEOF (DRM_WCHAR)));
    ChkMem(pszChallenge = (DRM_CHAR*)Oem_MemAlloc(cchChallenge));    

    ChkDR(Drm_LicenseAcq_GenerateChallenge(poManagerContext,/* retreive the actual challenge */
                rgpdstrRights,
                NO_OF(rgpdstrRights),
                NULL,
                NULL,
                0,
                rgchURL,
                &cchUrl,
                NULL,
                0,
                (DRM_BYTE *)pszChallenge,
                &cchChallenge));

    hfileOut = Oem_File_Open(NULL,
            pdstrOutputFileName->pwszString,
            OEM_GENERIC_READ | OEM_GENERIC_WRITE,
            OEM_FILE_SHARE_READ | OEM_FILE_SHARE_WRITE,
            OEM_CREATE_ALWAYS,
            OEM_ATTRIBUTE_NORMAL);

    if (hfileOut != OEM_INVALID_HANDLE_VALUE){  /* copy response to fp */
        if (!Oem_File_Write(hfileOut, pszChallenge, cchChallenge, &cbRead) 
                ||  cbRead != cchChallenge) {
            BDBG_WRN(("Failed to write challenge"));
            goto ErrorExit;
        }
    } else {
        BDBG_MSG(("Challenge file not opened"));
        goto ErrorExit;
    }
    
    if (f_pdstrFileURL->cchString > 0) {        /* if url output file requested */
        hfileURL = Oem_File_Open(NULL,
                f_pdstrFileURL->pwszString,
                OEM_GENERIC_READ | OEM_GENERIC_WRITE,
                OEM_FILE_SHARE_READ | OEM_FILE_SHARE_WRITE, 
                OEM_CREATE_ALWAYS, OEM_ATTRIBUTE_NORMAL);

        if (hfileURL != OEM_INVALID_HANDLE_VALUE) { 
            DRM_DWORD cbUrl = cchUrl * SIZEOF (DRM_WCHAR);
            if (!Oem_File_Write(hfileURL, pwszUrl, cbUrl, &cbWritten) 
                    ||  cbWritten != cbUrl){
                BDBG_WRN(("Failed to write Url file"));
            } Oem_File_Close(hfileURL);
        } else BDBG_WRN(("Url file not opened"));
    }

ErrorExit:
    if (hfileOut != OEM_INVALID_HANDLE_VALUE)   /* free up handles */
        Oem_File_Close(hfileOut);

    if (hfileIn != OEM_INVALID_HANDLE_VALUE) 
        Oem_File_Close(hfileIn);

    if (pwszUrl != NULL)                        /* free alloc-ed challenge */
        Oem_MemFree(pwszUrl);

    if (pszChallenge != NULL)                   
        Oem_MemFree(pszChallenge);

    if (poManagerContext != NULL) {
        Drm_Uninitialize(poManagerContext);
        Oem_MemFree(poManagerContext);
    }

	#ifdef PRDY_ROBUSTNESS
    if(f_pOEMContext != NULL)
        Oem_Destroy(f_pOEMContext);
	#endif

    return (ret);
}


/* Processes the license response as sent from the server, 
 * after the license challenge is delivered.
 * */
void 
drm_lic_process_response( DRM_CONST_STRING *pwszResponseFilename, 
        DRM_CONST_STRING *pwszDataStoreFile)
{
    DRM_RESULT dr = DRM_SUCCESS; DRM_DWORD dw;
    DRM_APP_CONTEXT *poManagerContext = NULL; DRM_DWORD iIO=0;
    DRM_BYTE* pbData = NULL; OEM_FILEHDL fpResponse = OEM_INVALID_HANDLE_VALUE;
    DRM_VOID* f_pOEMContext = NULL;
    
    ChkMem( poManagerContext = Oem_MemAlloc( SIZEOF(DRM_APP_CONTEXT) ) );
    ZEROMEM(poManagerContext, SIZEOF(DRM_APP_CONTEXT));

    wcsncpy( (DRM_WCHAR *)poManagerContext, (const DRM_WCHAR *)pwszResponseFilename->pwszString, pwszResponseFilename->cchString );
    ((DRM_WCHAR*)poManagerContext)[pwszResponseFilename->cchString] = g_wchNull;

    fpResponse = Oem_File_Open(NULL,
                               ((DRM_WCHAR*)poManagerContext),
                               OEM_GENERIC_READ,
                               OEM_FILE_SHARE_READ,
                               OEM_OPEN_EXISTING,
                               OEM_ATTRIBUTE_NORMAL);

    if(OEM_INVALID_HANDLE_VALUE == fpResponse) {
        BDBG_WRN(( "Input file not opened"));
        goto ErrorExit;
    }

#ifdef PRDY_ROBUSTNESS
    {
        NEXUS_ReadOtpIO  otp_id;
        bdrm_read_otp_id_priv(&otp_id);
        f_pOEMContext = Oem_Create(&otp_id);
    }
    ChkMem(f_pOEMContext);
#endif

    ChkDR (Drm_Initialize(poManagerContext, f_pOEMContext, pwszDataStoreFile));
    Oem_File_GetSize(fpResponse, &dw);       
    Oem_File_SetFilePointer(fpResponse, 0, OEM_FILE_BEGIN, NULL);
    ChkMem(pbData = (DRM_BYTE*)Oem_MemAlloc(dw));                     

    if (!Oem_File_Read(fpResponse, pbData, dw, &iIO) || iIO!=dw)
        ChkDR(DRM_E_FILEREADERROR);
                                                                     
    ChkDR(Drm_LicenseAcq_ProcessAckResponse(poManagerContext,    /* process response here */
               pbData, dw, NULL));

ErrorExit:                                                          /* TODO: handle exit */
    Drm_Uninitialize(poManagerContext);                         /* uninit manager context */

	#ifdef PRDY_ROBUSTNESS
    if(f_pOEMContext != NULL)
        Oem_Destroy(f_pOEMContext);
	#endif

    if (fpResponse != OEM_INVALID_HANDLE_VALUE)
        Oem_File_Close(fpResponse);

    if (pbData != NULL) 
        Oem_MemFree(pbData);

    if (poManagerContext != NULL) 
        Oem_MemFree(poManagerContext);
}



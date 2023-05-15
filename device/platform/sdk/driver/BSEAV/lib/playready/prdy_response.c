/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: prdy_response.c $
 * $brcm_Revision: 7 $
 * $brcm_Date: 10/15/12 3:13p $
 *
 * Module Description:
 *
 * Playready library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/playready/prdy_response.c $
 * 
 * 7   10/15/12 3:13p hugost
 * SW7425-4020: Pass id to Oem_Create
 * 
 * 6   7/17/12 4:13p piyushg
 * SW7425-3281: Add playready kernel mode support and nexus drm
 * implementation.
 * 
 * SW7425-2317/3   7/13/12 5:45p piyushg
 * SW7425-3281: Fix bunch of compile time warnings in both usermode and
 * kernelmode
 * 
 * SW7425-2317/2   3/7/12 4:31p piyushg
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
 * 5   11/1/11 9:51a hugost
 * SW7425-1625: Added MP4 fragment support
 * 
 * 4   10/25/11 3:18p piyushg
 * SW7425-1607: Added file header to track revision history.
 ***************************************************************************/

#include <drmcommon.h>
#include <drmutilities.h>
#include <drmcrt.h>
#include <drmcontextsizes.h>
#include <drmmanager.h>
#include <oemcommon.h>
#include <oem_brcm.h>
#ifndef __KERNEL__
#include <stdio.h>
#include <wchar.h>
#endif
#include <drmcmdlnpars.h>
#include "bstd.h"               /* BRCM includes */

#ifdef PRDY_ROBUSTNESS
#include "nexus_otpmsp.h"

extern void bdrm_read_otp_id_priv(NEXUS_ReadOtpIO *pOtpid);
#endif

BDBG_MODULE(bdrm_response);     /* debug handle */

/* processes the response as it is being sent from the license server.
** This is an XML blob that contains the encrypted license. Only the 
** rightful recipient should be able to obtain a valid license.
*/
DRM_RESULT 
bdrm_process_response(DRM_APP_CONTEXT* poManagerContext,
					  DRM_BYTE* pbData, DRM_DWORD dw)
{
    DRM_RESULT dr = DRM_E_FAIL;    
    #ifndef __KERNEL__
    DRM_LICENSE_RESPONSE f_poLicenseResponse;
    #else
    DRM_LICENSE_RESPONSE *f_poLicenseResponse;

    f_poLicenseResponse = BKNI_Malloc(sizeof(DRM_LICENSE_RESPONSE));
    #endif

    BDBG_ASSERT(poManagerContext != NULL);
    BDBG_ASSERT(pbData != NULL);


    BDBG_MSG(("Drm_LicenseAcq_ProcessResponse with : (%d) bytes\n", dw));

    if ((poManagerContext != NULL) && (pbData != NULL)) {
        #ifndef __KERNEL__
        dr = Drm_LicenseAcq_ProcessResponse(poManagerContext, NULL, NULL, pbData, dw, &f_poLicenseResponse);
        #else
        dr = Drm_LicenseAcq_ProcessResponse(poManagerContext, NULL, NULL, pbData, dw, f_poLicenseResponse);
        #endif
		BDBG_MSG(("Drm_LicenseAcq_ProcessResponse Code : (0x%x) \n", dr));
    }
    else {
        BDBG_MSG(("invalid context or data"));
    }

    #ifdef __KERNEL__
    BKNI_Free(f_poLicenseResponse);
    #endif
    
	return (dr);
}

/* loads a sample response file into a DRM_BYTE buffer. Use this with
** tests to verify if the response processing function works well.
*/
DRM_RESULT
bdrm_util_load_response(DRM_WCHAR *poManagerContext, DRM_BYTE** pbData, uint32_t* plen)
{
    DRM_RESULT dr = DRM_E_FAIL;
    DRM_DWORD iIO = 0, dw;
    OEM_FILEHDL fpResponse = OEM_INVALID_HANDLE_VALUE;

    if(OEM_INVALID_HANDLE_VALUE == (fpResponse = Oem_File_Open(NULL,
                              ((DRM_WCHAR*)poManagerContext), 
                              OEM_GENERIC_READ, OEM_FILE_SHARE_READ, 
                              OEM_OPEN_EXISTING, OEM_ATTRIBUTE_NORMAL))) {
        BDBG_MSG(("bdrm input file not opened : %ls\n", 
               poManagerContext));
        goto ErrorExit;
    }
    
    Oem_File_GetSize(fpResponse, &dw);        
    dr = Oem_File_SetFilePointer(fpResponse, 0, OEM_FILE_BEGIN, NULL);

    ChkMem(*pbData = (DRM_BYTE*)Oem_MemAlloc(dw));
    if (!Oem_File_Read(fpResponse, *pbData, dw, &iIO) || iIO != dw) {
        ChkDR(DRM_E_FILEREADERROR);
    } else if (plen != NULL) {
        *plen = iIO;
        BDBG_MSG(("\nbdrm I-FILE opened, file size = %d, data=%s\n", 
               dw, *pbData));
    }

ErrorExit:
    if (fpResponse != OEM_INVALID_HANDLE_VALUE)
        Oem_File_Close (fpResponse);

    return (dr);
}

/* same as process_response, takes input and dumps output to a file.
*/
void 
bdrm_process_file_response(DRM_CONST_STRING *pwszResponseFilename, DRM_DWORD dw, 
                           DRM_CONST_STRING *pwszDataStoreFile)
{
    DRM_DWORD iIO=0;
    DRM_BYTE* pbData = NULL;
    DRM_RESULT dr = DRM_SUCCESS;    
    DRM_VOID* f_pOEMContext = NULL;

    DRM_APP_CONTEXT *poManagerContext = NULL;
    OEM_FILEHDL fpResponse = OEM_INVALID_HANDLE_VALUE;
    
    ChkMem( poManagerContext = Oem_MemAlloc( SIZEOF(DRM_APP_CONTEXT) ) );
    ZEROMEM(poManagerContext, SIZEOF(DRM_APP_CONTEXT));

    wcsncpy( (DRM_WCHAR *)poManagerContext, (const DRM_WCHAR *)pwszResponseFilename->pwszString, 
             pwszResponseFilename->cchString );
    ((DRM_WCHAR*)poManagerContext)[pwszResponseFilename->cchString] = g_wchNull;

    fpResponse = Oem_File_Open(NULL,
                              ((DRM_WCHAR*)poManagerContext), 
                              OEM_GENERIC_READ, OEM_FILE_SHARE_READ, 
                              OEM_OPEN_EXISTING, OEM_ATTRIBUTE_NORMAL);
    
	if( OEM_INVALID_HANDLE_VALUE == fpResponse ) {
        BDBG_MSG(("Input file not opened\n"));
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
    ChkMem( pbData = (DRM_BYTE*) Oem_MemAlloc( dw ) );

    if (!Oem_File_Read(fpResponse, pbData, dw, &iIO) || iIO!=dw)
        ChkDR(DRM_E_FILEREADERROR);
    
    ChkDR( Drm_LicenseAcq_ProcessAckResponse( poManagerContext, pbData, dw, NULL ) );

ErrorExit:
    Drm_Uninitialize(poManagerContext);
    if (fpResponse != OEM_INVALID_HANDLE_VALUE)
        Oem_File_Close (fpResponse);

	#ifdef PRDY_ROBUSTNESS
    if(f_pOEMContext != NULL)
        Oem_Destroy(f_pOEMContext);
	#endif

    if (pbData != NULL)
        Oem_MemFree (pbData);
    
    if (poManagerContext != NULL)
        Oem_MemFree (poManagerContext);
}



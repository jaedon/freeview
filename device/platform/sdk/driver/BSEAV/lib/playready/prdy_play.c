/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: prdy_play.c $
 * $brcm_Revision: 10 $
 * $brcm_Date: 10/4/12 10:48a $
 *
 * Module Description:
 *
 * Playready library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/playready/prdy_play.c $
 * 
 * 10   10/4/12 10:48a hugost
 * SW7425-4032: Add custom data support to challenge generation
 * 
 * 9   8/15/12 12:30p hugost
 * SW7425-3573: Moved Drm_Initialize() call to bdrm_internal_create
 * 
 * 8   7/17/12 4:13p piyushg
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
 * 7   11/10/11 8:05a hugost
 * SW7425-1625:print datastore name
 * 
 * 6   11/1/11 9:51a hugost
 * SW7425-1625: Added MP4 fragment support
 * 
 * 5   10/25/11 3:18p piyushg
 * SW7425-1607: Added file header to track revision history.
 ***************************************************************************/

#include <drmcommon.h>
#include <drmutilities.h>
#include <drmcrt.h>
#include <drmcontextsizes.h>
#include <drmmanager.h>
#include <drmcmdlnpars.h>
#include "prdy_env.h"
#ifndef __KERNEL__
#include <stdio.h>
#include <sys/time.h>
#else
#endif

#include "bstd.h"           /* BRCM includes */
#include "prdy_core.h"

#define MAX_PATH	   260
#define RATE(val,time) ((float)(val)/((float)(time)->tv_sec + (time)->tv_usec*(0.000001)))

const DRM_CONST_STRING *g_proparray[] = {               /* global rights array */
    &g_dstrWMDRM_RIGHT_PLAYBACK                         /* right to play back the stream*/
};

BDBG_MODULE(bdrm_play);                                 /* debug handle */




/* License structure to use for callback
*/
typedef struct bdrm_license {
    DRM_DWORD count;                                    /* number of items */
    DRM_LICENSE_STATE_DATA data[NO_OF(g_proparray)];    /* license items */
} bdrm_license;

/* retreives the number of active license properties
*/
uint8_t 
bdrm_license_get_count (bdrm_license_t license)
{
    uint8_t count = 0;

    if(license != NULL) {
        count = license->count;
    } else {
        BDBG_WRN(("invalid license handle"));
    }
        
    return (count);
}

/* translates MSDRM license category into internal category type
*/
static bdrm_license_type 
bdrm_license_categ_to_type(DRM_DWORD dwCategory)
{
    bdrm_license_type ret = bdrm_lictype_unknown;
    switch(dwCategory) {
        case DRM_LICENSE_STATE_NORIGHT: ret = bdrm_lictype_noright; break;
        case DRM_LICENSE_STATE_FROM: ret = bdrm_lictype_from; break;
        case DRM_LICENSE_STATE_UNTIL: ret = bdrm_lictype_until; break;
        case DRM_LICENSE_STATE_FROM_UNTIL: ret = bdrm_lictype_fromuntil; break;
        case DRM_LICENSE_STATE_COUNT: ret = bdrm_lictype_count; break;
        case DRM_LICENSE_STATE_UNLIM: ret = bdrm_lictype_unlimited; break;
    }

    return (ret);
}

/* converts from drm time to system time
*/
void
bdrm_license_set_timedate(struct tm* s_time, DRMSYSTEMTIME *st)
{
    BDBG_ASSERT(s_time != NULL); 
    BDBG_ASSERT(st != NULL);

    if ((s_time != NULL) && (st != NULL)) {
        s_time->tm_year = st->wYear - 1900;     /* struct tm expects yr - 1900 */
        s_time->tm_mon  = st->wMonth - 1;       /* struct tm expects [0-11] */
        s_time->tm_mday = st->wDay;
        s_time->tm_hour = st->wHour;
        s_time->tm_min  = st->wMinute;
        s_time->tm_sec  = st->wSecond;
    }    
}

/* retreives one active license property at a time
*/
void 
bdrm_license_get_item (bdrm_license_t license, uint8_t index, bdrm_license_info* lic_info)
{
    DRM_DWORD date_index, count_index;
    DRMSYSTEMTIME st; DRM_LICENSE_STATE_DATA* lic_data = NULL;
    
    BDBG_ASSERT(license != NULL); BDBG_ASSERT(lic_info != NULL);
    if((license != NULL) && (lic_info != NULL)) {

        lic_data = license->data;
        lic_info->type = bdrm_license_categ_to_type(lic_data[index].dwCategory);
        lic_info->stream_id = lic_data[index].dwStreamId;
        lic_info->times_count = lic_data[index].dwNumDates; 

        if (0 != lic_data[index].dwNumDates) {
            for(date_index = 0; date_index < lic_data[index].dwNumDates; date_index++) {
                Oem_Clock_FileTimeToSystemTime(&(lic_data[index].datetime[date_index]), &st);
                bdrm_license_set_timedate(&(lic_info->times[date_index]), &st);
            }
        }

        lic_info->plays_count = lic_data[index].dwNumCounts;
        if (0 != lic_data[index].dwNumCounts) {
            for(count_index = 0; count_index < lic_data[index].dwNumCounts; count_index++) {
                lic_info->plays[count_index] = lic_data[index].dwCount[count_index];
            }
        }
    }
}

/* prints detailed license description for license rights
*/
DRM_RESULT 
drm_play_print_license( DRM_CHAR* szOutputPrefix, DRM_LICENSE_STATE_DATA* pDRMLicenseStateData )
{
    DRM_RESULT hr = DRM_SUCCESS;
    DRM_DWORD dwCountIndex;
    DRM_DWORD dwDateIndex;

#if !(BDBG_DEBUG_BUILD)
    BSTD_UNUSED(szOutputPrefix);
#endif

    do {
        /* print the type of right given by aggregating all the licenses for the content
        */
        BDBG_MSG((("%sDRM_LICENSE_DATA.dwStreamId: %ld"), szOutputPrefix, pDRMLicenseStateData->dwStreamId));
        switch(pDRMLicenseStateData->dwCategory) {
        case DRM_LICENSE_STATE_NORIGHT:          /* no rights to perform this action */
            BDBG_MSG((("%sDRM_LICENSE_DATA.dwCategory: WM_DRM_LICENSE_STATE_NORIGHT"), szOutputPrefix)); break;
        case DRM_LICENSE_STATE_UNLIM:            /* unlimited rights to perform this action */
            BDBG_MSG((("%sDRM_LICENSE_DATA.dwCategory: WM_DRM_LICENSE_STATE_UNLIM"), szOutputPrefix)); break;    
        case DRM_LICENSE_STATE_COUNT:            /* action may only be performed a certain number of times */
            BDBG_MSG((("%sDRM_LICENSE_DATA.dwCategory: WM_DRM_LICENSE_STATE_COUNT"), szOutputPrefix)); break;    
        case DRM_LICENSE_STATE_FROM:             /* action cannot be performed until a specific date */
            BDBG_MSG((("%sDRM_LICENSE_DATA.dwCategory: WM_DRM_LICENSE_STATE_FROM"), szOutputPrefix)); break;    
        case DRM_LICENSE_STATE_UNTIL:            /* action cannot be performed after a certain date */
            BDBG_MSG((("%sDRM_LICENSE_DATA.dwCategory: WM_DRM_LICENSE_STATE_UNTIL"), szOutputPrefix)); break;    
        case DRM_LICENSE_STATE_FROM_UNTIL:       /* action can only be performed within a specific range of dates */
            BDBG_MSG((("%sDRM_LICENSE_DATA.dwCategory: WM_DRM_LICENSE_STATE_FROM_UNTIL"), szOutputPrefix)); break;    
        case DRM_LICENSE_STATE_COUNT_FROM:       /* action can only be performed a certain number of times, starting from a specific date */
            BDBG_MSG((("%sDRM_LICENSE_DATA.dwCategory: WM_DRM_LICENSE_STATE_COUNT_FROM"), szOutputPrefix)); break;    
        case DRM_LICENSE_STATE_COUNT_UNTIL:      /* action can be performed a certain number of times until a specific date */
            BDBG_MSG((("%sDRM_LICENSE_DATA.dwCategory: WM_DRM_LICENSE_STATE_COUNT_UNTIL"), szOutputPrefix)); break;    
        case DRM_LICENSE_STATE_COUNT_FROM_UNTIL: /* action can only be performed a certain number of times, and only is a specific range of dates */
            BDBG_MSG((("%sDRM_LICENSE_DATA.dwCategory: WM_DRM_LICENSE_STATE_COUNT_FROM_UNTIL"), szOutputPrefix)); break;
        case DRM_LICENSE_STATE_EXPIRATION_AFTER_FIRSTUSE:/* license restrictions don't occur until after the first use */
            BDBG_MSG((("%sDRM_LICENSE_DATA.dwCategory: WM_DRM_LICENSE_STATE_EXPIRATION_AFTER_FIRSTUSE"), szOutputPrefix)); break;            
        default:
            BDBG_MSG((("%sDRM_LICENSE_DATA.dwCategory: Unknown! - %d"), szOutputPrefix, pDRMLicenseStateData->dwCategory));
        }
        
        /* if count limited, print the number of times the action can be performed
        */
        if (0 != pDRMLicenseStateData->dwNumCounts) {
            BDBG_MSG((("%sDRM_LICENSE_DATA.dwCount:"), szOutputPrefix));
            for(dwCountIndex = 0; dwCountIndex < pDRMLicenseStateData->dwNumCounts; dwCountIndex++) {
                BDBG_MSG((("  %ld"), pDRMLicenseStateData->dwCount[dwCountIndex]));
            }
            BDBG_MSG((("\n")));
        }
        
        /* if the action is date limited, print the date restriction(s)
        */
        if (0 != pDRMLicenseStateData->dwNumDates) {
            BDBG_MSG((("%sDRM_LICENSE_DATA.datetime:"), szOutputPrefix));
            for( dwDateIndex = 0; dwDateIndex < pDRMLicenseStateData->dwNumDates; dwDateIndex++) {
                DRMSYSTEMTIME  st;
                Oem_Clock_FileTimeToSystemTime(&(pDRMLicenseStateData->datetime[dwDateIndex]), &st);
                BDBG_MSG((("  On %04d/%02d/%02d at %02d:%02d:%02d.%03d"), st.wYear,
                    st.wMonth, st.wDay,
                    st.wHour, st.wMinute,
                    st.wSecond, st.wMilliseconds));
            }
            BDBG_MSG((("\n")));
        }

        /* if the aggregate license data cannot easily be represented, the "vague" value will be set.
        ** This will happen when two or more licenses with different right types (like COUNT and
        ** and FROM_UNTIL) make it impossible to represent the information simply.  For instance,
        ** if license 1 allows 5 plays and license 2 allows unlimited playback during the current
        ** month, then the user is guaranteed at least 5 plays, but possibly an unlimited number,
        ** if done within the current month.
        */
        BDBG_MSG((("%sDRM_LICENSE_DATA.dwVague: %ld "), szOutputPrefix, pDRMLicenseStateData->dwVague));
    }
    while (FALSE);
    
    return hr;
}

/* takes license properties and feeds them to an explicit print function
*/
static void
bdrm_fill_license(const DRM_CONST_STRING** proparr, DRM_DWORD prop_len, bdrm_license* license)
{
	DRM_DWORD dw;
    BDBG_MSG((("  Content rights information:")));

    BDBG_ASSERT(license != NULL);
    if(license != NULL) license->count = NO_OF(proparr);

	for (dw = 0; dw < prop_len ; dw++) {
		BDBG_MSG((( "    %ls :" ), proparr[dw]->pwszString));
		drm_play_print_license((DRM_CHAR *)("      "), &(license->data[dw]));
	}
}

/* called from the drmcore, to destroy the live license instance
*/
DRM_RESULT
bdrm_destroy_license(bdrm_license_t license)
{
    DRM_RESULT res = 0;

    BDBG_ASSERT(license);
    if(license != NULL) {
        MEMSET(license, 0, SIZEOF(bdrm_license));
        Oem_MemFree(license);
    }

    return (res);
}

/* called from the ASF parser to start grabbing of the license
*/
DRM_RESULT 
bdrm_lookup_license(DRM_BYTE* pbXML, DRM_DWORD cbXML,
                    DRM_APP_CONTEXT* pAppContext, 
                    DRM_DECRYPT_CONTEXT** outDecryptCtx,
                    bdrm_license_t* license,
                    DRMPFNPOLICYCALLBACK policy_cback,
                    void * policy_ctx) 
{
    const DRM_CONST_STRING *apdcsRights [1];

/*Playready */

    DRM_RESULT dr = DRM_SUCCESS; DRM_DECRYPT_CONTEXT* poDecryptContext = NULL;
    bdrm_license* pLicense = NULL;

    BDBG_MSG(("%s - entering", __FUNCTION__));

    if( (*outDecryptCtx) == NULL){
        
        ChkMem(poDecryptContext = Oem_MemAlloc(SIZEOF(DRM_DECRYPT_CONTEXT)));
        ZEROMEM(poDecryptContext, SIZEOF(DRM_DECRYPT_CONTEXT));
    }
    else {
        
        poDecryptContext = (* outDecryptCtx);
    }
    ChkMem(pLicense = Oem_MemAlloc(SIZEOF(bdrm_license)));            /* alloc a new license */
    ZEROMEM(pLicense, SIZEOF (bdrm_license));

    /*Playready */
    dr = Drm_Content_SetProperty(pAppContext, DRM_CSP_AUTODETECT_HEADER, pbXML, cbXML);     /* set header info */
    if(dr == DRM_E_HEADER_ALREADY_SET){
        Drm_Reinitialize(pAppContext);
        ChkDR(Drm_Content_SetProperty(pAppContext, DRM_CSP_AUTODETECT_HEADER, pbXML, cbXML));     /* set header info */
    }
    else {
        ChkDR(dr);
    }

    ChkDR(Drm_LicenseQuery_GetState(pAppContext, g_proparray, NO_OF(g_proparray), pLicense->data, policy_cback, policy_ctx));

#if 0
    BDBG_ERR(("\n<<STATE DATA %d %d %d %d %d>>>>>>\n", pLicense->data->dwStreamId, pLicense->data->dwCategory, pLicense->data->dwNumCounts, pLicense->data->dwCount[0], pLicense->data->dwNumDates));
#endif
    bdrm_fill_license(g_proparray, NO_OF(g_proparray), pLicense);   /* fill request array */

    if (license != NULL) *license = pLicense;                       /* copying license context out */

	apdcsRights [0] = (DRM_CONST_STRING *) &g_dstrWMDRM_RIGHT_PLAYBACK;

	dr = Drm_Reader_Bind (pAppContext, 
					   apdcsRights, NO_OF (apdcsRights),            /* bind to the license and extract context */
					   policy_cback, policy_ctx, poDecryptContext);

	if (dr == DRM_E_LICENSENOTFOUND) {                              /* could not find a license for the KID */
            BDBG_WRN(("no licenses found"));
            goto ErrorExit;
	} else if (DRM_FAILED( dr )) {                                  /* license binding failed */
            BDBG_ERR(("unexpected failure during bind. 0x%x", dr));
            goto ErrorExit;
	} else if (outDecryptCtx != NULL) {
            BDBG_WRN(("license found, decrypt context initialized"));
            *outDecryptCtx = poDecryptContext;                          /* return crypto context to drm core */
	}
	/* Decrypt and play content */
	ChkDR( Drm_Reader_Commit( pAppContext, policy_cback, policy_ctx ) );                    /* commit license changes to the store */

ErrorExit:    
    /* we should not kill the context here
    */

    BDBG_MSG(("%s - exiting dr %x", __FUNCTION__, dr));
    return(dr);
}


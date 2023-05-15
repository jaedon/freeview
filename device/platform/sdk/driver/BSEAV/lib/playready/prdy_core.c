/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: prdy_core.c $
 * $brcm_Revision: 17 $
 * $brcm_Date: 10/15/12 3:13p $
 *
 * Module Description:
 *
 * Playready library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/playready/prdy_core.c $
 * 
 * 17   10/15/12 3:13p hugost
 * SW7425-4020: Pass id to Oem_Create
 * 
 * 16   10/4/12 10:48a hugost
 * SW7425-4032: Add custom data support to challenge generation
 * 
 * 15   8/15/12 12:30p hugost
 * SW7425-3573: Moved Drm_Initialize() call to bdrm_internal_create
 * 
 * 14   7/19/12 1:56p piyushg
 * SW7231-909: Code cleanup. Fix memory leak issues.
 * 
 * 13   7/17/12 4:13p piyushg
 * SW7425-3281: Add playready kernel mode support and nexus drm
 * implementation.
 * 
 * 12   6/5/12 3:02p hugost
 * SW7425-3169: Modified playready to use new version of common crypto
 * library
 * 
 * 11   2/3/12 12:09p piyushg
 * SW7429-72: Revert previous chnages.
 * 
 * 10   11/22/11 7:49a hugost
 * SW7425-1827: Fixed issue in bdrm_load_license().
 * 
 * 9   11/4/11 3:53p katrep
 * SW7231-317:fixed segfault issue @ close
 * 
 * 8   11/1/11 9:51a hugost
 * SW7425-1625: Added MP4 fragment support
 * 
 * 7   10/25/11 3:18p piyushg
 * SW7425-1607: Added file header to track revision history.
 ***************************************************************************/

#ifndef __KERNEL__
#include <stdio.h>         /*  std includes */
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#else
#include <generated/autoconf.h>
#include <linux/string.h>
#endif


#include "bstd.h"           /* brcm includes */
#include "bkni.h"
#include "blst_list.h"
#include "bfifo.h"

#include "prdy_core.h"        /* drmcore includes */
#include "prdy_priv.h" 
#include "prdy_play.h"
#include "prdy_challenge.h"
#include "prdy_response.h"
#include "prdy_decryptor.h"
#include "prdy_asf.h"
#include "prdy_mp4.h"
#include "prdy_http.h"
#include "prdy_env.h"

#ifdef PRDY_ROBUSTNESS
#include "prdy_secure_clk.h"
#include "oem_brcm.h"
#include "oemcommon.h"
#endif

#if NEXUS_DRM_BUILD

/* Handle to a Nexus DMA channel.
 * Needed by the license security code.
 * Set by calling bdrm_set_dma_handle().
 */
static NEXUS_DmaHandle g_hDmaChannel = NULL;

#endif



/* context passed on to the grabber external function
*/
typedef struct bdrm_transport_ctx {
    bdrm_t drm;                         /* drm context */
    bdrm_license_t license;             /* license context */
} bdrm_transport_ctx;

#define CH_DEFAULT_NON_QUIET    (1)     /* default non-quiet on */
#define CH_DEFAULT_APP_SEC      (150)   /* default app security test-level */

BDBG_MODULE(bdrm);                              
#define BDBG_MSG_TRACE(x)                       
#ifdef PROFILE
    #undef BDBG_MSG
    #define BDBG_MSG(x)
#endif


#ifdef PRDY_ROBUSTNESS
#if 0
static DRM_RESULT bdrm_get_file_path_str(bdrm_file_id_e id, DRM_WCHAR* path, uint32_t buf_size);
#endif

#if 0
/*!+!hla bdrm_erase_uname_hds_files() will not work in kernel do this op in nexus wmdrm module?*/
/* revoke the wmdrmpd keys and dev cert, dev cert template, etc.
*/
static bdrm_err bdrm_erase_uname_hds_files(void);
#endif

/* Creates a char * string representing the datastore file path given the 'user_name' from the calling application
*/
static bdrm_err bdrm_create_uname_hds_file_path_str (char *buf, unsigned int buf_size, char *user_name);
#endif

/* default callback function for license grabber; this should not be used in
** a real scenario. provided only as an example.
*/
bdrm_err bdrm_util_default_extern (
    const bdrm_license_challenge* challenge,    /* [in] challenge - default challenge to be sent */
    bdrm_license_response* response,            /* [out] server response */ 
    const void* ctx                             /* [in] custom context */
);

/* default callback function for the license state notification
*/
bdrm_err bdrm_util_default_callback (
    bdrm_acq_state* lic_acq_state,              /* [in] license acquisition state */
    bdrm_license_t license,                     /* [in] license info - carries extra license data */
    const void* ctx                             /* [in] custom context */
);

#ifdef PRDY_ROBUSTNESS
void bdrm_read_otp_id_priv(NEXUS_ReadOtpIO *pOtpid)
{
    NEXUS_Error rc;

    BDBG_ASSERT(pOtpid != NULL);

    BKNI_Memset(pOtpid, 0, sizeof(NEXUS_ReadOtpIO));

#if (BCHP_CHIP != 7335 && BCHP_CHIP != 7125)
    rc = NEXUS_Security_ReadOTP(NEXUS_OtpCmdReadRegister_eKeyID, NEXUS_OtpKeyType_eA, pOtpid);
#else
    rc = NEXUS_Security_ReadOTP(NEXUS_OtpCmdReadRegister_eIdAWord, pOtpid);
#endif

    if(rc != NEXUS_SUCCESS)
    {
        BDBG_ERR(("NEXUS_Security_ReadOTP failed %x ", rc)); 
    }

    return;
}
#endif

/* verifies the crypto and manager contexts for validity
*/
bdrm_err 
bdrm_validate_ctx (bdrm_t drm) 
{
    bdrm_err ret = bdrm_err_fail;
    BDBG_ASSERT(drm != NULL);
    BDBG_MSG(("%s: Entering.\n", __FUNCTION__));

    if ((drm != NULL) &&
    (drm->poDecryptCtx != NULL) /* ~RY &&
    (drm->pAppContext != NULL)*/) {
        ret = bdrm_err_ok;
    }

    BDBG_MSG(("%s: Exiting, ret %d.\n", __FUNCTION__, ret));
    return(ret);
}

bdrm_err
bdrm_cleanup_license_store(bdrm_t drm)
{
    bdrm_err rc = bdrm_err_ok;
    BDBG_ASSERT(drm != NULL);

    if(Drm_StoreMgmt_CleanupStore(drm->pAppContext, NULL, 5, NULL)!= DRM_SUCCESS){
        rc = bdrm_err_fail;
    }

    BDBG_MSG(("%s: Exiting, rc %d.\n", __FUNCTION__, rc));
    return rc;
}


/* called used to repackage output protection levels to brcm structures
   and return the info the the user app. This callback is called each
   time a license with output levels is processed by the MS porting kit.
   It's register to the MS porting kit thru DRM_MGR_Bind().
*/

DRM_RESULT DRM_API bdrm_policy_callback(
    IN const DRM_VOID  *f_pvCallbackData,
    IN       DRM_DWORD  f_dwCallbackType,
    IN const DRM_VOID  *f_pv )
{
    bdrm_t drm = (bdrm_t)f_pv;
    bdrm_policy_t  policy;
    DRM_RESULT rc = DRM_SUCCESS;
    BDBG_MSG(("%s - entering", __FUNCTION__));

    BDBG_ASSERT(f_pvCallbackData != NULL);
    BDBG_ASSERT(f_pv != NULL);

    policy.type = f_dwCallbackType;

    switch(policy.type){
        case PLAY_OPL:
            policy.t.play.i_compressedDigitalVideo   = (uint32_t)((DRM_PLAY_OPL *)f_pvCallbackData)->minOPL.wCompressedDigitalVideo;
            policy.t.play.i_uncompressedDigitalVideo = (uint32_t)((DRM_PLAY_OPL *)f_pvCallbackData)->minOPL.wUncompressedDigitalVideo;
            policy.t.play.i_analogVideo              = (uint32_t)((DRM_PLAY_OPL *)f_pvCallbackData)->minOPL.wAnalogVideo;
            policy.t.play.i_compressedDigitalAudio   = (uint32_t)((DRM_PLAY_OPL *)f_pvCallbackData)->minOPL.wCompressedDigitalAudio;
            policy.t.play.i_uncompressedDigitalAudio = (uint32_t)((DRM_PLAY_OPL *)f_pvCallbackData)->minOPL.wUncompressedDigitalAudio;

            policy.t.play.i_resv_cIds  = (uint32_t)((DRM_PLAY_OPL *)f_pvCallbackData)->oplIdReserved.cIds;
            policy.t.play.i_resv_rgIds = (bdrm_guid_t *)((DRM_PLAY_OPL *)f_pvCallbackData)->oplIdReserved.rgIds;

            policy.t.play.i_vop_cEntries  = (uint32_t)((DRM_PLAY_OPL *)f_pvCallbackData)->vopi.cEntries;
            policy.t.play.i_vop           = (bdrm_video_out_protection_t *)((DRM_PLAY_OPL *)f_pvCallbackData)->vopi.rgVop;
            break;
        case COPY_OPL:
            policy.t.copy.i_minimumCopyLevel = (uint16_t)((DRM_COPY_OPL *)f_pvCallbackData)->wMinimumCopyLevel;
            policy.t.copy.i_includes_cIds    = (uint16_t)((DRM_COPY_OPL *)f_pvCallbackData)->oplIdIncludes.cIds;
            policy.t.copy.i_includes_rgIds   = (bdrm_guid_t *)((DRM_COPY_OPL *)f_pvCallbackData)->oplIdIncludes.rgIds;
            policy.t.copy.i_excludes_cIds    = (uint16_t)((DRM_COPY_OPL *)f_pvCallbackData)->oplIdExcludes.cIds;
            policy.t.copy.i_excludes_rgIds   = (bdrm_guid_t *)((DRM_COPY_OPL *)f_pvCallbackData)->oplIdExcludes.rgIds;
            break;
        case INCLUSION_LIST:
            BKNI_Memcpy(&policy.t.inc_list, f_pvCallbackData, sizeof(DRM_INCLUSION_LIST_CALLBACK_STRUCT));
            break;
        case EXTENDED_RESTRICTION_CONDITION:
            BKNI_Memcpy(&policy.t.condition, f_pvCallbackData, sizeof(DRM_EXTENDED_RESTRICTION_CALLBACK_STRUCT));
            break;
        case EXTENDED_RESTRICTION_ACTION:
            BKNI_Memcpy(&policy.t.action, f_pvCallbackData, sizeof(DRM_EXTENDED_RESTRICTION_CALLBACK_STRUCT));
            break;
        case EXTENDED_RESTRICTION_QUERY:
            BKNI_Memcpy(&policy.t.query, f_pvCallbackData, sizeof(DRM_EXTENDED_RESTRICTION_CALLBACK_STRUCT));
            break;
        case SECURE_STATE_TOKEN_RESOLVE:
            BKNI_Memcpy(&policy.t.token_res, f_pvCallbackData, sizeof(DRM_SECURE_STATE_TOKEN_RESOLVE_DATA));
            break;
        default:
            /* Unsupported operation */
            rc = DRM_E_RIGHTSNOTAVAILABLE;
            break;
    }

    if(rc == DRM_SUCCESS){
        if(drm->cfg->opl_cback(&policy, drm->cfg->opl_ctx) != bdrm_err_ok){
            rc = DRM_E_RIGHTSNOTAVAILABLE;
        }
    }

    BDBG_MSG(("%s: Exiting, rc %d.\n", __FUNCTION__, rc));
    return rc;
}

/* called on every header object callback
*/
bdrm_err
bdrm_content_handler(bdrm_t drm)
{
    bdrm_err ret = bdrm_err_ok;
    BDBG_MSG(("%s - entering", __FUNCTION__));

    if (bdrm_err_ok == bdrm_handle_drm(drm)) { /* Handle drm processing here */
        if (bdrm_validate_ctx(drm) == bdrm_err_ok) {
            bdrm_decryptor_assign(drm->dcrypt, drm);
        }
    }
    BDBG_MSG(("%s: Exiting, ret %d.\n", __FUNCTION__, ret));
    return(ret);
}


/* Called to determine if drm exists in this asf. Now read from the header list enum.
** Will be implemented in the same means as the asfhdr lib.
*/
bdrm_encr_state 
bdrm_is_drmencrypted (bdrm_t drm)                                  
{
    bdrm_encr_state ret = bdrm_encr_none;
    BDBG_MSG(("%s - entering", __FUNCTION__));

    if (drm != NULL) {
        ret = drm->cnt_fp.is_drmencrypted(drm);
    }

    BDBG_MSG(("%s: Exiting, ret %d.\n", __FUNCTION__, ret));
    return(ret);
}   

/* Helper func used to call back on license state change notifications,
   including the http state
*/
static bdrm_err
bdrm_change_lic_http_state (bdrm_t drm, bdrm_license_state lic_state, bdrm_license_state http_state, 
                            bdrm_license_t license)
{
    bdrm_err ret = bdrm_err_fail;
    bdrm_acq_state lic_acq_state;
    BDBG_MSG(("%s - entering", __FUNCTION__));

    BDBG_ASSERT(drm != NULL);
    if (drm != NULL) {
        drm->lic_state = lic_state;
        if ((drm->cfg != NULL) && (drm->cfg->lic_callback != NULL)) {
            lic_acq_state.license_state = lic_state;
            lic_acq_state.http_state = http_state;
            drm->cfg->lic_callback(&lic_acq_state, license, drm->cfg->lic_callback_ctx);
        }
    }

    BDBG_MSG(("%s: Exiting, ret %d.\n", __FUNCTION__, ret));
    return(ret);
}

/* Helper func used to call back on license state change notifications
*/
static bdrm_err
bdrm_change_lic_state (bdrm_t drm, bdrm_license_state lic_state, bdrm_license_t license)
{
    return bdrm_change_lic_http_state (drm, lic_state, bdrm_http_status_undefined, license);
}

/* Create a valid instance of the drm context
*/
static bdrm_err 
bdrm_internal_create (bdrm_t* pdrm, bdrm_cfg_t cfg)
{
    bdrm_err ret = bdrm_err_fail;
    bdrm_decryptor_t dcrypt = NULL;
    DRM_CONST_STRING pdstrDataStoreFile;
    DRM_RESULT dr;

#ifdef PRDY_ROBUSTNESS
    NEXUS_ReadOtpIO  otp_id;
    DRM_BOOL revoke;
    DRM_CONST_STRING pwszDataStore;
    DRM_CHAR  user_buf[DRM_MAX_PATH] = {0x00};
    DRM_CHAR  default_buf[DRM_MAX_PATH] = {0x00};
#endif

 #if DRM_SUPPORT_REVOCATION
    DRM_BYTE *pbRevocationBuffer = NULL;
#endif
    BDBG_ASSERT(pdrm != NULL);        
    BDBG_MSG(("%s - entering", __FUNCTION__));

#if (BDBG_DEBUG_BUILD)
    BDBG_SetModuleLevel("bdrm", BDBG_eWrn);
    BDBG_SetModuleLevel("bdrm_asf", BDBG_eWrn);
    BDBG_SetModuleLevel("bdrm_challenge", BDBG_eWrn);
    BDBG_SetModuleLevel("bdrm_content_decryptor", BDBG_eWrn);
    BDBG_SetModuleLevel("bdrm_http", BDBG_eMsg);
    BDBG_SetModuleLevel("bdrm_mp4", BDBG_eWrn);
    BDBG_SetModuleLevel("bdrm_play", BDBG_eWrn);
    BDBG_SetModuleLevel("bdrm_response", BDBG_eWrn);
    BDBG_SetModuleLevel("bdrm_secureclock", BDBG_eWrn);
#endif

    if (pdrm != NULL) {
        *pdrm = (bdrm_t)BKNI_Malloc(sizeof(struct bdrm_context));
        BKNI_Memset((void*)*pdrm,0,sizeof(struct bdrm_context));

        
        if (*pdrm != NULL) {

            switch(cfg->cnt_type){
                case bdrm_cnt_asf:
                    bdrm_init_asf_container((*pdrm));
                    break;
                case bdrm_cnt_mp4:
                    bdrm_init_mp4_container((*pdrm));
                    break;
                default:
                    BDBG_ERR(("%s - Container not supported.\n", __FUNCTION__));
                    goto ErrorExit;
                    break;
            }

            (*pdrm)->poDecryptCtx = NULL;   /* fill context members */

            (*pdrm)->cfg = (bdrm_cfg_t)BKNI_Malloc(sizeof(bdrm_cfg));
            if( (*pdrm)->cfg == NULL )
            {
                BDBG_ERR(("drm cfg alloc failed"));
                return bdrm_err_fail;
            }

            BKNI_Memcpy((*pdrm)->cfg, cfg, sizeof(bdrm_cfg));

#ifdef PRDY_ROBUSTNESS


            bdrm_read_otp_id_priv(&otp_id);

            (*pdrm)->f_pOEMContext = Oem_Create(&otp_id);
            if((*pdrm)->f_pOEMContext == NULL){
                BDBG_ERR(("%s - Oem_Create() failed\n", __FUNCTION__));
                goto ErrorExit;
            }

            if(Oem_Device_GetRevocationStatus((*pdrm)->f_pOEMContext, &revoke) != DRM_SUCCESS){
                BDBG_ERR(("%s - Oem_Device_GetRevocationStatus() failed. \n", __FUNCTION__));
                goto ErrorExit;
            }

            bdrm_env_default_datastore(&pwszDataStore);
            /* transform the URL from wide-char to ANSI */
            DRM_UTL_DemoteUNICODEtoASCII(pwszDataStore.pwszString, default_buf, pwszDataStore.cchString);
            if(cfg->user_data_store_file_path[0] != 0x00)
            {
                if(strcmp(cfg->user_data_store_file_path, (const char *)default_buf) != 0){
                    bdrm_create_uname_hds_file_path_str((char *)user_buf, sizeof(user_buf), cfg->user_data_store_file_path);
                    bdrm_env_set_curr_datastore( (DRM_VOID *)user_buf, 1);
                }else {
                    BDBG_MSG(("bdrm_open - DEFAULT CASE...\n"));      
                    BDBG_MSG(("bdrm_open - cfg->user_data_store_file_path(default) = '%s'\n", cfg->user_data_store_file_path));
                    bdrm_env_set_curr_datastore( (DRM_VOID *)&pwszDataStore, 0);
                }
            }
            else /*use default datastore*/
            {
                BDBG_MSG(("bdrm_open - DEFAULT CASE...\n"));      
                BDBG_MSG(("bdrm_open - cfg->user_data_store_file_path(default) = '%s'\n", cfg->user_data_store_file_path));
                bdrm_env_set_curr_datastore( (DRM_VOID *)&pwszDataStore, 0);
            }


            /* now the 'tmp' is copied, free the data */
            BDBG_MSG(("bdrm_open - User data store filepath = '%s'\n", cfg->user_data_store_file_path));
            bdrm_env_get_curr_datastore(&pdstrDataStoreFile);         /* read the datastore file from the env. */


            if(revoke){
                BDBG_ERR(("%s - Keys have been revoke.\n", __FUNCTION__));
#if 0
/*!+!hla bdrm_erase_uname_hds_files() will not work in kernel do this op in nexus wmdrm module?*/
/* revoke the wmdrmpd keys and dev cert, dev cert template, etc.
*/
                bdrm_wmdrm_srm_revoke();
#endif
            }
#else
            bdrm_env_datastore(&pdstrDataStoreFile);                        /* read the datastore file from the env. */
            (*pdrm)->f_pOEMContext = NULL;
#endif
 
   

            #if 0
            {
                int count = 0;
                while ((DRM_DWORD)count < pdstrDataStoreFile.cchString) {
                    printf("%c", pdstrDataStoreFile.pwszString[count++]);
                }
                printf("\n");
            }
            #endif

            ChkMem((*pdrm)->pAppContext = Oem_MemHeapAlloc(SIZEOF(DRM_APP_CONTEXT)));
            ZEROMEM((*pdrm)->pAppContext, SIZEOF(DRM_APP_CONTEXT));

            dr = Drm_Initialize((*pdrm)->pAppContext, (*pdrm)->f_pOEMContext, &pdstrDataStoreFile);
            if(dr == DRM_E_DEVCERTREADERROR){
                BDBG_ERR(("%s - DRM_E_DEVCERTREADERROR\n", __FUNCTION__));
            }
            else if(dr == DRM_E_MACHINEIDMISMATCH){
                BDBG_ERR(("%s - DRM_E_MACHINEIDMISMATCH\n", __FUNCTION__));
            }
            else if (dr == DRM_E_INVALIDDEVICECERTIFICATE){
                BDBG_ERR(("%s - DRM_E_INVALIDDEVICECERTIFICATE\n", __FUNCTION__));
            }
            if (DRM_FAILED( dr )){
                ChkDR(dr);
            }

#if DRM_SUPPORT_REVOCATION
            BDBG_MSG(("\nDRM_MGR_ProcessLicenseResponse with : Revocation Supported\n"));
            pbRevocationBuffer = ( DRM_BYTE * )Oem_MemAlloc( REVOCATION_BUFFER_SIZE );
            Drm_Revocation_SetBuffer((*pdrm)->pAppContext, pbRevocationBuffer, REVOCATION_BUFFER_SIZE );
#endif

            bdrm_decryptor_create(&dcrypt); BDBG_ASSERT(dcrypt != NULL);    
            if (dcrypt != NULL) {           /* check for successful creation */
                (*pdrm)->dcrypt = dcrypt;
                ret = bdrm_err_ok;
            } else {                        /* free and null-ify the main context */
                BDBG_ERR(("%s - bdrm_decryptor_create() failed.\n", __FUNCTION__));
                goto ErrorExit;
            }
            BDBG_MSG(("%s -%d\n", __FUNCTION__, __LINE__));
        }
    }

    BDBG_MSG(("%s: Exiting, ret %d.\n", __FUNCTION__, ret));
    return(ret);

ErrorExit:
    if(pbRevocationBuffer != NULL) Oem_MemFree( pbRevocationBuffer);
    if((*pdrm)->pAppContext != NULL){
        Drm_Uninitialize((*pdrm)->pAppContext);
        Oem_MemHeapFree((*pdrm)->pAppContext);
    }
#ifdef PRDY_ROBUSTNESS
    if((*pdrm)->f_pOEMContext) Oem_Destroy((*pdrm)->f_pOEMContext);
#endif
    if((*pdrm)) BKNI_Free(*pdrm);
    (*pdrm) = NULL;

    BDBG_MSG(("%s: Exiting, ret %d.\n", __FUNCTION__, ret));
    return(ret);
}

/* Init default drm core function. Called to install available
** header hooks and start up objects.
*/
bdrm_t 
bdrm_open (const char* file, bdrm_cfg* cfg)
{
 
    bdrm_t drm=0;
    bdrm_err ret = bdrm_err_fail;   


    BDBG_ASSERT(cfg != NULL);
    BDBG_MSG(("%s - entering", __FUNCTION__));


    if (cfg != NULL && (bdrm_err_ok == (ret = bdrm_internal_create(&drm, cfg)))) 
    {
        drm->ctx_state = bdrm_ctx_init;
        if (cfg->lic_extern == NULL) { 
            drm->cfg->lic_extern = bdrm_util_default_extern;
            drm->cfg->transport = bdrm_default_transport;
        } else {
            drm->cfg->transport = bdrm_external_transport; 
        }
        if (cfg->lic_callback == NULL){
           drm->cfg->lic_callback = bdrm_util_default_callback;
        }

        if(cfg->cnt_type == bdrm_cnt_asf) {
            if (drm->cfg->ext_asf_scanning) {
                drm->cfg->asf = NULL;
            }
            else {
#ifdef DRM_SCAN_ASF
                drm->asfp = basf_open_asf (file,  basf_hdr_ctx_custom, &cfg->asf);
#endif
            }

        }
        ret = drm->cnt_fp.reset_headers(drm);
    }

    BDBG_MSG(("%s: Exiting, drm %p.\n", __FUNCTION__, (void *)drm));
    return(drm);

}

/* Status callback func install
*/
void 
bdrm_install_lic_callback (bdrm_t ctx, fn_license_callback pfn_lic_callback, void* params)
{
    BDBG_ASSERT(ctx != NULL);
    BDBG_MSG(("%s - entering", __FUNCTION__));

    if (ctx != NULL) {
        ctx->lic_callback = pfn_lic_callback;
        ctx->lic_callback_ctx = params;
    }
    BDBG_MSG(("%s - exiting", __FUNCTION__));
}

/* External handling of license traffic func install
*/
void 
bdrm_install_lic_extern (bdrm_t ctx, fn_license_extern pfn_lic_extern, void* params)
{
    BDBG_ASSERT(ctx != NULL);
    BDBG_MSG(("%s - entering", __FUNCTION__));

    if (ctx != NULL) {
        ctx->lic_extern = pfn_lic_extern;
        ctx->lic_extern_ctx = params;
    }
    BDBG_MSG(("%s - exiting", __FUNCTION__));
}

/* Look for license in local store, if found keep it, if not, call external
*/
bdrm_err
bdrm_acquire_license (basf_parser_t asf)
{
    bdrm_err ret = bdrm_err_ok;
    bdrm_license_challenge lic_challenge;
    bdrm_license_response* lic_response;
    BDBG_MSG(("%s - entering", __FUNCTION__));

    BSTD_UNUSED(lic_challenge); BSTD_UNUSED(lic_response);
    BSTD_UNUSED(asf);
    
    BDBG_MSG(("%s: Exiting, ret %d.\n", __FUNCTION__, ret));
    return(ret);
}

/* Cleanup function. Called externally from the same app
** that starts the drm core, just before closing.
*/
bdrm_err
bdrm_close (bdrm_t drm)
{
    bdrm_err ret = bdrm_err_ok;

#if DRM_SUPPORT_REVOCATION
    DRM_BYTE        *pbRevocationBuffer = NULL;
    DRM_DWORD        cbRevocationBuffer;
#endif

    BDBG_MSG(("%s - entering", __FUNCTION__));
    BDBG_ASSERT(drm != NULL);

    if (drm != NULL) {
        drm->ctx_state = bdrm_ctx_finished;
        if(drm->dcrypt != NULL)
        {
            bdrm_decryptor_destroy(drm->dcrypt);
            drm->dcrypt = NULL;
        }

#ifdef PRDY_ROBUSTNESS
        if(drm->f_pOEMContext != NULL) Oem_Destroy(drm->f_pOEMContext);
#endif

#if DRM_SUPPORT_REVOCATION
        Drm_Revocation_GetBuffer(drm->pAppContext, &pbRevocationBuffer, &cbRevocationBuffer);
        if(pbRevocationBuffer != NULL) Oem_MemFree( pbRevocationBuffer);
#endif

        /* clean up dynalloc-ed crypto and manager contexts */
        if (drm->poDecryptCtx != NULL) Oem_MemFree(drm->poDecryptCtx);
        if (drm->pAppContext != NULL) {
            Drm_Uninitialize(drm->pAppContext);
            Oem_MemHeapFree(drm->pAppContext);
        }
        
#ifdef DRM_SCAN_ASF
        if (drm->asfp != NULL) {        /* this would be caused by an external scanner */
            basf_close_asf(drm->asfp);  /* only bother closing, if internal scanner is used */
            drm->asfp = NULL;
			drm->cfg->asf = NULL;       /* remove the cached handle,basf_close_asf frees this handle as well */ 
        }
#endif

        /* This is only needed for MP4 since the allocation happens in prd_mp4. For ASF, allocation/free happens
           in the nexus wmdrm pd core module.*/
        if(drm->cfg->cnt_type == bdrm_cnt_mp4){
            drm->cnt_fp.reset_headers(drm);
        }

        BKNI_Free(drm->cfg);
        BKNI_Free(drm);  
    }

    BDBG_MSG(("%s: Exiting, ret %d.\n", __FUNCTION__, ret));
    return(ret);
}


/* Install header callbacks. These will be called from the asf parser
** as it goes through the file.
*/
bdrm_err 
bdrm_init_handlers (bdrm_t drm)
{
    bdrm_err ret  = bdrm_err_fail;
    bdrm_license_t lic_info = NULL;

    BDBG_MSG(("%s - entering", __FUNCTION__));
    BDBG_ASSERT(drm != NULL);
    BDBG_ASSERT(drm->cfg != NULL);

    /* don't launch internal scan if it's done outside of drmcore 
     * */
    if ((drm != NULL) && (drm->cfg != NULL)) {

        if(drm->cfg->cnt_type == bdrm_cnt_asf){
            bdrm_change_lic_state(drm, bdrm_lic_pending, lic_info);
            #if 0
            b_asf_init_handlers(drm);
            #endif
            drm->ctx_state = bdrm_ctx_running;                  /* signal running state */        
        }

        ret = bdrm_err_ok;
    }

    BDBG_MSG(("%s: Exiting, ret %d.\n", __FUNCTION__, ret));
    return(ret);
}

/* Looks for a valid license for this content, in the designated datastore
*/
bdrm_err 
bdrm_load_license (bdrm_t drm, bdrm_license_t* license)
{
    bdrm_err    ret = bdrm_err_fail;
    DRM_BYTE* pbXML; DRM_DWORD cbXML;
    DRMPFNPOLICYCALLBACK policy_cback = NULL;
    void * policy_ctx = NULL;

    BDBG_MSG(("%s - entering", __FUNCTION__));
    BDBG_ASSERT(drm != NULL);

    /* Only register an policy callback if one is configured */
    if(drm->cfg->opl_cback != NULL){
        policy_cback = (DRMPFNPOLICYCALLBACK)bdrm_policy_callback;
        policy_ctx = (void *)drm;
    }

    if (drm != NULL) {
        DRM_RESULT  rc;

        drm->cnt_fp.get_xml_info(drm, &pbXML, (uint32_t *)&cbXML);

        /* if we can't find the license, let the other layers know */
        rc = bdrm_lookup_license(pbXML, cbXML, drm->pAppContext,
                                 &drm->poDecryptCtx, license, policy_cback, policy_ctx);
        if (DRM_SUCCEEDED(rc)) {
            ret = bdrm_err_ok;
            BDBG_WRN(("found a license\n"));
        } else {
            BDBG_WRN(("bdrm_lookup_license didn't succeed\n"));

            if (rc == DRM_E_LICENSEEXPIRED) {
                bdrm_change_lic_state(drm, bdrm_lic_expired, *license);
                ret = bdrm_err_license_expired;
            }
        }
    }

    BDBG_MSG(("%s: Exiting, ret %d\n", __FUNCTION__, ret));
    return(ret);
}


/* Cleans up the components in the challenge, as they are allocated in the
** bdrm_gen_challenge func below.
*/
bdrm_err 
bdrm_clean_challenge (const bdrm_license_challenge* challenge)
{
    bdrm_err ret = bdrm_err_fail;
    
    BDBG_MSG(("%s: Entering\n", __FUNCTION__));

    BDBG_ASSERT(challenge != NULL);

    if (challenge != NULL) {
        ret = bdrm_err_ok;
        if(challenge->url != NULL) Oem_MemFree(challenge->url); 
        if(challenge->data != NULL) Oem_MemFree(challenge->data); 
    }

    BDBG_MSG(("%s: Exiting, ret %d.\n", __FUNCTION__, ret));
    return(ret);       
}
/* Determines if the asf is encr. Called from the drm app to obtain a
** challenge which is later sent to a license server
*/

bdrm_err 
bdrm_gen_challenge (
    bdrm_t drm,
    const char *pCustomData,
    uint32_t customDataLen,
    bdrm_license_challenge* challenge, 
    bdrm_license_t license
)
{
    bdrm_err ret = bdrm_err_fail;   
    uint32_t ch_len = 0;
    DRM_CONST_STRING *f_pdstrFileURL = NULL;
    DRM_CHAR *ch_url, *ch_data;

    DRM_DECRYPT_CONTEXT* crypto_ctx = NULL;
    DRM_RESULT dr;
    
    BDBG_MSG(("%s: Entering\n", __FUNCTION__));

    BDBG_ASSERT(drm != NULL);
    BDBG_ASSERT(challenge != NULL);
    BSTD_UNUSED(f_pdstrFileURL);

    if ((drm != NULL) && (challenge != NULL)) {
        crypto_ctx = drm->poDecryptCtx;                 /* decryption context */

        dr =  bdrm_generate_lic_challenge(drm->pAppContext, (DRM_CHAR *)pCustomData, customDataLen, &ch_url, &ch_data, &ch_len);
        if (dr == DRM_SUCCESS) {
            ret = bdrm_err_ok; 
            challenge->len = ch_len;
            challenge->data = (char *)ch_data; 
            challenge->url = (char *)ch_url;                            
            #if 0
            BDBG_WRN(("drm generated challenge\n\turl=\"%s\" "
                        "data[0]=\'%c\' data[1]=\'%c\' data[2]=\'%c\'", 
                        challenge->url, challenge->data[0], challenge->data[1], 
                        challenge->data[2]));    
            BDBG_MSG(("%s", challenge->data));
            #endif
        } else{
            if(dr == DRM_E_NO_URL){
                /* No url found in the header. Some streams don't have an URL tag in their header, like some Netflix 
                    streams for example. 
                    Generate a challenge without url then. At this point the application, like netflix, will know where
                    to send it to get the license. */

                dr =  bdrm_generate_lic_challenge(drm->pAppContext, (DRM_CHAR *)pCustomData, customDataLen, NULL, &ch_data, &ch_len);
                ret = bdrm_err_ok; 
                challenge->len = ch_len;
                challenge->data = (char *)ch_data; 
                challenge->url = NULL;                            
                #if 0
                BDBG_WRN(("drm generated challenge without URL\n\turl=\"%p\" "
                            "data[0]=\'%c\' data[1]=\'%c\' data[2]=\'%c\'", 
                            challenge->url, challenge->data[0], challenge->data[1], 
                            challenge->data[2]));    
                BDBG_MSG(("%s", challenge->data));
                #endif
            }else {
                BDBG_WRN(("generating lic challenge failed"));
            }

            if(dr == DRM_E_NO_URL){
                BDBG_WRN(("could not find any url present in the stream header to send the challenge to."));
                bdrm_change_lic_state(drm, bdrm_lic_fail_no_url, license);         /* notify of failure, no url is present in the stream header to send challenge to. */
            }
                
        }
    }

    BDBG_MSG(("%s: Exiting, ret %d.\n", __FUNCTION__, ret));
    return(ret);
}


#ifdef PRDY_ROBUSTNESS
bdrm_err bdrm_gen_clk_challenge (bdrm_t drm, bdrm_clk_challenge* challenge) 
{
    bdrm_err    bdrm_rc = bdrm_err_fail;
    uint32_t url_len = 0;
    uint32_t ch_len = 0;
    DRM_CHAR* ch_url = NULL;
    DRM_BYTE *ch_data = NULL;
    DRM_RESULT dr = DRM_SUCCESS;
    
    BDBG_MSG(("%s: Entering\n", __FUNCTION__));

    if(drm == NULL)
    {
        BDBG_ERR(("bdrm_gen_clk_challenge -  'drm' is NULL.\n"));
        return bdrm_err_fail;
    }

    if (challenge != NULL) 
    {

        BDBG_MSG(("bdrm_gen_clk_challenge - Using existing context manager\n"));

        ChkDR(bdrm_secure_clk_gen_challenge(drm->pAppContext, &ch_url, &ch_data, &url_len, &ch_len));

        bdrm_rc = bdrm_err_ok;

        /*set the lengths*/
        challenge->url_len = url_len;
        challenge->ch_len = ch_len;
            
        /*assign the pointers to the data*/
        challenge->url = (char *)ch_url;
        challenge->ch_data = ch_data;
    }
    else
    {
        BDBG_ERR(("bdrm_gen_clk_challenge - verify input drm handle (%p) and pointer to challenge (%p)", drm, challenge));
    }
    

ErrorExit:
    if(dr != DRM_SUCCESS) bdrm_rc = bdrm_err_fail;

    BDBG_MSG(("%s: Exiting, bdrm_rc %d.\n", __FUNCTION__, bdrm_rc));
    return bdrm_rc;
}


bdrm_err bdrm_clean_clk_challenge (bdrm_clk_challenge* challenge) 
{
    bdrm_err    bdrm_rc = bdrm_err_ok;
    
    BDBG_MSG(("%s: Entering\n", __FUNCTION__));

    if(challenge != NULL)
    {
        bdrm_free_buffer((DRM_VOID*) challenge->url);
        bdrm_free_buffer(challenge->ch_data);
    }

    BDBG_MSG(("%s: Exiting, bdrm_rc %d.\n", __FUNCTION__, bdrm_rc));
    return bdrm_rc;
}


void bdrm_free_buffer(void* ptr)
{
    if(ptr != NULL){
        Oem_MemFree(ptr);
    }
}



bdrm_err bdrm_clk_process_response(bdrm_t drm,   /* [in] drm manager context */
                                   bdrm_clk_response* response)
{
    bdrm_err    bdrm_rc = bdrm_err_fail;
    DRM_RESULT dr = DRM_SUCCESS;
    
    BDBG_MSG(("%s: Entering\n", __FUNCTION__));

    if(drm == NULL)
    {
        BDBG_ERR(("bdrm_clk_process_response -  'drm' is NULL.\n"));
        return bdrm_err_fail;
    }

    if (response != NULL) 
    {
        ChkDR(bdrm_secure_clk_process_response(drm->pAppContext, 
                                              (DRM_BYTE *)response->data, 
                                              (DRM_DWORD)response->len, 
                                              (DRM_RESULT*)&(response->result)));
        
        bdrm_rc = bdrm_err_ok; 
    }
    else
    {
        BDBG_ERR(("bdrm_clk_process_response - verify input drm handle (%p) and pointer to response (%p)", drm, response));
    }

ErrorExit:
    if(dr != DRM_SUCCESS) bdrm_rc = bdrm_err_fail;

    BDBG_MSG(("%s: Exiting, bdrm_rc %d.\n", __FUNCTION__, bdrm_rc));
    return bdrm_rc;
}

/* get the clock status
*/
bdrm_err bdrm_clk_get_status(bdrm_t drm,   /* [in] drm manager context */
                             bdrm_clk_status* status)
{
    bdrm_err    bdrm_rc = bdrm_err_fail;
    DRM_CHAR *pwszSecureTime = NULL;
    DRM_DWORD pcchSecureTime = 0; 
    DRM_DWORD pdwFlags = 0;
    DRM_RESULT dr = DRM_SUCCESS;

    /*Following parameters seemed to be used when called wants to set a specific time and set a specific state */
    DRM_BYTE *pbSecureTimeWithStatus = NULL;                                        
    DRM_DWORD pcbSecureTimeWithStatus = 0;    

    BDBG_MSG(("%s: Entering\n", __FUNCTION__));

    if(drm != NULL)
    {
        ChkDR(bdrm_get_secure_clk_status(drm->pAppContext, 
                                            &pwszSecureTime, &pcchSecureTime, 
                                            &pdwFlags, 
                                            pbSecureTimeWithStatus, &pcbSecureTimeWithStatus));

        status->securetime = (unsigned char*)pwszSecureTime;
        status->len = pcchSecureTime;
        status->flags = (unsigned long*)pdwFlags;
        bdrm_rc = bdrm_err_ok;
    }
    else
    {
        BDBG_ERR(("bdrm_clk_get_status - verify input drm handle (%p)", drm));
    }

ErrorExit:
    if(dr != DRM_SUCCESS) bdrm_rc = bdrm_err_fail;

    BDBG_MSG(("%s: Exiting, bdrm_rc %d.\n", __FUNCTION__, bdrm_rc));
    return bdrm_rc;
}


/* Initialize oemtime module
*/ 
bdrm_err 
bdrm_oem_time_init()
{
    bdrm_err  rc = bdrm_err_ok;
    DRM_VOID *f_pOEMContext = NULL;
    NEXUS_ReadOtpIO  otp_id;

    BDBG_MSG(("%s: Entering\n", __FUNCTION__));

    bdrm_read_otp_id_priv(&otp_id);
    f_pOEMContext = Oem_Create(&otp_id);
    if(f_pOEMContext != NULL){
        if(OEM_TimeInit(f_pOEMContext) != DRM_SUCCESS){
            rc = bdrm_err_fail;
        }
    }
    else rc = bdrm_err_fail;


    if(f_pOEMContext) Oem_Destroy(f_pOEMContext);
    BDBG_MSG(("%s - exiting rc %x", __FUNCTION__, rc));
    return rc;
}


/* UnInitialize oemtime module
*/ 
bdrm_err 
bdrm_oem_time_uninit()
{
    bdrm_err  rc = bdrm_err_ok;
    DRM_VOID *f_pOEMContext = NULL;
    NEXUS_ReadOtpIO  otp_id;

    BDBG_MSG(("%s: Entering\n", __FUNCTION__));

    bdrm_read_otp_id_priv(&otp_id);
    f_pOEMContext = Oem_Create(&otp_id);
    if(f_pOEMContext != NULL){
        if(OEM_TimeUnInit(f_pOEMContext) != DRM_SUCCESS){
            rc = bdrm_err_fail;
        }
    }
    else rc = bdrm_err_fail;


    if(f_pOEMContext) Oem_Destroy(f_pOEMContext);
    BDBG_MSG(("%s - exiting rc %x", __FUNCTION__, rc));
    return rc;
}


/*  Called from the drm app to obtain client info which is later sent to a license server
*/
bdrm_err
bdrm_get_client_info(bdrm_t drm, uint8_t **pdata, uint32_t *len)
{
    bdrm_err  rc = bdrm_err_fail;
    DRM_CHAR* ci_xml = NULL;
    uint32_t  ci_length = 0;

    BDBG_MSG(("%s: Entering\n", __FUNCTION__));

    BDBG_ASSERT(drm);
    BDBG_ASSERT(pdata != NULL);

    if ((drm != NULL) && (pdata != NULL)){

        if(bdrm_generate_client_info(drm->pAppContext, &ci_xml, &ci_length) == DRM_SUCCESS){
            BDBG_MSG(("bdrm_get_client_info() - bdrm_generate_client_info() succeeded\n"));
            *pdata = (uint8_t *)ci_xml;
            *len = ci_length;
            rc = bdrm_err_ok;
        }else {
             BDBG_ERR(("bdrm_get_client_info -  bdrm_generate_client_info() failed\n"));
        }
    }
    else {
        BDBG_ERR(("bdrm_get_client_info -  invalid parameters (drm = '%p', pdata = '%p')\n", drm, pdata));
    }

    BDBG_MSG(("%s - exiting rc %x", __FUNCTION__, rc));
    return rc;
}


/* Cleans up the buffer allocated to hold the client info */
bdrm_err 
bdrm_clean_client_info (uint8_t *pdata)
{
    bdrm_err rc = bdrm_err_fail;

    BDBG_MSG(("%s: Entering\n", __FUNCTION__));

    BDBG_ASSERT(pdata != NULL);

    if (pdata != NULL) {
        rc = bdrm_err_ok;
        bdrm_free_buffer(pdata);
    }
    BDBG_MSG(("%s - exiting rc %x", __FUNCTION__, rc));
    return(rc);       
}

/* Feeds the license blob obtained from the server, back to the drm
** engine for storage and verification
*/
bdrm_err 
bdrm_feed_pdla_license (bdrm_t drm, uint8_t *plicense, uint32_t len)
{
    bdrm_err ret = bdrm_err_fail;
    DRM_RESULT res; 
    BDBG_MSG(("%s: Entering\n", __FUNCTION__));

    BDBG_ASSERT(drm != NULL);

    if ((drm != NULL) && (plicense != NULL)) {

        if (DRM_SUCCEEDED(res = bdrm_process_response(drm->pAppContext, plicense, len))) {
            ret = bdrm_err_ok; BDBG_MSG(("response processed ok\n"));
        } else {
            BDBG_MSG(("response <not> processed ok\n"));
        }
    }
    BDBG_MSG(("%s - exiting ret %x", __FUNCTION__, ret));
    return (ret);
}
#endif

/* Feeds the license blob obtained from the server, back to the drm
** engine for storage and verification
*/
bdrm_err 
bdrm_feed_license (bdrm_t drm, bdrm_license_response* resp, bdrm_license_t license)
{
    bdrm_err ret = bdrm_err_fail;
    DRM_RESULT res; DRM_BYTE* pbdata; DRM_DWORD dw;

    BDBG_MSG(("%s: Entering\n", __FUNCTION__));

    BDBG_ASSERT(drm != NULL);
    BDBG_ASSERT(resp != NULL);

    if ((drm != NULL) && (resp != NULL)) {
        dw = resp->len;
        pbdata = resp->resp_start;

        if (DRM_SUCCEEDED(res = bdrm_process_response(drm->pAppContext, pbdata, dw))) {
            ret = bdrm_err_ok; BDBG_ERR(("response processed ok\n"));
            bdrm_change_lic_state(drm, bdrm_lic_stored, license);         /* license stored */
        } else BDBG_ERR(("response <not> processed ok\n"));
        /*Only delete response buffer if we used drm_http to get the license */
        if(drm->cfg->lic_extern == bdrm_util_default_extern)
        {
            #if 0
            BKNI_Free(resp->data);
            #endif
        }
    }
    BDBG_MSG(("%s - exiting ret %x", __FUNCTION__, ret));
    return (ret);
}

/* default callback function for license grabber; this should not be used in
** a real scenario. provided only as example.
*/
bdrm_err
bdrm_util_default_extern(const bdrm_license_challenge* ch, bdrm_license_response* res,
                         const void* context)
{
    int32_t post_ret; 
    bdrm_err ret = bdrm_err_fail;    
    bdrm_transport_ctx* transport_ctx = (bdrm_transport_ctx*)context;
    BSTD_UNUSED(ch); BSTD_UNUSED(res);
    BSTD_UNUSED(post_ret);
    BSTD_UNUSED(transport_ctx);
    BSTD_UNUSED(context);

    BDBG_MSG(("%s: Entering\n", __FUNCTION__));
#if 0
    if(bdrm_is_drmencrypted(transport_ctx->drm) == bdrm_encr_wmdrm)
        post_ret = bhttpclient_license_post_default(ch->url, ch->data, ch->non_quiet, 
                   ch->app_security, &res->data, &res->resp_start, &res->len); 
    else {
        post_ret = bhttpclient_license_post_soap(ch->url, ch->data, ch->non_quiet, 
                   ch->app_security, &res->data, &res->resp_start, &res->len); 
    }
    ret = (post_ret >= (0)) ? bdrm_err_ok : bdrm_err_fail;

    /* handle license response, remote server */
    if (transport_ctx != NULL) {
        bdrm_change_lic_http_state(transport_ctx->drm, bdrm_lic_transferring, 
            post_ret, transport_ctx->license);
    }
#endif

    BDBG_MSG(("%s - exiting ret %x", __FUNCTION__, ret));
    return(ret);
}

bdrm_err
bdrm_handle_transport(bdrm_t drm, bdrm_license_t license, bdrm_license_challenge* challenge,
                      bdrm_license_response* response)
{
    bdrm_err ret = bdrm_err_fail;
    bdrm_transport_ctx transport_ctx;

    BDBG_MSG(("%s: Entering\n", __FUNCTION__));

    BDBG_ASSERT((drm != NULL) && (license != NULL));
    if ((drm != NULL) && (drm->cfg != NULL) && (drm->cfg->lic_extern != NULL)) {
        bdrm_change_lic_state(drm, bdrm_lic_transferring, license);             /* notify back transferring */                        
        if (drm->cfg->transport == bdrm_default_transport) {
            transport_ctx.license = license; transport_ctx.drm = drm;           /* init transport structure */            
            drm->cfg->lic_extern_ctx = (void*)&transport_ctx; 

            challenge->non_quiet = CH_DEFAULT_NON_QUIET;                        /* default to non-quiet and appsec testlevel */
            challenge->app_security = CH_DEFAULT_APP_SEC;
        }

        if (bdrm_err_ok != drm->cfg->lic_extern(challenge, response, 
                    drm->cfg->lic_extern_ctx)) 
         {                                /* call external grab function */
            bdrm_change_lic_state(drm, bdrm_lic_failtransfer, license);         /* notify of failed transfer */
        } else if (bdrm_err_ok == bdrm_feed_license(drm, response, license)) {
            ret = bdrm_err_ok;                                                  /* feed response back into hds */
        }                                                                       
        else
        {
            bdrm_change_lic_state(drm, bdrm_lic_failtransfer, license);         /* notify of failed transfer */
        }
    } else { 
        BDBG_ASSERT((drm->cfg != NULL) && (drm->cfg->lic_extern != NULL));      /* no external grab function assigned */
    }
    BDBG_MSG(("%s - exiting ret %x", __FUNCTION__, ret));
    return (ret);
}

/* translate callback license states into meaningful messages
*/
static bdrm_err
bdrm_translate_http_state (bdrm_http_state state, char* msg)
{
    bdrm_err ret = bdrm_err_ok;

    BDBG_MSG(("%s: Entering\n", __FUNCTION__));

    switch (state) {    
        case bdrm_http_status_failed_getpost:       BKNI_Snprintf(msg, strlen(msg), "%s", "(http get/post failure)"); break;/*strcpy(msg, "(http get/post failure)"); break;*/
        case bdrm_http_status_failed_connect:       BKNI_Snprintf(msg, strlen(msg), "%s", "(connection failure)"); break;/*strcpy(msg, "(connection failure)"); break;*/
        case bdrm_http_status_failed_internal:      BKNI_Snprintf(msg, strlen(msg), "%s", "(internal failure)"); break;/*strcpy(msg, "(internal failure)"); break;*/
        case bdrm_http_status_failed_response_read: BKNI_Snprintf(msg, strlen(msg), "%s", "(could not read response)"); break;/*strcpy(msg, "(could not read response)"); break;*/
        case bdrm_http_status_failed_response_code: BKNI_Snprintf(msg, strlen(msg), "%s", "(response code was not ok)"); break;/*strcpy(msg, "(response code was not ok)"); break;*/
        case bdrm_http_status_failed_no_license:    BKNI_Snprintf(msg, strlen(msg), "%s", "(response was ok, no license)"); break;/*strcpy(msg, "(response was ok, no license)"); break;*/
        case bdrm_http_status_failed_generic:       BKNI_Snprintf(msg, strlen(msg), "%s", "(generic failure)"); break;/*strcpy(msg, "(generic failure"); break;*/
        case bdrm_http_status_ok:                   BKNI_Snprintf(msg, strlen(msg), "%s", "(response had a license)"); break;/*strcpy(msg, "(response had a license)"); break;*/
        case bdrm_http_status_undefined:            BKNI_Snprintf(msg, strlen(msg), "%s", "(http was not needed)"); break;/*strcpy(msg, "(http was not needed)"); break;*/
        default:                                    BKNI_Snprintf(msg, strlen(msg), "%s", "(unknown)"); ret = bdrm_err_fail; break;/*strcpy(msg, "(unknown)"); ret = bdrm_err_fail; break;*/
    }

    BDBG_MSG(("%s - exiting ret %x", __FUNCTION__, ret));
    return(ret);
}

bdrm_err bdrm_get_license_challenge(
    bdrm_t drm,
    bdrm_license_challenge *p_challenge
    )
{
    bdrm_err ret = bdrm_err_ok;
    bdrm_license_t license = NULL;
    bdrm_license_challenge challenge;
    bdrm_encr_state state = bdrm_is_drmencrypted (drm);

    BDBG_MSG(("%s: Entering\n", __FUNCTION__));

    switch (state) {
        case bdrm_encr_wmdrm:
        case bdrm_encr_aes_ctr:
            BDBG_MSG(("\ndrm is present.\n"));
            if (bdrm_err_fail == (ret = bdrm_load_license (drm, &license))) 
            {           
                /* if license lookup fails */
                BDBG_MSG(("\n ******  drm could not load a license. so generate a challenge \n"));    
                BDBG_MSG(("\n ******  drm could not load a license. bdrm_change_lic_state\n"));            
                bdrm_change_lic_state(drm, bdrm_lic_faillocal, license);                /* license failed to load from local */
                if (bdrm_err_ok == (ret = bdrm_gen_challenge (drm, NULL, 0, &challenge, license)))
                {
                    p_challenge->data = challenge.data;
                    p_challenge->url = challenge.url;
                    p_challenge->len = challenge.len;
                }
                else 
                { 
                    bdrm_change_lic_state(drm, bdrm_lic_failtransfer, license);  /* failed generating the challenge */  
                }
            } 
            else
            {
                BDBG_MSG(("\n ******bdrm_handle_drm drm -  license acquired.\n"));
                bdrm_change_lic_state(drm, bdrm_lic_acquired, license);                 /* notify back 'acquired' */
            }

            break;
        case bdrm_encr_none:
        default:
            BDBG_MSG(("\ndrm is <not> present.\n"));                                    /* no sign of drm in this file */
            ret = bdrm_err_fail;
            break;
    }

    return ret;
}

/* Handles the entire drm process, from licesne lookup to challenge
** generation, response processing, etc.
*/
bdrm_err 
bdrm_handle_drm (bdrm_t drm)
{
    bdrm_err ret = bdrm_err_ok; bdrm_license_challenge challenge;
    bdrm_license_response response; bdrm_license_t license = NULL;
    bdrm_encr_state state = bdrm_is_drmencrypted (drm);

    BDBG_MSG(("%s: Entering\n", __FUNCTION__));

    switch (state) {
        case bdrm_encr_wmdrm:
        case bdrm_encr_aes_ctr:
            BDBG_MSG(("\ndrm is present.\n"));
            if ((drm->lic_state == bdrm_lic_faillocal) || 
                    (drm->lic_state == bdrm_lic_acquired)) break;                       /* we could be in a basf_feed loop */

            if (bdrm_err_fail == (ret = bdrm_load_license (drm, &license))) 
            {           /* if license lookup fails */
              BDBG_MSG(("\n ******bdrm_handle_drm   drm could not load a license. so generate a challenge \n"));    
              BDBG_MSG(("\n ******bdrm_handle_drm   drm could not load a license. bdrm_change_lic_state\n"));            
                bdrm_change_lic_state(drm, bdrm_lic_faillocal, license);                /* license failed to load from local */
              if (bdrm_err_ok == (ret = bdrm_gen_challenge (drm, NULL, 0, &challenge, license)))
              {      /* generate challenge */
                if(bdrm_err_ok == bdrm_handle_transport(drm, license, &challenge, &response))
                {
                 BDBG_MSG(("\n ******bdrm_handle_drm  - successfully process the resposne \n"));     
                 ret = bdrm_load_license (drm, &license);  /* look in the license store again */
                 BDBG_MSG(("\n ******bdrm_handle_drm  - look in the licesne store for stored license \n"));  
                 if (ret == bdrm_err_ok)
                 {
                    BDBG_MSG(("\n ******bdrm_handle_drm  - loaded license after generating challenage and process the response.\n"));
                    bdrm_change_lic_state(drm, bdrm_lic_acquired, license);                 /* notify back 'acquired' */
                    goto license_acquired;
                 }
                 else
                 {
                   /* need to fix this later, we now consider all the erros as the license expired */
                   BDBG_MSG(("\n ******bdrm_handle_drm The license was not loaded in the license store after processing the response, generate an error. 0x%x\n", ret));
                   BDBG_MSG(("\n ******bdrm_handle_drm - cleaning the allocate challenage\n"));             /* handle transport of license */
                   bdrm_change_lic_state(drm, bdrm_lic_expired, license);
                   goto error_exit;
                 }
                }   
                BDBG_MSG(("\n ******bdrm_handle_drm - cleaning the allocate challenage\n"));             /* handle transport of license */
                    bdrm_clean_challenge(&challenge);                                   /* clean up alloc-ed challenge */     
              } 
              else 
              { 
                bdrm_change_lic_state(drm, bdrm_lic_failtransfer, license);  /* failed generating the challenge */  
              }
            } 
            else
            {
                BDBG_MSG(("\n ******bdrm_handle_drm drm -  license acquired.\n"));
                bdrm_change_lic_state(drm, bdrm_lic_acquired, license);                 /* notify back 'acquired' */
            }

license_acquired:
    
            break;
        case bdrm_encr_none:
        default:
            BDBG_MSG(("\ndrm is <not> present.\n"));                                    /* no sign of drm in this file */
            ret = bdrm_err_fail;
            break;
    }

error_exit:
    if (license != NULL)
        bdrm_destroy_license(license);                                                  /* destroy dyn-alloc-ed license instance */
    BDBG_MSG(("%s - exiting ret %x", __FUNCTION__, ret));
    return(ret);
}

/* translate callback license states into meaningful messages
*/
static bdrm_err
bdrm_translate_lic_state (bdrm_license_state state, char* msg)
{
    bdrm_err ret = bdrm_err_ok;

    BDBG_MSG(("%s: Entering\n", __FUNCTION__));

    switch (state) {
        case bdrm_lic_pending:      BKNI_Snprintf(msg, strlen(msg), "%s", "(pending)"); break;/*strcpy(msg, "(pending)"); break;*/
        case bdrm_lic_transferring: BKNI_Snprintf(msg, strlen(msg), "%s", "(transferring)"); break;/*strcpy(msg, "(transferring)"); break;*/
        case bdrm_lic_localstore:   BKNI_Snprintf(msg, strlen(msg), "%s", "(local store)"); break;/*strcpy(msg, "(local store)"); break;*/
        case bdrm_lic_acquired:     BKNI_Snprintf(msg, strlen(msg), "%s", "(acquired)"); break;/*strcpy(msg, "(acquired)"); break;*/
        case bdrm_lic_faillocal:    BKNI_Snprintf(msg, strlen(msg), "%s", "(failed local)"); break;/*strcpy(msg, "(failed local)"); break;*/
        case bdrm_lic_failtransfer: BKNI_Snprintf(msg, strlen(msg), "%s", "(failed transfer)"); break;/*strcpy(msg, "(failed transfer)"); break;*/
        case bdrm_lic_expired:      BKNI_Snprintf(msg, strlen(msg), "%s", "(expired)"); break;/*strcpy(msg, "(expired)"); break;*/
        case bdrm_lic_failcount:    BKNI_Snprintf(msg, strlen(msg), "%s", "(failed count)"); break;/*strcpy(msg, "(failed count)"); break;*/
        case bdrm_lic_stored:       BKNI_Snprintf(msg, strlen(msg), "%s", "(stored)"); break;/*strcpy(msg, "(stored)"); break;*/
        default:                    BKNI_Snprintf(msg, strlen(msg), "%s", "(unknown)"); ret = bdrm_err_fail; break;/*strcpy(msg, "(unknown)"); ret = bdrm_err_fail; break;*/
    }

    BDBG_MSG(("%s - exiting ret %x", __FUNCTION__, ret));
    return(ret);
}

/* default callback function for the license state notification
*/
bdrm_err 
bdrm_util_default_callback (bdrm_acq_state* lic_acq_state, bdrm_license_t license,
                            const void* context)
{
    uint8_t count, index;
    bdrm_license_info lic_info;
    bdrm_err ret = bdrm_err_fail; 
    char msg[32]; 

    BDBG_MSG(("%s: Entering\n", __FUNCTION__));
    BSTD_UNUSED(license); BSTD_UNUSED(context);
    BDBG_ASSERT(lic_acq_state != NULL);
    
    if (lic_acq_state != NULL) {    
        bdrm_translate_lic_state(lic_acq_state->license_state, msg);
        BDBG_WRN(("license state changed (%d) [%s]", lic_acq_state->license_state, msg));
        if (lic_acq_state->http_state != bdrm_http_status_undefined) {
            bdrm_translate_http_state(lic_acq_state->http_state, msg);
            BDBG_WRN(("http state (%d) [%s]", lic_acq_state->http_state, msg));
        }
        count = bdrm_license_get_count(license);
        for (index = 0; index < count; index++) {
            BKNI_Memset(&lic_info, 0, sizeof(bdrm_license_info));
            bdrm_license_get_item(license, index, &lic_info);
            BDBG_MSG(("license for stream id=%d, type=%d", lic_info.stream_id, lic_info.type));
        }
    }

    BDBG_MSG(("%s - exiting ret %x", __FUNCTION__, ret));
    return(ret);
}

/* obtain a reference to the internal decryptor instance
*/
bdrm_err
bdrm_dcrypt_instance (bdrm_t drm, bdrm_decryptor_t* dcrypt)
{
    bdrm_err ret = bdrm_err_ok;
    
    BDBG_MSG(("%s: Entering\n", __FUNCTION__));
    BDBG_ASSERT(drm);
    BDBG_ASSERT(dcrypt);

    if ((drm != NULL) && (dcrypt != NULL)) {
        (*dcrypt) = drm->dcrypt;
        bdrm_decryptor_set_magic(drm->dcrypt, 
                BDRM_DCRYPT_MAGIC);
    }
    
    else ret = bdrm_err_fail;

    BDBG_MSG(("%s - exiting ret %x", __FUNCTION__, ret));
    return(ret);
}

/* obtain a reference to the internal manager decrypt context. this is needed
   for the decryptor. resolved by means of external decl in the decryptor
*/
bdrm_err 
bdrm_get_decrypt_context (bdrm_t drm, DRM_DECRYPT_CONTEXT** dcrypt_ctx)
{
    bdrm_err ret = bdrm_err_ok;
    
    BDBG_MSG(("%s: Entering\n", __FUNCTION__));
    BDBG_ASSERT(drm); 
    BDBG_ASSERT(dcrypt_ctx);

    if ((drm != NULL) && (dcrypt_ctx != NULL)) (*dcrypt_ctx) = drm->poDecryptCtx;
    else ret = bdrm_err_fail;

    BDBG_MSG(("%s - exiting ret %x", __FUNCTION__, ret));
    return(ret);
}

#ifdef PRDY_ROBUSTNESS
static bdrm_err 
bdrm_create_uname_hds_file_path_str (char *buf, unsigned int buf_size, char *user_name)
{
    DRM_CONST_STRING pwszDataStore;
    DRM_CHAR  tmp_dir_str[DRM_MAX_PATH] = {0x00};
    unsigned int size;

    DRM_CHAR  drm_datastore_file_ext[] = ".hds";
    bdrm_err ret = bdrm_err_ok;
    
    BDBG_MSG(("%s: Entering\n", __FUNCTION__));
    BDBG_MSG(("bdrm_create_uname_hds_file_path_str - Entered function: 'user_name' = %s, len = %d, buf_size = %d\n", user_name, strlen(user_name), buf_size));
    
    /*get the directory where the datastore files are located */
    bdrm_env_datastore_dir(&pwszDataStore);
    
    /* transform the string from wide-char to ANSI */
    DRM_UTL_DemoteUNICODEtoASCII(pwszDataStore.pwszString, tmp_dir_str, pwszDataStore.cchString);
    size = pwszDataStore.cchString + strlen((const char*)user_name) + strlen((const char*)drm_datastore_file_ext);
    if(size < buf_size){
        strncpy(buf, (const char*)tmp_dir_str, pwszDataStore.cchString);
        strncat(buf, user_name, strlen((const char*)user_name));
        strncat(buf, (const char*)drm_datastore_file_ext, strlen((const char*)drm_datastore_file_ext));
    }
    else {
        BDBG_ERR(("%s - buffer is too small", __FUNCTION__));
        ret = bdrm_err_fail;
    }

    BDBG_MSG(("bdrm_create_uname_hds_file_path_str - buf(3) = '%s', length = '%u'\n", buf, strlen(buf) ));

    return ret;
}


#if 0
static DRM_RESULT bdrm_get_file_path_str(bdrm_file_id_e id, DRM_WCHAR* path, uint32_t buf_size)
{
    DRM_CONST_STRING pwszDir;
    DRM_CONST_STRING pwszFilename;
    uint32_t size = 0;
    DRM_RESULT dr;

    BDBG_MSG(("%s: Entering\n", __FUNCTION__));

    switch(id)
    {
        case bdrm_file_dev_cert:
            bdrm_env_cert_dir(&pwszDir);
            bdrm_env_dev_cert_filename(&pwszFilename);
            break;
        case bdrm_file_hds_password:
            bdrm_env_datastore_dir(&pwszDir);
            bdrm_env_hds_password_filename(&pwszFilename);
            break;
        case bdrm_file_default_hds:
            bdrm_env_datastore_dir(&pwszDir);
            bdrm_env_hds_filename(&pwszFilename);
            break;
        case bdrm_file_zgpriv_sig:
            bdrm_env_cert_dir(&pwszDir);
            bdrm_env_zgpriv_sig_filename(&pwszFilename);
            break;
        case bdrm_file_zgpriv_enc:
            bdrm_env_cert_dir(&pwszDir);
            bdrm_env_zgpriv_enc_filename(&pwszFilename);
            break;
        case bdrm_file_zdev_cert:
            bdrm_env_cert_dir(&pwszDir);
            bdrm_env_zdev_cert_filename(&pwszFilename);
            break;
        case bdrm_file_revoke_info:
            bdrm_env_datastore_dir(&pwszDir);
            bdrm_env_revocation_filename(&pwszFilename);
            break;
        default:
            ChkDR(DRM_E_INVALIDARG);
            break;
    }

    size = pwszDir.cchString + pwszFilename.cchString;

    if(size < buf_size){
        
        ChkDR(DRM_STR_StringCchCopyNW(path, buf_size, pwszDir.pwszString, pwszDir.cchString));
        ChkDR(DRM_STR_StringCchCatW(path, buf_size, pwszFilename.pwszString));
    }
    else {
        dr = DRM_E_BUFFERTOOSMALL;
    }

ErrorExit:
    BDBG_MSG(("%s - exiting dr %x", __FUNCTION__, dr));
    return dr;
}
#endif

#if 0
/*!+!hla bdrm_erase_uname_hds_files() will not work in kernel do this op in nexus wmdrm module?*/
/* revoke the wmdrmpd keys and dev cert, dev cert template, etc.
*/
static bdrm_err bdrm_erase_uname_hds_files(void)
{
    bdrm_err rc = bdrm_err_ok;
    DIR *dirp = NULL;
    struct dirent *dir_buf = NULL ,*entry;
    size_t size_needed;
    int error;
    char *filename;
    char filepath[DRM_MAX_PATH];
    DRM_CHAR dirname[DRM_MAX_PATH] = {0x00};
    DRM_CONST_STRING pwszDir;

    BDBG_MSG(("%s: Entering\n", __FUNCTION__));
    bdrm_env_datastore_dir(&pwszDir);

    /* Transform the string from wide-char to ANSI  */
    DRM_UTL_DemoteUNICODEtoASCII(pwszDir.pwszString, dirname, pwszDir.cchString);

    dirp = opendir((const char*)dirname);
    if(dirp == NULL) goto ErrorExit;

    size_needed = (size_t)offsetof(struct dirent, d_name) + DRM_MAX_PATH;
    dir_buf = (struct dirent *)Oem_MemAlloc(size_needed);
    if(dir_buf == NULL) goto ErrorExit;

    while ((error = readdir_r(dirp, dir_buf, &entry)) == 0 && entry != NULL){

        filename = strstr(entry->d_name, ".hds");
        if(filename != NULL)
        {
            /* Build file path*/
            BKNI_Snprintf(filepath, strlen(filepath), "%s", (const char*)dirname);
            /*
            strcpy(filepath, (const char*)dirname); 
            */
            strcat(filepath, entry->d_name);

            if(unlink(filepath) < 0){
                if (errno != ENOENT){
                    rc = bdrm_err_fail;
                    break;
                }
            }
        }
    }
    if (errno != ENOENT && errno != 0)
    {
        rc = bdrm_err_fail;
    }

ErrorExit:
    if(dirp != NULL)  closedir(dirp);
    if(dir_buf != NULL) Oem_MemFree(dir_buf);

    BDBG_MSG(("%s - exiting rc %x", __FUNCTION__, rc));
    return rc;
}
#endif


#if 0
/*!+!hla bdrm_erase_uname_hds_files() will not work in kernel do this op in nexus wmdrm module?*/
/* revoke the wmdrmpd keys and dev cert, dev cert template, etc.
*/
bdrm_err bdrm_wmdrm_srm_revoke()
{
    bdrm_err rc = bdrm_err_ok;
    bdrm_file_id_e ii;

    BDBG_MSG(("%s: Entering\n", __FUNCTION__));

    for(ii = bdrm_file_dev_cert; ii < bdrm_file_max; ii++)
    {
        DRM_WCHAR  file_path[DRM_MAX_PATH] = {0x00};

        if(bdrm_get_file_path_str (ii, file_path, DRM_MAX_PATH) != DRM_SUCCESS) {
            rc = bdrm_err_fail; 
            break;
        }
        /*!+!hla Oem_File_Unlink() is not supported in kernel mode, I have remove the function
        deleting a file in kernel mode is a bad idea anyway for a ton of reasons.*/
        else if(Oem_File_Unlink(file_path) != DRM_SUCCESS) {
            rc = bdrm_err_fail; 
            break;
        }
    }

    if(rc == bdrm_err_ok)
        rc = bdrm_erase_uname_hds_files();

    BDBG_MSG(("%s - exiting rc %x", __FUNCTION__, rc));
    return rc;
}
#endif


/* renew the wmdrm device certificate template
*/
bdrm_err bdrm_wmdrm_srm_renew_dev_cert_template(uint8_t* buf, unsigned int buf_size)
{
    bdrm_err   rc = bdrm_err_ok;

    BSTD_UNUSED(buf);
    BSTD_UNUSED(buf_size);

    /* This functions is not needed for Playready because the device certificate template is stored
       in the Playready bin file. As a result, the code point directly to it when its read from the bin
       file. However, we keep this function to avoid unecessary changes to the bdvd_repackager and other
       upper layer modules. */
    return rc;
}
#endif

#if NEXUS_DRM_BUILD

/* Store a "local" copy of a Nexus DMA channel handle.
 * (The security code needs to use DMA.)
 * An application should
 * - call "NEXUS_Dma_Open()" to open a DMA channel
 * - call this routine to store the handle for the security code
 */
void bdrm_set_dma_handle( NEXUS_DmaHandle hDmaChannel )
{
    g_hDmaChannel = hDmaChannel;
}

/*
 * Retrieve the Nexus DMA handle.
 */
NEXUS_DmaHandle bdrm_get_dma_handle( void )
{
    return g_hDmaChannel;
}

#endif

/* TODO - start RY: might need this? */
/* set asf handle 
*/
void bdrm_set_asf (bdrm_t drm, basf_parser_t asf)
{
    BDBG_ASSERT(drm);
    BDBG_ASSERT(drm->cfg);
    
    drm->cfg->asf = asf;
}
/* TODO - end */


/* TODO - start -dummy to make build happy */
bdrm_err bdrm_check_license(bdrm_t drm, bdrm_license_t* license, char* pWrmHdr, unsigned int size)
{
    BSTD_UNUSED(drm);
    BSTD_UNUSED(license);
    BSTD_UNUSED(pWrmHdr);
    BSTD_UNUSED(size);
    BDBG_WRN(("TODO: %s called", __FUNCTION__));
    return bdrm_err_ok;
}

uint32_t bdrm_query_license_count(void)
{
    BDBG_WRN(("TODO: %s called", __FUNCTION__));
    return 0;
}

bdrm_err bdrm_get_license(uint32_t  uiIndex,         /* [in]      */ 
                             char*     pszLicense,      /* [out]     */
                             uint32_t* pcbLicenseLen)   /* [in, out] */
{
    BSTD_UNUSED(uiIndex);
    BSTD_UNUSED(pszLicense);
    BSTD_UNUSED(pcbLicenseLen);
   BDBG_WRN(("TODO: %s called", __FUNCTION__));
   return bdrm_err_ok;
}

bdrm_err
bdrm_get_license_state(bdrm_t drm,                          /* [in]   */
                       uint16_t* usLicHeader,               /* [in]   */
                       uint32_t  cbLicHeaderLen,            /* [in]   */
                       uint32_t* puiLicenseCategory,        /* [out]  */   
                       uint32_t* puiCurrentPlayBackCount,   /* [out]  */
                       uint32_t* puiMaxPlayBackCount,       /* [out]  */
                       struct tm* puiValidFromTime,         /* [out]  */
                       struct tm* puiValidUntilTime)        /* [out]  */ 
{
    BSTD_UNUSED(drm);
    BSTD_UNUSED(usLicHeader);
    BSTD_UNUSED(cbLicHeaderLen);
    BSTD_UNUSED(puiLicenseCategory);
    BSTD_UNUSED(puiCurrentPlayBackCount);
    BSTD_UNUSED(puiMaxPlayBackCount);
    BSTD_UNUSED(puiValidFromTime);
    BSTD_UNUSED(puiValidUntilTime);
    BDBG_WRN(("TODO: %s called", __FUNCTION__));
    return bdrm_err_ok;
}


#ifndef PRDY_ROBUSTNESS
bdrm_err
bdrm_get_client_info(bdrm_t drm, uint8_t **pdata, uint32_t *len)
{
    BSTD_UNUSED(drm);
    BSTD_UNUSED(pdata);
    BSTD_UNUSED(len);
    BDBG_WRN(("TODO: %s called", __FUNCTION__));
    return bdrm_err_ok;
}
#endif

/* TODO - end - dummy to make build happy */






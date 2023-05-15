/***************************************************************************
*     Copyright (c) 2006-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
*******************************************************************************/

#ifndef __PRDY_CORE_PRIV_H__
#define __PRDY_CORE_PRIV_H__

#include "prdy_core.h"
#include "prdy_mp4.h"
#ifdef DRM_SCAN_ASF
#include "asfpars.h"
#endif
#include "blst_list.h"
#include "bkni.h"

#include <drmcommon.h>      /* msdrm includes */
#include <drmutilities.h>
#include <drmcontextsizes.h>
#include <drmmanager.h>
#include <drmcrt.h>
#include <oemcommon.h>

#include "oem_brcm.h"
#ifdef PRDY_ROBUSTNESS
#include "common_crypto.h"
#endif
#include "bkni_multi.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* drm context states
*/
typedef enum bdrm_ctx_state {
    bdrm_ctx_init     = (0),              /* drm engine is starting */
    bdrm_ctx_running  = (1),              /* drm engine is running */
    bdrm_ctx_finished = (2)               /* drm stopped running */
} bdrm_ctx_state;

typedef enum bdrm_file_id {
    bdrm_file_dev_cert = 0,
    bdrm_file_hds_password,
    bdrm_file_default_hds,
    bdrm_file_zgpriv_sig,
    bdrm_file_zgpriv_enc,
    bdrm_file_zdev_cert,
    bdrm_file_revoke_info,
    bdrm_file_max
}bdrm_file_id_e;


#define AES_EKL_SIZE   (32)

/* holds drm objects extracted from the asf header
*/                                                                                                                                                                                                
typedef struct basf_drm_headers {
    basf_cencr_object encr;             /* content encryption obj */
    basf_xcencr_object xencr;           /* extended content encryption obj*/ 
    basf_digsign_object sign;           /* digital signature obj */
    basf_psi_object psi;                /* PLAYREADY : psi object */
} basf_drm_headers;

typedef struct bdrm_mp4_track_info_node {
    bdrm_mp4_protect_scheme *pScheme; /* Will be set to NULL of no protection scheme box is found for the track */
    uint32_t track_ID;
    BLST_D_ENTRY(bdrm_mp4_track_info_node)     track_link;                               /* Link to be used when this node is on the repackager list */
} bdrm_mp4_track_info_node_t;

typedef BLST_D_HEAD(bdrm_mp4_list, bdrm_mp4_track_info_node) bdrm_mp4_list_t;

typedef struct bdrm_mp4_headers {
    bdrm_mp4_pssh      pssh;     /* Protection System Specific Header Box */
    bdrm_mp4_list_t    tracks;   /* A list of every repackager which is attached to this multiplexer */
} bdrm_mp4_headers;

typedef struct bcnt_drm_headers{
    basf_drm_headers asf;
    bdrm_mp4_headers mp4;
} bcnt_drm_headers;

/* holds context for drm operations
*/
typedef struct bdrm_context {
    struct bdrm_decryptor* dcrypt;              /* decryptor object, Must be first element member in this structure */
    bdrm_ctx_state ctx_state;                   /* context state */
    bdrm_license_state lic_state;               /* license state */

    basf_drm_handlers asf_handler;              /* Container header handlers */
    bcnt_drm_headers  headers;                  /* Container headers */

    bdrm_cfg_t cfg;                             /* backptr to cfg */
    fn_license_callback lic_callback;           /* external status callback */
    void* lic_callback_ctx;                     /* external status callback context */
    fn_license_extern lic_extern;               /* external grabber func */
    void* lic_extern_ctx;                       /* external grabber func context */

    drm_cnt_fcts_t cnt_fp;
#ifdef DRM_SCAN_ASF
    basf_props_t asfp;                          /* asf parser context */
#endif
    DRM_APP_CONTEXT     *pAppContext;           /* Application context */    
    DRM_DECRYPT_CONTEXT *poDecryptCtx;          /* decryption context */    
    DRM_VOID *f_pOEMContext;                    /* Oem Context */
} bdrm_context;

#define BDRMND_SAMPLE_ID_LEN    (0x08)
#define DMA_BLKS_POOL_SIZE       (100)   /* Nb of DMA blocks in the pool */

/* drm decryptor instance */
typedef struct bdrm_decryptor {
    unsigned int magic;
    DRM_DECRYPT_CONTEXT MsDecryptCtx;   /* decryption context */   
    struct {
        uint8_t* buf;   /* linear buffer */
        size_t size;    /* current size */
        uint8_t* ekl_buf;
        uint8_t* aes_ctr_padding; /* Buffer used to add padding to the begining of dma transfer when the AES CTR operation doesn't start on byte 0 of the AES block.*/
    } dyn;

#ifdef PRDY_ROBUSTNESS
    NEXUS_DmaJobBlockSettings blks[DMA_BLKS_POOL_SIZE];
    CommonCryptoHandle cryptoHandle;
#endif

    bdrm_encr_state  asf_encr;
    
    bdrm_mp4_se se[MAX_SAMPLE_ENC_BOX];  /* Sample Encyrption box needed to decrypt the current fragment */
    bmp4_scheme_info *pSchemeInfo;       /* Decryption scheme inforamation needed to decrypt the current fragment's samples. */

    NEXUS_KeySlotHandle keyCtr;
    NEXUS_KeySlotHandle keyWmdrm;

    uint8_t repl_data[BDRMND_SAMPLE_ID_LEN];
    batom_checkpoint start_repl;

} bdrm_decryptor;

bdrm_err bdrm_content_handler(bdrm_t drm);

#ifdef __cplusplus
}
#endif

#endif /*__PRDY_CORE_PRIV_H__*/


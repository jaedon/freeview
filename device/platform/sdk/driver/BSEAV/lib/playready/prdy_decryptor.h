/***************************************************************************
 *     Copyright (c) 2002-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: prdy_decryptor.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 11/1/11 9:51a $
 *
 * Module Description:
 *
 * Playready library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/playready/prdy_decryptor.h $
 * 
 * 4   11/1/11 9:51a hugost
 * SW7425-1625: Added MP4 fragment support
 * 
 * 3   10/25/11 3:18p piyushg
 * SW7425-1607: Added file header to track revision history.
 ***************************************************************************/

#ifndef _DRM_DECRYPTOR_H__
#define _DRM_DECRYPTOR_H__

#include <prdy_core.h>
#include "berr.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* drm decryptor struct magic val */
#define BDRM_DCRYPT_MAGIC    (0x1234)
    
/*
** The BDRM decryptor object. 
*/
typedef struct bdrm_decryptor* bdrm_decryptor_t;

bdrm_err bdrm_decryptor_assign (
    bdrm_decryptor_t hDecryptor,            /* [in] the decryptor */ 
    bdrm_t drm                              /* [in] the DRM context */
    );

bdrm_err
bdrm_decryptor_set_encr_state (
    bdrm_decryptor_t hDecryptor,            /* [in] the decryptor */ 
    bdrm_encr_state encr                    /* [in] encryption state */ 
    );


void bdrm_decryptor_destroy_se(bdrm_decryptor_t hDecryptor);

/*
** Init the content decryptor object. The drm context must
** have been previously bound to the license. 
*/ 

bdrm_err bdrm_decryptor_create( 
    bdrm_decryptor_t* decryptor             /* [in] The out context */
    );

/* 
** Decrypt the given payload in-place. 
*/

void bdrm_decryptor_packet_filter( 
    bdrm_decryptor_t hDecryptor,            /* [in] The content decryptor object */
	size_t media_obj_length,
    batom_cursor *payload,
	const struct basf_payload_info *info
    );

/*
** Destroy the content decryptor object. Free up resources 
** acquired during the open. 
*/

void bdrm_decryptor_destroy(
    bdrm_decryptor_t hDecryptor             /* [in] The content decryptor object */
    );

/* 
** Callback function to be called from basf demux
*/
void b_asf_drm_payload(
    void *stream_cnxt,                      /* [in] Stream context */
    batom_accum_t source,                   /* [in] Atom source handle */
    batom_cursor *payload,                  /* [in] Cursor to actual payload */
    batom_cursor *replicated_data,          /* [in] Payload replicated data (not used for PD) */
    const struct basf_payload_info *info    /* [in] ASF payload metadata */
    );


void b_asf_drm_media_object (void *payload_cnxt, batom_cursor *first_payload,
                      const struct basf_payload_info *info);
/* 
** Func to set up struct magic
*/
void bdrm_decryptor_set_magic(
    bdrm_decryptor_t dcrypt,                /* [in] Decryptor handle */
    unsigned int magic                      /* [in] Debug magic val */
    );

/* 
** Func to get the validity of the struct
*/
unsigned int bdrm_decryptor_valid(
    bdrm_decryptor_t dcrypt                 /* [in] Decryptor handle*/
    );

#ifdef __cplusplus
}
#endif

#endif /* _DRM_DECRYPTOR_H__ */


/***************************************************************************
*     Copyright (c) 2011-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
*******************************************************************************/

#ifndef __KERNEL__
#include <stdio.h>         /*  std includes */
#include "asfpars.h"
#include <string.h>
#include <unistd.h>
#include <errno.h>
#else
#include <linux/string.h>
#endif


#include "bstd.h"           /* brcm includes */
#include "bkni.h"
#include "blst_list.h"
#include "bfifo.h"
#include "prdy_decryptor.h"

#include "prdy_priv.h" 

BDBG_MODULE(bdrm_asf);                              
#define BDBG_MSG_TRACE(x)                       
#ifdef PROFILE
    #undef BDBG_MSG
    #define BDBG_MSG(x)
#endif

/* DRM header GUIDs
*/
static const basf_guid
basf_content_encr_guid = 
{{0x22, 0x11, 0xB3, 0xFB, 0xBD, 0x23, 0x11, 0xD2, 0xB4, 0xB7, 0x00, 0xA0, 0xC9, 0x55, 0xFC, 0x6E}},
basf_content_encr_ext_guid = 
{{0x29, 0x8A, 0xE6, 0x14, 0x26, 0x22, 0x4C, 0x17, 0xB9, 0x35, 0xDA, 0xE0, 0x7E, 0xE9, 0x28, 0x9C}},
basf_content_digi_sign_guid = 
{{0x22, 0x11, 0xB3, 0xFC, 0xBD, 0x23, 0x11, 0xD2, 0xB4, 0xB7, 0x00, 0xA0, 0xC9, 0x55, 0xFC, 0x6E}},
/*PLAYREADY*/
basf_protection_system_identifier_guid =
{{0x9A, 0x04, 0xF0, 0x79, 0x98, 0x40, 0x42, 0x86, 0xAB, 0x92, 0xE6, 0x5B, 0xE0, 0x88, 0x5F, 0x95}},
basf_content_protection_system_microsoft_playready_guid =
{{0xF4, 0x63, 0x70, 0x10, 0x03, 0xC3, 0x42, 0xCD, 0xB9, 0x32, 0xB4, 0x8A, 0xDF, 0x3A, 0x6A, 0x54}},
basf_streamtype_playready_encrypted_command_media_guid =
{{0x86, 0x83, 0x97, 0x3A, 0x66, 0x39, 0x46, 0x3A, 0xAB, 0xD7, 0x64, 0xF1, 0xCE, 0x3E, 0xEA, 0xE0}};


static void 
b_asf_aes_ctr_payload (void* payload_cnxt, batom_accum_t source, 
    batom_cursor* first_payload, batom_cursor* replicated_data, 
    const struct basf_payload_info* info);

static void 
b_asf_wmdrm_payload (void* payload_cnxt, batom_accum_t source, 
    batom_cursor* first_payload, batom_cursor* replicated_data, 
    const struct basf_payload_info* info);

static void b_asf_aes_ctr_media_object (void *payload_cnxt, batom_cursor *first_payload,
                      const struct basf_payload_info *info);


#if !defined(DRM_SCAN_ASF) && defined(__KERNEL__)
unsigned
basf_read_arr(batom_cursor *c, basf_byte *str, unsigned len)
{
    unsigned i;    

#ifndef __HAS_NO_BIOVEC_INTERNALS__
    for(i=0; i < len; i++) {
        BATOM_NEXT(str[i],c);
    }

#else /* __HAS_NO_BIOVEC_INTERNALS__ */

    i = batom_cursor_copy(c, str, len);

#endif /* __HAS_NO_BIOVEC_INTERNALS__ */

    return (i);
}

/* read a zero-terminated string from the current position
*/
void 
basf_read_str(batom_cursor *c, basf_byte *str, unsigned len)
{
    unsigned i = basf_read_arr(c, str, len-1);
    str[i] = '\0';

    return;
}

#endif



#ifdef DEBUG
/* Debug print for content encrypt object
*/
static void
b_asf_print_content_encr_object (struct basf_cencr_object *encr)
{
    BDBG_ASSERT(encr != NULL);
    if (encr != NULL) {
        BDBG_MSG(("encr obj: sec_dlen=%u, sec[0]=%.2X sec[-1]=%.2X prot_tlen=%u" 
        " prot_type=\"%s\" keyid_len=%u keyid=\"%s\" licurl_len=%u"
        " licurl=\"%s\"\n", 
        encr->sec_dlen, encr->sec_data[0], 
        encr->sec_data[encr->sec_dlen-1], encr->prot_tlen, 
        encr->prot_type, encr->keyid_len, encr->keyid, 
        encr->licurl_len, encr->licurl));    
    }
}
#endif

/* Extract encryption object from asf header
*/
static unsigned int
b_asf_parse_content_encr_object (struct basf_parser_object_handler *handler, batom_cursor* cursor)
{
    unsigned int ret = 0;
    basf_cencr_object *encr;    
    basf_drm_object_handler* drmh = ((struct basf_drm_object_handler *)handler);

    BDBG_MSG(("%s - entering", __FUNCTION__));

    /* extract handle to context */    
    encr = &drmh->ctx->headers.asf.encr;

    encr->sec_dlen = basf_read_dword(cursor);           /* secret */
    encr->sec_data = BKNI_Malloc(encr->sec_dlen);
    basf_read_arr(cursor, encr->sec_data, encr->sec_dlen);

    encr->prot_tlen = basf_read_dword(cursor)+1;        /* protection type */
    encr->prot_type = BKNI_Malloc(encr->prot_tlen);
    basf_read_str(cursor, encr->prot_type, encr->prot_tlen);

    encr->keyid_len = basf_read_dword(cursor)+1;        /* key id */
    encr->keyid = BKNI_Malloc(encr->keyid_len);
    basf_read_str(cursor, encr->keyid, encr->keyid_len);

    encr->licurl_len = basf_read_dword(cursor)+1;       /* license url */
    encr->licurl = BKNI_Malloc(encr->licurl_len);
    basf_read_str(cursor, encr->licurl, encr->licurl_len);

    if (encr->keyid_len != 0 && encr->licurl_len != 0) {
        encr->state = bdrm_hdr_read_ok;
        bdrm_content_handler(drmh->ctx);
#ifdef DEBUG
        b_asf_print_content_encr_object(encr);
#endif /* DEBUG */
    }

    BDBG_MSG(("%s - exiting ret %x", __FUNCTION__, ret));
    return(ret);
}

#ifdef DEBUG
/* Debug print for content extended encrypt object
*/
static void
b_asf_print_content_encr_ext_object (struct basf_xcencr_object *xencr)
{
    BDBG_ASSERT(xencr != NULL);
    if (xencr != NULL) {
        BDBG_MSG(("xencr obj: size=%u data[0]=%.2X data[-1]=%.2X\n", 
        xencr->size, xencr->data[0], 
        xencr->data[xencr->size-1]));
    }
}
#endif

/* Extract extended encryption object from asf header
*/
static unsigned int
b_asf_parse_content_encr_ext_object (struct basf_parser_object_handler *handler, batom_cursor* cursor)
{   
    unsigned int ret = 0;
    struct basf_xcencr_object *xencr;
    basf_drm_object_handler* drmh = ((struct basf_drm_object_handler *)handler);

    BDBG_MSG(("%s - entering", __FUNCTION__));

    /* extract handle to context */
    xencr = &drmh->ctx->headers.asf.xencr;

    /* extended encryption data */
    xencr->size = basf_read_dword(cursor);
    xencr->data = BKNI_Malloc(xencr->size);
    basf_read_arr(cursor, xencr->data, xencr->size);

    if (xencr->size != 0) {
        xencr->state = bdrm_hdr_read_ok;
        bdrm_content_handler(drmh->ctx);
#ifdef DEBUG
        b_asf_print_content_encr_ext_object(xencr);
#endif /* DEBUG */    
    }

    BDBG_MSG(("%s - exiting ret %x", __FUNCTION__, ret));
    return(ret);
}

#ifdef DEBUG
/* Debug print for content digital signature object
*/
static void
b_asf_print_digi_sign_object (struct basf_digsign_object *digsign)
{
    BDBG_ASSERT(digsign != NULL);
    if (digsign != NULL) {
        BDBG_MSG(("digi sign obj: size=%u type=%u data[0]=%.2X data[-1]=%.2X\n",
        digsign->len, digsign->type, digsign->data[0], 
        digsign->data[digsign->len-1]));
    }
}
#endif

/* Extract digital signature object from asf header
*/
static unsigned int
b_asf_parse_digi_sign_object (struct basf_parser_object_handler *handler, batom_cursor* cursor)
{
    unsigned int ret = 0;
    struct basf_digsign_object *digsign;
    basf_drm_object_handler* drmh = ((struct basf_drm_object_handler *)handler);

    BDBG_MSG(("%s - entering", __FUNCTION__));

    /* extract handle to context */    
    digsign = &drmh->ctx->headers.asf.sign;

    digsign->type = basf_read_dword(cursor);
    digsign->len = basf_read_dword(cursor);

    digsign->data = BKNI_Malloc(digsign->len);
    basf_read_arr(cursor, digsign->data, digsign->len);

    if (digsign->len != 0) {
        digsign->state = bdrm_hdr_read_ok;
        bdrm_content_handler(drmh->ctx);
#ifdef DEBUG
        b_asf_print_digi_sign_object(digsign);
#endif /* DEBUG */
    }

    BDBG_MSG(("%s - exiting ret %x", __FUNCTION__, ret));
    return(ret);
}

#ifdef DEBUG
/* Debug print for content digital signature object
*/
static void
b_asf_print_psi_object (struct basf_psi_object *psi)
{
    BDBG_ASSERT(psi != NULL);
    if (psi != NULL) {
        BDBG_MSG(("protection system identifier obj: system version=%u size=%u data[0]=%.2X data[-1]=%.2X\n",
        psi->sysversion, psi->size, psi->data[0], 
        psi->data[psi->size-1]));
    }
}
#endif

/* PLAYREADY: Extract protection system identifier object from asf header
*/
static unsigned int
b_asf_parse_psi_object (struct basf_parser_object_handler *handler, batom_cursor* cursor)
{
    unsigned int ret = 0;
    struct basf_psi_object *psi;
    basf_drm_object_handler* drmh = ((struct basf_drm_object_handler *)handler);

    BDBG_MSG(("%s - entering", __FUNCTION__));

    /* extract handle to context */    
    psi = &drmh->ctx->headers.asf.psi;

    basf_read_guid(cursor, &psi->systemId);
    psi->sysversion = basf_read_dword(cursor);
    psi->size = basf_read_dword(cursor);

    psi->data = BKNI_Malloc(psi->size);
    basf_read_arr(cursor, psi->data, psi->size);

    if (psi->size != 0) {
        psi->state = bdrm_hdr_read_ok;
        bdrm_content_handler(drmh->ctx);
#ifdef DEBUG
        b_asf_print_psi_object(psi);
#endif /* DEBUG */
    }

    BDBG_MSG(("%s - exiting ret %x", __FUNCTION__, ret));
    return(ret);
}

void 
b_asf_wmdrm_payload (void* payload_cnxt, batom_accum_t source, 
    batom_cursor* first_payload, batom_cursor* replicated_data, 
    const struct basf_payload_info* info)
{
    bdrm_decryptor_t hDecryptor = NULL;
    size_t media_obj_len = info->media_object_size;

    BDBG_MSG(("%s - entering", __FUNCTION__));
    BDBG_MSG_TRACE(("default filter [%d] stream_num %d key_frame %d payload_length %d replicated_data_len %d media_obj_len [%d] \
                   offset_info_media_obj %d ", dfrun++, info->stream_number, info->key_frame,info->payload_length, info->replicated_data_length, media_obj_len, info->offset_into_media_object));

    BSTD_UNUSED(payload_cnxt);
    BSTD_UNUSED(source);
    BSTD_UNUSED(replicated_data);

    BDBG_ASSERT(media_obj_len != 0);
    BSTD_UNUSED(media_obj_len);
    BDBG_ASSERT(first_payload != NULL);

    if (first_payload != NULL) {
        hDecryptor = ((bdrm_decryptor_t)payload_cnxt);
        bdrm_decryptor_packet_filter(hDecryptor, info->payload_length,
                                       first_payload, /* repl_data, */
                                       info);   /* call the actual decrypt function */
    }
    BDBG_MSG(("%s - exiting", __FUNCTION__));
}

void 
b_asf_aes_ctr_payload (void* payload_cnxt, batom_accum_t source, 
    batom_cursor* first_payload, batom_cursor* replicated_data, 
    const struct basf_payload_info* info)
{
    bdrm_decryptor_t hDecryptor = ((bdrm_decryptor_t)payload_cnxt);
    BDBG_MSG(("%s - entering", __FUNCTION__));
    BSTD_UNUSED(source);
    BSTD_UNUSED(first_payload);
    BSTD_UNUSED(info);
    BATOM_SAVE(replicated_data, &hDecryptor->start_repl);

    batom_cursor_skip(replicated_data, info->replicated_data_length - BDRMND_SAMPLE_ID_LEN);

    batom_cursor_copy(replicated_data, &hDecryptor->repl_data, BDRMND_SAMPLE_ID_LEN);

    batom_cursor_rollback(replicated_data, &hDecryptor->start_repl);
    BDBG_MSG(("%s - exiting", __FUNCTION__));
}

/* called from the basf demux, when drm is enabled, to decrypt drm payloads
*/
void 
b_asf_drm_payload (void* payload_cnxt, batom_accum_t source, 
    batom_cursor* first_payload, batom_cursor* replicated_data, 
    const struct basf_payload_info* info)
{
    bdrm_decryptor_t hDecryptor = ((bdrm_decryptor_t)payload_cnxt);
    BDBG_MSG(("%s - entering", __FUNCTION__));

    if(hDecryptor != NULL){
        switch(hDecryptor->asf_encr){
            case bdrm_encr_wmdrm:
                b_asf_wmdrm_payload(payload_cnxt, source, first_payload, replicated_data, info);
                break;
            case bdrm_encr_aes_ctr:
                b_asf_aes_ctr_payload(payload_cnxt, source, first_payload, replicated_data, info);
                break;
            default:
                /* Ignore */
                break;
        }
    }
    BDBG_MSG(("%s - exiting", __FUNCTION__));
}

void b_asf_aes_ctr_media_object (void *payload_cnxt, batom_cursor *first_payload,
                      const struct basf_payload_info *info)
{
    bdrm_decryptor_t hDecryptor = ((bdrm_decryptor_t)payload_cnxt);
    size_t media_obj_len = info->media_object_size;

    BDBG_MSG(("%s - entering", __FUNCTION__));
    if(hDecryptor->asf_encr == bdrm_encr_aes_ctr) {

        BDBG_MSG(("default filter stream_num %d key_frame %d payload_length %d replicated_data_len %d media_obj_len [%d] \
					    offset_info_media_obj %d ", info->stream_number, info->key_frame,info->payload_length, info->replicated_data_length, media_obj_len, info->offset_into_media_object));
        BSTD_UNUSED(payload_cnxt);

        BDBG_ASSERT(media_obj_len != 0);
        BDBG_ASSERT(first_payload != NULL);

        if (first_payload != NULL) {
            bdrm_decryptor_packet_filter(hDecryptor, media_obj_len,
                                           first_payload, /* repl_data, */
                                           info);   /* call the actual decrypt function */
        }
    }
    else {
        /* Ignore */
    }
    BDBG_MSG(("%s - exiting", __FUNCTION__));
}

void b_asf_drm_media_object (void *payload_cnxt, batom_cursor *first_payload,
                      const struct basf_payload_info *info)
{
    bdrm_decryptor_t hDecryptor = ((bdrm_decryptor_t)payload_cnxt);

    BDBG_MSG(("%s - entering", __FUNCTION__));
    if(hDecryptor != NULL){
        b_asf_aes_ctr_media_object (payload_cnxt, first_payload, info);
    }
    BDBG_MSG(("%s - exiting", __FUNCTION__));
}
/* Called to determine if drm exists in this asf. Now read from the header list enum.
** Will be implemented in the same means as the asfhdr lib.
*/
bdrm_encr_state 
b_asf_is_encrypted (bdrm_t drm)                                  
{
    bdrm_encr_state ret = bdrm_encr_none;

    BDBG_MSG(("%s - entering", __FUNCTION__));
    BDBG_ASSERT(drm != NULL);
    if (drm != NULL) {   
        if ((bdrm_hdr_read_ok == drm->headers.asf.encr.state) &&
            (bdrm_hdr_read_ok == drm->headers.asf.xencr.state) && 
            (bdrm_hdr_read_ok == drm->headers.asf.sign.state))
        {
            /* WMDRM encrypted */
            BDBG_MSG(("WMDRM10 encryption found\n"));
            ret = bdrm_encr_wmdrm;
        }
        else if ((bdrm_hdr_read_ok == drm->headers.asf.psi.state))
        {
            /* Playready encrypted */
            BDBG_MSG(("PLAYREADY encryption found\n"));
            ret = bdrm_encr_aes_ctr;
        }
    
        /* Update decryptor encryption state.*/
        bdrm_decryptor_set_encr_state(drm->dcrypt, ret);
    }
    BDBG_MSG(("%s - exiting ret %x", __FUNCTION__, ret));
    return(ret);
}   

/* Content encryption object handler callback
*/
basf_parser_handler_action
b_asf_handler_cencr_object_cb (struct basf_parser_object_handler *handler, batom_t atom)
{
    batom_cursor cursor;
    BDBG_MSG(("%s - entering", __FUNCTION__));
    BDBG_MSG(("handler_content_cencr_object_cb (%d)", batom_len(atom)));

    batom_cursor_from_atom(&cursor, atom);
    b_asf_parse_content_encr_object(handler, &cursor);

    BDBG_MSG(("%s - exiting", __FUNCTION__));
    return(basf_parser_handler_action_none);
}

/* Extended content encryption object handler callback
*/
basf_parser_handler_action
b_asf_handler_xencr_object_cb (struct basf_parser_object_handler *handler, batom_t atom)
{
    batom_cursor cursor;
    BDBG_MSG(("%s - entering", __FUNCTION__));
    BDBG_MSG(("handler_content_xencr_object (%d)", batom_len(atom)));

    batom_cursor_from_atom(&cursor, atom);
    b_asf_parse_content_encr_ext_object(handler, &cursor);

    BDBG_MSG(("%s - exiting", __FUNCTION__));
    return(basf_parser_handler_action_none);
}

/* Digital signature object handler callback
*/
basf_parser_handler_action
b_asf_handler_digsign_object_cb (struct basf_parser_object_handler *handler, batom_t atom)
{
    batom_cursor cursor;
    BDBG_MSG(("%s - entering", __FUNCTION__));
    BDBG_MSG(("handler_content_digsign_object (%d)", batom_len(atom)));

    batom_cursor_from_atom(&cursor, atom);
    b_asf_parse_digi_sign_object(handler, &cursor);

    BDBG_MSG(("%s - exiting", __FUNCTION__));
    return(basf_parser_handler_action_none);
}


/* PLAYREADY : Protection system identifier object handler callback
*/
basf_parser_handler_action
b_asf_handler_psi_object_cb (struct basf_parser_object_handler *handler, batom_t atom)
{
    batom_cursor cursor;
    BDBG_MSG(("%s - entering", __FUNCTION__));
    BDBG_MSG(("handler_content_psi_object (%d)", batom_len(atom)));
    
    batom_cursor_from_atom(&cursor, atom);
    b_asf_parse_psi_object(handler, &cursor);

    BDBG_MSG(("%s - exiting", __FUNCTION__));
    return(basf_parser_handler_action_none);
}

/* Internal cleanup function. Destroys dyn allocated objects.
*/
static void
b_asf_destroy_drm_headers (bdrm_t drm)
{

    basf_drm_headers* hdrs = &drm->headers.asf;
    BDBG_MSG(("%s - entering", __FUNCTION__));


    if (hdrs->encr.sec_data != NULL) { 
        BKNI_Free(hdrs->encr.sec_data);   /* Free dyn memalloc-ed stuff */
    }
    
    if (NULL != hdrs->encr.prot_type) { 
        BKNI_Free(hdrs->encr.prot_type);  /* Free dyn memalloc-ed stuff */
    }
    
    if (NULL != hdrs->encr.keyid) { 
         BKNI_Free(hdrs->encr.keyid);     /* Free dyn memalloc-ed stuff */
    }
    
    if (NULL !=hdrs->xencr.data) { 
        BKNI_Free(hdrs->xencr.data);     /* Free dyn memalloc-ed stuff */
    }
    
    if (NULL != hdrs->sign.data) { 
        BKNI_Free(hdrs->sign.data);     /* Free dyn memalloc-ed stuff */
    }
    
	/*PLAYREADY*/
    if (NULL != hdrs->psi.data) { 
        BKNI_Free(hdrs->psi.data);     /* Free dyn memalloc-ed stuff */
    }

    BDBG_MSG(("%s - exiting", __FUNCTION__));
    return;
}

/* Clean up the dynamically created obj and other dependend stuff
*/
static void
b_asf_destroy_drm_obj (bdrm_t drm)
{
    BDBG_ASSERT(drm != NULL);
    BDBG_ASSERT(drm->cfg != NULL);

    BDBG_MSG(("%s - entering", __FUNCTION__));
    if (drm != NULL) {
        if ((drm->cfg != NULL) && (drm->cfg->asf != NULL) && 
            (drm->asf_handler.state == basf_drm_handlers_init)) {
                basf_parser_remove_object_handler(drm->cfg->asf, &drm->asf_handler.encr_handler.handler);
                basf_parser_remove_object_handler(drm->cfg->asf, &drm->asf_handler.xencr_handler.handler);
                basf_parser_remove_object_handler(drm->cfg->asf, &drm->asf_handler.digsign_handler.handler);      
                basf_parser_remove_object_handler(drm->cfg->asf, &drm->asf_handler.psi_handler.handler);
        }
    }
    
    BDBG_MSG(("%s - exiting", __FUNCTION__));
    return;
}


/* Init headers to known state
*/
static bdrm_err
b_asf_reset_headers (bdrm_t drm)
{   
    bdrm_err ret = bdrm_err_fail;
    BDBG_ASSERT(drm != NULL);
    BDBG_MSG(("%s - entering", __FUNCTION__));

    if (drm != NULL) {
        drm->headers.asf.encr.state = bdrm_hdr_none;
        drm->headers.asf.xencr.state = bdrm_hdr_none;
        drm->headers.asf.sign.state = bdrm_hdr_none;
        drm->headers.asf.psi.state = bdrm_hdr_none;
        drm->asf_handler.state = basf_drm_handlers_uninit;
        ret = bdrm_err_ok;   
    }
    
    BDBG_MSG(("%s - exiting ret %x", __FUNCTION__, ret));
    return(ret);
}

#ifdef DRM_SCAN_ASF
/* Control function for the asf parser loop. If all drm headers are read,
** we stop the main asf reader loop. Used only when ext_scanning is not set.
*/
static basf_feed_control 
b_asf_feed_state_handler(void* context)
{
    basf_feed_control ret = basf_feed_continue;
    bdrm_t drm = (bdrm_t)context;

    BDBG_MSG(("%s - entering", __FUNCTION__));
    BDBG_ASSERT(drm != NULL);

    if ((drm != NULL) && (b_asf_is_encrypted(drm) >= bdrm_encr_wmdrm)) {
        BDBG_MSG(("time to stop loop here"));
        ret = basf_feed_stop;
    }
    
    BDBG_MSG(("%s - exiting ret %x", __FUNCTION__, ret));
    return(ret);
}
#endif

void
b_asf_init_handlers (bdrm_t drm)
{
 
    BDBG_ASSERT(drm != NULL);
    BDBG_ASSERT(drm->cfg != NULL);
    BDBG_MSG(("%s - entering", __FUNCTION__));

    /* don't launch internal scan if it's done outside of drmcore 
     * */
    if ((drm != NULL) && (drm->cfg != NULL)) {
       
        if(drm->cfg->asf)
        {
        basf_parser_install_object_handler(drm->cfg->asf, &drm->asf_handler.encr_handler.handler, 
            &basf_content_encr_guid, b_asf_handler_cencr_object_cb);
       
        basf_parser_install_object_handler(drm->cfg->asf, &drm->asf_handler.xencr_handler.handler, 
            &basf_content_encr_ext_guid, b_asf_handler_xencr_object_cb);
       
        basf_parser_install_object_handler(drm->cfg->asf, &drm->asf_handler.digsign_handler.handler, 
            &basf_content_digi_sign_guid, b_asf_handler_digsign_object_cb);

        /*PLAYREADY*/
        basf_parser_install_object_handler(drm->cfg->asf, &drm->asf_handler.psi_handler.handler,
            &basf_protection_system_identifier_guid, b_asf_handler_psi_object_cb);
        
        /* set context for each header handlers */
        drm->asf_handler.encr_handler.ctx = drm;
        drm->asf_handler.xencr_handler.ctx = drm;
        drm->asf_handler.digsign_handler.ctx = drm;
        drm->asf_handler.psi_handler.ctx = drm;

        drm->asf_handler.state = basf_drm_handlers_init;       /* mark handlers as initialized */
        }
        else
        	   drm->asf_handler.state = basf_drm_handlers_uninit;

#ifdef DRM_SCAN_ASF
        if (!drm->cfg->ext_asf_scanning) {
            basf_install_feed_handler(drm->asfp, b_asf_feed_state_handler, (void*)drm);        
            basf_scan_asf(drm->asfp);                       /* read drm headers */
        }
#endif
        drm->ctx_state = bdrm_ctx_running;                  /* signal running state */        
   }

    BDBG_MSG(("%s - exiting", __FUNCTION__));
    return;
}

void
b_asf_get_xml_info (bdrm_t drm, uint8_t **pbXML, uint32_t *cbXML )
{
    BDBG_ASSERT(drm != NULL);
    BDBG_ASSERT(pbXML != NULL);
    BDBG_ASSERT(cbXML != NULL);
    BDBG_MSG(("%s - entering", __FUNCTION__));

    if(bdrm_hdr_read_ok == drm->headers.asf.psi.state){
        /* We have a playready object, get xml info from PSI object */
        *pbXML = drm->headers.asf.psi.data; *cbXML = drm->headers.asf.psi.size;
    }
    else if (bdrm_hdr_read_ok == drm->headers.asf.xencr.state){
        /* We have a WMDRM object, get xml info from extended encrypted object */
        *pbXML = drm->headers.asf.xencr.data; *cbXML = drm->headers.asf.xencr.size;
    } else {
        *pbXML = NULL; *cbXML = 0;
    }

    BDBG_MSG(("%s - exiting", __FUNCTION__));
    return;
}


/* update the asf context with the decrypt data, called from the settop throttle
*/
void bdrm_update_asf_stream_cfg (void* dcrypt_ctx, unsigned stream_id,
    basf_stream_cfg *cfg)
{
    bdrm_decryptor_t drm_dcrypt = NULL;
    BSTD_UNUSED(dcrypt_ctx);
    BSTD_UNUSED(stream_id);
    BDBG_ASSERT(cfg);
    BDBG_MSG(("%s - entering", __FUNCTION__));

    if (cfg != NULL) {
        
        drm_dcrypt = (*(bdrm_decryptor_t*)(dcrypt_ctx));
        cfg->stream_cnxt = (*(void**)(dcrypt_ctx)); 

        if (drm_dcrypt != NULL) {
            cfg->media_object = b_asf_drm_media_object;
            cfg->payload = b_asf_drm_payload;
            #ifndef __KERNEL__
            BDBG_MSG(("STREAM_CTX UPDATE ASF = 0x%x valid=0x%x\n", 
                (unsigned int)cfg->stream_cnxt,
                bdrm_decryptor_valid(drm_dcrypt)));
            #endif
        }
    }

    BDBG_MSG(("%s - exiting", __FUNCTION__));
} 

void bdrm_init_asf_container (bdrm_t drm)
{
    BDBG_ASSERT(drm != NULL);

    BDBG_MSG(("%s - entering", __FUNCTION__));
    drm->cnt_fp.reset_headers    = b_asf_reset_headers;
    drm->cnt_fp.destroy_headers  = b_asf_destroy_drm_headers;
    drm->cnt_fp.destroy_drm_objs = b_asf_destroy_drm_obj;
    drm->cnt_fp.is_drmencrypted  = b_asf_is_encrypted;
    drm->cnt_fp.get_xml_info     = b_asf_get_xml_info;
    BDBG_MSG(("%s - exiting", __FUNCTION__));
    return;
}


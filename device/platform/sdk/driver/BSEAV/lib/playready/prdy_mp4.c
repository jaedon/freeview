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

#include <drmcipher.h>
#include <byteorder.h>
#include "prdy_decryptor.h"
#include "prdy_priv.h" 
#include "bmp4_fragment_demux.h"
#include "nexus_memory.h"

BDBG_MODULE(bdrm_mp4);
#define BDBG_MSG_TRACE(x)                       
#ifdef PROFILE
    #undef BDBG_MSG
    #define BDBG_MSG(x)
#endif

static const bmedia_guid
/*PLAYREADY*/
bmedia_protection_system_identifier_guid =
{{0x9A, 0x04, 0xF0, 0x79, 0x98, 0x40, 0x42, 0x86, 0xAB, 0x92, 0xE6, 0x5B, 0xE0, 0x88, 0x5F, 0x95}};

#define BMP4_VISUAL_ENTRY_SIZE   (78)  /* Size of a Visual Sample Entry in bytes */
#define BMP4_AUDIO_ENTRY_SIZE    (28)  /* Size of an Audio Sample Entry in bytes */

#define BMP4_PROTECTIONSCHEMEINFO BMP4_TYPE('s','i','n','f')
#define BMP4_ORIGINALFMT          BMP4_TYPE('f','r','m','a')
#define BMP4_IPMPINFO             BMP4_TYPE('i','m','i','f')
#define BMP4_SCHEMETYPE           BMP4_TYPE('s','c','h','m')
#define BMP4_SCHEMEINFO           BMP4_TYPE('s','c','h','i')
#define BMP4_PIFF_SCHEMETYPE      BMP4_TYPE('p','i','f','f')

#define BMP4_NETFLIX_PSSH         BMP4_TYPE('p','s','s','h')
#define BMP4_PROPRITARY_BMP4      BMP4_TYPE('b','m','p','4')

#define CONVERT_ALG_ID_TO_DRM_ENCR_STATE(_alg_id, _state) do { \
    if(_alg_id == 0) _state = bdrm_encr_none;                    \
    else if(_alg_id == 1) _state = bdrm_encr_aes_ctr;            \
    else _state = bdrm_encr_aes_cbc;  }while(0)

#define GET_TRACK_INFO(_pChild, _list, _link, _track_ID) do {                                                     \
        for (_pChild = BLST_D_FIRST(_list); _pChild != NULL; _pChild = BLST_D_NEXT(_pChild, _link)) {             \
            if(_pChild != NULL){                                                                                  \
                if(_pChild->track_ID == _track_ID){                                                               \
                    break;                                                                                        \
                }                                                                                                 \
                else continue;                                                                                    \
            }                                                                                                     \
            else break;                                                                                           \
        }                                                                                                         \
    } while(0)


/* Get a the se box bound to a given track id. Return null if none are found. */
static bdrm_mp4_se * bdrm_get_se_ptr(bdrm_decryptor * pDecryptor, uint32_t track_id)
{
    int ii;

    for(ii = 0; ii < MAX_SAMPLE_ENC_BOX; ii++){
        if(pDecryptor->se[ii].track_Id == track_id)
            return  &pDecryptor->se[ii];
    }

    return NULL;
}

/* Get a se box from the pool and bind it to a track id */
static bdrm_mp4_se * bdrm_get_availble_se_ptr(bdrm_decryptor * pDecryptor, uint32_t track_id)
{
    int ii;

    for(ii = 0; ii < MAX_SAMPLE_ENC_BOX; ii++){
        if(pDecryptor->se[ii].inUse == false){
            pDecryptor->se[ii].inUse = true;
            pDecryptor->se[ii].track_Id = track_id;
            return  &pDecryptor->se[ii];
        }
    }

    BDBG_ERR(("%s - To many encrypted track, increase pool", __FUNCTION__));
    return NULL;

}

bdrm_err bdrm_mp4_get_codingname(bdrm_t drm, uint32_t track_ID, uint32_t *pCodingName)
{
    bdrm_err                    rc = bdrm_err_fail;
    bdrm_mp4_track_info_node_t *pChild;

    BDBG_MSG(("%s - entering", __FUNCTION__));

    if(drm && pCodingName ){

        GET_TRACK_INFO(pChild, &drm->headers.mp4.tracks, track_link, track_ID);
        if(pChild){
             *pCodingName = pChild->pScheme->frma.codingname;
             rc = bdrm_err_ok;
        }
    }
    BDBG_MSG(("%s - exiting, rc %d", __FUNCTION__, rc));
    return rc;
}

static
bdrm_err bmp4_parse_sample_enc_box(bdrm_t drm, batom_cursor *cursor, uint32_t track_ID)
{
    bdrm_err     rc = bdrm_err_ok;
    uint32_t     ii, jj;
    bdrm_mp4_se *pSe;
    bdrm_mp4_box_se_sample *pSample;
    bdrm_mp4_box_se_entry *pEntry;
    uint32_t alg_id;
    bdrm_mp4_track_info_node_t *pChild;

    BDBG_MSG(("%s - entering", __FUNCTION__));

    BDBG_ASSERT(drm != NULL);
    BDBG_ASSERT(cursor != NULL);

    pSe = bdrm_get_se_ptr(drm->dcrypt, track_ID);
    if(pSe == NULL){
        pSe = bdrm_get_availble_se_ptr(drm->dcrypt, track_ID);
    }
    BDBG_ASSERT(pSe != NULL);

    if(bmp4_parse_box_extended(cursor, &pSe->extended)){

        if(bmp4_parse_fullbox(cursor, &pSe->fullbox)){
            if(pSe->fullbox.flags & 0x000001){

                alg_id = batom_cursor_uint24_be(cursor);
                CONVERT_ALG_ID_TO_DRM_ENCR_STATE(alg_id, pSe->info.alg_id);
                pSe->info.iv_size = batom_cursor_byte(cursor);
                batom_cursor_copy(cursor, pSe->info.kid, 16);
                drm->dcrypt->pSchemeInfo = &pSe->info; /* Scheme info to use for the current fragment */
            }
            else {
                GET_TRACK_INFO(pChild, &drm->headers.mp4.tracks, track_link, track_ID);
                if(pChild){
                    drm->dcrypt->pSchemeInfo = &pChild->pScheme->te.info; /* Scheme info to use for the current fragment */
                } else {
                    BDBG_ASSERT(false);
                }
            }

            pSe->sample_count = batom_cursor_uint32_be(cursor);
            if(pSe->sample_count != 0){
                if(pSe->sample_count > SAMPLES_POOL_SIZE){
                    BDBG_ERR(("%s - Sample pools too small, increase SAMPLES_POOL_SIZE %x\n", __FUNCTION__));
                    BDBG_ASSERT(false);
                    
                }

                pSample = pSe->samples;

                for(ii = 0; ii < pSe->sample_count; ii++){

                    batom_cursor_copy(cursor, &pSample->iv[8], 8);
                    if(drm->dcrypt->pSchemeInfo->iv_size == 16) batom_cursor_copy(cursor, pSample->iv, 8);
                    if(pSe->fullbox.flags & 0x000002){

                        pSample->nbOfEntries = batom_cursor_uint16_be(cursor);
                        if(pSample->nbOfEntries != 0){

                            if(pSample->nbOfEntries  > MAX_ENTRIES_PER_SAMPLE){
                                BDBG_ERR(("%s - Default nb of entry too small, increase MAX_ENTRIES_PER_SAMPLE %x\n", __FUNCTION__));
                                BDBG_ASSERT(false);
                            }
                            pEntry = pSample->entries;
                            for(jj = 0; jj <  pSample->nbOfEntries; jj++){
                                pEntry->bytesOfClearData = batom_cursor_uint16_be(cursor);
                                pEntry->bytesOfEncData = batom_cursor_uint32_be(cursor);
                                pEntry++;
                            }
                        }
                    }
                    pSample++;
                }
            }
        }
        else {
            rc = bdrm_err_fail;
        }
    }
    else {
        rc = bdrm_err_fail;
    }

    BDBG_MSG(("%s - exiting, rc %d", __FUNCTION__, rc));
    return rc;
}

static
bdrm_err bdrm_parse_mp4_traf_box(bdrm_t drm,  batom_cursor *cursor, bmp4_box *pBox)
{
    bdrm_err     rc = bdrm_err_ok;

    bmp4_box      box;
    size_t        box_hdr_size = 8;
    uint32_t      ii;
    struct batom_checkpoint start;
    bool se_box_found = false;
    bmp4_track_fragment_header traf_hdr;

    BDBG_ASSERT(pBox->type == BMP4_TRACK_FRAGMENT);
    BDBG_MSG(("%s - entering", __FUNCTION__));

    /*  According to the ISO 14496-12 spec:  It is strongly recommended that all header boxes be placed first in their container: these boxes are
        the Movie Header, Track Header, Media Header, and the specific media headers inside the Media
        Information Box (e.g. the Video Media Header). 
                   
        Since we dont have any warranties that it is indeed the case. Look for the track fragement header first. We need to find it first to know
        which track id we are dealing for here. And to choose the IV size accordingly when the Sample Encryption box is parsed. */
    for(ii = 0; ii < pBox->size; ii += (box_hdr_size + box.size))
    {    
        box_hdr_size = bmp4_parse_box(cursor, &box);
        if(box_hdr_size==0) {
            break;
        }

        if(box.type == BMP4_TRACK_FRAGMENT_HEADER) {
            if(!bmp4_parse_track_fragment_header(cursor, &traf_hdr)){
                BDBG_ERR(("%s - bmp4_parse_track_fragment_header failed %x\n", __FUNCTION__));
                goto ErrorExit;
            }

            if(se_box_found) break; 
            else continue;
        } else {

            if(box.type == BMP4_EXTENDED){
                /* Save position of the SE box */
                batom_cursor_save(cursor, &start);
                se_box_found = true;  /* We have found the SE box before the tfhd box */
            }
            /* Not the box we are looking for. Skip over it.*/
            batom_cursor_skip(cursor, box.size - box_hdr_size);
        }
    }
    if(se_box_found) {
        /* Rollback to the position of the SE box. */
        batom_cursor_rollback(cursor, &start);
        if(bmp4_parse_sample_enc_box(drm, cursor, traf_hdr.track_ID) != bdrm_err_ok){
            BDBG_ERR(("%s - bmp4_parse_sample_enc_box failed %x\n", __FUNCTION__));
            goto ErrorExit;
        }
    } 
    else {
        /* No Sample encryption box found. The content is most likely unencrypted */
    }

ErrorExit:
    BDBG_MSG(("%s - exiting", __FUNCTION__));
    return rc;
}

static
bdrm_err bdrm_mp4_parse_moof(bdrm_t drm, batom_cursor *cursor, bmp4_box *pBox)
{

    bdrm_err     rc = bdrm_err_ok;
    bmp4_box     box;
    size_t       box_hdr_size;
    uint32_t     ii;

    BDBG_MSG(("%s - entering", __FUNCTION__));

    BDBG_ASSERT(cursor != NULL);
    BDBG_ASSERT(pBox->type == BMP4_MOVIE_FRAGMENT);
    for(ii = 0; ii < pBox->size; ii += (box_hdr_size + box.size))
    {
        box_hdr_size = bmp4_parse_box(cursor, &box);

        if(box_hdr_size == 0) {
            break;
        }

        switch(box.type){
            case BMP4_TRACK_FRAGMENT:
                rc = bdrm_parse_mp4_traf_box(drm, cursor, &box);
                if(rc != bdrm_err_ok) goto ErrorExit;
                break;
            default :
                /* Not the box we are looking for. Skip over it.*/
                batom_cursor_skip(cursor, box.size - box_hdr_size);
                break;
        }
    }

ErrorExit:
    BDBG_MSG(("%s - exiting, rc %d", __FUNCTION__, rc));
    return rc;
}


/* This function parse either a bmp4 super box, or a traf box, depending on the application context */
void bdrm_parse_mp4_fragment_context(void *user_context, const batom_cursor *cursor, size_t payload_len)
{
    bmp4_box      box;
    bmp4_box      fragment;
    size_t        box_hdr_size = 8;
    uint32_t      ii;
    bdrm_t drm = (bdrm_t)user_context;
    struct batom_checkpoint start;

    BDBG_MSG(("%s - entering", __FUNCTION__));
    BSTD_UNUSED(payload_len);

    /* save cursor position on entry */
    batom_cursor_save(cursor, &start);

    if(bmp4_parse_box((batom_cursor *)cursor, &fragment)){
        
        if(fragment.type == BMP4_TRACK_FRAGMENT)
        {
            /* We received a traf box, parse it to find the sample encryption box*/
            if(bdrm_parse_mp4_traf_box(drm, (batom_cursor *)cursor, &fragment) != bdrm_err_ok){
                BDBG_ERR(("%s - bdrm_parse_mp4_traf_box failed %x\n", __FUNCTION__));
                goto ErrorExit;
            }
        }
        else { 
            /* Not a traf box, check if we are dealing with a bmp4 box propritary */
            if(fragment.type == BMP4_PROPRITARY_BMP4){

                for(ii = 0; ii < fragment.size; ii += (box_hdr_size + box.size)){
                    box_hdr_size = bmp4_parse_box((batom_cursor *)cursor, &box);
                    
                    if(box_hdr_size==0) {
                        break;
                    }

                    switch(box.type){
                        case BMP4_MOVIE_FRAGMENT:
                            if(bdrm_mp4_parse_moof(drm, (batom_cursor *)cursor, &box)!= bdrm_err_ok){
                                BDBG_ERR(("%s - bdrm_parse_mp4_traf_box failed %x\n", __FUNCTION__));
                                goto ErrorExit;
                            }
                            break;
                        default :
                            /* Not the box we are looking for. Skip over it.*/
                            batom_cursor_skip((batom_cursor *)cursor, box.size - box_hdr_size);
                            break;
                    }
                }
            }
            else {
                /* Invalid box. */
            }
        }
    }
    else {
        /* Not a box */
    }

ErrorExit:
    batom_cursor_rollback((batom_cursor*)cursor, &start);
    BDBG_MSG(("%s - exiting", __FUNCTION__));
    return;
}


DRM_RESULT 
do_decrypt_sw_mp4 (bdrm_decryptor_t pDecryptor, uint8_t *pdata, size_t len, uint8_t* pIv, uint64_t byteOffset)
{
    DRM_RESULT dr;
    DRM_AES_COUNTER_MODE_CONTEXT f_pCtrContext;
    BDBG_MSG(("%s - entering", __FUNCTION__));

    NETWORKBYTES_TO_QWORD(f_pCtrContext.qwInitializationVector, &pIv[8], 0)  

    f_pCtrContext.qwBlockOffset = byteOffset >> 4;
    f_pCtrContext.bByteOffset = (DRM_BYTE)(byteOffset & 0xf);

    dr = Drm_Reader_Decrypt( (DRM_DECRYPT_CONTEXT *)(pDecryptor->MsDecryptCtx.rgbBuffer), &f_pCtrContext, pdata, len); 

    BDBG_MSG(("%s - exiting, rc %d", __FUNCTION__, dr));
    return dr;
}

#ifdef PRDY_USE_SW_DECRYPTION
void bdrm_decrypt_mp4_sample(void *user_context, uint32_t track_ID, unsigned sample_no, const batom_cursor *data, size_t payload_len)
{
    bdrm_t drm = (bdrm_t )user_context;
    struct batom_checkpoint start;

    bdrm_mp4_box_se_sample *pSample;
    bmp4_scheme_info *pSchemeInfo;
    bdrm_mp4_se *pSe;
    uint32_t jj = 0;
    
    uint64_t byteOffset = 0;
    DRM_RESULT dr;

    unsigned pos = 0;
    unsigned char* ps;      /* source buffer */
    unsigned int off = 0, copy = 0;

    bdrm_decryptor *pDecryptor;
    uint32_t length;

    BDBG_MSG(("%s - entering", __FUNCTION__));
    BSTD_UNUSED(track_ID);

    if (drm == NULL) {
        BDBG_ERR(("%s - no active drm", __FUNCTION__));
        return;
    }

    if (drm->dcrypt == NULL) {
        BDBG_ERR(("%s - no active decryptor", __FUNCTION__));
        return;
    }

    pDecryptor = drm->dcrypt;

    /* save cursor position on entry */
    batom_cursor_save(data, &start);

    pSe = bdrm_get_se_ptr(pDecryptor, track_ID);
    if (pSe == NULL) {
        return;
    }

    pSchemeInfo = pDecryptor->pSchemeInfo;

    if(pSchemeInfo== NULL){
        return;
    }

    if(pSchemeInfo->alg_id == 0x3)
        return; /* Playready does not suppport AES-CBC*/
    
    if(pSchemeInfo->alg_id == 0x0){
        /* Data in the clear, nothing to do. */
        BDBG_MSG(("Data in the clear, nothing to do\n"));
        return ;
    }

    /* make sure the linear buffer can hold the payload */
    if(payload_len > pDecryptor->dyn.size) {
        Oem_MemHeapFree(pDecryptor->dyn.buf);
        pDecryptor->dyn.buf = Oem_MemHeapAlloc(payload_len);
        pDecryptor->dyn.size = payload_len;
    }
    ps = pDecryptor->dyn.buf;
 
    /* Get IV from the current sample */
    pSample = &pSe->samples[sample_no];

    dr = Drm_Reader_InitDecrypt( (DRM_DECRYPT_CONTEXT *)(pDecryptor->MsDecryptCtx.rgbBuffer), NULL, payload_len); 
    if(dr != 0){
        return ;
    }

    /* Setup dma blocks. */
    if(pSe->fullbox.flags &= 0x000002){
 
        for(jj = 0; jj < pSample->nbOfEntries; jj++){
            batom_cursor_copy((batom_cursor *)data, ps, pSample->entries[jj].bytesOfEncData + pSample->entries[jj].bytesOfClearData);

            dr = do_decrypt_sw_mp4 (pDecryptor,
                               &ps[pSample->entries[jj].bytesOfClearData],
                               pSample->entries[jj].bytesOfEncData,
                               pSample->iv, byteOffset);

            if(dr == 0){
                ps += pSample->entries[jj].bytesOfEncData + pSample->entries[jj].bytesOfClearData;
                byteOffset += pSample->entries[jj].bytesOfEncData;
            } else{
                BDBG_ERR(("%s - do_decrypt_sw_mp4 failed %x\n", __FUNCTION__, dr));
            }

        }
    }
    else {
       if(pSchemeInfo->alg_id == 0x2){
           length = payload_len;

            batom_cursor_copy((batom_cursor *)data, ps, length);
            do_decrypt_sw_mp4 ( pDecryptor,
                                ps,
                                length,
                                pSample->iv, byteOffset);
       }
    }


    /* Rollback to beginning of payload*/
    batom_cursor_rollback((batom_cursor *)data, &start ); 
    ps = pDecryptor->dyn.buf;
    pos = 0;

    /* copy decrypted data back into app */
    if (data->left >= (int)payload_len) {           /* direct copy */
        BDBG_MSG(("pld->left(%d) > info->pld_len(%d)", data->left, payload_len));
        BKNI_Memcpy((uint8_t*)data->cursor, ps, payload_len);
    } else {
        BDBG_MSG(("pld->left(%d) < info->pld_len(%d) (straddle)", data->left, payload_len));            
        for (off=0; pos < payload_len; batom_cursor_skip((batom_cursor *)data, data->left+off)) {    
            /* straddle copy */
            if(BATOM_IS_EOF(data)) {
                BDBG_ASSERT(0); break;                          /* shouldn't happen */
            }
            BDBG_MSG(("straddle copy: %d pos=%d", data->left, pos));
            if (pos+data->left+off <= payload_len) {
                copy = data->left+off;               
            } else {
                copy = (unsigned int)(payload_len-pos);
            }
            BKNI_Memcpy((uint8_t*)data->cursor-off, ps, copy);
            ps += copy;
            pos += copy;
            off = 1;                
        }
    }
    batom_cursor_rollback((batom_cursor*)data, &start);

    BDBG_MSG(("%s - exiting", __FUNCTION__));
}


bdrm_err bdrm_decrypt_aes_ctr_samples(
    bdrm_t drm, 
    uint64_t nonce,
    uint64_t blockCounter,
    size_t  byteOffset,
    const NEXUS_DmaJobBlockSettings *pBlks,
    uint32_t nDmaBlocks
)
{

    BSTD_UNUSED(drm);
    BSTD_UNUSED(nonce);
    BSTD_UNUSED(blockCounter);
    BSTD_UNUSED(byteOffset);
    BSTD_UNUSED(pBlks);
    BSTD_UNUSED(nDmaBlocks);
    BDBG_ERR(("bdrm_decrypt_aes_ctr_samples: NOT IMPLEMENTED with software decryption built option."));
    return bdrm_err_fail;

}
#else

static size_t 
bdrm_atom_cursor_refill(batom_cursor *cursor)
{

    BDBG_ASSERT(cursor->left<=0);
    BDBG_MSG(("%s - entering", __FUNCTION__));

    if (cursor->left==0) {
        unsigned pos;
        const batom_vec *vec;
        BDBG_ASSERT(cursor->vec);
        BDBG_ASSERT(cursor->pos <= cursor->count);
        for(pos=cursor->pos,vec=&cursor->vec[pos];pos < cursor->count;vec++) {
            pos++;
            cursor->pos = pos;
            if (vec->len>0) {
                cursor->left = vec->len;
                cursor->cursor = vec->base;
                BDBG_MSG_TRACE(("b_atom_cursor_refill: %#lx %u:%u %u %#lx", (unsigned long)cursor, cursor->pos, cursor->count, cursor->left, (unsigned long)cursor->cursor));
                return (size_t)cursor->left;
            }
        }

        BDBG_MSG_TRACE(("b_atom_cursor_refill: %#lx %u %u EOF", (unsigned long)cursor, cursor->pos, cursor->count));
        /* reached EOF */
        cursor->left = BATOM_EOF;
    }

    BDBG_MSG(("%s - exiting", __FUNCTION__));
    return 0;
}


/* Map dma blocks to cursor's vectors. 
   This function return the number of bytes references in the dma blocks.
*/
size_t 
bdrm_map_cursor_to_dmablks(batom_cursor *cursor, size_t count, NEXUS_DmaJobBlockSettings *blks, uint32_t nb_blks_avail, uint32_t *nb_blks_used)
{
    size_t left;
    uint32_t ii = 0;

    BDBG_MSG(("%s - entering", __FUNCTION__));
    BDBG_ASSERT(cursor);
    BDBG_ASSERT((int)count>=0);

    for(left=count; ii < nb_blks_avail; ) {
        const uint8_t *src=cursor->cursor;
        int src_left = cursor->left;

        if(src_left>=(int)left) {
            cursor->cursor = src+left;
            cursor->left = src_left-left;
            NEXUS_DmaJob_GetDefaultBlockSettings(&blks[ii]);
            blks[ii].pSrcAddr  = (uint8_t *)src;
            blks[ii].pDestAddr = (uint8_t *)src;
            blks[ii].blockSize = left;

            left = 0;
            ii++;
            break;
        }
        if(src_left>=0) {
            cursor->cursor = src+src_left;
            cursor->left = 0;
            left -= src_left;
            NEXUS_DmaJob_GetDefaultBlockSettings(&blks[ii]);
            blks[ii].pSrcAddr  = (uint8_t *)src;
            blks[ii].pDestAddr = (uint8_t *)src;
            blks[ii].blockSize = src_left;
            ii++;

            if (bdrm_atom_cursor_refill(cursor)==0){
                break;
            }
        } else {
            break;
        }
    }
    (*nb_blks_used) = ii;

    BDBG_MSG(("%s - exiting", __FUNCTION__));
    return count-left;
}

void bdrm_decrypt_mp4_sample(void *user_context, uint32_t track_ID, unsigned sample_no, const batom_cursor *data, size_t payload_len)
{
    bdrm_t drm = (bdrm_t )user_context;
    struct batom_checkpoint start;

    bdrm_mp4_box_se_sample *pSample;
    bmp4_scheme_info *pSchemeInfo;
    bdrm_mp4_se *pSe;
    uint32_t nb_Blks = 1; /* The first block is for external key and iv */
    uint32_t jj = 0;
    uint32_t nb_blks_used;
    NEXUS_Error rc;
    DRM_CIPHER_CONTEXT *cipher_ctx;

    size_t bytes_ref;
    bdrm_decryptor *pDecryptor;
    CommonCryptoJobSettings jobSettings;

#undef COMPARE_SW_HW_DEC
#ifdef COMPARE_SW_HW_DEC
    uint64_t byteOffset = 0;
    uint8_t *dec_sw;
    uint8_t *dec_hw;

    DRM_RESULT dr;
#endif

    BDBG_MSG(("%s - entering", __FUNCTION__));
    if (drm == NULL) {
        BDBG_ERR(("%s - no active drm", __FUNCTION__));
        return;
    }

    if (drm->dcrypt == NULL) {
        BDBG_ERR(("%s - no active decryptor", __FUNCTION__));
        return;
    }

    /* save cursor position on entry */
    batom_cursor_save(data, &start);

#ifdef COMPARE_SW_HW_DEC
    dec_sw = BKNI_Malloc(payload_len);
    dec_hw = BKNI_Malloc(payload_len);

    ps = dec_sw;
    batom_cursor_copy((batom_cursor *)data, dec_sw, payload_len);
    batom_cursor_rollback((batom_cursor *)data, &start);
#endif


    pDecryptor = drm->dcrypt;

    pSe = bdrm_get_se_ptr(pDecryptor, track_ID);
    if (pSe == NULL) {
        return;
    }

    pSchemeInfo = pDecryptor->pSchemeInfo;
    if(pSchemeInfo== NULL){
        return;
    }

    if(pSchemeInfo->alg_id == 0x3)
        return; /* Playready does not suppport AES-CBC*/
    
    if(pSchemeInfo->alg_id == 0x0){
        /* Data in the clear, nothing to do. */
        BDBG_MSG(("Data in the clear, nothing to do\n"));
        return;
    }

    /* Get IV from the current sample */
    pSample = &pSe->samples[sample_no];


#ifdef COMPARE_SW_HW_DEC
    dr = Drm_Reader_InitDecrypt( (DRM_DECRYPT_CONTEXT *)(pDecryptor->MsDecryptCtx.rgbBuffer), NULL, payload_len); 
    if(dr != 0){
        return;
    }
#endif
    /*  Encryption using external key and IV.  Scatter and gather must be used for 
        external key and IV to work */
    cipher_ctx = (DRM_CIPHER_CONTEXT*)(pDecryptor->MsDecryptCtx.rgbBuffer);
    BKNI_Memcpy(pDecryptor->dyn.ekl_buf,   cipher_ctx->rgbContentKey+8, 8);	/* Copy key.  H and L need to be swapped */
    BKNI_Memcpy(pDecryptor->dyn.ekl_buf+8, cipher_ctx->rgbContentKey, 8);	
    BKNI_Memcpy(&pDecryptor->dyn.ekl_buf[16], pSample->iv, 16);
    BKNI_Memset(&pDecryptor->dyn.ekl_buf[16], 0, 8);
    
    
    /* Set Key and IV */
    NEXUS_DmaJob_GetDefaultBlockSettings(&pDecryptor->blks[0]);
    pDecryptor->blks[0].pSrcAddr  = pDecryptor->dyn.ekl_buf;
    pDecryptor->blks[0].pDestAddr = pDecryptor->dyn.ekl_buf;
    pDecryptor->blks[0].blockSize = AES_EKL_SIZE;
    pDecryptor->blks[0].resetCrypto              = true;
    pDecryptor->blks[0].scatterGatherCryptoStart = true;

    NEXUS_FlushCache(pDecryptor->blks[0].pSrcAddr, AES_EKL_SIZE);

    /* Setup dma blocks. */
    if(pSe->fullbox.flags &= 0x000002){

        for(jj = 0; jj < pSample->nbOfEntries; jj++){
            batom_cursor_skip((batom_cursor *)data, pSample->entries[jj].bytesOfClearData);
            bytes_ref = bdrm_map_cursor_to_dmablks((batom_cursor *)data, pSample->entries[jj].bytesOfEncData, &pDecryptor->blks[nb_Blks], DMA_BLKS_POOL_SIZE - 1, &nb_blks_used);
            if(bytes_ref != pSample->entries[jj].bytesOfEncData){
                 BDBG_ERR(("%s - out of DMA blks, return without decrypting", __FUNCTION__));
                 return;
            }

            nb_Blks += nb_blks_used;
            

#ifdef COMPARE_SW_HW_DEC
            dr = do_decrypt_sw_mp4 (pDecryptor,
                               &ps[pSample->entries[jj].bytesOfClearData],
                               pSample->entries[jj].bytesOfEncData,
                               pSample->iv, byteOffset);

            if(dr == 0){
                ps += pSample->entries[jj].bytesOfEncData + pSample->entries[jj].bytesOfClearData;
                byteOffset += pSample->entries[jj].bytesOfEncData;
            }
#endif

        } 
        pDecryptor->blks[nb_Blks-1].scatterGatherCryptoEnd = true;
    }
    else {
       if(pSchemeInfo->alg_id == 0x2){
            bytes_ref = bdrm_map_cursor_to_dmablks((batom_cursor *)data, payload_len, &pDecryptor->blks[1], DMA_BLKS_POOL_SIZE - 1, &nb_blks_used);
            if(bytes_ref != payload_len){
                 BDBG_ERR(("%s - out of DMA blks, return without decrypting", __FUNCTION__));
                 return;
            }
        }


#ifdef COMPARE_SW_HW_DEC
        dr = do_decrypt_sw_mp4 (pDecryptor,
                            dec_sw, payload_len,
                            pSample->iv, byteOffset);
#endif    

        nb_Blks += nb_blks_used;
        pDecryptor->blks[nb_Blks-1].scatterGatherCryptoEnd = true;
    }


    CommonCrypto_GetDefaultJobSettings(&jobSettings);
    jobSettings.keySlot = pDecryptor->keyCtr;

    rc = CommonCrypto_DmaXfer(pDecryptor->cryptoHandle, &jobSettings, pDecryptor->blks, nb_Blks);
    if(rc != NEXUS_SUCCESS)
    {
        BDBG_ERR(("%s - CommonCrypto_DmaXfer failed rc %x\n", __FUNCTION__, rc));
    }
    
#ifdef COMPARE_SW_HW_DEC

    batom_cursor_rollback((batom_cursor *)data, &start);
    batom_cursor_copy((batom_cursor *)data, dec_hw, payload_len);
    /* memcmp*/
    if(BKNI_Memcmp(dec_hw, dec_sw, payload_len) != 0)
    {
        printf("Mismatch\n");
    }

    BKNI_Free(dec_hw);
    BKNI_Free(dec_sw);
#endif

    batom_cursor_rollback((batom_cursor *)data, &start);

    BDBG_MSG(("%s - exiting", __FUNCTION__));
}



bdrm_err bdrm_decrypt_aes_ctr_samples(
    bdrm_t drm, 
    uint64_t nonce,
    uint64_t blockCounter,
    size_t  byteOffset,
    const NEXUS_DmaJobBlockSettings *pBlks,
    uint32_t nDmaBlocks
)
{
    uint32_t nb_Blks = 1;       /* The first block is for external key and iv. The second block might contain padding */
    unsigned nbBlkInHeader = 0; /* header contains the descriptor pointing to the key and possibly an extra block 
                                   for padding when decryption doesn't start on a AES block boundary */
    NEXUS_Error rc;
    DRM_CIPHER_CONTEXT *cipher_ctx;
    bdrm_err dr = bdrm_err_ok;

    bdrm_decryptor *pDecryptor;
    CommonCryptoJobSettings jobSettings;
    unsigned ii = 1;

    if (drm == NULL) {
        BDBG_ERR(("%s - no active drm", __FUNCTION__));
        return dr;
    }

    if (drm->dcrypt == NULL) {
        BDBG_ERR(("%s - no active decryptor", __FUNCTION__));
        return dr;
    }

    pDecryptor = drm->dcrypt;

    /*  Encryption using external key and IV.  Scatter and gather must be used for 
        external key and IV to work */
    cipher_ctx = (DRM_CIPHER_CONTEXT*)(pDecryptor->MsDecryptCtx.rgbBuffer);
    BKNI_Memcpy(pDecryptor->dyn.ekl_buf,   cipher_ctx->rgbContentKey+8, 8);	/* Copy key.  H and L need to be swapped */
    BKNI_Memcpy(pDecryptor->dyn.ekl_buf+8, cipher_ctx->rgbContentKey, 8);	
    
    /*NONCE - CTR must be reversed in the DMA descriptor to CTR - NONCE */
    QWORD_TO_NETWORKBYTES(&pDecryptor->dyn.ekl_buf[24], 0, nonce );
    if(blockCounter != 0){
        QWORD_TO_NETWORKBYTES(&pDecryptor->dyn.ekl_buf[16], 0, blockCounter );
    }
    else {
        BKNI_Memset(&pDecryptor->dyn.ekl_buf[16], 0, 8);
    }

    /* Set Key and IV */
    NEXUS_DmaJob_GetDefaultBlockSettings(&pDecryptor->blks[0]);
    pDecryptor->blks[0].pSrcAddr  = pDecryptor->dyn.ekl_buf;
    pDecryptor->blks[0].pDestAddr = pDecryptor->dyn.ekl_buf;
    pDecryptor->blks[0].blockSize = AES_EKL_SIZE;
    pDecryptor->blks[0].resetCrypto              = true;
    pDecryptor->blks[0].scatterGatherCryptoStart = true;
    pDecryptor->blks[0].cached = true;

    if(byteOffset != 0){
        /*Setup DMA descriptor with dummy data padding to get the DMA engine to work on a whole AES block when bytesOffset is not 0. */
        pDecryptor->blks[1].pSrcAddr  = pDecryptor->dyn.aes_ctr_padding;
        pDecryptor->blks[1].pDestAddr = pDecryptor->dyn.aes_ctr_padding;
        pDecryptor->blks[1].blockSize = byteOffset;
        pDecryptor->blks[1].resetCrypto = false;
        pDecryptor->blks[1].scatterGatherCryptoStart = false;
        pDecryptor->blks[1].scatterGatherCryptoEnd = false;
        pDecryptor->blks[1].cached = true;
        nb_Blks++;
        ii++;
    }
    nbBlkInHeader = ii;

    nb_Blks += nDmaBlocks;

    for(ii; ii < nb_Blks; ii++){
        pDecryptor->blks[ii].pSrcAddr = pBlks[ii - nbBlkInHeader].pSrcAddr;
        pDecryptor->blks[ii].pDestAddr = pBlks[ii - nbBlkInHeader].pDestAddr;
        pDecryptor->blks[ii].blockSize = pBlks[ii - nbBlkInHeader].blockSize;

        pDecryptor->blks[ii].resetCrypto = false;
        pDecryptor->blks[ii].scatterGatherCryptoStart = false;
        pDecryptor->blks[ii].scatterGatherCryptoEnd = false;
        pDecryptor->blks[ii].cached = true;
    }
    pDecryptor->blks[nb_Blks-1].scatterGatherCryptoEnd = true;

    CommonCrypto_GetDefaultJobSettings(&jobSettings);
    jobSettings.keySlot = pDecryptor->keyCtr;

    rc = CommonCrypto_DmaXfer(pDecryptor->cryptoHandle, &jobSettings, pDecryptor->blks, nb_Blks);
    if(rc != NEXUS_SUCCESS)
    {
        BDBG_ERR(("%s - CommonCrypto_DmaXfer failed rc %x\n", __FUNCTION__, rc));
        dr = bdrm_err_fail;
    }

    BDBG_MSG(("%s - exiting", __FUNCTION__));

    return dr;
}
#endif

/* Called to determine if drm exists in this asf. Now read from the header list enum.
** Will be implemented in the same means as the asfhdr lib.
*/
bdrm_encr_state 
bdrm_mp4_is_encrypted (bdrm_t drm)                                  
{
    bdrm_encr_state ret = bdrm_encr_none;
    bdrm_mp4_track_info_node_t *pChild;

    BDBG_MSG(("%s - entering", __FUNCTION__));
    BDBG_ASSERT(drm != NULL);

    if(drm->headers.mp4.pssh.data != NULL)
    {    
        for (pChild = BLST_D_FIRST( &drm->headers.mp4.tracks); pChild; pChild = BLST_D_NEXT(pChild, track_link)) {
            if(pChild != NULL){
                if(pChild->pScheme != NULL){
                    if(pChild->pScheme->te.info.alg_id == bdrm_encr_aes_ctr){
                         ret= bdrm_encr_aes_ctr;
                         break;
                    }
                    else if(pChild->pScheme->te.info.alg_id == bdrm_encr_aes_cbc){
                        ret = bdrm_encr_aes_cbc;
                        break;
                    }
                }
                else continue;
            }
            else break;
        }
    }

    BDBG_MSG(("%s - exiting ret %x", __FUNCTION__, ret));
    return(ret);
}   


/* Internal cleanup function. Destroys dyn allocated mp4 buffers.
*/
static void
bdrm_mp4_destroy_drm_headers (bdrm_t drm)
{
    bdrm_mp4_headers *pHrds = &drm->headers.mp4;
    bdrm_mp4_track_info_node_t *pNode;

    BDBG_MSG(("%s - entering", __FUNCTION__));

    if(pHrds->pssh.data != NULL){
        NEXUS_Memory_Free(pHrds->pssh.data);
    }
 
    while(!BLST_D_EMPTY(&pHrds->tracks)){
        pNode = BLST_D_FIRST(&pHrds->tracks);
        NEXUS_Memory_Free(pNode->pScheme);
        BLST_D_REMOVE(&pHrds->tracks, pNode, track_link);
        NEXUS_Memory_Free(pNode);
    }

    BKNI_Memset(pHrds, 0, sizeof(bdrm_mp4_headers));

    BDBG_MSG(("%s - exiting", __FUNCTION__));
    return;

}


/* Init headers to known state
*/
static bdrm_err
bdrm_mp4_reset_headers(bdrm_t drm)
{   
    bdrm_err rc = bdrm_err_ok;
    BDBG_MSG(("%s - entering", __FUNCTION__));

    bdrm_mp4_destroy_drm_headers(drm);
    bdrm_decryptor_destroy_se(drm->dcrypt);

    BLST_D_INIT(&drm->headers.mp4.tracks);
    BDBG_MSG(("%s - exiting rc %x", __FUNCTION__, rc));
    return rc;
}


/* Clean up the dynamically created obj and other dependend stuff
*/
static void
bdrm_mp4_destroy_drm_obj (bdrm_t drm)
{
    BSTD_UNUSED(drm);
    /* No need to do anything */
    return;
}

void
bdrm_mp4_get_xml_info (bdrm_t drm, uint8_t **pbXML, uint32_t *cbXML )
{
    BDBG_ASSERT(drm != NULL);
    BDBG_ASSERT(pbXML != NULL);
    BDBG_ASSERT(cbXML != NULL);
    BDBG_MSG(("%s - entering", __FUNCTION__));
    if(drm->headers.mp4.pssh.data != NULL){
        /* get xml info from PSSH object */
        *pbXML = drm->headers.mp4.pssh.data; *cbXML = drm->headers.mp4.pssh.size;
    }
    else {
        *pbXML = NULL; *cbXML = 0;
    }
    BDBG_MSG(("%s - exiting", __FUNCTION__));
    return;
}

void bdrm_init_mp4_container (bdrm_t drm)
{
    BDBG_ASSERT(drm != NULL);
    BDBG_MSG(("%s - entering", __FUNCTION__));
    BLST_D_INIT(&drm->headers.mp4.tracks);

    drm->cnt_fp.reset_headers    = bdrm_mp4_reset_headers;
    drm->cnt_fp.destroy_headers  = bdrm_mp4_destroy_drm_headers;
    drm->cnt_fp.destroy_drm_objs = bdrm_mp4_destroy_drm_obj;
    drm->cnt_fp.is_drmencrypted  = bdrm_mp4_is_encrypted;
    drm->cnt_fp.get_xml_info     = bdrm_mp4_get_xml_info;
    BDBG_MSG(("%s - exiting", __FUNCTION__));
    return;
}

bdrm_err bdrm_mp4_set_pssh(bdrm_t drm, const void *pData, size_t dataLength, const void * pSystemId, size_t systemIdLength)
{
    bdrm_err rc = bdrm_err_ok;
    NEXUS_Error errCode = NEXUS_SUCCESS;
    NEXUS_MemoryAllocationSettings allocSettings;

    BDBG_ASSERT(drm != NULL);
    BDBG_ASSERT(pData != NULL);
    BDBG_ASSERT(pSystemId != NULL);
    BDBG_ASSERT(systemIdLength ==16);

    NEXUS_Memory_GetDefaultAllocationSettings(&allocSettings);
    allocSettings.heap = drm->cfg->hHeap;

    errCode = NEXUS_Memory_Allocate(dataLength, &allocSettings, (void *)(&drm->headers.mp4.pssh.data));
    if ( errCode )
    {
        rc = bdrm_err_fail;
    }
    else
    {
        BDBG_MSG(("SET PSSH BOX"));
        BKNI_Memcpy(drm->headers.mp4.pssh.data, pData, dataLength);
        drm->headers.mp4.pssh.size = dataLength;
        BKNI_Memcpy(drm->headers.mp4.pssh.systemId, pSystemId, systemIdLength);
    }

    return rc;

}

bdrm_err bdrm_mp4_setProtectionScheme(bdrm_t drm, const bdrm_mp4_protect_scheme *pScheme, uint32_t trackID)
{
    bdrm_err rc = bdrm_err_fail;
    bdrm_mp4_track_info_node_t *pNode;
    NEXUS_MemoryAllocationSettings allocSettings;

    BDBG_ASSERT(drm != NULL);
    BDBG_ASSERT(pScheme != NULL);

    BDBG_MSG(("%s - entering", __FUNCTION__));

    /* Check if we already have an entry for the track. If we do, update it.*/
    GET_TRACK_INFO(pNode, &drm->headers.mp4.tracks, track_link, trackID);
    if(pNode){
        /* Something is wrong, we already have an entry in the list for the given track*/
        BDBG_ERR(("%s - duplicate entry for track ID %d detected", __FUNCTION__, trackID));
    }
    else {
        /* Add the protection scheme to the list */

        NEXUS_Memory_GetDefaultAllocationSettings(&allocSettings);
        allocSettings.heap = drm->cfg->hHeap;

        if(NEXUS_Memory_Allocate(sizeof(bdrm_mp4_track_info_node_t), &allocSettings, (void *)(&pNode)) == NEXUS_SUCCESS)
        {
            BKNI_Memset(pNode, 0, sizeof(bdrm_mp4_track_info_node_t));
            if(NEXUS_Memory_Allocate(sizeof(bdrm_mp4_protect_scheme), &allocSettings, (void *)(&pNode->pScheme)) == NEXUS_SUCCESS)
            {
                pNode->track_ID = trackID;
                BKNI_Memcpy(pNode->pScheme, pScheme, sizeof(bdrm_mp4_protect_scheme));
                BLST_D_INSERT_HEAD(&drm->headers.mp4.tracks, (pNode), track_link);
                rc = bdrm_err_ok;

            }
            else {
                NEXUS_Memory_Free(pNode);
            }
        }
    }

    return rc;
}



#if 0/*HW decryption with temp buffer; working*/
void bdrm_decrypt_mp4_sample(void *user_context, uint32_t track_ID, unsigned sample_no, const batom_cursor *data, size_t payload_len)
{
    bdrm_t drm = (bdrm_t )user_context;
    struct batom_checkpoint start;

    bdrm_mp4_track_info_node_t *pTrackInfo;
    bdrm_mp4_box_se_sample *pSample;
    bmp4_scheme_info *pSchemeInfo;
    bdrm_mp4_se *pSe;
    uint32_t nb_Blks = 1; /* The first block is for external key and iv */
    uint32_t jj = 0;
    NEXUS_Error rc;
    CommonCryptoJobSettings jobSettings;

    static int run = 0; 
    unsigned pos = 0;
    unsigned char* ps;      /* source buffer */
    unsigned int off = 0, copy = 0;
    bdrm_decryptor *pDecryptor;

    uint8_t* pCopyBuf;
    BDBG_MSG(("%s - entering", __FUNCTION__));
    if (drm == NULL) {
        BDBG_ERR(("%s - no active drm", __FUNCTION__));
        return;
    }

    if (drm->dcrypt == NULL) {
        BDBG_ERR(("%s - no active decryptor", __FUNCTION__));
        return;
    }

    /* save cursor position on entry */
    batom_cursor_save(data, &start);

    NEXUS_Memory_Allocate(payload_len, NULL, &pCopyBuf);
    batom_cursor_copy((batom_cursor *)data, pCopyBuf, payload_len);
    ps = pCopyBuf;

    pDecryptor = drm->dcrypt;
    pSe = bdrm_get_se_ptr(pDecryptor, track_ID);
    if (pSe == NULL) {
        return;
    }
    GET_TRACK_INFO(pTrackInfo, &drm->headers.mp4.tracks, track_link, track_ID);

    pSchemeInfo = pDecryptor->pSchemeInfo;

    if(pSchemeInfo->alg_id == 0x3)
        return; /* Playready does not suppport AES-CBC*/
    
    if(pSchemeInfo->alg_id == 0x0){
        /* Data in the clear, nothing to do. */
        BDBG_MSG(("Data in the clear, nothing to do\n"));
        return;
    }
 
    /* Get IV from the current sample */
    pSample = &pSe->samples[sample_no];

    /*  Encryption using external key and IV.  Scatter and gather must be used for 
	    external key and IV to work */
    BKNI_Memcpy(pDecryptor->dyn.ekl_buf,  ((DRM_CIPHER_CONTEXT *)(pDecryptor->MsDecryptCtx.rgbBuffer))->rgbContentKey+8, 8);	/* Copy key.  H and L need to be swapped */
    BKNI_Memcpy(pDecryptor->dyn.ekl_buf+8,  ((DRM_CIPHER_CONTEXT *)(pDecryptor->MsDecryptCtx.rgbBuffer))->rgbContentKey, 8);	
    BKNI_Memcpy(&pDecryptor->dyn.ekl_buf[16], pSample->iv, 16);
    
    /* Set Key and IV */
    NEXUS_DmaJob_GetDefaultBlockSettings(&pDecryptor->blks[0]);
    pDecryptor->blks[0].pSrcAddr   = pDecryptor->dyn.ekl_buf;
    pDecryptor->blks[0].pDestAddr  = pDecryptor->dyn.ekl_buf;
    pDecryptor->blks[0].blockSize  = AES_EKL_SIZE;
    pDecryptor->blks[0].resetCrypto              = true;
    pDecryptor->blks[0].scatterGatherCryptoStart = true;

    /* Setup dma blocks. */
    if(pSe->fullbox.flags &= 0x000002){


        for(jj = 0; jj < pSample->nbOfEntries; jj++){
            ps += pSample->entries[jj].bytesOfClearData;

            NEXUS_DmaJob_GetDefaultBlockSettings(&pDecryptor->blks[nb_Blks]);
            pDecryptor->blks[nb_Blks].pSrcAddr  = ps;
            pDecryptor->blks[nb_Blks].pDestAddr = ps;
            pDecryptor->blks[nb_Blks].blockSize = pSample->entries[jj].bytesOfEncData;
            pDecryptor->blks[nb_Blks].resetCrypto              = true;

            ps += pSample->entries[jj].bytesOfEncData;
            nb_Blks++;
        }
        pDecryptor->blks[nb_Blks-1].scatterGatherCryptoEnd = true;
    }
    else {
       NEXUS_DmaJob_GetDefaultBlockSettings(&pDecryptor->blks[1]);

       if(pSchemeInfo->alg_id == 0x2){
            pDecryptor->blks[1].pSrcAddr  = ps;
            pDecryptor->blks[1].pDestAddr = ps;
            pDecryptor->blks[1].blockSize = payload_len;
            pDecryptor->blks[1].resetCrypto              = true;
        }
        else {
            pDecryptor->blks[1].pSrcAddr  = ps;
            pDecryptor->blks[1].pDestAddr = ps;
            pDecryptor->blks[1].blockSize = payload_len - (payload_len % 16);
            pDecryptor->blks[1].resetCrypto              = true;
        }
        nb_Blks++;
        pDecryptor->blks[nb_Blks-1].scatterGatherCryptoEnd = true;
    }

    CommonCrypto_GetDefaultJobSettings(&jobSettings);
    jobSettings.keySlot = pDecryptor->keyCtr;

    rc = CommonCrypto_DmaXfer(pDecryptor->cryptoHandle, &jobSettings, pDecryptor->blks, nb_Blks);
    if(rc != NEXUS_SUCCESS)
    {
        BDBG_ERR(("%s - CommonCrypto_DmaXfer failed rc %x\n", __FUNCTION__, rc));
    }
    else {

        batom_cursor_rollback((batom_cursor *)data, &start); 

        ps = pCopyBuf;
        pos = 0; run++; 

        /* copy decrypted data back into app */
        if (data->left >= (int)payload_len) {           /* direct copy */
            BDBG_MSG(("pld->left(%d) > info->pld_len(%d)", data->left, payload_len));
            BKNI_Memcpy((uint8_t*)data->cursor, ps, payload_len);
        } else {
            BDBG_MSG(("pld->left(%d) < info->pld_len(%d) (straddle)", data->left, payload_len));            
            for (off=0; pos < payload_len; batom_cursor_skip(data, data->left+off)) {    
                /* straddle copy */
                if(BATOM_IS_EOF(data)) {
                    BDBG_ASSERT(0); break;                          /* shouldn't happen */
                }
                BDBG_MSG(("straddle copy: %d pos=%d", data->left, pos));
                if (pos+data->left+off <= payload_len) {
                    copy = data->left+off;               
                } else {
                    copy = payload_len-pos;
                }
                BKNI_Memcpy((uint8_t*)data->cursor-off, ps, copy);
                ps += copy;
                pos += copy;
                off = 1;                
            }
        }
    }

    NEXUS_Memory_Free(pCopyBuf);
    batom_cursor_rollback((batom_cursor *)data, &start);
    BDBG_MSG(("%s - exiting", __FUNCTION__));
}

#endif

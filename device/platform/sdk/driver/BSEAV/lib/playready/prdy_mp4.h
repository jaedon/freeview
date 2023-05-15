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

#ifndef _DRM_PRDY_MP4_H__
#define _DRM_PRDY_MP4_H__

#include "bmp4_util.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct bmp4_cipher_info {
    bdrm_encr_state alg_id; 
    uint8_t  iv_size;
    uint8_t  kid[16];
}bmp4_scheme_info;

typedef struct bdrm_mp4_te_box{
    bmp4_fullbox     fullbox;
    bmp4_scheme_info info;
}bdrm_mp4_te_box;

typedef struct bdrm_mp4_scheme_type_box{
    bmp4_fullbox fullbox;
    uint32_t scheme_type;
    uint32_t scheme_version;
}bdrm_mp4_scheme_type_box;

typedef struct bdrm_mp4_original_fmt_box{
    uint32_t codingname;
}bdrm_mp4_original_fmt_box;


/* There is one protection scheme per track */
typedef struct bdrm_mp4_protect_scheme{
    bdrm_mp4_scheme_type_box  schm;   /* One per protection scheme */
    bdrm_mp4_original_fmt_box frma;   /* One per protection scheme */
    bdrm_mp4_te_box           te;     /* One per protection scheme*/
}bdrm_mp4_protect_scheme;


/* PIFF Protection System Specific Header Box */
typedef struct bdrm_mp4_pssh {
    bmp4_box_extended extended;
    bmp4_fullbox fullbox;
    uint8_t  systemId[16];
    uint32_t size;
    uint8_t *data;
} bdrm_mp4_pssh;


typedef struct bdrm_mp4_box_se_entry {
    uint16_t bytesOfClearData;
    uint32_t bytesOfEncData;
}bdrm_mp4_box_se_entry;


#define MAX_ENTRIES_PER_SAMPLE        (10)  /*Max number of entries per sample */

typedef struct bdrm_mp4_box_se_sample {
    uint8_t  iv[16];  /* If the IV size is 8, then bytes 0 to 7 of teh 16 byte array contains the IV. */
    
    /* The following three values are only meaningfull if flag & 0x000002 is true */
    uint16_t nbOfEntries;
    bdrm_mp4_box_se_entry entries[MAX_ENTRIES_PER_SAMPLE];

}bdrm_mp4_box_se_sample;


#define SAMPLES_POOL_SIZE        (500)  /* Nb of bdrm_mp4_box_se_sample in the pool */

typedef struct bdrm_mp4_se {
    bmp4_box_extended extended;
    bmp4_fullbox fullbox;
    bmp4_scheme_info info;
    uint32_t sample_count;
    bdrm_mp4_box_se_sample samples[SAMPLES_POOL_SIZE];
    
    uint32_t track_Id; /* Id of the track this se box belongs to*/
    bool inUse;        /* Flag used to managed the pool of track id. Use to detect it the se has been assign a track id.*/
}bdrm_mp4_se;




/*
** Init the DRM MP4 container object. 
*/ 

void bdrm_init_mp4_container (bdrm_t drm);

bdrm_err bdrm_mp4_get_codingname(bdrm_t drm, uint32_t track_ID, uint32_t *pCodingName);
void bdrm_parse_mp4_fragment_context(void *user_context, const batom_cursor *cursor, size_t payload_len);
void bdrm_decrypt_mp4_sample(void *user_context, uint32_t track_ID, unsigned sample_no, const batom_cursor *data, size_t payload_len);
bdrm_err bdrm_mp4_set_pssh(bdrm_t drm, const void *pData, size_t dataLength, const void * pSystemId, size_t systemIdLength);
bdrm_err bdrm_mp4_setProtectionScheme(bdrm_t drm, const bdrm_mp4_protect_scheme *pScheme, uint32_t trackID);


bdrm_err bdrm_decrypt_aes_ctr_samples(bdrm_t drm, uint64_t nonce, uint64_t blockCounter, size_t  byteOffset,
                                  const NEXUS_DmaJobBlockSettings *pBlks, uint32_t nBlocks);

#ifdef __cplusplus
}
#endif

#endif /* _DRM_PRDY_MP4_H__ */


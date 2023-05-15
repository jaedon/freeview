/***************************************************************************
*     Copyright (c) 2006-2007, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: asfpars.h $
* $brcm_Revision: 16 $
* $brcm_Date: 7/20/07 3:22p $
*
* Module Description:
*
* DRM library
* 
* Revision History:
*
* $brcm_Log: /BSEAV/lib/asf/asfpars.h $
* 
* 16   7/20/07 3:22p vsilyaev
* PR 32813: Update to compile with new basf_parser/basf_stream
* 
* 15   7/9/07 3:48p erickson
* PR32256: added wma_pro
* 
* 13   1/19/07 3:16p ptimariu
* PR26800: added codec-id/compression-id to stream hdr obj
* 
* 12   12/6/06 4:40p ptimariu
* PR26366: fixed stream object endian
* 
* 10   10/25/06 3:11p ptimariu
* PR24026: added support for extended stream and bitrate
* 
* 9   9/11/06 6:30p ptimariu
* PR20685: added functs from drm_util
* 
* 8   9/1/06 1:33p ptimariu
* PR20685: added codec info
* 
* 7   7/26/06 2:15p tokushig
* PR20685: added DRM support to brutus
* 
* SanDiego_DRM_ASF/2   7/20/06 3:34p tokushig
* merge from main
* 
* 6   6/30/06 12:01p ptimariu
* PR20586: added support for custom parse context - drm
* 
* SanDiego_DRM_ASF/1   6/21/06 10:38a tokushig
* add extern "C" for c++ applications
* 
* 6   6/30/06 12:01p ptimariu
* PR20586: added support for custom parse context - drm
* 
* 5   6/20/06 2:32p ptimariu
* PR20685: misc cleanup fixes
* 
* 7   6/16/06 4:58p ptimariu
* PR20685: fixed loop parsing crash. demux was being uninitialized
* multiple times.
* 
* 5   6/7/06 6:43p ptimariu
* PR20685: added BLST_S_INIT for codec list
* 
* 4   6/7/06 6:36p ptimariu
* PR20685: updated with codec list, added cleanup funcs
* 
* 3   6/7/06 12:17p ptimariu
* PR20685: update asfpars sample with file header parsing
* 
* 2   5/31/06 10:47a ptimariu
* PR20685: update api
* 
*******************************************************************************/

#ifndef __ASFPARS_H__
#define __ASFPARS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "basf_parser.h"
#include "biobits.h"

#define MAX_CODEC_NAME      128
#define MAX_CODEC_DESCR     128
#define MAX_ASF_STREAMS     128

/* FOURCC builder macro 
*/
#if (BSTD_CPU_ENDIAN == BSTD_ENDIAN_BIG)
#define BASF_FOURCC(a,b,c,d)(((uint32_t)d)|(((uint32_t)c)<<8)|\
        (((uint32_t)b)<<16)|(((uint32_t)a)<<24))
#else 
#define BASF_FOURCC(a,b,c,d)(((uint32_t)a)|(((uint32_t)b)<<8)|\
        (((uint32_t)c)<<16)|(((uint32_t)d)<<24))
#endif

/* get basic codec_id info
*/
#define basf_codec_vc1_advanced(id) ((id==BASF_FOURCC('W','M','V','A'))||(id==BASF_FOURCC('W','V','C','1')))    /* is this advanced profile? */
#define basf_codec_vc1_simplemain(id) (id==BASF_FOURCC('W','M','V','3'))    /* is this simple/main profile? */
#define basf_codec_wma_standard(id) (id==0x161)  
#define basf_codec_wma_professional(id) (id==0x162) 
    
/* ASF CODEC types
*/
typedef enum asf_codec_type {
    asf_cdt_video   = 0x0001,		/* audio codec */
    asf_cdt_audio   = 0x0002,		/* video codec */
    asf_cdt_unknown = 0xFFFF		/* unknown codec */
} asf_codec_type;

/* ASF header contexts; can be general or drm
*/
typedef enum basf_hdr_ctx {
    basf_hdr_ctx_gen,                /* general (file props, etc) */
    basf_hdr_ctx_gen_demux,          /* general contex + use demux */
    basf_hdr_ctx_custom,             /* custom context (drm) */
    basf_hdr_ctx_unk                 /* unknown context */
} basf_hdr_ctx;

/* Individual codec description
*/
typedef struct basf_hdr_codec_indiv {
    BLST_S_ENTRY(basf_hdr_codec_indiv) link;
    asf_codec_type type;
    char name[MAX_CODEC_NAME];
    char description[MAX_CODEC_DESCR];
    uint32_t info_len;
    uint8_t* info;    
} basf_hdr_codec_indiv;

/* Codecs accessor list
*/
typedef BLST_S_HEAD(basf_hdr_codec_lst, basf_hdr_codec_indiv) basf_hdr_codec_lst;

/* Codecs header(s). Contains a list of headers
*/
typedef struct basf_hdr_codecs {
    basf_guid reserved;
    basf_dword codec_count;
    basf_hdr_codec_lst list;
} basf_hdr_codecs;

/* Language header
*/
typedef struct basf_hdr_language {
    basf_word language_count;
    basf_wchar** language_id;
} basf_hdr_language;

/* Individual metadata description
*/
typedef struct basf_hdr_meta_indiv {
    basf_word reserved;
    basf_word stream_num;
    basf_wchar* name;
    basf_word data_type;
    basf_word data_len;
    basf_byte* data;
} basf_hdr_meta_indiv;

/* Metadata header(s). Contains a list of headers
*/
typedef struct basf_hdr_metadata {
    basf_word meta_count;
    basf_hdr_meta_indiv* metas;
} basf_hdr_metadata;

/* Media stream type - audio, video, unk
*/
typedef enum basf_media_stream_type {
    basf_media_stream_audio  = 0x01,
    basf_media_stream_video  = 0x02,
    basf_media_stream_unk    = 0x03
} basf_media_stream_type;

/* Feed control statuses
*/
typedef enum basf_feed_control {
    basf_feed_continue      = 0x00, 
    basf_feed_stop          = 0x01
} basf_feed_control;

/* Stream header
*/
typedef struct basf_hdr_stream {
    BLST_S_ENTRY(basf_hdr_stream) link;
    basf_media_stream_type stream_type;
    basf_guid err_corr_type;
    basf_qword time_offset;
    basf_dword type_spec_datalen;
    basf_dword err_cor_datalen;
#define B_ASFP_FLAGS_GET_STREAM_NO(flags) (B_GET_BITS(flags, 6, 0))  /* read stream id */
#define B_ASFP_FLAGS_GET_RESERVED(flags)  (B_GET_BITS(flags, 14, 7)) /* unused, should be 0 */
#define B_ASFP_FLAGS_GET_ENCRYPTED(flags) (B_GET_BIT(flags, 15))     /* encrypted flag */
    basf_word flags;                /* stream object flags */
    basf_dword reserved;            /* not used, should be 0 */
    basf_byte* type_spec_data;      /* type specific data */
    basf_byte* err_cor_data;        /* error correction data */
    basf_dword codec_id;            /* no match for this in the actual stream object hdr -> */
} basf_hdr_stream;                  /* -> this means codec-id for audio, compression-id for video */
                                    /* TODO: extend this with other a/v stream data */                                   
    
/* Extended stream header
*/
typedef struct basf_hdr_ext_stream {
    BLST_S_ENTRY(basf_hdr_ext_stream) link;
    basf_dword max_bw;
} basf_hdr_ext_stream;

/* Streams accessor list
*/
typedef BLST_S_HEAD(basf_hdr_stream_lst, basf_hdr_stream) basf_hdr_stream_lst;
typedef BLST_S_HEAD(basf_hdr_ext_stream_lst, basf_hdr_ext_stream) basf_hdr_ext_stream_lst;

/* File header
*/
typedef struct basf_hdr_file {
    basf_guid id;
    basf_qword size;
    basf_qword creation_date;
    basf_qword datapack_count;
    basf_qword play_duration;
    basf_qword send_duration;
    basf_qword preroll;
    basf_dword flags;				/* - broadcast flag 1 (lsb)
									   - seekable flag  1
									   - reserved       30 */
    basf_dword min_dt_pack_size;
    basf_dword max_dt_pack_size;
    basf_dword max_bitrate;
} basf_hdr_file;

/* Bitrate header structure
*/
typedef struct basf_hdr_bitrate {
    basf_dword count;
    basf_dword stream_id[MAX_ASF_STREAMS];
    basf_dword stream_bitrate[MAX_ASF_STREAMS];
} basf_hdr_bitrate;

/* Content description header
*/
typedef struct basf_hdr_content {
    char* title;                    /* wma title info */
    char* author;                   /* wma author info */
    char* copyright;                /* wma copyright info */
    char* description;              /* wma description info */
    char* rating;                   /* wma rating info */
} basf_hdr_content;

typedef struct basf_headers* basf_headers_t;    /* opaque decl for headers */
typedef struct basf_props* basf_props_t;        /* opaque decl for ASF properties */

/* Callback func for feed control
*/
typedef basf_feed_control(*basf_fn_feed_handler)(void* context);

/* Header types
*/
typedef enum asf_header_types {
    asf_hdr_type_content_descr,
    asf_hdr_type_file,
    asf_hdr_type_stream,
    asf_hdr_type_meta,
    asf_hdr_type_language,
    asf_hdr_type_codecs
} asf_header_types;

/* Get a handle to the asf file headers
*/
basf_props_t basf_open_asf (
	const char* filename,		/* [in] filename - name of the .wma,.wmv,.asf file */ 
    basf_hdr_ctx context,       /* [in] context - generic, custom */
    basf_parser_t* asf                 /* [out] asf - live asf parser instance */
	); 

/* Get a handle to the asf file headers, when asf file chunk is given
*/
basf_props_t basf_open_asf_chunk(
    unsigned char* chunk_data,  /* [in] chunk_data - outside chunk */ 
    unsigned int chunk_size,    /* [in] chunk_size - size of the outside chunk */
    basf_hdr_ctx context,       /* [in] context - generic, custom */
    basf_parser_t* asf                 /* [out] asf - live asf parser instance */
    );

/* Start scanning the asf for headers
*/
int basf_scan_asf (
	basf_props_t props			/* [in] props - asf handle */
	);

/* Start scanning the asf file chunk for headers
*/
int basf_scan_asf_chunk (
	basf_props_t props			/* [in] props - asf handle */
	);

/* Close opened asf handle
*/
int basf_close_asf (
	basf_props_t asf			/* [in] props - asf handle */
	);

/* Installs a feed control callback funct
*/
void basf_install_feed_handler (
    basf_props_t asf,			/* [in] props - asf handle */
    basf_fn_feed_handler hdlr,  /* [in] external feed control callback function */
    void* ctx                   /* [in] external feed control callback function */
	);

/* Accessor func to codec headers
*/
void basf_get_codec_hdr (
	basf_props_t props,			/* [in] props - asf handle */ 
	basf_hdr_codec_lst** codecs /* [out] codecs - list of ptrs to codecs */ 
	);

/* Accessor func to language headers
*/
void basf_get_lang_hdr (
	basf_props_t props,			/* [in] props - asf handle */ 
	basf_hdr_language** lang	/* [out] langs - list of ptrs to langs */ 
	);

/* Accessor func to meta headers
*/
void basf_get_meta_hdr(
	basf_props_t props,			/* [in] props - asf handle */ 
	basf_hdr_metadata**	metas	/* [out] metas - list of ptrs to meta headers */ 
	);

/* Accessor func to stream headers
*/
void basf_get_stream_list_hdr (
	basf_props_t props,			/* [in] props - asf handle */ 
	basf_hdr_stream_lst** strms /* [out] strms - list of ptrs to streams */ 
	); 

/* Accessor func to file headers
*/
void basf_get_file_hdr ( 
	basf_props_t props,			/* [in] props - asf handle */ 
	basf_hdr_file** files		/* [out] files - list of ptrs to files */
	);

/* Accessor func to the bitrate header
*/
void basf_get_bitrate_hdr (
    basf_props_t props,         /* [in] props - asf handle */ 
    basf_hdr_bitrate** bitrate  /* [out] files - list of ptrs to bitrates */
    );

/* Accessor func to content headers
*/
void basf_get_content_hdr ( 
	basf_props_t props,			/* [in] props - asf handle */ 
	basf_hdr_content** contents	/* [out] contents - list of ptrs to contents */ 
	);

/* read an arbitrary array from the current position
*/
unsigned basf_read_arr (
    batom_cursor *c,          /* [in] file cursor, current position */
    basf_byte *arr,         /* [out] string to be filled */
    unsigned len            /* [in] max length of  the array */
    );

/* read a zero-terminated string from the current position
*/
void basf_read_str(
    batom_cursor *c,          /* [in] file cursor, current position */
    basf_byte *str,         /* [out] string to be filled */
    unsigned len            /* [in] max length of  the string */
    );

#ifdef __cplusplus
}
#endif
#endif /* __ASFPARS_H__ */



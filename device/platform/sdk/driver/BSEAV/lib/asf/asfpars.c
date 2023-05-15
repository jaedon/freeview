/***************************************************************************
*     Copyright (c) 2006-2008, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: asfpars.c $
* $brcm_Revision: 26 $
* $brcm_Date: 10/1/08 3:09p $
*
* Module Description:
*
* DRM library
* 
* Revision History:
*
* $brcm_Log: /BSEAV/lib/asf/asfpars.c $
* 
* 26   10/1/08 3:09p btosi
* PR47488: removed "inline" so that we don't need the c99 flag
* 
* 25   7/20/07 3:22p vsilyaev
* PR 32813: Update to compile with new basf_parser/basf_stream
* 
* 22   1/19/07 5:08p ptimariu
* PR26800: correct reported bitrate in debug mode
* 
* 21   1/19/07 3:21p ptimariu
* PR26800: added codec-id/compression-id to stream hdr obj
* 
* 20   12/6/06 4:31p ptimariu
* PR26366: fixed stream object endian
* 
* 19   11/3/06 4:59p ptimariu
* PR24026: got rid of some warnings
* 
* 18   10/25/06 3:10p ptimariu
* PR24026: added support for extended stream and bitrate
* 
* 17   9/11/06 6:52p ptimariu
* PR20685: removed reference to drmutil.h
* 
* 16   9/11/06 6:29p ptimariu
* PR20685: add functs from drm_util.c
* 
* 15   9/1/06 1:35p ptimariu
* PR20685: added codec info
* 
* 14   7/20/06 10:42a ptimariu
* PR20685: clean up inactive sections
* 
* 13   6/30/06 12:00p ptimariu
* PR20586: added support for custom parse context - drm
* 
* 12   6/21/06 2:48p ptimariu
* PR20685: replaced BGB_MSGs
* 
* 10   6/21/06 12:07p ptimariu
* PR20685: cleanup props correcting order of memfree.
* 
* 9   6/20/06 3:03p ptimariu
* PR20685: added hack to loop one more time once we're passed the header
* section, to allow the recycle function to get called.
* 
* 8   6/20/06 2:33p ptimariu
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

#include "bstd.h"       /* standard includes */
#include "bkni.h"
#include "bfifo.h"

#include "asfpars.h"    /* local includes */
#include "basf_stream.h"
#include "biobits.h"

BDBG_MODULE(basf_pars);
#define BDBG_MSG_TRACE(x)
#ifdef PROFILE
#undef BDBG_MSG
#define BDBG_MSG(x)
#endif

#if 1
#define BUF_SIZE                (4096*16)
#else
#define BUF_SIZE                (197*7)
#endif

/* local defines */
#define OBJ_STATE_INIT                      (0x00)
#define OBJ_STATE_COMPLETE                  (0x01)
#define BASF_VIDEO_UNK_FOURCC               (0x00)
#define BASF_VIDEO_OBJTYPE_BMPHDR_OFF       ((32+32+8)/8)
#define BASF_VIDEO_BMPHDR_COMPRESSID_OFF    ((32+32+32+16+16)/8)

/* local macros
*/
/* calc half of q */
#define HALF(q)                 ((q)/2+1)								

/* dealloc para */
#define FREE_SET(para)          do { if ((para) != NULL) {              \
                                        BKNI_Free((para));              \
                                        (para) = NULL;                  \
                                     }                                  \
                                } while(0);					    

/* alloc a buff p of len length */
#define MALLOC_SET(p, length)   do { if ((length) > 0) {                \
                                        (p) = BKNI_Malloc((length));    \
                                        BKNI_Memset((p), 0, (length));  \
                                    } else (p) = NULL;                  \
                                } while(0);   

/* ASF Stream context
*/
typedef struct stream_cnxt {
	basf_stream_t stream;
	batom_pipe_t pipe;
	FILE *pes_out, *ts_out;
	unsigned pid_cont_counter;
	unsigned ts_pid;
} stream_cnxt;

/* Generic object handler used in the asf context
*/
typedef struct basf_gen_object_handler {
    basf_parser_object_handler handler;    /* asf handler */    
    basf_props_t props;             /* back-ptr to context */
    int state;                      /* handler state */
} basf_gen_object_handler;

typedef enum basf_read_src_type {
    basf_src_type_chunk = 0x00,     /* chunk source */
    basf_src_type_file  = 0x01      /* file source */
} basf_read_src_type;

/* ASF properties struct
*/
typedef struct basf_props {
    basf_parser_cfg asf_cfg;               /* asf config */
	batom_factory_t factory;		/* atom factory */
	batom_pipe_t pipe;				/* source for the asf_parser */
    basf_parser_t asf;                     /* asf parser */
    basf_headers_t headers;         /* header set */
    basf_demux_cfg demux_cfg;       /* demux configuration */
    FILE *fin;                      /* input file handle */    
    stream_cnxt audio, video;       /* audio/video context */
    basf_demux_t demux;             /* demux instance */    
    basf_stream_cfg stream_cfg;     /* stream configuration */   
    struct _feed_handler {
        basf_fn_feed_handler handler;   /* feed control callback */
        void* context;                  /* feed callback context */
    } feed_handler;
    basf_gen_object_handler stream_handler;	    /* stream header handler */
    basf_gen_object_handler ext_stream_handler;	/* ext stream header handler */
    basf_gen_object_handler content_handler;    /* content header handler */
    basf_gen_object_handler lang_handler;	    /* language header handler */
    basf_gen_object_handler meta_handler;	    /* meta header handler */
    basf_gen_object_handler extc_handler;	    /* extended content header handler */
    basf_gen_object_handler codec_handler;	    /* codec header handler */
    basf_gen_object_handler file_handler;	    /* file header handler */
    basf_gen_object_handler bitrate_handler;    /* bitrate stream handler */
    struct ext_chunk {              /* in case we use an external chunk */                          
        unsigned char* data;        /* chunk data */
        unsigned int size;          /* chunk size */
    } ext_chunk;                    /* in case we use an external chunk */                          
    basf_read_src_type src_type;    /* source type */
    bool demux_attached;            /* is demux attached */
} basf_props;

/* ASF headers struct, the only struct visible from the outside
*/
typedef struct basf_headers {
    basf_hdr_codecs codecs;         /* codecs header */
    basf_hdr_language language;     /* languages header */
    basf_hdr_metadata meta;         /* meta header */
    basf_hdr_stream_lst streams;    /* list of streams */
    basf_hdr_ext_stream_lst ext_streams;/* list of extended streams */
    basf_hdr_file file;             /* file header */
    basf_hdr_bitrate bitrate;       /* file header */
    basf_hdr_content content_descr; /* extended content header */
    basf_hdr_ctx context;           /* header context */
} basf_headers;

/* Global identifiers for the headers we're interested in scanning.
*/
static const basf_guid
    codec_list_guid =
    {{0x86, 0xD1, 0x52, 0x40, 0x31, 0x1D, 0x11, 0xD0, 0xA3, 0xA4, 0x00, 0xA0, 0xC9, 0x03, 0x48, 0xF6}},
    language_list_guid =
    {{0x7C, 0x43, 0x46, 0xA9, 0xEF, 0xE0, 0x4B, 0xFC, 0xB2, 0x29, 0x39, 0x3E, 0xDE, 0x41, 0x5C, 0x85}},
    metadata_guid =
    {{0xC5, 0xF8, 0xCB, 0xEA, 0x5B, 0xAF, 0x48, 0x77, 0x84, 0x67, 0xAA, 0x8C, 0x44, 0xFA, 0x4C, 0xCA}},
    stream_properties_guid =
    {{0xB7, 0xDC, 0x07, 0x91, 0xA9, 0xB7, 0x11, 0xCF, 0x8E, 0xE6, 0x00, 0xC0, 0x0C, 0x20, 0x53, 0x65}},
    ext_stream_properties_guid =
    {{0x14, 0xE6, 0xA5, 0xCB, 0xC6, 0x72, 0x43, 0x32, 0x83, 0x99, 0xA9, 0x69, 0x52, 0x06, 0x5B, 0x5A}},
    file_properties_guid =
    {{0x8C, 0xAB, 0xDC, 0xA1, 0xA9, 0x47, 0x11, 0xCF, 0x8E, 0xE4, 0x00, 0xC0, 0x0C, 0x20, 0x53, 0x65}},
    extended_content_description_guid =
    {{0xD2, 0xD0, 0xA4, 0x40, 0xE3, 0x07, 0x11, 0xD2, 0x97, 0xF0, 0x00, 0xA0, 0xC9, 0x5E, 0xA8, 0x50}},
    content_description_guid = 
    {{0x75, 0xB2, 0x26, 0x33, 0x66, 0x8E, 0x11, 0xCF, 0xA6, 0xD9, 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C}},
    bitrate_properties_guid = 
    {{0x7B, 0xF8, 0x75, 0xCE, 0x46, 0x8D, 0x11, 0xD1, 0x8D, 0x82, 0x00, 0x60, 0x97, 0xC9, 0xA2, 0xB2}};
    
/* Global identifiers for asf media stream types. 
*/
static const basf_guid
  basf_audio_media_stream_guid = 
	{{0xF8, 0x69, 0x9E, 0x40, 0x5B, 0x4D, 0x11, 0xCF, 0xA8, 0xFD, 0x00, 0x80, 0x5F, 0x5C, 0x44, 0x2B}},
  basf_video_media_stream_guid = 
	{{0xBC, 0x19, 0xEF, 0xC0, 0x5B, 0x4D, 0x11, 0xCF, 0xA8, 0xFD, 0x00, 0x80, 0x5F, 0x5C, 0x44, 0x2B}};

/* read an arbitrary array from the current position
*/
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

static unsigned
b_asf_read_wstr(batom_cursor *cursor, char *str, size_t len, size_t asf_len ) 
{
	unsigned i,j = 0;
	if (len == 0 || asf_len == 0) goto exit;
	len--;

	for (i=0;i<asf_len && i<len;i++) {
		str[i] = basf_read_byte(cursor);
	}
	batom_cursor_skip(cursor, asf_len-i);
	for (j=2;j<i;j+=2) {
		str[j/2]=str[j];
	}
	str[j/2]='\0';

	exit:
	return j/2;
}

static unsigned
b_asf_read_wstr_word(batom_cursor *cursor, char *str, size_t len)
{
	return b_asf_read_wstr(cursor, str, len, 2*basf_read_word(cursor));
}

static unsigned
b_asf_read_wstr_byte(batom_cursor *cursor, char *str, size_t len)
{
	return b_asf_read_wstr(cursor, str, len, basf_read_byte(cursor));
}

static int
basf_create_props(basf_props_t* props)
{
    int ret = 0;
    BDBG_ASSERT(props);

    if ((props != NULL) && ((*props = BKNI_Malloc(sizeof(basf_props))) != NULL)) {
        BKNI_Memset((*props), 0, sizeof(basf_props));
        
        (*props)->headers = BKNI_Malloc(sizeof(basf_headers));
        BKNI_Memset((*props)->headers, 0, sizeof(basf_headers));
        
        (*props)->feed_handler.handler = NULL;
        (*props)->feed_handler.context = NULL;        
    } else ret = (-1);
    
    return (ret);
}

static int 
basf_cleanup_props(basf_props_t props)
{
    int ret = 0;
    BDBG_ASSERT(props != NULL);

    if (props != NULL) {
        BDBG_ASSERT((props)->headers != NULL);
        if ((props)->headers != NULL) {
            BDBG_MSG(("cleaning up headers"));
            BKNI_Free((props)->headers);
        }
        BDBG_MSG(("cleaning up props"));
        BKNI_Free(props);        
    } else ret = (-1);    
    
    return (ret);
}

static void
content_description_header_reset(basf_hdr_content* content_hdr)
{
   BDBG_ASSERT (content_hdr != NULL);
   if (content_hdr != NULL) {
       BDBG_MSG(("cleaning up content header"));
       FREE_SET(content_hdr->title);
       FREE_SET(content_hdr->author);
       FREE_SET(content_hdr->copyright);
       FREE_SET(content_hdr->description);
       FREE_SET(content_hdr->rating);
   }   
}

static void
content_description_header_set(batom_cursor* cursor, basf_hdr_content* content_hdr)
{
   basf_word content_title_len = 0, content_author_len = 0, content_copyr_len = 0;
   basf_word content_descr_len = 0, content_rating_len = 0;

   BDBG_ASSERT(content_hdr);
   if (content_hdr != NULL) {
       content_hdr->title = NULL; content_hdr->author = NULL;
       content_hdr->copyright = NULL; content_hdr->description = NULL;
       content_hdr->rating = NULL;
   } else return;

   content_title_len = basf_read_word(cursor); content_author_len = basf_read_word(cursor);
   content_copyr_len = basf_read_word(cursor); content_descr_len = basf_read_word(cursor);
   content_rating_len = basf_read_word(cursor);
   
   if (content_title_len) {
       MALLOC_SET(content_hdr->title, content_title_len); 
       b_asf_read_wstr(cursor, content_hdr->title, content_title_len, content_title_len);   
   }    
   if (content_author_len) {
       MALLOC_SET(content_hdr->author, content_author_len);   
       b_asf_read_wstr(cursor, content_hdr->author, content_author_len, content_author_len);
   }       
   if (content_copyr_len) {
       MALLOC_SET(content_hdr->copyright, content_copyr_len); 
       b_asf_read_wstr(cursor, content_hdr->copyright, content_copyr_len, content_copyr_len);
   }
   if (content_descr_len) {
       MALLOC_SET(content_hdr->description, content_descr_len);
       b_asf_read_wstr(cursor, content_hdr->description, content_descr_len, content_descr_len);
   }       
   if (content_rating_len) {
       MALLOC_SET(content_hdr->rating, content_rating_len);
       b_asf_read_wstr(cursor, content_hdr->rating, content_rating_len, content_rating_len);
   }

#ifdef _DEBUG
   BDBG_MSG(("content-title=%s(%d)\ncontent-author=%s(%d)\ncontent-copyr=%s(%d)\n"
          "content-descr=%s(%d)\ncontent-rating=%s(%d)\n",
          content_hdr->title, content_title_len,
          content_hdr->author, content_author_len,
          content_hdr->copyright, content_copyr_len, 
          content_hdr->description, content_descr_len,
          content_hdr->rating, content_rating_len));
#endif /* _DEBUG */
}

static basf_parser_handler_action
content_description_object(struct basf_parser_object_handler *handler, batom_t atom)
{
   batom_cursor cursor;
   basf_props_t props = ((struct basf_gen_object_handler *)handler)->props;
   basf_hdr_content* content_hdr = &props->headers->content_descr;

   BDBG_MSG(("content_description_object"));
   batom_cursor_from_atom(&cursor, atom);

   content_description_header_set(&cursor, content_hdr);
   ((struct basf_gen_object_handler *)handler)->state = OBJ_STATE_COMPLETE;

   return  basf_parser_handler_action_none;
}

static basf_parser_handler_action
extended_content_description_object(struct basf_parser_object_handler *handler, batom_t atom)
{
    basf_word content_descriptor_count;
    unsigned i;
	batom_cursor cursor;

    BSTD_UNUSED(handler);

    BDBG_MSG(("extended_content_description_object"));
	batom_cursor_from_atom(&cursor, atom);
    content_descriptor_count = basf_read_word(&cursor);
    for(i=0;i<content_descriptor_count;i++) {
        char descriptor_name[128];
        basf_word value_type;
        basf_word value_len;

        b_asf_read_wstr(&cursor, descriptor_name, sizeof(descriptor_name), basf_read_word(&cursor));
        value_type = basf_read_word(&cursor);
        value_len = basf_read_word(&cursor);
        batom_cursor_skip(&cursor, value_len);
        BDBG_ASSERT(!BATOM_IS_EOF(&cursor));
        BDBG_MSG(("description: %u %s\n", i, descriptor_name));
    }

    BDBG_ASSERT(!BATOM_IS_EOF(&cursor));
    ((struct basf_gen_object_handler *)handler)->state = OBJ_STATE_COMPLETE;
    
    return  basf_parser_handler_action_none/* basf_parser_handler_action_return */;
}

static basf_parser_handler_action
file_properties_object(struct basf_parser_object_handler *handler, batom_t atom)
{
    basf_dword flags;
	batom_cursor cursor;

#define B_ASF_Broadcast_Flag B_GET_BIT(flags,0)
#define B_ASF_Seekable_Flag B_GET_BIT(flags,1)

    basf_props_t props = ((struct basf_gen_object_handler *)handler)->props;
    basf_hdr_file* file_hdr = &props->headers->file;    

	batom_cursor_from_atom(&cursor, atom);

    basf_read_guid(&cursor, &file_hdr->id);
    file_hdr->size = basf_read_qword(&cursor);
    file_hdr->creation_date = basf_read_qword(&cursor);

    file_hdr->datapack_count = basf_read_qword(&cursor);
    file_hdr->play_duration = basf_read_qword(&cursor);
    file_hdr->send_duration = basf_read_qword(&cursor);
    file_hdr->preroll = basf_read_qword(&cursor);    

    file_hdr->flags = flags = basf_read_dword(&cursor);
    if (B_ASF_Broadcast_Flag) { BDBG_MSG((" Broadcast\n")); }
    if (B_ASF_Seekable_Flag) { BDBG_MSG((" Seekable\n")); }

    file_hdr->min_dt_pack_size = basf_read_dword(&cursor);
    file_hdr->max_dt_pack_size = basf_read_dword(&cursor);
    file_hdr->max_bitrate = basf_read_dword(&cursor);

    BDBG_ASSERT(!BATOM_IS_EOF(&cursor));
    ((struct basf_gen_object_handler *)handler)->state = OBJ_STATE_COMPLETE;

    return  basf_parser_handler_action_none/* basf_parser_handler_action_return */;
}

int 
basf_stream_hdr_insert(basf_props_t props, basf_hdr_stream** out_hdr)
{
    int ret = 0;
    basf_hdr_stream* iter, *stream_hdr;

    stream_hdr = BKNI_Malloc(sizeof(basf_hdr_stream));
    for(iter = BLST_S_FIRST(&props->headers->streams); iter; 
        iter = BLST_S_NEXT(iter,link));
        ret++; printf("+");

    if(iter == NULL){
        BLST_S_INSERT_HEAD(&props->headers->streams, stream_hdr, link);
        *out_hdr = stream_hdr;
    }
    return (ret);
}

int 
basf_ext_stream_hdr_insert(basf_props_t props, basf_hdr_ext_stream** out_hdr)
{
    int ret = 0;
    basf_hdr_ext_stream* iter, *ext_stream_hdr;

    ext_stream_hdr = BKNI_Malloc(sizeof(basf_hdr_ext_stream));
    for(iter = BLST_S_FIRST(&props->headers->ext_streams); iter; 
        iter = BLST_S_NEXT(iter,link));
        ret++; printf("+");

    if(iter == NULL){
        BLST_S_INSERT_HEAD(&props->headers->ext_streams, ext_stream_hdr, link);
        *out_hdr = ext_stream_hdr;
    }
    return (ret);
}

int 
basf_stream_hdr_cleanup(basf_props_t props)
{
    int ret = 0, count = 0;    
    basf_hdr_stream* stream_hdr, *lnk;
    BDBG_MSG(("--stream headers cleanup\n"));

    for(stream_hdr = BLST_S_FIRST(&props->headers->streams); stream_hdr; 
        stream_hdr = lnk, count++) {
            BDBG_MSG(("cleaning up stream (%d)\n", count));
            lnk = BLST_S_NEXT(stream_hdr, link); BKNI_Free(stream_hdr);
        }

    return (ret);
}

/* extract either audio/video stream type information, such as CodecId
 * only works with A/V for now. This is needed to extract the codec_id to then 
 * match it with one of the codec objects.
 * */
void
stream_properties_object_type_spec(basf_hdr_stream* stream_hdr, batom_cursor* cursor,
        basf_media_stream_type stream_type)
{
    basf_word video_fmt_dtsize;
    batom_checkpoint start; 
    basf_dword codec_id = BASF_VIDEO_UNK_FOURCC;
    
    BDBG_ASSERT(stream_hdr && cursor);
    batom_cursor_save(cursor, &start);                                    /* save cursor */
    
    switch (stream_type) {
    case basf_media_stream_video: 
        batom_cursor_skip(cursor, BASF_VIDEO_OBJTYPE_BMPHDR_OFF);         /* skip up to fromat data BITMAPINFOHEADER */
        if ((video_fmt_dtsize = basf_read_word(cursor)) != 0) {
            batom_cursor_skip(cursor, BASF_VIDEO_BMPHDR_COMPRESSID_OFF);  /* skip into BITMAPINFOHEADER data */
            codec_id = basf_read_dword(cursor);                         /* read compression id data */
        } else { codec_id = BASF_VIDEO_UNK_FOURCC; }
        BDBG_MSG_TRACE(("video stream type, codec_id=%.2X %.2X %.2X %.2X (%d)", 
                    ((uint8_t*)&codec_id)[0], ((uint8_t*)&codec_id)[1],
                    ((uint8_t*)&codec_id)[2], ((uint8_t*)&codec_id)[3], 
                    codec_id)); break;
    case basf_media_stream_audio: 
        codec_id = basf_read_word(cursor);
        BDBG_MSG_TRACE(("audio stream type, codec_id=%.2X %.2X %.2X %.2X (%d)", 
                    ((uint8_t*)&codec_id)[0], ((uint8_t*)&codec_id)[1],
                    ((uint8_t*)&codec_id)[2], ((uint8_t*)&codec_id)[3], 
                    codec_id)); break;
        case basf_media_stream_unk:
        BDBG_MSG_TRACE(("unknown stream type")); break;
    }
    BDBG_MSG_TRACE(("codec %d is : %s %s %s", codec_id,
        basf_codec_vc1_simplemain(codec_id)? "[sm]"  : "[NOT sm]",
        basf_codec_vc1_advanced(codec_id)  ? "[ap]"  : "[NOT ap]",
        basf_codec_wma_standard(codec_id)  ? "[wma]" : "[NOT wma]"));
    stream_hdr->codec_id = codec_id;
    batom_cursor_rollback(cursor, &start);                                 /* restore old pos */
}

static basf_parser_handler_action
stream_properties_object(basf_parser_object_handler *handler, batom_t atom)
{
    basf_word flags;
    basf_guid stream_type;
    basf_hdr_stream* stream_hdr;
	batom_cursor cursor;

    basf_props_t props = ((struct basf_gen_object_handler *)handler)->props;
    basf_stream_hdr_insert(props, &stream_hdr);

    BDBG_MSG(("stream_properties_object"));
	batom_cursor_from_atom(&cursor, atom);
    
    basf_read_guid(&cursor, &stream_type);
    if(basf_compare_guid(&stream_type, &basf_video_media_stream_guid) == 0)
        stream_hdr->stream_type = basf_media_stream_video;
    else if(basf_compare_guid(&stream_type, &basf_audio_media_stream_guid) == 0)
        stream_hdr->stream_type = basf_media_stream_audio;
    else
        stream_hdr->stream_type = basf_media_stream_unk;

    basf_read_guid(&cursor, &stream_hdr->err_corr_type);
    stream_hdr->time_offset = basf_read_qword(&cursor);
    stream_hdr->type_spec_datalen = basf_read_dword(&cursor);
    stream_hdr->err_cor_datalen = basf_read_dword(&cursor);
    stream_hdr->flags = flags = basf_read_word(&cursor);

    BDBG_MSG(("properties stream: %u",B_GET_BITS(flags,6,0)));

#ifdef _DEBUG
    BDBG_MSG(("properties stream: %d, reserved: %d, encrypted: %d\n", 
        B_GET_BITS(flags,6,0), 
        B_GET_BITS(flags,14,7), 
        B_GET_BITS(flags,15,15)));        
#endif /* _DEBUG */

    batom_cursor_skip(&cursor, sizeof(basf_dword));
    if (stream_hdr->type_spec_datalen != 0) {
        /* batom_cursor_skip(cursor, stream_hdr->type_spec_datalen); */
        stream_properties_object_type_spec(stream_hdr, &cursor, stream_hdr->stream_type);
    }
    batom_cursor_skip(&cursor, stream_hdr->err_cor_datalen);
    BDBG_ASSERT(!BATOM_IS_EOF(&cursor));
    ((struct basf_gen_object_handler *)handler)->state = OBJ_STATE_COMPLETE;

    return basf_parser_handler_action_none;
}

static basf_parser_handler_action
bitrate_properties_object(basf_parser_object_handler *handler, batom_t atom)
{
    basf_word index;    
    basf_word flags;
	batom_cursor cursor;
    
    basf_props_t props = ((struct basf_gen_object_handler *)handler)->props;
    basf_hdr_bitrate* bitrate_hdr = &props->headers->bitrate;    
    
    BDBG_MSG(("bitrate_properties_object"));
	batom_cursor_from_atom(&cursor, atom);
    bitrate_hdr->count = basf_read_word(&cursor);
    for(index = 0; index < bitrate_hdr->count; index++) {    
        flags = basf_read_word(&cursor);
        bitrate_hdr->stream_bitrate[index] = basf_read_dword(&cursor);
        bitrate_hdr->stream_id[index] = B_GET_BITS(flags,6,0);
#ifdef _DEBUG
    BDBG_ERR(("bitrate stream: %d [%d], reserved: %d, bitrate: %d\n", 
        B_GET_BITS(flags,6,0), bitrate_hdr->count,
        B_GET_BITS(flags,15,7), bitrate_hdr->stream_bitrate[index]));        
#endif /* _DEBUG */
    }

    BDBG_ASSERT(!BATOM_IS_EOF(&cursor));
    ((struct basf_gen_object_handler *)handler)->state = OBJ_STATE_COMPLETE;
    
    /* should continue to scan headers */
    return basf_parser_handler_action_none; /* basf_parser_handler_action_return */
}

static basf_parser_handler_action
ext_stream_properties_object(basf_parser_object_handler *handler, batom_t atom)
{
    basf_word flags;
    basf_guid stream_type;
    basf_hdr_ext_stream* ext_stream_hdr;
	batom_cursor cursor;

    basf_props_t props = ((struct basf_gen_object_handler *)handler)->props;
    basf_ext_stream_hdr_insert(props, &ext_stream_hdr);

    BDBG_MSG(("ext_stream_properties_object"));
	batom_cursor_from_atom(&cursor, atom);

    BSTD_UNUSED(flags);
    BSTD_UNUSED(stream_type);

    BDBG_ASSERT(!BATOM_IS_EOF(&cursor));
    ((struct basf_gen_object_handler *)handler)->state = OBJ_STATE_COMPLETE;

    return basf_parser_handler_action_none;
}

static basf_parser_handler_action
metadata_object(basf_parser_object_handler *handler, batom_t atom)
{
    unsigned i;
    basf_word description_records_count;
    basf_hdr_metadata* meta_hdr;
	batom_cursor cursor;

    basf_props_t props = ((struct basf_gen_object_handler *)handler)->props;
    meta_hdr = &props->headers->meta;

    BSTD_UNUSED(meta_hdr);
    BDBG_MSG(("metadata_object"));
	batom_cursor_from_atom(&cursor, atom);

    description_records_count=basf_read_word(&cursor);
    for(i=0;i<description_records_count;i++) {
        basf_word stream_number;
        basf_word name_length;
        basf_word data_type;
        basf_dword data_length;
        char name[128];

        batom_cursor_skip(&cursor, sizeof(basf_word));
        stream_number = basf_read_word(&cursor);
        name_length = basf_read_word(&cursor);
        data_type = basf_read_word(&cursor);
        data_length = basf_read_dword(&cursor);
        b_asf_read_wstr(&cursor, name, sizeof(name), name_length);
        batom_cursor_skip(&cursor, data_length);
        BDBG_ASSERT(!BATOM_IS_EOF(&cursor));
        BDBG_MSG(("metadata: %u stream:%u %s\n", i, stream_number, name));
    }
    BDBG_ASSERT(!BATOM_IS_EOF(&cursor));
    ((struct basf_gen_object_handler *)handler)->state = OBJ_STATE_COMPLETE;

    return basf_parser_handler_action_none;
}

static basf_parser_handler_action
language_list_object(basf_parser_object_handler *handler, batom_t atom)
{
    unsigned i;
    char lang[128];
    basf_word language_ID_records_count;
    basf_hdr_language* language_hdr;
	batom_cursor cursor;
    
    basf_props_t props = ((struct basf_gen_object_handler *)handler)->props;
    language_hdr = &props->headers->language;

    BSTD_UNUSED(language_hdr);
    BDBG_MSG(("language_list_object"));
	batom_cursor_from_atom(&cursor, atom);

    language_ID_records_count=basf_read_word(&cursor);
    for(i=0;i<language_ID_records_count;i++) {
        b_asf_read_wstr_byte(&cursor, lang, sizeof(lang));
        BDBG_ASSERT(!BATOM_IS_EOF(&cursor));
        BDBG_MSG(("Language: %u %s\n", i, lang));
    }
    BDBG_ASSERT(!BATOM_IS_EOF(&cursor));
    ((struct basf_gen_object_handler *)handler)->state = OBJ_STATE_COMPLETE;
   
    return basf_parser_handler_action_none;
}

static void
basf_codec_hdr_init(basf_hdr_codec_indiv* codec_hdr) 
{
    BDBG_ASSERT(codec_hdr != NULL);
    if (codec_hdr != NULL) {
        BKNI_Memset(codec_hdr->name, 0, sizeof(codec_hdr->name));
        BKNI_Memset(codec_hdr->description, 0, sizeof(codec_hdr->description));
    }
}

int 
basf_codec_hdr_insert(basf_props_t props, basf_hdr_codec_indiv** out_hdr)
{
    int ret = 0;
    basf_hdr_codec_indiv* iter, *codec_hdr;

    codec_hdr = BKNI_Malloc(sizeof(basf_hdr_codec_indiv));
    for(iter = BLST_S_FIRST(&props->headers->codecs.list); iter; 
        iter = BLST_S_NEXT(iter,link));
        ret++; printf("+");

    if(iter == NULL) {
        BLST_S_INSERT_HEAD(&props->headers->codecs.list, codec_hdr, link);
        *out_hdr = codec_hdr;
        basf_codec_hdr_init(*out_hdr);
    }
    return (ret);
}

int 
basf_codec_hdr_cleanup(basf_props_t props)
{
    int count = 0, ret = 0;
    basf_hdr_codec_indiv* codec_hdr, *lnk;
    BDBG_MSG(("--codec headers cleanup\n"));

    for(codec_hdr = BLST_S_FIRST(&props->headers->codecs.list); codec_hdr; 
        codec_hdr = lnk, count++) {
            BDBG_MSG(("cleaning up codec (%d)\n", count));     
            BKNI_Free(codec_hdr->info);
            lnk = BLST_S_NEXT(codec_hdr, link); BKNI_Free(codec_hdr);
        }

    return (ret);
}

static basf_parser_handler_action
codec_list_object(struct basf_parser_object_handler *handler, batom_t atom)
{
    basf_dword type;
    unsigned i;
    const char *stype;
    size_t codec_count;
    basf_hdr_codec_indiv* codec_hdr;
    basf_props_t props = ((struct basf_gen_object_handler *)handler)->props;
	batom_cursor cursor;
    
    BDBG_MSG(("Codec list object"));
	batom_cursor_from_atom(&cursor,atom);
    batom_cursor_skip(&cursor, sizeof(basf_guid));
    codec_count = basf_read_dword(&cursor);

    for(i=0;i<codec_count;i++) {
        basf_codec_hdr_insert(props, &codec_hdr);        
        type = basf_read_word(&cursor);
        b_asf_read_wstr_word(&cursor, codec_hdr->name, sizeof(codec_hdr->name));
        b_asf_read_wstr_word(&cursor, codec_hdr->description, sizeof(codec_hdr->description));

        codec_hdr->info_len = basf_read_word(&cursor);
        codec_hdr->info = BKNI_Malloc(codec_hdr->info_len);
        basf_read_arr(&cursor, codec_hdr->info, codec_hdr->info_len);
        
        switch(type) {
        case 1: stype="Video";codec_hdr->type = asf_cdt_video;break;
        case 2: stype="Audio";codec_hdr->type = asf_cdt_audio;break;
        default: stype="Unknown";codec_hdr->type = asf_cdt_unknown;break;;
        }

        BDBG_ASSERT(!BATOM_IS_EOF(&cursor));
        BDBG_MSG(("%u: %s codec: %s %s %d", i, stype, codec_hdr->name, codec_hdr->description,
               codec_hdr->info_len));        
    }

    BDBG_ASSERT(!BATOM_IS_EOF(&cursor));
    ((struct basf_gen_object_handler *)handler)->state = OBJ_STATE_COMPLETE;

    return basf_parser_handler_action_none;
}


/* Set flag indicating if demux is attached 
*/
static void 
basf_set_attached_streams (basf_props_t props, bool attached)
{
    props->demux_attached = attached;
    BDBG_MSG_TRACE(("setting attached: %d", attached));
}

/* Return demux attached streams flag 
*/
static bool 
basf_attached_streams (basf_props_t props)
{
    return props->demux_attached;
}

/* Return status indicating if demux is enabled 
*/
static bool 
basf_demux_enabled (basf_props_t props)
{
    return (props->headers) && 
        (props->headers->context == basf_hdr_ctx_gen_demux);
}

/* Return status indicating if generic context is used 
*/
static bool 
basf_gen_ctx_enabled (basf_props_t props)
{
    return (props->headers) && 
        ((props->headers->context == basf_hdr_ctx_gen_demux) || 
         (props->headers->context == basf_hdr_ctx_gen));
}

/* Initialize prop headers and stream handlers 
*/
static void 
basf_init_props_headers(basf_props_t props)
{
    BLST_S_INIT(&props->headers->streams);
    BLST_S_INIT(&props->headers->ext_streams);
    BLST_S_INIT(&props->headers->codecs.list);

    props->stream_handler.props = props; 
    props->content_handler.props = props;
    props->lang_handler.props = props; 

    props->meta_handler.props = props;
    props->extc_handler.props = props; 
    props->codec_handler.props = props;

    props->file_handler.props = props; 
    props->ext_stream_handler.props = props;
    props->bitrate_handler.props = props;
}   

/* Install prop header handler callback functions 
*/
static void
basf_install_props_handlers(basf_props_t props)
{
    basf_parser_install_object_handler(props->asf, &props->codec_handler.handler, 
            &codec_list_guid, codec_list_object);
    basf_parser_install_object_handler(props->asf, &props->lang_handler.handler, 
            &language_list_guid, language_list_object);
    basf_parser_install_object_handler(props->asf, &props->meta_handler.handler, 
            &metadata_guid, metadata_object);

    basf_parser_install_object_handler(props->asf, &props->file_handler.handler, 
            &file_properties_guid, file_properties_object);
    basf_parser_install_object_handler(props->asf, &props->extc_handler.handler, 
            &extended_content_description_guid, extended_content_description_object);
    basf_parser_install_object_handler(props->asf, &props->stream_handler.handler, 
            &stream_properties_guid, stream_properties_object);

    basf_parser_install_object_handler(props->asf, &props->ext_stream_handler.handler, 
            &ext_stream_properties_guid, ext_stream_properties_object);
    basf_parser_install_object_handler(props->asf, &props->content_handler.handler, 
            &content_description_guid, content_description_object);
    basf_parser_install_object_handler(props->asf, &props->bitrate_handler.handler, 
            &bitrate_properties_guid, bitrate_properties_object);        
}

/* Open the stream and assign it an opaque handle
*/
int 
basf_config_asf(basf_props_t props, int quiet)
{
    int ret = 0;
    BSTD_UNUSED(quiet);
   
	props->factory = batom_factory_create(bkni_alloc, 16);
	if(!props->factory) {
		return -1;
	}
	props->pipe = batom_pipe_create(props->factory);
	BDBG_ASSERT(props->pipe);

    basf_parser_default_cfg(&props->asf_cfg);
    
    props->asf = basf_parser_create(props->factory, &props->asf_cfg);
	BDBG_ASSERT(props->asf);
    basf_set_attached_streams(props, false);
    
    /* only hook up the demux if asked from the outside */
    if (basf_demux_enabled(props)) { 
        basf_demux_default_cfg(&props->demux_cfg);
        props->demux = basf_demux_create(props->asf, props->factory, bkni_alloc, &props->demux_cfg);
		BDBG_ASSERT(props->demux);
    }
    
    /* when using generic context, also install hooks */
    if (basf_gen_ctx_enabled(props)) {
        basf_init_props_headers(props);
        basf_install_props_handlers(props);
    }

    return (ret);
}

/* Zerorize demux streams, if they exist
*/
static void
basf_detach_streams(basf_props_t props)
{
    if (props->demux) {
        if (props->video.stream) {          /* detach video */
            basf_stream_detach(props->demux, props->video.stream);
            props->video.stream = NULL;
        }
        if (props->audio.stream) {          /* detach audio */
            basf_stream_detach(props->demux, props->audio.stream);
            props->audio.stream = NULL;
        }
    }
}

/* Close asf handle and uninitialize callbacks 
*/
int 
basf_close_asf(basf_props_t pprops)
{
    int ret = 0; 
    basf_props_t props = pprops;
    
    if (props == NULL) {
        return (ret);
    }
   
    BDBG_MSG_TRACE(("closing asf parser"));
    
    /* only do this if outside demux asks for it */
    if (basf_demux_enabled(props)) {
        if (basf_attached_streams(props)) {
            basf_detach_streams(props);
        }
        basf_demux_destroy(props->demux);
    }
    
    /* cleanup headers, uninstall hooks when using generic context */ 
    if (basf_gen_ctx_enabled(props)) {

        BDBG_MSG_TRACE(("cleaning generic context"));
        if (props->content_handler.state == OBJ_STATE_COMPLETE) {
            content_description_header_reset(&props->headers->content_descr);
        }
        if (props->codec_handler.state == OBJ_STATE_COMPLETE) {
            basf_codec_hdr_cleanup(props); 
        }
        if (props->stream_handler.state == OBJ_STATE_COMPLETE) {
            basf_stream_hdr_cleanup(props);
        }
        
        /* uninstall object handler hooks into parser */
        basf_parser_remove_object_handler(props->asf, &props->codec_handler.handler);
        basf_parser_remove_object_handler(props->asf, &props->lang_handler.handler);
        basf_parser_remove_object_handler(props->asf, &props->meta_handler.handler);
        basf_parser_remove_object_handler(props->asf, &props->file_handler.handler);
        
        basf_parser_remove_object_handler(props->asf, &props->extc_handler.handler);
        basf_parser_remove_object_handler(props->asf, &props->stream_handler.handler);
        basf_parser_remove_object_handler(props->asf, &props->ext_stream_handler.handler);
        basf_parser_remove_object_handler(props->asf, &props->content_handler.handler);
        basf_parser_remove_object_handler(props->asf, &props->bitrate_handler.handler);
    }

    basf_parser_destroy(props->asf); 
    BDBG_MSG_TRACE(("destroyed asf instance"));
    ret = 0;
	batom_pipe_destroy(props->pipe);
	batom_factory_destroy(props->factory);
    
    if (basf_src_type_file == props->src_type) {
        fclose(props->fin); 
        props->fin = NULL;
    }

    ret = basf_cleanup_props(props); 
    props = NULL;

    return (ret);
}

/* Set up demux parameters 
*/
void
basf_config_demux(basf_props_t props, uint8_t video_stream, uint8_t video_id,
                  uint8_t audio_stream, uint8_t audio_id)
{
    props->video.stream = props->audio.stream = NULL;
    basf_set_attached_streams(props, true);
	BSTD_UNUSED(audio_id);
	BSTD_UNUSED(video_id);
    
    if (video_stream) {
        basf_stream_initialize(&props->stream_cfg);
		props->video.pipe = batom_pipe_create(props->factory);
		BDBG_ASSERT(props->video.pipe);
        props->video.pid_cont_counter = 0;
        props->video.stream  = basf_stream_attach(props->demux, props->video.pipe, video_stream, &props->stream_cfg);
    }
    
    if (audio_stream) {
        basf_stream_initialize(&props->stream_cfg);
   		props->audio.pipe = batom_pipe_create(props->factory);
		BDBG_ASSERT(props->audio.pipe);
   		props->audio.pid_cont_counter = 0;
        props->audio.stream  = basf_stream_attach(props->demux, props->audio.pipe, audio_stream, &props->stream_cfg);
    }
}

/* Tells the app whether to continue or not in the general case
*/
static basf_feed_control 
basf_feed_state_gen_handler(basf_props_t props)
{
    basf_feed_control ret = basf_feed_continue;

    /* we stop at the stream header, should be last (?) */
    if(props->stream_handler.state != OBJ_STATE_COMPLETE)
        return (ret);

#ifdef __HAS_ASFSCAN_HDRSTOP__
    if(props->meta_handler.state != OBJ_STATE_COMPLETE)
        return (ret);
    if(props->extc_handler.state != OBJ_STATE_COMPLETE)
        return (ret);
    if(props->content_handler.state != OBJ_STATE_COMPLETE)
        return (ret);
    if(props->codec_handler.state != OBJ_STATE_COMPLETE)
        return (ret);
    if(props->file_handler.state != OBJ_STATE_COMPLETE)
        return (ret);
    if(props->content_handler.state != OBJ_STATE_COMPLETE)
        return (ret);
#endif /* __HAS_ASFSCAN_HDRSTOP__ */

    return (ret = basf_feed_stop);
}

/* Extract property handle from the ASF file specified in the file 
*/
basf_props_t 
basf_open_asf(const char* file, basf_hdr_ctx context, basf_parser_t* asf)
{
    basf_props_t props = NULL;    

    if (-1 != basf_create_props(&props)) {

        props->fin = fopen(file,"rb");
        props->src_type = basf_src_type_file;
    
        if (!props->fin) {
            BDBG_WRN(("could not open: %s\n", file));
            perror(file); return (props);
        } else {
            BDBG_MSG(("opened %s\n", file));
    
            BDBG_ASSERT (props != NULL); BDBG_ASSERT (props->headers != NULL);
            if ((props != NULL) && (props->headers != NULL)) {
                props->headers->context = context;
            }
       
            basf_config_asf(props, 0);    
            BDBG_ASSERT(asf != NULL);
            
            if (asf != NULL) {
                (*asf) = props->asf;
            }
        }
    }

    BDBG_ASSERT(props);
    
    return (props);
}

/* Extract property handle from the ASF file chunk specified
*/
basf_props_t 
basf_open_asf_chunk(unsigned char* chunk_data, unsigned int chunk_size, 
                 basf_hdr_ctx context, basf_parser_t* asf)
{
    basf_props_t props = NULL;    

    if (-1 != basf_create_props(&props)) {

        props->ext_chunk.data = chunk_data;
        props->ext_chunk.size = chunk_size;
        props->src_type = basf_src_type_chunk;
    
        if (!props->ext_chunk.size || props->ext_chunk.data==NULL) {
            BDBG_WRN(("could not open chunk sized: %d\n", props->ext_chunk.size));
            return (props);
        } else {
            BDBG_MSG(("opened chunk sized: %d\n", props->ext_chunk.size));
    
            BDBG_ASSERT (props != NULL); BDBG_ASSERT (props->headers != NULL);
            if ((props != NULL) && (props->headers != NULL)) props->headers->context = context;
    
            basf_config_asf(props, 0);    
    
            BDBG_ASSERT(asf != NULL);
            if (asf != NULL) (*asf) = props->asf;
        }
    }

    return (props);
}

/* Controls the stop condition for the scanner
*/
static basf_feed_control 
basf_props_feed_state(basf_props_t props)
{   
    basf_parser_info asf_info;
    basf_feed_control ret = basf_feed_continue;

    BDBG_ASSERT(props); BDBG_ASSERT(props->headers);

    /* look for header terminator, and stop if hit */
    basf_parser_get_info(props->asf, &asf_info);
    if(asf_info.state == basf_parser_state_data_object) {
        ret = basf_feed_stop;
    } else {
        switch(props->headers->context) {
            case basf_hdr_ctx_gen: 
            case basf_hdr_ctx_gen_demux:
                ret = basf_feed_state_gen_handler(props); 
                break;        
            case basf_hdr_ctx_custom:
                if (props->feed_handler.handler != NULL) {
                    ret = props->feed_handler.handler(props->feed_handler.context); 
                }
                break;
            case basf_hdr_ctx_unk:
                BDBG_WRN(("undefined header context")); 
                break;
        }
    }

    return (ret);
}

static void 
b_asf_props_atom_free(batom_t atom, void *user)
{
	void *ptr = *(void **)user;
	BDBG_MSG(("free_asf %#lx", (unsigned long)atom));
	BSTD_UNUSED(atom);
	BKNI_Free(ptr);
	return;
}

static const batom_user b_asf_props_atom = {
	b_asf_props_atom_free,
	sizeof(void **)
};


/* Main asf loop scanner
*/
int 
basf_scan_asf(basf_props_t props)
{
    char *buf; int rc;
    size_t feed_len;
    unsigned int len, loop_count = 1, loop_stop = 0;

    BDBG_ASSERT(props->src_type == basf_src_type_file);
    BSTD_UNUSED(loop_stop);     /* actually used if HAS_ASFSCAN_LOOPSTOP */
    
    for(;;) {
		batom_t atom;
        len = BUF_SIZE;
        buf = BKNI_Malloc(len);
        if (!buf) {
            perror("malloc");return (1);
        }
        BDBG_MSG_TRACE(("alloc %#lx", (unsigned long)buf));

        for(;;) {
            BDBG_MSG_TRACE(("read %ld:%u", ftell(props->fin)));
            rc = fread(buf, 1, len, props->fin);
            if (rc>0) {
                len = (size_t)rc;
            } else  {
                if (feof(props->fin)) {
                    loop_count--;
                    if (loop_count>0) {
                        BDBG_WRN(("looping %u", loop_count));
                        fseek(props->fin, 0, SEEK_SET);
                        continue;
                    }
                    len=0;
                    break;
                } else {
                    perror("fread");
                    return (1);
                }
            }
            break;
        }
        if (len==0) {
            break;
        }
		atom = batom_from_range(props->factory, buf, len, &b_asf_props_atom, &buf);
		BDBG_ASSERT(atom);
        buf = NULL; /* its either been added to list or already freed */
		batom_pipe_push(props->pipe, atom);

#ifdef __HAS_ASFSCAN_LOOPSTOP__        
        /* loop (feed) one more time to allow recycle function to be called. */
        if (basf_feed_stop == basf_props_feed_state(props)) {
            loop_stop = 1; 
        }
#endif /* __HAS_ASFSCAN_LOOPSTOP__ */        
        
        if (basf_feed_stop == basf_props_feed_state(props)) {
            basf_parser_reset(props->asf);
            break;
        }
        if ((feed_len = basf_parser_feed(props->asf, props->pipe)) == 0) {
            if (basf_demux_enabled(props) && basf_attached_streams(props)) { 
                basf_detach_streams(props);
            }
            feed_len = basf_parser_feed(props->asf, props->pipe);
            BDBG_ASSERT(feed_len==len);            
        } 

#ifdef __HAS_ASFSCAN_LOOPSTOP__        
        else if (loop_stop) {
			batom_pipe_flush(pipe);
            basf_parser_reset(props->asf);
            BDBG_MSG(("resetting loop to original state"));
            break;
        }
#endif /* __HAS_ASFSCAN_LOOPSTOP__ */        
    
        
    } /* for(;;) */

    if (buf) {
        BDBG_MSG_TRACE(("free %#lx", (unsigned long)buf));
        BKNI_Free(buf);
    }
	batom_pipe_flush(props->pipe);
	
    
    return (0);
}

/* Scan one chunk at a time, coming from an outside source.
*/
int 
basf_scan_asf_chunk(basf_props_t props)
{
    size_t feed_len;
    unsigned int loop_stop = 0;
	batom_t atom;

    BDBG_ASSERT(props->src_type == basf_src_type_chunk);

    if ((props->src_type != basf_src_type_chunk) || 
        (basf_feed_stop == basf_props_feed_state(props))) {
        basf_parser_reset(props->asf); 
        goto out;
    } 
	/* XXX assume that user blocks are BKNI_Alloc allocated */
	atom = batom_from_range(props->factory, props->ext_chunk.data, props->ext_chunk.size, &b_asf_props_atom, &props->ext_chunk.data);
	BDBG_ASSERT(atom);
	batom_pipe_push(props->pipe, atom);
    if ((feed_len = basf_parser_feed(props->asf, props->pipe)) == 0) {
        if (basf_demux_enabled(props) && basf_attached_streams(props)) { 
            basf_detach_streams(props);
        }
        feed_len = basf_parser_feed(props->asf, props->pipe);
        BDBG_ASSERT(feed_len==props->ext_chunk.size);            
   } else if (loop_stop) {
        basf_parser_reset(props->asf);
        BDBG_MSG(("resetting loop to original state"));
        goto out;
   }
        
out:
    return (0);
}

/* Header accesor functions.
*/
void 
basf_get_codec_hdr(basf_props_t props, basf_hdr_codec_lst** codecs)
{
    BDBG_ASSERT(codecs != NULL);        
    if(codecs != NULL) *codecs = &props->headers->codecs.list;
}

void 
basf_get_lang_hdr(basf_props_t props, basf_hdr_language** lang)
{
    BDBG_ASSERT(lang != NULL);        
    if(lang != NULL) *lang = &props->headers->language;
}

void 
basf_get_meta_hdr(basf_props_t props, basf_hdr_metadata** meta)
{
    BDBG_ASSERT(meta != NULL);        
    if(meta != NULL) *meta = &props->headers->meta;
}

void 
basf_get_stream_list_hdr(basf_props_t props, basf_hdr_stream_lst** streams)
{
    BDBG_ASSERT(streams != NULL);        
    if(streams != NULL) *streams = &props->headers->streams;
}

void 
basf_get_file_hdr(basf_props_t props, basf_hdr_file** file)
{
    BDBG_ASSERT(file != NULL);      
    if(file != NULL) *file = &props->headers->file;
}

void 
basf_get_bitrate_hdr(basf_props_t props, basf_hdr_bitrate** bitrate)
{
    BDBG_ASSERT(bitrate != NULL);      
    if(bitrate != NULL) *bitrate = &props->headers->bitrate;
}

void 
basf_get_content_hdr(basf_props_t props, basf_hdr_content** content)
{
    BDBG_ASSERT(content != NULL);        
    if(content != NULL) *content = &props->headers->content_descr;
}

void 
basf_install_feed_handler(basf_props_t props, basf_fn_feed_handler handler, void* context)
{
    BDBG_ASSERT(props != NULL);
    if(props != NULL) { 
        props->feed_handler.handler = handler;
        props->feed_handler.context = context;
    }
}


/***************************************************************************
 *     Copyright (c) 2007-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: subtitle_parse.h $
 * $brcm_Revision: 11 $
 * $brcm_Date: 4/22/09 10:19a $
 *
 * Module Description:
 *
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/utils/subtitle/subtitle_parse.h $
 * 
 * 11   4/22/09 10:19a gmohile
 * PR 54480 : Add subtitle transparency
 * 
 * 10   1/15/09 10:22a gmohile
 * PR 51011 : Fix memory leaks
 * 
 * 9   7/22/08 9:48a gmohile
 * PR 43477 : Add end time to clear subtitles
 * 
 * 8   6/10/08 10:56a gmohile
 * PR 43477 : Checking HD support for subtitles
 * 
 * 
 *******************************************************************************/
#include "bavi_parser.h"
#include "bavi_stream.h"
#include "bavi_probe.h"

#define DATA_BUFFERS 25
#define MAX_TRACKS 8

typedef struct bstitle_parse * bstitle_parse_t;
typedef struct bstitle_info * bstitle_info_t;

typedef struct bsubtitle_palette
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} b_subtitle_palette;


typedef struct bsubtitle_hdr
{    
	  /*     Duration format : [HH:MM:SS.XXX-hh:mm:ss.xxx] */    
	  /*     NOTE: This string is not null terminated! */
	  
	  int8_t duration[27];           
	  
	  /*     Dimensions and coordinates */
	  
	  uint16_t width;                
	  uint16_t height;               
	  uint16_t left;                 
	  uint16_t top;                  
	  uint16_t right;                
	  uint16_t bottom;               
	  uint16_t field_offset;         
	  
	  /*     2-bit (4 color) palette */
	  
	  b_subtitle_palette background;   
	  b_subtitle_palette pattern1;      
	  b_subtitle_palette pattern2;    
	  b_subtitle_palette pattern3;
	  
	  uint8_t background_transparency;
	  uint8_t pattern1_transparency;
	  uint8_t pattern2_transparency;
	  uint8_t pattern3_transparency;

	  /*     Rle data */
	  
	  uint8_t *rleData;              
} b_subtitle_hdr;

typedef struct bstitle_cfg_params
{
	  void * cnxt;
	  void (*app_cb)(void * cnxt, int32_t start_time, int32_t end_time);
} b_stitle_cfg_params;

typedef struct bstitle_parse_object_handler {
	  bavi_parser_handler handler; /* must be first member */
	  bstitle_parse_t parse;
} b_stitle_parse_object_handler;


typedef struct track_info{
	  struct {
		    int track_id;
		    bvideo_codec codec;
	  } video_info[MAX_TRACKS];
	  struct {
		    int track_id;
		    baudio_format codec;
	  } audio_info[MAX_TRACKS];
	  int total_audio_tracks;
	  int total_video_tracks; 
} b_track_info;

struct bstitle_parse {
	  unsigned track_number;
	  unsigned track_index;
	  b_stitle_parse_object_handler subtitle;
	  uint32_t width;
	  uint32_t height;
	  uint8_t *raw_data[DATA_BUFFERS];
	  uint8_t rptr;
	  uint8_t wptr;
	  b_subtitle_hdr info;
	  bool activated;
	  b_stitle_cfg_params *cfg_params;
          bavi_parser_t avi_parser;
	  bstitle_info_t stitle_info;
	  bavi_subtitle_type subtitle_type;	 
};

typedef struct bstitle_info{
	  bstitle_parse_t stitle_parse[MAX_TRACKS];
	  int total_tracks;
} b_stitle_info;


#define BSUBTITLE_HDR_SIZE            (0x35)
#define BSUBTITLE_INFO_SIZE           (sizeof(b_subtitle_hdr)) 
#define B_CREATE_PALETTE(a,r,g,b) (a<<24 | r<<16 | g<<8 | b<<0)

bstitle_parse_t bstitle_parse_init(char *file, b_stitle_cfg_params *cfg_params, b_track_info *track_info);
void bstitle_parse_uninit(bstitle_parse_t subtitles);
void bstitle_activate(void *application_cnxt, unsigned stream_id, bavi_parser_t parser, bavi_stream_t stream);
void bstitle_deactivate(void *application_cnxt, unsigned stream_id, bavi_parser_t parser, bavi_stream_t stream);
uint8_t * bstitle_get_buffer(bstitle_parse_t stitle_parse); 
bstitle_parse_t bstitle_switch(bstitle_parse_t stitle_parse, int track_id);
void bstitle_set(bstitle_parse_t stitle_parse);

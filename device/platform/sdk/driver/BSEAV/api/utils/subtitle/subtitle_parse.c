/***************************************************************************
 *     Copyright (c) 2007-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: subtitle_parse.c $
 * $brcm_Revision: 15 $
 * $brcm_Date: 4/22/09 10:19a $
 *
 * Module Description:
 *
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/utils/subtitle/subtitle_parse.c $
 * 
 * 15   4/22/09 10:19a gmohile
 * PR 54480 : Add subtitle transparency
 * 
 * 14   3/11/09 8:58a gmohile
 * PR 49988 : Update to latest media changes
 * 
 * 13   7/26/08 3:50p maivu
 * PR 44679: Added divx subtitle build option for nexus
 * 
 * 12   7/22/08 9:53a gmohile
 * PR 43477 : Add end time to clear subtitles
 * 
 * 11   6/23/08 6:27p mward
 * PR44040: null-terminate strings before sending to atoi.
 * 
 * 10   6/19/08 4:47p mward
 * PR43855: Eliminate compiler warning for DEBUG=n.
 * 
 * 9   6/10/08 10:56a gmohile
 * PR 43477 : Checking HD support for subtitles
 * 
 * 
 *******************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "bmedia_filter.h"


#include "subtitle_parse.h"

#ifdef USE_NEXUS
#include "nexus_base.h"
#define b_atoi(x) NEXUS_atoi(x)
#else
#include "bsettop.h"
#endif


BDBG_MODULE(bsubtitle_parse);

#define read8(pBuffer,offs)       (*(pBuffer + offs))
#define read16(pBuffer,offs)      (*(pBuffer + offs) | (*(pBuffer + offs + 1) << 8))
#define read32(pBuffer,offs)      (*(pBuffer + offs) | (*(pBuffer + offs + 1) << 8) | (*(pBuffer + offs + 2) << 16) | (*(pBuffer + offs + 3) << 24))

bstitle_parse_t g_stitle_parse;

static uint8_t getNibble(uint8_t *pBuffer, int32_t offs)
{
    int32_t b = offs/2;
    int32_t h = (offs%2 == 1);

    uint8_t ret;

    if(h == 0)
    {
        ret = (pBuffer[b] & 0xF0) >> 4;
    }
    else
    {
        ret = (pBuffer[b] & 0x0F) >> 0;
    }

    return ret;
}

static void setNibble(uint8_t *pBuffer, int32_t offs, uint8_t n)
{
    int32_t b = offs/2;
    int32_t h = (offs%2 == 1);

    if(h == 0)
    {
        pBuffer[b] = (pBuffer[b] & 0x0F) | (n << 4);
    }
    else
    {
        pBuffer[b] = (pBuffer[b] & 0xF0) | (n << 0);
    }

    return;
}


void bstitle_get_header_info(uint8_t *buffer, bstitle_parse_t stitle_parse)
{
	BKNI_Memcpy(stitle_parse->info.duration, buffer, 27);

	stitle_parse->info.width = read16(buffer, 0x1B);
	stitle_parse->info.height = read16(buffer, 0x1D);
	stitle_parse->info.left = read16(buffer, 0x1F);
	stitle_parse->info.top = read16(buffer, 0x21);
	stitle_parse->info.right = read16(buffer, 0x23);
	stitle_parse->info.bottom = read16(buffer, 0x25);
	stitle_parse->info.field_offset = read16(buffer, 0x27);

	stitle_parse->info.background.red = read8(buffer, 0x29);
	stitle_parse->info.background.green = read8(buffer, 0x2A);
	stitle_parse->info.background.blue = read8(buffer, 0x2B);

	stitle_parse->info.pattern1.red = read8(buffer, 0x2C);
	stitle_parse->info.pattern1.green = read8(buffer, 0x2D);
	stitle_parse->info.pattern1.blue = read8(buffer, 0x2E);

	stitle_parse->info.pattern2.red = read8(buffer, 0x2F);
	stitle_parse->info.pattern2.green = read8(buffer, 0x30);
	stitle_parse->info.pattern2.blue = read8(buffer, 0x31);

	stitle_parse->info.pattern3.red = read8(buffer, 0x32);
	stitle_parse->info.pattern3.green = read8(buffer, 0x33);
	stitle_parse->info.pattern3.blue = read8(buffer, 0x34);	

	

	if(stitle_parse->subtitle_type == bavi_subtitle_type_dxsa){
		stitle_parse->info.background_transparency = read8(buffer, 0x35);
		stitle_parse->info.pattern1_transparency = read8(buffer, 0x36);
		stitle_parse->info.pattern2_transparency = read8(buffer, 0x37);
		stitle_parse->info.pattern3_transparency = read8(buffer, 0x38);
		stitle_parse->info.rleData = buffer + BSUBTITLE_HDR_SIZE + (4 * sizeof(uint8_t));
	}
	else {
		stitle_parse->info.background_transparency = 0xFF;
		stitle_parse->info.pattern1_transparency = 0xFF;
		stitle_parse->info.pattern2_transparency = 0xFF;
		stitle_parse->info.pattern3_transparency = 0xFF;
		stitle_parse->info.rleData = buffer + BSUBTITLE_HDR_SIZE;
	}

}

int bstitle_parse_rle2raw(bstitle_parse_t stitle_parse, int32_t rleSize)
{
	int32_t width, height, field_offset;
	
	/* encoded top/bottom fields */
	uint8_t *pEncoded[2];

	/* write/read cursors for top/bottom fields */
	int32_t w[2], r[2]={0,0};

	/* read stop offsets */
	int32_t rstop[2];

	uint8_t* rawData = stitle_parse->raw_data[stitle_parse->wptr] + sizeof(stitle_parse->info);

	if(stitle_parse->subtitle_type == bavi_subtitle_type_dxsa){
		rleSize -= (BSUBTITLE_HDR_SIZE + (4 * sizeof(uint8_t)));
	}
	else{
		rleSize -= BSUBTITLE_HDR_SIZE;
	}
	
	/* cache metrics */
	width = stitle_parse->info.width;
	height = stitle_parse->info.height;
	field_offset = stitle_parse->info.field_offset;

	/* read stop offsets */
	rstop[0] = field_offset*2;
	rstop[1] = (rleSize - field_offset)*2;

	/* encode field (top) */
	pEncoded[0] = (uint8_t*)(stitle_parse->info.rleData);

	/* encode field (bottom) */
	pEncoded[1] = (uint8_t*)(stitle_parse->info.rleData + field_offset);

	/* write cursors */
	w[0] = width*(height-1);
	w[1] = width*(height-2);

	/* continuously decode lines until height is reached */
	while(w[0] >= 0 || w[1] >= 0) {
		int32_t v;
		
		/* iterate through both top and bottom fields */
		for(v=0;v<2;v++) {
			int32_t col, len, i;
			
			if(w[v] < 0)
			  continue;
			
			if(r[v] < rstop[v]) {
				/* grab next input nibble */
				int32_t rle = getNibble(pEncoded[v], r[v]++);
				
				if(rle < 0x04) {
					rle = (rle << 4) | getNibble(pEncoded[v], r[v]++);
					
					if(rle < 0x10) {		      
						rle = (rle << 4) | getNibble(pEncoded[v], r[v]++);

						if(rle < 0x040) {                       
							rle = (rle << 4) | getNibble(pEncoded[v], r[v]++);
							
							if(rle < 0x0004)
							  rle |= (width - w[v]%width) << 2;
						}
					}
				}

				col = rle & 0x03;
				len = rle >> 2;
				
				if(len > (width - w[v]%width) || len == 0) {
					len = width - w[v]%width;
				}
			}
			else {			 
				col = 0;
				len = width - w[v]%width;
			}

			for(i=0;i<len;i++) {
				setNibble(rawData, w[v]++, (uint8_t)col);
			}
			
			if(w[v]%width == 0) {           
				w[v] -= width*3;
				
				if(r[v]%2 == 1)
				  r[v]++;
			}
		}
	}
			
	return 0;
}

uint8_t * bstitle_get_buffer(bstitle_parse_t stitle_parse) 
{
	uint8_t * buf = NULL;

	if(stitle_parse->wptr != stitle_parse->rptr){	
		buf = stitle_parse->raw_data[stitle_parse->rptr];
		stitle_parse->rptr++;
		if(stitle_parse->rptr >= DATA_BUFFERS){
			stitle_parse->rptr = 0;
		}
	}
	
	return buf;	
}

int32_t bstitle_get_start_time(bstitle_parse_t stitle_parse)
{
	int32_t hr, min, sec, msec, time;
	char hrs[3], mins[3], secs[3], msecs[4];

	BKNI_Memcpy(hrs, stitle_parse->info.duration+1 ,2);
	BKNI_Memcpy(mins, stitle_parse->info.duration+4 ,2);
	BKNI_Memcpy(secs, stitle_parse->info.duration+7 ,2);
	BKNI_Memcpy(msecs, stitle_parse->info.duration+10 ,3);
	hrs[2] = mins[2] = secs[2] = msecs[3] = '\0';

	hr = b_atoi(hrs);
	min = b_atoi(mins);
	sec = b_atoi(secs);
	msec = b_atoi(msecs);

	time = (((((hr*60) + min)*60) + sec)*1000) + msec;

	return time;
}

int32_t bstitle_get_end_time(bstitle_parse_t stitle_parse)
{
	int32_t hr, min, sec, msec, time;
	char hrs[3], mins[3], secs[3], msecs[4];	

	BKNI_Memcpy(hrs, stitle_parse->info.duration+14 ,2);
	BKNI_Memcpy(mins, stitle_parse->info.duration+17 ,2);
	BKNI_Memcpy(secs, stitle_parse->info.duration+20 ,2);
	BKNI_Memcpy(msecs, stitle_parse->info.duration+23 ,3);
	hrs[2] = mins[2] = secs[2] = msecs[3] = '\0';

	hr = b_atoi(hrs);
	min = b_atoi(mins);
	sec = b_atoi(secs);
	msec = b_atoi(msecs);

	time = (((((hr*60) + min)*60) + sec)*1000) + msec;

	return time;
}

bavi_parser_action bstitle_parser(bavi_parser_handler *handler, bavi_fourcc fourcc, batom_t object)
{
	batom_cursor c;
	bstitle_parse_t stitle_parse = ((b_stitle_parse_object_handler *)handler)->parse;
	size_t length = object?batom_len(object):0;
	uint8_t *buffer;
	int32_t start_time, end_time;

#if BDBG_DEBUG_BUILD
	BDBG_MSG(("subtitle_parse: " BMEDIA_FOURCC_FORMAT "[" BMEDIA_FOURCC_FORMAT "] %u bytes -> 0x%08x", BMEDIA_FOURCC_ARG(handler->fourcc), BMEDIA_FOURCC_ARG(fourcc), length, stitle_parse));
#else
	BSTD_UNUSED(fourcc);
#endif
	batom_cursor_from_atom(&c, object);

	if(c.count>1){
		buffer = BKNI_Malloc(length);				
		batom_cursor_copy(&c, buffer, length);					
	}
	else{			
		buffer = (uint8_t*)c.cursor;		
	}

	bstitle_get_header_info(buffer, stitle_parse);	

	start_time = bstitle_get_start_time(stitle_parse);
	end_time = bstitle_get_end_time(stitle_parse);

	if((end_time-start_time)<500)
		goto done;

	/*Write Header Info to buffer*/
	BKNI_Memcpy(stitle_parse->raw_data[stitle_parse->wptr], &stitle_parse->info, BSUBTITLE_INFO_SIZE);	
	/*Convert RLE to Raw Data and write to buffer*/
	bstitle_parse_rle2raw(stitle_parse, length/* -BSUBTITLE_HDR_SIZE */);	
	stitle_parse->wptr++;
	if(stitle_parse->wptr >= DATA_BUFFERS){
		stitle_parse->wptr = 0;
	}

	BDBG_ASSERT((stitle_parse->wptr != stitle_parse->rptr));       

	if((stitle_parse->cfg_params->app_cb)){
		stitle_parse->cfg_params->app_cb(stitle_parse->cfg_params->cnxt, start_time, end_time);
	}

done :
	if(c.count>1){
		BKNI_Free(buffer);		
	}
	
	if(object) {
		batom_release(object);
	}

	return bavi_parser_action_none;
}

void bstitle_set(bstitle_parse_t stitle_parse)
{
	g_stitle_parse = stitle_parse;
}

bstitle_parse_t bstitle_get(void)
{
	return g_stitle_parse;
}
     
void bstitle_activate(void *application_cnxt, unsigned stream_id, bavi_parser_t parser, bavi_stream_t stream)
{
	bstitle_parse_t stitle_parse = bstitle_get();
	char strm_no[2];
	int i;

	BSTD_UNUSED(stream);
	BSTD_UNUSED(stream_id);
	BSTD_UNUSED(application_cnxt);
	
	if(stitle_parse){
		strm_no[0] = '0' | (((stitle_parse->track_number-1)/10)%10);
		strm_no[1] = '0' | ((stitle_parse->track_number-1)%10);
		
		if(!stitle_parse->activated){
			stitle_parse->subtitle.parse = stitle_parse;
			bavi_parser_install_handler(parser, &stitle_parse->subtitle.handler, BMEDIA_FOURCC(strm_no[0],strm_no[1],'s','b'), bstitle_parser);
			for(i=0; i<DATA_BUFFERS; i++){
				stitle_parse->raw_data[i] = BKNI_Malloc(((stitle_parse->width*stitle_parse->height)/2)+BSUBTITLE_INFO_SIZE);
			}
			stitle_parse->rptr=0;
			stitle_parse->wptr=0;
			stitle_parse->activated = true;	
			stitle_parse->avi_parser = parser;
		}
		
	}
}


void bstitle_deactivate(void *application_cnxt, unsigned stream_id, bavi_parser_t parser, bavi_stream_t stream)
{
	bstitle_parse_t stitle_parse = bstitle_get();
	int i;

	BSTD_UNUSED(stream);
	BSTD_UNUSED(stream_id);     
	BSTD_UNUSED(application_cnxt);

	if(stitle_parse){
		if(stitle_parse->activated){
			bavi_parser_remove_handler(parser, &stitle_parse->subtitle.handler);
			for(i=0; i<DATA_BUFFERS; i++){
				BKNI_Free(stitle_parse->raw_data[i]);
			}
			stitle_parse->rptr=0;
			stitle_parse->wptr=0;
			stitle_parse->activated = false;		
		}
	}
}

bstitle_parse_t
bstitle_switch(bstitle_parse_t stitle_parse, int track_id)
{
	bstitle_info_t info = stitle_parse->stitle_info;
	bavi_parser_t avi_parser = stitle_parse->avi_parser;
	bstitle_parse_t parse = NULL;

	if(!bstitle_get())
		goto error;

	bstitle_deactivate(&stitle_parse, 0, avi_parser, NULL);
	if(track_id<1 || track_id> info->total_tracks){
		parse = stitle_parse;
	}else{
		parse = info->stitle_parse[track_id-1];
		bstitle_set(parse);
		bstitle_activate(&parse, 0, avi_parser, NULL);
	}
	
error:
	return parse;
}

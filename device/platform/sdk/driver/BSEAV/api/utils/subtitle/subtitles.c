/***************************************************************************
 *     Copyright (c) 2007-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: subtitles.c $
 * $brcm_Revision: 15 $
 * $brcm_Date: 7/22/08 9:50a $
 *
 * Module Description:
 *
 * Subtitle app
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/utils/subtitle/subtitles.c $
 * 
 * 15   7/22/08 9:50a gmohile
 * PR 43477 : Add end time to clear subtitles
 * 
 * 14   6/10/08 5:22p gmohile
 * PR 43447 : Cancel all timers when subtitles are cleared
 * 
 * 13   6/10/08 10:56a gmohile
 * PR 43477 : Checking HD support for subtitles
 * 
 * 
 *******************************************************************************/

#include "bsettop.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include "subtitle_parse.h"

#include "bsettop_os.h"

#include "blst_list.h"

#ifdef B_HAS_DIVX_DRM
#include "bdrm_decrypt.h"
#endif


typedef struct b_timer_info {
	  BLST_D_ENTRY(b_timer_info) link;
	  b_timer_t timer;
	  bstitle_parse_t stitle;
} b_timer_info;

typedef struct b_timer_list {
	  BLST_D_HEAD(node, b_timer_info) list;
} b_timer_list;


uint32_t color_key;

bgraphics_t graphics;
bsurface_t fb, surface;
bdecode_t decode;
bstitle_parse_t subtitles;
b_track_info track_info;

b_timer_list timer_list;

#define HD_WIDTH 1920
#define HD_HEIGHT 1080
#define SD_WIDTH 720
#define SD_HEIGHT 480

static struct {
	int trickModeRate;
	int fast;
} state = {0,0};


static void display_subtitle(void *cntx);
static void destroy_subtitle(void *cntx);


void set_subtitle(void * cnxt, int32_t start_time, int32_t end_time)
{	
	long diff;
	bdecode_status decode_status;
	b_timer_info * start_timer_info = NULL;
	b_timer_info * end_timer_info = NULL;

	start_timer_info = BKNI_Malloc(sizeof(b_timer_info));
	end_timer_info = BKNI_Malloc(sizeof(b_timer_info));       

	BSTD_UNUSED(cnxt);

	start_timer_info->stitle = subtitles;
	end_timer_info->stitle = subtitles;

	b_unlock();
	bdecode_get_status(decode, &decode_status);
	diff = decode_status.video_pts/45;
	start_timer_info->timer = b_timer_schedule((start_time-diff), display_subtitle, start_timer_info);
	end_timer_info->timer = b_timer_schedule((end_time-diff-50), destroy_subtitle, end_timer_info);
	b_lock();	

	BLST_D_INSERT_HEAD(&timer_list.list, start_timer_info, link);
	BLST_D_INSERT_HEAD(&timer_list.list, end_timer_info, link);
}

void render_subtitle(bgraphics_t graphics, bsurface_t fb, uint8_t * buffer)
{
	uint32_t palette[4];
	unsigned int x, y, height, width;
	unsigned int *ptr;
	unsigned int i,j;
	uint8_t val, *buf, *raw_data; 
	b_subtitle_hdr info;
	bsurface_settings surface_settings;
	bsurface_memory mem;
	bgraphics_settings gs;

	memcpy(&info, buffer, BSUBTITLE_INFO_SIZE);
	raw_data = buffer + BSUBTITLE_INFO_SIZE;	

	bsurface_get(surface, &surface_settings);       
	bsurface_get_memory(surface, &mem);
		
	bsurface_fill(surface, &surface_settings.position, 0);

	palette[0] = B_CREATE_PALETTE(info.background.red, info.background.green,info.background.blue);
	palette[1] = B_CREATE_PALETTE(info.pattern.red, info.pattern.green, info.pattern.blue);
	palette[2] = B_CREATE_PALETTE(info.emphasis1.red, info.emphasis1.green, info.emphasis1.blue);
	palette[3] = B_CREATE_PALETTE(info.emphasis2.red, info.emphasis2.green, info.emphasis2.blue);	       
	
	x = (surface_settings.position.width - info.width)/2;
	y =  info.top;
	height = info.height;
	width = info.width;

	bgraphics_get(graphics, &gs);
	gs.chromakey_enabled = /* true */false;
	gs.upper_chromakey = palette[0];
	gs.lower_chromakey = palette[0];
	bgraphics_set(graphics, &gs); 

	buf= raw_data + ((width/2)*(height-1));
	ptr = (unsigned int*)mem.buffer;
	ptr += (y * mem.pitch) / 4;
	for(i=0; i<height; i++){	
		for(j=x; j<(width+x); j++){	
			if(j&0x1){
				val = (uint8_t)(*buf & 0x0F) >> 0;
				buf++;
			}
			else{
				val = (uint8_t)(*buf & 0xF0) >> 4;
			}				
			ptr[j] = (val==0)?val:palette[val];		
		}
		ptr += mem.pitch / 4;
		buf -= width;
	}	
	
	bsurface_copy(fb, NULL, surface, NULL);
}

void clear_subtitle(bsurface_t fb)
{
	bsurface_settings surface_settings;
	b_timer_info * timer_info;

	bsurface_get(fb, &surface_settings);
	bsurface_fill(fb, &surface_settings.position, 0);
	bsurface_sync(fb);	

	while( NULL != (timer_info=BLST_D_FIRST(&timer_list.list))){
		b_timer_cancel(timer_info->timer);
		BLST_D_REMOVE(&timer_list.list, timer_info, link);
		BKNI_Free(timer_info);
	}
}


static void display_subtitle(void *cntx)
{
	b_timer_info * timer_info = (b_timer_info *)cntx;
	bstitle_parse_t subtitles = timer_info->stitle;
	uint8_t *buffer=NULL;

	buffer = bstitle_get_buffer(subtitles);
	if(buffer){
	  render_subtitle(graphics, fb, buffer);
	}

	BLST_D_REMOVE(&timer_list.list, timer_info, link);
	BKNI_Free(timer_info);
}

static void destroy_subtitle(void *cntx)
{
	bsurface_settings surface_settings;
	b_timer_info * timer_info = (b_timer_info *)cntx;

	bsurface_get(fb, &surface_settings); 
	bsurface_fill(fb, &surface_settings.position, 0);
	bsurface_sync(fb);

	BLST_D_REMOVE(&timer_list.list, timer_info, link);
	BKNI_Free(timer_info);
}


static void
printMenu(void)
{
	printf(
	"Subtitle App Menu:\n"
	"?       Print this menu\n"
	"st      NUM  Set Subtitle track number to display\n"
	"at      NUM  Set Audio track number to display\n"
	"ff      Fast Forward\n"
	"rew     Rewind\n"
	"play    Play\n"
        "q       Quit\n");	
}

static int
trickMode(bplayback_t pb, int fast, int dir)
{
	int rc;

	/* reset */
	if (fast != state.fast) {
		state.trickModeRate = 0;
		state.fast = fast;
	}

	/* hop over 1 */
	if (state.trickModeRate == 0 && dir > 0)
		state.trickModeRate++;
	else if (state.trickModeRate == 2 && dir < 0)
		state.trickModeRate--;
	state.trickModeRate += dir;

	if (state.trickModeRate == 0) {
		if(subtitles){
			subtitles = bstitle_switch((bstitle_parse_t)subtitles, subtitles->track_index);
		}
		rc = bplayback_play(pb);
	}
	else {
		int rate;
		bplayback_trickmode_params params;			

		if(subtitles){
			bstitle_switch((bstitle_parse_t)subtitles, 0);
			clear_subtitle(fb);
		}

		if (state.fast)
			rate = BPLAYBACK_NORMALPLAY_SPEED * state.trickModeRate;
		else 
			rate = BPLAYBACK_NORMALPLAY_SPEED / state.trickModeRate;
			
		rc = bplayback_get_trickmode_by_rate(pb, rate, &params);
		if (!rc)
			rc = bplayback_trickmode(pb, &params);		
	}
	return rc;
}

static int
processCmd(bplayback_t pb, const char *s) {
	const char *end = s+strcspn(s, " \t");
	const char *param = NULL;
	if (end)
		param = end+strspn(end, " \t");

#define CMD(STR) (!strncmp(s, STR, end-s))
	printf("processCmd '%s'\n", s);
	if (CMD("?") || CMD("help")) {
		printMenu();
	}	
	else if (CMD("q")) {
		return 1;
	}       
	else if (CMD("st")) {		
		int track;
		char *cmd;
		bdecode_status status;

		cmd = strtok((char *)param, " \t");
	
		if (cmd) {
			track = atoi(cmd);
			if(track<=subtitles->stitle_info->total_tracks){
				printf("Dispaying Subtitle Track %d\n", track);
				clear_subtitle(fb);
				subtitles = bstitle_switch((bstitle_parse_t)subtitles, track);
				bdecode_get_status(decode, &status);
				bplayback_goto_timestamp(pb, status.video_pts/45, SEEK_SET);
			}
			else {
				printf("Max Subtitle Tracks Found is : %d\n", subtitles->stitle_info->total_tracks);
				printf("Please Enter a Track number between 1 - %d\n", subtitles->stitle_info->total_tracks);
			}			
		}		
	}
	else if (CMD("at")) {		
		int track;
		char *cmd;	

		cmd = strtok((char *)param, " \t");
	
		if (cmd) {
			track = atoi(cmd);
			if(track<=track_info.total_audio_tracks){
				printf("Switching to Audio Track %d\n", track);				
				bdecode_set_audio_program(decode, track-1);				
			}
			else {
				printf("Max Audio Tracks Found is : %d\n", track_info.total_audio_tracks);
				printf("Please Enter a Track number between 1 - %d\n", track_info.total_audio_tracks);
			}
		}		
	}
	else if (CMD("rew")) {
		trickMode(pb, 1, -1);
	}
	else if (CMD("ff")) {
		trickMode(pb, 1, 1);
	}
	else if (CMD("play")) {
		if(subtitles){		
			subtitles = bstitle_switch((bstitle_parse_t)subtitles, subtitles->track_index);
		}
		bplayback_play(pb);
		state.trickModeRate = 0;
	}
#ifdef USE_READLINE
	else if (CMD("h")) {
		HIST_ENTRY **list;
		register int i;

		list = history_list ();
		if (list)
		{
			for (i = 0; list[i]; i++)
			fprintf (stderr, "%d: %s\r\n", i, list[i]->line);
		}
	}
#endif
	else {
		printf("Unknown command: %s\n", s);
	}
	return 0;
}

static int
processCmdString(bplayback_t pb, char *s) {
	char *cmd;
	char *end = s+strlen(s);
	cmd = strtok(s, ";");
	while (cmd) {
		/* strtok will be reused again inside processCmd,
		and it means it will change strtok global state.
		This is a workaround. */
		char *testend = cmd + strlen(cmd);
		cmd += strspn(cmd, " \t");
		if (processCmd(pb, cmd))
			return 1;
		if (testend == end)
			break;
		cmd = strtok(testend+1, ";");
	}
	return 0;
}


static void
DVRMenu(bplayback_t pb)
{
	char s[256];

	printMenu();
 	while (1)
	{
		char *cmd;

#ifdef USE_READLINE
		cmd = readline(">");
		if (cmd) {
			strcpy(s, cmd);
			if (*s)
				add_history(s);
			free(cmd);
		}
#else
		printf(">");
		cmd = fgets(s,sizeof(s)-1,stdin);
		if (cmd==NULL) {
			break;
		}
		cmd = strchr(s, '\n');
		if (cmd) {
			*cmd = '\0';
		}
#endif

		if (processCmdString(pb, s))
			break;
	}
}

static bplayback_loopmode handleloop(void *context)
{
	BSTD_UNUSED(context);

	return bplayback_loopmode_pause;
}


int main(int argc, const char *argv[]) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	bdisplay_t display;
	bsurface_create_settings create_settings;	
	bdecode_window_t window;
	bstream_t stream;
	bstream_mpeg mpeg;
	bplayback_file_t file=NULL;
	bplayback_t playback=NULL;
	bplaypump_t playpump=NULL;
	bplayback_params playback_params;
	b_stitle_cfg_params cfg_params;	
#ifdef B_HAS_DIVX_DRM
	bdrm_decrypt_t decrypt;
#endif
	uint32_t trk_cnt; 
	bdisplay_settings disp_cfg;

	if (rc) goto error;

	if(argc<2){
		printf("PLEASE SPECIFY AN INPUT FILE NAME\n");
		return 1;
	}
	  

	cfg_params.cnxt = NULL;
	cfg_params.app_cb = set_subtitle;

	subtitles = bstitle_parse_init((char *)argv[1], &cfg_params, &track_info);
	BLST_D_INIT(&timer_list.list);
	
	display = bdisplay_open(B_ID(0));	
	bdisplay_get(display, &disp_cfg);
	disp_cfg.format = bvideo_format_1080i;
	disp_cfg.dvi = boutput_hdmi_open(B_ID(0));
	disp_cfg.component = boutput_component_open(B_ID(0));
	disp_cfg.composite = NULL;
	disp_cfg.svideo = NULL;
	disp_cfg.rf = NULL;
	bdisplay_set(display, &disp_cfg);
	
	graphics = bgraphics_open(B_ID(0),display);
	decode = bdecode_open(B_ID(0));
	window = bdecode_window_open(B_ID(0), display);

	bsurface_create_settings_init(&create_settings, graphics);
	create_settings.pixel_format = bgraphics_pixel_format_a8_r8_g8_b8;	
	fb = bgraphics_create_framebuffer(graphics, &create_settings, false);

	bsurface_create_settings_init(&create_settings, graphics);
	create_settings.pixel_format = bgraphics_pixel_format_a8_r8_g8_b8;
#if 0
	if(subtitles->height <= 480){
		create_settings.width = SD_WIDTH;
		create_settings.height = SD_HEIGHT;
	}
	else{
		create_settings.width = HD_WIDTH;
		create_settings.height = HD_HEIGHT;
	}
#endif	
	create_settings.width = subtitles->width;
	create_settings.height = subtitles->height;

	surface = bsurface_create(graphics, &create_settings);		
	
	file = bplayback_file_open(argv[1], argv[1]);
	playback = bplayback_open();


	/* get a decode going so we can see alpha in the fb */
	bstream_mpeg_init(&mpeg);
	
	for(trk_cnt=0; trk_cnt<(uint32_t)track_info.total_video_tracks; trk_cnt++){
		mpeg.video[trk_cnt].pid = track_info.video_info[trk_cnt].track_id;
		mpeg.video[trk_cnt].format = track_info.video_info[trk_cnt].codec;
	}

	for(trk_cnt=0; trk_cnt<(uint32_t)track_info.total_audio_tracks; trk_cnt++){
		mpeg.audio[trk_cnt].pid = track_info.audio_info[trk_cnt].track_id;
		mpeg.audio[trk_cnt].format = track_info.audio_info[trk_cnt].codec;
	}

	mpeg.mpeg_type = bstream_mpeg_type_avi;

#ifdef B_HAS_DIVX_DRM
	decrypt = bdrm_decrypt_init(argv[1]);
	memcpy(&mpeg.encryption.key, &decrypt, sizeof(decrypt));
#endif

	playpump = bplaypump_open(B_ID(0), NULL);


	bplayback_params_init(&playback_params, playback);
	playback_params.index_format =bindex_format_none;
	playback_params.callback_context = playback;
	playback_params.end_of_stream = handleloop;
	playback_params.beginning_of_stream = handleloop;
	
	printf("STARTING PLAYBACK\n");

	stream = bplayback_start(playback, playpump, &mpeg, file, &playback_params);
	if (!stream) goto error;	
	if (bdecode_start(decode, stream, window))
		goto error;

	DVRMenu(playback);
	
	/* shutdown */
	bdecode_stop(decode);
	bplayback_stop(playback);
	bplayback_file_close(file);
	bdecode_close(decode);
	bgraphics_close(graphics);
	bdecode_window_close(window);
	bdisplay_close(display);
	bsettop_uninit();
	return 0;
error:
	return 1;
}

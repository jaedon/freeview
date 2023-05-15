/***************************************************************************
 *     Copyright (c) 2007-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: subtitle_init.c $
 * $brcm_Revision: 7 $
 * $brcm_Date: 1/15/09 10:22a $
 *
 * Module Description:
 *
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/utils/subtitle/subtitle_init.c $
 * 
 * 7   1/15/09 10:22a gmohile
 * PR 51011 : Fix memory leaks
 * 
 * 6   1/8/09 1:56a erickson
 * PR43477: survive bad filename
 *
 * 5   6/10/08 10:56a gmohile
 * PR 43477 : Checking HD support for subtitles
 *
 *
 *******************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "bmedia_filter.h"
#include "bmedia_probe.h"
#include "bfile_stdio.h"

#include "subtitle_parse.h"

BDBG_MODULE(bsubtitle_init);

b_stitle_info info;

bstitle_parse_t
bstitle_parse_init(char *file, b_stitle_cfg_params *cfg_params, b_track_info *track_info)
{
	bmedia_probe_t probe;
	FILE *fin;
	bfile_io_read_t fd;
	const bmedia_probe_stream *stream;
	const bmedia_probe_track *track;
	bmedia_probe_config probe_config;
	char stream_info[512];
	int track_no=0;
	int audio_trk_num, video_trk_num;
	int user_input;

	probe = bmedia_probe_create();

	fin = fopen(file,"rb");
	if (!fin) return NULL;

	fd = bfile_stdio_read_attach(fin);

	bmedia_probe_default_cfg(&probe_config);
	probe_config.file_name = file;
	probe_config.type = bstream_mpeg_type_avi;
	stream = bmedia_probe_parse(probe, fd, &probe_config);
	BKNI_Memset(track_info, 0, sizeof(*track_info));
	audio_trk_num = video_trk_num = 0;
	if(stream) {
		bmedia_stream_to_string(stream, stream_info, sizeof(stream_info));
		BDBG_WRN(("%s: %s", file, stream_info));
		for(track=BLST_SQ_FIRST(&stream->tracks);track;track=BLST_SQ_NEXT(track, link)) {
			if(track->type==bmedia_track_type_video) {
			    BDBG_WRN(("video track %u codec:%u width %d height %d", track->number, track->info.video.codec, track->info.video.width, track->info.video.height));
			}

			switch(track->type) {
			case bmedia_track_type_video:
				if(track->info.video.codec == bvideo_codec_unknown){

					info.stitle_parse[track_no] = BKNI_Malloc(sizeof(struct bstitle_parse)); /*Free up this memory on exit*/

					if (!info.stitle_parse[track_no]) {
						BDBG_WRN(("bstitle_parse_init: out of memory %u", sizeof(struct bstitle_parse)));
						goto error;
					}
					info.stitle_parse[track_no]->track_number = track->number;
					info.stitle_parse[track_no]->track_index = track_no+1;
					info.stitle_parse[track_no]->width = track->info.video.width;
					info.stitle_parse[track_no]->height = track->info.video.height;
					info.stitle_parse[track_no]->activated = false;
					info.stitle_parse[track_no]->cfg_params = cfg_params;
					info.stitle_parse[track_no]->stitle_info = &info;
					info.stitle_parse[track_no]->subtitle_type = ((bavi_probe_track *)track)->subtitle_type;
					track_no++;
				}
				else {
					track_info->video_info[video_trk_num].track_id = track->number;
					track_info->video_info[video_trk_num].codec = track->info.video.codec;
					video_trk_num++;
				}
				break;
			case bmedia_track_type_audio:
				track_info->audio_info[audio_trk_num].track_id = track->number;
				track_info->audio_info[audio_trk_num].codec = track->info.audio.codec;
				audio_trk_num++;
			default:
				break;
			}
		}
		info.total_tracks = track_no;
		track_info->total_audio_tracks = audio_trk_num;
		track_info->total_video_tracks = video_trk_num;
error:
		bmedia_probe_stream_free(probe, stream);
	}

	bfile_stdio_read_detach(fd);
	fclose(fin);
	bmedia_probe_destroy(probe);

	printf("/***************************************************************/\n");
	printf("FOUND %d SUBTITLE TRACKS\n", track_no);
	printf("PLEASE ENTER SELECTION 1 - %d TO SELECT SUBTITLE TRACK TO DISPLAY\n", track_no);
	scanf("%d", &user_input);
	printf("DISPLAYING SUBTITLE TRACK %d\n", user_input);
	printf("/***************************************************************/\n");

	if(user_input<1 || user_input > track_no){
		/* return NULL; */
		bstitle_set(NULL);
		return info.stitle_parse[0];
	}
	else{
		bstitle_set(info.stitle_parse[user_input-1]);
		return info.stitle_parse[user_input-1];
	}

}

void
bstitle_parse_uninit(bstitle_parse_t subtitles)
{
	unsigned i;
	bstitle_info_t subtitles_info = subtitles->stitle_info;       
	
	for(i=0; i<MAX_TRACKS; i++){
		if(subtitles_info->stitle_parse[i]){
			BKNI_Free(subtitles_info->stitle_parse[i]);
		}
	}
	  
}



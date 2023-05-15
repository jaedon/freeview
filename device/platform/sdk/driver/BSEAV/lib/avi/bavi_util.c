/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bavi_util.c $
 * $brcm_Revision: 35 $
 * $brcm_Date: 1/20/12 1:37p $
 *
 * Module Description:
 *
 * AVI parser library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/avi/bavi_util.c $
 * 
 * 35   1/20/12 1:37p vsilyaev
 * SW7425-2178: Relaxed parsing of streamheader
 * 
 * 34   4/4/11 12:27p vsilyaev
 * SWBLURAY-25181: Use common way to detect AC3 audio type
 * 
 * 33   4/4/11 12:17p vsilyaev
 * SWBLURAY-25181: Better handle AC3 audio streams
 * 
 * fix_bdvd_v3.0_sec_mdal_SWBLURAY-25181/1   4/4/11 11:39a ialauder
 * Default AC3 streams to maybe VBR.  If the length of a block is greater
 * than the nBlockAlign then the stream has to be CBR.
 * 
 * 32   9/20/10 6:08p vsilyaev
 * SWBLURAY-22489: Resovled overwriting of vbr_scale
 * 
 * fix_bdvd_v3.0_SWBLURAY-22489/1   9/16/10 5:44p ialauder
 * Fix calculation of VBR scale
 * 
 * 31   6/11/09 1:26p vsilyaev
 * PR 55912: Added guesswork (based on the nBlockAlign) for detecting
 * duration of LSF VBR audio packets
 * 
 * 30   3/6/09 11:34a vsilyaev
 * PR 52903: Added the 'break;' statement after BDBG_ASSERT(0)
 * 
 * 29   3/6/09 10:19a vsilyaev
 * PR 46190: Fixed adjustment for CBR scale
 * 
 * 28   3/2/09 3:05p vsilyaev
 * PR 52434: Post merge updates/unification of parsing video data
 * 
 * 27   3/2/09 10:26a vsilyaev
 * PR 52434: Merge XViD and VC1 AP support in MKV
 * 
 * 25   2/10/09 12:18p vsilyaev
 * PR 46190: Fixed timesamp calculations for PCM audio
 * 
 * 24   1/21/09 12:57p vsilyaev
 * PR 51103: Earlier detect CBR AVI/MP3 streams
 * 
 * 23   1/14/09 10:01a vsilyaev
 * PR 50813: Merged fix for WMA audio
 * 
 * dev_bdvd_v1.42_send_only_first_wma_timestamp_in_avi/1   1/5/09 10:58a gooslin
 * Force WMA audio to CBR
 * 
 * 22   12/18/08 10:42a vsilyaev
 * PR 46190: Adding support for PCM audio
 * 
 * 21   12/8/08 1:44p vsilyaev
 * PR 49718: Print MaxBytesPerSec
 * 
 * 20   10/15/08 11:27p vsilyaev
 * PR 47927: Return duration for AVI tracks
 * 
 * 19   10/1/08 12:25p vsilyaev
 * PR 47334: PR 10960_DVD: Properly handle audio bitrate detection, when
 * starting in the default state
 * 
 * 18   10/1/08 12:01p vsilyaev
 * PR 47334: Fix corner cases in detection CBR/VBR audio
 * 
 * fix_bdvd_v1.4_pr10960/1   10/1/08 2:28p zjwang
 * fixing AC3 audio AV sync lose by setting audio->max_framelen as wave-
 * >nBlockAlign;
 * fixing a CBR audio track incorrectly identified as VBR by referring to
 * "nBlockAlign" field, if it is 1, transitting audio_type from maybe_vbr
 * to cbr directly
 * 
 * 17   9/25/08 2:18p vsilyaev
 * PR 47334: Use calculated max audio frame size to select between VBR and
 * CBR audio
 * 
 * 16   9/25/08 12:53p vsilyaev
 * PR 47334: Reworked WaveFormat parsing
 * 
 * 15   9/25/08 12:34p vsilyaev
 * PR 47334: Added code to handle files with missing cbSize fiiled in the
 * WaveHeader.
 * 
 * fix_bdvd_v1.4_pr10889/1   9/25/08 2:52p mfalco
 * add support waveformat structures which are missing the cbSize field
 * 
 * 14   9/2/08 4:23p vsilyaev
 * PR 46412: Added support for probing OpenDML AVI 2.0 files
 * 
 * 13   8/25/08 3:03p vsilyaev
 * PR 46119: Improved audio timestamp algorithm to handle streams with
 * CBR->VBR transition
 * 
 * 12   8/19/08 5:35p vsilyaev
 * PR 45748: Fixed assert for large custom waveheader data
 * 
 * 11   8/8/08 3:08p vsilyaev
 * PR 45314: Fixed warning
 * 
 * 10   8/8/08 11:58a vsilyaev
 * PR 45314: Merged fix for WMA audio support in AVI files
 * 
 * 9   8/6/08 6:49p vsilyaev
 * PR 45200: PR 45191: Fixed threshold in CBR/VBR detection
 * 
 * 8   7/31/08 1:10p vsilyaev
 * PR 45314: Merged WMA audio support in AVI files
 * 
 * dev_bdvd_pr9870/2   7/31/08 9:26a gooslin
 * Cleanup WMA audio changes
 * 
 * dev_bdvd_pr9870/1   7/29/08 4:22p gooslin
 * Add support for WMA streams in AVI files
 * 
 * 7   7/28/08 2:05p vsilyaev
 * PR 45191: Classify 8x and large increase in size of audio block as an
 * VBR audio
 * 
 * 6   5/16/08 10:42a vsilyaev
 * PR 42379: Unified timestamp calculation between avi_player and
 * avi_stream. Added heuristics in timestamp calculation to pass quirks
 * in certain streams.
 * 
 * 5   2/12/07 6:14p vsilyaev
 * PR 25701: Fixed timestamps for audio
 * 
 * 4   2/9/07 1:10p vsilyaev
 * PR 25701: Added stream and format specific hanlder
 * 
 * 3   2/8/07 11:47p vsilyaev
 * PR 25701: bavi_stream parser
 * 
 * 2   2/6/07 6:48p vsilyaev
 * PR 25701: Modified AVI parser to work with batom framework
 * 
 * 1   12/18/06 7:23p vsilyaev
 * PR 25701: AVI parser
 * 
 *******************************************************************************/

#include "bstd.h"
#include "bavi_util.h"
#include "bkni.h"

BDBG_MODULE(bavi_util);

#define B_AVI_FLAG(flag,prfx,bit) (((flag)&(prfx##_##bit))? #bit " ":"")
bool
bavi_read_streamheader(bavi_streamheader *sh, batom_t header)
{
	batom_cursor c;
	batom_cursor_from_atom(&c,header);

	/*
	sh->fcc = bavi_read_fourcc(&c);
	sh->cb = bavi_read_dword(&c);
	*/
	sh->fccType = bavi_read_fourcc(&c);
	sh->fccHandler = bavi_read_fourcc(&c);
	sh->dwFlags = bavi_read_dword(&c);
	sh->wPriority = bavi_read_word(&c);
	sh->wLanguage = bavi_read_word(&c);
	sh->dwInitialFrames = bavi_read_dword(&c);
	sh->dwScale = bavi_read_dword(&c);
	sh->dwRate = bavi_read_dword(&c);
	sh->dwStart = bavi_read_dword(&c);
	sh->dwLength = bavi_read_dword(&c);
	sh->dwSuggestedBufferSize = bavi_read_dword(&c);
	sh->dwQuality = bavi_read_dword(&c);
	sh->dwSampleSize = bavi_read_dword(&c);
    sh->rcFrame.left = 0;
    sh->rcFrame.top = 0;
    sh->rcFrame.right = 0;
    sh->rcFrame.bottom = 0;
    /* treat rcFrame as optional element */
    if(batom_cursor_reserve(&c,4*2)>=4*2) {
        sh->rcFrame.left = bavi_read_short_int(&c);
        sh->rcFrame.top = bavi_read_short_int(&c);
        sh->rcFrame.right = bavi_read_short_int(&c);
        sh->rcFrame.bottom = bavi_read_short_int(&c);
    }
	BDBG_MSG(("bavi_read_streamheader: " BMEDIA_FOURCC_FORMAT " %s InitialFrames:%u Scale:%u Rate:%u SampleSize:%u Start:%u Length:%u", BMEDIA_FOURCC_ARG(sh->fccType), B_AVI_FLAG(sh->dwFlags,BAVI_SF, DISABLED), (unsigned)sh->dwInitialFrames, (unsigned)sh->dwScale, (unsigned)sh->dwRate, (unsigned)sh->dwSampleSize, (unsigned)sh->dwStart, (unsigned)sh->dwLength));
	return !BATOM_IS_EOF(&c);
}

bool
bavi_read_mainheader(bavi_mainheader *mh, batom_t header)
{
	batom_cursor c;
	unsigned i;
	batom_cursor_from_atom(&c,header);

	mh->dwMicroSecPerFrame = bavi_read_dword(&c);
	mh->dwMaxBytesPerSec = bavi_read_dword(&c);
	mh->dwPaddingGranularity = bavi_read_dword(&c);
	mh->dwFlags = bavi_read_dword(&c);
	mh->dwTotalFrames = bavi_read_dword(&c);
	mh->dwInitialFrames = bavi_read_dword(&c);
	mh->dwStreams = bavi_read_dword(&c);
	mh->dwSuggestedBufferSize = bavi_read_dword(&c);
	mh->dwWidth = bavi_read_dword(&c);
	mh->dwHeight = bavi_read_dword(&c);
	for(i=0;i<sizeof(mh->dwReserved)/sizeof(*mh->dwReserved);i++) {
		mh->dwReserved[i] = bavi_read_dword(&c);
	}
	BDBG_MSG(("bavi_read_mainheader: %s%s%s MaxBytesPerSec:%u MicroSecPerFrame:%u Streams:%u TotalFrames:%u InitialFrames:%u %ux%u", B_AVI_FLAG(mh->dwFlags,BAVI_F,HASINDEX), B_AVI_FLAG(mh->dwFlags, BAVI_F, MUSTUSEINDEX), B_AVI_FLAG(mh->dwFlags, BAVI_F, ISINTERLEAVED), (unsigned)mh->dwMaxBytesPerSec, (unsigned)mh->dwMicroSecPerFrame, (unsigned)mh->dwStreams, (unsigned)mh->dwTotalFrames, (unsigned)mh->dwInitialFrames, (unsigned)mh->dwWidth, (unsigned)mh->dwHeight));
	return !BATOM_IS_EOF(&c);
}

bool bavi_read_dmlh(bavi_dmlh *dmlh, batom_t header)
{
	batom_cursor c;
	batom_cursor_from_atom(&c,header);

	dmlh->dwTotalFrames = bavi_read_dword(&c);
	BDBG_MSG(("bavi_read_dmlh: TotalFrames:%u", (unsigned)dmlh->dwTotalFrames));

	return !BATOM_IS_EOF(&c);
}

void 
bavi_audio_state_init(bavi_audio_state *audio)
{
    BDBG_ASSERT(audio);
    audio->min_size = 0;
    audio->max_size = 0;
    audio->vbr_scale = 1;
    audio->cbr_scale = 1;
    audio->block_count = 0;
	audio->data_size = 0;
	audio->skew = 0;
    audio->header.dwStart = 0;
    audio->header.nSamplesPerSec = 0;
    audio->header.dwRate = 0;
    audio->wFormatTag = 0;
    return;
}

#define B_AVI_AUDIO_TYPE_STR(type) (type==bavi_audio_stream_type_cbr)?"CBR":(type==bavi_audio_stream_type_vbr?"VBR":(type==bavi_audio_stream_type_maybe_vbr?"MAYBE VBR":"MAYBE CBR"))

void 
bavi_audio_state_set_header(bavi_audio_state *audio, const bavi_streamheader *header, const bmedia_waveformatex *wave)
{
    BDBG_ASSERT(audio);
    BDBG_ASSERT(header);
    BDBG_ASSERT(wave);
    audio->header.dwStart = header->dwStart;
    audio->header.dwRate = header->dwRate;
    audio->header.nSamplesPerSec = wave->nSamplesPerSec;
	audio->header.nBlockAlign = wave->nBlockAlign;
    audio->block_count = 0;
	audio->data_size = 0;
	audio->skew = 0;
    audio->wFormatTag = wave->wFormatTag;
	audio->max_framelen = wave->nBlockAlign; /* initialize audio max framlen as wave->nBlockAlign  */
	if(wave->nSamplesPerSec) {
		switch(wave->wFormatTag) {
		case 0x0050: /* MPEG1 L1/L2 */ audio->max_framelen = (144000*448)/wave->nSamplesPerSec; break; /* 448 KBps max audio rate for MPEG1 L1 audio */
		case 0x0055: /* MPEG1 L3 */ audio->max_framelen = (144000*320)/wave->nSamplesPerSec; break; /* 320 KBps max audio rate for MPEG1 L3 audio */
		default: break;
		}
	}

    /* selecting length(duration) of audio VBR frame */
    if(   wave->nSamplesPerSec>=32000 /* if sample rate is 33,44.1 and 48Khz each audio sample corresponds to 1152 samples (e.g. single MPEG Layer2/3 frame) */
       || wave->nBlockAlign == 1152) {  /* or if nBlockAlign is 1152 */
          audio->vbr_scale = 1152*1000;
    } else {
          audio->vbr_scale = 576*1000; /* for low sampling rates  each audio sample corresponds to half of 1152 samples (e.g. half of MPEG Layer2/3 frame) */
    }

    if(BMEDIA_WAVFMTEX_AUDIO_PCM(wave)) {
        /* for PCM audio, time is always derived from number of played samples, and coefficients to convert samples to time are taken from the wave header */
        audio->header.dwRate = wave->nChannels * wave->wBitsPerSample * wave->nSamplesPerSec; /* rate in bits per second */
	    audio->cbr_scale = 8; /* to convert from bytes to bits */
	    audio->type = bavi_audio_stream_type_cbr;
    } else if(BMEDIA_WAVFMTEX_AUDIO_WMA(wave) || BMEDIA_WAVFMTEX_AUDIO_WMA_PRO(wave)) {
	    audio->type = bavi_audio_stream_type_cbr;
    } else if(BMEDIA_WAVFMTEX_AUDIO_AC3(wave)) {
        audio->type = bavi_audio_stream_type_maybe_vbr;        
        audio->vbr_scale = 256 * 6 * 1000; /* AC3 Frame Size */
    } else {
	    if( !(wave->nBlockAlign==576 || wave->nBlockAlign==1152)) { /* not valid nBlockAlign for VBR audio */
			audio->type = bavi_audio_stream_type_cbr;
		}  else {
	        audio->type = bavi_audio_stream_type_maybe_vbr;
        }
        if(audio->header.dwRate>=(1000000/8)) { /* if audio stream bitrate exceeds 1Mbps */
            audio->cbr_scale = header->dwScale;
        }
    }
 BDBG_MSG(("bavi_audio_state_set_header:%#lx nBlockAlign:%u:%u VBR scale:%u CBR scale:%u", (unsigned long)audio,  (unsigned)wave->nBlockAlign, (unsigned)audio->max_framelen, (unsigned)audio->vbr_scale, (unsigned)audio->cbr_scale));
    return;
}


void 
bavi_audio_state_update(bavi_audio_state *audio, size_t block_len)
{
    BDBG_ASSERT(audio);
	if(audio->type==bavi_audio_stream_type_vbr || audio->type==bavi_audio_stream_type_cbr) {
		goto done;
	}

    /* If the stream is AC3 and the size of the block is greater than the nBlockAlign,
        then the file must be CBR */
    if(BMEDIA_WAVFMTEX_AUDIO_AC3(audio)) {
        if (audio->type == bavi_audio_stream_type_maybe_vbr &&
            block_len > audio->header.nBlockAlign) {
                audio->type=bavi_audio_stream_type_cbr;
        }
        goto done;
    }

    if(audio->block_count>0) {
        if(block_len < audio->min_size) {
            audio->min_size = block_len;
        } else if(block_len > audio->max_size) {
            audio->max_size = block_len;
        }
    } else {
        audio->min_size = block_len;
        audio->max_size = block_len;
    }
	if(audio->type==bavi_audio_stream_type_maybe_vbr) {

		if( !(audio->header.nBlockAlign==576 || audio->header.nBlockAlign==1152)) { /* not valid nBlockAlign for VBR audio */
			audio->type = bavi_audio_stream_type_cbr;
			goto done;
		} 
		if(block_len > audio->max_framelen) { /* if audio block size exceeds max size of audio frame, it means there is a multiple frames of audio in the single block, which means that audio is CBR */
			if( audio->min_size!=0 && 
				 ((block_len % audio->min_size)!=0 || /* have found block that is not integer multiplier of smallest block */
				  (block_len/audio->min_size)>=8) /* have found block that is 8 times larger then a smallest block */
				 ) {
				audio->type = bavi_audio_stream_type_vbr; 
			} else {
			    audio->type = bavi_audio_stream_type_maybe_cbr;
            }
            goto done;
		}
		if(audio->block_count>100) {
			audio->type = bavi_audio_stream_type_vbr;
		}
	} else if(audio->type==bavi_audio_stream_type_maybe_cbr) {
		if(block_len < audio->header.nBlockAlign) {
			bmedia_player_pos timestamp_cbr, timestamp_vbr;
			timestamp_cbr = bavi_audio_state_get_timestamp(audio, audio->block_count, audio->data_size);
			audio->type = bavi_audio_stream_type_maybe_vbr;
			timestamp_vbr = bavi_audio_state_get_timestamp(audio, audio->block_count, audio->data_size);
			audio->skew += timestamp_cbr - timestamp_vbr;
    		BDBG_MSG(("bavi_audio_state_update:%#lx CBR:%u -> VBR:%u skew:%d", (unsigned long)audio, timestamp_cbr, timestamp_vbr, audio->skew));
			goto done;
		}
		if(audio->block_count>100) {
			audio->type = bavi_audio_stream_type_cbr;
		}
	}
done:
	audio->data_size += block_len;
    audio->block_count++;
    BDBG_MSG(("bavi_audio_state_update:%#lx[%s] block_len:%u(%u:%u) block_count:%u", (unsigned long)audio, B_AVI_AUDIO_TYPE_STR(audio->type), (unsigned)block_len, (unsigned)audio->min_size, (unsigned)audio->max_size, (unsigned)audio->block_count));
    return;
}

bmedia_player_pos
bavi_audio_state_get_timestamp(const bavi_audio_state *audio, unsigned block_count, unsigned data_size)
{
    bmedia_player_pos timestamp;

    BDBG_ASSERT(audio);
    if(audio->header.nSamplesPerSec==0 || audio->header.dwRate==0) {
        BDBG_WRN(("bavi_audio_state_get_timestamp:%#lx invalid audio header", (unsigned long)audio));
        return 0;
    }

    switch(audio->type) {
    case bavi_audio_stream_type_maybe_vbr:
    case bavi_audio_stream_type_vbr:
		timestamp = ((audio->vbr_scale*(uint64_t)(block_count + audio->header.dwStart))/audio->header.nSamplesPerSec);
        break;
    case bavi_audio_stream_type_maybe_cbr:
    case bavi_audio_stream_type_cbr:
		timestamp = ((1000*audio->cbr_scale)*(uint64_t)(audio->header.dwStart+data_size)/audio->header.dwRate);
        break;
    default:
        BDBG_ASSERT(0);
        timestamp = 0;
        break;
    }
	timestamp += audio->skew;
    BDBG_MSG(("bavi_audio_state_get_timestamp:%#lx[%s] timestamp:%u block_count:%u data_size:%u", (unsigned long)audio, B_AVI_AUDIO_TYPE_STR(audio->type), (unsigned)timestamp, block_count, data_size));
    return timestamp;
}

void 
bavi_audio_state_get_memo(const bavi_audio_state *audio, bavi_audio_state_memo *memo)
{
	memo->type = audio->type;
	memo->skew = audio->skew;
	return;
}

void 
bavi_audio_state_clear_memo(bavi_audio_state_memo *memo)
{
	memo->type = bavi_audio_stream_type_maybe_vbr;
	memo->skew = 0;
	return;
}

void 
bavi_audio_state_set_memo(bavi_audio_state *audio, const bavi_audio_state_memo *memo, unsigned block_count, unsigned data_size)
{
	audio->type = memo->type;
	audio->skew = memo->skew;
	audio->block_count = block_count;
	audio->data_size = data_size;
	return;
}


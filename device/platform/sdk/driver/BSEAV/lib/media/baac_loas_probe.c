/***************************************************************************
 *     Copyright (c) 2007 Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: baac_loas_probe.c $
 * $brcm_Revision: 11 $
 * $brcm_Date: 12/29/10 3:48p $
 *
 * Module Description:
 *
 * BMedia library, Probe of AAC audio with LOAS encapsulation
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/baac_loas_probe.c $
 * 
 * 11   12/29/10 3:48p vsilyaev
 * SW7405-4532: Added case for 'PS (Parametric Stereo)' object type
 * 
 * 10   8/27/10 12:26p vsilyaev
 * SW7405-4819: Validate EOS condition prior to using value
 * 
 * 9   8/5/10 1:17p vsilyaev
 * SW7405-4681: Parse all programs and layers
 *
 * 8   8/5/10 11:54a vsilyaev
 * SW7405-4681: Fixed implementation of LatmGetValue
 *
 * 7   7/30/10 12:43p vsilyaev
 * SW7405-4681: Parse all programs in the LOAS mux element
 * 
 * 6   6/24/10 7:28p vsilyaev
 * SW7405-4532: Updated feeder loop
 * 
 * 5   5/11/10 1:31p vsilyaev
 * SW7125-285: Fixed warning
 * 
 * 4   4/23/10 5:59p vsilyaev
 * SW7125-253: Fixed warning '... break strict-aliasing rules
 * 
 * 3   4/8/10 6:55p vsilyaev
 * SW7405-3773: Fixed probing of [bad] AAC LOAS streams
 * 
 * 2   2/3/10 12:12p vsilyaev
 * SWBLURAY-18771: Added AAC profile for MP4 and MKV containers
 * 
 * int_bdvd_v3.0_issue18771/2   2/3/10 11:05a ialauder
 * Added comments regarding the AAC documentation
 * 
 * int_bdvd_v3.0_issue18771/1   2/3/10 10:17a ialauder
 * Store the AAC profile
 * 
 * 1   9/29/09 2:43p vsilyaev
 * SWDEPRECATED-3624: AAC LOAS probe
 * 
 * 
 * 
 *******************************************************************************/
#include "bstd.h"
#include "baac_loas_probe.h"
#include "bkni.h"
#include "biobits.h"
#include "baac_probe.h"
BDBG_MODULE(baac_loas_probe);

#define BDBG_MSG_TRACE(x)   /* BDBG_MSG(x) */

#define B_AAC_LOAS_N_FRAMES	2
#define B_AAC_LOAS_N_MAX_FRAMES	40
#define B_AAC_LOAS_SYNC	0x2B7
#define B_AAC_LOAS_SYNC_LEN	2

/* Parsing based on ISO-IEAC 14496 3rd Ed Subpart 01 - Coding of audio-visual objects - Part 3 Audio. Subpart 1: Main */
typedef struct bmedia_probe_audio_aac_loas *baac_loas_probe_t;
BDBG_OBJECT_ID(baac_loas_probe_t);

struct bmedia_probe_audio_aac_loas {
	BDBG_OBJECT(baac_loas_probe_t)
	batom_accum_t acc;
};

bmedia_probe_base_es_t 
b_aac_loas_probe_create(batom_factory_t factory)
{
	baac_loas_probe_t aac_loas;
	BDBG_ASSERT(factory);

	aac_loas = BKNI_Malloc(sizeof(*aac_loas));
	if(!aac_loas) {
		goto err_alloc;
	}
	BDBG_OBJECT_INIT(aac_loas, baac_loas_probe_t);
	aac_loas->acc = batom_accum_create(factory);
	if(!aac_loas->acc) {
		goto err_acc;
	}
	return (bmedia_probe_base_es_t)aac_loas;
err_acc:
	BKNI_Free(aac_loas);
err_alloc:
	return NULL;
}

void 
b_aac_loas_probe_destroy(bmedia_probe_base_es_t probe)
{
	baac_loas_probe_t aac_loas = (baac_loas_probe_t)probe;
	BDBG_OBJECT_ASSERT(aac_loas, baac_loas_probe_t);
	batom_accum_destroy(aac_loas->acc);
	BDBG_OBJECT_DESTROY(aac_loas, baac_loas_probe_t);
	BKNI_Free(aac_loas);
	return;
}

void 
b_aac_loas_probe_reset(bmedia_probe_base_es_t probe)
{
	baac_loas_probe_t aac_loas = (baac_loas_probe_t)probe;
	BDBG_OBJECT_ASSERT(aac_loas, baac_loas_probe_t);
	batom_accum_clear(aac_loas->acc);
	return;
}

static uint32_t
b_aac_loas_LatmGetValue(batom_bitstream *bs)
{
    unsigned bytes;
    unsigned i;
    uint32_t value;

    /* Table 1.30 . Syntax of LatmGetValue() */
    bytes = batom_bitstream_bits(bs, 2);
    if(batom_bitstream_eof(bs)) {
        return 0;
    }
    for(value=0,i=0;i<=bytes;i++) {
       value *= 256;
       value += batom_bitstream_bits(bs, 8);
    }
    return value;
}

/* Table 1.16 . Sampling Frequency Index */
static const unsigned b_aac_loas_sample_rate[]={
	96000,
	88200,
	64000,
	48000,
	44100,
	32000,
	24000,
	22050,
	16000,
	12000,
	11025,
	8000,
    7350
};

static const uint8_t b_aac_loas_channels[]={
	0,
	1,
	2,
	3,
	4,
	5,
	5+1,
	7+1
};

static  unsigned
b_aac_load_probe_parse_GetAudioObjectType(batom_bitstream *bs)
{
    unsigned audioObjectType;
    /* Table 1.14 . Syntax of GetAudioObjectType() */
    audioObjectType = batom_bitstream_bits(bs, 5);
    if(audioObjectType==31) {
        audioObjectType = 32 + batom_bitstream_bits(bs, 6);
    }
    return audioObjectType;
}

static unsigned
b_aac_load_probe_parse_SamplingRate(batom_bitstream *bs)
{
    unsigned sample_rate=0;
    unsigned samplingFrequencyIndex;

    samplingFrequencyIndex = batom_bitstream_bits(bs, 4);
    if(batom_bitstream_eof(bs)) {
        return 0;
    }
    if(samplingFrequencyIndex==0xf) {
        sample_rate = batom_bitstream_bits(bs, 24);
    } else {
        if(samplingFrequencyIndex<sizeof(b_aac_loas_sample_rate)/sizeof(*b_aac_loas_sample_rate)) {
            sample_rate = b_aac_loas_sample_rate[samplingFrequencyIndex];
        }
    }
    return sample_rate;
}


static unsigned
b_aac_loas_probe_parse_AudioSpecificConfig(batom_bitstream *bs, bmedia_probe_audio *info)
{
    unsigned audioObjectType;
    unsigned channelConfiguration;
    bmedia_probe_aac_audio codec_specific;
    unsigned sample_rate;

    /* Table 1.13 . Syntax of AudioSpecificConfig() */
    audioObjectType = b_aac_load_probe_parse_GetAudioObjectType(bs);
    sample_rate = b_aac_load_probe_parse_SamplingRate(bs);
    BDBG_MSG_TRACE(("audioObjectType:%u sample_rate:%u", audioObjectType, sample_rate));
    if(sample_rate) {
        info->sample_rate = sample_rate;
    }
    channelConfiguration = batom_bitstream_bits(bs, 4);
    BDBG_MSG_TRACE(("channelConfiguration:%u", channelConfiguration));
    if(batom_bitstream_eof(bs)) {
        return 0;
    }
    info->channel_count = b_aac_loas_channels[channelConfiguration];
    BDBG_MSG_TRACE(("channel_count:%u", info->channel_count));

    if(audioObjectType==5) { /* SBR */
        sample_rate = b_aac_load_probe_parse_SamplingRate(bs);
        if(sample_rate) {
            info->sample_rate = sample_rate;
        }
        audioObjectType = b_aac_load_probe_parse_GetAudioObjectType(bs);
    }


    info->sample_size = 16;
    /* Table 1.15 . Audio Object Types */
    switch(audioObjectType) {
    case 1: /* AAC Main */
    case 2: /* AAC LC (Low Complexity) */
    case 3: /* AAC SSR (Scalable Sample Rate) */
    case 4: /* AAC LTP (Long Term Prediction) */
    case 5: /* SBR (Spectral Band Replication) */
    case 6: /* AAC Scalable */
    case 29: /* PS (Parametric Stereo) */
    /*  This configuration is based on the MPEG-4 audio object types, ISO/IEC 14496-3 {ed 3.0}, page 20.  
        If a new eumeration is added please ensure it is kept in order. */
        codec_specific.profile = (bmedia_probe_aac_profile) audioObjectType;
        info->codec = baudio_format_aac_plus_loas;
        break;
    default:
        info->codec = baudio_format_unknown;
        break;
    }
    BKNI_Memcpy(&info->codec_specific, &codec_specific, sizeof(codec_specific));
    BDBG_MSG_TRACE(("audioObjectType:%u done", audioObjectType));
    return audioObjectType;
}

static bool
b_aac_loas_probe_parse_AudioMuxElement(batom_cursor *cursor, bmedia_probe_audio *info)
{
    batom_bitstream bs;
    unsigned audioMuxVersionA=0;
    unsigned audioMuxVersion;
    bool valid = false;
    bool useSameStreamMux;

    batom_bitstream_init(&bs, cursor);
    /* Table 1.28 . Syntax of AudioMuxElement()  */
    useSameStreamMux = batom_bitstream_bit(&bs);
    if(batom_bitstream_eof(&bs)) {
        return false;
    }
    BDBG_MSG_TRACE(("useSameStreamMux:%u", useSameStreamMux));
    if(useSameStreamMux) { /* useSameStreamMux */
        return false;
    }
    /* Table 1.29 . Syntax of StreamMuxConfig() */
    audioMuxVersion = batom_bitstream_bits(&bs, 1);
    if(batom_bitstream_eof(&bs)) {
        return false;
    }
    if(audioMuxVersion==1) { /* audioMuxVersion  */
        audioMuxVersionA = batom_bitstream_bits(&bs, 1);
    }
    if(batom_bitstream_eof(&bs)) {
        return false;
    }
    BDBG_MSG_TRACE(("audioMuxVersion:%u audioMuxVersionA:%u", audioMuxVersion, audioMuxVersionA));
    if(audioMuxVersionA==0) {
        bool allStreamsSameTimeFraming;
        unsigned numSubFrames;
        unsigned numProgram;
        unsigned prog;

        if(audioMuxVersion) {
            unsigned taraBufferFullness = b_aac_loas_LatmGetValue(&bs);
            BSTD_UNUSED(taraBufferFullness);
            BDBG_MSG_TRACE(("taraBufferFullness:%u", taraBufferFullness));
        }
        allStreamsSameTimeFraming = batom_bitstream_bit(&bs);
        numSubFrames = batom_bitstream_bits(&bs, 6);
        numProgram = batom_bitstream_bits(&bs, 4);
        BDBG_MSG_TRACE(("allStreamsSameTimeFraming:%u numSubFrames:%u numProgram:%u", allStreamsSameTimeFraming, numSubFrames, numProgram));
        for(prog = 0; prog <= numProgram; prog++) {
            unsigned numLayer;
            unsigned lay;
            unsigned prevAudioObjectType = 0;

            numLayer = batom_bitstream_bits(&bs, 3);
            BDBG_MSG_TRACE(("numLayer:%u", numLayer));
            for(lay = 0; lay <= numLayer; lay++)  {
                bool useSameConfig;
                unsigned audioObjectType=0;
                unsigned frameLengthType;

                BDBG_MSG(("layer %u:%u prog %u:%u", lay, numLayer, prog, numProgram));
                if(prog==0 && lay==0) {
                    useSameConfig = false;
                } else {
                    useSameConfig = batom_bitstream_bit(&bs);
                }
                if(!useSameConfig) {
                    bmedia_probe_audio temp_info=*info;
                    unsigned ascLen=0;
                    unsigned audioSpecificConfigStart;

                    if(audioMuxVersion) {
                        ascLen =  b_aac_loas_LatmGetValue(&bs);
                    } 
                    audioSpecificConfigStart = batom_bitstream_bit_position(&bs);
                    BDBG_MSG_TRACE(("ascLen:%u audioSpecificConfigStart:%u", ascLen, audioSpecificConfigStart));
                    audioObjectType = b_aac_loas_probe_parse_AudioSpecificConfig(&bs, &temp_info);
                    if(batom_bitstream_eof(&bs)) {
                        return false;
                    }
                    BDBG_MSG_TRACE(("codec:%u", temp_info.codec));
                    if(temp_info.codec!=baudio_format_unknown) {
                        BDBG_MSG_TRACE(("info valid: %u,%u,%u", temp_info.codec, temp_info.sample_rate, temp_info.channel_count));
                        valid = true;
                        *info = temp_info;
                    }
                    if(ascLen) {
                        unsigned audioSpecificConfigFinish = batom_bitstream_bit_position(&bs);
                        unsigned audioSpecificConfigLen;

                        BDBG_ASSERT(audioSpecificConfigFinish>= audioSpecificConfigStart);
                        audioSpecificConfigLen = audioSpecificConfigFinish-audioSpecificConfigStart;
                        BDBG_MSG_TRACE(("ascLen:%u audioSpecificConfigLen:%u", ascLen, audioSpecificConfigLen));
                        if(ascLen>audioSpecificConfigLen) {
                            batom_bitstream_drop_bits(&bs, ascLen-audioSpecificConfigLen);
                        } else {
                            return false;
                        }
                    } else {
                        /* we don't parse entire  audioSpecificConfig, so parsing should stop there */
                        return valid;
                    }
                    if(batom_bitstream_eof(&bs)) {
                        return false;
                    }
                }
                frameLengthType = batom_bitstream_bits(&bs, 3);
                if(batom_bitstream_eof(&bs)) {
                    return false;
                }
                if(frameLengthType==0) {
                    unsigned latmBufferFullness = batom_bitstream_bits(&bs, 8);
                    BSTD_UNUSED(latmBufferFullness);
                    if(!allStreamsSameTimeFraming) {
                        if( (audioObjectType==6 || audioObjectType==20) &&
                            (prevAudioObjectType==8 || audioObjectType==24)) {
                            unsigned coreFrameOffset = batom_bitstream_bits(&bs, 6);
                            BSTD_UNUSED(coreFrameOffset);
                        }
                    }
                } else if(frameLengthType==1) {
                    unsigned frameLength = batom_bitstream_bits(&bs, 9);
                    BSTD_UNUSED(frameLength);
                } else if(frameLengthType==4||frameLengthType==5||frameLengthType==3) {
                    unsigned celpFrameLengthTableIndex = batom_bitstream_bits(&bs, 6);
                    BSTD_UNUSED(celpFrameLengthTableIndex);
                } else if(frameLengthType==6 || frameLengthType==7) {
                    unsigned hvxcFrameLengthTableIndex = batom_bitstream_bits(&bs, 1);
                    BSTD_UNUSED(hvxcFrameLengthTableIndex);
                }
                if(batom_bitstream_eof(&bs)) {
                    return false;
                }
                prevAudioObjectType = audioObjectType;
            }
        }
    } 
    return valid;
}

static size_t
b_aac_loas_probe_parse_header(batom_cursor *cursor, bmedia_probe_audio *info, uint32_t sync_word)
{
    int extra_byte;
    size_t len;
    batom_cursor AudioMuxElement;

	BDBG_MSG(("b_aac_loas_probe_parse_header: sync_word:%#x left:%u", sync_word, batom_cursor_size(cursor)));

    extra_byte = batom_cursor_next(cursor);
    if(BATOM_EOF==extra_byte) {
        return 0;
    }
    sync_word =  (sync_word << 8) | extra_byte;
    len = B_GET_BITS(sync_word, 12, 0);
    BATOM_CLONE(&AudioMuxElement, cursor);
    if(b_aac_loas_probe_parse_AudioMuxElement(&AudioMuxElement, info)) {
        info->bitrate = (8*(len+B_AAC_LOAS_SYNC_LEN+1)* info->sample_rate) / (1024 /* AAC samples per frame */ * 1000 /* KBps */);
    }
    return len;
}


bmedia_probe_track *
b_aac_loas_probe_feed(bmedia_probe_base_es_t probe, batom_t atom, bool *done)
{
	baac_loas_probe_t aac_loas = (baac_loas_probe_t)probe;
	batom_cursor cursor;
	batom_accum_t acc;
    bmedia_probe_audio info;
    batom_cursor clear_point;
    bool clear_point_valid;
	BSTD_UNUSED(done);


	BDBG_OBJECT_ASSERT(aac_loas, baac_loas_probe_t);
	acc = aac_loas->acc;
	batom_accum_add_atom(acc, atom);
    BKNI_Memset(&info, 0, sizeof(info)); /* this is not needed, used to fix warning-bug in GCC compiler */
	for(;;) {
		size_t byte_cnt=0;
		unsigned frame_cnt;
		bmedia_probe_track *track;
		uint32_t temp = 0;

        BKNI_Memset(&info, 0, sizeof(info));
        info.codec = baudio_format_unknown;
        clear_point_valid = false;
		batom_cursor_from_accum(&cursor, acc);
		for(;;) {
			int byte;
			byte = batom_cursor_next(&cursor);
			if(byte==BATOM_EOF) {
				goto done;
			}
			byte_cnt++;
			temp = ((temp&0xFF)<<8)|byte;
			if(B_GET_BITS(temp,15, 5)==B_AAC_LOAS_SYNC) { /* only supported AudioSyncStream, EPAudioSyncStream is not supported */
				break;
			}
		}

		/* found sync word, now look for back-to-back frames */
		for(frame_cnt=0;;frame_cnt++) {
            bmedia_probe_audio probe_info;
            size_t frame_len;
			BDBG_MSG(("b_aac_loas_probe_feed: %#lx sync_word %#x at:%u frame_cnt:%u", (unsigned long)aac_loas, temp, batom_cursor_pos(&cursor)-B_AAC_LOAS_SYNC_LEN, frame_cnt));
            
            if(!clear_point_valid) {
                size_t pos = batom_cursor_pos(&cursor);
                batom_cursor_from_accum(&clear_point, acc);
                if(pos>=B_AAC_LOAS_SYNC_LEN) {
                    batom_cursor_skip(&clear_point, byte_cnt-B_AAC_LOAS_SYNC_LEN);
                }
            }

            BKNI_Memset(&probe_info, 0, sizeof(probe_info)); /* this is not needed, used to fix warning-bug in GCC compiler */
			frame_len = b_aac_loas_probe_parse_header(&cursor, &probe_info, temp);
			if(BATOM_IS_EOF(&cursor)) {
				goto done; /* wait for more data */
			} else if(frame_len>0) {
				BDBG_MSG(("b_aac_loas_probe_feed: %#lx header %u at:%u size:%u samplerate:%u channels:%u codec:%u", (unsigned long)aac_loas, frame_cnt, batom_cursor_pos(&cursor), frame_len, probe_info.sample_rate, probe_info.channel_count, probe_info.codec));

                if(probe_info.sample_size>0) {
                    if(!clear_point_valid) {
                        clear_point_valid = true;
                    }
                    info = probe_info;
                }
                if(frame_cnt>=B_AAC_LOAS_N_MAX_FRAMES || (frame_cnt>=B_AAC_LOAS_N_FRAMES && info.sample_size!=0 && info.codec != baudio_format_unknown)) {
                    break;
                }
				batom_cursor_skip(&cursor, frame_len);
				temp = batom_cursor_uint16_be(&cursor); /* read next syncword */
				if(BATOM_IS_EOF(&cursor)) {
					goto done; /* wait for more data */
				}
				if(B_GET_BITS(temp, 15,5)==B_AAC_LOAS_SYNC) {
					continue;
				}
				BDBG_MSG(("b_aac_loas_probe_feed: %#lx out of sync at %u %#x-%#x(%#x)", (unsigned long)aac_loas, batom_cursor_pos(&cursor)-B_AAC_LOAS_SYNC_LEN, temp, B_GET_BITS(temp,15,5), B_AAC_LOAS_SYNC));
			} else {
				BDBG_MSG(("b_aac_loas_probe_feed: %#lx invalid frame at %u", (unsigned long)aac_loas, batom_cursor_pos(&cursor)-B_AAC_LOAS_SYNC_LEN));
            }
			byte_cnt+=B_AAC_LOAS_SYNC_LEN;
            clear_point_valid = false;
            goto done;
		}
		/* have found enough back-to-back frames */
		batom_accum_clear(acc);
		/* found all frames */
		track = BKNI_Malloc(sizeof(*track));
		if(track) {
			bmedia_probe_track_init(track);
			track->type = bmedia_track_type_audio;
			track->info.audio = info;
		}
		return track;
done:
        if(clear_point_valid) {
            byte_cnt = batom_cursor_pos(&clear_point);
            BDBG_MSG(("b_aac_loas_probe_feed: %#lx trim to frame %u", (unsigned long)aac_loas, byte_cnt));
            byte_cnt += B_AAC_LOAS_SYNC_LEN;
        } 
        BDBG_MSG(("b_aac_loas_probe_feed: %#lx trim %u", (unsigned long)aac_loas, byte_cnt));
        if(byte_cnt<=B_AAC_LOAS_SYNC_LEN) {
            break;
        }
        batom_cursor_from_accum(&cursor, acc);
        batom_cursor_skip(&cursor, byte_cnt-B_AAC_LOAS_SYNC_LEN);
        batom_accum_trim(acc, &cursor);
	}
	return NULL;
}

static const bmedia_probe_file_ext b_aac_loas_ext[] =  {
	{"aac"}, 
	{""}
};

const bmedia_probe_es_desc baac_loas_probe = {
	bmedia_track_type_audio,
	{
		baudio_format_mpeg
	},
	b_aac_loas_ext,
	(20*2*B_AAC_LOAS_N_FRAMES)*8192,
	b_aac_loas_probe_create,
	b_aac_loas_probe_destroy,
	b_aac_loas_probe_feed,
	b_aac_loas_probe_reset,
	bmedia_probe_es_nolast
};
	

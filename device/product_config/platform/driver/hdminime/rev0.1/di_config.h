#ifndef __DI_CONFIG_H__
#define __DI_CONFIG_H__

// for inputband
#define NUM_DEMOD			1
#define NUM_CI				0


// for parserband
#define NUM_LIVE			1
#define NUM_PIP				0
#define NUM_TSR				0
#define NUM_REC				0
#define NUM_TRANSCODING		0
#define NUM_REMUX			0

// for pluypump
#define NUM_PLAYBACK					1
#define NUM_THUMNAIL_DECODE				1
#define NUM_AUDIO_PLAYBACK				1
#define NUM_PLAYPUMP_TRANSCODING		(NUM_TRANSCODING*4)

#define NUM_VIDEO_DECODER				(NUM_LIVE + NUM_PIP)
#define SUPPORT_VIDEO_CODEC_SVC			0

#if defined(CONFIG_MAX_PLAYBACK_INSTANCE)
#if CONFIG_MAX_PLAYBACK_INSTANCE != NUM_PLAYBACK
#error NUM_PLAYBACK
#endif
#endif


#if defined(CONFIG_MAX_RECORD_INSTANCE)
#if CONFIG_MAX_RECORD_INSTANCE != (NUM_TSR+NUM_REC)
//#error CONFIG_MAX_RECORD_INSTANCE
#endif
#endif

#if defined(CONFIG_MAX_TRANSCODER_CHANNEL)
#if CONFIG_MAX_TRANSCODER_CHANNEL != NUM_TRANSCODING
#error NUM_PLAYPUMP_TRANSCODING
#endif
#endif


#define CONFIG_NUM_INPUT_BANDS				(NUM_DEMOD+NUM_CI)
#define CONFIG_NUM_PARSER_BANDS				(NUM_LIVE+NUM_PIP+NUM_TSR+NUM_REC+NUM_TRANSCODING+NUM_REMUX)
#define CONFIG_NUM_PLAYPUMPS				(NUM_PLAYBACK+NUM_THUMNAIL_DECODE+NUM_AUDIO_PLAYBACK+NUM_PLAYPUMP_TRANSCODING)


// to check product configration
#if CONFIG_NUM_INPUT_BANDS > NEXUS_NUM_INPUT_BANDS
#error NEXUS_NUM_INPUT_BANDS
#endif
#if CONFIG_NUM_PARSER_BANDS > NEXUS_NUM_PARSER_BANDS
#error NEXUS_NUM_PLAYPUMPS
#endif
#if CONFIG_NUM_PLAYPUMPS > NEXUS_NUM_PLAYPUMPS
#error NEXUS_NUM_PLAYPUMPS
#endif


#endif

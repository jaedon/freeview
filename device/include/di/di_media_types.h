#ifndef	__DI_MEDIA_TYPES_H__
#define	__DI_MEDIA_TYPES_H__

typedef enum
{
	eDI_MEDIA_Speed_RW_X256 	= -17,
	eDI_MEDIA_Speed_RW_X128		= -16,
	eDI_MEDIA_Speed_RW_X100		= -15,
	eDI_MEDIA_Speed_RW_X64		= -14, 
	eDI_MEDIA_Speed_RW_X32		= -13,
	eDI_MEDIA_Speed_RW_X30		= -12,
	eDI_MEDIA_Speed_RW_X16		= -11,
	eDI_MEDIA_Speed_RW_X10		= -10,
	eDI_MEDIA_Speed_RW_X8		= -9,
	eDI_MEDIA_Speed_RW_X4		= -8,
	eDI_MEDIA_Speed_RW_X2		= -7, 
	eDI_MEDIA_Speed_RW_X12_10	= -6,	
	eDI_MEDIA_Speed_RW_X1		= -5,
	eDI_MEDIA_Speed_RW_X_1_2	= -4, 
	eDI_MEDIA_Speed_RW_X_1_4	= -3,
	eDI_MEDIA_Speed_RW_X_1_8	= -2, 
	eDI_MEDIA_Speed_RW_X_1_10	= -1, 
	eDI_MEDIA_Speed_Pause		= 0,
	eDI_MEDIA_Speed_FF_X_1_10	= 1,
	eDI_MEDIA_Speed_FF_X_1_8	= 2,
	eDI_MEDIA_Speed_FF_X_1_4	= 3,
	eDI_MEDIA_Speed_FF_X_1_2	= 4, 
	eDI_MEDIA_Speed_Normal		= 5,
	eDI_MEDIA_Speed_FF_X12_10	= 6,	
	eDI_MEDIA_Speed_FF_X2		= 7,	
	eDI_MEDIA_Speed_FF_X4		= 8,
	eDI_MEDIA_Speed_FF_X8		= 9,
	eDI_MEDIA_Speed_FF_X10		= 10,
	eDI_MEDIA_Speed_FF_X16		= 11,
	eDI_MEDIA_Speed_FF_X30		= 12,
	eDI_MEDIA_Speed_FF_X32		= 13,
	eDI_MEDIA_Speed_FF_X64		= 14,
	eDI_MEDIA_Speed_FF_X100		= 15,
	eDI_MEDIA_Speed_FF_X128		= 16,
	eDI_MEDIA_Speed_FF_X256		= 17,
	eDI_MEDIA_Speed_None	
}DI_MEDIA_PlaySpeed_e;

/**
 * Define media Video Format
 */
typedef enum
{
	DI_MEDIA_VIDEO_FORMAT_NONE			= 0,
	DI_MEDIA_VIDEO_FORMAT_MPEG1,	/**< MPEG1 Video */
	DI_MEDIA_VIDEO_FORMAT_MPEG2,	/**< MPEG2 Video */
	DI_MEDIA_VIDEO_FORMAT_MPEG4Part2,	/**< MPEG4 Part2 Video */
	DI_MEDIA_VIDEO_FORMAT_H261,	/**< H.261 Video */
	DI_MEDIA_VIDEO_FORMAT_H263,	/**< H.263 Video */
	DI_MEDIA_VIDEO_FORMAT_H264,	/**< H.264 Video */
	DI_MEDIA_VIDEO_FORMAT_VC1SimpleMain,	/**< VC1 Simple Main Video */
	DI_MEDIA_VIDEO_FORMAT_VC1,	/**< VC1 Video */
	DI_MEDIA_VIDEO_FORMAT_AVS,	/**< AVS Video */
	DI_MEDIA_VIDEO_FORMAT_DIVX_311
}DI_MEDIA_VIDEO_FORMAT_e;

/**
 * Define media Audio Format
 */
typedef enum
{
	DI_MEDIA_AUDIO_FORMAT_NONE = 0,
	DI_MEDIA_AUDIO_FORMAT_MPEG,
	DI_MEDIA_AUDIO_FORMAT_MP3,
	DI_MEDIA_AUDIO_FORMAT_AAC,
	DI_MEDIA_AUDIO_FORMAT_AAC_ADTS = DI_MEDIA_AUDIO_FORMAT_AAC,
	DI_MEDIA_AUDIO_FORMAT_AAC_LOAS,
	DI_MEDIA_AUDIO_FORMAT_AAC_PLUS,
	DI_MEDIA_AUDIO_FORMAT_AAC_PLUS_LOAS,
	DI_MEDIA_AUDIO_FORMAT_AAC_PLUS_ADTS,	/* AAC plus SBR. aka MPEG-4 High Efficiency (AAC-HE), with LOAS (Low Overhead Audio Stream) sync and LATM mux */
	DI_MEDIA_AUDIO_FORMAT_AC3,
	DI_MEDIA_AUDIO_FORMAT_AC3_PLUS,
	DI_MEDIA_AUDIO_FORMAT_PCM,
    DI_MEDIA_AUDIO_FORMAT_DTS,            /* Digital Digital Surround sound. */
    DI_MEDIA_AUDIO_FORMAT_LPCM_HD_DVD,      /* LPCM, HD-DVD mode */
    DI_MEDIA_AUDIO_FORMAT_LPCM_BLUERAY,     /* LPCM, Blu-Ray mode */
    DI_MEDIA_AUDIO_FORMAT_DTS_HD,          /* Digital Digital Surround sound, HD */
    DI_MEDIA_AUDIO_FORMAT_WMA_STD,         /* WMA Standard */
    DI_MEDIA_AUDIO_FORMAT_WMA_PRO,         /* WMA Professional */
    DI_MEDIA_AUDIO_FORMAT_FLAC,         /* FLAC */
    DI_MEDIA_AUDIO_FORMAT_LPCM_DVD,        /* LPCM, DVD mode */
    DI_MEDIA_AUDIO_FORMAT_PCMWAVE,				/* PCM audio with Wave header - Used with streams containing PCM audio */
    DI_MEDIA_AUDIO_FORMAT_ADPCM,			/* ADPCM */
	DI_MEDIA_AUDIO_FORMAT_VORBIS,			/* OGG Vorbis */
	DI_MEDIA_AUDIO_FORMAT_APE,				/* Monkey's Audio */
	DI_MEDIA_AUDIO_FORMAT_OPUS,
    DI_MEDIA_AUDIO_FORMAT_DRA
}DI_MEDIA_AUDIO_FORMAT_e;

/**
 * Define Audio Decoder Type
 */
typedef enum {
	DI_MEDIA_AUDIO_DECODER_NONE,
	DI_MEDIA_AUDIO_DECODER_MAIN,		// Main decoder (MPEG, AC3 downmix .... )
	DI_MEDIA_AUDIO_DECODER_SUB,		// Audio Description
	DI_MEDIA_AUDIO_DECODER_SPDIF		// AC3 bypass
} DI_MEDIA_AUDIO_TYPE_e;

/**
 * Define Audio Decoder Type
 */
typedef enum {
	DI_MEDIA_VIDEO_DECODER_NONE,
	DI_MEDIA_VIDEO_DECODER_MAIN,		// Main decoder
	DI_MEDIA_VIDEO_DECODER_SUB		// Sub decoder
} DI_MEDIA_VIDEO_TYPE_e;

typedef enum
{
	DI_MEDIA_PLAYPUMP_DIVX=0,
	DI_MEDIA_PLAYPUMP_VIDEO,    // for PES, ES
	DI_MEDIA_PLAYPUMP_AUDIO_MAIN, // for MP3, PES, ES
	DI_MEDIA_PLAYPUMP_AUDIO_SUB, // for MP3, PES, ES
	DI_MEDIA_PLAYPUMP_MAX
}DI_MEDIA_PLAYPUMP;

#endif /*__DI_MEDIA_TYPES_H__*/


#ifndef MX_DETECT_DEFINE__H_
#define MX_DETECT_DEFINE__H_



#ifdef __cplusplus
extern "C" 
{
#endif


#include "mxtypes.h"


//Struct
// ------------------------------------------------------------------------------
// |63 ~ 28 |27 ~ 24 |23~22 | 21~20 |19~18|17~16 |15 ~ 12|11 ~ 8 |7 ~ 4 |3 ~ 0  |
// ------------------------------------------------------------------------------
// |        |channels|timeS | byteS | play|Direct|        Profile ID			|	// Music
// ------------------------------------------------------------------------------

typedef struct _MXMEDIA_INFO_TAG
{
	MX_INT32 media_format;		// mandatory
	MX_INT32 media_info;		// required
	MX_INT32 duration;		// optional
	MX_UINT64 protocolInfo;	// optional

}MXMEDIA_INFO;





// 20101220 More Detect
typedef enum _MXMEDIA_ASPECT_RATIO
{
	MX_ASPECTRATIO_NONE		= 0,
	MX_ASPECTRATIO_1_1		= 1,
	MX_ASPECTRATIO_4_3		= 2,
	MX_ASPECTRATIO_16_9		= 3,
	MX_ASPECTRATIO_221_1	= 4,	// 2.21 :1	
}MXMEDIA_ASPECT_RATIO;


typedef enum _MXMEDIA_FRAME_RATE
{
	MX_FRAMERATE_NONE	= 0,
	MX_FRAMERATE_23_976	= 1,		// 23.976
	MX_FRAMERATE_24		= 2,		
	MX_FRAMERATE_25		= 3,
	MX_FRAMERATE_29_97	= 4,		// 29.97
	MX_FRAMERATE_30		= 5,
	MX_FRAMERATE_50		= 6,
	MX_FRAMERATE_59_94	= 7,		// 59.94
	MX_FRAMERATE_60		= 8,	
}MXMEDIA_FRAME_RATE;



//----------------------------------------------------------------------//
typedef struct VideoInfoTag
//----------------------------------------------------------------------//
{
	MX_INT16 codec;
	MX_INT16 codecEx;
	MX_INT32 duration;
	MX_INT32 width , height;
	MX_INT32 lang;


	// 20101220 More Detect
	MXMEDIA_ASPECT_RATIO	Aspect_ratio;	// 4
	MXMEDIA_FRAME_RATE		Framerate;		// 4
	int Bitrate;							// 18
} VideoInfo;

//----------------------------------------------------------------------//
typedef struct AudioInfoTag
//----------------------------------------------------------------------//
{
	MX_INT16 codec;
	MX_INT16 codecEx;
	MX_INT32 duration;
	MX_INT32 frequency;
	MX_INT32 channel;
	MX_INT32 bitPerSample;
	MX_INT32 bitrate;
	MX_INT32 lang;
} AudioInfo ;


//----------------------------------------------------------------------//
typedef struct ImageInfoTag
//----------------------------------------------------------------------//
{
	MX_INT32 width, height;
} ImageInfo;

//----------------------------------------------------------------------//
typedef struct SubtitleInfoTag
//----------------------------------------------------------------------//
{
	void * info;	
} SubtitleInfo;


//----------------------------------------------------------------------//
typedef struct _SectionInfo
//----------------------------------------------------------------------//
{	
	MX_INT32 TitleCmd;
	MX_INT8 *Content;			// ISO_8859_1이면 지역코드값에 기반하여 변환 , UNICODE 이면 알아서 변환
	MX_INT32 nContentSize;
	MX_INT32 EncondingType;		// MX_ENCODING_ISO_8859_1 , MX_ENCODING_UNICODE
	struct _SectionInfo *pNext;
} SectionInfo;








#define MAX_STREAM_INFO 5

//----------------------------------------------------------------------//
typedef struct avInfoTag
//----------------------------------------------------------------------//
{
	MX_INT32 videoCnt, audioCnt;
	VideoInfo video[MAX_STREAM_INFO];
	AudioInfo audio[MAX_STREAM_INFO];	
	SubtitleInfo subtitle;
} AVInfo;




//----------------------------------------------------------------------//
typedef union unionInfoTag
//----------------------------------------------------------------------//
{
	AVInfo av;
	ImageInfo image;	
} UnionInfo;





//----------------------------------------------------------------------//
typedef struct codecInfoTag
//----------------------------------------------------------------------//
{	
	MX_INT32 media_format;		// media_format
	MX_INT32 media_subformat;	// detail

	MX_INT32 encrypted;			// isDRM
	MX_UINT64 filesize;

	UnionInfo media; 
	
}MXCODEC_INFO;




//----------------------------------------------------------------------//
typedef struct mediaInfoTag
//----------------------------------------------------------------------//
{

	MX_UINT64 filesize;
	SectionInfo * section;

}MXMETA_INFO;














#define MX_MAX_RET		5


// 아래 MX_MAX_RET 값과 20의 값을 변경시에는 Android NDK 에도 적용 시켜 줘야 합니다.
// 검색은 MXMULTI_RET을 통해서 찾으시면 됩니다.

typedef struct _MXMULTI_RET
{
	MX_INT32 retCount;
	MX_INT8 retValue[ MX_MAX_RET ] [ 20 ];
}MXMULTI_RET;



typedef struct _MXDETECT_INFO_TITLE
{
	MX_INT32 titleCmd;
	MX_INT8 titleStr[128];

}MXDETECT_INFO_TITLE;



#define VCODEC_UNKNOWN			0
#define VCODEC_MPEG1			1
#define VCODEC_MPEG2			52
#define VCODEC_MPEG				53		// mpeg1 or mpeg2 or else



#define VCODEC_MP4V				8
#define VCODEC_WMV7				10
#define VCODEC_WMV8				11
#define VCODEC_WMV9				12
#define VCODEC_DVRMS			13
#define VCODEC_DIVX3			20
#define VCODEC_DIVX4			21
#define VCODEC_DIVX5			22
#define VCODEC_MP43				23
#define VCODEC_XVID				24
#define VCODEC_H264				30
#define VCODEC_WVC1				31
#define VCODEC_DV				50
#define VCODEC_MS				51


#define VCODEC_CVID				100		// http://en.wikipedia.org/wiki/Cinepak
#define VCODEC_SVQ				101		// http://developer.apple.com/documentation/QuickTime/QTFF/QTFFChap3/chapter_4_section_2.html#//apple_ref/doc/uid/TP40000939-CH205-BBCBACAB
#define VCODEC_H263				102


#define ACODEC_UNKNOWN			0
#define ACODEC_MPEG				1

#define ACODEC_WMA				3
#define ACODEC_WMAPRO			4
#define ACODEC_PCM				5
#define ACODEC_PCM_LSBFIRST		6
#define ACODEC_AC3				7
#define ACODEC_DTS				8
#define ACODEC_LPCM				9
#define ACODEC_LPCM_LSBFIRST	10

#define ACODEC_AAC_MAIN			11
#define ACODEC_AAC_LTP			12
#define ACODEC_AAC_LC			13
#define ACODEC_AAC_SSR			14
#define ACODEC_AAC_UNKNOWN		15
	#define ACODEC_AAC_SUB_RAW	0
	#define ACODEC_AAC_SUB_ADIF	1
	#define ACODEC_AAC_SUB_ADTS	2


#define ACODEC_OGG				20
#define ACODEC_FLAC				21


#define ACODEC_IMA				100
#define ACODEC_QDESIGN			101



// SectionInfo's Encoding Type value
#define	MX_ENCODING_ISO_8859_1		100
#define	MX_ENCODING_UNICODE			200


// SectionInfo's TitleCmd
typedef enum _MX_MP3_INFO
{
	//--------------------------------------------
	// mp3
	//--------------------------------------------
	MX_MP3_TITLE = 1,
	MX_MP3_ARTIST,
	MX_MP3_ALBUM,
	MX_MP3_GENRE,
	MX_MP3_COMPOSER,
	MX_MP3_PUBLISHER,
	MX_MP3_TRACK,
	MX_MP3_YEAR,
	MX_MP3_TYPE,
	MX_MP3_THUMBNAIL = 100,
		
}
MX_MP3_INFO;


//static MX_INT8 *titles[5] ; // = { "Title","Author","Copyright","Description","Rating" };
//static MX_INT8 *szGoodNames[] = {"WM/Year", "WM/Genre", "WM/Composer", 
//								"WM/Publisher", "WM/AlbumTitle", "WM/AlbumArtist",  "WM/ProviderRating"};
	
typedef enum _MX_WMA_INFO
{
	//--------------------------------------------
	// wma
	//--------------------------------------------	
	MX_WMA_TITLE = 1,
	MX_WMA_AUTHOR,
	MX_WMA_COPYRIGHT,
	MX_WMA_DESC,
	MX_WMA_RATING,

	MX_WMA_YEAR,
	MX_WMA_GENRE,
	MX_WMA_COMPOSER,
	MX_WMA_PUBLISHER,
	MX_WMA_ALBUMTITLE,
	MX_WMA_ALBUMARTIST,
	MX_WMA_PROVIDERRATING,
	
	MX_WMA_THUMBNAIL = MX_MP3_THUMBNAIL,
}
MX_WMA_INFO;

typedef enum _MX_IMAGE_INFO
{
	MX_IMG_EXIF_CAMERA_MAKER = 1,
	MX_IMG_EXIF_CAMERA_MODEL,
	MX_IMG_EXIF_CAMERA_DATE,
	MX_IMG_EXIF_CAMERA_RESOLUTION,
	MX_IMG_EXIF_CAMERA_ORIENTATION,
	MX_IMG_EXIF_CAMERA_FLASH,
	MX_IMG_EXIF_CAMERA_FOCAL_LENGTH,
	MX_IMG_EXIF_CAMERA_FOCUS_DIST,
	MX_IMG_EXIF_CAMERA_APERTURE,
	MX_IMG_EXIF_CAMERA_ISO,


	//--------------------------------------------
	// image
	//--------------------------------------------
	MX_IMG_JPG,
	MX_IMG_BMP,
	MX_IMG_GIF,
	MX_IMG_PNG,	
		
	// 20100802 thumbnail
	MX_IMG_EXIF_THUMBNAIL = MX_MP3_THUMBNAIL,
		
}MX_IMAGE_INFO;



















/////////////////////////////////////////////////////////////////////
//media Type Start
/////////////////////////////////////////////////////////////////////
#ifdef __VALUPS_PLAYER__
#include "valups_type.h"

 /* container type */
#define	MEDIA_CONT_MPEG1_SYSTEM	ICPLAYER_CONT_MPEG1_SYSTEM	
#define	MEDIA_CONT_MPEG2_PS		ICPLAYER_CONT_MPEG2_PS			// program stream
#define	MEDIA_CONT_MPEG2_TS		ICPLAYER_CONT_MPEG2_TS			// transport stream
#define	MEDIA_CONT_ASF			ICPLAYER_CONT_ASF				// WMV, ASF, WMA		// WMV9
#define	MEDIA_CONT_AVI			ICPLAYER_CONT_AVI			
#define	MEDIA_CONT_MPEG4		ICPLAYER_CONT_MPEG4				// MPEG4 system
#define	MEDIA_CONT_FLV			ICPLAYER_CONT_FLV				// flash video

// <2008/10/16 added>
#define	MEDIA_CONT_MPEG2_ES		ICPLAYER_CONT_MPEG2_ES			// 
#define	MEDIA_CONT_3GPP			ICPLAYER_CONT_3GPP				

#define	MEDIA_CONT_MP3			ICPLAYER_CONT_MP3			
#define	MEDIA_CONT_WMA			ICPLAYER_CONT_WMA			
#define	MEDIA_CONT_WAV			ICPLAYER_CONT_WAV			
#define	MEDIA_CONT_AC3			ICPLAYER_CONT_AC3			
#define	MEDIA_CONT_AAC			ICPLAYER_CONT_AAC			
#define	MEDIA_CONT_OGG			ICPLAYER_CONT_OGG			
#define	MEDIA_CONT_FLAC			ICPLAYER_CONT_FLAC			
#define	MEDIA_CONT_LPCM			ICPLAYER_CONT_LPCM			
// </2008/10/16 added>


/* container sub type */
#define	MEDIA_CONT_MPEG2_TS_188	ICPLAYER_CONT_MPEG2_TS_188		// 188-byte packet
#define	MEDIA_CONT_MPEG2_TS_192	ICPLAYER_CONT_MPEG2_TS_192		// 192-byte packet



// <2008/10/16 added>
#define	MEDIA_CONT_LPCM_16_44100_1	ICPLAYER_CONT_LPCM_16_44100_1	// 
#define	MEDIA_CONT_LPCM_16_44100_2	ICPLAYER_CONT_LPCM_16_44100_2	// 
#define	MEDIA_CONT_LPCM_16_48000_1	ICPLAYER_CONT_LPCM_16_48000_1	// 
#define	MEDIA_CONT_LPCM_16_48000_2	ICPLAYER_CONT_LPCM_16_48000_2	// 

#define MEDIA_UNKNOWN_VALUE		ICPLYAER_UNKNOWN_VALUE		



/* codec type */
#define MEDIA_CODEC_MPEG1			ICPLAYER_CODEC_MPEG1		
#define MEDIA_CODEC_MPEG2			ICPLAYER_CODEC_MPEG2		
#define MEDIA_CODEC_MPEG4			ICPLAYER_CODEC_MPEG4		
#define MEDIA_CODEC_WMV9			ICPLAYER_CODEC_WMV9			
#define MEDIA_CODEC_AVC			ICPLAYER_CODEC_AVC			

/* Resolution Type */
#define MEDIA_RES_CIF15			ICPLAYER_RES_CIF15			
#define MEDIA_RES_CIF30			ICPLAYER_RES_CIF30			
#define MEDIA_RES_QCIF15			ICPLAYER_RES_QCIF15			

#define MEDIA_RES_LOW				ICPLAYER_RES_LOW			
#define MEDIA_RES_MED				ICPLAYER_RES_MED			
#define MEDIA_RES_HIGH				ICPLAYER_RES_HIGH			

#define MEDIA_RES_NTSC				ICPLAYER_RES_NTSC			
#define MEDIA_RES_PAL				ICPLAYER_RES_PAL			

#define MEDIA_RES_HD				ICPLAYER_RES_HD				
#define MEDIA_RES_SD				ICPLAYER_RES_SD				

#define MEDIA_RES_VGA				ICPLAYER_RES_VGA			

/* Audio Codec Type */
#define MEDIA_ACODEC_AAC			ICPLAYER_ACODEC_AAC				
#define MEDIA_ACODEC_AAC_520		ICPLAYER_ACODEC_AAC_520			
#define MEDIA_ACODEC_AAC_540		ICPLAYER_ACODEC_AAC_540			
#define MEDIA_ACODEC_AAC_940		ICPLAYER_ACODEC_AAC_940			
#define MEDIA_ACODEC_AAC_LTP		ICPLAYER_ACODEC_AAC_LTP			
#define MEDIA_ACODEC_AAC_LTP_520	ICPLAYER_ACODEC_AAC_LTP_520		

#define MEDIA_ACODEC_HEAAC			ICPLAYER_ACODEC_HEAAC			
#define MEDIA_ACODEC_HEAAC_L12		ICPLAYER_ACODEC_HEAAC_L12		
#define MEDIA_ACODEC_HEAAC_L1B		ICPLAYER_ACODEC_HEAAC_L1B		
#define MEDIA_ACODEC_HEAAC_L2		ICPLAYER_ACODEC_HEAAC_L2		

#define MEDIA_ACODEC_AC3			ICPLAYER_ACODEC_AC3				
#define MEDIA_ACODEC_XAC3			ICPLAYER_ACODEC_XAC3			

#define MEDIA_ACODEC_AMR			ICPLAYER_ACODEC_AMR				
#define MEDIA_ACODEC_AMR_WBPLUS	ICPLAYER_ACODEC_AMR_WBPLUS		

#define MEDIA_ACODEC_ATRAC3PLUS	ICPLAYER_ACODEC_ATRAC3PLUS		

#define MEDIA_ACODEC_BSAC			ICPLAYER_ACODEC_BSAC			

#define MEDIA_ACODEC_G726			ICPLAYER_ACODEC_G726			

#define MEDIA_ACODEC_MPEG1_L3		ICPLAYER_ACODEC_MPEG1_L3		

#define MEDIA_ACODEC_MPEG2_L2		ICPLAYER_ACODEC_MPEG2_L2		

#define MEDIA_ACODEC_PRO			ICPLAYER_ACODEC_PRO				
#define MEDIA_ACODEC_BASE			ICPLAYER_ACODEC_BASE			
#define MEDIA_ACODEC_FULL			ICPLAYER_ACODEC_FULL			
#define MEDIA_ACODEC_MP3			ICPLAYER_ACODEC_MP3				


/* region type */
#define MEDIA_REG_NA				ICPLAYER_REG_NA					
#define MEDIA_REG_KO				ICPLAYER_REG_KO					
#define MEDIA_REG_EU				ICPLAYER_REG_EU					


/* profile type */
#define	MEDIA_PROFILE_ASP			ICPLAYER_PROFILE_ASP			// advanced simple profile
#define	MEDIA_PROFILE_BL			ICPLAYER_PROFILE_BL				// baseline profile
#define	MEDIA_PROFILE_CO			ICPLAYER_PROFILE_CO				// core profile
#define	MEDIA_PROFILE_H263			ICPLAYER_PROFILE_H263			// H.263 profile
#define	MEDIA_PROFILE_HM			ICPLAYER_PROFILE_HM				//High MAT profile
#define	MEDIA_PROFILE_MP			ICPLAYER_PROFILE_MP				// main profile
#define	MEDIA_PROFILE_SP			ICPLAYER_PROFILE_SP				// simple profile


/* Timestamp */
#define	MEDIA_TIMESTAMP_NULL		ICPLAYER_TIMESTAMP_NULL			// TimeStamp 필드가 0으로 채워져 있다
#define	MEDIA_TIMESTAMP_ISO		ICPLAYER_TIMESTAMP_ISO			// TimeStamp 필드가 빠져있다
#define	MEDIA_TIMESTAMP_T			ICPLAYER_TIMESTAMP_T			// TimeStamp 필드가 유효한 값으로 채워져 있다
// </2008/10/16 added>


/////////////////////////////////////////////////////////////////////
//media Type Else
/////////////////////////////////////////////////////////////////////
#else

/* container type */
#define	MEDIA_CONT_MPEG1_SYSTEM	0x01
#define	MEDIA_CONT_MPEG2_PS		0x02	// program stream
#define	MEDIA_CONT_MPEG2_TS		0x03	// transport stream
#define	MEDIA_CONT_ASF				0x04	// WMV, ASF, WMA		// WMV9
#define	MEDIA_CONT_AVI				0x05
#define	MEDIA_CONT_MPEG4			0x07	// MPEG4 system
#define	MEDIA_CONT_FLV				0x08	// flash video

// <2008/10/16 added>
#define	MEDIA_CONT_MPEG2_ES		0x09	// 
#define	MEDIA_CONT_3GPP			0x0A	

#define	MEDIA_CONT_MP3				0x06
#define	MEDIA_CONT_WMA				0x12
#define	MEDIA_CONT_WAV				0x13
#define	MEDIA_CONT_AC3				0x14
#define	MEDIA_CONT_AAC				0x15
#define	MEDIA_CONT_OGG				0x16
#define	MEDIA_CONT_FLAC			0x17
#define	MEDIA_CONT_LPCM			0x18
// </2008/10/16 added>


/* container sub type */
#define	MEDIA_CONT_MPEG2_TS_188	0x01	// 188-byte packet
#define	MEDIA_CONT_MPEG2_TS_192	0x02	// 192-byte packet



// <2008/10/16 added>
#define	MEDIA_CONT_LPCM_16_44100_1	0x03	// 
#define	MEDIA_CONT_LPCM_16_44100_2	0x04	// 
#define	MEDIA_CONT_LPCM_16_48000_1	0x05	// 
#define	MEDIA_CONT_LPCM_16_48000_2	0x06	// 

#define MEDIA_UNKNOWN_VALUE		0x00


/* codec type */
#define MEDIA_CODEC_MPEG1			0x01
#define MEDIA_CODEC_MPEG2			0x02
#define MEDIA_CODEC_MPEG4			0x03
#define MEDIA_CODEC_WMV9			0x04
#define MEDIA_CODEC_AVC			0x05

/* Resolution Type */
#define MEDIA_RES_CIF15			0x11
#define MEDIA_RES_CIF30			0x12
#define MEDIA_RES_QCIF15			0x13

#define MEDIA_RES_LOW				0x21
#define MEDIA_RES_MED				0x22
#define MEDIA_RES_HIGH				0x23

#define MEDIA_RES_NTSC				0x31
#define MEDIA_RES_PAL				0x32

#define MEDIA_RES_HD				0x41
#define MEDIA_RES_SD				0x42

#define MEDIA_RES_VGA				0x51

/* Audio Codec Type */
#define MEDIA_ACODEC_AAC				0x11
#define MEDIA_ACODEC_AAC_520			0x12
#define MEDIA_ACODEC_AAC_540			0x13
#define MEDIA_ACODEC_AAC_940			0x14
#define MEDIA_ACODEC_AAC_LTP			0x15
#define MEDIA_ACODEC_AAC_LTP_520		0x16

#define MEDIA_ACODEC_HEAAC			0x21
#define MEDIA_ACODEC_HEAAC_L12		0x22
#define MEDIA_ACODEC_HEAAC_L1B		0x23
#define MEDIA_ACODEC_HEAAC_L2		0x24

#define MEDIA_ACODEC_AC3			0x31
#define MEDIA_ACODEC_XAC3			0x32

#define MEDIA_ACODEC_AMR			0x41
#define MEDIA_ACODEC_AMR_WBPLUS	0x42

#define MEDIA_ACODEC_ATRAC3PLUS	0x51

#define MEDIA_ACODEC_BSAC			0x61

#define MEDIA_ACODEC_G726			0x71

#define MEDIA_ACODEC_MPEG1_L3		0x81

#define MEDIA_ACODEC_MPEG2_L2		0x91

#define MEDIA_ACODEC_PRO			0xA1
#define MEDIA_ACODEC_BASE			0xA2
#define MEDIA_ACODEC_FULL			0xA3
#define MEDIA_ACODEC_MP3			0xA4


/* region type */
#define MEDIA_REG_NA				0x01
#define MEDIA_REG_KO				0x02
#define MEDIA_REG_EU				0x03


/* profile type */
#define	MEDIA_PROFILE_ASP			0x01	// advanced simple profile
#define	MEDIA_PROFILE_BL			0x02	// baseline profile
#define	MEDIA_PROFILE_CO			0x03	// core profile
#define	MEDIA_PROFILE_H263			0x04	// H.263 profile
#define	MEDIA_PROFILE_HM			0x05	//High MAT profile
#define	MEDIA_PROFILE_MP			0x06	// main profile
#define	MEDIA_PROFILE_SP			0x07	// simple profile


/* Timestamp */
#define	MEDIA_TIMESTAMP_NULL	0x01	// TimeStamp 필드가 0으로 채워져 있다
#define	MEDIA_TIMESTAMP_ISO		0x02	// TimeStamp 필드가 빠져있다
#define	MEDIA_TIMESTAMP_T		0x03	// TimeStamp 필드가 유효한 값으로 채워져 있다
// </2008/10/16 added>
#endif
/////////////////////////////////////////////////////////////////////
//media Type End
/////////////////////////////////////////////////////////////////////



	
#ifdef __cplusplus
}
#endif






#endif


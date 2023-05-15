#ifndef MX_INC_MEDIA_FORMAT_H__
#define MX_INC_MEDIA_FORMAT_H__



#ifdef __cplusplus
extern "C" 
{
#endif


//---------------------------------------------------------------------------------
// Return value
//---------------------------------------------------------------------------------
#define MEDIA_SUCCESS	(1)
#define MEDIA_FAIL		(0)




#define UNKNOWN_STR		("unknown_str")

//------------------------------------------------------------------------------
// media_format
//------------------------------------------------------------------------------
#define MF_UNKNOWN		0


// VIDEO
#define	MF_VIDEO_WMV		0x110
#define	MF_VIDEO_MPG		0x120
	#define MSF_VIDEO_MPEG1			0x120
	
	// 20100121 jonghyun
	//#define MSF_VIDEO_MPEG2_TS188	0x121
	//#define MSF_VIDEO_MPEG2_TS192	0x122
	//#define MSF_VIDEO_MPEG2_PS	0x125
	#define MSF_VIDEO_MPEG_TS188	0x121
	#define MSF_VIDEO_MPEG_TS192	0x122
	#define MSF_VIDEO_MPEG_PS		0x125

	#define MSF_VIDEO_DVD			0x123
	#define MSF_VIDEO_VCD			0x124
	

#define	MF_VIDEO_AVI		0x130 
#define	MF_VIDEO_MP4		0x140
#define MF_VIDEO_MKV		0x150
#define MF_VIDEO_K3G		0x160
#define MF_VIDEO_MOV		0x170
#define MF_VIDEO_3GP		0x180
#define MF_VIDEO_FLV		0x190
#define MF_VIDEO_WEBM		0x1A0 //yhkang added webM by google
#define MF_VIDEO_M3U		0x1B0

		
// AUDIO
#define	MF_AUDIO_WMA		0x210 
#define	MF_AUDIO_MP3		0x220
#define MF_AUDIO_WAV		0x240
#define MF_AUDIO_AC3		0x250
#define MF_AUDIO_AAC		0x260		//MPEG4
#define MF_AUDIO_OGG		0x270
#define MF_AUDIO_FLAC		0x280
#define MF_AUDIO_APE		0x290


#define MF_AUDIO_AMR		0x2B0
#define MF_AUDIO_ATRAC3		0x2C0

#define MF_AUDIO_LPCM		0x2D0
#define	MF_AUDIO_MP4		0x2E0
#define	MF_AUDIO_MP4A		0x2E1
#define MF_AUDIO_M3U		0x2F0


// IMAGE
#define	MF_IMAGE_JPG 		0x310
#define	MF_IMAGE_BMP 		0x320
#define	MF_IMAGE_PNG 		0x330
#define	MF_IMAGE_GIF 		0x340
#define	MF_IMAGE_TIFF 		0x350

// dypark : 가상의 Thumbnail 포맷 정의 2012-05-24
#ifdef DMS_DUMMY_THUMBNAIL_RESOURCE	
#define	MX_IMAGE_THUMB_JPG	0x360
#endif

//------------------------------------------------------------------------------

// TEXT ( Documnet )
#define MF_TEXT_TXT		0x410
#define MF_TEXT_DOC		0x420
#define MF_TEXT_HTML	0x430
#define MF_TEXT_PDF		0x440
#define MF_TEXT_RTF		0x450

#define MF_TEXT_DOCX	0x460
#define MF_TEXT_XLS		0x470
#define MF_TEXT_PPT		0x480
#define MF_TEXT_XLSX	0x490
#define MF_TEXT_PPTX	0x4A0
#define MF_TEXT_XML		0x4B0

// SYSTEM
#define MF_SYSTEM_DTCP	0x510
#define MF_SYSTEM_SMI	0x520
#define MF_SYSTEM_SUB	0x530
	// 20101001 - subTitle 추가
#define MF_SYSTEM_SRT	0x540
#define MF_SYSTEM_SSA	0x550

#define MF_SYSTEM_IFO	0x560
#define MF_SYSTEM_M3U	0x570


	
//------------------------------------------------------------------------------
// media_info
//------------------------------------------------------------------------------
#define	MI_NULL						(0x0)
// AUDIO
	// LPCM
#define	MI_AUDIO_LPCM_16_48000_2	0x7D0
#define	MI_AUDIO_LPCM_16_48000_1	0x7D1
#define	MI_AUDIO_LPCM_16_44100_2	0x7D2
#define	MI_AUDIO_LPCM_16_44100_1	0x7D3
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
// media_type
//------------------------------------------------------------------------------
#define MF_VIDEO	0x100
#define MF_AUDIO	0x200
#define MF_IMAGE	0x300
#define MF_TEXT		0x400
#define MF_SYSTEM	0x500




#define MASK_MEDIATYPE( a )  ( (a) & 0xf00 )
//------------------------------------------------------------------------------











#define MXMEDIA_OK						(0)

// error value define
#define MXMEDIA_FAIL					(-1)
#define MXMEDIA_ERR_PARAMETER			(-2)
#define MXMEDIA_ERR_CALLFUNCTION		(-3)

#define MXMEDIA_ERR_NOT_DEFINED_CMD		(-4)
#define MXMEDIA_ERR_NOT_HTTP			(-5)		// http 로 무언가 시켰는데 http가 아닐때.




#ifdef __cplusplus
}
#endif



#endif


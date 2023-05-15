#ifndef MX_MEDIA_PROFILE_DEFINE_H__
#define MX_MEDIA_PROFILE_DEFINE_H__




#ifdef __cplusplus
extern "C" 
{
#endif





//-----------------------------------------------------------------------------------------//
//Media Profiles
//-----------------------------------------------------------------------------------------//
#define MP_UNKNOWN			( 0x00 )


//-----------------------------------------------------------------------------------------//
// Image
//-----------------------------------------------------------------------------------------//
#define MP_JPEG_SM			( 0x01 )		// max = 640 x 480
#define MP_JPEG_MED			( 0x02 )		// max = 1024 x 768
#define MP_JPEG_LRG			( 0x03 )		// max = 4096 x 4096
#define MP_JPEG_TN			( 0x04 )		// max = 160 x 160
#define MP_JPEG_SM_ICO		( 0x05 )		// max = 48 x 48
#define MP_JPEG_LRG_ICO		( 0x06 )		// max = 120 x 120
	
#define MP_PNG_LRG			( 0x11 )		// max = 4096 x 4096	
#define MP_PNG_TN			( 0x12 )		// max = 160 x 160		
#define MP_PNG_SM_ICO		( 0x13 )		// max = 48 x 48		
#define MP_PNG_LRG_ICO		( 0x14 )		// max = 120 x 120
//-----------------------------------------------------------------------------------------//
  
    
//-----------------------------------------------------------------------------------------//
//Audio
//-----------------------------------------------------------------------------------------//
//ac3
#define MP_AUDIO_AC3		( 0x01 )	// audio/vnd.dolby.dd-raw , 2ch multi , dvd audio

//amr profiles
#define MP_AUDIO_AMR_3GPP	( 0x11 )	// audio/3gpp, audio/mp4 , mono , cellular Phones and PDA
#define MP_AUDIO_AMR_WBPLUS	( 0x12 )	// audio/3gpp,				2ch , cellular Phones and PDA , DVB

//ATRAC3Plus Profiles					// Sony Compress Codec
#define MP_AUDIO_ATRAC3PLUS	( 0x21 )	// audio/x-sony-oma,		2ch , md , pc ,multi channel audio

//LPCM Profiles
#define MP_AUDIO_LPCM		( 0x31 )	// audio/L16,				2ch ,	Radio Broadcasts , User Created Audio Content
		
//MP3 Profiles
#define MP_AUDIO_MP3		( 0x41 )	// audio/mpeg,				2ch ,	
#define MP_AUDIO_MP3X		( 0x42 )	// audio/mpeg,				2ch ,	Radio BroadCasts , Ripping CD Audio Content , internet radio broadcasting

//MPEG-4(AAC) Profiles	
#define MP_AUDIO_AAC_ADTS			( 0x51 )	// audio/vnd.dlna.adts,		2ch ,	Radio BroadCasts , internet radio broadcasting , ARIB Radio
#define MP_AUDIO_AAC_ADTS_320		( 0x52 )	// audio/vnd.dlna.adts,		2ch ,	Radio BroadCasts , Digital radio broadcasting , ARIB Radio
#define MP_AUDIO_AAC_LTP_ISO		( 0x53 )	// audio/mp4,audio/3gpp,	2ch ,	Radio BroadCasts , Audio component of TV Broadcast 
#define MP_AUDIO_AAC_LTP_MULT5_ISO	( 0x54 )	// audio/mp4,audio/3gpp,	multi ,	Radio BroadCasts , Audio component of TV Broadcast ( 5.1 channels )
												// audio/vnd.dlna.adts
#define MP_AUDIO_AAC_LTP_MULT7_ISO	( 0x55 )	// audio/mp4,audio/3gpp,	multi ,	Radio BroadCasts , Audio component of TV Broadcast ( 7.1 channels )
												// audio/vnd.dlna.adts
#define MP_AUDIO_AAC_MULT5_ADTS		( 0x56 )	// audio/vnd.dlna.adts,		multi ,	Radio BroadCasts , internet radio broadcasting , (5.1 Channels )
#define MP_AUDIO_AAC_MULT5_ISO		( 0x57 )	// audio/mp4,audio/3gpp,	multi ,	required in ISMA (5.1 Channels )
#define MP_AUDIO_AAC_ISO_320		( 0x58 )	// audio/mp4,audio/3gpp


#define MP_AUDIO_HEAAC_L2_ADTS		( 0x61 )	// audio/vnd.dlna.adts,		2ch,	Radio BroadCasts , Digital radio broadcasting 
#define MP_AUDIO_HEAAC_L2_ISO		( 0x62 )	// audio/mp4,audio/3gpp,	2ch,	Radio BroadCasts , Digital radio broadcasting 
#define MP_AUDIO_HEAAC_L3_ADTS		( 0x63 )	// audio/vnd.dlna.adts,		2ch,	Radio BroadCasts , Digital radio broadcasting 
#define MP_AUDIO_HEAAC_L3_ISO		( 0x64 )	// audio/mp4,audio/3gpp,	2ch,	Radio BroadCasts , Digital radio broadcasting 
#define MP_AUDIO_HEAAC_MULT5_ADTS	( 0x65 )	// audio/vnd.dlna.adts,		multi ,	Radio BroadCasts , DVD-A , Multichannel Music Tracks from AV Content , (5.1 Channels )
#define MP_AUDIO_HEAAC_MULT5_ISO	( 0x66 )	// audio/mp4,audio/3gpp,	multi ,	required in ISMA (5.1 Channels ) , 3GPP , ISMA , DVB
#define MP_AUDIO_HEAAC_ADTS_320		( 0x67 )	// audio/vnd.dlna.adts,		2ch ,	Radio BroadCasts , Digital radio broadcasting  
#define MP_AUDIO_HEAAC_L2_ISO_320	( 0x68 )	// audio/mp4,audio/3gpp,	2ch,	Radio BroadCasts , Digital radio broadcasting 

#define MP_AUDIO_BSAC_ISO			( 0x71 )	// audio/mp4,audio/3gpp,	2ch,	Radio BroadCasts , Audio component of TV Broadcast 
#define MP_AUDIO_BSAC_MULT5_ISO		( 0x72 )	// audio/mp4,audio/3gpp,	2ch,	Radio BroadCasts , Audio component of TV Broadcast ( 5.1 Channels )



//WMA Profiles
#define MP_AUDIO_WMABASE			( 0x81 )	// audio/x-ms-wma,				2ch ,	Internet Music Services , Internet Radio , wma content ( bitrates less than 193kbps )
#define MP_AUDIO_WMAFULL			( 0x82 )	// audio/x-ms-wma,				2ch ,	Internet Music Services , Internet Radio
#define MP_AUDIO_WMAPRO				( 0x83 )	// audio/x-ms-wma,				2ch multi ,	Internet Music Services , Internet Radio ( WMA Professional version )
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//Video
//-----------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------//
//MPEG 1
//-----------------------------------------------------------------------------------------//
#define MP_VIDEO_MPEG1								( 0x1101 )	// video/mpeg	//MPEG-1 video with 2 channel MPEG-1 Layer2 audio encapsulated in MPEG-1 system


//-----------------------------------------------------------------------------------------//
//MPEG2
//-----------------------------------------------------------------------------------------//
#define MP_VIDEO_MPEG_PS_NTSC						( 0x2101 )	// video/mpeg	//Profile for NTSC-formatted AV class media
#define MP_VIDEO_MPEG_PS_NTSC_XAC3					( 0x2102 )	// video/mpeg	//Profile for NTSC-formatted AV class media
#define MP_VIDEO_MPEG_PS_PAL						( 0x2103 )	// video/mpeg	//Profile for PAL-formatted AV class media
#define MP_VIDEO_MPEG_PS_PAL_XAC3					( 0x2104 )	// video/mpeg	//Profile for PAL-formatted AV class media
#define MP_VIDEO_MPEG_TS_SD_NA						( 0x2201 )	// video/vnd.dlna.mpeg-tts	//North America region profile for Standard Definition AV class utilizing a DLNA Transport Packet with zero value timestamp 

#define MP_VIDEO_MPEG_TS_SD_NA_T					( 0x2202 )	// video/vnd.dlna.mpeg-tts	//North America region profile for Standard Definition AV class utilizing a DLNA Transport Packet with a valid non-zero timestamp 
#define MP_VIDEO_MPEG_TS_SD_NA_ISO					( 0x2203 )	// video/mpeg	//North America region profile for Standard Definition AV class utilizing a DLNA Transport Packet without a Timestamp field 
#define MP_VIDEO_MPEG_TS_HD_NA						( 0x2204 )	// video/vnd.dlna.mpeg-tts	//North America region profile for High Definition AV class utilizing a DLNA Transport Packet with zero value timestamp
#define MP_VIDEO_MPEG_TS_HD_NA_T					( 0x2205 )	// video/vnd.dlna.mpeg-tts	//North America region profile for High Definition AV class utilizing a DLNA Transport Packet with a valid non-zero timestamp

#define MP_VIDEO_MPEG_TS_HD_NA_ISO					( 0x2206 )	// video/mpeg	//North America region profile for High Definition AV class utilizing a DLNA Transport Packet without a Timestamp field 
#define MP_VIDEO_MPEG_TS_SD_EU						( 0x2301 )	// video/vnd.dlna.mpeg-tts	//European region profile for Standard Definition AV class utilizing a DLNA Transport Packet with zero value timestamp
#define MP_VIDEO_MPEG_TS_SD_EU_T					( 0x2302 )	// video/vnd.dlna.mpeg-tts	//European region profile for Standard Definition AV class utilizing a DLNA Transport Packet with a valid non-zero timestamp
#define MP_VIDEO_MPEG_TS_SD_EU_ISO					( 0x2303 )	// Video/mpeg	//European region profile for Standard Definition AV class utilizing a DLNA Transport Packet without a Timestamp field

#define MP_VIDEO_MPEG_TS_SD_KO						( 0x2401 )	// video/vnd.dlna.mpeg-tts	//Korea region profile for Standard Definition AV utilizing a DLNA Transport Packet with zero value timestamp
#define MP_VIDEO_MPEG_TS_SD_KO_T					( 0x2402 )	// video/vnd.dlna.mpeg-tts	//Korea region profile for Standard Definition AV class utilizing a DLNA Transport Packet with a valid non-zero timestamp
#define MP_VIDEO_MPEG_TS_SD_KO_ISO					( 0x2403 )	// video/mpeg	//Korea region profile for Standard Definition AV class utilizing a DLNA Transport Packet without a Timestamp field

#define MP_VIDEO_MPEG_TS_HD_KO						( 0x2404 )	// video/vnd.dlna.mpeg-tts	//Korea region profile for High Definition AV class utilizing a DLNA Transport Packet with zero value timestamp
#define MP_VIDEO_MPEG_TS_HD_KO_T					( 0x2405 )	// video/vnd.dlna.mpeg-tts	//Korea region profile for High Definition AV class utilizing a DLNA Transport Packet with a valid non-zero timestamp
#define MP_VIDEO_MPEG_TS_HD_KO_ISO					( 0x2406 )	// video/mpeg	//Korea region profile for High Definition AV class utilizing a DLNA Transport Packet without a Timestamp field

#define MP_VIDEO_MPEG_TS_HD_KO_XAC3					( 0x2407 )	// video/vnd.dlna.mpeg-tts	//Korea region profile for transcoded High Definition AV class media with a zero value timestamp
#define MP_VIDEO_MPEG_TS_HD_KO_XAC3_T				( 0x2408 )	// video/vnd.dlna.mpeg-tts	//Korea region profile for transcoded High Definition AV class media with a valid non-zero timestamp
#define MP_VIDEO_MPEG_TS_HD_KO_XAC3_ISO				( 0x2409 )	// video/mpeg	//Korea region profile for transcoded High Definition AV class media without a Timestamp field

#define MP_VIDEO_MPEG_TS_HD_NA_XAC3					( 0x2501 )	// video/vnd.dlna.mpeg-tts	//North America region profile for transcoded High Definition AV class media with a zero value timestamp
#define MP_VIDEO_MPEG_TS_HD_NA_XAC3_T				( 0x2502 )	// video/vnd.dlna.mpeg-tts	//North America region profile for transcoded High Definition AV class media with a valid non-zero timestamp
#define MP_VIDEO_MPEG_TS_HD_NA_XAC3_ISO				( 0x2503 )	// video/mpeg	//North America region profile for transcoded High Definition AV class media without a Timestamp field

#define MP_VIDEO_MPEG_TS_SD_KO_XAC3					( 0x2601 )	// video/vnd.dlna.mpeg-tts	//Korea region profile for  Standard Definition AV class media with a zero value timestamp
#define MP_VIDEO_MPEG_TS_SD_KO_XAC3_T				( 0x2602 )	// video/vnd.dlna.mpeg-tts	//Korea region profile for Standard Definition AV class media with a valid non-zero timestamp
#define MP_VIDEO_MPEG_TS_SD_KO_XAC3_ISO				( 0x2603 )	// video/mpeg	//Korea region profile for  Standard Definition AV class media without a Timestamp field

#define MP_VIDEO_MPEG_TS_SD_NA_XAC3					( 0x2701 )	// video/vnd.dlna.mpeg-tts	//North America region profile for Standard Definition AV class media with a zero value timestamp
#define MP_VIDEO_MPEG_TS_SD_NA_XAC3_T				( 0x2702 )	// video/vnd.dlna.mpeg-tts	//North America region profile for Standard Definition AV class media with a valid non-zero timestamp
#define MP_VIDEO_MPEG_TS_SD_NA_XAC3_ISO				( 0x2703 )	// video/mpeg	//North America region profile for Standard Definition AV class media without a Timestamp field
#define MP_VIDEO_MPEG_TS_MP_LL_AAC					( 0x2801 )	// video/vnd.dlna.mpeg-tts	//MPEG-2 Main Profile at Low Level with AAC LC audio encapsulated in MPEG-2 transport stream with zero value timestamp

#define MP_VIDEO_MPEG_TS_MP_LL_AAC_T				( 0x2802 )	// video/vnd.dlna.mpeg-tts	//MPEG-2 Main Profile at Low Level with AAC LC audio encapsulated in MPEG-2 transport stream with valid timestamp
#define MP_VIDEO_MPEG_TS_MP_LL_AAC_ISO				( 0x2803 )	// video/mpeg	//MPEG-2 Main Profile at Low Level with AAC LC audio encapsulated in MPEG-2 transport stream without a Timestamp field
#define MP_VIDEO_MPEG_ES_PAL						( 0x2901 )	// video/mpeg	//Profile defining ES encapsulation for transport of MPEG_PS_PAL over RTP
#define MP_VIDEO_MPEG_ES_NTSC						( 0x2902 )	// video/mpeg	//Profile defining ES encapsulation for transport of MPEG_PS_NTSC over RTP
#define MP_VIDEO_MPEG_ES_PAL_XAC3					( 0x2903 )	// video/mpeg	//Profile defining ES encapsulation for transport of MPEG_PS_PAL_XAC3 over RTP

#define MP_VIDEO_MPEG_ES_NTSC_XAC3					( 0x2904 )	// video/mpeg	//Profile defining ES encapsulation for transport ofMPEG_PS_NTSC_XAC3 over RTP

// 20120224
#define MP_VIDEO_MPEG_TS_SD_JP_MPEG1_L2_T			( 0x2905 )

// 20121120
#define MP_VIDEO_MPEG_TS_JP_T						( 0x2906 )

// 20130117
#define MP_VIDEO_MPEG_TS_HD_60_L2_T					( 0x2907 )

//-----------------------------------------------------------------------------------------//
//MPEG4 Part2
//-----------------------------------------------------------------------------------------//
#define MP_VIDEO_MPEG4_P2_MP4_SP_AAC				( 0x3001 )	// video/mp4	//MPEG-4 Part2 Simple Profile with AAC LC audio, encapsulated in MP4.
#define MP_VIDEO_MPEG4_P2_MP4_SP_HEAAC				( 0x3002 )	// video/mp4	//MPEG-4 Part2 Simple Profile with HE AAC audio, encapsulated in MP4
#define MP_VIDEO_MPEG4_P2_MP4_SP_ATRAC3plus			( 0x3003 )	// video/mp4	//MPEG-4 Part2 Simple Profile with ATRAC3plus audio, encapsulated in MP4
#define MP_VIDEO_MPEG4_P2_MP4_SP_AAC_LTP			( 0x3004 )	// video/mp4	//MPEG-4 Part 2 Simple Profile with AAC LTP audio encapsulated in MP4
#define MP_VIDEO_MPEG4_P2_MP4_SP_L2_AAC				( 0x3005 )	// video/mp4	//MPEG-4 Part 2 Simple Profile Level 2 with AAC audio encapsulated in MP4

#define MP_VIDEO_MPEG4_P2_MP4_SP_L2_AMR				( 0x3006 )	// video/mp4	//MPEG-4 Part2 Simple Profile Level 2 with AMR audio encapsulated in MP4
#define MP_VIDEO_MPEG4_P2_TS_SP_AAC					( 0x3101 )	// video/vnd.dlna.mpeg-tts	//MPEG-4 Part2 Simple Profile with AAC LC audio encapsulated in MPEG-2-TS with a zero timstamp field.
#define MP_VIDEO_MPEG4_P2_TS_SP_AAC_T				( 0x3102 )	// video/vnd.dlna.mpeg-tts 	//MPEG-4 Part2 Simple Profile with AAC LC audio encapsulated in MPEG-2-TS with a valid timestamp field.
#define MP_VIDEO_MPEG4_P2_TS_SP_AAC_ISO				( 0x3103 )	// video/mpeg	//MPEG-4 Part2 Simple Profile with AAC LC audio encapsulated in MPEG-2-TS without a Timestamp field
#define MP_VIDEO_MPEG4_P2_TS_SP_MPEG1_L3			( 0x3104 )	// video/vnd.dlna.mpeg-tts	//MPEG-4 Part2 Simple Profile with MPEG-1 Layer3 audio encapsulated in MPEG-2-TS with a zero timestamp field.

#define MP_VIDEO_MPEG4_P2_TS_SP_MPEG1_L3_T			( 0x3105 )	// video/vnd.dlna.mpeg-tts 	//MPEG-4 Part2 Simple Profile with MPEG-1 Layer3 audio encapsulated in MPEG-2-TS with a valid timestamp field.
#define MP_VIDEO_MPEG4_P2_TS_SP_MPEG1_L3_ISO		( 0x3106 )	// video/mpeg	//MPEG-4 Part2 Simple Profile with MPEG-1 Layer3 audio encapsulated in MPEG-2-TS without a Timestamp field.
#define MP_VIDEO_MPEG4_P2_TS_SP_AC3					( 0x3107 )	// video/vnd.dlna.mpeg-tts	//MPEG-4 Part2 Simple Profile with AC3 multi-channel audio encapsulated in MPEG-2-TS with a zero timestamp field.
#define MP_VIDEO_MPEG4_P2_TS_SP_AC3_T				( 0x3108 )	// video/vnd.dlna.mpeg-tts 	//MPEG-4 Part2 Simple Profile with AC3 multi-channel audio encapsulated in MPEG-2-TS with a valid timestamp field.

#define MP_VIDEO_MPEG4_P2_TS_SP_AC3_ISO				( 0x3109 )	// video/mpeg	//MPEG-4 Part2 Simple Profile with AC3 multi-channel audio encapsulated in MPEG-2-TS without a Timestamp field.
#define MP_VIDEO_MPEG4_P2_TS_SP_MPEG2_L2			( 0x310a )	// video/vnd.dlna.mpeg-tts	//MPEG-4 Part2 Simple Profile with MPEG-1/2 Layer1/2 multi-channel audio encapsulated in MPEG-2-TS with a zero timestamp field.
#define MP_VIDEO_MPEG4_P2_TS_SP_MPEG2_L2_T			( 0x310b )	// video/vnd.dlna.mpeg-tts	//MPEG-4 Part2 Simple Profile with MPEG-1/2 Layer 1/2 multi-channel audio encapsulated in MPEG-2-TS with a valid imestamp field.
#define MP_VIDEO_MPEG4_P2_TS_SP_MPEG2_L2_ISO		( 0x310c )	// video/mpeg	//MPEG-4 Part2 Simple Profile with MPEG-1/2 Layer 1/2 multi-channel audio encapsulated in MPEG-2-TS without a Timestamp field.

#define MP_VIDEO_MPEG4_P2_ASF_SP_G726				( 0x3201 )	// video/x-ms-asf	//MPEG-4 Part2 Simple Profile with G.726 audio encapsulated inASF
#define MP_VIDEO_MPEG4_P2_MP4_SP_VGA_AAC			( 0x3301 )	// video/mp4	//MPEG-4 Part 2 Simple Profile Level 3+ with AAC audio encapsulated inMP4
#define MP_VIDEO_MPEG4_P2_MP4_SP_VGA_HEAAC			( 0x3302 )	// video/mp4	//MPEG-4 Part 2 Simple Profile Level 3+ with HEAAC audio encapsulated inMP4
#define MP_VIDEO_MPEG4_P2_MP4_ASP_AAC				( 0x3303 )	// video/mp4	//MPEG-4 Part2 Advanced Simple Profile with AAC LC encapsulated inMP4.
#define MP_VIDEO_MPEG4_P2_MP4_ASP_HEAAC				( 0x3304 )	// video/mp4	//MPEG-4 Part2 Advanced Simple Profile with HE AAC audio encapsulated inMP4.
#define MP_VIDEO_MPEG4_P2_MP4_ASP_HEAAC_MULT5		( 0x3305 )	// video/mp4	//MPEG-4 Part2 Advanced Simple Profile with HE AAC multi-channel audio encapsulated inMP4.

#define MP_VIDEO_MPEG4_P2_MP4_ASP_ATRAC3plus		( 0x3306 )	// video/mp4	//MPEG-4 Part2 Advanced Simple Profile with ATRAC3plus audio encapsulated in MP4
#define MP_VIDEO_MPEG4_P2_TS_ASP_AAC				( 0x3401 )	// video/vnd.dlna.mpeg-tts	//MPEG-4 Part2 Advanced Simple Profile with AAC LC audio encapsulated in MPEG-2-TS with a zero timestamp field.
#define MP_VIDEO_MPEG4_P2_TS_ASP_AAC_T				( 0x3402 )	// video/vnd.dlna.mpeg-tts	//MPEG-4 Part2 Advanced Simple Profile with AAC LC audio encapsulated in MPEG-2-TS with a valid timestamp field.
#define MP_VIDEO_MPEG4_P2_TS_ASP_AAC_ISO			( 0x3403 )	// video/mpeg	//MPEG-4 Part2 Advanced Simple Profile with AAC LC audio encapsulated in MPEG-2-TS without a Timestamp field.

#define MP_VIDEO_MPEG4_P2_TS_ASP_MPEG1_L3			( 0x3404 )	// video/vnd.dlna.mpeg-tts	//MPEG-4 Part2 Advanced Simple Profile with MPEG-1 Layer3 audio encapsulated in MPEG-2-TS with a zero timestamp field.
#define MP_VIDEO_MPEG4_P2_TS_ASP_MPEG1_L3_T			( 0x3405 )	// video/vnd.dlna.mpeg-tts	//MPEG-4 Part2 Advanced Simple Profile with MPEG-1 Layer3 audio encapsulated in MPEG-2-TS with a valid timestamp field.
#define MP_VIDEO_MPEG4_P2_TS_ASP_MPEG1_L3_ISO		( 0x3406 )	// video/mpeg	//MPEG-4 Part2 Advanced Simple Profile with MPEG-1 Layer3 audio encapsulated in MPEG-2-TS without a Timestamp field.
#define MP_VIDEO_MPEG4_P2_TS_ASP_AC3				( 0x3407 )	// video/vnd.dlna.mpeg-tts	//MPEG-4 Part2 Advanced Simple Profile with AC3 multi-channel audio encapsulated in MPEG-2-TS with a zero timestamp field

#define MP_VIDEO_MPEG4_P2_TS_ASP_AC3_T				( 0x3408 )	// video/vnd.dlna.mpeg-tts	//MPEG-4 Part2 Advanced Simple Profile with AC3 multi-channel audio encapsulated in MPEG-2-TS with a valid timestamp field.
#define MP_VIDEO_MPEG4_P2_TS_ASP_AC3_ISO			( 0x3409 )	// video/mpeg	//MPEG-4 Part2 Advanced Simple Profile with AC3 multi-channel audio encapsulated in MPEG-2-TS without a Timestamp field.
#define MP_VIDEO_MPEG4_P2_MP4_ASP_L5_SO_AAC			( 0x3501 )	// video/mp4	//MPEG-4 Part2 Advanced Simple Profile up to Level 5 with only Simple Object with AAC LC audio encapsulated in MP4.
#define MP_VIDEO_MPEG4_P2_MP4_ASP_L5_SO_HEAAC	 	( 0x3502 )	// video/mp4	//MPEG-4 Part2 Advanced Simple Profile up to Level 5 with only Simple Object with HE AAC audio encapsulated in MP4.

#define MP_VIDEO_MPEG4_P2_MP4_ASP_L5_SO_HEAAC_MULT5	( 0x3503 )	// video/mp4	//MPEG-4 Part2 Advanced Simple Profile up to Level 5 with only Simple Object with HE AAC multi-channel audio encapsulated in MP4.
#define MP_VIDEO_MPEG4_P2_ASF_ASP_L5_SO_G726		( 0x3504 )	// video/x-ms-asf	//MPEG-4 Part2 Advanced Simple Profile up to Level5 with only Simple Object with G.726 audio encapsulated in ASF.
#define MP_VIDEO_MPEG4_P2_MP4_ASP_L4_SO_AAC			( 0x3601 )	// video/mp4	//MPEG-4 Part2 Advanced Simple Profile up to Level 4 with only Simple Object with AAC LC audio encapsulated in MP4.
#define MP_VIDEO_MPEG4_P2_MP4_ASP_L4_SO_HEAAC		( 0x3602 )	// video/mp4	//MPEG-4 Part2 Advanced Simple Profile up to Level 4 with only Simple Object with HE AAC audio encapsulated in MP4.

#define MP_VIDEO_MPEG4_P2_MP4_ASP_L4_SO_HEAAC_MULT5	( 0x3603 )	// video/mp4	//MPEG-4 Part2 Advanced Simple Profile up to Level 4 with only Simple Object with HE AAC multi-channel audio encapsulated inMP4.
#define MP_VIDEO_MPEG4_P2_ASF_ASP_L4_SO_G726		( 0x3604 )	// video/x-ms-asf	//MPEG-4 Part2 Advanced Simple Profile up to Level4 with only Simple Object with G.726 audio encapsulated inASF. 
#define MP_VIDEO_MPEG4_H263_MP4_P0_L10_AAC 			( 0x3701 )	// video/3gpp 	//H263 Profile 0 Level 10 with AAC LC audio encapsulated inMP4. 
#define MP_VIDEO_MPEG4_H263_MP4_P0_L10_AAC_LTP		( 0x3702 )	// video/3gpp	//H263 Profile 0 Level 10 with AAC LTP audio encapsulated inMP4.
#define MP_VIDEO_MPEG4_H263_3GPP_P0_L10_AMR_WBplus	( 0x3703 )	// video/3gpp	//H263 Profile 0 Level 10 with AMR-WB+ audio encapsulated in3GPP

#define MP_VIDEO_MPEG4_P2_TS_CO_AC3					( 0x3801 )	// video/vnd.dlna.mpeg-tts	//MPEG-4 Part2 Core Profile with AC3 multi-channel audio encapsulated in MPEG-2-TS with a zero timestamp field.
#define MP_VIDEO_MPEG4_P2_TS_CO_AC3_T				( 0x3802 )	// video/vnd.dlna.mpeg-tts	//MPEG-4 Part2 Core Profile with AC3 multi-channel audio encapsulated in MPEG-2-TS with a valid timestamp field.
#define MP_VIDEO_MPEG4_P2_TS_CO_AC3_ISO				( 0x3803 )	// video/mpeg	//MPEG-4 Part2 Core Profile with AC3 multi-channel audio encapsulated in MPEG-2-TS without a Timestamp field.
#define MP_VIDEO_MPEG4_P2_TS_CO_MPEG2_L2			( 0x3804 )	// video/vnd.dlna.mpeg-tts	//MPEG-4 Part2 Core Profile with MPEG-1/2 Layer1/2 multi-channel audio encapsulated in MPEG-2 transport stream with a zero timestamp field.

#define MP_VIDEO_MPEG4_P2_TS_CO_MPEG2_L2_T			( 0x3805 )	// video/vnd.dlna.mpeg-tts	//MPEG-4 Part2 Core Profile with MPEG-1/2 Layer1/2 multi-channel audio encapsulated in MPEG-2-TS with a valid timestamp field.
#define MP_VIDEO_MPEG4_P2_TS_CO_MPEG2_L2_ISO		( 0x3806 )	// video/mpeg	//MPEG-4 Part2 Core Profile with MPEG-1/2 Layer1/2 multi-channel audio encapsulated in MPEG-2-TS without a Timestemp field.
#define MP_VIDEO_MPEG4_P2_3GPP_SP_L0B_AAC			( 0x3901 )	// video/3gpp video/mp4	//MPEG-4 Part 2 Simple Profile Level 0b with AAC audio encapsulated in 3GPP
#define MP_VIDEO_MPEG4_P2_3GPP_SP_L0B_AMR			( 0x3902 )	// video/3gpp video/mp4 	//MPEG-4 Part 2 Simple Profile Level 0b with AMR audio encapsulated in 3GPP
#define MP_VIDEO_MPEG4_H263_3GPP_P3_L10_AMR			( 0x3a01 )	// video/3gpp video/mp4	//MPEG-4 H.263 Profile 3 Level 10 with AMR audio encapsulated in 3GPP

//-----------------------------------------------------------------------------------------//
//MPEG4 Part10 AVC
//-----------------------------------------------------------------------------------------//
#define MP_VIDEO_AVC_TS_MP_SD_AAC_MULT5 			( 0x4101 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream main profile standard def with AAC audio with zero timestamp field
#define MP_VIDEO_AVC_TS_MP_SD_AAC_MULT5_T 			( 0x4102 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream main profile standard def with AAC audio with valid timestamp field
#define MP_VIDEO_AVC_TS_MP_SD_AAC_MULT5_ISO			( 0x4103 )	// video/mpeg	//AVC wrapped in MPEG-2 transport stream main profile standard def with AAC audio without a Timestamp field
#define MP_VIDEO_AVC_TS_MP_SD_HEAAC_L2				( 0x4104 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream main profile standard def with HEAAC L2 audio with zero timestamp field

#define MP_VIDEO_AVC_TS_MP_SD_HEAAC_L2_T 			( 0x4105 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream main profile standard def with HEAAC L2 audio with valid timestamp field
#define MP_VIDEO_AVC_TS_MP_SD_HEAAC_L2_ISO 			( 0x4106 )	// video/mpeg	//AVC wrapped in MPEG-2 transport stream main profile standard def with HEAAC L2 audio without a Timestamp field
#define MP_VIDEO_AVC_TS_MP_SD_MPEG1_L3 				( 0x4107 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream main profile standard def with MPEG-1 layer 3 audio with zero timestamp field
#define MP_VIDEO_AVC_TS_MP_SD_MPEG1_L3_T			( 0x4108 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream main profile standard def with MPEG-1 layer 3 audio with valid timestamp field

#define MP_VIDEO_AVC_TS_MP_SD_MPEG1_L3_ISO 			( 0x4109 )	// video/mpeg	//AVC wrapped in MPEG-2 transport stream main profile standard def with MPEG-1 layer 3 audio without a Timestamp field
#define MP_VIDEO_AVC_TS_MP_SD_AC3 					( 0x410a )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream main profile standard def with AC3 audio with zero timestamp field
#define MP_VIDEO_AVC_TS_MP_SD_AC3_T					( 0x410b )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream main profile standard def with AC3 audio with valid timestamp field
#define MP_VIDEO_AVC_TS_MP_SD_AC3_ISO				( 0x410c )	// video/mpeg	//AVC wrapped in MPEG-2 transport stream main profile standard def with AC3 audio without a Timestamp field

#define MP_VIDEO_AVC_TS_MP_SD_AAC_LTP				( 0x410d )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream main profile standard def with AAC LTP audio with zero timestamp field
#define MP_VIDEO_AVC_TS_MP_SD_AAC_LTP_T				( 0x410e )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream main profile standard def with AAC LTP audio with valid timestamp field
#define MP_VIDEO_AVC_TS_MP_SD_AAC_LTP_ISO			( 0x410f )	// video/mpeg	//AVC wrapped in MPEG-2 transport stream main profile standard def with AAC LTP audio without a Timestamp field
#define MP_VIDEO_AVC_TS_MP_SD_AAC_LTP_MULT5			( 0x4111 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream main profile standard def with AAC LTP audio with zero timestamp field

#define MP_VIDEO_AVC_TS_MP_SD_AAC_LTP_MULT5_T 		( 0x4111 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream main profile standarddef with AAC_LTP audio with valid timestamp field
#define MP_VIDEO_AVC_TS_MP_SD_AAC_LTP_MULT5_ISO 	( 0x4112 )	// video/mpeg	//AVC wrapped in MPEG-2 transport stream main profile standarddef with AAC_LTP audio without a Timestamp field
#define MP_VIDEO_AVC_TS_MP_SD_AAC_LTP_MULT7 		( 0x4113 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream main profile standarddef with AAC LTP audio with zero timestampfield
#define MP_VIDEO_AVC_TS_MP_SD_AAC_LTP_MULT7_T		( 0x4114 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream main profile standarddef with AAC LTP audio with valid timestamp field

#define MP_VIDEO_AVC_TS_MP_SD_AAC_LTP_MULT7_ISO 	( 0x4115 )	// video/mpeg	//AVC wrapped in MPEG-2 transport stream main profile standard def with AAC LTP audio without a Timestamp field
#define MP_VIDEO_AVC_TS_MP_SD_BSAC					( 0x4116 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream main profile standard def with BSAC audio with zero timestamp field
#define MP_VIDEO_AVC_TS_MP_SD_BSAC_T				( 0x4117 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream main profile standard def with BSAC audio with valid timestamp field
#define MP_VIDEO_AVC_TS_MP_SD_BSAC_ISO				( 0x4118 )	// video/mpeg	//AVC wrapped in MPEG-2 transport stream main profile standard def with BSAC audio without a Timestamp field

#define MP_VIDEO_AVC_MP4_MP_SD_AAC_MULT5 			( 0x4201 )	// video/mp4	//AVC main profile standard def with AAC audio encapsulated in MP4
#define MP_VIDEO_AVC_MP4_MP_SD_HEAAC_L2				( 0x4202 )	// video/mp4	//AVC main profile standard def with HEAAC L2 audio encapsulated in MP4
#define MP_VIDEO_AVC_MP4_MP_SD_MPEG1_L3				( 0x4203 )	// video/mp4	//AVC main profile standard def with MPEG-1 L3 audio encapsulated in MP4
#define MP_VIDEO_AVC_MP4_MP_SD_AC3					( 0x4204 )	// video/mp4	//AVC main profile standard def with AC3 audio encapsulated in MP4
#define MP_VIDEO_AVC_MP4_MP_SD_AAC_LTP				( 0x4205 )	// video/mp4	//AVC main profile standard def with AAC_LTP audio encapsulated in MP4
#define MP_VIDEO_AVC_MP4_MP_SD_AAC_LTP_MULT5		( 0x4206 )	// video/mp4	//AVC main profile standard def with AAC LTP audio encapsulated in MP4

#define MP_VIDEO_AVC_MP4_MP_SD_AAC_LTP_MULT7	 	( 0x4207 )	// video/mp4	//AVC main profile standard def with AAC LTP audio encapsulated in MP4
#define MP_VIDEO_AVC_MP4_MP_SD_ATRAC3plus 			( 0x4208 )	// video/mp4	//AVC main profile standard def with ATRAC3plus audio encapsulated in MP4
#define MP_VIDEO_AVC_MP4_BL_L3L_SD_AAC				( 0x4301 )	// video/mp4	//AVC baseline profile SD/VGA with AAC audio encapsulated in MP4
#define MP_VIDEO_AVC_MP4_BL_L3L_SD_HEAAC			( 0x4302 )	// video/mp4	//AVC baseline profile SD/VGA with HEAAC audio encapsulated in MP4
#define MP_VIDEO_AVC_MP4_BL_L3_SD_AAC				( 0x4303 )	// video/mp4	//AVC baseline profile standard def with AAC audio encapsulated in MP4
#define MP_VIDEO_AVC_MP4_MP_SD_BSAC					( 0x4304 )	// video/mp4	//AVC baseline profile standard def with BSAC audio encapsulated in MP4

#define MP_VIDEO_AVC_TS_BL_CIF30_AAC_MULT5			( 0x4401 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream baseline profile CIF30 with AAC audio with zero timestamp field
#define MP_VIDEO_AVC_TS_BL_CIF30_AAC_MULT5_T	 	( 0x4402 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream baseline profile CIF30 with AAC audio with valid timestamp field
#define MP_VIDEO_AVC_TS_BL_CIF30_AAC_MULT5_ISO		( 0x4403 )	// video/mpeg	//AVC wrapped in MPEG-2 transport stream baseline profile CIF30 with AAC audio without a Timestamp field

#define MP_VIDEO_AVC_TS_BL_CIF30_HEAAC_L2 			( 0x4404 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream baseline profile CIF30 with HEAAC audio with zero timestamp field
#define MP_VIDEO_AVC_TS_BL_CIF30_HEAAC_L2_T			( 0x4405 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream baseline profile CIF30 with HEAAC audio with valid timestamp field
#define MP_VIDEO_AVC_TS_BL_CIF30_HEAAC_L2_ISO		( 0x4406 )	// video/mpeg	//AVC wrapped in MPEG-2 transport stream baseline profile CIF30 with HEAAC audio without a Timestamp field

#define MP_VIDEO_AVC_TS_BL_CIF30_MPEG1_L3			( 0x4407 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream baseline profile CIF30 with MPEG-1 layer 3 audio with zero timestamp field
#define MP_VIDEO_AVC_TS_BL_CIF30_MPEG1_L3_T			( 0x4408 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream baseline profile CIF30 with MPEG-1 layer 3 audio with valid timestamp field
#define MP_VIDEO_AVC_TS_BL_CIF30_MPEG1_L3_ISO		( 0x4409 )	// video/mpeg	//AVC wrapped in MPEG-2 transport stream baseline profile CIF30 with MPEG-1 layer 3 audio without a Timestamp field
#define MP_VIDEO_AVC_TS_BL_CIF30_AC3				( 0x4410 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream baseline profile CIF30 with AC3 audio with zero timestamp field

#define MP_VIDEO_AVC_TS_BL_CIF30_AC3_T 				( 0x440a )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream baseline profile CIF30 with AC3 audio with valid timestamp field
#define MP_VIDEO_AVC_TS_BL_CIF30_AC3_ISO			( 0x440b )	// video/mpeg	//AVC wrapped in MPEG-2 transport stream baseline profile CIF30 with AC3 audio without a Timestamp field
#define MP_VIDEO_AVC_TS_BL_CIF30_AAC_LTP			( 0x440c )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream baseline profile CIF30 with AAC LTP audio with zero timestamp field
#define MP_VIDEO_AVC_TS_BL_CIF30_AAC_LTP_T			( 0x440d )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream baseline profile CIF30 with AAC LTP audio with valid timestamp field

#define MP_VIDEO_AVC_TS_BL_CIF30_AAC_LTP_ISO		( 0x440e )	// video/mpeg	//AVC wrapped in MPEG-2 transport stream baseline profile CIF30 with AAC LTP audio without a Timestamp field
#define MP_VIDEO_AVC_TS_BL_CIF30_AAC_LTP_MULT5 		( 0x440f )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream baseline profile CIF30 with AAC_LTP audio with zero timestamp field
#define MP_VIDEO_AVC_TS_BL_CIF30_AAC_LTP_MULT5_T 	( 0x4411 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream baseline profile CIF30 with AAC_LTP audio with valid timestamp field
#define MP_VIDEO_AVC_TS_BL_CIF30_AAC_LTP_MULT5_ISO	( 0x4412 )	// video/mpeg	//AVC wrapped in MPEG-2 transport stream baseline profile CIF30 with AAC_LTP audio without a Timestamp field

#define MP_VIDEO_AVC_TS_BL_CIF30_AAC_940			( 0x4413 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream baseline profile CIF30 with AAC audio with zero timestamp field
#define MP_VIDEO_AVC_TS_BL_CIF30_AAC_940_T			( 0x4414 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream baseline profile CIF30 with AAC audio with valid timestamp field
#define MP_VIDEO_AVC_TS_BL_CIF30_AAC_940_ISO		( 0x4415 )	// video/mpeg	//AVC wrapped in MPEG-2 transport stream baseline profile CIF30 with AAC audio without a Timestamp field
#define MP_VIDEO_AVC_MP4_BL_CIF30_AAC_MULT5			( 0x4501 )	// video/mp4	//AVC wrapped in MP4 baseline profile CIF30 with AAC audio 

#define MP_VIDEO_AVC_MP4_BL_CIF30_HEAAC_L2			( 0x4502 )	// video/mp4	//AVC wrapped in MP4 baseline profile CIF30 with HEAAC audio 
#define MP_VIDEO_AVC_MP4_BL_CIF30_MPEG1_L3			( 0x4503 )	// video/mp4	//AVC wrapped in MP4 baseline profile CIF30 with MPEG-1 layer3 audio
#define MP_VIDEO_AVC_MP4_BL_CIF30_AC3 				( 0x4504 )	// video/mp4	//AVC wrapped in MP4 baseline profile CIF30 with AC3 audio
#define MP_VIDEO_AVC_MP4_BL_CIF30_AAC_LTP			( 0x4505 )	// video/mp4	//AVC wrapped in MP4 baseline profile CIF30 with AAC LTP audio
#define MP_VIDEO_AVC_MP4_BL_CIF30_AAC_LTP_MULT5		( 0x4506 )	// video/mp4	//AVC wrapped in MP4 baseline profile CIF30 with AAC LTP audio

#define MP_VIDEO_AVC_MP4_BL_L2_CIF30_AAC 			( 0x4507 )	// video/mp4	//AVC wrapped in MP4 baseline profile CIF 30 with AAC audio
#define MP_VIDEO_AVC_MP4_BL_CIF30_BSAC				( 0x4508 )	// video/mp4	//AVC wrapped in MP4 baseline profile CIF30 with BSAC audio
#define MP_VIDEO_AVC_MP4_BL_CIF30_BSAC_MULT5		( 0x4509 )	// video/mp4	//AVC wrapped in MP4 baseline profile CIF30 with BSAC audio
#define MP_VIDEO_AVC_MP4_BL_CIF15_HEAAC				( 0x450a )	// video/mp4	//AVC wrapped in MP4 baseline profile CIF15 with HEAAC audio
#define MP_VIDEO_AVC_MP4_BL_CIF15_AMR				( 0x450b )	// video/mp4	//AVC wrapped in MP4 baseline profile CIF15 with AMR audio

#define MP_VIDEO_AVC_TS_MP_HD_AAC_MULT5				( 0x4601 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream main profile HD with AAC audio with zero timestamp field
#define MP_VIDEO_AVC_TS_MP_HD_AAC_MULT5_T 			( 0x4602 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream main profile HD with AAC audio with valid timestamp field
#define MP_VIDEO_AVC_TS_MP_HD_AAC_MULT5_ISO			( 0x4603 )	// video/mpeg	//AVC wrapped in MPEG-2 transport stream main profile HD with AAC audio without a Timestamp field
#define MP_VIDEO_AVC_TS_MP_HD_HEAAC_L2				( 0x4604 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream main profile HD with HEAAC audio with zero timestamp field

#define MP_VIDEO_AVC_TS_MP_HD_HEAAC_L2_T			( 0x4605 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream main profile HD with HEAAC audio with valid timestamp field
#define MP_VIDEO_AVC_TS_MP_HD_HEAAC_L2_ISO		 	( 0x4606 )	// video/mpeg	//AVC wrapped in MPEG-2 transport stream main profile HD with HEAAC audio without a Timestamp field
#define MP_VIDEO_AVC_TS_MP_HD_MPEG1_L3				( 0x4607 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream main profile HD with MPEG-1 layer 3 audio with zero timestamp field
#define MP_VIDEO_AVC_TS_MP_HD_MPEG1_L3_T 			( 0x4608 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream main profile HD with MPEG-1 layer 3 audio with valid timestamp field
#define MP_VIDEO_AVC_TS_MP_HD_MPEG1_L3_ISO			( 0x4609 )	// video/mpeg	//AVC wrapped in MPEG-2 transport stream main profile HD with MPEG-1 layer 3 audio without a Timestamp field

#define MP_VIDEO_AVC_TS_MP_HD_AC3 					( 0x460a )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream main profile HD with AC3 audio with zero timestamp field
#define MP_VIDEO_AVC_TS_MP_HD_AC3_T 				( 0x460b )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream main profile HD with AC3 audio with valid timestamp field
#define MP_VIDEO_AVC_TS_MP_HD_AC3_ISO				( 0x460c )	// video/mpeg	//AVC wrapped in MPEG-2 transport stream main profile HD with AC3 audio without a Timestamp field
#define MP_VIDEO_AVC_TS_MP_HD_AAC 					( 0x460d )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream main profile HD with AAC audio with zero timestamp field
#define MP_VIDEO_AVC_TS_MP_HD_AAC_T					( 0x460e )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream main profile HD with AAC audio with valid timestamp field

#define MP_VIDEO_AVC_TS_MP_HD_AAC_ISO				( 0x460f )	// video/mpeg	//AVC wrapped in MPEG-2 transport stream main profile HD with AAC audio without a Timestamp field
#define MP_VIDEO_AVC_TS_MP_HD_AAC_LTP 				( 0x4611 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream main profile HD with AAC LTP audio with zero timestamp field
#define MP_VIDEO_AVC_TS_MP_HD_AAC_LTP_T				( 0x4612 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream main profile HD with AAC LTP audio with valid timestamp field
#define MP_VIDEO_AVC_TS_MP_HD_AAC_LTP_ISO			( 0x4613 )	// video/mpeg	//AVC wrapped in MPEG-2 transport stream main profile HD with AAC LTP audio without a Timestamp field
#define MP_VIDEO_AVC_TS_MP_HD_AAC_LTP_MULT5			( 0x4614 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream main profile HD with AAC LTP audio with zero timestamp field

#define MP_VIDEO_AVC_TS_MP_HD_AAC_LTP_MULT5_T 		( 0x4615 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream main profile HD with AAC LTP audio with valid timestamp field
#define MP_VIDEO_AVC_TS_MP_HD_AAC_LTP_MULT5_ISO 	( 0x4616 )	// video/mpeg	//AVC wrapped in MPEG-2 transport stream main profile HD with AAC LTP audio without a Timestamp field
#define MP_VIDEO_AVC_TS_MP_HD_AAC_LTP_MULT7			( 0x4617 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream main profile HD with AAC LTP audio with zero timestamp field
#define MP_VIDEO_AVC_TS_MP_HD_AAC_LTP_MULT7_T		( 0x4618 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream main profile HD with AAC LTP audio with valid timestamp field
#define MP_VIDEO_AVC_TS_MP_HD_AAC_LTP_MULT7_ISO		( 0x4619 )	// video/mpeg	//AVC wrapped in MPEG-2 transport stream main profile HD with AAC LTP audio without a Timestamp field

#define MP_VIDEO_AVC_TS_BL_CIF15_AAC 				( 0x4701 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream transport stream baseline profile CIF15 with AAC audio with zero timestamp field
#define MP_VIDEO_AVC_TS_BL_CIF15_AAC_T 				( 0x4702 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream baseline profile CIF15 with AAC audio with valid timestamp field
#define MP_VIDEO_AVC_TS_BL_CIF15_AAC_ISO			( 0x4703 )	// video/mpeg	//AVC wrapped in MPEG-2 transport stream baseline profile CIF15 with AAC audio without a Timestamp field

#define MP_VIDEO_AVC_TS_BL_CIF15_AAC_540 			( 0x4704 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream baseline profile CIF15 with AAC audio with zero timestamp field
#define MP_VIDEO_AVC_TS_BL_CIF15_AAC_540_T 			( 0x4705 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream baseline profile CIF15 with AAC audio with valid timestamp field
#define MP_VIDEO_AVC_TS_BL_CIF15_AAC_540_ISO		( 0x4706 )	// video/mpeg	//AVC wrapped in MPEG-2 transport stream baseline profile CIF15 with AAC audio without a Timestamp field
#define MP_VIDEO_AVC_TS_BL_CIF15_AAC_LTP			( 0x4707 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream baseline profile CIF15 with AAC LTP audio with zero timestamp field

#define MP_VIDEO_AVC_TS_BL_CIF15_AAC_LTP_T 			( 0x4708 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream baseline profile CIF15 with AAC LTP audio with valid timestamp field
#define MP_VIDEO_AVC_TS_BL_CIF15_AAC_LTP_ISO		( 0x4709 )	// video/mpeg	//AVC wrapped in MPEG-2 transport stream baseline profile CIF15 with AAC LTP audio without a Timestamp field
#define MP_VIDEO_AVC_TS_BL_CIF15_BSAC 				( 0x4710 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream baseline profile CIF15 with BSAC audio with zero timestamp field
#define MP_VIDEO_AVC_TS_BL_CIF15_BSAC_T				( 0x4711 )	// video/vnd.dlna.mpeg-tts	//AVC wrapped in MPEG-2 transport stream baseline profile CIF15 with BSAC audio with valid timestamp field

#define MP_VIDEO_AVC_TS_BL_CIF15_BSAC_ISO			( 0x4712 )	// video/mpeg	//AVC wrapped in MPEG-2 transport stream transport stream baseline profile CIF15 with BSAC audio without a Timestamp field
#define MP_VIDEO_AVC_MP4_BL_CIF15_AAC				( 0x4801 )	// video/mp4	//AVC wrapped in MP4 baseline profile CIF15 with AAC audio
#define MP_VIDEO_AVC_MP4_BL_CIF15_AAC_520			( 0x4802 )	// video/mp4	//AVC wrapped in MP4 baseline profile CIF15 with AAC LC audio

#define MP_VIDEO_AVC_MP4_BL_CIF15_AAC_LTP 			( 0x4803 )	// video/mp4	//AVC wrapped in MP4 baseline profile CIF15 with AAC LTP audio 
#define MP_VIDEO_AVC_MP4_BL_CIF15_AAC_LTP_520		( 0x4804 )	// video/mp4	//AVC wrapped in MP4 baseline profile CIF15 with AAC LTP audio
#define MP_VIDEO_AVC_MP4_BL_CIF15_BSAC				( 0x4805 )	// video/mp4	//AVC wrapped in MP4 baseline profile CIF 15 with BSAC audio
#define MP_VIDEO_AVC_MP4_BL_L12_CIF15_HEAAC			( 0x4806 )	// video/mp4	//AVC wrapped in MP4 baseline profile CIF 15 with HEAAC L2 audio

#define MP_VIDEO_AVC_MP4_BL_L1B_QCIF15_HEAAC		( 0x4807 )	// video/mp4	//AVC wrapped in MP4 baseline profile QCIF with HEAAC L2 audio
#define MP_VIDEO_AVC_3GPP_BL_CIF30_AMR_WBplus		( 0x4901 )	// video/3gpp	//AVC wrapped in 3GPP baseline profile CIF30 with AMR-WB+ audio
#define MP_VIDEO_AVC_3GPP_BL_CIF15_AMR_WBplus		( 0x4902 )	// video/3gpp	//AVC wrapped in 3GPP baseline profile CIF15 with AMR-WB+ audio
#define MP_VIDEO_AVC_3GPP_BL_QCIF15_AAC				( 0x4903 )	// video/3gpp	//AVC wrapped in 3GPP baseline profile QCIF15 with AAC audio
#define MP_VIDEO_AVC_3GPP_BL_QCIF15_AAC_LTP			( 0x4904 )	// video/3gpp	//AVC wrapped in 3GPP  baseline profile QCIF15 with AAC LTP audio

#define MP_VIDEO_AVC_3GPP_BL_QCIF15_HEAAC 			( 0x4905 )	// video/3gpp video/mp4	//AVC wrapped in 3GPP baseline profile QCIF15 with HEAAC Audio
#define MP_VIDEO_AVC_3GPP_BL_QCIF15_AMR_WBplus	 	( 0x4906 )	// video/3gpp	//AVC wrapped in 3GPP baseline profile QCIF15 with AMR-WB+ audio
#define MP_VIDEO_AVC_3GPP_BL_QCIF15_AMR				( 0x4907 )	// video/3gpp video/mp4	//AVC wrapped in 3GPP baseline profile QCIF15 with AMR Audio

// 20120224
#define MP_VIDEO_AVC_TS_JP_AAC_T					( 0x4908 )	


//-----------------------------------------------------------------------------------------//
//WMV 9 Profiles ( asf )
//-----------------------------------------------------------------------------------------//
#define MP_VIDEO_WMVMED_BASE 	( 0x5001 )	// video/x-ms-wmv	//Medium resolution video (Main Profile at Medium Level) with baseline WMA audio  
#define MP_VIDEO_WMVMED_FULL 	( 0x5002 )	// video/x-ms-wmv	//Medium resolution video (Main Profile at Medium Level) with full WMA audio  
#define MP_VIDEO_WMVMED_PRO 	( 0x5003 )	// video/x-ms-wmv	//Medium resolution video (Main Profile at Medium Level) with WMA professional audio  
#define MP_VIDEO_WMVHIGH_FULL 	( 0x5004 )	// video/x-ms-wmv	//High resolution video (Main Profile at High Level) with full WMA audio  
#define MP_VIDEO_WMVHIGH_PRO 	( 0x5005 )	// video/x-ms-wmv	//High resolution video (Main Profile at High Level) with WMA professional audio  
#define MP_VIDEO_WMVHM_BASE 	( 0x5006 )	// video/x-ms-wmv	//HighMAT profile
#define MP_VIDEO_WMVSPLL_BASE 	( 0x5007 )	// video/x-ms-wmv	//Low resolution video (Simple Profile at Low Level) with baseline WMA audio  
#define MP_VIDEO_WMVSPML_BASE 	( 0x5008 )	// video/x-ms-wmv	//Low resolution video (Simple profile at Medium Level) with baseline WMA audio  
#define MP_VIDEO_WMVSPML_MP3	( 0x5009 )	// video/x-ms-wmv	//Low resolution video (Simple Profile at Medium Level) with MP3 audio  
		





#define MASK_MPEG2_SUBTYPE( a ) ( (a) & 0xf0 )
#define SUBTYPE_188		0xa0
#define SUBTYPE_192		0xb0


#define TIMESTAMP_188			0xa1
#define TIMESTAMP_NON_VALID_192 0xb1
#define TIMESTAMP_VALID_192		0xb2













#ifdef __cplusplus
}
#endif


#endif


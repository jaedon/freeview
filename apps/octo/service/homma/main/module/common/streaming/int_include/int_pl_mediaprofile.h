/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  MEDIAPROFILE.h.
	@brief

	Description:  									\n
	Module: PL/PROFILE					\n

	Copyright (c) 2012 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___PL_MediaProfile_H___
#define	___PL_MediaProfile_H___


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define HxMediaProfile_NAME_MAX_LEN				(96)
#define HxMediaProfile_USERAGENT_MAX_LEN		(512)


/*******************************************************************/
/********************      Enum     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Structs   *************************/
/*******************************************************************/
typedef struct _PL_MediaProfile_t		PL_MediaProfile_t;


typedef struct _PLMediaProfile_Item_t
{
	Handle_t							hItem;
	HINT32								nIndex;	/* profile에서 create하면서 부여 됨. */

	DxVideoCodec_e						eVideoCodec;
	DxAudioCodec_e						eAudioCodec;

	HINT32 								nAudioBitrateKbps;
	HINT32								nVideoBitrateKbps;
	HINT32								nSdVideoBitrateKbps;
	HINT32								nHdVideoBitrateKbps;
	HINT32								nFrameRate;
	HINT32								nScreenWidth;
	HINT32								nScreenHeight;

	HBOOL								bInterlaceMode;
	HBOOL								bUseTranscoder;

	DxAspectRatio_e						eVideoAspectRatio;

	/*video-codec extension*/
	DxVideoEncodeProfile_e			eVideoCodecProfile;
	DxVideoEncodeLevel_e			eVideoCodecLevel;

	/*need probe-info from live*/
	HBOOL	bUsingLiveAudioCodec;	//(true : bypass, false : using eAudioCodec)
	HBOOL	bUsingLiveAspectRatio;	//(true : bypass, false : using eVideoAspectRatio)

	HCHAR								*pszString;
} PLMediaProfile_Item_t;


/*******************************************************************/
/********************      Public Functions     **************************/
/*******************************************************************/
extern	HERROR	PL_MediaProfile_Init (void);
extern	HERROR	PL_MediaProfile_DeInit (void);

extern 	HERROR	PL_MediaProfile_Open( const HCHAR *device_name, const HCHAR *pszUserAgent, PL_MediaProfile_t **ret_pstProfile );
extern	HERROR	PL_MediaProfile_Close( const PL_MediaProfile_t *profile );
extern	HERROR	PL_MediaProfile_GetCount( HINT32 *ret_count );
extern	HERROR	PL_MediaProfile_ForEach(void (*func)(void* key , void* value , void* user_data), void *userdata);

extern 	HERROR	PL_MediaProfile_GetStartNumber( const PL_MediaProfile_t *profile, HINT32 *ret_start_number);
extern	HERROR	PL_MediaProfile_GetLastNumber( const PL_MediaProfile_t *profile, HINT32 *ret_last_number);
extern 	HERROR	PL_MediaProfile_GetIsInsertPCR( const PL_MediaProfile_t *profile, HBOOL *ret_insert_pcr);
extern	HERROR	PL_MediaProfile_GetGapChunkCount( const PL_MediaProfile_t *profile, HINT32 *ret_gap_chunk_count);
extern	HERROR	PL_MediaProfile_GetBitrateM3U8WaitChunkCount( const PL_MediaProfile_t *profile, HINT32 *ret_wait_chunk_count);
extern	HERROR	PL_MediaProfile_GetTSWaitChunkTime( const PL_MediaProfile_t *profile, HINT32 *ret_wait_chunk_time);
extern	HERROR	PL_MediaProfile_GetVideoChunkMultiply( const PL_MediaProfile_t *profile, HINT32 *ret_video_chunk_multiply);
extern	HERROR	PL_MediaProfile_GetAudioChunkMultiply( const PL_MediaProfile_t *profile, HINT32 *ret_audio_chunk_multiply);
extern	HERROR	PL_MediaProfile_GetList( const PL_MediaProfile_t *profile , HxList_t **ret_list );
extern	HERROR	PL_MediaProfile_GetItem( const PL_MediaProfile_t *profile, HINT32 item_number, PLMediaProfile_Item_t **ret_item);
extern	HERROR	PL_MediaProfile_GetItemCount( const PL_MediaProfile_t *profile, HINT32 *ret_count);

extern	HOM_Result_e	PL_MediaProfile_GetHandle( const PL_MediaProfile_t *pstProfile , Handle_t *ret_phProfileHandle);
extern	HOM_Result_e	PL_MediaProfile_GetByHandle( const  Handle_t hProfileHandle, PL_MediaProfile_t **ppstProfile);

extern	HERROR	PL_MediaProfile_Item_Get( const Handle_t profileitem_handle, PLMediaProfile_Item_t **ret_item);
extern	HERROR	PL_MediaProfile_Item_GetBandwidthKBitrate( const Handle_t profileitem_handle, HINT32 *ret_bandwidth_kbyte);
extern	HERROR	PL_MediaProfile_Item_GetStartNumber( const Handle_t profileitem_handle, HINT32 *ret_start_number);
extern	HERROR	PL_MediaProfile_Item_GetLastNumber( const Handle_t profileitem_handle, HINT32 *ret_last_number);
extern	HERROR	PL_MediaProfile_Item_GetIsInsertPCR( const Handle_t profileitem_handle, HBOOL *ret_insert_pcr);
extern	HERROR	PL_MediaProfile_Item_GetBitrateM3U8WaitChunkCount( const Handle_t profileitem_handle, HINT32 *ret_wait_chunk_count);
extern	HERROR	PL_MediaProfile_Item_GetTSWaitChunkTime( const Handle_t profileitem_handle, HINT32 *ret_wait_chunk_time);
extern	HERROR	PL_MediaProfile_Item_GetM3U8PrintChunkTime( const Handle_t profileitem_handle, HINT32 *ret_m3u8_print_time);
extern	HERROR	PL_MediaProfile_Item_GetGapChunkCount( const Handle_t profileitem_handle, HINT32 *ret_gap_chunk_count);
extern	HERROR	PL_MediaProfile_Item_GetVideoChunkSize( const Handle_t profileitem_handle, HINT32 *ret_video_chunk_size);
extern	HERROR	PL_MediaProfile_Item_GetAudioChunkSize( const Handle_t profileitem_handle, HINT32 *ret_audio_chunk_size);
extern	HERROR	PL_MediaProfile_Item_GetList( const Handle_t profileitem_handle , HxList_t **ret_list );
extern	HERROR	PL_MediaProfile_Item_GetItemCount( const Handle_t profileitem_handle, HINT32 *ret_count);





/*******************************************************************/
/********************      Debug     *******************************/
/*******************************************************************/
//#if defined(CONFIG_DEBUG)
//for debug
extern	HERROR	PL_MediaProfile_Dump (void *profile);
extern	void	PL_MediaProfile_ItemDump(void * data);
extern	void	PL_MediaProfile_ItemFullNameDump(void * data);
extern	void	PL_MediaProfile_ItemFullNameNumberDump(void * data);
//#endif


#endif	//___PL_MediaProfile_H___

/**
	@file     mgr_media.h
	@brief    file_name & simple comment.

	Description: \n
	Module:  \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2013/03/27		initiated					chmin\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__MGR_MEDIA_H__
#define	__MGR_MEDIA_H__

#define	____INSIDE_MGR_MEDIA_H____

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*		Util, System
 */
#include <linkedlist.h>
#include <octo_common.h>

/*		TV Service
 */
#include <isosvc.h>
#include <svc_fs.h>

/*		TV Manager
 */
#include <mgr_pvr_action.h>


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

// VideoList 에서의 ListItem 표시
#define	FLAG_LOCK						0x0001
#define	FLAG_SCRAMBLE					0x0002
#define	FLAG_RECORD						0x0004
#define	FLAG_PLAY						0x0008
#define	FLAG_NEW_WATCH					0x0010
#define	FLAG_REC_FAILED					0x0020
#define	FLAG_RADIO						0x0040
#define	FLAG_DRM_EXPIRATION				0x0080
#define	FLAG_DRM_EXPIRED				0x0100
#define	FLAG_GUIDANCE					0x0200
#define	FLAG_ENCRYPTED					0x0400
#define	FLAG_PROTECTED					0x0800
#define	FLAG_HAVE_DRM					0x1000
#define	FLAG_DELETE						0x2000 //for digiturk
#define	FLAG_AUTO_DELETED				0x4000

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
enum
{
	eMEVT_MEDIA			= eMEVT_MEDIA_START,

	/* - Description : when other module calls media player explicitly ap_mplayer_stop()
		   - Parameters Usage :
			   handle : N.A
			   p1 : ulSlot
			   p2 : N.A
			   p3 : N.A */
	eMEVT_MEDIA_MPLAYER_EXPLICIT_STOP,

	eMEVT_MEDIA_SEND_DRM_INFORMATION,

	eMEVT_MEDIA_UPDATE_COPYCONTROL_INFO,

	eMEVT_MEDIA_OUTPUT_CONTROL,

	eMEVT_MEDIA_DVB_SUBTITLE_START,

	eMEVT_MEDIA_DVB_SUBTITLE_STOP,

	eMEVT_MEDIA_MAX
};

typedef struct
{
	HUINT32			attrib;								// file attributes
	HUINT8			path[FS_MAX_FILE_NAME_LEN];			// file path
	HUINT8			name[FS_MAX_FILE_NAME_LEN];			// file name except for preceeding path, 이슈가 발견되어 256으로 늘립니다.
	HUINT8			select;								// 1 if selected
	HUINT16			subItemNum;							// sub item num
	HBOOL			isMountTop;							// 1 if it is top-mounted position
	HUINT32			duration;							// MP3 duration
	HUINT8			brokenFile;
	HUINT8			ucMediaType;
	HUINT32			privateAttrib;						// private attribute (eg. series folder, media container)
#if defined(CONFIG_MW_MM_PVR) || defined(CONFIG_MW_MM_MEDIA_XVID)
	HUINT8			ucDoneReadInfo;							// 한 번 읽은 후 다시 읽지 않도록 하기 위함.
	HUINT32			ulContentID;							// ContentID for Videolist
	UNIXTIME		ulStartTime;							// Recording Start Time
	HUINT16			usState;								// FLAG_LOCK, FLAG_SCRAMBLE, FLAG_RECORD, FLAG_PLAY, FLAG_NEW_WATCH
#endif
#if defined(CONFIG_MW_MM_PVR)
	HINT32			nChNum;									// channel number for sorting (only recorded files)
	HUINT8			ucValidThumbImage;						// Thumb 이미지가 존재하는가
	HUINT32			*pulThumbImage;							// Thumb 이미지
#endif
#if defined(CONFIG_MW_MM_MEDIA_PHOTO_RESIZE_ROTATE)
	HUINT32			nDegree;
#endif
} MgrMedia_Item_t;

#define MGR_MEDIA_DRMINFO_KEYSIZE 128
#define MGR_MEDIA_DRMINFO_KEYID_SIZE 16

typedef enum
{
	MGR_MEDIA_DRMTYPE_NONE,
	MGR_MEDIA_DRMTYPE_PLAYREADY,
	MGR_MEDIA_DRMTYPE_WMDRM,
	MGR_MEDIA_DRMTYPE_VERIMATRIX,
	MGR_MEDIA_DRMTYPE_NAGRAPRM,
	MGR_MEDIA_DRMTYPE_MARLIN,
} eMgrMediaDRMType_e;

typedef struct
{
	eMgrMediaDRMType_e	eDRMType;
	eMediaEncryptType_t	eEncryptionType;
	eMEDIA_StreamType_t eStreamType;
	HUINT32             ulTransportSessionId;
	HUINT32             hPlayBackHandle;
	HUINT16				usEmi;
#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
	HUINT32				ulKeyHandle;
	HUINT32				ulKeyIdSize;
	HUINT8				aucKeyId[MGR_MEDIA_DRMINFO_KEYID_SIZE];
#else
	HUINT32				ulKeySize;
	HUINT8				ucEncryptKey[MGR_MEDIA_DRMINFO_KEYSIZE];
#endif
	HUINT32				ulUserDataSize;
	void				*pvUserData;

} MgrMedia_DrmInformation_t;

// ================= Functions ======================== //
void			MGR_MEDIA_SetVideoLoadedPath (HUINT8 *pPath);
HUINT8*			MGR_MEDIA_GetVideoLoadedPath (void);
void			MGR_MEDIA_FreeMediaItem (void *pvContents);

// ap_media_video_thumbnail.c
#if defined(CONFIG_MW_MM_PVR)
BUS_Callback_t	MGR_MakeThumbnail_GetProc (void);
HBOOL			MGR_MakeThumbnail_IsAvailable (void);
HERROR			MGR_MakeThumbnail_Start (void);
HERROR			MGR_MakeThumbnail_Stop (void);
#endif


#include "_mgr_mplayer.h"
#undef	____INSIDE_MGR_MEDIA_H____

#endif /* __MGR_MEDIA_H__ */


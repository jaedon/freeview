/* $Header: $ */

#ifndef __DLIB_CONFIG_HMX4KBBC_H__
#define __DLIB_CONFIG_HMX4KBBC_H__

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_____INSIDE_DLIB_CONFIG_H_____)
	#error ("!!! _dlib_config_hmx4kbbc.h shall be included only in the dlib_config.h !!!\n");
#endif

/*		DB	>>>
 */

#undef DxMAX_PRV_NAME_LEN
#undef DxMAX_NET_NAME_LEN
#define DxMAX_PRV_NAME_LEN					31 /* spec 20 * 1.5 (jis(2bytes)->utf8(3bytes)) + 1*/
#define DxMAX_NET_NAME_LEN					31 /* spec 20 * 1.5 (jis(2bytes)->utf8(3bytes)) + 1*/
/*		DB End 		*/

/* 		PVR Recordings	>>>
 */
#undef DxPVR_NUM_BOOKMARK
#define DxPVR_NUM_BOOKMARK					32

#undef DxPVR_THUMBNAIL_WIDTH
#undef DxPVR_THUMBNAIL_HEIGHT
#define	DxPVR_THUMBNAIL_WIDTH					140
#define	DxPVR_THUMBNAIL_HEIGHT				78
/* 		PVR Recordings End	*/

/* Default DMS DB Path */
#undef DxMW_DMS_DEFAULT_DB_PATH
#undef DxMW_DMS_DEFAULT_PATH
#define DxMW_DMS_DEFAULT_DB_PATH					"/tmp/dms/dms_cds.db"
#define DxMW_DMS_DEFAULT_PATH						"/tmp/dms"

/* Default Scan Filter - OCTO2.0 New Define*/
#undef DxMW_DMS_DEFAULT_SCANFILTER_VIDEO
#undef DxMW_DMS_DEFAULT_SCANFILTER_PHOTO
#undef DxMW_DMS_DEFAULT_SCANFILTER_MUSIC
#undef DxMW_DMS_DEFAULT_SCANFILTER_RECORD
#define DxMW_DMS_DEFAULT_SCANFILTER_VIDEO			"ts;tts;mpg;"
#define DxMW_DMS_DEFAULT_SCANFILTER_PHOTO			""
#define DxMW_DMS_DEFAULT_SCANFILTER_MUSIC			""
#define DxMW_DMS_DEFAULT_SCANFILTER_RECORD		""

/* Default Exception Path */
#undef DxMW_DMS_DEFAULT_EXCEPTIONPATH_1
#undef DxMW_DMS_DEFAULT_EXCEPTIONPATH_2
#undef DxMW_DMS_DEFAULT_EXCEPTIONPATH_3
#define DxMW_DMS_DEFAULT_EXCEPTIONPATH_1			"/media/drive1/.tsr"
#define DxMW_DMS_DEFAULT_EXCEPTIONPATH_2
#define DxMW_DMS_DEFAULT_EXCEPTIONPATH_3

/* Thumbnail */
#undef DxMW_DMS_DEFAULT_THUMBNAIL_PATH
#define DxMW_DMS_DEFAULT_THUMBNAIL_PATH			"/tmp/dms/thumbnail"

/* Live Streaming */
#undef DxMW_DMS_LIVESTREAMING_PREFIX_CHANNEL
#define DxMW_DMS_LIVESTREAMING_PREFIX_CHANNEL		"/tuner/HCHANNEL:"

#undef DxMW_DMS_UPLOAD_PATH
#undef DxMW_DMS_UPLOAD_TEMP_PATH
#define DxMW_DMS_UPLOAD_PATH						"/tmp/dms/tmp"
#define DxMW_DMS_UPLOAD_TEMP_PATH					"/tmp/dms/tmp"

#undef DxMW_DMS_BACKUP_CHECK_DB_NAME1
#undef DxMW_DMS_BACKUP_CHECK_DB_NAME2
#define DxMW_DMS_BACKUP_CHECK_DB_NAME1			".dms1.db"
#define DxMW_DMS_BACKUP_CHECK_DB_NAME2			".dms2.db"

/* EPG */
#undef DxEPG_EVENT_CELL_MIN_NUM
#undef DxEPG_EXT_EVENT_CELL_MIN_NUM
#undef DxEPG_EXT_TEXT_MIN_NUM
#define DxEPG_EVENT_CELL_MIN_NUM							(50)
#define DxEPG_EXT_EVENT_CELL_MIN_NUM						(5)
#define DxEPG_EXT_TEXT_MIN_NUM							(200)

// i-plate 등에서 한 번에 500개 이상 읽어가는 경우, 이벤트가 전혀 안나오는 것처럼 보인다. 그래서 최소한 XXX 개(방송환경에 따라 맞는 수치를 찾아야함) 이상인지를 결정해야함
#undef DxEPG_EVENT_CELL_NUM
#undef DxEPG_EXT_EVENT_CELL_NUM
#undef DxEPG_EXT_TEXT_NUM
#undef DxEPG_SHORT_EVENT_NAME
#undef DxEPG_EVENT_TEXT
#undef DxEPG_EXT_TEXT_LEN
#define DxEPG_EVENT_CELL_NUM								(800)	/* full size 확인해야함 */
#define DxEPG_EXT_EVENT_CELL_NUM							(20)		/* Guidance를 Ext 와 함께 운영함. */
#define DxEPG_EXT_TEXT_NUM								(DxEPG_EVENT_CELL_NUM * 5)	/* 이벤트당 Name 1, Text 3, Ext를 위해서는 따로 추가할 필요가 없는 것으로 암.  1개는 혹시 몰라서 */
#define DxEPG_SHORT_EVENT_NAME							(128)	/* 128 * 1 */
#define DxEPG_EVENT_TEXT									(384)	/* 128 * 3 */
#define DxEPG_EXT_TEXT_LEN								(128)	/* Size per one text */

#undef DxEPG_SHORT_TEXT_MAX
#undef DxEPG_EXT_INFO_MAX
#undef DxEPG_EXT_ITEM_MAX
#undef DxEPG_PARENTAL_RATING_MAX
#undef DxEPG_LINKAGE_INFO_MAX
#undef DxEPG_GUIDANCE_INFO_MAX
#define DxEPG_SHORT_TEXT_MAX								(1)
#define DxEPG_EXT_INFO_MAX								(1)
#define DxEPG_EXT_ITEM_MAX								(16)	// 최대 16개 ARIB TR-B15 32.1.2.1
#define DxEPG_PARENTAL_RATING_MAX 						(1)
#define DxEPG_LINKAGE_INFO_MAX							(2)
#define DxEPG_GUIDANCE_INFO_MAX							(1)

#undef DxEPG_CONTENTS_MAX
#define DxEPG_CONTENTS_MAX									7

#undef DxEPG_FS_RAWEIT_SAVE_FLASH_PATH
#define DxEPG_FS_RAWEIT_SAVE_FLASH_PATH				CONFIG_PRODUCT_BACKUPDATA_DIR "/dvbepg/"

#undef DxSVC_FS_PVR_RESERVED_SIZE_KB
#define DxSVC_FS_PVR_RESERVED_SIZE_KB 		(1*1024*1024)	// tsr tmp file buffer (1M x KB = 1G)

#ifdef __cplusplus
}
#endif


#endif // __DLIB_CONFIG_JCNGS_H__



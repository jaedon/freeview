/* $Header: $ */

#ifndef __DLIB_CONFIG_BASE_H__
#define __DLIB_CONFIG_BASE_H__

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_____INSIDE_DLIB_CONFIG_H_____)
	#error ("!!! _dlib_config_base.h shall be included only in the dlib_config.h !!!\n");
#endif

/*		DB
 */
#define DxMAX_TS_NUM						1000
#define DxMAX_PRV_NUM						500
#define DxMAX_NET_NUM						100
#define DxMAX_BOUQ_NUM						100
#define DxMAX_PRV_NAME_LEN					22
#define DxMAX_NET_NAME_LEN					22
#define DxMAX_BOUQ_NAME_LEN				36
#define DxMAX_GROUP_NAME_LEN				36
#define DxGROUP_LIST_BUF_LEN				64
#define DxBOUQUET_LIST_BUF_LEN				64

//	FreeSat
#define DxSTART_FREESAT_GENRE_ID			1000
#define DxFSAT_SVCID_INVALID				0xffff
#define DxMAX_FSAT_SVC_SHORT_NAME_LEN		(27) // (8+1) * 3
// FreeSat End
#define DxMAX_DEFAULT_AUTHORITY_LEN			33
#define DxMAX_GUIDANCE_INFO_LEN				256
#define DxMAX_T_REGION_NAME_LEN				36 // LangCode(3) + Real Len(32) + Last(1)
#define DxMAX_T_REGION_NUM					9
#define DxMAX_URI_PREFIX_INFO_LEN				256

#define DxMAX_TS_NAME_LEN					31 /* 20 spec * 1.5(code convert buffer) + 1	(30+1)	 only for ARIB Digital Terrestrial */
#define DxMAX_AFFILIATION_ID_NUM			16

// Multilingual bouquet name
#define DxMAX_BOUQ_LANG_LEN					4
#define DxMAX_MULTI_BOUQ_NAME_NUM			8

/*		DB End		*/



// For PVR Recordings

#define DxPVR_NUM_BOOKMARK					4

#define	DxPVR_THUMBNAIL_WIDTH					270
#define	DxPVR_THUMBNAIL_HEIGHT				152

#define DxMAX_GUIDANCE_DATA_LEN				255

/* Default DMS DB Path */
#define DxMW_DMS_DEFAULT_DB_PATH					"/mnt/hd2/dms_cds.db"
#define DxMW_DMS_DEFAULT_PATH						"/mnt/hd2"

/* Default Scan Filter - OCTO2.0 New Define*/
#define DxMW_DMS_DEFAULT_SCANFILTER_VIDEO			"asf;wmv;divx;avi;mp4;m4v;mkv;ts;trp;tp;vob;mpg;mpeg;3gp;mov;flv;"
#define DxMW_DMS_DEFAULT_SCANFILTER_PHOTO			"jpeg;jpg;png;gif;bmp;"
#define DxMW_DMS_DEFAULT_SCANFILTER_MUSIC			"mp3;pcm;wma;m4a;wav;flac;"
#define DxMW_DMS_DEFAULT_SCANFILTER_RECORD		"hjm;"

/* Default Exception Path */
#define DxMW_DMS_DEFAULT_EXCEPTIONPATH_1			"/mnt/hd2/.tsr"
#define DxMW_DMS_DEFAULT_EXCEPTIONPATH_2
#define DxMW_DMS_DEFAULT_EXCEPTIONPATH_3

#define DxMW_DMS_DEFAULT_THUMBNAIL_PATH			NULL

#define DxMW_DMS_LIVESTREAMING_PREFIX_CHANNEL		NULL

#define DxMW_DMS_UPLOAD_PATH						"/mnt/hd2/Upload"
#define DxMW_DMS_UPLOAD_TEMP_PATH					"/mnt/hd2/Upload"

#define DxMW_DMS_BACKUP_CHECK_DB_NAME1			NULL
#define DxMW_DMS_BACKUP_CHECK_DB_NAME2			NULL

/* Default Model Info */
#define DxMW_DMS_DEFAULT_MODEL_DESCRIPTION		"HUMAX Set-Top Box"
#define DxMW_DMS_DEFAULT_MODEL_URL				"www.humaxdigital.com/"
#define DxMW_DMS_DEFAULT_MODEL_NUMBER				"undefined"
#define DxMW_DMS_DEFAULT_SERIAL_NUMBER			"undefined"

#define DxMEDIA_CERT_PATH							NULL
#define DxMEDIA_KEY_PATH							NULL

/* EPG */
#define DxEPG_EVENT_CELL_MIN_NUM							(500)
#define DxEPG_EXT_EVENT_CELL_MIN_NUM						(200)
#define DxEPG_EXT_TEXT_MIN_NUM							(400)

#define DxEPG_EVENT_CELL_NUM								(1200)
#define DxEPG_EXT_EVENT_CELL_NUM							(1200)	/* Guidance를 Ext 와 함께 운영함. */
#define DxEPG_EXT_TEXT_NUM								(DxEPG_EVENT_CELL_NUM * 5)	/* 이벤트당 Name 1, Text 3, Ext를 위해서는 따로 추가할 필요가 없는 것으로 암.  1개는 혹시 몰라서 */
#define DxEPG_SHORT_EVENT_NAME							(128)	/* 128 * 1 */
#define DxEPG_EVENT_TEXT									(384)	/* 128 * 3 */
#define DxEPG_EXT_TEXT_LEN								(128)	/* Size per one text */
#define DxEPG_SHORT_TEXT_MAX								(1)
#define DxEPG_EXT_INFO_MAX								(1)
#define DxEPG_EXT_ITEM_MAX								(4) // Need Check
#define DxEPG_PARENTAL_RATING_MAX 						(1)
#define DxEPG_LINKAGE_INFO_MAX							(2)
#define DxEPG_GUIDANCE_INFO_MAX							(1)

#define DxEPG_CONTENTS_MAX								1

// epg data 를 위한 mount path 를 이곳으로.. , model 에 맞게 수정 가능하도록..
#define	EPG_HUMAX_MOUNT_PATH		"/mnt/hd1"	// default..

#define DxEPG_FS_RAWEIT_SAVE_FLASH_PATH					CONFIG_PRODUCT_DATA_DIR "/dvbepg/"

// 여러 사양이 겹칠 수 있으니 여기서 값은 큰 값 순서대로 선택되게 배열하자.
// (UPC 이고 Delayed Recording이면 큰 쪽이 우선되어야 할 것임)
// CONFIG_MW_MM_DELAYED_REC
#define DxSVC_FS_PVR_RESERVED_SIZE_KB						(40*1024*1024)	// tsr tmp file buffer (40M x KB = 40G)

#if defined(CONFIG_PROD_ICORDPRO) || defined(CONFIG_PROD_HDNEO)
#define DxSVC_FS_PVR_SAFE_AREA_SIZE_KB					(34*2*1024)			//(34M) * 2(2rec) = 68MB : hdd가 완전 full 상태가 되면 오동작의 우려가 있으므로 약간의 여유 공간은 둬야 (HD기준 1시간에 4GB, 30초면 34M)
#else
#define DxSVC_FS_PVR_SAFE_AREA_SIZE_KB					(34*4*1024)			//(34M) * 4(4rec) = 136MB : hdd가 완전 full 상태가 되면 오동작의 우려가 있으므로 약간의 여유 공간은 둬야 (HD기준 1시간에 4GB, 30초면 34M)
#endif

#define DxSVC_FS_PVR_MIN_SIZE_KB			(2*1024*1024)		// Minimum Storage size for PVR  (default : 2GB)
#define DxSVC_FS_PVR_USB_FLASH_MAX_RESERVED_SIZE_KB	(4*1024*1024) 	// Maximum reserved(TSR) size of USB Flash (default : 4GB)
#define DxSVC_FS_PVR_USB_FLASH_RESERVED_SIZE_RATIO	0.33		// A ratio of the USB Flash reserved size (default : 1/3)

#define DxSVC_FS_PVR_MIN_HDD_SIZE_KB	 	62500000		//(64*1000*1000*1000/1024)	==> Mininum hdd size for assigning fixed TSR size(40GB), default : 64GB

// Related Content Table
#define	DxRCT_MAX_MEDIA_URI						256
#define	DxRCT_MAX_PROMOTIONAL_TEXT				256
#define	DxRCT_MAX_IMAGE_ICON_TYPE_CHART			256
#define	DxRCT_MAX_IMAGE_ICON_URL_LEN				256
#define	DxRCT_MAX_SHORT_EVENT_NAME				256
#define	DxRCT_MAX_SHORT_EVENT_TEXT				256

#define 	DxRCT_IMAGE_NAME	"tmp/RctImageIcon"
#define 	DxRCT_IMAGE_PATH_LEN 	512
#define		DxNCD_MAX_MESSAGE_LEN				(512)		/* 실제 message 와 date 정보를 포함하기에 512로 늘리자 */


//Target Region Name
#define DxTRD_MAX_CONFLICT_SVC_NAME				128
#define DxTRD_MAX_T_REGION_NUM					9

#ifdef __cplusplus
}
#endif


#endif // __DLIB_CONFIG_BASE_H__



#ifndef __MHEG_PARAM_H__
#define __MHEG_PARAM_H__

/*************************************************************
 * @file		mheg_param.h
 * @date		2013/11/16
 * @brief		Humax SnT Mheg Player
 *
 * http://www.humaxdigital.com
 *************************************************************/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	HIGHWORD(longWord)	((HUINT16)( sizeof(longWord) == 4 ? ((HUINT16)((0xFFFF0000 & longWord) >> 16)) : 0 ))
#define	LOWWORD(longWord)	((HUINT16)( sizeof(longWord) == 4 ? ((HUINT16)(0x0000FFFF & longWord)) : longWord ))


/** @brief ISO/IEC 13818-1에서 정의한 Stream type assignments. And there're more value.(0x07-0x0e) */
#define	ISO_VIDEO_1				0x01
#define	ISO_VIDEO_2				0x02
#define	ISO_AUDIO_1				0x03
#define	ISO_AUDIO_2				0x04
#define	ISO_PRIVDATA_1			0x05
#define	ISO_PRIVDATA_2			0x06
#define	ISO_DSMCC				0x08
#define	ISO_DSMCC_U_N			0x0B
#define	ISO_AUDIO_AAC			0x0f	/** < ISO/IEC 13818-7 Audio with ADTS transport syntax (AAC) */
#define	ISO_AUDIO_4				0x11	/** < MPEG4 High Efficiency AAC audio coding */
#define	ISO_VIDEO_4				0x1B	/** < H264/AVC video coding */
//#define	ISO_PES					0x06 /* MPEG-2 Packetized Elementary Stream packets containing private data ARIB caption/super */
#define	ISO_DATA					0x0D /* ISO/IEC 13818-6 typeD ARIB */
#define	ISO_AUDIO_AC3				0x81

#define	PID_NULL					0x2000
#define	PID_CURRENT					0x2001

#define	UK_DTT_MHEG_DATA_BROADCAST_ID		(0x0106)
#define	UK_DTT_MHEG_APPLICATION_CODE1		(0x0101)
#define	UK_DTT_MHEG_APPLICATION_CODE2		(0x0505)		// Added in D-Book 6

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
enum
{
	PAT_TABLE_ID                             = 0x00,
	CAT_TABLE_ID                             = 0x01,
	PMT_TABLE_ID                             = 0x02,
	TSDT_TABLE_ID                            = 0x03,
	NIT_ACTUAL_TABLE_ID                      = 0x40,
	NIT_OTHER_TABLE_ID                       = 0x41,
	SDT_ACTUAL_TABLE_ID                      = 0x42,
	SDT_OTHER_TABLE_ID                       = 0x46,
	EIT_ACTUAL_TABLE_ID                      = 0x4e,
	EIT_OTHER_TABLE_ID                       = 0x4f,
	EIT_ACTUAL_SCHEDULE_TABLE_ID_0           = 0x50,
	EIT_ACTUAL_SCHEDULE_TABLE_ID_1           = 0x51,
	EIT_ACTUAL_SCHEDULE_TABLE_ID_2           = 0x52,
	EIT_ACTUAL_SCHEDULE_TABLE_ID_3           = 0x53,
	EIT_ACTUAL_SCHEDULE_TABLE_ID_4           = 0x54,
	EIT_ACTUAL_SCHEDULE_TABLE_ID_5           = 0x55,
	EIT_ACTUAL_SCHEDULE_TABLE_ID_6           = 0x56,
	EIT_ACTUAL_SCHEDULE_TABLE_ID_7           = 0x57,
	EIT_ACTUAL_SCHEDULE_TABLE_ID_8           = 0x58,
	EIT_ACTUAL_SCHEDULE_TABLE_ID_9           = 0x59,
	EIT_ACTUAL_SCHEDULE_TABLE_ID_10          = 0x5a,
	EIT_ACTUAL_SCHEDULE_TABLE_ID_11          = 0x5b,
	EIT_ACTUAL_SCHEDULE_TABLE_ID_12          = 0x5c,
	EIT_ACTUAL_SCHEDULE_TABLE_ID_13          = 0x5d,
	EIT_ACTUAL_SCHEDULE_TABLE_ID_14          = 0x5e,
	EIT_ACTUAL_SCHEDULE_TABLE_ID_15          = 0x5f,
	EIT_OTHER_SCHEDULE_TABLE_ID_0            = 0x60,
	EIT_OTHER_SCHEDULE_TABLE_ID_1            = 0x61,
	EIT_OTHER_SCHEDULE_TABLE_ID_2            = 0x62,
	EIT_OTHER_SCHEDULE_TABLE_ID_3            = 0x63,
	EIT_OTHER_SCHEDULE_TABLE_ID_4            = 0x64,
	EIT_OTHER_SCHEDULE_TABLE_ID_5            = 0x65,
	EIT_OTHER_SCHEDULE_TABLE_ID_6            = 0x66,
	EIT_OTHER_SCHEDULE_TABLE_ID_7            = 0x67,
	EIT_OTHER_SCHEDULE_TABLE_ID_8            = 0x68,
	EIT_OTHER_SCHEDULE_TABLE_ID_9            = 0x69,
	EIT_OTHER_SCHEDULE_TABLE_ID_10           = 0x6a,
	EIT_OTHER_SCHEDULE_TABLE_ID_11           = 0x6b,
	EIT_OTHER_SCHEDULE_TABLE_ID_12           = 0x6c,
	EIT_OTHER_SCHEDULE_TABLE_ID_13           = 0x6d,
	EIT_OTHER_SCHEDULE_TABLE_ID_14           = 0x6e,
	EIT_OTHER_SCHEDULE_TABLE_ID_15           = 0x6f,

	BAT_TABLE_ID                             = 0x4a,
	RST_TABLE_ID                             = 0x71,
	TDT_TABLE_ID                             = 0x70,
	TOT_TABLE_ID                             = 0x73,
	AIT_TABLE_ID                             = 0x74,

	DIT_ARIB_TABLE_ID			= 0x7E,	/* Already defined DIT_TABLE_ID */
	SIT_ARIB_TABLE_ID			= 0x7F,

	DIT_TABLE_ID                             = 0xc5,
	DIT_TABLE_ID_EXT                     = 0xc5c5,
	DGT_TABLE_ID                             = 0xc6,
	DBT_TABLE_ID                             = 0xc7,

	DSI_TABLE_ID                             = 0x3b,		/* dsm-cc data carousel */
	DII_TABLE_ID				= 0x3b,		/* dsm-cc data carousel */
	DDB_TABLE_ID                             = 0x3c,		/* dsm-cc data carousel */
	DESC_TABLE_ID							 = 0x3d,		/* dsm-cc data carousel stream description*/
	UNT_TABLE_ID                             = 0x4b,		/* dvb-ssu enhanced profile */
	RCT_TABLE_ID                             = 0x76,    /* related content table TS 102-323 & 300-468 */
	CIT_TABLE_ID                             = 0xa0,
	CPT_TABLE_ID                             = 0xa1,
	SGT_TABLE_ID                             = 0x91,		/* from SUPPORT_ASTRA_LCN */
	RPD_TABLE_ID						     = 0xe0,		/* from TNTSat - Regionalization Pool desiptor Section Table */
	DSC_TABLE_ID                             = 0xf0,
	ICT_TABLE_ID                             = 0xfe,

	SDTT_TABLE_ID					         = 0xc3,							/* Japan, ARIB */
	BIT_TABLE_ID					         = 0xc4,							/* Japan, ARIB */
	CDT_TABLE_ID						 	 = 0xc8,							/* Japan, ARIB */

	FSAT_EITPF_PP_OTH_TABLE_ID		= 0xd1,						// Freesat Tunneld Eit pf++ Oth

	ETC_TABLE_ID                             = 0xf1
};

typedef enum
{
	eSI_TableType_PAT = 0,
	eSI_TableType_CAT,
	eSI_TableType_PMT,
	eSI_TableType_NIT_ACT,
	eSI_TableType_NIT_OTH,
	eSI_TableType_SDT_ACT,
	eSI_TableType_SDT_OTH,
	eSI_TableType_BAT,
	eSI_TableType_TDT,
	eSI_TableType_TOT,
	eSI_TableType_DSI,
	eSI_TableType_DDB,
	eSI_TableType_EIT_ACTPF,
	eSI_TableType_EIT_OTHPF,
	eSI_TableType_EIT_ACTSC,
	eSI_TableType_EIT_OTHSC,
	eSI_TableType_UNT,
	eSI_TableType_CIT,
	eSI_TableType_CPT,
	eSI_TableType_SGT,
	eSI_TableType_AIT,
	eSI_TableType_DIT,
	eSI_TableType_PMT_N,

	eSI_TableType_POSTCODE_TREE,		/* For freesat */
	eSI_TableType_POSTCODE_REGION,		/* For freesat */

	eSI_TableType_ICT,		/* For Irdeto OTA */

	eSI_TableType_REGIONAL_POOL,		/* for TNTSat */

	eSI_TableType_RCT,		/* for TV Anytime */
/*  다음 si table들은 구현하면서 하나씩 추가할 예정
	eSI_TableType_EIT_NVOD,
	eSI_TableType_EMM,
	eSI_TableType_ECM,
	eSI_TableType_RST,
	eSI_TableType_ICT,
	eSI_TableType_DIT,
	eSI_TableType_DSC,
	eSI_TableType_CIT,
	eSI_TableType_CPT,
	eSI_TableType_Reserved01,
	eSI_TableType_Reserved02,
	eSI_TableType_Reserved03,
	eSI_TableType_Reserved04,
*/
	MAX_SI_TABLE_TYPE
} SiTableType_t;


typedef enum
{
	eAUDIO_TYPE_UNDEFINED			= 0,		/**< Normal Audio. */
	eAUDIO_TYPE_CLEAN_EFFECTS		= 1,
	eAUDIO_TYPE_HEARING_IMPAIRED	= 2,
	eAUDIO_TYPE_VISUAL_IMPAIRED		= 3,	/**< AD(Audio Description). */
	eAUDIO_TYPE_RESERVED			= 4
} AUDIO_Type_t;

typedef enum
{
	eAUDIO_TYPE_SUPP_MAIN = 0,
	eAUDIO_TYPE_SUPP_AD_VISUAL_IMPARED,
	eAUDIO_TYPE_SUPP_CLEAN_AUDIO_HEARING_IMPAIRED,
	eAUDIO_TYPE_SUPP_SPOKEN_SUBITITE_VISUAL_IMPARED,
	eAUDIO_TYPE_SUPPLEMENTARY_MAIN
} SUPPLEMENTARY_AUDIO_Type_t;

typedef enum
{
	eMHEG_MONITOR_MODE_None		= 0,

	eMHEG_MONITOR_MODE_Running
} MHEG_MONITOR_Mode_e;

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
#endif




#ifndef	FSAT_TYPES_H__
#define FSAT_TYPES_H__
#include "hlib.h"
#include "dlib_schedule.h"
#ifdef	__cplusplus
extern	"C" {
#endif

#define	SUPPORT_DENC_FILE
#ifdef SUPPORT_DENC_FILE
    #define USE_JSON_URL
#endif
/* Start === related to META SVC */
#define	fsatDxNAME_LEN							256
#define	fsatDxGROUP_LEN							16
#define	fsatDxGROUP_LIST_BUF_LEN					64
#define	fsatDxBOUQUET_LEN						16
#define	fsatDxANTINFO_NAME_LEN						24
#define	fsatDxDESC_LEN							256
#define	fsatDxDESC_TEXT_LEN						(512 + 4)
#define	fsatDxCHICON_URL_LEN_					64

#define	fsatDxFSAT_LANG_CODE_LEN				3
#define	fsatDxDEFAULT_AUTHORITY_LEN				33
#define	fsatDxFSAT_SVC_SHORT_NAME_LEN			(27)
#define	fsatDxFSAT_SVC_GUIDANCE_LEN				(255)
#define	fsatDxFSAT_SVC_PREFIX_URI_LEN			(255)

#define	fsatDxCRID_LEN							128
#define	fsatDxEVENT_CONTENT						16
#define	fsatDxEVENT_LINKAGE						8

#define 	fsatDxJP_MAX_AFFILIATION_ID_NUM			16
#define 	fsatDxJP_MAX_TS_NAME_LEN				(20+1)	/* only for ARIB Digital Terrestrial */

/* Start === related to META RECLIST */
#define fsatDxRECLIST_NAME_LEN	128
#define fsatDxRECLIST_URL		512
#define fsatDxRECLIST_CID		512
#define fsatDxRECLIST_ITEM		128
#define fsatDxRECLIST_ONE_SCAN_UNIT_NUM			32
#define fsatDxRECLIST_MAX_UPDATE_CMD   			5
#define fsatDxRECLIST_MAX_UPDATE_TAG			32
#define fsatDxRECLIST_KEEP_TAG					"KEEP"
#define fsatDxRECLIST_BOOKMARK_NAME_LEN			32
#define fsatDxRECLIST_NUM_BOOKMARK				32
#define	fsatDxRECLIST_EXT_EVT_MAX				16
#define	fsatDxRECLIST_EXT_TEXT_BUFSIZE			(256 * fsatDxRECLIST_EXT_EVT_MAX)
#define fsatDxRECLIST_EXT_TEXT_LEN				32



#define fsatDxPVR_PARENTAL_RATING_MAX			3
#define fsatDxPVR_EXT_ITEM_MAX					15

#define fsatDxPVR_EVENT_TEXT					256
#define fsatDxPVR_LANG_ISO_639_LEN				3
#define fsatDxFREESAT_PVR_GUIDANCE_DATA_MAX		255
#define fsatDxEPG_COMPONENT_DESC_MAX			10



/*******************************************************************/
/********************     Video  					*************************/
/*******************************************************************/
#define _________enum_video__________________________________________________________


typedef enum
{
	efsatDxVIDEO_TYPE_ALL	= 0,
	efsatDxVIDEO_TYPE_SD	= 1,
	efsatDxVIDEO_TYPE_HD	= 2,
} fsatDxVideoType_e;

typedef enum
{
	efsatDxVIDEO_CODEC_UNKNOWN 			= 0,
	efsatDxVIDEO_CODEC_MPEG1			= 1,		/**< MPEG-1 Video (ISO/IEC 11172-2) */
	efsatDxVIDEO_CODEC_MPEG2			= 2, 	/**< MPEG-2 Video (ISO/IEC 13818-2) */
	efsatDxVIDEO_CODEC_MPEG4Part2		= 3,		/**< MPEG-4 Part 2 Video */
	efsatDxVIDEO_CODEC_H261				= 4,		/**< H.261 Video. The value of the enum is not based on PSI standards. */
	efsatDxVIDEO_CODEC_H263				= 5,		/**< H.263 Video. The value of the enum is not based on PSI standards. */
	efsatDxVIDEO_CODEC_H264				= 6,		/**< H.264 (ITU-T) or ISO/IEC 14496-10/MPEG-4 AVC */
	efsatDxVIDEO_CODEC_H264_Svc			= 7, 	/**< Scalable Video Codec extension of H.264  */
	efsatDxVIDEO_CODEC_H264_Mvc			= 8, 	/**< Multi View Coding extension of H.264 */
	efsatDxVIDEO_CODEC_VC1				= 9, 	/**< VC-1 Advanced Profile */
	efsatDxVIDEO_CODEC_VC1_SimpleMain	= 10, 	/**< VC-1 Simple & Main Profile */
	efsatDxVIDEO_CODEC_AVS				= 11,	/**< AVS coded video */
	efsatDxVIDEO_CODEC_RV40				= 12,	/**< RV 4.0 coded video */
	efsatDxVIDEO_CODEC_VP6				= 13,	/**< VP6 coded video */
	efsatDxVIDEO_CODEC_DIVX311			= 14,	/**< DivX 3.11 coded video */
	efsatDxVIDEO_CODEC_XVID				= 15,	/**< XVid coded video */
	efsatDxVIDEO_CODEC_Max
} fsatDxVideoCodec_e;

/*******************************************************************/
/********************     Audio  					*************************/
/*******************************************************************/
#define _________enum_Audeo___________________________________________________________________

typedef enum
{
	efsatDxAUDIO_CODEC_UNKNOWN 				= 0,	/* unknown/not supported audio format */
	efsatDxAUDIO_CODEC_MPEG					= 1,	/* MPEG1/2, layer 1/2. This does not support layer 3 (mp3). */
	efsatDxAUDIO_CODEC_MP3					= 2,	/* MPEG1/2, layer 3. */
	efsatDxAUDIO_CODEC_DOLBY_AC3			= 3,	/* Dolby Digital AC3 audio */
	efsatDxAUDIO_CODEC_DOLBY_AC3P 			= 4,	/* Dolby Digital Plus (AC3+ or DDP) audio */
	efsatDxAUDIO_CODEC_PCM					= 5,	/* PCM audio - Generally used only with inputs such as SPDIF or HDMI. */

	efsatDxAUDIO_CODEC_AAC					= 6,	/* Advanced audio coding. Part of MPEG-4 */
	efsatDxAUDIO_CODEC_AAC_LOAS				= 7,	/* Advanced audio coding. Part of MPEG-4 */
	efsatDxAUDIO_CODEC_AAC_PLUS				= 8,	/* AAC plus SBR. aka MPEG-4 High Efficiency (AAC-HE) with ADTS (Audio Data Transport Format) */
	efsatDxAUDIO_CODEC_AAC_PLUS_ADTS			= 9,	/* AAC plus SBR. aka MPEG-4 High Efficiency (AAC-HE), with ADTS (Audio Data Transport Format) sync and LATM mux */
	efsatDxAUDIO_CODEC_DTS					= 10,	/* Digital Digital Surround sound, uses non-legacy frame-sync */
	efsatDxAUDIO_CODEC_DTS_HD					= 11,	/* Digital Digital Surround sound, HD, uses non-legacy frame-sync, decodes only DTS part of DTS-HD stream */
	efsatDxAUDIO_CODEC_LPCM_DVD				= 12,	/* LPCM, DVD mode */
	efsatDxAUDIO_CODEC_LPCM_HD_DVD			= 13,	/* LPCM, HD-DVD mode */
	efsatDxAUDIO_CODEC_LPCM_BLUERAY			= 14,	/* LPCM, Blu-Ray mode */
	efsatDxAUDIO_CODEC_WMA_STD				= 15,	/* WMA Standard */
	efsatDxAUDIO_CODEC_WMA_PRO				= 16,	/* WMA Professional */
	efsatDxAUDIO_CODEC_PCMWAVE				= 17,	/* PCM audio with Wave header - Used with streams containing PCM audio */
	efsatDxAUDIO_CODEC_DRA					= 18,   /* Dynamic Resolution Adaptation.  Used in Blu-Ray and China Broadcasts. */
	efsatDxAUDIO_CODEC_AIFF					= 19,
	efsatDxAUDIO_CODEC_Max
} fsatDxAudioCodec_e;

typedef	enum
{
	efsatDxAUDIO_CONFIG_UNKNOWN				= 0,
	efsatDxAUDIO_CONFIG_STEREO,
	efsatDxAUDIO_CONFIG_MONOLEFT,
	efsatDxAUDIO_CONFIG_MONORIGHT,
	efsatDxAUDIO_CONFIG_MIX,
} fsatDxAudioConfig_e;

/*******************************************************************/
/********************    channel 					*************************/
/*******************************************************************/
#define __________channel___________________________________________________________

typedef	enum
{
	efsatDxDELIVERY_ALL		= 0x00,
	efsatDxDELIVERY_SAT		= 0x01,
	efsatDxDELIVERY_TER		= 0x02,
	efsatDxDELIVERY_CAB		= 0x04,
	efsatDxDELIVERY_ANALOG	= 0x08,
	efsatDxDELIVERY_NET		= 0x10,
} fsatDxDeliveryType_e;


typedef enum
{
	efsatDxTER_BANDWIDTH_8M			= 0,	/**< 8MHz (DVB-T/DVB-H/DMB-T/H). 占쏙옙占? DAB Eureka 147(1.712MHz), IEEE 802.11a(20MHz) */
	efsatDxTER_BANDWIDTH_7M			= 1,	/**< 7MHz */
	efsatDxTER_BANDWIDTH_6M			= 2,	/**< 6MHz */
	efsatDxTER_T2BANDWIDTH_5MHZ		= 3,
	efsatDxTER_T2BANDWIDTH_10MHZ		= 4,
	efsatDxTER_T2BANDWIDTH_1P7MHZ		= 5
} fsatDxTer_BandWidth_e;

/* Modulation Constellation */
typedef enum
{
	efsatDxTER_CONSTELLATION_AUTO	= 0,
	efsatDxTER_CONSTELLATION_QPSK	= 1,
	efsatDxTER_CONSTELLATION_16QAM = 2,
	efsatDxTER_CONSTELLATION_64QAM = 3,
	efsatDxTER_CONSTELLATION_256QAM= 4

} fsatDxTer_Constellation_e;

typedef enum
{
	efsatDxTER_HIERACHY_AUTO		= 0,
	efsatDxTER_HIERACHY_NONE		= 1,
	efsatDxTER_HIERACHY_1			= 2,
	efsatDxTER_HIERACHY_2			= 3,
	efsatDxTER_HIERACHY_4			= 4
} fsatDxTer_Hierachy_e;


/* Inner FEC Code Rate */
typedef enum
{
	efsatDxTER_CODERATE_AUTO	= 0,
	efsatDxTER_CODERATE_1_2		= 1,	/**< 1/2 */
	efsatDxTER_CODERATE_2_3		= 2,	/**< 2/3 */
	efsatDxTER_CODERATE_3_4		= 3,	/**< 3/4 */
	efsatDxTER_CODERATE_5_6		= 4,	/**< 5/6 */
	efsatDxTER_CODERATE_7_8		= 5 	/**< 7/8 */
} fsatDxTer_CodeRate_e;

/* Number of Subcarriers. (Number of FFT input data values.) */
typedef enum
{
	efsatDxTER_TRANSMODE_AUTO		= 0,
	efsatDxTER_TRANSMODE_2K			= 1,	/**< FFT 2048. 1705 subcarriers */
	efsatDxTER_TRANSMODE_8K			= 2,	/**< FFT 8192. 6817 subcarriers */
	efsatDxTER_T2TRANSMODE_1K		= 3,
	efsatDxTER_T2TRANSMODE_4K		= 4,
	efsatDxTER_T2TRANSMODE_16K 		= 5,
	efsatDxTER_T2TRANSMODE_32K 		= 6
} fsatDxTer_TransMode_e;

/**< Additional guard interval */
typedef enum
{
	efsatDxTER_GUARDINTERVAL_AUTO	= 0,
	efsatDxTER_GUARDINTERVAL_1_4	=	1,	/**< 1/4 */
	efsatDxTER_GUARDINTERVAL_1_8	= 2,	/**< 1/8 */
	efsatDxTER_GUARDINTERVAL_1_16	= 3,	/**< 1/16 */
	efsatDxTER_GUARDINTERVAL_1_32	= 4,	/**< 1/32 */
	efsatDxTER_T2GUARDINTERVAL_1_128	= 5,	/**< 1/128 */
	efsatDxTER_T2GUARDINTERVAL_19_128	= 6,	/**< 19/128 */
	efsatDxTER_T2GUARDINTERVAL_19_256	= 7 /**< 19/256 */

} fsatDxTer_GuardInterval_e;

typedef enum
{
	efsatDxTER_STREAM_HP			= 0,	/**< HP stream */
	efsatDxTER_STREAM_LP			= 1 	/**< LP stream */
} fsatDxTer_Stream_e;

typedef enum
{
	efsatDxTER_OFFSET_AUTO 		= 0,
	efsatDxTER_OFFSET_NONE 		= 1,	/**< No offset */
	efsatDxTER_OFFSET_1_6_P		= 2,	/**< +1/6MHz */
	efsatDxTER_OFFSET_1_6_N		= 3,	/**< -1/6MHz */
	efsatDxTER_OFFSET_2_6_P		= 4,	/**< +2/6MHz */
	efsatDxTER_OFFSET_2_6_N		= 5,	/**< -2/6MHz */
	efsatDxTER_OFFSET_3_6_P		= 6,	/**< +3/6MHz */
	efsatDxTER_OFFSET_3_6_N		= 7 	/**< -3/6MHz */
} fsatDxTer_Offset_e;
typedef enum
{
	efsatDxTER_DVB_T			= 0,		/**< T2 stream */
	efsatDxTER_DVB_T2			= 1 		/**< T stream */
} fsatDxTer_System_e;

typedef enum
{
	efsatDxTER_SISO			= 0,
	efsatDxTER_MISO			= 1
} fsatDxTer_PreambleFormat_e;
typedef enum
{
	efsatDxTER_NOT_MIXED		= 0,
	efsatDxTER_MIXED			= 1
} fsatDxTer_Mixed_e;
typedef enum
{
	efsatDxTER_PP_AUTO 	= 0,
	efsatDxTER_PP1 		= 1,
	efsatDxTER_PP2 		= 2,
	efsatDxTER_PP3 		= 3,
	efsatDxTER_PP4 		= 4,
	efsatDxTER_PP5 		= 5,
	efsatDxTER_PP6 		= 6,
	efsatDxTER_PP7 		= 7,
	efsatDxTER_PP8 		= 8
} fsatDxTer_Pilot_Pattern_e;

typedef enum
{
	efsatDxTER_T2NOT_USED		= 0,
	efsatDxTER_T2USED			= 1
} fsatDxTer_ExtendedCarrier_e;

typedef enum
{
	efsatDxTER_REDUCTION_NONE			= 0,
	efsatDxTER_ACE 						= 1,
	efsatDxTER_TR						= 2,
	efsatDxTER_AVE_TR					= 3
} fsatDxTer_PAPR_Reduction_e;

typedef enum
{
	efsatDxCAB_CONSTELLATION_AUTO		= 0,
	efsatDxCAB_CONSTELLATION_16QAM 		= 1,
	efsatDxCAB_CONSTELLATION_32QAM 		= 2,
	efsatDxCAB_CONSTELLATION_64QAM 		= 3,
	efsatDxCAB_CONSTELLATION_128QAM		= 4,
	efsatDxCAB_CONSTELLATION_256QAM		= 5
} fsatDxCab_Constellation_e;

typedef enum
{
	efsatDxCAB_SPECTRUM_AUTO			= 0,
	efsatDxCAB_SPECTRUM_NORMAL 			= 1,
	efsatDxCAB_SPECTRUM_INVERTED		= 2
} fsatDxCab_Spectrum_e;



typedef enum
{
	efsatDxANT_TYPE_LNB_ONLY			= 0,	/**< LNB only */
	efsatDxANT_TYPE_DISEQC				= 1,	/**< Diseqc switch, Motor */
	efsatDxANT_TYPE_SCD					= 2,	/**< Single Cable Distribution */
	efsatDxANT_TYPE_SMATV 				= 3,	/**< SMATV */
	efsatDxANT_TYPE_MONOBLOCK_LNB 		= 4
} fsatDxAntennaType_e;



typedef enum
{
	efsatDxSAT_CODERATE_AUTO			= 0,		/* Code rate : Auto detect */
	efsatDxSAT_CODERATE_NONE			= 1,		/* DVBS Code Rate */
	efsatDxSAT_CODERATE_1_2				= 2,		/* Code rate : 1/2 */
	efsatDxSAT_CODERATE_2_3				= 3,		/* Code rate : 2/3 */
	efsatDxSAT_CODERATE_3_4				= 4,		/* Code rate : 3/4 */
	efsatDxSAT_CODERATE_5_6				= 5,		/* Code rate : 5/6 */
	efsatDxSAT_CODERATE_7_8				= 6,		/* Code rate : 7/8 */

	/* DVBS2 Code Rate */
	efsatDxSAT_CODERATE_3_5				= 7,		/* Code rate : 3/5 */
	efsatDxSAT_CODERATE_4_5				= 8,		/* Code rate : 4/5 */
	efsatDxSAT_CODERATE_5_11			= 9,		/* Code rate : 5/11 */
	efsatDxSAT_CODERATE_6_7				= 10,		/* Code rate : 6/7 */
	efsatDxSAT_CODERATE_8_9				= 11,		/* Code rate : 8/9 */
	efsatDxSAT_CODERATE_9_10			= 12		/* Code rate : 9/10 */

} fsatDxSat_CodeRate_e;
typedef enum
{
	efsatDxSAT_POLAR_AUTO				= 0,
	efsatDxSAT_POLAR_HOR				= 1,
	efsatDxSAT_POLAR_VER				= 2,
	efsatDxSAT_POLAR_LEFT				= 3,
	efsatDxSAT_POLAR_RIGHT 				= 4
}  fsatDxSat_Polarization_e;
typedef enum
{
	efsatDxSAT_TRANS_DVBS			= 0,
	efsatDxSAT_TRANS_DVBS2 		= 1
}  fsatDxSat_TransportSpec_e;

typedef enum
{
	efsatDxSAT_PSK_AUTO			= 0,
	efsatDxSAT_PSK_QPSK			= 1,
	efsatDxSAT_PSK_8PSK			= 2
} fsatDxSat_PskModulation_e;

typedef enum
{
	efsatDxSAT_PILOT_AUTO			= 0,			/**< Pilot : Auto detect */
	efsatDxSAT_PILOT_OFF			= 1,			/**< Pilot : Off */
	efsatDxSAT_PILOT_ON			= 2 			/**< Pilot : On */
} fsatDxSat_Pilot_e;

typedef enum
{
	efsatDxSAT_ROLL_020			= 0,		/**< Roll off : 0.20 */
	efsatDxSAT_ROLL_025			= 1,		/**< Roll off : 0.25 */
	efsatDxSAT_ROLL_035			= 2 		/**< Roll off : 0.35 */
} fsatDxSat_RollOff_e;


typedef enum
{
  efsatDxLNB_VOLT_STD			= 0,			   /**< Vertical : 11v, Horizontal 15v */
  efsatDxLNB_VOLT_HIGH			= 1 			   /**< Vertical : 12v, Horizontal 16v */
} fsatDxSAT_LnbVoltage_e;
typedef enum
{
	efsatDxDISEQC_VER_1_0 		= 0,
	efsatDxDISEQC_VER_1_1 		= 1,
	efsatDxDISEQC_VER_1_2 		= 2,
	efsatDxDISEQC_VER_1_3 		= 3,
	efsatDxDISEQC_VER_2_0 		= 4
} fsatDxSAT_DiseqcVer_e;

typedef enum
{
	efsatDxDISEQC_INPUT_NONE		= 0,
	efsatDxDISEQC_INPUT_A 		= 1,
	efsatDxDISEQC_INPUT_B 		= 2,
	efsatDxDISEQC_INPUT_C 		= 3,
	efsatDxDISEQC_INPUT_D 		= 4
} fsatDxSAT_DiseqcInput_e;

typedef enum
{
	efsatDxSCD_INPUT_NONE 		= 0,
	efsatDxSCD_INPUT_SCD_A		= 1,
	efsatDxSCD_INPUT_SCD_B		= 2
} fsatDxSAT_ScdInput_e;

typedef enum
{
	efsatDxTONE_BURST_NONE		= 0,
	efsatDxTONE_BURST_A			= 1,
	efsatDxTONE_BURST_B			= 2
} fsatDxSAT_ToneBurst_e;




/*******************************************************************/
/********************    service					*************************/
/*******************************************************************/
#define _________enum_service_________________________________________________________


typedef	enum
{
	efsatDxSVC_UPDATE_NORMAL			= 0x00000000,
	efsatDxSVC_UPDATE_NEWCHANNEL		= 0x00000001,
	efsatDxSVC_UPDATE_UPDATECHANNEL		= 0x00000002,
	efsatDxSVC_UPDATE_READONLY			= 0x00000003
} fsatDxSvcUpdateFlag_e;


typedef	enum
{
	efsatDxSVC_TYPE_All			= 0,
	efsatDxSVC_TYPE_TV			= 0x0001,
	efsatDxSVC_TYPE_RADIO		= 0x0002,
	efsatDxSVC_TYPE_DATA		= 0x0004,
	efsatDxSVC_TYPE_SUBFEED		= 0x0008,
	efsatDxSVC_TYPE_REPLACEMENT	= 0x0010,
	efsatDxSVC_TYPE_HIDDEN   	= 0x0020,
	efsatDxSVC_TYPE_MHPTV		= 0x0040,
	efsatDxSVC_TYPE_ENGINEERING	= 0x0080,
	efsatDxSVC_TYPE_RFVOD		= 0x0100,
} fsatDxSvcType_e;

typedef enum
{
	efsatDxSvcUserFlag_All					= 0,
	efsatDxSvcUserFlag_01_FsatUserCustom		= 0x00000001,	/**< all or incluing this condition	, CONFIG_OP_FREESAT */
	efsatDxSvcUserFlag_02_Selectable			= 0x00000002,	/**< all or incluing this condition	, CONFIG_OP_FREESAT or CONFIG_OP_UK_DTT_DBOOK */
	efsatDxSvcUserFlag_03_SvcDeletedByUser	= 0x00000004,	/**< exclusive condition			, Italy DTT에서 사용 중(CONFIG_DB_SVC_TRASH)이나 같은 요구 사항 내에서 사용 가능 */
	efsatDxSvcUserFlag_04_CanalreadyTerLcn	= 0x00000008,	/**< excusive condition 			, Canal Ready 에서 사용 중 */
	efsatDxSvcUserFlag_05_reserved			= 0x00000010,	/**< exclusive condition			, SkyD에서 사용 중, preferred service가 DCM이후에 없어졌다는 의미*/
	efsatDxSvcUserFlag_06_TemporaryAlive		= 0x00000020,	/**< all or incluing this condition	, add or update 이후, one shot으로 delete하기 위해 사용하는 flag */
	efsatDxSvcUserFlag_07_SkipChUpDown		= 0x00000040,	/**< all or exclusive condition		, Channel List에서는 보이나 Channel Up/Down Key로는 Access 되지 않는 Service를 의미한다. */
	efsatDxSvcUserFlag_08_RegionalSvc			= 0x00000080,	/**< exclusive condition			, TNTSAT에서 사용 */
	efsatDxSvcUserFlag_09_AdultSvc			= 0x00000100,	/**< exclusive condition			, CONFIG_OP_FREESAT */
	efsatDxSvcUserFlag_10_NationalSvc			= 0x00000200,	/**<exclusive condition				, CanalReady TNTSat only */
	efsatDxSvcUserFlag_11_FreeCiMode			= 0x00000400,	/**<exclusive condition				, CI+ Model only */
	efsatDxSvcUserFlag_12_IsExistFreqList		= 0x00000800,
	efsatDxSvcUserFlag_13_VolatileSvc			= 0x00001000,	/**< exclusive condition    	     , 휘발성 Service로 저장하지 않는다. Tune Action에서 사용 */
	efsatDxSvcUserFlag_14_NoRecTsr			= 0x00002000,	/**< exclusive condition 			, Recodring 과 TSR을 허용하지 않는 service */
	efsatDxSvcUserFlag_15_TsrOnly				= 0x00004000,	/**< exclusive condition 			, TSR만 허용 하는 service */
	efsatDxSvcUserFlag_16_QuasiStaticAlive	= 0x00008000,
	efsatDxSvcUserFlag_17_TivuSatLcn			= 0x00010000,
	efsatDxSvcUserFlag_18_DigiturkEutelSat	= 0x00020000,	/**< exclusive condition 			 */
	efsatDxSvcUserFlag_19_DigiturkTurkSat		= 0x00040000,	/**< exclusive condition 			 */
	efsatDxSvcUserFlag_20_ParentalControl		= 0x00080000,	/**< all condistion					, Parental control이 있었던 서비스를 marking. 이 mark가 없으면 wtv가 parental check를 skip한다. */
	efsatDxSvcUserFlag_21_LockSvcByUser		= 0x00100000,	/**< exclusive condistion			, Japan에서 사용 중, 튜닝시 패스워드를 물어야 하는 채널락 */
	efsatDxSvcUserFlag_22_SkipSvcByUser		= 0x00200000,	/**< exclusive condistion			, Japan에서 사용 중, 채널업/다운시 해당 서비스 스킵 (eSvcUserFlag_07_SkipChUpDown과 구분되어야 함.) */
	efsatDxSvcUserFlag_23_PreferSvcByUser	= 0x00400000,	/**< exclusive condistion			, Japan에서 사용 중, 선호채널키 선택시 튜닝되어야하는 서비스를 설정 (Favorite group은 채널번호 및 네트워크순으로 sorting/find 지원하지 않기 때문에 사용할 수 없음.) */
	efsatDxSvcUserFlag_24_TempSvc			= 0x00800000,	/**< exclusive condistion			, Japan에서 사용 중, 임시서비스 */
	efsatDxSvcUserFlag_25_MaskCh			= 0x01000000,	/**< exclusive condistion			, Japan에서 사용 중, 채널마스킹 */
	efsatDxSvcUserFlag_26_NordigLcnV2		= 0x02000000,	/**< exclusive condistion			, nordig lcn v2라는 의미  */
	efsatDxSvcUserFlag_27_reserved			= 0x04000000,
	efsatDxSvcUserFlag_28_reserved			= 0x08000000,
	efsatDxSvcUserFlag_29_reserved			= 0x10000000,
	efsatDxSvcUserFlag_30_reserved			= 0x20000000,
	efsatDxSvcUserFlag_31_reserved			= 0x40000000,
	efsatDxSvcUserFlag_32_NewService			= 0x80000000	/**< all or including this condition , 새로운 Channel이 발견 되었을때 해당 플래그를 세워준다. OCTO 2.0기준 해당 플래그는 OCTO가 APPL로 줄수만 있고 APPL은 해당 플래그를 세워 줘도 OCTO가 무시한다.*/
} fsatDxUserFlag_t;

typedef	enum
{
	efsatDxCAS_TYPE_All			= 0,
	efsatDxCAS_TYPE_FTA			= 0x0001,
	efsatDxCAS_TYPE_NAGRA		= 0x0002,
	efsatDxCAS_TYPE_IRDETO		= 0x0004,
	efsatDxCAS_TYPE_NDS			= 0x0008,
	efsatDxCAS_TYPE_VIACCESS	= 0x0010,
	efsatDxCAS_TYPE_CONAX		= 0x0020,
	efsatDxCAS_TYPE_MEDIAGUARD	= 0x0040,
	efsatDxCAS_TYPE_CRYPTOWORKS	= 0x0080,
	efsatDxCAS_TYPE_JPBCAS		= 0x0100,
	efsatDxCAS_TYPE_JPCCAS		= 0x0200,
	efsatDxCAS_TYPE_CAS			= 0xFFFE
} fsatDxCasType_e;





#define _____freesat_service______________________________________________________
#if defined(CONFIG_OP_FREESAT)

typedef struct
{

	HINT32		svcId;				///< "fsat_svcid"
	HINT32		regionId;			///< "fsat_rgnid"
	HCHAR		defaultAuth[fsatDxDEFAULT_AUTHORITY_LEN];	///< "default_auth"
	HINT32		contentMgr;			///< "fta_contentmgr"

	/* channel icon 임시 추가 - /usr/browser/webapps/image/logo/chlog/svcid.png */
	HCHAR		szShortName[fsatDxFSAT_SVC_SHORT_NAME_LEN];

	HUINT8		ucUserCustomizable;
	HUINT8		ucUserSelectable;
	HUINT8		ucAdultSvc;
	HINT32          ucVideoCodecH264;

	HUINT8		ucGuidanceType;
	HUINT8		ucGuidanceMode;
	HUINT8		szGuidanceStr[fsatDxFSAT_SVC_GUIDANCE_LEN];

	HUINT8		ucPresentFollow;
	// TODO: add more freesat related code here
} fsatDxFreesatService_t;

typedef struct
{
	HCHAR	defaultAuth[fsatDxDEFAULT_AUTHORITY_LEN];

} fsatDxFreesatTs_t;

typedef struct
{
	HCHAR	defaultAuth[fsatDxDEFAULT_AUTHORITY_LEN];

} fsatDxFreesatNet_t;

typedef struct
{
	HUINT8	ucGroupType;
	HUINT8	ucNondestructiveTuneFlag;
	HUINT8	ucReturnChannelAccessFlag;
	HUINT8	szIso639LangCode[fsatDxFSAT_LANG_CODE_LEN+1];
	HUINT8	ucG2ExtensionFlag;
	HUINT8	ucDefaultVisableFlag;
	HUINT8	ucAdultChannelsGroupFlag;
	HUINT8	ucIpChannelsGroupFlag;
	HUINT8	ucCaChannelsGroupFlag;
}fsatDxFreesatGroup_t;



typedef enum
{
	efsatDxFSat_GUIDANCE_TYPE_NONE	= 0,
	efsatDxFSat_GUIDANCE_TYPE_MAX	,
} fsatDxFreesat_PVR_GuidanceType_e;


typedef struct	tagfsatDxPvrGuidance
{
	fsatDxFreesat_PVR_GuidanceType_e	eType;
	HUINT8					stLangCode[fsatDxPVR_LANG_ISO_639_LEN+1];
	HUINT8					stData[fsatDxFREESAT_PVR_GUIDANCE_DATA_MAX+1];
}fsatDxFreesat_Pvr_Guidance_t;

typedef struct
{
	fsatDxFreesat_Pvr_Guidance_t	astGuidance;
} fsatDxFreesat_RecList_EVTData_t;

typedef struct
{
	fsatDxFreesat_RecList_EVTData_t	stEvt;
} fsatDxFreesatRecListData_t;

#endif	// #if defined(CONFIG_OP_FREESAT)

/*******************************************************************/
/********************    PVR			*************************/
/*******************************************************************/
#define _________enum_PVR_________________________________________________________


typedef enum {
	efsatDxREC_STS_RECORD_START = 0,
	efsatDxREC_STS_RECORDING,
	efsatDxREC_STS_RECORDED,
	efsatDxREC_STS_INCOMPLETED
}fsatDxRecStatus_e;

typedef enum {
	efsatDxREC_FAILED_NOFAIL		= 0,
	efsatDxREC_FAILED_HDDFULL,
	efsatDxREC_FAILED_MAXNUM,
	efsatDxREC_FAILED_MAXRECORDLISTNUM,
	efsatDxREC_FAILED_LESS30SEC,
	efsatDxREC_FAILED_NOSIGNAL,
	efsatDxREC_FAILED_NONSERVICE,
	efsatDxREC_FAILED_SEARCHING,
	efsatDxREC_FAILED_CONFLICT_TP,
	efsatDxREC_FAILED_DRM,
	efsatDxREC_FAILED_NOSC_SCR,
	efsatDxREC_FAILED_NORIGHT_SCR,
	efsatDxREC_FAILED_NOHDD,
	efsatDxREC_INCOMPLETE_HDDFULL,
	efsatDxREC_FAILD_UNKNOWN,
	efsatDxREC_FAILED_NOUSBHDD,
	efsatDxREC_FAILED_DISCONNECTUSBHDD,
	efsatDxREC_FAILED_EXTUSB_CopyDel,
	efsatDxREC_FAILED_SAME_SVC_RECORDING,
	efsatDxREC_FAILED_HIGHER_PRIORITY,
	efsatDxREC_FAILED_UNABLE_TRACKING
}fsatDxRecEndReasonType_e;


#define fsatDxPVR_MAX_AUDIO_ES_NUM				16
#define fsatDxPVR_MAX_SUBTTL_ES_NUM				12

/* @brief PMT -> ISO 639 language descriptor (Audio Type) */
typedef enum
{
	efsatDxAUDIO_TYPE_UNDEFINED			= 0,		/**< Normal Audio. */
	efsatDxAUDIO_TYPE_CLEAN_EFFECTS		= 1,
	efsatDxAUDIO_TYPE_HEARING_IMPAIRED	= 2,
	efsatDxAUDIO_TYPE_VISUAL_IMPAIRED	= 3,	/**< AD(Audio Description). */
	efsatDxAUDIO_TYPE_RESERVED			= 4
} fsatDxAUDIO_Type_e;


/** @brief Subtitle infomation in PMT */
typedef struct tagfsatDxMETA_PVR_SBTL_INFO
{
	HUINT8				ucComponentType;		/** < Teletext: 0x00~0x05, Subtitle: 0x10~0x13, 0x20~0x23 */
	HUINT8				aucLangCode [fsatDxPVR_LANG_ISO_639_LEN+1];
} fsatDxMETA_PvrSbtlInfo_t;


/** @brief Subtitle infomation in PMT */
typedef struct tagfsatDxMETA_PVR_SUBTTL_INFO_LIST
{
	HUINT32     			ulItemNum;				/**< Subtitle 개수 */
	fsatDxMETA_PvrSbtlInfo_t	astSbtlInfo[fsatDxPVR_MAX_SUBTTL_ES_NUM];
} fsatDxMETA_PvrSbtlInfoList_t;


#define fsatDxMAX_MWPVR_AUD_NUM 4
typedef struct tagfsatDxMWPVR_639_LAN_INFO
{
	fsatDxAUDIO_Type_e	eAudType;
	HUINT8			szIso639LangCode[fsatDxPVR_LANG_ISO_639_LEN+1];
} fsatDxMwPvrIso639LangInfo_t;


typedef enum
{
	efsatDxPVR_AUD_CPNT_SINGLE_MONO = 0,
	efsatDxPVR_AUD_CPNT_DUAL_MONO,
	efsatDxPVR_AUD_CPNT_STEREO,
	efsatDxPVR_AUD_CPNT_MULTILINGUAL,
	efsatDxPVR_AUD_CPNT_SURROUND,
	efsatDxPVR_AUD_CPNT_DOLBY,
} fsatDxPVR_AUDIO_CPNT_e;

typedef struct tagfsatDxMETA_PVR_AUDIO_INFO
{
	HUINT32					ulLangNum;
	fsatDxMwPvrIso639LangInfo_t	stLangInfo[fsatDxMAX_MWPVR_AUD_NUM];
} fsatDxMETA_PvrAudioInfo_t;


/** @brief Subtitle infomation in PMT */
typedef struct tagfsatDxMETA_PVR_AUDIO_INFO_LIST
{
	HUINT32     			ulItemNum;
	fsatDxMETA_PvrAudioInfo_t	astAudioInfo[fsatDxPVR_MAX_AUDIO_ES_NUM];
} fsatDxMETA_PvrAudioInfoList_t;


#define _____end_freesat_service______________________________________________

typedef enum
{
	efsatDxOPTYPE_ALL			= 0,
	efsatDxOPTYPE_NONE		    = 1,
	efsatDxOPTYPE_APSHD		    = 2,
	efsatDxOPTYPE_FREESAT		= 3,
	efsatDxOPTYPE_CANALSAT	    = 4,
	efsatDxOPTYPE_NTVPLUS		= 5,
	efsatDxOPTYPE_UPC			= 6,
	efsatDxOPTYPE_ADTV		    = 7,
	efsatDxOPTYPE_ALJAZEERA	    = 8,
	efsatDxOPTYPE_CNE			= 9,
	efsatDxOPTYPE_DIGITURK	    = 10,
	efsatDxOPTYPE_NONDIGITURK	= 11,
	efsatDxOPTYPE_TIVUSAT		= 12,
	efsatDxOPTYPE_MOSTELECOM	= 13,
	efsatDxOPTYPE_MAX 	    	= 0XFF
} fsatDxOPTYPE_e;

/*******************************************************************/
/********************    Struct				*************************/
/*******************************************************************/
#define _________STRUCT_________________________________________________________


typedef struct
{
	HINT32				uid;
	HINT32				version;
	HINT32				tsuid;
	HINT32				prvuid;
	HINT32				antuid;
	HINT32				grpuids[fsatDxGROUP_LEN];
	HINT32				bqtuids[fsatDxBOUQUET_LEN];

	HINT32				svcid;
	HINT32				tsid;
	HINT32				onid;

	HINT32				lcn;

	HCHAR				name[fsatDxNAME_LEN];
	fsatDxSvcType_e			svcType;
	fsatDxSvcType_e			orgSvcType;
	HINT32				optype;
	fsatDxDeliveryType_e 	deliType;
	HCHAR				satType[fsatDxANTINFO_NAME_LEN];
	HBOOL				visibleFlag;
	HBOOL				locked;
	HBOOL				removed;

	HINT32				pmtPid;
	HINT32				pcrPid;
	HINT32				videoPid;
	HINT32				audioPid;
	HINT32				ttxPid;
	HINT32				audioAuxPid;
	HINT32				dolbyPid;

	fsatDxVideoType_e		highDefinition;
	fsatDxCasType_e			casType;
	HBOOL				dolbyFlag;
	fsatDxVideoCodec_e		videoCodec;
	fsatDxAudioCodec_e		audioCodec;
	fsatDxAudioCodec_e		audioAuxCodec;
	fsatDxAudioCodec_e		dolbyCodec;
	HBOOL				lcnFlag;
	fsatDxAudioConfig_e		audioConfig; // stereoConfig

	fsatDxSvcUpdateFlag_e	updateSvcFlag;
	union {
#ifdef	CONFIG_OP_FREESAT
		fsatDxFreesatService_t	freesat;
#else
		void *	__;
#endif
	} ex;

    HUINT32                 __CRC__;  //CHECK CRC
} fsatDxService_t;

typedef struct
{
	HCHAR		*item;
	HCHAR		*description;
} fsatDxEventItem_t;

typedef struct
{
	HUINT8			content;
	HUINT8			type;
	HUINT8			tag;
	HxLANG_Id_e	language;
	HCHAR			*text;
} fsatDxEventComponent_t;

typedef struct
{
	HUINT8			type;
	HCHAR			*crid;
} fsatDxEventCRID_t;

typedef struct
{
	HUINT16			onid;
	HUINT16			tsid;
	HUINT16			svcid;
	HUINT8			linkageType;
	HUINT8			*privateData;
} fsatDxEventLinkage_t;


typedef struct
{
	HUINT8			guidancetype;
	HCHAR			*guidancedata;
} fsatDxEventGuidanceInfo_t;

typedef	struct
{
	HUINT8		mediausage;
	UNIXTIME	startdatetime;
	UNIXTIME	stopdatetime;
	HBOOL		availablenow;

} fsatDxEventOnlineAvailInfo_t;

typedef	struct
{
	HUINT8		mediatype;
	HUINT8		mediausage;
	HBOOL		media_prefix_location;
	HUINT8		media_prefix_index;
	HCHAR		*URI_char;
} fsatDxEventMediaLocatorInfo_t;


typedef struct
{
	HCHAR			*szShortEventPromo;
} fsatDxEventShortEventPromotionInfo_t;



typedef struct
{
	fsatDxEventGuidanceInfo_t		fsatGuidance;
	fsatDxEventShortEventPromotionInfo_t	fsatShortEventPromo;
	HxVector_t				*fsatMediaLocatorInfo;
	HxVector_t				*ext_descriptor;
}fsatDxFreesatEvent_t;

typedef struct
{
	HINT32		refCount;
	HINT32		size;
	HUINT8		*pucCollector;
}fsatDxDataCollector_t;

typedef struct
{
	HINT32				refCount;
	HINT32				svcuid;
	HUINT16				eventId;
	HUINT32				startTime;
	HUINT32				duration;
	HUINT8				runStatus;
	HBOOL				freeCaMode;


	HCHAR				*name;
	HCHAR				*text;
	HINT32				parentalRating;
	HUINT16				content[fsatDxEVENT_CONTENT];

	HCHAR				*longText;
	HxVector_t			*item;			///< extended info items : APKS_EventItem_t *
	HxVector_t			*linkage;		///< .... the others : APKS_EventLinkage_t *
	HxVector_t			*component; 	///< .... the others : APKS_EventComponent_t *
	HxVector_t			*crid;			///< .... the others : APKS_EventCRID_t *
	HxVector_t			*descriptor;	///< raw descriptor

	union extension {
		/**
		 * 일단 여기에 코드 넣는건 비추!!!
		 * raw descriptor로 왠만한건 다 커버 가능하니, opl에서 raw descriptor를 파싱 하셔요
		 * 이 밑은 정말 어쩔수 없는 경우만 임시로 넣습니다.
		 */
#ifdef	CONFIG_OP_FREESAT
		fsatDxFreesatEvent_t	fsateventinfo;
#endif
		void *dummy;
	} extension;

	fsatDxDataCollector_t	*pstCollector;
} fsatDxEvent_t;





typedef struct	tagfsatDxPvrParentalRating
{
	HUINT8					aucIsoCountryCode[fsatDxPVR_LANG_ISO_639_LEN+1];
	HUINT8					ucParentalRating;
}fsatDxPvr_ParentalRating_t;

typedef struct	tagfsatDxPvrEvtComponent
{
	HUINT8					ucStreamContent;
	HUINT8					ucComponentType;
	HUINT8					aucIsoCountryCode[fsatDxPVR_LANG_ISO_639_LEN + 1];
}fsatDxPvr_EvtComponent_t;


typedef	struct
{
	UNIXTIME				ulStartTime;
	UNIXTIME				ulDuration;
	HUINT8					ucPRNum;
	fsatDxPvr_ParentalRating_t	astParentalRating[fsatDxPVR_PARENTAL_RATING_MAX];

	HUINT8		ucContentNibble;				// genre
	HUINT8		ucUserNibble;					// genre

	HCHAR		aucText[fsatDxPVR_EVENT_TEXT+1];	// description
	HCHAR		aucSeriesId[DxPVR_TVANYTIME_CRID_TOTAL_LEN];

	fsatDxPvr_EvtComponent_t stComponent[fsatDxEPG_COMPONENT_DESC_MAX];
} fsatDxRecList_EVTData_t;


typedef struct
{
	HCHAR	aucDescription[fsatDxPVR_EVENT_TEXT+1];	// ext evt description
	HCHAR	aucItem[fsatDxPVR_EVENT_TEXT+1]; 		// ext evt item
} fsatDxRecList_Ext_ItemData_t;


typedef	struct
{
	HUINT8		ucDesNum;
	HUINT8		ucLastDesNum;
	HCHAR		szIso639LangCode[fsatDxPVR_LANG_ISO_639_LEN+1];
	HUINT8		ucItemNum;
	fsatDxRecList_Ext_ItemData_t	astItem[fsatDxPVR_EXT_ITEM_MAX];
} fsatDxRecList_ExtEvt_t;

typedef	struct
{
	HUINT32 ulPos;
	HCHAR 	szName[fsatDxRECLIST_BOOKMARK_NAME_LEN+1];
}fsatDxRecList_Bookmark_t;


typedef	struct
{
	HUINT32		ulContentId;
	HBOOL		bNotified;
	HCHAR		szUrl[fsatDxRECLIST_URL+1];
	HCHAR		szName[fsatDxRECLIST_NAME_LEN+1];
	HCHAR		szThumbnailPath[fsatDxRECLIST_URL+1];

	HUINT32		ulRecStartTime;
	HUINT32		ulRecDuration;
	HUINT32		ulRecOrgDuration;

	fsatDxRecStatus_e			eRecStatus;
	fsatDxRecEndReasonType_e	eRecEndReason;
	HBOOL		bScrambled;
	HBOOL		bEncrypted;
	HBOOL		bEventBased;
	HUINT16		usEventId;
	HBOOL		bSeriesRec;

	HUINT32		ulLastViewTime;
	HBOOL		bUserLock;
	HBOOL		bWillBeDeleted;
	HBOOL		bAudoDeleted;
	HBOOL		bPlayed;
	HBOOL		bBlocked;	// svc lock or pin lock or pr lock ...

	HUINT16		usNumOfBookmark;
	HCHAR				szSvcName[fsatDxRECLIST_NAME_LEN+1];
	HINT32				nSvcNo;
	HINT32				svcUId;
	fsatDxSvcType_e			eSvcType;
	fsatDxVideoType_e		eVideoType;
	fsatDxCasType_e			eCasType;

	fsatDxMETA_PvrAudioInfoList_t	stAudioInfoList;
	fsatDxMETA_PvrSbtlInfoList_t	stSbtlInfoList;

	// Event Information
	fsatDxRecList_EVTData_t 	stEvt;
	HUINT32					ulExtEvtCnt;
	HUINT32                 __CRC__;  //CHECK CRC

	// below data are encode & decoded as extension (due to size ;;) & op specified data
	fsatDxRecList_Bookmark_t	astBookmarks[fsatDxRECLIST_NUM_BOOKMARK];
	fsatDxRecList_ExtEvt_t		*pstExtEvts;
	HCHAR					aucExtText[fsatDxRECLIST_EXT_TEXT_BUFSIZE+1]; 	// ext evt text

	union
	{
#ifdef	CONFIG_OP_FREESAT
		fsatDxFreesatRecListData_t	freesat;
#else
		void *	__;
#endif
	} extension;
} fsatDxRecListData_t;


typedef	struct
{
	HUINT32		ulContentId;
	HBOOL		bNotified;
	HCHAR		szUrl[fsatDxRECLIST_URL+1];
	HCHAR		szName[fsatDxRECLIST_NAME_LEN+1];
	HCHAR		szThumbnailPath[fsatDxRECLIST_URL+1];

	HUINT32		ulRecStartTime;
	HUINT32		ulRecDuration;
	HUINT32		ulRecOrgDuration;

	fsatDxRecStatus_e			eRecStatus;
	fsatDxRecEndReasonType_e	eRecEndReason;
	HBOOL		bScrambled;
	HBOOL		bEncrypted;
	HBOOL		bEventBased;
	HUINT16		usEventId;
	HBOOL		bSeriesRec;

	HUINT32		ulLastViewTime;
	HBOOL		bUserLock;
	HBOOL		bWillBeDeleted;
	HBOOL		bAudoDeleted;
	HBOOL		bPlayed;
	HBOOL		bBlocked;	// svc lock or pin lock or pr lock ...

	HUINT16		usNumOfBookmark;
	HCHAR		szSvcName[fsatDxRECLIST_NAME_LEN+1];
	HINT32		nSvcNo;
	HINT32		svcUId;
	fsatDxSvcType_e			eSvcType;
	fsatDxVideoType_e		eVideoType;
	fsatDxCasType_e			eCasType;

	fsatDxMETA_PvrAudioInfoList_t	stAudioInfoList;
	fsatDxMETA_PvrSbtlInfoList_t	stSbtlInfoList;

	// Event Information
	fsatDxRecList_EVTData_t					stEvt;
	HUINT32								ulExtEvtCnt;
	HUINT32                 __CRC__;  //CHECK CRC
} fsatDxRecListBasic_t;






typedef struct
{
	/* LNB Parameters */
	HUINT32 					ulLnbFreq;				/**< LNB frequency 占실댐옙 CH_UNIVERSAL_LNB_FREQ(0xffffffff) */
	fsatDxSAT_LnbVoltage_e			lnbVoltage; 			/**< Voltage drop compensation */
} fsatDxLnbAntenna_t;



typedef struct
{
	/* LNB Parameters */
	HUINT32 						ulLnbFreq;				/**< LNB frequency CH_UNIVERSAL_LNB_FREQ(0xffffffff) */
	fsatDxSAT_LnbVoltage_e				lnbVoltage; 			/**< Voltage drop compensation */
	HBOOL							b22kTone;				/**< 22KHz Tone : On/Off */

	/* Diseqc Switch Parameters */
	fsatDxSAT_DiseqcVer_e				diseqcVersion;			/**< Diseqc Ver. 1.0, 1.1, 1.2, 1.3, 2.0 */
	fsatDxSAT_DiseqcInput_e				diseqcInput;			/**< Diseqc A, B, C, D */
	fsatDxSAT_ToneBurst_e				toneBurst;				/**< NONE, A, B */

	/* Diseqc 1.2, 1.3 motor parameters */
	HUINT32 						ucMotorPosition;		/**< for DiSEqC 1.2 */
	HINT32							sSatLonggitude; 		/**< for DiSEqC 1.3 */
} fsatDxDiseqcAntenna_t;

typedef struct
{
	/* LNB Parameters */
	HUINT32 							ulLnbFreq;				/**< LNB frequency CH_UNIVERSAL_LNB_FREQ(0xffffffff) */
	fsatDxSAT_LnbVoltage_e					lnbVoltage; 			/**< Voltage drop compensation */
	HBOOL								b22kTone;				/**< 22KHz tone ON/OFF */

	/* SCD Switch Parameters */
	fsatDxSAT_ScdInput_e 					scdInput;				/**< SCD diseqc input A, B */
	HUINT32 							ulScdUserBandNum;
	HUINT32 							ulScdUserBandFreq;
	HUINT32 							ulScdUserBandNum1;
	HUINT32 							ulScdUserBandFreq1;
} fsatDxScdAntenna_t;


typedef struct
{
	/* DVBS Parameters */
	HUINT32 							ulFrequency;			/**< Tuning Frequency, MHz(Unit) */
	HUINT32 							ulSymbolRate;			/**< Symbol Rate, KSymbol/sec */
	fsatDxSat_CodeRate_e 					fecCodeRate;			/**< FEC Code Rate */
	fsatDxSat_Polarization_e 				polarization;			/**< Polarity : Vertical/Horizontal*/

	/* DVBS2 Parameters */
	fsatDxSat_TransportSpec_e				transSpec;				/**< DVBS, DVBS2 */
	fsatDxSat_PskModulation_e				pskMod; 				/**< Modulation Type : QPSK, 8PSK, AUTO*/
	fsatDxSat_Pilot_e						pilot;					/**< Pilot On/Off/Auto */
	fsatDxSat_RollOff_e						rollOff;				/**< Roll off : 0.20, 0.25, 0.35 */

	HCHAR								satType[fsatDxANTINFO_NAME_LEN];
	HINT32								antuid;
} fsatDxSat_TuningTPInfo_t;


typedef struct
{
	fsatDxDeliveryType_e				deliveryType;

	HUINT32 						ulTunerGroupId;


	HUINT32 						ulFrequency;
	HUINT32 						ulSymbolRate;
	fsatDxCab_Constellation_e			constellation;
	fsatDxCab_Spectrum_e 				spectrum;
	HINT32							nTsId;
	HINT32							nNetId;
} fsatDxCabTuningInfo_t;


typedef struct
{
	HINT32							uid;
	HCHAR							satType[fsatDxANTINFO_NAME_LEN];
	HCHAR							satName[fsatDxANTINFO_NAME_LEN];
	fsatDxAntennaType_e				antType;

	union {
		fsatDxLnbAntenna_t			lnb;
		fsatDxDiseqcAntenna_t 		diseqc;
		fsatDxScdAntenna_t			scd;
	} info;
} fsatDxAntInfo_t;

typedef struct
{
	HUINT32 					ulPlpId;
	fsatDxTer_PreambleFormat_e		preambleFormat;
	fsatDxTer_Mixed_e				mixed;
	fsatDxTer_Pilot_Pattern_e		pilotPattern;
	fsatDxTer_ExtendedCarrier_e		extenedCarrier;
	fsatDxTer_PAPR_Reduction_e		PAPRreduction;
	HUINT32 					numPlpBlocks;
}fsatDxTer_T2_Param_t;


typedef struct
{
	fsatDxDeliveryType_e				deliveryType;
	HUINT32 						ulTunerGroupId;

	fsatDxAntennaType_e					antennaType;
	fsatDxSat_TuningTPInfo_t 			tuningInfo;
	fsatDxAntInfo_t					antInfo;
#if	0
	union
	{
		fsatDxLnbAntenna_t				lnb;
		fsatDxDiseqcAntenna_t 			diseqc;
		fsatDxScdAntenna_t				scd;
	} antInfo;
#endif
} fsatDxSatTuningInfo_t;


typedef struct
{
	fsatDxDeliveryType_e				deliveryType;
	HUINT32							ulTunerGroupId;

	HUINT32							ulFrequency;
	fsatDxTer_BandWidth_e				bandWidth;
	fsatDxTer_Constellation_e			constellation;
	fsatDxTer_Hierachy_e				hierachy;
	fsatDxTer_CodeRate_e				codeRate;
	fsatDxTer_TransMode_e				transMode;
	fsatDxTer_GuardInterval_e			guardInterval;
	fsatDxTer_Stream_e					stream;
	fsatDxTer_Offset_e					offset;
	/* DVB-T2 */
	fsatDxTer_System_e					system;
	fsatDxTer_T2_Param_t				t2param;
} fsatDxTerTuningInfo_t;

typedef struct
{
	fsatDxDeliveryType_e		eDeliType;
	HUINT32					ulTunerId;

	// TODO: we have to define..
} fsatDxNetTuneInfo_t;


typedef union
{
	fsatDxDeliveryType_e		eDeliType;
	fsatDxSatTuningInfo_t		sat;
	fsatDxTerTuningInfo_t		ter;
	fsatDxCabTuningInfo_t		cab;
	fsatDxNetTuneInfo_t			net;
} fsatDxTuneParam_t;

typedef struct
{
	HINT32				uid;
	HINT32				version;
	HINT32				netuid;
	HINT32				tsid;
	HINT32				onid;
	HINT32				tunerid;

	fsatDxDeliveryType_e	deliType;
	fsatDxTuneParam_t		tuningParam;

	union {
#ifdef	CONFIG_OP_FREESAT
		fsatDxFreesatTs_t	freesat;
#else
		void *	__;
#endif
	} ex;
    HUINT32                 __CRC__;  //CHECK CRC
} fsatDxTransponder_t;


typedef struct
{
	HINT32				uid;
	HINT32				version;
	HINT32				onid;
	HCHAR				name[fsatDxNAME_LEN];
	fsatDxDeliveryType_e	deliType;

	union {
#ifdef	CONFIG_OP_FREESAT
		fsatDxFreesatNet_t	freesat;
#else
		void *	__;
#endif
	} ex;
       HUINT32                 __CRC__;  //CHECK CRC
} fsatDxNetwork_t;

typedef struct
{
	HINT32				uid;
	HINT32				version;
	HCHAR				name[fsatDxNAME_LEN];

	union {
		void *	__;
	} ex;
     HUINT32                 __CRC__;  //CHECK CRC
} fsatDxProvider_t;

typedef struct
{
	HINT32				uid;
	HINT32				version;
	HINT32				id;
	HCHAR				name[fsatDxNAME_LEN];

	union {
#ifdef	CONFIG_OP_FREESAT
		fsatDxFreesatGroup_t	freesat;
#else
		void *	__;
#endif
	} ex;
    HUINT32                 __CRC__;  //CHECK CRC
} fsatDxGroup_t;

typedef struct
{
	HINT32				uid;
	HINT32				version;
	HINT32				id;
	HCHAR				name[fsatDxNAME_LEN];

	union {
		void *	__;
	} ex;
    HUINT32                 __CRC__;  //CHECK CRC
} fsatDxBouquet_t;


typedef union
{
	struct common{
		HINT32	uid;
		HINT32	version;
	} common;

	fsatDxService_t			service;
	fsatDxTransponder_t		transponder;
	fsatDxNetwork_t			network;
	fsatDxProvider_t		provider;
	fsatDxGroup_t			group;
	fsatDxBouquet_t			bouquet;
} fsatDxSvcTable_t;


#define	fsatDxNAMEOF(type)	#type

#define _______API_FUNCTIONS______________________________________________

fsatDxEvent_t *				fsatDxEvent_Decode (HxJSONBIN_t decoder, fsatDxEvent_t *type, HxJSON_t json);

fsatDxRecListData_t*		fsatDxRecData_DecodeBin (fsatDxRecListData_t *type, HxJSON_t json);
const HCHAR *			fsatDxRecData_EncodeBin (HxJSONBIN_t enc, const fsatDxRecListData_t *recData);
HERROR		            fsatDxRecData_Decode (fsatDxRecListData_t *Rec, HxJSON_t json);
HCHAR *			        fsatDxRecData_Encode (const fsatDxRecListData_t *Rec);

DxSCHEDULE_t *			fsatDxSchedule_DecodeBin (DxSCHEDULE_t *shcedule, HxJSON_t json);
const HCHAR *			fsatDxSchedule_EncodeBin (HxJSONBIN_t enc, const DxSCHEDULE_t *shcedule);
HERROR  			    fsatDxSchedule_Decode (DxSCHEDULE_t *shcedule, HxJSON_t json);
HCHAR *			        fsatDxSchedule_Encode (const DxSCHEDULE_t *shcedule);


fsatDxService_t *			fsatDxService_DecodeBin (fsatDxService_t *type, HxJSON_t json);
const HCHAR *			fsatDxService_EncodeBin (HxJSONBIN_t enc,const fsatDxService_t *type);
fsatDxService_t *	        fsatDxService_Decode(fsatDxService_t *service, HxJSON_t json);
HCHAR*                  fsatDxService_Encode(const fsatDxService_t *pvData);

fsatDxTransponder_t *		fsatDxTransponder_DecodeBin (fsatDxTransponder_t *type, HxJSON_t json);
const HCHAR *			fsatDxTransponder_EncodeBin (HxJSONBIN_t enc, const fsatDxTransponder_t *type);
fsatDxTransponder_t *	    fsatDxTransponder_Decode (fsatDxTransponder_t *transponder, HxJSON_t json);
HCHAR *			        fsatDxTransponder_Encode (const fsatDxTransponder_t *transponder);

fsatDxProvider_t *			fsatDxProvider_DecodeBin (fsatDxProvider_t *type, HxJSON_t json);
const HCHAR *			fsatDxProvider_EncodeBin (HxJSONBIN_t enc, const fsatDxProvider_t *type);
fsatDxProvider_t *			fsatDxProvider_Decode (fsatDxProvider_t *provider, HxJSON_t json);
HCHAR *			        fsatDxProvider_Encode (const fsatDxProvider_t *provider);

fsatDxGroup_t *				fsatDxGroup_DecodeBin (fsatDxGroup_t *type, HxJSON_t json);
const HCHAR *			fsatDxGroup_EncodeBin (HxJSONBIN_t enc, const fsatDxGroup_t *type);
fsatDxGroup_t *				fsatDxGroup_Decode (fsatDxGroup_t *group, HxJSON_t json);
HCHAR *			        fsatDxGroup_Encode (const fsatDxGroup_t *group);

fsatDxBouquet_t *			fsatDxBouquet_DecodeBin (fsatDxBouquet_t *type, HxJSON_t json);
const HCHAR *			fsatDxBouquet_EncodeBin (HxJSONBIN_t enc, const fsatDxBouquet_t *type);
fsatDxBouquet_t *			fsatDxBouquet_Decode(fsatDxBouquet_t *bouquet, HxJSON_t json);
HCHAR *			        fsatDxBouquet_Encode(const fsatDxBouquet_t *bouquet);

fsatDxNetwork_t *			fsatDxNetwork_DecodeBin (fsatDxNetwork_t *type, HxJSON_t json);
const HCHAR *			fsatDxNetwork_EncodeBin (HxJSONBIN_t enc, const fsatDxNetwork_t *type);
fsatDxNetwork_t *			fsatDxNetwork_Decode (fsatDxNetwork_t *network, HxJSON_t json);
HCHAR *			        fsatDxNetwork_Encode (const fsatDxNetwork_t *network);

#ifdef	SUPPORT_DENC_FILE
HCHAR*					fsatDxXXX_EncodeFile(HCHAR *objectName,  void * data,HINT32 size);
void*					fsatDxXXX_DecodeFile(HCHAR *objectName,  void * data ,HINT32 *size);
#endif
void				fsatDxService_Print (FILE *fp, const fsatDxService_t *t);
void				fsatDxTransponder_Print (FILE *fp, const fsatDxTransponder_t *t);
void				fsatDxProvider_Print (FILE *fp, const fsatDxProvider_t *t);
void				fsatDxGroup_Print (FILE *fp, const fsatDxGroup_t *t);
void				fsatDxBouquet_Print (FILE *fp, const fsatDxBouquet_t *t);
void				fsatDxNetwork_Print (FILE *fp, const fsatDxNetwork_t *t);

#ifdef	__cplusplus
}
#endif


#endif /* FSAT_TYPES_H__ */


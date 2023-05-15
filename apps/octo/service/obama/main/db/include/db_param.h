

#ifndef	__DB_PARAM_H__
#define	__DB_PARAM_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define DbParam_UNDEFINED			"undefined"

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eDB_PARAM_TYPE_Int		= 0,
	eDB_PARAM_TYPE_String,
	eDB_PARAM_TYPE_Enum,
	eDB_PARAM_TYPE_Bin,
	eDB_PARAM_TYPE_Max
} DbParam_Type_e;

/** @brief DB PARAM Item Enum */
/*	ITEM NAME, which includes Operator's Name or Specification Name is Not Allowed.
 *  Will Be Removed without Notification!!!!!
 *  e.g) eDB_PARAM_ITEM_KDG_XXX
 *  e.g) eDB_PARAM_ITEM_ZIGGO_XXX
 *  e.g) eDB_PARAM_ITEM_DTG_XXX
 *  e.g) eDB_PARAM_ITEM_NORDIG_XXX
 */
typedef enum
{
	eDB_PARAM_ITEM_UNKNOWN = 0,						/* 0  */
	eDB_PARAM_ITEM_OPERATOR_TYPE,					/* 1  */	/**< DxOperator_e		*/
	eDB_PARAM_ITEM_LANG_MAIN,						/* 2  */	/**< HxLANG_Id_e		*/
	eDB_PARAM_ITEM_LANG_EVENT,						/* 3  */	/**< HxLANG_Id_e		*/
	eDB_PARAM_ITEM_LANG_EVENT1,						/* 4  */	/**< HxLANG_Id_e		*/
	eDB_PARAM_ITEM_LANG_AUDIO0, 					/* 5  */	/**< HxLANG_Id_e		*/
	eDB_PARAM_ITEM_LANG_AUDIO1,						/* 6  */	/**< HxLANG_Id_e		*/
	eDB_PARAM_ITEM_LANG_AUDIO2, 					/* 7  */	/**< HxLANG_Id_e		*/
	eDB_PARAM_ITEM_LANG_AUDIO3, 					/* 8  */	/**< HxLANG_Id_e		*/
	eDB_PARAM_ITEM_LANG_AUDIO4, 					/* 9  */	/**< HxLANG_Id_e		*/
	eDB_PARAM_ITEM_LANG_AUDIO5, 					/* 10 */	/**< HxLANG_Id_e		*/
	eDB_PARAM_ITEM_LANG_AUDIO6, 					/* 11 */	/**< HxLANG_Id_e		*/
	eDB_PARAM_ITEM_LANG_AUDIO7, 					/* 12 */	/**< HxLANG_Id_e		*/
	eDB_PARAM_ITEM_LANG_AUDIO_UNDEFINED, 			/* 13 */	/**< HxLANG_Id_e		*/
	eDB_PARAM_ITEM_LANG_AUDIO_ORIGINAL,				/* 14 */	/**< HxLANG_Id_e		*/
	eDB_PARAM_ITEM_LANG_AUDIO_DEFAULT,				/* 15 */	/**< HxLANG_Id_e		*/
	eDB_PARAM_ITEM_LANG_SUBTITLE0,					/* 16 */	/**< HxLANG_Id_e		*/
	eDB_PARAM_ITEM_LANG_SUBTITLE1,					/* 17 */	/**< HxLANG_Id_e		*/
	eDB_PARAM_ITEM_LANG_SUBTITLE2,					/* 18 */	/**< HxLANG_Id_e		*/
	eDB_PARAM_ITEM_LANG_SUBTITLE3,					/* 19 */	/**< HxLANG_Id_e		*/
	eDB_PARAM_ITEM_LANG_SUBTITLE4,					/* 20 */	/**< HxLANG_Id_e		*/
	eDB_PARAM_ITEM_LANG_SUBTITLE5,					/* 21 */	/**< HxLANG_Id_e		*/
	eDB_PARAM_ITEM_LANG_SUBTITLE6,					/* 22 */	/**< HxLANG_Id_e		*/
	eDB_PARAM_ITEM_LANG_SUBTITLE7,					/* 23 */	/**< HxLANG_Id_e		*/
	eDB_PARAM_ITEM_LANG_SUBTITLE_UNDEFINED,			/* 24 */	/**< HxLANG_Id_e		*/
	eDB_PARAM_ITEM_LANG_SUBTITLE_ORIGINAL,			/* 25 */	/**< HxLANG_Id_e		*/
	eDB_PARAM_ITEM_LANG_SUBTITLE_DEFAULT,			/* 26 */	/**< HxLANG_Id_e		*/
	eDB_PARAM_ITEM_HARDOFHEARING,					/* 27 */	/**< HBOOL				*/
	eDB_PARAM_ITEM_MATURITYRATING,					/* 28 */	/**< DxRatingAge_e		*/
	eDB_PARAM_ITEM_SKIPFORWARDSEC,					/* 29 */	/**< HUINT32		ulSkipForwardSec	*/
	eDB_PARAM_ITEM_INSTANTREPLAYSEC,				/* 30 */	/**< HUINT32		ulInstantReplaySec	*/
	eDB_PARAM_ITEM_COUNTRY_ID, 						/* 31 */	/**< HxCountry_e	eCountryId 		*/
	eDB_PARAM_ITEM_AUDIO_DESCRIPTION,				/* 32 */	/**< HBOOL			bAudioDescription	*/
	eDB_PARAM_ITEM_PINCODE,							/* 33 */	/**< HCHAR  		*pszPinCode			*/
	eDB_PARAM_ITEM_PADDINGTIME_START,				/* 34 */	/**< HUINT32 ulPaddingTimeStart		*/
	eDB_PARAM_ITEM_PADDINGTIME_STOP,				/* 35 */	/**< HUINT32 ulPaddingTimeStop		*/
	eDB_PARAM_ITEM_TSR_ONOFF,						/* 36 */	/**< HBOOL bOn		*/
	eDB_PARAM_ITEM_AUTO_DELETE,						/* 37 */	/**< HBOOL bOn		*/
	eDB_PARAM_ITEM_STEREO_SELECT,					/* 38 */	/**< DxStereoSelect_e eStereoSelect		*/
	eDB_PARAM_ITEM_DUALMONO_SELECT,					/* 39 */	/**< DxDualMonoSelect_e eDualMonoSelect		*/
	eDB_PARAM_ITEM_HDMI_AUDIO_FORMAT,				/* 40 */	/**< DxDigitalAudioFormat_e eDigitalAudioFormat		*/
	eDB_PARAM_ITEM_SPDIF_AUDIO_FORMAT,				/* 41 */	/**< DxDigitalAudioFormat_e eDigitalAudioFormat		*/
	eDB_PARAM_ITEM_LIPSYNC_DELAY,					/* 42 */	/**< HUINT32 ulDelay_msec		*/
	eDB_PARAM_ITEM_LAST_VOLUME,						/* 43 */	/**< HUINT32 ulVolume		*/
	eDB_PARAM_ITEM_LAST_DECODER_VOLUME, 			/* 44 */			/**< HUINT32 ulVolume		*/
	eDB_PARAM_ITEM_INVISIBLE_SEARCH,				/* 45 */	/**< HBOOL bInvisibleSearch		*/
	eDB_PARAM_ITEM_POSTCODE,						/* 46 */	/**< 		*/
	eDB_PARAM_ITEM_ANT_CONN_TYPE,					/* 47 */	/**< DxAntennaConnection_e eAntConn	*/
	eDB_PARAM_ITEM_GUIDANCE_POLICY,					/* 48 */	/**< DxGuidancePolicy_e	eGPolicy */
	eDB_PARAM_ITEM_CHANNEL_SETUP_PIN,				/* 49 */	/**< HBOOL bSettup */
	eDB_PARAM_ITEM_ADULTCHANNELS,					/* 50 */	/**< DxAdultChannelType_e eAdultChannel */
	eDB_PARAM_ITEM_SUBTITLE_SETTING,				/* 51 */	/**< HBOOL	bSubtitle	*/
	eDB_PARAM_ITEM_HDMI_CEC_ENABLE,					/* 52 */	/**< HBOOL	bCecEnable */
	eDB_PARAM_ITEM_DAYLIGHT_SAVING,					/* 54 */	/**< HBOOL 	bEnable */
	eDB_PARAM_ITEM_VIRTUAL_SECURECW_SETTING,		/* 55 */	/**< HBOOL 	bOn	*/
	eDB_PARAM_ITEM_UPDATE_TIME_ON_SPECIFIC_TP,		/* 56 */	/**< HBOOL 	bEnable */
	eDB_PARAM_ITEM_2ND_INT_HDD_UUID,				/* 57 */	/**< HCHAR			*pszUuid */
	eDB_PARAM_ITEM_TVSCART_FORMAT,					/* 58 */	/**< DxScartFormat_e eScartFormat 	*/
	eDB_PARAM_ITEM_VCRSCART_FORMAT,					/* 59 */	/**< DxScartFormat_e eScartFormat 	*/
	eDB_PARAM_ITEM_SCART_PATH,						/* 60 */	/**< DxScartPath_e eScartPath 	*/
	eDB_PARAM_ITEM_TV_ASPECTRATIO,					/* 61 */	/**< DxAspectRatio_e eTvAspectRatio 	*/
	eDB_PARAM_ITEM_DFC_SELECT,						/* 62 */	/**< DxDfcSelect_e eDfcSelect	*/
	eDB_PARAM_ITEM_RESOLUTION_SELECT,				/* 63 */	/**< DxResolutionSelect_e eResolutionSelect	*/
	eDB_PARAM_ITEM_BACKGROUND_COLOR_R,				/* 64 */	/**< HUINT8	ucR	*/
	eDB_PARAM_ITEM_BACKGROUND_COLOR_G,				/* 65 */	/**< HUINT8	ucG	*/
	eDB_PARAM_ITEM_BACKGROUND_COLOR_B,				/* 66 */	/**< HUINT8 ucB */
	eDB_PARAM_ITEM_3D_SELECT,						/* 67 */	/**< DxHdmi3DSelect_e e3DSelect */
	eDB_PARAM_ITEM_SWUPDATE_VERSION,                /* 68 */
	eDB_PARAM_ITEM_IPSWUPDATE_VERSION,              /* 69 */
	eDB_PARAM_ITEM_GMT_OFFSET_MINUTES,				/* 70 */	/**	HUINT32 nGmtOffset  */ 	//	eDxSETUP_SYSTEM_GMT_OFFSET_MINUTES
	eDB_PARAM_ITEM_TIME_REGION_ID,					/* 71 */	/**	HUINT32 nItemValue	*/	//	eDxSETUP_SYSTEM_TIME_REGION_ID
	eDB_PARAM_ITEM_ANTSET_ANTENNATYPE,				/* 72 */	/**< DbSvc_UiAntType_e */ //	eDxSETUP_SYSTEM_ANTSET_ANTENNATYPE
	eDB_PARAM_ITEM_ANTSET_SAT_TUNER01,				/* 73 */	/**< (HINT32)ulAntIdx  */ //	eDxSETUP_SYSTEM_ANTSET_SAT_TUNER01
	eDB_PARAM_ITEM_ANTSET_SAT_TUNER02,				/* 74 */	/**< (HINT32)ulAntIdx  */ //	eDxSETUP_SYSTEM_ANTSET_SAT_TUNER02
	eDB_PARAM_ITEM_ANTSET_SAT_TUNER03,				/* 75 */	/**< (HINT32)ulAntIdx  */ //	eDxSETUP_SYSTEM_ANTSET_SAT_TUNER03
	eDB_PARAM_ITEM_SRCH_TPSELECTION,				/* 76 */	/**< Binary    */ 			//	eDxSETUP_SYSTEM_SRCH_TPSELECTION
	eDB_PARAM_ITEM_MOTORSET_LNBFREQ, 				/* 77 */	/**< (HUINT32)ulLnbFreq	 */ //	eDxSETUP_SYSTEM_MOTORSET_LNBFREQ
	eDB_PARAM_ITEM_MOTORSET_LNBPOWER,				/* 78 */	/**< (HBOOL)bLnbPower	 */ //	eDxSETUP_SYSTEM_MOTORSET_LNBPOWER
	eDB_PARAM_ITEM_MOTORSET_MOTORTYPE,				/* 79 */	/**< DxSAT_DiseqcVer_e */	//	eDxSETUP_SYSTEM_MOTORSET_MOTORTYPE
	eDB_PARAM_ITEM_POSITIONSET_SATTYPE,				/* 80 */	/**< SVC_SatType_t */		//	eDxSETUP_SYSTEM_POSITIONSET_SATTYPE
	eDB_PARAM_ITEM_POSITIONSET_MOTORDRIVE,			/* 81 */	/**< CH_MotorDriveMode_t */	//	eDxSETUP_SYSTEM_POSITIONSET_MOTORDRIVE
	eDB_PARAM_ITEM_POSITIONSET_DRIVESIZE,			/* 82 */	/**< HUINT32 ulDriveSize */	//	eDxSETUP_SYSTEM_POSITIONSET_DRIVESIZE
	eDB_PARAM_ITEM_LIMITSET_MOTORLIMIT,				/* 83 */	/**< HBOOL bLimitOnOff */	//	eDxSETUP_SYSTEM_LIMITSET_MOTORLIMIT
	eDB_PARAM_ITEM_LIMITSET_LIMITPOS,				/* 84 */	/**< HBOOL bLimitOnOff */	//	eDxSETUP_SYSTEM_LIMITSET_LIMITPOS
	eDB_PARAM_ITEM_SAT_TUNER_SELECTION,				/* 85 */	/**< HUINT32 ulTunerId */	//	eDxSETUP_SYSTEM_SAT_TUNER_SELECTION
	eDB_PARAM_ITEM_SAT_SRCH_ALL_ANT,				/* 86 */	/**< HBOOL bSrchAllAnt */	//	eDxSETUP_SYSTEM_SAT_SRCH_ALL_ANT
	eDB_PARAM_ITEM_SAT_TRANSPONDER,					/* 87 */	/**< HUINT32 ulTpIndex */	//  eDxSETUP_SYSTEM_SAT_TRANSPONDER
	eDB_PARAM_ITEM_SAT_NETWORK_SRCH,				/* 88 */	/**< HUINT32 ulNetwork */	//	eDxSETUP_SYSTEM_SAT_NETWORK_SRCH
	eDB_PARAM_ITEM_SAT_CAFREE_TYPE,					/* 89 */	/**< HUINT32 ulCasType */	//	eDxSETUP_SYSTEM_SAT_CAFREE_TYPE
	eDB_PARAM_ITEM_SAT_CHANNEL_TYPE,				/* 90 */	/**< HUINT32 ulChType */	//	eDxSETUP_SYSTEM_SAT_CHANNEL_TYPE
	eDB_PARAM_ITEM_SAT_SEARCH_MODE,					/* 91 */	/**< HUINT32 ulSearchMode *///	eDxSETUP_SYSTEM_SAT_SEARCH_MODE
	eDB_PARAM_ITEM_TER_SRCH_CHANNEL,				/* 92 */	/**< HUINT8	ucChanNum */	//	eDxSETUP_SYSTEM_TER_SRCH_CHANNEL
	eDB_PARAM_ITEM_TER_SRCH_FREQUENCY,				/* 93 */	/**< HUINT32 ulFreq	*/		//	eDxSETUP_SYSTEM_TER_SRCH_FREQUENCY
	eDB_PARAM_ITEM_TER_SRCH_BANDWIDTH,				/* 94 */	/**< DxTer_BandWidth_e	*/	//	eDxSETUP_SYSTEM_TER_SRCH_BANDWIDTH
	eDB_PARAM_ITEM_TER_SRCH_NETSEARCH,				/* 95 */	/**< HUINT32 ulNetwork	*/	//	eDxSETUP_SYSTEM_TER_SRCH_NETSEARCH
	eDB_PARAM_ITEM_TER_SRCH_NETWORKID,				/* 96 */	/**< HUINT32 ulNetId */		//	eDxSETUP_SYSTEM_TER_SRCH_NETWORKID
	eDB_PARAM_ITEM_TER_SRCH_POWER,					/* 97 */	/**< (HUINT8)nVal */		//	eDxSETUP_SYSTEM_TER_SRCH_POWER
	eDB_PARAM_ITEM_TER_SRCH_TRANSMISSION,			/* 98 */	/**< DxTer_System_e */		//	eDxSETUP_SYSTEM_TER_SRCH_TRANSMISSION
	eDB_PARAM_ITEM_TER_SRCH_AUTO_CH_NUMBERING,		/* 99 */	/**< HBOOL bAutoChNumbering */	//	eDxSETUP_SYSTEM_TER_SRCH_AUTO_CH_NUMBERING
	eDB_PARAM_ITEM_CAB_SRCH_TUNERID,				/* 100*/	/**< HUINT32 ulTunerId */	//	eDxSETUP_SYSTEM_CAB_SRCH_TUNERID
	eDB_PARAM_ITEM_CAB_SRCH_NETSEARCH,				/* 101*/	/**< HUINT32 ulNetwork */	//	eDxSETUP_SYSTEM_CAB_SRCH_NETSEARCH
	eDB_PARAM_ITEM_CAB_SRCH_CAFREE_TYPE,			/* 102*/	/**< HUINT32 ulCasType */	//	eDxSETUP_SYSTEM_CAB_SRCH_CAFREE_TYPE
	eDB_PARAM_ITEM_CAB_SRCH_CHANNEL_TYPE,			/* 103*/	/**< HUINT32 ulChType */	//	eDxSETUP_SYSTEM_CAB_SRCH_CHANNEL_TYPE
	eDB_PARAM_ITEM_CAB_SRCH_FREQUENCY,				/* 104*/	/**< HUINT32 ulFreq */		//	eDxSETUP_SYSTEM_CAB_SRCH_FREQUENCY
	eDB_PARAM_ITEM_CAB_SRCH_SYMBOL_RATE,			/* 105*/	/**< HUINT32 ulSR */		//	eDxSETUP_SYSTEM_CAB_SRCH_SYMBOL_RATE
	eDB_PARAM_ITEM_CAB_SRCH_MODULATION,				/* 106*/	/**< HUINT32 ulCon */		//	eDxSETUP_SYSTEM_CAB_SRCH_MODULATION
	eDB_PARAM_ITEM_CAB_SRCH_NETWORK_ID,				/* 107*/	/**< HUINT32 ulNetworkId */	//	eDxSETUP_SYSTEM_CAB_SRCH_NETWORK_ID
	eDB_PARAM_ITEM_CAB_LINEAR_SRCH_STARTFREQUENCY,	/* 108*/	/**< HUINT32 ulFreq */		//	eDxSETUP_SYSTEM_CAB_LINEAR_SRCH_STARTFREQUENCY
	eDB_PARAM_ITEM_CAB_LINEAR_SRCH_STOPFREQUENCY,	/* 109*/	/**< HUINT32 pulFreq */		//	eDxSETUP_SYSTEM_CAB_LINEAR_SRCH_STOPFREQUENCY
	eDB_PARAM_ITEM_CAB_LINEAR_SRCH_STEP,			/* 110*/	/**< HUINT32 ulStep */		//	eDxSETUP_SYSTEM_CAB_LINEAR_SRCH_STEP
	eDB_PARAM_ITEM_CAB_LINEAR_SRCH_MODULATION,		/* 111*/	/**< HUINT32 ulCon */		//	eDxSETUP_SYSTEM_CAB_LINEAR_SRCH_MODULATION
	eDB_PARAM_ITEM_CAB_LINEAR_SRCH_SYMBOL_RATE,		/* 112*/	/**< HUINT32 ulSR */		//	eDxSETUP_SYSTEM_CAB_LINEAR_SRCH_SYMBOL_RATE
	eDB_PARAM_ITEM_CAB_SRCH_NETWORK_SRCH_MODE,		/* 113*/	/**< HUINT32 ulSearchMode *///	eDxSETUP_SYSTEM_CAB_SRCH_NETWORK_SRCH_MODE
	eDB_PARAM_ITEM_MAN_TP_FREQUENCY,				/* 114*/	/**< HUINT32 ulFrequency */	//	eDxSETUP_SYSTEM_MAN_TP_FREQUENCY
	eDB_PARAM_ITEM_MAN_TP_SYMBOLRATE,				/* 115*/	/**< HUINT32 ulSymbolRate  *///	eDxSETUP_SYSTEM_MAN_TP_SYMBOLRATE
	eDB_PARAM_ITEM_MAN_TP_POLARIZATION,				/* 116*/	/**< DxSat_Polarization_e *///	eDxSETUP_SYSTEM_MAN_TP_POLARIZATION
	eDB_PARAM_ITEM_MAN_TP_FEC,						/* 117*/	/**< DxSat_CodeRate_e */	//	eDxSETUP_SYSTEM_MAN_TP_FEC
	eDB_PARAM_ITEM_MAN_TP_TRANSSPEC,				/* 118*/	/**< DxSat_TransportSpec_e *///	eDxSETUP_SYSTEM_MAN_TP_TRANSSPEC
	eDB_PARAM_ITEM_MAN_TP_MODULATION,				/* 119*/	/**< DxSat_PskModulation_e *///	eDxSETUP_SYSTEM_MAN_TP_MODULATION
	eDB_PARAM_ITEM_MAN_TP_PILOT,					/* 120*/	/**< DxSat_Pilot_e */		//	eDxSETUP_SYSTEM_MAN_TP_PILOT
	eDB_PARAM_ITEM_MOTOR_USER_LONGITUDE,			/* 121*/	/**< HINT32 nLongitude */	//	eDxSETUP_SYSTEM_MOTOR_USER_LONGITUDE
	eDB_PARAM_ITEM_TVTV_ENABLE,						/* 122*/	/**< HBOOL bTvtvEnable */	//	eDxSETUP_SYSTEM_TVTV_ENABLE
	eDB_PARAM_ITEM_TVTV_DOWNTYPE,					/* 123*/	/**< HBOOL bUpdateTime */	//	eDxSETUP_SYSTEM_TVTV_DOWNTYPE
	eDB_PARAM_ITEM_MY_LONGITUDE,					/* 124*/	/**< HINT32 nLongitude */	//	eDxSETUP_USER_MY_LONGITUDE
	eDB_PARAM_ITEM_MY_LATITUDE,						/* 125*/	/**< HINT32 nLatitude */	//	eDxSETUP_USER_MY_LATITUDE
	eDB_PARAM_ITEM_CUR_SVC,							/* 126*/	/**< Handle_t hCurSvc */	//	eDxSETUP_USER_CUR_SVC
	eDB_PARAM_ITEM_USER_PINCODE,					/* 127*/	/**< Text */				//	eDxSETUP_USER_PINCODE
	eDB_PARAM_ITEM_REGION_CODE,						/* 128*/	/**< HUINT32 sg_RegionCode *///	eDxSETUP_USER_REGION_CODE
	eDB_PARAM_ITEM_BOUQUET_ID,						/* 129*/	/**< HUINT32 sg_BouquetId *///	eDxSETUP_USER_BOUQUET_ID
	eDB_PARAM_ITEM_HOME_TP_INFO_1,					/* 130*/	/**< Binary */				//	eDxSETUP_USER_HOME_TP_INFO_1
	eDB_PARAM_ITEM_HOME_TP_INFO_2,					/* 131*/	/**< Binary */				//	eDxSETUP_USER_HOME_TP_INFO_2
	eDB_PARAM_ITEM_HOME_TP_INFO_3,					/* 132*/	/**< Binary */				//	eDxSETUP_USER_HOME_TP_INFO_3
	eDB_PARAM_ITEM_BAT_INFO,						/* 133*/	/**< HUINT32 nNewCrcOfCrc *///	eDxSETUP_USER_BAT_INFO
	eDB_PARAM_ITEM_LAST_TV_SVC,						/* 134*/	/**< Handle_t hSvc */		//	eDxSETUP_USER_LAST_TV_SVC
	eDB_PARAM_ITEM_LAST_RADIO_SVC,					/* 135*/	/**< Handle_t hSvc */		//	eDxSETUP_USER_LAST_RADIO_SVC
	eDB_PARAM_ITEM_UPC_7DAY_TIMEDATA,				/* 136*/	/**< UNIXTIME ulSavedTime *///	eDxSETUP_USER_UPC_7DAY_TIMEDATA
	eDB_PARAM_ITEM_OTA_DETECT_INFO,					/* 137*/	/**< Binary */				//	eDxSETUP_USER_OTA_DETECT_INFO
	eDB_PARAM_ITEM_OTA_FLAG,						/* 138*/	/**< (HINT32)TRUE */		//	eDxSETUP_USER_OTA_FLAG
	eDB_PARAM_ITEM_OTA_USERCANCELLED_VER,			/* 139*/	/**< HUINT32 ulCanceledVer *///	eDxSETUP_USER_OTA_USERCANCELLED_VER
	eDB_PARAM_ITEM_OTA_USERCANCELLED_TYPE,			/* 140*/	/**< HUINT16 usCanceledType *///eDxSETUP_USER_OTA_USERCANCELLED_TYPE
	eDB_PARAM_ITEM_OTA_TSINFO,						/* 141*/	/**< Binary */				//	eDxSETUP_USER_OTA_TSINFO
	eDB_PARAM_ITEM_OTA_OTAINFO,						/* 142*/	/**< Binary */				//	eDxSETUP_USER_OTA_OTAINFO
	eDB_PARAM_ITEM_OTA_DOWNLOADEDINFO,				/* 143*/	/**< Binary */				//	eDxSETUP_USER_OTA_DOWNLOADEDINFO
	eDB_PARAM_ITEM_OTA_EMERGENCYINFO,				/* 144*/	/**< Binary */				//	eDxSETUP_USER_OTA_EMERGENCYINFO
	eDB_PARAM_ITEM_OTA_SCHEDULEINFO,				/* 145*/	/**< Binary */				//	eDxSETUP_USER_OTA_SCHEDULEINFO
	eDB_PARAM_ITEM_OTA_RETRYCOUNT,					/* 146*/	/**< HINT32	nCount */		//	eDxSETUP_USER_OTA_RETRYCOUNT
	eDB_PARAM_ITEM_OTA_RETRYVER,					/* 147*/	/**< HUINT32 ulRetryOtaVer *///	eDxSETUP_USER_OTA_RETRYVER
	eDB_PARAM_ITEM_OTA_RETRYTYPE,					/* 148*/	/**< HUINT16 usRetryOtaType *///eDxSETUP_USER_OTA_RETRYTYPE
	eDB_PARAM_ITEM_OTA_WAKEUP_FLAG,					/* 149*/	/**< HINT32		nValue */	//	eDxSETUP_USER_OTA_WAKEUP_FLAG
	eDB_PARAM_ITEM_ANTENNA_SORT_METHOD,				/* 150*/	/**< MgrTplist_SatSortMethod_e */// eDxSETUP_USER_ANTENNA_SORT_METHOD
	eDB_PARAM_ITEM_PVR_DEFAULT_HDD_UUID,			/* 151*/	/**< Text */				//	eDxSETUP_USER_PVR_DEFAULT_HDD_UUID
	eDB_PARAM_ITEM_HOME_CHANNEL,					/* 152*/	/**< Binary */				//	eDxSETUP_USER_HOME_CHANNEL
	eDB_PARAM_ITEM_REGULAR_AUTO_UPDATE_SUCCESS,		/* 153*/	/**< Regular_Auto_Update_Success_t */	//	eDxSETUP_SYSTEM_REGULAR_AUTO_UPDATE_SUCCESS
	eDB_PARAM_ITEM_ADULT_CHANNEL,					/* 154*/	/**< int value */						//	eDxSETUP_SYSTEM_ADULT_CHANNEL
	eDB_PARAM_ITEM_GUIDANCE_CHANNEL_GENRE,			/* 155*/	/**< HINT32	nVal */						//	eDxSETUP_SYSTEM_GUIDANCE_CHANNEL_GENRE
	eDB_PARAM_ITEM_NIT_TS_LOOP_CRC32,				/* 156*/	/**< HUINT32 ulNewCrc32 */				//	eDxSETUP_USER_NIT_TS_LOOP_CRC32
	eDB_PARAM_ITEM_FAV_CHNUM_ORDER,					/* 157*/	/**< HINT32 nValue */					//	eDxSETUP_SYSTEM_FAV_CHNUM_ORDER
	eDB_PARAM_ITEM_DYNAMIC_UPDATE_SETTING,			/* 158*/	/**< HINT32 nItemValue */				//	eDxSETUP_SYSTEM_DYNAMIC_UPDATE_SETTING
	eDB_PARAM_ITEM_CABLE_SEARCH_MODE,				/* 159*/	/**< HUINT32 ulSearchMode */			//	eDxSETUP_SYSTEM_CABLE_SEARCH_MODE
	eDB_PARAM_ITEM_SI_MODE,							/* 160*/	/**< HUINT32 ulSearchMode */			//	eDxSETUP_USER_FSAT_SI_MODE		// Factory Item
	eDB_PARAM_ITEM_SVCLISTID,						/* 161*/	// eDxSETUP_SYSTEM_SES_SVCLISTID,
	eDB_PARAM_ITEM_SVCLISTVER,						/* 162*/	// eDxSETUP_SYSTEM_SES_SVCLISTVER,
	eDB_PARAM_ITEM_SVCLISTNAME,						/* 163*/	// eDxSETUP_SYSTEM_SES_SVCLISTNAME,
	eDB_PARAM_ITEM_TVNUM,							/* 164*/	// eDxSETUP_SYSTEM_SES_TVNUM,
	eDB_PARAM_ITEM_RADIONUM,						/* 165*/	// eDxSETUP_SYSTEM_SES_RADIONUM,
	eDB_PARAM_ITEM_LASTCHECKEDTIME,					/* 166*/	// eDxSETUP_SYSTEM_SES_LASTCHECKEDTIME,
	eDB_PARAM_ITEM_LASTUPDATEDTIME,					/* 167*/	// eDxSETUP_SYSTEM_SES_LASTUPDATEDTIME,
	eDB_PARAM_ITEM_REPLACESVC_NUM,					/* 168*/	// eDxSETUP_SYSTEM_SES_REPLACESVC_NUM,
	eDB_PARAM_ITEM_REPLACESVC_ITEMS,				/* 169*/	// eDxSETUP_SYSTEM_SES_REPLACESVC_ITEMS,
	eDB_PARAM_ITEM_NEWSVC_NUM,						/* 170*/	// eDxSETUP_SYSTEM_SES_NEWSVC_NUM,
	eDB_PARAM_ITEM_NEWSVC_ITEMS,					/* 171*/	// eDxSETUP_SYSTEM_SES_NEWSVC_ITEMS,
	eDB_PARAM_ITEM_SCINSERTED,						/* 172*/	// eDxSETUP_SYSTEM_SES_SCINSERTED,
	eDB_PARAM_ITEM_REPLACELATER_NUM,				/* 173*/	// eDxSETUP_SYSTEM_SES_REPLACELATER_NUM,
	eDB_PARAM_ITEM_REPLACELATER_ITEMS,				/* 174*/	// eDxSETUP_SYSTEM_SES_REPLACELATER_ITEMS,
	eDB_PARAM_ITEM_COLDBOOT_RESULT,					/* 175*/	// eDxSETUP_SYSTEM_SES_COLDBOOT_RESULT,
	eDB_PARAM_ITEM_SATREC_DETECTINFO,				/* 176*/	// eDxSETUP_SYSTEM_SES_SATREC_DETECTINFO,
	eDB_PARAM_ITEM_SATREC_FBDVERSION,				/* 177*/	// eDxSETUP_SYSTEM_SES_SATREC_FBDVERSION,
	eDB_PARAM_ITEM_SATREC_BDVERSION,				/* 178*/	// eDxSETUP_SYSTEM_SES_SATREC_BDVERSION,
	eDB_PARAM_ITEM_SATREC_RECEIVERID,				/* 179*/	///**< HINT32	receverid */
	eDB_PARAM_ITEM_SATREC_UPDATE_INTERVAL,			/* 180*/	//**< HINT32	receverid */
	eDB_PARAM_ITEM_OLDPREPAIDRIGHT,					/* 181*/	// eDxSETUP_SYSTEM_SES_OLDPREPAIDRIGHT,
	eDB_PARAM_ITEM_EM36_OPEN,						/* 182*/	// eDxSETUP_SYSTEM_SES_EM36_OPEN,
	eDB_PARAM_ITEM_SC_OLD_SN,						/* 183*/	// eDxSETUP_SYSTEM_SES_SC_OLD_SN,
	eDB_PARAM_ITEM_FAVCHLISTNUMBERING,              /* 184*/
	eDB_PARAM_ITEM_DESCRAMBLEONOFF, 				/* 185*/	/**< HBOOL bOn		*/
	eDB_PARAM_ITEM_BATUPDATEONOFF,					/* 186*/	//
	eDB_PARAM_ITEM_NITLCNUPDATEONOFF,				/* 187*/	//
	eDB_PARAM_ITEM_ASO_NOTIFY_FLAG,					/* 188*/	/* 	ASO : Analog Switch Off - HBOOL bNotify */	// eDxSETUP_USER_ASO_NOTIFY_FLAG
	eDB_PARAM_ITEM_ASO_NETWORK_CHANGE_INFO, 		/* 189*/	/*< Binary */									// eDxSETUP_USER_ASO_NETWORK_CHANGE_INFO
	eDB_PARAM_ITEM_ASO_NETWORK_MSG_INFO, 			/* 190*/	/*< Binary */									// eDxSETUP_USER_ASO_ASO_NETWORK_MSG_INFO
	eDB_PARAM_ITEM_ASO_LAST_SCAN_TIME,				/* 191*/	/** UNIXTIME ulLastScanTime */					// eDxSETUP_USER_ASO_LAST_SCAN_TIME
	eDB_PARAM_ITEM_REGION_NAMEINFO_CNT,				/* 192*/	/* HUINT32 ulCount */							// eDxSETUP_USER_REGION_NAMEINFO_CNT
	eDB_PARAM_ITEM_REGION_NAMEINFO,					/* 193*/	/* <Binary */									// eDxSETUP_USER_REGION_NAMEINFO_PTR
	eDB_PARAM_ITEM_DISPLAY_CRID_SETTING, 			/* 194*/	/* <HBOOL bOnOff >*/							// eDxSETUP_SYSTEM_DISPLAY_CRID_SETTING
	eDB_PARAM_ITEM_ANTENNA_POWER,					/* 195*/	/**< HINT32 nItemValue */						// eDxSETUP_SYSTEM_ANTENNA_POWER
	eDB_PARAM_ITEM_LAST_MIC_VOLUME,					/* 196*/	/**< HUINT32 ulMicVolume		*/
	eDB_PARAM_ITEM_LIVESTREAM_PRIORITY,				/* 197*/	/**< DxLiveStreamPriority_e	*/
	eDB_PARAM_ITEM_AUTOMATIC_POWERDOWN,				/* 198*/	/** HUINT32 */								//eDxSETUP_SYSTEM_AUTOMATIC_POWERDOWN
	eDB_PARAM_ITEM_PRIORITY_HDMIOUTPUT,				/* 199*/	/* HBOOL */

    eDB_PARAM_ITEM_HDMI_CEC_VOLUME_ENABLE,			/* 53 */	/**< HBOOL	bVolumeControlOfCec */
	eDB_PARAM_ITEM_MAX                              /* 200*/
} DbParam_Item_e;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

HERROR	DB_PARAM_LoadDefault(HBOOL bKeepFactoryValue); /**< TRUE면 생산 공정에서 설정된 값들 유지, FALSE면 factory reset */
void	DB_PARAM_Close(void);
HERROR	DB_PARAM_Sync(void);
HERROR	DB_PARAM_RemoveCache(void);

HERROR	DB_PARAM_SetItem (DbParam_Item_e eItem, HUINT32 ulData, HUINT32 ulParam);
HERROR	DB_PARAM_GetItem (DbParam_Item_e eItem, HUINT32 *pulData, HUINT32 ulParam);
HERROR	DB_PARAM_GetItemType (DbParam_Item_e eItem, DbParam_Type_e *peDataType);
HERROR	DB_PARAM_GetDefaultItem (DbParam_Item_e eItem, HUINT32 *pulData);

HBOOL	DB_PARAM_KeepingItemFinished (void);
HBOOL	DB_PARAM_KeepingItemAllTableRemoved (void *pvKeepingItemContext, HUINT32 ulType);
HERROR	DB_PARAM_KeepingItemWrite (void *pvKeepingItemContext);


#endif /* __DB_PARAM_H__ */


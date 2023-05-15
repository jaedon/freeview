/* $Header: $ */

#ifndef __SILIB_DESC_H__
#define __SILIB_DESC_H__

#ifndef _____INCLUDED_ONLY_IN_SILIB_H____
#error The file [sxdes.h] shall not be included except silib.h
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Enumerations         ********************/
/*******************************************************************/
typedef enum
{
	eSIxERR_OK		= ERR_OK,
	eSIxERR_FAIL,
	eSIxERR_INVALID,
	eSIxERR_SOURCE_NULL,
	eSIxERR_TARGET_NULL,
	eSIxERR_NOT_ENOUGH_MEMORY,
	eSIxERR_NOT_SUPPORT,
} SIxError_e;

typedef enum
{
	eSIxSPEC_DVB			= 0,	/** < ETS 300 468의 descriptor의 tag */
	eSIxSPEC_DVBEXT,				/** < ETS 300 468의 v1.10.1 (2009-07) 버전에서 추가된 extension descriptor의 tag */
	eSIxSPEC_SSUUNT,				/** < TS 102 006에 정의된 SSU UNT desctiptor tag */
	eSIxSPEC_ARIB,
	eSIxSPEC_FSAT,
	eSIxSPEC_UKDTT,					/** < UK DTT */
	eSIxSPEC_SKAPA,
	eSIxSPEC_ASTRA,
	eSIxSPEC_NORDIG,
	eSIxSPEC_ZIGGO,
	eSIxSPEC_BOXER,
	eSIxSPEC_SES,
	eSIxSPEC_ERTELECOM,
	eSIxSPEC_AUSDTT,				/** < Australia DTT */
	eSIxSPEC_ITADTT,				/** < Italy DTT */
	eSIxSPEC_MBC,					/** < Middel-East, North-Africa MBC */

/*	Please add an item or items obove this line. 			*/
/*	Otherwise, items will be removed without being noticed.	*/
	eSIxSPEC_MAX
} SIxSpec_e;

typedef enum
{
	// 여기에는 ETS 300 468에까지 정의해 놓은 descriptor만을 등록시킨다.
	// 0x80 ~ 0xFE 에 해당하는 private descriptor는 각자의 사양에서 직접 등록시키도록 합시다.

	/** < ISO/IEC 13818-1에 정의된 descriptor tag */
 	eSIxDESCTAG_VIDEO_STREAM					= 0x02,	/** < Video Stream Descriptor */
 	eSIxDESCTAG_AUDIO_STREAM					= 0x03,	/** < Audio Stream Descriptor */
 	eSIxDESCTAG_HIERARCHY						= 0x04,	/** < Hierarchy Descriptor */
 	eSIxDESCTAG_REGISTRATION					= 0x05,	/** < Registration Descriptor */
 	eSIxDESCTAG_DATA_STREAM_ALIGNMENT			= 0x06,	/** < Data Stream Alignment Descriptor */
 	eSIxDESCTAG_TARGET_BACKGROUND_GRID			= 0x07,	/** < Target Background Grid Descriptor */
 	eSIxDESCTAG_VIDEO_WINDOW					= 0x08,	/** < Video Window Descriptor */
 	eSIxDESCTAG_CA								= 0x09,	/** < CA Descriptor */
 	eSIxDESCTAG_ISO_639_LANGUAGE				= 0x0A,	/** < ISO 639 Language Descriptor */
 	eSIxDESCTAG_SYSTEM_CLOCK					= 0x0B,	/** < System Clock Descriptor */
 	eSIxDESCTAG_MULTIPLEX_BUFFER_UTILIZATION	= 0x0C,	/** < Multiplex Buffer Utilization Descriptor */
 	eSIxDESCTAG_COPYRIGHT						= 0x0D,	/** < Copyright Descriptor */
 	eSIxDESCTAG_MAXIMUM_BITRATE				= 0x0E,	/** < Maximum Bitrate Descriptor */
 	eSIxDESCTAG_PRIVATE_DATA_INDICATOR			= 0x0F,	/** < Private Data Indicator Descriptor */
 	eSIxDESCTAG_SMOOTHING_BUFFER				= 0x10,	/** < Smoothing Buffer Descriptor */
 	eSIxDESCTAG_STD							= 0x11,	/** < STD Descriptor */
 	eSIxDESCTAG_CAROUSEL_ID					= 0x13,	/** < Carousel ID Descriptor */
 	eSIxDESCTAG_ASSOCIATION_TAG				= 0x14,	/** < Association Tag Descriptor */
 	eSIxDESCTAG_DEFERRED_ASSOCIATION_TAG		= 0x15,	/** < Deferred Association Tag Descriptor */
	eSIxDESCTAG_AVC_VIDEO						= 0x28,	/** < AVC Video Descriptor */
	/** < ETS 300 468에 정의된 descriptor tag */
 	eSIxDESCTAG_NETWORK_NAME					= 0x40,	/** < Network Name Descriptor */
 	eSIxDESCTAG_SERVICE_LIST					= 0x41,	/** < Service List Descriptor */
 	eSIxDESCTAG_STUFFING						= 0x42,	/** < Stuffing Descriptor */
 	eSIxDESCTAG_SATELLITE_DELIVERY_SYSTEM		= 0x43,	/** < Satellite Delivery System Descriptor */
 	eSIxDESCTAG_CABLE_DELIVERY_SYSTEM			= 0x44,	/** < Cable Delivery System Descriptor */
 	eSIxDESCTAG_BOUQUET_NAME					= 0x47,	/** < Bouquet Name Descriptor */
 	eSIxDESCTAG_SERVICE						= 0x48,	/** < Service Descriptor */
 	eSIxDESCTAG_COUNTRY_AVAILABILITY			= 0x49,	/** < Country Availability Descriptor */
 	eSIxDESCTAG_LINKAGE						= 0x4A,	/** < Linkage Descriptor */
 	eSIxDESCTAG_NVOD_REFERENCE					= 0x4B,	/** < NVOD Reference Descriptor */
 	eSIxDESCTAG_TIME_SHIFTED_SERVICE			= 0x4C,	/** < Time Shifted Service Descriptor */
 	eSIxDESCTAG_SHORT_EVENT					= 0x4D,	/** < Short Event Descriptor */
 	eSIxDESCTAG_EXTENDED_EVENT					= 0x4E,	/** < Extended Event Descriptor */
 	eSIxDESCTAG_TIME_SHIFTED_EVENT				= 0x4F,	/** < Time Shifted Event Descriptor */
 	eSIxDESCTAG_COMPONENT						= 0x50,	/** < Component Descriptor */
 	eSIxDESCTAG_MOSAIC							= 0x51,	/** < Mosaic Descriptor */
 	eSIxDESCTAG_STREAM_IDENTIFIER				= 0x52,	/** < Stream Identifier Descriptor */
 	eSIxDESCTAG_CA_IDENTIFIER					= 0x53,	/** < CA Identifier Descriptor */
 	eSIxDESCTAG_CONTENT						= 0x54,	/** < Content Descriptor */
 	eSIxDESCTAG_PARENTAL_RATING				= 0x55,	/** < Parental Rating Descriptor */
 	eSIxDESCTAG_TELETEXT						= 0x56,	/** < Teletext Descriptor */
 	eSIxDESCTAG_TELEPHONE						= 0x57,	/** < Telephone Descriptor */
 	eSIxDESCTAG_LOCAL_TIME_OFFSET				= 0x58,	/** < Local Time Offset Descriptor */
 	eSIxDESCTAG_SUBTITLING						= 0x59,	/** < Subtitling Descriptor */
 	eSIxDESCTAG_TERRESTRIAL_DELIVERY_SYSTEM	= 0x5A,	/** < Terrestrial Delivery System Descriptor */
 	eSIxDESCTAG_MULTILINGUAL_NETWORK_NAME		= 0x5B,	/** < Multilingual Network Name Descriptor */
 	eSIxDESCTAG_MULTILINGUAL_BOUQUET_NAME		= 0x5C,	/** < Multilingual Bouquet Name Descriptor */
 	eSIxDESCTAG_MULTILINGUAL_SERVICE_NAME		= 0x5D,	/** < Multilingual Service Name Descriptor */
 	eSIxDESCTAG_MULTILINGUAL_COMPONENT			= 0x5E,	/** < Multilingual Component Descriptor */
 	eSIxDESCTAG_PRIVATE_DATA_SPECIFIER			= 0x5F,	/** < Private Data Specifier Descriptor */
 	eSIxDESCTAG_SERVICE_MOVE					= 0x60,	/** < Service Move Descriptor */
 	eSIxDESCTAG_SHORT_SMOOTHING_BUFFER			= 0x61,	/** < Short Smoothing Buffer Descriptor */
	eSIxDESCTAG_FREQUENCY_LIST					= 0x62,	/** < Frequency List Descriptor */
	eSIxDESCTAG_PARTIAL_TRANSPORT_STREAM		= 0x63, /** < Partial Transport Stream (TS) Descriptor */
	eSIxDESCTAG_DATA_BROADCAST					= 0x64,	/** < Data Boadcast Descriptor */
	eSIxDESCTAG_DATA_BROADCAST_ID				= 0x66,	/** < Data Boadcast Descriptor (NEW, 2001)*/
	eSIxDESCTAG_PDC							= 0x69,	/** < PDC Descriptor (300 468 v1.7.1) */
	eSIxDESCTAG_DOLBY_AC3						= 0x6A,	/** < AC3(Dolby) Descriptor */
	eSIxDESCTAG_ANCILLARY_DATA					= 0x6B,
	eSIxDESCTAG_CELL_LIST						= 0x6C,
	eSIxDESCTAG_CELL_FREQ_LIST					= 0x6D,
	eSIxDESCTAG_ANNOUNCEMENT_SUPPORT			= 0x6E,
	eSIxDESCTAG_APPLICATION_SIGNALLING			= 0x6F,	/** < Application Signalling Descriptor */
	eSIxDESCTAG_ADAPTATION_FIELD_DATA			= 0x70,
	eSIxDESCTAG_SVC_IDENTIFIER					= 0x71,
	eSIxDESCTAG_SVC_AVAILABILITY				= 0x72,
	eSIxDESCTAG_DEFAULT_AUTHORITY				= 0x73,	/** < Default Authority descriptor */
	eSIxDESCTAG_RELATED_CONTENT				= 0x74,
	eSIxDESCTAG_CONTENT_IDENTIFIER				= 0x76,	/** < Content identifier descriptor */
	eSIxDESCTAG_S2_SATELLITE_DELIVERY_SYSTEM 	= 0x79,
	eSIxDESCTAG_ENHANCED_AC3					= 0x7A,	/** < Enhanced - AC3(Dolby Plus) Descriptor */
	eSIxDESCTAG_DTS							= 0x7B,	/** < see DVB 300-468 annex G */
	eSIxDESCTAG_AAC							= 0x7C,	/** < see DVB 300-468 annex H */
	eSIxDESCTAG_XAIT_LOCATION					= 0x7D,
	eSIxDESCTAG_FTA_CONTENT_MANAGEMENT			= 0x7E,
	eSIxDESCTAG_EXTENSION						= 0x7F,

	eSIxDESCTAG_PRIVATE_START					= 0x80,  // Private Desctriptor Tag
	eSIxDESCTAG_MAX							= eSIxDESCTAG_PRIVATE_START,

	/** < DVB Extension Descriptor Tags */
	/** < 여기에는 ETS 300 468의 v1.10.1 (2009-07) 버전에서 추가된 extension descriptor의 TAG를 기술한다 */
	eSIxDESCTAG_EXT_IMG_ICON					= 0x00,
	eSIxDESCTAG_EXT_CPCM_DEL_SIGNALLING		= 0x01,
	eSIxDESCTAG_EXT_CP							= 0x02,
	eSIxDESCTAG_EXT_CP_IDENTIFIER				= 0x03,
	eSIxDESCTAG_EXT_T2_DELIVERY_SYSTEM		= 0x04,
	eSIxDESCTAG_EXT_SH_DELIVERY_SYSTEM		= 0x05,
	eSIxDESCTAG_EXT_SUPPLEMENTARY_AUDIO		= 0x06,
	eSIxDESCTAG_EXT_NETWORK_CHANGE_NOTIFY	= 0x07,
	eSIxDESCTAG_EXT_MESSAGE					= 0x08,
	eSIxDESCTAG_EXT_TARGET_REG				= 0x09,
	eSIxDESCTAG_EXT_TARGET_REG_NAME			= 0x0A,
	eSIxDESCTAG_EXT_SERVICE_RELOCATED 		= 0x0B,
	eSIxDESCTAG_EXT_XAIT_PID				= 0x0C,
	eSIxDESCTAG_EXT_C2_DELIVERY_SYSTEM		= 0x0D,
	eSIxDESCTAG_EXT_DTS_HD_AUDIO_STREAM		= 0x0E,
	eSIxDESCTAG_EXT_DTS_NEURAL 				= 0x0F,
	eSIxDESCTAG_EXT_VIDEO_DEPTH_RANGE		= 0x10,
	eSIxDESCTAG_EXT_T2MI					= 0x11,
	eSIxDESCTAG_EXT_Reserve					= 0x12, //reserve
	eSIxDESCTAG_EXT_URI_LINKAGE 			= 0x13,
	eSIxDESCTAG_EXT_BCI_ANCILLARY_DATA		= 0x14,

	/** < TS 102 006에 정의된 SSU UNT desctiptor tag */
	eSIxDESCTAG_UNT_START_RESERVED				= 0x00,		/** < reserved								*/
	eSIxDESCTAG_UNT_SCHEDULING					= 0x01,		/** < Scheduling Descriptor					*/
	eSIxDESCTAG_UNT_UPDATE						= 0x02,		/** < Update Descriptor						*/
	eSIxDESCTAG_UNT_SSU_LOCATION					= 0x03,		/** < SSU Location Descriptor				*/
 	eSIxDESCTAG_UNT_MESSAGE						= 0x04,		/** < Message Descriptor					*/
 	eSIxDESCTAG_UNT_SSU_EVENT_NAME				= 0x05,		/** < SSU Event Name Descriptor				*/
 	eSIxDESCTAG_UNT_TARGET_SMARTCARD				= 0x06,		/** < Target Smartcard Descriptor			*/
 	eSIxDESCTAG_UNT_TARGET_MAC_ADDRESS			= 0x07,		/** < MAC Address Descriptor				*/
 	eSIxDESCTAG_UNT_TARGET_SERIAL_NUMBER			= 0x08,		/** < Serial Number Descriptor				*/
 	eSIxDESCTAG_UNT_TARGET_IP_ADDRESS			= 0x09,		/** < IP Address Descriptor					*/
 	eSIxDESCTAG_UNT_TARGET_IPV6_ADDRESS			= 0x0A,		/** < IPv6 Address Descriptor				*/
 	eSIxDESCTAG_UNT_SSU_SUBGROUP_ASSOCIATION	= 0x0B,		/** < SSU Subgroup Association Descriptor	*/

	/** < AIT Descriptor Tags */
	eSIxDESCTAG_AIT_APPLICATION							= 0x00,		/** < 0x00 Application descriptor (AIT) */
	eSIxDESCTAG_AIT_APPLICATION_NAME					= 0x01,		/** < 0x01 Application name descriptor (AIT) */
	eSIxDESCTAG_AIT_TRANSPORT_PROTOCOL					= 0x02,		/** < 0x02 Tansport protocol descriptor (AIT) */
	eSIxDESCTAG_AIT_SIMPLE_APPLICATION_LOCALTION 		= 0x15,		/** < 0x15 Simple application location descriptor (AIT) */
	eSIxDESCTAG_AIT_APPLICATION_USAGE					= 0x16,		/** < 0x16 Application usage descriptor (AIT) */
	eSIxDESCTAG_AIT_SIMPLE_APPLICATION_BOUNDARY			= 0x17,		/** < 0x17 Simple application boundary descriptor (AIT) */

	/** < SES Descriptor Tags */
	eSIxDESCTAG_SES_DTCP								= 0x88,		/** < 0x88 DTCP (PMT)										*/
	eSIxDESCTAG_SES_SERVICE_LIST_NAME					= 0x88,		/** < 0x88 Service list name descriptor (SGT)				*/
	eSIxDESCTAG_SES_BOUQUET_LIST						= 0x93,		/** < 0x93 Bouquet list descriptor (SGT)					*/
	eSIxDESCTAG_SES_VIRTUAL_SERVICE_ID					= 0xD1,		/** < 0xd1 Virtual service id descriptor (SGT)				*/
	eSIxDESCTAG_SES_COPY_CONTROL						= 0xDE,		/** < 0xde DRM and Copy control (EIT, ECM)				*/

	/** < Ertelecom Descriptor Tags */
	eSIxDESCTAG_ERTELECOM_DTCP							= 0x88,		/** < 0x88 DTCP (PMT)										*/

	/** < Arib Descriptor Tags */
	eSIxDESCTAG_ARIB_BROADCASTER_ID					= 0x85,		/** < 0x85 Broadcaster ID (SIT)								*/
	eSIxDESCTAG_ARIB_DTCP								= 0x88,		/** < 0x88 DTCP (PMT)										*/
	eSIxDESCTAG_ARIB_HIERARCHICAL_TRANSMISSION			= 0xC0,		/** < 0xC0 Hierarchical transmission descriptor (PMT) 		*/
	eSIxDESCTAG_ARIB_DIGITAL_COPY_CONTROL				= 0xC1,		/** < 0xC1 Digital copy control descriptor (PMT)   			*/
	eSIxDESCTAG_ARIB_NETWORK_IDENTIFICATION			= 0xC2,		/** < 0xC2 Network Identification descriptor (SIT)			*/
	eSIxDESCTAG_ARIB_PARTIAL_TRANSPORT_STREAM_TIME		= 0xC3,		/** < 0xC3 Partial Transport Stream Time descriptor (SIT)	*/
	eSIxDESCTAG_ARIB_AUDIO_COMPONENT					= 0xC4,		/** < 0xC4 Audio component descriptor (EIT)          		*/
	eSIxDESCTAG_ARIB_HYPER_LINK						= 0xC5,		/** < 0xC5 Hyper link descriptor							*/
	eSIxDESCTAG_ARIB_TARGET_AREA						= 0xC6,		/** < 0xC6 Target Area descriptor							*/
	eSIxDESCTAG_ARIB_DATA_CONTENTS						= 0xC7,		/** < 0xC7 Data contents descriptor (EIT)            		*/
	eSIxDESCTAG_ARIB_VIDEO_DECODE_CONTROL				= 0xC8,		/** < 0xC8 Video decode control descriptor (PMT)    	 	*/
	eSIxDESCTAG_ARIB_DOWNLOAD_CONTENT					= 0xC9,		/** < 0xC9 Download content descriptor(Note1)   			*/
	eSIxDESCTAG_ARIB_CA_EMM_TS							= 0xCA,		/** < 0xCA CA_EMM_TS descriptor                				*/
	eSIxDESCTAG_ARIB_CA_CONTRACT_INFOMATION			= 0xCB,		/** < 0xCB CA contract information descriptor   			*/
	eSIxDESCTAG_ARIB_CA_SERVICE						= 0xCC,		/** < 0xCC CA service descriptor                			*/
	eSIxDESCTAG_ARIB_TS_INFORMATION					= 0xCD,		/** < 0xCD TS information descriptor (NIT)					*/
	eSIxDESCTAG_ARIB_EXTENDED_BROADCASTER				= 0xCE,		/** < 0xCE Extended broadcaster (BIT)						*/
	eSIxDESCTAG_ARIB_LOGO_TRANSMISSION					= 0xCF,		/** < 0xCF Logo transmission								*/
	eSIxDESCTAG_ARIB_CABLE_TS_DIVISION_SYSTEM			= 0xF9,		/** < 0xF9 Cable TS division system (NIT) 					*/
	eSIxDESCTAG_ARIB_TERRESTRIAL_DELIVERY_SYSTEM		= 0xFA,		/** < 0xFA Terrestrial delivery system (NIT) 				*/
	eSIxDESCTAG_ARIB_PARTIAL_RECEPTION					= 0xFB,		/** < 0xFB Partial reception (NIT) 							*/
	eSIxDESCTAG_ARIB_EMERGENCY_INFORMATION				= 0xFC,		/** < 0xFC Emergency information descriptor (PMT/NIT)  		*/
	eSIxDESCTAG_ARIB_DATA_COMPONENT					= 0xFD,		/** < 0xFD Data component descriptor (PMT)           		*/
	eSIxDESCTAG_ARIB_SYSTEM_MANAGEMENT					= 0xFE,		/** < 0xFE System management descriptor (PMT/NIT/TLV-NIT)   */
	eSIxDESCTAG_ARIB_SERIES							= 0xD5,		/** < 0xD5 Series descriptor (EIT)                  		*/
	eSIxDESCTAG_ARIB_EVENT_GROUP						= 0xD6,		/** < 0xD6 Event group descriptor               			*/
	eSIxDESCTAG_ARIB_SI_PARAMETER						= 0xD7,		/** < 0xD7 SI parameter descriptor (BIT)             		*/
	eSIxDESCTAG_ARIB_BROADCASTER_NAME					= 0xD8,		/** < 0xD8 Broadcaster name descriptor (BIT)         		*/
	eSIxDESCTAG_ARIB_COMPONENT_GROUP					= 0xD9,		/** < 0xD9 Component group descriptor (EIT)          		*/
	eSIxDESCTAG_ARIB_CONTENT_AVAILABILITY				= 0xDE,		/** < 0xDE Content availability descriptor (EIT)     		*/

	/** < FreeSat Descriptor Tags */
	eSIxDESCTAG_FSAT_TUNNELLED_DATA 					= 0xD0, 	/** < 0xD0 OK, PMT											*/
	eSIxDESCTAG_FSAT_ALT_TUNNELLED_DATA 				= 0xD1, 	/** < OK, PMT 												*/
	eSIxDESCTAG_FSAT_LINKAGE							= 0xD2, 	/** < OK, NIT, T_NIT		: 1st, Once						*/
	eSIxDESCTAG_FSAT_REGIONALISED_LOGICAL_CHANNEL	= 0xD3, 	/** < NK, T_BAT				: 2nd, Multi					*/
	eSIxDESCTAG_FSAT_REGION_NAME						= 0xD4, 	/** < OK, T_BAT 			: 1st, Multi					*/
	eSIxDESCTAG_FSAT_SERVICE_GROUP					= 0xD5, 	/** < NK, T_BAT				: 1st, Multi					*/
	eSIxDESCTAG_FSAT_INTERACTIVE_STORAGE				= 0xD6, 	/** < OK, T_BAT				: 1st, Multi,	Related MHEG 	*/
	eSIxDESCTAG_FSAT_INFO_LOCATION					= 0xD7, 	/** < OK, T_BAT				: 1st, Once, 필요없다고 여겨짐*/
	eSIxDESCTAG_FSAT_SERVICEGROUP_NAME				= 0xD8, 	/** < NK, T_BAT				: 1st, Multi 					*/
	eSIxDESCTAG_FSAT_SHORT_SERVICE_NAME 				= 0xD9, 	/** < OK, T_SDT				: Multi							*/
	eSIxDESCTAG_FSAT_GUIDANCE							= 0xDA, 	/** < OK, T_SDT, T_EIT		: Zero or Only Once per lang for zero type */
	eSIxDESCTAG_FSAT_INTERACTIVE_RESTRICTION			= 0xDB, 	/** < OK, T_BAT				: 2nd, Once, Related MHEG 		*/
	eSIxDESCTAG_FSAT_CONTENT_MANAGEMENT 				= 0xDC, 	/** < NK, T_SDT, T_BAT		: Zero or Once in All loop		*/
	eSIxDESCTAG_FSAT_ONLINE_AVAILABILITY 					= 0xDE, 	/** < OK, EIT 												*/
	eSIxDESCTAG_FSAT_PREFIX 								= 0xDF, 	/** < OK, NIT,BAT,EIT		: 1st, Multi					*/
	eSIxDESCTAG_FSAT_MEDIA_LOCATOR						= 0xE0, 	/** < OK, BAT,SDT,EIT		: 1st, Multi					*/
	eSIxDESCTAG_FSAT_SHORT_EVENT_PROMOTIONAL			= 0xE1,		/** < OK, EIT												*/

	/** < UK DTT Descriptor Tags */
	eSIxDESCTAG_UKDTT_METADATA_POINTER					= 0x25,		/** < OK, NIT,SDT			: For BRL or Enhanced-EPG Products */
	eSIxDESCTAG_UKDTT_LOGICAL_CHANNEL 					= 0x83, 	/** < 0x83 Logical channel descriptor (NIT) 					*/
	eSIxDESCTAG_UKDTT_PREFERRED_NAME_LIST 				= 0x84,
	eSIxDESCTAG_UKDTT_PREFERRED_NAME_IDENTIFIER			= 0x85,
	eSIxDESCTAG_UKDTT_SERVICE_ATTRIBUTE					= 0x86, 	/** < OK, NIT			*/
	eSIxDESCTAG_UKDTT_HD_SHORT_SERVICE_NAME	 		= 0x87, 	/** < OK, SDT			*/
	eSIxDESCTAG_UKDTT_HD_SIMULCAST_LOGICAL_CHANNEL	= 0x88, 	/** < OK, NIT			*/
	eSIxDESCTAG_UKDTT_GUIDANCE 							= 0x89, 	/** < OK, SDT,EIT			*/

	/** < ITA DTT Descriptor Tags */
	eSIxDESCTAG_ITADTT_LOGICAL_CHANNEL					= 0x83,		/** < OK, NIT			*/
	eSIxDESCTAG_ITADTT_PREFERRED_NAME_LIST				= 0x84,		/** < TODO			*/
	eSIxDESCTAG_ITADTT_EACEM_STREAM_IDENTIFIER			= 0x86,		/** < TODO			*/
	eSIxDESCTAG_ITADTT_PREFERRED_NAME_IDENTIFIER		= 0x85, 	/** < TODO			*/
	eSIxDESCTAG_ITADTT_HD_SIMULCAST_LOGICAL_CHANNEL		= 0x88, 	/** < OK, NIT			*/

	/** < Nordig Descriptor Tags */
	eSIxDESCTAG_NORDIG_LOGICAL_CHANNEL					= 0x83,		/** < 0x83 Logical channel descriptor (NIT, version 1) */
	eSIxDESCTAG_NORDIG_LOGICAL_CHANNEL_V2				= 0x87,		/** < 0x87 Logical channel descriptor (NIT, version 2) */
	eSIxDESCTAG_NORDIG_DTCP								= 0x88,		/** < 0x88 DTCP (PMT) */
	eSIxDESCTAG_NORDIG_CONTENT_PROTECTION				= 0xA0,		/** < 0xA0 Content protection descriptor (PMT) */

	/** < TDC Descriptor Tags */
	eSIxDESCTAG_TDC_CHANNEL								= 0x82,		/** < 0x82 Channel Descriptor (NIT) */

	/** < FreeTV Descriptor Tags (Australia operational practice OP-41) */

	/**
	  *		LCN Descriptor Tags
	  *
	  *		- FreeTV Descriptor Tags (Australia operational practice OP-41)
	  *		- ER Telecom Descriptor Tags (Russia Cable)
	**/
	eSIxDESCTAG_LOGICAL_CHANNEL 							= 0x83, 	/** < 0x83 Logical channel descriptor (NIT) */

	/** < ZIGGO CAB Descriptor Tags */
	eSIxDESCTAG_ZIGGO_EACEM_STREAM_IDENTIFIER 			= 0x86, 	/** < 0x86 Eacem stream identifier descriptor (NIT) */
	eSIxDESCTAG_ZIGGO_HD_SIMULCAST_LOGICAL_CHANNEL		= 0x88, 	/** < 0x88 HD SIMULCAST descriptor (NIT) */

	/** < MBC Descriptor Tags */
	eSIxDESCTAG_MBC_LOGICAL_CHANNEL						= 0x83,		/** < 0x83 Logical channel descriptor (BAT) */
	
	eSIxDESCTAG_ALL										= 0xFF
} SIxDescriptorTag_e;

/*******************************************************************/
/********************      Data Type            ********************/
/*******************************************************************/

typedef struct
{
	SIxTextEncoding_e			 eTextEnc;
	HCHAR						*pszArg;
} SIxDescriptorHeaderParam_t;

typedef struct
{
	SIxSpec_e		 			eSpec;
	SIxDescriptorTag_e			eTag;
	SIxDescriptorHeaderParam_t	stParam;
	HUINT32						ulLength;		/** < Length of Raw Data */
} SIxDescriptorHeader_t;

typedef struct
{
	SIxSpec_e			eSpec;
	SIxDescriptorTag_e	eTag;
	HUINT32				ulLength;		/** < Length of Raw Data */
	HUINT8			 	aucRaw[];		/** < [0] : Tag, [1] : Length */
} SIxDescriptorRawData_t;

typedef union
{
	SIxDescriptorTag_e	 	eSpec;
	SIxDescriptorHeader_t	stHead;
	SIxDescriptorRawData_t	stRaw;
} SIxDescriptor_t;

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

extern SIxError_e	SILIB_DESC_Init (void *(*_malloc)(HLONG), HINT32 (*_free)(void *));
extern SIxError_e	SILIB_DESC_New (const SIxDescriptorTag_e eTag, const SIxSpec_e eSpec, const SIxDescriptorHeaderParam_t *pstParam, const HUINT8 *pucRawData, SIxDescriptor_t **ppstDes);
extern SIxError_e	SILIB_DESC_Delete (SIxDescriptor_t *pstDes);
extern SIxError_e	SILIB_DESC_Clone (SIxDescriptor_t **ppTarget, const SIxDescriptor_t *pstSource);
extern SIxError_e	SILIB_DESC_Print (const SIxDescriptor_t *pstDes);
extern HCHAR*		SILIB_DESC_GetStrOfSIxSpec_e(SIxSpec_e eSpec);


#ifdef __cplusplus
}
#endif

#endif // __SILIB_DESC_H__



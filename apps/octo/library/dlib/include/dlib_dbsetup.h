/* $Header: $ */

#ifndef DLIB_DBSETUP_H__
#define DLIB_DBSETUP_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <hlib.h>

#define		DxDBSETUP_SYSTEM		"DxDBSETUP_SYSTEM"

#define		DxDBSETUP_FIELDNAME_LEN			63
#define		eDxSETUP_SUPPORTED_LANGUAGE		8

//	ENUM Value�� Integrity �κ� �߰� �Ǿ�� ��.
typedef	enum
{
	eDxSETUP_SYSTEM_XXXX			= 0x10000000,
		eDxSETUP_SYSTEM_OPERATOR,

		eDxSETUP_SYSTEM_LANGUAGE_XXXX			= 0x10000100,
		eDxSETUP_SYSTEM_MAIN_LANGUAGE,
		eDxSETUP_SYSTEM_AUDIO_LANGUAGE			= eDxSETUP_SYSTEM_MAIN_LANGUAGE + eDxSETUP_SUPPORTED_LANGUAGE,
		eDxSETUP_SYSTEM_SUBTITLE_LANGUAGE		= eDxSETUP_SYSTEM_AUDIO_LANGUAGE + eDxSETUP_SUPPORTED_LANGUAGE,
		eDxSETUP_SYSTEM_EVENT_LANGUAGE			= eDxSETUP_SYSTEM_SUBTITLE_LANGUAGE + eDxSETUP_SUPPORTED_LANGUAGE,
		//	add here for category LANGUAGE
		//	end

		eDxSETUP_SYSTEM_AUDIO_XXXX				= 0x10000200,
		eDxSETUP_SYSTEM_HARDOFHEARING,
		eDxSETUP_SYSTEM_AUDIODESCRIPTION,
		eDxSETUP_SYSTEM_MATURITYRATING,
		eDxSETUP_SYSTEM_AUDIO_DUMMY1,// ���ڸ�.
		eDxSETUP_SYSTEM_AUDIO_DUMMY2, // ���ڸ�.
		eDxSETUP_SYSTEM_STEREO_SELECT,
		eDxSETUP_SYSTEM_DUAL_MONO,
		eDxSETUP_SYSTEM_HDMI_AUDIO_FORMAT,
		eDxSETUP_SYSTEM_SPDIF_AUDIO_FORMAT,
		eDxSETUP_SYSTEM_LIPSYNC_DELAY,

		//	add here for category AUDIO
		//	end

		eDxSETUP_SYSTEM_PVR_XXXX				= 0x10000300,
		eDxSETUP_SYSTEM_SKIP_FORWARD_SEC,
		eDxSETUP_SYSTEM_INSTANCE_REPLAY_SEC,
		eDxSETUP_SYSTEM_COUNTRY_INDEX,
		eDxSETUP_SYSTEM_PADDING_START_TIME,
		eDxSETUP_SYSTEM_PADDING_END_TIME,
		eDxSETUP_SYSTEM_TSR_ON_OFF,
		eDxSETUP_SYSTEM_AUTO_DELETE,
		eDxSETUP_SYSTEM_2ND_INT_HDD_UUID,
		eDxSETUP_SYSTEM_DESCRAMBLEONOFF,
		//	add here for category PVR
		//	end

		eDxSETUP_SYSTEM_OPERATION_XXXX			= 0x10000400,
		eDxSETUP_SYSTEM_CHANNEL_SETUP_PIN,
		eDxSETUP_SYSTEM_GUIDANCE_POLICY,
		eDxSETUP_SYSTEM_SUBTITLE_SETTING,
		eDxSETUP_SYSTEM_HDMICEC_MODE,
		eDxSETUP_SYSTEM_GMT_OFFSET_MINUTES,
		eDxSETUP_SYSTEM_TIME_REGION_ID,
		eDxSETUP_SYSTEM_DUMMY1,		// ��� ����.
		eDxSETUP_SYSTEM_DAYLIGHT_SAVING,
		eDxSETUP_SYSTEM_UPDATE_TIME_CTRL,
		eDxSETUP_SYSTEM_UPDATE_TIME,
		eDxSETUP_SYSTEM_PPV_LIMIT_MONEY,
		eDxSETUP_SYSTEM_INSTALLATION_PIN_CODE,
		eDxSETUP_SYSTEM_DUMMY2, // ��� ����.
		eDxSETUP_SYSTEM_CAPTION_SETUP,
		eDxSETUP_SYSTEM_CAPTION_LANGUAGE,
		eDxSETUP_SYSTEM_VIDEO_BK_COLOR, // ��� ����.
		eDxSETUP_SYSTEM_AUTOMATIC_POWERDOWN,
		eDxSETUP_SYSTEM_PWR_SAVING_ON_STANDBY,		// pama �� �̵�. hapi_registry ���.
		eDxSETUP_SYSTEM_REGULAR_AUTO_UPDATE_STANDBY,
		eDxSETUP_SYSTEM_REGULAR_AUTO_UPDATE_OPERATE,
		eDxSETUP_SYSTEM_REGULAR_AUTO_UPDATE_SET,
		eDxSETUP_SYSTEM_REGULAR_AUTO_UPDATE_SUCCESS,
		eDxSETUP_SYSTEM_AUTO_SLEEP,
		eDxSETUP_SYSTEM_ADULT_CHANNEL,
		eDxSETUP_SYSTEM_GUIDANCE_CHANNEL_GENRE,
		eDxSETUP_SYSTEM_FAV_CHNUM_ORDER,
		eDxSETUP_SYSTEM_DYNAMIC_UPDATE_SETTING,
		eDxSETUP_SYSTEM_POSTCODE,
		eDxSETUP_SYSTEM_ADULTCHANNEL,

		// output video
		eDxSETUP_STSTEM_TVSCART_FORMAT,
		eDxSETUP_STSTEM_VCRSCART_FORMAT,
		eDxSETUP_STSTEM_SCART_PATH,
		eDxSETUP_STSTEM_TV_ASPECTRATIO,
		eDxSETUP_STSTEM_DFC_SELECT,
		eDxSETUP_STSTEM_RESOLUTION_SELECT,
		eDxSETUP_STSTEM_3D_SELECT,
		eDxSETUP_STSTEM_BACKGROUND_COLOR_R,
		eDxSETUP_STSTEM_BACKGROUND_COLOR_G,
		eDxSETUP_STSTEM_BACKGROUND_COLOR_B,

		eDxSETUP_SYSTEM_ANTENNA_POWER,
		eDxSETUP_SYSTEM_PRIORITY_HDMIOUTPUT,
		eDxSETUP_SYSTEM_HDMICEC_VOLUME_ENABLE,
		//	add here for category OPERATION
		//	end

		eDxSETUP_SYSTEM_SCAN_XXXX				= 0x10000500,
		eDxSETUP_SYSTEM_INVISIBLE_SEARCH,
		eDxSETUP_SYSTEM_ANTENNA_CONNECTION,
		eDxSETUP_SYSTEM_ANTSET_ANTENNATYPE,
		eDxSETUP_SYSTEM_ANTSET_SAT_TUNER01,
		eDxSETUP_SYSTEM_ANTSET_SAT_TUNER02,
		eDxSETUP_SYSTEM_ANTSET_SAT_TUNER03,
		eDxSETUP_SYSTEM_ANTSET_NUMOFSAT,
		eDxSETUP_SYSTEM_SRCH_TPSELECTION,
		eDxSETUP_SYSTEM_MOTORSET_LNBFREQ,
		eDxSETUP_SYSTEM_MOTORSET_LNBPOWER,
		eDxSETUP_SYSTEM_MOTORSET_MOTORTYPE,
		eDxSETUP_SYSTEM_POSITIONSET_SATTYPE,
		eDxSETUP_SYSTEM_POSITIONSET_MOTORDRIVE,
		eDxSETUP_SYSTEM_POSITIONSET_DRIVESIZE,
		eDxSETUP_SYSTEM_LIMITSET_MOTORLIMIT,
		eDxSETUP_SYSTEM_LIMITSET_LIMITPOS,
		eDxSETUP_SYSTEM_SAT_TUNER_SELECTION,
		eDxSETUP_SYSTEM_SAT_SRCH_ALL_ANT,
		eDxSETUP_SYSTEM_SAT_TRANSPONDER,
		eDxSETUP_SYSTEM_SAT_NETWORK_SRCH,
		eDxSETUP_SYSTEM_SAT_CAFREE_TYPE,
		eDxSETUP_SYSTEM_SAT_CHANNEL_TYPE,
		eDxSETUP_SYSTEM_SAT_SEARCH_MODE,
		eDxSETUP_SYSTEM_SAT_SRCH_AUTO_CH_NUMBERING,
		eDxSETUP_SYSTEM_TER_SRCH_CHANNEL,
		eDxSETUP_SYSTEM_TER_SRCH_FREQUENCY,
		eDxSETUP_SYSTEM_TER_SRCH_BANDWIDTH,
		eDxSETUP_SYSTEM_TER_SRCH_NETSEARCH,
		eDxSETUP_SYSTEM_TER_SRCH_NETWORKID,
		eDxSETUP_SYSTEM_TER_SRCH_POWER,
		eDxSETUP_SYSTEM_TER_SRCH_TRANSMISSION,
		eDxSETUP_SYSTEM_TER_SRCH_AUTO_CH_NUMBERING,
		eDxSETUP_SYSTEM_CAB_SRCH_TUNERID,
		eDxSETUP_SYSTEM_CAB_SRCH_NETSEARCH,
		eDxSETUP_SYSTEM_CAB_SRCH_CAFREE_TYPE,
		eDxSETUP_SYSTEM_CAB_SRCH_CHANNEL_TYPE,
		eDxSETUP_SYSTEM_CAB_SRCH_FREQUENCY,
		eDxSETUP_SYSTEM_CAB_SRCH_SYMBOL_RATE,
		eDxSETUP_SYSTEM_CAB_SRCH_MODULATION,
		eDxSETUP_SYSTEM_CAB_SRCH_NETWORK_ID,
		eDxSETUP_SYSTEM_CAB_LINEAR_SRCH_STARTFREQUENCY,
		eDxSETUP_SYSTEM_CAB_LINEAR_SRCH_STOPFREQUENCY,
		eDxSETUP_SYSTEM_CAB_LINEAR_SRCH_STEP,
		eDxSETUP_SYSTEM_CAB_LINEAR_SRCH_MODULATION,
		eDxSETUP_SYSTEM_CAB_LINEAR_SRCH_SYMBOL_RATE,
		eDxSETUP_SYSTEM_CAB_SRCH_NETWORK_SRCH_MODE,
		eDxSETUP_SYSTEM_SRCH_NETWORK,
		eDxSETUP_SYSTEM_MAN_TP_FREQUENCY,
		eDxSETUP_SYSTEM_MAN_TP_SYMBOLRATE,
		eDxSETUP_SYSTEM_MAN_TP_POLARIZATION,
		eDxSETUP_SYSTEM_MAN_TP_FEC,
		eDxSETUP_SYSTEM_MAN_TP_TRANSSPEC,
		eDxSETUP_SYSTEM_MAN_TP_MODULATION,
		eDxSETUP_SYSTEM_MAN_TP_PILOT,
		eDxSETUP_SYSTEM_MOTOR_USER_LONGITUDE,
		eDxSETUP_SYSTEM_CABLE_SEARCH_MODE,
		eDxSETUP_SYSTEM_SEARCH_ACN,
		//	add here for category SCAN
		//	end

		eDxSETUP_SYSTEM_NETWORK_XXXX			= 0x10002000,
		eDxSETUP_SYSTEM_NETWORK_MAIN_DEV_INDEX,
		eDxSETUP_SYSTEM_NETWORK_MAIN_DEV_ON,
		eDxSETUP_SYSTEM_NETWORK_SUB_DEV_INDEX,
		eDxSETUP_SYSTEM_NETWORK_SUB_DEV_ON,
		eDxSETUP_SYSTEM_ETHERNET_CONF_1ST_ON,
		eDxSETUP_SYSTEM_ETHERNET_CONF_1ST,
		eDxSETUP_SYSTEM_ETHERNET_CONF_1ST_AUTO_DNS,
		eDxSETUP_SYSTEM_ETHERNET_CONF_1ST_IP,
		eDxSETUP_SYSTEM_ETHERNET_CONF_1ST_NETMASK,
		eDxSETUP_SYSTEM_ETHERNET_CONF_1ST_GATEWAY,
		eDxSETUP_SYSTEM_ETHERNET_CONF_1ST_DNS,
		eDxSETUP_SYSTEM_ETHERNET_CONF_1ST_SECONDARY_DNS,
		eDxSETUP_SYSTEM_ETHERNET_CONF_1ST_IPV6,
		eDxSETUP_SYSTEM_ETHERNET_CONF_1ST_IPV6_AUTO_DNS,
		eDxSETUP_SYSTEM_ETHERNET_CONF_1ST_IPV6_IP,
		eDxSETUP_SYSTEM_ETHERNET_CONF_1ST_IPV6_PREFIX,
		eDxSETUP_SYSTEM_ETHERNET_CONF_1ST_IPV6_GATEWAY,
		eDxSETUP_SYSTEM_ETHERNET_CONF_1ST_IPV6_DNS,
		eDxSETUP_SYSTEM_ETHERNET_CONF_1ST_IPV6_SECONDARY_DNS,
		eDxSETUP_SYSTEM_ETHERNET_CONF_1ST_IPV6_TUNNEL_START_IP,
		eDxSETUP_SYSTEM_ETHERNET_CONF_1ST_IPV6_TUNNEL_END_IP,
		eDxSETUP_SYSTEM_ETHERNET_CONF_2ND_ON,
		eDxSETUP_SYSTEM_ETHERNET_CONF_2ND,
		eDxSETUP_SYSTEM_ETHERNET_CONF_2ND_AUTO_DNS,
		eDxSETUP_SYSTEM_ETHERNET_CONF_2ND_IP,
		eDxSETUP_SYSTEM_ETHERNET_CONF_2ND_NETMASK,
		eDxSETUP_SYSTEM_ETHERNET_CONF_2ND_GATEWAY,
		eDxSETUP_SYSTEM_ETHERNET_CONF_2ND_DNS,
		eDxSETUP_SYSTEM_ETHERNET_CONF_2ND_SECONDARY_DNS,
		eDxSETUP_SYSTEM_ETHERNET_CONF_2ND_IPV6,
		eDxSETUP_SYSTEM_ETHERNET_CONF_2ND_IPV6_AUTO_DNS,
		eDxSETUP_SYSTEM_ETHERNET_CONF_2ND_IPV6_IP,
		eDxSETUP_SYSTEM_ETHERNET_CONF_2ND_IPV6_PREFIX,
		eDxSETUP_SYSTEM_ETHERNET_CONF_2ND_IPV6_GATEWAY,
		eDxSETUP_SYSTEM_ETHERNET_CONF_2ND_IPV6_DNS,
		eDxSETUP_SYSTEM_ETHERNET_CONF_2ND_IPV6_SECONDARY_DNS,
		eDxSETUP_SYSTEM_ETHERNET_CONF_2ND_IPV6_TUNNEL_START_IP,
		eDxSETUP_SYSTEM_ETHERNET_CONF_2ND_IPV6_TUNNEL_END_IP,
		eDxSETUP_SYSTEM_ETHERNET_CONF_3TH_ON,
		eDxSETUP_SYSTEM_ETHERNET_CONF_3TH,
		eDxSETUP_SYSTEM_ETHERNET_CONF_3TH_AUTO_DNS,
		eDxSETUP_SYSTEM_ETHERNET_CONF_3TH_IP,
		eDxSETUP_SYSTEM_ETHERNET_CONF_3TH_NETMASK,
		eDxSETUP_SYSTEM_ETHERNET_CONF_3TH_GATEWAY,
		eDxSETUP_SYSTEM_ETHERNET_CONF_3TH_DNS,
		eDxSETUP_SYSTEM_ETHERNET_CONF_3TH_SECONDARY_DNS,
		eDxSETUP_SYSTEM_ETHERNET_CONF_3TH_IPV6,
		eDxSETUP_SYSTEM_ETHERNET_CONF_3TH_IPV6_AUTO_DNS,
		eDxSETUP_SYSTEM_ETHERNET_CONF_3TH_IPV6_IP,
		eDxSETUP_SYSTEM_ETHERNET_CONF_3TH_IPV6_PREFIX,
		eDxSETUP_SYSTEM_ETHERNET_CONF_3TH_IPV6_GATEWAY,
		eDxSETUP_SYSTEM_ETHERNET_CONF_3TH_IPV6_DNS,
		eDxSETUP_SYSTEM_ETHERNET_CONF_3TH_IPV6_SECONDARY_DNS,
		eDxSETUP_SYSTEM_ETHERNET_CONF_3TH_IPV6_TUNNEL_START_IP,
		eDxSETUP_SYSTEM_ETHERNET_CONF_3TH_IPV6_TUNNEL_END_IP,
		eDxSETUP_SYSTEM_ETHERNET_CONF_4TH_ON,
		eDxSETUP_SYSTEM_ETHERNET_CONF_4TH,
		eDxSETUP_SYSTEM_ETHERNET_CONF_4TH_AUTO_DNS,
		eDxSETUP_SYSTEM_ETHERNET_CONF_4TH_IP,
		eDxSETUP_SYSTEM_ETHERNET_CONF_4TH_NETMASK,
		eDxSETUP_SYSTEM_ETHERNET_CONF_4TH_GATEWAY,
		eDxSETUP_SYSTEM_ETHERNET_CONF_4TH_DNS,
		eDxSETUP_SYSTEM_ETHERNET_CONF_4TH_SECONDARY_DNS,
		eDxSETUP_SYSTEM_ETHERNET_CONF_4TH_IPV6,
		eDxSETUP_SYSTEM_ETHERNET_CONF_4TH_IPV6_AUTO_DNS,
		eDxSETUP_SYSTEM_ETHERNET_CONF_4TH_IPV6_IP,
		eDxSETUP_SYSTEM_ETHERNET_CONF_4TH_IPV6_PREFIX,
		eDxSETUP_SYSTEM_ETHERNET_CONF_4TH_IPV6_GATEWAY,
		eDxSETUP_SYSTEM_ETHERNET_CONF_4TH_IPV6_DNS,
		eDxSETUP_SYSTEM_ETHERNET_CONF_4TH_IPV6_SECONDARY_DNS,
		eDxSETUP_SYSTEM_ETHERNET_CONF_4TH_IPV6_TUNNEL_START_IP,
		eDxSETUP_SYSTEM_ETHERNET_CONF_4TH_IPV6_TUNNEL_END_IP,
		eDxSETUP_SYSTEM_ETHERNET_CONF_5TH_ON,
		eDxSETUP_SYSTEM_ETHERNET_CONF_5TH,
		eDxSETUP_SYSTEM_ETHERNET_CONF_5TH_AUTO_DNS,
		eDxSETUP_SYSTEM_ETHERNET_CONF_5TH_IP,
		eDxSETUP_SYSTEM_ETHERNET_CONF_5TH_NETMASK,
		eDxSETUP_SYSTEM_ETHERNET_CONF_5TH_GATEWAY,
		eDxSETUP_SYSTEM_ETHERNET_CONF_5TH_DNS,
		eDxSETUP_SYSTEM_ETHERNET_CONF_5TH_SECONDARY_DNS,
		eDxSETUP_SYSTEM_ETHERNET_CONF_5TH_IPV6,
		eDxSETUP_SYSTEM_ETHERNET_CONF_5TH_IPV6_AUTO_DNS,
		eDxSETUP_SYSTEM_ETHERNET_CONF_5TH_IPV6_IP,
		eDxSETUP_SYSTEM_ETHERNET_CONF_5TH_IPV6_PREFIX,
		eDxSETUP_SYSTEM_ETHERNET_CONF_5TH_IPV6_GATEWAY,
		eDxSETUP_SYSTEM_ETHERNET_CONF_5TH_IPV6_DNS,
		eDxSETUP_SYSTEM_ETHERNET_CONF_5TH_IPV6_SECONDARY_DNS,
		eDxSETUP_SYSTEM_ETHERNET_CONF_5TH_IPV6_TUNNEL_START_IP,
		eDxSETUP_SYSTEM_ETHERNET_CONF_5TH_IPV6_TUNNEL_END_IP,
		eDxSETUP_SYSTEM_ETHERNET_CONF_6TH_ON,
		eDxSETUP_SYSTEM_ETHERNET_CONF_6TH,
		eDxSETUP_SYSTEM_ETHERNET_CONF_6TH_AUTO_DNS,
		eDxSETUP_SYSTEM_ETHERNET_CONF_6TH_IP,
		eDxSETUP_SYSTEM_ETHERNET_CONF_6TH_NETMASK,
		eDxSETUP_SYSTEM_ETHERNET_CONF_6TH_GATEWAY,
		eDxSETUP_SYSTEM_ETHERNET_CONF_6TH_DNS,
		eDxSETUP_SYSTEM_ETHERNET_CONF_6TH_SECONDARY_DNS,
		eDxSETUP_SYSTEM_ETHERNET_CONF_6TH_IPV6,
		eDxSETUP_SYSTEM_ETHERNET_CONF_6TH_IPV6_AUTO_DNS,
		eDxSETUP_SYSTEM_ETHERNET_CONF_6TH_IPV6_IP,
		eDxSETUP_SYSTEM_ETHERNET_CONF_6TH_IPV6_PREFIX,
		eDxSETUP_SYSTEM_ETHERNET_CONF_6TH_IPV6_GATEWAY,
		eDxSETUP_SYSTEM_ETHERNET_CONF_6TH_IPV6_DNS,
		eDxSETUP_SYSTEM_ETHERNET_CONF_6TH_IPV6_SECONDARY_DNS,
		eDxSETUP_SYSTEM_ETHERNET_CONF_6TH_IPV6_TUNNEL_START_IP,
		eDxSETUP_SYSTEM_ETHERNET_CONF_6TH_IPV6_TUNNEL_END_IP,
		eDxSETUP_SYSTEM_WLAN_IS_CONNECTED,
		eDxSETUP_SYSTEM_WLAN_IS_WPS,
		eDxSETUP_SYSTEM_WLAN_CONNECTED_AP,
		eDxSETUP_SYSTEM_WLAN_KEY,
		eDxSETUP_SYSTEM_WLAN_SECURITY,
		eDxSETUP_SYSTEM_WLAN_AUTHEN,
		eDxSETUP_SYSTEM_WLAN_ENCRYPT,
		eDxSETUP_SYSTEM_FTP_ON,
		eDxSETUP_SYSTEM_DMS_ON,

		eDxSETUP_SYSTEM_DHCPSERVER_ON,
		eDxSETUP_SYSTEM_DHCPSERVER_DEV,
		eDxSETUP_SYSTEM_MAC_ADRESS,
		eDxSETUP_SYSTEM_DIGITAL_CONTENT_PROTECTION,
		eDxSETUP_SYSTEM_FTP_SERVER_LAUNCH,
		eDxSETUP_SYSTEM_WLAN_FREQUENCY,
		eDxSETUP_SYSTEM_WLAN_CHANNEL,
		eDxSETUP_SYSTEM_NETWORK_DUMMY01,					// eDxSETUP_SYSTEM_BT_ENABLE,					// ��� ����.

		eDxSETUP_SYSTEM_DHCPSERVER_LEASE_TIME,
		eDxSETUP_SYSTEM_DHCPSERVER_START_IP,
		eDxSETUP_SYSTEM_DHCPSERVER_END_IP,
		eDxSETUP_SYSTEM_DHCPSERVER_ROUTER,
		eDxSETUP_SYSTEM_DHCPSERVER_NETMASK,
		eDxSETUP_SYSTEM_DHCPSERVER_1ST_DNS,
		eDxSETUP_SYSTEM_DHCPSERVER_2ND_DNS,
		eDxSETUP_SYSTEM_DHCPSERVER_3TH_DNS,

		eDxSETUP_SYSTEM_ETHERNET_CONF_1ST_IP_VERSION,
		eDxSETUP_SYSTEM_ETHERNET_CONF_2ND_IP_VERSION,
		eDxSETUP_SYSTEM_ETHERNET_CONF_3TH_IP_VERSION,
		eDxSETUP_SYSTEM_ETHERNET_CONF_4TH_IP_VERSION,
		eDxSETUP_SYSTEM_ETHERNET_CONF_5TH_IP_VERSION,
		eDxSETUP_SYSTEM_ETHERNET_CONF_6TH_IP_VERSION,
		//	add here for category NETWORK
		//	end

		eDxSETUP_SYSTEM_FUNCTION_XXXX				= 0x20000000,
		eDxSETUP_SYSTEM_TVTV_ENABLE,
		eDxSETUP_SYSTEM_TVTV_DOWNTYPE,
		eDxSETUP_SYSTEM_FUNCTION_DUMMY01,			// eDxSETUP_SYSTEM_TVTV_DOWNTIME,				// ��� ����.
		eDxSETUP_SYSTEM_REGISTRATION_CODE,
		eDxSETUP_SYSTEM_TRIPLEID_UPLOAD,
		eDxSETUP_SYSTEM_FUNCTION_DUMMY02,			// ��� ����.
		eDxSETUP_SYSTEM_FUNCTION_DUMMY03,			// ��� ����.
		eDxSETUP_SYSTEM_FUNCTION_DUMMY04,			// ��� ����.
		eDxSETUP_SYSTEM_FUNCTION_DUMMY05,			// ��� ����.
		eDxSETUP_SYSTEM_IR_NATIONAL_CODE,
		eDxSETUP_SYSTEM_IR_SOL_SETTING,
		eDxSETUP_SYSTEM_IR_HOMING_CHANNEL,
		eDxSETUP_SYSTEM_IR_VIRTUAL_SECURECW_SETTING,
		eDxSETUP_SYSTEM_DISPLAY_CRID_SETTING,


		//	add here for category FUNCTION
		//	end

		eDxSETUP_SYSTEM_JP_XXXX						= 0x20010000,
		eDxSETUP_SYSTEM_JP_DT_SRCH_MODE,
		eDxSETUP_SYSTEM_JP_DT_RESCAN_FLAG,
		eDxSETUP_SYSTEM_JP_DT_SRCH_START_FREQ,
		eDxSETUP_SYSTEM_JP_DT_SRCH_END_FREQ,
		eDxSETUP_SYSTEM_JP_DT_SRCH_CH_NUM,
		eDxSETUP_SYSTEM_JP_SELECTED_AUDIO_ES_INDEX,
		eDxSETUP_SYSTEM_JP_SELECTED_DUAL_MONO_INDEX,
		eDxSETUP_SYSTEM_JP_POSTCODE,
		eDxSETUP_SYSTEM_JP_NAVI_URL,
		eDxSETUP_SYSTEM_JP_INTERACTVURL,
		eDxSETUP_SYSTEM_JP_VODHOMEURL,
		eDxSETUP_SYSTEM_JP_ACSURL,
		eDxSETUP_SYSTEM_JP_LOCALAREA,
		eDxSETUP_SYSTEM_JP_GLOBALAREACODE,
		eDxSETUP_SYSTEM_JP_CAPTION_ONOFF,
		eDxSETUP_SYSTEM_JP_CAPTION_LANGUAGE,
		eDxSETUP_SYSTEM_JP_SUPER_ONOFF,
		eDxSETUP_SYSTEM_JP_SUPER_LANGUAGE,
		eDxSETUP_SYSTEM_JP_SETUP_LAUNCH_MODE,
		eDxSETUP_SYSTEM_JP_DISPLAY_MODE_3D,
		eDxSETUP_SYSTEM_JP_LIMITPERPPV,
		eDxSETUP_SYSTEM_JP_LIMITPERPPM,
		eDxSETUP_SYSTEM_JP_PINUNLOCK_EXPIRATION,
		eDxSETUP_SYSTEM_JP_PERMISSION_LEVEL,
		eDxSETUP_SYSTEM_JP_THREED_MODE,
		eDxSETUP_SYSTEM_JP_AUTO_SWDOWNLOAD,
		eDxSETUP_SYSTEM_JP_CHANNEL_MASK,
		eDxSETUP_SYSTEM_JP_SEAMLESS,
		eDxSETUP_SYSTEM_JP_AUTOPPVPURCHASE,
		eDxSETUP_SYSTEM_JP_BMLINFODISP_ENABLE,
		eDxSETUP_SYSTEM_JP_FRONTDISP_ENABLE,
		eDxSETUP_SYSTEM_JP_DMSDEVICEHANDLE,
		eDxSETUP_SYSTEM_JP_DMSDESTINATIONID,
		eDxSETUP_SYSTEM_JP_CHANNELMASK_PATTERN,
		eDxSETUP_SYSTEM_JP_DEFAULTFACTORYFORDAP,
		eDxSETUP_SYSTEM_JP_DEFAULTDISCFORDAP,

		eDxSETUP_SYSTEM_JP_LOGCAPTURE_WRITE_MODE,
		eDxSETUP_SYSTEM_JP_STANDBY_MODE,
		eDxSETUP_SYSTEM_JP_INTERNETBROWSERURL,
		//	add here for category JAPAN
		//	end

		eDxSETUP_SYSTEM_SES_XXXX				= 0x20020000,
		eDxSETUP_SYSTEM_SES_SVCLISTID,
		eDxSETUP_SYSTEM_SES_SVCLISTVER,
		eDxSETUP_SYSTEM_SES_SVCLISTNAME,
		eDxSETUP_SYSTEM_SES_TVNUM,
		eDxSETUP_SYSTEM_SES_RADIONUM,
		eDxSETUP_SYSTEM_SES_LASTCHECKEDTIME,
		eDxSETUP_SYSTEM_SES_LASTUPDATEDTIME,
		eDxSETUP_SYSTEM_SES_REPLACESVC_NUM,
		eDxSETUP_SYSTEM_SES_REPLACESVC_ITEMS,
		eDxSETUP_SYSTEM_SES_NEWSVC_NUM,
		eDxSETUP_SYSTEM_SES_NEWSVC_ITEMS,
		eDxSETUP_SYSTEM_SES_SCINSERTED,
		eDxSETUP_SYSTEM_SES_REPLACELATER_NUM,
		eDxSETUP_SYSTEM_SES_REPLACELATER_ITEMS,
		eDxSETUP_SYSTEM_SES_COLDBOOT_RESULT,
		eDxSETUP_SYSTEM_SES_SATREC_DETECTINFO,
		eDxSETUP_SYSTEM_SES_SATREC_FBDVERSION,
		eDxSETUP_SYSTEM_SES_SATREC_BDVERSION,
		eDxSETUP_SYSTEM_SES_OLDPREPAIDRIGHT,
		eDxSETUP_SYSTEM_SES_EM36_OPEN,
		eDxSETUP_SYSTEM_SES_SC_OLD_SN,
		//	add here for category SYSTEM_SES
		//	end

		eDxSETUP_SYSTEM_OP_002					= 0x20030000,
		//	add here for category SYSTEM_OP_002
		//	end

		eDxSETUP_SYSTEM_OP_003				= 0x20040000,
		//	add here for category SYSTEM_OP_003
		//	end

		eDxSETUP_SYSTEM_OP_004					= 0x20050000,
		//	add here for category SYSTEM_OP_004
		//	end

		eDxSETUP_SYSTEM_FREESAT_XXXX				= 0x20060000,
		eDxSETUP_SYSTEM_FREESAT_POSTCODE,
		eDxSETUP_SYSTEM_FREESAT_ADULTCHANNELS,
		//	add here for category SYSTEM_FREESAT
		//	end

		eDxSETUP_SYSTEM_IPEPG_XXXX					= 0x20070000,
		eDxSETUP_SYSTEM_IPEPG_UPDATETIME,			// ��� ����.
		eDxSETUP_SYSTEM_IPEPG_SERVICEUPDATED,		// ��� ����.
		//	add here for category IPEPG
		//	end

	eDxSETUP_SECURE_XXXX			= 0x30000000,
		eDxSETUP_SECURE_SYSTEM_PINCODE,
		//	add here for category SECURE
		//	end

	eDxSETUP_USER_XXXXXXXX			= 0x40000000,
		eDxSETUP_USER_MY_LONGITUDE,
		eDxSETUP_USER_MY_LATITUDE,
		eDxSETUP_USER_CUR_SVC,
		eDxSETUP_USER_CHLIST_GROUP,					// ��� ����.
		eDxSETUP_USER_CHLIST_ALPHABETGROUP,
		eDxSETUP_USER_ZAPPING_ALPHABETGROUP,
		eDxSETUP_USER_PINCODE,
		eDxSETUP_USER_POWEROFF_TYPE_REBOOT,
		eDxSETUP_USER_LAST_STANDBY,
		eDxSETUP_USER_LAST_VOLUME,
		eDxSETUP_USER_CURSUB_GRP_IDX,
		eDxSETUP_USER_LAST_SUB_GRP_IDX,
		eDxSETUP_USER_CUR_EPG_SHOW_SUB_GRP_IDX,
		eDxSETUP_USER_CUR_EPG_SHOW_SUB_GRP_CHANGE,
		eDxSETUP_USER_CUR_GRP_TV_RAIDO_KIND,
		eDxSETUP_USER_LAST_CATEGORY_TV_SUB_GRP_IDX,
		eDxSETUP_USER_LAST_CATEGORY_RADIO_SUB_GRP_IDX,
		eDxSETUP_USER_LAST_BOUQUET_TV_SUB_GRP_IDX,
		eDxSETUP_USER_LAST_BOUQUET_RADIO_SUB_GRP_IDX,
		eDxSETUP_USER_LAST_FAVOURITE_TV_SUB_GRP_IDX,
		eDxSETUP_USER_LAST_FAVOURITE_RADIO_SUB_GRP_IDX,
		eDxSETUP_USER_REGION_CODE,
		eDxSETUP_USER_BOUQUET_ID,
		eDxSETUP_USER_HOME_TP_INFO_1,
		eDxSETUP_USER_HOME_TP_INFO_2,
		eDxSETUP_USER_HOME_TP_INFO_3,
		eDxSETUP_USER_BAT_INFO,
		eDxSETUP_USER_FSAT_SI_MODE,
		eDxSETUP_USER_LAST_TV_SVC,
		eDxSETUP_USER_LAST_RADIO_SVC,
		eDxSETUP_USER_ASO_NOTIFY_FLAG,
		eDxSETUP_USER_ASO_NETWORK_CHANGE_INFO,
		eDxSETUP_USER_ASO_ASO_NETWORK_MSG_INFO,
		eDxSETUP_USER_ASO_LAST_SCAN_TIME,
		eDxSETUP_USER_REGION_NAMEINFO_CNT,
		eDxSETUP_USER_REGION_NAMEINFO_PTR,
		eDxSETUP_USER_LAST_MIC_VOLUME,
		eDxSETUP_USER_LAST_DECODER_VOLUME,
		eDxSETUP_USER_LIVE_STREAM_PRIORITY,
		eDxSETUP_USER_BAT_UPGRADE_ONOFF,
		eDxSETUP_USER_NIT_LCN_UPGRADE_ONOFF,
		//	add here for category USER_XXX
		//	end

		eDxSETUP_USER_UPC_EXPIRED_COUNT	= 0x40010000,
		eDxSETUP_USER_UPC_LAST_OTA_VER,
		eDxSETUP_USER_UPC_UPDATE_STATUS,
		eDxSETUP_USER_UPC_7DAY_TIMEDATA,
		//	add here for category USER_UPC
		//	end

		eDxSETUP_USER_OTA_DUMMY01	= 0x40020000,
		eDxSETUP_USER_OTA_DUMMY02,
		eDxSETUP_USER_OTA_DUMMY03,
		eDxSETUP_USER_OTA_DUMMY04,
		eDxSETUP_USER_OTA_DETECT_INFO,
		eDxSETUP_USER_OTA_FLAG,
		eDxSETUP_USER_OTA_USERCANCELLED_VER,
		eDxSETUP_USER_OTA_USERCANCELLED_TYPE,
		eDxSETUP_USER_OTA_TSINFO,
		eDxSETUP_USER_OTA_OTAINFO,
		eDxSETUP_USER_OTA_DOWNLOADEDINFO,
		eDxSETUP_USER_OTA_EMERGENCYINFO,
		eDxSETUP_USER_OTA_SCHEDULEINFO,
		eDxSETUP_USER_OTA_RETRYCOUNT,
		eDxSETUP_USER_OTA_RETRYVER,
		eDxSETUP_USER_OTA_RETRYTYPE,
		eDxSETUP_USER_OTA_WAKEUP_FLAG,
		eDxSETUP_USER_ANTENNA_SORT_METHOD,
		eDxSETUP_USER_PVR_DEFAULT_HDD_UUID,
		eDxSETUP_USER_HOME_CHANNEL,
		//	add here for category USER_OTA
		//	end

		eDxSETUP_USER_SI_DUMMY000	= 0x40030000,
		eDxSETUP_USER_SI_DUMMY001,
		eDxSETUP_USER_SI_DUMMY002,
		eDxSETUP_USER_SI_DUMMY003,
		eDxSETUP_USER_SI_DUMMY004,
		eDxSETUP_USER_SI_DUMMY005,
		eDxSETUP_USER_SI_DUMMY006,
		eDxSETUP_USER_SI_DUMMY007,
		eDxSETUP_USER_NIT_TS_LOOP_CRC32,			// Obama Internal Use
		eDxSETUP_USER_NIT_TS_LOOP_CRC32_BS,			// Obama Internal Use
		eDxSETUP_USER_NIT_TS_LOOP_CRC32_CATV,		// Obama Internal Use
		//	add here for category USER_SI
		//	end

		/* ���� !!!!!!
		 * eDxSETUP_USER_XXX_DUMMY01 ~ eDxSETUP_USER_XXX_DUMMY17 ����
		 * ����� �� �ִ� ������.
		 * �̸� �ٲپ ����� ��..
		 * ���� �߰��� �����ϰų� �����ϸ� ū�ϳ��ϴ�.!!!!!
		 */
		eDxSETUP_USER_XXX_DUMMY01	= 0x40040000,
		eDxSETUP_USER_XXX_DUMMY02,
		eDxSETUP_USER_XXX_DUMMY03,
		eDxSETUP_USER_XXX_DUMMY04,
		eDxSETUP_USER_XXX_DUMMY05,
		eDxSETUP_USER_XXX_DUMMY06,
		eDxSETUP_USER_XXX_DUMMY07,
		eDxSETUP_USER_XXX_DUMMY08,
		eDxSETUP_USER_XXX_DUMMY09,
		eDxSETUP_USER_XXX_DUMMY10,
		eDxSETUP_USER_XXX_DUMMY11,
		eDxSETUP_USER_XXX_DUMMY12,
		eDxSETUP_USER_XXX_DUMMY13,
		eDxSETUP_USER_XXX_DUMMY14,
		eDxSETUP_USER_XXX_DUMMY15,
		eDxSETUP_USER_XXX_DUMMY16,
		eDxSETUP_USER_XXX_DUMMY17,
		eDxSETUP_USER_MEDIA_PLAYER_DMP_LAST_CONTENTS,
		eDxSETUP_USER_MEDIA_PLAYER_DMP_LAST_STARTTIME,
		//	add here for category USER_MEDIA
		//	end

		/*	���ο� ī�װ����� �Ʒ��� �߰��ϼ���...
		 */
		eDxSETUP_USER_XXX	= 0x40050000,
		eDxSETUP_SYSTEM_TER_REGION_CNT,
		eDxSETUP_SYSTEM_TER_REGION_PTR,


	eDxSETUP_HIDDEN_XXXXXXXX			= 0x50000000,
		eDxSETUP_HIDDEN_UPDATE_TIME_ON_ALL_TP,
		//	add here for category HIDDEN
		//	end

		//	add here for category XXXX
		//	end
} DxDBSETUP_Item_e;

typedef	struct
{
	HUINT32		uid;
	DxType_e	eItemType;
	HINT32		nValue;
	HINT32		nSize;
	HUINT8		pucData[];
} DxDBSetupItem_t;

#ifdef __cplusplus
}
#endif

#endif // DLIB_SERVICE_H__


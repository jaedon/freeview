
#ifndef ________MXLIB_CONFIG_H__
#define ________MXLIB_CONFIG_H__

// 모든 모듈은 config.h 에서 이 header 를 참조하자

// configuraton 이 하나 이상의 모듈을 참조하거나 sample 이 참조해야되는 feature 를 넣자.
// project 별로 정의할 수 있게 하자.

///////////////////////////////////////////////////////////////////////////////
// 모든 feature 설명
///////////////////////////////////////////////////////////////////////////////

#undef _MEMORY_TEST	 // 메모리 테스트를 위해 사용한다.
#undef _MX_MALLOC_TEST // MX_MALLOC 을 테스트할때 사용한다.

#define IFACE_NAME "eth0"

///////////////////////////////////////////////////////////////////////////////
// Default
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// all platform list
///////////////////////////////////////////////////////////////////////////////
// 이것은 make file 에서 정의하자

// #define WIN32
// #define _ANDROID
// #define IPHONE_OS
// #define _POSIX

///////////////////////////////////////////////////////////////////////////////
// all project list
///////////////////////////////////////////////////////////////////////////////

// #define HUMAX_RECENT
// #define DLNA_CTT_TEST
// #define HUMAX_LEGACY
// #define HUMAX_LEGACY_DTCP_CUSTOM
// #define HUMAX_JAPAN_2012
#define HUMAX_HMS_2012
// #define HUMAX_HMS_NGS
// #define HUMAX_JC_NGS_2012
// #define LG_DMS
// #define _KT_ANDROID
// #define KT_VERSION_SUPPORT
// #define _WOON_VERSION_SUPPORT
// #define _OTT_N_MA_VERSION_SUPPORT
// #define _LG_S_DONGLE_VERSION_SUPPORT
// #define _PULSUS_VERSION_SUPPORT
// #define ZODIAC_DMS_2015

/* 2013/4/3 mhkang: iOS용 mxlib에서 define되어 있었음. 일단, 주석처리하여 merge함.
	makefile 등에 빌드옵션으로 define해야할 것 같음.
#define HUMAX_HMS_DMC_IPHONE
*/


// 필요할때 프로젝트 추가하고 테스트하시고 마지막 release 할때는 HUMAX_RECENT 살리기

///////////////////////////////////////////////////////////////////////////////
// 프로젝트별 세부 세팅
///////////////////////////////////////////////////////////////////////////////

//#########################################
#ifdef DLNA_CTT_TEST
//#########################################
	#undef DTCP_MOVE_SUPPORT
	#define DLNAQOS
	#define SFM_FOLDER_ONLY // 폴더 구조 간단히 하는거.. .테스트 시간을 줄이기 위해
#endif /* DLNA_CTT_TEST */

#define SFM_FOLDER_ONLY



//#########################################
#ifdef HUMAX_LEGACY
//#########################################
	#define HUMAX_COMMON
#endif /* HUMAX_LEGACY */





//#########################################
#ifdef HUMAX_LEGACY_DTCP_CUSTOM
//#########################################
	#define HUMAX_COMMON
#endif /* HUMAX_LEGACY_DTCP_CUSTOM */





//#########################################
#ifdef HUMAX_JAPAN_2012
//#########################################
	#define HUMAX_COMMON

	#ifdef _POSIX
//		#define _MEMORY_TEST
//		#define MXDTCP_MALLOC_TEST
//		#define MXFILE_MALLOC_TEST
//		#define AES_KEY_PRE_ALLOC
//		#define HMX_MEM_TEST
//		#define MXFILE_WEB_DEBUG
	#else
		#undef _MEMORY_TEST
		#undef MXDTCP_MALLOC_TEST
		#undef MXFILE_MALLOC_TEST
//		#undef AES_KEY_PRE_ALLOC
	#endif

	// #define DTCP_DEBUG
	#define VIDEO_SINGLE_CONNECTION_TEST_20120227	// 하나의 컨넥션만 유지하게 하는 옵션
	#define WEBSERVER_ACCESS_MGR_20120228
#endif /* HUMAX_JAPAN_2012 */





//#########################################
#ifdef HUMAX_HMS_2012
//#########################################
#define HUMAX_COMMON

	#define MXCORE_DISABLE_IP_SUPPORT

	#ifdef _POSIX
		#define INCLUDE_VERSION_INFO
		#define MXWEBSERVER_SESSION_TIMEOUT_20120523		// for HUMAX HMS by jonghyun. should ctt test.
	#else
		#undef _MEMORY_TEST
		#undef MXDTCP_MALLOC_TEST
		#undef MXFILE_MALLOC_TEST
	#endif

	#define DMS_SERVER_FUNC_CONFIG
	#ifdef DMS_SERVER_FUNC_CONFIG
		#define DLNA_USER_DEVICE_CAPABILITY
		#define DMS_USER_VIRTUAL_MULTIPLERESOURCE
		#define DMS_USER_MEDIATRANSPORT_CONFIG
	#endif

	//#define DMS_DB_QUICK_CHECK

	   //HUMAX_JP_PROPERTIES_20120212
		//SUPPORT_PROP_RATING
		//DATE_SUPPORT_20120222

		//WEBSERVER_ACCESS_MGR_20120228
		//VIDEO_SINGLE_CONNECTION_TEST_20120227


	// #define DTCP_DEBUG
	//#define VIDEO_SINGLE_CONNECTION_TEST_20120227	// 하나의 컨넥션만 유지하게 하는 옵션
	//#define WEBSERVER_ACCESS_MGR_20120228

	// 20120330
	#define HUMAX_EPG_TUNER_FEATURE_20120328
	#ifdef HUMAX_EPG_TUNER_FEATURE_20120328
	//#define EPGITEM_CHILD
	#define HUMAX_EPG_CALLBACK_FEATURE_20121206
			#ifdef HUMAX_EPG_CALLBACK_FEATURE_20121206
				#define HUMAX_X_CHANNEL_ADDITIONAL_INFO
			//	#define HUMAX_EPG_CALLBACK_BROWSER_PROGLAM
			#endif
			#ifdef DMS_SERVER_FUNC_CONFIG
				#define DMS_USER_RESOURCE_MODIFY_M3U8_POSITION
			#endif
	#endif

	#define DMS_USE_SPECIAL_TUNER_OBJECT_ID // channel container의 objectid와 상관없이 "tuner" 이름으로 invoke browsing할 때 필요함.

	/* 2015/9/1 yjlim:
	  . when the PVR DB managed logically in 3rd-party m/w, we can get PVR list using below feature
	   --> from now on HUMAX also use this feature for PVR */

	#define THIRDPARTY_PVR_FEATURE_20150509

	#define DMR_X_GET_FLING_INFO_SUPPORT

	// 20121112 HLS
	#define SUPPORT_HLS
	#ifdef SUPPORT_HLS
		#define MAKE_M3U8_VIRTURALRESOURCE
		#define SUPPORT_NTLS

		#ifdef WIN32
			#define TRANSCODER_FFMPEG
		#else
			#define HUMAX_HW_TRANSCODER
		#endif
	#endif

	#if defined(DMS_SERVER_FUNC_CONFIG) && defined(SUPPORT_HLS)
			#if defined(HUMAX_EPG_CALLBACK_FEATURE_20121206)
				#define DMS_USER_RESOURCE_MODIFY_M3U8_POSITION_EPG
			#endif
		#define DMS_USER_RESOURCE_MODIFY_M3U8_POSITION_MEDIA
	#endif

	#ifndef _WOON_VERSION_SUPPORT
	#define _WOON_VERSION_SUPPORT
	#endif

	#ifndef HUMAX_SATIP
	#define HUMAX_SATIP
	#endif

#endif /* HUMAX_HMS_2012 */





//#########################################
#ifdef HUMAX_JC_NGS_2012
	#define HUMAX_COMMON
	#define XSRS_CP_SUPPORT
	#define HDRL_CP_SUPPORT
	#define DTCP_MOVE_SUPPORT


//		#define _MEMORY_TEST
//		#define MXDTCP_MALLOC_TEST
//		#define _MEMORY_TEST_PRINT

	#define	TIMESEEK_SUPPORT // For DMP

	#ifdef _POSIX
		#define INCLUDE_VERSION_INFO
	#endif

	#define HUMAX_JP_PROPERTIES_20120212
	#define JLAB_PARENTALRATING_SUPPORT
	#define LIVE_STREAMING
	// RatingContent Support

	// <20120426>
	#define DMS_SERVER_FUNC_CONFIG
	#ifdef DMS_SERVER_FUNC_CONFIG
			  #define DLNA_USER_DEVICE_CAPABILITY
			  #undef DMS_USER_VIRTUAL_MULTIPLERESOURCE
			  #define DMS_USER_VIRTUAL_MULTIPLEITEM
			  #define DMS_USER_MEDIATRANSPORT_CONFIG
	#endif
	#define HUMAX_EPG_TUNER_FEATURE_20120328
	// </20120426>

	// TEST NEW Function
	#define SFM_VIRTUALNAME_USE_AT_REALFOLDER_20120402
	#define WEBSERVER_ACCESS_MANAGER
	#define	VIDEO_SINGLE_CONNECTION_TEST_20120227
	#define DMS_TIMESEEK_SUPPORT_20120702
	#define MEDIATRANSFER_BUFFERING_20130214

#endif /* HUMAX_JC_NGS_2012 */




//#########################################
#ifdef ZODIAC_DMS_2015
//#########################################
#define HUMAX_COMMON

//	#define SUPPORT_NTLS_USING_UDS		// 2015/5/8 mhkang: NTLS시 UDS socket을 사용하여 TS를 전달하는 방식, fp_Streaming_TS_Get 콜백함수가 호출되지 않고 socket으로 전달된다.

	#define MXCORE_DISABLE_IP_SUPPORT

	#ifdef _POSIX
		#define INCLUDE_VERSION_INFO
		#define MXWEBSERVER_SESSION_TIMEOUT_20120523		// for HUMAX HMS by jonghyun. should ctt test.
	#else
		#undef _MEMORY_TEST
		#undef MXDTCP_MALLOC_TEST
		#undef MXFILE_MALLOC_TEST
	#endif

	#define DMS_SERVER_FUNC_CONFIG
	#ifdef DMS_SERVER_FUNC_CONFIG
		#define DLNA_USER_DEVICE_CAPABILITY
		#define DMS_USER_VIRTUAL_MULTIPLERESOURCE
		#define DMS_USER_MEDIATRANSPORT_CONFIG
	#endif

	//#define DMS_DB_QUICK_CHECK

	   //HUMAX_JP_PROPERTIES_20120212
		//SUPPORT_PROP_RATING
		//DATE_SUPPORT_20120222

		//WEBSERVER_ACCESS_MGR_20120228
		//VIDEO_SINGLE_CONNECTION_TEST_20120227


	// #define DTCP_DEBUG
	//#define VIDEO_SINGLE_CONNECTION_TEST_20120227	// 하나의 컨넥션만 유지하게 하는 옵션
	//#define WEBSERVER_ACCESS_MGR_20120228

	// 20120330
	#define HUMAX_EPG_TUNER_FEATURE_20120328
	#ifdef HUMAX_EPG_TUNER_FEATURE_20120328
	//#define EPGITEM_CHILD
	#define HUMAX_EPG_CALLBACK_FEATURE_20121206
			#ifdef HUMAX_EPG_CALLBACK_FEATURE_20121206
				#define HUMAX_X_CHANNEL_ADDITIONAL_INFO
			//	#define HUMAX_EPG_CALLBACK_BROWSER_PROGLAM
			#endif
			#ifdef DMS_SERVER_FUNC_CONFIG
				#define DMS_USER_RESOURCE_MODIFY_M3U8_POSITION
			#endif
	#endif

	#define DMR_X_GET_FLING_INFO_SUPPORT

	// 20121112 HLS
	#define SUPPORT_HLS
	#ifdef SUPPORT_HLS
		#define MAKE_M3U8_VIRTURALRESOURCE
		#define SUPPORT_NTLS

		#ifdef WIN32
			#define TRANSCODER_FFMPEG
		#else
			#define HUMAX_HW_TRANSCODER
		#endif
	#endif

	#if defined(DMS_SERVER_FUNC_CONFIG) && defined(SUPPORT_HLS)
			#if defined(HUMAX_EPG_CALLBACK_FEATURE_20121206)
				#define DMS_USER_RESOURCE_MODIFY_M3U8_POSITION_EPG
			#endif
		#define DMS_USER_RESOURCE_MODIFY_M3U8_POSITION_MEDIA
	#endif

	/* 2015/5/9 yjlim: (ZODIAC)
	  when the PVR DB managed logically in 3rd-party m/w, we can get PVR list using below feature */
	#define THIRDPARTY_PVR_FEATURE_20150509

	#define _MXDBC_DB_ONLY_ONE
	#define DMS_CONTENT_AGGREGATION_INTERFACE_SUPPORT

	/* 2015/10/12 yjlim: (ZODIAC)
	  To support DTCP/IP Streaming on PS3, enable below feature */
	#define DMS_TIMESEEK_SUPPORT_20120702

	/* 2015/11/04 ciyoon: (ZODIAC)
	  To fix DTCP/IP Streaming issue on PS3, enable below feature
	  which supports Enhanced Subfunction(RESPONSE2 ...), RTT, and MOVE */
	#define DTCP_MOVE_SUPPORT

//	#ifndef _WOON_VERSION_SUPPORT
//	#define _WOON_VERSION_SUPPORT
//	#endif

//	#ifndef HUMAX_SATIP
//	#define HUMAX_SATIP
//	#endif
	#define DMS_DLNA_FUNC_SWITCH
	#define DMS_ONLY_ANSWER_FROM_SAME_IP_DMP

	/* 2016/04/01 ciyoon: (ZODIAC)
	  To protect the device DTCP private key, ECDSA with private operation is done at DI or OEM layer. */
	#define DTCP_HARDENING_PRVKEY_SECUREDATA

#endif /* ZODIAC_DMS_2015 */



//#########################################
#ifdef HUMAX_COMMON
//#########################################

#ifdef DTCP_SUPPORT
	#define _HUMAX_SECURE_FILE


#ifndef WIN32
	#define DTCP_KEY_CUSTOM_API
// 20120518 blocked for General Test by jonghyun
	#define DTCP_HARDWARE_ENGINE

/* 2015/3/20 yjlim : (FVP4000T)
  - DTCP_MOVE_SUPPORT config 제거함 (일본향을 제외하고는 원래 없었던 config로 보임)
   : DTCP_MOVE_SUPPORT config가 켜져있을 경우 외부 DMP와의 AKE 과정 중에서 RTT 단계를 거친다.
   : calypso에서 사용하는 DTCP 소스에서는 MOVE_SUPPORT를 사용하지 않게 되있어서 DMS 단에서도 해당 기능을 disable 해야 한다.
   : 해당 config를 끄지 않으면 RTT 단계를 거치면서 DMP에서 RTT가 not implement 되어있다고 나오면서 AKE 실패한다.

	#define DTCP_MOVE_SUPPORT
*/

	// 20120628
	#define DTCP_USE_ECC_LIB
	#define DTCP_USE_ECC_OPENSSL
//	// Move to common.mk
#else
//	#define DTCP_HARDWARE_ENGINE
#endif

	#define DTCP_KEY_HUMAX
	#define DTCP_HW_KEY

#elif DTCP17_SUPPORT
 	#define _HUMAX_SECURE_FILE


#ifndef WIN32
	#define DTCP_KEY_CUSTOM_API
// 20120518 blocked for General Test by jonghyun
	#define DTCP_HARDWARE_ENGINE

//	// Move to common.mk
#else
//	#define DTCP_HARDWARE_ENGINE
#endif

	#define DTCP_KEY_HUMAX
	#define DTCP_HW_KEY

#endif
#endif /* HUMAX_COMMON */





//#########################################
#ifdef LG_DMS
//#########################################
	#define DTCP_SUPPORT
#endif /* LG_DMS */





//#########################################
#ifdef HUMAX_RECENT
//#########################################
	#ifndef _ANDROID
		#define DTCP_SUPPORT
		#define DTCP_MOVE_SUPPORT
	#endif

	#define DLNAQOS
	//#define _MEMORY_TEST
	//#define _MX_MALLOC_TEST

#endif /* HUMAX_RECENT */


/* 2013/4/3 mhkang: iOS용 mxlib에서 define되어 있었음. 일단, 주석처리하여 meger함
#ifdef HUMAX_HMS_DMC_IPHONE

	#define DMC_X_GET_FLING_INFO_SUPPORT
	#define DMC_PAIRING_DEVICE_SUPPORT


#endif // HUMAX_HMS_DMC_IPHONE
*/

//#########################################
#ifdef _KT_ANDROID
//#########################################
	#define DTCP_SUPPORT
	#define _DEVICE_DESCRIOTION_SUPPORT
#endif /* _KT_ANDROID */





//#########################################
#ifdef _WOON_VERSION_SUPPORT
//#########################################
//#define MXLOG_FILE_LOG_SUPPORT

#define _MXDBC_DB_ONLY_ONE

#define MSCP_TITLE_UTF8_STRING
#define MSCP_FRIENDLY_NAME_UTF8_STRING

#define DMS_PAIRING_DEVICE_SUPPORT
#define DMC_PAIRING_DEVICE_SUPPORT
#define DMS_CONTENT_AGGREGATION_INTERFACE_SUPPORT
#define DMS_REALFOLDER_DO_NOT_SEARCH
//#define DMS_DUMMY_THUMBNAIL_RESOURCE
//#define DMS_DISKSCAN_WORKING_QUEUE

//#define RAS_UI_WEB_SERVER_SUPPORT
//#define RAS_UI_WEB_SERVER_TEST				// localhost가 아닌 경우에도 접근 가능한 서버를 생성한다.

#define RAS_ACCESS_EXPIRED
#define RAS_SSL_SUPPORT
#define RAS_OAUTH_SUPPORT
#define RAS_TRANSCODER_THUMBNAIL_SUPPORT

#define RAS_SEND_OK_THREAD_POOL_SUPPORT		// send시 thread pool로 전달할 수 있도록 함.
#define _HUMAX_CONNECTION_SERVER_SUPPORT
//#define _OAUTH_80_PORT_DISABLE			// 생략하면 humax connection 서버에서 정상동작 하지 않음.

#ifdef _POSIX
#define DMS_DO_NOT_ANSWER_FROM_SAME_IP_DMP
#define DMC_DO_NOT_ANSWER_FROM_SAME_IP_DEVIE
#endif

#define RAS_METADATA_READ_ONLY

#ifdef _WIN32
#define _WIN_WCHAR_SUPPORT
#endif
#endif	/* _WOON_VERSION_SUPPORT */





//#########################################
#ifdef _PULSUS_VERSION_SUPPORT
//#########################################
	#define DMC_X_DO_COMMAND_SUPPORT
	#define DMC_X_EVENT_SUPPORT
	#define DMR_X_DO_COMMAND_SUPPORT
	#define DMR_X_EVENT_SUPPORT
	#define DMR_SEEK_CHECK_SUPPORT
#endif // _PULSUS_VERSION_SUPPORT


//#########################################
#ifdef _HALO_CJ_VERSION_SUPPORT
//#########################################
	#define DMC_X_DO_COMMAND_SUPPORT
	#define DMC_X_EVENT_SUPPORT
	#define DMR_X_DO_COMMAND_SUPPORT
	#define DMR_X_EVENT_SUPPORT

	/* 2013/2/25 mhkang: SEEK하기전에 지원되는 SEEK type인지 체크하기 위한 옵션
	 * 다양한 미디어에 대한 seek를 하기위헤서는 아래 DMR_SEEK_CHECK_SUPPORT을 comment out(박도유전임 설명임)
	 */
	//#define DMR_SEEK_CHECK_SUPPORT
#endif




//#########################################
#ifdef KT_VERSION_SUPPORT
//#########################################
	#undef _DEVICE_DESCRIOTION_SUPPORT
	#define _DEVICE_DESCRIOTION_SUPPORT
	#undef _KT_VERSION_SUPPORT
	#define _KT_VERSION_SUPPORT

	#ifndef _ANDROID
		#undef POWERMANAGER
		#define POWERMANAGER
		//#define SMI_ENCODING yhkang : 2012.02.deprecated
		#undef SUPPORT_TRANSCODER
		#define SUPPORT_TRANSCODER
		#undef DTCP_SUPPORT

		// 20110210
		#undef TRANSCODING_INFO_MANAGE
		#define TRANSCODING_INFO_MANAGE

		// 20110902
		#undef MAKE_M3U8_VIRTURALRESOURCE
		#define MAKE_M3U8_VIRTURALRESOURCE
	#endif //_ANDROID
#endif // KT_VERSION_SUPPORT





//#########################################
#ifdef _OTT_N_MA_VERSION_SUPPORT
//#########################################
	#define DMC_DEVICE_TINY_INFO_EX_SUPPORT
	#define DMC_X_GET_FLING_INFO_SUPPORT
	#define DMC_PAIRING_DEVICE_SUPPORT

	#define HUMAX_X_CHANNEL_ADDITIONAL_INFO
	#define HUMAX_EPG_TUNER_FEATURE_20120328

//    #define DTCP_SUPPORT
//    #define DTCP_KEY_CUSTOM_API
//    #define DTCP_MOVE_SUPPORT
//    #define DTCP_KEY_HUMAX
//    #define DTCP_HW_KEY
//    #define DTCP_USE_ECC_LIB
//    #define DTCP_USE_ECC_OPENSSL
	#define DLNA_USER_DEVICE_CAPABILITY
	#define DMS_SERVER_FUNC_CONFIG

	//#define _OAUTH_80_PORT_DISABLE

#endif /* _OTT_N_MA_VERSION_SUPPORT */





//#########################################
#ifdef _LG_S_DONGLE_VERSION_SUPPORT
//#########################################
#define DMR_X_INIT_WFD_SUPPORT
#endif	/* _LG_S_DONGLE_VERSION_SUPPORT */

//#########################################
#ifdef HUMAX_SATIP
//#########################################
#ifdef WIN32
#define _WIN_WCHAR_SUPPORT
#define SATIP_FILE_TEST
//#define SATIP_FILE_TEST_FILTER
#endif

//#define SATIP_RTSP_MULTICAST_SUPPORT

#define SATIP_DEVICE_SUPPORT
#define SATIP_CP_SUPPORT
#define MSCP_FRIENDLY_NAME_UTF8_STRING

#define SATIP_TEARDOWN_STREAMID_ERROR_SUPPORT

#ifdef _POSIX
#define DMC_DO_NOT_ANSWER_FROM_SAME_IP_DEVIE
#endif

#define MXCORE_DISABLE_IP_SUPPORT
#endif	/* HUMAX_SATIP */

extern const char *MXLIB_NIC_CB_SUPPORT;

/****************************************************************/
// Low power mode (DLNA CVP2)
/****************************************************************/
// #define DMS_ENABLE_CVP2_LOWPOWER_SERVICE
// #define MSCP_ENABLE_CVP2_LOWPOWER_CP

/****************************************************************/
// View Primary Output Port
/****************************************************************/
// #define DMS_ENABLE_OCAP_VPOP

#endif // ________MXLIB_CONFIG_H__

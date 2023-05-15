/* $Header: $ */

#ifndef __IPEPG_CONFIG_HMS1000S_H__
#define __IPEPG_CONFIG_HMS1000S_H__

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_____INSIDE_IPEPG_CONFIG_H_____)
	#error ("!!! _ipepg_config_hms1000s.h shall be included only in the ipepg_config.h !!!\n");
#endif

#if defined(CONFIG_SUPPORT_NATIVE_APP)
#undef IPEPG_PART_OF_TIME
#undef IPEPG_TOTAL_TIME
#undef EPG_TRIPLEID_FILE
#undef HUMAX_UPLOADFILE_PATH
#undef RP_UPLOAD_FILE

#define IPEPG_PART_OF_TIME						(24*14)					// 시간으로 나누어 받는다. 	24시간.
#define IPEPG_TOTAL_TIME						(24*14)				// 1주치를 받는다. 			24시간 * 7 시간
#define EPG_TRIPLEID_FILE 						CONFIG_PRODUCT_USERDATA_DIR"/ipepg/tripleid.xml"
#define HUMAX_UPLOADFILE_PATH					CONFIG_PRODUCT_USERDATA_DIR"/ipepg/stb_status.xml"
#define	RP_UPLOAD_FILE							CONFIG_PRODUCT_USERDATA_DIR"/ipepg/tmp_status.xml"

#endif


#define IPEPG_VERSION_CHECK
#if defined (IPEPG_VERSION_CHECK)
	#undef IPEPG_VERSION_CHECK_NO_VERSIONFILE
	#undef IPEPG_VERSION_MINOR
	#undef IPEPG_VERSION_NUM

	#define IPEPG_VERSION_CHECK_NO_VERSIONFILE	// @@@ 기존에 version 관리를 하지 않은 경우, 무조건 삭제 하기 위한 정의 @@@
	#define IPEPG_VERSION_MINOR 	"1"			// 파생 파일에서는 MAJOR의 값은 수정하지 않는다.
	#define IPEPG_VERSION_NUM		IPEPG_VERSION_MAJOR"."IPEPG_VERSION_MINOR
#endif

#if defined(CONFIG_DEBUG)
	#if defined(IPEPG_TEST_DIFFERENT_TIME)				// live stream 이 아닌 경우 epg 시간과 live stream 시간을 변경하여 화면에 보이도록 한다.
		#undef CUR_YEAR
		#undef CUR_MONTH
		#undef CUR_DAY

		#define CUR_YEAR								(2014)
		#define CUR_MONTH								(12)
		#define CUR_DAY									(10)
	#endif

#endif

#define IPEPG_PRIORITY_DOWN
#if defined (IPEPG_PRIORITY_DOWN)
	#if defined(CONFIG_SUPPORT_NATIVE_APP)
		#define IPEPG_PRIORITY_XML_PATH		CONFIG_PRODUCT_USERDATA_DIR"/ipepg/"   //xml의 파일 경로
		#define IPEPG_PRIORITY_INTERVAL		5        //2주치 받는 채널의 간격, ex) 50으로 설정된 경우 우선순위가 높은 채널 50개 부터 2주치를 받는다.
		#define IPEPG_PRIORITY_LIMIT_CNT	100000		// 250개 서비스만 받는다.
	#else
		#define IPEPG_PRIORITY_XML_PATH		"/etc/"   //xml의 파일 경로
		#define IPEPG_PRIORITY_INTERVAL		5        //2주치 받는 채널의 간격, ex) 50으로 설정된 경우 우선순위가 높은 채널 50개 부터 2주치를 받는다.
		#define IPEPG_PRIORITY_LIMIT_CNT	250		// 250개 서비스만 받는다.
	#endif
#endif

#define IPEPG_THUMBNAIL_NON_DOWN

#ifdef __cplusplus
}
#endif


#endif // __IPEPG_CONFIG_HMS1000T_H__



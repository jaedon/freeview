/* $Header: $ */

#ifndef __IPEPG_CONFIG_ICORDPRO_H__
#define __IPEPG_CONFIG_ICORDPRO_H__

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_____INSIDE_IPEPG_CONFIG_H_____)
	#error ("!!! _ipepg_config_iordpro.h shall be included only in the ipepg_config.h !!!\n");
#endif


// 메모리 부족으로 8일치 수신 (Series Recording을 위해서 7일 + 1일)
#undef IPEPG_TOTAL_TIME
#define IPEPG_TOTAL_TIME			(24*8)			// 8일치를 받는다. 			24시간 * 8일

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
		#define CUR_MONTH								(03)
		#define CUR_DAY									(26)
	#endif

#endif

#if defined(CONFIG_DEBUG)

	//#define IPEPG_DEBUG_DOWNLOAD_XML									// XML을 저장하여 확인해보기 위한 용도
	//#define IPEPG_DEBUG_DOWNLOAD_XML_PATH				"/tmp/ipepg_xml_data"

#endif


#define IPEPG_PRIORITY_DOWN
#if defined (IPEPG_PRIORITY_DOWN)
	#define IPEPG_PRIORITY_XML_PATH		"/etc/"   //xml의 파일 경로
	#define IPEPG_PRIORITY_INTERVAL		5        //2주치 받는 채널의 간격, ex) 50으로 설정된 경우 우선순위가 높은 채널 50개 부터 2주치를 받는다.
	#define IPEPG_MAX_EVENTCOUNT		130000    //total event 갯수
#endif

#define IPEPG_THUMBNAIL_NON_DOWN

#ifdef __cplusplus
}
#endif


#endif // __IPEPG_CONFIG_ICORDPRO_H__



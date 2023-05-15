/* $Header: $ */

#ifndef __IPEPG_CONFIG_HMS1000T_H__
#define __IPEPG_CONFIG_HMS1000T_H__

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_____INSIDE_IPEPG_CONFIG_H_____)
	#error ("!!! _ipepg_config_hms1000t.h shall be included only in the ipepg_config.h !!!\n");
#endif


// 호주향 IPEPG의 경우, iceTV에서 forwardEPG를 1주일 치만 전달한다.
#define IPEPG_TOTAL_TIME			(24*7)			// 1주치를 받는다. 			24시간 * 7일

#define IPEPG_BACKWARD_SUPPORT
#define IPEPG_BACKWARD_TOTAL_TIME  	(24*14)			// 2주치를 받는다. 			24시간 * 14일
#define IPEPG_PARSER_RATING_DO_NOT_MODIFY				// Rating 정보를 파싱할때, 별도의 수정을 하지 않고 그대로 저장한다. 


#if defined(CONFIG_DEBUG)
	//#define IPEPG_FUNCTION_COLOR_TRACE									// 함수의 시작 및 끝에 사용하는 로그에, 색을 사용한다
	//#define	IPEPG_TEST_DIFFERENT_TIME									// 현재시간 변경 test 진행.		
	//#define IPEPG_TEST_DOWNLOAD_REPEAT_TIME				(60*5)			// 120 초 마다 다시 받도록 test 진행. 
	
	#define IPEPG_DEBUG_DOWNLOAD_XML									// XML을 저장하여 확인해보기 위한 용도
	#define IPEPG_DEBUG_DOWNLOAD_XML_PATH				"/tmp/ipepg_xml_data"
	#if defined(IPEPG_TEST_DIFFERENT_TIME)				// live stream 이 아닌 경우 epg 시간과 live stream 시간을 변경하여 화면에 보이도록 한다.		
		#undef CUR_YEAR
		#undef CUR_MONTH
		#undef CUR_DAY
		
		#define CUR_YEAR								(2014)				
		#define CUR_MONTH								(01)
		#define CUR_DAY									(06)
	#endif

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

#ifdef __cplusplus
}
#endif


#endif // __IPEPG_CONFIG_HMS1000T_H__



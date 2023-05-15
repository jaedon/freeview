/* $Header: $ */

#ifndef __IPEPG_CONFIG_BASE_H__
#define __IPEPG_CONFIG_BASE_H__

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_____INSIDE_IPEPG_CONFIG_H_____)
	#error ("!!! _ipepg_config_base.h shall be included only in the ipepg_config.h !!!\n");
#endif


#if defined(CONFIG_DEBUG)
//#define	IPEPG_TEST_DIFFERENT_TIME											// 현재시간 변경 test 진행.
//#define IPEPG_TEST_DOWNLOAD_REPEAT_TIME			(3600)						// 60 초 마다 다시 받도록 test 진행.
//#define IPEPG_DEBUG_DOWNLOAD_XML					// XML을 저장하여 확인해보기 위한 용도 // size 가 커지면 momory가 충분하지 않아 죽을수 있음.
//#define IPEPG_DEBUG_DOWNLOAD_XML_PATH				"/tmp/ipepg_xml_data"

#endif

#if !defined(CONFIG_DEBUG) && defined(CONFIG_DEVICE_DEBUG)
//#define CFG_RELEASE_DEBUG
//#define CFG_OPEN_DEBUG
#endif
#define IPEPG_MAX_IMAGE_SIZE					(300)
#define IPEPG_MAX_DOWNLOAD_RETRY_COUNT			(3)					// Download 실패시, 재시도할 최대 횟수


#define IPEPG_PART_OF_TIME						(24)					// 시간으로 나누어 받는다. 	24시간.
#define IPEPG_TOTAL_TIME						(24*14)				// 2주치를 받는다. 			24시간 * 14 시간
#define IPEPG_DELAYED_TIME						(1000)
#define IPEPG_OFFSET_TIME						(-2*60*60)			// current time기준으로 2시간 전부터 2주치를 받는다.

//#define IPEPG_BACKWARD_SUPPORT
#define IPEPG_BACKWARD_TOTAL_TIME  				(24*14)			// 2주치를 받는다. 			24시간 * 14 시간


#if defined(IPEPG_TEST_DIFFERENT_TIME)										// live stream 이 아닌 경우 epg 시간과 live stream 시간을 변경하여 화면에 보이도록 한다.
#define CUR_YEAR								(2014)
#define CUR_MONTH								(3)
#define CUR_DAY									(1)
#endif


#define	IPEPG_SERVER_URL						CONFIG_IPEPG_SERVER_URL
#define IPEPG_URL_FILE							"/url/url_epg.txt"
#define HUMAX_GETRPLIST_URL						"api/GetRemoteProgramList?deviceID="
#define HUMAX_SETRPLIST_URL						"api/UploadStatus?deviceID="

#define	IPEPG_SERVICELIST_URL					"api/GetIPEPGChannels?deviceID="
#define IPEPG_EPGMEDIALIST_URL					"api/GetIPEPGProgramMedias?deviceid="
#ifdef CONFIG_FUNC_EPG_USE_SVCUID
#define	IPEPG_EPGLIST_URL						"api/GetIPEPGPrograms?deviceid="
#else
#define	IPEPG_EPGLIST_URL						"api/GetIPEPGProgramsFromTripleID?deviceid="
#endif

#define	IPEPG_STARTTIME_URL						"fromtime"
#define IPEPG_ENDTIME_URL						"totime"
#define IPEPG_TIMESTAMP_URL						"timestamp"
#define	IPEPG_HSVC_URL							"hsvc"
#define	IPEPG_TSID_URL							"tsid"
#define	IPEPG_ONID_URL							"onid"
#define	IPEPG_SVCID_URL							"svcid"

#define EPG_SET_TRIPLEID_URL 					"api/UploadChannel?deviceID="
#define IPEPG_GETLOGIN_URL						"api/Login?deviceID="

#if defined(CONFIG_IPEPG_REPOSITORY_HDD)
#define EPG_TRIPLEID_FILE 						"/mnt/hd1/tripleid.xml"
#define HUMAX_UPLOADFILE_PATH					"/mnt/hd1/stb_status.xml"
#define	RP_UPLOAD_FILE							"/mnt/hd1/tmp_status.xml"
#else
#define EPG_TRIPLEID_FILE 						CONFIG_PRODUCT_USERDATA_DIR"/ipepg/tripleid.xml"
#define HUMAX_UPLOADFILE_PATH					CONFIG_PRODUCT_USERDATA_DIR"/ipepg/stb_status.xml"
#define	RP_UPLOAD_FILE							CONFIG_PRODUCT_USERDATA_DIR"/ipepg/tmp_status.xml"
#endif

#define RP_ALLOWED_DEVICE_FILE					CONFIG_PRODUCT_USERDATA_DIR"/allowedmaclist.xml"

#define IPEPG_TEMP_ZIP_FILE						"/tmp/temp.gz"
#define IPEPG_TEMP_DECOMPRESS_FILE				"/tmp/temp.dat"

#define IPEPG_START_HOUR						4				// mac으로 시간을 가져오지 못하면 4시 30분에 시작하게 한다.
#define IPEPG_START_MINUTE						30
#define IPEPG_DEFAULT_START_HOUR				1				// 새벽 1시 이후에 받도록 한다.

#define IPEPG_HTTP_DOWNLOAD_RETRY_WAITTIME		(180)			// IPEPG DATA의 다운로드를 재시도 할때 기다리는 대기 시간
#define IPEPG_HTTP_TIMEOUT						(240)			// EPG Get 시도시 240초 동안 들어오지 않으면 fail 되어 다음 EPG를 받는다.

#define IPEPG_SERVICE_LIST_MAX_SIZE				(1024*1024)		// Download 할 svclist xml의 MAX SIZE를 설정한다.
#define IPEPG_EVENT_DATA_MAX_SIZE				(4*1024*1024)	// Download 할 epg xml의 MAX SIZE를 설정한다.
#define HUMAX_EPG_PID_KIND 						(2)				// IP EPG의 default 종류 이며 epg 공급 업체가 변경 되거나 server 팀의 요청시 주정될수 있다.


/*
 IPEPG_VERSION_CHECK
 ===================

 VersionNumber 규칙
 ------------------
 Major.Minor
	 Major: integer [IPEPG_VERSION_MAJOR]
	 Minor: integer [IPEPG_VERSION_MINOR]

 값 관련
 ------------------
 	version 값이 기존과 다르다면(크던 작던 상관하지 않는다), DB를 삭제한다.
 	(DxChannelLogo_t.db / DxIpService_t.db / eDxEPG_TYPE_IP.db)

 	 - BASE(_ipepg_config_base.h) 파일에서는, Major의 값만 수정하고, Minor값은 0으로 유지한다.
	 - Major는 BASE 파일에서 설정한다.
	 	- Major의 변경은 DB의 구조가 변경이 될 수 밖에 없는 경우 변경 한다.
	 - Minor는 BASE 파일에서는 기본 값만 설정하고, 향별 config 파일에서 수정한다. (_ipepg_config_[model].h)

	 - 기존에 Version 관리를 하지 않은 경우,
	 	비교 여부 및 DB삭제는, 모델별로 결정한다.
*/
#define IPEPG_VERSION_CHECK
#if defined (IPEPG_VERSION_CHECK)
	// @@@ 기존에 version 관리를 하지 않은 경우, 무조건 삭제 하기 위한 정의 @@@
	#define IPEPG_VERSION_CHECK_NO_VERSIONFILE
#endif

#define IPEPG_VERSION_MAJOR 	"1" // BASE 파일에서는 MAJOR의 값만 수정한다.
#define IPEPG_VERSION_MINOR 	"0" // BASE 파일에서는 MINOR의 값을 수정하지 않는다. (!!! DO NOT UPDATE !!!)
#define IPEPG_VERSION_NUM		IPEPG_VERSION_MAJOR"."IPEPG_VERSION_MINOR
#define IPEPG_VERSION_DIR		CONFIG_PRODUCT_USERDATA_DIR
#define IPEPG_VERSION_FILE		CONFIG_PRODUCT_USERDATA_DIR "/ipepg_version.cfg"



#ifdef __cplusplus
}
#endif


#endif // __IPEPG_CONFIG_BASE_H__



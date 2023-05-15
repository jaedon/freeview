/* $Header: $ */

#ifndef __IPEPG_CONFIG_FVP4000T_H__
#define __IPEPG_CONFIG_FVP4000T_H__

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_____INSIDE_IPEPG_CONFIG_H_____)
	#error ("!!! _ipepg_config_base.h shall be included only in the ipepg_config.h !!!\n");
#endif

#undef IPEPG_USE_COMPRESS

// TODO: dapi include 하는 잘못된 구조. 어떻게 고쳐야 하지?
#include 	"../../../../octo/service/dama/include/dapi.h"

typedef enum
{
	IPEPG_FVC_STATE_IDLE = 0,
	IPEPG_FVC_STATE_PROCESSING,	// uploading || downloading || rp processing
	IPEPG_FVC_STATE_STOPPING,
	IPEPG_FVC_STATE_NUMBER
}IPEPG_FVC_STATE_e;

#define IPEPG_FVC_LEFT_JOB_WHOLE_MASK  0x003F
typedef enum
{
	IPEPG_FVC_LEFT_JOB_NOTHING = 0x0000,
	IPEPG_FVC_LEFT_JOB_SVC_UPLOAD = 0x0001,
	IPEPG_FVC_LEFT_JOB_SVC_DOWNLOAD = 0x0002,
	IPEPG_FVC_LEFT_JOB_EPG_DOWNLOAD = 0x0004,
	IPEPG_FVC_LEFT_JOB_RP_UPLOAD = 0x0008,
	IPEPG_FVC_LEFT_JOB_RP_DOWNLOAD = 0x0010,
	IPEPG_FVC_LEFT_JOB_WAIT_RETRY = 0X0020,
	IPEPG_FVC_LEFT_JOB_SERIES_EPG_DOWNLOAD = 0X0040,
	IPEPG_FVC_LEFT_JOB_NUMBER,
}IPEPG_FVC_LEFT_JOB_e;


#undef IPEPG_TEST_DIFFERENT_TIME
#if defined(CONFIG_DEBUG)
//#define	IPEPG_TEST_DIFFERENT_TIME											// 현재시간 변경 test 진행.
#if defined(IPEPG_TEST_DIFFERENT_TIME)										// live stream 이 아닌 경우 epg 시간과 live stream 시간을 변경하여 화면에 보이도록 한다.
#undef CUR_YEAR
#undef CUR_MONTH
#undef CUR_DAY
#define CUR_YEAR								(2014)
#define CUR_MONTH								(10)
#define CUR_DAY									(22)
#endif

#endif

#define IPEPG_HEADER_MAX_SIZE				(256*1024)	// Download 할 epg xml의 MAX SIZE를 설정한다.

#define IPEPG_FVC_MAX_DELETE_QUERY	(400)

#undef IPEPG_HTTP_DOWNLOAD_RETRY_WAITTIME
#define IPEPG_HTTP_DOWNLOAD_RETRY_WAITTIME (5)

#undef IPEPG_HTTP_TIMEOUT
#define IPEPG_HTTP_TIMEOUT (30)

#define IPEPG_FVC_URL_FILE			"/url/url_fvc.txt"

#undef IPEPG_PART_OF_TIME
#define	IPEPG_PART_OF_TIME (3)

#undef IPEPG_SVCID_URL
#define IPEPG_SVCID_URL	""

#undef IPEPG_SERVICELIST_URL
#define IPEPG_SERVICELIST_URL "services"

#undef IPEPG_EPGLIST_URL
#define IPEPG_EPGLIST_URL "schedules"

#undef IPEPG_STARTTIME_URL
#define IPEPG_STARTTIME_URL "start"

#define IPEPG_SERVICEID_URL "sids[]"
#define	IPEPG_NETWORK_ID_URL "nid"


#undef IPEPG_ENDTIME_URL
#define IPEPG_ENDTIME_URL "end"


#undef IPEPG_TOTAL_TIME
#undef IPEPG_BACKWARD_SUPPORT
#undef IPEPG_BACKWARD_TOTAL_TIME

#define IPEPG_TOTAL_TIME						(7*24)				// backward IP만 테스트 하기위해 받지 않는다.
#define IPEPG_BACKWARD_SUPPORT
#define IPEPG_BACKWARD_TOTAL_TIME  				(7*24)			// 1주치를 받는다. 			24시간 * 7 일

#define IPEPG_FVC_HEADER_EXPIRES			"Expires"
#define IPEPG_FVC_HEADER_CACHE_CONTROL		"Cache-Control"
#define IPEPG_FVC_HEADER_LAST_MODIFIED		"Last-Modified"
#define IPEPG_FVC_HEADER_IF_MODIFIED_SINCE	"If-Modified-Since"
#define	IPEPG_FVC_HEADER_BASE_URL			"x-baseurl"
#define	IPEPG_FVC_HEADER_AUTH_HASH			"x-auth-hash"
#define	IPEPG_FVC_HEADER_AUTH_TIMESTAMP		"x-auth-timestamp"
#define	IPEPG_FVC_STATUS_CODE_BAD			eHxTP_HTTP_RESCODE_BAD_REQUEST
#define	IPEPG_FVC_STATUS_CODE_NOT_MODIFIED	eHxTP_HTTP_RESCODE_NOT_MODIFIED
#define	IPEPG_FVC_STATUS_CODE_OK			eHxTP_HTTP_RESCODE_OK
#define	IPEPG_FVC_STATUS_CODE_UNAUTHORIZED	eHxTP_HTTP_RESCODE_UNAUTHORIZED

#define IPEPG_THUMBNAIL_NON_DOWN

#define IPEPG_VERSION_CHECK
#if defined (IPEPG_VERSION_CHECK)
	#undef IPEPG_VERSION_CHECK_NO_VERSIONFILE
	#undef IPEPG_VERSION_MINOR
	#undef IPEPG_VERSION_NUM

	#define IPEPG_VERSION_CHECK_NO_VERSIONFILE	// @@@ 기존에 version 관리를 하지 않은 경우, 무조건 삭제 하기 위한 정의 @@@
	#define IPEPG_VERSION_MINOR 	"8"			// 파생 파일에서는 MAJOR의 값은 수정하지 않는다.
	#define IPEPG_VERSION_NUM		IPEPG_VERSION_MAJOR"."IPEPG_VERSION_MINOR
#endif

// remote recording을 쓰지 않을때 필요없는 기능을 끄도록 만들었다. 켜더라도 새로 넣어야 하는 코드가 많다.
#define	IPEPG_REMOTE_RECORDING_ON
// caching을 켜게 되면 epg를 계속 업데이트 해준다. 윗 레이어에서 필요할때 호출하는 방식을 사용할때는 caching을 켜면 안 된다.
//#define	IPEPG_FVC_EPG_CACHING_ON
// season episode를 켜기 위해서는 freeview에서 season, episode 정보를 받고, 그 정보를 parsing을 해줘야한다. 그러면  자동으로 ip epg->schedule-> recording meta로 들어간다.
//#define IPEPG_GET_SERIES_SCHEDULE_SEASON_EPISODE

#define TVPORTAL_ROOT_CA	"/usr/browser/opera_dir/certs/root/humax_tvportal_rootcert_2k_20101106_pub.pem"
#define TVPORTAL_CLIENT_CA_KEY	"/usr/browser/opera_dir/certs/client/htvdev_portal_ccert_2k_20101106.p12"
#define HTTPS_PREFIX_URL_LENGTH		8
#define HTTPS_PREFIX		"https://"

#ifdef __cplusplus
}
#endif


#endif // __IPEPG_CONFIG_FVP4000T_H__



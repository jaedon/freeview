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

// TODO: dapi include �ϴ� �߸��� ����. ��� ���ľ� ����?
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
//#define	IPEPG_TEST_DIFFERENT_TIME											// ����ð� ���� test ����.
#if defined(IPEPG_TEST_DIFFERENT_TIME)										// live stream �� �ƴ� ��� epg �ð��� live stream �ð��� �����Ͽ� ȭ�鿡 ���̵��� �Ѵ�.
#undef CUR_YEAR
#undef CUR_MONTH
#undef CUR_DAY
#define CUR_YEAR								(2014)
#define CUR_MONTH								(10)
#define CUR_DAY									(22)
#endif

#endif

#define IPEPG_HEADER_MAX_SIZE				(256*1024)	// Download �� epg xml�� MAX SIZE�� �����Ѵ�.

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

#define IPEPG_TOTAL_TIME						(7*24)				// backward IP�� �׽�Ʈ �ϱ����� ���� �ʴ´�.
#define IPEPG_BACKWARD_SUPPORT
#define IPEPG_BACKWARD_TOTAL_TIME  				(7*24)			// 1��ġ�� �޴´�. 			24�ð� * 7 ��

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

	#define IPEPG_VERSION_CHECK_NO_VERSIONFILE	// @@@ ������ version ������ ���� ���� ���, ������ ���� �ϱ� ���� ���� @@@
	#define IPEPG_VERSION_MINOR 	"8"			// �Ļ� ���Ͽ����� MAJOR�� ���� �������� �ʴ´�.
	#define IPEPG_VERSION_NUM		IPEPG_VERSION_MAJOR"."IPEPG_VERSION_MINOR
#endif

// remote recording�� ���� ������ �ʿ���� ����� ������ �������. �Ѵ��� ���� �־�� �ϴ� �ڵ尡 ����.
#define	IPEPG_REMOTE_RECORDING_ON
// caching�� �Ѱ� �Ǹ� epg�� ��� ������Ʈ ���ش�. �� ���̾�� �ʿ��Ҷ� ȣ���ϴ� ����� ����Ҷ��� caching�� �Ѹ� �� �ȴ�.
//#define	IPEPG_FVC_EPG_CACHING_ON
// season episode�� �ѱ� ���ؼ��� freeview���� season, episode ������ �ް�, �� ������ parsing�� ������Ѵ�. �׷���  �ڵ����� ip epg->schedule-> recording meta�� ����.
//#define IPEPG_GET_SERIES_SCHEDULE_SEASON_EPISODE

#define TVPORTAL_ROOT_CA	"/usr/browser/opera_dir/certs/root/humax_tvportal_rootcert_2k_20101106_pub.pem"
#define TVPORTAL_CLIENT_CA_KEY	"/usr/browser/opera_dir/certs/client/htvdev_portal_ccert_2k_20101106.p12"
#define HTTPS_PREFIX_URL_LENGTH		8
#define HTTPS_PREFIX		"https://"

#ifdef __cplusplus
}
#endif


#endif // __IPEPG_CONFIG_FVP4000T_H__



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
//#define	IPEPG_TEST_DIFFERENT_TIME											// ����ð� ���� test ����.
//#define IPEPG_TEST_DOWNLOAD_REPEAT_TIME			(3600)						// 60 �� ���� �ٽ� �޵��� test ����.
//#define IPEPG_DEBUG_DOWNLOAD_XML					// XML�� �����Ͽ� Ȯ���غ��� ���� �뵵 // size �� Ŀ���� momory�� ������� �ʾ� ������ ����.
//#define IPEPG_DEBUG_DOWNLOAD_XML_PATH				"/tmp/ipepg_xml_data"

#endif

#if !defined(CONFIG_DEBUG) && defined(CONFIG_DEVICE_DEBUG)
//#define CFG_RELEASE_DEBUG
//#define CFG_OPEN_DEBUG
#endif
#define IPEPG_MAX_IMAGE_SIZE					(300)
#define IPEPG_MAX_DOWNLOAD_RETRY_COUNT			(3)					// Download ���н�, ��õ��� �ִ� Ƚ��


#define IPEPG_PART_OF_TIME						(24)					// �ð����� ������ �޴´�. 	24�ð�.
#define IPEPG_TOTAL_TIME						(24*14)				// 2��ġ�� �޴´�. 			24�ð� * 14 �ð�
#define IPEPG_DELAYED_TIME						(1000)
#define IPEPG_OFFSET_TIME						(-2*60*60)			// current time�������� 2�ð� ������ 2��ġ�� �޴´�.

//#define IPEPG_BACKWARD_SUPPORT
#define IPEPG_BACKWARD_TOTAL_TIME  				(24*14)			// 2��ġ�� �޴´�. 			24�ð� * 14 �ð�


#if defined(IPEPG_TEST_DIFFERENT_TIME)										// live stream �� �ƴ� ��� epg �ð��� live stream �ð��� �����Ͽ� ȭ�鿡 ���̵��� �Ѵ�.
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

#define IPEPG_START_HOUR						4				// mac���� �ð��� �������� ���ϸ� 4�� 30�п� �����ϰ� �Ѵ�.
#define IPEPG_START_MINUTE						30
#define IPEPG_DEFAULT_START_HOUR				1				// ���� 1�� ���Ŀ� �޵��� �Ѵ�.

#define IPEPG_HTTP_DOWNLOAD_RETRY_WAITTIME		(180)			// IPEPG DATA�� �ٿ�ε带 ��õ� �Ҷ� ��ٸ��� ��� �ð�
#define IPEPG_HTTP_TIMEOUT						(240)			// EPG Get �õ��� 240�� ���� ������ ������ fail �Ǿ� ���� EPG�� �޴´�.

#define IPEPG_SERVICE_LIST_MAX_SIZE				(1024*1024)		// Download �� svclist xml�� MAX SIZE�� �����Ѵ�.
#define IPEPG_EVENT_DATA_MAX_SIZE				(4*1024*1024)	// Download �� epg xml�� MAX SIZE�� �����Ѵ�.
#define HUMAX_EPG_PID_KIND 						(2)				// IP EPG�� default ���� �̸� epg ���� ��ü�� ���� �ǰų� server ���� ��û�� �����ɼ� �ִ�.


/*
 IPEPG_VERSION_CHECK
 ===================

 VersionNumber ��Ģ
 ------------------
 Major.Minor
	 Major: integer [IPEPG_VERSION_MAJOR]
	 Minor: integer [IPEPG_VERSION_MINOR]

 �� ����
 ------------------
 	version ���� ������ �ٸ��ٸ�(ũ�� �۴� ������� �ʴ´�), DB�� �����Ѵ�.
 	(DxChannelLogo_t.db / DxIpService_t.db / eDxEPG_TYPE_IP.db)

 	 - BASE(_ipepg_config_base.h) ���Ͽ�����, Major�� ���� �����ϰ�, Minor���� 0���� �����Ѵ�.
	 - Major�� BASE ���Ͽ��� �����Ѵ�.
	 	- Major�� ������ DB�� ������ ������ �� �� �ۿ� ���� ��� ���� �Ѵ�.
	 - Minor�� BASE ���Ͽ����� �⺻ ���� �����ϰ�, �⺰ config ���Ͽ��� �����Ѵ�. (_ipepg_config_[model].h)

	 - ������ Version ������ ���� ���� ���,
	 	�� ���� �� DB������, �𵨺��� �����Ѵ�.
*/
#define IPEPG_VERSION_CHECK
#if defined (IPEPG_VERSION_CHECK)
	// @@@ ������ version ������ ���� ���� ���, ������ ���� �ϱ� ���� ���� @@@
	#define IPEPG_VERSION_CHECK_NO_VERSIONFILE
#endif

#define IPEPG_VERSION_MAJOR 	"1" // BASE ���Ͽ����� MAJOR�� ���� �����Ѵ�.
#define IPEPG_VERSION_MINOR 	"0" // BASE ���Ͽ����� MINOR�� ���� �������� �ʴ´�. (!!! DO NOT UPDATE !!!)
#define IPEPG_VERSION_NUM		IPEPG_VERSION_MAJOR"."IPEPG_VERSION_MINOR
#define IPEPG_VERSION_DIR		CONFIG_PRODUCT_USERDATA_DIR
#define IPEPG_VERSION_FILE		CONFIG_PRODUCT_USERDATA_DIR "/ipepg_version.cfg"



#ifdef __cplusplus
}
#endif


#endif // __IPEPG_CONFIG_BASE_H__



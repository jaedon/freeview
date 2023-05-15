/* $Header: $ */

#ifndef __IPEPG_CONFIG_ICORDPRO_H__
#define __IPEPG_CONFIG_ICORDPRO_H__

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_____INSIDE_IPEPG_CONFIG_H_____)
	#error ("!!! _ipepg_config_iordpro.h shall be included only in the ipepg_config.h !!!\n");
#endif


// �޸� �������� 8��ġ ���� (Series Recording�� ���ؼ� 7�� + 1��)
#undef IPEPG_TOTAL_TIME
#define IPEPG_TOTAL_TIME			(24*8)			// 8��ġ�� �޴´�. 			24�ð� * 8��

#define IPEPG_VERSION_CHECK
#if defined (IPEPG_VERSION_CHECK)
	#undef IPEPG_VERSION_CHECK_NO_VERSIONFILE
	#undef IPEPG_VERSION_MINOR
	#undef IPEPG_VERSION_NUM

	#define IPEPG_VERSION_CHECK_NO_VERSIONFILE	// @@@ ������ version ������ ���� ���� ���, ������ ���� �ϱ� ���� ���� @@@
	#define IPEPG_VERSION_MINOR 	"1"			// �Ļ� ���Ͽ����� MAJOR�� ���� �������� �ʴ´�.
	#define IPEPG_VERSION_NUM		IPEPG_VERSION_MAJOR"."IPEPG_VERSION_MINOR
#endif

#if defined(CONFIG_DEBUG)
	#if defined(IPEPG_TEST_DIFFERENT_TIME)				// live stream �� �ƴ� ��� epg �ð��� live stream �ð��� �����Ͽ� ȭ�鿡 ���̵��� �Ѵ�.
		#undef CUR_YEAR
		#undef CUR_MONTH
		#undef CUR_DAY

		#define CUR_YEAR								(2014)
		#define CUR_MONTH								(03)
		#define CUR_DAY									(26)
	#endif

#endif

#if defined(CONFIG_DEBUG)

	//#define IPEPG_DEBUG_DOWNLOAD_XML									// XML�� �����Ͽ� Ȯ���غ��� ���� �뵵
	//#define IPEPG_DEBUG_DOWNLOAD_XML_PATH				"/tmp/ipepg_xml_data"

#endif


#define IPEPG_PRIORITY_DOWN
#if defined (IPEPG_PRIORITY_DOWN)
	#define IPEPG_PRIORITY_XML_PATH		"/etc/"   //xml�� ���� ���
	#define IPEPG_PRIORITY_INTERVAL		5        //2��ġ �޴� ä���� ����, ex) 50���� ������ ��� �켱������ ���� ä�� 50�� ���� 2��ġ�� �޴´�.
	#define IPEPG_MAX_EVENTCOUNT		130000    //total event ����
#endif

#define IPEPG_THUMBNAIL_NON_DOWN

#ifdef __cplusplus
}
#endif


#endif // __IPEPG_CONFIG_ICORDPRO_H__



/* $Header: $ */

#ifndef __IPEPG_CONFIG_HMS1000T_H__
#define __IPEPG_CONFIG_HMS1000T_H__

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_____INSIDE_IPEPG_CONFIG_H_____)
	#error ("!!! _ipepg_config_hms1000t.h shall be included only in the ipepg_config.h !!!\n");
#endif


// ȣ���� IPEPG�� ���, iceTV���� forwardEPG�� 1���� ġ�� �����Ѵ�.
#define IPEPG_TOTAL_TIME			(24*7)			// 1��ġ�� �޴´�. 			24�ð� * 7��

#define IPEPG_BACKWARD_SUPPORT
#define IPEPG_BACKWARD_TOTAL_TIME  	(24*14)			// 2��ġ�� �޴´�. 			24�ð� * 14��
#define IPEPG_PARSER_RATING_DO_NOT_MODIFY				// Rating ������ �Ľ��Ҷ�, ������ ������ ���� �ʰ� �״�� �����Ѵ�. 


#if defined(CONFIG_DEBUG)
	//#define IPEPG_FUNCTION_COLOR_TRACE									// �Լ��� ���� �� ���� ����ϴ� �α׿�, ���� ����Ѵ�
	//#define	IPEPG_TEST_DIFFERENT_TIME									// ����ð� ���� test ����.		
	//#define IPEPG_TEST_DOWNLOAD_REPEAT_TIME				(60*5)			// 120 �� ���� �ٽ� �޵��� test ����. 
	
	#define IPEPG_DEBUG_DOWNLOAD_XML									// XML�� �����Ͽ� Ȯ���غ��� ���� �뵵
	#define IPEPG_DEBUG_DOWNLOAD_XML_PATH				"/tmp/ipepg_xml_data"
	#if defined(IPEPG_TEST_DIFFERENT_TIME)				// live stream �� �ƴ� ��� epg �ð��� live stream �ð��� �����Ͽ� ȭ�鿡 ���̵��� �Ѵ�.		
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

	#define IPEPG_VERSION_CHECK_NO_VERSIONFILE	// @@@ ������ version ������ ���� ���� ���, ������ ���� �ϱ� ���� ���� @@@		
	#define IPEPG_VERSION_MINOR 	"1"			// �Ļ� ���Ͽ����� MAJOR�� ���� �������� �ʴ´�.
	#define IPEPG_VERSION_NUM		IPEPG_VERSION_MAJOR"."IPEPG_VERSION_MINOR	
#endif

#ifdef __cplusplus
}
#endif


#endif // __IPEPG_CONFIG_HMS1000T_H__



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

#define IPEPG_PART_OF_TIME						(24*14)					// �ð����� ������ �޴´�. 	24�ð�.
#define IPEPG_TOTAL_TIME						(24*14)				// 1��ġ�� �޴´�. 			24�ð� * 7 �ð�
#define EPG_TRIPLEID_FILE 						CONFIG_PRODUCT_USERDATA_DIR"/ipepg/tripleid.xml"
#define HUMAX_UPLOADFILE_PATH					CONFIG_PRODUCT_USERDATA_DIR"/ipepg/stb_status.xml"
#define	RP_UPLOAD_FILE							CONFIG_PRODUCT_USERDATA_DIR"/ipepg/tmp_status.xml"

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

#if defined(CONFIG_DEBUG)
	#if defined(IPEPG_TEST_DIFFERENT_TIME)				// live stream �� �ƴ� ��� epg �ð��� live stream �ð��� �����Ͽ� ȭ�鿡 ���̵��� �Ѵ�.
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
		#define IPEPG_PRIORITY_XML_PATH		CONFIG_PRODUCT_USERDATA_DIR"/ipepg/"   //xml�� ���� ���
		#define IPEPG_PRIORITY_INTERVAL		5        //2��ġ �޴� ä���� ����, ex) 50���� ������ ��� �켱������ ���� ä�� 50�� ���� 2��ġ�� �޴´�.
		#define IPEPG_PRIORITY_LIMIT_CNT	100000		// 250�� ���񽺸� �޴´�.
	#else
		#define IPEPG_PRIORITY_XML_PATH		"/etc/"   //xml�� ���� ���
		#define IPEPG_PRIORITY_INTERVAL		5        //2��ġ �޴� ä���� ����, ex) 50���� ������ ��� �켱������ ���� ä�� 50�� ���� 2��ġ�� �޴´�.
		#define IPEPG_PRIORITY_LIMIT_CNT	250		// 250�� ���񽺸� �޴´�.
	#endif
#endif

#define IPEPG_THUMBNAIL_NON_DOWN

#ifdef __cplusplus
}
#endif


#endif // __IPEPG_CONFIG_HMS1000T_H__



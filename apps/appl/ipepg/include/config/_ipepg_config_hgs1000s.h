/* $Header: $ */

#ifndef __IPEPG_CONFIG_HGS1000S_H__
#define __IPEPG_CONFIG_HGS1000S_H__

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_____INSIDE_IPEPG_CONFIG_H_____)
	#error ("!!! _ipepg_config_hgs1000s.h shall be included only in the ipepg_config.h !!!\n");
#endif

/* Receive the specified day as below due to the lack of memory. */
#undef	IPEPG_TOTAL_TIME
#define	IPEPG_TOTAL_TIME			(24 * 8)	/* receive one day more for series recording (24hours * 8days) */

#define IPEPG_VERSION_CHECK
#if defined (IPEPG_VERSION_CHECK)
	#undef IPEPG_VERSION_CHECK_NO_VERSIONFILE
	#undef IPEPG_VERSION_MINOR
	#undef IPEPG_VERSION_NUM

	#define IPEPG_VERSION_CHECK_NO_VERSIONFILE	/* define to unconditionally undefine in case of reglecting the management of version in previous */
	#define IPEPG_VERSION_MINOR 	"1"			/* do not modify Major value at model file. */
	#define IPEPG_VERSION_NUM		IPEPG_VERSION_MAJOR"."IPEPG_VERSION_MINOR
#endif

#if defined(CONFIG_DEBUG)
	#if defined(IPEPG_TEST_DIFFERENT_TIME)		/* show changed epg and live stream time if it is not live stream. */
		#undef CUR_YEAR
		#undef CUR_MONTH
		#undef CUR_DAY

		#define CUR_YEAR						(2014)
		#define CUR_MONTH						(03)
		#define CUR_DAY							(26)
	#endif

	#define IPEPG_FUNCTION_COLOR_TRACE			/* Color Trace for Debug */
	//#define IPEPG_PRINT_DUMP
	//#define IPEPG_DEBUG_DOWNLOAD_XML			/* purpose of checking xml file */
	//#define IPEPG_DEBUG_DOWNLOAD_XML_PATH		"/tmp/ipepg_xml_data"
#endif

#define IPEPG_EVENT_UPDATE_SLEEP				(10)

#define IPEPG_PRIORITY_DOWN
#if defined (IPEPG_PRIORITY_DOWN)
	#define IPEPG_PRIORITY_XML_PATH		"/etc/"	/* xml file path that is received from the german brach office. */
	#define IPEPG_PRIORITY_INTERVAL		5		/* download interval of each services,
												   e.g. 5 stands for downloading 2 weeks ipepg data from 5 services in order of priority */
	#define IPEPG_PRIORITY_LIMIT_CNT	250		/* download 250 services */
	#define IPEPG_PRIORITY_LIMIT_SKIP_CNT		5		/* Limit download try counts */
#endif

#define IPEPG_THUMBNAIL_NON_DOWN

#ifdef __cplusplus
}
#endif

#endif /* __IPEPG_CONFIG_HGS1000S_H__ */


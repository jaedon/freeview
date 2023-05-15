

#ifndef _DTCP_CONFIG_H
#define _DTCP_CONFIG_H


//#include "../../mxlib_config.h"
#include "mxlib_config.h"

/* Due to lack of SRM, DTCP-AKE fail case happens. From now on, SRM is enabled for All models. ALM #332392 */
//#define DTCP_SRM_SKIP

///////////// VALUPS CONFIGURATION //////////////////////



#ifdef HUMAX_LEGACY_DTCP_CUSTOM
	#define DTCP_HUMAX_CUSTOM_KEY
#endif



#ifdef DTCP_DEBUG
	#define DEMO_MODE
	#define DEMO_PACKET
#endif


#ifdef DTCP_MOVE_SUPPORT

	#define DTCP_RTT_SUPPORT

#endif

#ifdef DTCP_RTT_SUPPORT

	#define DTCP_SUPPORT_ENHANCED

#endif




#define DTCP_KEY_ONE_FILE




/////////////////////////////////////////////////////////
// #define MXDTCP_MALLOC_TEST
/////////////////////////////////////////////////////////

#ifdef MXDTCP_MALLOC_TEST

	#define MXDTCP_MALLOC		MX_TEST_MALLOC
	#define MXDTCP_FREE			MX_TEST_FREE
	#define MXDTCP_REALLOC		MX_TEST_REALLOC
	#define MXDTCP_CALLOC		MX_TEST_CALLOC
	#define MXDTCP_FREE_NULL	MX_TEST_FREE_NULL
	#define MXDTCP_STRDUP		MX_TEST_STRDUP

//#define MX_MALLOC #error
//#define MX_FREE #error

#else

	#define MXDTCP_MALLOC		MX_MALLOC
	#define MXDTCP_FREE			MX_FREE
	#define MXDTCP_REALLOC		MX_REALLOC
	#define MXDTCP_CALLOC		MX_CALLOC
	#define MXDTCP_FREE_NULL	MX_FREE_NULL
	#define MXDTCP_STRDUP		MX_STRDUP

#endif


#ifdef MXDTCP_MALLOC_TEST

#define MXDTCP_MEMORY_CHECK

#endif

#endif

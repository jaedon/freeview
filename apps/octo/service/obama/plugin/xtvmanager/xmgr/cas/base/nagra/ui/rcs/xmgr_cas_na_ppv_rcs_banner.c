/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <ray_popupui_coordinate.h>

#include <na_mgr_cak_ext.h>
#include <settings.h>

#include <namgr_main.h>

#include "../../local_include/_xmgr_cas_na_util.h"
#include "../../local_include/_xmgr_cas_na_adapt.h"
#include "../../local_include/_xmgr_cas_na.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define XMGR_CAS_NAPPV_RCS_PARAM_WARRING_REMOVE(arg1, arg2, arg3, arg4)				\
			HxLOG_Info("\n");
			{															\
			(void)arg1;													\
			(void)arg2;													\
			(void)arg3;													\
			(void)arg4;					 								\
			}
#define XMGR_CAS_NAPPV_RCS_POINTER_INVALID_CHECK(pArg, returnValue)					\
			HxLOG_Info("\n");
			if(pArg == NULL)													\
			{																	\
			HxLOG_Critical("\n\n");												\
			return returnValue;													\
			}
#define XMGR_CAS_NAPPV_RCS_HERROR_INVALID_CHECK(pArg, returnValue)					\
			HxLOG_Info("\n");
			if(pArg != ERR_BUS_NO_ERROR)									\
			{															\
			HxLOG_Critical("\n\n");												\
			return returnValue;											\
			}


/* end of file */

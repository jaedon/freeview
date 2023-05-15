/***************************************************************
* $Workfile:   ci_mgr.h  $
* $Modtime:   Jan 12 2007 23:26:48  $
* Auther : Jaehee Cho / hmkim (CI+)
*
* Desc :
*
* Copyright (c) 2002 by Humax Co., Ltd.
* All right reserved
****************************************************************/

#ifndef __CIMGR_H__
#define __CIMGR_H__

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************
* include
***************************************************************/

#include <octo_common.h>

#include <_svc_cas_mgr_common.h>



/***************************************************************
* definition
***************************************************************/

/***************************************************************
* typedef
***************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

/* for debug cmd */
void CI_PrintCAMInfo(void);
void CI_PrintContextInfo(void);
HERROR CI_PrintSdtStatus(void);
void CI_PrintUriStatus(HUINT16 usSlotIdToCheck);

#if defined(CONFIG_DEBUG) && defined(CONFIG_MW_CI_PLUS)
void CI_Debug_SetUriStatus(void *pstVoidUriInfo);
#endif // #if defined(CONFIG_DEBUG) && defined(CONFIG_MW_CI_PLUS)


#ifdef __cplusplus
}
#endif

#endif /* __CIMGR_H__ */


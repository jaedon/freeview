/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/16 $
 * $brcm_Date: 2/10/12 5:25p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/bchp_priv.h $
 * 
 * Hydra_Software_Devel/16   2/10/12 5:25p rjlewis
 * SW7425-2218: removed AvsLock/Unlock and added get handle.
 * 
 * Hydra_Software_Devel/15   2/1/12 7:03p rjlewis
 * SW7425-2216: Adding support for AVS Lock/Unlock.
 * 
 * Hydra_Software_Devel/14   8/16/11 11:31a rjlewis
 * SW7346-117: Added support for a chip specific stand-by mode function.
 * 
 * Hydra_Software_Devel/13   8/3/11 11:46a rjlewis
 * SW7346-117: added support for GetAvsData function.
 * 
 * Hydra_Software_Devel/12   6/20/11 12:01p jtna
 * SW7425-752: handle shared nodes correctly
 * 
 * Hydra_Software_Devel/11   1/20/11 1:40p gmohile
 * SW7408-212 : Add support for avs settings
 * 
 * Hydra_Software_Devel/10   1/18/11 4:12p jtna
 * SW7420-972: added initComplete member
 * 
 * Hydra_Software_Devel/9   11/18/10 7:33p jtna
 * SW7420-972: refactor init process
 * 
 * Hydra_Software_Devel/8   10/20/10 6:45p jtna
 * SW7420-972: rewrite to support HW_ -> HW_ dependency
 * 
 * Hydra_Software_Devel/7   9/24/10 4:29p jtna
 * SW7420-972: change regHandle from void* to BREG_Handle
 * 
 * Hydra_Software_Devel/6   9/1/10 3:34p jtna
 * SW7420-972: add bchp_pwr resource init scheme
 * 
 * Hydra_Software_Devel/5   8/20/10 12:31p jtna
 * SW7405-4433: change pHandle to handle
 * 
 * Hydra_Software_Devel/4   8/19/10 3:10p jtna
 * SW7405-4433: merge BCHP_PWR
 * 
 * Hydra_Software_Devel/3   6/10/04 5:43p jasonh
 * PR 11257: Added initial support of BCHP_GetFeature for 7038 chipsets.
 * 
 * Hydra_Software_Devel/2   9/26/03 8:49a dlwin
 * Updated to return 16 bits for Chip ID and Chip Rev.
 * 
 * Hydra_Software_Devel/1   9/24/03 11:30a dlwin
 * Initial version
 * 
 ***************************************************************************/
#ifndef BCHP_PRIV_H__
#define BCHP_PRIV_H__

#include "bstd.h"
#include "bchp.h"
#include "breg_mem.h"
#include "bkni_multi.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
	This typedef defines the pointer to chip Close function.

Description:
	If the close function pointer is used, then open function pointer must
	also exist.

See Also:
	BCHP_Close()

****************************************************************************/
typedef BERR_Code (*BCHP_CloseFunc)(
	BCHP_Handle hChip						/* [in] Chip handle */
	);

/***************************************************************************
Summary:
	This typedef defines the pointer to chip GetChipInfo function.

Description:
	This function pointer is provided for chips that requires specific needs
	when BCHP_GetChipInfo() is called.

See Also:
	BCHP_GetChipInfo()

****************************************************************************/
typedef BERR_Code (*BCHP_GetChipInfoFunc)(
	const BCHP_Handle hChip,				/* [in] Chip handle */
	uint16_t *pChipId,						/* [out] Chip Id */
	uint16_t *pChipRev						/* [out] Chip Rev. */
	);

/***************************************************************************
Summary:
	This typedef defines the pointer to chip GetFeature function.

Description:
	This function pointer is provided for chips that have specific features
	that need to be queried.

See Also:
	BCHP_GetFeature()

****************************************************************************/
typedef BERR_Code (*BCHP_GetFeatureFunc)(
	const BCHP_Handle hChip,				/* [in] Chip handle. */
	const BCHP_Feature eFeature,			/* [in] Feature to be queried. */
	void *pFeatureValue						/* [out] Feature value .*/
	);

/***************************************************************************
Summary:
	This typedef defines the pointer to chip MonitorPvt function.

Description:
	This function pointer is provided for chips that have AVS

See Also:
	BCHP_MonitorPvt()

****************************************************************************/
typedef void (*BCHP_MonitorPvtFunc)(
	const BCHP_Handle hChip,	    /* [in] Chip handle. */
	BCHP_AvsSettings *pSettings     /* [in] AVS settings. */	
	);


/***************************************************************************
Summary:
	This typedef defines the pointer to chip GetAvsData function.

Description:
	This function pointer is provided for chips that have AVS.

See Also:
	BCHP_GetAvsData()

****************************************************************************/
typedef BERR_Code (*BCHP_GetAvsDataFunc)(
	const BCHP_Handle hChip,	    /* [in] Chip handle */
	BCHP_AvsData *pData             /* [out] pointer to location to return data */
	);

/***************************************************************************
Summary:
	This typedef defines the pointer to chip StandbyMode function.

Description:
	This function pointer is provided for chips that support changing power modes.

See Also:
	BCHP_StandbyMode()

****************************************************************************/
typedef BERR_Code (*BCHP_StandbyModeFunc)(
	const BCHP_Handle hChip,  /* [in] Chip handle. */
	bool activate             /* [in] true to activate standby mode */
	);


typedef struct BCHP_PWR_P_Context {
    BKNI_MutexHandle lock; /* for internal sync */
    
    unsigned *pubRefcnt;   /* public refcnt is non-recursive and enforces that you only release 
                              what you've previously acquired. only applies to nonleaf nodes */

    unsigned *privRefcnt;  /* private refcnt is recursive. applies to all nodes */
    bool *init;            /* every HW node must be initialized before it can be acquired/released. 
                              initialized means powered down */
    bool initComplete;     /* true if BCHP_PWR_P_Init() has completed */
    bool *magnumCtrl;      /* true if MAGNUM_CONTROLLED HW_ node */
    bool *sharedCtrl;      /* true if HW_ node has at least one HW_ node parent that is 
                              MAGNUM_CONTROLLED and one that is not */
} BCHP_PWR_P_Context;

typedef struct BCHP_PWR_P_Context *BCHP_PWR_Handle;
typedef struct BCHP_AVS_P_Context *BCHP_AVS_Handle;

typedef struct BCHP_P_Context
{
	void *chipHandle;						/* Chip Specific handle */
	BREG_Handle regHandle;					/* register handle */
	BCHP_CloseFunc pCloseFunc;				/* ptr to Close func. */
	BCHP_GetChipInfoFunc pGetChipInfoFunc;	/* ptr to GetChipInfo func. */
	BCHP_GetFeatureFunc pGetFeatureFunc;	/* ptr to GetFeature func. */
	BCHP_MonitorPvtFunc pMonitorPvtFunc;	/* ptr to MonitorPvtFunc func. */
	BCHP_GetAvsDataFunc pGetAvsDataFunc;	/* ptr to GetAvsDataFunc func. */
	BCHP_StandbyModeFunc pStandbyModeFunc;  /* ptr to StandbyModeFunc func. */
	BCHP_PWR_Handle pwrManager;				/* BCHP_PWR handle */
	BCHP_AVS_Handle avsHandle;				/* BCHP_AVS handle */
} BCHP_P_Context;

typedef enum BCHP_PWR_P_ResourceType {
    BCHP_PWR_P_ResourceType_eLeaf,     /* a leaf node is always a HW_ node */
    BCHP_PWR_P_ResourceType_eNonLeaf,  /* a non-leaf node that is not a HW_ node */
    BCHP_PWR_P_ResourceType_eNonLeafHw /* a non-leaf node that is also a HW_ node. 
                                          these nodes can only have other HW_ nodes as dependencies */

} BCHP_PWR_P_ResourceType;

struct BCHP_PWR_P_Resource {
    BCHP_PWR_P_ResourceType type;
    unsigned id; /* the #define number */
    const char name[32];
};

typedef struct BCHP_PWR_P_Resource BCHP_PWR_P_Resource;

BERR_Code BCHP_PWR_Open(
    BCHP_PWR_Handle *pHandle, /* [out] */
    BCHP_Handle chp           /* [in] */
    );

void BCHP_PWR_Close(
    BCHP_PWR_Handle handle   /* [in] */
    );

#ifdef __cplusplus
}
#endif

#endif /*BCHP_PRIV_H__*/


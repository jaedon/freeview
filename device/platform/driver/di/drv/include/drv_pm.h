/*
** drv_pm.h
**

**
** @defgroup
** @author
** @note
** @brief
** @file
*/
#ifndef __DRV_PM_H__
#define __DRV_PM_H__
/*----------------------------------------------------------------------------------
** Headers
**--------------------------------------------------------------------------------*/
#include "drv_err.h"
#include "drv_micom.h"

/*----------------------------------------------------------------------------------
** defines
**--------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------
** typedef
**--------------------------------------------------------------------------------*/
typedef void (*pfn_pm_cb)(void);

typedef enum {
	DRV_PM_BOOTMODE_WARM = 0,
	DRV_PM_BOOTMODE_COLD,
	DRV_PM_BOOTMODE_MAX
} DRV_PM_BOOTMODE_E;

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
typedef struct{
	HBOOL bIR;
	HBOOL bFrontKey;
	HBOOL bWOL;
	HBOOL bTimer;
	struct{
		struct{
			HUINT32 ulInterval;
		}stTimer;
	}stInfo;
}DRV_PM_STBY_Settings;
#endif

/*----------------------------------------------------------------------------------
** Variables
**--------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------
** Functions
**--------------------------------------------------------------------------------*/
#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
DRV_Error	DRV_PM_Init(void);
HBOOL		DRV_PM_GetStandbyStatus(void);
DRV_Error 	DRV_PM_GetWakeUpStatus(DRV_MICOM_WAKEUP_SRC *nWakeupSrc);
DRV_Error 	DRV_PM_StandbyStates1(int active);
DRV_Error 	DRV_PM_StandbyStates2(DRV_PM_STBY_Settings stbySettings);
DRV_Error 	DRV_PM_StandbyStates3(DRV_PM_BOOTMODE_E bootmode, DRV_PM_STBY_Settings stbySettings);
#endif
DRV_Error	DRV_PM_RegisterCallback(pfn_pm_cb pCbUninit, pfn_pm_cb  pCbReinit);
void		DRV_PM_PowerDown(void);
void		DRV_PM_PowerUp(void);
void 		DRV_PM_DemodPowerSet(HBOOL bOn);



#endif /* __DRV_PM_H__ */

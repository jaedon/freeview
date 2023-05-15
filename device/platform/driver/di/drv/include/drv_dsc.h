#ifndef __DRV_DSC_H__
#define __DRV_DSC_H__

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
#include "htype.h"
#include "drv_err.h"


/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/

/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/

DRV_Error DRV_DSC_Init(void);
NEXUS_KeySlotHandle DRV_DSC_GetKeySlotHandle(HUINT32 ulDemuxId, HUINT16 uspid);
DRV_Error DRV_DSC_OpenKeySlotForRecord(HUINT32 ulChIndex, NEXUS_KeySlotHandle *pKeySlotHandle);
DRV_Error DRV_DSC_CloseKeySlotForRecord(HUINT32 ulChIndex, NEXUS_KeySlotHandle *pKeySlotHandle);
DRV_Error DRV_DSC_DumpKeySlot(HUINT32 ulTestCase);
DRV_Error DRV_DSC_GetKeySlotByPidNum(HUINT16 usPid);
#endif /* !__DRV_DSC_H__ */



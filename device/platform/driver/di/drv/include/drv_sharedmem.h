/**
 * drv_sharedmem.h
*/

#ifndef __DRV_SHAREDMEM_H__
#define __DRV_SHAREDMEM_H__

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
#include "htype.h"
#include "drv_err.h"
#include <sys/shm.h>

#include "nexus_display.h"
#include "nexus_ir_input.h"

/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
#define DRV_DISP_SHAREDMEM_ID      5678
#define DRV_IR_SHAREDMEM_ID      0123

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
typedef struct {
    NEXUS_DisplayHandle         disp_handle0;
    NEXUS_DisplayHandle         disp_handle1;
}DRV_DISP_SHAREDMEM_t;

typedef struct {
    NEXUS_IrInputHandle gIrHandle;
}DRV_IR_SHAREDMEM_t;

/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
bool DRV_SHAREDMEM_Init(size_t shMemSz, key_t Id);
void *DRV_SHAREDMEM_Setup(size_t shMemSz, key_t Id, bool bCreate);
void DRV_SHAREDMEM_Destroy(void * shMem);

#endif /* !__DRV_AUDIO_H__ */


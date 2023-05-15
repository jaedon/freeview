/*=============================================================================
Copyright (c) 2010 Broadcom Europe Limited.
All rights reserved.

Project  :  EGL driver
Module   :  Abstract H/W Interface

FILE DESCRIPTION
Defines an abstract interface that will be used to interact with platform h/w.
=============================================================================*/

#ifndef _BEGL_HWPLATFORM_H__
#define _BEGL_HWPLATFORM_H__

#ifndef __cplusplus
#include <stdint.h>
#ifndef WIN32
#include <stdbool.h>
#else
typedef uint8_t bool;
#define false 0
#define true 1
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
   BEGL_HW_NO_COMMAND = 0,
   BEGL_HW_L2CACTL,
   BEGL_HW_SLCACTL,
   BEGL_HW_INTCTL,
   BEGL_HW_INTENA,
   BEGL_HW_INTDIS,
   BEGL_HW_CT0CS,
   BEGL_HW_CT1CS,
   BEGL_HW_CT0EA,
   BEGL_HW_CT1EA,
   BEGL_HW_CT0CA,
   BEGL_HW_CT1CA,
   BEGL_HW_PCS,
   BEGL_HW_BFC,
   BEGL_HW_RFC,
   BEGL_HW_BPOA,
   BEGL_HW_BPOS,
   BEGL_HW_CACHE_CTRL,
   BEGL_HW_PCTRC,
   BEGL_HW_PCTRE,
   BEGL_HW_PCTR0,
   BEGL_HW_PCTRS0,
   BEGL_HW_PCTR1,
   BEGL_HW_PCTRS1,
   BEGL_HW_PCTR2,
   BEGL_HW_PCTRS2,
   BEGL_HW_PCTR3,
   BEGL_HW_PCTRS3,
   BEGL_HW_PCTR4,
   BEGL_HW_PCTRS4,
   BEGL_HW_PCTR5,
   BEGL_HW_PCTRS5,
   BEGL_HW_PCTR6,
   BEGL_HW_PCTRS6,
   BEGL_HW_PCTR7,
   BEGL_HW_PCTRS7,
   BEGL_HW_PCTR8,
   BEGL_HW_PCTRS8,
   BEGL_HW_PCTR9,
   BEGL_HW_PCTRS9,
   BEGL_HW_PCTR10,
   BEGL_HW_PCTRS10,
   BEGL_HW_PCTR11,
   BEGL_HW_PCTRS11,
   BEGL_HW_PCTR12,
   BEGL_HW_PCTRS12,
   BEGL_HW_PCTR13,
   BEGL_HW_PCTRS13,
   BEGL_HW_PCTR14,
   BEGL_HW_PCTRS14,
   BEGL_HW_PCTR15,
   BEGL_HW_PCTRS15,
   BEGL_HW_GCA_PM_SEL,
   BEGL_HW_GCA_BW_CNT,
   BEGL_HW_GCA_BW_MEM_CNT,
   BEGL_HW_IDENT0,
   BEGL_HW_IDENT1,
   BEGL_HW_IDENT2,
   BEGL_HW_SQRSV0,
   BEGL_HW_IDENT3,
   BEGL_HW_SQRSV1,
   BEGL_HW_SQCNTL,
   BEGL_HW_SQCSTAT,
   BEGL_HW_SRQPC,
   BEGL_HW_SRQUA,
   BEGL_HW_SRQUL,
   BEGL_HW_SRQCS,
   BEGL_HW_SCRATCH,
   BEGL_HW_ENDSWP,
   BEGL_HW_VPACNTL,
   BEGL_HW_VPMBASE,
   BEGL_HW_BPCA,
   BEGL_HW_BPCS,
   BEGL_HW_BXCF,
   BEGL_HW_CACHE_ID,
   BEGL_HW_CACHE_SWAP_CTRL,
   BEGL_HW_CACHE_SWAP_CTRL_2,
   BEGL_HW_CACHE_STATUS,
   BEGL_HW_PM_CTRL,
   BEGL_HW_V3D_BW_CNT,
   BEGL_HW_MEM_BW_CNT,
   BEGL_HW_LOW_PRI_ID,
   BEGL_HW_MEM_PROTECT_0,
   BEGL_HW_MEM_PROTECT_1,
   BEGL_HW_MEM_PROTECT_2,
   BEGL_HW_MEM_PROTECT_3,
   BEGL_HW_MEM_PROTECT_4,
   BEGL_HW_MEM_PROTECT_5,
   BEGL_HW_MEM_PROTECT_6,
   BEGL_HW_MEM_PROTECT_7,
   BEGL_HW_MEM_PROTECT_STATUS,
   BEGL_HW_AXI_BRIDGE_STATUS,
   BEGL_HW_SCRATCH_0,
   BEGL_HW_SAFE_SHUTDOWN,
   BEGL_HW_SAFE_SHUTDOWN_ACK,
   BEGL_HW_DBCFG,
   BEGL_HW_DBSCS,
   BEGL_HW_DBSCFG,
   BEGL_HW_DBSSR,
   BEGL_HW_DBSDR0,
   BEGL_HW_DBSDR1,
   BEGL_HW_DBSDR2,
   BEGL_HW_DBSDR3,
   BEGL_HW_DBQRUN,
   BEGL_HW_DBQHLT,
   BEGL_HW_DBQSTP,
   BEGL_HW_DBQITE,
   BEGL_HW_DBQITC,
   BEGL_HW_DBQGHC,
   BEGL_HW_DBQGHG,
   BEGL_HW_DBQGHH,
   BEGL_HW_DBGE,
   BEGL_HW_FDBGO,
   BEGL_HW_FDBGB,
   BEGL_HW_FDBGR,
   BEGL_HW_FDBGS,
   BEGL_HW_ERRSTAT,
   BEGL_HW_CT00RA0,
   BEGL_HW_CT01RA0,
   BEGL_HW_CT0LC,
   BEGL_HW_CT1LC,
   BEGL_HW_CT0PC,
   BEGL_HW_CT1PC,
   BEGL_HW_BRIDGE_REVISION,
   BEGL_HW_BRIDGE_CTRL,
   BEGL_HW_BRIDGE_SW_INIT_0,
   BEGL_HW_BRIDGE_SW_INIT_1,
   BEGL_HW_INTDONE,
   BEGL_HW_NUM_COMMANDS       /* Must be last */
} BEGL_HWCommand;

typedef enum BEGL_HWShaderType
{
   BEGL_HWShaderTypeMain, 
   BEGL_HWShaderTypeUserShader,
   BEGL_HWShaderTypeMax
} BEGL_HWShaderType;

typedef void (*BEGL_HWISRCallback)(void);

typedef struct BEGL_HWAcquireSettings
{
   BEGL_HWShaderType     shaderType;
   BEGL_HWISRCallback    ISR;
   BEGL_HWISRCallback    tryAcquireAgain;
   uint32_t              actualVPM;
   uint32_t              minMaxVPM;
   bool                  forceReset;
} BEGL_HWAcquireSettings;

typedef struct BEGL_HWReleaseSettings
{
   BEGL_HWShaderType     shaderType;
} BEGL_HWReleaseSettings;

/* The platform MUST provide an implementation of this interface in order that the EGL driver
 * can interact with platform hardware.
 */
typedef struct
{
   /* Context pointer - opaque to the 3d driver code, but passed out in all function pointer calls.
      Prevents the client code needing to perform context lookups. */
   void *context;

   /* Acquire access to the 3D hardware */
   bool (*Acquire)(void *context, const BEGL_HWAcquireSettings *settings);

   /* Release access to the 3D hardware */
   bool (*Release)(void *context, const BEGL_HWReleaseSettings *settings);

   /* Issue command to the 3D hardware. You must acquire first. */
   void (*SendCommand)(void *context, BEGL_HWCommand command, uint32_t data);

   /* Read status from 3D hardware */
   uint32_t (*ReadStatus)(void *context, BEGL_HWCommand status);

} BEGL_HWInterface;

#ifdef __cplusplus
}
#endif

#endif /* _BEGL_HWPLATFORM_H__ */

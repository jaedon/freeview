/*=============================================================================
Copyright (c) 2010 Broadcom Europe Limited.
All rights reserved.

Project  :  Default Nexus platform API for EGL driver
Module   :  Nexus platform 

FILE DESCRIPTION
DESC
=============================================================================*/

#include "nexus_platform.h"
#include "nexus_graphics3d.h"

#include "bkni_multi.h"

#include <EGL/egl.h>

#include <malloc.h>
#include <memory.h>
#include <assert.h>
#include <pthread.h>

typedef struct
{
   uint32_t                commandMap[BEGL_HW_NUM_COMMANDS];
   NEXUS_Graphics3dHandle  nexusHandle;
   BEGL_HWISRCallback      mainISRCallback;
   BEGL_HWISRCallback      tryAgainCallback;
   BKNI_MutexHandle        criticalSection;
} NXPL_HWData;

/*****************************************************************************
 * H/W driver interface
 *****************************************************************************/

static void DriverMainISRHandler(void *context, int param)
{
   NXPL_HWData *data = (NXPL_HWData*)context;

   if (data)
   {
      BKNI_AcquireMutex(data->criticalSection);
      if (data->mainISRCallback != NULL)
         data->mainISRCallback();
#ifndef NDEBUG
      else
         printf("V3D ISR while not acquired *************************\n");
#endif
      BKNI_ReleaseMutex(data->criticalSection);
   }
}

static void DriverTryAgainHandler(void *context, int param)
{
   NXPL_HWData *data = (NXPL_HWData*)context;

   if (data)
   {
      BKNI_AcquireMutex(data->criticalSection);
      if (data->tryAgainCallback != NULL)
         data->tryAgainCallback();
#ifndef NDEBUG
      else
         printf("TryAgain before any acquire *********************\n");
#endif
      BKNI_ReleaseMutex(data->criticalSection);
   }
}

/* Acquire 3D hardware*/
static bool DriverAcquire(void *context, const BEGL_HWAcquireSettings *settings)
{
   NEXUS_Graphics3dAcquireSettings  acquireSettings;
   NXPL_HWData                   *data = (NXPL_HWData*)context;
   NEXUS_Error                      err;

   NEXUS_Graphics3d_GetDefaultAcquireSettings(&acquireSettings);

   acquireSettings.userVertexPipeMemory = settings->actualVPM;

   data->mainISRCallback = settings->ISR;

   /* stop acquire from trashing tryAgainCallback if an irq is half way through */
   BKNI_AcquireMutex(data->criticalSection);
   data->tryAgainCallback = settings->tryAcquireAgain;
   BKNI_ReleaseMutex(data->criticalSection);

   assert(data->nexusHandle != NULL);
   if (data->nexusHandle == NULL)
      return false;

      /* Opening the Nexus module acquires it for exclusive use */
   err = NEXUS_Graphics3d_Acquire(data->nexusHandle, &acquireSettings);

   return err == NEXUS_SUCCESS;
}

/* Release 3D hardware*/
static bool DriverRelease(void *context, const BEGL_HWReleaseSettings *settings)
{
   NXPL_HWData *data = (NXPL_HWData*)context;
   NEXUS_Error  err;

   if (data && data->nexusHandle)
   {
      /* Release the h/w */
      err = NEXUS_Graphics3d_Release(data->nexusHandle);

      if (err == NEXUS_SUCCESS)
      {
         /* stop release from trashing mainISRCallback if an irq is half way through */
         BKNI_AcquireMutex(data->criticalSection);
         /* The core definitely released (wasn't just a reference count), so we can reset our ISR callback pointer */
         data->mainISRCallback = NULL;
         BKNI_ReleaseMutex(data->criticalSection);
      }

      return true;
   }

   return false;
}

/* Issue command to the 3D hardware*/
static void DriverSendCommand(void *context, BEGL_HWCommand command, uint32_t sendData)
{
   NXPL_HWData             *data = (NXPL_HWData*)context;
   NEXUS_Graphics3dCommand nexusCmd;

#ifndef NDEBUG
   if (data && data->mainISRCallback == NULL &&
       ((command < BEGL_HW_IDENT0) || (command > BEGL_HW_IDENT2)))
      printf("DriverSendCommand when not acquired ******************* %p\n", (void *)command);
#endif

   NEXUS_Graphics3d_GetDefaultCommand(&nexusCmd);
   nexusCmd.commandId = data->commandMap[command];
   nexusCmd.data = sendData;

   if (data != NULL && data->nexusHandle != NULL && command > BEGL_HW_NO_COMMAND && command < BEGL_HW_NUM_COMMANDS)
      NEXUS_Graphics3d_SendCommand(data->nexusHandle, &nexusCmd);
}

/* Read status from 3D hardware*/
static uint32_t DriverReadStatus(void *context, BEGL_HWCommand status)
{
   NXPL_HWData             *data = (NXPL_HWData*)context;
   uint32_t                val = 0;
   NEXUS_Graphics3dStatus  nexusStatus;

#ifndef NDEBUG
   if (data && data->mainISRCallback == NULL &&
       ((status < BEGL_HW_IDENT0) || (status > BEGL_HW_IDENT2)))
      printf("DriverReadStatus when not acquired ******************* %p\n", (void *)status);
#endif

   if (data != NULL && data->nexusHandle != NULL && status > BEGL_HW_NO_COMMAND && status < BEGL_HW_NUM_COMMANDS)
   {
      if (NEXUS_Graphics3d_GetStatus(data->nexusHandle, data->commandMap[status], &nexusStatus) == NEXUS_SUCCESS)
         val = nexusStatus.status;
   }
      
   return val;
}

BEGL_HWInterface *NXPL_CreateHWInterface(void)
{
   BEGL_HWInterface *hw = (BEGL_HWInterface*)malloc(sizeof(BEGL_HWInterface));
   if (hw != NULL)
   {
      NXPL_HWData *data = (NXPL_HWData*)malloc(sizeof(NXPL_HWData));
      memset(hw, 0, sizeof(BEGL_HWInterface));

      if (data != NULL)
      {
         pthread_mutexattr_t mutexattr;
         pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE_NP);

         NEXUS_Graphics3dOpenSettings  settings;

         memset(data, 0, sizeof(NXPL_HWData));

         hw->context = (void*)data;
         hw->Acquire = DriverAcquire;
         hw->Release = DriverRelease;
         hw->SendCommand = DriverSendCommand;
         hw->ReadStatus = DriverReadStatus;

         NEXUS_Graphics3d_GetDefaultOpenSettings(&settings);

         settings.controlCallback.callback = DriverMainISRHandler;
         settings.controlCallback.context = data;
         settings.controlCallback.param = 0;

         settings.tryAgainCallback.callback = DriverTryAgainHandler;
         settings.tryAgainCallback.context = data;
         settings.tryAgainCallback.param = 0;

         data->nexusHandle = NEXUS_Graphics3d_Open(&settings);

         /* initialize critical section */
         BKNI_CreateMutex(&data->criticalSection);

         /* Setup the command mappings */
         data->commandMap[BEGL_HW_NO_COMMAND]     = NEXUS_Graphics3dCommandId_eNO_COMMAND;
         data->commandMap[BEGL_HW_L2CACTL]        = NEXUS_Graphics3dCommandId_eL2CACTL;
         data->commandMap[BEGL_HW_SLCACTL]        = NEXUS_Graphics3dCommandId_eSLCACTL;
         data->commandMap[BEGL_HW_INTCTL]         = NEXUS_Graphics3dCommandId_eINTCTL;
         data->commandMap[BEGL_HW_INTENA]         = NEXUS_Graphics3dCommandId_eINTENA;
         data->commandMap[BEGL_HW_INTDIS]         = NEXUS_Graphics3dCommandId_eINTDIS;
         data->commandMap[BEGL_HW_CT0CS]          = NEXUS_Graphics3dCommandId_eCT0CS;
         data->commandMap[BEGL_HW_CT1CS]          = NEXUS_Graphics3dCommandId_eCT1CS;
         data->commandMap[BEGL_HW_CT0EA]          = NEXUS_Graphics3dCommandId_eCT0EA;
         data->commandMap[BEGL_HW_CT1EA]          = NEXUS_Graphics3dCommandId_eCT1EA;
         data->commandMap[BEGL_HW_CT0CA]          = NEXUS_Graphics3dCommandId_eCT0CA;
         data->commandMap[BEGL_HW_CT1CA]          = NEXUS_Graphics3dCommandId_eCT1CA;
         data->commandMap[BEGL_HW_PCS]            = NEXUS_Graphics3dCommandId_ePCS;
         data->commandMap[BEGL_HW_BFC]            = NEXUS_Graphics3dCommandId_eBFC;
         data->commandMap[BEGL_HW_RFC]            = NEXUS_Graphics3dCommandId_eRFC;
         data->commandMap[BEGL_HW_BPOA]           = NEXUS_Graphics3dCommandId_eBPOA;
         data->commandMap[BEGL_HW_BPOS]           = NEXUS_Graphics3dCommandId_eBPOS;
         data->commandMap[BEGL_HW_CACHE_CTRL]     = NEXUS_Graphics3dCommandId_eCACHE_CTRL;
         data->commandMap[BEGL_HW_PCTRC]          = NEXUS_Graphics3dCommandId_ePCTRC;
         data->commandMap[BEGL_HW_PCTRE]          = NEXUS_Graphics3dCommandId_ePCTRE;
         data->commandMap[BEGL_HW_PCTR0]          = NEXUS_Graphics3dCommandId_ePCTR0;
         data->commandMap[BEGL_HW_PCTRS0]         = NEXUS_Graphics3dCommandId_ePCTRS0;
         data->commandMap[BEGL_HW_PCTR1]          = NEXUS_Graphics3dCommandId_ePCTR1;
         data->commandMap[BEGL_HW_PCTRS1]         = NEXUS_Graphics3dCommandId_ePCTRS1;
         data->commandMap[BEGL_HW_PCTR2]          = NEXUS_Graphics3dCommandId_ePCTR2;
         data->commandMap[BEGL_HW_PCTRS2]         = NEXUS_Graphics3dCommandId_ePCTRS2;
         data->commandMap[BEGL_HW_PCTR3]          = NEXUS_Graphics3dCommandId_ePCTR3;
         data->commandMap[BEGL_HW_PCTRS3]         = NEXUS_Graphics3dCommandId_ePCTRS3;
         data->commandMap[BEGL_HW_PCTR4]          = NEXUS_Graphics3dCommandId_ePCTR4;
         data->commandMap[BEGL_HW_PCTRS4]         = NEXUS_Graphics3dCommandId_ePCTRS4;
         data->commandMap[BEGL_HW_PCTR5]          = NEXUS_Graphics3dCommandId_ePCTR5;
         data->commandMap[BEGL_HW_PCTRS5]         = NEXUS_Graphics3dCommandId_ePCTRS5;
         data->commandMap[BEGL_HW_PCTR6]          = NEXUS_Graphics3dCommandId_ePCTR6;
         data->commandMap[BEGL_HW_PCTRS6]         = NEXUS_Graphics3dCommandId_ePCTRS6;
         data->commandMap[BEGL_HW_PCTR7]          = NEXUS_Graphics3dCommandId_ePCTR7;
         data->commandMap[BEGL_HW_PCTRS7]         = NEXUS_Graphics3dCommandId_ePCTRS7;
         data->commandMap[BEGL_HW_PCTR8]          = NEXUS_Graphics3dCommandId_ePCTR8;
         data->commandMap[BEGL_HW_PCTRS8]         = NEXUS_Graphics3dCommandId_ePCTRS8;
         data->commandMap[BEGL_HW_PCTR9]          = NEXUS_Graphics3dCommandId_ePCTR9;
         data->commandMap[BEGL_HW_PCTRS9]         = NEXUS_Graphics3dCommandId_ePCTRS9;
         data->commandMap[BEGL_HW_PCTR10]         = NEXUS_Graphics3dCommandId_ePCTR10;
         data->commandMap[BEGL_HW_PCTRS10]        = NEXUS_Graphics3dCommandId_ePCTRS10;
         data->commandMap[BEGL_HW_PCTR11]         = NEXUS_Graphics3dCommandId_ePCTR11;
         data->commandMap[BEGL_HW_PCTRS11]        = NEXUS_Graphics3dCommandId_ePCTRS11;
         data->commandMap[BEGL_HW_PCTR12]         = NEXUS_Graphics3dCommandId_ePCTR12;
         data->commandMap[BEGL_HW_PCTRS12]        = NEXUS_Graphics3dCommandId_ePCTRS12;
         data->commandMap[BEGL_HW_PCTR13]         = NEXUS_Graphics3dCommandId_ePCTR13;
         data->commandMap[BEGL_HW_PCTRS13]        = NEXUS_Graphics3dCommandId_ePCTRS13;
         data->commandMap[BEGL_HW_PCTR14]         = NEXUS_Graphics3dCommandId_ePCTR14;
         data->commandMap[BEGL_HW_PCTRS14]        = NEXUS_Graphics3dCommandId_ePCTRS14;
         data->commandMap[BEGL_HW_PCTR15]         = NEXUS_Graphics3dCommandId_ePCTR15;
         data->commandMap[BEGL_HW_PCTRS15]        = NEXUS_Graphics3dCommandId_ePCTRS15;
         data->commandMap[BEGL_HW_GCA_PM_SEL]     = NEXUS_Graphics3dCommandId_eGCA_PM_SEL;
         data->commandMap[BEGL_HW_GCA_BW_CNT]     = NEXUS_Graphics3dCommandId_eGCA_BW_CNT;
         data->commandMap[BEGL_HW_GCA_BW_MEM_CNT] = NEXUS_Graphics3dCommandId_eGCA_BW_MEM_CNT;
         data->commandMap[BEGL_HW_IDENT0]         = NEXUS_Graphics3dCommandId_eIDENT0;
         data->commandMap[BEGL_HW_IDENT1]         = NEXUS_Graphics3dCommandId_eIDENT1;
         data->commandMap[BEGL_HW_IDENT2]         = NEXUS_Graphics3dCommandId_eIDENT2;
         data->commandMap[BEGL_HW_SQRSV0]         = NEXUS_Graphics3dCommandId_eSQRSV0;
         data->commandMap[BEGL_HW_IDENT3]             = NEXUS_Graphics3dCommandId_eIDENT3;
         data->commandMap[BEGL_HW_SQRSV1]             = NEXUS_Graphics3dCommandId_eSQRSV1;
         data->commandMap[BEGL_HW_SQCNTL]             = NEXUS_Graphics3dCommandId_eSQCNTL;
         data->commandMap[BEGL_HW_SQCSTAT]            = NEXUS_Graphics3dCommandId_eSQCSTAT;
         data->commandMap[BEGL_HW_SRQPC]              = NEXUS_Graphics3dCommandId_eSRQPC;
         data->commandMap[BEGL_HW_SRQUA]              = NEXUS_Graphics3dCommandId_eSRQUA;
         data->commandMap[BEGL_HW_SRQUL]              = NEXUS_Graphics3dCommandId_eSRQUL;
         data->commandMap[BEGL_HW_SRQCS]              = NEXUS_Graphics3dCommandId_eSRQCS;
         data->commandMap[BEGL_HW_SCRATCH]            = NEXUS_Graphics3dCommandId_eSCRATCH;
         data->commandMap[BEGL_HW_ENDSWP]             = NEXUS_Graphics3dCommandId_eENDSWP;
         data->commandMap[BEGL_HW_VPACNTL]            = NEXUS_Graphics3dCommandId_eVPACNTL;
         data->commandMap[BEGL_HW_VPMBASE]            = NEXUS_Graphics3dCommandId_eVPMBASE;
         data->commandMap[BEGL_HW_BPCA]               = NEXUS_Graphics3dCommandId_eBPCA;
         data->commandMap[BEGL_HW_BPCS]               = NEXUS_Graphics3dCommandId_eBPCS;
         data->commandMap[BEGL_HW_BXCF]               = NEXUS_Graphics3dCommandId_eBXCF;
         data->commandMap[BEGL_HW_CACHE_ID]           = NEXUS_Graphics3dCommandId_eCACHE_ID;
         data->commandMap[BEGL_HW_CACHE_SWAP_CTRL]    = NEXUS_Graphics3dCommandId_eCACHE_SWAP_CTRL;
         data->commandMap[BEGL_HW_CACHE_SWAP_CTRL_2]  = NEXUS_Graphics3dCommandId_eCACHE_SWAP_CTRL_2;
         data->commandMap[BEGL_HW_CACHE_STATUS]       = NEXUS_Graphics3dCommandId_eCACHE_STATUS;
         data->commandMap[BEGL_HW_PM_CTRL]            = NEXUS_Graphics3dCommandId_ePM_CTRL;
         data->commandMap[BEGL_HW_V3D_BW_CNT]         = NEXUS_Graphics3dCommandId_eV3D_BW_CNT;
         data->commandMap[BEGL_HW_MEM_BW_CNT]         = NEXUS_Graphics3dCommandId_eMEM_BW_CNT;
         data->commandMap[BEGL_HW_LOW_PRI_ID]         = NEXUS_Graphics3dCommandId_eLOW_PRI_ID;
         data->commandMap[BEGL_HW_MEM_PROTECT_0]      = NEXUS_Graphics3dCommandId_eMEM_PROTECT_0;
         data->commandMap[BEGL_HW_MEM_PROTECT_1]      = NEXUS_Graphics3dCommandId_eMEM_PROTECT_1;
         data->commandMap[BEGL_HW_MEM_PROTECT_2]      = NEXUS_Graphics3dCommandId_eMEM_PROTECT_2;
         data->commandMap[BEGL_HW_MEM_PROTECT_3]      = NEXUS_Graphics3dCommandId_eMEM_PROTECT_3;
         data->commandMap[BEGL_HW_MEM_PROTECT_4]      = NEXUS_Graphics3dCommandId_eMEM_PROTECT_4;
         data->commandMap[BEGL_HW_MEM_PROTECT_5]      = NEXUS_Graphics3dCommandId_eMEM_PROTECT_5;
         data->commandMap[BEGL_HW_MEM_PROTECT_6]      = NEXUS_Graphics3dCommandId_eMEM_PROTECT_6;
         data->commandMap[BEGL_HW_MEM_PROTECT_7]      = NEXUS_Graphics3dCommandId_eMEM_PROTECT_7;
         data->commandMap[BEGL_HW_MEM_PROTECT_STATUS] = NEXUS_Graphics3dCommandId_eMEM_PROTECT_STATUS;
         data->commandMap[BEGL_HW_AXI_BRIDGE_STATUS]  = NEXUS_Graphics3dCommandId_eAXI_BRIDGE_STATUS;
         data->commandMap[BEGL_HW_SCRATCH_0]          = NEXUS_Graphics3dCommandId_eSCRATCH_0;
         data->commandMap[BEGL_HW_SAFE_SHUTDOWN]      = NEXUS_Graphics3dCommandId_eSAFE_SHUTDOWN;
         data->commandMap[BEGL_HW_SAFE_SHUTDOWN_ACK]  = NEXUS_Graphics3dCommandId_eSAFE_SHUTDOWN_ACK;
         data->commandMap[BEGL_HW_DBCFG]              = NEXUS_Graphics3dCommandId_eDBCFG;
         data->commandMap[BEGL_HW_DBSCS]              = NEXUS_Graphics3dCommandId_eDBSCS;
         data->commandMap[BEGL_HW_DBSCFG]             = NEXUS_Graphics3dCommandId_eDBSCFG;
         data->commandMap[BEGL_HW_DBSSR]              = NEXUS_Graphics3dCommandId_eDBSSR;
         data->commandMap[BEGL_HW_DBSDR0]             = NEXUS_Graphics3dCommandId_eDBSDR0;
         data->commandMap[BEGL_HW_DBSDR1]             = NEXUS_Graphics3dCommandId_eDBSDR1;
         data->commandMap[BEGL_HW_DBSDR2]             = NEXUS_Graphics3dCommandId_eDBSDR2;
         data->commandMap[BEGL_HW_DBSDR3]             = NEXUS_Graphics3dCommandId_eDBSDR3;
         data->commandMap[BEGL_HW_DBQRUN]             = NEXUS_Graphics3dCommandId_eDBQRUN;
         data->commandMap[BEGL_HW_DBQHLT]             = NEXUS_Graphics3dCommandId_eDBQHLT;
         data->commandMap[BEGL_HW_DBQSTP]             = NEXUS_Graphics3dCommandId_eDBQSTP;
         data->commandMap[BEGL_HW_DBQITE]             = NEXUS_Graphics3dCommandId_eDBQITE;
         data->commandMap[BEGL_HW_DBQITC]             = NEXUS_Graphics3dCommandId_eDBQITC;
         data->commandMap[BEGL_HW_DBQGHC]             = NEXUS_Graphics3dCommandId_eDBQGHC;
         data->commandMap[BEGL_HW_DBQGHG]             = NEXUS_Graphics3dCommandId_eDBQGHG;
         data->commandMap[BEGL_HW_DBQGHH]             = NEXUS_Graphics3dCommandId_eDBQGHH;
         data->commandMap[BEGL_HW_DBGE]               = NEXUS_Graphics3dCommandId_eDBGE;
         data->commandMap[BEGL_HW_FDBGO]              = NEXUS_Graphics3dCommandId_eFDBGO;
         data->commandMap[BEGL_HW_FDBGB]              = NEXUS_Graphics3dCommandId_eFDBGB;
         data->commandMap[BEGL_HW_FDBGR]              = NEXUS_Graphics3dCommandId_eFDBGR;
         data->commandMap[BEGL_HW_FDBGS]              = NEXUS_Graphics3dCommandId_eFDBGS;
         data->commandMap[BEGL_HW_ERRSTAT]            = NEXUS_Graphics3dCommandId_eERRSTAT;
         data->commandMap[BEGL_HW_CT00RA0]            = NEXUS_Graphics3dCommandId_eCT00RA0;
         data->commandMap[BEGL_HW_CT01RA0]            = NEXUS_Graphics3dCommandId_eCT01RA0;
         data->commandMap[BEGL_HW_CT0LC]              = NEXUS_Graphics3dCommandId_eCT0LC;
         data->commandMap[BEGL_HW_CT1LC]              = NEXUS_Graphics3dCommandId_eCT1LC;
         data->commandMap[BEGL_HW_CT0PC]              = NEXUS_Graphics3dCommandId_eCT0PC;
         data->commandMap[BEGL_HW_CT1PC]              = NEXUS_Graphics3dCommandId_eCT1PC;
         data->commandMap[BEGL_HW_BRIDGE_REVISION]    = NEXUS_Graphics3dCommandId_eBRIDGE_REVISION;
         data->commandMap[BEGL_HW_BRIDGE_CTRL]        = NEXUS_Graphics3dCommandId_eBRIDGE_CTRL;
         data->commandMap[BEGL_HW_BRIDGE_SW_INIT_0]   = NEXUS_Graphics3dCommandId_eBRIDGE_SW_INIT_0;
         data->commandMap[BEGL_HW_BRIDGE_SW_INIT_1]   = NEXUS_Graphics3dCommandId_eBRIDGE_SW_INIT_1;
         data->commandMap[BEGL_HW_INTDONE]            = NEXUS_Graphics3dCommandId_eINTDONE;
      }
   }
   return hw;
}

void NXPL_DestroyHWInterface(BEGL_HWInterface *hw)
{
   if (hw != NULL)
   {
      NXPL_HWData *data = (NXPL_HWData*)hw->context;

      if (data != NULL)
      {
         BKNI_DestroyMutex(data->criticalSection);
         NEXUS_Graphics3d_Close(data->nexusHandle);
         free(hw->context);
      }

      memset(hw, 0, sizeof(BEGL_HWInterface));
      free(hw);
   }
}

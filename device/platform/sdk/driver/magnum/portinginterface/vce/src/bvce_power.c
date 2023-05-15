/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvce_power.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/1/12 2:03p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vce/src/bvce_power.c $
 * 
 * Hydra_Software_Devel/1   10/1/12 2:03p nilesh
 * SW7445-39: Split power and output code into separate files
 *
 ***************************************************************************/

/* base modules */
#include "bstd.h"           /* standard types */
#include "berr.h"           /* error code */
#include "bdbg.h"           /* debug interface */
#include "bkni.h"           /* kernel interface */

#ifdef BCHP_PWR_SUPPORT
#include "bchp_pwr.h"
#endif

#include "bvce.h"
#include "bvce_priv.h"

#include "bvce_power.h"

BDBG_MODULE(BVCE_POWER);

void
BVCE_Power_P_AcquireResource(
      BVCE_Handle hVce,
      BVCE_Power_Type eType
      )
{
   BDBG_ENTER( BVCE_Power_P_AcquireResource );

#ifdef BCHP_PWR_SUPPORT
   if ( 0 != hVce->stPlatformConfig.stPower.astResource[eType].id )
   {
      if ( 0 == hVce->stPlatformConfig.stPower.astResource[eType].uiRefCount )
      {
         BCHP_PWR_AcquireResource(
               hVce->handles.hChp,
               hVce->stPlatformConfig.stPower.astResource[eType].id
               );
      }
   }
#endif

   hVce->stPlatformConfig.stPower.astResource[eType].uiRefCount++;

   BDBG_LEAVE( BVCE_Power_P_AcquireResource );
}

void
BVCE_Power_P_ReleaseResource(
      BVCE_Handle hVce,
      BVCE_Power_Type eType
      )
{
   BDBG_ENTER( BVCE_Power_P_ReleaseResource );

   BDBG_ASSERT( 0 != hVce->stPlatformConfig.stPower.astResource[eType].uiRefCount );
   hVce->stPlatformConfig.stPower.astResource[eType].uiRefCount--;

#ifdef BCHP_PWR_SUPPORT
   if ( 0 != hVce->stPlatformConfig.stPower.astResource[eType].id )
   {
      if ( 0 == hVce->stPlatformConfig.stPower.astResource[eType].uiRefCount )
      {
         BCHP_PWR_ReleaseResource(
               hVce->handles.hChp,
               hVce->stPlatformConfig.stPower.astResource[eType].id
               );
      }
   }
#endif

   BDBG_LEAVE( BVCE_Power_P_ReleaseResource );
}

unsigned
BVCE_Power_P_QueryResource(
      BVCE_Handle hVce,
      BVCE_Power_Type eType
      )
{
   unsigned uiReferenceCount = 0;

   BDBG_ENTER( BVCE_Power_P_QueryResource );

   uiReferenceCount = hVce->stPlatformConfig.stPower.astResource[eType].uiRefCount;

   BDBG_LEAVE( BVCE_Power_P_QueryResource );

   return uiReferenceCount;
}

void
BVCE_Power_P_ReleaseAllResources(
      BVCE_Handle hVce
      )
{
   BVCE_Power_Type eType;

   BDBG_ENTER( BVCE_Power_P_ReleaseAllResources );

   for ( eType = 0; eType < BVCE_Power_Type_eMax; eType++ )
   {
#ifdef BCHP_PWR_SUPPORT
      if ( ( 0 != hVce->stPlatformConfig.stPower.astResource[eType].id )
            && ( 0 != hVce->stPlatformConfig.stPower.astResource[eType].uiRefCount ) )
      {
         BCHP_PWR_ReleaseResource(
               hVce->handles.hChp,
               hVce->stPlatformConfig.stPower.astResource[eType].id
               );
      }
#endif

      hVce->stPlatformConfig.stPower.astResource[eType].uiRefCount = 0;
   }

   BDBG_LEAVE( BVCE_Power_P_ReleaseAllResources );
}

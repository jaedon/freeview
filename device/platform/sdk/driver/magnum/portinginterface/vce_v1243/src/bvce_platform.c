/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvce_platform.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 8/10/11 4:22p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vce/7425/bvce_platform.c $
 * 
 * Hydra_Software_Devel/5   8/10/11 4:22p nilesh
 * SW7425-448: Added Dynamic Power Management
 * 
 * Hydra_Software_Devel/4   6/14/11 11:33a nilesh
 * SW7425-448: Disable power management until API finalized
 * 
 * Hydra_Software_Devel/3   6/10/11 11:33a nilesh
 * SW7425-448: Added dynamic power management
 * 
 * Hydra_Software_Devel/2   3/14/11 11:30a nilesh
 * SW7425-1: Fixed compiler warnings when DEBUG=n
 * 
 * Hydra_Software_Devel/1   11/17/10 12:41p nilesh
 * SW7425-1: Moved platform specific code
 *
 ***************************************************************************/
#include "bstd.h"           /* standard types */
#include "berr.h"           /* error code */
#include "bdbg.h"           /* debug interface */
#include "bkni.h"           /* kernel interface */
#include "bint.h"

#include "bvce_platform.h"

#ifdef BCHP_PWR_SUPPORT
#include "bchp_pwr.h"
#endif

BDBG_MODULE(BVCE_PLATFORM);

BERR_Code
BVCE_Platform_P_WriteRegisterList(
         BREG_Handle hReg,
         const BVCE_Platform_P_RegisterSetting *astRegisterList,
         uint32_t uiCount
         )
{
   uint32_t i;
   uint32_t uiRegValue;

   BDBG_ENTER( BVCE_P_WriteRegisterList );

   for ( i = 0; i < uiCount; i++ )
   {
      uiRegValue = BREG_Read32(
               hReg,
               astRegisterList[i].uiAddress
               );

      uiRegValue &= ~astRegisterList[i].uiMask;
      uiRegValue |= astRegisterList[i].uiValue & astRegisterList[i].uiMask;

      BREG_Write32(
               hReg,
               astRegisterList[i].uiAddress,
               uiRegValue
               );

#if BDBG_DEBUG_BUILD
      {
        uint32_t uiRegValueActual = BREG_Read32(
                                                hReg,
                                                astRegisterList[i].uiAddress
                                                );

        BDBG_MSG(("@0x%08x <-- 0x%08x (0x%08x) - %s",
                  astRegisterList[i].uiAddress,
                  uiRegValue,
                  uiRegValueActual,
                  astRegisterList[i].szDescription
                  ));
      }
#endif
   }

   BDBG_LEAVE( BVCE_P_WriteRegisterList );

   return BERR_TRACE( BERR_SUCCESS );
}

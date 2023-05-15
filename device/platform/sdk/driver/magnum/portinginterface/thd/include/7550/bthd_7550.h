/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bthd_7550.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 9/7/12 9:16a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/thd/include/7550/bthd_7550.h $
 * 
 * Hydra_Software_Devel/1   9/7/12 9:16a farshidf
 * SW7425-3819: Remove symbolic links and flatten out THD directory
 * structure
 * 
 * Hydra_Software_Devel/2   11/1/09 2:19p farshidf
 * SW7550-38: merge the latest
 * 
 * Hydra_Software_Devel/THD_7550_40/2   10/1/09 4:04p farshidf
 * SW7550-38: remove ifd code and add the Dynamic Aquire Param
 * 
 * Hydra_Software_Devel/THD_7550_40/1   9/21/09 4:07p jputnam
 * SW7550-40:  Fixed VELOCE_EMULATION ifdef in P_TuneAcquire
 * - Cleaned-up FI coefficient tables
 * - Reworked WriteFICoef() function
 * - Cosmetic cleanup of indentation and comments
 * 
 * Hydra_Software_Devel/1   7/31/09 4:26p farshidf
 * PR41450: add the 7550 thd code
 * 
 ***************************************************************************/

#ifndef _BTHD_7550_H__
#define _BTHD_7550_H__           

#include "bthd.h"

#ifdef __cplusplus
extern "C" {
#endif
  
/***************************************************************************
 * Summary:
 *   This function returns the default settings for 7550 THD module.
 *
 * Description:
 *   This function is responsible for returns the default setting for 
 *   7550 THD module. The returning default setting should be used when
 *   opening the device.
 *
 * Returns:
 *   TODO:
 *  
 * See Also:
 *   None.
 *   
 ***************************************************************************/
BERR_Code BTHD_7550_GetDefaultSettings(BTHD_Settings *);
  
/***************************************************************************
 * Summary:
 *   This function returns the default inbandParams for 7550 THD module.
 *
 * Description:
 *   This function is responsible for returns the default setting for 
 *   7550 THD module. The returning default setting should be used when
 *   opening the device.
 *
 * Returns:
 *   TODO:
 *   
 * See Also:
 *   None.
 *     
 ***************************************************************************/
  BERR_Code BTHD_7550_GetDefaultInbandParams(BTHD_InbandParams *);
  

#ifdef __cplusplus
}
#endif

#endif /* BTHD_7550_H__ */


/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhab_4517.h $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 7/24/12 10:02a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/hab/4517/bhab_4517.h $
 * 
 * Hydra_Software_Devel/4   7/24/12 10:02a enavarro
 * SWSATFE-140: update LEAP_CTRL_SPARE3 definition
 * 
 * Hydra_Software_Devel/3   1/11/12 4:01p ronchan
 * SWSATFE-140: add fsk rx ready host interrupt bit
 * 
 * Hydra_Software_Devel/2   10/24/11 11:35a enavarro
 * SWSATFE-140: updated irq_status0, irq_status1, spare0, and spare3 bit
 * definitions
 * 
 * Hydra_Software_Devel/1   10/18/11 10:27a enavarro
 * SWSATFE-140: initial version
 *
 ***************************************************************************/

#ifndef BHAB_4517_H
#define BHAB_4517_H

#ifdef __cplusplus
extern "C" {
#endif


#include "bhab.h"


/* interrupt bits in LEAP_HOST_IRQ_STATUS0 */
#define BHAB_4517_HIRQ0_INIT_DONE            0x00000001
#define BHAB_4517_HIRQ0_HAB_DONE             0x00000002
#define BHAB_4517_HIRQ0_HAB_ERROR            0x00000004 /* look at SPARE3[3:0] for error code */
#define BHAB_4517_HIRQ0_FW_ERROR             0x00000008 /* look at SPARE3[7:4] for error code */
#define BHAB_4517_HIRQ0_DISEQC_DONE          0x00000010
#define BHAB_4517_HIRQ0_DISEQC_OVERVOLTAGE   0x00000020
#define BHAB_4517_HIRQ0_DISEQC_UNDERVOLTAGE  0x00000040
#define BHAB_4517_HIRQ0_FSK_MSG_READY        0x00000080

/* interrupt bits in LEAP_HOST_IRQ_STATUS1 */
#define BHAB_4517_HIRQ1_CHAN0_LOCK_CHANGE    0x00000001
#define BHAB_4517_HIRQ1_CHAN0_STATUS_EVENT   0x00000002
#define BHAB_4517_HIRQ1_CHAN0_RESERVED       0x00000004
#define BHAB_4517_HIRQ1_CHAN0_PEAK_SCAN_DONE 0x00000008
#define BHAB_4517_HIRQ1_CHAN1_LOCK_CHANGE    0x00000010
#define BHAB_4517_HIRQ1_CHAN1_STATUS_EVENT   0x00000020
#define BHAB_4517_HIRQ1_CHAN1_RESERVED       0x00000040
#define BHAB_4517_HIRQ1_CHAN1_PEAK_SCAN_DONE 0x00000080
#define BHAB_4517_HIRQ1_CHAN2_LOCK_CHANGE    0x00000100
#define BHAB_4517_HIRQ1_CHAN2_STATUS_EVENT   0x00000200
#define BHAB_4517_HIRQ1_CHAN2_RESERVED       0x00000400
#define BHAB_4517_HIRQ1_CHAN2_PEAK_SCAN_DONE 0x00000800

/* LEAP_CTRL_SPARE0 bits */
#define BHAB_4517_SPARE0_LOCK0               0x00000001
#define BHAB_4517_SPARE0_LOCK1               0x00000002
#define BHAB_4517_SPARE0_LOCK2               0x00000004
#define BHAB_4517_SPARE0_INIT_DONE           0x80000000

/* LEAP_CTRL_SPARE1 bits */

/* LEAP_CTRL_SPARE2 bits */

/* LEAP_CTRL_SPARE3 bits */
#define BHAB_4517_SPARE3_HAB_ERROR_CODE_MASK     0x0000000F
#define BHAB_4517_SPARE3_HAB_ERROR_CODE_SHIFT    0
#define BHAB_4517_SPARE3_FW_ERROR_CODE_MASK      0x000000F0
#define BHAB_4517_SPARE3_FW_ERROR_CODE_SHIFT     4
#define BHAB_4517_SPARE3_LAST_HAB_CMD_RCVD_MASK  0x00FF0000
#define BHAB_4517_SPARE3_LAST_HAB_CMD_RCVD_SHIFT 16 
#define BHAB_4517_SPARE3_LAST_HAB_CMD_SVCD_MASK  0xFF000000
#define BHAB_4517_SPARE3_LAST_HAB_CMD_SVCD_SHIFT 24 


/***************************************************************************
Summary:
   This struct contains interrupt status information that is passed to the
   callback routine.
Description:
   A pointer to this struct will be passed as the first parameter (void*) 
   to the callback routine.
Returns:
   BERR_Code
See Also:
   BHAB_IntCallbackFunc
****************************************************************************/
typedef struct BHAB_4517_IrqStatus
{
   uint32_t status0;
   uint32_t status1;
   void     *pParm1;
   int      parm2;
} BHAB_4517_IrqStatus;


/***************************************************************************
Summary:
   This function returns the default settings for the 4517 HAB module.
Description:
   This function returns the default settings for the 4517 HAB module. 
Returns:
   BERR_Code
See Also:
   BHAB_Open()
****************************************************************************/
BERR_Code BHAB_4517_GetDefaultSettings(
   BHAB_Settings * pDefSetting     /* [in] Default settings */
);


#ifdef __cplusplus
}
#endif

#endif

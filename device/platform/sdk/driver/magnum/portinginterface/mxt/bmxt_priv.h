/***************************************************************************
 *     Copyright (c) 2003-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmxt_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/8 $
 * $brcm_Date: 1/7/13 4:37p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/mxt/bmxt_priv.h $
 * 
 * Hydra_Software_Devel/8   1/7/13 4:37p jtna
 * SW7425-4359: add BMXT_Get/SetPowerSaverSettings
 * 
 * Hydra_Software_Devel/7   7/23/12 2:43p jtna
 * SW3472-14: extend parser mapping functionality for platforms with 16PBs
 * 
 * Hydra_Software_Devel/6   7/19/12 4:17p jtna
 * SW3472-14: add 3472 MXT support. change MTSIF in API naming to
 * MTSIF_TX, in order to differentiate with MTSIF_RX
 * 
 * Hydra_Software_Devel/5   3/6/12 6:02p jtna
 * SW7425-2574: allow BMXT to build without any RPC references
 * 
 * Hydra_Software_Devel/4   10/27/11 4:09p jtna
 * SW7425-1483: use correct regoffset for given chip
 * 
 * Hydra_Software_Devel/3   10/27/11 2:46p jtna
 * SW7425-1483: remove redundant internal variables. some more code clean-
 * up
 * 
 * Hydra_Software_Devel/2   10/26/11 3:14p jtna
 * SW7425-1483: deprecate BMXT_NUM_* and use chip-specific numbers instead
 * 
 * Hydra_Software_Devel/1   10/21/11 6:41p jtna
 * SW7425-1483: add generic bmxt implementation
 * 
 ***************************************************************************/

#ifndef BMXT_PRIV_H__
#define BMXT_PRIV_H__

#include "breg_mem.h"
#include "bchp.h"
#include "bmxt.h"
#include "bhab.h"

/* Defines raw HAB test mesg hdr */
#define HAB_MSG_HDR(OPCODE,N,CORE_TYPE,CORE_ID) \
    { ((uint8_t)(((uint16_t)(OPCODE)) >> 2)), \
    (((uint8_t)(0x03 & (OPCODE)) << 6) | ((uint8_t)((N)>>4))), \
    ((((uint8_t)(((N)& 0x0F) << 4))) | ((uint8_t)(0x0F & (CORE_TYPE)))), \
    ((uint8_t)(CORE_ID)) }
    
#define BMXT_CORE_ID        0x0
#define BMXT_WRITE_REGISTER 0x3FB
#define BMXT_READ_REGISTER  0x3FC
#define CORE_TYPE_GLOBAL    0x0

#ifdef __cplusplus
extern "C" {
#endif
    
/***************************************************************************
Summary:
    The handle for the transport module. Users should not directly access the 
    contents.
****************************************************************************/
typedef struct BMXT_P_TransportData
{
    BCHP_Handle hChp;
    BREG_Handle hReg;
    BMXT_Settings settings;
    BHAB_Handle hHab;
    BRPC_Handle hRpc;
    BMXT_PowerSaverSettings powerSettings;

    /* number of resources */
    struct {
        unsigned mtsifTx, mtsifRx, ib, pb, tsmf;
        unsigned remapPb; /* remappable parsers */
    } num;
    
    unsigned regOffsetFe; /* register offset for DEMOD_XPT_FE */
} BMXT_P_TransportData;

uint32_t BMXT_P_GetRegOffsetFe(BMXT_Handle handle);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BMXT_PRIV_H__ */


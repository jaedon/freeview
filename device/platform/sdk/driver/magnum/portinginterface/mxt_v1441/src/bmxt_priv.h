/***************************************************************************
 *     Copyright (c) 2003-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: $
 * $brcm_Revision: $
 * $brcm_Date: $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: $
 * 
 ***************************************************************************/

#ifndef BMXT_PRIV_H__
#define BMXT_PRIV_H__

#include "breg_mem.h"
#include "bchp.h"
#include "bmxt.h"

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

#if (BMXT_3128_SUPPORT || BMXT_3383_SUPPORT || BMXT_4528_SUPPORT || BMXT_4517_SUPPORT || BMXT_3472_SUPPORT)
#define BMXT_3128_FAMILY 1
#endif
#if (BMXT_4538_SUPPORT || BMXT_3384_SUPPORT || BMXT_7366_SUPPORT || BMXT_4548_SUPPORT || BMXT_45216_SUPPORT || BMXT_7364_SUPPORT)
#define BMXT_4538_FAMILY 1
#endif

#if (BMXT_3128_SUPPORT || BMXT_4528_SUPPORT || BMXT_4517_SUPPORT || BMXT_3472_SUPPORT || BMXT_4538_SUPPORT || BMXT_4548_SUPPORT || BMXT_45216_SUPPORT)
#define BMXT_HAB_SUPPORT 1
#endif
#if (BMXT_3383_SUPPORT || BMXT_3384_SUPPORT)
#define BMXT_RPC_SUPPORT 1
#endif
#if (BMXT_7366_SUPPORT || BMXT_7364_SUPPORT)
#define BMXT_REG_SUPPORT 1
#endif
/* note, we're currently not equipped to support a (hypothetical) situation with two 4538-family MXT instances,
one with REG_SUPPORT and one without, because bmxt_4538.c must choose between including
bmxt_4538_demod_xpt_fe.h or bchp_demod_xpt_fe.h */

/* must be a runtime check (not compile time flag) so we can e.g. switch between HAB and RPC at runtime */
/* TODO: 7145A0 has no DEMOD_XPT_FE block, but 7145B0 will. reg access will likely be via BREG, but slave device reg access will likely be via RPC,
i.e. virtual device access method is device from the real device access method */
#define BMXT_IS_RPC_BASED(chip) ((chip==BMXT_Chip_e3383) || (chip==BMXT_Chip_e3384) || (chip==BMXT_Chip_e7145))
#define BMXT_IS_REG_BASED(chip) ((chip==BMXT_Chip_e7366) || (chip==BMXT_Chip_e7364))
#define BMXT_IS_HAB_BASED(chip) ( (!BMXT_IS_RPC_BASED(chip)) && (!BMXT_IS_REG_BASED(chip)) )

#define BMXT_IS_3128_FAMILY(chip) ((chip==BMXT_Chip_e3128) || (chip==BMXT_Chip_e3383) || (chip==BMXT_Chip_e4528) || (chip==BMXT_Chip_e4517) || (chip==BMXT_Chip_e3472))

#ifdef __cplusplus
extern "C" {
#endif
    
typedef struct BMXT_P_Resources
{
    unsigned mtsifTx, mtsifRx, ib, pb, pbRemap, pbTbg, tsmf;
} BMXT_P_Resources;

typedef struct BMXT_P_TransportData
{
    BCHP_Handle hChp;
    BREG_Handle hReg;
    BMXT_Settings settings;
    void* hHab;
    void* hRpc;
    BMXT_PowerSaverSettings powerSettings;

    BMXT_P_Resources num; /* number of resources */
    unsigned regOffsetFe; /* register offset for DEMOD_XPT_FE */
} BMXT_P_TransportData;

void BMXT_P_GetNumResourcesAll(BMXT_Chip chip, BMXT_ChipRev rev, BMXT_P_Resources *pNum);
uint32_t BMXT_P_GetRegOffsetFe(BMXT_Handle handle);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BMXT_PRIV_H__ */


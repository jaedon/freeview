/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmxt.c $
 * $brcm_Revision: Hydra_Software_Devel/12 $
 * $brcm_Date: 9/28/12 7:04p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/mxt/bmxt.c $
 * 
 * Hydra_Software_Devel/12   9/28/12 7:04p jtna
 * SW7425-4008: add 4538 MXT support
 * 
 * Hydra_Software_Devel/11   9/28/12 2:45p jtna
 * SW7425-1483: refactor to use chip-specific header
 * 
 * Hydra_Software_Devel/10   9/27/12 4:59p jtna
 * SW7425-1483: get rid of function pointers
 * 
 * Hydra_Software_Devel/9   7/19/12 4:17p jtna
 * SW3472-14: add 3472 MXT support. change MTSIF in API naming to
 * MTSIF_TX, in order to differentiate with MTSIF_RX
 * 
 * Hydra_Software_Devel/8   1/25/12 4:48p jtna
 * SW7425-2267: make use of register R/W RPC and enable all APIs for 3383
 * MXT
 * 
 * Hydra_Software_Devel/7   1/23/12 5:34p jtna
 * SW7425-2125: add 4517 MXT support
 * 
 * Hydra_Software_Devel/6   1/5/12 5:16p jtna
 * SW3128-84: add BMXT_ReadIntStatusRegister()
 * 
 * Hydra_Software_Devel/5   1/3/12 11:45a jtna
 * SW7425-1483: change BMXT_GetNumResources to not require BMXT handle
 * 
 * Hydra_Software_Devel/4   10/27/11 4:09p jtna
 * SW7425-1483: use correct regoffset for given chip
 * 
 * Hydra_Software_Devel/3   10/26/11 6:32p jtna
 * SW7425-1483: enforce API consistency. general clean-up.
 * 
 * Hydra_Software_Devel/2   10/26/11 3:15p jtna
 * SW7425-1483: deprecate BMXT_NUM_* and use chip-specific numbers instead
 * 
 * Hydra_Software_Devel/1   10/21/11 6:42p jtna
 * SW7425-1483: add generic bmxt implementation
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bchp.h"
#include "bkni.h"
#include "bmxt_priv.h"
#include "bmxt.h"
#include "bmxt_demod_xpt_fe.h"

BDBG_MODULE(bmxt);

/* 
           3128  3383  3383  4528  4517  3472  4538
           ABC0   A01    B0    A0    A0    A0    A0
MTSIF TX      1     1     1     2     2     2     2
MTSIF RX      0     0     0     0     0     1     1
IB           16    16    16    16    16    16    16
PB            9     9     9     9    16    16    32
TSMF          2     2     8     2     8     8    10

*/

unsigned BMXT_GetNumResources(BMXT_Chip chip, BMXT_ChipRev rev, BMXT_ResourceType resource)
{
    unsigned numMtsifTx = 1;
    unsigned numMtsifRx = 0;
    unsigned numIb = 16;
    unsigned numPb = 9;
    unsigned numTsmf = 2;
    unsigned val;
    
    if (chip==BMXT_Chip_e3128) {
        if (rev==BMXT_ChipRev_eA0) {
        }
        else if (rev==BMXT_ChipRev_eB0) {
        }
        else if (rev==BMXT_ChipRev_eC0) {
        }
        else if (rev==BMXT_ChipRev_eC1) {
        }
    }
    else if (chip==BMXT_Chip_e3383) {
        if (rev==BMXT_ChipRev_eA0) {
        }
        else if (rev==BMXT_ChipRev_eA1) {
        }
        else if (rev==BMXT_ChipRev_eB0) {
            numTsmf = 8;
        }
    }
    else if (chip==BMXT_Chip_e4528) {
        numMtsifTx = 2;
    }
    else if (chip==BMXT_Chip_e3472) {
        numMtsifTx = 2;
        numMtsifRx = 1;
        numPb = 16;
        numTsmf = 8;
    }
    else if (chip==BMXT_Chip_e4538) {
        numMtsifTx = 2;
        numMtsifRx = 1;
        numPb = 32;
        numTsmf = 10;        
    }
    else {
        BERR_TRACE(BERR_INVALID_PARAMETER);
        return 0;
    }

    switch (resource) {
        case BMXT_ResourceType_eMtsifTx: { val = numMtsifTx; break; }
        case BMXT_ResourceType_eMtsifRx: { val = numMtsifRx; break; }
        case BMXT_ResourceType_eInputBand: { val = numIb; break; }
        case BMXT_ResourceType_eParser: { val = numPb; break; }
        case BMXT_ResourceType_eTsmf: { val = numTsmf; break; }
        default: { BERR_TRACE(BERR_INVALID_PARAMETER); return 0; }
    }

    return val;
}

uint32_t BMXT_P_GetRegOffsetFe(BMXT_Handle handle)
{
    BMXT_Chip chip = handle->settings.chip;
    
    if (chip==BMXT_Chip_e3383) {
        /* the RPC call takes offset-based (i.e. 0-based) register addresses */
        return BCHP_DEMOD_XPT_FE_FE_CTRL;
    }
    else {
        return 0;
    }
}


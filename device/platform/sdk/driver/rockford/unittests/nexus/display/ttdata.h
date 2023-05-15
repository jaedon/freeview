/***************************************************************************
 *     Copyright (c) 2001-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: ttdata.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 2/5/08 10:09a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/display/ttdata.h $
 * 
 * Hydra_Software_Devel/1   2/5/08 10:09a erickson
 * PR38679: added teletext encode
 * 
 ***************************************************************************/
#include "bstd.h"

/* copied from bvbi.h */
#define BVBI_TT_MAX_LINESIZE 42
#define BVBI_TT_ETS_FRAMING_CODE 0x27
#define BVBI_TT_INVALID_FRAMING_CODE              0xFF

typedef struct BVBI_TT_Line
{
    uint8_t ucFramingCode;
    uint8_t aucData[BVBI_TT_MAX_LINESIZE];
}
BVBI_TT_Line;

/* from ttdata0.c and ttdata1.c */
extern const BVBI_TT_Line s_aLines0[];
extern const BVBI_TT_Line s_aLines1[];

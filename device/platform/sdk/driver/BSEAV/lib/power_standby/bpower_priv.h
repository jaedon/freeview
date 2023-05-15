/***************************************************************************
 *     Copyright (c) 2002-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bpower_priv.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 10/29/08 4:00p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/power_standby/bpower_priv.h $
 * 
 * 1   10/29/08 4:00p maivu
 * PR 48036: power standby library, based off power_standby app in
 * rockford. Initial support for 3548/3556. Tested and confirmed working
 * on 93549B0
 * 
 ******************************************************************/
#ifndef BPOWER_PRIV_H__
#define BPOWER_PRIV_H__

#include "bpower_standby.h"

#ifdef __cplusplus
extern "C" {
#endif

void *bpower_p_brcm_mem_init(void);
int bpower_p_brcm_standby(bpower_standby_t power_standby);
int bpower_standby_p_os_powerdown(bpower_standby_t power_standby);
int bpower_standby_p_os_powerup(bpower_standby_t power_standby);
void bpower_standby_p_init(bpower_standby_t power_standby);


#ifdef __cplusplus
}
#endif

#endif /* BPOWER_PRIVE_H__ */


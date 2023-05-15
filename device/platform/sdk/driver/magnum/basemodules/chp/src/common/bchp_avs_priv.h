/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_avs_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 9/19/12 5:17p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7425/bchp_avs_priv.h $
 * 
 * Hydra_Software_Devel/4   9/19/12 5:17p rjlewis
 * SW7425-2218: Add support for multiple types of Locks.
 * 
 * Hydra_Software_Devel/3   3/15/12 3:49p rjlewis
 * SW7425-2218: Updated to support new voltage data structure and get
 * function.
 * 
 * Hydra_Software_Devel/2   3/2/12 6:22p rjlewis
 * SW7425-2218: Added function for getting number of central oscillators.
 * 
 * Hydra_Software_Devel/1   2/17/12 6:08p rjlewis
 * SW7346-117: new priv include file for test software.
 * 
 *
 ***************************************************************************/
#ifndef BCHP_AVS_PRIV_H__
#define BCHP_AVS_PRIV_H__

#include "bstd.h"
#include "bchp.h"

/* 
** The functions in this file are used to provide dymanic testing (or checking) of the AVS module.
** These functions should only be used for testing/validation purposes.  They can be destructive
** and cause the system to crash.  Use these functions with extreme caution.
*/

typedef struct {
	bool valid; /* true if data filled in */

	uint32_t last_dac;  /* the last value written to the DAC */
	uint32_t last_temp; /* last data read from temperature register */

	uint32_t V_0p99, V_1p1_0, V_1p1_1, V_2p75, V_3p63; /* last values read from voltage registers */
	/* below are the converted (to voltages) values of above */
	uint32_t last_voltage_1p1_0, last_voltage_1p1_1, last_voltage_0p99, last_voltage_2p75, last_voltage_3p63;
} AvsTestData;

#ifdef __cplusplus
extern "C" {
#endif

/* These are the different type of "lock" requests permitted */
typedef enum {
	eAvsNoLock,
	eAvsLockRegisters, /* do not verify/restore registers during normal processing */
	eAvsPause          /* pause AVS processing until unlock request */
} AvsLockType_t;

/* The number of oscillators varies from platform to platform -- here is the number on this platform */
unsigned AvsGetNumberCentrals(void);
unsigned AvsGetNumberRemotes(void);

/* Get/Set the DAC */
uint32_t AvsGetDAC(BCHP_P_AvsHandle hHandle);
void AvsSetDAC(BCHP_P_AvsHandle hHandle, uint32_t dac_code);

/* Get status value of a specific oscillator */
uint32_t AvsReadCentralOscillator(BREG_Handle hRegister, unsigned oscillator);
uint32_t AvsReadRemoteOscillator(BREG_Handle hRegister, unsigned oscillator);

/* Get threshold values for a specific oscillator */
void AvsReadCentralOscThresholds(BREG_Handle hRegister, unsigned oscillator, uint32_t *reg_min, uint32_t *reg_max);
void AvsReadRemoteOscThresholds(BREG_Handle hRegister, unsigned oscillator, uint32_t *reg_min, uint32_t *reg_max);

/* This is used to prevent the AVS processing from verifying any of its registers */
BERR_Code AvsLock(BCHP_P_AvsHandle hHandle, AvsLockType_t type);

/* This un-does the Lock operation */
BERR_Code AvsUnlock(BCHP_P_AvsHandle hHandle);

/* This provides miscellaneous AVS data for test purposes */
void AvsGetTestData(BCHP_P_AvsHandle hHandle, AvsTestData *data);

#ifdef __cplusplus
}
#endif

#endif /* BCHP_AVS_PRIV_H__ */


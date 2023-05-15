/*
 * Misc utility routines for accessing PMU corerev specific features
 * of the SiliconBackplane-based Broadcom chips.
 *
 * Copyright (C) 2014, Broadcom Corporation. All Rights Reserved.
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * $Id: hndpmu.c 475037 2014-05-02 23:55:49Z $
 */


/*
 * Note: this file contains PLL/FLL related functions. A chip can contain multiple PLLs/FLLs.
 * However, in the context of this file the baseband ('BB') PLL/FLL is referred to.
 *
 * Throughout this code, the prefixes 'pmu0_', 'pmu1_' and 'pmu2_' are used.
 * They refer to different revisions of the PMU (which is at revision 18 @ Apr 25, 2012)
 * pmu1_ marks the transition from PLL to ADFLL (Digital Frequency Locked Loop). It supports
 * fractional frequency generation. pmu2_ does not support fractional frequency generation.
 */

#include <bcm_cfg.h>
#include <typedefs.h>
#include <bcmdefs.h>
#include <osl.h>
#include <bcmutils.h>
#include <siutils.h>
#include <bcmdevs.h>
#include <hndsoc.h>
#include <sbchipc.h>
#include <hndpmu.h>

#define	PMU_ERROR(args)

#define	PMU_MSG(args)

/* To check in verbose debugging messages not intended
 * to be on except on private builds.
 */
#define	PMU_NONE(args)

/** contains resource bit positions for a specific chip */
struct rsc_per_chip_s {
	uint8 ht_avail;
	uint8 macphy_clkavail;
	uint8 ht_start;
	uint8 otp_pu;
};

typedef struct rsc_per_chip_s rsc_per_chip_t;


/* SDIO Pad drive strength to select value mappings.
 * The last strength value in each table must be 0 (the tri-state value).
 */
typedef struct {
	uint8 strength;			/* Pad Drive Strength in mA */
	uint8 sel;			/* Chip-specific select value */
} sdiod_drive_str_t;

/* SDIO Drive Strength to sel value table for PMU Rev 1 */
static const sdiod_drive_str_t BCMINITDATA(sdiod_drive_strength_tab1)[] = {
	{4, 0x2},
	{2, 0x3},
	{1, 0x0},
	{0, 0x0} };

/* SDIO Drive Strength to sel value table for PMU Rev 2, 3 */
static const sdiod_drive_str_t BCMINITDATA(sdiod_drive_strength_tab2)[] = {
	{12, 0x7},
	{10, 0x6},
	{8, 0x5},
	{6, 0x4},
	{4, 0x2},
	{2, 0x1},
	{0, 0x0} };

/* SDIO Drive Strength to sel value table for PMU Rev 8 (1.8V) */
static const sdiod_drive_str_t BCMINITDATA(sdiod_drive_strength_tab3)[] = {
	{32, 0x7},
	{26, 0x6},
	{22, 0x5},
	{16, 0x4},
	{12, 0x3},
	{8, 0x2},
	{4, 0x1},
	{0, 0x0} };

/* SDIO Drive Strength to sel value table for PMU Rev 11 (1.8v) */
static const sdiod_drive_str_t BCMINITDATA(sdiod_drive_strength_tab4_1v8)[] = {
	{32, 0x6},
	{26, 0x7},
	{22, 0x4},
	{16, 0x5},
	{12, 0x2},
	{8, 0x3},
	{4, 0x0},
	{0, 0x1} };

/* SDIO Drive Strength to sel value table for PMU Rev 11 (1.2v) */

/* SDIO Drive Strength to sel value table for PMU Rev 11 (2.5v) */

/* SDIO Drive Strength to sel value table for PMU Rev 13 (1.8v) */
static const sdiod_drive_str_t BCMINITDATA(sdiod_drive_strength_tab5_1v8)[] = {
	{6, 0x7},
	{5, 0x6},
	{4, 0x5},
	{3, 0x4},
	{2, 0x2},
	{1, 0x1},
	{0, 0x0} };

/* SDIO Drive Strength to sel value table for PMU Rev 13 (3.3v) */

/** SDIO Drive Strength to sel value table for PMU Rev 17 (1.8v) */
static const sdiod_drive_str_t BCMINITDATA(sdiod_drive_strength_tab6_1v8)[] = {
	{3, 0x3},
	{2, 0x2},
	{1, 0x1},
	{0, 0x0} };


/**
 * SDIO Drive Strength to sel value table for 43143 PMU Rev 17, see Confluence 43143 Toplevel
 * architecture page, section 'PMU Chip Control 1 Register definition', click link to picture
 * BCM43143_sel_sdio_signals.jpg. Valid after PMU Chip Control 0 Register, bit31 (override) has
 * been written '1'.
 */
#if !defined(BCM_SDIO_VDDIO) || BCM_SDIO_VDDIO == 33

static const sdiod_drive_str_t BCMINITDATA(sdiod_drive_strength_tab7_3v3)[] = {
	/* note: for 14, 10, 6 and 2mA hw timing is not met according to rtl team */
	{16, 0x7},
	{12, 0x5},
	{8,  0x3},
	{4,  0x1} }; /* note: 43143 does not support tristate */

#else

static const sdiod_drive_str_t BCMINITDATA(sdiod_drive_strength_tab7_1v8)[] = {
	/* note: for 7, 5, 3 and 1mA hw timing is not met according to rtl team */
	{8, 0x7},
	{6, 0x5},
	{4,  0x3},
	{2,  0x1} }; /* note: 43143 does not support tristate */

#endif /* BCM_SDIO_VDDIO */

#define SDIOD_DRVSTR_KEY(chip, pmu)	(((chip) << 16) | (pmu))

/**
 * Balance between stable SDIO operation and power consumption is achieved using this function.
 * Note that each drive strength table is for a specific VDDIO of the SDIO pads, ideally this
 * function should read the VDDIO itself to select the correct table. For now it has been solved
 * with the 'BCM_SDIO_VDDIO' preprocessor constant.
 *
 * 'drivestrength': desired pad drive strength in mA. Drive strength of 0 requests tri-state (if
 *		    hardware supports this), if no hw support drive strength is not programmed.
 */
void
BCMINITFN(si_sdiod_drive_strength_init)(si_t *sih, osl_t *osh, uint32 drivestrength)
{
	sdiod_drive_str_t *str_tab = NULL;
	uint32 str_mask = 0;	/* only alter desired bits in PMU chipcontrol 1 register */
	uint32 str_shift = 0;
	uint32 str_ovr_pmuctl = PMU_CHIPCTL0; /* PMU chipcontrol register containing override bit */
	uint32 str_ovr_pmuval = 0;            /* position of bit within this register */

	if (!(sih->cccaps & CC_CAP_PMU)) {
		return;
	}

	switch (SDIOD_DRVSTR_KEY(sih->chip, sih->pmurev)) {
	case SDIOD_DRVSTR_KEY(BCM4325_CHIP_ID, 1):
		str_tab = (sdiod_drive_str_t *)&sdiod_drive_strength_tab1;
		str_mask = 0x30000000;
		str_shift = 28;
		break;
	case SDIOD_DRVSTR_KEY(BCM4325_CHIP_ID, 2):
	case SDIOD_DRVSTR_KEY(BCM4325_CHIP_ID, 3):
	case SDIOD_DRVSTR_KEY(BCM4315_CHIP_ID, 4):
		str_tab = (sdiod_drive_str_t *)&sdiod_drive_strength_tab2;
		str_mask = 0x00003800;
		str_shift = 11;
		break;
	case SDIOD_DRVSTR_KEY(BCM4336_CHIP_ID, 8):
	case SDIOD_DRVSTR_KEY(BCM4336_CHIP_ID, 11):
		if (sih->pmurev == 8) {
			str_tab = (sdiod_drive_str_t *)&sdiod_drive_strength_tab3;
		}
		else if (sih->pmurev == 11) {
			str_tab = (sdiod_drive_str_t *)&sdiod_drive_strength_tab4_1v8;
		}
		str_mask = 0x00003800;
		str_shift = 11;
		break;
	case SDIOD_DRVSTR_KEY(BCM4330_CHIP_ID, 12):
		str_tab = (sdiod_drive_str_t *)&sdiod_drive_strength_tab4_1v8;
		str_mask = 0x00003800;
		str_shift = 11;
		break;
	case SDIOD_DRVSTR_KEY(BCM43362_CHIP_ID, 13):
		str_tab = (sdiod_drive_str_t *)&sdiod_drive_strength_tab5_1v8;
		str_mask = 0x00003800;
		str_shift = 11;
		break;
	case SDIOD_DRVSTR_KEY(BCM4334_CHIP_ID, 17):
		str_tab = (sdiod_drive_str_t *)&sdiod_drive_strength_tab6_1v8;
		str_mask = 0x00001800;
		str_shift = 11;
		break;
	case SDIOD_DRVSTR_KEY(BCM43143_CHIP_ID, 17):
#if !defined(BCM_SDIO_VDDIO) || BCM_SDIO_VDDIO == 33
		if (drivestrength >=  ARRAYLAST(sdiod_drive_strength_tab7_3v3)->strength) {
			str_tab = (sdiod_drive_str_t *)&sdiod_drive_strength_tab7_3v3;
		}
#else
		if (drivestrength >=  ARRAYLAST(sdiod_drive_strength_tab7_1v8)->strength) {
			str_tab = (sdiod_drive_str_t *)&sdiod_drive_strength_tab7_1v8;
		}
#endif /* BCM_SDIO_VDDIO */
		str_mask = 0x00000007;
		str_ovr_pmuval = PMU43143_CC0_SDIO_DRSTR_OVR;
		break;
	default:
		PMU_MSG(("No SDIO Drive strength init done for chip %s rev %d pmurev %d\n",
		         bcm_chipname(sih->chip, chn, 8), sih->chiprev, sih->pmurev));
		break;
	}

	if (str_tab != NULL) {
		uint32 cc_data_temp;
		int i;

		/* Pick the lowest available drive strength equal or greater than the
		 * requested strength.	Drive strength of 0 requests tri-state.
		 */
		for (i = 0; drivestrength < str_tab[i].strength; i++)
			;

		if (i > 0 && drivestrength > str_tab[i].strength)
			i--;

		W_REG(osh, PMUREG(sih, chipcontrol_addr), PMU_CHIPCTL1);
		cc_data_temp = R_REG(osh, PMUREG(sih, chipcontrol_data));
		cc_data_temp &= ~str_mask;
		cc_data_temp |= str_tab[i].sel << str_shift;
		W_REG(osh, PMUREG(sih, chipcontrol_data), cc_data_temp);
		if (str_ovr_pmuval) { /* enables the selected drive strength */
			W_REG(osh,  PMUREG(sih, chipcontrol_addr), str_ovr_pmuctl);
			OR_REG(osh, PMUREG(sih, chipcontrol_data), str_ovr_pmuval);
		}
		PMU_MSG(("SDIO: %dmA drive strength requested; set to %dmA\n",
		         drivestrength, str_tab[i].strength));
	}
} /* si_sdiod_drive_strength_init */
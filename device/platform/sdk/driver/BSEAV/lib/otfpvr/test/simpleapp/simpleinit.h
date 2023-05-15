/***************************************************************************
 *	   Copyright (c) 2004-2009, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: simpleinit.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/3/09 1:53p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/otfpvr/onepass/test/simpleapp/simpleinit.h $
 * 
 * 1   2/3/09 1:53p vsilyaev
 * PR 50887: Merged for further integration into the baseline
 * 
 * Generic_OTF/1   8/30/07 6:34p rmaddali
 * Adding simpleapp, this is used to test OTF independently
 * 
 * Hydra_Software_Devel/7   4/1/05 1:28p dkaufman
 * PR2757: Added brap and brap_transport init and handle accessors
 * 
 * Hydra_Software_Devel/6   3/7/05 8:00a dkaufman
 * PR2757: Added API for XVD, ARC
 * 
 * Hydra_Software_Devel/5   11/5/04 2:27p dkaufman
 * PR2542: Added MakePathToTgtSvr
 * 
 * Hydra_Software_Devel/4   9/22/04 11:28a dkaufman
 * PR2310: Add HDMI init
 * 
 * Hydra_Software_Devel/3   7/27/04 11:36a dkaufman
 * PR1983: Added MVD
 * 
 * Hydra_Software_Devel/2   5/21/04 7:32a dkaufman
 * PR1519: Added AUD and XPT modules
 * 
 * Hydra_Software_Devel/1   3/19/04 10:20a dkaufman
 * PR1182: Refactor app
 * 
 ***************************************************************************/

#ifndef SIMPLEINIT_H
#define SIMPLEINIT_H

#include "bstd.h"
#include "bchp.h"
#include "bchp_common.h"

#if (BCHP_CHIP == 7400)
#include "bchp_7400.h"
#include "bint_7400.h"
#endif

#if (BCHP_CHIP == 7401)
#include "bchp_7401.h"
#include "bint_7401.h"
#endif

#include "bdbg.h"
#include "breg_mem.h"
#include "bint_plat.h"


#include "bmem.h"
#include "btmr.h"
#include "bgrc.h"
#include "bsur.h"
#include "bpxl.h"
#include "bvdc.h"

#include "bi2c.h"
#include "bspi.h"
#include "bicp.h"
#include "birb.h"
#include "bkir.h"
#include "bkpd.h"
#include "bled.h"
#include "bgio.h"
#include "bxpt.h"
#include "bxvd.h"

#include "bhdm.h"
#include "brap.h"


#include "bxpt_pcr.h"

#include "bxpt_rave.h"
#include "bxpt_pcr_offset.h"


#include "bpcrlib.h"

#define MAX_TSFS_PATH_LEN 200

/* How often the "idle" event must be inserted into the TPIT data for RASP (ms) */
#define TPIT_PACKET_TIMEOUT_MS (50)
#define TPIT_CLOCK_RATE        (108000000)                 /* 108MHz clock rate */
#define TPIT_TICKS_PER_MS      (TPIT_CLOCK_RATE / 1000)

/* Utility to make paths for opening files */
char*				MakePathToTgtSvr(char* subpath, char* path);

/* Primary initialization routine */
BERR_Code 			InitApp(void);

/* Routines for testing */
BERR_Code			CloseAndReopenGIO(void);
BERR_Code			CloseAndReopenURT(void);

#endif

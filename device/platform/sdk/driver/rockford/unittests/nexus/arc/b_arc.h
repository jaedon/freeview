/***************************************************************************
 *     Copyright (c) 2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: b_arc.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 3/27/12 9:47a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/arc/b_arc.h $
 * 
 * Hydra_Software_Devel/1   3/27/12 9:47a erickson
 * SW7358-245: arc library
 * 
 ***************************************************************************/
#ifndef B_ARC_H__
#define B_ARC_H__

#if __cplusplus
extern "C" {
#endif

/**
simple library to configure ARC's

no interrupt support

this assumes nexus/magnum is not doing automatic configuration of ARC's. run nexus with export custom_arc=y.

sample code:

    // use ARC2 on MEMC0
    b_arc_t arc = b_arc_open(0, 2);
    b_arc_set_clients(arc);
    b_arc_exclude_client(arc, B_ARC_M2MC_0); // test M2MC
    b_arc_get_settings(arc, &settings);
    settings.offset = NEXUS_AddrToOffset(app.pointer);
    settings.size = app.size;
    b_arc_set_settings(arc, &settings);
    b_arc_apply(arc);
    
    // do work
    
    b_arc_check(arc);

**/
typedef struct b_arc *b_arc_t;

b_arc_t b_arc_open(unsigned memc, unsigned arc_number);

void b_arc_close(b_arc_t arc);

typedef struct b_arc_settings
{
    unsigned offset; /* physical address */
    unsigned size;
    
    bool writeCmdAbort;
    bool writeCheck;
    bool readCmdAbort;
    bool readCheck;
    bool exclusive;
} b_arc_settings;

void b_arc_get_settings(b_arc_t arc, b_arc_settings *p_settings);
int b_arc_set_settings(b_arc_t arc, const b_arc_settings *p_settings);

/***
The following is a very partial set of SCB client id's
For a complete list, see BMRC_P_astClientTbl[] in bmrc_clienttable_priv.c.
Please expand this list of macros as needed, but keep naming consistency between chips.
*/
#if BCHP_CHIP == 7425 || BCHP_CHIP == 7435
#define B_ARC_M2MC_0          90
#define B_ARC_HIF_CPU_CACHE_0 124
#define B_ARC_HIF_CPU_CACHE_1 125
#elif BCHP_CHIP == 7405
#define B_ARC_M2MC_0          3
#define B_ARC_HIF_CPU_CACHE_0 124
#define B_ARC_HIF_CPU_CACHE_1 125
#endif

/* set to all 0's */
void b_arc_clear_clients(b_arc_t arc);
/* set a client to 1 */
int b_arc_add_client(b_arc_t arc, unsigned scb_client_id);
/* set to all 1's */
void b_arc_set_clients(b_arc_t arc);
/* set a client to 0 */
int b_arc_exclude_client(b_arc_t arc, unsigned scb_client_id);

/* apply clients and settings to ARC */
int b_arc_apply(b_arc_t arc);

/* check if ARC has violation */
int b_arc_check(b_arc_t arc);

#if __cplusplus
}
#endif

#endif

/***************************************************************************
 *     Copyright (c) 2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: b_arc.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 3/27/12 9:47a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/arc/b_arc.c $
 * 
 * Hydra_Software_Devel/1   3/27/12 9:47a erickson
 * SW7358-245: arc library
 * 
 ***************************************************************************/
#include "b_arc.h"
#include "nexus_platform.h"

#include "bchp_memc_gen_0.h"
#include "bchp_memc_gen_1.h"

#define B_MEMC_ARC_OFFSET_CNTRL                 0
#define B_MEMC_ARC_OFFSET_ADRS_RANGE_LOW        4
#define B_MEMC_ARC_OFFSET_ADRS_RANGE_HIGH       8
#define B_MEMC_ARC_OFFSET_READ_RIGHTS_0         12
#define B_MEMC_ARC_OFFSET_READ_RIGHTS_1         16
#define B_MEMC_ARC_OFFSET_READ_RIGHTS_2         20
#define B_MEMC_ARC_OFFSET_READ_RIGHTS_3         24
#define B_MEMC_ARC_OFFSET_WRITE_RIGHTS_0        28
#define B_MEMC_ARC_OFFSET_WRITE_RIGHTS_1        32
#define B_MEMC_ARC_OFFSET_WRITE_RIGHTS_2        36
#define B_MEMC_ARC_OFFSET_WRITE_RIGHTS_3        40
#define B_MEMC_ARC_OFFSET_VIOLATION_INFO_START_ADDR     44
#define B_MEMC_ARC_OFFSET_VIOLATION_INFO_END_ADDR       48
#define B_MEMC_ARC_OFFSET_VIOLATION_INFO_CMD            52
#define B_MEMC_ARC_OFFSET_VIOLATION_INFO_CLEAR          56

/**
impl
**/
struct b_arc {
    unsigned memc;
    unsigned arc;
    unsigned regoffset;
    b_arc_settings settings;
#define MAX_CLIENTS (4*32)
    uint32_t clients[MAX_CLIENTS/32];
};

b_arc_t b_arc_open(unsigned memc, unsigned arc_no)
{
    b_arc_t arc;
    arc = BKNI_Malloc(sizeof(*arc));
    if (!arc) return NULL;
    BKNI_Memset(arc, 0, sizeof(*arc));
    arc->memc = memc;
    arc->arc = arc_no;
    arc->regoffset = BCHP_MEMC_GEN_0_ARC_0_CNTRL +
        (BCHP_MEMC_GEN_1_ARC_0_CNTRL - BCHP_MEMC_GEN_0_ARC_0_CNTRL)*memc +
        (BCHP_MEMC_GEN_0_ARC_1_CNTRL - BCHP_MEMC_GEN_0_ARC_0_CNTRL)*arc_no;
    arc->settings.writeCmdAbort = true;
    arc->settings.writeCheck = true;
    arc->settings.readCmdAbort = true;
    arc->settings.readCheck = true;
    arc->settings.exclusive = false;
    return arc;
}

void b_arc_close(b_arc_t arc)
{
    BKNI_Free(arc);
}

void b_arc_get_settings(b_arc_t arc, b_arc_settings *p_settings)
{
    *p_settings = arc->settings;
}

int b_arc_set_settings(b_arc_t arc, const b_arc_settings *p_settings)
{
    arc->settings = *p_settings;
    return 0;
}

int b_arc_apply(b_arc_t arc)
{
    /* disable, clear, set, enable */
    NEXUS_Platform_WriteRegister(arc->regoffset+B_MEMC_ARC_OFFSET_CNTRL, 0);
    NEXUS_Platform_WriteRegister(arc->regoffset+B_MEMC_ARC_OFFSET_VIOLATION_INFO_CLEAR, 1);
    NEXUS_Platform_WriteRegister(arc->regoffset+B_MEMC_ARC_OFFSET_VIOLATION_INFO_CLEAR, 0);
    NEXUS_Platform_WriteRegister(arc->regoffset+B_MEMC_ARC_OFFSET_ADRS_RANGE_LOW, arc->settings.offset >> 3);
    NEXUS_Platform_WriteRegister(arc->regoffset+B_MEMC_ARC_OFFSET_ADRS_RANGE_HIGH, (arc->settings.offset+arc->settings.size-1) >> 3);
    NEXUS_Platform_WriteRegister(arc->regoffset+B_MEMC_ARC_OFFSET_READ_RIGHTS_0, arc->clients[0]);
    NEXUS_Platform_WriteRegister(arc->regoffset+B_MEMC_ARC_OFFSET_READ_RIGHTS_1, arc->clients[1]);
    NEXUS_Platform_WriteRegister(arc->regoffset+B_MEMC_ARC_OFFSET_READ_RIGHTS_2, arc->clients[2]);
    NEXUS_Platform_WriteRegister(arc->regoffset+B_MEMC_ARC_OFFSET_READ_RIGHTS_3, arc->clients[3]);
    NEXUS_Platform_WriteRegister(arc->regoffset+B_MEMC_ARC_OFFSET_WRITE_RIGHTS_0, arc->clients[0]);
    NEXUS_Platform_WriteRegister(arc->regoffset+B_MEMC_ARC_OFFSET_WRITE_RIGHTS_1, arc->clients[1]);
    NEXUS_Platform_WriteRegister(arc->regoffset+B_MEMC_ARC_OFFSET_WRITE_RIGHTS_2, arc->clients[2]);
    NEXUS_Platform_WriteRegister(arc->regoffset+B_MEMC_ARC_OFFSET_WRITE_RIGHTS_3, arc->clients[3]);
    NEXUS_Platform_WriteRegister(arc->regoffset+B_MEMC_ARC_OFFSET_CNTRL,
        (arc->settings.writeCmdAbort ?1<<4:0) |
        (arc->settings.writeCheck    ?1<<3:0) |
        (arc->settings.readCmdAbort  ?1<<2:0) |
        (arc->settings.readCheck     ?1<<1:0) |
        (arc->settings.exclusive     ?1<<0:0)
        );
    return 0;
}

void b_arc_clear_clients(b_arc_t arc)
{
    BKNI_Memset(arc->clients, 0, sizeof(arc->clients));
}

int b_arc_add_client(b_arc_t arc, unsigned client)
{
    arc->clients[client/32] |= 1<<(client%32);
    return 0;
}

void b_arc_set_clients(b_arc_t arc)
{
    BKNI_Memset(arc->clients, 0xFF, sizeof(arc->clients));
}

int b_arc_exclude_client(b_arc_t arc, unsigned client)
{
    arc->clients[client/32] &= ~(1<<(client%32));
    return 0;
}

int b_arc_check(b_arc_t arc)
{
    uint32_t v0, v1, v2;
    NEXUS_Platform_ReadRegister(arc->regoffset+B_MEMC_ARC_OFFSET_VIOLATION_INFO_START_ADDR, &v0);
    NEXUS_Platform_ReadRegister(arc->regoffset+B_MEMC_ARC_OFFSET_VIOLATION_INFO_END_ADDR, &v1);
    NEXUS_Platform_ReadRegister(arc->regoffset+B_MEMC_ARC_OFFSET_VIOLATION_INFO_CMD, &v2);
    /* TODO: translate REQ_TYPE */
    if (v2) {
        fprintf(stderr, "ARC %d violation: %#x...%#x, client %d, NMB %d, REQ_TYPE %d\n", arc->arc, v0<<3, v1<<3,
            (v2>>24)&0x7F,
            (v2>>12)&0x3FF,
            v2&0x1FF);
        return 1;
    }
    else {
        return 0;
    }
}

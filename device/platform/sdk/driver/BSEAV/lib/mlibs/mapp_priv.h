/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mapp_priv.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 9/24/12 10:12a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mapp_priv.h $
 * 
 * 4   9/24/12 10:12a erickson
 * SW7435-368: remove long-held mutex
 * 
 * 3   4/8/09 5:08p vsilyaev
 * PR 54015: Don't include bkni_multi.h into the bdbg.h. All thread-aware
 * modules should include explicitly bkni_multi.h
 * 
 * 2   5/15/08 12:23p jgarrett
 * PR 42114: Reworking idle lock to use KNI and be more efficient
 * 
 * 1   2/7/05 11:08p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/2   2/3/04 10:04a erickson
 * PR9497: only applies to linux
 * 
 * Irvine_BSEAVSW_Devel/1   1/29/04 2:27p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 *
 ****************************************************************/
#ifndef MAPP_PRIV_H__
#define MAPP_PRIV_H__

#include "mapplication.h"
#include "bwin.h"
#include "bkni_multi.h"

struct MApplication::Private {
    struct {
        BKNI_MutexHandle mutex;
#define MAPP_MAX_IDLE_REQUEST 16
        bool request[MAPP_MAX_IDLE_REQUEST];
        unsigned wptr, rptr;
        unsigned current;
    } idle;
    
    bwin_engine_t win_engine;
    bwin_framebuffer_t fb;
};

#endif /* MAPP_PRIV_H__ */

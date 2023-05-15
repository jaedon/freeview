/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: test1.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 3/10/03 10:05a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/debuginterface/test/test1.c $
 * 
 * Hydra_Software_Devel/1   3/10/03 10:05a vsilyaev
 * Test case for the debug interface.
 * 
 ***************************************************************************/

#include "bstd.h"

BDBG_MODULE(test_module);



void
print_messages_1(void)
{
    
    BDBG_MSG(("Debug messages - test1.c - Level MSG"));
    BDBG_WRN(("Debug messages - test1.c - Level WRN"));
    BDBG_ERR(("Debug messages - test1.c - Level ERR"));
    return;
}

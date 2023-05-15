/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: apetest_common.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 7/10/12 9:25a $
 *
 * Module Description: include file for all APE audio testing common includes
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/ape/97422/common/apetest_common.h $
 * 
 * Hydra_Software_Devel/1   7/10/12 9:25a jgarrett
 * SW7425-3457: Adding ape_decode application
 * 
 * Hydra_Software_Devel/1   4/11/12 7:25p jgarrett
 * SW7425-2075: Adding HDMITX support and HBR test tone app
 *
 ***************************************************************************/
#ifndef APETEST_COMMON_H_
#define APETEST_COMMON_H_

#include <stdio.h>          /* for printf */

#define RET_ERR(x)  do{BERR_Code retCode=x; if (retCode) {printf("!!!Error %s failed retCode=0x%x\n", #x, retCode); return BERR_TRACE(retCode);}}while(0)
#define MSG_ERR(x)  do{BERR_Code retCode=x; if (retCode) {printf("!!!Error %s failed retCode=0x%x, continuing\n", #x, retCode);}}while(0)
#define RET_NULL(x) do{if (x==NULL) {printf("!!!Error: %s ==NULL\n", #x); return BERR_TRACE(BERR_UNKNOWN);}}while(0)

#include "apetest_utils.h"
#include "apetest_board.h"
#include "apetest_cmdline.h" 
#include "apetest_hdmitx.h"
#include "apetest_pblib.h"
#include "apetest_capture.h"

#endif /* #ifndef APETEST_COMMON_H_ */


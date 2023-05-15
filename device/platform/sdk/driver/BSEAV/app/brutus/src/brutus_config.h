/***************************************************************************
 *     Copyright (c) 2002-2005, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: brutus_config.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 6/14/05 2:14p $
 *
 * Module Description: Set config file defaults and force
 * certain config settings after the user configuration has been read.
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/brutus_config.h $
 * 
 * 2   6/14/05 2:14p erickson
 * PR15216: converted to BKNI_Snprintf
 * 
 * 1   2/7/05 7:56p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/1   2/9/04 11:57a erickson
 * PR8850: separated config code from main.cpp
 *
 ************************************************************************/
#ifndef BRUTUS_CONFIG_H__
#define BRUTUS_CONFIG_H__
#include "cfgsettings.h"
#include "bstd.h"
#include "bkni.h"


/**
* Summary:
* Set Config defaults before reading the config file or applying cmd line parameters.
*
* Description:
* After this is called, you can change whatever settings you like.
* Then call configConfig() to make sure nothing's inconsistant.
**/
void setDefaults(Config *cfg);

/**
* Summary:
* Enforce that certain settings are consistant.
*
* Description:
* After processing cmdline parameters and reading the config
* file, this makes sure everything is coherent and it forces
* some settings depending on the platform.
*
* This is needed so that the rest of Brutus doesn't have to double-check everything.
*
* If something is critically wrong, the function returns non-zero and the
* app should fail.
**/
int conformConfig(Config *cfg);

#endif // BRUTUS_CONFIG_H__

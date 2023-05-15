/***************************************************************************
 *     (c)2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: argenv.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/6/12 6:06p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/argenv.h $
 * 
 * 2   9/6/12 6:06p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
/* $Id: argenv.h,v 1.3 2005/01/22 21:15:51 lhamilton Exp $ */
/* ARGENV.H - Copyright (C) 2001 Sand Video, Inc., All Rights Reserved
To be included when linking with argutil.c.
  
Revision history:
  mm/dd/yyyy Who        Description
  01/26/2001 John Iler  initial release
*/

#if !defined(ARGENV_INCLUDED)     /* prevents multiple inclusion errors */

/* File includes *************************************************************/

/* Defines *******************************************************************/
/* Checks for specified command line argument.
  Args: szItem - argument name.  If switch, precede with switch introducer (e.g.
    "/enable").  If nonswitch specify which by number, starting with 1 (e.g. "1").
  Returns: nonzero if argument exists, else 0
  Effects: requires previous call to ArgEnvInit()
*/
#define ArgExist(szItem) (ArgValue(szItem) != NULL)

/* Checks for specified environment variable.
  Args: szVarName - environment variable name (case insensitive)
  Returns: nonzero if variable exists, else 0
  Effects: requires previous call to ArgEnvInit()
*/
#define EnvExist(szVarName) (EnvValue(szVarName) != NULL)

#ifdef __cplusplus
extern "C"
{
#endif

/* Structure definitions *****************************************************/

/* Function declarations *****************************************************/
extern int ArgEnvInit(int nArgc, char *cpArgv[], char *cpEnvp[]);
extern char *ArgValid(const char *szAllowed);
extern char *ArgValue(const char *szId);
extern char *EnvValue(const char *szName);
int ArgEnvSetup(char *buf);
void ArgFreeMemory();

/* Global/static variables ***************************************************/

/* Routines ******************************************************************/

#ifdef __cplusplus
}
#endif

#define ARGENV_INCLUDED        /* prevents multiple inclusion errors */
#endif

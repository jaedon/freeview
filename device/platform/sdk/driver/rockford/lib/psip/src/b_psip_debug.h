/***************************************************************************
*     (c)2004-2008 Broadcom Corporation
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
* $brcm_Workfile: b_psip_debug.h $
* $brcm_Revision: Hydra_Software_Devel/1 $
* $brcm_Date: 9/16/08 7:39p $
*
* Description:
*   API name: PSIP
*    Debug library routines for PSIP application library.
*
* Revision History:
*
* $brcm_Log: /rockford/lib/psip/src/b_psip_debug.h $
* 
* Hydra_Software_Devel/1   9/16/08 7:39p cttok
* PR46977: imported from nexus
* 
* 1   6/13/08 6:12p tokushig
* PR42421: split b_psip_lib.c into multiple files, updated comments and
*  headers, removed some dead code
* 
***************************************************************************/
#ifndef B_PSIP_DEBUG_H__
#define B_PSIP_DEBUG_H__

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
    Print the Program Allocation Table (PAT)
***************************************************************************/
void printPAT(
	B_PSIPi * pSi, 
	uint8_t * buffer, 
	size_t    size
	);

/***************************************************************************
Summary:
    Print the Program Map Table (PMT)
***************************************************************************/
void printPMT(
	B_PSIPi * pSi, 
	uint8_t * buffer, 
	size_t    size
	);

/***************************************************************************
Summary:
    Print the Master Guide Table (MGT)
***************************************************************************/
void printMGT(
	B_PSIPi * pSi, 
	uint8_t * buffer, 
	size_t    size
	);

/***************************************************************************
Summary:
    Print the Virtual Channel Table (VCT)
***************************************************************************/
void printVCT(
	B_PSIPi * pSi, 
	uint8_t * buffer, 
	size_t    size
	);

/***************************************************************************
Summary:
    Print the Region Rating Table (RRT)
***************************************************************************/
void printRRT(
	B_PSIPi * pSi, 
	uint8_t * buffer, 
	size_t    size
	);

/***************************************************************************
Summary:
    Print the Event Information Table (EIT)
***************************************************************************/
void printEIT(
	B_PSIPi * pSi, 
	uint8_t * buffer, 
	size_t    size
	);

/***************************************************************************
Summary:
    Print the System Time Table (STT)
***************************************************************************/
void printSTT(
	uint8_t * buffer, 
	size_t    size
	);

/***************************************************************************
Summary:
    Print the PSIP header
***************************************************************************/
void printHeader(
	TS_PSI_header headerpsi
	);

/***************************************************************************
Summary:
    Print the given PSIP table
***************************************************************************/
void printTable(
	B_PSIP  * pSi, 
	uint8_t   table_id, 
	uint8_t * buffer, 
	size_t    size
	);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef B_PSIP_DEBUG_H__ */


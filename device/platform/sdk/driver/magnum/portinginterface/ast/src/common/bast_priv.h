/***************************************************************************
 *     Copyright (c) 2005-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 6/5/06 5:37p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/bast_priv.h $
 * 
 * Hydra_Software_Devel/2   6/5/06 5:37p enavarro
 * PR 18135: fixed compiler warnings
 * 
 * Hydra_Software_Devel/1   11/14/05 10:27a enavarro
 * PR 18135: initial version
 *
 ***************************************************************************/

#ifndef BAST_PRIV_H__
#define BAST_PRIV_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "bast.h"


struct BAST_P_ChannelHandle;


/******************************************************************************
Summary:
   This is the structure for the BAST_Handle. 
******************************************************************************/
typedef struct BAST_P_Handle
{
   BAST_Settings        settings;   /* user settings */
   uint8_t              totalChannels;
   struct BAST_P_ChannelHandle **pChannels;
   void                 *pImpl;     /* pointer to chip-specific structure */
} BAST_P_Handle;


/******************************************************************************
Summary:
   This is the structure for the BAST_ChannelHandle. 
******************************************************************************/
typedef struct BAST_P_ChannelHandle
{
   BAST_ChannelSettings settings;
   BAST_P_Handle        *pDevice;
   void                 *pImpl;   /* pointer to chip-specific structure */
   uint8_t              channel;
} BAST_P_ChannelHandle;


#ifdef __cplusplus
}
#endif

#endif /* BAST_PRIV_H__ */


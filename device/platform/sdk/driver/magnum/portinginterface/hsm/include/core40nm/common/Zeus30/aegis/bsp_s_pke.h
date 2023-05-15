/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsp_s_pke.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 11/19/12 11:59a $
 * Security Processor Code for Register Access
 ***************************************************************************/

#ifndef BSP_S_PKE_H__
#define BSP_S_PKE_H__


typedef enum BCMD_PKE_CmdPollInCmdField_e
{
    BCMD_PollingCommand_CmdInputField_ePollingTarget = (5<<2) + 3 /* 8 bits indicating user vs. secure rsa */
} BCMD_PKE_CmdPollInCmdField_e;


typedef enum BCMD_PollingCommand_PollingTarget_e
    {
        BCMD_PollingCommand_PollingTarget_eSecureRSA,
        BCMD_PollingCommand_PollingTarget_eUserRSA,
        BCMD_PollingCommand_PollingTarget_eMax
    } BCMD_PollingCommand_PollingTarget_e;

#endif /* BSP_S_PKE_H__ */


/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsp_s_pkl.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 11/19/12 11:59a $
 * Security Processor Code for Register Access
 ***************************************************************************/

#ifndef BSP_S_PKL_H__
#define BSP_S_PKL_H__


/* BCMD_PklCmds input command buffer definition */
/*
|-------------------|-------------------|-------------------|---------------------|
|  31..24           | 23..16            | 15..8             | 7..0                |
|-------------------|-------------------|-------------------|---------------------|
|  unused           | unused            | unused            | eSubCmd         - 8 |
|-------------------|-------------------|-------------------|---------------------|
*/

typedef enum BCMD_PKL_InCmd_e
{
	BCMD_PKL_InCmd_SubCmd	= (5 << 2) + 3 	/* 8 bit enum */	 	
} BCMD_PKL_InCmd_e;

typedef enum BCMD_PKL_SubCmd_e
{
	BCMD_PKL_SubCmd_ePklInit	= 0,
	BCMD_PKL_SubCmd_ePklDryAck  = 1,
	BCMD_PKL_SubCmd_eMax
} BCMD_PKL_SubCmd_e;



/* Output buffer table for all the following commands*/
/* common output header bytes(first 5 words - see bsp_s_commands.h)
|--------------|------------|------------|------------------------------------| 
|  31..24      | 23..16     | 15..8      | 7..0                               | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_CommonBufferFields_eVerNum    | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_CommonBufferFields_eOwnerId   | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_CommonBufferFields_eContMode  | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | BCMD_CommonBufferFields_eParamLen               | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_CommonBufferFields_eStatus    | 
|--------------|------------|------------|------------------------------------| 
*/
/* There is no additional fields in the output command */


#endif /* BSP_S_PKL_H__ */


/***************************************************************************
*     Copyright (c) 2003-2006, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: memmgr.h $
* $brcm_Revision: Hydra_Software_Devel/1 $
* $brcm_Date: 3/10/06 12:47p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /rockford/appframework/src/vxworks/memmgr.h $
* 
* Hydra_Software_Devel/1   3/10/06 12:47p maivu
* PR 18367: Initial Vxworks support
* 
***************************************************************************/
 
#ifndef _MEMMGR_H
#define _MEMMGR_H


#if __cplusplus
extern "C" {
#endif

int  memmgr_Init(BSystem_Info *pInfo);
int  memmgr_Done(void);

#if __cplusplus
}
#endif

#endif /* _MEMMGR_H */








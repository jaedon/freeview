/***************************************************************************
 *    Copyright (c) 2004-2008, Broadcom Corporation
 *    All Rights Reserved
 *    Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_userdata_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/15 $
 * $brcm_Date: 4/7/08 7:08p $
 *
 * Module Description:
 *	 This header contains private data and functions for user data. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/7401/bxvd_userdata_priv.h $
 * 
 * Hydra_Software_Devel/15   4/7/08 7:08p pblanco
 * PR41311: Merged parameter settability changes from single decode branch
 * 
 * Hydra_Software_Devel/14   4/4/08 4:32p pblanco
 * PR41311: Backed out queue item size change. Queue depth can still be
 * adjusted
 * 
 * Hydra_Software_Devel/13   4/3/08 4:33p pblanco
 * PR41311: Made queue depth and item size runtime settable parameters
 * 
 * Hydra_Software_Devel/12   9/14/07 3:44p pblanco
 * PR34863: Decreased userdata queue depth from 64 to 32 to allow userdata
 * item sizes of 2048 to coexist with the kmalloc limit of < 128K.
 * 
 * Hydra_Software_Devel/11   9/10/07 1:04p pblanco
 * PR34636: Increase size of user data item from 512 to 2048 for all
 * platforms
 * 
 * Hydra_Software_Devel/10   2/21/07 11:40a pblanco
 * PR26433: Set formatting to standard agreed upon within the XVD group on
 * 2/20/07.
 * 
 * Hydra_Software_Devel/9   2/15/07 3:51p pblanco
 * PR27683: Added PTS and PTS valid flag to userdata.
 * 
 * Hydra_Software_Devel/8   12/12/06 3:02p davidp
 * PR25443: Merge Simplified FW API branch into mainline
 * 
 * Hydra_Software_Devel/xvd_7401_PR25443_Simplified_FW_API/2   11/15/06 4:13p davidp
 * PR25443: Merge mainline changes into branch, clean up code.
 * 
 * Hydra_Software_Devel/xvd_7401_PR25443_Simplified_FW_API/1   11/9/06 2:05p davidp
 * PR25443: Add userdata data area to queue element.
 * 
 * Hydra_Software_Devel/7   5/26/06 1:55p davidp
 * PR21740: Store userdata context in channel context, convert userdata
 * addr using private memory heap info.
 * 
 * Hydra_Software_Devel/6   12/1/05 1:44p pblanco
 * PR16052: Changed function prototype for
 * BXVD_P_Userdata_EnqueueDataPointer.
 * 
 * Hydra_Software_Devel/5   11/18/05 8:26a pblanco
 * PR16052: Changed enqueue function prototype to accept field and
 * pulldown args.
 * 
 * Hydra_Software_Devel/3   11/11/05 9:49a pblanco
 * PR16052: Added channel handle argument to
 * BXVD_P_Userdata_EnqeueDataPointer prototype.
 * 
 * Hydra_Software_Devel/2   11/11/05 7:27a pblanco
 * PR16052: Added function prototype for
 * BXVD_P_Userdata_EnqueueDataPointer.
 * 
 * ***************************************************************************/
#ifndef BXVD_USERDATA_PRIV_H__
#define BXVD_USERDATA_PRIV_H__

#include "bstd.h"				 /* standard types */
#include "bavc.h"				 /* for userdata */
#include "bdbg.h"				 /* Dbglib */
#include "bxvd_vdec_info.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Circular queue */
#define BXVD_P_USERDATA_QUEUE_START 0
#define BXVD_P_USERDATA_QUEUE_MAX   32

#define BXVD_P_USERDATA_ITEM_SIZE   2048

typedef struct _QUEUE_MGR_
{
      int ulReadPtr;
      int ulWritePtr;
      int ulNextPtr;
      int ulQueueDepth;
      struct data
      {
            int           protocol;
            unsigned long ulFlags;
            unsigned long ulPulldown;
            unsigned long ulPTS;
            unsigned char *uUserData;
      } *queue_data;
} QUEUE_MGR;


/* The internal structure for the User Data handle */
typedef struct BXVD_P_UserDataContext
{
      BXVD_ChannelHandle     hXvdCh;
      BINT_CallbackHandle    hCbUserdata;
      BXVD_Userdata_Settings sUserdataSettings;
      void                   *pBfr; /* user data buffer */
      bool                   bCallbackEnabled;
      /* Userdata callback registered */
      BINT_CallbackFunc	     fUserdataCallback_isr;
      void		     *pParm1;
      int 		     parm2;
      QUEUE_MGR              queue;
} BXVD_P_UserDataContext ;


/*
 * Called by DM when valid user data is available in the PPB.
 */
BERR_Code BXVD_P_Userdata_EnqueueDataPointer(BXVD_ChannelHandle hXvdCh, 
                                             int           protocol,
                                             unsigned long p_UserData,
                                             unsigned long ulFlags,
                                             unsigned long ulPulldown,
                                             unsigned long ulPTS);

#ifdef __cplusplus
}
#endif

#endif /* BXVD_USERDATA_PRIV_H__ */
/* End of file. */
 



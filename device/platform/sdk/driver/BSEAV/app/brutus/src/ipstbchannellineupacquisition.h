/***************************************************************************
  * Copyright (c) 2004-2006, Broadcom Corporation
  * All Rights Reserved
  * Confidential Property of Broadcom Corporation
  *
  * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
  * AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
  * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
  *
  * Module Description: IP Channel Lineup Acquisition Module Header file
  *
  * brcm_Workfile: $
  * $brcm_Revision: 3 $
  * $brcm_Date: 11/6/06 10:53a $
  *
  * $brcm_Log: /BSEAV/app/brutus/src/ipstbchannellineupacquisition.h $
  * 
  * 3   11/6/06 10:53a ssood
  * PR25072: Unable to play Video error while recording a SAP channel
  * 
  * 2   10/2/06 8:08a ssood
  * PR24576: Added Keywords to enable revision history
  *
  *************************************************************************/
extern int ipChannelLineupAcquisitionStart(ChannelManager *chMgr, AVManager *avMgr, int sapTimeout);
extern void ipChannelLineupAcquisitionStop(void);

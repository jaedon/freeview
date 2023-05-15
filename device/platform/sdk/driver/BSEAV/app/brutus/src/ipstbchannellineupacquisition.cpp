/***************************************************************************
  * Copyright (c) 2004-2009, Broadcom Corporation
  * All Rights Reserved
  * Confidential Property of Broadcom Corporation
  *
  * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
  * AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
  * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
  *
  * Module Description: IP Channel Lineup Acquisition Module
  *
  * brcm_Workfile: $
  * $brcm_Revision: 12 $
  * $brcm_Date: 6/26/09 3:51p $
  *
  * $brcm_Log: /BSEAV/app/brutus/src/ipstbchannellineupacquisition.cpp $
  * 
  * 12   6/26/09 3:51p erickson
  * PR56373: remove unused code
  *
  * 11   1/29/07 11:17a ssood
  * PR24918: Channel Map Redesign to make its access Thread-safe
  *
  * 10   11/6/06 10:53a ssood
  * PR25072: Unable to play Video error while recording a SAP channel
  *
  * 9   10/20/06 5:36p ssood
  * PR25033: Brutus crash due to incomplete channel map synchronization
  * functions
  *
  * 8   10/13/06 12:43p ssood
  * PR24576: Added a runtime flag to disable SAP processing
  *
  * 7   10/11/06 4:43p ssood
  * PR24576: Brutus Core Dump: flag was not being set to force AUTO PID
  * detection
  *
  * 6   10/11/06 1:17p ssood
  * PR24576: Changed protocol from no_rtcp to full RTP for IP Channels
  * added via SAP
  *
  * 5   10/6/06 1:07p ssood
  * PR24576: changed printf to BDBG_MSG modules & also added a new module
  * name
  *
  * 4   10/2/06 8:08a ssood
  * PR24576: Added Keywords to enable revision history
  *
  *************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "control.h"
#include "brutus_app.h"
#include "mapplication.h"
#include "channelmap.h"
#include "channelmgr.h"
#include "bstd.h"

#include "blive_ext.h"

static AVManager *avManager;

BDBG_MODULE(brutus_ip_channels);

//
// Brutus file to setup IP Channel Lineup Aquisition
//

//
// Callback function implementation to delete a Channel Map Entry
// Called by the IP Channel Map Library
//
void delChannelEntry(void *appCtx, int sourceChNum)
{
  ChannelManager *chMgr;
  AVStream *avStream;

  chMgr = (ChannelManager *)appCtx;
  BDBG_ASSERT(chMgr != NULL);

  // check if channel being deleted has an AVStream associated with it
  // (either this channel is being recorded and/or being live decoded).
  // if so, defer the channel deletion until chennel is tuned out.
  // Set a flag to indicate deferred channel deletion to the AV untune logic.
  if ((avStream = avManager->find(bchannel_entry_SAP, sourceChNum)) != NULL)
  {
    avStream->setChDeferDelete();
    return;
  }

  // now delete the channel
  if ( chMgr->map()->deleteEntry(bchannel_entry_SAP, sourceChNum) == false )
  {
    BDBG_ERR(("ERROR: delChannelEntry(): Can't Delete Channel: (sourceChNum = %d)", sourceChNum));
    return;
  }

  BDBG_MSG(("**************************************************************"));
  BDBG_MSG(("Deleted IP Channel (sourceChNum %d)", sourceChNum));
  BDBG_MSG(("**************************************************************"));

  return;
}

//
// Function to receive a new channel entry
// Called by the IP Channel Map Library
//
void addChannelEntry(void *appCtx, bIpChannelMapInfo_t *ipChannelInfo)
{
  ChannelManager *chMgr;
  ChannelMap::ChannelMapEntry *chMapEntry;
  int channelIndex = -1;

  // get pointer to the Channel Map
  chMgr = (ChannelManager *)appCtx;
  BDBG_ASSERT(chMgr != NULL);

  // allocate a new Channel Map Entry
  chMapEntry = new ChannelMap::ChannelMapEntry();
  if (chMapEntry == NULL)
  {
    BDBG_ERR(("ERROR: addChannelEntry(): failed to allocate a new Channel Map Entry"));
    return;
  }

  // set channel entry type to be IP Stream
  chMapEntry->setType(btuner_ip_stream);

  // set channel entry sub type to be IP SAP
  chMapEntry->setSubType(bchannel_entry_SAP);

  //
  // Note: for now, only Video portion of the IP Channel Info is being used
  //

  // set the flag to do auto PID detection for this IP channel
  chMapEntry->ip.hasProgramPidsLine = false;
  chMapEntry->ip.gotAutoPids = false;

  // set the IP Address
  strcpy(chMapEntry->ip.params.open_params.ip_addr, ipChannelInfo->videoInfo.ipAddr);

  // set the Port Number
  chMapEntry->ip.params.open_params.port = ipChannelInfo->videoInfo.port;

  // set the Transport Protocol
  if (ipChannelInfo->videoInfo.protocol == UDP)
  {
        chMapEntry->ip.params.open_params.protocol = bsettop_ip_protocol_udp;
  }
  else if (ipChannelInfo->videoInfo.protocol == RTP_AVP)
  {
        chMapEntry->ip.params.open_params.protocol = bsettop_ip_protocol_rtp;
  }

  // set the source Channel Number
  chMapEntry->sourceChNum = ipChannelInfo->sourceChNum;

  // enable the entry
  chMapEntry->disabled = 0;

  // now add this entry into the ChannelMap
  chMgr->map()->add(chMapEntry);

  BDBG_MSG(("**************************************************************"));
  BDBG_MSG(("Added Channel (IP %s Port %d Proto %s, SourceChNum %d)",
      chMapEntry->ip.params.open_params.ip_addr,
      chMapEntry->ip.params.open_params.port,
      chMapEntry->ip.params.open_params.protocol == bsettop_ip_protocol_udp ? "UDP" : "RTP",
      chMapEntry->sourceChNum));
  BDBG_MSG(("**************************************************************"));

  return;
}

void ipChannelLineupAcquisitionStop(void)
{
  if(getenv("blive_sap_disable"))
  {
    if((strcasecmp(getenv("blive_sap_disable"), "y"))==0)
    {
      return;
    }
  }

  blive_sap_exit();
  printf("IP Channel Lineup Acquisition Stopped\n");
  return;
}

//
// Start IP Channel Lineup Acquisition
//
int ipChannelLineupAcquisitionStart(ChannelManager *chMgr, AVManager *avMgr, int sapTimeout)
{

  if(getenv("blive_sap_disable"))
  {
    if((strcasecmp(getenv("blive_sap_disable"), "y"))==0)
    {
      printf("IP Channel Lineup Acquisition is Disbled\n");
      return (0);
    }
  }

  avManager = avMgr;

  // initialize SAP Listener
  if (blive_sap_init(chMgr, sapTimeout, addChannelEntry, delChannelEntry) != 0)
  {
    BDBG_ERR(("ERROR: Failed to Setup the IP Channel Acquisition Module\n"));
    return (-1);
  }

  // initialize any additional SAP Addresses

  printf("IP Channel Lineup Acquisition Started\n");
  return (0);
}


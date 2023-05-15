/***************************************************************************
 *     Copyright (c) 2004-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mpeg2transportstreamrtpsource.cpp $
 * $brcm_Revision: 1 $
 * $brcm_Date: 7/25/06 5:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/mpeg2transportstreamrtpsource.cpp $
 * 
 * 1   7/25/06 5:05p jjordan
 * PR22906: Integrate liveMedia open-source for RTP and RTCP support
 * 
 ***************************************************************************/

 // JJ - new module
#include "UsageEnvironment.hh"
#include "GroupsockHelper.hh"
#include "mpeg2transportstreamrtpsource.h"

#define PAYLOAD_TYPE_MP2T	33

MPEG2TransportStreamRTPSource*
MPEG2TransportStreamRTPSource::createNew(UsageEnvironment& env, 
                                         Groupsock* RTPgs,
                                         unsigned rtpTimestampFrequency) {
	return new MPEG2TransportStreamRTPSource(env, RTPgs, 
											 rtpTimestampFrequency);
}

MPEG2TransportStreamRTPSource
::MPEG2TransportStreamRTPSource(UsageEnvironment& env, 
                                Groupsock* RTPgs,
                                unsigned rtpTimestampFrequency)
	: RTPSource(env, RTPgs, PAYLOAD_TYPE_MP2T, rtpTimestampFrequency) {
}

MPEG2TransportStreamRTPSource::~MPEG2TransportStreamRTPSource() {
	fRTPInterface.stopNetworkReading();
}

char const* MPEG2TransportStreamRTPSource::MIMEtype() const {
	return "video/MP2T";
}


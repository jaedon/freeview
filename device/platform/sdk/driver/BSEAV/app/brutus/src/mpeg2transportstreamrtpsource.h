/***************************************************************************
 *     Copyright (c) 2004-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mpeg2transportstreamrtpsource.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 7/25/06 5:06p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/mpeg2transportstreamrtpsource.h $
 * 
 * 1   7/25/06 5:06p jjordan
 * PR22906: Integrate liveMedia open-source for RTP and RTCP support
 * 
 ***************************************************************************/

 // JJ - new module
#ifndef _MPEG2TRANSPORTSTREAMRTPSOURCE_H
#define _MPEG2TRANSPORTSTREAMRTPSOURCE_H

#ifndef _RTP_SOURCE_HH
#include "RTPSource.hh"
#endif

class MPEG2TransportStreamRTPSource: public RTPSource {
public:
	static MPEG2TransportStreamRTPSource*
		createNew(UsageEnvironment& env, Groupsock* RTPgs,
				  unsigned rtpPayloadFrequency = 90000);

protected:
	virtual ~MPEG2TransportStreamRTPSource();

private:
	MPEG2TransportStreamRTPSource(UsageEnvironment& env, Groupsock* RTPgs,
								  unsigned rtpTimestampFrequency);
	virtual char const* MIMEtype() const; 

	virtual void doGetNextFrame() {};
	virtual void setPacketReorderingThresholdTime(unsigned uSeconds) {};
};

#endif /* _MPEG2TRANSPORTSTREAMRTPSOURCE_H */



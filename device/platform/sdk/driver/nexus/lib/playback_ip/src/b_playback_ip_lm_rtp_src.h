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
 * $brcm_Workfile: b_playback_ip_lm_rtp_src.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 7/21/08 4:17p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/lib/playback_ip/src/b_playback_ip_lm_rtp_src.h $
 * 
 * 1   7/21/08 4:17p ssood
 * PR42739: Moving LiveMedia wrapper functionality part of IP Applib
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
	virtual void setPacketReorderingThresholdTime(unsigned uSeconds) {(void)uSeconds;};
};

#endif /* _MPEG2TRANSPORTSTREAMRTPSOURCE_H */



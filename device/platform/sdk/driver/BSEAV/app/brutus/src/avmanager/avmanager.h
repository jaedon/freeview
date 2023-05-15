/***************************************************************************
 *     Copyright (c) 2002-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: avmanager.h $
 * $brcm_Revision: 11 $
 * $brcm_Date: 7/9/09 11:50a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/avmanager/avmanager.h $
 * 
 * 11   7/9/09 11:50a jtna
 * PR56522: Remove multi-decode fast channel change feature. To be
 * replaced with video primer FCC.
 * 
 * 10   5/27/08 4:42p jtna
 * PR42200: Multi-decode fast channel change feature
 * 
 * 9   1/29/07 11:18a ssood
 * PR24918: Channel Map Redesign to make its access Thread-safe
 * 
 * 8   11/6/06 10:53a ssood
 * PR25072: Unable to play Video error while recording a SAP channel
 * 
 * 7   11/2/06 11:06a arbisman
 * PR23775: Modified to use playback manager
 * 
 * 6   9/28/06 6:55p arbisman
 * PR24289: Back-out changes
 * 
 * 4   5/25/05 11:09a erickson
 * PR14721: removed lock_callback and simply read channelmgr state at task
 * time
 * 
 * 3   4/9/05 12:20a erickson
 * PR14701: added SUBCHANNEL_SUPPORT and moved tune/untune code from
 * AVManager to AVStream. This way, AVStream can retune and rescan itself
 * as it comes in and out of lock.
 * 
 * 2   4/7/05 5:13p erickson
 * PR14701: rearrange AVStream so that current channel is stored in a
 * wrapper
 * 
 * 1   2/7/05 7:52p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/1   4/30/04 12:06p erickson
 * PR10832: rework of brutus to support single AVManager and
 * ChannelManager for system
 *
 ******************************************************************************/
#ifndef AVMANAGER_H__
#define AVMANAGER_H__

#include <bsettop.h>
#include <mlist.h>
#include <bstd.h>
#include <bkni_multi.h>
#include "avstream.h"

class ChannelManager;


/**
The AVManager calls the ChannelManager and builds AVStream objects using the streams and
tuners returned. It has it's own check in/check out system for these AVStream
objects.

Like the ChannelManager, you can have one AVManager instance for the entire settop.
This instance can be used by multiple application instances running in separate
threads. Synchronization is implemented internally.

You can also instantiate two AVManagers if you want two separate instances.

This is a thin layer on top of ChannelManager, but it keeps backend logic (AV) out of 
frontend code.
**/
class AVManager {
public:
	AVManager(ChannelManager *chanmgr);
	virtual ~AVManager();

	/**
	Summary:
	Tune a channel.
	
	Description:
	If you successfully tune, you will receive an AVStream. You can decode or record
	this stream. When you are no longer decoding or recording, you should untune
	the stream in order to make the resources available to others.

	The channel number is relative to the ChannelManager passed into the AVManager
	constructor.

	If you are channel changing, it may be okay to use a stream that is currently
	being recorded. If false, you are requesting a totally unused stream.
	**/
	AVStream *tune(int channel, bool allowRecordingStream = false, Video *video=NULL, PlaybackManager *playMgr=NULL);

	/**
	Only untune after record and decode have stopped.
	**/
	void untune(AVStream *stream);
	
	/**
	Call this from an external application to provide timer functionality.
	**/
	void timer(unsigned long time);

	/**
	Funtion to find the stream associated with a given channel
        using the channel entry subtype & ch num assigned by the ch creator
	**/
	AVStream *find(bchannel_entry_sub_type chSubType, int sourceChNum);

protected:
	BKNI_MutexHandle _mutex;
	ChannelManager *_chanmgr;
	MAutoList<AVStream> _streams; // AVManager owns these instances
	MList<AVStream> _checkLockList; // streams that must be checked

	AVStream *find(bband_t band);
};

#endif // AVMANAGER_H__

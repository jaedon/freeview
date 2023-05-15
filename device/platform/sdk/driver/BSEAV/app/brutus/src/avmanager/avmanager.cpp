/***************************************************************************
 *     Copyright (c) 2002-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: avmanager.cpp $
 * $brcm_Revision: 19 $
 * $brcm_Date: 7/9/09 11:49a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/avmanager/avmanager.cpp $
 * 
 * 19   7/9/09 11:49a jtna
 * PR56522: Remove multi-decode fast channel change feature. To be
 * replaced with video primer FCC.
 * 
 * 18   5/27/09 11:53a jtna
 * PR42200: remove comment
 * 
 * 17   5/27/08 4:42p jtna
 * PR42200: Multi-decode fast channel change feature
 * 
 * 16   11/19/07 10:10a mward
 * PR35907: Don't re-use "idle" stream.  DUAL_OUTPUT's other thread may
 * have tuned it.
 * 
 * 15   1/29/07 11:18a ssood
 * PR24918: Channel Map Redesign to make its access Thread-safe
 * 
 * 14   11/6/06 10:53a ssood
 * PR25072: Unable to play Video error while recording a SAP channel
 * 
 * 13   11/2/06 11:06a arbisman
 * PR23775: Modified to use playback manager
 * 
 * PROD_IPSTB_REFSW_Devel/2   10/24/06 2:43p arbisman
 * PR24976: IP Channel can only be recorded once at a time
 * 
 * PROD_IPSTB_REFSW_Devel/1   10/12/06 7:25p arbisman
 * PR23775: Use playback manager
 * 
 * 12   9/28/06 6:55p arbisman
 * PR24289: Back-out changes
 * 
 * 10   8/17/05 4:57p erickson
 * PR16718: must check if AVStream::tune fails
 * 
 * 9   5/25/05 11:09a erickson
 * PR14721: removed lock_callback and simply read channelmgr state at task
 * time
 * 
 * 8   4/22/05 4:46p erickson
 * PR14721: added debug
 * 
 * 7   4/19/05 3:46p erickson
 * PR14701: make digital subchannel support standard for all platforms
 * 
 * 6   4/19/05 10:42a erickson
 * PR14721: handle relock for dual tuner
 * 
 * 5   4/14/05 10:58a erickson
 * PR14701: removed extra ReleaseMutex and make sure we only add unique
 * entries to _checkLockList
 * 
 * 4   4/9/05 12:20a erickson
 * PR14701: added SUBCHANNEL_SUPPORT and moved tune/untune code from
 * AVManager to AVStream. This way, AVStream can retune and rescan itself
 * as it comes in and out of lock.
 * 
 * 3   4/7/05 5:13p erickson
 * PR14701: rearrange AVStream so that current channel is stored in a
 * wrapper
 * 
 * 2   3/28/05 4:58p erickson
 * PR11759: added multiple linein support
 * 
 * 1   2/7/05 7:52p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/6   6/11/04 3:48p erickson
 * PR11332: improved some debug msgs
 * 
 * Irvine_BSEAVSW_Devel/5   5/28/04 12:34p erickson
 * PR11258: added better dbg
 * 
 * Irvine_BSEAVSW_Devel/4   5/27/04 6:35p bandrews
 * PR11254: Moved encoder restart into block for non-recording streams.
 * 
 * Irvine_BSEAVSW_Devel/3   5/10/04 4:51p erickson
 * PR10832: added requiredEncodeForDecode flag instead of hardcoding it
 * into AVManager
 * 
 * Irvine_BSEAVSW_Devel/2   4/30/04 5:27p erickson
 * PR10832: more debug related to new channelmgr/avmanager
 * 
 * Irvine_BSEAVSW_Devel/1   4/30/04 12:06p erickson
 * PR10832: rework of brutus to support single AVManager and
 * ChannelManager for system
 *
 ******************************************************************************/
#include "avmanager.h"
#include "avstream.h"
#include "recordengine.h"
#include "video.h"
#include "playengine.h"
#include "channelmgr.h"
#include <assert.h>
#include <bstd.h>
BDBG_MODULE(avmanager);

AVManager::AVManager(ChannelManager *chanmgr)
{
	_chanmgr = chanmgr;
	BKNI_CreateMutex(&_mutex);
}

AVManager::~AVManager()
{
	BKNI_DestroyMutex(_mutex);
}

AVStream *AVManager::find(bband_t band)
{
	AVStream *str;
	MListItr<AVStream> itr(&_streams);
	for (str = itr.first(); str; str = itr.next()) {
		if (str->band() == band)
			break;
	}
	return str;
}

AVStream *AVManager::find(bchannel_entry_sub_type subType, int sourceChNum)
{
        AVStream *str = NULL;
        MListItr<AVStream> itr(&_streams);
        for (str = itr.first(); str; str = itr.next()) {
                if (str->chSubType() == subType && str->chSourceChNum() == sourceChNum)
                        break;
        }
        return str;
}

AVStream *AVManager::tune(int chnum, bool allowRecordingStream, Video *video, PlaybackManager *playMgr)
{
	AVStream *str = NULL;

	BDBG_MSG(("tune ch%d (%d streams already tuned)",chnum, _streams.total()));

	BKNI_AcquireMutex(_mutex);

	if (allowRecordingStream) {
		MListItr<AVStream> itr(&_streams);
		for (str = itr.first(); str; str = itr.next()) {
#if(B_HAS_PLAYPUMP_IP)
            /* IP playpump doesn't allow two playback channels with the same IP addr/port combo */
			if (str->ch() == chnum && str->hasDecode() && str->isRecording()) 
			    break;
#endif
			if (str->ch() == chnum && !str->hasDecode()) 
			{
				if (!str->isRecording())
                {
                    BDBG_MSG(("Found a tuned stream that is idle"));
                }
                else
                {
                    BDBG_MSG(("Bringing background record to foreground"));
                    break;
                }
			}
		}
	}

	if (!str) {
		str = new AVStream(_chanmgr);
		if (str->tune(chnum, video, playMgr)) {
			delete str;
			str = NULL;
		}
		else {
			_streams.add(str);
			BDBG_MSG(("new AVStream %p",str ));
		}
	}
	
	BKNI_ReleaseMutex(_mutex);
	return str;
}

void AVManager::untune(AVStream *str)
{
	BKNI_AcquireMutex(_mutex);
	str->untune();
	_streams.remove(str);
	_checkLockList.remove(str);
	BDBG_MSG(("delete AVStream %p", str));
	delete str;
	BKNI_ReleaseMutex(_mutex);
}

void AVManager::timer(unsigned long time)
{
	bband_t band;
	AVStream *str;
	
	BKNI_AcquireMutex(_mutex);
	
	while ((band = _chanmgr->getLockChange())) {
		str = find(band);
		BDBG_MSG(("lockChange for AVSteam=%p, band=%p", str, band));
		if (str) {
			if (_checkLockList.index(str) == -1)
				_checkLockList.add(str);
		}
		else {
			BDBG_ERR(("Unable to find AVStream for band %p", band));
		}
	}
	
	MListItr<AVStream> itr(&_streams);
	/* pass on the timer to all avstreams */
	for (str = itr.first(); str; str = itr.next())
		str->timer(time);

	/* Now we can actually check lock */		
	while (_checkLockList.total()) {
		str = _checkLockList.get(0);
		BDBG_MSG(("timer checking %p", str));
		str->checkLock();
		_checkLockList.remove(0);
	}

	BKNI_ReleaseMutex(_mutex);
}


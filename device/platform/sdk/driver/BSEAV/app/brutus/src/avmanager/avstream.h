/***************************************************************************
 *     Copyright (c) 2002-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: avstream.h $
 * $brcm_Revision: 28 $
 * $brcm_Date: 7/23/10 12:38p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/avmanager/avstream.h $
 * 
 * 28   7/23/10 12:38p jtna
 * SW7550-478: extend video primer FCC support to platforms with fewer STC
 * channels
 * 
 * 27   1/21/10 5:42p mward
 * SW7400-2517: merge to main support for BUILD_SYSTEM=magnum.
 * 
 * SW7400-2517/2   11/18/09 9:15a mward
 * SW7400-2517: merge latest from main.
 * 
 * 26   10/23/09 4:28p jtna
 * SW7405-3265: handle video primer FCC better
 * 
 * SW7400-2517/1   9/14/09 3:09p mward
 * SW7400-2517: Maintain Native Settop API (BUILD_SYSTEM=magnum) build for
 * 97400.
 * 
 * 25   8/25/09 12:47p jtna
 * SWDEPRECATED-3910: video primer fast channel change feature
 * 
 * 24   7/9/09 11:50a jtna
 * PR56522: Remove multi-decode fast channel change feature. To be
 * replaced with video primer FCC.
 * 
 * 23   9/15/08 12:40p tokushig
 * PR43426: merge to main
 * 
 * BRUTUS_97405_97043/2   9/3/08 2:51p tokushig
 * PR43426: use transcode handle opened in control in avstream.  do not
 * open another one in startLiveTranscode
 * 
 * BRUTUS_97405_97043/1   7/31/08 5:38p tokushig
 * PR45076: added button scrolling on homescreen and blue skin support.
 * 
 * 22   8/13/08 6:59p shyam
 * PR43426 : Changes for modal window
 * 
 * 21   7/30/08 8:11p shyam
 * PR43426 : Add dynamic toggling between transcode and decode
 * 
 * 20   6/25/08 12:55p shyam
 * PR43426 : Add 7043 Transcode support
 * 
 * 19   5/27/08 4:42p jtna
 * PR42200: Multi-decode fast channel change feature
 * 
 * 18   12/6/07 12:53p erickson
 * PR37967: rename functions for consistency
 * 
 * 17   6/22/07 1:52p erickson
 * PR30701: reimplemented AVStream's record on lock feature. we must
 * support AVStream's which don't have bstream_t yet.
 * 
 * 16   1/29/07 11:18a ssood
 * PR24918: Channel Map Redesign to make its access Thread-safe
 * 
 * 15   12/13/06 5:18p erickson
 * PR26538: fix no PVR_SUPPORT
 * 
 * 14   11/6/06 10:53a ssood
 * PR25072: Unable to play Video error while recording a SAP channel
 * 
 * 13   11/2/06 11:06a arbisman
 * PR23775: Modified to use playback manager
 * 
 * PROD_IPSTB_REFSW_Devel/1   10/12/06 7:27p arbisman
 * PR23775: Use playback manager for IP channels
 * 
 * 12   9/28/06 6:56p arbisman
 * PR24289: Back-out changes
 * 
 * 10   4/7/06 10:52a ahulse
 * PR20504: Merge Vpp Modifications into main branch
 * 
 * Vpp_7042_Demo/1   3/29/06 5:51p ahulse
 * PR20504: Enable support for changeing 7042's vpp settings when encoding
 * analog
 * 
 * 9   3/22/06 12:59p mphillip
 * PR19786: Fix a couple errors with PVR recording, and transitioning
 * in/out of PVR record
 * 
 * 8   3/15/06 4:03p mphillip
 * PR19786: Initial implementation.  Add CONTINUOUS_RECORD_ENABLED=1 to
 * brutus.cfg to enable
 * 
 * 6   8/24/05 6:09p arbisman
 * PR16678: Add support for WinCE
 * 
 * 5   4/19/05 10:42a erickson
 * PR14721: handle relock for dual tuner
 * 
 * 4   4/18/05 12:24p erickson
 * PR14593: update the ui when an AVStream changes in the background
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
 * 1   2/7/05 7:53p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/2   4/30/04 5:28p erickson
 * PR10832: more debug related to new channelmgr/avmanager
 * 
 * Irvine_BSEAVSW_Devel/1   4/30/04 12:06p erickson
 * PR10832: rework of brutus to support single AVManager and
 * ChannelManager for system
 *
 ******************************************************************************/
#ifndef AVSTREAM_H__
#define AVSTREAM_H__

#include <bsettop.h>
#include <mlist.h>
#include "channelmap.h"
#if NEXUS_BUILD
#include "nexus_video_decoder.h"
#include "nexus_video_decoder_primer.h"
#endif
class ChannelManager;
class RecordEngine;
class DecodeEngine;
class DecodeManager;
class PlaybackManager;
class PlayEngine;
class Video;

/**
The AVStream bundles everything needed for a live stream.
You can record or decode an AVStream.

On digital channels, it handles loss and regaining of lock. If lock is lost,
a timer is started. If the timer expires and it's still not locked, decode will be
stopped and the programs will be cleared. When you lock, if you have no programs, 
it will scan the PSI information, then start decode on the current channel.

In encapsulates the following:
  o Analog or digital bstream_t
  o btuner_t used to obtain the stream
  o Encode or linein resources which are needed for the analog stream

For some platforms, analog streams must always be encoded, even for decode.
This is handled internally.

An AVStream is obtained from AVManager::tune. After getting an AVStream
you should decode, record or do both. When you are done decoding and recording,
you should untune the stream in order to make resources available.
*/
class AVStream {
public:
	AVStream(ChannelManager *chanmgr);

	int tune(int chnum, 
		Video *video, /* only used for IP settop */
		PlaybackManager *playMgr=NULL /* only used for IP settop */
		);
	int untune();
	bool isLocked();
	
	/* If the stream changes in the background, this callback
	will fire. */
	void setCallback(bsettop_callback callback, void *context);
	
#ifdef PVR_SUPPORT
	/**
	Summary:
	Start recording the AVStream.

	Description:
	The Video .info file will be overwritten.
	The DecodeManager is required in order to stopTimeshifting upon stopRecord.
	**/
	int startRecord(RecordEngine *record, DecodeManager *decodeManager, Video *video);
	bool isRecording() const {return (_record != 0);}
	RecordEngine *recordEngine() const;
	void stopRecord();
#else
	bool isRecording() const {return false;}
#endif

	/**
	Start live decode of the AVStream.
	If the AVStream doesn't have a stream yet (possibly no lock), it will keep
	the DecodeEngine in case it gains lock. It will release the DecodeEngine
	when you call stopDecode.
	**/
	int startDecode(DecodeEngine *decode);
	bool isDecoding() const {return _decode && liveStream();}
	bool hasDecode() const {return (_decode != 0);}
	void stopDecode();

	/**
	Returns true if the live decode is digital.
	**/
	bool isDigitalDecode() const {return !_analogStream || _encodeForDecode;}
	bool isEncoding() const {return _digitalStream && _analogStream;}
	btuner_t tuner() const {return _tuner;}
	bband_t band() const {return _band;}
	bstream_t liveStream() const;
	int ch() const {return _channel.chnum;}
	int chSubType() const {return _channel.entry.getSubType();}
	int chSourceChNum() const {return _channel.entry.sourceChNum;}
	Video *onLockVideo() const {return _onLockVideo;}
	
	void checkLock(bool fromTimer = false);
	
	/**
	Called from AVManager to provide timer functionality.
	**/
	void timer(unsigned long time);

#ifdef PVR_SUPPORT
	PlayEngine *playEngine() const {return _playback;}
#endif
	void setChDeferDelete() {_chDeferDelete = true;}

#ifdef B_HAS_TRANSCODE
    bool isTranscoding() const {return _transcodeOn;}
	int startLiveTranscode(bencode_quality quality, bool transcodeEnable) ;
	bool isTranscoding() { return _transcodeOn ; }
#endif

/* video primer fast channel change feature */
	int initPrimers(int stopIndex);
	int initPrimers(DecodeEngine *d, int stopIndex);
	void uninitPrimers();
	void setEnablePrimers(bool use) { enablePrimers = use; }
	int getNumOpenedPrimers() { return numOpenedPrimers; }
	int setChannelFromPrimer(int oldCh, int newCh);

protected:
	ChannelManager *_chanmgr;
	ChannelMap::Channel _channel;
	
	bsettop_callback _callback;
	void *_context;
	
	bstream_t _digitalStream; /* current digital stream for this tuner */
	bstream_t _analogStream; /* if analog, this will be set. if you're encoding,
								the digital_stream will also be set. */
	bband_t _band; /* if we couldn't lock, we store the band for future lock */

	btuner_t _tuner; /* current tuner allocated */
	btuner_stream_type _tunertype;

	/* If analog, you may have linein and encode resources associated with
	this tuner. */
	btuner_linein_t _linein;
	bencode_t _encode;

	bool _encodeForDecode; /* If true, then the encoder is always on for an analog
							stream, so don't stop it when record stops. Otherwise the
							AVStream should stop/start the encoder
							with the record. */

	RecordEngine *_record; /* If not NULL, it's recording */
	DecodeEngine *_decode; /* If not NULL, it's decoding */
	DecodeManager *_decodeManager; /* This is required in order to stopTimeshifting */
#ifdef PVR_SUPPORT
	PlayEngine    *_playback;
	PlaybackManager *_pbManager;
#endif
    
	Video *_onLockVideo;

	/* Encoding is handled internally */
	int startEncode(bencode_quality quality, bencode_vpp_mode vpp_mode );
	void stopEncode();
	unsigned long _lastTime, _unlockTime;
	bool _chDeferDelete; /* true if channel associated with this stream is marked for deferred deletion */

#ifdef B_HAS_TRANSCODE
	bstream_t _transcodedStream; /* transcoded stream from the tuner */
	bool _transcodeOn ;
#endif
#ifdef NEXUS_BUILD
	#define MAX_FCC_PRIMERS 8
	NEXUS_VideoDecoderStartSettings videoProgram[MAX_FCC_PRIMERS];
	NEXUS_VideoDecoderPrimerHandle videoPrimer[MAX_FCC_PRIMERS];
#endif
	bool enablePrimers;
	int numOpenedPrimers;
};

#endif // AVSTREAM_H__

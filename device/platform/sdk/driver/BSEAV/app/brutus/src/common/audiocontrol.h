/***************************************************************************
 *     Copyright (c) 2002-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: audiocontrol.h $
 * $brcm_Revision: 7 $
 * $brcm_Date: 8/14/09 3:38p $
 *
 * Module Description: PCM playback control
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/common/audiocontrol.h $
 * 
 * 7   8/14/09 3:38p erickson
 * PR55994: refactor thumbnail
 *
 * 6   8/15/08 10:08a erickson
 * PR42789: bluetooth update
 *
 * 5   4/10/08 6:42p jgarrett
 * PR 41620: MP3 playback crashes after stop
 *
 * 4   1/10/06 3:33p jrubio
 * PR18606: add bpcm_play_t struct and destructor
 *
 * 3   7/18/05 12:08p erickson
 * PR15793: fixed for legacy, replaced pthread with b_start/stop_task
 *
 * 2   6/30/05 8:20a erickson
 * PR15793: made pcmthread support more generic, and disabld 3560 support
 * which has multiple problems
 *
 * 1   2/7/05 8:03p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/2   10/26/04 5:05p erickson
 * PR10763: added user-mode PCM playback support (enabling MP3 decode on
 * 97038 and others)
 *
 * Irvine_BSEAVSW_Devel/3   5/9/03 5:34p erickson
 * cheops is now browseable in pvrgui with broadcast notification
 *
 * Irvine_BSEAVSW_Devel/2   5/1/03 2:16p erickson
 * all platforms
 *
 * Irvine_BSEAVSW_Devel/1   4/30/03 5:02p erickson
 * new code common to narmer and cheops
 *
 ***************************************************************************/
#ifndef AUDIOCONTROL_H__
#define AUDIOCONTROL_H__

#include <mstring.h>
#include <mstringlist.h>
class Control;
#include "bsettop.h"
#include "bsettop_os.h"
#include "control.h"


/**
* AudioControl plays audio files using the shell and applications
* assumed to exist. The particular applications vary from platform
* to platform.
*
* For FPU-based platforms (937xx and 97320), mpg123 is used for MP3 files.
* For non FPU platforms, madplay is used for MP3 files.
* playwave is used for WAV files.
**/
class AudioControl {
public:
	AudioControl(Control *control);

    ~AudioControl();

	/**
	* playAudio plays an file.
	* If the file begins with http:, it is assumed to be streamed over the HTTP
	* protocol.
	*/
	bool playAudio(MString file);

	/**
	* Play a list of files.
	* NOTE: If madplay is used, only one file can be played at a time for now.
	**/
	bool playAudio(MStringList &list);

	/**
	* Returns true if PCM audio was playing and was stopped.
	* If reenable is false, MPEG/Analog audio is not reenabled.
	**/
	bool stopAudio(bool reenable = true);

	bool isPlaying() const {return _audioPid?true:false;}
    bool stopRequested() const {return _stopped;}
    BKNI_EventHandle getTaskEvent() const {return _taskEvent;}
    BKNI_EventHandle getStoppedEvent() const {return _stoppedEvent;}
    bpcm_play_t pcm_play() {return _pcm_play;}

protected:
	pid_t _audioPid;
    bpcm_play_t _pcm_play;

	int _pipe[2];
	b_task_t _task;
	void startAudioThread();
	void stopAudioThread();
	static void *b_play_audio(void *data);
    bool _stopped;
    BKNI_EventHandle _taskEvent, _stoppedEvent;

	Control *_control;
	Control *control() const {return _control;}
};

#endif //AUDIOCONTROL_H__

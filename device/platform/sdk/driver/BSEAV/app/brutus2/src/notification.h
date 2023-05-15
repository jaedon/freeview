/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: notification.h $
* $brcm_Revision: 29 $
* $brcm_Date: 10/11/12 2:36p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/notification.h $
* 
* 29   10/11/12 2:36p jrubio
* SW7346-1004: take out unused notifications
* 
* 28   10/8/12 6:47p tokushig
* SW7231-749: add lua commands for display settings
* 
* 27   10/8/12 4:15p jrubio
* SW7346-1004: add notifications
* 
* 26   10/7/12 3:05p tokushig
* SW7231-749: add notifications for display setting changes
* 
* 25   9/29/12 9:24p tokushig
* SW7231-749: connect audio settings window panel to simple audio
* decoder.  pid, spdif, hdmi, downmix, dualmono, dolby drc, and dialog
* norm are implemented.
* 
* 24   9/26/12 4:27p tokushig
* SW7231-749: add notification to set spdif/hdmi input type
* 
* 23   9/21/12 8:16p tokushig
* SW7231-749: change scan to scanStart notification name and add
* notification cmd to set program audio pid
* 
* 23   9/21/12 8:15p tokushig
* SW7231-749: change scan to scanStart notification name and add
* notification cmd to set program audio pid
* 
* 22   9/17/12 10:26a jrubio
* SW7346-1004: add record events
* 
* 21   8/29/12 10:48a jrubio
* SW7346-1004: add record event
* 
* 20   5/10/12 3:41p tokushig
* SW7231-749: add support for playback notifications.
* 
* 19   4/20/12 2:55p jrubio
* SW7346-644: remove unused eNotify vars
* 
* 18   4/19/12 7:57p tokushig
* SW7231-749: add dynamic scan options to main menu.  buttons will only
* correspond to allowed scan types based on the types of available
* tuners
* 
* 17   4/18/12 5:17p jrubio
* SW7346-644: add key presses for pvr
* 
* 16   4/18/12 12:49a jrubio
* SW7346-644: add Refresh from Disk notification
* 
* 18   4/19/12 7:52p tokushig
* SW7231-749: add dynamic scan options to main menu.  buttons will only
* correspond to allowed scan types based on the types of available
* tuners
* 
* 14   4/17/12 4:36p tokushig
* SW7231-749: menu button on remote will show/hide menu.  on screen
* channel number will timeout in 3 secs if the menu is not shown.  if
* the menu is shown, the channel number will always be displayed.
* 
* 13   4/15/12 9:43p tokushig
* SW7231-749: added audio source changed notification, audio decode
* start/stop notification, and video decode start/stop notification
* support
* 
* 13   4/15/12 9:42p tokushig
* SW7231-749: added audio source changed notification, audio decode
* start/stop notification, and video decode start/stop notification
* support
* 
* 12   4/15/12 12:12a jrubio
* SW7346-644: add playback stop event
* 
* 11   4/11/12 1:22p jrubio
* SW7346-644: add playback Start Notification
* 
* 10   4/9/12 7:29p tokushig
* SW7405-5581: added video source changed callback support
* 
* 9   3/27/12 3:30p tokushig
* SW7405-5581: synchronize lua commands
* 
* 8   3/12/12 8:52p tokushig
* SW7405-5581: add brutus.channelListDump() lua command
* 
* 7   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 6   2/28/12 3:01p tokushig
* SW7405-5581: add timer support and use it for deferred tuning using ch
* up/down.  fast pressing of ch up/down remote buttons will update the
* ui but not tune until button presses stop for 500msecs.  this allows
* the user to skip channels while traversing the channel list using the
* remote
* 
* 
*********************************************************************************/

#ifndef NOTIFICATION_H__
#define NOTIFICATION_H__

#include "brutus.h"
#include "brutus_os.h"
#include "mlist.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum eNotification {
    eNotify_All,
    eNotify_KeyUp,                  /* keypress - key pressed up */
    eNotify_KeyDown,                /* keypress - key pressed down */

    eNotify_Tune,                   /* command  - tune */
    eNotify_ChUp,                   /* command  - change channel up */
    eNotify_ChDown,                 /* command  - change channel down */
    eNotify_ScanStart,              /* command  - start channel scan */
    eNotify_ScanStop,               /* command  - stop channel scan */
    eNotify_PlaybackStart,          /* command  - start a playback */
    eNotify_PlaybackStop,           /* command  - stop a playback */
    eNotify_PlaybackTrickMode,      /* command  - do trickmode playback */
    eNotify_RecordStart,            /* command  - start a record */
    eNotify_RecordStop,             /* command  - stop a record */
    eNotify_RefreshPlaybackList,    /* command  - stop a playback */
    eNotify_ChannelListLoad,        /* command  - load channel list */
    eNotify_ChannelListSave,        /* command  - save channel list */
    eNotify_ChannelListDump,        /* command  - dump channel list */
    eNotify_SetAudioProgram,        /* command  - change current audio pid */
    eNotify_SetSpdifInput,          /* command  - change spdif input type */
    eNotify_SetHdmiAudioInput,      /* command  - change hdmi audio input type */
    eNotify_SetAudioDownmix,        /* command  - change audio downmix */
    eNotify_SetAudioDualMono,       /* command  - change audio dual mono */
    eNotify_SetDolbyDRC,            /* command  - change dolby dynamic range control */
    eNotify_SetDolbyDialogNorm,     /* command  - change dolby dialog normalization */
    eNotify_SetVideoFormat,         /* command  - change video format and graphics */
    eNotify_SetColorSpace,          /* command  - change video colorspace */
    eNotify_SetMpaaDecimation,      /* command  - change mpaa decimation state */
    eNotify_SetDeinterlacer,        /* command  - change MAD deinterlacer state */
    eNotify_SetBoxDetect,           /* command  - change box detect state */
    eNotify_SetAspectRatio,         /* command  - change aspect ratio */
    eNotify_SetAutoVideoFormat,     /* command  - change auto video format setting */
    eNotify_Exit,                   /* command  - exit brutus2 */

    eNotify_ChannelListChanged,     /* status   - channel list has changed */
    eNotify_CurrentChannel,         /* status   - current channel has changed */
    eNotify_DeferredChannel,        /* status   - deferred channel has changed */
    eNotify_ScanStarted,            /* status   - scan has started */
    eNotify_ScanStopped,            /* status   - scan has stopped */
    eNotify_ScanProgress,           /* status   - scan percent complete */
    eNotify_PlaybackListChanged,    /* status   - playback list updated */
    eNotify_PlaybackStateChanged,   /* status   - playback state has changed */
    eNotify_RecordStarted,          /* status   - record has started */
    eNotify_RecordStopped,          /* status   - record has stopped */
    eNotify_VideoSourceChanged,     /* status   - video source has changed */
    eNotify_VideoDecodeStarted,     /* status   - video decode started */
    eNotify_VideoDecodeStopped,     /* status   - video decode stopped */
    eNotify_AudioSourceChanged,     /* status   - audio source has changed */
    eNotify_AudioDecodeStarted,     /* status   - audio decode started */
    eNotify_AudioDecodeStopped,     /* status   - audio decode stopped */
    eNotify_VideoFormatChanged,     /* status   - video format has changed */
    eNotify_ColorSpaceChanged,      /* status   - video colorspace has changed */
    eNotify_MpaaDecimationChanged,  /* status   - video component mpaa decimation has changed */
    eNotify_DeinterlacerChanged,    /* status   - video deinterlacer setting has changed */
    eNotify_BoxDetectChanged,       /* status   - video box detect setting has changed */
    eNotify_AspectRatioChanged,     /* status   - video aspect ratio setting has changed */
    eNotify_AutoVideoFormatChanged, /* status   - auto video format setting has changed */
    eNotify_Timeout,                /* status   - timer expired */
    eNotify_Invalid,
    eNotify_Max
} eNotification;

/*
    CNotification is passed between CSubjects and CObservers.  It contains
    an id indicating the notification type and an optional data pointer.
*/
class CNotification 
{
public:
    CNotification(eNotification id, void * data = NULL);
    virtual eNotification getId(void);
    virtual void setData(void * data);
    virtual void * getData(void);
    ~CNotification(void);
protected:
    eNotification   _id;
    void          * _data;
};

/* 
    CObserver objects are able to monitor notifications from CSubject objects.
*/
class CObserver;
typedef void (* NOTIFY_CALLBACK)(CObserver * context, CNotification & notification);

class CObserver 
{
public:
    CObserver(NOTIFY_CALLBACK notifyCallback, CObserver * context = NULL);
    virtual eRet notify(CNotification & notification);
    virtual void setNotifyCallback(NOTIFY_CALLBACK notifyCallback);
    virtual void setNotifyContext(CObserver * context);
    bool operator==(const CObserver & other) const {
        return ((_notifyCallback == other._notifyCallback) &&
                (_context == other._context));
    };
    virtual ~CObserver(void);

    virtual void processNotification(CNotification & notification);

protected:
    NOTIFY_CALLBACK   _notifyCallback;
    CObserver       * _context;    
};

/* 
    CSubject objects can be monitored by CObserver objects.  Changes to
    the subject can be monitored by one or more CObserver objects.
*/
class CSubject 
{
public:
    CSubject(void);
    virtual eRet registerObserver(CObserver * observer, eNotification notification = eNotify_All);
    virtual eRet unregisterObserver(CObserver * observer, eNotification notification = eNotify_All);
    virtual eRet notifyObservers(eNotification notification, void * data = NULL);
    virtual eRet clearAllObservers(void);
    virtual ~CSubject(void);

protected:
    class CObserverListElem
    {
    public:
        CObserverListElem(CObserver * pObserver, eNotification notification) :
            _pObserver(pObserver),
            _notification(notification) { }
        CObserver * getObserver() { return _pObserver; }
        eNotification getNotification() { return _notification; }
        bool operator==(const CObserverListElem & other) const {
            return ((_pObserver == other._pObserver) && 
                    (_notification == other._notification));
        }

    protected:
        CObserver     * _pObserver;
        eNotification   _notification;
    };

    MList <CObserverListElem> _observerList;
    B_MutexHandle             _mutex;
};


#ifdef __cplusplus
}
#endif

#endif /* NOTIFICATION_H__ */

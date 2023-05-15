/***************************************************************************
 *     Copyright (c) 2002-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: recordengine.h $
 * $brcm_Revision: 5 $
 * $brcm_Date: 8/30/10 11:52a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/avmanager/recordengine.h $
 * 
 * 5   8/30/10 11:52a erickson
 * SW7550-551: clear record overflow
 * 
 * 4   12/6/07 2:08p erickson
 * PR37038: add CONTINUOUS_RECORD_DURATION, remove unused fixedSize and
 * CIRCFILE.
 * 
 * 3   9/10/07 4:15p ahulse
 * PR28979: Have control poll recordengine for record error condition
 * 
 * 2   3/15/06 4:04p mphillip
 * PR19786: Initial implementation.  Add CONTINUOUS_RECORD_ENABLED=1 to
 * brutus.cfg to enable
 * 
 * 1   2/7/05 7:54p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 ***************************************************************************/
#ifndef RECORDENGINE_H__
#define RECORDENGINE_H__

#include "microwidgets.h"
#include "mstring.h"
#include "bsettop.h"
#include "video.h"
#include "instmanager.h"

// used by playengine and recengine
#define DEFAULT_ENCRYPTION_KEY {12345678, 87654312, 0, 0} //NOTE: 12, not 21!

class Config;

BEGIN_EV(RecordEngine)
	EV_METHOD(RecordEngine, StateChanged)
END_EV()

class PlayEngine;

class RecordEngine {
	friend class PlayEngine;

public:
	RecordEngine(const char *name, int index);
	virtual ~RecordEngine();

	void setEncryptionKey(unsigned long key[4]);

	int start(Video *video);
	int stop();

	void setModalState( bool displayed );	/* If this instance has displayed modal box set to true */
	bool getModalState( void );

	int index() const {return _index;}
	bool valid() const {return _record != NULL;}
	const char *name() const {return _name;}
	brecord_t handle() const {return _record;}
	Video *video() const {return _video;}
	void getStatus(brecord_status *status) const;
	bool isActive() const {return _recording;}

	/* Returns true is there is an error to report */
	bool getError(brecord_error *error, bool *overflow) const;
	void clearOverflowError();
	
	void *fifo_file() { return _fifo_file; };

protected:
	Video *_video;
	brecpump_t _recpump;
	brecord_t _record;
	brecord_file_t _file;
	void *_fifo_file;
	MString _name;
	bool _recording;
	bool _hasError;
	bool _modalDisplayed;   /* set to true if modal dialog box has been displayed already */
	brecord_error _error; // only meaningful if _hasError is true and _overflow is false
	bool _overflow; // only meaningful if _hasError is true
	uint32_t _key[4];
	int _index;

	static void error_callback(void *context, brecord_error reason);
	static void overflow_callback(void *context, bool *stop);

	SUPPORT_EV(RecordEngine)
	SUPPORT_EV_METHOD(StateChanged)
};

/**
Use a RecordManager to check in/out RecordEngine's.
**/
class RecordManager : public InstanceManager<RecordEngine> {
};

#endif //RECORDENGINE_H__

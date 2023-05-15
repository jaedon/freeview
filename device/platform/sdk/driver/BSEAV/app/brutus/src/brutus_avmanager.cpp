/***************************************************************************
 *     Copyright (c) 2002-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: brutus_avmanager.cpp $
 * $brcm_Revision: 2 $
 * $brcm_Date: 10/16/07 12:33p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/brutus_avmanager.cpp $
 * 
 * 2   10/16/07 12:33p erickson
 * PR36068: survive bad RecordEngine instantiation
 * 
 * 1   2/7/05 7:55p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/3   5/28/04 12:32p erickson
 * PR11258: added more dbg output
 * 
 * Irvine_BSEAVSW_Devel/2   4/30/04 5:27p erickson
 * PR10832: more debug related to new channelmgr/avmanager
 * 
 * Irvine_BSEAVSW_Devel/1   4/30/04 12:06p erickson
 * PR10832: rework of brutus to support single AVManager and
 * ChannelManager for system
 *
 ******************************************************************/
#include "avmanager.h"
#include "channelmgr.h"
#include "cfgsettings.h"
#include "recordengine.h"
#include <assert.h>
#include <bstd.h>
BDBG_MODULE(brutus_avmanager);

static bool readEncryptionKeys(unsigned long key[], const Config *cfg) {
	// we have to read at least one to report back that it's useful
	bool got_one = 0;

	// update encryption keys
	for (int i=0;i<4;i++) {
		char buf[40];
		sprintf(buf, "ENCRYPTION_KEY%d", i);
		if (cfg->get(buf)) {
			unsigned long temp = strtoul(cfg->get(buf), NULL, 16);
			fprintf(stderr, "OVERRIDE: encryption key[%d] = %lu\n", i, temp);
			key[i] = temp;
			got_one = true;
		}
	}
	return got_one;
}

#ifdef PVR_SUPPORT
RecordManager *createRecordManager(int records, const Config *cfg)
{
	RecordManager *recordManager = new RecordManager();
	unsigned long key[4] = {0,0,0,0};
	bool setKeys = readEncryptionKeys(key, cfg);

	for (int i=0;i<records;i++) {
		RecordEngine *rec = new RecordEngine("rec", i);
		if (!rec->valid()) {
    		BDBG_ERR(("unable to create RecordEngine %d", i));
    		delete rec;
    		break;
		}

		if (setKeys)
			rec->setEncryptionKey(key);

		recordManager->add(rec);
	}
	return recordManager;
}

void destroyRecordManager(RecordManager *recordManager)
{
	if (recordManager->active())
		BDBG_ERR(("Destroying an AVManager with %d active record session(s)",
			recordManager->active()));
	/* Check out all the RecordEngine's and destory them */
	RecordEngine *rec;
	while ((rec = recordManager->checkOut())) {
		delete rec;
	}

	delete recordManager;
}
#endif

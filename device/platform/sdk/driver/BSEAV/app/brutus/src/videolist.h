/***************************************************************************
 *     Copyright (c) 2002-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: videolist.h $
 * $brcm_Revision: 5 $
 * $brcm_Date: 4/2/08 3:56p $
 *
 * Module Description: Collection of screens for pvrgui demo app
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/videolist.h $
 * 
 * 5   4/2/08 3:56p mward
 * PR40307: Don't include any thumbnail support with PVR_SUPPORT=n.
 * 
 * 4   3/18/08 4:05p erickson
 * PR40307: added thumbnail feature
 * 
 * 3   2/21/08 2:13p ahulse
 * PR25542: Enable retreval of video object by media filename
 * 
 * 2   7/18/07 7:06p vsilyaev
 * PR 32813: Modified to scan all supported media files, fixed memory leak
 * 
 * 1   3/14/07 10:16a erickson
 * PR26267: refactor Brutus into separate VideoList and PlaybackScreen
 * 
 ***************************************************************************/
#ifndef VIDEOLIST_H__
#define VIDEOLIST_H__

#include <mlist.h>
#include <mstringlist.h>
#include <microwidgets.h>
#include "video.h"
#ifdef PVR_SUPPORT
#include "bthumbnail.h"
#endif

BEGIN_EV(VideoList)
    EV_METHOD(VideoList, Update) // called any time a Video is added or removed
END_EV()

/**
Data structure for managing Video objects.
Brutus RecordScreen and PlaybackScreen use it.
Use Update event to keep UI in sync.
**/
class VideoList {
public:
#ifdef PVR_SUPPORT
	VideoList(bthumbnail_manager_t thumbnail_manager);
#else
	VideoList();
#endif	
	// multiple video directory management
	void addDir(const char *label, const char *dir);
	
	const char *getMediaFileName(int index);

	const char *defaultVideoDirLabel();
	const char *defaultVideoDir();

	/**
	Incremental update from disk, adding or removing Video's as needed.
	**/
	void refreshFromDisk();

	void add(Video *vid);
	Video *findMedia(const char *mediafile) const;
	Video *find(const char *infoFileName) const;
	Video *get(int index) const;
	int total() const;
	
	/* Remove Video from data structure, but not from disk. 
	Call refreshFromDisk to repopulate VideoList. */
	void clear();

	/**
	Deletes the video from the disk, including info file, mpegfile
	and indexfile.
	Returns true if all files were deleted. If not all files were deleted, the Video remains in the list.
	**/
	bool deleteVideo(Video *vid);
	void deleteAll();

	/* Find the next file and sort indexes for creating a new Video */
	void findNextVideoIndex(int *fileId, int *sortIndex);
	
protected:
	MStringList _dirs, _labels;
	int _lastFileNumber;
	MList<Video> _videoList;
#ifdef PVR_SUPPORT
	bthumbnail_manager_t _thumbnail_manager;
#endif	
	/* add/remove to/from list and fire updateCallback */
	bool removeVideo(Video *vid);
	
    SUPPORT_EV(VideoList)
    SUPPORT_EV_METHOD(Update)
private: 
	Video *parse_infofile(MString &infofilename, const char *dirstr, const char *label);
};

#endif /* VIDEO_LISTH__ */


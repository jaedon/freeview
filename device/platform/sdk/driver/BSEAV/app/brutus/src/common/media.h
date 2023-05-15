/***************************************************************************
 *     Copyright (c) 2003-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: media.h $
 * $brcm_Revision: 6 $
 * $brcm_Date: 9/17/07 12:42p $
 *
 * Module Description: Wrapper for .info file
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/common/media.h $
 * 
 * 6   9/17/07 12:42p jrubio
 * 
 * PR34613: Added function to write info file for network record
 * 
 * 5   12/21/05 11:53p erickson
 * PR18256: added copy constructor
 * 
 * 4   8/24/05 6:13p arbisman
 * PR16678: Add support for WinCE
 * 
 * 3   8/17/05 5:25p erickson
 * PR16719: rework Video and Media to never return MString as return value
 * 
 * 2   4/27/05 1:15p erickson
 * PR15075: use int64_t for 64 bit file offset instead of assuming off_t
 * is 64 bits (it's not on vxworks)
 * 
 * 1   2/7/05 8:04p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/5   2/4/05 10:48a erickson
 * PR12728: cleanup for -pedantic warnings and errors
 * 
 * Irvine_BSEAVSW_Devel/4   11/19/04 3:29p erickson
 * PR13256: rework Media so that it's more extensible and so print
 * functions are thread-safe and more general
 * 
 * Irvine_BSEAVSW_Devel/3   7/23/03 6:25p erickson
 * allow absolute index/media file paths
 *
 * Irvine_BSEAVSW_Devel/2   6/20/03 2:43p erickson
 * PES_VOB support
 *
 * Irvine_BSEAVSW_Devel/3   5/9/03 5:35p erickson
 * cheops is now browseable in pvrgui with broadcast notification
 * 
 * Irvine_BSEAVSW_Devel/2   5/2/03 4:00p erickson
 * initial cheops work
 *
 * Irvine_BSEAVSW_Devel/1   4/30/03 5:02p erickson
 * new code common to narmer and cheops
 *
 ***************************************************************************/
#ifndef MEDIA_H__
#define MEDIA_H__

#ifndef _WIN32_WCE
	#include <time.h>
#endif
#include "mstring.h"

/**
A Media object represents an audio, video or image resource which can be
used in a multi-media system. It stores information regarding:
1) type of media, including decoding method needed to play
2) location where to retrieve it and method needed to retrieve it
3) meta-data like author, description

Media does not depend on the Settop API, therefore it does not implement
MPEG Video and related audio modes. This is handled in the subclass Video.

**/
class Media {
public:
	Media();
	Media(Media &media); // copy constructor
	virtual ~Media() {}


	/* Information about the media format. This is extensible.
	Use setFormat() to set this information.
	The following information should be unique to each format for
	a given system, but this is not enforced. */
	MString formatName; /* same name passed into setFormat() */
	int formatNumber; /* optional */
	MString mimeType; /* optional */
	enum Type {
		eAudio, eVideo, ePicture, eAll
	} type;

	/* Sets format information based on unique format string.
	Returns 0 if format string found. */
	virtual int setFormat(const char *formatName);

	/* Storage information */
	MString infofile; // filename of .info file, includes path
	MString mediafile; // does not include path. use setPath() and mediafilePath() for this.

	/* Meta-data */
	MString title;
	MString description;
	MString genre;
	MString artist;
	MString collection; // i.e. album, series

	time_t recordtime; // seconds since Jan 1, 1970
	int sortIndex;

	// transient data
	// 0=unused,1=playing,2=recording,3=playing and recording
	// set by Control::setUsage
	int currentUsage;
	MString dirLabel;
	int marked; // used for scanning the disk

	/* Returns path + mediafile	*/
	const char *mediafilePath();
	
	/* Set the path to the media file */
	virtual void setPath(const char *path);

	/*
	mediafile size in bytes
	*/
	int64_t size();

	/*
	length of mediafile in milliseconds
	*/
	virtual int length();

	/*
	.info file operations
	*/
	int read(const char *filename);
	int write(const char *filename = NULL);

    int writeNetwork(const char *fname,int sd, char * f );
	// deletes the infofile, mediafile and any other related files
	virtual int deleteFiles();
	
	/* Adds current Media path to front of filename. Takes into account absolute
	filenames. */
	void addPath(MString &fullpath, const char *filename);

protected:
	MString _dirpath;
	MString _mediafilePath;
	virtual bool readNameValue(const char *name, const char *value);
	virtual int writeNameValues(FILE *f) {return 0;}
    virtual int writeNameValuesNetwork(char *f) {return 0;}
};

/**
The following functions are thread-safe, memory-overrun-safe methods of
converting common media-related data (i.e. length, time) into
commonly used string formats for the GUI.
*/

/* Prints memory size (in B, KB, MB or GB units) */
int b_print_size(char *buf, int length, int64_t size, bool shortname = true);

#define B_TIME_PERIOD_FORMAT_LONG 0			/* long format (H hours, M minutes, S seconds) */
#define B_TIME_PERIOD_FORMAT_COMPACT 1		/* compact format ([HH:]MM:SS) */

/* Prints time period (a duration) */
int b_print_time_period(
	char *buf, int length,
	int msec, /* time period in milliseconds */
	int format /* Use MSEC_TIME_FORMAT_ macros defined above */
	);

/* Prints calendar time */
int b_print_calendar_time(char *buf, int length, time_t time);

#endif // MEDIA_H__

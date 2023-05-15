/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: media.cpp $
 * $brcm_Revision: 13 $
 * $brcm_Date: 4/27/12 4:03p $
 *
 * Module Description: Wrapper for .info file
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/common/media.cpp $
 * 
 * 13   4/27/12 4:03p mward
 * SW7435-150:  Return error reading useless .info file.  No need to add
 * it to the list.  Case-sensitive compare of "mediafile", to exclude
 * ip_streamer .info files, which have a mediaFile entry.
 * 
 * 12   8/14/09 3:51p erickson
 * PR55994: fix #include for bsettop.h
 * 
 * 11   2/26/08 7:32a jrubio
 * PR39363: fix compile warnings
 * 
 * 10   9/17/07 12:42p jrubio
 * PR34613: Added function to write info file for network record
 * 
 * 9   6/14/07 3:23p rjlewis
 * PR32170: support Bs and KBs in size prints.  Made one common version
 * (no floats).
 * 
 * 8   10/27/06 11:47a erickson
 * PR24374: set title to mediafile if no title
 * 
 * 7   12/21/05 11:53p erickson
 * PR18256: added copy constructor
 * 
 * 6   12/19/05 6:39p rjlewis
 * PR18713: create a default media file name when it can't be retrieved
 * from ".info" file.
 * 
 * 5   8/24/05 6:13p arbisman
 * PR16678: Add support for WinCE
 * 
 * 4   8/17/05 5:25p erickson
 * PR16719: rework Video and Media to never return MString as return value
 * 
 * 3   6/14/05 2:15p erickson
 * PR15216: converted to BKNI_Snprintf
 * 
 * 2   4/27/05 1:15p erickson
 * PR15075: use int64_t for 64 bit file offset instead of assuming off_t
 * is 64 bits (it's not on vxworks)
 * 
 * 1   2/7/05 8:04p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/12   2/4/05 10:48a erickson
 * PR12728: cleanup for -pedantic warnings and errors
 * 
 * Irvine_BSEAVSW_Devel/11   11/19/04 3:29p erickson
 * PR13256: rework Media so that it's more extensible and so print
 * functions are thread-safe and more general
 * 
 * Irvine_BSEAVSW_Devel/10   7/14/04 1:36a erickson
 * PR11287: refactor linux-specific code into brutus_linux.cpp for easier
 * porting
 * 
 * Irvine_BSEAVSW_Devel/9   7/7/04 2:36p liangl
 * PR11817: Recorded video size shows negative number if the recorded size
 * is over 200MB
 * 
 * Irvine_BSEAVSW_Devel/8   2/26/04 11:32a erickson
 * PR9902: check if fopen fails and return error code
 * 
 * Irvine_BSEAVSW_Devel/7   2/3/04 10:14a erickson
 * PR9497: typecast for vxworks sytem calls
 * 
 * Irvine_BSEAVSW_Devel/6   11/6/03 5:12p erickson
 * PR8570: initial vxworks migration
 * 
 * Irvine_BSEAVSW_Devel/PR8570/1   11/6/03 5:05p erickson
 * initial vxworks migration
 *
 * Irvine_BSEAVSW_Devel/5   9/17/03 12:32p erickson
 * converted from original debug interface to magnum debug interface
 *
 * Irvine_BSEAVSW_Devel/4   8/27/03 4:37p erickson
 * initialize all members in constructor
 *
 * Irvine_BSEAVSW_Devel/3   7/23/03 6:25p erickson
 * allow absolute index/media file paths
 *
 * Irvine_BSEAVSW_Devel/2   6/20/03 2:43p erickson
 * PES_VOB support
 *
 * Irvine_BSEAVSW_Devel/3   5/9/03 5:34p erickson
 * cheops is now browseable in pvrgui with broadcast notification
 *
 * Irvine_BSEAVSW_Devel/2   5/2/03 4:00p erickson
 * initial cheops work
 *
 * Irvine_BSEAVSW_Devel/1   4/30/03 5:02p erickson
 * new code common to narmer and cheops
 *
 ***************************************************************************/
#ifdef _WIN32_WCE
#include <windows.h>
#include <Winbase.h>
#endif

#include <stdio.h>
#include "bstd.h"
#include "bsettop.h"
#include "bsettop_os.h"
#include "bkni.h"
#include "brutus_os.h"
#include "media.h"
#include "mstringlist.h"

BDBG_MODULE(media);

Media::Media()
{
	type = eAll;
	formatNumber = 0;
	recordtime = 0;
	sortIndex = 0;
	currentUsage = 0;
	marked = 0;
}

Media::Media(Media &media)
{
	formatName = media.formatName;
	formatNumber = media.formatNumber;
	type = media.type;
	infofile = media.infofile; 
	mediafile = media.mediafile;
	title = media.title;
	description = media.description;
	genre = media.genre;
	artist = media.artist;
	collection = media.collection; 
	recordtime = media.recordtime; 
	sortIndex = media.sortIndex;
	_dirpath = media._dirpath;
	_mediafilePath = media._mediafilePath;

	// transient	
	currentUsage = 0;
	marked = 0; 
}

int Media::read(const char *fname)
{
	MStringList list;
	infofile = fname;
	if (!readFile(fname, list))
		return -1;

	int values = 0;
	for (const char *buf = list.first(); buf; buf = list.next()) {
		char *val = strchr(buf, '=');
		if (val) {
			*val++ = 0;
			readNameValue(buf, val);
		}
		values++;
	}
	
	/* make sure every Video has a title. if not specified, use mediafile. */
	if (!title) {
		title = mediafile;
		/* if there's no mediafile, this .info is worthless, but give it a clean name. */
		if (!title) {
			title = "<empty>";
            return -1;
		}
	}

	return 0;
}

bool Media::readNameValue(const char *name, const char *value)
{
	if (!strcasecmp(name, "name"))
		title = value;
	else if (!strcmp(name, "mediafile") ||
			!strcasecmp(name, "mpegfile")) //deprecated
		mediafile = value;
	else if (!strcasecmp(name, "recordtime"))
		recordtime = MString(value).toInt();
	else if (!strcasecmp(name, "sortindex"))
		sortIndex = MString(value).toInt();
	else if (!strcasecmp(name, "description"))
		description = value;
	else if (!strcasecmp(name, "genre"))
		genre = value;
	else if (!strcasecmp(name, "artist"))
		artist = value;
	else if (!strcasecmp(name, "collection"))
		collection = value;
	else if (!strcasecmp(name, "format") ||
			!strcasecmp(name, "pidType")) // deprecated
	{
		setFormat(value);
	}
	else
		return false;
	return true;
}

int Media::write(const char *fname)
{
	if (fname == NULL)
		fname = infofile;
	else
		infofile = fname;
	FILE *f = fopen(infofile, "w");
	if (!f) return -1;

	if (title)
		fprintf(f, "name=%s\n", (const char *)title);
	if (description)
		fprintf(f, "description=%s\n", (const char *)description);
	if (genre)
		fprintf(f, "genre=%s\n", (const char *)genre);
	if (artist)
		fprintf(f, "artist=%s\n", (const char *)artist);
	if (collection)
		fprintf(f, "collection=%s\n", (const char *)collection);
	if (mediafile)
		fprintf(f, "mediafile=%s\n", (const char *)mediafile);
	if (recordtime)
		fprintf(f, "recordtime=%d\n", recordtime);
	if (sortIndex)
		fprintf(f, "sortindex=%d\n", sortIndex);
	fprintf(f, "format=%s\n", (const char *)formatName);

	writeNameValues(f);

	fflush(f);
	fclose(f);
	b_sync_disk();
	return 0;
}

int Media::writeNetwork(const char *fname,int sd, char * f )
{
    
    int temp = 0;
    char * original = f;

	if (fname == NULL)
		fname = infofile;
	else
		infofile = fname;

    BKNI_Memset(f, 0 ,256);
    
    
	if (!f) return -1;

	if (title)
		sprintf(f, "name=%s\n", (const char *)title);

    f += (strlen(f)-temp);
    temp = strlen(f);

	if (description)
		sprintf(f, "description=%s\n", (const char *)description);

    f += (strlen(f)-temp);
    temp = strlen(f);

    if (genre)
		sprintf(f, "genre=%s\n", (const char *)genre);

    f += (strlen(f)-temp);
    temp = strlen(f);

    if (artist)
		sprintf(f, "artist=%s\n", (const char *)artist);

    f += (strlen(f)-temp);
    temp = strlen(f);

    if (collection)
		sprintf(f, "collection=%s\n", (const char *)collection);

    f += (strlen(f)-temp);
    temp = strlen(f);

    if (mediafile)
		sprintf(f, "mediafile=%s\n", (const char *)mediafile);

    f += (strlen(f)-temp);
    temp = strlen(f);

    if (recordtime)
		sprintf(f, "recordtime=%d\n", recordtime);

    f += (strlen(f)-temp);
    temp = strlen(f);

    if (sortIndex)
		sprintf(f, "sortindex=%d\n", sortIndex);

    f += (strlen(f)-temp);
    temp = strlen(f);

    sprintf(f, "format=%s\n", (const char *)formatName);

    f += (strlen(f)-temp);
   

	writeNameValuesNetwork(f);

    f = original;
   
    
    return 0;
}



int Media::deleteFiles()
{
	// delete them in order of recoverability.
	if (mediafile)
		b_delete_file(mediafilePath());
	if (b_delete_file(infofile)) {
		BDBG_ERR(("Delete %s failed", (const char*)infofile));
		return -1;
	}
	return 0;
}

int64_t Media::size()
{
	int64_t size;
	return b_get_file_size(mediafilePath(), &size) ? 0 : size;
}

int Media::length()
{
	//TODO:
	return 0;
}

void Media::addPath(MString &fullpath, const char *filename)
{
	if (filename && filename[0] == '/')
		fullpath = filename; /* absolute path */
	else
		fullpath = _dirpath + filename;
}

const char *Media::mediafilePath()
{
	/* compute once */
	if (!_mediafilePath) {
		addPath(_mediafilePath, mediafile);
	}
	return _mediafilePath;
}

static struct {
	const char *name; /* name used in infofile */
	const char *mimeType;
	Media::Type type;
} g_formatData[] = {
	{"WAV",	"audio/x-wav",	Media::eAudio},
	{"MP3",	"audio/mpeg",	Media::eAudio},
	{"MP2",	"audio/mp2",	Media::eAudio},
	{"AC3",	"audio/ac3",	Media::eAudio},
	{"JPEG","image/jpeg",	Media::ePicture},
	{"BMP",	"image/bmp",	Media::ePicture},
	{"PNG",	"image/png",	Media::ePicture},
	{"GIF",	"image/gif",	Media::ePicture}
};

int Media::setFormat(const char *aFormatName)
{
	if (aFormatName)
	for (unsigned i=0;i<sizeof(g_formatData)/sizeof(g_formatData[0]);i++) {
		if (!strcasecmp(aFormatName, g_formatData[i].name)) {
			formatName = aFormatName;
			formatNumber = 0;
			mimeType = g_formatData[i].mimeType;
			type = g_formatData[i].type;
			return 0;
		}
	}
	return -1;
}

void Media::setPath(const char *path)
{
	_dirpath = path;
	_mediafilePath = NULL; /* clear */
}

//////////////////////////////////////////

int b_print_size(char *buf, int length, int64_t sz, bool shortname)
{
	int num = sz, dec = 0;
	const char *sizeunits = (shortname?"B":"Bytes");
	if (sz > 1024) {
		if (sz < 1024*1024) {
			sz = (sz * 100) / 1024;
			sizeunits = (shortname?"KB":"Kilobytes");
		} else {
			sz /= 1024; // now its in KB
			if (sz < 1024*1024) {
				sz = (sz * 100) / 1024;
				sizeunits = (shortname?"MB":"Megabytes");
			} else {
				sz /= 1024; // now its in MB
				sz = (sz * 100) / 1024;
				sizeunits = (shortname?"GB":"Gigabytes");
			}
		}
		num = sz / 100;
		dec = (sz % 100) / 10; /* start with two digits of precision and turn it into one */
		if (((sz % 10) >= 5) && (++dec > 9)) { num++; dec=0; } /* round up */
    }
	return BKNI_Snprintf(buf, length, "%d.%d %s", num, dec, sizeunits);
}

int b_print_time_period(char *buf, int length, int msec, int format)
{
	int len = 0;
	int seconds = msec / 1000;
	int hours = seconds / 3600;
	int minutes = (seconds % 3600) / 60;
	seconds = seconds % 60;

	switch (format) {
	case B_TIME_PERIOD_FORMAT_LONG:
		{
		MString s;
		// this prevents lots of reallocs
		s.allocate(100);
		if (hours) {
			s = MString(hours) + " hour";
			if (hours != 1)
				s += "s";
		}
		if (minutes) {
			if (!s.isNull())
				s += ", ";
			s += MString(minutes) + " minute";
			if (minutes != 1)
				s += "s";
		}
		if (seconds && !hours && minutes < 5) {
			if (!s.isNull())
				s += ", ";
			s += MString(seconds) + " second";
			if (seconds != 1)
				s += "s";
		}
		if (s.isNull())
			s = "0";
		strncpy(buf, s, length);
		len = strlen(buf);
		}
		break;
	case B_TIME_PERIOD_FORMAT_COMPACT:
		{
		if (hours)
			len = BKNI_Snprintf(buf, length, "%d:%02d:%02d", hours, minutes, seconds);
		else
			len = BKNI_Snprintf(buf, length, "%d:%02d", minutes, seconds);
		}
		break;
	}

	return len;
}

int b_print_calendar_time(char *buf, int length, time_t time)
{
	static char *month[] = {
		(char *)"Jan",
		(char *)"Feb",
		(char *)"Mar",
		(char *)"Apr",
		(char *)"May",
		(char *)"Jun",
		(char *)"Jul",
		(char *)"Aug",
		(char *)"Sep",
		(char *)"Oct",
		(char *)"Nov",
		(char *)"Dec"};
	// I don't like strftime because day and hour are zero padded!
	//strftime(buf, 512, "%b %d, %Y  %I:%M %P", gmtime(&recordtime));
#ifndef _WIN32_WCE
 	struct tm *t = gmtime(&time);
	if (t)
		return BKNI_Snprintf(buf, length, "%s %d, %d %d:%02d %s",
			month[t->tm_mon],
			t->tm_mday,
			t->tm_year+1900,
			(t->tm_hour==0)?12:t->tm_hour%12,
			t->tm_min,
			(t->tm_hour>12)?"pm":"am");
	
	strncpy(buf, "Unknown", length);
	return strlen(buf);
#else

    SYSTEMTIME sysTime;
    GetSystemTime(&sysTime);
    return BKNI_Snprintf(buf, length,"%s %d, %d %d:%02d %s",
	    month[sysTime.wMonth],
	    sysTime.wDay,
	    sysTime.wYear,
		(sysTime.wHour==0)?12:sysTime.wHour%12,
	    sysTime.wMinute,
	    (sysTime.wHour>12)?"pm":"am"
	    );

#endif
}



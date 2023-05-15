/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: xmltags.h $
* $brcm_Revision: 8 $
* $brcm_Date: 5/15/12 4:52p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/xmltags.h $
* 
* 8   5/15/12 4:52p jrubio
* SW7346-644: add info file attributes
* 
* 7   4/13/12 11:47a jrubio
* SW7346-644: add pid description tag, rename Info File Tag to <infofile>
* 
* 6   4/11/12 1:18p jrubio
* SW7346-644: add tag names for playback info file
* 
* 5   3/12/12 9:02p tokushig
* SW7405-5581: fix problem with out of sync codec to string conversion.
* changed conversion of av types code to use hash tables instead of enum
* based array mappings to avoid out of sync lookup problems that can
* occur when NEXUS_AudioCodec and NEXUS_VideoCodec enums do not match
* lookup array mappings.  if a new nexus enum is added, existing lookup
* will still work...lookups involving the new enum value will map to
* unknown.  if a nexus enum is removed, the compiler will catch the
* inconsistency.
* 
* 4   3/12/12 4:42p tokushig
* SW7405-5581: after channel load, unnumbered channels will be auto-
* numbered starting with the last valid channel number in the channel
* list.  channel numbers can not be saved/restored in channel list
* save/load.  moved code to load channels from xml to the appropriate
* channel classes.
* 
* 3   3/9/12 3:23p jrubio
* SW7346-644: add new tags
* 
* 2   3/7/12 10:11a jrubio
* SW7346-644: add XML Max define
* 
* 1   2/29/12 2:13p jrubio
* SW7346-644: add xml names
* 
* 
*********************************************************************************/
#ifndef XMLTAGS_H__
#define XMLTAGS_H__


#ifdef __cplusplus
extern "C" {
#endif

#define XML_MAX_LENGTH 12

//xml tags
#define XML_TAG_CHANNEL                  "channel"
#define XML_TAG_CHANNELLIST              "channellist"
#define XML_TAG_PID                      "pid"
#define XML_TAG_INFOFILE                 "infofile"

/*XML Atrributes */
#define XML_ATT_MAJOR                    "major"
#define XML_ATT_MINOR                    "minor"
#define XML_ATT_TYPE                     "type"
#define XML_ATT_BANDWIDTH                "bandwidth"
#define XML_ATT_MODE                     "mode"
#define XML_ATT_FREQ                     "freq"
#define XML_ATT_SYMRATE                  "symrate"
#define XML_ATT_DISEQC                   "diseqc"
#define XML_ATT_TONE                     "tone"

#define XML_ATT_ANNEX                    "annex"

#define  XML_ATT_VIDEOPID                "videopid"
#define  XML_ATT_AUDIOPID                "audiopid"
#define  XML_ATT_PCRPID                  "pcrpid"
#define  XML_ATT_VIDEOTYPE               "videotype"
#define  XML_ATT_AUDIOTYPE               "audiotype"
#define  XML_ATT_ANCILLARYPID            "ancillarypid"

#define  XML_ATT_FILENAME                "filename"
#define  XML_ATT_INDEXNAME               "indexname"
#define  XML_ATT_PATH                    "path"
#define  XML_ATT_DESCRIPTION             "description"

#ifdef __cplusplus
}
#endif

#endif /* XMLTAGS_H__ */

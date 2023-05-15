/***************************************************************************
 *     Copyright (c) 2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: ipstreamer.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 10/11/06 3:51p $
 *
 * Module Description:
 *      IP Streamer h file.
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/ipstreamer.h $
 * 
 * 2   10/11/06 3:51p jrubio
 * PR24672: IPStreamer lib now resides in BSEAV/lib
 * 
 * 1   10/2/06 10:38a jrubio
 * PR24672:IP Streamer support
 * 
 *
 *************************************************************/
#ifndef IPSTREAMER_H__
#define IPSTREAMER_H__

#include "bsettop.h"
#include "streamer.h"
class MStringList;

/**
Summary:
 Process ip streamer map file and start IP Streamer service
*/
class IPStreamer  {
public:

    IPStreamer();

    ~IPStreamer();

    /**
	Summary:
    Return the total number of streames in the ip streamer
    file list.
	**/
	int totalStreams() const {return _totalstreams;}

    /**
     * Does all the work for streaming
     * @param char * name
     */
	int ControlIPStreaming(char * name);

    char * getFilename() {return _filename;}

    void setFilename(char * name) { _filename = name;}

     /* IP Streamer structure*/
    typedef struct ipstreamer_t {
        streamer_t * str;
        b_task_t task_handle;
        bool active;
    }ipstreamer_t;

protected:

   
    ipstreamer_t _ipstreamer_list[3]; /* Hard Code for now*/
    int _totalstreams;
    char * _filename;
};

#endif //IPSTREAMER_H__

/***************************************************************************
 *     Copyright (c) 2003-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: ipstreamer.cpp $
 * $brcm_Revision: 3 $
 * $brcm_Date: 12/29/06 4:43p $
 *
 * Module Description:
 *     Process ipstr.txt and setup IP Streamer
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/ipstreamer.cpp $
 * 
 * 3   12/29/06 4:43p erickson
 * PR26813: b_start_task now requires b_task_params, which should be
 * initialized with b_task_params_init
 * 
 * 2   10/11/06 3:51p jrubio
 * PR24672: IPStreamer lib now resides in BSEAV/lib
 * 
 * 1   10/2/06 10:42a jrubio
 * PR24672: IP Streamer Support
 * 
 *
 *************************************************************/

#include "brutus_app.h"
#include "bsettop_os.h"
#include "brutus_os.h"
#include "bstd.h"
#include "string.h"
#include "mediatypes.h"
#include <stdio.h>
#include <mstringlist.h>
#include "ipstreamer.h"
#include "streamer.h"

#define MAX_STR_LENGTH 128
BDBG_MODULE(ipstreamer);



IPStreamer::IPStreamer()
{
    int i =0;

    for (i=0;i<3;i++) 
    {
        _ipstreamer_list[i].str = NULL;
        _ipstreamer_list[i].active = false;
        _ipstreamer_list[i].task_handle= NULL;
    }
    _totalstreams =0;

}

IPStreamer::~IPStreamer()
{
    int i = 0;

    BDBG_MSG(("Cleaning up IPStreamer"));
    for (i=0;i<3;i++) 
    {
        if(_ipstreamer_list[i].active)
        {
            BDBG_MSG(("Stopping number %d stream",i));
            streamer_stop(_ipstreamer_list[i].str);
            b_stop_task(_ipstreamer_list[i].task_handle);
            BKNI_Free(_ipstreamer_list[i].str);
        }
    }
    
}


/* borrow some processing from trinity pvr directory, so we do not have to 
   compile in other objects we do not need*/

static void strip_linefeed(char * str) {
    char * ptr3;
    if ( (ptr3 = strchr(str, '\n')) != NULL) {
	*ptr3 = 0;
    }
}
static int indexExist(streamer_t * pb) {
	char index_fname[256];
	struct stat  stats;
	
	strcpy(index_fname,pb->filename);
	strcat(index_fname,".sct");
	
	if (stat(index_fname, &stats) <0 ) {
		return 0;
	}
	return 1;
}

static void streamer_init(streamer_t * str)
{
    str->fd = -1;
	str->ch =  0;
    str->use_mmap = 0;
	str->kernel_mode = 1;
	str->port = 5000;
	str->protocol = 0;
	str->multicast = 1;
    str->state = _stop;
	sprintf(str->dest,"225.0.0.%d",str->ch+7);
}

static streamer_t *  process_info(const char * name) 
{
    streamer_t * str = NULL;
    FILE * fp;
    static char line[MAX_STR_LENGTH];
    char * ptr1, * ptr2;
    int len;
    
    if ((str = (streamer_t * )BKNI_Malloc(sizeof(struct streamer_t))) == NULL) 
    {
        BDBG_ERR(("%s malloc failed", __FUNCTION__));
        goto err0;
    }
    BKNI_Memset(str, 0, sizeof(struct streamer_t));
    
    if (name == NULL) {
	    /* generate an empty pvr */
	    return str;
    }
    streamer_init(str);

    /* pvr->IndexFormat = bindex_format_bcm; */

    /* parse the info file */
    if ((fp = fopen(name, "rt")) == NULL) 
    {
        BDBG_ERR(("%s cannot open %s", __FUNCTION__, name));
        goto err1;
    }
    ptr1 = &line[0];
    
    while (fgets(line, MAX_STR_LENGTH, fp)) {
	if ( (ptr2 = strchr(line, '=')) == NULL) {
	    continue;
	}
	*ptr2++ = 0;
	if (!strcasecmp(ptr1, "indexfile")) {
	    strip_linefeed(ptr2);
	    sprintf(str->indexfile, "videos/%s", ptr2); 
	}
	else if (!strcasecmp(ptr1, "mediafile")) {
	    strip_linefeed(ptr2);
	    sprintf(str->filename, "videos/%s",  ptr2); 
	}
	else if (!strcasecmp(ptr1, "vidpid"))
	    str->videopid = strtoul(ptr2, NULL, 0);
	else if (!strcasecmp(ptr1, "format")) {
	    strip_linefeed(ptr2);
	    if (!strcasecmp(ptr2, "AVC_TS")) {
            BDBG_MSG((" Format not needed right now"));
		    /* pvr->mpeg.video[0].format = bvideo_codec_h264; */
	    }		    
	    else if (!strcasecmp(ptr2, "TS")) {
            BDBG_MSG((" Format not needed right now"));
		    /* pvr->mpeg.video[0].format = bvideo_codec_mpeg2;*/
	    }  else {
		    BDBG_ERR(("Unsupported format %s\n",ptr2));
	    } 
	}
	else if (!strcasecmp(ptr1, "audpid")) {
	    int i = 0;
        BDBG_MSG(("Audio PID not needed right now "));
        /*
        while (*ptr2) {
        pvr->mpeg.audio[i++].pid = strtoul(ptr2, (char **)&ptr2, 0);
		if (*ptr2) ptr2++; /* if not terminated, assume it's a single comma 
	    }*/
	}
/* TODO  Remove code or streamer must accept these inputs*/
#if 0
    else if (!strcasecmp(ptr1, "audtype")) {
		int i = 0;

        BDBG_MSG(("Audio PID not needed right now "));

		while (*ptr2) {
			pvr->mpeg.audio[i++].format = (baudio_format)strtoul(ptr2, (char **)&ptr2, 0);
			if (*ptr2) ptr2++; /* if not terminated, assume it's a single comma */
		}

	}
	else if (!strcasecmp(ptr1, "pcrpid"))
	    pvr->mpeg.pcr_pid = strtoul(ptr2, NULL, 0);
	else if (!strcasecmp(ptr1, "name")) {
	    strip_linefeed(ptr2);
	    strncpy(pvr->name, ptr2, MAX_STR_LENGTH); 
	}
	else if (!strcasecmp(ptr1, "description")) {
	    strip_linefeed(ptr2);
	    strncpy(pvr->description, ptr2, MAX_STR_LENGTH); 
	}
#endif
	else {
	    BDBG_MSG(("unknown name:val %s %s", ptr1, ptr2));
	}
    }


    fclose(fp);
    return str;
 err1:
    BKNI_Free(str);
 err0:
    return NULL;
}

static void *startIPStreamer_from_task(void *data)
{
    void * makehappy = NULL;

    /* Create index file if none exists */
    if (!indexExist(((IPStreamer::ipstreamer_t *)data)->str)) {
		    BDBG_MSG(("index file does not exist, generating now\n"));
		    stream_create_index(((IPStreamer::ipstreamer_t *)data)->str);
		    BDBG_MSG(("index file generated\n"));
	}

    /* start call to streamer_start This will be in this thread*/
    streamer_start(((IPStreamer::ipstreamer_t *)data)->str);
    return makehappy ;
}

int IPStreamer::ControlIPStreaming(char *filename) {
	MStringList list;
    char info_file[128];
    
    if (!readFile(filename, list))
    {
        BDBG_ERR(("READFILE ERROR on %s",filename));
        return -1;
    }

	for (const char *s = list.first(); s; s = list.next()) {
		/* check for line comment */
		char *comment = strchr(s, '#');
		if (comment)
			*comment = 0;

		MStringList list;
		MStringList::split(s, " \t", list);

		if (!list.total()) {
			continue;
		}
		else if ( (list.total() == 3) && _totalstreams < 3) {
			strncpy(info_file, list[2], sizeof(info_file) );
			BDBG_MSG(("info file is %s", info_file));
			streamer_t * ipstr_prms = NULL;
			if ((ipstr_prms = process_info(info_file))) {
				b_task_params task_params;
				b_task_params_init(&task_params);
				strncpy(ipstr_prms->dest,list[0],16);
				ipstr_prms->port = atoi(list[1]);
				ipstr_prms->ch = _totalstreams;
				_ipstreamer_list[_totalstreams].active=true;
				_ipstreamer_list[_totalstreams].str = ipstr_prms;
				BDBG_MSG(("IP Address\t %s\n port number\t %d",ipstr_prms->dest,ipstr_prms->port)); 
				b_start_task(&_ipstreamer_list[_totalstreams].task_handle,&task_params,startIPStreamer_from_task,(void *)&_ipstreamer_list[_totalstreams]);
			}
			else {
				BDBG_ERR(("Streamer could not be started with "
				"info file %s",info_file));
				continue;
			}
		}
		else {
			BDBG_ERR(("Already maxed out on IP Streams that we can Stream!"));
		}
		
		_totalstreams++;
	}

    /* Done starting streamer's wait now.*/

    BDBG_MSG((" Total Streams %d, exit ContolIPStreaming",_totalstreams));
	return 0;
}

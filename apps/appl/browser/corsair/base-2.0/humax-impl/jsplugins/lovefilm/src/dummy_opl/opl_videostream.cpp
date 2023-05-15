/*
 *
 *
 *
 *
 */

#include "opl_videostream.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

OplVideoStream *opl_videostream_get_dummy()
{
	OplVideoStream *as = (OplVideoStream *)malloc(sizeof(*as));
	if ( !as ) return NULL;

	strcpy(as->streamType, "VideoStream");
	as->isDownloading = true;
	as->isActive = true;
	as->downloadingFragmentIndex = 5;
	as->playingFragmentIndex = 2;
	strcpy(as->serverIpAddr, "10.0.13.35");
	as->bytesStreamed = 1024;

	return as;
}

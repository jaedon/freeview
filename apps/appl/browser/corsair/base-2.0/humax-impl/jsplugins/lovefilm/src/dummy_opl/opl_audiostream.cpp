/*
 *
 *
 *
 *
 */

#include "opl_audiostream.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

OplAudioStream *opl_audiostream_get_dummy()
{
	OplAudioStream *as = (OplAudioStream *)malloc(sizeof(*as));
	if ( !as ) return NULL;

	strcpy(as->streamType, "AudioStream");
	as->isDownloading = true;
	as->isActive = true;
	as->downloadingFragmentIndex = 5;
	as->playingFragmentIndex = 2;
	strcpy(as->serverIpAddr, "10.0.13.35");
	as->bytesStreamed = 1024;

	return as;
}

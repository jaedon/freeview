/*
 *
 *
 *
 *
 */

#include "opl_player.h"
#include <stdio.h>

OplPlayer::OplPlayer()
:m_listener(NULL), m_url(NULL)
{
}

OplPlayer::~OplPlayer()
{
}

void OplPlayer::setListener(OplPlayerListener_t listener)
{
	m_listener = listener;
}

void OplPlayer::setAssetUrl(const char * url)
{
	m_url = (char *)url;
}

void OplPlayer::setDrmConfig(const char * drmType)
{
	printf("OplPlayer::setDrmConfig : %s\n", drmType);
}

void OplPlayer::setBandwidthMultiplier(int multiplier)
{
	printf( "OplPlayer::setBandwidthMultiplier : %d\n", multiplier );
}

void OplPlayer::setPlayReadyLicense(const char * license)
{
	printf("OplPlayer::setPlayReadyLicense : %s\n", license);
}

void OplPlayer::play()
{
}

void OplPlayer::pause()
{
}

void OplPlayer::seek(int time)
{
}

void OplPlayer::stop()
{
}

void OplPlayer::unload()
{
}

void OplPlayer::setPlaybackSpeed(int speed)
{
}

void OplPlayer::setBufferTimeRequiredForInitialPlayback(int time)
{
}

void OplPlayer::setBufferTimeRequiredAfterExhaustion(int time)
{
}

void OplPlayer::setBufferTimeRequiredAfterStreamSwitch(int time)
{
}

void OplPlayer::setBufferTimeRequiredAfterSeeking(int time)
{
}

void OplPlayer::setFragmentsDownloadedEventFrequency(int frequency)
{
}

void OplPlayer::setPosition(int x, int y)
{
}

void OplPlayer::setSize(int width, int height)
{
}
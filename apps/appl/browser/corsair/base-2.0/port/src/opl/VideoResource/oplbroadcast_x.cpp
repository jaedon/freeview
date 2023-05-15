/**************************************************************************************/
/**
 * @file bplbroadcast.cpp
 *
 * Video Broadcast
 *
 **************************************************************************************
 **/

#include "oplbroadcast.h"

#include <hlib.h>


/******************************************************************
	class OplVideoBroadcast
******************************************************************/
OPL_Channel_t	OplVideoBroadcast::getCurrentChannel()
{
	return OPL_Broadcast_GetCurrentChannel(m_vbcastHandle);
}

OPL_Channel_t	OplVideoBroadcast::getLatestChannel()
{
	return OPL_Broadcast_GetLatestChannel(m_vbcastHandle);
}


int OplVideoBroadcast::getAVOutputsCount(void)
{
	return 0;
}

OplAVOutput *OplVideoBroadcast::getAVOutputByIndex(int nIndex)
{
	OplAVOutput *output = new OplAVOutput();
	if ( !output )
	{
		return NULL;
	}

	/* TODO: */

	return output;
}

OplRecorder *OplVideoBroadcast::getRecorder()
{
	if ( m_recorder ) return m_recorder;

	m_recorder = new OplRecorder();
	return m_recorder;
}

int OplVideoBroadcast::getPlaySpeedCount()
{
	int count = 0;
	int *speeds = NULL;
	if ( OPL_Broadcast_GetPlaySpeeds(m_vbcastHandle, &speeds, &count) != BPL_STATUS_OK )
	{
		return -1;
	}

	HLIB_STD_MemFree(speeds);
	return count;
}

bool OplVideoBroadcast::getPlaySpeeds(int **speeds_out)
{
	int count = 0;
	int *speeds = NULL;
	if ( OPL_Broadcast_GetPlaySpeeds(m_vbcastHandle, &speeds, &count) != BPL_STATUS_OK )
	{
		return false;
	}

	*speeds_out = new int[count];
	if ( !*speeds_out ) return false;

	for ( int i=0; i<count; ++i )
		(*speeds_out)[i] = speeds[i];

	HLIB_STD_MemFree(speeds);
	return true;
}
OPL_Channel_t	OplVideoBroadcast::createChannelObject(unsigned short  nIdType, int onid, int tsid, int sid)
{
	return OPL_Broadcast_CreateChannelObjectByTripleID(m_vbcastHandle, onid, tsid, sid);
}
#if 1 // HUMAX EXTENSION
OPL_Channel_t	OplVideoBroadcast::createChannelObject(unsigned short  nIdType, unsigned short usMajorChannel, unsigned short usMinorChannel)
{
	return OPL_Broadcast_CreateChannelObject(m_vbcastHandle, usMajorChannel, usMinorChannel);
}
#endif


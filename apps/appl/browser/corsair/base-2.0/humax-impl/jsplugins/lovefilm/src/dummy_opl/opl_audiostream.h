/*
 *
 *
 *
 *
 */

#ifndef __OPL_AUDIO_STREAM_H__
#define __OPL_AUDIO_STREAM_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	char	streamType[32];
	bool	isDownloading;
	bool	isActive;
	unsigned int	downloadingFragmentIndex;
	unsigned int	playingFragmentIndex;
	char	serverIpAddr[32];
	unsigned int	bytesStreamed;
} OplAudioStream;

OplAudioStream *opl_audiostream_get_dummy();

#ifdef __cplusplus
}
#endif

#endif

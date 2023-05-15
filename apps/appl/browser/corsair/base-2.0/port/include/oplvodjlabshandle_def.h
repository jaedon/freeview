
#ifndef __OPLVODJLABSHANDLE_DEF_H__
#define __OPLVODJLABSHANDLE_DEF_H__

typedef struct {
	int svcuid;
	unsigned short eventId;
	int parentalRating;
	char *name;
	char *description;
	char *longDescription;
	unsigned int startTime;
	unsigned int duration;
	bool hasRecording;
	bool subtitles;
	bool isHD;
	unsigned short content;
	int digitalCopyControl;
	int analogCopyControl;
	bool is3D;
	bool isCharged;
	char *contractVertificationInfo;
	int recordingStatus;
} OplVodProgramme_t;

#endif // __OPLVODJLABSHANDLE_DEF_H__
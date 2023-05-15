/**************************************************************************************/
/**
 * @file oplrecording.h
 *
 *
 *
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#ifndef __OPLRECORDING_H__
#define __OPLRECORDING_H__

#include	"opltype.h"
#include	"oplscheduledprogramme.h"
#include "oplbookmark.h"

#ifdef __cplusplus
extern "C" {
#endif	//__cplusplus

typedef enum {
	OPLRECORDING_SHOWTYPE_LIVE		= 0,
	OPLRECORDING_SHOWTYPE_FIRST_RUN = 1,
	OPLRECORDING_SHOWTYPE_RERUN		= 2
} eOplRecordingShowType;

typedef enum {
	OPLRECORDING_CASTYPE_FTA			= 0,
	OPLRECORDING_CASTYPE_NAGRA			= 1,
	OPLRECORDING_CASTYPE_IRDETO			= 2,
	OPLRECORDING_CASTYPE_NDS			= 3,
	OPLRECORDING_CASTYPE_VIACCESS		= 4,
	OPLRECORDING_CASTYPE_CONAX			= 5,
	OPLRECORDING_CASTYPE_MEDIAGUARD		= 6,
	OPLRECORDING_CASTYPE_CRYPTOWORKS	= 7,
	OPLRECORDING_CASTYPE_JPBCAS			= 10,
	OPLRECORDING_CASTYPE_JPCCAS			= 11,
	OPLRECORDING_CASTYPE_ALLCAS			= 12,
	OPLRECORDING_CASTYPE_VERIMATRIX		= 13
} eOplRecordingCasType;

typedef enum {
	OPLRECORDING_COPYCONTROL_COPYOK				= 0,
	OPLRECORDING_COPYCONTROL_NOFURTHERCOPY		= 1,
	OPLRECORDING_COPYCONTROL_COPYONCE			= 2,
	OPLRECORDING_COPYCONTROL_COPYPROHIBITED		= 3
} eOplRecordingCopyControlType;

#define OPLRECORDING_MAX_NAME_LEN			(64)
#define OPLRECORDING_MAX_ID_LEN 		   	(64)
#define OPLRECORDING_MAX_LONGNAME_LEN		(128)
#define OPLRECORDING_MAX_DESC_LEN			(512)
#define OPLRECORDING_MAX_LONGDESC_LEN		(2048)
#define OPLRECORDING_MAX_STRCOLLECTION_LEN		(512)
#define OPLRECORDING_MAX_COMPONENT_LEN		(64)

/******************************************************************
	Enum
 ******************************************************************/
typedef struct
{
	OPL_ScheduledItem_t	super;
	int					state;
	int					error;
	char				*id;
	OPL_BOOL			isManual;
	OPL_BOOL			doNotDelete;
	OPL_BOOL			willBeDeleted;
	int					saveDays;
	int					saveEpisodes;
	OPL_BOOL			blocked;
	int					showType;
	OPL_BOOL			subtitles;
	OPL_Collection_t*	subtitleLanguages;
	OPL_BOOL			isHD;
	OPL_BOOL			isWideScreen;
	int					audioType;
	OPL_BOOL			isMultilingual;
	OPL_Collection_t*	audioLanguages;
	OPL_Collection_t*	genres;
	unsigned long		recordingStartTime;
	unsigned long		recordingDuration;
	OPL_Collection_t*	bookmarks;
	OPL_BOOL			locked;
	int					lastPlayPosition;
	int					lcn;
	OPL_BOOL			isNotified;
	char				*thumbnail;
	OPL_Collection_t*	extEventInfo;
	OPL_Collection_t*	components;
	OPL_BOOL			isScrambled;
	OPL_BOOL			isEncrypted;
	int					copyControl;
	int					casType;
	unsigned int		expiryDate;
	OPL_Collection_t*	pausedPoints;


#if defined(CONFIG_OP_JAPAN)
	int					digitalCopyControl;
	int 				analogCopyControl;
	OPL_BOOL			isTranscoded;
	int					digitalCopyCount;
	int					parentalRating;		/* 시간 지정예약에서 첫번째 이벤트는 Rating이 없다가 두번째 Event에 Rating이 존재 할 경우에 대한 Stream 전체의 Rating 속성 관리 */
	OPL_BOOL			isWatched;
	OPL_Collection_t*	chapters;
#endif

	char				*guidance;
	unsigned long		doNotDeleteSetTime;

	OPL_Collection_t*	seriesCRIDs;
	char				*programmeCRID;
	char				*seriesTitle;
	OPL_Collection_t*	subtitletypes;
	OPL_BOOL			isManualLocked;
	char*				szPath;
	char*				szFileName;

	//1  주의, 워닝, 코션
	/**
	 * OPL_Recording_t에 * 형식의 필드를 추가할 때는 반드시 OPL_Recording_Delete와 OPL_Recording_Clone도
	 * 같이 구현해 주셔요~~
	 */

	/**
	 * js-plugin으로 data를 전달해 줄 때 메모리 누수 땜시, garbage 포인터를 하나 두고,
	 * 이 주소를 주도록 한다. (이게 뭔소린가... 하면, source insight reference 보셔요... ^^;)
	 */
	void 					*garbage;
} OPL_Recording_t;

OPL_Recording_t *	OPL_Recording_New (const char *signature, ...);
void				OPL_Recording_Delete (OPL_Recording_t *self);
OPL_Recording_t *	OPL_Recording_Clone (const OPL_Recording_t *self);
OPL_Recording_t *	OPL_Recording_ScheduledToRecording_Clone (const OPL_ScheduledItem_t *self);
OPL_Recording_t *	OPL_Recording_ScheduledAndRecording_Clone (const OPL_ScheduledItem_t *scheduleditem, const OPL_Recording_t *recordingitem);

OPL_Recording_t*	OPL_Recording_GetUpdatableData (OPL_Recording_t *recording, OPL_HANDLE recDataHandle);

OPL_Bookmark_t*		OPL_Recording_Bookmark_New (unsigned long	time, const char *name);
void				OPL_Recording_Bookmark_Delete (OPL_Bookmark_t *self);
OPL_Bookmark_t *	OPL_Recording_Bookmark_Clone (const OPL_Bookmark_t *self);

OPL_Bookmark_t*		OPL_Recording_Bookmark_Add(OPL_HANDLE	recHandle, unsigned int time , const char *name);
void				OPL_Recording_Bookmark_Remove(OPL_HANDLE	recHandle, OPL_HANDLE	bookmarkHandle);

void *			OPL_Recording_PutGarbage (OPL_Recording_t *recording, void *garbage);

char*			OPL_Recording_HasAudioDescription(OPL_Recording_t *recording);
char*			OPL_Recording_HasSigned(OPL_Recording_t *recording);
char*			OPL_Recording_HasAC3(OPL_Recording_t *recording);
char*			OPL_Recording_GetThumbnail(OPL_Recording_t *recording);
int				OPL_Recording_GetCasIDs(OPL_Recording_t *recording, eOplRecordingCasType **casIDs);

#ifdef __cplusplus
};
#endif	//__cplusplus

#endif	//__OPLRECORDING_H__


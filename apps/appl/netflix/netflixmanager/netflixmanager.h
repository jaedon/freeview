/**************************************************************
 *	@file		netflixmanager.h
 *	Netflix Manager
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2015/03/04
 *	@author			humax
 *	@brief			Netflix Manager
 **************************************************************/
#ifndef ____NETFLIX_MANAGER_H_
#define ____NETFLIX_MANAGER_H_

#define	MAX_ENCORDED_URL_LEN		4352	// 4Kbytes + 256bytes

typedef enum {
	Source_NONE,
	Source_NETFLIX_BUTTON				= 1,
	Source_DEDICATED_ON_SCREEN_ICON		= 2,
	Source_ITEM_IN_APPLICATION_LIST		= 3,
	Source_SEARCH_RESULT				= 4,
	Source_SEARCH_CONTINUATION			= 5,
	Source_WEB_BROWSER					= 6,
	Source_LIVE_FOLDER					= 7,
	Source_EXTERNAL_APP					= 8,
	Source_iBANNER_AD					= 9,
	Source_EXTERNAL_CONTROL_PROTOCOL	= 10,
	Source_META_DISCOVERY				= 11,
	Source_DIAL							= 12,
	Source_VOICE_CONTROL				= 13,
	Source_VISUAL_GESTURE				= 14,
	Source_TOUCH_GESTURE				= 15,
	Source_VIRTUAL_REMOTE				= 16,
	Source_EPG_GRID						= 17,
	Source_CHANNEL_NUMBER				= 18,
	Source_POWER_ON_FROM_NETFLIX_BUTTON = 19,
	Source_DIAL_3RD_PARTY				= 20,
	Source_TITLE_RECOMMEND				= 21,
	Source_SUSPENDED_AT_POWER_ON		= 22,
	Source_SUSPENDED_AFTER_APP_RESTART	= 23,
	Source_SUSPENDED_CHANNEL_SURF		= 25,
	Source_SUSPENDED_CHANNEL_INFO_BAR	= 26,
	Source_NETFLIX_PRE_APP				= 27,
	Source_PRE_APP_ICON					= 28,
	Source_PROMO						= 29,
	Source_OS_PRE_APP					= 30,
	Source_TESTING						= 99
} NFLIX_Launch_Source_Type_t;

typedef enum {
	Reason_NONE,
	Reason_USER_Kill					= 1,	// User requests Netflix application to self-terminate
	Reason_USER_Nav						= 2,	// User navigated away from application (ex: press Home)
	Reason_DIAL_Stop					= 3,	// DIAL issuing 'stop' to application
	Reason_AM_LowResource				= 4,	// AM terminated application due to low resources
	Reason_STANDBY_ON					= 5,	// Enter the standby mode	
	Reason_FACTORY_RESET				= 6,	// Factory reset
	Reason_DIAL_Other_Launch			= 7,
	Reason_Unknown						= 98,	// Used if AM cannot relay the above enums
	Reason_Testing						= 99,	// Can be used during testing/development phase
	Reason_LAST_ENTRY_POSITION
}NFLIX_Exit_Reason_Type_t;


typedef enum {
	NFLIX_NOTIFY_EXIT_NORMAL = 0,
	NFLIX_NOTIFY_START_NORMAL = 1,
	NFLIX_NOTIFY_EXIT_STANDBY = 7,
	NFLIX_NOTIFY_EXIT_CH_UP,
	NFLIX_NOTIFY_EXIT_CH_DOWN,
	NFLIX_NOTIFY_EXIT_DIAL,
	NFLIX_NOTIFY_EXIT_DIAL_OTHER_LAUNCH,
	NFLIX_NOTIFY_EXIT_EPG,
	NFLIX_NOTIFY_EXIT_MY_PAGE,
	NFLIX_NOTIFY_TRYTO_EXIT = 20,
	NFLIX_NOTIFY_ACTIVATED = 30,
	NFLIX_NOTIFY_DEACTIVATED = 31,
	NFLIX_NOTIFY_UNDEFINED
}NFLIX_NotifyState_t;

typedef enum {
	NFLIX_OVERLAY_POPUP_WITH_KEY,		// 0
	NFLIX_OVERLAY_POPUP_WITHOUT_KEY,	// 1
	NFLIX_OVERLAY_CLOSE_WITH_EXIT,		// 2
	NFLIX_OVERLAY_CLOSE_WITHOUT_EXIT,	// 3
	NFLIX_OVERLAY_NA
}NFLIX_Overlay_Ctrl_t;

typedef enum{
	Category_BASIC_SD,
	Category_BASIC_HD,
	Category_MOVIES_SD,
	Category_MOVIES_HD,
	Category_CHILDREN_SD,
	Category_CHILDREN_HD,
	Category_LEARNING_SD,
	Category_LEARNING_HD,
	Category_INDIE_SD,
	Category_INDIE_HD,
	Category_OTHER,
	Category_MAX
}NFLIX_Category_t;

typedef struct {
	NFLIX_Launch_Source_Type_t		enSrc;
	unsigned char						szURL[MAX_ENCORDED_URL_LEN];
	unsigned char					szTrackURL[MAX_ENCORDED_URL_LEN]; // TrackURL or AdditionalData URL
	unsigned int					ulTrackId;
	unsigned int					ulChId;
	unsigned int					ulPrevChId;
	unsigned int					ulNextChId;
	NFLIX_Category_t				enCategory;
} Launch_Param_t;

//HAPI_SendSignalBinary(eHSIG_StartNetflixAppl, sizeof(NFLIX_Launch_Param_t), (HUINT8*)pstParam);
//HAPI_SendSignalNumber(eHSIG_StopNetflixAppl, ulExistReason);

#endif

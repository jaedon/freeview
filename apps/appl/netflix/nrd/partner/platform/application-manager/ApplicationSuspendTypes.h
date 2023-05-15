#ifndef _NF_APPLICATION_SUSPEND_TYPES_
#define _NF_APPLICATION_SUSPEND_TYPES_

enum Reason {
    Reason_USER_Kill                    = 1,    // User requests Netflix application to self-terminate
    Reason_USER_Nav                     = 2,    // User navigated away from application (ex: press Home)
    Reason_DIAL_Stop                    = 3,    // DIAL issuing 'stop' to application
    Reason_AM_LowResource               = 4,    // AM terminated application due to low resources
    Reason_Unknown                      = 98,   // Used if AM cannot relay the above enums
    Reason_Testing                      = 99,   // Can be used during testing/development phase
    Reason_LAST_ENTRY_POSITION
};

enum Source {
    Source_NETFLIX_BUTTON               = 1,
    Source_DEDICATED_ON_SCREEN_ICON     = 2,
    Source_ITEM_IN_APPLICATION_LIST     = 3,
    Source_SEARCH_RESULT                = 4,
    Source_SEARCH_CONTINUATION          = 5,
    Source_WEB_BROWSER                  = 6,
    Source_LIVE_FOLDER                  = 7,
    Source_EXTERNAL_APP                 = 8,
    Source_iBANNER_AD                   = 9,
    Source_EXTERNAL_CONTROL_PROTOCOL    = 10,
    Source_META_DISCOVERY               = 11,
    Source_DIAL                         = 12,
    Source_VOICE_CONTROL                = 13,
    Source_VISUAL_GESTURE               = 14,
    Source_TOUCH_GESTURE                = 15,
    Source_VIRTUAL_REMOTE               = 16,
    Source_EPG_GRID                     = 17,
    Source_CHANNEL_NUMBER               = 18,
    Source_POWER_ON_FROM_NETFLIX_BUTTON = 19,
    Source_DIAL_3RD_PARTY               = 20,
    Source_TITLE_RECOMMEND              = 21,
    Source_SUSPENDED_AT_POWER_ON        = 22,
    Source_SUSPENDED_AFTER_APP_RESTART  = 23,
    Source_TESTING                      = 99
};

#endif /* _NF_APPLICATION_SUSPEND_TYPES_ */


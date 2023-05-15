#ifndef RC_SERVER_H_
#define RC_SERVER_H_

#include <common.h>
#include <pl_common.h>
#include <netinet/in.h>


typedef enum {
	RC_Status_Paired,
	RC_Status_Unpaired,
} PL_RC_Status;

typedef PL_RC_Status 	(*RC_client_pair_cb)		(const char *pMsg, const char *pName);
typedef PL_RC_Status	(*RC_client_unpair_cb)		(const char *pMsg);
typedef void		 	(*RC_client_message_cb)	(const char *pMsg);
typedef void		 	(*RC_client_launch_app_cb)	(const char *pUrl);
typedef void		 	(*RC_set_friendly_name_cb)	(const char *pName);

struct RCClientCallbacks {
    RC_client_pair_cb 			pair_cb;
    RC_client_unpair_cb 		unpair_cb;
    RC_client_message_cb 		msg_cb;
	RC_client_launch_app_cb		launch_app_cb;
	RC_set_friendly_name_cb		set_friendly_name_cb;
};

#if 0
#define DIAL_MAX_PAYLOAD (4096)

typedef void * DIAL_run_t;
#endif

HERROR	PL_RC_start(HCHAR *pszFriendlyName, struct RCClientCallbacks *client_cb);

void 	PL_RC_stop(void);

HERROR 	PL_RC_SetApplicationURL(void);

HERROR	PL_RC_SetFriendlyName(const HCHAR *pszFriendlyName);

HERROR 	PL_RC_UnpairClient(void);

HERROR 	PL_RC_SetPowerStatus(HBOOL bOperating);

HERROR	PL_DIAL_register_client_cb(struct RCClientCallbacks *callbacks);


#endif  // RC_SERVER_H_
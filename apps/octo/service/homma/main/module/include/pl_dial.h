#ifndef DIAL_SERVER_H_
#define DIAL_SERVER_H_

#include <common.h>
#include <pl_common.h>
#include <netinet/in.h>

typedef enum {
	DIAL_Status_Stop,
	DIAL_Status_Running,
} PL_DIAL_Status;

#define DIAL_MAX_PAYLOAD (4096)

typedef void * DIAL_run_t;

typedef PL_DIAL_Status 	(*DIAL_app_start_cb)	(const char *app_name, const char * url, const char *args, size_t arglen);
typedef void 			(*DIAL_app_stop_cb)		(const char *app_name);
typedef PL_DIAL_Status 	(*DIAL_app_status_cb)	(const char *app_name, int* pCanStop);

struct DIALAppCallbacks {
    DIAL_app_start_cb start_cb;
    DIAL_app_stop_cb stop_cb;
    DIAL_app_status_cb status_cb;
};

HERROR	PL_DIAL_start(HCHAR *pszFriendlyName);
void 	PL_DIAL_stop(void);
HERROR 	PL_DIAL_SetApplicationURL(void);
HERROR	PL_DIAL_SetFriendlyName(const HCHAR *pszFriendlyName);
HERROR	PL_DIAL_registerAppHandler(struct DIALAppCallbacks *cbs);
HERROR	PL_DIAL_register_app(const char *app_name/*, struct DIALAppCallbacks *callbacks*/);
HERROR	PL_DIAL_unregister_app(const char *app_name);
HUINT32	PL_DIAL_get_port(void);
HERROR	PL_DIAL_update_state_app(const HCHAR *app_name, PL_DIAL_Status state);
HERROR	PL_DIAL_setCookie(const char * cookie);
HERROR PL_DIAL_GetFriendlyName(HCHAR * pFriendlyName, HUINT32 size);
HERROR PL_DIAL_GetAdditionalDataUrl(HCHAR * pAdditionalDataUrl, HUINT32 size, HCHAR * appName);

#endif  // DIAL_SERVER_H_

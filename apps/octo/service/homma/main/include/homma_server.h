#ifndef	__HOMMA_SERVER_H__
#define	__HOMMA_SERVER_H__

#define	________________Header_Files_______________________________
#include "homma_int.h"

#define ________________Typedef____________________________________
typedef	struct{
	HINT32				sock;
	HULONG				tid;
	HULONG				msgq;

	HUINT32				queries;

	HxList_t    		*notifiers;
	HINT32 				semaphore;
	HBOOL				flagFinish;
	HBOOL				finished;
} homma_session_t;

#define ________________Internal_Functions_________________________
HERROR session_send_to_msg(homma_session_t *session, HUINT32 requestId, const HCHAR *data, HINT32 len);

#define ________________Public_Interfaces__________________________
void    HOMMA_SERVER_Run( void );

#endif	//__HOMMA_SERVER_H__



#ifndef	__DAMA_SERVER_H__
#define	__DAMA_SERVER_H__


typedef struct
{
	HINT32	sock;
	HINT32	prior;
	HUINT32	reqId;
#if defined(CONFIG_DAMA_MULTITHREAD)
	HBOOL	forDummy;
	void	*pvQueuePnt;
	void	*pvInstPnt;
	HUINT32	 ulMaskValue;
#endif
	HUINT32	size;
	void * data;	// Don't free. Already Aloc with DAMA_Request_t( + req->size).
} DAMA_Request_t;


void	DAMA_SERVER_Run (void);
DAMA_Request_t *	DAMA_SERVER_Listen (void);
void				DAMA_SERVER_SendToMainThread(DAMA_Request_t *req);
HBOOL	DAMA_SERVER_Response (HINT32 sock, const void *data, HUINT32 size, HUINT32 reqId);
HUINT32		DAMA_SERVER_Broadcast (const void *data, HUINT32 size, HUINT32 reqId);
HUINT32		DAMA_SERVER_RemaindRequest(void);

#endif	//__DAMA_SERVER_H__


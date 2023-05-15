/*************************************************************
 * @file		hxsocket.h
 * @date		2012/03/13
 * @author		someone
 * @brief		HLIB Socket API

 * http://www.humaxdigital.com
 *************************************************************/

#define ___HEADER________________________________________________________________________________________

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <htype.h>
#include <sys/queue.h>

#include "vkernel.h"


#define ___DEFINE________________________________________________________________________________________

#define VK_RPC_DEBUG
#ifdef VK_RPC_DEBUG
#define PrintDebug(fmt, ...) 		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintError(fmt, ...)    	VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...)				VK_DBG_Print("(+)%s\n", __FUNCTION__)
#define PrintExit(...)				VK_DBG_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintDebug(fmt, ...)    	while (0) ((int (*)(char *, ...)) 0)
#define PrintError(fmt, ...)		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...) 			while (0) ((int (*)(char *, ...)) 0)
#define PrintExit(...)				while (0) ((int (*)(char *, ...)) 0)
#endif /* VK_RPC_DEBUG */

#define VKRPC_METHOD_MAGIC_CODE    0xcafebebe
#define VKRPC_METHOD_NAME_LEN    64
#define VKRPC_METHOD_SIGNATURE    "ilsb"

#define RPC_ERR(ret) {						\
		if(ret) {						\
			PrintError("ERROR %s,%d\n", __FUNCTION__, __LINE__); \
			goto rpc_err;				\
		}							\
	}

#define RPC_SEM_ERR(ret) {						\
		if(ret) {						\
			PrintError("ERROR %s,%d\n", __FUNCTION__, __LINE__); \
			goto sem_err;				\
		}							\
	}


#define ___TYPEDEF_______________________________________________________________________________________

typedef struct {
	HCHAR method_name[VKRPC_METHOD_NAME_LEN];
	HCHAR *method_sign;
	VkRPC_Func_t pfn;
	void *usrdata;
} vkrpc_method_t;

typedef struct {
	HUINT32 magic_code;
	HCHAR method_name[VKRPC_METHOD_NAME_LEN];
	HUINT32 method_type;
	HINT32 obj_num;
	VKRPC_OBJECT_T **obj;
	HUINT32 pkt_size;
} vkrpc_packet_t;

typedef struct {
	HUINT32 req_id;
	unsigned long done;
	VkRPC_Response_t pfn;
	void *usrdata;
} vkrpc_response_t;

typedef struct {
	HCHAR notifier_name[VKRPC_METHOD_NAME_LEN];
	HUINT32 clisock_hdl;	
	VkRPC_Noti_t pfn;
	void *pvData;
} vkrpc_notifier_t;
	
struct vkrpc_method_queue_t {
	vkrpc_method_t methods;
	TAILQ_ENTRY(vkrpc_method_queue_t) entries;
};

struct vkrpc_response_queue_t {
	vkrpc_response_t res;
	TAILQ_ENTRY(vkrpc_response_queue_t) entries;
};

struct vkrpc_notifier_queue_t {
	vkrpc_notifier_t noti;
	TAILQ_ENTRY(vkrpc_notifier_queue_t) entries;
};

typedef struct {
	HINT32 prog_id;
	HUINT32 unique_id;
	HUINT32 sock_hdl;
	unsigned long sem_mq;
	unsigned long sem_rq;
	unsigned long sem_nq;
	TAILQ_HEAD(vkrpc_method_queue_list_t,vkrpc_method_queue_t) mq;
	TAILQ_HEAD(vkrpc_response_queue_list_t,vkrpc_response_queue_t) rq;
	TAILQ_HEAD(vkrpc_notifier_queue_list_t,vkrpc_notifier_queue_t) nq;
} vkrpc_t;

#if defined(VK_RPC_DEBUG)
struct dbg_rpc_hdl_t {
	HUINT32 rpc_hdl;
	TAILQ_ENTRY(dbg_rpc_hdl_t) entries;
};
TAILQ_HEAD(dbg_rpc_list_t, dbg_rpc_hdl_t) dbg_rpc_queue;
#endif



#define ___STATIC_VARIABLES______________________________________________________________________________

#if defined(VK_RPC_DEBUG)
static unsigned long dbgsem;
#endif


#define ___PRIVATE_FUNCTION_PROTOTYPE____________________________________________________________________

#define	___PRIVATE_API___________________________________________________________________________________

static HBOOL P_RPC_VerifySignature(const HCHAR *pszSignature)
{
	if (!pszSignature)
		return FALSE;
	
	while (*pszSignature != '\0')
	{
		if (VK_strchr(VKRPC_METHOD_SIGNATURE, *pszSignature++) == NULL)
			return FALSE;
	}	
	return TRUE;
}

static void P_RPC_FreePacket(vkrpc_packet_t *pkt)
{
	HINT32 i;

	if (pkt)
	{
		if (pkt->obj)
		{
			for (i=0; i<pkt->obj_num; i++)
			{
				if (pkt->obj[i]) VK_free(pkt->obj[i]);
			}
			VK_free(pkt->obj);		
		}
		VK_free(pkt);
	}
	
	return;
}

static vkrpc_packet_t *P_RPC_MakePacket(HUINT32 eCallType,
	                                    const HCHAR *pszMethodName,
	                                    const HCHAR *pszSignature,
	                                    va_list ap)
{
	HINT32 i;
	vkrpc_packet_t *pkt = NULL;
	HCHAR *text;
	
	RPC_ERR((!pszMethodName));
	RPC_ERR((!pszSignature));

	pkt = VK_malloc(sizeof(vkrpc_packet_t));
	RPC_ERR((!pkt));
	VK_memset(pkt, 0, sizeof(vkrpc_packet_t));

	pkt->magic_code = VKRPC_METHOD_MAGIC_CODE;
	pkt->method_type =  eCallType;
	VK_snprintf(pkt->method_name, sizeof(pkt->method_name), "%s", pszMethodName);
	pkt->obj_num = VK_strlen(pszSignature);

	pkt->pkt_size += sizeof(pkt->magic_code);
	pkt->pkt_size += sizeof(pkt->method_name);
	pkt->pkt_size += sizeof(pkt->method_type);
	pkt->pkt_size += sizeof(pkt->obj_num);
	pkt->pkt_size += sizeof(pkt->pkt_size);

	if (pkt->obj_num)
	{
		pkt->obj = (VKRPC_OBJECT_T**)VK_malloc(sizeof(VKRPC_OBJECT_T*)*pkt->obj_num);
		RPC_ERR((!pkt->obj));
		VK_memset(pkt->obj, 0, sizeof(VKRPC_OBJECT_T*)*pkt->obj_num);		
		
		for (i=0; i<pkt->obj_num; i++)
		{
			pkt->obj[i] = (VKRPC_OBJECT_T*)VK_malloc(sizeof(VKRPC_OBJECT_T));
			RPC_ERR((!pkt->obj[i]));

			switch(*pszSignature++) {
			case 'i':
				pkt->obj[i]->type = VkOBJECT_TYPE_INT;
				pkt->obj[i]->u.i.num = va_arg(ap, HINT32);
				pkt->pkt_size += sizeof(HINT32);
				break;
			case 'l':
				pkt->obj[i]->type = VkOBJECT_TYPE_INT64;
				pkt->obj[i]->u.l.num64 = va_arg(ap, HINT64);
				pkt->pkt_size += sizeof(HINT64);
				break;
			case 's':
				pkt->obj[i]->type = VkOBJECT_TYPE_STR;
				text = va_arg(ap, HCHAR *);
				pkt->obj[i]->u.s.str = (text)?text:"";
				pkt->pkt_size += VK_strlen(pkt->obj[i]->u.s.str)+1;
				break;
			case 'b':
				pkt->obj[i]->type = VkOBJECT_TYPE_BIN;
				pkt->obj[i]->u.b.data = va_arg(ap, void *);
				pkt->obj[i]->u.b.size = va_arg(ap, HUINT32);
				pkt->pkt_size += sizeof(HUINT32);
				pkt->pkt_size += pkt->obj[i]->u.b.size;
				break;
			default:
				RPC_ERR((1));
			}
			pkt->pkt_size += sizeof(VKRPC_OBJECT_TYPE_E);
		}
	}

	return pkt;	
rpc_err:
	P_RPC_FreePacket(pkt);
	return NULL;
}

static HUINT8 *P_RPC_SerializePacket(vkrpc_packet_t *pkt)
{
	HUINT8 *buf = NULL;
	HUINT32 offset = 0;
	HINT32 i;
	RPC_ERR((!pkt));

	buf = VK_malloc(pkt->pkt_size);
	RPC_ERR((!buf));

	VK_memcpy(&buf[offset], &pkt->magic_code, sizeof(pkt->magic_code));
	offset += sizeof(pkt->magic_code);
	VK_memcpy(&buf[offset], &pkt->method_name, sizeof(pkt->method_name));
	offset += sizeof(pkt->method_name);
	VK_memcpy(&buf[offset], &pkt->method_type, sizeof(pkt->method_type));
	offset += sizeof(pkt->method_type);
	VK_memcpy(&buf[offset], &pkt->obj_num, sizeof(pkt->obj_num));
	offset += sizeof(pkt->obj_num);

	if (pkt->obj_num)
	{
		for (i=0; i<pkt->obj_num; i++)
		{
			VK_memcpy(&buf[offset], &pkt->obj[i]->type, sizeof(VKRPC_OBJECT_TYPE_E));
			offset += sizeof(VKRPC_OBJECT_TYPE_E);
			switch(pkt->obj[i]->type) {
			case VkOBJECT_TYPE_INT:
				VK_memcpy(&buf[offset], &pkt->obj[i]->u.i.num, sizeof(HINT32));
				offset += sizeof(HINT32);
				break;
			case VkOBJECT_TYPE_INT64:
				VK_memcpy(&buf[offset], &pkt->obj[i]->u.l.num64, sizeof(HINT64));
				offset += sizeof(HINT64);
				break;
			case VkOBJECT_TYPE_STR:
				VK_memcpy(&buf[offset], pkt->obj[i]->u.s.str, VK_strlen(pkt->obj[i]->u.s.str)+1);
				offset += VK_strlen(pkt->obj[i]->u.s.str)+1;
				break;
			case VkOBJECT_TYPE_BIN:
				VK_memcpy(&buf[offset], &pkt->obj[i]->u.b.size, sizeof(HUINT32));
				offset += sizeof(HUINT32);
				VK_memcpy(&buf[offset], pkt->obj[i]->u.b.data, pkt->obj[i]->u.b.size);
				offset += pkt->obj[i]->u.b.size;
				break;
			default:
				break;
			}
		}		
	}

	VK_memcpy(&buf[offset], &pkt->pkt_size, sizeof(pkt->pkt_size));
	offset += sizeof(pkt->pkt_size);

	RPC_ERR((offset!=pkt->pkt_size));
	return buf;

rpc_err:
	if (buf) VK_free(buf);
	return NULL;
}

static vkrpc_packet_t *P_RPC_DeserializePacket(HUINT8 *srzd, HUINT32 size)
{
	vkrpc_packet_t *pkt = NULL;
	HUINT32 offset = 0;
	HINT32 i;
	VKRPC_OBJECT_TYPE_E obj_type;

	RPC_ERR((!srzd && !size));
	pkt = VK_malloc(sizeof(vkrpc_packet_t));
	RPC_ERR((!pkt));

	/*typedef struct {
		HUINT32 magic_code;
		HCHAR method_name[VKRPC_METHOD_NAME_LEN];
		HUINT32 method_calltype;
		HINT32 obj_num;
		VKRPC_OBJECT_T **obj;
		HUINT32 packet_byte_size;
	} vkrpc_packet_t;*/

	VK_memcpy(&pkt->magic_code, &srzd[offset], sizeof(pkt->magic_code));
	offset+=sizeof(pkt->magic_code);
	VK_memcpy(pkt->method_name, &srzd[offset], sizeof(pkt->method_name));
	offset+=sizeof(pkt->method_name);
	VK_memcpy(&pkt->method_type, &srzd[offset], sizeof(pkt->method_type));
	offset+=sizeof(pkt->method_type);
	VK_memcpy(&pkt->obj_num, &srzd[offset], sizeof(pkt->obj_num));
	offset+=sizeof(pkt->obj_num);

	if (pkt->obj_num)
	{
		pkt->obj = (VKRPC_OBJECT_T**)VK_malloc(sizeof(VKRPC_OBJECT_T*)*pkt->obj_num);
		RPC_ERR((!pkt->obj));
		VK_memset(pkt->obj, 0, sizeof(VKRPC_OBJECT_T*)*pkt->obj_num);		
		
		for (i=0; i<pkt->obj_num; i++)
		{
			pkt->obj[i] = (VKRPC_OBJECT_T*)VK_malloc(sizeof(VKRPC_OBJECT_T));
			RPC_ERR((!pkt->obj[i]));

			VK_memcpy(&obj_type, &srzd[offset], sizeof(obj_type));

			switch(obj_type) {
			case VkOBJECT_TYPE_INT:
				pkt->obj[i]->type = VkOBJECT_TYPE_INT;
				offset += sizeof(VKRPC_OBJECT_TYPE_E);
				VK_memcpy(&pkt->obj[i]->u.i.num, &srzd[offset], sizeof(pkt->obj[i]->u.i.num));
				offset += sizeof(pkt->obj[i]->u.i.num);
				break;
			case VkOBJECT_TYPE_INT64:
				pkt->obj[i]->type = VkOBJECT_TYPE_INT64;
				offset += sizeof(VKRPC_OBJECT_TYPE_E);
				VK_memcpy(&pkt->obj[i]->u.l.num64, &srzd[offset], sizeof(pkt->obj[i]->u.l.num64));
				offset += sizeof(pkt->obj[i]->u.l.num64);
				break;
			case VkOBJECT_TYPE_STR:
				pkt->obj[i]->type = VkOBJECT_TYPE_STR;
				offset += sizeof(VKRPC_OBJECT_TYPE_E);
				pkt->obj[i]->u.s.str = &srzd[offset];
				offset += VK_strlen(pkt->obj[i]->u.s.str)+1;
				break;
			case VkOBJECT_TYPE_BIN:
				pkt->obj[i]->type = VkOBJECT_TYPE_BIN;
				offset += sizeof(VKRPC_OBJECT_TYPE_E);				
				VK_memcpy(&pkt->obj[i]->u.b.size, &srzd[offset], sizeof(pkt->obj[i]->u.b.size));
				offset += sizeof(pkt->obj[i]->u.b.size);
				pkt->obj[i]->u.b.data = &srzd[offset];
				offset += pkt->obj[i]->u.b.size;
				break;
			default:
				RPC_ERR((1));
			}			
		}
	}
	VK_memcpy(&pkt->pkt_size, &srzd[offset], sizeof(pkt->pkt_size));

	RPC_ERR((pkt->pkt_size != size));		
	return pkt;
	
rpc_err:
	P_RPC_FreePacket(pkt);
	return NULL;
}

static void P_RPC_FreeSerializedPacket(HUINT8 *data)
{
	if (data) VK_free(data);
	return;
}

static HINT32 P_RPC_RegisterNotifier(HUINT32 nSockClient, HINT32 nArgc, VKRPC_OBJECT_T **apArgv, HUINT32 nRequestId, void *pvUserdata)
{
	HCHAR *notifierName;
	struct vkrpc_notifier_queue_t *nq = NULL;
	vkrpc_t *rpc_hdl = (vkrpc_t*)pvUserdata;

	HAPPY(nRequestId);
	
	RPC_ERR((!rpc_hdl));
	RPC_ERR((nArgc!=1));

	notifierName = apArgv[0]->u.s.str;
	RPC_ERR((!notifierName));

	nq = VK_malloc(sizeof(struct vkrpc_notifier_queue_t));
	RPC_ERR((!nq));

	VK_snprintf(nq->noti.notifier_name, sizeof(nq->noti.notifier_name), "%s", notifierName);
	nq->noti.clisock_hdl = (HUINT32)nSockClient;
	nq->noti.pfn = NULL;
	nq->noti.pvData = NULL;

	VK_SEM_Get(rpc_hdl->sem_nq);
	TAILQ_INSERT_TAIL(&rpc_hdl->nq, nq, entries);
	VK_SEM_Release(rpc_hdl->sem_nq);
	return 0;
	
rpc_err:
	if (nq) VK_free(nq);
	return -1;
}

#define	___PUBLIC_API____________________________________________________________________________________

int VK_RPC_Open(HUINT32 nRoute,
                HINT32 nProgId,
                HUINT32 *hRpc)
{
	vkrpc_t *rpc_hdl = NULL;
	HINT32 rc;
#if defined(VK_RPC_DEBUG)
	struct dbg_rpc_hdl_t *dbg_rpc = NULL;
#endif

	RPC_ERR((!nRoute));

	rpc_hdl = (vkrpc_t*)VK_malloc(sizeof(vkrpc_t));
	RPC_ERR((!rpc_hdl));

	rpc_hdl->prog_id = nProgId;
	rpc_hdl->unique_id = (HUINT32)rpc_hdl;
	rpc_hdl->sock_hdl = nRoute;
	(void)VK_SEM_Create((unsigned long*)&rpc_hdl->sem_mq, "rpcsemmq", VK_SUSPENDTYPE_PRIORITY);
	(void)VK_SEM_Create((unsigned long*)&rpc_hdl->sem_rq, "rpcsemrq", VK_SUSPENDTYPE_PRIORITY);
	(void)VK_SEM_Create((unsigned long*)&rpc_hdl->sem_nq, "rpcsemnq", VK_SUSPENDTYPE_PRIORITY);

	TAILQ_INIT(&rpc_hdl->mq);
	TAILQ_INIT(&rpc_hdl->rq);
	TAILQ_INIT(&rpc_hdl->nq);
	
#if defined(VK_RPC_DEBUG)
	dbg_rpc = VK_malloc(sizeof(struct dbg_rpc_hdl_t));
	if (dbg_rpc)
	{
		VK_SEM_Get(dbgsem);
		dbg_rpc->rpc_hdl = (HUINT32)rpc_hdl;
		TAILQ_INSERT_TAIL(&dbg_rpc_queue, dbg_rpc, entries);
		VK_SEM_Release(dbgsem);
	}
#endif

	rc = VK_RPC_RegisterCall((HUINT32)rpc_hdl, "vkrpc:registerNotifier", "s", P_RPC_RegisterNotifier, (void*)rpc_hdl);
	RPC_ERR((rc!=VK_OK));

	*hRpc = (HUINT32)rpc_hdl;
	return VK_OK;
	
rpc_err:
	if (rpc_hdl) VK_free(rpc_hdl);
#if defined(VK_RPC_DEBUG)
	if (dbg_rpc) VK_free(dbg_rpc);
#endif
	*hRpc = 0;
	return VK_ERROR;
}

int VK_RPC_Close(HUINT32 hRpc)
{
	int ret = VK_ERROR;
	vkrpc_t *rpc_hdl = (vkrpc_t*)hRpc;
	struct vkrpc_method_queue_t *mq, *tmq;
	struct vkrpc_response_queue_t *rq, *trq;
	struct vkrpc_notifier_queue_t *nq, *tnq;
#if defined(VK_RPC_DEBUG)
	struct dbg_rpc_hdl_t *dq, *tdq;
#endif

	RPC_ERR((!rpc_hdl));
	RPC_ERR((rpc_hdl->unique_id!=hRpc));

	VK_SEM_Get(rpc_hdl->sem_mq);
	for (mq = TAILQ_FIRST(&rpc_hdl->mq); mq != NULL; mq = tmq) 
	{
		tmq = TAILQ_NEXT(mq, entries);
		TAILQ_REMOVE(&rpc_hdl->mq, mq, entries);
		if (mq->methods.method_sign)
			VK_free(mq->methods.method_sign);
		VK_free(mq);
	}
	VK_SEM_Release(rpc_hdl->sem_mq);

	VK_SEM_Get(rpc_hdl->sem_rq);
	for (rq = TAILQ_FIRST(&rpc_hdl->rq); rq != NULL; rq = trq) 
	{
		trq = TAILQ_NEXT(rq, entries);
		TAILQ_REMOVE(&rpc_hdl->rq, rq, entries);
		VK_free(rq);
	}
	VK_SEM_Release(rpc_hdl->sem_rq);

	VK_SEM_Get(rpc_hdl->sem_nq);
	for (nq = TAILQ_FIRST(&rpc_hdl->nq); nq != NULL; nq = tnq) 
	{
		tnq = TAILQ_NEXT(nq, entries);
		TAILQ_REMOVE(&rpc_hdl->nq, nq, entries);
		VK_free(nq);
	}
	VK_SEM_Release(rpc_hdl->sem_nq);

	VK_SEM_Destroy(rpc_hdl->sem_mq);	
	VK_SEM_Destroy(rpc_hdl->sem_rq);
	VK_SEM_Destroy(rpc_hdl->sem_nq);	
	
#if defined(VK_RPC_DEBUG)
	VK_SEM_Get(dbgsem);
	for (dq = TAILQ_FIRST(&dbg_rpc_queue); dq != NULL; dq = tdq) 
	{
		tdq = TAILQ_NEXT(dq, entries);
		if (dq->rpc_hdl==(HUINT32)rpc_hdl)
		{
			TAILQ_REMOVE(&dbg_rpc_queue, dq, entries);
			VK_free(dq);
		}
	}
	VK_SEM_Release(dbgsem);
#endif
	VK_free(rpc_hdl);

	ret = VK_OK;
rpc_err:
	return ret;
}

int VK_RPC_RegisterCall(HUINT32 hRpc,
	                    const HCHAR *pszMethodName,
	                    const HCHAR *pszSignature,
	                    VkRPC_Func_t pfnFunction,
	                    void *pvUserData)
{
	int ret = VK_ERROR;
	vkrpc_t *rpc_hdl = (vkrpc_t*)hRpc;
	struct vkrpc_method_queue_t *mq;

	RPC_ERR((!rpc_hdl));
	RPC_ERR((rpc_hdl->unique_id!=hRpc));
	RPC_ERR((!pszMethodName));
	RPC_ERR((!pszSignature));
	RPC_ERR((!P_RPC_VerifySignature(pszSignature)));
	
	mq = VK_malloc(sizeof(struct vkrpc_method_queue_t));
	RPC_ERR((!mq));

	VK_snprintf(mq->methods.method_name, sizeof(mq->methods.method_name), "%s", pszMethodName);
	mq->methods.method_sign = VK_strdup(pszSignature);
	mq->methods.pfn = (VkRPC_Func_t)pfnFunction;
	mq->methods.usrdata = pvUserData;

	VK_SEM_Get(rpc_hdl->sem_mq);
	TAILQ_INSERT_TAIL(&rpc_hdl->mq, mq, entries);
	VK_SEM_Release(rpc_hdl->sem_mq);

	ret = VK_OK;
rpc_err:
	return ret;
}

int VK_RPC_RegisterNotifier(HUINT32 hRpc,
	                        const HCHAR *pszNotifierName,
	                        void *pvData,
	                        VkRPC_Noti_t pfnNotifierCb)
{
	HINT32 ret = VK_ERROR;
	vkrpc_t *rpc_hdl = (vkrpc_t*)hRpc;
	struct vkrpc_notifier_queue_t *nq = NULL;

	RPC_ERR((!rpc_hdl));
	RPC_ERR((rpc_hdl->unique_id!=hRpc));
	RPC_ERR((!pszNotifierName));

	ret = VK_RPC_Call(hRpc, "vkrpc:registerNotifier", "s", pszNotifierName);
	RPC_ERR((ret!=VK_OK));

	nq = VK_malloc(sizeof(struct vkrpc_notifier_queue_t));
	RPC_ERR((!nq));

	VK_snprintf(nq->noti.notifier_name, sizeof(nq->noti.notifier_name), "%s", pszNotifierName);
	nq->noti.clisock_hdl = 0;
	nq->noti.pfn = pfnNotifierCb;
	nq->noti.pvData = pvData;

	VK_SEM_Get(rpc_hdl->sem_nq);
	TAILQ_INSERT_TAIL(&rpc_hdl->nq, nq, entries);
	VK_SEM_Release(rpc_hdl->sem_nq);
	
	return VK_OK;
	
rpc_err:
	if (nq) VK_free(nq);
	return ret;
}

int	VK_RPC_Request(HUINT32 hRpc,
	               VkRPC_Response_t pfnCallback,
	               HUINT32 nTimeout,
	               void *pvUserdata,
	               const HCHAR *pszMethodName,
	               const HCHAR *pszSignature,
	               ...)
{
	vkrpc_t *rpc_hdl = (vkrpc_t*)hRpc;
	vkrpc_packet_t *pkt = NULL;
	struct vkrpc_response_queue_t *rq = NULL;
	va_list ap;
	HUINT8 *srzd = NULL;;
	int ret = VK_ERROR;
	HCHAR name[32];
	unsigned long evt;

	RPC_ERR((!rpc_hdl));
	RPC_ERR((rpc_hdl->unique_id!=hRpc));
	RPC_ERR((!pszMethodName));
	RPC_ERR((!pszSignature));
	RPC_ERR((!P_RPC_VerifySignature(pszSignature)));

	va_start(ap, pszSignature);
	pkt = (vkrpc_packet_t*)P_RPC_MakePacket(VkRPC_CALLTYPE_REQUEST, pszMethodName, pszSignature, ap);
	va_end(ap);

	RPC_ERR((!pkt));

	srzd = P_RPC_SerializePacket(pkt);
	RPC_ERR((!srzd));

	rq = VK_malloc(sizeof(struct vkrpc_response_queue_t));
	RPC_ERR((!rq));
	
	rq->res.pfn = pfnCallback;
	rq->res.usrdata = pvUserdata;
	rq->res.req_id = (HUINT32)rq;
	VK_snprintf(name, sizeof(name), "evt%x", (HUINT32)rq);			
	ret = VK_EVENT_Create(&rq->res.done, name);
	if (ret != VK_OK)
	{
		PrintError("Critical! can't create event handle(rc:%d)\n", ret);
		VK_free(rq);
		goto rpc_err;
	}

	VK_SEM_Get(rpc_hdl->sem_rq);
	TAILQ_INSERT_TAIL(&rpc_hdl->rq, rq, entries);
	VK_SEM_Release(rpc_hdl->sem_rq);

	ret = VK_SOCKET_Write(rpc_hdl->sock_hdl, (void*)srzd, pkt->pkt_size, (HUINT32)rq);
	RPC_ERR((ret!=VK_OK));

	/* wait for item of response-queue cleared */
	if (VK_EVENT_ReceiveTimeout(rq->res.done, &evt, nTimeout) == VK_TIMEOUT)
		PrintError("Critical! RPC timeout (id:0x%x)\n", rpc_hdl->prog_id);

	VK_EVENT_Destroy(rq->res.done);
	VK_SEM_Get(rpc_hdl->sem_rq);
	TAILQ_REMOVE(&rpc_hdl->rq, rq, entries);
	VK_SEM_Release(rpc_hdl->sem_rq);
	VK_free(rq);
	
	ret = VK_OK;	
rpc_err:
	P_RPC_FreeSerializedPacket(srzd);
	P_RPC_FreePacket(pkt);	
	return ret;
}

int	VK_RPC_Call(HUINT32 hRpc,
	            const HCHAR *pszMethodName,
	            const HCHAR *pszSignature,
	            ...)
{
	vkrpc_t *rpc_hdl = (vkrpc_t*)hRpc;
	vkrpc_packet_t *pkt = NULL;
	va_list ap;
	HUINT8 *srzd = NULL;;
	int ret = VK_ERROR;

	RPC_ERR((!rpc_hdl));
	RPC_ERR((rpc_hdl->unique_id!=hRpc));
	RPC_ERR((!pszMethodName));
	RPC_ERR((!pszSignature));
	RPC_ERR((!P_RPC_VerifySignature(pszSignature)));

	va_start(ap, pszSignature);
	pkt = (vkrpc_packet_t*)P_RPC_MakePacket(VkRPC_CALLTYPE_CALL, pszMethodName, pszSignature, ap);
	va_end(ap);
	
	RPC_ERR((!pkt));

	srzd = P_RPC_SerializePacket(pkt);
	RPC_ERR((!srzd));

	ret = VK_SOCKET_Write(rpc_hdl->sock_hdl, (void*)srzd, pkt->pkt_size, rpc_hdl->prog_id);
	RPC_ERR((ret!=VK_OK));
	
	ret = VK_OK;	
rpc_err:
	P_RPC_FreeSerializedPacket(srzd);
	P_RPC_FreePacket(pkt);	
	return ret;
}

int	VK_RPC_Response(HUINT32 hRpc,
	                HUINT32 hSockClient,
	                HUINT32 nReqId,
	                const HCHAR *pszMethodName,
	                const HCHAR *pszSignature,
	                ...)
{
	vkrpc_t *rpc_hdl = (vkrpc_t*)hRpc;
	vkrpc_packet_t *pkt = NULL;
	va_list ap;
	HUINT8 *srzd = NULL;;
	int ret = VK_ERROR;

	RPC_ERR((!rpc_hdl));
	RPC_ERR((rpc_hdl->unique_id!=hRpc));
	RPC_ERR((!hSockClient));
	RPC_ERR((!pszMethodName));
	RPC_ERR((!pszSignature));
	RPC_ERR((!P_RPC_VerifySignature(pszSignature)));

	va_start(ap, pszSignature);
	pkt = (vkrpc_packet_t*)P_RPC_MakePacket(VkRPC_CALLTYPE_RESPONSE, pszMethodName, pszSignature, ap);
	va_end(ap);
	
	RPC_ERR((!pkt));

	srzd = P_RPC_SerializePacket(pkt);
	RPC_ERR((!srzd));

	ret = VK_SOCKET_Write(hSockClient, (void*)srzd, pkt->pkt_size, nReqId);
	RPC_ERR((ret!=VK_OK));

	ret = VK_OK;
rpc_err:
	P_RPC_FreeSerializedPacket(srzd);
	P_RPC_FreePacket(pkt);	
	return ret;
}

int VK_RPC_Notify(HUINT32 hRpc,
	              const HCHAR *pszNotifierName,
	              const HCHAR *pszSignature,
	              ...)
{
	vkrpc_t *rpc_hdl = (vkrpc_t*)hRpc;
	vkrpc_packet_t *pkt = NULL;
	va_list ap;
	HUINT8 *srzd = NULL;;
	int ret = VK_ERROR;
	struct vkrpc_notifier_queue_t *nq;

	RPC_ERR((!rpc_hdl));
	RPC_ERR((rpc_hdl->unique_id!=hRpc));
	RPC_ERR((!pszNotifierName));
	RPC_ERR((!pszSignature));
	RPC_ERR((!P_RPC_VerifySignature(pszSignature)));

	VK_SEM_Get(rpc_hdl->sem_nq);
	TAILQ_FOREACH(nq, &rpc_hdl->nq, entries)
	{
		if (VK_strncmp(nq->noti.notifier_name, pszNotifierName, sizeof(nq->noti.notifier_name)) == 0)
		{
			va_start(ap, pszSignature);
			pkt = (vkrpc_packet_t*)P_RPC_MakePacket(VkRPC_CALLTYPE_NOTIFY, pszNotifierName, pszSignature, ap);
			va_end(ap);
			RPC_SEM_ERR((!pkt));

			srzd = P_RPC_SerializePacket(pkt);
			RPC_SEM_ERR((!srzd));

			ret = VK_SOCKET_Write(nq->noti.clisock_hdl, (void*)srzd, pkt->pkt_size, rpc_hdl->prog_id);
			RPC_SEM_ERR((ret!=VK_OK));

			P_RPC_FreeSerializedPacket(srzd);
			P_RPC_FreePacket(pkt);

			srzd = NULL;
			pkt = NULL;
		}
	}
	VK_SEM_Release(rpc_hdl->sem_nq);
	return VK_OK;

sem_err:
	VK_SEM_Release(rpc_hdl->sem_nq);
rpc_err:
	P_RPC_FreeSerializedPacket(srzd);
	P_RPC_FreePacket(pkt);	
	return ret;
}

int VK_RPC_ProcessRpc(HUINT32 hRpc,
	                  HUINT32 nReqId,
	                  void *pvData,
	                  HUINT32 ulSize,
	                  HUINT32 hClientSock)
{
	int ret = VK_ERROR;
	vkrpc_t *rpc_hdl = (vkrpc_t*)hRpc;
	vkrpc_packet_t *pkt = NULL;
	struct vkrpc_method_queue_t *mq;
	struct vkrpc_response_queue_t *rq;
	struct vkrpc_notifier_queue_t *nq;

	RPC_ERR((!rpc_hdl));
	RPC_ERR((rpc_hdl->unique_id!=hRpc));

	pkt = P_RPC_DeserializePacket(pvData, ulSize);
	RPC_ERR((!pkt));

	/*typedef struct {
		HUINT32 magic_code;
		HCHAR method_name[VKRPC_METHOD_NAME_LEN];
		HUINT32 method_calltype;
		HINT32 obj_num;
		VKRPC_OBJECT_T **obj;
		HUINT32 packet_byte_size;
	} vkrpc_packet_t;*/
	
	RPC_ERR((pkt->magic_code!=VKRPC_METHOD_MAGIC_CODE));

	switch(pkt->method_type) {
	case VkRPC_CALLTYPE_CALL:
	case VkRPC_CALLTYPE_REQUEST:
		VK_SEM_Get(rpc_hdl->sem_mq);
		TAILQ_FOREACH(mq, &rpc_hdl->mq, entries)
		{
			if (!strncmp(mq->methods.method_name, pkt->method_name, sizeof(pkt->method_name))
				&& mq->methods.pfn) {
				mq->methods.pfn(hClientSock, pkt->obj_num, pkt->obj, nReqId, mq->methods.usrdata);
			}
		}
		VK_SEM_Release(rpc_hdl->sem_mq);
		break;	
	case VkRPC_CALLTYPE_RESPONSE:
		VK_SEM_Get(rpc_hdl->sem_rq);
		TAILQ_FOREACH(rq, &rpc_hdl->rq, entries)
		{
			if (rq->res.req_id == nReqId) {
				if (rq->res.pfn)
					rq->res.pfn(pkt->obj_num, pkt->obj, nReqId, rq->res.usrdata);
				VK_EVENT_Send(rq->res.done, 0);
			}
		}
		VK_SEM_Release(rpc_hdl->sem_rq);
		break;
	case VkRPC_CALLTYPE_NOTIFY:
		VK_SEM_Get(rpc_hdl->sem_nq);
		TAILQ_FOREACH(nq, &rpc_hdl->nq, entries)
		{
			if (nq->noti.pfn
				&& VK_strncmp(nq->noti.notifier_name, pkt->method_name, sizeof(nq->noti.notifier_name))==0)
			{
				nq->noti.pfn(pkt->obj_num, pkt->obj, nReqId, nq->noti.pvData);
			}
		}
		VK_SEM_Release(rpc_hdl->sem_nq);
		break;
	default:
		break;
	}
	
	ret = VK_OK;
rpc_err:
	P_RPC_FreePacket(pkt);
	return ret;
}

int VK_RPC_Init(void)
{
	HINT32 rc = VK_OK;
	
#if defined(VK_RPC_DEBUG)	
	TAILQ_INIT(&dbg_rpc_queue);
	rc = VK_SEM_Create(&dbgsem, "vkrpcdbgsem", VK_SUSPENDTYPE_FIFO);
#endif

	return rc;
}

void VK_RPC_PrintRpcInfo(void)
{
#if defined(VK_SOCKET_DEBUG)
	vkrpc_t *rpc_hdl;
	struct dbg_rpc_hdl_t *dq;
	struct vkrpc_method_queue_t *mq;

	VK_SEM_Get(dbgsem);
	TAILQ_FOREACH(dq, &dbg_rpc_queue, entries)
	{
		rpc_hdl = (vkrpc_t*)dq->rpc_hdl;		
		PrintDebug("***************************[%s info]**********************************\n", "rpc");
		PrintDebug("** prog id: 0x%x\n", rpc_hdl->prog_id);
		VK_SEM_Get(rpc_hdl->sem_mq);
		TAILQ_FOREACH(mq, &rpc_hdl->mq, entries)
		{
			PrintDebug("** method name: %s\n", mq->methods.method_name);
			PrintDebug("** method signature: %s\n", mq->methods.method_sign);
		}
		VK_SEM_Release(rpc_hdl->sem_mq);
		PrintDebug("***************************[%s end]***********************************\n", "rpc");
	}
	VK_SEM_Release(dbgsem);
#endif
}

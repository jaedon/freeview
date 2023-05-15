#include <hlib.h>
#include "_dlib_port_humax_svc.h"


typedef struct
{
	DxField_t		field;
	HBOOL			(* getData) (DxField_t *field, const void *src, HUINT32 size);
} DxIPSVC_Field_t;


#define	RETURN_IF_VOID(expr)	do{if(expr)return;}while(0)
#define	RETURN_IF(expr, err)	do{if(expr)return (err);}while(0)
#define	GOTO_IF(expr, pos)		do{if(expr)goto pos;}while(0)

#define	D_RETURN_IF_VOID(expr)	do{if(expr){HxLOG_Debug("%s failed!!\n", #expr);return;}}while(0)
#define	D_RETURN_IF(expr, err)	do{if(expr){HxLOG_Debug("%s failed!!\n", #expr);return (err);}}while(0)
#define	D_GOTO_IF(expr, pos)	do{if(expr){HxLOG_Debug("%s failed!!\n", #expr);goto pos;}}while(0)

#define	W_RETURN_IF_VOID(expr)	do{if(expr){HxLOG_Warning("%s failed!!\n", #expr);return;}}while(0)
#define	W_RETURN_IF(expr, err)	do{if(expr){HxLOG_Warning("%s failed!!\n", #expr);return (err);}}while(0)
#define	W_GOTO_IF(expr, pos)	do{if(expr){HxLOG_Warning("%s failed!!\n", #expr);goto pos;}}while(0)

#define	E_RETURN_IF_VOID(expr)	do{if(expr){HxLOG_Error("%s failed!!\n", #expr);return;}}while(0)
#define	E_RETURN_IF(expr, err)	do{if(expr){HxLOG_Error("%s failed!!\n", #expr);return (err);}}while(0)
#define	E_GOTO_IF(expr, pos)	do{if(expr){HxLOG_Error("%s failed!!\n", #expr);goto pos;}}while(0)
void 			DLIB_SVC_HUMAX_Print(DxIpService_t	*Svc);

static HBOOL	dlib_svc_humax_GetTsID(DxField_t *field, const void *src, HUINT32 size)
{
	DxIpService_t * pSvc = (DxIpService_t *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.integer = pSvc->usTsId;
	return TRUE;
}

static HBOOL	dlib_svc_humax_GetSvcID (DxField_t *field, const void *src, HUINT32 size)
{
	DxIpService_t * pSvc = (DxIpService_t *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.integer = pSvc->usSvcId;
	return TRUE;
}

static HBOOL	dlib_svc_humax_GetOnID (DxField_t *field, const void *src, HUINT32 size)
{
	DxIpService_t * pSvc = (DxIpService_t *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.integer = pSvc->usOnId;
	return TRUE;
}
static HBOOL	dlib_svc_humax_GetTriplet (DxField_t *field, const void *src, HUINT32 size)
{
	DxIpService_t * pSvc = (DxIpService_t *)src;
	HCHAR	buf[32];
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);

	HxSTD_PrintToStrN(buf, 32, "%x.%x.%x"
					, pSvc->usOnId
					, pSvc->usTsId
					, pSvc->usSvcId
				);

	field->u.string = DLIB_StrDup(buf);
	RETURN_IF(field->u.string == NULL, FALSE);
	HxLOG_Debug("Triplet[%s]\n", field->u.string);
	return TRUE;
}

static HBOOL	dlib_svc_humax_GetMediaSupport (DxField_t *field, const void *src, HUINT32 size)
{
	DxIpService_t * pSvc = (DxIpService_t *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.integer = pSvc->ulMediaSupport;
	return TRUE;
}

static const DxIPSVC_Field_t	s_SVC_HUMAX_FIELDS[] =
{
	{{eDxTYPE_INTEGER	, "tsid"			, FALSE, {0}	}, dlib_svc_humax_GetTsID}
	, {{eDxTYPE_INTEGER	, "sid"				, FALSE, {0}	}, dlib_svc_humax_GetSvcID}
	, {{eDxTYPE_INTEGER	, "onid"			, FALSE, {0}	}, dlib_svc_humax_GetOnID}
	, {{eDxTYPE_STRING 	, "triplet"   		, FALSE, {0}	}, dlib_svc_humax_GetTriplet}
	, {{eDxTYPE_INTEGER	, "mediaSupport" 	, FALSE, {0}	}, dlib_svc_humax_GetMediaSupport}

};

DxIpService_t *	dlib_svc_humax_NewSvc (HINT32 uid, const HUINT8 *src, HUINT32 size)
{
	DxIpService_t		*Db		= NULL;
	DxIpService_t		*Svc 	= NULL;

	HxLOG_Trace();
	Db = (DxIpService_t *)src;

	RETURN_IF(Db == NULL || size == 0 , NULL);
	Svc = (DxIpService_t *)DLIB_Malloc(sizeof(DxIpService_t));
	RETURN_IF(Svc == NULL, NULL);
	HxSTD_MemCopy(Svc,Db,sizeof(DxIpService_t));

#if defined(CONFIG_DEBUG)
	DLIB_SVC_HUMAX_Print(Svc);
#endif

	return Svc;
}


void dlib_svc_humax_Free (DxIpService_t *Svc)
{
	HxLOG_Trace();

	RETURN_IF_VOID(Svc == NULL);
	DLIB_Free(Svc);
}

#define DLIB_IPSVC_DBFILE CONFIG_PRODUCT_USERDATA_DIR "/ipepg/DxIpService_t.db"

const HCHAR *	DLIB_SVC_HUMAX_GetDBFullName (HINT32 dupIndex)
{
	if(dupIndex == 0)
		return DLIB_IPSVC_DBFILE;
	else
		return NULL;
}

HUINT32			DLIB_SVC_HUMAX_GetSyncTime (void)
{
	return 10;
}

HINT32			DLIB_SVC_HUMAX_GetFieldCount (void)
{
	return sizeof(s_SVC_HUMAX_FIELDS) / sizeof(s_SVC_HUMAX_FIELDS[0]);
}

DxField_t *		DLIB_SVC_HUMAX_GetField (HINT32 index, const void *src, HUINT32 srcSize)
{
	DxField_t field;

	if (DLIB_SVC_HUMAX_GetFieldCount() <= index)
	{
		HxLOG_Warning("%s(index) index(index) out of range~!\n", __FUNCTION__, index);
		return NULL;
	}
	memcpy(&field, &(s_SVC_HUMAX_FIELDS[index].field), sizeof(DxField_t));

	if (src && srcSize && s_SVC_HUMAX_FIELDS[index].getData)
	{
		if (!s_SVC_HUMAX_FIELDS[index].getData(&field, src, srcSize))
		{
			HxLOG_Warning("%s(%s) Cannot get data\n", __FUNCTION__, s_SVC_HUMAX_FIELDS[index].field.name);
			return NULL;
		}
#if 0//defined(CONFIG_DEBUG)
		if(field.type == eDxTYPE_INTEGER)
			printf("[%s] [%d]\n", field.name, field.u.integer);
		else if(field.type == eDxTYPE_STRING)
			printf("[%s] [%s]\n", field.name, field.u.string);
#endif

	}
	// 메모리 함수는 HxSTD_MemXXX를 사용한다.
	return (DxField_t *)DLIB_MemDup(&field, sizeof(DxField_t));
}

HERROR			DLIB_SVC_HUMAX_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// 파일 저장용으로 encoding이 필요한 경우 이 함수가 호출 됨
	// dst는 메모리 할당을 해서 전달해야 함 (HxSTD_MemXXX를 사용)
	// json이나 xml과 같은 script 형태로 저장해야 할 것임
	return ERR_FAIL;
}

HERROR			DLIB_SVC_HUMAX_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// 파일 저장용으로 encoding이 된 경우, load후 이 함수가 호출됨
	// dst는 메모리 할당을 해서 전달해야 함 (HxSTD_MemXXX를 사용)
	return ERR_FAIL;
}

HERROR			DLIB_SVC_HUMAX_Compress (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// 데이터 전송 시, 효율을 높이기 위해 압축이 필요한 경우 이 함수 호출됨
	// 압축 방식은 진짜 압축이 아닌, 그냥 string을 binary로 바꾸는 등
	// 아무튼 데이터를 줄이기만 하면 됨
	// dst는 메모리 할당을 해서 전달해야 함 (HxSTD_MemXXX를 사용)
	return ERR_FAIL;
}

HERROR			DLIB_SVC_HUMAX_Extract (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// 압축된 데이터 원복
	// dst는 메모리 할당을 해서 전달해야 함 (HxSTD_MemXXX를 사용)
	return ERR_FAIL;
}

HERROR			DLIB_SVC_HUMAX_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type)
{
	DxIpService_t	*Svc = NULL;
	//HxLOG_Debug("UID : %08x\n", uid);
	Svc = dlib_svc_humax_NewSvc((HINT32)uid, (const HUINT8 *) src, srcSize);

	if (Svc == NULL)
		return ERR_FAIL;

	*type = Svc;
	return ERR_OK;
}


void			DLIB_SVC_HUMAX_FreeType (void *type)
{
	RETURN_IF_VOID(type == NULL);
	dlib_svc_humax_Free(type);
}

void 			DLIB_SVC_HUMAX_Print(DxIpService_t	*Svc)
{
	if(Svc == NULL)
		return;

	HxLOG_Print(
		"\tIP SERVICE:: svcId[0x%08x] tsId[0x%08x] onId[0x*08x] \n"
			, Svc->usSvcId , Svc->usTsId, Svc->usOnId);
}


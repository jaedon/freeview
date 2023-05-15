#include <hlib.h>
#include "_dlib_port_tvtv_svc.h"


typedef struct
{
	DxField_t		field;
	HBOOL		(* getData) (DxField_t *field, const void *src, HUINT32 size);
} DxTVTVSVC_Field_t;


#define	RETURN_IF_VOID(expr)		do{if(expr)return;}while(0)
#define	RETURN_IF(expr, err)		do{if(expr)return (err);}while(0)
#define	GOTO_IF(expr, pos)		do{if(expr)goto pos;}while(0)

#define	D_RETURN_IF_VOID(expr)	do{if(expr){HxLOG_Debug("%s failed!!\n", #expr);return;}}while(0)
#define	D_RETURN_IF(expr, err)	do{if(expr){HxLOG_Debug("%s failed!!\n", #expr);return (err);}}while(0)
#define	D_GOTO_IF(expr, pos)		do{if(expr){HxLOG_Debug("%s failed!!\n", #expr);goto pos;}}while(0)

#define	W_RETURN_IF_VOID(expr)	do{if(expr){HxLOG_Warning("%s failed!!\n", #expr);return;}}while(0)
#define	W_RETURN_IF(expr, err)	do{if(expr){HxLOG_Warning("%s failed!!\n", #expr);return (err);}}while(0)
#define	W_GOTO_IF(expr, pos)	do{if(expr){HxLOG_Warning("%s failed!!\n", #expr);goto pos;}}while(0)

#define	E_RETURN_IF_VOID(expr)	do{if(expr){HxLOG_Error("%s failed!!\n", #expr);return;}}while(0)
#define	E_RETURN_IF(expr, err)		do{if(expr){HxLOG_Error("%s failed!!\n", #expr);return (err);}}while(0)
#define	E_GOTO_IF(expr, pos)		do{if(expr){HxLOG_Error("%s failed!!\n", #expr);goto pos;}}while(0)
void 	DLIB_SVC_TVTV_Print(DxTvtvService_t	*Svc);

static HBOOL	dlib_svc_tvtv_GetTvtvID(DxField_t *field, const void *src, HUINT32 size)
{
	DxTvtvService_t * pSvc = (DxTvtvService_t *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.integer = pSvc->usTvtvId;
	return TRUE;
}

static HBOOL	dlib_svc_tvtv_GetTsID(DxField_t *field, const void *src, HUINT32 size)
{
	DxTvtvService_t * pSvc = (DxTvtvService_t *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.integer = pSvc->usTsId;
	return TRUE;
}

static HBOOL	dlib_svc_tvtv_GetSvcID (DxField_t *field, const void *src, HUINT32 size)
{
	DxTvtvService_t * pSvc = (DxTvtvService_t *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.integer = pSvc->usSvcId;
	return TRUE;
}

static HBOOL	dlib_svc_tvtv_GetOnID (DxField_t *field, const void *src, HUINT32 size)
{
	DxTvtvService_t * pSvc = (DxTvtvService_t *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.integer = pSvc->usOnId;
	return TRUE;
}
static HBOOL	dlib_svc_tvtv_GetTriplet (DxField_t *field, const void *src, HUINT32 size)
{
	DxTvtvService_t * pSvc = (DxTvtvService_t *)src;
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

static const DxTVTVSVC_Field_t	s_SVC_TVTV_FIELDS[] =
{
	{{eDxTYPE_INTEGER	, "tvtvId"			, FALSE, {0}	}, dlib_svc_tvtv_GetTvtvID}
	, {{eDxTYPE_INTEGER	, "tsid"			, FALSE, {0}	}, dlib_svc_tvtv_GetTsID}
	, {{eDxTYPE_INTEGER	, "sid"			, FALSE, {0}	}, dlib_svc_tvtv_GetSvcID}
	, {{eDxTYPE_INTEGER	, "onid"			, FALSE, {0}	}, dlib_svc_tvtv_GetOnID}
	, {{eDxTYPE_STRING 	, "triplet"   		, FALSE, {0}	}, dlib_svc_tvtv_GetTriplet}
};

DxTvtvService_t *	dlib_svc_tvtv_NewSvc (HINT32 uid, const HUINT8 *src, HUINT32 size)
{
	DxTvtvService_t		*Db		= NULL;
	DxTvtvService_t		*Svc 	= NULL;
	HxLOG_Trace();
	Db = (DxTvtvService_t *)src;

	RETURN_IF(Db == NULL || size == 0 , NULL);
	Svc = (DxTvtvService_t *)DLIB_Malloc(sizeof(DxTvtvService_t));
	RETURN_IF(Svc == NULL, NULL);
	HxSTD_MemCopy(Svc,Db,sizeof(DxTvtvService_t));

#if defined(CONFIG_DEBUG)
	DLIB_SVC_TVTV_Print(Svc);
#endif

	return Svc;
}


void dlib_svc_tvtv_Free (DxTvtvService_t *Svc)
{
	HxLOG_Trace();

	RETURN_IF_VOID(Svc == NULL);
	DLIB_Free(Svc);
}
#define DLIB_TVTVSVC_DBFILE CONFIG_PRODUCT_USERDATA_DIR "/tvtvepg/DxTvtvService_t.db"

const HCHAR *	DLIB_SVC_TVTV_GetDBFullName (HINT32 dupIndex)
{
	if(dupIndex == 0)
		return DLIB_TVTVSVC_DBFILE;
	else
		return NULL;
}

HUINT32			DLIB_SVC_TVTV_GetSyncTime (void)
{
	return 10;
}

HINT32			DLIB_SVC_TVTV_GetFieldCount (void)
{
	return sizeof(s_SVC_TVTV_FIELDS) / sizeof(s_SVC_TVTV_FIELDS[0]);
}

DxField_t *		DLIB_SVC_TVTV_GetField (HINT32 index, const void *src, HUINT32 srcSize)
{
	DxField_t field;

	if (DLIB_SVC_TVTV_GetFieldCount() <= index)
	{
		HxLOG_Warning("%s(index) index(index) out of range~!\n", __FUNCTION__, index);
		return NULL;
	}
	memcpy(&field, &(s_SVC_TVTV_FIELDS[index].field), sizeof(DxField_t));

	if (src && srcSize && s_SVC_TVTV_FIELDS[index].getData)
	{
		if (!s_SVC_TVTV_FIELDS[index].getData(&field, src, srcSize))
		{
			HxLOG_Warning("%s(%s) Cannot get data\n", __FUNCTION__, s_SVC_TVTV_FIELDS[index].field.name);
			return NULL;
		}
#if 0//defined(CONFIG_DEBUG)
		if(field.type == eDxTYPE_INTEGER)
			printf("[%s] [%d]\n", field.name, field.u.integer);
		else if(field.type == eDxTYPE_STRING)
			printf("[%s] [%s]\n", field.name, field.u.string);
#endif

	}
	// �޸� �Լ��� HxSTD_MemXXX�� ����Ѵ�.
	return (DxField_t *)DLIB_MemDup(&field, sizeof(DxField_t));
}

HERROR			DLIB_SVC_TVTV_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// ���� ��������� encoding�� �ʿ��� ��� �� �Լ��� ȣ�� ��
	// dst�� �޸� �Ҵ��� �ؼ� �����ؾ� �� (HxSTD_MemXXX�� ���)
	// json�̳� xml�� ���� script ���·� �����ؾ� �� ����
	return ERR_FAIL;
}

HERROR			DLIB_SVC_TVTV_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// ���� ��������� encoding�� �� ���, load�� �� �Լ��� ȣ���
	// dst�� �޸� �Ҵ��� �ؼ� �����ؾ� �� (HxSTD_MemXXX�� ���)
	return ERR_FAIL;
}

HERROR			DLIB_SVC_TVTV_Compress (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// ������ ���� ��, ȿ���� ���̱� ���� ������ �ʿ��� ��� �� �Լ� ȣ���
	// ���� ����� ��¥ ������ �ƴ�, �׳� string�� binary�� �ٲٴ� ��
	// �ƹ�ư �����͸� ���̱⸸ �ϸ� ��
	// dst�� �޸� �Ҵ��� �ؼ� �����ؾ� �� (HxSTD_MemXXX�� ���)
	return ERR_FAIL;
}

HERROR			DLIB_SVC_TVTV_Extract (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// ����� ������ ����
	// dst�� �޸� �Ҵ��� �ؼ� �����ؾ� �� (HxSTD_MemXXX�� ���)
	return ERR_FAIL;
}

HERROR			DLIB_SVC_TVTV_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type)
{
	DxTvtvService_t	*Svc = NULL;
	//HxLOG_Debug("UID : %08x\n", uid);
	Svc = dlib_svc_tvtv_NewSvc((HINT32)uid, (const HUINT8 *) src, srcSize);

	if (Svc == NULL)
		return ERR_FAIL;

	*type = Svc;
	return ERR_OK;
}


void			DLIB_SVC_TVTV_FreeType (void *type)
{
	RETURN_IF_VOID(type == NULL);
	dlib_svc_tvtv_Free(type);
}

void 			DLIB_SVC_TVTV_Print(DxTvtvService_t	*Svc)
{
	if(Svc == NULL)
		return;

	HxLOG_Print(
		"\tTVTV SERVICE:: tvtvId[0x%08x] svcId[0x%08x] tsId[0x%08x] onId[0x*08x] \n"
			, Svc->usTvtvId, Svc->usSvcId , Svc->usTsId, Svc->usOnId);
}


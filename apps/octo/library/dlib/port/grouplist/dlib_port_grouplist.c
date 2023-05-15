
#include <hlib.h>
#include "_dlib_port_grouplist.h"

typedef	struct
{
	DxField_t		field;
	HBOOL			(* getData) (DxField_t *field, const void *src, HUINT32 size);
} DxGroupList_Field_t;

static HBOOL	dlib_grouplist_GetName (DxField_t *field, const void *src, HUINT32 size)
{
	const DxGroupList_t	*gl = (DxGroupList_t*)src;

	HxLOG_Assert(src && size);
	field->u.string = DLIB_StrDup(gl->szGroupName);

	return TRUE;
}

static const DxGroupList_Field_t	s_GroupList_Fields[] =
{
	{{eDxTYPE_STRING , "name"      , TRUE , {0}}, dlib_grouplist_GetName}
};

HINT32			DLIB_GROUPLIST_GetFieldCount (void)
{
	return sizeof(s_GroupList_Fields) / sizeof(s_GroupList_Fields[0]);
}

DxField_t *		DLIB_GROUPLIST_GetField (HINT32 index, const void *src, HUINT32 srcSize)
{
	DxField_t field;

	if (DLIB_GROUPLIST_GetFieldCount() <= index)
	{
		HxLOG_Warning("%s(index) index(index) out of range~!\n", __FUNCTION__, index);
		return NULL;
	}
	memcpy(&field, &(s_GroupList_Fields[index].field), sizeof(DxField_t));

	if (src && srcSize && s_GroupList_Fields[index].getData)
	{
		if (!s_GroupList_Fields[index].getData(&field, src, srcSize))
		{
			HxLOG_Warning("%s(%s) Cannot get data\n", __FUNCTION__, s_GroupList_Fields[index].field.name);
			return NULL;
		}
	}

	// 메모리 함수는 HxSTD_MemXXX를 사용한다.
	return (DxField_t *)DLIB_MemDup(&field, sizeof(DxField_t));
}

void			DLIB_GROUPLIST_Print (FILE *fp, const DxGroupList_t *t)
{
	fprintf(fp, "GroupList[%s] size [%d]\n", t->szGroupName, t->nSize);
}




#include <hlib.h>
#include "_dlib_port_dbsetup.h"

void			DLIB_DBSETUP_Init(void)
{
//	dlib_RegisterDECModule(DxNAMEOF(DxDBSetup_t), dlib_provider_Encoder, dlib_provider_Decoder);
}

HINT32			DLIB_DBSETUP_GetFieldCount (void)
{
	return 0;
}

DxField_t *		DLIB_DBSETUP_GetField (HINT32 index, const void *src, HUINT32 srcSize)
{
	return NULL;
}

HERROR			DLIB_DBSETUP_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	(void)(uid);

	return ERR_FAIL;
}

HERROR			DLIB_DBSETUP_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	(void)(uid);

	return ERR_FAIL;
}

HERROR			DLIB_DBSETUP_Compress (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// ������ ���� ��, ȿ���� ���̱� ���� ������ �ʿ��� ��� �� �Լ� ȣ���
	// ���� ����� ��¥ ������ �ƴ�, �׳� string�� binary�� �ٲٴ� ��
	// �ƹ�ư �����͸� ���̱⸸ �ϸ� ��
	// dst�� �޸� �Ҵ��� �ؼ� �����ؾ� �� (HxSTD_MemXXX�� ���)

	return ERR_FAIL;
}

HERROR			DLIB_DBSETUP_Extract (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// ����� ������ ����
	// dst�� �޸� �Ҵ��� �ؼ� �����ؾ� �� (HxSTD_MemXXX�� ���)

	return ERR_FAIL;
}


HERROR			DLIB_DBSETUP_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type)
{
	return ERR_FAIL;
}

void			DLIB_DBSETUP_FreeType (void *type)
{
}




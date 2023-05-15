/* $Header: $ */

#ifndef SIxTEXTDECODER_H__
#define SIxTEXTDECODER_H__

#ifndef _____INCLUDED_ONLY_IN_SILIB_H____
#error The file [sxtextdecoder.h] shall not be included except silib.h
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "hlib.h"


typedef enum
{
	eSIxTEXT_ENC_DEFAULT,
	eSIxTEXT_ENC_300468,					/** < 300-468 �� ���� ����Ǵ� encoding type	*/
	eSIxTEXT_ENC_300468_UK,					/** < 300-468 base�� text�� NULL ���� ���� ��� �� �� �ִ�, For UK DTT + UK DTT HD + UK FSAT + Decompress, Let's delete UKDTT after all implementation */
	eSIxTEXT_ENC_300468_8859_9_BASE,		/** < code table�� ������ 8859-9�� ��ü�Ǵ� �� �ܿ��� 300-468�� ����ȴ� */
	eSIxTEXT_ENC_JIS,						/** < japan���� si�� ������ String. */

	eSIxTEXT_ENC_UNKNOWN
} SIxTextEncoding_e;


void	SILIB_TEXT_Free(void *p);

HCHAR *	SILIB_TEXT_MakeUtf8 (
			  SIxTextEncoding_e enc
			, const HCHAR *langCode
			, const HUINT8 *sitext
			, HUINT32 textLen
		);

HCHAR *	SILIB_TEXT_MakeUtf8Ex (
			  SIxTextEncoding_e enc
			, const HCHAR *langCode
			, const HUINT8 *sitext
			, HUINT32 textLen
			, HCHAR *(* result)(const HCHAR *, void *)
			, void *userdata
		);



#ifdef __cplusplus
}
#endif

#endif // SIxTEXTDECODER_H__




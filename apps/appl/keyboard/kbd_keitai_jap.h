/*
	copyright by ccash@humaxdigital.com

	Japanese SMS type Virtual Keyboard -
	C- Source code.

	�Ϻ��� �޴��� �Է� ����� ��ö Ű���带 ����
	Ű ó�� ��ƾ.
*/


#ifndef	__KBD_KEITAI_JAP_H__
#define	__KBD_KEITAI_JAP_H__


/* ���� ���� ���� ������ϵ��� include ���� ����.  ������ copy & paste ���� ���� ������ �ϸ� �ڵ�����. */

#include	<ondk.h>
#include	<hapi.h>


/*
	���⿡ ��� ���Ϸ� �̾� ���� �͵��� ������ ���´�.
*/
#define	MAX_NEW_HIRAGANA	120		/* ���� ��ȯ�� �Է��� ���󰡳� �Է� ���� = 3*40 = 120 bytes */



typedef	int	KEYCODE;

/* Ű���� �Է��� ���� ��ų �� ȣ��Ǵ� �ݹ��Լ�. 			*/
/*	Ű����� �Էµ� ���ڿ��� complete_string �� �����ϰ�,  	*/
/*	Ű���带 ȣ�� �� Application ������ ���� ���δ� apply_change �� ���� �Ѵ�. */
typedef	void	(*KBDCallback_Done)(HUINT8 *complete_string, HBOOL apply_change);


typedef	enum
{	KBD_NUMERIC_MODE,
	KBD_ALPHABET_MODE,
	KBD_KATAKANA_MODE,
	KBD_KANJI_MODE,
	KBD_CANDIDATE_SELECT_MODE,	/* ���� ��ȯ �� = �ĺ� ���� ��� */
} SMS_INPUT_MODE;

typedef	enum
{	KBD_NUMERIC_AVAILABLE = 0x01,
	KBD_ALPHABET_AVAILABLE = 0x02,
	KBD_KATAKANA_AVAILABLE = 0x04,
	KBD_KANJI_AVAILABLE = 0x08,
} SMS_INPUT_AVAILABLE;

#define	KBD_ALL_AVAILABLE 	( KBD_NUMERIC_AVAILABLE | KBD_ALPHABET_AVAILABLE | KBD_KATAKANA_AVAILABLE | KBD_KANJI_AVAILABLE )



/* ���� ��带 ������ �ش�. */
void		KBD_SetComposeMode(SMS_INPUT_MODE mode);
/* ���� ������ ����� �����Ѵ�. */
void		KBD_SetAvailableInputMode(SMS_INPUT_AVAILABLE available_mode);
/* �������ڿ��� �� �Է� ������ ���� ��ġ �� ������ �ش�. */
void		KBD_SetInputPosition(int pos);
/* ���� ���ڿ��� MAX ���۸� ������ �ش�. */
void		KBD_SetBufferMaxLength(int max_length);
/* ������ ���ڿ��� ������ �ش�. */
void		KBD_SetStringToEdit(HUINT8 *string);

/* ������ ���ڿ��� ������ �ش�. */
int 		KBD_SetEditWindow(ONDKFontHandle font, int x, int y, int w, int h);

/* ������ �Ϸ�Ǹ� �Ϸ�� ���ڸ� ������ �ݹ��Լ��� ����Ѵ�. */
void		KBD_RegisterCallBack(KBDCallback_Done function);
/* ���� �Է����� ��带 �˷� �ش�. - ����/īŸī��/����/���� - �Է¸�� ǥ�� ��. */
SMS_INPUT_MODE	KBD_GetIMEMode(void);
SMS_INPUT_AVAILABLE	KBD_GetAvailableIMEMode(void);

/* ���� ��ȯ���� (���󰡳�) ���ڿ��� ���̸� ���´�. */
int		KBD_GetComposeStringLength(void);
/* ���� ��ȯ���� (���󰡳�) ���ڿ��� ��� �´�. */
HUINT8	*KBD_GetComposeString(void);
/* ���� ��ȯ�� ���� �ĺ��� ������ ��� �´�. */
int		KBD_GetCandidateNumber(void);
/* �ĺ� ����Ʈ ��, ���õ� �ĺ��� �ε����� ��´�.  */
int		KBD_GetSelectedCandidate(void);

/* ���� ��ȯ�� ���� index ��°�� �ĺ� ���ڿ�(UTF8)�� ��� �´�. */
HUINT8*	KBD_GetCandidatestring(int index);



/* Ű���� �Է� ó���� ���� Ű �ڵ带 �����Ѵ�. */
HInputRetType_e 	KBD_KeitaiInput(int keycode, int type);
/* ���� ���� (�Է�/���� ����) ���ڿ��� �����ش�. */
int		KBD_DisplayEditingString(ONDKSurfaceHandle surface, ONDKFontHandle font, int x, int y, int w, int h, ONDK_Color_t color, HBOOL drawCaret);


#endif		//__KBD_KEITAI_JAP_H__


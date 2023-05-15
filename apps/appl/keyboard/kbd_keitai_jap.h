/*
	copyright by ccash@humaxdigital.com

	Japanese SMS type Virtual Keyboard -
	C- Source code.

	일본어 휴대폰 입력 방식의 버철 키보드를 위한
	키 처리 루틴.
*/


#ifndef	__KBD_KEITAI_JAP_H__
#define	__KBD_KEITAI_JAP_H__


/* 제발 쓸데 없는 헤더파일들은 include 하지 말자.  무조건 copy & paste 하지 말고 정리좀 하며 코딩하자. */

#include	<ondk.h>
#include	<hapi.h>


/*
	여기에 헤더 파일로 뽑아 놓을 것들을 정리해 놓는다.
*/
#define	MAX_NEW_HIRAGANA	120		/* 한자 변환을 입력할 히라가나 입력 버퍼 = 3*40 = 120 bytes */



typedef	int	KEYCODE;

/* 키보드 입력을 종료 시킬 때 호출되는 콜백함수. 			*/
/*	키보드로 입력된 문자열은 complete_string 에 설정하고,  	*/
/*	키보드를 호출 한 Application 으로의 전달 여부는 apply_change 로 전달 한다. */
typedef	void	(*KBDCallback_Done)(HUINT8 *complete_string, HBOOL apply_change);


typedef	enum
{	KBD_NUMERIC_MODE,
	KBD_ALPHABET_MODE,
	KBD_KATAKANA_MODE,
	KBD_KANJI_MODE,
	KBD_CANDIDATE_SELECT_MODE,	/* 한자 변환 중 = 후보 선택 모드 */
} SMS_INPUT_MODE;

typedef	enum
{	KBD_NUMERIC_AVAILABLE = 0x01,
	KBD_ALPHABET_AVAILABLE = 0x02,
	KBD_KATAKANA_AVAILABLE = 0x04,
	KBD_KANJI_AVAILABLE = 0x08,
} SMS_INPUT_AVAILABLE;

#define	KBD_ALL_AVAILABLE 	( KBD_NUMERIC_AVAILABLE | KBD_ALPHABET_AVAILABLE | KBD_KATAKANA_AVAILABLE | KBD_KANJI_AVAILABLE )



/* 편집 모드를 설정해 준다. */
void		KBD_SetComposeMode(SMS_INPUT_MODE mode);
/* 편집 가능한 대상을 지정한다. */
void		KBD_SetAvailableInputMode(SMS_INPUT_AVAILABLE available_mode);
/* 편집문자열의 새 입력 문자의 삽입 위치 를 설정해 준다. */
void		KBD_SetInputPosition(int pos);
/* 편집 문자열의 MAX 버퍼를 설정해 준다. */
void		KBD_SetBufferMaxLength(int max_length);
/* 편집할 문자열을 설정해 준다. */
void		KBD_SetStringToEdit(HUINT8 *string);

/* 편집할 문자열을 설정해 준다. */
int 		KBD_SetEditWindow(ONDKFontHandle font, int x, int y, int w, int h);

/* 편집이 완료되면 완료될 문자를 전달할 콜백함수를 등록한다. */
void		KBD_RegisterCallBack(KBDCallback_Done function);
/* 현재 입력중인 모드를 알려 준다. - 한자/카타카나/영문/숫자 - 입력모드 표시 용. */
SMS_INPUT_MODE	KBD_GetIMEMode(void);
SMS_INPUT_AVAILABLE	KBD_GetAvailableIMEMode(void);

/* 현재 변환중인 (히라가나) 문자열의 길이를 얻어온다. */
int		KBD_GetComposeStringLength(void);
/* 현재 변환중인 (히라가나) 문자열을 얻어 온다. */
HUINT8	*KBD_GetComposeString(void);
/* 한자 변환을 위한 후보의 갯수를 얻어 온다. */
int		KBD_GetCandidateNumber(void);
/* 후보 리스트 중, 선택된 후보의 인덱스를 얻는다.  */
int		KBD_GetSelectedCandidate(void);

/* 한자 변환을 위한 index 번째의 후보 문자열(UTF8)을 얻어 온다. */
HUINT8*	KBD_GetCandidatestring(int index);



/* 키보드 입력 처리를 위해 키 코드를 전달한다. */
HInputRetType_e 	KBD_KeitaiInput(int keycode, int type);
/* 편집 중인 (입력/조합 중인) 문자열을 보여준다. */
int		KBD_DisplayEditingString(ONDKSurfaceHandle surface, ONDKFontHandle font, int x, int y, int w, int h, ONDK_Color_t color, HBOOL drawCaret);


#endif		//__KBD_KEITAI_JAP_H__


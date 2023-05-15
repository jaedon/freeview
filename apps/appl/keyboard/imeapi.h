/*******************************************************************************
*
* ImeAPI.h 
*
* Revision 1.00 - 2005-03-28
*	- Implemented & modified by JK Baek
*
********************************************************************************/

#ifndef	IME_API_H
#define	IME_API_H
#include	<htype.h>

#define STANDARD_CONFIG_USE_RESOURCE_JAPANESE

#define MiteiMax						50
#define BunsetuMax					MiteiMax * 2

struct T_IME_ {
	char 			fTmp[BunsetuMax];
	char 			fCandidate[BunsetuMax];
	char 			fContinuation[BunsetuMax];
};
typedef struct T_IME_ 	T_IME;

typedef struct IME_ {
	char				*text;
	struct IME_		*pNext;
} IME_ELEM;

HBOOL				IME_Init(void);
HBOOL				IME_Destroy(void);
HBOOL				IME_AddItem(HUINT8 *inText, HINT32 inLength);
HBOOL				IME_DeleteItem(void);
HUINT8 *			IME_GetItemText(HINT32 index);
HINT32				IME_GetItemNum(void);
HINT32				IME_GetCurrentSelectedItemIndex(void);
void					IME_SetCurrentSelectedItemIndex(HINT32 selected_index);

HBOOL 			HIME_Initialize(T_IME *self);
HBOOL 			HIME_Finalize(T_IME *self);
HINT32 				HIME_StartConversion(T_IME *self, HUINT8 *in_string);
HINT32 				HIME_LengthenCandidate(T_IME *self);
HINT32 				HIME_ShortenCandidate(T_IME *self);
HUINT8 *			HIME_NextCandidate(T_IME *self, HBOOL in_buffer);
HINT32 				HIME_ConvertKatakanaCandidate(T_IME *self, HUINT8 *outStr);
HINT32 				HIME_ConvertHiraganaCandidate(T_IME *self, HUINT8 *outStr);
HINT32 				HIME_ConvertHankakuCandidate(T_IME *self, HUINT8 *outStr);
HINT32 				HIME_ConvertZenkakuCandidate(T_IME *self, HUINT8 *outStr);
HINT32 				HIME_NextWord(T_IME *self);

HUINT8 *			HIME_ContinuationString(T_IME *self);
HUINT8 *			HIME_CandidateString(T_IME *self);

#endif	/* IME_API_H */




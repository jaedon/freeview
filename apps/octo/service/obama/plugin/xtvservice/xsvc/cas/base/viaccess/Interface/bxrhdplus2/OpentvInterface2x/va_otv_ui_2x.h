/*
    COPYRIGHT VIACCESS S.A.
    date        : 06/02/2006
    file        : va_otv_ui_2x.h
    Description : It is the Viaccess'API of CAS functionalities

    $Revision: 1 $
*/

#ifndef _VA_OTV_UI_2X_H_
#define _VA_OTV_UI_2X_H_


#include "va_otv_ui_type2x.h"

/*************/
/*   Macros  */
/*************/
#define o_VA_GetTheMessage(_msg)        (((tO_VA_UI_OpenTvMessage*)(_msg))->uiMessage)

#define o_VA_GetTheRequestPtr(_msg)     (((tO_VA_UI_OpenTvMessage*)(_msg))->info.request.ptr)
#define o_VA_GetTheProgression(_msg)    (((tO_VA_UI_OpenTvMessage*)(_msg))->info.request.dwProgression)

#define o_VA_GetTheSlotId(_msg)         (((tO_VA_UI_OpenTvMessage*)(_msg))->info.event.bScSlot)
#define o_VA_GetTheAcsId(_msg)          (((tO_VA_UI_OpenTvMessage*)(_msg))->info.event.bAcsId)
#define o_VA_GetTheScrambledType(_msg)      (((tO_VA_UI_OpenTvMessage*)(_msg))->info.event.bScrambledType)
#define o_VA_GetTheStreamHandle(_msg)  (((tO_VA_UI_OpenTvMessage*)(_msg))->info.event.dwStbStreamHandle)
#define o_VA_GetTheQevStatus(_msg)  (((tO_VA_UI_OpenTvMessage*)(_msg))->info.event.bCamlockStatus)


/*************/
/* Functions */
/*************/

INT o_VA_SetDefaultFocus(void);
INT o_VA_ResetFocus(void);

INT  o_VA_InitializeAcs(const tO_VA_INIT_ConfigurationParameters  *pParameters);
INT  o_VA_StopAcs(void);
void o_VA_StartAcs(void);



#endif /* _VA_OTV_UI_2X_H_ */

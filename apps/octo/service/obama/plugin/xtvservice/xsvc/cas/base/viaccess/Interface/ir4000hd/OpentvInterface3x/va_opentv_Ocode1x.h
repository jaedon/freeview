/*
    COPYRIGHT VIACCESS S.A.
    date        : 30/06/2006
    file        : va_opentv_Ocode1x.h
    Description : It is the Viaccess'API of CAS functionalities

*/

#ifndef _VA_OPENTV_O_CODE_1X_H_
#define _VA_OPENTV_O_CODE_1X_H_

#include "va_opentv_Otype1x.h"

extern int  VA_request(va_common_request * requete);
extern void VA_set_focus(va_msg_set_t msg_set, va_focus_t focus_flag);

#endif /* _VA_OPENTV_O_CODE_1X_H_ */

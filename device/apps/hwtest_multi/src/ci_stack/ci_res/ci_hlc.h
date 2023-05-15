
/***************************************************************
* $Workfile:   ci_hlc.h  $
* $Modtime:
* Auther : hmkim
*
* Desc : Host Language & Country (CI+)
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
****************************************************************/

#ifndef _CI_HLC_H_
#define _CI_HLC_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************
* include
***************************************************************/
#include "ci_plus.h"


/***************************************************************
* definition
***************************************************************/


/***************************************************************
* typedef
***************************************************************/
typedef struct {
	unsigned short	usSsNo;
	CI_SLOT_ID		usSlotId;
} CI_HLC_INSTANCE;


/***************************************************************
* global function prototypes
***************************************************************/
int CiHlc_Init(void);
int CiHlc_SsOpenedProc(unsigned short usSsNo);
int CiHlc_SsClosedProc(unsigned short usSsNo);
int CiHlc_ApduProc(unsigned short usSsNo, unsigned long ulApduTag,
					unsigned char *pucApduData, unsigned long ulApduLen);

int CiHlc_NotifyHostCountryInfoChanged(CI_SLOT_ID usSlotId);
int CiHlc_NotifyHostLanguageInfoChanged(CI_SLOT_ID usSlotId);


#ifdef __cplusplus
}
#endif

#endif /* _CI_HLC_H_ */

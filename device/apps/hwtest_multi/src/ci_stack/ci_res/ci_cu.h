
/***************************************************************
* $Workfile:   ci_cu.h  $
* $Modtime:
* Auther : hmkim
*
* Desc : CAM Firmware Upgrade (CI+)
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
****************************************************************/

#ifndef _CI_CU_H_
#define _CI_CU_H_

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
#define CI_CU_UNKNOWN_DOWNLOAD_DURATION	0x0000
#define CI_CU_TIMEOUT						60000 /* CAM Firmware Upgrade timeout value in ms : CI+ 스펙 1.0 에선 30초였으나 1.1 이후에선 60초로 변경됨 */


/***************************************************************
* typedef
***************************************************************/

/* CI_CAM_UPGRADE 의 파라미터 : upgrade types */
typedef enum {
	CI_CU_UPGRADE_TYPE_DELAYED = 0x00,
	CI_CU_UPGRADE_TYPE_IMMEDIATE = 0x01
} CI_CU_UPGRADE_TYPE;

/* CI_CAM_UPGRADE 에 대한 응답 (CI_CAM_UPGRADE_REPLY) 파라미터 : host answers

   호스트 입장에서 answer 선택은 다음과 같은 룰을 따라야 한다 :
   1. CI_HOST_MODE_NORMAL (일반적인 상황)에서는 CI_CU_UPGRADE_TYPE_DELAYED, CI_CU_UPGRADE_TYPE_IMMEDIATE 에 상관없이 CI_CU_UPGRADE_ANSWER_ASK 로 응답해야 한다.
   2. CI_HOST_MODE_UNATTENDED (예:PVR 모델에서 예약 레코딩 중인 상황)에서는 CI_CU_UPGRADE_TYPE_DELAYED 를 받은 경우에 한해서 CI_CU_UPGRADE_ANSWER_NO 로 응답해도 무방.
      (단, 이 경우라도 CI_CU_UPGRADE_TYPE_IMMEDIATE 를 받았으면 CI_CU_UPGRADE_ANSWER_YES 로 응답해야 한다)
   3. CI_HOST_MODE_SERVICE (기타의 상황)에서는 CI_CU_UPGRADE_TYPE_DELAYED, CI_CU_UPGRADE_TYPE_IMMEDIATE 에 상관없이 CI_CU_UPGRADE_ANSWER_YES 로 응답해야 한다.
*/
typedef enum {
	CI_CU_UPGRADE_ANSWER_NO = 0x00,
	CI_CU_UPGRADE_ANSWER_YES = 0x01,	/* 이 응답을 하면 캠에서 HCI 리소스 세션을 열 것이다 */
	CI_CU_UPGRADE_ANSWER_ASK = 0x02		/* 이 응답을 하면 캠에서 MMI를 띄울 것이다 */
} CI_CU_UPGRADE_ANSWER;

/* CI_CAM_UPGRADE_COMPLETE 의 파라미터 : reset requests */
typedef enum {
	CI_CU_RESET_REQUEST_PCMCIA = 0x00,	/* set RESET signal */
	CI_CU_RESET_REQUEST_CAM = 0x01,		/* set RS flag */
	CI_CU_RESET_REQUEST_NO_RESET = 0x02
} CI_CU_RESET_REQUEST;

/* CU instance */
typedef struct {
	unsigned short	usSsNo;
	CI_SLOT_ID		usSlotId;
	unsigned long	ulTimerID;
} CI_CU_INSTANCE;


/***************************************************************
* global function prototypes
***************************************************************/
int CiCu_Init(void);
int CiCu_SsOpenedProc(unsigned short usSsNo);
int CiCu_SsClosedProc(unsigned short usSsNo);
int CiCu_ApduProc(unsigned short usSsNo, unsigned long ulApduTag,
					unsigned char *pucApduData, unsigned long ulApduLen);


#ifdef __cplusplus
}
#endif

#endif /* _CI_CU_H_ */

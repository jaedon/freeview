
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
#define CI_CU_TIMEOUT						60000 /* CAM Firmware Upgrade timeout value in ms : CI+ ���� 1.0 ���� 30�ʿ����� 1.1 ���Ŀ��� 60�ʷ� ����� */


/***************************************************************
* typedef
***************************************************************/

/* CI_CAM_UPGRADE �� �Ķ���� : upgrade types */
typedef enum {
	CI_CU_UPGRADE_TYPE_DELAYED = 0x00,
	CI_CU_UPGRADE_TYPE_IMMEDIATE = 0x01
} CI_CU_UPGRADE_TYPE;

/* CI_CAM_UPGRADE �� ���� ���� (CI_CAM_UPGRADE_REPLY) �Ķ���� : host answers

   ȣ��Ʈ ���忡�� answer ������ ������ ���� ���� ����� �Ѵ� :
   1. CI_HOST_MODE_NORMAL (�Ϲ����� ��Ȳ)������ CI_CU_UPGRADE_TYPE_DELAYED, CI_CU_UPGRADE_TYPE_IMMEDIATE �� ������� CI_CU_UPGRADE_ANSWER_ASK �� �����ؾ� �Ѵ�.
   2. CI_HOST_MODE_UNATTENDED (��:PVR �𵨿��� ���� ���ڵ� ���� ��Ȳ)������ CI_CU_UPGRADE_TYPE_DELAYED �� ���� ��쿡 ���ؼ� CI_CU_UPGRADE_ANSWER_NO �� �����ص� ����.
      (��, �� ���� CI_CU_UPGRADE_TYPE_IMMEDIATE �� �޾����� CI_CU_UPGRADE_ANSWER_YES �� �����ؾ� �Ѵ�)
   3. CI_HOST_MODE_SERVICE (��Ÿ�� ��Ȳ)������ CI_CU_UPGRADE_TYPE_DELAYED, CI_CU_UPGRADE_TYPE_IMMEDIATE �� ������� CI_CU_UPGRADE_ANSWER_YES �� �����ؾ� �Ѵ�.
*/
typedef enum {
	CI_CU_UPGRADE_ANSWER_NO = 0x00,
	CI_CU_UPGRADE_ANSWER_YES = 0x01,	/* �� ������ �ϸ� ķ���� HCI ���ҽ� ������ �� ���̴� */
	CI_CU_UPGRADE_ANSWER_ASK = 0x02		/* �� ������ �ϸ� ķ���� MMI�� ��� ���̴� */
} CI_CU_UPGRADE_ANSWER;

/* CI_CAM_UPGRADE_COMPLETE �� �Ķ���� : reset requests */
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

/***************************************************************
* $Workfile:   ci_cc_main.h  $
* $Modtime:
* Auther : hmkim
*
* Desc : Content Control (CI+) Main
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
****************************************************************/

#ifndef _CI_CC_MAIN_H_
#define _CI_CC_MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************
* include
***************************************************************/
#include "ci_plus.h"

/***************************************************************
* global function prototypes
***************************************************************/
/* CI CC (Content Control Resource) */
int CiCc_Init(void);
int CiCc_SsOpenedProc(unsigned short usSsNo);
int CiCc_SsClosedProc(unsigned short usSsNo);
int CiCc_ApduProc(unsigned short usSsNo, unsigned long ulApduTag, unsigned char *pucApduData, unsigned long ulApduLen);

int CiCc_GetHostId(unsigned char *pucHostId); // �� �Լ��� ȣ���� CiCc_Task() �⵿�� �Ϸ�� �Ŀ��� ��ȿ�� ����� �ܿ� ������ ��
int CiCc_GetBindedCamId(CI_SLOT_ID usSlotId, unsigned char *pucBindedCamId); // �� �Լ��� ȣ���� ķ ������ ���� �ð� ����� �Ŀ��� ��ȿ�� ����� �ܿ� ������ ��
int CiCc_GetBindedCamBrandId(CI_SLOT_ID usSlotId, unsigned short *pusBindedCamBrandId); // �� �Լ��� ȣ���� ķ ������ ���� �ð� ����� �Ŀ��� ��ȿ�� ����� �ܿ� ������ ��
int CiCc_GetNegotiatedCckCipher(CI_SLOT_ID usSlotId, CI_CC_KEY_CIPHER *penNegotiatedCckCipher); // �� �Լ��� ȣ���� ķ ������ ���� �ð� ����� �Ŀ��� ��ȿ�� ����� �ܿ� ������ ��
int CiCc_SetNegotiatedCckCipher(CI_SLOT_ID usSlotId, CI_CC_KEY_CIPHER enNegotiatedCckCipher); // �׽�Ʈ �뵵�� �Լ���. �ٸ� �뵵�� ������� �� ��!
void CiCc_ReqSetCckToDsc(CI_SLOT_ID usSlotId); // �� �Լ��� ȣ���� ķ ������ ���� �ð� ����� �Ŀ��� ��ȿ�� ����� �ܿ� ������ ��
void CiCc_SetUriDefault(CI_SLOT_ID usSlotId, HBOOL bOn); // �� �Լ��� ȣ���� ķ ������ ���� �ð� ����� �Ŀ��� ��ȿ�� ����� �ܿ� ������ ��
HERROR CiCc_IsInitialized(void); // �� �Լ��� ȣ���� CiCc_Task() �⵿�� �Ϸ�� �Ŀ��� ��ȿ�� ����� �ܿ� ������ ��
int CiCc_IsOpened(CI_SLOT_ID usSlotId); // �� �Լ��� ȣ���� ķ ������ ���� �ð� ����� �Ŀ��� ��ȿ�� ����� �ܿ� ������ ��
int CiCc_IsBusy(CI_SLOT_ID usSlotId); // �� �Լ��� ȣ���� ķ ������ ���� �ð� ����� �Ŀ��� ��ȿ�� ����� �ܿ� ������ ��

#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
HULONG CiCc_GetCcResourceVersionBySlot(CI_SLOT_ID usSlotId);
int CiCc_PrepareRecordStart(CI_SLOT_ID usSlotId, Handle_t hAction, CI_OPERATING_MODE eOperatingMode, unsigned short usProgramNumber);
int CiCc_ChangeOperatingMode(CI_SLOT_ID usSlotId, unsigned int unSessionId, CI_OPERATING_MODE enOperatingMode, unsigned short usProgramNumber);
int CiCc_GetRecordingStatus(CI_SLOT_ID usSlotId, HUINT8 *pucCcRecordingStatus);
int CiCc_RecordStop(CI_SLOT_ID usSlotId, unsigned short usProgramNumber);
//int CiCc_Playback(CI_SLOT_ID usSlotId, unsigned int unSessionId, unsigned short usProgramNumber, unsigned char *pucLicenseData, unsigned short usLicenseDataLen);
int CiCc_CheckLicense(CI_SLOT_ID usSlotId, unsigned int unSessionId, unsigned char *pucLicenseData, unsigned short usLicenseDataLen);
int CiCc_GetPinCapaInfo(CI_SLOT_ID usSlotId, CI_PIN_CAPA_INFO *pstPinCapaInfo); // �� �Լ��� ȣ���� ķ ������ ���� �ð� ����� �Ŀ��� ��ȿ�� ����� �ܿ� ������ ��
int CiCc_StoreCicamPinInfo(CI_SLOT_ID usSlotId, HUINT8 *pucPinInfo, UNIXTIME utPinSavedTime);
int CiCc_GetUriInfo(CI_SLOT_ID usSlotId, CIPLUS_UriInfo_t *pstUriInfo);
int CiCc_PinPlayback(CI_SLOT_ID usSlotId, HUINT32 unMetaIndex, Handle_t hPbAction, HUINT8 ucAgeLimit, HUINT8 *pucPrivateData);
int CiCc_SendHostLicense(CI_SLOT_ID usSlotId, HUINT32 unMetaIndex, Handle_t hPbAction, HUINT16 usProgramNum, HUINT8 *pucLicenseData, HUINT16 usLicenseDataLen);
#endif

// for CI+ v1.3 spec Operator Profile...
int CiCc_SetOperatorProfileSupportInfo(CI_SLOT_ID usSlotId, HUINT8 ucNitVer, HUINT8 ucInfoVer);
int CiCc_GetAuthContextBySlotId(CI_SLOT_ID usSlotId, CI_AUTH_CTX 	*pstCiAuthCtx);
HBOOL CiCc_CheckCcAuthenticationComplete(HUINT16 usSlotId);

void CiCc_PrintDebugInfo(CI_SLOT_ID usSlotId, int flagPrintMoreDetail);


#ifdef __cplusplus
}
#endif

#endif /* _CI_CC_MAIN_H_ */

/***************************************************************
* $Workfile:   ci_op.h  $
* $Modtime:
* Auther : kkkim / hmkim
*
* Desc : Operator Profile (CI+ v1.3)
*
* Copyright (c) 2011 by Humax Co., Ltd.
* All right reserved
****************************************************************/

#ifndef _CI_OP_H_
#define _CI_OP_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************
* include
***************************************************************/


/***************************************************************
* global function prototypes
***************************************************************/
/* CI OP (Operator Profile) */
int CiOp_Init(void);
int CiOp_SsOpenedProc(unsigned short usSsNo);
int CiOp_SsClosedProc(unsigned short usSsNo);
int CiOp_ApduProc(unsigned short usSsNo, unsigned long ulApduTag, unsigned char *pucApduData, unsigned long ulApduLen);

int CiOp_SendOperatorSearchStart(CI_SLOT_ID usSlotId);
int CiOp_GetOperatorReservedTime(HUINT16 usSlotId, UNIXTIME *putTime);
HBOOL CiOp_CheckSimpleEntitlementChange(CI_SLOT_ID usSlotId);
int CiOp_SendOperatorExitBySlotId(CI_SLOT_ID usSlotId);
int CiOp_SendOperatorStatusReqBySlotId(CI_SLOT_ID usSlotId);
HBOOL CiOp_GetOperatorProfileErrStatus(CI_SLOT_ID usSlotId);

#ifdef __cplusplus
}
#endif

#endif /* _CI_OP_H_ */

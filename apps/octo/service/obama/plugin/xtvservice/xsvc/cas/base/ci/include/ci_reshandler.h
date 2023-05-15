/* $Header:   //BASE/archives/STING_T/mw/cas/ci/ci_res/ci_reshandler.h-arc   1.0   17 Dec 2004 10:32:34   jhkim1  $ */
/***************************************************************
* $Workfile:   ci_reshandler.h  $
* $Modtime:   13 Dec 2004 09:27:32  $
* Auther : Jaehee Cho / hmkim (CI+)
*
* Desc :
*
* Copyright (c) 2002 by Humax Co., Ltd.
* All right reserved
****************************************************************/

#ifndef _CI_RESHANDLER_H_
#define _CI_RESHANDLER_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************
*
* include
*
***************************************************************/
#include "ci_res.h"


/***************************************************************
*
* typedef
*
***************************************************************/

typedef unsigned char CI_STACK_SS_STATE;
/* in dvb_cod.h
session_status values
#define SS_OK				0x00
#define SS_NOTALLOCATED		0xf0
#define SS_NOTAVAILABLE		0xf1
#define SS_BADVERSION		0xf2
#define SS_BUSY				0xf3
*/

typedef enum {
	CI_SS_NONE = 0,
	CI_SS_REQESTED_OPEN,
	CI_SS_OPENED
} CI_SS_STATE;

#if 0
/**************************
* CI Resource ID version 1
**************************/
#ifdef __BIG_ENDIAN__
	typedef struct {			// *** Public Resource ID ***
		int bitIdType : 2;		// resource id type : 0, 1, 2
		int bitClass : 14;		// resource class
		int bitType : 10;		// resource type
		int bitVer : 6;			// resource version
	} CI_PUB_RES_ID_V1;

	typedef struct {			// *** Private Resource ID ***
		int bitIdType : 2;		// resource id type : 3
		int bitDefiner : 10;	// resource definer
		int bitIdentity : 20;	// resource identity
	} CI_PRI_RES_ID_V1;
#else
	typedef struct {
		int bitVer : 6;
		int bitType : 10;
		int bitClass : 14;
		int bitIdType : 2;
	} CI_PUB_RES_ID_V1;

	typedef struct {
		int bitIdentity : 20;
		int bitDefiner : 10;
		int bitIdType : 2;
	} CI_PRI_RES_ID_V1;
#endif	// __BIG_ENDIAN__

/**************************
* CI Resource ID version 2
**************************/
#ifdef __BIG_ENDIAN__
	typedef struct {			// *** Public Resource ID ***
		int bitIdType : 2;		// resource id type : 0, 1, 2
		int bitClass : 14;		// resource class
		int bitType : 4;		// rsource type
		int bitInst : 6;		// resource instance. generally means module ID
		int bitVer : 6;			// resource version
	} CI_PUB_RES_ID_V2;

	typedef struct {			// *** Private Resource ID ***
		int bitIdType : 2;		// resource id type :3
		int bitAuth : 4;		// resource authority
		int bitDefiner : 12;	// resource definer
		int bitIdentity : 14;	// resource identity
	} CI_PRI_RES_ID_V2;
#else
	typedef struct {
		int bitVer : 6;
		int bitInst : 6;
		int bitType : 4;
		int bitClass : 14;
		int bitIdType : 2;
	} CI_PUB_RES_ID_V2;

	typedef struct {
		int bitIdentity : 14;
		int bitDefiner : 12;
		int bitAuth : 4;
		int bitIdType : 2;
	} CI_PRI_RES_ID_V2;
#endif	// __BIG_ENDIAN__
#endif

typedef union {
	unsigned long 		ulId;		// if Resource is not exist, 0x00
	#if 0
	CI_PUB_RES_ID_V1	ulPubIdV1;
	CI_PRI_RES_ID_V1	ulPriIdV1;
	CI_PUB_RES_ID_V2	ulPubIdV2;
	CI_PRI_RES_ID_V2	ulPriIdV2;
	#endif
} CI_RES_ID;


/**********************************
* CI Resource Handler's Callbacks
**********************************/
/********************************************************************
 Function   :	CI_CALLBACK_SS_OPENED
 Description :	called by Resource Handler to init resource
 Input      :	usSsNo - Session Number
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
typedef int (*CI_CALLBACK_INIT)(void);

/********************************************************************
 Function   :	CI_CALLBACK_SS_OPENED
 Description :	called by Resource Handler whenever session of resource is opened
 Input      :	usSsNo - Session Number
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
typedef int (*CI_CALLBACK_SS_OPENED)(unsigned short usSsNo);

/********************************************************************
 Function   :	CI_CALLBACK_SS_CLOSED
 Description :	called by Resource Handler whenever session of resource is closed
 Input      :	usSsNo - Session Number
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
typedef int (*CI_CALLBACK_SS_CLOSED)(unsigned short usSsNo);

/********************************************************************
 Function   :	CI_CALLBACK_APDU
 Description :	called by Resource Handler whenever APDU data is received
 Input      :	usSsNo - Session Number
 				ulApduTag - APDU tag defined in CI
 				pucApduData - pointer of APDU data defined in CI
 				ulApduLen - length of APDU data
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
typedef int (*CI_CALLBACK_APDU)(unsigned short usSsNo, unsigned long ulApduTag, unsigned char *pucApduData, unsigned long ulApduLen);

typedef struct {							//*** CI Callback function list ***
	CI_CALLBACK_INIT			pfnCiInit;
	CI_CALLBACK_SS_OPENED		pfnCiSsOpened;
	CI_CALLBACK_SS_CLOSED		pfnCiSsClosed;
	CI_CALLBACK_APDU			pfnCiApdu;
} CI_RES_CB_LIST;

typedef struct {							//*** CI Resource Instance ***
	CI_RES_ID			uResId;				// Resource ID
	CI_SLOT_ID			usSrcId;			// CI Resource Source ID
											// 0~1 : CI Slot ID, 0x02~0xfe : reserved, 0xff : CI_HOST_RES
	unsigned char		ucResIdType;		// Resource ID Type : 0~2 for Public Resource ID, 3 for Private Resource ID : see CI_RES_PRI_TYPE
	unsigned short		usResClass;		// Resource Class. don't-care vlaue in case of Private Resource
	unsigned char		ucMaxSs;			// Max Number of Session
	unsigned char		ucSsCnt;			// Created session count
	CI_RES_CB_LIST		cbList;				// callback function list
} CI_RES_INST, *CI_RES_HANDLE;

typedef struct {							// CI TC Instance
	CI_SLOT_ID			usSlotId;			// CI Slot ID : 0,1
	unsigned char		ucTcId;				// CI TC ID. if TC is not exist, 0x00
} CI_TC_INST, *CI_TC_HANDLE;

typedef struct {							//*** CI Session Instance ***
	CI_SS_STATE				nState;			// Session State : SS_OK/SS_NOTALLOCATED
											//			  /SS_NOTAVAILABLE/SS_BADVERSION/SS_BUSY
	CI_TC_HANDLE			hTc;			// TC ID
	CI_RES_HANDLE			hRes;			// Resource handle
	unsigned short			usSsNo;			// Session number
	unsigned long			ulOpendSsResId;	// CAM���� session open ��û ���� �� Session Res ID�� ����. CI+v1.3 ���� �Ǵ��ϱ� ���� �ַ� ���.
} CI_SS_INST, *CI_SS_HANDLE;


/***************************************************************
* global function prototypes : CIRH (CI Resource Handler)
***************************************************************/

/********************************************************************
 Function   :	CIRH_Init
 Description :	initialize CIRH
  				Before call CIRH_Init(), you must initialize CI Resource supported by Host.
 				CI Initialization include registering callback to CIRH.
 				But when module is inserted, CI Resource supported by module is initialized.
 Input      :
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CIRH_Init(void);

/********************************************************************
 Function   :	CIRH_GetRes
 Description :	Get Resources from CIRH
 Input      :
 Output     :
 Return     : 	CI Resource Array
 ********************************************************************/
CI_RES_INST * CIRH_GetRes(void);

/********************************************************************
 Function   :	CIRH_AddRes
 Description :	Add(register) Resource to CIRH
 Input      :	ulResId - resource ID
 				usSrcId - Source ID
 				ucMaxSs - Max Number of Sessions
 				puCbList - pointer of callback function list
 Output     :
 Return     : 	CI Resource Handle allocated to CIRH (CI_RES_HANDLE)
 				NULL for Failure.
 ********************************************************************/
CI_RES_HANDLE CIRH_AddRes(unsigned long ulResId, CI_SLOT_ID usSrcId, unsigned char ucMaxSs, CI_RES_CB_LIST *pCbList);

/********************************************************************
 Function   :	CIRH_DeleteRes
 Description :	Delete(register) Resource from CIRH
 Input      :	hRes - resource handle
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CIRH_DeleteRes(CI_RES_HANDLE hRes);

/********************************************************************
 Function   :	CIRH_GetSlotIdBySsNo
 Description :	get a CI slot ID mapped usSsNo
 Input      :	usSsNo - Session Number
 Output     :	pusSlotId - pointer of CI slot ID
 Return     :	0 for OK, -1 for failure
 ********************************************************************/
int CIRH_GetSlotIdBySsNo(unsigned short usSsNo, CI_SLOT_ID *pusSlotId);

/********************************************************************
 Function   :	CIRH_GetSs
 Description :	Get CI Session Handle
 Input      :	usSsNo - Session Number
 Output     :
 Return     : 	CI Session Handle allocated to CIRH (CI_SS_HANDLE)
 				NULL for failure
 ********************************************************************/
CI_SS_HANDLE CIRH_GetSs(unsigned short usSsNo);

/********************************************************************
 Function   :	CIRH_GetSsFromRes
 Description :	Get CI Session Handle
 Input      :	usSlotId - Slot ID
 				ulResId - Resource ID
 Output     :
 Return     : 	CI Session Handle allocated to CIRH (CI_SS_HANDLE)
 				NULL for failure
 ********************************************************************/
CI_SS_HANDLE CIRH_GetSsFromRes(CI_SLOT_ID usSlotId, unsigned long ulResId);

/********************************************************************
 Function   :	CIRH_GetTcInfo
 Description :	Get CI TC Information
 Input      :	ucTcId - TC ID
 Output     :
 Return     : 	pointer of CI TC Information
 				NULL for failure
 ********************************************************************/
CI_TC_HANDLE CIRH_GetTc(unsigned char ucTcId);

/********************************************************************
 Function   :	CIRH_GetTcId
 Description :	get a CI TC ID mapped usSlotId
 Input      :	usSlotId - CI Slot ID
 Output     :	pucTcId - pointer of CI TC ID
 Return     :	0 for OK, -1 for failure
 ********************************************************************/
int CIRH_GetTcId(CI_SLOT_ID usSlotId, unsigned char *pucTcId);

/********************************************************************
 Function   :	CIRH_GetResCnt
 Description :	Get Number of Registered Resource in CIRH
 Input      :
 Output     :
 Return     : 	Number of Resource registered to CIRH
 ********************************************************************/
unsigned char CIRH_GetResCnt(void);

/********************************************************************
 Function   :	CIRH_ReqCloseSs
 Description :	Request CIRH to close session
 Input      :	usSsNo - session number
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CIRH_ReqCloseSs(unsigned short usSsNo);


/********************************************************************
 Function   :	CIRH_SendApdu
 Description :	Send APDU to CIRH
 Input      :	usSsNo - session number
 				pucApdu - pointer of APDU
 						must be freed by caller ??
 				ulApduLen - APDU length
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CIRH_SendApdu(unsigned short usSsNo, unsigned char *pucApdu, unsigned long ulApduLen);

/********************************************************************
 Function   :	CIRH_GetApduTag
 Description :	Get APDU tag from APDU
 Input      :	pucApdu - pointer of APDU
 Output     :
 Return     : 	APDU tag (apdu_tag in CI spec.)
 ********************************************************************/
unsigned long CIRH_GetApduTag(unsigned char *pucApdu);

/********************************************************************
 Function   :	CIRH_GetApduDataLength
 Description :	Get length of APDU data from APDU
 Input      :	pucApdu - pointer of APDU
 Output     :
 Return     : 	length of APDU Data
 ********************************************************************/
unsigned long CIRH_GetApduDataLength(unsigned char *pucApdu);

/********************************************************************
 Function   :	CIRH_GetApduData
 Description :	Get APDU Data from APDU
 Input      :	pucApdu - pointer of APDU
 Output     :
 Return     : 	pointer of APDU data
 ********************************************************************/
unsigned char * CIRH_GetApduData(unsigned char *pucApdu);

/********************************************************************
 Function   :	CIRH_ConvertApduLengthField
 Description :	Convert length value to length_field() in CI spec.
 Input      :	ulLen - length to be converted
 Output     :	pucLenField - pointer of length_field()
 				pucFieldSize - pointer of Field Size(byte count)
 Return     :
 ********************************************************************/
void CIRH_ConvertApduLengthField(unsigned long ulLen, unsigned char *pucLenField, unsigned char *pucFieldSize);

/********************************************************************
 Function   :	CIRH_DeleteTc
 Description :	Delete TC
 Input      :	usSlotId - Slot Id
 Output     :
 Return     :
 ********************************************************************/
int CIRH_DeleteTc(CI_SLOT_ID usSlotId);

/********************************************************************
 Function   :	CIRH_Task
 Description :	CIRH Task
 Input      :	pvParam - currently not used
 Output     :
 Return     :
 ********************************************************************/
void CIRH_Task(void *pvParam);

void CIRH_PrintRes(void);

#ifdef __cplusplus
}
#endif

#endif /* _CI_RESHANDLER_H_ */


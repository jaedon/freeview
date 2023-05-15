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
* definition
*
***************************************************************/

//*** CI Resources supported by host currently ***

#define	CI_RM_RES_V1_ID			0x00010041		// Resource Manager v1
#define	CI_RM_RES_V2_ID			0x00010042		// Resource Manager v2 : currently v1 APDUs only supported.
#define	CI_AI_RES_V1_ID			0x00020041		// Application Information v1
#define	CI_AI_RES_V2_ID			0x00020042		// Application Information v2
#define	CI_AI_RES_V3_ID			0x00020043		// Application Information v3 : CI+ mandatory.
#define	CI_CAS_RES_V1_ID			0x00030041		// Conditional Access Support
#define	CI_HCI_RES_V1_ID			0x00200041		// Host Control Interface : CI+ mandatory.
#define	CI_DT_RES_V1_ID			0x00240041		// Date-Time
#define	CI_MMI_RES_V1_ID			0x00400041		// MMI : currently HL-MMI only supported and it's enough.
#define	CI_AMMI_RES_V1_ID			0x00410041		// CI+ Browser Application MMI : CI+ mandatory.
#define CI_CC_RES_V1_ID			0x008c1001		// Content Control : CI+ mandatory.
#define CI_HLC_RES_V1_ID			0x008d1001		// Host Language & Country : CI+ mandatory.
#define CI_CU_RES_V1_ID			0x008e1001		// CAM Firmware Upgrade : CI+ mandatory.
#define CI_SAS_RES_V1_ID			0x00961001		// Specific Application Support for MHP : currently not supported.
#define CI_CAPVR_RES_V1_ID		0x00971001		// CA PVR : currently not supported.

#define	CI_AUTH_RES_V1_ID			0x00100041		// Authentication : used with _INCLUDE_BD_CI_ only
#define	CI_CP_RES_V2_ID			0x00041001		// Copy Protection : used with _INCLUDE_BD_CI_ only -> CP 리소스는 CC 리소스와 함께 사용될 수는 있으나 동시에 세션을 오픈해서는 안된다 (CI+ 스펙)

//*** Other definitions related with CI Resources ***

#define	CI_HOST_RES				0xff			// CI Host Resource
#define	CI_RES_PRI_TYPE			3				// 0~2 for public resource, 3 for private resource
#define	MAX_CI_RES_INST			20				// temporary value. to be changed optimal value

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
/***********************
* CI Resource ID version 1
***********************/
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

/***********************
* CI Resource ID version 2
***********************/
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
	CI Resource Handler's Callbacks
**********************************/
/********************************************************************
 Function   :	CI_CALLBACK_SS_OPENED
 Description :	called by Resource Handler to init resource
 Input      :	usSsNo - Session Number
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
typedef int
		(*CI_CALLBACK_INIT)
		(void);

/********************************************************************
 Function   :	CI_CALLBACK_SS_OPENED
 Description :	called by Resource Handler whenever session of resource is opened
 Input      :	usSsNo - Session Number
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
typedef int
		(*CI_CALLBACK_SS_OPENED)
		(unsigned short usSsNo);

/********************************************************************
 Function   :	CI_CALLBACK_SS_CLOSED
 Description :	called by Resource Handler whenever session of resource is closed
 Input      :	usSsNo - Session Number
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
typedef int
		(*CI_CALLBACK_SS_CLOSED)
		(unsigned short usSsNo);

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
typedef int
		(*CI_CALLBACK_APDU)
		(unsigned short usSsNo, unsigned long ulApduTag,
		unsigned char *pucApduData, unsigned long ulApduLen);

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
} CI_SS_INST, *CI_SS_HANDLE;


/***************************************************************
* global function prototypes
***************************************************************/
/* CIRH (CI Resource Handler) */
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
int	CIRH_Init(void);


/********************************************************************
 Function   :	CIRH_GetRes
 Description :	Get Resources from CIRH
 Input      :
 Output     :
 Return     : 	CI Resource Array
 ********************************************************************/
CI_RES_INST *	CIRH_GetRes(void);


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
CI_RES_HANDLE	CIRH_AddRes
				(unsigned long ulResId, CI_SLOT_ID usSrcId,
				unsigned char ucMaxSs, CI_RES_CB_LIST *pCbList);


/********************************************************************
 Function   :	CIRH_DeleteRes
 Description :	Delete(register) Resource from CIRH
 Input      :	hRes - resource handle
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int	CIRH_DeleteRes(CI_RES_HANDLE hRes);


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
CI_SS_HANDLE	CIRH_GetSsFromRes(CI_SLOT_ID usSlotId, unsigned long ulResId);


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
int	CIRH_GetTcId(CI_SLOT_ID usSlotId, unsigned char *pucTcId);


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
int	CIRH_SendApdu(unsigned short usSsNo, unsigned char *pucApdu,
					unsigned long ulApduLen);


/********************************************************************
=============== APDU format ==================
APDU() {
-----------------------------------------------------------------------
	apdu_tag						24		APDU Tag
-----------------------------------------------------------------------
	length_field()							length of APDU Data (1 <= number of byte <= 3)
-----------------------------------------------------------------------
	for (i=0; i<length_value; i++) {
		data_byte					8		APDU Data
	}
-----------------------------------------------------------------------
}
 ********************************************************************/

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



#ifdef __cplusplus
}
#endif

#endif /* _CI_RESHANDLER_H_ */


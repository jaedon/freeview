
/***************************************************************
* $Workfile:   ci_plus.h  $
* $Modtime:
* Auther : hmkim
*
* Desc : CI+ Protocol Porting Layer Header
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
****************************************************************/

#ifndef _CI_PLUS_H_
#define _CI_PLUS_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************
* include //////////////////////////////////////////////////////
***************************************************************/

#include "ci.h"
#include "ci_reshandler.h"

#include <di_factory.h> // for pfnDI_FAC_DOWNLOAD_Notify()


/***************************************************************
* ARC Error Codes //////////////////////////////////////////////
***************************************************************/

typedef enum {
	CI_ARC_ERR_Not_Defined = -1,
	// 이하 값들은 CI+ 스펙에 정의되어 있는 것으로 enum 값을 수정하거나 임의의 항목을 추가하지 말 것 !!
	CI_ARC_ERR_NONE = 0,
	CI_ARC_ERR_Module_Revoked,					//   1 detect : CAM		host action : none
	CI_ARC_ERR_Host_Revoked,						//   2 detect : CAM		host action : none
	CI_ARC_ERR_SAC_Failed,						//   3 detect : CAM/host	host action : Host stops the CICAM
	CI_ARC_ERR_CCK_Failed,						//   4 detect : CAM/host	host action : Host stops the CICAM
	CI_ARC_ERR_CAM_Upgrade_Failed_Bootloader,	//   5 detect : CAM		host action : none
	CI_ARC_ERR_CAM_Upgrade_Failed_Location,	//   6 detect : CAM		host action : none
	CI_ARC_ERR_CAM_Upgrade_Failed_Signature,	//   7 detect : CAM		host action : none
	CI_ARC_ERR_Auth_Failed_Retries_Exhausted,	//   8 detect : CAM		host action : none                 -> CICAM goes to pass-through mode when Authentication Failed
	CI_ARC_ERR_Auth_Failed_Invalid_Signature,	//   9 detect : CAM/host	host action : Host stops the CICAM
	CI_ARC_ERR_Auth_Failed_Invalid_Auth_Key,	//  10 detect : CAM/host	host action : Host stops the CICAM
	CI_ARC_ERR_Auth_Failed_Key_Computation,	//  11 detect : CAM/host	host action : Host stops the CICAM
	CI_ARC_ERR_Auth_Failed_DH,					//  12 detect : CAM/host	host action : Host stops the CICAM
	CI_ARC_ERR_CAM_Cert_Syntax_Incorrect,		//  13 detect : host		host action : Host stops the CICAM
	CI_ARC_ERR_CAM_Cert_Expired,					//  14 detect : host		host action : Host goes to DVB-CI mode (The host behaves like a DVB-CI compliant host)
	CI_ARC_ERR_CAM_Cert_Invalid_Signature,		//  15 detect : host		host action : Host stops the CICAM
	CI_ARC_ERR_Host_Cert_Syntax_Incorrect,		//  16 detect : CAM		host action : none
	CI_ARC_ERR_Host_Cert_Expired,				//  17 detect : CAM		host action : none
	CI_ARC_ERR_Host_Cert_Invalid_Signature,	//  18 detect : CAM		host action : none
	CI_ARC_ERR_OP_Cert_Syntax_Incorrect,		//  19 detect : CAM		host action : none
	CI_ARC_ERR_OP_Cert_Expired,					//  20 detect : CAM		host action : none
	CI_ARC_ERR_OP_Cert_Invalid_Signature,		//  21 detect : CAM		host action : none
	CI_ARC_ERR_CAM_Requires_Update,				//  22 detect : CAM		host action : none
	CI_ARC_ERR_Reserved_for_CI_Plus = 23,		//  23 detect : CAM		host action : none
	CI_ARC_ERR_Private_Use_for_OP = 128			// 128 detect : CAM		host action : none
} CI_ARC_ERR;

typedef enum {
	CI_ACR_ACTION_NONE = 0,
	CI_ARC_ACTION_STOP_CICAM,
	CI_ARC_ACTION_GOTO_NORMAL_MODE
} CI_ARC_ACTION;


/***************************************************************
* for ci_ai.c //////////////////////////////////////////////////
***************************************************************/

/*-------------------------------------------------*/
/*  호스트가 지원하는 PCMCIA data rate (CI+ 스펙)  */
/*-------------------------------------------------*/

#define CI_PCMCIA_DATA_RATE_72_MBPS	0x00
#define CI_PCMCIA_DATA_RATE_96_MBPS	0x01

// get the proper data rate on the PCMCIA bus : return CI_PCMCIA_DATA_RATE_72_MBPS for 72 Mbps (host mandatory), CI_PCMCIA_DATA_RATE_96_MBPS for 96 Mbps (host optional)
unsigned char CI_GetDataRate (void);


/***************************************************************
* for ci_ammi.c and CI+ Application Presentation Engine ////////
***************************************************************/

/*----------------------*/
/*  스펙에 정의된 값들  */
/*----------------------*/

#define CI_AMMI_APP_DOMAIN_ID	"CIMHEGP1"	/* (0x43494d4845475031) */ /* CI+ Application Domain ("CIEngineProfile1") Identifier */

// RequestStartAck()'s AckCode
typedef enum {
	CI_AMMI_REQUEST_START_ACK_CODE_OK = 0x01,
	CI_AMMI_REQUEST_START_ACK_CODE_WRONG_API = 0x02,	// Application domain not supported
	CI_AMMI_REQUEST_START_ACK_CODE_API_BUSY = 0x03	// Application domain supported but not currently available
} CI_AMMI_REQUEST_START_ACK_CODE; // TS 101 699 스펙

// FileReq()'s RequestType
typedef enum {
	CI_AMMI_REQUEST_TYPE_FILE = 0x00,
	CI_AMMI_REQUEST_TYPE_DATA = 0x01
} CI_AMMI_REQUEST_TYPE; // CI+ 스펙

// AppAbortReq()'s AbortReqCode
typedef enum {
	CI_AMMI_ABORT_REQUEST_CODE_USER_CANCEL = 0x01,
	CI_AMMI_ABORT_REQUEST_CODE_SYSTEM_CANCEL = 0x02
} CI_AMMI_ABORT_REQUEST_CODE; // CI+ 스펙

/*-------------------------------------------------------------*/
/*  messge types to be sent to CI+ MHEG engine from ci_ammi.c  */
/*-------------------------------------------------------------*/

typedef enum {
	CI_AMMI_MSG_REQ_START,
	CI_AMMI_MSG_FILE_ACK,
	CI_AMMI_MSG_REQ_APP_ABORT,
	CI_AMMI_MSG_REQ_APP_ABORT_ACK,
	CI_AMMI_MSG_MAX
} CI_AMMI_MSG_TYPE;

/*-------------------------------------------------------------------------------------------*/
/*  data types to be sent to CI+ MHEG engine from ci_ammi.c according to each messge typese  */
/*-------------------------------------------------------------------------------------------*/

// 주의 사항 :
// 이하 포인터들은 ci_ammi.c 에서 memory alloc 하여 callback 호출을 통해 CI+ MHEG engine 으로 전달되며 callback 호출 완료된 시점에서 ci_ammi.c 자체적으로 free 하므로
// CI+ MHEG engine 에서는 이를 local copy 해서 사용해야 함 (절대 이 포인터 그대로 사용해서는 안됨) !!

typedef struct {
	unsigned short	usSessionId;
	unsigned char	ucAppDomainIdLength;
	unsigned char *	pucAppDomainId;		// CI_AMMI_APP_DOMAIN_ID 과 같아야...
	unsigned char	ucInitialObjectLength;
	unsigned char *	pucInitialObject;
} CI_AMMI_TYPE_REQ_START;

typedef struct {
	unsigned short	usSessionId;
	unsigned char	ucFileOK;				// 1 이어야 유효, 0 이면 무효.
	unsigned char	ucReqType;				// CI_AMMI_REQUEST_TYPE
	unsigned char	ucFileNameLength;		// ucReqType 이 CI_AMMI_REQUEST_TYPE_FILE 일 때 유효.
	unsigned char *	pucFileNameByte;		//
	unsigned long	ulFileDataLength;		//
	unsigned char *	pucFileDataByte;		//
	unsigned long	ulDataByteLength;		// ucReqType 이 CI_AMMI_REQUEST_TYPE_DATA 일 때 유효.
	unsigned char *	pucDataByte;			//
} CI_AMMI_TYPE_FILE_ACK;

typedef struct {
	unsigned short	usSessionId;
	unsigned char	ucAbortReqCode;		// CI_AMMI_ABORT_REQUEST_CODE
} CI_AMMI_TYPE_REQ_APP_ABORT;

typedef struct {
	unsigned short	usSessionId;
//	unsigned char   ucAbortAckCode;			// CI+ 스펙 1.2, TS 101 699 1.1.1, D-BOOK 5.0 모두 분명히 명시되어 있지 않아 일단 막아둠
} CI_AMMI_TYPE_APP_ABORT_ACK;

typedef union {
	CI_AMMI_TYPE_REQ_START		typeReqStart;
	CI_AMMI_TYPE_FILE_ACK		typeFileAck;
	CI_AMMI_TYPE_REQ_APP_ABORT	typeReqAppAbort;
	CI_AMMI_TYPE_APP_ABORT_ACK	typeAppAbortAck;
} CI_AMMI_CONTENT;

/*------------------------------------------------------------------------------------------------------------------------------*/
/*  callback function type between ci_ammi.c and CI+ MHEG engine, which is called by ci_ammi.c to send data to CI+ MHEG engine  */
/*------------------------------------------------------------------------------------------------------------------------------*/

// 주의 사항 : enMsgType 가 CI_AMMI_MSG_REQ_START 일 때는 CI+ MHEG engine 은 반드시 AckCode 를 리턴해 줘야한다 (CI_AMMI_REQUEST_START_ACK_CODE)
typedef HUINT32 (*CI_AMMI_CALLBACK_T)(CI_AMMI_MSG_TYPE enMsgType, CI_AMMI_CONTENT ustContent);

// register callback function to ci_ammi.c, which should be called by CI+ MHEG init flow. it returns -1 if there is any error
int CI_RegisterAppMmiCallback (CI_AMMI_CALLBACK_T pfnCallback);

// request file or data to ci_ammi.c, which is called by CI+ MHEG engine. it returns -1 if there is any error
// - usSessionId    : CI_AMMI_MSG_REQ_START 메시지로 받은 usSessionId
// - ucReqType      : CI_AMMI_REQUEST_TYPE
// - pucBytes       : ucReqType 값에 따라 파일명 내지는 데이터명
// - ulBytesLen     : pucBytes 의 길이
int CI_RequestFileorData (unsigned short usSessionId, unsigned char ucReqType, unsigned char *pucBytes, unsigned long ulBytesLen);

// request app abort to ci_ammi.c, which is called by CI+ MHEG engine. it returns -1 if there is any error
// - usSessionId    : CI_AMMI_MSG_REQ_START 또는 CI_AMMI_MSG_FILE_ACK 메시지로 받은 usSessionId
// - ucAbortReqCode : CI_AMMI_ABORT_REQUEST_CODE
int CI_RequestAppAbort (unsigned short usSessionId, unsigned char ucAbortReqCode);


/***************************************************************
* for ci_cc_main.c /////////////////////////////////////////////
***************************************************************/

/*------------------------------------------------------------------------------------------*/
/*  non-volatile memory 저장 데이터들 (host license constants and authentication contexts)  */
/*------------------------------------------------------------------------------------------*/

// host license constants : < 주의 > 이 내용은 생산 S/W 에도 적용되므로 절대 임의로 수정하지 말 것 !!

// SCTE 41 스펙에선 Certificates shorter than 2048 shall be padded to 2048 bytes by adding NULL bytes (0x00) at the trailing end... 라고 되어 있음.

#define CI_MAX_CERT_LEN			2048

#ifdef CI_PLUS_PRODUCTION_Credentials
#define CI_ROOT_CERT_LEN			1014	// ciplus_root.cert -> cer
#define CI_HOST_BRAND_CERT_LEN	1048 	// ciplus_humax_ca.cert -> cer
#define CI_HOST_DEV_CERT_LEN		1058 	// 0A0BF75306A286DD-cert.der
#endif

#ifdef CI_PLUS_TEST_Technology_from_TrustCenter
#define CI_ROOT_CERT_LEN			1002	// ciplus_test_root.crt -> cer
#define CI_HOST_BRAND_CERT_LEN	1008 	// ciplus_test_ca.crt -> cer
#define CI_HOST_DEV_CERT_LEN		1007 	// 1D1852D21C1ED5DC-cert.der
#endif

#ifdef CI_PLUS_TEST_Technology_from_SmarDTV
#define CI_ROOT_CERT_LEN			1108
#define CI_HOST_BRAND_CERT_LEN	1089
#define CI_HOST_DEV_CERT_LEN		1045	// 1045, 1051, 1033 bytes for test kit !!
#endif

#define CI_DH_P_LEN				256		// 이하 값들은 아마 변경될 것 같지는 않다...
#define CI_DH_G_LEN				256
#define CI_DH_Q_LEN				32
#define CI_HDQ_N_LEN				256
#define CI_HDQ_D_LEN				256
#define CI_HDQ_P_LEN				128
#define CI_HDQ_Q_LEN				128
#define CI_HDQ_DP_LEN				128
#define CI_HDQ_DQ_LEN				128
#define CI_HDQ_QINV_LEN			128

typedef struct
{
	// flag
	unsigned char	ucSavedFlag;				// 0 : not saved, 1 : HDQ 1st form saved, 2 : HDQ 2nd form saved

	// 인증서 (루트 / 호스트 브랜드 / 해당 호스트 디바이스 인증서)
	unsigned char	RootCert[CI_MAX_CERT_LEN];
	unsigned short	RootCertLen;
	unsigned char	Host_BrandCert[CI_MAX_CERT_LEN];
	unsigned short	Host_BrandCertLen;
	unsigned char	Host_DevCert[CI_MAX_CERT_LEN];
	unsigned short	Host_DevCertLen;

	// 호스트 아이디 및 호스트가 지원 가능한 Cipher (해당 호스트 디바이스 인증서에 있는 내용이나 사용상의 편의로 호스트 디바이스 인증서로부터 유도하여 별도 저장한다)
	unsigned char	Host_ID[8];
	         int	Host_CckCipher;			// CI_CC_KEY_CIPHER_DES_56_ECB or CI_CC_KEY_CIPHER_AES_128_CBC

	// DH params
	unsigned char	DH_p[CI_DH_P_LEN];
	unsigned char	DH_g[CI_DH_G_LEN];
	unsigned char	DH_q[CI_DH_Q_LEN];

	// RSA private key (해당 호스트 디바이스 인증서와 매칭되는 키 인증서로부터 유도하여 별도 저장한다)
	unsigned char	HDQ_n[CI_HDQ_N_LEN];		// HDQ_n 의 경우엔 해당 호스트 디바이스 인증서 및 그에 매칭되는 키 인증서로부터 각기 유도 가능. 각기 유도한 값이 서로 같은지 확인하도록 한다.
	unsigned char	HDQ_d[CI_HDQ_D_LEN];
	unsigned char	HDQ_p[CI_HDQ_P_LEN];
	unsigned char	HDQ_q[CI_HDQ_Q_LEN];
	unsigned char	HDQ_dP[CI_HDQ_DP_LEN];
	unsigned char	HDQ_dQ[CI_HDQ_DQ_LEN];
	unsigned char	HDQ_qInv[CI_HDQ_QINV_LEN];

	// 기타 license constants
	unsigned char	PRNG_seed[16];				// CI_CC_CIPHER_BLOCK_SIZE, PRNG_seed will be changed by each PRNG() calling
	unsigned char	SLK[16];					// CI_CC_CIPHER_KEY_LEN
	unsigned char	SIV[16];					// CI_CC_CIPHER_BLOCK_SIZE
	unsigned char	CLK[16];					// CI_CC_CIPHER_KEY_LEN

	// 호스트 생성 constants
	unsigned char	DHH[256];					// PRNG_seed 값으로부터 유도 (본래는 random 값이어야 하지만 구현된 PRNG() 특성상 이와 같이 초기에 고정된 값으로 운용한다)
	unsigned char	DHPH[256];					// DHH 및 DH params 값으로부터 유도 (위와 같은 이유로 초기에 고정된 값으로 운용한다)

	// digest
	unsigned char	ucDigest[32];				// SHA_256_DIGEST_SIZE, 저장된 데이터의 무결성 검사를 위해 SHA-256 digest 값을 생성하여 저장한다.
} CI_HOST_LICENSE_CONST;

// authentication contexts

#define CI_NUM_OF_AUTH_CTX		5			// A host shall store 5 authentication contexts. If there is no match the CICAM shall retry at most 5 times.

typedef struct {
	unsigned char	ucSavedFlag; 				// 0 : not saved, 1 : only ucBindedCamId, usBindedCamBrandId and nNegotiatedCckCipher saved, 2 : all saved (binding done)

	unsigned char	ucBindedCamId[8]; 		// binded to host
	unsigned short	usBindedCamBrandId;		// CicamBrandId ::= INTEGER (1..65535)
	         int	nNegotiatedCckCipher; 	// negotiated scrambling cipher : CI_CC_KEY_CIPHER_DES_56_ECB or CI_CC_KEY_CIPHER_AES_128_CBC

	unsigned char	DHSK[16];
	unsigned char	AKH[32];
} CI_AUTH_CTX;

typedef struct
{
	CI_AUTH_CTX		ctx[CI_NUM_OF_AUTH_CTX];
	unsigned char	ucDigest[32];				// SHA_256_DIGEST_SIZE, 저장된 데이터의 무결성 검사를 위해 SHA-256 digest 값을 생성하여 저장한다.
} CI_AUTH_CTXs;

// load / save CI+ host license constants data and return -1 if there is any error
int CI_LoadHostLicenseConstants (CI_HOST_LICENSE_CONST *pstCiHostLicenseConst);
#if defined(CI_PLUS_PRODUCTION_Credentials) || defined(CI_PLUS_TEST_Technology_from_TrustCenter) || defined(CI_PLUS_TEST_Technology_from_SmarDTV)
int CI_SaveHostLicenseConstantsForTestKitOnly (void);
#endif

// load / save / CI+ authentication contexts data for given slot and return -1 if there is any error
int CI_LoadAuthContexts (CI_SLOT_ID usSlotId, CI_AUTH_CTXs *pstCiAuthCtxs);
int CI_SaveAuthContexts (CI_SLOT_ID usSlotId, CI_AUTH_CTXs *pstCiAuthCtxs);

// get current UTC time and return -1 if there is any error ("YYMMDDHHMMSSZ" 형태의 UTC Time format 스트링을 얻어온다)
int CI_GetCurrentUtcTime (unsigned char szUtcTime[13]);

// compare UTC Times : return 1 if t1 > t2, 0 if t1 = t2, -1 if t1 < t2
int CI_CompareUtcTimes (unsigned char szUtcTime1[13], unsigned char szUtcTime2[13]);

/*---------------------*/
/*  디스크램블러 관련  */
/*---------------------*/

// 디스크램블러에 사용될 cipher (comes from "Scrambler capabilities" extension during each device certificate exchange according to "Scrambling Cipher Selection Rules")
typedef enum {
	CI_CC_KEY_CIPHER_DES_56_ECB = 0,
	CI_CC_KEY_CIPHER_AES_128_CBC = 1
} CI_CC_KEY_CIPHER;

// 디스크램블러에 셋팅될 키의 모드 (comes from "key_register")
typedef enum {
	CI_CC_KEY_MODE_EVEN = 0,	// its CCK should be applied to the period during Transport Scrambling Control bits are 0b10
	CI_CC_KEY_MODE_ODD = 1	// its CCK should be applied to the period during Transport Scrambling Control bits are 0b11
} CI_CC_KEY_MODE;

// set descrambler key (CCKey) to descrambler and return -1 if there is any error
int CI_SetCckToDesc (CI_SLOT_ID usSlotId, CI_CC_KEY_CIPHER enKeyCipher, CI_CC_KEY_MODE enKeyMode, unsigned char *pucCck, unsigned char *pucCiv);

/*--------------------------------------*/
/*  Usage Rules Information (URI) 관련  */
/*--------------------------------------*/

#define CI_CC_URI_MODE_DEFAULT		1
#define CI_CC_URI_MODE_TIMEOUT		2
#define CI_CC_URI_MODE_FROMCAM		3

// Analogue Protection System (APS) for Macrovision on/off : 2 bits
#define CI_CC_URI_APS_OFF				0x0					// Copy Protection Encoding Off
#define CI_CC_URI_APS_ON_1			0x1					// AGC Process On, Split Burst Off
#define CI_CC_URI_APS_ON_2			0x2					// AGC Process On, 2 line Split Burst On
#define CI_CC_URI_APS_ON_3			0x3					// AGC Process On, 4 line Split Burst On
#define CI_CC_URI_APS_DEFAULT		CI_CC_URI_APS_OFF

// Encryption Mode Indicator (EMI) or Copy Control Information (CCI) : 2 bits
#define CI_CC_URI_EMI_COPY_FREE		0x0					// Copying not restricted
#define CI_CC_URI_EMI_COPY_NO_MORE	0x1					// No further copying is permitted
#define CI_CC_URI_EMI_COPY_ONCE		0x2					// One generation copy is permitted
#define CI_CC_URI_EMI_COPY_NEVER		0x3					// Copying is prohibited
#define CI_CC_URI_EMI_DEFAULT		CI_CC_URI_EMI_COPY_NEVER

// Image Constrained Trigger (ICT) for HD downscale on/off : 1 bit
#define CI_CC_URI_ICT_OFF				0x0					// No Image Constraint asserted
#define CI_CC_URI_ICT_ON				0x1					// Image Constraint required
#define CI_CC_URI_ICT_DEFAULT		CI_CC_URI_ICT_OFF
#define CI_CC_URI_ICT_TIMEOUT		CI_CC_URI_ICT_ON	// CI_CC_URI_TIMEOUT 시 ICT 적용값

// Redistribution Control Trigger (RCT) for Encryption Plus Non-assert : 1 bit (for EMI 관련 추가 제어용)
#define CI_CC_URI_RCT_OFF				0x0					// No Redistribution Control asserted. Default.
#define CI_CC_URI_RCT_ON				0x1					// Redistribution Control asserted
#define CI_CC_URI_RCT_DEFAULT		CI_CC_URI_RCT_OFF

// Retention Limit (RL) : 6 bits (0x00, 0x01, 0x02 or 0x03..0x3F for EMI 관련 추가 제어용)
#define CI_CC_URI_RL_DEFAULT			0x00				// Default retention limit of 90 minutes applies

// apply URI to external interface and return -1 if there is any error
// - ucMode         : see CI_CC_URI_MODE_
// - ucAps          : see CI_CC_URI_APS_
// - ucEmi          : see CI_CC_URI_EMI_
// - ucIct          : see CI_CC_URI_ICT_
// - ucRct          : see CI_CC_URI_RCT_
// - ulMinutesForRL : minutes for RL
int CI_ApplyUriToExternalOutput (
	CI_SLOT_ID usSlotId,
	unsigned short usServiceId,
	unsigned char ucMode,
	unsigned char ucAps,
	unsigned char ucEmi,
	unsigned char ucIct,
	unsigned char ucRct,
	unsigned long ulMinutesForRL);

/*----------------------------------------------------------------------------------------------------------------------------------*/
/*  CI Protection Descriptor in SDT actual used for Host Service Shunning (CI+ 스펙 1.0 대비하여 1.1 이후에선 아래와 같이 변경되었음)

	ci_protection_descriptor() {
		descriptor_tag					: 8 bits (= 0xCE)
		descriptor_length				: 8 bits
		free_ci_mode_flag				: 1 bit
		match_brand_flag				: 1 bit
		reserved_future_use				: 6 bits (= all 1s)
		if(match_brand_flag == 1) {
			number_of_entries			: 8 bits
			for(i=0; i<n; i++) {
				cicam_brand_identifier	: 16 bits
			}
		}
		for(i=0; i<n; i++) {
			private_data_byte			: 8 bits
		}
	}

	free_ci_mode_flag      : This is a 1-bit field identifying the CI operating mode.
	                         When set to "0", indicates that all of the component streams of the service do not require CI Plus protection.
	                         When set to "1", indicates that all of the component streams of the service require CI Plus protection
	                         if they are not transmitted in the clear on the broadcast network.
	match_brand_flag       : This is a 1-bit field signifying that the descriptor includes a list of cicam_brand_identifiers.
	                         When set to "0", indicates that this service has no chosen CICAM brands.
	                         When set to "1", indicates that this service has chosen to set CICAM brands.
	                         The match_brand_flag is only interpreted when the free_ci_mode_flag is set to "1".
	cicam_brand_identifier : This is a 16-bit field that identifies the CICAM brands that may be used with the service.
	                         When no CICAM brand identifiers are present, any CI Plus CICAM may be used with the Host.
	                         When one or more CICAM brand identifiers are specified,
	                         the Host shall only operate with a CI Plus CICAM device whose Device Certificate cicamBrandId matches the cicam_brand_identifier.
	                         If none of the cicam_brand_identifiers present are matched with the CICAM device certificate then the CICAM shall be shunned for this service.
	                         The cicam_brand_identifier value 0x0000 is reserved and shall not be used.

    "Private Data Specifier Descriptor" shall precede the ci_protection_descriptor in the SDTActual descriptor loop.
    (The private data specifier value is defined in the CI Plus Licensee Specification -> 0x00000040 (CI Plus LLP))

	- CI Plus Protected Service modes -

	1. ci_protection_descriptor absent
	   and DVB CI and CI Plus CAM                               -> Host Service Shunning : in-active
	2. ci_protection_descriptor present and free_CI_mode is "0"
	   and DVB CI and CI Plus CAM                               -> Host Service Shunning : in-active
	3. ci_protection_descriptor present and free_CI_mode is "1"
	   and DVB CI CAM                                           -> Host Service Shunning : active
	4. ci_protection_descriptor present, free_CI_mode is "1"
	   and match_brand_flag = "0" or number_of_entries = "0"
	   and CI Plus CAM                                          -> Host Service Shunning : in-active
	5. ci_protection_descriptor present, free_CI_mode is "1",
	   match_brand_flag = "1" and number_of_entries ≠ "0"
	   and CICAM brand identifier not matched
	   and CI Plus CAM                                          -> Host Service Shunning : active
	6. ci_protection_descriptor present, free_CI_mode is "1",
	   match_brand_flag = "1" and number_of_entries ≠ "0"
	   and CICAM brand identifier matched
	   and CI Plus CAM                                          -> Host Service Shunning : in-active

	해킹 캠을 상정하여 호스트는 TS by-pass mode로 SDT actual을 수신하여 (즉, Trusted acquisition mode)
	ci_protection_descriptor()의 free_ci_mode 가 CI_PLUS_PROTECTED_MODE 로 셋팅되어 있는지 등을 확인해야 한다 (위의 조건).
	만약 ci_protection_descriptor()의 free_ci_mode 가 CI_PLUS_PROTECTED_MODE 로 셋팅되어 있는 상태에서
	캠이 CI+ 모드로 동작하지 않거나 CI+ 모드로 동작하더라도
	match_brand_flag = "1" and number_of_entries ≠ "0" 인데 cicam_brand_identifier 가 매칭되지 않는다면
	호스트 차원에서 서비스를 shunning 시켜야 한다 (set by-pass mode).  */
/*----------------------------------------------------------------------------------------------------------------------------------*/

typedef enum {
	CI_PLUS_FREE_MODE = 0x0,
	CI_PLUS_PROTECTED_MODE = 0x1
} CI_PLUS_MODE;

// set / get plus mode for given slot (Host Service Shunning 을 위한 SI/SVC 연동 함수)
void CI_SetPlusMode (CI_PLUS_MODE enPlusMode);
CI_PLUS_MODE CI_GetPlusMode (void);


/***************************************************************
* for ci_cu.c and other modules ////////////////////////////////
***************************************************************/

/*--------------------*/
/*  호스트 동작 모드  */
/*--------------------*/

typedef enum {
	CI_HOST_MODE_NORMAL = 0,
	CI_HOST_MODE_UNATTENDED,
	CI_HOST_MODE_SERVICE
} CI_HOST_MODE;

// set / get host mode
void CI_SetHostMode (CI_HOST_MODE enHostMode); /* <TODO_CI_PLUS> : 적절한 외부 모듈에서 호출해야... (ex: 메뉴 진입 등) - LF7300/LH3300/HB1의 경우엔 불필요하나 PVR 모델 등에선 고려 필요함 !! */
CI_HOST_MODE CI_GetHostMode (void);


/***************************************************************
* for ci_hci.c /////////////////////////////////////////////////
***************************************************************/



/***************************************************************
* for ci_hlc.c /////////////////////////////////////////////////
***************************************************************/

// get host country code as ISO 3166-1 alpha 3
int CI_GetCountryCode (unsigned char *pucCode);

// get host language code as ISO 639 Part 2
int CI_GetLanguageCode (unsigned char *pucCode);


/***************************************************************
* for 외부 모듈 /////////////////////////////////////////////////
***************************************************************/

// download CI+ host license constants data -> 히든 메뉴에서 사용 ("CI+ Credentials Download")
int CI_DownloadHostLicenseConstants (HUINT8 ucValue);

// CI+ host license download 결과 받기 위한 callback 등록 -> CI_DownloadHostLicenseConstants()와 함께 사용
//int CI_AddNotifyFuncForDownloading (pfnDI_FAC_DOWNLOAD_Notify fnFacNotify);

// init CI+ authentication contexts data and return -1 if there is any error -> 히든 메뉴에서 사용 ("CI+ Authentication Key Reset")
int CI_InitAuthContexts (CI_SLOT_ID usSlotId);

// get Host ID and return -1 if there is any error (이 경우 ID는 empty 또는 invalid 로 간주) -> information 메뉴에서 사용
int CI_GetHostId (unsigned char ucHostId[8]);

// get current binded CI+ CAM ID for given slot and return -1 if there is any error (이 경우 ID는 empty 또는 invalid 로 간주) -> information 메뉴에서 사용
int CI_GetBindedCamId (CI_SLOT_ID usSlotId, unsigned char ucBindedCamId[8]);

// get current binded CI+ CAM Brand ID for given slot and return -1 if there is any error (이 경우 ID는 empty 또는 invalid 로 간주) -> Host Service Shunning 플로우에서 사용
// BindedCamBrandId 값이 0 으로 리턴되는 경우 BindedCamBrandId 가 없는 것을 의미한다.
int CI_GetBindedCamBrandId (CI_SLOT_ID usSlotId, unsigned short *pusBindedCamBrandId);

// get negotiated scrambling cipher for given slot to set descrambler mode when descrambler open and return -1 if there is any error -> descrambler open 시 사용
int CI_GetNegotiatedCckCipher (CI_SLOT_ID usSlotId, CI_CC_KEY_CIPHER *penNegotiatedCckCipher);

// request CC to set descrambler key (CCKey) to descrambler -> descrambler 초기화 직후에 사용
void CI_ReqSetCckToDesc (CI_SLOT_ID usSlotId);

// set URI default values and URI timer for given slot to do control external interface when zapping -> 채널 전환시 꼭 사용해야 한다!
void CI_SetUriDefault (CI_SLOT_ID usSlotId, HBOOL bOn);

// check CI+ CAM is inserted for given slot : return 1 if CI+ CAM is inserted, 0 if no CAM or normal CAM is inserted
int CI_IsCiPlusCam (CI_SLOT_ID usSlotId);

// notify host country setting is changed to CAM and return -1 if there is any error -> 국가 설정이 변경된 직후에 꼭 사용해야 한다!
int CI_NotifyHostCountrySettingChangedToCam (CI_SLOT_ID usSlotId);

// notify host language setting is changed to CAM and return -1 if there is any error -> 메뉴 언어 설정이 변경된 직후에 꼭 사용해야 한다!
int CI_NotifyHostLanguageSettingChangedToCam (CI_SLOT_ID usSlotId);


/***************************************************************
* for MES (interface) //////////////////////////////////////////
***************************************************************/

// for MES : CI+ host license constants 파일에서 호스트 아이디를 얻기 위한 함수임 (MES 프로토콜을 위한 함수임)
int CI_MES_GetHostIdFromLicenseConstDataFile (char* hostId);

// CI+ host license constants data downloading callback function (on booting process)
HERROR  CI_MES_LicenseDownCbFunc (HINT32 lResult);
void 	CI_MES_SetLicenseDownCbRegState (HBOOL bFlag);
HBOOL 	CI_MES_IsLicenseDownCbRegSuccess (void);
HBOOL 	CI_MES_IsLicenseDownTaskEnd (void);


/***************************************************************
* callback to notify for CI / CAS manager //////////////////////
***************************************************************/

// pfnNotifyCcReport (for CC status or error reporting) : UI 처리 및 CiCc_DoActionForArcErr()에서 처리되는 것 외에 필요한 액션을 수행 ---
typedef enum
{
	// 라이선스 데이터가 훼손된 경우 (치명적 에러) -> "Can't load CI+ host license constants !" 표시할 것
	eCiPlus_CcReport_CanNotLoadHostLicenseConstants = 1,

	// 라이선스 데이터가 저장이 안된 경우 (치명적 에러) -> "CI+ host license constants not saved !" 표시할 것
	eCiPlus_CcReport_HostLicenseConstantsNotSaved,

	// 바인딩이 되지 않은 CI+ 캠이 삽입되어 바인딩 과정 시작시 (상태 표시) -> "Virgin CI+ CAM inserted. Authentication in progress. Please wait 1~2 minutes." 표시할 것
	eCiPlus_CcReport_VirginCamInsertedAndAuthenticationInProgress,

	// 캠과 세트 간의 바인딩 완료 (상태 표시) -> "Authentication success" 표시할 것 (또는 eCiPlus_CcReport_VirginCamInsertedAndAuthenticationInProgress 문구 닫거나...)
	eCiPlus_CcReport_AuthenticationSuccess, // 호스트 인증키가 만들어진 직후 시점.

	// 캠과 세트 간의 상호 인증 완료 (CAS/CI mgr 에서 사용하기 위한 용도)
	eCiPlus_CcReport_AuthenticationDoneAndStartSacEstablishment, // SAC Establishment 개시된 시점.

	// 캠과 세트 간의 URI Version 체크 완료 (CAS/CI mgr 에서 사용하기 위한 용도)
	eCiPlus_CcReport_UriVersionNegotiationDone, // URI Version Negotiation 된 직후 시점.

	// 캠과 세트 간의 인증 실패 (에러) -> "Authentication failed (error %d) !" 표시할 것 (여기서 %d 는 enArcErr 값)
	// 단, enArcErr 값이 CI_ARC_ERR_CAM_Cert_Expired 인 경우에는 "Authentication failed. Please check the time is correct on set and re-insert your CAM !" 표기할 것
	eCiPlus_CcReport_AuthenticationFailed,

	// 캠과 세트 간의 데이터 전송 또는 세트의 동작상 오류 (에러) -> "CI+ operation failed (error %d) !" 표시할 것 (여기서 %d 는 enArcErr 값)
	eCiPlus_CcReport_OperationFailed,

	// CC 프로토콜 상에서 캠이 응답하지 않는 경우 (에러) -> "No response from CI+ CAM (error %d) !" 표시할 것 (여기서 %d 는 enArcErr 값)
	eCiPlus_CcReport_NoResponseFromCam
} CIPLUS_CcReportMode_t;

typedef struct
{
	CIPLUS_CcReportMode_t eCcReport;
	CI_ARC_ERR enArcErr; // eCcReport 가 eCiPlus_CcReport_AuthenticationFailed / eCiPlus_CcReport_OperationFailed / eCiPlus_CcReport_NoResponseFromCam 인 경우에만 유효함.
} CIPLUS_CcReportInfo_t;

// pfnNotifySetKey (for setting descrambler with DES or AES keys) ---
typedef struct
{
	CI_CC_KEY_CIPHER enKeyCipher;
	CI_CC_KEY_MODE enKeyMode;
	unsigned char *pucCck;	// [16] : AES-128-CBC 기준. DES-56-ECB 의 경우엔 [0-7] 만 사용.
	unsigned char *pucCiv;	// [16] : AES-128-CBC 기준. DES-56-ECB 의 경우엔 사용되지 않음.
} CIPLUS_KeyInfo_t;

// pfnNotifyUri ---
typedef struct
{
	unsigned short usServiceId;	// ucMode 가 CI_CC_URI_MODE_FROMCAM 일 때만 유효.
	unsigned char ucMode;			// see CI_CC_URI_MODE_
	unsigned char ucAps;			// see CI_CC_URI_APS_
	unsigned char ucEmi;			// see CI_CC_URI_EMI_
	unsigned char ucIct;			// see CI_CC_URI_ICT_
	unsigned char ucRct;			// see CI_CC_URI_RCT_
	unsigned long ulMinutesForRL;
} CIPLUS_UriInfo_t;

// pfnNotifyHostTune ---
typedef enum
{
	eCiPlus_Tune_Start,
	eCiPlus_Tune_Stop,		// 이 경우는 CI/CI+ 스택에서 처리하지 않고 CAS AP 쪽에서 처리하도록 한다.
	eCiPlus_Tune_Cancel		// 이 경우는 CI/CI+ 스택에서 처리하지 않고 CAS AP 쪽에서 처리하도록 한다.
} CIPLUS_TuneMode_t;

typedef struct
{
	CIPLUS_TuneMode_t eTuneMode;
	unsigned short usNetId;
	unsigned short usOnId;
	unsigned short usTsId;
	unsigned short usSvcId;
} CIPLUS_TuneInfo_t;

// pfnNotifyPidReplace ---
typedef enum
{
	eCiPlus_Replace_Start,	// usOriginalPid -> usTargetPId
	eCiPlus_Replace_Stop,		// usTargetPId -> usOriginalPid
	eCiPlus_Replace_Cancel	// usTargetPId -> usOriginalPid
} CIPLUS_ReplacePidMode_t;

typedef struct
{
	CIPLUS_ReplacePidMode_t eReplaceMode;
	unsigned short usOriginalPid;
	unsigned short usTargetPId;
} CIPLUS_PidInfo_t;

// pfnNotifyCamUpgrade (for CAM firmware upgrade status) : UI 처리 및 상황에 따라 필요한 액션을 수행 ---
typedef enum
{
	eCiPlus_CamUpgrade_Start,				// CAM firmware upgrade 진행 시작 처리 (현재로선 처리 사항 없음)
	eCiPlus_CamUpgrade_Processing,			// CAM firmware upgrade 진행 상태 처리 (진행률을 UI로 표시하고 user interaction 을 막을 것)
	eCiPlus_CamUpgrade_DoneOrCancelled,		// CAM firmware upgrade 진행 완료 처리 (UI로 표시하고 막혔던 user interaction 을 풀고 상황에 따라 캠 리셋할 것)
	eCiPlus_CamUpgrade_Failed,				// CAM firmware upgrade 진행 실패 처리 (UI로 표시하고 캠 리셋할 것)
	eCiPlus_CamUpgrade_Broken				// CAM firmware upgrade 진행 취소 처리 (유저에 의해 캠이 제거된 상황에 대한 처리)
} CIPLUS_CamUpgradeMode_t;

typedef enum
{
	eCiPlus_CamUpgradeReset_PCMCIA = 0x00,	/* set RESET signal */ /* 일단 두 경우 모두 PAL_CI_ResetModule()을 호출하도록 한다... */
	eCiPlus_CamUpgradeReset_CAM = 0x01,		/* set RS flag */
	eCiPlus_CamUpgradeReset_NO_RESET = 0x02
} CIPLUS_CamUpgradeResetMode_t;			// CI_CU_RESET_REQUEST 값에 대응 : 이 값들은 CI+ 스펙에 정의되어 있는 것으로 enum 값을 수정하거나 임의의 항목을 추가하지 말 것 !!

typedef struct
{
	CIPLUS_CamUpgradeMode_t eCamUpgradeMode;
	unsigned char ucDownloadProgressInPercentage; /* 진행률(%) */
	CIPLUS_CamUpgradeResetMode_t eCamUpgradeResetMode;
} CIPLUS_CamUpgradeInfo_t;

// callback functions ---
typedef int (*CI_NOTIFY_CC_REPORT_FUNC)(CI_SLOT_ID usSlotId, CIPLUS_CcReportInfo_t *pstCcReportInfo);	// 주의 사항 : 이 콜백의 경우 내부 수행 시간이 최적화 되어 있어야 함에 유의할 것 !!
typedef int (*CI_NOTIFY_SET_KEY_FUNC)(CI_SLOT_ID usSlotId, CIPLUS_KeyInfo_t *pstKeyInfo);				// 주의 사항 : 이 콜백의 경우 내부 수행 시간이 최적화 되어 있어야 함에 유의할 것 !!
typedef int (*CI_NOTIFY_URI_CONTROL_FUNC)(CI_SLOT_ID usSlotId, CIPLUS_UriInfo_t *pstUriInfo);			// 주의 사항 : 이 콜백의 경우 내부 수행 시간이 최적화 되어 있어야 함에 유의할 것 !!
typedef int (*CI_NOTIFY_HOST_TUNE_FUNC)(CI_SLOT_ID usSlotId, CIPLUS_TuneInfo_t *pstTuneInfo);
typedef int (*CI_NOTIFY_PID_REPLACE_FUNC)(CI_SLOT_ID usSlotId, CIPLUS_PidInfo_t *pstPidInfo);
typedef int (*CI_NOTIFY_CAM_UPGRADE_FUNC)(CI_SLOT_ID usSlotId, CIPLUS_CamUpgradeInfo_t *pstCamUpgradeInfo);

typedef struct {
	CI_NOTIFY_CC_REPORT_FUNC	pfnNotifyCcReport;	// 주의 사항 : 이 콜백의 경우 내부 수행 시간이 최적화 되어 있어야 함에 유의할 것 !!
	CI_NOTIFY_SET_KEY_FUNC		pfnNotifySetKey;		// 주의 사항 : 이 콜백의 경우 내부 수행 시간이 최적화 되어 있어야 함에 유의할 것 !!
	CI_NOTIFY_URI_CONTROL_FUNC	pfnNotifyUriControl;	// 주의 사항 : 이 콜백의 경우 내부 수행 시간이 최적화 되어 있어야 함에 유의할 것 !!
	CI_NOTIFY_HOST_TUNE_FUNC	pfnNotifyHostTune;
	CI_NOTIFY_PID_REPLACE_FUNC	pfnNotifyPidReplace;
	CI_NOTIFY_CAM_UPGRADE_FUNC	pfnNotifyCamUpgrade;
} CIPLUS_NOTIFY_FUNC_LIST;

int CIPLUS_Init(void);
int CIPLUS_AddNotifyFunc(CIPLUS_NOTIFY_FUNC_LIST notifyList);
int CIPLUS_DeleteNotifyFunc(CIPLUS_NOTIFY_FUNC_LIST notifyList);


#ifdef __cplusplus
}
#endif

#endif /* _CI_PLUS_H_ */


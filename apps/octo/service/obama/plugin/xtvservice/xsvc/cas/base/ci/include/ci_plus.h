
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
#include <svc_cas.h>
#include <_svc_cas_sub_api.h>


#include <ci.h>
#include <ci_reshandler.h>

#include <dlib.h> // CI 빌드를 위해 임시로 include. dlib_cas.h 로 옮겨간 type들이 존재하여서...


/***************************************************************
* for ci_ai.c and the related modules //////////////////////////
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

#if CONFIG_MW_CI_NUM_SLOT == 2
#define CONFIT_CI_PLUS_AUTH_MULTI_TASK
#define CONFIG_CI_PLUS_AUTH_STORE_VER2
#endif

/*--------------------------------------*/
/*	Usage Rules Information (URI) 관련	*/
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

	// Digital Only Token (DOT) for analogue video output : 1 bit
#define CI_CC_URI_DOT_OFF				0x0					// No Digital Only Constraint asserted (default)
#define CI_CC_URI_DOT_ON				0x1					// Digital Only Constraint asserted; output on analogue video outputs is prohibited.
#define CI_CC_URI_DOT_DEFAULT			CI_CC_URI_DOT_OFF

	// Retention Limit (RL) for URI version 1 : 6 bits (0x00 for 90 min, 0x01 for 6 hours, 0x02 for 12 hours, or 0x03~0x3F)
	// Retention Limit (RL) for URI version 2 : 8 bits (0x00 for 90 min, 0x01 for 6 hours, 0x02 for 12 hours, or 0x03~0xFE, or 0xFF for Unlimited retention period)
#define CI_CC_URI_RL_DEFAULT			0x00				// Default retention limit of 90 minutes applies
#define CI_CC_URI_RL_UNLIMITED			0xff				// Unlimited retention period

/*----------------------*/
/*  스펙에 정의된 값들  */
/*----------------------*/

#define CI_AMMI_APP_DOMAIN_ID	"CIMHEGP1"	/* (0x43494d4845475031) */ /* CI Application Domain ("CIEngineProfile1") Identifier */
#define UK_AMMI_APP_DOMAIN_ID	"UKMHEGP1"	/* (0x554B4D4845475031) */ /* UK Application Domain ("UK Profile 1 MHEG-5") Identifier */

// RequestStartAck()'s AckCode
typedef enum
{
	CI_AMMI_REQUEST_START_ACK_CODE_OK = 0x01,
	CI_AMMI_REQUEST_START_ACK_CODE_WRONG_API = 0x02,	// Application domain not supported
	CI_AMMI_REQUEST_START_ACK_CODE_API_BUSY = 0x03	// Application domain supported but not currently available
} CI_AMMI_REQUEST_START_ACK_CODE; // TS 101 699 스펙

#if 0 // dlib 으로 이동.
// FileReq()'s RequestType
typedef enum
{
	CI_AMMI_REQUEST_TYPE_FILE = 0x00,
	CI_AMMI_REQUEST_TYPE_DATA = 0x01
} CI_AMMI_REQUEST_TYPE; // CI+ 스펙

// AppAbortReq()'s AbortReqCode
typedef enum
{
	CI_AMMI_ABORT_REQUEST_CODE_USER_CANCEL = 0x01,
	CI_AMMI_ABORT_REQUEST_CODE_SYSTEM_CANCEL = 0x02
} CI_AMMI_ABORT_REQUEST_CODE; // CI+ 스펙


/*--------------------------------------------------------------*/
/*  message types to be sent to CI+ MHEG engine from ci_ammi.c  */
/*--------------------------------------------------------------*/

typedef enum
{
	CI_AMMI_MSG_REQ_START,
	CI_AMMI_MSG_FILE_ACK,
	CI_AMMI_MSG_REQ_APP_ABORT,
	CI_AMMI_MSG_APP_ABORT_ACK,
	CI_AMMI_MSG_MAX
} CI_AMMI_MSG_TYPE;


/*-------------------------------------------------------------------------------------------*/
/*  data types to be sent to CI+ MHEG engine from ci_ammi.c according to each message types  */
/*-------------------------------------------------------------------------------------------*/

// 주의 사항 :
// 이하 포인터들은 ci_ammi.c 에서 callback 호출을 통해 CI+ MHEG engine 으로 전달되며 callback 호출이 완료된 시점에서는 무효하게 되므로
// callback 호출이 완료된 이후에 CI+ MHEG engine 에서 이를 사용할 필요가 있다면 반드시 local copy 해서 사용해야 함 (절대 이 포인터 그대로 사용해서는 안됨) !!
// 단, ci_ammi.c 와 CI+ MHEG engine 간의 동작이 synchronous 하다면 신경쓰지 않아도 될 것임.

typedef struct
{
	unsigned short				usSessionId;
	unsigned char				ucAppDomainIdLen;
	unsigned char *				pucAppDomainId;		// in case of CI+ : <applicationDomainIndentifier>[;<option1>;<option2>;…;<option#>], <applicationDomainIndentifier> = CI_AMMI_APP_DOMAIN_ID
	unsigned char				ucInitialObjectLen;
	unsigned char *				pucInitialObject;
} CI_AMMI_TYPE_REQ_START;						// see itk_appmmi_req_start_t in case of using S&T MHEG engine

typedef struct
{
	unsigned short				usSessionId;
	unsigned char				ucFileOK;				// 1 이어야 유효, 0 이면 무효.
	CI_AMMI_REQUEST_TYPE		enReqType;
	unsigned long				ulBytesLen;			// if enReqType is CI_AMMI_REQUEST_TYPE_FILE, "1 + file name length (x) + 4 + file data length (y)" in case of using S&T MHEG engine
	unsigned char *				pucBytes;				// if enReqType is CI_AMMI_REQUEST_TYPE_FILE, "1 byte for x + x bytes for file name + 4 bytes for y + y bytes for file data" in case of using S&T MHEG engine
} CI_AMMI_TYPE_FILE_ACK;							// see itk_appmmi_file_ack_t in case of using S&T MHEG engine

typedef struct
{
	unsigned short				usSessionId;
	CI_AMMI_ABORT_REQUEST_CODE	enAbortReqCode;
} CI_AMMI_TYPE_REQ_APP_ABORT;

typedef struct
{
	unsigned short				usSessionId;
//	unsigned char   			ucAbortAckCode;			// CI+ 스펙 1.2, TS 101 699 1.1.1, D-BOOK 5.0 모두 분명히 명시되어 있지 않아 일단 막아둠
} CI_AMMI_TYPE_APP_ABORT_ACK;

typedef union
{
	CI_AMMI_TYPE_REQ_START		typeReqStart;
	CI_AMMI_TYPE_FILE_ACK		typeFileAck;
	CI_AMMI_TYPE_REQ_APP_ABORT	typeReqAppAbort;
	CI_AMMI_TYPE_APP_ABORT_ACK	typeAppAbortAck;
} CI_AMMI_CONTENT;
#endif
/*------------------------------------------------------------------------------------------------------------------------------*/
/*  callback function type between ci_ammi.c and CI+ MHEG engine, which is called by ci_ammi.c to send data to CI+ MHEG engine  */
/*------------------------------------------------------------------------------------------------------------------------------*/

#if 0 // move to oapi_cas_ci.h //임시로 dlib_cas.h 에 위치
// 주의 사항 : enMsgType 가 CI_AMMI_MSG_REQ_START 일 때는 CI+ MHEG engine 은 반드시 AckCode 를 리턴해 줘야한다 (CI_AMMI_REQUEST_START_ACK_CODE)
typedef int (*CI_AMMI_CALLBACK_T)(CI_AMMI_MSG_TYPE enMsgType, CI_AMMI_CONTENT *pustContent);
#endif

// register callback function to ci_ammi.c, which should be called by CI+ MHEG init flow. it returns -1 if there is any error
int CI_RegisterAppMmiCallback (CI_AMMI_CALLBACK_T pfnCallback);

// request file or data to ci_ammi.c, which is called by CI+ MHEG engine. it returns -1 if there is any error
// - usSessionId    : CI_AMMI_MSG_REQ_START 메시지로 받은 usSessionId
// - enReqType      : CI_AMMI_REQUEST_TYPE
// - pucReqName     : enReqType 값에 따라 파일명 내지는 데이터명
// - ulReqNameLen   : pucReqName 의 길이 (사실상 unsigned char 이고 unsigned int 로 충분)
int CI_RequestFileorData (unsigned short usSessionId, CI_AMMI_REQUEST_TYPE enReqType, unsigned char *pucReqName, unsigned long ulReqNameLen);

// request app abort to ci_ammi.c, which is called by CI+ MHEG engine. it returns -1 if there is any error
// - usSessionId    : CI_AMMI_MSG_REQ_START 또는 CI_AMMI_MSG_FILE_ACK 메시지로 받은 usSessionId
// - enAbortReqCode : CI_AMMI_ABORT_REQUEST_CODE
int CI_RequestAppAbort (unsigned short usSessionId, CI_AMMI_ABORT_REQUEST_CODE enAbortReqCode);

/***************************************************************
* for ci_auth.c and the related modules ////////////////////////
***************************************************************/

/***************************************************************
* for ci_cc_main.c and the related modules /////////////////////
***************************************************************/

/*-------------------*/
/*  ARC Error Codes  */
/*-------------------*/

typedef enum
{
	CI_ARC_ACTION_NONE = 0,
	CI_ARC_ACTION_STOP_CAM,
	CI_ARC_ACTION_GOTO_CI_MODE
} CI_ARC_ACTION;

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
#define CI_HOST_BRAND_CERT_LEN	1023 	// ciplus_test_ca.crt -> cer
#define CI_HOST_DEV_CERT_LEN		1035 	// 1D1852D21C1ED5DC-cert.der
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
	unsigned char	ucSavedFlag;				// 0 : not saved (이하 정보 유효하지 않음), 1 : HDQ 1st form saved, 2 : HDQ 2nd form saved

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
#define CICAM_PIN_LENGTH		4			// 일반적으로 CAS PIN은 4자리이다. 추후에 4자리가 아닌 PIN을 요구하는 CAS가 존재하면 수정 필요함.

// User로 부터 입력받아 저장된 PIN의 상태
typedef enum
{
	CI_CC_STORED_PIN_STATE_NOT_VERIFIED = 0,
	CI_CC_STORED_PIN_STATE_OK,
	CI_CC_STORED_PIN_STATE_FAIL,
	CI_CC_STORED_PIN_STATE_STANDBY_FOR_CACHED_PIN
} CI_CC_STORED_PIN_STATE;

typedef struct
{
	unsigned char	ucSavedFlag; 				// 0 : not saved (이하 정보 유효하지 않음), 1 : only ucBindedCamId, usBindedCamBrandId and nNegotiatedCckCipher saved, 2 : all saved (binding done)

	unsigned char	ucBindedCamId[8]; 		// binded to host
	unsigned short	usBindedCamBrandId;		// CicamBrandId ::= INTEGER (1..65535)
	         int	nNegotiatedCckCipher; 	// negotiated scrambling cipher : CI_CC_KEY_CIPHER_DES_56_ECB or CI_CC_KEY_CIPHER_AES_128_CBC

	unsigned char	DHSK[16];
	unsigned char	AKH[32];

	// Operator Profile 동작을 위해 저장해야 하는 내용
	HBOOL			bOpSupport;
	HBOOL			bUseOpList;
	unsigned char	ucNitVer;
	unsigned char	ucInfoVer;

	/* For CICAM PIN Save and Control */
#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
	unsigned char	ucPinCapability;		// 해당 CAM이 지원하는 PIN Capability
	unsigned char	aucCicamPinCode[CICAM_PIN_LENGTH];	// user가 Host에 저장한 CICAM PIN code
	unsigned char	ucRating;				// CI+CAM(CAS부분)에 설정된 Parental Control 나이 제한
	UNIXTIME		utPinSavedTimeByUser;	// user에 의하여 CICAM PIN이 저장된 시간
	CI_CC_STORED_PIN_STATE eStoredPinVerification;
#endif

	#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
	unsigned char	ucDigest[32];
	#endif
} CI_AUTH_CTX;

typedef struct
{
	CI_AUTH_CTX		ctx[CI_NUM_OF_AUTH_CTX];
	unsigned char	ucDigest[32];				// SHA_256_DIGEST_SIZE, 저장된 데이터의 무결성 검사를 위해 SHA-256 digest 값을 생성하여 저장한다.
} CI_AUTH_CTXs;

// load / save CI+ host license constants data and return -1 if there is any error
int CI_LoadHostLicenseConstants (CI_HOST_LICENSE_CONST *pstCiHostLicenseConst);
#if defined(CI_PLUS_PRODUCTION_Credentials) || defined(CI_PLUS_TEST_Technology_from_TrustCenter) || defined(CI_PLUS_TEST_Technology_from_SmarDTV)
int CI_SaveHostLicenseConstantsForTestKitOnly (HBOOL bRetHostLicense, unsigned char *);
#endif


#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)

// init CI+ authentication contexts data and return -1 if there is any error -> 히든 메뉴에서 사용 ("CI+ Authentication Key Reset")
int CI_InitAuthContexts (void);

// load / save / CI+ authentication contexts data for given slot and return -1 if there is any error
int CI_LoadAuthContexts (CI_AUTH_CTXs *pstCiAuthCtxs);
int CI_SaveAuthContexts (void);

#else

// init CI+ authentication contexts data and return -1 if there is any error -> 히든 메뉴에서 사용 ("CI+ Authentication Key Reset")
int CI_InitAuthContexts (CI_SLOT_ID usSlotId);

// load / save / CI+ authentication contexts data for given slot and return -1 if there is any error
int CI_LoadAuthContexts (CI_SLOT_ID usSlotId, CI_AUTH_CTXs *pstCiAuthCtxs);
int CI_SaveAuthContexts (CI_SLOT_ID usSlotId, CI_AUTH_CTXs *pstCiAuthCtxs);
#endif

// get current UTC time and return -1 if there is any error ("YYYYMMDDHHMMSSZ" 형태의 UTC Time format 스트링을 얻어온다)
int CI_GetCurrentUtcTime (unsigned char szUtcTime[15]);

// compare UTC Times : return 1 if t1 > t2, 0 if t1 = t2, -1 if t1 < t2
int CI_CompareUtcTimes (unsigned char szUtcTime1[15], unsigned char szUtcTime2[15]);

/*---------------------*/
/*  디스크램블러 관련  */
/*---------------------*/

// 디스크램블러에 사용될 cipher (comes from "Scrambler capabilities" extension during each device certificate exchange according to "Scrambling Cipher Selection Rules")
typedef enum
{
	CI_CC_KEY_CIPHER_DES_56_ECB = 0,
	CI_CC_KEY_CIPHER_AES_128_CBC = 1
} CI_CC_KEY_CIPHER;

// 디스크램블러에 셋팅될 키의 모드 (comes from "key_register")
typedef enum
{
	CI_CC_KEY_MODE_EVEN = 0,	// its CCK should be applied to the period during Transport Scrambling Control bits are 0b10
	CI_CC_KEY_MODE_ODD = 1	// its CCK should be applied to the period during Transport Scrambling Control bits are 0b11
} CI_CC_KEY_MODE;


// set descrambler key (CCKey) to descrambler and return -1 if there is any error
int CI_SetCckToDsc (CI_SLOT_ID usSlotId, CI_CC_KEY_CIPHER enKeyCipher, CI_CC_KEY_MODE enKeyMode, unsigned char *pucCck, unsigned char *pucCiv);

// apply URI to external interface and return -1 if there is any error
// - ucMode         : see CI_CC_URI_MODE_
// - ucAps          : see CI_CC_URI_APS_
// - ucEmi          : see CI_CC_URI_EMI_
// - ucIct          : see CI_CC_URI_ICT_
// - ucRct          : see CI_CC_URI_RCT_
// - ulMinutesForRL : minutes for RL
int CI_ApplyUriToExternalOutput (CI_SLOT_ID usSlotId, CIPLUS_UriInfo_t stUriInfo);

// apply SRM data and return -1 if there is any error
int CI_ApplySrmData (CI_SLOT_ID usSlotId, unsigned char *pucSrmData, unsigned short usSrmDataLen);

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

typedef enum
{
	CI_PLUS_FREE_MODE = 0x0,
	CI_PLUS_PROTECTED_MODE = 0x1
} CI_PLUS_MODE;

// set / get plus mode for given slot (Host Service Shunning 을 위한 SI/SVC 연동 함수)
int CI_SetPlusMode (CI_SLOT_ID usSlotId, CI_PLUS_MODE enPlusMode);
int CI_GetPlusMode (CI_SLOT_ID usSlotId, CI_PLUS_MODE *penPlusMode);

/*--------------------------------------*/
/*  Content License Exchanges 관련 (CI+ v1.3)  */
/*--------------------------------------*/

// operating_mode
typedef enum
{
	CI_OPERATING_MODE_WATCH_AND_BUFFER = 0x00,						// eCAS_ActionType_Tsr_Rec 에 해당
	CI_OPERATING_MODE_TIMESHIFT = 0x01,								// eCAS_ActionType_Tsr_Play, eCAS_ActionType_Playback 에 해당
	CI_OPERATING_MODE_UNATTENDED_RECORDING = 0x02					// eCAS_ActionType_Recording 에 해당
} CI_OPERATING_MODE;

// license_status
typedef enum
{
	CI_CONTENT_LICENSE_STATUS_OK = 0x00,
	CI_CONTENT_LICENSE_STATUS_NO_ENTITLEMENT = 0x01,				// Descrambling not possible, no entitlement (record only)
	CI_CONTENT_LICENSE_STATUS_UNDEFINED_ERROR = 0x02,				// Descrambling not possible, undefined error (record only)
	CI_CONTENT_LICENSE_STATUS_ENTITLEMENT_EXPIRED = 0x03,			// Entitlement rights expired (playback & status check)
	CI_CONTENT_LICENSE_STATUS_PLAY_COUNT_EXCEEDED = 0x04,			// Play count exceeded (playback & status check)
	CI_CONTENT_LICENSE_STATUS_RETENTION_LIMIT_EXCEEDED = 0x05,	// Retention limit exceeded (playback & status check)
	CI_CONTENT_LICENSE_STATUS_INVALID_LICENSE = 0x06				// Invalid license (playback & status check)
} CI_CONTENT_LICENSE_STATUS;

/**********************/
/* for "status_field" */
/**********************/
typedef enum {
	CI_CC_STATUS_OK = 0x00,
	CI_CC_STATUS_NO_CC_SUPPORT = 0x01,
	CI_CC_STATUS_HOST_BUSY = 0x02,
	CI_CC_STATUS_AUTH_FAILED_OR_NOT_REQ = 0x03,
	CI_CC_STATUS_CICAM_BUSY = 0x04, /* added from CI+ v1.3 */
	CI_CC_STATUS_RECORDING_MODE_ERROR = 0x05 /* added from CI+ v1.3 */
} CI_CC_STATUS;


// meta info 로 다루어져야 할 것들
typedef struct
{
	Handle_t		hAction;
//	time stamp 는 수신하는 쪽에서 수신 시점을 기준으로 셋팅 (CAM 에서 오지 않으므로)
//	unsigned long  ulTimeStamp;

//	ucBindedCamId[8] 는 수신하는 쪽에서 수신 시점에 CI_GetBindedCamId() 함수를 이용하여 얻어 저장하고 있다가 CI_Playback() 시점에 CI_GetBindedCamId() 한 결과와 비교해서 처리해야 한다.
//	(On playback of content which has an associated license then the Host shall send the license to the original content recording CICAM for evaluation.)
	unsigned char	ucBindedCamId[8];
	unsigned short 	usProgramNumber;

	unsigned char 	ucAps;			// see CI_CC_URI_APS_
	unsigned char 	ucEmi;			// see CI_CC_URI_EMI_
	unsigned char 	ucIct;			// see CI_CC_URI_ICT_
	unsigned char 	ucRct;			// see CI_CC_URI_RCT_
	unsigned char 	ucDot;			// see CI_CC_URI_DOT_
	unsigned long 	ulMinutesForRL;	// minutes for RL (0 means unlimited retention)

	unsigned char	*pucLicenseData;
	unsigned int	unLicenseDataLen;
} CI_CONTENT_LICENSE_INFO;


/*-----------------------------*/
/*  Parental Control 관련 (CI+ v1.3)  */
/*-----------------------------*/

#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
// cc_PIN_capabilities_reply()'s capability_field
typedef enum
{
	CI_PIN_CAPABILITY_NONE = 0x00,						// CICAM has no capability to handle PIN
	CI_PIN_CAPABILITY_ONLY_CAS = 0x01,				// CICAM only handles CAS controlled content PIN
	CI_PIN_CAPABILITY_BOTH = 0x02,						// CICAM can handle both CAS controlled content PIN and non CAS controlled content PIN
	CI_PIN_CAPABILITY_ONLY_CAS_WITH_CACHE = 0x03,	// CICAM only handles CAS controlled content PIN (with CICAM cached PINs)
	CI_PIN_CAPABILITY_BOTH_WITH_CACHE = 0x04		// CICAM can handle both CAS controlled content PIN and non CAS controlled content PIN (with CICAM cached PINs)
} CI_PIN_CAPABILITY;

// cc_PIN_reply()'s PINcode_status_field
typedef enum
{
	CI_PIN_STATUS_BAD_PIN = 0x00,
	CI_PIN_STATUS_CICAM_BUSY = 0x01,
	CI_PIN_STATUS_PIN_CORRECT = 0x02,
	CI_PIN_STATUS_PIN_UNCONFIRMED = 0x03,
	CI_PIN_STATUS_VIDEO_BLAKING_NOT_REQUIRED = 0x04,
	CI_PIN_STATUS_CONTENT_CA_SCRAMBLED = 0x05
} CI_PIN_STATUS;

// for CI_GetPinCapaInfo()
typedef struct
{
	// "capability_field"
	CI_PIN_CAPABILITY	enPinCapa;

	// "pin_change_time_utc"
	// the time when the CICAM PIN was last changed.
	// This is a 40-bit field which specifies the date and time in MJD and UTC when the PIN was last changed (See start_time field of the EIT in EN 300 468).
	// This field is encoded as 40-bits giving the 16 LSBs MJD followed by 24-bits coded as 6 digits in 4-bit BCD.
	// This field shall be specified as zero if the PIN is not handled or when it has never been changed.
	// The Host may use the this to warn the end-user that any unattended recording programmed may fail when it was programmed before and scheduled after the time indicated by the pin_change_time_utc field.
	unsigned char			ucPinChangeTimeUtc[5];

	// "rating"
	// This 8-bit field is coded as DVB rating (3+years).
	// Rating is defined in EN 300 468 parental rating descriptor.
	// This is the current rating as set in the CICAM.
	// This field allows the Host to exert parental control when the Host rating is set at a lower level than the CICAM rating.
	// The Host may use the cc_PIN_MMI_req() APDU for this purpose depending on the CICAM PIN capabilities.
	// The CICAM shall not request a PIN entry for an age rating less than this value.
	unsigned char			ucRating;

	// bPinCapaSet은 capability_reply로 받는 정보는 아니지만, 현재 해당 CAM에 대하여 Pin Capability 가 setting 된 상태인지 파악하기 위해 추가함.
	HBOOL					bPinCapaSet;
} CI_PIN_CAPA_INFO;

typedef struct
{
	HUINT16			usProgramNum;
	CI_PIN_STATUS	ePinStatus;
	HUINT8			ucRating;
	UNIXTIME		utEventTime;
	HUINT8			aucPrivateData[15];
	HUINT8			aucBindedCamId[8];
} CI_PIN_EVENT_INFO;
#endif


/***************************************************************
* for ci_cu.c and the related modules //////////////////////////
***************************************************************/

/*--------------------*/
/*  호스트 동작 모드  */
/*--------------------*/

typedef enum
{
	CI_HOST_MODE_NORMAL = 0,
	CI_HOST_MODE_UNATTENDED,
	CI_HOST_MODE_SERVICE
} CI_HOST_MODE;

// set / get host mode
void CI_SetHostMode (CI_HOST_MODE enHostMode); /* <TODO_CI_PLUS> : 적절한 외부 모듈에서 호출해야... (ex: 메뉴 진입 등) - LF7300/LH3300/HB1의 경우엔 불필요하나 PVR 모델 등에선 고려 필요함 !! */
CI_HOST_MODE CI_GetHostMode (void);


/***************************************************************
* for ci_op.c and the related modules (Operator Profile) ///////
***************************************************************/

/*--------------------*/
/*  Tune 동작 모드    */
/*--------------------*/

typedef enum
{
	CI_OP_REFRESH_REQUEST_NONE = 0,
	CI_OP_REFRESH_REQUEST_TUNE,
	CI_OP_REFRESH_REQUEST_FORCED_TUNE,
	CI_OP_REFRESH_REQUEST_RESERVED_TUNE
} CI_OP_REFRESH_REQUEST;


/***************************************************************
* for ci_hci.c and the related modules /////////////////////////
***************************************************************/

// tune_reply()'s status_field
typedef enum
{
	CI_TUNE_STATUS_OK = 0x00, // Successful tuning. Following a successful tuning operation the Host shall return a ca_pmt() extracted from the applicable PMT.
	CI_TUNE_STATUS_ERR_DSD = 0x01, // Unsupported delivery system descriptor
	CI_TUNE_STATUS_ERR_Tuner_Not_Locking = 0x02,
	CI_TUNE_STATUS_ERR_Tuner_Busy = 0x03,
	CI_TUNE_STATUS_ERR_Parameters = 0x04, // Bad or missing parameters. If a mandatory descriptor is missing in the tune request, then this error shall be returned. Mandatory descriptors are listed in tune_broadcast_req()
	CI_TUNE_STATUS_ERR_Service_Not_Found = 0x05,
	CI_TUNE_STATUS_ERR_Undefined = 0x06
} CI_TUNE_STATUS;

// ask_release_reply()'s release_reply
typedef enum
{
	CI_TUNE_RELEASE_OK = 0x00, // Release OK : Host regains control of the tuner
	CI_TUNE_RELEASE_REFUSED = 0x01 // Release Refused : CICAM retains control of the tuner
} CI_TUNE_RELEASE;

/***************************************************************
* for ci_hlc.c and the related modules /////////////////////////
***************************************************************/

// get host country code as ISO 3166-1 alpha 3
int CI_GetCountryCode (unsigned char *pucCode);

// get host language code as ISO 639 Part 2
int CI_GetLanguageCode (unsigned char *pucCode);

/***************************************************************
* for 외부 모듈 ////////////////////////////////////////////////
***************************************************************/

// get Host ID and return -1 if there is any error (이 경우 ID는 empty 또는 invalid 로 간주) -> information 메뉴에서 사용
int CI_GetHostId (unsigned char ucHostId[8]);

// get current binded CI+ CAM ID for given slot and return -1 if there is any error (이 경우 ID는 empty 또는 invalid 로 간주) -> information 메뉴에서 사용
int CI_GetBindedCamId (CI_SLOT_ID usSlotId, unsigned char *pucBindedCamId);

// get current binded CI+ CAM Brand ID for given slot and return -1 if there is any error (이 경우 ID는 empty 또는 invalid 로 간주) -> Host Service Shunning 플로우에서 사용
// BindedCamBrandId 값이 0 으로 리턴되는 경우 BindedCamBrandId 가 없는 것을 의미한다.
int CI_GetBindedCamBrandId (CI_SLOT_ID usSlotId, unsigned short *pusBindedCamBrandId);

// get negotiated scrambling cipher for given slot to set descrambler mode when descrambler open and return -1 if there is any error -> descrambler open 시 사용
int CI_GetNegotiatedCckCipher (CI_SLOT_ID usSlotId, CI_CC_KEY_CIPHER *penNegotiatedCckCipher);

// request CC to set descrambler key (CCKey) to descrambler -> descrambler 초기화 직후에 사용
void CI_ReqSetCckToDsc (CI_SLOT_ID usSlotId);

// set URI default values and URI timer for given slot to do control external interface when zapping -> 채널 전환시 꼭 사용해야 한다!
void CI_SetUriDefault (CI_SLOT_ID usSlotId, HBOOL bOn);

// check CI+ CAM is inserted for given slot : return positive value if CI+ CAM is inserted, 0/-1 if no CAM or normal CAM is inserted
int CI_IsCiPlusCam (CI_SLOT_ID usSlotId);

// check whether operator profile is supported with the CAM for given slot : return 1 if supported, 0 if not supported, -1 if no CAM is inserted
HBOOL CI_OperatorProfileSupported (CI_SLOT_ID usSlotId);

#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
HULONG CI_GetCcResVersion(CI_SLOT_ID usSlotId);

// signals the start of a CA protected service recording to the CICAM and return -1 if there is any error (CI+ v1.3)
// -> CA protected service 의 TSR 또는 레코딩시 사용 (CI mgr 에서 CI+ descrambling 시 호출)
// 이 경우 enOperatingMode 는 CI_OPERATING_MODE_WATCH_AND_BUFFER 또는 CI_OPERATING_MODE_UNATTENDED_RECORDING 이어야 한다.
// pucPinCode 는 option 이고 pucPinCode 를 전달하지 않는 경우 pucPinCode 는 NULL, unPinCodeLen 는 0 이어야 한다.
int CI_RecordStart (CI_SLOT_ID usSlotId, Handle_t hAction, CI_OPERATING_MODE eOperatingMode, unsigned short usProgramNumber);

// changes the operating mode and return -1 if there is any error (CI+ v1.3)
// -> CI_RecordStart() 호출 이후에 모드 전환시 사용 (예: CI_OPERATING_MODE_WATCH_AND_BUFFER -> CI_OPERATING_MODE_TIMESHIFT)
int CI_ChangeOperatingMode (CI_SLOT_ID usSlotId, unsigned int unSessionId, CI_OPERATING_MODE enOperatingMode, unsigned short usProgramNumber);

// inform the CICAM that the recording has stopped and return -1 if there is any error (CI+ v1.3)
// -> CI_RecordStart() 호출 이후에 TSR 또는 레코딩 중지시 사용
int CI_RecordStop (CI_SLOT_ID usSlotId, unsigned short usProgramNumber);

int CI_GetRecordingStatus(CI_SLOT_ID usSlotId, HUINT8 *pucCcRecordingStatus);

// signals the start of a CA protected service playback to the CICAM and return -1 if there is any error (CI+ v1.3)
// -> CI_RecordStart() 로 녹화된 서비스의 playback 시 사용 (content license 를 받은 경우에 한해)
// 주의 사항 :
// The license exchange on playback protocol is performed in parallel while playing the content to ensure that playback start is not delayed.
// If the Playback License response is not OK, or the response takes longer than 10 seconds the play back shall be stopped.
//int CI_Playback (CI_SLOT_ID usSlotId, unsigned int unSessionId, unsigned short usProgramNumber, unsigned char *pucLicenseData, unsigned int unLicenseDataLen);

// interrogate the CICAM about the current status of a content license associated with a recording and return -1 if there is any error (CI+ v1.3)
// -> CI_RecordStart() 로 녹화된 서비스의 license status 및 남아 있는 play count 정보를 얻기 위해 사용 (content license 를 받은 경우에 한해)
int CI_CheckLicense (CI_SLOT_ID usSlotId, unsigned int unSessionId, unsigned char *pucLicenseData, unsigned int unLicenseDataLen);

// get PIN capability info for given slot and return -1 if there is any error (CI+ v1.3)
// -> CI_RecordStart() 호출 직전에 사용
int CI_GetPinCapaInfo (CI_SLOT_ID usSlotId, CI_PIN_CAPA_INFO *pstPinCapaInfo);

int CI_StoreCicamPinInfo (CI_SLOT_ID usSlotId, HUINT8 *pucPinInfo, UNIXTIME utPinSavedTime);

int CI_GetUriInfo(CI_SLOT_ID usSlotId, CIPLUS_UriInfo_t *pstUriInfo);

int CI_PinPlayback (CI_SLOT_ID usSlotId, HUINT32 unMetaIndex, Handle_t hPbAction, HUINT8 ucAgeLimit, HUINT8 *pucPrivateData);

// signals the start of a CA protected service playback to the CICAM and return -1 if there is any error (CI+ v1.3)
// -> CI_RecordStart() 로 녹화된 서비스의 playback 시 사용 (content license 를 받은 경우에 한해)
// 주의 사항 :
// The license exchange on playback protocol is performed in parallel while playing the content to ensure that playback start is not delayed.
// If the Playback License response is not OK, or the response takes longer than 10 seconds the play back shall be stopped.
int CI_SendHostLicense (CI_SLOT_ID usSlotId, HUINT32 unMetaIndex, Handle_t hPbAction, HUINT16 usProgramNum, HUINT8 *pucLicenseData, HUINT16 usLicenseLength);

#endif

#if defined(CONFIG_MW_CI_PLUS_HCI_V_2)
// notify host tuning result to CAM and return -1 if there is any error (CI+ v1.3)
// -> pfnNotifyHostTune 을 받은 후 그 결과 리턴시에 사용
int CI_TuneReply (CI_SLOT_ID usSlotId, HUINT32 ulTargetResource, CI_TUNE_STATUS eTuneResult);
#endif


// notify host country setting is changed to CAM and return -1 if there is any error -> 국가 설정이 변경된 직후에 꼭 사용해야 한다!
int CI_NotifyHostCountrySettingChangedToCam (CI_SLOT_ID usSlotId);

// notify host language setting is changed to CAM and return -1 if there is any error -> 메뉴 언어 설정이 변경된 직후에 꼭 사용해야 한다!
int CI_NotifyHostLanguageSettingChangedToCam (CI_SLOT_ID usSlotId);

#ifdef CONFIG_OP_CANALREADY

// get the information of canal ready init_notify() and return -1 if there is any error -> canal service installation 시 사용
int CI_CanalReadyGetInitInfo (CI_SLOT_ID usSlotId, SvcCas_CiAuthInfo_t *pstCiAuthInfo);

// reply to canal ready morality_level_notify() and return -1 if there is any error -> canal service parent control management 시 사용 (올바른 PIN code 가 입력된 경우 사용)
int CI_CanalReadyMoralityLevelUnlock (CI_SLOT_ID usSlotId);

#endif

unsigned char CI_OperatorNitVersion(void);
int CI_OperatorPrepareStatus(CI_SLOT_ID usSlotId, HBOOL bSearchAvailable);
int CI_OperatorTuneStatus(CI_SLOT_ID usSlotId, HUINT8 ucStrength, HUINT8 ucQuality, HUINT8 ucStatus, HUINT8 *pucDes, HUINT8 ucDesLen, HUINT8 ucNextDesNum);
HBOOL CI_GetOperatorProfileErrStatus (CI_SLOT_ID usSlotId);
int CI_SetOperatorProfileSupportInfo(CI_SLOT_ID usSlotId, HUINT8 ucNitVer, HUINT8 ucInfoVer);
int CI_GetAuthContextBySlotId(CI_SLOT_ID usSlotId, CI_AUTH_CTX *pstCiAuthCtx);
HBOOL CI_CheckCcAuthenticationComplete(CI_SLOT_ID usSlotId);
HBOOL CI_CheckSimpleEntitlementChange(CI_SLOT_ID usSlotId);
int CI_SendOperatorExitBySlotId(CI_SLOT_ID usSlotId);
int CI_SendOperatorStatusReqBySlotId(CI_SLOT_ID usSlotId);
int CI_GetOperatorReservedTime(HUINT16 usSlotId, void *pvOut);


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

#if defined(CONFIG_MW_CI_PLUS_V_1_3)
typedef enum {
	CI_CC_RECORDING_NOT_CONTROLED	= 0x00,
	CI_CC_RECORDING_STOPPED,
	CI_CC_RECORDING_STARTED
}CI_CC_RECORDING_STATUS;

typedef struct
{
	Handle_t 		hAction;
	HUINT32			unMetaIndex;
	CI_PIN_STATUS	ePinStatus;
} CIPLUS_CcPinStatusInfo_t;
#endif

typedef struct
{
	SvcCas_CiPlusCcReportMode_e eCcReport;
	SvcCas_CiArcErr_e enArcErr; // eCcReport 가 eCiPlus_CcReport_AuthenticationFailed / eCiPlus_CcReport_OperationFailed / eCiPlus_CcReport_NoResponseFromCam 인 경우에만 유효함.
} CIPLUS_CcReportInfo_t;

// pfnNotifySetKey (for setting descrambler with DES or AES keys) ---
typedef struct
{
	CI_CC_KEY_CIPHER enKeyCipher;
	CI_CC_KEY_MODE enKeyMode;
	unsigned char *pucCck;	// [16] : AES-128-CBC 기준. DES-56-ECB 의 경우엔 [0-7] 만 사용.
	unsigned char *pucCiv;	// [16] : AES-128-CBC 기준. DES-56-ECB 의 경우엔 사용되지 않음.
} CIPLUS_KeyInfo_t;

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

typedef struct
{
	SvcCas_CiPlusCamUpgradeMode_e eCamUpgradeMode;
	unsigned char ucDownloadProgressInPercentage; /* 진행률(%) */
	SvcCas_CiPlusCamUpgradeResetMode_e eCamUpgradeResetMode;
} CIPLUS_CamUpgradeInfo_t;

// pfnNotifyCcConfirmData
// :Host가 CAM에게 cc_sac_data_req()를 보낸 응답으로 받은 cc_sac_data_cfn data들에 대해 각각 callback 으로 처리하면, callback function이 너무 많아져 data를 다 올리고 ci mgr에서 분기 ---
/* Host 에서 CAM으로 전송하는 Protocols (for CI+ v1.3 spec)*/
typedef enum {
	eCiCcProtocolType_None = 0,
	eCiCcProtocolType_PlaybackLicenseExchange,
	eCiCcProtocolType_LicenseCheckExchange,
	eCiCcProtocolType_RecordStart,
	eCiCcProtocolType_RecordStop,
	eCiCcProtocolType_ChangeOperatingMode,

	eCiCcProtocolType_Max
} CI_CC_PROTOCOL_TYPE;

typedef struct {
	unsigned int				unMetaIndex;
	unsigned short 				usProgramNumber;

	CI_CONTENT_LICENSE_STATUS	eLicenseStatus;

	unsigned char 				ucAps;			// see CI_CC_URI_APS_
	unsigned char 				ucEmi;			// see CI_CC_URI_EMI_
	unsigned char 				ucIct;			// see CI_CC_URI_ICT_
	unsigned char 				ucRct;			// see CI_CC_URI_RCT_
	unsigned char 				ucDot;			// see CI_CC_URI_DOT_
	unsigned long 				ulMinutesForRL;	// minutes for RL (0 means unlimited retention)

	unsigned char				*pucLicenseData;
	unsigned int				unLicenseDataLen;
} CIPLUS_CcPlaybackLicenseInfo_t;

typedef struct {
	CI_CONTENT_LICENSE_STATUS	eLicenseStatus;
	HUINT8						ucPlayCount;
} CIPLUS_CcLicenseCheckInfo_t;

typedef struct {
	CI_CC_STATUS	eRecordStartStatus;
} CIPLUS_CcRecordStartInfo_t;

typedef struct {
	CI_CC_STATUS	eRecordStopStatus;
} CIPLUS_CcRecordStopInfo_t;

typedef struct {
	CI_CC_STATUS	eChangeOperateModeStatus;
} CIPLUS_CcChangeOperatingModeInfo_t;


typedef struct
{
	CI_CC_PROTOCOL_TYPE				eCcProtocolType;
	Handle_t						hAction;
	union
	{
		CIPLUS_CcPlaybackLicenseInfo_t		stPbLicenseInfo;
		CIPLUS_CcLicenseCheckInfo_t			stLicenseCheckInfo;
		CIPLUS_CcRecordStartInfo_t			stRecordStartInfo;
		CIPLUS_CcRecordStopInfo_t			stRecordStopInfo;
		CIPLUS_CcChangeOperatingModeInfo_t	stChangeOpModeInfo;
	} data;
} CIPLUS_CcConfirmDataInfo_t;


// callback functions ---
typedef int (*CI_NOTIFY_CC_REPORT_FUNC)(CI_SLOT_ID usSlotId, CIPLUS_CcReportInfo_t *pstCcReportInfo); // 주의 사항 : 이 콜백의 경우 내부 수행 시간이 최적화 되어 있어야 함에 유의할 것 !!
typedef int (*CI_NOTIFY_PIN_CAPA_INFO_FUNC)(CI_SLOT_ID usSlotId); // 주의 사항 : 이 콜백의 경우 내부 수행 시간이 최적화 되어 있어야 함에 유의할 것 !!
typedef int (*CI_NOTIFY_SET_KEY_FUNC)(CI_SLOT_ID usSlotId, CIPLUS_KeyInfo_t *pstKeyInfo); // 주의 사항 : 이 콜백의 경우 내부 수행 시간이 최적화 되어 있어야 함에 유의할 것 !!
typedef int (*CI_NOTIFY_URI_CONTROL_FUNC)(CI_SLOT_ID usSlotId, CIPLUS_UriInfo_t *pstUriInfo); // 주의 사항 : 이 콜백의 경우 내부 수행 시간이 최적화 되어 있어야 함에 유의할 것 !!
typedef int (*CI_NOTIFY_SRM_CONTROL_FUNC)(CI_SLOT_ID usSlotId, unsigned char *pucSrmData, unsigned short usSrmDataLen); // 주의 사항 : 이 콜백의 경우 내부 수행 시간이 최적화 되어 있어야 함에 유의할 것 !!
typedef int (*CI_NOTIFY_HOST_TUNE_FUNC)(CI_SLOT_ID usSlotId, SvcCas_CiPlusTuneInfo_t *pstTuneInfo);
typedef int (*CI_NOTIFY_PID_REPLACE_FUNC)(CI_SLOT_ID usSlotId, CIPLUS_PidInfo_t *pstPidInfo);
typedef int (*CI_NOTIFY_CAM_UPGRADE_FUNC)(CI_SLOT_ID usSlotId, CIPLUS_CamUpgradeInfo_t *pstCamUpgradeInfo);
#if defined(CONFIG_OP_CANALREADY)
typedef int (*CI_NOTIFY_CANALREADY_INIT_FUNC)(CI_SLOT_ID usSlotId, SvcCas_CiAuthInfo_t *pstCiAuthInfo);
typedef int (*CI_NOTIFY_CANALREADY_MORAL_LEVEL_FUNC)(CI_SLOT_ID usSlotId, unsigned char ucLevel);
typedef int (*CI_NOTIFY_CANALREADY_MORAL_PINRESET_FUNC)(CI_SLOT_ID usSlotId);
#endif
#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
typedef int (*CI_NOTIFY_CC_PIN_STATUS_FUNC)(CI_SLOT_ID usSlotId, CIPLUS_CcPinStatusInfo_t *pstCcPinStatusInfo);
typedef int (*CI_NOTIFY_CC_CNF_DATA_FUNC)(CI_SLOT_ID usSlotId, CIPLUS_CcConfirmDataInfo_t *pstCcConfirmDataInfo);
// CI_CONTENT_LICENSE_INFO_CB : CI_RecordStart(), CI_Playback() 를 호출하는 caller 가 license info 를 받기 위한 콜백 함수임 (즉, 함수 본체는 caller 쪽에 구현되어 있어야 함)
// unSessionId 의 관리는 caller 에서 하고 CI_CONTENT_LICENSE_INFO_CB 는 받은 unSessionId 를 전달함
typedef int (*CI_NOTIFY_CC_LICENSE_FUNC)(CI_SLOT_ID usSlotId, unsigned int unSessionId, CI_CONTENT_LICENSE_STATUS enLicenseStatus, CI_CONTENT_LICENSE_INFO *pstLicenseInfo); // return 0 if there is no error
// CI_CONTENT_LICENSE_CHECK_CB : CI_CheckLicense() 를 호출하는 caller 가 licenses check info 를 받기 위한 콜백 함수임 (즉, 함수 본체는 caller 쪽에 구현되어 있어야 함)
// unSessionId 의 관리는 caller 에서 하고 CI_CONTENT_LICENSE_CHECK_CB 는 받은 unSessionId 를 전달함
typedef int (*CI_NOTIFY_CONTENT_LICENSE_CHECK_FUNC)(CI_SLOT_ID usSlotId, unsigned int unSessionId, CI_CONTENT_LICENSE_STATUS enLicenseStatus, unsigned char ucPlayCount); // return 0 if there is no error
typedef int (*CI_NOTIFY_CC_PIN_EVENT_FUNC)(CI_SLOT_ID usSlotId, CI_PIN_EVENT_INFO *pstCcPinEvent, Handle_t hRecAction);
#endif
typedef int (*CI_NOTIFY_OPERATOR_PREPARE_REQ_FUNC)(CI_SLOT_ID usSlotId, CI_OP_REFRESH_REQUEST eOpRefreshReq, unsigned char *pszProfileName);
typedef int (*CI_NOTIFY_OPERATOR_TUNE_FUNC)(CI_SLOT_ID usSlotId, unsigned char *pucDes, unsigned short usDesLen);
typedef int (*CI_NOTIFY_OPERATOR_NIT_FUNC)(CI_SLOT_ID usSlotId, unsigned char *pszProfileName, unsigned char* textCoding, unsigned char *pucCiCamNit, unsigned short usCiCamNitLen);


typedef struct
{
	CI_NOTIFY_CC_REPORT_FUNC	pfnNotifyCcReport;	// 주의 사항 : 이 콜백의 경우 내부 수행 시간이 최적화 되어 있어야 함에 유의할 것 !!
	CI_NOTIFY_PIN_CAPA_INFO_FUNC	pfnNotifyPinCapaInfo;	// 주의 사항 : 이 콜백의 경우 내부 수행 시간이 최적화 되어 있어야 함에 유의할 것 !!
	CI_NOTIFY_SET_KEY_FUNC		pfnNotifySetKey;		// 주의 사항 : 이 콜백의 경우 내부 수행 시간이 최적화 되어 있어야 함에 유의할 것 !!
	CI_NOTIFY_URI_CONTROL_FUNC	pfnNotifyUriControl;	// 주의 사항 : 이 콜백의 경우 내부 수행 시간이 최적화 되어 있어야 함에 유의할 것 !!
	CI_NOTIFY_SRM_CONTROL_FUNC	pfnNotifySrmControl;	// 주의 사항 : 이 콜백의 경우 내부 수행 시간이 최적화 되어 있어야 함에 유의할 것 !!
	CI_NOTIFY_HOST_TUNE_FUNC	pfnNotifyHostTune;
	CI_NOTIFY_PID_REPLACE_FUNC	pfnNotifyPidReplace;
	CI_NOTIFY_CAM_UPGRADE_FUNC	pfnNotifyCamUpgrade;
#if defined(CONFIG_OP_CANALREADY)
	CI_NOTIFY_CANALREADY_INIT_FUNC				pfnNotifyCanalReadyInit;
	CI_NOTIFY_CANALREADY_MORAL_LEVEL_FUNC		pfnNotifyCanalReadyMoralLevel;
	CI_NOTIFY_CANALREADY_MORAL_PINRESET_FUNC	pfnNotifyCanalReadyPinReset;
#endif
#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
	CI_NOTIFY_CC_PIN_STATUS_FUNC				pfnNotifyCcPinStatus;
	CI_NOTIFY_CC_CNF_DATA_FUNC					pfnNotifyCcCnfData;
	CI_NOTIFY_CC_LICENSE_FUNC					pfnNotifyCcLicense;
	CI_NOTIFY_CC_PIN_EVENT_FUNC					pfnNotifyCcPinEvent;
#endif
	CI_NOTIFY_OPERATOR_PREPARE_REQ_FUNC			pfnNotifyOperatorPrepareReq;
	CI_NOTIFY_OPERATOR_TUNE_FUNC				pfnNotifyOperatorTune;
	CI_NOTIFY_OPERATOR_NIT_FUNC					pfnNotifyOperatorNit;
} CIPLUS_NOTIFY_FUNC_LIST;

int CIPLUS_Init(void);
int CIPLUS_AddNotifyFunc(CIPLUS_NOTIFY_FUNC_LIST notifyList);
int CIPLUS_DeleteNotifyFunc(CIPLUS_NOTIFY_FUNC_LIST notifyList);


#ifdef __cplusplus
}
#endif

#endif /* _CI_PLUS_H_ */



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
* include //////////////////////////////////////////////////////
***************************************************************/

#include "ci_plus.h"


/***************************************************************
* definition ///////////////////////////////////////////////////
***************************************************************/

#define CI_CC_SYSTEM_ID_BITMASK_V1	0x01				/* cc_system_id_bitmask for version 1 */
#define CI_CC_MAX_DATA_NBR			4					/* CI+ ���� 1.2 �������� CC �� �������� �󿡼� �ִ� 4�������� ���ȴ� */

/**************************************/
/* Secure Authenticated Channel (SAC) */
/**************************************/

#define CI_CC_SAC_MSG_FLAG_FALSE		0
#define CI_CC_SAC_MSG_FLAG_TRUE		1
#define CI_CC_SAC_MSG_CNT_MAX		0xffffffff

/*********************************/
/* Usage Rules Information (URI) */
/*********************************/

#define CI_CC_URI_TIMEOUT				10000				/* URI message delivery timeout value in ms */
#define CI_CC_URI_PROTOCOL_DEFAULT	0x01

/*****************************************/
/* Tag Length Format (TLF) for Signature */
/*****************************************/

#define CI_CC_TLF_VER_DEFAULT		0x01
#define CI_CC_TLF_MSG_LABEL_HOST_A	0x02				/* msg_label when host authenticate Signature_A */
#define CI_CC_TLF_MSG_LABEL_HOST_V	0x03				/* msg_label when host verify Signature_B */

#define CI_CC_TLF_TAG_VER				0x00
#define CI_CC_TLF_TAG_MSG_LABEL		0x01
#define CI_CC_TLF_TAG_AUTH_NONCE		0x02
#define CI_CC_TLF_TAG_DHPM			0x03
#define CI_CC_TLF_TAG_DHPH			0x04

#define CI_CC_TLF_LEN_VER				0x0008
#define CI_CC_TLF_LEN_MSG_LABEL		0x0008
#define CI_CC_TLF_LEN_AUTH_NONCE		0x0100
#define CI_CC_TLF_LEN_DHPM			0x0800
#define CI_CC_TLF_LEN_DHPH			0x0800


/***************************************************************
* typedef //////////////////////////////////////////////////////
***************************************************************/

/**********************/
/* for "status_field" */
/**********************/
typedef enum {
	CI_CC_STATUS_OK = 0x00,
	CI_CC_STATUS_NO_CC_SUPPORT = 0x01,
	CI_CC_STATUS_HOST_BUSY = 0x02,
	CI_CC_STATUS_AUTH_FAILED_OR_NOT_REQ = 0x03
} CI_CC_STATUS;

/****************************************/
/* Secure Authenticated Channel message */
/****************************************/
/*
	sac_message() {
		message_counter											: 32 bits
		- message header -
		protocol_version										: 4 bits (should be 0)
		authentication_cipher_flag								: 3 bits (indicates the cipher that is used to generate the authentication field. should be 0 = AES-128-XCBC-MAC)
		payload_encryption_flag									: 1 bits (should be 1 for cc_sac_data_req(), cc_sac_data_cnf(), cc_sac_sync_req(), cc_sac_sync_cnf())
		encryption_cipher_flag									: 3 bits (indicates the cipher that is used to encrypt the message payload. should be 0 = AES-128 in CBC mode)
		reserved for future use									: 5 bits
		length_payload											: 16 bits
		- message body -
		if (payload_encryption_flag == MSG_FLAG_TRUE) {
			encrypted_payload									: length_payload * 8 + 128 bits -> If payload is not any multiple of CI_CC_CIPHER_BLOCK_SIZE the message is padded by adding a 1 bit and then 0 bits until the block size is filled (OneAndZeroes padding)
		} else if (payload_encryption_flag == MSG_FLAG_FALSE) {
			payload												: length_payload * 8 bits
			authentication										: 128 bits
		}
	}
*/
typedef struct {
	unsigned		protocol_version:4;
	unsigned		authentication_cipher_flag:3;
	unsigned		payload_encryption_flag:1;
} CI_CC_SAC_MSG_HDR1;	// ����Ϸ��� endian ����ؾ� �ϹǷ� �ϴ� ����μ� ������� ����.

typedef struct {
	unsigned		encryption_cipher_flag:3;
	unsigned		reserved:5;
} CI_CC_SAC_MSG_HDR2;	// ����Ϸ��� endian ����ؾ� �ϹǷ� �ϴ� ����μ� ������� ����.

typedef struct {
	unsigned long	ulMsgCnt;
	unsigned char	ucHdr1;				// CI_CC_SAC_MSG_HDR1
	unsigned char	ucHdr2;				// CI_CC_SAC_MSG_HDR2
	unsigned short	usPayloadLen;
	unsigned char	*pucPayloadData;	// just pointer to payload (don't mem alloc for this)
	unsigned char	ucAuth[16];
} CI_CC_SAC_MSG;

/***********************************************************************************************/
/* Usage Rules Information message (CI+ ���� 1.0 ����Ͽ� 1.1 ���Ŀ��� �Ʒ��� ���� ����Ǿ���) */
/***********************************************************************************************/
/*
	uri_message() {
		protocol_version = URI_DEFAULT	: 8 bits  (default : 0x01     -> CI_CC_URI_PROTOCOL_DEFAULT)
		aps_copy_control_info			: 2 bits  (default : 0b00     -> CI_CC_URI_APS_DEFAULT)
		emi_copy_control_info			: 2 bits  (default : 0b11     -> CI_CC_URI_EMI_DEFAULT)
		ict_copy_control_info			: 1 bit   (default : 0b0      -> CI_CC_URI_ICT_DEFAULT -> if CI_CC_URI_TIMEOUT occurs, CI_CC_URI_ICT_TIMEOUT)
		rct_copy_control_info			: 1 bit   (default : 0b0      -> CI_CC_URI_RCT_DEFAULT)
		reserved for future use			: 4 bits  (all 0s)
		rl_copy_control_info			: 6 bits  (default : 0b000000 -> CI_CC_URI_RL_DEFAULT)
		reserved for future use			: 40 bits (all 0s)
	}
*/
typedef struct {
	unsigned char	ucApsCopyCtrl;		// see CI_CC_URI_APS_
	unsigned char	ucEmiCopyCtrl;		// see CI_CC_URI_EMI_
	unsigned char	ucIctCopyCtrl;		// see CI_CC_URI_ICT_
	unsigned char	ucRctCopyCtrl;		// see CI_CC_URI_RCT_
	unsigned char	ucRlCopyCtrl;		// see CI_CC_URI_RL_
} CI_CC_URI_MSG;

/****************************************/
/* ķ<->ȣ��Ʈ�� ���޵Ǵ� ������ ������ */
/****************************************/
typedef enum {
	CI_CC_Brand_ID = 1,			/* 400 bits  : Defined by the License Document */
	CI_CC_Host_ID = 5,			/* 64 bits   : Generated by the ROT and included in the X.509 certificate. */
	CI_CC_CICAM_ID = 6,			/* 64 bits   : Generated by the ROT and included in the X.509 certificate. */
	CI_CC_Host_BrandCert = 7, 	/* variable  : Host Brand Certificate */
	CI_CC_CICAM_BrandCert = 8,	/* variable  : CICAM Brand Certificate */
	CI_CC_Kp = 12,					/* 256 bits  : CICAM's key precursor to Host for CCK */
	CI_CC_DHPH = 13,				/* 2048 bits : DH Public Key Host */ /* volatile */
	CI_CC_DHPM = 14,				/* 2048 bits : DH Public Key module/CICAM */ /* volatile */
	CI_CC_Host_DevCert = 15, 		/* variable  : Host Device Certificate Data */
	CI_CC_CICAM_DevCert = 16,	/* variable  : CICAM Device Certificate Data */
	CI_CC_Signature_A = 17,		/* 2048 bits : The signature of Host DH public key */
	CI_CC_Signature_B = 18,		/* 2048 bits : The signature of CICAM DH public key */
	CI_CC_auth_nonce = 19,		/* 256 bits  : Random nonce of 256 bits generated by the CICAM and transmitted by the CICAM to the host for use in the authentication protocol. */
	CI_CC_Ns_Host = 20,			/* 64 bits   : Host's challenge to CICAM for SAC */
	CI_CC_Ns_module = 21,			/* 64 bits   : CICAM's challenge to Host for SAC */
	CI_CC_AKH = 22,				/* 256 bits  : Authentication Key Host */ /* non-volatile */ /* host have to reply within 5 sec */ /* If the AKH is not available, then host shall transmit a value of all zeros as an invalid AKH. When receiving this invalid AKH the CICAM starts the authentication protocol. */
	CI_CC_AKM = 23,				/* 256 bits  : Authentication Key Module/CICAM */ /* non-volatile */
	CI_CC_uri_message = 25,		/* 64 bits   : Data message carrying the Usage Rules Information. */
	CI_CC_program_number = 26,	/* 16 bits   : MPEG program number. */
	CI_CC_uri_confirm = 27,		/* 256 bits  : Hash on the data confirmed by the host. */ /* host have to reply within 1 sec and apply URI to external interface within another 1 sec */
	CI_CC_key_register = 28,		/* 8 bits    : (uimsbf) 0 = even, 1 = odd, other values not supported. */
	CI_CC_uri_versions = 29,		/* 256 bits  : Bitmask expressing the URI versions that can be supported by the host. Format is 'uimsbf' */
	CI_CC_status_field = 30,		/* 8 bits    : Status field in APDU confirm messages. */
	CI_CC_srm_data = 31,			/* N/C       : System Renewability Message for HDCP (SRMs for HDCP are defined in HDCP specification v 1.3. First generation SRMs shall not exceed 5 kilobytes.) - APDU�� �Ǿ���⿣ �ʹ��� ū ���̴�!! */
	CI_CC_srm_confirm = 32		/* 256 bits  : Hash on the data confirmed by the host. */
} CI_CC_DATA_ID;

/****************************************************************************************/
/* ķ���� ȣ��Ʈ�� ���޵Ǵ� �����͵� (�ٸ� �����͵��� ���� keep local �ؾ� �ϴ� �͵鸸) */
/****************************************************************************************/
typedef struct {
	unsigned char	CICAM_BrandCert[CI_MAX_CERT_LEN];
	unsigned short	CICAM_BrandCertLen;
	unsigned char	Kp[32];
	unsigned char	DHPM[256];
	unsigned char	CICAM_DevCert[CI_MAX_CERT_LEN];
	unsigned short	CICAM_DevCertLen;
	unsigned char	Signature_B[256];
	unsigned char	auth_nonce[32];
	unsigned char	Ns_module[8];
	unsigned char	uri_message[8];
	CI_CC_URI_MSG	stUri;
	unsigned short	usProgramNumber; // ȣ��Ʈ�� ca_pmt -> ķ -> ķ�� program_number -> ȣ��Ʈ
	CI_CC_KEY_MODE	enKeyRegister;
} CI_CC_CICAM_DATA;

/**********************************************************************************************************************************/
/* ȣ��Ʈ ��ü ���� �����͵� (host license constants / non-volatile �� �ƴϸ鼭 �ٸ� �����͵��� ���� keep local �ؾ� �ϴ� �͵鸸) */
/**********************************************************************************************************************************/
typedef struct {
	unsigned char	Ns_Host[8];
	unsigned char	SEK[16];
	unsigned char	SAK[16];
	unsigned char	UCK[32];
	unsigned char	CCK[16];		// AES-128-CBC ����. DES-56-ECB �� ��쿣 [0-7] �� ���.
	unsigned char	CIV[16];		// AES-128-CBC ����. DES-56-ECB �� ��쿣 ������ ����.
} CI_CC_HOST_DATA;

/************/
/* CC phase */
/************/
typedef enum {

	/* CC ���ҽ� �ʱ�ȭ ���� �Ǵ� CC ���ҽ� ���� ���� ���� */
	CI_CC_PHASE_Init = 0,

	/* CC ���ҽ� ���� ���� ���� */
	CI_CC_PHASE_SS_Opened,

	/* 1. cc_open_req
	   2. cc_open_cnf : cc_system_id_bitmask ���ޱ��� �Ϸ��� ����.
	   *. ȣ��Ʈ�� 5�� ���� �����ؾ� �Ѵ�. */
	CI_CC_PHASE_Host_Capability_Evaluation_Replied,

	/* 1. cc_data_req : CI_CC_auth_nonce �ް�,
	   2. cc_data_cnf : CI_CC_DHPH, CI_CC_Signature_A, CI_CC_Host_DevCert, CI_CC_Host_BrandCert ����,
	   3. cc_data_req : CI_CC_DHPM, CI_CC_Signature_B, CI_CC_CICAM_DevCert, CI_CC_CICAM_BrandCert �ް�,
	   4. cc_data_cnf : CI_CC_status_field ���ޱ��� �Ϸ��� ����.
	   *. ķ�� ȣ��Ʈ�� �̹� ���ε��Ǿ� �ִٸ� �� ������ �Ͼ�� �ʴ´� (CI+ ���� 1.2 ����) */
	CI_CC_PHASE_Authentication_Requested,
	CI_CC_PHASE_Authentication_Replied,

	/* 1. cc_data_req : CI_CC_AKH ��û�ް�,
	   2. cc_data_cnf : CI_CC_AKH ���ޱ��� �Ϸ��� ����.
	   *. ȣ��Ʈ�� 5�� ���� �����ؾ� �Ѵ�. ���� ������ AKH �� AKM �� ��ġ���� ������ ķ�� 5��(CI_NUM_OF_AUTH_CTX)���� ���û�ϰ� CI_ARC_ERR_Auth_Failed_Retries_Exhausted ó���� ���̴� */
	CI_CC_PHASE_Authentication_Key_Verification_Replied,

	/* 1. cc_data_req : CI_CC_CICAM_ID, CI_CC_Ns_module, CI_CC_key_register �ް� (CI+ ���� 1.1 ���Ŀ��� �� ��� CI_CC_key_register�� ���� �ʴ´�),
	   2. cc_data_cnf : CI_CC_Host_ID, CI_CC_Ns_Host ���ޱ��� �Ϸ��� ����.
	   *. ���Ŀ� cc_sync_req : calculating the new SAC key material ��û�ް� �� ���̴� */
	CI_CC_PHASE_SAC_Establishment_Requested,
	CI_CC_PHASE_SAC_Establishment_Replied,

	/* 1. cc_sac_data_req : CI_CC_uri_versions ��û�ް�,
	   2. cc_sac_data_cnf : CI_CC_uri_versions ���ޱ��� �Ϸ��� ����. */
	CI_CC_PHASE_URI_Version_Negotiation_Replied,

	/* 1. cc_sac_data_req : CI_CC_CICAM_ID, CI_CC_Kp, CI_CC_key_register �ް�,
	   2. cc_sac_data_cnf : CI_CC_Host_ID, CI_CC_status_field ���ޱ��� �Ϸ��� ����.
	   *. ���Ŀ� cc_sac_sync_req : calculating the new CC key ��û�ް� �� ���̴� (�̿� ���� ȣ��Ʈ�� 1�� "�̸�" ���� cc_sac_sync_cnf �����ؾ� �Ѵ�) */
	CI_CC_PHASE_CCK_Calculating_Requested,
	CI_CC_PHASE_CCK_Calculating_Replied

	/* ���Ŀ� URI ���� �� CCK refreshing */

} CI_CC_PHASE;
/*
	SMiT CI+ prototype ķ�� ��츦 ���� ��� (AKH �� AKM �� ��ġ���� ���� ��),

	CI_CC_PHASE_Init ->
	CI_CC_PHASE_SS_Opened ->
	CI_CC_PHASE_Host_Capability_Evaluation_Replied ->
	CI_CC_PHASE_Authentication_Key_Verification_Replied -> ...
	CI_CC_PHASE_Authentication_Requested & CI_CC_PHASE_Authentication_Replied ->
	CI_CC_PHASE_Authentication_Key_Verification_Replied ->
	CI_CC_PHASE_SAC_Establishment_Requested -> CI_CC_PHASE_SAC_Establishment_Replied ->
	CI_CC_PHASE_URI_Version_Negotiation_Replied ->
	CI_CC_PHASE_CCK_Calculating_Requested & CI_CC_PHASE_CCK_Calculating_Replied ->
	...
*/

/***************/
/* CC instance */
/***************/
typedef struct {
	unsigned short		usSsNo;
	CI_CC_PHASE			enPhase;
	CI_ARC_ERR			enArcErr;
	CI_SLOT_ID			usSlotId;
	int					nCurAuthCtxIdx;
	int					nTryAuthCtxIdx;
	unsigned long		ulSacMsgCnt;
	unsigned long		ulCcTimerID; /* <TODO_CI_PLUS> : CC �� �������� �󿡼� ķ�� �������� �ʴ� ���(eCiPlus_CcReport_NoResponseFromCam)�� ���� ���� �ڵ鸵�� ó���϶�... */
	unsigned long		ulUriTimerID;
	CI_CC_CICAM_DATA	stCicamData;
	CI_CC_HOST_DATA		stHostData;
} CI_CC_INSTANCE;

/*******************/
/* CC task message */
/*******************/
typedef struct {
	unsigned short		usSsNo;
	unsigned long		ulApduTag;
	unsigned char		*pucApduData;
	unsigned long		ulApduLen;
} CI_CC_TASK_MSG;

/* �Ʒ� define �� Ű ����� ������� ķ�� confirm �� ���� ������ �ֱ� ���� ����Ѵ�.
   �� define �� ������� ������ Ű ������ ��ġ�� ķ�� confirm �� �ְ� �ȴ� (�����ս��� ���ٸ� �̰��� �����). */
#define USE_SUB_TASK_TO_CALCULATE_KEYS

#ifdef USE_SUB_TASK_TO_CALCULATE_KEYS

typedef enum {
	CI_CC_DO_CALCULATE_CCK = 1,
	CI_CC_DO_CALCULATE_SAC
} CI_CC_SUB_TASK_CMD;

typedef struct {
	CI_CC_SUB_TASK_CMD	command;
	CI_CC_INSTANCE		*hCc;
	HBOOL				bDoCalculateCck;
	HBOOL				bDoSetDescrambler;
} CI_CC_SUB_TASK_MSG;

#endif

/*

	=======================================
	 ���� ���� : �ֿ� ȣ��Ʈ Key Hierarchy
	=======================================

	< Credentials layer >

	* Root certificate                                                : stored (license constant) / keep local (not replaceable)
	* Host_BrandCert (Brand certificate)                              : stored (license constant) / exchange   (not replaceable)
	* Host_DevCert   (Device certificate)                             : stored (license constant) / exchange   (not replaceable)

	* Host_ID (64 bits) <- Host Device certificate <- Host Manufacturer certificate <- Host Root certificate

	* DH_p (Diffie-Hellman prime modulus)                             : stored (license constant) / keep local
	* DH_g (Diffie-Hellman generator/base modulus)                    : stored (license constant) / keep local
	* DH_q (Diffie-Hellman Sophie Germain constant)                   : stored (license constant) / keep local
	* HDP  (Host Device Public RSA key)                               : stored                    / exchange
	* HDQ  (Host Device Private RSA key)                              : stored (license constant) / keep local (not replaceable)
	* PRNG_seed (Per product seed for Pseudo-Random Number Generator) : stored (license constant) / keep local (not replaceable)

	< Authentication layer >

	* DHH  (DH nonce exponent Host, 2048 bits)            : volatile / keep local <- PRNG
	* DHPH (DH Public key Host, 2048 bits)                : volatile / exchange   <- DHPH = DH_g ^ DHH mod DH_p
	* DHSK (DH shared Secret Key, private key, 2048 bits) : stored   / keep local <- DHSK = DHPM ^ DHH mod DH_p (ķ �ʿ����� DHSK = DHPH ^ DHM mod DH_p)
	* AKH  (Authentication Key Host, 256 bits)            : stored (on host) / exchange (protected) <- AKH = AKM = SHA-256 (CICAM_ID || Host_ID || DHSK), CICAM_ID : ķ�� ������ ���������� ����.

	< SAC layer >

	* Ns_Host (nonce from Host, 64 bits)            : volatile / exchange   <- PRNG
	* Ks_Host (Key seed Host, 256 bits)             : volatile / keep local <- Ks_Host = SHA-256 (DHSK' || AKH || Ns_Host || Ns_Module), DHSK' = The LSB 128 bits of DHSK
	* SLK     (not mentioned in CI+ spec)           : This is the 128 bit AES key for the f-SAC dummy function.
	* SAK     (SAC Authentication Key, 128 bits)    : volatile / keep local <- SAK = f-SAC (Ks_Host)
	* SEK     (SAC Encryption Key, 128 bits)        : volatile / keep local <- SEK = f-SAC (Ks_Host)
	* SIV     (SAC Initialisation Vector, 128 bits) : stored (license constant) / keep local

	< CC layer >

	* CLK (not mentioned in CI+ spec)                                  : This is the 128 bit AES key for the f-CC dummy function.
	* CCK (CC Key, 64 bits for DES-56-ECB or 128 bits for AES-128-CBC) : volatile / keep local <- CCK = f-CC (Kp)
	* CIV (CC Initialisation Vector)                                   : volatile / keep local <- CIV = f-CC (Kp)

	================================================
	 ���� ���� : ȣ��Ʈ���� ���Ǵ� Cipher Algo ��
	================================================

	* DH  : Authentication �ÿ� DH 3-pass �������� ���� (CI_CC_PHASE_Authentication_Requested ~ CI_CC_PHASE_Authentication_Replied)
	* RSA : RSASSA-PSS + SHA-1.
	        1. CICAM_BrandCert �� CICAM_DevCert�� signature �����ÿ� ����.
	        2. Signature_A �����ÿ� ����. Signature_A = A_RSA {HDQ} (TLF (version || msg_label || auth_nonce || DHPH)).
	        3. Signature_B �����ÿ� ����. TLF (version || msg_label || auth_nonce || DHPH || DHPM) = V_RSA {MDP} (Signature_B).
	* SHA : SHA-256.
	        1. AKH �����ÿ� ����.
	        2. Ks_Host �����ÿ� ����.
	        3. uri_confirm �����ÿ� ����. uri_confirm = SHA-256 (uri_message || UCK), UCK = SHA-256 (SAK).
	        4. srm_confirm �����ÿ� ����.
	* DES : contents�� local re-scramble / descramble �ϴµ� DES-56-ECB ���� -> ���� ķ�� ȣ��Ʈ descrambler logic ���� ó���ϹǷ� ���⼭�� ������� ����.
	        clear_packet = D {CCK} (ts_packet)
	* AES : 1. contents�� local re-scramble / descramble �ϴµ� AES-128-CBC ���� -> ���� ķ�� ȣ��Ʈ descrambler logic ���� ó���ϹǷ� ���⼭�� ������� ����.
	           clear_packet = D {CCK, CIV} (ts_packet)
	        2. DHH, Ns_Host �����ÿ� AES-128-ECB based PRNG ���� (with PRNG_seed which is a 128 bit AES key).
	        3. sac_message()'s encrypted_payload �� decrypt / encrypt �ϴµ� AES-128-CBC ���� (with SIV).
	           encrypted_payload = E {SEK, SIV} (original_unencrypted_payload || authentication)
	           payload || authentication = D {SEK, SIV} (encrypted_payload)
	        4. sac_message()'s authentication �� verify / generate �ϴµ� AES-128-XCBC-MAC ����.
	           authentication = MAC {SAK} (length(header) || message_counter || header || original_unencrypted_payload)

	================================
	 ���� ���� : Certificate Format
	================================

	Certificate ::= SEQUENCE {
	     tbsCertificate     TBSCertificate,
	     signatureAlgorithm AlgorithmIdentifier,
	     signatureValue BIT STRING }

	TBSCertificate ::= SEQUENCE {
	     version        [0]   EXPLICIT Version DEFAULT v1, -> CI+ implementations shall use X.509 version 3 (02).
	     serialNumber         CertificateSerialNumber, -> Each certificate shall include a unique serial number which shall be assigned by the issuer of the certificate.
	     signature            AlgorithmIdentifier, -> All certificates use RSASSA-PSS signatures with the default parameters.
	     issuer               Name,
	     validity             Validity, -> "YYMMDDHHMMSSZ" (The year field shall be interpreted as 20YY)
	     subject              Name, -> If Certificate type is Device certificate, CN: <device ID>.
	                                   The device ID is a hexadecimal number that consists of 16 digits.
	                                   To store this number in an X.501 Common Name (CN) attribute, it must be converted into a string.
	                                   Each digit is represented by the corresponding ASCII code, i.e. 1 is written as 0x31 and 7 as 0x37.
	                                   For the hexadecimal digits A to F, uppercase letters are used (hex values 0x41 to 0x46).
	     subjectPublicKeyInfo SubjectPublicKeyInfo, -> CI+ : The RSA key's public exponent shall be 65537 == 0x10001, the modulus length shall be 1024, 2048 or 3072 bits.
	     issuerUniqueID [1]   IMPLICIT UniqueIdentifier OPTIONAL,
	                          -- If present, version MUST be v2 or v3
	                          -> CI+ certificates shall not use unique identifiers.
	     subjectUniqueID[2]   IMPLICIT UniqueIdentifier OPTIONAL,
	                          -- If present, version MUST be v2 or v3
	                          -> CI+ certificates shall not use unique identifiers.
	     extensions     [3]   EXPLICIT Extensions OPTIONAL
	                          -- If present, version MUST be v3, see CI+ spec "9.3.9 extensions".
	                          -- "Key usage" shall always be present and marked as critical.
	                          -- "Scrambler capabilities" shall be present in each device certificate and marked as critical.
	                          -- The optional "CI+ info" shall be present in a device certificate only and shall NOT be declared as critical.
	}

	Version ::= INTEGER { v1(0), v2(1), v3(2) }

	CertificateSerialNumber ::= INTEGER

	Validity ::= SEQUENCE {
	    notBefore Time,
	    notAfter Time }

	Time ::= CHOICE {
	     utcTime UTCTime,
	     generalTime GeneralizedTime }

	UniqueIdentifier ::= BIT STRING

	SubjectPublicKeyInfo ::= SEQUENCE {
	     algorithm AlgorithmIdentifier,
	     subjectPublicKey BIT STRING }

	Extensions ::= SEQUENCE SIZE (1..MAX) OF Extension

	Extension ::= SEQUENCE {
	     extnID OBJECT IDENTIFIER,
	     critical BOOLEAN DEFAULT FALSE,
	     extnValue OCTET STRING }

*/


/***************************************************************
* global function prototypes ///////////////////////////////////
***************************************************************/

int CiCc_Init(void);
int CiCc_SsOpenedProc(unsigned short usSsNo);
int CiCc_SsClosedProc(unsigned short usSsNo);
int CiCc_ApduProc(unsigned short usSsNo, unsigned long ulApduTag,
					unsigned char *pucApduData, unsigned long ulApduLen);

int CiCc_GetHostId(unsigned char *pucHostId); // �� �Լ��� ȣ���� CiCc_Task() �⵿�� �Ϸ�� �Ŀ��� ��ȿ�� ����� �ܿ� ������ ��
int CiCc_GetBindedCamId(CI_SLOT_ID usSlotId, unsigned char *pucBindedCamId); // �� �Լ��� ȣ���� ķ ������ ���� �ð� ����� �Ŀ��� ��ȿ�� ����� �ܿ� ������ ��
int CiCc_GetBindedCamBrandId(CI_SLOT_ID usSlotId, unsigned short *pusBindedCamBrandId); // �� �Լ��� ȣ���� ķ ������ ���� �ð� ����� �Ŀ��� ��ȿ�� ����� �ܿ� ������ ��
int CiCc_GetNegotiatedCckCipher(CI_SLOT_ID usSlotId, CI_CC_KEY_CIPHER *penNegotiatedCckCipher); // �� �Լ��� ȣ���� ķ ������ ���� �ð� ����� �Ŀ��� ��ȿ�� ����� �ܿ� ������ ��
int CiCc_SetNegotiatedCckCipher(CI_SLOT_ID usSlotId, CI_CC_KEY_CIPHER enNegotiatedCckCipher); // �׽�Ʈ �뵵�� �Լ���. �ٸ� �뵵�� ������� �� ��!
void CiCc_ReqSetCckToDesc(CI_SLOT_ID usSlotId); // �� �Լ��� ȣ���� ķ ������ ���� �ð� ����� �Ŀ��� ��ȿ�� ����� �ܿ� ������ ��
void CiCc_SetUriDefault(CI_SLOT_ID usSlotId, HBOOL bOn); // �� �Լ��� ȣ���� ķ ������ ���� �ð� ����� �Ŀ��� ��ȿ�� ����� �ܿ� ������ ��
int CiCc_IsInitialized(void); // �� �Լ��� ȣ���� CiCc_Task() �⵿�� �Ϸ�� �Ŀ��� ��ȿ�� ����� �ܿ� ������ ��
int CiCc_IsOpened(CI_SLOT_ID usSlotId); // �� �Լ��� ȣ���� ķ ������ ���� �ð� ����� �Ŀ��� ��ȿ�� ����� �ܿ� ������ ��
int CiCc_IsBusy(CI_SLOT_ID usSlotId); // �� �Լ��� ȣ���� ķ ������ ���� �ð� ����� �Ŀ��� ��ȿ�� ����� �ܿ� ������ ��

void CiCc_PrintDebugInfo(CI_SLOT_ID usSlotId, int flagPrintMoreDetail);


#ifdef __cplusplus
}
#endif

#endif /* _CI_CC_MAIN_H_ */

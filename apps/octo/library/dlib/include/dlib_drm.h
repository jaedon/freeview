/* $Header: $ */
#ifndef DLIB_DRM_H__
#define DLIB_DRM_H__

#ifdef __cplusplus
extern "C" {
#endif
#if 0
#include <hlib.h>

typedef enum
{
	eDxDRM_RESULT_SUCCESS,
	eDxDRM_RESULT_UNKNOWN_ERROR,
	eDxDRM_RESULT_CANNOT_PROCESS_REQUEST,
	eDxDRM_RESULT_UNKNOWN_MIME_TYPE,
	eDxDRM_RESULT_USER_CONSENT_NEEDED,
	eDxDRM_RESULT_UNKNOWN_DRM_SYSTEM,
	eDxDRM_RESULT_WRONG_FORMAT,
} eDxDRM_Result_e;

// CI+ DRM ������ Metafile�� ���� ���θ� Ȯ�� �ϱ����Ͽ� ����ϴ� enum
typedef enum
{
	eDxDRM_CIPLUS_WRITE_NONE,
	eDxDRM_CIPLUS_WRITE_WRITING,
	eDxDRM_CIPLUS_WRITE_DONE
} DxDRM_CIPLUS_Write_e;

typedef enum
{
	eDxDRM_CIPLUS_CCV1_RECORD_OK,
	eDxDRM_CIPLUS_CCV2_RECORD_WAIT,			// CC RESOURCE VER2 SPECIFICATION ���� �ٷ�� RECORDING START PROTOCOL�� ���� RECORDING CONTENT�� ��Ÿ ��
	eDxDRM_CIPLUS_CCV2_RECORD_START,
	eDxDRM_CIPLUS_CCV2_RECORD_ERROR,
//	eDxDRM_CIPLUS_CCV2_RECORD_STOP,

	eDxDRM_CIPLUS_CC_RECORD_MAX 		= 7
} DxDRM_CIPLUS_Record_e;


typedef struct
{
	DxDRM_CIPLUS_Record_e			eCiPlusCcRecInfo;
} DxDRM_CIPLUS_RecordCtrl_t;

typedef struct
{
	HUINT16						usProgramNum;
	HUINT8						*pucLicenseData;
	HUINT16						usLicenseLength;
	HUINT8						aucBindedCamId[8];
} DxDRM_CIPLUS_License_t;

typedef struct
{
	HUINT32						utTimeStamp;
	HUINT16						usProgramNum;
	HUINT8						ucAgeLimit;		// ���� ���� (HUINT8 �� �������� �ʴٸ� �ٲ㵵 ��!)
	HUINT8						aucPrivateData[15];
	HUINT8						aucBindedCamId[8];
} DxDRM_CIPLUS_Rating_t;

typedef struct
{
	unsigned char 				ucMode;			// see CI_CC_URI_MODE_
	unsigned char 				ucAps;			// see CI_CC_URI_APS_
	unsigned char 				ucEmi;			// see CI_CC_URI_EMI_
	unsigned char 				ucIct;			// see CI_CC_URI_ICT_
	unsigned char 				ucRct;			// see CI_CC_URI_RCT_
	unsigned char 				ucDot;			// see CI_CC_URI_DOT_
	unsigned long 				ulMinutesForRL;		// 0 means unlimited retention
} DxDRM_CIPLUS_URI_t;

#define	DxDRM_CIPLUS_CAMID_LEN	8
typedef struct
{
	HBOOL		bCamNeedContent;
	HUINT8		aucBindedCamId[DxDRM_CIPLUS_CAMID_LEN];
} DxDRM_CIPLUS_CamID_t;

// CI+ ���ۿ� �־� DRM ������ ������ �ִ� ���ۿ� ���Ͽ� �� ���۸��� type�� �޸��Ͽ� ó��
typedef enum
{
	eDxDRM_CIPLUS_DRM_TYPE_URI		= 0,
	eDxDRM_CIPLUS_DRM_TYPE_RECCTRL,
	eDxDRM_CIPLUS_DRM_TYPE_LICENSE,
	eDxDRM_CIPLUS_DRM_TYPE_PARENTALCTRL,

	// �߰����� ó���� �ʿ��� ��� ��� �߰� (MAX�� 7�� ���� ����� �� ����. �� ����ؾ� �� ��� code ���� �ʿ�)
	// Max 7�� ����ϴ� ������ ���� CI MGR���� DRM ������ �ø��� 4byte�� ��� ������ ��ֱܳ� ���ؼ�����. ����� CI MGR������ ���� ���� ���� �� �ִ�.
	// ���� CI MGR ver.1 �κ��� local_mwcutil_ConvertCiPlusDrmInfoToDrmMsgArg() �Լ��� �����ϸ� �� �׷��߸� �ߴ��� �� �� ����.

	eDxDRM_CIPLUS_DRM_TYPE_MAX		= 7
} DxDRM_CIPLUS_DrmType_e;


typedef struct
{
	DxDRM_CIPLUS_DrmType_e 	eCiPlusDrmType;
	HUINT8					ucCiPlusDrmInfoVer;
	DxDRM_CIPLUS_CamID_t	stCiPlusCamID;
	union{
		DxDRM_CIPLUS_URI_t			stCiPlusUriInfo;
		DxDRM_CIPLUS_RecordCtrl_t	stCiPlusRecCtrl;
		DxDRM_CIPLUS_License_t		stCiPlusLicenseInfo;
		DxDRM_CIPLUS_Rating_t		stCiPlusParentalCtrlInfo;
	} ciplusDrmInfo;
} DxDRM_CiPlus_t;


typedef struct
{
	HUINT16	usSvcType;
	HUINT8	ucCopyControlType;		/* 2bits */
	HUINT8	ucDigitalRecording;		/* 2bits */
	HUINT8	ucAnalogRecording;		/* 2bits */
	HUINT8	ucCopyRestrictionMode;	/* For Dubbing-10 */
	HUINT8	ucImageContraintToken;
	HUINT8	ucEncryptionMode;
	HUINT8	ucRetentionMode;
	HUINT8	ucRetentionState;
}DxDRM_AribCpInfo_t;

typedef enum
{
	eDxDRM_TYPE_NONE,
	eDxDRM_TYPE_V1,
	eDxDRM_TYPE_CIPLUS,
	eDxDRM_TYPE_ARIB_CPINFO,
	eDxDRM_TYPE_MAX
} DxDRM_Type_e;

typedef struct
{
	HINT32	type;
	HUINT8	reserved[60];
} DxDRM_V1_t;

typedef struct
{
	DxDRM_Type_e	type;

	union {
		DxDRM_V1_t			v1;
		DxDRM_CiPlus_t		ciplus;
		DxDRM_AribCpInfo_t	AribCpInfo;
		// ����� �߰��Ͻø� �ǰ�, size 60byte ���ర���� ������ ���� �߰��ϼ���.
	} u;
} DxDRM_t;

HCHAR *		DxDRM_Encode (HINT32 jsonWriter, const DxDRM_t *drm, const HxCryptKey_t *key);
DxDRM_t *	DxDRM_Decode (DxDRM_t *drm, HxJSON_t json, const HxCryptKey_t *key);
DxDRM_t	*	DxDRM_Clone  (const DxDRM_t *drm);
void		DxDRM_Delete (DxDRM_t *drm);
HINT32		DxDRM_Compare (const DxDRM_t *lhs, const DxDRM_t *rhs);
HCHAR *		DxDRM_Encrypt (const HCHAR *json, const HxCryptKey_t *key);
HCHAR *		DxDRM_Decrypt (const HCHAR *base64, const HxCryptKey_t *key);
void		DxDRM_FreeData(void *p);

#endif
#ifdef __cplusplus
}
#endif

#endif // DLIB_DRM_H__



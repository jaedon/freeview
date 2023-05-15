#ifndef _DI_DSC_H_
#define _DI_DSC_H_

/********************************************************************
 Header Files
 ********************************************************************/
#include "htype.h"
#include "di_err.h"

/***************************************************************
  Macro definition
***************************************************************/
/***************************************************************
  Type definition
***************************************************************/
typedef enum {
	DI_DSC_DESC_DVB,
	DI_DSC_DESC_AES,
	DI_DSC_DESC_DES,
	DI_DSC_DESC_AES_CLEARMODE,	//CI+
	DI_DSC_DESC_DES_CLEARMODE,	//CI+
	DI_DSC_DESC_3DESABA,
	DI_DSC_DESC_MULTI2
} DI_DSC_DESC_TYPE;

typedef enum {
	DI_DSC_DESC_ODD,
	DI_DSC_DESC_EVEN
} DI_DSC_DESC_ODD_EVEN;

typedef enum DI_DSC_DESC_AlgorithmVariant
{
	DI_DSC_DESC_AlgorithmVariant_eEcb,
	DI_DSC_DESC_AlgorithmVariant_eCbc,
	DI_DSC_DESC_AlgorithmVariant_eCounter,
	/* Add new algorithm variant definition before this line */
	DI_DSC_DESC_AlgorithmVariant_eMax
}   DI_DSC_DESC_AlgorithmVariant;

typedef struct DI_DSC_EncryptionSettings
{
	DI_DSC_DESC_AlgorithmVariant	algorithmVar;
	HBOOL	bIsUsed;
	HUINT16	multi2KeySelect;
	HUINT8	multi2Rounds;
	HUINT8	multi2SysKey[32];
} DI_DSC_EncryptionSettings;

/**
@ingroup Demux
Get Descrambler id.
@param[in] HUINT32 ulDemuxId
@param[in] DESC_TYPE type
@param[out] HUINT32 *DescId
@retval DI_ERR_CODE
*/
DI_ERR_CODE DI_DSC_Open(HUINT32 ulDemuxId, DI_DSC_DESC_TYPE type, HUINT32 *DescId, DI_DSC_EncryptionSettings *encryptionSetting);

/**
@ingroup Demux
Set pid for descrambler id.
@param[in] HUINT32 DescId
@param[in] HUINT16 pid
@retval DI_ERR_CODE
*/
DI_ERR_CODE DI_DSC_SetPid(HUINT32 DescId, HUINT16 pid);
DI_ERR_CODE DI_DSC_ClearPid(HUINT32 DescId, HUINT16 pid);

/**
@ingroup Demux
Close descrambler id.
@param[in] HUINT32 DescId
@retval DI_ERR_CODE
*/
DI_ERR_CODE DI_DSC_Close(HUINT32 DescId);

/**
@ingroup Demux
Set key for descrambler.
@param[in] HUINT32 DescId
@param[in] DESC_ODD_EVEN parity
@param[in] HUINT8 *p_key_data
@param[in] HUINT32 key_length
@param[in] HUINT8 *p_init_vector
@param[in] HUNIT32 vector_length
@retval DI_ERR_CODE
*/
DI_ERR_CODE DI_DSC_SetKey(HUINT32 DescId,
DI_DSC_DESC_ODD_EVEN parity,
HUINT8 *p_key_data,
HUINT32 key_length,
HUINT8 *p_init_vector,                          // 사용하지 않는 경우 NULL
HUINT32 vector_length);                        // 사용하지 않는 경우 '0'

DI_ERR_CODE DI_DSC_SetProtectionKey(HUINT32 DescId, HUINT8 *pCWPK, HUINT32 key_length);
DI_ERR_CODE DI_DSC_ClearProtectionKey(HUINT32 DescId);
DI_ERR_CODE DI_DSC_SetMulti2SysKey(HUINT32 DescId, DI_DSC_EncryptionSettings *encryptionSetting);

// verimatrix iptv, refer to CONFIG_CAS_VE
DI_ERR_CODE BRCM_SetVmxDemuxId(HUINT32 ulDemuxId);
DI_ERR_CODE BRCM_SetAlgorithmConfig(HUINT8 ucSvcIdx, HUINT8 ucEncAlgo);

#endif /* _DI_DSC_H_ */


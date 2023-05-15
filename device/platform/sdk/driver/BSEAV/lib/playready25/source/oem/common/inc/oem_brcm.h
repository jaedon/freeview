/**************************************************************************
///    Copyright (c)2010, Broadcom Corporation
///    All Rights Reserved
///    Confidential Property of Broadcom Corporation
///
/// THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
/// AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
/// EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
///
/// @brief Header file containing hardware specific functions for the OEM module
///
*****************************************************************************/

#ifndef OEM_BRCM_H
#define OEM_BRCM_H


/* bcm_oem_init.c*/
#if PRDY_ROBUSTNESS
DRM_VOID * BCM_Oem_Create_1st();

DRM_VOID BCM_Oem_Create_2nd(DRM_VOID *f_pOEMContext);

DRM_VOID BCM_Oem_Destroy(DRM_VOID *f_pOEMContext);
#endif

DRM_VOID * BCM_Oem_MemHeapAlloc(
    DRM_DWORD f_cbSize
);

DRM_VOID BCM_Oem_MemHeapFree(
    DRM_VOID *f_pv
);


/* bcm_oemimpl.c */
DRM_VOID * BCM_Oem_MemAlloc(
    DRM_DWORD f_cbSize );


DRM_VOID BCM_Oem_MemFree(
    DRM_VOID *f_pv );


DRM_VOID* BCM_Oem_MemRealloc(
    DRM_VOID *f_pv,
    DRM_DWORD f_cbSize );

#if PRDY_ROBUSTNESS

DRM_RESULT BCM_Oem_Random_GetBytes(
    DRM_VOID *f_pOEMContext,
    DRM_BYTE  *f_pbData,
    DRM_DWORD  f_cbData );
#endif


/* bcm_oem_m2m_dma.c*/
DRM_RESULT BCM_DecryptFromFile (DRM_VOID  *f_pOEMContext,
	const   DRM_DEVICE_ID_TYPE   f_eIDType,
	uint8_t* pDmaBuf,
	DRM_DWORD   cbFile,
	uint8_t *pKey);


DRM_RESULT BCM_EncryptToFile (DRM_VOID  *f_pOEMContext,
	const   DRM_DEVICE_ID_TYPE   f_eIDType,
	uint8_t* pDmaBuf,
	DRM_DWORD   cbFile,
	uint32_t size,
	uint8_t *pKey);


DRM_RESULT BCM_GetPrPrivateKey(
    DRM_VOID *f_pOEMContext,
    const  DRM_DEVICE_KEY_TYPE    f_eKeyType,
    DRM_BYTE *f_pbPrivkey,
    DRM_DWORD f_cbPrivKey );


DRM_RESULT BCM_GetPrivateKey(
    DRM_VOID *f_pOEMContext,
    const DRM_BOOL f_fGCprivkey,
    DRM_BYTE *f_pbPrivkey,
    DRM_DWORD f_cbPrivKey );


DRM_VOID BCM_Setpr_keys_valid(	DRM_VOID *f_pOEMContext, DRM_BOOL pr_keys_valid);


DRM_BOOL BCM_Getpr_keys_valid(DRM_VOID *f_pOEMContext);


DRM_VOID BCM_SetZGroupCert(	DRM_VOID *f_pOEMContext, uint8_t *pZGroupCert);


uint8_t * BCM_GetZGroupCert( DRM_VOID *f_pOEMContext);


DRM_VOID BCM_SetZGroupCertSize( DRM_VOID *f_pOEMContext, uint32_t zgroupCertSize);


unsigned int BCM_GetZGroupCertSize( DRM_VOID *f_pOEMContext);


DRM_VOID BCM_Setrevoked(	 DRM_VOID *f_pOEMContext, DRM_BOOL revoked);


DRM_BOOL BCM_Getrevoked( DRM_VOID *f_pOEMContext);


DRM_VOID BCM_Setwmdrm_keys_valid(	 DRM_VOID *f_pOEMContext, DRM_BOOL wmdrm_keys_valid);


DRM_BOOL BCM_Getwmdrm_keys_valid( DRM_VOID *f_pOEMContext);


DRM_VOID BCM_SetDevTemplate( DRM_VOID *f_pOEMContext, uint8_t *pDevTemplate);


uint8_t * BCM_GetDevTemplate( DRM_VOID *f_pOEMContext);


unsigned int BCM_GetDevTemplateSize( DRM_VOID *f_pOEMContext);


DRM_RESULT BCM_GetChipUniqueID(uint8_t *pucChipID, uint32_t ulChipIDLength);


uint8_t* BCM_Getwmdrm_serial_nb( DRM_VOID *f_pOEMContext);


uint8_t* BCM_Getprdy_serial_nb( DRM_VOID *f_pOEMContext);


DRM_RESULT BCM_ReadBinFilePrdy(
	DRM_VOID *f_pOEMContext,
	unsigned char cust_key,
	unsigned char var_lo,
	unsigned char var_hi,
	unsigned char *SerialNumber,
	void *playready_data,
	unsigned int playready_data_size,
	void *DeviceCertificateTemplateSize,
	unsigned char *pSignature,
	unsigned char *prdyBinFile,
	unsigned int group_cert_offset,
	unsigned char *key);


DRM_RESULT BCM_ReadBinFileWmdrm(
	DRM_VOID *f_pOEMContext,
	unsigned char cust_key,
	unsigned char var_lo,
	unsigned char var_hi,
	unsigned char *SerialNumber,
	void *wmdrm_data,
	unsigned int wmdrm_data_size,
	void *reserved,
	unsigned char *pSignature,
	unsigned char *prdyBinFile,
	unsigned int cbRead,
	unsigned char *key,
	uint32_t *pencTemplateSize);


DRM_RESULT BCM_Generate_Random_Number(DRM_BYTE *f_rgbSecStorePassword, DRM_DWORD  *f_pcbBuffer);


int BCM_DECRYPTOR_SOC_Init(void);
int BCM_DECRYPTOR_SOC_Uninit(void);

void*  BCM_DECRYPTOR_SOC_Create(int eDecType);
void BCM_DECRYPTOR_SOC_GetType(void* tDecCtx, int *eDecType);

int BCM_DECRYPTOR_SOC_Destroy(void* tDecCtx);

int BCM_DECRYPTOR_SOC_DoDecrypt(void* tDecCtx, unsigned char *pData, unsigned int ulSize, unsigned char *key, unsigned char *iv);
int BCM_DECRYPTOR_SOC_DoDecrypt_ExCTR(void* tDecCtx, unsigned char *pData, unsigned int ulSize, unsigned char *key, unsigned char *iv, unsigned char *ecount_buf, unsigned int *num);

/* SOC Malloc */
void *BCM_DECRYPTOR_SOC_Malloc(unsigned int unBytes);
void BCM_DECRYPTOR_SOC_Free(void *pMemory);

/* Enc/Dec with OTP Key */
int BCM_DECRYPTOR_SOC_OTPKEY_DoEncrypt(unsigned char *in, unsigned char *out, unsigned int size);
int BCM_DECRYPTOR_SOC_OTPKEY_DoDecrypt(unsigned char *in, unsigned char *out, unsigned int size);


#endif /*OEM_BRCM_H*/

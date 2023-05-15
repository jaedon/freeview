/*****************************************************************************
 *    Copyright (c)2010, Broadcom Corporation
 *    All Rights Reserved
 *    Confidential Property of Broadcom Corporation
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * @brief Header file containing hardware specific functions for the OEM module
 *
 *****************************************************************************/

#ifndef OEM_CONFIG_H
#define OEM_CONFIG_H


#ifdef __cplusplus
extern "C" {
#endif

ENTER_PK_NAMESPACE_CODE;

#ifdef PRDY_ROBUSTNESS
#include <drm_data.h>
#else 


wchar_t * bdrm_get_revocation_fname(void);
uint32_t  bdrm_get_revocation_fname_lgth(void);

/******************************************************************************
** FUNCTION:
**   bdrm_get_cert_dir
**
** DESCRIPTION:
**   This function is used to retrieve the string containing the folder where
** the certificates will be saved in the file system.
**
** PARAMETERS:
**   N/A
**
** RETURNS:
**   A string containing the folder where the certificates will be saved in the 
** file system.
**
** NOTES:
**  The playready libraries and drmparms are compiled using -fshort-wchar. Make 
** sure that your application uses the same compilation flags to avoid unexpected
** behaviors.
**
** SEE ALSO:
**  bdrm_get_cert_dir_lgth
**
******************************************************************************/
wchar_t * bdrm_get_cert_dir();

/******************************************************************************
** FUNCTION:
**   bdrm_get_cert_dir_lgth
**
** DESCRIPTION:
**   This function is used to retrieve the length of the string containing the 
** folder where the certificates will be saved in the file system.
**
** PARAMETERS:
**   N/A
**
** RETURNS:
**   The length of the string in bytes.
**
** NOTES:
**  The playready libraries and drmparms are compiled using -fshort-wchar. Make 
** sure that your application uses the same compilation flags to avoid unexpected
** behaviors.
**
** SEE ALSO:
**  bdrm_get_cert_dir
**
******************************************************************************/
uint32_t  bdrm_get_cert_dir_lgth();

/******************************************************************************
** FUNCTION:
**   bdrm_get_cert_templates_dir
**
** DESCRIPTION:
**   This function is used to retrieve the string containing the folder where 
** the certificates templates will be saved in the file system.
**
** PARAMETERS:
**   N/A
**
** RETURNS:
**   A string containing the folder where the certificates templates will be 
** saved in the file system.
**
** NOTES:
**  The playready libraries and drmparms are compiled using -fshort-wchar. Make 
** sure that your application uses the same compilation flags to avoid unexpected
** behaviors.
**
** SEE ALSO:
**  bdrm_get_cert_templates_dir_lgth
**
******************************************************************************/
wchar_t * bdrm_get_cert_templates_dir();

/******************************************************************************
** FUNCTION:
**   bdrm_get_cert_templates_dir_lgth
**
** DESCRIPTION:
**   This function is used to retrieve the length of the string containing the 
** folder where the certificates templates will be saved in the file system..
**
** PARAMETERS:
**   N/A
**
** RETURNS:
**   The length of the string in bytes.
**
** NOTES:
**  The playready libraries and drmparms are compiled using -fshort-wchar. Make 
** sure that your application uses the same compilation flags to avoid unexpected
** behaviors.
**
** SEE ALSO:
**  bdrm_get_cert_templates_dir
**
******************************************************************************/
uint32_t  bdrm_get_cert_templates_dir_lgth();


/******************************************************************************
** FUNCTION:
**   bdrm_get_hds_dir
**
** DESCRIPTION:
**   This function is used to retrieve the string containing the folder where the
** data store files will be saved in the file system.
**
** PARAMETERS:
**   N/A
**
** RETURNS:
**   A string containing the folder where the data store files will be saved in 
** the file system.
**
** NOTES:
**  The playready libraries and drmparms are compiled using -fshort-wchar. Make 
** sure that your application uses the same compilation flags to avoid unexpected
** behaviors.
**
** SEE ALSO:
**  bdrm_get_hds_dir_lgth
**
******************************************************************************/
wchar_t * bdrm_get_hds_dir();

/******************************************************************************
** FUNCTION:
**   bdrm_get_hds_dir_lgth
**
** DESCRIPTION:
**   This function is used to retrieve the length of the string containing the 
** folder where the data store files will be saved in the file system.
**
** PARAMETERS:
**   N/A
**
** RETURNS:
**   The length of the string in bytes.
**
** NOTES:
**  The playready libraries and drmparms are compiled using -fshort-wchar. Make 
** sure that your application uses the same compilation flags to avoid unexpected
** behaviors.
**
** SEE ALSO:
**  bdrm_get_hds_dir
**
******************************************************************************/
uint32_t  bdrm_get_hds_dir_lgth();

/******************************************************************************
** FUNCTION:
**   bdrm_get_hds_fname
**
** DESCRIPTION:
**   This function is used to retrieve the string containing the data store filename.
**
** PARAMETERS:
**   N/A
**
** RETURNS:
**   A string containing the data store filename.
**
** NOTES:
**  The playready libraries and drmparms are compiled using -fshort-wchar. Make 
** sure that your application uses the same compilation flags to avoid unexpected
** behaviors.
**
** SEE ALSO:
**  bdrm_get_hds_fname_lgth
**
******************************************************************************/
wchar_t * bdrm_get_hds_fname();

/******************************************************************************
** FUNCTION:
**   bdrm_get_hds_fname_lgth
**
** DESCRIPTION:
**   This function is used to retrieve the length of the data store filename.
**
** PARAMETERS:
**   N/A
**
** RETURNS:
**   The length of the filename in bytes.
**
** NOTES:
**  The playready libraries and drmparms are compiled using -fshort-wchar. Make 
** sure that your application uses the same compilation flags to avoid unexpected
** behaviors.
**
** SEE ALSO:
**  bdrm_get_hds_fname
**
******************************************************************************/
uint32_t  bdrm_get_hds_fname_lgth();

/******************************************************************************
** FUNCTION:
**   bdrm_get_hds_pwd_fname
**
** DESCRIPTION:
**   This function is used to retrieve the string containing the data store 
** password filename.
**
** PARAMETERS:
**   N/A
**
** RETURNS:
**   A string containing the data store password filename.
**
** NOTES:
**  The playready libraries and drmparms are compiled using -fshort-wchar. Make 
** sure that your application uses the same compilation flags to avoid unexpected
** behaviors.
**
** SEE ALSO:
**  bdrm_get_hds_pwd_fname_lgth
**
******************************************************************************/
wchar_t * bdrm_get_hds_pwd_fname();

/******************************************************************************
** FUNCTION:
**   bdrm_get_hds_pwd_fname_lgth
**
** DESCRIPTION:
**   This function is used to retrieve the length of the data store password 
** filename.
**
** PARAMETERS:
**   N/A
**
** RETURNS:
**   The length of the filename in bytes.
**
** NOTES:
**  The playready libraries and drmparms are compiled using -fshort-wchar. Make 
** sure that your application uses the same compilation flags to avoid unexpected
** behaviors.
**
** SEE ALSO:
**  bdrm_get_hds_pwd_fname
**
******************************************************************************/
uint32_t  bdrm_get_hds_pwd_fname_lgth();

/******************************************************************************
** FUNCTION:
**   bdrm_get_dev_cert_fname_lgth
**
** DESCRIPTION:
**   This function is used to retrieve the string containing the device certificate 
** filename.
**
** PARAMETERS:
**   N/A
**
** RETURNS:
**   A string containing the device certificate filename.
**
** NOTES:
**  The playready libraries and drmparms are compiled using -fshort-wchar. Make 
** sure that your application uses the same compilation flags to avoid unexpected
** behaviors.
**
** SEE ALSO:
**  bdrm_get_dev_cert_fname
**
******************************************************************************/
wchar_t * bdrm_get_dev_cert_fname();

/******************************************************************************
** FUNCTION:
**   bdrm_get_dev_cert_fname_lgth
**
** DESCRIPTION:
**   This function is used to retrieve the length of the device certificate filename.
**
** PARAMETERS:
**   N/A
**
** RETURNS:
**   The length of the filename in bytes.
**
** NOTES:
**  The playready libraries and drmparms are compiled using -fshort-wchar. Make 
** sure that your application uses the same compilation flags to avoid unexpected
** behaviors.
**
** SEE ALSO:
**  bdrm_get_dev_cert_fname
**
******************************************************************************/
uint32_t  bdrm_get_dev_cert_fname_lgth();


/******************************************************************************
** FUNCTION:
**   bdrm_get_zdev_cert_fname
**
** DESCRIPTION:
**   This function is used to retrieve the string containing the binary format
** device certificate filename.
**
** PARAMETERS:
**   N/A
**
** RETURNS:
**  A string containing the binary format device certificate filename.
**
** NOTES:
**  The playready libraries and drmparms are compiled using -fshort-wchar. Make 
** sure that your application uses the same compilation flags to avoid unexpected
** behaviors.
**
** SEE ALSO:
**  bdrm_get_zdev_cert_fname_lgth
**
******************************************************************************/
wchar_t * bdrm_get_zdev_cert_fname();

/******************************************************************************
** FUNCTION:
**   bdrm_get_zdev_cert_fname_lgth
**
** DESCRIPTION:
**   This function is used to retrieve the length of the binary format device 
** certificate filename.
**
** PARAMETERS:
**   N/A
**
** RETURNS:
**   The length of the filename in bytes.
**
** NOTES:
**  The playready libraries and drmparms are compiled using -fshort-wchar. Make 
** sure that your application uses the same compilation flags to avoid unexpected
** behaviors.
**
** SEE ALSO:
**  bdrm_get_zdev_cert_fname
**
******************************************************************************/
uint32_t  bdrm_get_zdev_cert_fname_lgth();

/******************************************************************************
** FUNCTION:
**   bdrm_get_ndr_cert_fname
**
** DESCRIPTION:
**   This function is used to retrieve the string containing the NRD certificate
** filename.
**
** PARAMETERS:
**   N/A
**
** RETURNS:
**  A string containing the NRD certificate filename.
**
** NOTES:
**  The playready libraries and drmparms are compiled using -fshort-wchar. Make 
** sure that your application uses the same compilation flags to avoid unexpected
** behaviors.
**
** SEE ALSO:
**  bdrm_get_ndr_cert_fname_lgth
**
******************************************************************************/
wchar_t * bdrm_get_ndr_cert_fname();

/******************************************************************************
** FUNCTION:
**   bdrm_get_ndr_cert_fname_lgth
**
** DESCRIPTION:
**   This function is used to retrieve the length of the NRD certificate filename.
**
** PARAMETERS:
**   N/A
**
** RETURNS:
**   The length of the filename in bytes.
**
** NOTES:
**  The playready libraries and drmparms are compiled using -fshort-wchar. Make 
** sure that your application uses the same compilation flags to avoid unexpected
** behaviors.
**
** SEE ALSO:
**  bdrm_get_ndr_cert_fname
**
******************************************************************************/
uint32_t  bdrm_get_ndr_cert_fname_lgth();

/******************************************************************************
** FUNCTION:
**   bdrm_get_ndr_cert_template_fname
**
** DESCRIPTION:
**   This function is used to retrieve the string containing the NRD certificate
** template filename.
**
** PARAMETERS:
**   N/A
**
** RETURNS:
**  A string containing the NRD certificate template filename.
**
** NOTES:
**  The playready libraries and drmparms are compiled using -fshort-wchar. Make 
** sure that your application uses the same compilation flags to avoid unexpected
** behaviors.
**
** SEE ALSO:
**  bdrm_get_ndr_cert_template_fname_lgth
**
******************************************************************************/
wchar_t * bdrm_get_ndr_cert_template_fname();

/******************************************************************************
** FUNCTION:
**   bdrm_get_ndr_cert_template_fname_lgth
**
** DESCRIPTION:
**   This function is used to retrieve the length of the NRD certificate template
** filename.
**
** PARAMETERS:
**   N/A
**
** RETURNS:
**   The length of the filename in bytes.
**
** NOTES:
**  The playready libraries and drmparms are compiled using -fshort-wchar. Make 
** sure that your application uses the same compilation flags to avoid unexpected
** behaviors.
**
** SEE ALSO:
**  bdrm_get_ndr_cert_template_fname
**
******************************************************************************/
uint32_t  bdrm_get_ndr_cert_template_fname_lgth();

/******************************************************************************
** FUNCTION:
**   bdrm_get_zgpriv_sig_key_fname
**
** DESCRIPTION:
**   This function is used to retrieve the string containing the device signing 
** public key filename.
**
** PARAMETERS:
**   N/A
**
** RETURNS:
**  A string containing the device signing public key filename.
**
** NOTES:
**  The playready libraries and drmparms are compiled using -fshort-wchar. Make 
** sure that your application uses the same compilation flags to avoid unexpected
** behaviors.
**
** SEE ALSO:
**  bdrm_get_zgpriv_sig_key_fname_lgth
**
******************************************************************************/
wchar_t * bdrm_get_zgpriv_sig_key_fname();

/******************************************************************************
** FUNCTION:
**   bdrm_get_zgpriv_sig_key_fname_lgth
**
** DESCRIPTION:
**   This function is used to retrieve the length of the device signing public key
** filename.
**
** PARAMETERS:
**   N/A
**
** RETURNS:
**   The length of the filename in bytes.
**
** NOTES:
**  The playready libraries and drmparms are compiled using -fshort-wchar. Make 
** sure that your application uses the same compilation flags to avoid unexpected
** behaviors.
**
** SEE ALSO:
**  bdrm_get_zgpriv_sig_key_fname
**
******************************************************************************/
uint32_t  bdrm_get_zgpriv_sig_key_fname_lgth();

/******************************************************************************
** FUNCTION:
**   bdrm_get_zgpriv_enc_key_fname
**
** DESCRIPTION:
**   This function is used to retrieve the string containing the device encryption
** key filename.
**
** PARAMETERS:
**   N/A
**
** RETURNS:
**  A string containing the device encryption key filename.
**
** NOTES:
**  The playready libraries and drmparms are compiled using -fshort-wchar. Make 
** sure that your application uses the same compilation flags to avoid unexpected
** behaviors.
**
** SEE ALSO:
**  bdrm_get_zgpriv_enc_key_fname_lgth
**
******************************************************************************/
wchar_t * bdrm_get_zgpriv_enc_key_fname();

/******************************************************************************
** FUNCTION:
**   bdrm_get_zgpriv_enc_key_fname_lgth
**
** DESCRIPTION:
**   This function is used to retrieve the length of the device encryption key 
** filename.
**
** PARAMETERS:
**   N/A
**
** RETURNS:
**   The length of the filename in bytes.
**
** NOTES:
**  The playready libraries and drmparms are compiled using -fshort-wchar. Make 
** sure that your application uses the same compilation flags to avoid unexpected
** behaviors.
**
** SEE ALSO:
**  bdrm_get_zgpriv_enc_key_fname
**
******************************************************************************/
uint32_t  bdrm_get_zgpriv_enc_key_fname_lgth();


/******************************************************************************
** FUNCTION:
**   bdrm_get_ndr_priv_key_fname
**
** DESCRIPTION:
**   This function is used to retrieve the string containing the NDR private key
** filename.
**
** PARAMETERS:
**   N/A
**
** RETURNS:
**  A string containing the NDR private key filename.
**
** NOTES:
**  The playready libraries and drmparms are compiled using -fshort-wchar. Make 
** sure that your application uses the same compilation flags to avoid unexpected
** behaviors.
**
** SEE ALSO:
**  bdrm_get_ndr_priv_key_fname_lgth
**
******************************************************************************/
wchar_t * bdrm_get_ndr_priv_key_fname();

/******************************************************************************
** FUNCTION:
**   bdrm_get_ndr_priv_key_fname_lgth
**
** DESCRIPTION:
**   This function is used to retrieve the length of the NDR private key filename.
**
** PARAMETERS:
**   N/A
**
** RETURNS:
**   The length of the filename in bytes.
**
** NOTES:
**  The playready libraries and drmparms are compiled using -fshort-wchar. Make 
** sure that your application uses the same compilation flags to avoid unexpected
** behaviors.
**
** SEE ALSO:
**  bdrm_get_ndr_priv_key_fname
**
******************************************************************************/
uint32_t  bdrm_get_ndr_priv_key_fname_lgth();

#endif

EXIT_PK_NAMESPACE_CODE;

#ifdef __cplusplus
}
#endif

#endif /*OEM_CONFIG_H*/

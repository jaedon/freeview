/***************************************************************************
 *     Copyright (c) 2007-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: cdl_estb.h $
 * $brcm_Revision: PROD_DSG_REFSW_Devel/7 $
 * $brcm_Date: 7/21/08 10:59a $
 *
 * Module Description:  OpenCable Common Download estb functions.
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/trinity/src/cdl/cdl_estb.h $
 * 
 * PROD_DSG_REFSW_Devel/7   7/21/08 10:59a jackli
 * PR38602: Merge with docsis 2.1.0 alpha1
 * 
 * PROD_DSG_REFSW_Devel/6   6/20/08 3:09p jackli
 * PR38602: set max secure header lenght to 6k
 * 
 * PROD_DSG_REFSW_Devel/5   5/12/08 2:25p jackli
 * PR38602: get secure header len before downloading.
 * 
 * PROD_DSG_REFSW_Devel/4   4/8/08 4:19p jackli
 * PR38602: add dsgcclib support for cdltest
 * 
 * PROD_DSG_REFSW_Devel/3   3/5/08 3:25p jackli
 * PR38602: CDL support, cdl_dsg and multiple CVC
 * 
 * PROD_DSG_REFSW_Devel/2   1/11/08 2:45p jackli
 * PR38602: CDL support, eCM & CVT TFTP download
 * 
 * PROD_DSG_REFSW_Devel/1   1/8/08 4:33p jackli
 * PR38602: add common dowload support
 * 
 * 
 ***************************************************************************/
#ifndef CDL_ESTB_H
#define CDL_ESTB_H

/* 
 * there's no spec about how big the secure header can be,
 * this is the maximum secure header size we can think of.
 */
#define MAX_SECURE_HEADER_SIZE (6 * 1024) 


/* definations based on dsg_api.h */
enum
{
	cdl_estb_kManufCvc		= 0x01,
	cdl_estb_kCosignerCvc	= 0x02
};

int cdl_estb_init();
int cdl_estb_uninit();
int cdl_estb_open() ;
int cdl_estb_close() ;
int cdl_estb_check_cvc(uint8_t *cvc, uint32_t length, uint8_t type);
int cdl_estb_authenticate_code_image( uint8_t * fileName, uint8_t fileNameLen, 
				      uint32_t tftpServerIpAddress);
int cdl_estb_download_tftp_by_cvt( char * fileName, uint32_t fileNameLen, 
				   uint32_t tftpServerIpAddress);
int cdl_estb_validate_secure_header_by_file( char * fileName);
int cdl_estb_validate_secure_header_by_buf( uint8_t * buf, int len);
int cdl_estb_authenticate_cvt(void *cdl_cvt_recv, unsigned int len);
void cdl_estb_set_monolithic_image_temp_file_name (char * fn) ;
char * cdl_estb_get_monolithic_image_temp_file_name (void) ;
void cdl_estb_set_cb_check_integrity(int (*check_integrity) (char * mono_file));

#endif  /* CDL_ESTB_H */

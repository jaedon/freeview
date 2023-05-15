/*
 * (c) 2011-2012 Humax Co., Ltd. 
 * This program is produced by Humax Co., Ltd. ("Humax") and 
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee, 
 * non-assignable, non-transferable and non-exclusive license to use this Software. 
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software 
 * you agree to the responsibility to take all reasonable efforts to protect the any information 
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and 
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately. 
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, 
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE. 
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies 
 * of the Software including all documentation. This License will terminate immediately without notice from Humax 
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies 
 * of the Software and all documentation.  

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice. 
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court, 
 * in the Republic of Korea.
*/

/* This header files is only for supply chain management codes in application layer .
   the irdeto loader layer has own header using directly using Loader core header
   Then, You must not this header applying to loader layer. 						*/


#if !defined(__IR_SUP_HEADER__)
#define __IR_SUP_HEADER__


/* Error returns for I_HLSUP module */
#define I_HLSUP_OK             (0)
#define I_HLSUP_ERR            (-1)
#define I_HLSUP_ERR_NULLPTR    (-2)
#define I_HLSUP_ERR_BBINFO     (-3)
#define I_HLSUP_ERR_READNVRAM  (-4)

/* Valid / invalid definitions */
#define I_HLSUP_VALID          (0)
#define I_HLSUP_INVALID        (-1)

/* key status */
#define I_HLSUP_KS_KDO  (1)
#define I_HLSUP_KS_OK   (0)
#define I_HLSUP_KS_ERR  (-1)
#define I_HLSUP_KS_NEW  (-2)

#define	KEY_SIZE             	(168)
#define	MAGIC_SIZE           	(4)

/* Field index to NVRAM structure */
#define  SYSID_INDEX          	(0)
#define  KEYVER_INDEX         	(SYSID_INDEX + sizeof(IHL_U16))
#define  FLAGS_INDEX          	(KEYVER_INDEX + sizeof(IHL_U16))
#define  KEY_INDEX            	(FLAGS_INDEX + sizeof(IHL_U16))
#define  VARIANT_INDEX        	(KEY_INDEX + KEY_SIZE)
#define  MAGIC1_INDEX         	(VARIANT_INDEX + sizeof(IHL_U16))
#define  SYSIDDUP_INDEX       	(MAGIC1_INDEX + MAGIC_SIZE)
#define  KEYVERDUP_INDEX     	(SYSIDDUP_INDEX + sizeof(IHL_U16))
#define  VARIANTDUP_INDEX     (KEYVERDUP_INDEX + sizeof(IHL_U16))
#define  MAGIC2_INDEX         	(VARIANTDUP_INDEX + sizeof(IHL_U16))
#define  SIGVER0_INDEX        	(MAGIC2_INDEX + MAGIC_SIZE)
#define  IMAGE0_INDEX         	(SIGVER0_INDEX + sizeof(IHL_U16))
#define  SPARE0_INDEX         	(IMAGE0_INDEX + sizeof(IHL_U8))
#define  MAGIC3_INDEX         	(SPARE0_INDEX + sizeof(IHL_U8))
#define  SIGVER1_INDEX        	(MAGIC3_INDEX + MAGIC_SIZE)
#define  IMAGE1_INDEX         	(SIGVER1_INDEX + sizeof(IHL_U16))
#define  SPARE1_INDEX         	(IMAGE1_INDEX + sizeof(IHL_U8))
#define  MAGIC4_INDEX         	(SPARE1_INDEX + sizeof(IHL_U8))
#define  LAST_INDEX           	(MAGIC4_INDEX + MAGIC_SIZE)

/* Actual structure of NVRAM */
/*                                              	offset  block
   IHL_U16   wSysId;                              	0       1
   IHL_U16   wKeyVer;                        		2       1
   IHL_U16   wFlags;                              	4       1
   IHL_U8    abKey[KEY_SIZE];                     	6       1
   IHL_U16   wVariant;                            	174     1
   IHL_U8    abMagic1[MAGIC_SIZE];                	176     1

   IHL_U16   wSysIdDup;                          	180     2
   IHL_U16   wKeyVerDup;                          	182     2
   IHL_U16   wVariantDup;                         	184     2
   IHL_U8    abMagic2[MAGIC_SIZE];                	186     2

   IHL_U16   awSigVer0                            	190     3
   IHL_U8    bImage0                              	192     3
   IHL_U8    bSpare0                              	193     3
   IHL_U8    abMagic3[MAGIC_SIZE];                	194     3

   IHL_U16   awSigVer1                            	198     4
   IHL_U8    bImage1                              	200     4
   IHL_U8    bSpare1                              	201     4
   IHL_U8    abMagic4[MAGIC_SIZE];                	202     4
                                            	Total 206 bytes
*/

/* Block types in Irdeto partition */
typedef enum
{
   NVRAM_KEY_BLOCK  	= 0,
   NVRAM_DUP_BLOCK  	= 1,
   NVRAM_VER0_BLOCK 	= 2,
   NVRAM_VER1_BLOCK 	= 3,
   NVRAM_BLOCK_ALL  	= 4
}  NVRAM_BLOCKS;

/* Start of definition block from FLASH.H This must reflect changes made
   to FLASH.H */
#define RANDOM_NUMBER_LEN      		(4)
#define SERIAL_NUMBER_SIZE     		(4)
#define FLASH_BB_KEY_LEN       		(SERIAL_NUMBER_SIZE + RANDOM_NUMBER_LEN)
/* End of definition block from FLASH.H */

/* Number of blocks maintained in NVRAM partition */
#define NO_BLOCKS      				(4)

/* jfd 11/6/99 Block pass/fail check macro */
#define BlockGood(bnum)    			(!(u8FailedBlocks & (1 << (bnum))))


#endif


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

#include "octo_common.h"
#include "ir_api.h"
#include "ir_loader.h"
#include "flash.h"
#include "ir_nvr.h"
#include "ir_sup.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
#ifdef CONFIG_DEBUG
HUINT32 IR3_SUPDebugLevel = DBG_ASSERT;
#endif

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/





/********************************************************************
 Global/Static Variable Definition
 ********************************************************************/
/* Local copy of Irdeto NVRAM partition */
static IHL_U8 au8IrdetoScram[IR_NVRAM_PARTITION_IRDETO_SIZE];
static IHL_U8 au8IrdetoClear[IR_NVRAM_PARTITION_IRDETO_SIZE];

/* Magic numbers stored in NVRAM partition */
static IHL_U8 au8MagicNumber[MAGIC_SIZE] =
{
   0x5A,
   0x69,
   'r',
   's'
};

//FLASH_BB_VERSION_STRUCTURE stFlashLoaderData;


/********************************************************************
 External Variable Declaration
 ********************************************************************/


/********************************************************************
 Internal Function
 ********************************************************************/
/** Private Function Prototypes *********************************************/
static void		  	I_HLSUP_VariBlockCipher(IHL_U8 pau8OutBlock [],
              				                 const IHL_U8 pau8InBlock [],
                            				 const IHL_U16 u16BlockSize,
                              				 const IHL_U8 pau8Key [],
                              				 const IHL_U8 u8KeySize);
static IHL_U16 		I_HLSUP_GetWord(IHL_U16 u16Index);
static IHL_S8		I_HLSUP_MagicNumbersOK(IHL_PU8 pu8Numbers);
static IHL_S16 		M_HLSUP_ReadNVRAM(IHL_PU8 pu8Buffer, IHL_U16 u16Offset, IHL_U16 u16Length);
static IHL_S16		M_HLSUP_GetBBInfo(IHL_PU8 pu8SerNum, IHL_PU8 pu8RandNum);

/*****************************************************************************
*                                                                            *
*       ROUTINE : M_HLSUP_ReadNVRAM                                          *
*                                                                            *
*   DESCRIPTION : This call will read data from the Irdeto partition stored  *
*                 in the platform NVRAM.                                     *
*                                                                            *
*          NOTE : This call assumes the manufacterer will init and read the  *
*                 NVRAM device.                                              *
*                                                                            *
*         INPUT : u16Offset  - offset into Irdeto partition where the read   *
*                              operation must start.                         *
*                 u16Length  - the number of bytes to read from NVRAM.       *
*                                                                            *
*        OUTPUT : pu8Buffer  - pointer to destination buffer.                *
*                                                                            *
*       RETURNS :  0 = OK                                                    *
*                 -1 = Error reading device                                  *
*                 -2 = Invalid pointer                                       *
*                 -3 = Invalid offset                                        *
*                 -4 = Invalid length                                        *
*                                                                            *
*        Author : Jon Darrington                                             *
*                                                                            *
*  Last Changed : 11 Jun 1999                                                *
*                                                                            *
*****************************************************************************/
static IHL_S16 M_HLSUP_ReadNVRAM (IHL_PU8 pu8Buffer, IHL_U16 u16Offset,
                           IHL_U16 u16Length)
{
	IHL_S16  ret;
	HxLOG_Info("pu8Buffer=0x%X,u16Offset=0x%X,u16Length=0x%X\n",pu8Buffer, u16Offset,u16Length );

	ret =  (IHL_S16)CAS_IR_NVR_Partition_Read(IRNVR_PART_IRDETO, u16Offset, pu8Buffer, u16Length );

	#if 0 // for debugging dump
	{
		int i;
		for(i=0; i< u16Length; i++ )
		{
			printf("%02X ", pu8Buffer[i] );
			if(i %16==15)
				printf("\n");
		}
		printf("\n");

	}
	#endif

	return ret;
}

/*****************************************************************************
*                                                                            *
*       ROUTINE : M_HLSUP_GetBBInfo                                          *
*                                                                            *
*   DESCRIPTION : This call will obtain the random number and serial number  *
*                 fields from the flash boot block control block.            *
*                                                                            *
*         INPUT : None.                                                      *
*                                                                            *
*        OUTPUT : pu8SerNum - 4 bytes of serial number                       *
*                 pu8RandNum - 4 bytes of random number                      *
*                                                                            *
*       RETURNS :  0 = OK                                                    *
*                 -1 = Error                                                 *
*                                                                            *
*        Author : Jon Darrington                                             *
*                                                                            *
*  Last Changed : 11 Jun 1999                                                *
*                                                                            *
*****************************************************************************/
static IHL_S16 M_HLSUP_GetBBInfo (IHL_PU8 pu8SerNum, IHL_PU8 pu8RandNum)
{
	HUINT32 ulSerRanNum;

	HxLOG_Info("\n");

	xsvc_cas_IrLoaderGetSerialNum(&ulSerRanNum);
	memcpy(pu8SerNum, (unsigned char *)&ulSerRanNum, SERIAL_NUMBER_SIZE);

	xsvc_cas_IrLoaderGetRandomNum(&ulSerRanNum);
	memcpy(pu8RandNum, (unsigned char *)&ulSerRanNum, RANDOM_NUMBER_LEN);

	return 0;
}

/** Module private functions ************************************************/
/*****************************************************************************
*                                                                            *
*     Routine : I_HLSUP_VariBlockCipher                                     *
*                                                                            *
* Description : VariBlockCipher - a single routine for encrypting and        *
*               decrypting any even block size under any size of key         *
*               securely, efficiently and simply. (A small modification would*
*               be required to allow for odd block sizes.)                   *
*                                                                            *
*               The decryption operation is identical, except the byte order *
*               in the provided key must be reversed.  The same routine can  *
*               be used for both operations only if bBlockSize is even.      *
*               The cipher is self-inverse for symmetric keys.               *
*                                                                            *
*               The operation may be done in place (both pointers equal), or *
*               may be done from one location to another non-overlapping     *
*               location.                                                    *
*                                                                            *
*               Although not shown to be necessary, it is preferred that the *
*               GCD of keysize and blocksize be 1.                           *
*                                                                            *
*               The blocksize and keysize may be made larger than bytes,     *
*               provided the internal variable set to the product of these is*
*               made long enough to accommodate this product.                *
*                                                                            *
*       Input : pau8InBlock     : A pointer to the input block, ie the data  *
*                                 block to be encrypted or decrypted.        *
*               u8BlockSize     : The block size of the input block; must be *
*                                 an even number.                            *
*               pau8Key         : A pointer to the key that should be used   *
*                                 for encryption/decryption.                 *
*               u8KeySize       : The key size in bytes.                     *
*                                                                            *
*      Output : pau8OutBlock    : A pointer to the encrypted or decrypted    *
*                                 block.                                     *
*                                                                            *
*     Returns : None.                                                        *
*                                                                            *
*      Author : Jon Darrington                                               *
*                                                                            *
* Last Changed: 11 Jun 1999                                                  *
*                                                                            *
*****************************************************************************/
void I_HLSUP_VariBlockCipher (IHL_U8 pau8OutBlock [],
                              const IHL_U8 pau8InBlock [],
                              const IHL_U16 u16BlockSize,
                              const IHL_U8 pau8Key [],
                              const IHL_U8 u8KeySize)
{

   static const IHL_U8 au8Sbox [256] =
   {
      0x10, 0xDA, 0xA9, 0xA8, 0xC6, 0x6A, 0x7A, 0x4D,
      0x52, 0xF2, 0x02, 0xB2, 0xAA, 0x55, 0xB7, 0x45,
      0xA6, 0x9C, 0xFE, 0x90, 0xE8, 0xF1, 0x9E, 0x6F,
      0x98, 0x78, 0x7B, 0x88, 0xFC, 0xA3, 0x58, 0x9A,
      0x1A, 0xB5, 0xED, 0x97, 0x9F, 0x3E, 0xB4, 0x95,
      0x5E, 0xE0, 0x0C, 0x15, 0xBB, 0x1E, 0xD4, 0x13,
      0x17, 0xA4, 0x0A, 0x23, 0xA0, 0x28, 0x38, 0xA5,
      0x79, 0x65, 0xFD, 0xE3, 0x11, 0xEE, 0x9B, 0x87,
      0x18, 0x44, 0x60, 0x34, 0xD3, 0xC2, 0x92, 0xBD,
      0xE6, 0x83, 0xF6, 0x53, 0x7D, 0x99, 0x66, 0x6C,
      0x5C, 0x89, 0x74, 0x6E, 0x40, 0x68, 0xBC, 0xAB,
      0x57, 0x2D, 0xE2, 0x09, 0x85, 0xD8, 0xAC, 0xBA,
      0x77, 0xD9, 0xF7, 0xB8, 0x71, 0x47, 0xC7, 0x2B,
      0xE5, 0x31, 0x82, 0xCC, 0x64, 0x9D, 0x76, 0x8E,
      0x8A, 0xDC, 0x16, 0x7C, 0xFB, 0x33, 0x54, 0xA7,
      0xBF, 0x37, 0x63, 0x0B, 0x35, 0x6D, 0x5B, 0x32,
      0xCF, 0x51, 0xEF, 0xD5, 0x0F, 0x46, 0xB1, 0xC9,
      0x43, 0x00, 0x1C, 0x7E, 0x70, 0x8F, 0x29, 0x3F,
      0x0E, 0x4A, 0x5F, 0x41, 0xAF, 0xF8, 0xCB, 0xCD,
      0xE9, 0x75, 0x1B, 0xDD, 0x8B, 0xE7, 0x27, 0x59,
      0x73, 0x0D, 0xB9, 0x8C, 0x62, 0x20, 0xD6, 0xAD,
      0x7F, 0xA2, 0x1F, 0x49, 0x93, 0xCA, 0x2E, 0x07,
      0xDE, 0xDB, 0x94, 0x69, 0xEA, 0x14, 0x3C, 0x4F,
      0xD0, 0xC1, 0xEC, 0x8D, 0x39, 0xB6, 0xEB, 0xE4,
      0xBE, 0x30, 0xFF, 0x21, 0x42, 0x3A, 0xE1, 0x25,
      0x06, 0xF4, 0x03, 0xC4, 0x3D, 0x6B, 0xDF, 0x1D,
      0x2C, 0x81, 0x2F, 0x4B, 0xD2, 0x84, 0x4C, 0x56,
      0x86, 0x22, 0x01, 0xF3, 0x72, 0x24, 0x08, 0x05,
      0x67, 0x12, 0xFA, 0xD1, 0x80, 0xB3, 0x04, 0xC3,
      0xF5, 0x5A, 0x26, 0xC8, 0x36, 0xF0, 0x19, 0x48,
      0x4E, 0xC5, 0x91, 0x5D, 0x2A, 0xAE, 0x96, 0x50,
      0xF9, 0xB0, 0xA1, 0xD7, 0x61, 0xC0, 0x3B, 0xCE,
   };

   IHL_U16 u16Step;
   IHL_U8 u8Tmp;
   IHL_U16 u16I, u16J, u16T0, u16T1, u16T2;

   HxLOG_Info("\n");

   /* reverse and copy data into the destination, which may be the source */
   for (u16I=(u16BlockSize+1)/2, u16J=u16BlockSize-u16I-1; u16I < u16BlockSize;
         u16I++, u16J--)
   {
      u8Tmp = pau8InBlock[u16I];
      pau8OutBlock[u16I] = pau8InBlock[u16J];
      pau8OutBlock[u16J] = u8Tmp;
   }

   /* perform the ciphering loop */
   u16I = 0;
   u16J = 0;
   u16T0 = u16BlockSize - 1;
   u16T1 = 1;
   u16T2 = (u16BlockSize+1)/2;
   for (u16Step = u16BlockSize * u8KeySize; u16Step != 0; u16Step--)
   {
      u8Tmp = au8Sbox[pau8OutBlock[u16I] ^ pau8Key[u16J]];
      pau8OutBlock[u16T0] ^= u8Tmp;
      pau8OutBlock[u16T1] ^= u8Tmp;
      pau8OutBlock[u16T2] ^= u8Tmp;
      u16T0 = u16I;
      u16I = u16T1;
      u16T1 += 1;
      if (u16T1 == u16BlockSize)
      {
         u16T1 = 0;
      }
      u16T2 += 1;
      if (u16T2 == u16BlockSize)
      {
         u16T2 = 0;
      }
      u16J += 1;
      if (u16J == u8KeySize)
      {
         u16J = 0;
      }
   }

   /* erase internal data for cleanliness */
   u8Tmp = 0x00;
} /* I_HLSUP_VariBlockCipher */

/*****************************************************************************
*                                                                            *
*       ROUTINE : I_HLSUP_GetWord                                            *
*                                                                            *
*   DESCRIPTION : This function returns the word value at the specified      *
*                 address in the Irdeto NVRAM partition.                     *
*                 NOTE : The word will be assembled in big-endian format.    *
*                                                                            *
*         INPUT : u16Index - index into partition.                           *
*                                                                            *
*        OUTPUT : None.                                                      *
*                                                                            *
*       RETURNS : Word value.                                                *
*                                                                            *
*        Author : Jon Darrington                                             *
*                                                                            *
*  Last Changed : 11 Jun 1999                                                *
*                                                                            *
*****************************************************************************/
static IHL_U16 I_HLSUP_GetWord (IHL_U16 u16Index)
{
   HxLOG_Info("u16Index=0x%X\n", u16Index);
   if (u16Index < IR_NVRAM_PARTITION_IRDETO_SIZE - 1)
   {
      return ((au8IrdetoClear[u16Index] << 8) + au8IrdetoClear[u16Index + 1]);
   }
   else
   {
      return (0);
   }
}  /* I_IHLSUP_GetWord */

/*****************************************************************************
*                                                                            *
*       ROUTINE : I_HLSUP_MagicNumbersOK                                     *
*                                                                            *
*   DESCRIPTION : This function tests for valid magic numbers in the         *
*                 specified memory block.                                    *
*                                                                            *
*         INPUT : pu8Numbers - pointer to magic numbers.                     *
*                                                                            *
*        OUTPUT : None.                                                      *
*                                                                            *
*       RETURNS : 0 if magic numbers are correct, else -1.                   *
*                                                                            *
*        Author : Jon Darrington                                             *
*                                                                            *
*  Last Changed : 11 Jun 1999                                                *
*                                                                            *
*****************************************************************************/
static IHL_S8 I_HLSUP_MagicNumbersOK (IHL_PU8 pu8Numbers)
{
   int i;

	HxLOG_Info("\n");

   for (i = 0; i < MAGIC_SIZE; i++)
   {
      if (pu8Numbers[i] != au8MagicNumber[i])
      {
		  HxLOG_Print("Magic Number NOT_OK!\n");

         return (-1);
      }
   }
   HxLOG_Print("Magic Number OK!\n");
   return (0);

} /* I_HLSUP_MagicNumbersOK */

/********************************************************************
 Global Function
 ********************************************************************/
/*****************************************************************************
*                                                                            *
*       ROUTINE : I_HLSUP_GetStatusInfo                                      *
*                                                                            *
*   DESCRIPTION : This function obtains key and variant status information   *
*                 for the STB.                                               *
*                                                                            *
*         INPUT : None.                                                      *
*                                                                            *
*        OUTPUT : pstStatus - Structure filled with status information.      *
*                                                                            *
*       RETURNS : I_HLSUP_OK = status information structure is valid.        *
*                 I_HLSUP_ERR = status information could not be obtained.    *
*                                                                            *
*        Author : Jon Darrington                                             *
*                                                                            *
*  Last Changed : 11 Jun 1999                                                *
*                                                                            *
*****************************************************************************/
IHL_S16 I_HLSUP_GetStatusInfo (IHL_PSTATUS pstStatus)
{
   IHL_U8 au8RevKey[FLASH_BB_KEY_LEN];
   IHL_U8 au8Key[FLASH_BB_KEY_LEN];
   int    i;
   IHL_U8 u8FailMask = 0x01;
   /* Failed NVRAM blocks based on Magic Numbers -
      use bit pattern to indicate failure of multiple blocks */
   IHL_U8 u8FailedBlocks;

   /* Block start indices into NVRAM partition */
   IHL_U16 au16BlockIndex[NO_BLOCKS] =
   {
      (IHL_U16)(SYSID_INDEX),
      (IHL_U16)(SYSIDDUP_INDEX),
      (IHL_U16)(SIGVER0_INDEX),
      (IHL_U16)(SIGVER1_INDEX)
   };

   /* Sizes (in bytes) of blocks in NVRAM partition */
   IHL_U16 au16BlockSize[NO_BLOCKS] =
   {
      (IHL_U16)(SYSIDDUP_INDEX - SYSID_INDEX),
      (IHL_U16)(SIGVER0_INDEX - SYSIDDUP_INDEX),
      (IHL_U16)(SIGVER1_INDEX - SIGVER0_INDEX),
      (IHL_U16)(LAST_INDEX - SIGVER1_INDEX)
   };

   /* Magic number indices into NVRAM partition */
   IHL_U16  au16MagicIndex[NO_BLOCKS] =
   {
      (IHL_U16)(MAGIC1_INDEX),
      (IHL_U16)(MAGIC2_INDEX),
      (IHL_U16)(MAGIC3_INDEX),
      (IHL_U16)(MAGIC4_INDEX)
   };

   u8FailedBlocks = 0;

	HxLOG_Info("");

   /* jfd 11/6/99 Check for NULL ptr passed */
   if (pstStatus == NULL)
   {
      return (I_HLSUP_ERR_NULLPTR);
   }

   /* jfd 11/6/99 Initialise output structure */
   pstStatus->s16SysIDValid  = I_HLSUP_INVALID;
   pstStatus->s16KeyVerValid = I_HLSUP_INVALID;
   pstStatus->s16SigVerValid = I_HLSUP_INVALID;
   pstStatus->u8BlockFailMask = 0;
   for (i = 0; i < NO_BLOCKS; i++)
   {
      pstStatus->u8BlockFailMask &= 1 << i;
   }

   /* Read Irdeto NVRAM partition using manufacturer's routine */
   if (M_HLSUP_ReadNVRAM (au8IrdetoScram, 0, IR_NVRAM_PARTITION_IRDETO_SIZE) == 0)
   {
      /* jfd 11/6/99 Obtain boot block control block information */
      if (M_HLSUP_GetBBInfo (au8Key, &au8Key[SERIAL_NUMBER_SIZE]) == 0)
      {

         /* Reverse the key byte order */
         for (i = 0; i < FLASH_BB_KEY_LEN; i++)
         {
            au8RevKey[i] = au8Key[FLASH_BB_KEY_LEN - 1 - i];
         }

         /* Decrypt and check magic numbers of all blocks in NVRAM partition */
         u8FailMask = 0x01;
         for (i = 0; i < NO_BLOCKS; i++)
         {
            I_HLSUP_VariBlockCipher (&au8IrdetoClear[au16BlockIndex[i]],
                                     &au8IrdetoScram[au16BlockIndex[i]],
                                     au16BlockSize[i],
                                     &au8RevKey[0],
                                     FLASH_BB_KEY_LEN);

            /* Check for magic numbers in NVRAM partition */
            if (I_HLSUP_MagicNumbersOK(&au8IrdetoClear[au16MagicIndex[i]])
                  == -1)
            {
               u8FailedBlocks |= u8FailMask;
            }
            /* rls - 15/05/98 : bFailMask should be shifted for each loop
               increment */
            u8FailMask <<= 1;
         }

         /* Copy failed block mask into output structure */
         pstStatus->u8BlockFailMask = u8FailedBlocks;

         /* jfd 11/6/99 Decide which elements are valid and update output
            structure accordingly */
         /* jfd 11/6/99 Handle sys_id, key ver and variant together */
         if (BlockGood (NVRAM_KEY_BLOCK))
         {
            if (BlockGood (NVRAM_DUP_BLOCK))
            {
               /* jfd 11/6/99 Both blocks good therefore check individual
                  items */
               if (I_HLSUP_GetWord (SYSID_INDEX)
                     == I_HLSUP_GetWord (SYSIDDUP_INDEX))
               {
                  pstStatus->s16SysIDValid = I_HLSUP_VALID;
                  pstStatus->u16SysID = I_HLSUP_GetWord (SYSID_INDEX);
               }

               if (I_HLSUP_GetWord (KEYVER_INDEX)
                     == I_HLSUP_GetWord (KEYVERDUP_INDEX))
               {
                  pstStatus->s16KeyVerValid = I_HLSUP_VALID;
                  pstStatus->u16KeyVer = I_HLSUP_GetWord (KEYVER_INDEX);
               }

               if (I_HLSUP_GetWord (VARIANT_INDEX)
                     == I_HLSUP_GetWord (VARIANTDUP_INDEX))
               {
                  pstStatus->s16VariantValid = I_HLSUP_VALID;
                  pstStatus->u16Variant = I_HLSUP_GetWord (VARIANT_INDEX);
               }
            }
            else /* BlockGood (NVRAM_DUP_BLOCK) */
            {
               /* Only key block valid, therefore use values from there */
               pstStatus->s16SysIDValid = I_HLSUP_VALID;
               pstStatus->u16SysID = I_HLSUP_GetWord (SYSID_INDEX);

               pstStatus->s16KeyVerValid = I_HLSUP_VALID;
               pstStatus->u16KeyVer = I_HLSUP_GetWord (KEYVER_INDEX);

               pstStatus->s16VariantValid = I_HLSUP_VALID;
               pstStatus->u16Variant = I_HLSUP_GetWord (VARIANT_INDEX);
            }
         }
         else /* BlockGood (NVRAM_KEY_BLOCK) */
         {
            if (BlockGood (NVRAM_DUP_BLOCK))
            {
               /* Only dup block valid, therefore use values from there */
               pstStatus->s16SysIDValid = I_HLSUP_VALID;
               pstStatus->u16SysID = I_HLSUP_GetWord (SYSIDDUP_INDEX);

               pstStatus->s16KeyVerValid = I_HLSUP_VALID;
               pstStatus->u16KeyVer = I_HLSUP_GetWord (KEYVERDUP_INDEX);

               pstStatus->s16VariantValid = I_HLSUP_VALID;
               pstStatus->u16Variant = I_HLSUP_GetWord (VARIANTDUP_INDEX);
            }
         }

         /* jfd 11/6/99 Now handle signature version */
         if (BlockGood (NVRAM_VER0_BLOCK))
         {
            if (BlockGood (NVRAM_VER1_BLOCK))
            {
               /* jfd 11/6/99 Both blocks good therefore check values */
               if (I_HLSUP_GetWord (SIGVER0_INDEX)
                     == I_HLSUP_GetWord (SIGVER1_INDEX))
               {
                  pstStatus->s16SigVerValid = I_HLSUP_VALID;
                  pstStatus->u16SigVer = I_HLSUP_GetWord (SIGVER0_INDEX);
               }
            }
            else /* BlockGood (NVRAM_VER1_BLOCK) */
            {
               /* jfd 11/6/99 Only ver0 good, therefore use value there */
               pstStatus->s16SigVerValid = I_HLSUP_VALID;
               pstStatus->u16SigVer = I_HLSUP_GetWord (SIGVER0_INDEX);
            }
         }
         else /* BlockGood (NVRAM_VER1_BLOCK) */
         {
            if (BlockGood (NVRAM_VER1_BLOCK))
            {
               /* jfd 11/6/99 Only ver1 good, therefore use value there */
               pstStatus->s16SigVerValid = I_HLSUP_VALID;
               pstStatus->u16SigVer = I_HLSUP_GetWord (SIGVER1_INDEX);
            }
         }
         /* jfd 11/6/99 Trash clear image */
         for (i = 0; i < IR_NVRAM_PARTITION_IRDETO_SIZE; i++)
         {
            au8IrdetoClear[i] = 0;
         }

         return (I_HLSUP_OK);
      }
      else /* Failed to obtain boot block control block info */
      {
         /* jfd 11/6/99 Trash clear image */
         for (i = 0; i < IR_NVRAM_PARTITION_IRDETO_SIZE; i++)
         {
            au8IrdetoClear[i] = 0;
         }

         return (I_HLSUP_ERR_BBINFO);
      }
   }
   else /* NVRAM read failed */
   {
      return(I_HLSUP_ERR_READNVRAM);
   }
} /* I_HLSUP_GetStatusInfo */



#include <string.h>
#include <stdio.h>

#include "fdi_cas_vmxbc_api.h" // bc_consts.h + bc_types.h + bc_main.h 을 하나로 통합.

#include "vkernel.h"
#include "fvk.h"

int16_t  FS_SetECMFilter(uint8_t bFilterId, enFilterMode_t enFilterMode, uint16_t wEcmPid, uint8_t bTableId, uint8_t bVersion, uint8_t bPage)
{
	return k_BcSuccess;
}

int16_t  FS_ReadECM(uint8_t bFilterId, uint8_t* pabBuffer, uint16_t* pwLen)
{
	return k_BcSuccess;
}

int16_t  FS_FlushECM_Buffer(uint8_t bFilterId)
{
	return k_BcSuccess;
}

int16_t  FS_SetEMMFilter(uint8_t bFilterIndex, uint8_t bAddressLength, uint8_t* pabAddress)
{
    	return k_BcSuccess;
}

int16_t  FS_SetEMM_Pid(uint16_t wEmmPid)
{
    	return k_BcSuccess;
}

int16_t  FS_ReadEMM(uint8_t* pabBuffer, uint16_t* pwLen)
{
    	return k_BcSuccess;
}

int16_t  FS_FlushEMM_Buffer(void_t)
{
    	return k_BcSuccess;
}

int16_t  FS_StartDescrambling(uint16_t wIndex, uint16_t *pawStreamPid, uint8_t bServiceIdx)
{
   	return k_BcSuccess;
}

int16_t  FS_StopDescrambling(uint8_t bServiceIdx)
{
    	return k_BcSuccess;
}

int16_t  MMI_SetDescrambling_State(uint16_t wIndex, uint16_t* pawStreamPid, enDescState_t * paenDescState, uint8_t bServiceIdx)
{
    	return k_BcSuccess;
}

int16_t  MMI_SetSmartcard_State(enScState_t enScState)
{
    	return k_BcSuccess;
}

signed long  SYS_GetTickCount(void_t)
{
	return 0;
}

int16_t  SYS_ReadNvmBlock(uint8_t* pabDest, uint16_t wLength)
{
   	return k_BcSuccess;
}

int16_t  SYS_WriteNvmBlock(uint8_t* pabSrc, uint16_t wLength)
{
	return k_BcSuccess;
}

#ifdef BC_NSC_INCLUDED
/**
 *  @brief  Reads Data from non volatile memory
 *  @param[in]  bFileId
 *                identifier for the two different memory locations (only 0 and 1 values are used for the multiple times read and write area,
 *                and 2 is used for the one time area of 16 kBytes)
 *  @param[in]  pabDest
 *                pointer to destination
 *  @param[in]  wLength
 *                number of bytes to read
 *  @return  k_BcSuccess, k_BcError
 *  @remarks
 *    The host system must grant access to non-volatile memory (e.g. EEPROM or Flash) of size not smaller than 2048 byte, for advanced security 4096 bytes,
 *    for bFileId being 0 or 1.
 *    This memory block differs from the memory block used for SYS_ReadNvmBlock/ SYS_WriteNvmBlock.
 *    This flash area is written every time a change in the smartcard free system configuration happens.
 * 
 *	
 *	<advanced secrutiy인경우 아래의 내용으로 진행햐야함 >
 * 	Changed Functions
 *		This function has a changed semantics. For bFileId = 0 and
 *		bFileId = 1, the size of the memory array is not 2048 bytes,
 *		but 4096 bytes. If DVR is activated this size increases to 8192
 *		bytes for each block.
 *		Normally this function is only needed if smartcard free is
 *		activated. In case of advanced security, this function is also
 *		needed for smartcard only operation.
 */
int16_t  SYS_ReadNvmData(uint8_t bBlockId, uint8_t* pabDest, uint16_t wLength)
{
	if (bBlockId == 0)
	{

	}
	else if (bBlockId == 1)
	{

	}
	else if (bBlockId == 2)
	{
		if(FDI_SYSTEM_VMX_ReadNvmData(bBlockId, pabDest, wLength) == 1)
		{
			return k_BcError;
		}
	}
	else
	{
		// TODO
		return k_BcError;
	}

	return k_BcSuccess;
}
#endif

#ifdef BC_NSC_INCLUDED
/**
 *  @brief  Writes Data to non volatile memory
 *  @param[in]  bFileId
 *                identifier for the two different memory locations (only 0 and 1 values are used for the multiple times read and write area,
 *                and 2 is used for the one time area of 16 kBytes)
 *  @param[in]  pabSrc
 *                pointer to data which should be written
 *  @param[in]  wLength
 *                number of bytes to write
 *  @return  k_BcSuccess, k_BcError
 *  @remarks
 *    The host system must grant access to non-volatile memory (e.g. EEPROM or Flash) of size not smaller than 2048 byte, for advanced security 4096 bytes.
 *    The frequency of write accesses is very low and is therefore not critical at all with respect to guaranteed write cycles of flash memory.
 *    Two memory blocks in different EEPROM/Flash segments must be provided to ensure, that a write failure due to power off guarantees the previously written memory block is valid.
 *    If the data in these segments get lost,the set top can no longer be used with the smartcard free system.
 *    The set top box cannot be reindividualized to the same id in case of such a loss.
 *	
 *	
 *	<advanced secrutiy인경우 아래의 내용으로 진행햐야함 >
 * 	Changed Functions
 *		This function has a changed semantics. For bFileId = 0 and
 *		bFileId = 1, the size of the memory array is not 2048 bytes,
 *		but 4096 bytes. If DVR is activated this size increases to 8192
 *		bytes for each block.
 *		Normally this function is only needed if smartcard free is
 *		activated. In case of advanced security, this function is also
 *		needed for smartcard only operation.
 */
int16_t  SYS_WriteNvmData(uint8_t bBlockId, uint8_t* pabSrc, uint16_t wLength)
{
	if (bBlockId == 0)
	{

	}
	else if (bBlockId == 1)
	{

	}
	else if (bBlockId == 2)
	{
		if(FDI_SYSTEM_VMX_WriteNvmData(bBlockId, pabSrc, wLength) == 1)
		{
			return k_BcError;
		}
	}
	else
	{
		// TODO
		return k_BcError;
	}

    	return k_BcSuccess;
}
#endif

signed long  SYS_Random(void_t)
{
	return 0;
}

int16_t  SYS_SetDialogue(uint16_t wDialogLength, uint8_t* pabDialogue)
{
    	return k_BcSuccess;
}

/**
 *  @brief  returns the individual box id
 *  @param[out]  pabBoxId
 *                 this 8-byte-array is used to return the individual box id
 *  @return  none
 *  @remarks
 *    In case of an advanced security implementation, where the chip-set-id is available in the chip-set OTP-area,
 *    this call is used to inform the set top box application about the chip-set id, a 64bit-value.
 *    In this case the value must be taken by the set top box as the individual box id.
 * 
 *    Changed Functions
 *  		 This function has a changed semantics. While in the normal,
 *  		non advanced security mode, this function hands over the box
 *  		id from the set top box application to the library, the direction
 *  		is changed for advanced security. For advanced security the
 *  		betacrypt library reports the box-id, which is read from the chip
 *  		set, to the set top box application.
 */
void_t SYS_GetBoxId( uint8_t* pabBoxId )
{
	FDI_SYSTEM_VMX_SendBoxId(pabBoxId);
}

int16_t  SC_Write(uint8_t* pabBuffer, uint16_t* pwLen, uint16_t wTimeout)
{
    	return k_BcSuccess;
}

int16_t  SC_Read(uint8_t* pabBuffer, uint16_t* pwLen)
{
    	return k_BcSuccess;
}

int16_t  SC_IOCTL(enCmd_t cmd, void_t* pvParams, uint16_t* pwLen)
{
	return k_BcSuccess;
}

void_t   OSD_BuildWindow( uint8_t* pabMsg, int16_t wMode, int16_t wX, int16_t wY, int16_t wW, int16_t wH, int8_t bBackground, int8_t bAlpha, int8_t bForeground )
{
}

uint16_t  OSD_DisplayWindow( uint8_t bDisplayMode, uint16_t wDuration )
{
    	return 1;
}

void_t   ISC_CheckPin( uint8_t bPinIndex, uint8_t bTextSelector, uint8_t bServiceIdx )
{
}

void_t   ISC_OrderPin( uint8_t bPinIndex, uint32_t lPurse,	uint32_t lCost, uint16_t wMult, uint16_t wDiv, uint8_t bLocation, uint8_t bSign0, uint8_t bSign1000, uint8_t bCount, uint8_t bLen, uint8_t *abText, uint32_t lEventId, uint8_t bServiceIdx )
{
}

void_t *LIBC_memcpy( void_t *s1, const void_t *s2, uint32_t n )
{ 
	return VK_memcpy(s1, s2, n);
}

int16_t LIBC_memcmp( const void_t *s1, const void_t *s2, uint32_t n )
{ 
	return VK_memcmp(s1, s2, n);
}

void_t *LIBC_memmove( void_t *s1, const void_t *s2, uint32_t n )
{ 
	return memmove(s1, s2, n);
}

void_t *LIBC_memset( void_t *s, signed long i, uint32_t n )
{ 
	return VK_memset(s, i, n);
}

int8_t *LIBC_strcpy( int8_t *s1, const int8_t *s2)
{ 
	return VK_strcpy(s1, s2);
}

void_t *LIBC_malloc( signed long size)
{ 
	return VK_MEM_Alloc(size);
}

void_t LIBC_free( void_t * ptr)
{
    if (ptr == NULL)
    {
        return;
    }

    VK_MEM_Free(ptr);
}

int16_t IO_Printf( const char *format, /* args*/ ... )
{
	return 0;
}

int16_t IO_GetChar( void )
{
	return 0;
} 

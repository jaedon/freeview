#ifndef BHSM_7550_OTP_H__
#define BHSM_7550_OTP_H__

#include "bhsm_otpmsp.h"
#include "chip_secure_7550.h"

#define BPI_OTP_MSP_BITS_PER_PHY_ROW 0x39
#define BPI_OTP_MSP_BIT_OFFSET 0x6
#define BPI_LOTP_SECTION_MSP_LOGICAL_BASE_ADDRESS 0xC00
#define BPI_OTP_MSP_LOCK_BIT_OFFSET 0x3

void read_OTP_ID(BHSM_Handle in_handle, unsigned long * otpID);

void read_OTP_MAC_ID(BHSM_Handle in_handle, unsigned long * otpMacID);

unsigned long BPI_Otp_ProgramMspEnum(BHSM_Handle in_handle, unsigned long in_usMspEnum, unsigned long in_ucNumberBitsToProg, unsigned long in_ulDataValueToProg);

unsigned long BPI_Otp_ExecuteCommand(
	BHSM_Handle 	in_handle,
	unsigned char in_ucOtpCmd,
	unsigned short in_usRowAddr,
	unsigned long in_ucBitAddr );

unsigned long BPI_Otp_ProgPatternSequence(BHSM_Handle in_handle);

unsigned long BPI_Otp_CheckProgPatternSequenceStatus(BHSM_Handle in_handle);

unsigned long  BPI_Otp_ProgDataBit(
	BHSM_Handle in_handle,
    unsigned char   in_ucBitAddr,     /* [in] 1 to 54  */
    unsigned short  in_usRowAddr,     /* [in] 0 to 255 for 130nm and 0 to 319 for 65nm */
    unsigned char   in_bDataBitValue  /* [in] 0 or 1 */
    );
unsigned long BPI_Otp_DataInit(void);

unsigned long BPI_Otp_ProgramMspBit(
	BHSM_Handle in_handle,
    unsigned char   in_ucBitAddr,   	/* [in] 1 to 54 */
    unsigned short  in_usRowAddr,   	/* [in] */
	unsigned char   in_ucLockBitAddr,  	/* [in] */
    unsigned short  in_usLockRowAddr,  	/* [in] */
    unsigned long in_bBitValue    	/* [in] 0 or 1 */
    );

void read_MSP(BHSM_Handle in_handle, unsigned long * ulMSP, unsigned long mspEnum, unsigned long numMSPBits);

void read_HASH(BHSM_Handle in_handle, unsigned long mspEnum, unsigned long * ulArrayHash);

#endif /* BHSM_OTPMSP_H__ */


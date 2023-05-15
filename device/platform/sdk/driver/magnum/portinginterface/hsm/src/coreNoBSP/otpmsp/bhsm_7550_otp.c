#include "bhsm_7550_otp.h"

unsigned long check_OTP_bit_status (BHSM_Handle in_handle, unsigned long in_usMspEnum)
{
	unsigned long regVal = 0;
	unsigned long bitVal = in_usMspEnum % 32;
	regVal = BREG_Read32(in_handle->regHandle,XPT_OTP_MC1_BASE_LOCK + 4*(in_usMspEnum/32));

	if (regVal & (1 << bitVal))
		return 0;
	else
		return 1;
}

void read_OTP_ID(BHSM_Handle in_handle, unsigned long * otpID)
{
	unsigned long temp;
	temp = BREG_Read32(in_handle->regHandle, XPT_OTP_ID_WORD_LO);
	otpID[0] = (((temp >> 24) & 0xff) | ((temp >> 8) & 0xff00) | ((temp << 8) & 0xff0000) | ((temp << 24) & 0xff000000));
	temp = BREG_Read32(in_handle->regHandle, XPT_OTP_ID_WORD_HI);
	otpID[1] = (((temp >> 24) & 0xff) | ((temp >> 8) & 0xff00) | ((temp << 8) & 0xff0000) | ((temp << 24) & 0xff000000));
}

void read_OTP_MAC_ID(BHSM_Handle in_handle, unsigned long * otpMacID)
{
	otpMacID[0] = BREG_Read32(in_handle->regHandle, XPT_OTP_MAC_WORD_LO);
	otpMacID[1] = BREG_Read32(in_handle->regHandle, XPT_OTP_MAC_WORD_HI);
}

void read_MSP(BHSM_Handle in_handle, unsigned long * ulMSP, unsigned long mspEnum, unsigned long numMSPBits)
{
	unsigned long regVal;

	if (numMSPBits > 32)
		return;

	if ((32 - mspEnum % 32) <= numMSPBits) /* two MSP XPT registers are required to be read to get all bits */
	{
		regVal = BREG_Read32(in_handle->regHandle,XPT_OTP_MC1_BASE + 4*(mspEnum/32));
		*ulMSP = regVal >> (mspEnum % 32);

		regVal = BREG_Read32(in_handle->regHandle,XPT_OTP_MC1_BASE + 4*((mspEnum+32)/32));
		regVal &= (1 << (numMSPBits - (32 - mspEnum % 32))) - 1;
		*ulMSP |= regVal << (32 - (mspEnum % 32));
	} else
	{
		regVal = BREG_Read32(in_handle->regHandle,XPT_OTP_MC1_BASE + 4*((mspEnum)/32));
		regVal = regVal >> mspEnum % 32;
		regVal &= (1 << numMSPBits) - 1;
		*ulMSP = regVal;
	}
}

void read_HASH(BHSM_Handle in_handle, unsigned long mspEnum, unsigned long * ulArrayHash)
{
	unsigned long regVal, temp, i;

	regVal = BREG_Read32(in_handle->regHandle, XPT_OTP_MC1_BASE + 4*(mspEnum/32));
	regVal = regVal >> 24;
	ulArrayHash [0] = regVal;

	for (i = 1; i < 5; i++)
	{
		regVal = BREG_Read32(in_handle->regHandle, XPT_OTP_MC1_BASE + 4*((mspEnum+i*32)/32));
		temp = regVal << 8;
		regVal = regVal >> 24;
		ulArrayHash [i-1] |= temp;
		ulArrayHash [i] = regVal;
	}

	regVal = BREG_Read32(in_handle->regHandle, XPT_OTP_MC1_BASE + 4*((mspEnum+5*32)/32));
	temp = regVal << 8;
	ulArrayHash [4] |= temp;
}

/*
 ***********************************************************************************
 * BPI_Otp_ProgPatternSequence -
Description:
    This function writes the 8 32-bit patterns into the BSP_OTP_PROG_PATTERN register
    and checks the PATTERN_UNLOCK_STATUS in the BSP_OTP_SIGNAL_READ register to
    check that the otp programming is unlocked or not.

Reentrant:
    No
Parameters:
    None
Returns:
    BSP_STATUS_eSuccess if programming is unlocked else
    BSP_STATUS_eFailOtpProgPatternSequence
 ***********************************************************************************
*/
unsigned long BPI_Otp_ProgPatternSequence(BHSM_Handle in_handle)
{
	unsigned long pattern [8] = {0xbc32f4ac, 0xd18616b6, 0x9feb4d54, 0x4a27bf4a, 0xcf1c3178, 0xe2db98a0, 0x24f64bba, 0x7698e712};

    unsigned char i;

    unsigned long ucRetVal = 1;

    /* Set Interface Select register to Firmware(BSP) instead of JTAG */
	BREG_Write32(in_handle->regHandle,XPT_OTP_IF_SEL, 0x00000001 );

	for (i = 0; i < 8; i++)
        BREG_Write32(in_handle->regHandle,XPT_OTP_PROG_PATTERN, pattern[i]);

    /* Read the PATTERN_UNLOCK_STATUS from OTP_SIGNAL_READ_REGISTER */
    ucRetVal = BPI_Otp_CheckProgPatternSequenceStatus(in_handle);

    return ucRetVal;
}

/*
 ***********************************************************************************
 * BPI_Otp_CheckProgPatternSequenceStatus -
 ***********************************************************************************
*/
unsigned long BPI_Otp_CheckProgPatternSequenceStatus(BHSM_Handle in_handle)
{
    unsigned long ulRegValue;
    unsigned long ucRetVal = 1;

    ulRegValue = BREG_Read32(in_handle->regHandle,XPT_OTP_SIGNAL_READ);

    /*
    ** If the PATTERN_UNLOCK_STATUS becomes set then the otp programming
    ** successfully unblocked.
    */
    if (ulRegValue & XPT_OTP_SIGNAL_READ_PATTERN_UNLOCK_STATUS_MASK)
    {
        ucRetVal = 0;
    }
    else
    {
        ucRetVal =  1;
    }

    return ucRetVal;
}

unsigned long BPI_Otp_ProgramMspEnum(BHSM_Handle in_handle, unsigned long in_usMspEnum, unsigned long in_ucNumberBitsToProg, unsigned long in_ulDataValueToProg)
{
    unsigned long BPI_Otp_ProgramMspEnum = 0;
    unsigned long ucBitIndex, usRowIndex, ucLockBitIndex, usLockRowIndex, bitVal, regLockVal;

    BPI_Otp_ProgPatternSequence(in_handle->regHandle);

    while (in_ucNumberBitsToProg) {
		bitVal = in_usMspEnum % 32;
		regLockVal = BREG_Read32(in_handle->regHandle,XPT_OTP_MC1_BASE_LOCK + 4*(in_usMspEnum/32));

		if (regLockVal & (1 << bitVal))
		{
			printf ("OTP Bit already programmed");
			return -1;
		}

		ucBitIndex = in_usMspEnum * BPI_OTP_MSP_BIT_OFFSET % BPI_OTP_MSP_BITS_PER_PHY_ROW;

		usRowIndex = BPI_LOTP_SECTION_MSP_LOGICAL_BASE_ADDRESS + 2 * in_usMspEnum * BPI_OTP_MSP_BIT_OFFSET / BPI_OTP_MSP_BITS_PER_PHY_ROW;

		ucLockBitIndex = (in_usMspEnum * BPI_OTP_MSP_BIT_OFFSET + BPI_OTP_MSP_LOCK_BIT_OFFSET) % BPI_OTP_MSP_BITS_PER_PHY_ROW;

		usLockRowIndex = BPI_LOTP_SECTION_MSP_LOGICAL_BASE_ADDRESS + (2 * (in_usMspEnum * BPI_OTP_MSP_BIT_OFFSET + BPI_OTP_MSP_LOCK_BIT_OFFSET)) / BPI_OTP_MSP_BITS_PER_PHY_ROW;

    	BPI_Otp_ProgramMspEnum = BPI_Otp_ProgramMspBit(in_handle, ucBitIndex, usRowIndex, ucLockBitIndex, usLockRowIndex, (in_ulDataValueToProg & 0x1));
		in_ulDataValueToProg = in_ulDataValueToProg >> 1; /* shift right to next MSP bit */
		in_usMspEnum = in_usMspEnum + 1; /* increment enum */
		in_ucNumberBitsToProg = in_ucNumberBitsToProg - 1; /* decrement number of bits until zero */
	}

    return BPI_Otp_ProgramMspEnum;
}

/**************************************************************
 * BPI_Otp_ProgramMspBit -
 *
 */
unsigned long BPI_Otp_ProgramMspBit(
	BHSM_Handle 	in_handle,
    unsigned char   in_ucBitAddr,   	/* [in] 1 to 54 */
    unsigned short  in_usRowAddr,   	/* [in] */
	unsigned char   in_ucLockBitAddr,  	/* [in] */
    unsigned short  in_usLockRowAddr,  	/* [in] */
    unsigned long in_bBitValue    	/* [in] 0 or 1 */
    )
{
    unsigned char ucRetVal[3];
	unsigned char ucLockRetVal[3];
	unsigned char i;

    /*
    *  These varaibles are introduced to check the status on individual physical
    *  msp bit and then decide the overall status for the msp bit, if it success,
    *  fail or partial success.
    */
    unsigned char ucOverallMspDataBitStatus = 1;
    unsigned char ucOverallMspLockBitStatus = 1;
    unsigned char ucOverallMspStatus = 0;

	/*
	** Initialize the return value to avoid surprises
	*/
	for (i=0; i<3;i++)
	{
		ucRetVal[i] = 1;
		ucLockRetVal[i] = 1;
	}

	/* Check if the lock bit is set for the corresponding MSP bit */
	if (in_bBitValue)		/*progamming to a 1 or 0 */
	{	 /*
		 * We need to attempt programming data bits irrespective of current bit value to regain
		 * redundancy in case previous programming attempts only programmed 1 or 2 bits.
		 */
			/* Program all MSP redundant bits */
			for (i=0; i<3; i++)
			{
				ucRetVal[i] = BPI_Otp_ProgDataBit(in_handle, in_ucBitAddr, in_usRowAddr, in_bBitValue);
				in_ucBitAddr++;
				/* Note we only program 57 MSP bits out of 58*/
				/* Due to the way the Latch OTP map is we will not have a case where the
				** wrap around will happen as the individual data bits or lock bits are not separated
				** at that boundary */
				if( in_ucBitAddr == 57)	/* assuming the bit index is now 0 to 57*/
				{
					in_ucBitAddr=0;
					in_usRowAddr += 2;
				}
			}

			/* Evaluate overall data bit status */
			if ( (ucRetVal[0] == 0) && (ucRetVal[1] == 0) &&
				 (ucRetVal[2] == 0))
			{
				ucOverallMspDataBitStatus = 0;
			}
			else if ( ((ucRetVal[0] == 0) && (ucRetVal[1] == 0)) ||
					  ((ucRetVal[1] == 0) && (ucRetVal[2] == 0)) ||
					  ((ucRetVal[0] == 0) && (ucRetVal[2] == 0)))
			{
				ucOverallMspDataBitStatus = 0;
			}
			else if ( (ucRetVal[0] != 0) &&
					  (ucRetVal[1] != 0) &&
					  (ucRetVal[2] != 0))
			{
				ucOverallMspDataBitStatus = 1;
			}
			else
			{
				ucOverallMspDataBitStatus = 1;
			}

#if 0
			/* if at least 1 MSP data bit got programmed then we need to program all the lock bits */
			if ( (ucOverallMspDataBitStatus != 1) ||
				 (in_bBitReadValue == 1))
			{
				goto OTP_MSP_PROGRAM_LOCK_BITS;
			}
			else
			{
				goto OTP_MSP_PROGRAM_EVALUATE_STATUS;
			}
#endif
	}

OTP_MSP_PROGRAM_LOCK_BITS:
		/* programming only the lock bits only now */
		for (i=0; i<3; i++)
		{
			ucLockRetVal[i] = BPI_Otp_ProgDataBit(in_handle, in_ucLockBitAddr, in_usLockRowAddr, 1);
			in_ucLockBitAddr++;
			if( in_ucLockBitAddr == 57)
			{
				in_ucLockBitAddr = 0;
				in_usLockRowAddr += 2;  /* row address need to increment by 2 as in LOTP only even address are given.*/
			}
		}

		/* Evaluate overall lock data bit status */
		if ( (ucLockRetVal[0] == 0) && (ucLockRetVal[1] == 0) &&
			 (ucLockRetVal[2] == 0))
		{
			ucOverallMspLockBitStatus = 0;
		}
		else if ( ((ucLockRetVal[0] == 0) && (ucLockRetVal[1] == 0)) ||
				  ((ucLockRetVal[1] == 0) && (ucLockRetVal[2] == 0)) ||
				  ((ucLockRetVal[0] == 0) && (ucLockRetVal[2] == 0)))
		{
			ucOverallMspLockBitStatus = 0;
		}
		else if ( (ucLockRetVal[0] != 0) &&
				  (ucLockRetVal[1] != 0) &&
				  (ucLockRetVal[2] != 0))
		{
			ucOverallMspLockBitStatus = 1;
		}
		else
		{
			ucOverallMspLockBitStatus = 1;
		}

#if 0
OTP_MSP_PROGRAM_EVALUATE_STATUS:
		/*
		 * Here we need to evaluate the overall lock and data bit status to generate final status
		*/
		if (in_bBitValue == 1)
		{
			/* We need to evaluate both lock and data bit status */
			if ((ucOverallMspDataBitStatus == 1) ||
				(ucOverallMspLockBitStatus == 1))
			{
				(in_bBitLockValue == 0) ? (ucOverallMspStatus = 1) :
										  (ucOverallMspStatus = 1);
			}
			else if ((ucOverallMspDataBitStatus == 1) ||
					 (ucOverallMspLockBitStatus == 1))
			{
				ucOverallMspStatus = 1;
			}
			else
			{
				ucOverallMspStatus = 0;
			}
		}
		else
		{
			/* Only evaluate lock bit status */
			if (ucOverallMspLockBitStatus == 1)
			{
				(in_bBitLockValue == 0) ? (ucOverallMspStatus = 1) :
										  (ucOverallMspStatus = 1);
			}
			else if (ucOverallMspLockBitStatus == 1)
			{
				ucOverallMspStatus = 1;
			}
			else
			{
				ucOverallMspStatus = 0;
			}
		}

#endif
    return ucOverallMspStatus;
} /* end BPI_Otp_ProgramMspBit() */

/**************************************************************
 * BPI_Otp_ProgDataBit - program one bit
 */
unsigned long  BPI_Otp_ProgDataBit(
	BHSM_Handle 	in_handle,
    unsigned char   in_ucBitAddr,     /* [in] 1 to 54  */
    unsigned short  in_usRowAddr,     /* [in] 0 to 255 for 130nm and 0 to 319 for 65nm */
    unsigned char   in_bDataBitValue  /* [in] 0 or 1 */
    )
{
    int ucRetVal = 0; /* success */

	/* In LOTP the default bit value is 0 so there is no need to program it */
	if (in_bDataBitValue)
	{
		ucRetVal = 	BPI_Otp_ExecuteCommand(in_handle, 1, in_usRowAddr, in_ucBitAddr); /* PROG_BIT_INT_CLK_CMD */
	}

    return( ucRetVal );
} /* end BPI_Otp_ProgDataBit() */

/**************************************************************
 * BPI_Otp_ExecuteCommand -
 * This function processes the OTP commands and returns the result of
 * the command.
 */
unsigned long BPI_Otp_ExecuteCommand(
	BHSM_Handle 	in_handle,
	unsigned char in_ucOtpCmd,
	unsigned short in_usRowAddr,
	unsigned long in_ucBitAddr )
{
	unsigned long  ucRetVal = 0; /* success */
	unsigned long	ulRegValue, ulOtpRegValue1 = 0;

	/*
	** Note: The OTP register reads have been changed to 32 bits read from the 8 bits read
	** as it will guard against any possible bit movements amond the different chipsets.
	*/

	/* Set Interface Select register to Firmware */
	BREG_Write32(in_handle->regHandle, XPT_OTP_IF_SEL, 0x00000001);

	/* Wait for OTP busy to be low before issuing any command*/
	ulRegValue = BREG_Read32(in_handle->regHandle, XPT_OTP_SIGNAL_READ);
	while( ulRegValue & XPT_OTP_SIGNAL_READ_OTP_BUSY_BITS)
	{
		ulRegValue = BREG_Read32(in_handle->regHandle, XPT_OTP_SIGNAL_READ);
	}

	/* set the appropriate bits */
	ulOtpRegValue1 = in_usRowAddr << 5 | in_ucBitAddr << 17;

	if (in_ucOtpCmd == 0x1)
		ulOtpRegValue1 |= XPT_OTP_PROG_SIGNALS_PROG_DIN_MASK;

	/* Set the command */
	ulOtpRegValue1 |= in_ucOtpCmd << 1;

	/* Set START to '1' to execute the command */
	ulOtpRegValue1 |= XPT_OTP_PROG_SIGNALS_START_MASK;

	ulRegValue = BREG_Read32(in_handle->regHandle, XPT_OTP_SIGNAL_READ);

	BREG_Write32(in_handle->regHandle, XPT_OTP_PROG_SIGNALS, ulOtpRegValue1);

	/* Wait for OTP_DONE or OTP_Failure for command status */
	ulRegValue = BREG_Read32(in_handle->regHandle, XPT_OTP_SIGNAL_READ);

	while (!(ulRegValue & XPT_OTP_SIGNAL_READ_OTP_DONE_MASK))
	{
		ulRegValue = BREG_Read32(in_handle->regHandle, XPT_OTP_SIGNAL_READ);
	}

	/* Check if the command failed */
	if( ulRegValue & XPT_OTP_SIGNAL_READ_FAIL_MASK )
	{
		ucRetVal = 1;
	}

	/* Check if the valid bit is set for the program bit*/
		/* Check for the valid bit */
	if (!(ulRegValue & XPT_OTP_SIGNAL_READ_VERIFY_VALID_MASK))
	{
		ucRetVal = 1;
	}

	/* If the read key command was successful then check for DATA_FINISHED*/
	if (in_ucOtpCmd == 0x9) /* read key command */
	{
		ulRegValue = BREG_Read32(in_handle->regHandle, XPT_OTP_FLT_STAT);
		/* wait for OTP_DATA_FINISHED to be 1 */
		while( !(ulRegValue & XPT_OTP_FLT_STAT_OTP_DATA_FINISHED_MASK))
		{
			ulRegValue = BREG_Read32(in_handle->regHandle, XPT_OTP_FLT_STAT);
		}
	}

	BREG_Write32(in_handle->regHandle, XPT_OTP_PROG_SIGNALS, 0x0001a000);
	return ucRetVal;
}

/**********************************************************

REMOTE RECORD RECEIVER SAMPLE CODE
(c) Red Bee Media 2012, all rights reserved
This code is strictly confidential and may only be used or
reproduced under strict licence

This source code is provided as part of the SES Remote Recording
Receiver specification version as listed below and is based on
work by S&T, TV Genius, and SES S.A.

***********************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "rr.h"
#include "crc.h"
#include "md5.h"

/*helper function to provide powers*/
long power (long a, long b){
	long c;
	for (c =1 ; b> 0; b--){
		c = c * a;
	}
	return c;
}

/*! helper unction to validate the receiver ID*/
long validateReceiverId(long receiverId){
	return (receiverId < 0L || receiverId >= 2 << RECEIVER_ID_LENGTH_BITS)? -1 : receiverId;
}

/*! helper function for base 21 encoding*/
void base21encode(unsigned long value, char * encodedString)
{
	int i;

	encodedString[CONFIRMATION_CODE_LENGTH_CHARS] = 0;

	//transform to a coded string
	for (i = CONFIRMATION_CODE_LENGTH_CHARS -1; i >=0 ; i --)
	{
		const char * codeString = CONFIRMATION_CODE_CODING_STRING;

		//cycle through each digit in the coded string and set it from the code key...
		encodedString[i] = codeString[value % strlen(codeString)];

		//remove the coded data from the confirmation code...
		value = value / strlen(codeString);
	}
}

/*! helper function for base 21 decoding */
unsigned long base21decode(char * encodedString)
{
	unsigned long value = 0;
	unsigned char * ch;
	int pos;
	const char * codeString = CONFIRMATION_CODE_CODING_STRING;
	int i;

	//cycle through each character in the string...
	for (i = 0; i <  CONFIRMATION_CODE_LENGTH_CHARS; i++)
	{
		//find the index in the code string
		ch = strchr(codeString, encodedString[i]);
		pos = (int)ch - (int)codeString;

		//add to the number...
		value = value * strlen(codeString)  + pos;
	}

	return value;
}

/* helper function to return an MD5 hash of the registration code */
void getHashedRegistrationCode(unsigned char * registrationCode, unsigned char * hash)
{
	unsigned char toHash[REMOTE_RECORD_SECRET_KEY_LENGTH_CHARS+ REGISTRATION_CODE_LENGTH_CHARS+1];
	MD5_CTX mdContext;

	memset(toHash, 0, REMOTE_RECORD_SECRET_KEY_LENGTH_CHARS+ REGISTRATION_CODE_LENGTH_CHARS+1);

	//concatenate the registration code and the secret key
	strncpy(toHash, registrationCode, REGISTRATION_CODE_LENGTH_CHARS);
	strncpy(&toHash[REGISTRATION_CODE_LENGTH_CHARS], REMOTE_RECORD_SECRET_KEY, REMOTE_RECORD_SECRET_KEY_LENGTH_CHARS + 1);
	toHash[REMOTE_RECORD_SECRET_KEY_LENGTH_CHARS+ REGISTRATION_CODE_LENGTH_CHARS] = 0;

	//calculate the MD5 hash...
	MD5Init (&mdContext);
	MD5Update (&mdContext, toHash,  strlen (toHash));
	MD5Final (&mdContext);

	//return the last two bytes from the MD5 hash...
	hash[0] = mdContext.digest[14];
	hash[1] = mdContext.digest[15];
}


/*! helper unction to calculate the checksum for a passed receiver ID. The receiverID should be a 28 bit receiverID as an integer. The CRC checksum is calculated and truncated to 3 digits*/
int getReceiverChecksum(long receiverId){
	unsigned char receiver_id_string[RECEIVER_ID_LENGTH_CHARS+1];
	crc checksum;

	//print the integer into a string and calculate the checksum...
	sprintf(receiver_id_string, "%li", receiverId);
	checksum = crcSlow(receiver_id_string, sizeof(char) * strlen(receiver_id_string));

	//truncate the checksum to the required number of digits...
	checksum = checksum % power(10 , (REGISTRATION_CODE_LENGTH_CHARS - RECEIVER_ID_LENGTH_CHARS));

	//ensure no leading zero...
	checksum += (checksum < power(10 , (REGISTRATION_CODE_LENGTH_CHARS - RECEIVER_ID_LENGTH_CHARS-1) ) ? power(10 , (REGISTRATION_CODE_LENGTH_CHARS - RECEIVER_ID_LENGTH_CHARS-1)): 0);

	return checksum;
}

/*! Function to get the registration code from a given received ID by appending the checksum*/
void getRegistrationCodeFromReceiverId(long receiverId, unsigned char * registrationCode){
	int checksum;

	//Add a 1 to the beginning of the receiver ID to prevent leading zeros
	receiverId = power (10 , (RECEIVER_ID_LENGTH_CHARS-1)) + receiverId;

	//calculate the checksum..
	checksum = getReceiverChecksum(receiverId);

	//concatenate the checksum and the receiver ID to make the registration code
	sprintf(registrationCode, "%li%i", receiverId, checksum);
}

/*! Function to extract the receiverId from the registration code by extracting all but the last three digits. If the receiver ID is out of range or the checksum is incorrect then this function returns -1*/
long getReceieverIdFromRegistrationCode(unsigned char * registrationCode){
	long receiverId;
	int checksum;
	unsigned char receiverIdString[RECEIVER_ID_LENGTH_CHARS+1];

	//extract the checksum from the last few digits...
	checksum = atoi(&registrationCode[RECEIVER_ID_LENGTH_CHARS]);

	//Copy the received ID from the first few digits and cast to a number...
	strncpy(receiverIdString, registrationCode,RECEIVER_ID_LENGTH_CHARS);
	receiverIdString[RECEIVER_ID_LENGTH_CHARS] = 0;
	receiverId = atol( receiverIdString) ;

	//validate the checksum against the receiver ID...
	if (checksum != getReceiverChecksum(receiverId))
	{
		return -1L;
	}

	//remove the 1 from the beginning. This is added in getRegistrationCodeFromReceiverId to remove leading zeros
	receiverId = receiverId - power(10L , (RECEIVER_ID_LENGTH_CHARS-1));

	//return a validated receiver ID...
	return validateReceiverId(receiverId );
}

/*! Function to return a confirmation code based on a receiver ID and the the relevant receiver information. The confirmation code is returned in the passed string */
void getConfirmationCodeFromRegistrationCodeAndReceiverInformation(unsigned char * registrationCode, struct receiverInformation info, unsigned char * confirmationCode){
	unsigned char rawConfirmationCode[CONFIRMATION_CODE_LENGTH_RAW_CHARS];
	unsigned long confirmationCodeLong;

	//construct the hashed registration code
	getHashedRegistrationCode(registrationCode, rawConfirmationCode);

	//add the receiver information to the last two bytes
	rawConfirmationCode[2] = (info.platform  << 4) + ( info.manufacturer >> 4);
	rawConfirmationCode[3] = ((info.manufacturer % (1 << 4) ) << 4) + info.tuner_config;

	//cast the confirmation code to an unsigned long...
	confirmationCodeLong = (unsigned long) (rawConfirmationCode[0] << 24) + (rawConfirmationCode[1] << 16) + (rawConfirmationCode[2] << 8) + rawConfirmationCode[3];

	//transform to a coded string
	base21encode(confirmationCodeLong, confirmationCode);

	//close the string...
	confirmationCode[CONFIRMATION_CODE_LENGTH_CHARS] = 0;
}

/*! Function to extract receiver information from the confirmation code. Not required on the set top box implementaiton but required for unit testing of the algorithm. The function returns -1 if the confirmation code does not match the required hash */
int getReceiverInformationFromConfirmationCode(struct receiverInformation * info, unsigned char * confirmationCode, long receiverId){
	unsigned char registrationCode[REGISTRATION_CODE_LENGTH_CHARS +1];
	unsigned long confirmationLong;
	unsigned char decodedConfirmationCode[CONFIRMATION_CODE_LENGTH_RAW_CHARS];
	unsigned char hash[2];

	//get the registration code from the receiver id...
	getRegistrationCodeFromReceiverId(receiverId, registrationCode);

	//get the md5 hash from the registration code...
	getHashedRegistrationCode(registrationCode, hash);

	//decode the confirmation code to extract it as a number...
	confirmationLong =  base21decode(confirmationCode);

	//extract each byte from the confirmation code...
	decodedConfirmationCode[0] = (confirmationLong >> 24) % ( 1 << 8);
	decodedConfirmationCode[1] = (confirmationLong >> 16) % ( 1 << 8);
	decodedConfirmationCode[2] = (confirmationLong >> 8) % ( 1 << 8);
	decodedConfirmationCode[3] = confirmationLong  % ( 1 << 8);

	//check that the md5 hash matches...
	if ((hash[0] != decodedConfirmationCode[0]) || (hash[1] != decodedConfirmationCode[1]))
	{
		//return -1 if the hash does not match...
		info->platform = PLATFORM_RESERVED;
		info->manufacturer = MANUFACTURER_RESERVED;
		info->tuner_config = CONFIG_INCOMPATIBLE;
		return -1;
	}

	//otherwise decode the receiver information from the bytes...
	info->platform = decodedConfirmationCode[2] >>4 ;
	info->manufacturer = ((decodedConfirmationCode[2] % ( 1 << 4))  << 4) + (decodedConfirmationCode[3] >>4);
	info->tuner_config = decodedConfirmationCode[3] % (1 << 4);

	return 0;
}


/**********************************************************

REMOTE RECORD RECEIVER SAMPLE CODE
(c) Red Bee Media 2012, all rights reserved
This code is strictly confidential and may only be used or
reproduced under strict licence

This source code is provided as part of the SES Remote Recording
Receiver specification version as listed below and is based on
work by S&T, TV Genius, and SES S.A.

***********************************************************/

#ifndef REMOTE_RECORD_VERSION
#define REMOTE_RECORD_VERSION 1

/*! Define the secret key */
#define REMOTE_RECORD_SECRET_KEY "E<u@JcS]JoP~ )43  ^0uQ03o;pJ^NG#An,Nk^8BfJDrp)kv&.wxT*&)]kAxH|ts"
#define REMOTE_RECORD_SECRET_KEY_LENGTH_CHARS 64

/*!Define the confirmation code coding string */
#define CONFIRMATION_CODE_CODING_STRING "PYTRWQSDFGHJKLMNBVCXZ"

/*! Define the lengths of various elements*/
#define REGISTRATION_CODE_LENGTH_CHARS 12
#define RECEIVER_ID_LENGTH_BITS 28
#define RECEIVER_ID_LENGTH_CHARS 9
#define CONFIRMATION_CODE_LENGTH_RAW_CHARS 4
#define CONFIRMATION_CODE_LENGTH_CHARS 8


/*! Enumerated type for receiver platform */
typedef enum receiverPlatform {
	PLATFORM_RESERVED = 0
	,PLATFORM_ASTRA = 1
	,PLATFORM_HD_PLUS = 2
};

/*! Enumerated type for receiver manufacturer */
typedef enum receiverManufacturer {
	MANUFACTURER_RESERVED = 0
	,MANUFACTURER_ATEVIO = 1
	,MANUFACTURER_BOCA = 2
	,MANUFACTURER_COMAG = 3
	,MANUFACTURER_DIGITALBOX = 4
	,MANUFACTURER_DYON = 5
	,MANUFACTURER_FTE_MAXIMAL = 6
	,MANUFACTURER_GRUNDIG = 7
	,MANUFACTURER_HOMECAST = 8
	,MANUFACTURER_HUMAX = 9
	,MANUFACTURER_IMPERIAL = 10
	,MANUFACTURER_INVERTO = 11
	,MANUFACTURER_I_SET = 12
	,MANUFACTURER_JAEGER = 13
	,MANUFACTURER_KAON = 14
	,MANUFACTURER_KATHREIN = 15
	,MANUFACTURER_LENUSS = 16
	,MANUFACTURER_LG = 17
	,MANUFACTURER_LOEWE = 18
	,MANUFACTURER_LOGISAT = 19
	,MANUFACTURER_MEDION = 20
	,MANUFACTURER_MICROM = 21
	,MANUFACTURER_NANOXX = 22
	,MANUFACTURER_OPTIQUM = 23
	,MANUFACTURER_ORBITECH = 24
	,MANUFACTURER_PANASONIC = 25
	,MANUFACTURER_PHILIPS = 26
	,MANUFACTURER_POLLIN = 27
	,MANUFACTURER_SAMSUNG = 28
	,MANUFACTURER_SCHWAIGER = 29
	,MANUFACTURER_SCS = 30
	,MANUFACTURER_SEG = 31
	,MANUFACTURER_SET_ONE = 32
	,MANUFACTURER_SHARP = 33
	,MANUFACTURER_SKT = 34
	,MANUFACTURER_SKYMASTER = 35
	,MANUFACTURER_SL_C_T = 36
	,MANUFACTURER_SMART = 37
	,MANUFACTURER_SONY = 38
	,MANUFACTURER_STRONG = 39
	,MANUFACTURER_TECHNISAT = 40
	,MANUFACTURER_TECHNOTREND = 41
	,MANUFACTURER_TECHWOOD = 42
	,MANUFACTURER_TELEFUNKEN = 43
	,MANUFACTURER_TELEKOM = 44
	,MANUFACTURER_TELESTAR = 45
	,MANUFACTURER_TOPFIELD = 46
	,MANUFACTURER_TRIAX = 47
	,MANUFACTURER_VANTAGE = 48
	,MANUFACTURER_VIDEOWEB = 49
	,MANUFACTURER_WISI = 50
	,MANUFACTURER_XORO = 51
};

/*! Enumerated type for receiver configuration information */
typedef enum receiverConfiguration{
	CONFIG_INCOMPATIBLE = 0
	,CONFIG_SINGLE_TP = 1
	,CONFIG_TWO_CONSTRAINED_TPS = 2
	,CONFIG_TWO_UNSCONSTRAINED_TPS = 3
	,CONFIG_MORE_THAN_TWO_CONSTRAINED_TPS = 4
	,CONFIG_MORE_THAN_TWO_UNCONSTRAINED_TPS = 5
};

/*! Structure to hold receiver information */
typedef struct receiverInformation
{
	enum receiverPlatform platform;
	enum receiverManufacturer manufacturer;
    enum receiverConfiguration tuner_config;
};

/* Function prototypes */
void getRegistrationCodeFromReceiverId(long receiverId, unsigned char * registrationCode);
long getReceieverIdFromRegistrationCode(unsigned char * registrationCode);
void getConfirmationCodeFromRegistrationCodeAndReceiverInformation(unsigned char * registrationCode, struct receiverInformation info, unsigned char * confirmationCode);
int getReceiverInformationFromConfirmationCode(struct receiverInformation * info, unsigned char * confirmationCode, long receiverId);

#endif
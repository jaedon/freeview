/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  isosvc.h
	@brief

	Description: 		\n
	Module: mw/common/util			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	_ISOSVC_H_
#define	_ISOSVC_H_

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <htype.h>
#include <stb_params.h>
#include <hlib.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/* 8859, 6937, 300468 차이가 없다. 의미상 구분일 뿐 */
typedef int			STR_LEN;
typedef	char			*CH_POINTER;
typedef	HUINT8	CH_8859;
typedef	HUINT8	CH_6937;
typedef	HUINT8		CH_300468;
typedef	HUINT8		CH_ASCII;
typedef	HUINT8		CH_1BYTE;
typedef	HUINT8	CH_UTF8;
typedef	HUINT16	CH_UCS2;
typedef	unsigned long		CH_UCS4;		/**< 4byte 보장은 unsigned long만 가능, unsigned int는 system dependent 하다. C99 */

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define ____iso_3166_codes____
/*
	from http://en.wikipedia.org/wiki/ISO_3166-1_alpha-3

	Officially assigned code elements
	Below is a complete list of the current officially assigned ISO 3166-1 alpha-3 codes,
	with country names being English short country names officially used by the ISO 3166 Maintenance Agency (ISO 3166/MA):
*/
#define ISO3166CODE_ABW "ABW" //Aruba
#define ISO3166CODE_AFG "AFG" //Afghanistan
#define ISO3166CODE_AGO "AGO" //Angola
#define ISO3166CODE_AIA "AIA" //Anguilla
#define ISO3166CODE_ALA "ALA" //Aland Islands
#define ISO3166CODE_ALB "ALB" //Albania
#define ISO3166CODE_AND "AND" //Andorra
#define ISO3166CODE_ANT "ANT" //Netherlands Antilles
#define ISO3166CODE_ARE "ARE" //United Arab Emirates
#define ISO3166CODE_ARG "ARG" //Argentina
#define ISO3166CODE_ARM "ARM" //Armenia
#define ISO3166CODE_ASM "ASM" //American Samoa
#define ISO3166CODE_ATA "ATA" //Antarctica
#define ISO3166CODE_ATF "ATF" //French Southern Territories
#define ISO3166CODE_ATG "ATG" //Antigua and Barbuda
#define ISO3166CODE_AUS "AUS" //Australia
#define ISO3166CODE_AUT "AUT" //Austria
#define ISO3166CODE_AZE "AZE" //Azerbaijan
#define ISO3166CODE_BDI "BDI" //Burundi
#define ISO3166CODE_BEL "BEL" //Belgium
#define ISO3166CODE_BEN "BEN" //Benin
#define ISO3166CODE_BFA "BFA" //Burkina Faso
#define ISO3166CODE_BGD "BGD" //Bangladesh
#define ISO3166CODE_BGR "BGR" //Bulgaria
#define ISO3166CODE_BHR "BHR" //Bahrain
#define ISO3166CODE_BHS "BHS" //Bahamas
#define ISO3166CODE_BIH "BIH" //Bosnia and Herzegovina
#define ISO3166CODE_BLM "BLM" //Saint Barthelemy
#define ISO3166CODE_BLR "BLR" //Belarus
#define ISO3166CODE_BLZ "BLZ" //Belize
#define ISO3166CODE_BMU "BMU" //Bermuda
#define ISO3166CODE_BOL "BOL" //Bolivia
#define ISO3166CODE_BRA "BRA" //Brazil
#define ISO3166CODE_BRB "BRB" //Barbados
#define ISO3166CODE_BRN "BRN" //Brunei Darussalam
#define ISO3166CODE_BTN "BTN" //Bhutan
#define ISO3166CODE_BVT "BVT" //Bouvet Island
#define ISO3166CODE_BWA "BWA" //Botswana
#define ISO3166CODE_CAF "CAF" //Central African Republic
#define ISO3166CODE_CAN "CAN" //Canada
#define ISO3166CODE_CCK "CCK" //Cocos (Keeling) Islands
#define ISO3166CODE_CHE "CHE" //Switzerland
#define ISO3166CODE_CHL "CHL" //Chile
#define ISO3166CODE_CHN "CHN" //China
#define ISO3166CODE_CIV "CIV" //Cote d'Ivoire
#define ISO3166CODE_CMR "CMR" //Cameroon
#define ISO3166CODE_COD "COD" //Congo, the Democratic Republic of the
#define ISO3166CODE_COG "COG" //Congo
#define ISO3166CODE_COK "COK" //Cook Islands
#define ISO3166CODE_COL "COL" //Colombia
#define ISO3166CODE_COM "COM" //Comoros
#define ISO3166CODE_CPV "CPV" //Cape Verde
#define ISO3166CODE_CRI "CRI" //Costa Rica
#define ISO3166CODE_CUB "CUB" //Cuba
#define ISO3166CODE_CXR "CXR" //Christmas Island
#define ISO3166CODE_CYM "CYM" //Cayman Islands
#define ISO3166CODE_CYP "CYP" //Cyprus
#define ISO3166CODE_CZE "CZE" //Czech Republic
#define ISO3166CODE_DEU "DEU" //Germany
#define ISO3166CODE_DJI "DJI" //Djibouti
#define ISO3166CODE_DMA "DMA" //Dominica
#define ISO3166CODE_DNK "DNK" //Denmark
#define ISO3166CODE_DOM "DOM" //Dominican Republic
#define ISO3166CODE_DZA "DZA" //Algeria
#define ISO3166CODE_ECU "ECU" //Ecuador
#define ISO3166CODE_EGY "EGY" //Egypt
#define ISO3166CODE_ERI "ERI" //Eritrea
#define ISO3166CODE_ESH "ESH" //Western Sahara
#define ISO3166CODE_ESP "ESP" //Spain
#define ISO3166CODE_EST "EST" //Estonia
#define ISO3166CODE_ETH "ETH" //Ethiopia
#define ISO3166CODE_FIN "FIN" //Finland
#define ISO3166CODE_FJI "FJI" //Fiji
#define ISO3166CODE_FLK "FLK" //Falkland Islands (Malvinas)
#define ISO3166CODE_FRA "FRA" //France
#define ISO3166CODE_FRO "FRO" //Faroe Islands
#define ISO3166CODE_FSM "FSM" //Micronesia, Federated States of
#define ISO3166CODE_GAB "GAB" //Gabon
#define ISO3166CODE_GBR "GBR" //United Kingdom
#define ISO3166CODE_GEO "GEO" //Georgia
#define ISO3166CODE_GGY "GGY" //Guernsey
#define ISO3166CODE_GHA "GHA" //Ghana
#define ISO3166CODE_GIN "GIN" //Guinea
#define ISO3166CODE_GIB "GIB" //Gibraltar
#define ISO3166CODE_GLP "GLP" //Guadeloupe
#define ISO3166CODE_GMB "GMB" //Gambia
#define ISO3166CODE_GNB "GNB" //Guinea-Bissau
#define ISO3166CODE_GNQ "GNQ" //Equatorial Guinea
#define ISO3166CODE_GRC "GRC" //Greece
#define ISO3166CODE_GRD "GRD" //Grenada
#define ISO3166CODE_GRL "GRL" //Greenland
#define ISO3166CODE_GTM "GTM" //Guatemala
#define ISO3166CODE_GUF "GUF" //French Guiana
#define ISO3166CODE_GUM "GUM" //Guam
#define ISO3166CODE_GUY "GUY" //Guyana
#define ISO3166CODE_HKG "HKG" //Hong Kong
#define ISO3166CODE_HMD "HMD" //Heard Island and McDonald Islands
#define ISO3166CODE_HND "HND" //Honduras
#define ISO3166CODE_HRV "HRV" //Croatia
#define ISO3166CODE_HTI "HTI" //Haiti
#define ISO3166CODE_HUN "HUN" //Hungary
#define ISO3166CODE_IDN "IDN" //Indonesia
#define ISO3166CODE_IMN "IMN" //Isle of Man
#define ISO3166CODE_IND "IND" //India
#define ISO3166CODE_IOT "IOT" //British Indian Ocean Territory
#define ISO3166CODE_IRL "IRL" //Ireland
#define ISO3166CODE_IRN "IRN" //Iran, Islamic Republic of
#define ISO3166CODE_IRQ "IRQ" //Iraq
#define ISO3166CODE_ISL "ISL" //Iceland
#define ISO3166CODE_ISR "ISR" //Israel
#define ISO3166CODE_ITA "ITA" //Italy
#define ISO3166CODE_JAM "JAM" //Jamaica
#define ISO3166CODE_JEY "JEY" //Jersey
#define ISO3166CODE_JOR "JOR" //Jordan
#define ISO3166CODE_JPN "JPN" //Japan
#define ISO3166CODE_KAZ "KAZ" //Kazakhstan
#define ISO3166CODE_KEN "KEN" //Kenya
#define ISO3166CODE_KGZ "KGZ" //Kyrgyzstan
#define ISO3166CODE_KHM "KHM" //Cambodia
#define ISO3166CODE_KIR "KIR" //Kiribati
#define ISO3166CODE_KNA "KNA" //Saint Kitts and Nevis
#define ISO3166CODE_KOR "KOR" //Korea, Republic of
#define ISO3166CODE_KWT "KWT" //Kuwait
#define ISO3166CODE_LAO "LAO" //Lao People's Democratic Republic
#define ISO3166CODE_LBN "LBN" //Lebanon
#define ISO3166CODE_LBR "LBR" //Liberia
#define ISO3166CODE_LBY "LBY" //Libyan Arab Jamahiriya
#define ISO3166CODE_LCA "LCA" //Saint Lucia
#define ISO3166CODE_LIE "LIE" //Liechtenstein
#define ISO3166CODE_LKA "LKA" //Sri Lanka
#define ISO3166CODE_LSO "LSO" //Lesotho
#define ISO3166CODE_LTU "LTU" //Lithuania
#define ISO3166CODE_LUX "LUX" //Luxembourg
#define ISO3166CODE_LVA "LVA" //Latvia
#define ISO3166CODE_MAC "MAC" //Macao
#define ISO3166CODE_MAF "MAF" //Saint Martin (French part)
#define ISO3166CODE_MAR "MAR" //Morocco
#define ISO3166CODE_MCO "MCO" //Monaco
#define ISO3166CODE_MDA "MDA" //Moldova
#define ISO3166CODE_MDG "MDG" //Madagascar
#define ISO3166CODE_MDV "MDV" //Maldives
#define ISO3166CODE_MEX "MEX" //Mexico
#define ISO3166CODE_MHL "MHL" //Marshall Islands
#define ISO3166CODE_MKD "MKD" //Macedonia, the former Yugoslav Republic of
#define ISO3166CODE_MLI "MLI" //Mali
#define ISO3166CODE_MLT "MLT" //Malta
#define ISO3166CODE_MMR "MMR" //Myanmar
#define ISO3166CODE_MNE "MNE" //Montenegro
#define ISO3166CODE_MNG "MNG" //Mongolia
#define ISO3166CODE_MNP "MNP" //Northern Mariana Islands
#define ISO3166CODE_MOZ "MOZ" //Mozambique
#define ISO3166CODE_MRT "MRT" //Mauritania
#define ISO3166CODE_MSR "MSR" //Montserrat
#define ISO3166CODE_MTQ "MTQ" //Martinique
#define ISO3166CODE_MUS "MUS" //Mauritius
#define ISO3166CODE_MWI "MWI" //Malawi
#define ISO3166CODE_MYS "MYS" //Malaysia
#define ISO3166CODE_MYT "MYT" //Mayotte
#define ISO3166CODE_NAM "NAM" //Namibia
#define ISO3166CODE_NCL "NCL" //New Caledonia
#define ISO3166CODE_NER "NER" //Niger
#define ISO3166CODE_NFK "NFK" //Norfolk Island
#define ISO3166CODE_NGA "NGA" //Nigeria
#define ISO3166CODE_NIC "NIC" //Nicaragua
#define ISO3166CODE_NOR "NOR" //Norway
#define ISO3166CODE_NIU "NIU" //Niue
#define ISO3166CODE_NLD "NLD" //Netherlands
#define ISO3166CODE_NPL "NPL" //Nepal
#define ISO3166CODE_NRU "NRU" //Nauru
#define ISO3166CODE_NZL "NZL" //New Zealand
#define ISO3166CODE_OMN "OMN" //Oman
#define ISO3166CODE_PAK "PAK" //Pakistan
#define ISO3166CODE_PAN "PAN" //Panama
#define ISO3166CODE_PCN "PCN" //Pitcairn
#define ISO3166CODE_PER "PER" //Peru
#define ISO3166CODE_PHL "PHL" //Philippines
#define ISO3166CODE_PLW "PLW" //Palau
#define ISO3166CODE_PNG "PNG" //Papua New Guinea
#define ISO3166CODE_POL "POL" //Poland
#define ISO3166CODE_PRI "PRI" //Puerto Rico
#define ISO3166CODE_PRK "PRK" //Korea, Democratic People's Republic of
#define ISO3166CODE_PRT "PRT" //Portugal
#define ISO3166CODE_PRY "PRY" //Paraguay
#define ISO3166CODE_PSE "PSE" //Palestinian Territory, Occupied
#define ISO3166CODE_PYF "PYF" //French Polynesia
#define ISO3166CODE_QAT "QAT" //Qatar
#define ISO3166CODE_REU "REU" //Reunion
#define ISO3166CODE_ROU "ROU" //Romania
#define ISO3166CODE_RUS "RUS" //Russian Federation
#define ISO3166CODE_RWA "RWA" //Rwanda
#define ISO3166CODE_SAU "SAU" //Saudi Arabia
#define ISO3166CODE_SDN "SDN" //Sudan
#define ISO3166CODE_SEN "SEN" //Senegal
#define ISO3166CODE_SGP "SGP" //Singapore
#define ISO3166CODE_SGS "SGS" //South Georgia and the South Sandwich Islands
#define ISO3166CODE_SHN "SHN" //Saint Helena
#define ISO3166CODE_SJM "SJM" //Svalbard and Jan Mayen
#define ISO3166CODE_SLB "SLB" //Solomon Islands
#define ISO3166CODE_SLE "SLE" //Sierra Leone
#define ISO3166CODE_SLV "SLV" //El Salvador
#define ISO3166CODE_SMR "SMR" //San Marino
#define ISO3166CODE_SOM "SOM" //Somalia
#define ISO3166CODE_SPM "SPM" //Saint Pierre and Miquelon
#define ISO3166CODE_SRB "SRB" //Serbia
#define ISO3166CODE_STP "STP" //Sao Tome and Principe
#define ISO3166CODE_SUR "SUR" //Suriname
#define ISO3166CODE_SVK "SVK" //Slovakia
#define ISO3166CODE_SVN "SVN" //Slovenia
#define ISO3166CODE_SWE "SWE" //Sweden
#define ISO3166CODE_SWZ "SWZ" //Swaziland
#define ISO3166CODE_SYC "SYC" //Seychelles
#define ISO3166CODE_SYR "SYR" //Syrian Arab Republic
#define ISO3166CODE_TCA "TCA" //Turks and Caicos Islands
#define ISO3166CODE_TCD "TCD" //Chad
#define ISO3166CODE_TGO "TGO" //Togo
#define ISO3166CODE_THA "THA" //Thailand
#define ISO3166CODE_TJK "TJK" //Tajikistan
#define ISO3166CODE_TKL "TKL" //Tokelau
#define ISO3166CODE_TKM "TKM" //Turkmenistan
#define ISO3166CODE_TLS "TLS" //Timor-Leste
#define ISO3166CODE_TON "TON" //Tonga
#define ISO3166CODE_TTO "TTO" //Trinidad and Tobago
#define ISO3166CODE_TUN "TUN" //Tunisia
#define ISO3166CODE_TUR "TUR" //Turkey
#define ISO3166CODE_TUV "TUV" //Tuvalu
#define ISO3166CODE_TWN "TWN" //Taiwan, Province of China
#define ISO3166CODE_TZA "TZA" //Tanzania, United Republic of
#define ISO3166CODE_UGA "UGA" //Uganda
#define ISO3166CODE_UKR "UKR" //Ukraine
#define ISO3166CODE_UMI "UMI" //United States Minor Outlying Islands
#define ISO3166CODE_URY "URY" //Uruguay
#define ISO3166CODE_USA "USA" //United States
#define ISO3166CODE_UZB "UZB" //Uzbekistan
#define ISO3166CODE_VAT "VAT" //Holy See (Vatican City State)
#define ISO3166CODE_VCT "VCT" //Saint Vincent and the Grenadines
#define ISO3166CODE_VEN "VEN" //Venezuela
#define ISO3166CODE_VGB "VGB" //Virgin Islands, British
#define ISO3166CODE_VIR "VIR" //Virgin Islands, U.S.
#define ISO3166CODE_VNM "VNM" //Viet Nam
#define ISO3166CODE_VUT "VUT" //Vanuatu
#define ISO3166CODE_WLF "WLF" //Wallis and Futuna
#define ISO3166CODE_WSM "WSM" //Samoa
#define ISO3166CODE_YEM "YEM" //Yemen
#define ISO3166CODE_ZAF "ZAF" //South Africa
#define ISO3166CODE_ZMB "ZMB" //Zambia
#define ISO3166CODE_ZWE "ZWE" //Zimbabwe


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
#define	ERR_UT_NOMATCH		1


typedef	struct {
	HUINT8	acCode1[4];
	HUINT8	acCode2[4];
	HUINT8	acCode3[4];

	HUINT8	acFontSel[4];
	HUINT32	unFontSelLen;
} mwUtIso639Font;


typedef HERROR (*MwcCountDrawableCharNum)(HUINT8 *szString, HUINT32 *pulCharNum);
typedef HERROR (*MwcGetDrawableChar)(HUINT8 *szString, HUINT32 ulIndex, HUINT8 *szCharWithCode, HUINT32 *pulCharLenWithCode, HUINT8 *szCharOnly, HUINT32 *pulCharLenOnly);
typedef HERROR (*MwcConvertCodepageToUCS2)(const CH_1BYTE *pSrcStr, const HINT32 nSrcStrLen, HINT32 *pnUsedByte, CH_UCS2 *pDestUCS2Str, HINT32 *pnDestUCS2StrLen);
typedef HERROR (*MwcConvertUCS2ToCodepage)(const CH_UCS2 *pSrcUCS2Str, const HINT32 nSrcStrLen, const HINT32 nBufferLen, HINT32 *pnUsedByte, CH_1BYTE *pDestStr, HINT32 *pnDestStrLen);
typedef HERROR (*MwcGetEmphasizedString)(HUINT8 *szSrcStr, HUINT8 *szDstStr);

typedef struct tagCodePageContext
{
	MwcCountDrawableCharNum 	cbCountDrawableCharNum;
	MwcGetDrawableChar			cbGetDrawableChar;
	MwcConvertCodepageToUCS2	cbConvertCodepageToUCS2;
	MwcConvertUCS2ToCodepage	cbConvertUCS2ToCodepage; /* optional */
	MwcGetEmphasizedString		cbGetEmphasizedString;	 /* optional */
	void					*pTable;				 /* optional : 300468만족하기 위해 만듬 */
}CPContext_t;


typedef enum tag_eEn300468Encoding_TextBytePosition
{
	eTextBytePos_None = 0,
	eTextBytePos_First,
	eTextBytePos_Second,
	eTextBytePos_Third,
	eTextBytePos_Fourth,
	eTextBytePos_Max
} eTextBytePos_t;


typedef enum {
	ERR_ENCODING_OK=0,
	ERR_ENCODING_FAIL,
	ERR_NOT_SUPPORTED_CODEPAGE,
	ERR_OUTOF_CODEPAGE,
	ERR_ILLEGAL_SEQ,
	ERR_TOOSHORT,
	ERR_CODEPAGE_CH,
	ERR_CONTROL_CH,
	ERR_WHITE_CH,
	ERR_USERDEFINE_CH,
	ERR_NULL_CH,
	ERR_NEWLINE_CH,
	ERR_BUFFER_TOOSMALL,
	ERR_CONV_MAX
} ConvEncoding_Err_t;

#if 0
typedef enum {
/* single byte container */
	eHxCP_ISO_ASCII=0,		/**< 7bit code, but storage is 8bit */
	eHxCP_ISO_SINGLEBYTE,	/**< 8bit code, plane text, almost same with 6937 */
	eHxCP_ISO_6937,
	eHxCP_ISO_8859,	/**< not plane text, take character table code  */
	eHxCP_ISO_8859_1,
	eHxCP_ISO_8859_2,
	eHxCP_ISO_8859_3,
	eHxCP_ISO_8859_4,
	eHxCP_ISO_8859_5,
	eHxCP_ISO_8859_6,
	eHxCP_ISO_8859_7,
	eHxCP_ISO_8859_8,
	eHxCP_ISO_8859_9,
	eHxCP_ISO_8859_10,
	eHxCP_ISO_8859_11,
	eHxCP_ISO_8859_12,
	eHxCP_ISO_8859_13,
	eHxCP_ISO_8859_14,
	eHxCP_ISO_8859_15,
	eHxCP_ISO_300468,
//	eHxCP_ISO_EUROPE300468,
	eHxCP_ISO_LG,	/**< ㅜㅜ, 보통은 ascii, icon은 UTF-16 */
	eHxCP_UNICODE_UTF8,
	eHxCP_UNICODE_DVB_UTF8,

/* Area codepage */
	eHxCP_ISO_JAPAN_EUC,
	eHxCP_ISO_JAPAN_JIS,
	eHxCP_ISO_JAPAN_SJIS,

/* JUST define, humax dont use */
	eHxCP_ISO_SIMPLEFIED_CHINESE,
	eHxCP_ISO_TRADITIONAL_CHINESE,
/* 2 byte container */
	eHxCP_UNICODE_UCS2,
	eHxCP_UNICODE_DVB_UCS2,
/* 4 byte container */
	eHxCP_ISO_UCS4,
/* extra */
	eHxCP_ISO_EMFLASH,
	eHxCP_ISO_EMFLASH_UNI,

/* Win CP */
	eHxCP_WIN_1251, //Russia 향에서 CP1251 (for Cyrillic) 에 대응하기 위해 추가

/* ~ */
	eHCodepage_MAX
} eHxCP_CodePage_e;
#endif
typedef enum
{
	eSITEXT_ENCODED_NONE = 0,

	eSITEXT_ENCODED_300468,				// 300-468 에 완전 적용되는 encoding type
	eSITEXT_ENCODED_300468_EUROPE,		// code table이 없으면 8859-9로 대체되는 것 외에는 300-468에 적용된다
	eSITEXT_ENCODED_300468_UK,			// 300-468 base에 text에 NULL 문자 등이 들어 올 수 있다, For UK DTT + UK DTT HD + UK FSAT + Decompress, Let's delete UKDTT after all implementation
	eSITEXT_ENCODED_300468_TRUEVISION,	// code table에 없으면 8859-11, 로케일코드가 0x6인경우, 0x7로 치환
	eSITEXT_ENCODED_JAPAN_JIS,			// japan에서 si로 들어오는 String.

	// Text 변환 사양이 달라지면 위에 계속 추가하도록 한다.
	eSITEXT_ENCODED_MAX
} SiTxtEncodedType_t;


typedef struct CharacterMapItem_UCS
{
	HUINT8		ucISOCode;
	HUINT16		usUNICODE;
}CHARMAPITEM_toUnicode;


typedef struct CharacterMapItem
{
	HUINT8		ucISOCode;
	HUINT16		usUNICODE;
}CHARMAPITEM_t;

#define		DECODE_CHARACTER_COPY_WHITECH	0

/* en 300 468 character code */
#define		EN300468_LOCALE_CODE_1BYTE_LEN	1
#define		EN300468_LOCALE_CODE_MAX		3
#define		EN300468_1BYTE_LOCALE_CODE_MAX 0x15
#define		EN300468_3BYTE_LOCALE_CODE_MAX 0x0F

#define		EN300468_8859_5_1BTYE_VALUE		0x01
#define		EN300468_8859_6_1BTYE_VALUE		0x02
#define		EN300468_8859_7_1BTYE_VALUE		0x03
#define		EN300468_8859_8_1BTYE_VALUE		0x04
#define		EN300468_8859_9_1BTYE_VALUE		0x05
#define		EN300468_8859_10_1BTYE_VALUE		0x06
#define		EN300468_8859_11_1BTYE_VALUE		0x07
#define		EN300468_8859_12_1BTYE_VALUE		0x08
#define		EN300468_8859_13_1BTYE_VALUE		0x09
#define		EN300468_8859_14_1BTYE_VALUE		0x0A
#define		EN300468_8859_15_1BTYE_VALUE		0x0B
#define		EN300468_8859_3BTYE_FIRST_VALUE		0x10
#define		EN300468_8859_3BTYE_2ND_VALUE		0x00

#define		EN300468_UTF16_1BYTE_VALUE		0x11
#define		EN300468_KSC5601_1987_1BYTE_VALUE		0x12
#define		EN300468_GB_2312_1980_1BYTE_VALUE		0x13
#define		EN300468_BIG5_1BYTE_VALUE				0x14
#define		EN300468_UTF8_1BYTE_VALUE		0x15

#define		EN300468_8859_1_3BYTE_3TH_VALUE		0x01
#define		EN300468_8859_2_3BYTE_3TH_VALUE		0x02
#define		EN300468_8859_3_3BYTE_3TH_VALUE		0x03
#define		EN300468_8859_4_3BYTE_3TH_VALUE		0x04
#define		EN300468_8859_5_3BYTE_3TH_VALUE		0x05
#define		EN300468_8859_6_3BYTE_3TH_VALUE		0x06
#define		EN300468_8859_7_3BYTE_3TH_VALUE		0x07
#define		EN300468_8859_8_3BYTE_3TH_VALUE		0x08
#define		EN300468_8859_9_3BYTE_3TH_VALUE		0x09
#define		EN300468_8859_10_3BYTE_3TH_VALUE		0x0A
#define		EN300468_8859_11_3BYTE_3TH_VALUE		0x0B
/* 8859-12, reserved for indian */
#define		EN300468_8859_13_3BYTE_3TH_VALUE		0x0D
#define		EN300468_8859_14_3BYTE_3TH_VALUE		0x0E
#define		EN300468_8859_15_3BYTE_3TH_VALUE		0x0F

#define		EN300468_8859_1_3BYTE_VALUE		(((EN300468_8859_3BTYE_FIRST_VALUE)<<16)|((EN300468_8859_3BTYE_2ND_VALUE)<<8)|(EN300468_8859_1_3BYTE_3TH_VALUE))
#define		EN300468_8859_2_3BYTE_VALUE		(((EN300468_8859_3BTYE_FIRST_VALUE)<<16)|((EN300468_8859_3BTYE_2ND_VALUE)<<8)|(EN300468_8859_2_3BYTE_3TH_VALUE))
#define		EN300468_8859_3_3BYTE_VALUE		(((EN300468_8859_3BTYE_FIRST_VALUE)<<16)|((EN300468_8859_3BTYE_2ND_VALUE)<<8)|(EN300468_8859_3_3BYTE_3TH_VALUE))
#define		EN300468_8859_4_3BYTE_VALUE		(((EN300468_8859_3BTYE_FIRST_VALUE)<<16)|((EN300468_8859_3BTYE_2ND_VALUE)<<8)|(EN300468_8859_4_3BYTE_3TH_VALUE))
#define		EN300468_8859_5_3BYTE_VALUE		(((EN300468_8859_3BTYE_FIRST_VALUE)<<16)|((EN300468_8859_3BTYE_2ND_VALUE)<<8)|(EN300468_8859_5_3BYTE_3TH_VALUE))
#define		EN300468_8859_6_3BYTE_VALUE		(((EN300468_8859_3BTYE_FIRST_VALUE)<<16)|((EN300468_8859_3BTYE_2ND_VALUE)<<8)|(EN300468_8859_6_3BYTE_3TH_VALUE))
#define		EN300468_8859_7_3BYTE_VALUE		(((EN300468_8859_3BTYE_FIRST_VALUE)<<16)|((EN300468_8859_3BTYE_2ND_VALUE)<<8)|(EN300468_8859_7_3BYTE_3TH_VALUE))
#define		EN300468_8859_8_3BYTE_VALUE		(((EN300468_8859_3BTYE_FIRST_VALUE)<<16)|((EN300468_8859_3BTYE_2ND_VALUE)<<8)|(EN300468_8859_8_3BYTE_3TH_VALUE))
#define		EN300468_8859_9_3BYTE_VALUE		(((EN300468_8859_3BTYE_FIRST_VALUE)<<16)|((EN300468_8859_3BTYE_2ND_VALUE)<<8)|(EN300468_8859_9_3BYTE_3TH_VALUE))
#define		EN300468_8859_10_3BYTE_VALUE		(((EN300468_8859_3BTYE_FIRST_VALUE)<<16)|((EN300468_8859_3BTYE_2ND_VALUE)<<8)|(EN300468_8859_10_3BYTE_3TH_VALUE))
#define		EN300468_8859_11_3BYTE_VALUE		(((EN300468_8859_3BTYE_FIRST_VALUE)<<16)|((EN300468_8859_3BTYE_2ND_VALUE)<<8)|(EN300468_8859_11_3BYTE_3TH_VALUE))
/* 8859-12, reserved for indian */
#define		EN300468_8859_13_3BYTE_VALUE		(((EN300468_8859_3BTYE_FIRST_VALUE)<<16)|((EN300468_8859_3BTYE_2ND_VALUE)<<8)|(EN300468_8859_13_3BYTE_3TH_VALUE))
#define		EN300468_8859_14_3BYTE_VALUE		(((EN300468_8859_3BTYE_FIRST_VALUE)<<16)|((EN300468_8859_3BTYE_2ND_VALUE)<<8)|(EN300468_8859_14_3BYTE_3TH_VALUE))
#define		EN300468_8859_15_3BYTE_VALUE		(((EN300468_8859_3BTYE_FIRST_VALUE)<<16)|((EN300468_8859_3BTYE_2ND_VALUE)<<8)|(EN300468_8859_15_3BYTE_3TH_VALUE))

#define		CH_1BYTE_NULL_CHARACTER				(CH_1BYTE)0x00
#define		CH_1BYTE_WHITE_SPACE_START_VALUE	(CH_1BYTE)0x01
#define		CH_1BYTE_WHITE_SPACE_END_VALUE		(CH_1BYTE)0x1F
#define		CH_1BYTE_EFFECTIVE_CH_START_VALUE	(CH_1BYTE)0x20
#define		CH_1BYTE_EFFECTIVE_CH_END_VALUE		(CH_1BYTE)0xFF
#define		CH_1BYTE_ASCII_END_VALUE			(CH_1BYTE)0x7E
#define		CH_1BYTE_USERDEFINE_CODE_START_VALUE	(CH_1BYTE)0x7F
#define		CH_1BYTE_USERDEFINE_CODE_END_VALUE	(CH_1BYTE)0x9F
#define		CH_1BYTE_6937_COMBINE_BYTE_VALUE	(CH_1BYTE)0xC0
#define		CH_1BYTE_CAPITAL_LETTER_A_VALUE		(CH_1BYTE)0x41
#define		CH_1BYTE_CAPITAL_LETTER_Z_VALUE		(CH_1BYTE)0x5A
#define		CH_1BYTE_SMALL_LETTER_A_VALUE		(CH_1BYTE)0x61
#define		CH_1BYTE_SMALL_LETTER_Z_VALUE		(CH_1BYTE)0x7A
#define		CH_1BYTE_USERDEFINE_CODE_IGNORE_CH	(CH_1BYTE)0x9F

#define		CH_UCS2_NULL_CHARACTER				(CH_UCS2)0x0000
#define		CH_UCS2_EFFECTIVE_CH_START_VALUE	(CH_UCS2)0x0020
#define		CH_UCS2_ASCII_END_VALUE			(CH_UCS2)0x007E
#define		CH_UCS2_USERDEFINE_CODE_START_VALUE	(CH_UCS2)0x007F
#define		CH_UCS2_USERDEFINE_CODE_END_VALUE	(CH_UCS2)0x009F

#define		CH_UCS2_USERDEFINE_CODE_IGNORE_CH	(CH_UCS2)0x009F

#define		CH_UCS4_NULL_CHARACTER				(CH_UCS4)0x00000000



/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
#if defined(CONFIG_PROD_LANG_ARABIC)
void	MWC_UTIL_RegisterCurrentArabicPersian(HINT32 langID);
#endif

/**
  @brief     MWC_UTIL_ConvertCharUpper

  Function Name:	make input letter to Upper letter
  Function Description:
  Parameter:
  type: (Sync or Async)
  Return Value:
*/
extern void		MWC_UTIL_ConvertCharUpper(HUINT8 *pStr);

/**
  @brief     MWC_UTIL_ConvertCharLower

  Function Name:	make input letter to Lower letter
  Function Description:
  Parameter:
  type: (Sync or Async)
  Return Value:
*/
extern void		MWC_UTIL_ConvertCharLower(HUINT8 *pStr);

/**
  @brief     MWC_UTIL_ConvertStringUpper

  Function Name:
  Function Description:
  Parameter:
  type: (Sync or Async)
  Return Value:
*/
void 		MWC_UTIL_ConvertStringUpper(
							HUINT8 *string			/**< */
							);

/**
	@brief  : convert input alphabet string to lower case
*/
void 		MWC_UTIL_ConvertStringLower(HUINT8 *string);


/**
  @brief     MWC_UTIL_EliminateSoftHypenCharForString

  Function Name:
  Function Description:
  Parameter:
  type: (Sync or Async)
  Return Value:
*/

void 		MWC_UTIL_EliminateSoftHypenCharForString(
							HUINT8 *pucMem
							);

/**
  @brief     MWC_UTIL_BeInString

  Function Name:
  Function Description:
  Parameter:
  type: (Sync or Async)
  Return Value:
*/
HBOOL		MWC_UTIL_BeInString(
							HINT8 *szObj, 			/**< */
							HINT8 *szString			/**< */
							);

/**
  @brief     MWC_UTIL_BeInStringn

  Function Name:
  Function Description:
  Parameter:
  type: (Sync or Async)
  Return Value:
*/
HBOOL		MWC_UTIL_BeInStringn(
							HINT8 *szObj, 			/**< */
							HINT32 size,				/**< */
							HINT8 *szString			/**< */
							);

/**
  @brief     MWC_UTIL_IsIso639Language

  Function Name:
  Function Description:
  Parameter:
  type: (Sync or Async)
  Return Value:
*/
HBOOL		MWC_UTIL_IsIso639Language(HUINT8 *pcInCode);


/**
  @brief     MWC_UTIL_SelectIso639LanguageEncoding

  Function Name:
  Function Description:
  Parameter:
  type: (Sync or Async)
  Return Value:
*/
HUINT32		MWC_UTIL_SelectIso639LanguageEncoding(
							HUINT8 *pcInCode, 		/**< */
							HUINT8 *pcOutFont, 		/**< */
							HUINT32 *punLen			/**< */
							);

/**
  @brief     MWC_UTIL_DvbStrcpy

  Function Name:
  Function Description:
  Parameter:
  type: (Sync or Async)
  Return Value:
*/
HUINT8		*MWC_UTIL_DvbStrcpy(
							HUINT8 *pcDst, 			/**< */
							const HUINT8 *pcSrc			/**< */
							);

/**
  @brief     MWC_UTIL_DvbStrcpyStrOnly

  Function Name:
  Function Description:
  Parameter:
  type: (Sync or Async)
  Return Value:
*/
HUINT8		*MWC_UTIL_DvbStrcpyStrOnly(
							HUINT8 *pcDst, 			/**< */
							const HUINT8 *pcSrc			/**< */
							);

/**
  @brief     MWC_UTIL_DvbStrncpy

  Function Name:
  Function Description:
  Parameter:
  type: (Sync or Async)
  Return Value:
*/


HUINT8		*UTIL_DvbStrcpyStrOnlySelf(HUINT8 *pcSrc);

HUINT8		*MWC_UTIL_DvbStrncpy(
							HUINT8 *pcDst, 			/**< */
							const HUINT8 *pcSrc, 			/**< */
							HUINT32 unLen			/**< */
							);

/**
  @brief     MWC_UTIL_DvbStrncpyStrOnly

  Function Name:
  Function Description:
  Parameter:
  type: (Sync or Async)
  Return Value:
*/
HUINT8		*MWC_UTIL_DvbStrncpyStrOnly(
							HUINT8 *pcDst, 			/**< */
							const HUINT8 *pcSrc, 			/**< */
							HUINT32 unLen			/**< */
							);

/**
  @brief     MWC_UTIL_DvbStrlen

  Function Name:
  Function Description:
  Parameter:
  type: (Sync or Async)
  Return Value:
*/
HUINT32		MWC_UTIL_DvbStrlen(
							const HUINT8 *pcStr			/**< */
							);

/**
  @brief     MWC_UTIL_DvbStrlenStrOnly

  Function Name:
  Function Description:
  Parameter:
  type: (Sync or Async)
  Return Value:
*/
HUINT32		MWC_UTIL_DvbStrlenStrOnly(
							const HUINT8 *pcStr			/**< */
							);

/**
  @brief     MWC_UTIL_DvbStrcat

  Function Name:
  Function Description:
  Parameter:
  type: (Sync or Async)
  Return Value:
*/
HUINT8		*MWC_UTIL_DvbStrcat(
							HUINT8 *pcDst, 			/**< */
							const HUINT8 *pcSrc			/**< */
							);

/**
  @brief     MWC_UTIL_DvbStrncat

  Function Name:
  Function Description:
  Parameter:
  type: (Sync or Async)
  Return Value:
*/
HUINT8		*MWC_UTIL_DvbStrncat(
							HUINT8 *pcDst, 			/**< */
							const HUINT8 *pcSrc, 			/**< */
							HUINT32 unLen			/**< */
							);

/**
  @brief     MWC_UTIL_DvbStrcatLang

  Function Name:
  Function Description:
  Parameter:
  type: (Sync or Async)
  Return Value:
*/
HUINT8		*MWC_UTIL_DvbStrcatLang(
							HUINT8 *pcDst, 			/**< */
							const HUINT8 *pcSrc			/**< */
							);

/**
  @brief     MWC_UTIL_DvbStrcmp

  Function Name:
  Function Description:
  Parameter:
  type: (Sync or Async)
  Return Value:
*/
HINT32		MWC_UTIL_DvbStrcmp(
							const HUINT8 *pcStr1, 			/**< */
							const HUINT8 *pcStr2			/**< */
							);

/**
  @brief     MWC_UTIL_DvbStrncmp

  Function Name:
  Function Description:
  Parameter:
  type: (Sync or Async)
  Return Value:
*/
HINT32		MWC_UTIL_DvbStrncmp(
							const HUINT8 *pcStr1, 			/**< */
							const HUINT8 *pcStr2, 			/**< */
							HUINT32 unLen			/**< */
							);

HUINT32		MWC_UTIL_UCS2Strlen(const HUINT16 *pusStr);
CH_UCS2		*MWC_UTIL_UCS2Strcat(CH_UCS2 *pcDst, const CH_UCS2 *pcSrc);
CH_UCS2		*MWC_UTIL_UCS2Strncat(CH_UCS2 *pcDst, const CH_UCS2 *pcSrc, HUINT32 unLen);

/**
  @brief     MWC_UTIL_GetLocaleCodeLen

  Function Name:
  Function Description:
  Parameter:
  type: (Sync or Async)
  Return Value:
*/
HUINT32		MWC_UTIL_GetLocaleCodeLen(const HUINT8 *pcStr);


HUINT32		MWC_UTIL_GetLocaleCode(const HUINT8 *pcStr, HUINT8 *pLocaleCode);

eHxCP_CodePage_e		MWC_UTIL_GetEncodingByLocaleCode(const HUINT8 *pcStr);


/**
  @brief     MWC_UTIL_ChangeIso639toLowercase

  Function Name:
  Function Description:
  Parameter:
  type: (Sync or Async)
  Return Value:
*/
HUINT32		MWC_UTIL_ChangeIso639toLowercase(
							HUINT8 *pcStr			/**< */
							);


/**
  @brief     MWC_UTIL_DvbGetStringOnly

  Function Name:
  Function Description:
  Parameter:
  type: (Sync or Async)
  Return Value:
*/
HUINT8*		MWC_UTIL_DvbGetStringOnly(
							HUINT8* pcStr			/**< */
							);

extern HUINT8	*MWC_UTIL_DvbUtf8Sprintf(HUINT8 *pStr, const HUINT8 *format, ...);

extern ConvEncoding_Err_t	MWC_UTIL_ConvertSingleByteToUtf8(
							const CH_1BYTE *pStrSingle,
							const HINT32 nStrSingleLen,
							const HINT32 nBufferLen,
							HINT32 *pnUsedByte,
							CH_UTF8 *pUtf8Str,
							HINT32 *pnUtf8StrLen);

extern ConvEncoding_Err_t	MWC_UTIL_ConvertUCS2ToUtf8(
							const CH_UCS2 *pStrUcs2,
							const HINT32 nStrUcs2Len,
							const HINT32 nBufferLen,
							HINT32 *pnUsedByte,
							CH_UTF8 *pUtf8Str,
							HINT32 *pnUtf8StrLen);

extern ConvEncoding_Err_t	MWC_UTIL_ConvertUCS4ToUtf8(
							const CH_UCS4 *pStrUcs4,
							const HINT32 nStrUcs4Len,
							const HINT32 nBufferLen,
							HINT32 *pnUsedByte,
							CH_UTF8 *pUtf8Str,
							HINT32 *pnUtf8StrLen);

extern ConvEncoding_Err_t	MWC_UTIL_ConvertUtf8ToSingleByte(
							const CH_UTF8 *pStrUtf8,
							const HINT32 nStrUtf8Len,
							HINT32 *pnUsedByte,
							CH_1BYTE *pSingleByteStr,
							HINT32 *pnSingleByteStrLen);

#if defined(CONFIG_PROD_LANG_ARABIC)
extern HBOOL				MWC_UTIL_CheckSpecialCharacterInString(
							CH_UTF8 *pStr);

extern HBOOL				MWC_UTIL_CheckUnicodeArabicCharacterInString(
							CH_UCS2 *pStr);

extern HBOOL 				MWC_UTIL_IsDualJoiningArabicChar(
							CH_UCS2 oneChar);

extern HBOOL 				MWC_UTIL_IsRightJoiningArabicChar(
							CH_UCS2 oneChar);

extern ConvEncoding_Err_t	MWC_UTIL_ConvertLogicalAraToVisualAra(
							CH_UCS2 *pLogStr,
							HINT32 nUnicodeStrLen,
							CH_UCS2 *pVisStr,
							HINT32 nBufSize,
							HINT32 *nVisualOrderStrLen);
#endif



extern ConvEncoding_Err_t	MWC_UTIL_ConvertEncodingToUCS2(
							const eHxCP_CodePage_e eEncoding,
							const CH_1BYTE *pSrcStr,
							const HINT32 nSrcStrLen,
							HINT32 *pnUsedByte,
							CH_UCS2 *pUcs2Str,
							HINT32 *pnUcs2StrLen);

extern ConvEncoding_Err_t	MWC_UTIL_ConvertEncodingToUCS4(
							const eHxCP_CodePage_e eEncoding,
							const CH_1BYTE *pSrcStr,
							const HINT32 nSrcStrLen,
							HINT32 *pnUsedByte,
							CH_UCS4 *pUcs4Str,
							HINT32 *pnUcs4StrLen);


extern HERROR		MWC_UTIL_GetISO639IdxBy639Code(HUINT8 *p639Code, eISO639Idx_t *pLangIdx);
extern HERROR		MWC_UTIL_Get639CodeByISO639Idx(const eISO639Idx_t eLangIdx, HUINT8 *pStr);
extern HERROR		MWC_UTIL_Get639CodeByLangIdx(const HxLANG_Id_e eLangIdx, HINT32 *plMatchCount, HUINT8 *pStr1, HUINT8 *pStr2, HUINT8 *pStr3);
extern HERROR		MWC_UTIL_GetLangFullNameByISO639Idx(const eISO639Idx_t eISO639Idx, HUINT8 *pFullnameStr);
extern HERROR		MWC_UTIL_GetLangFullNameByLangIdx(const HxLANG_Id_e eLangIdx, HUINT8 *pFullnameStr);
extern HERROR		MWC_UTIL_GetLangFullNameBy639Code(HUINT8 *p639Code, HUINT8 *pFullnameStr);
extern HERROR		MWC_UTIL_GetLangIDBy639Idx(const eISO639Idx_t e639Idx, HxLANG_Id_e *peLangId);
extern HERROR		MWC_UTIL_GetLangIDBy639Code(HUINT8 *p639Code, HxLANG_Id_e *peLangId);
extern HERROR		MWC_UTIL_GetLangIDBy639_1Code(HUINT8 *p639_1Code, HxLANG_Id_e *peLangId);
extern HERROR		MWC_UTIL_GetLangFullNameBy639_1Code(HUINT8 *p639_1Code, HUINT8 *pFullnameStr);

extern HERROR MWC_UTIL_DvbStrGetSHYValue(HUINT8 *pucMem, HUINT32 ucMemLen, HUINT32 *pCharCodeTableLen, HUINT32 *pShyValue);
extern HUINT32 MWC_UTIL_EliminateShyAndControlCharacter(HUINT8 *string, HUINT32 strLen);
extern HUINT32 MWC_UTIL_ReplaceDvbCRLF(HUINT8 *string, HUINT32 strLen);

#if 0
/**
  @remark   get iso 3166 code(which is a country code) matched input parameter 'eCountryIndex'
  @remark   type : Inline
  @input	country index 'Country_Idx_t', it is defined in stb_params.h
  @output	3 bytes iso 3166 code. 'szIso3166Code' is a buffer to get
  @return   ERR_OK   : found code
  @return   ERR_FAIL : unknown country index
*/
HERROR MWC_UTIL_GetIso3166CodeByCountryIndex(Country_Idx_t eCountryIndex, HUINT8 *szIso3166Code);
/**
  @remark   get country index matched input parameter iso 3166 code
  @remark   type : Inline
  @input	3 bytes iso 3166 code. 'szIso3166Code' is a buffer to get
  @output	country index 'Country_Idx_t', it is defined in stb_params.h
  @return   ERR_OK   : found country index
  @return   ERR_FAIL : can't find matched country index
*/
HERROR MWC_UTIL_GetCountryIndexByIso3166Code(HUINT8 *szIso3166Code, Country_Idx_t *peCountryIndex);
#endif

/**
  @remark   get iso 3166 code(which is a country code) matched input parameter 'eCountryIndex'
  @remark   type : Inline
  @input	country id 'HxCountry_e', it is defined in _hlib_country.h
  @output	3 bytes iso 3166 code. 'szIso3166Code' is a buffer to get
  @return   ERR_OK   : found code
  @return   ERR_FAIL : unknown country index
*/
HERROR MWC_UTIL_GetIso3166CodeByCountryId(HxCountry_e eCountryId, HUINT8 *szIso3166Code);
/**
  @remark   get country index matched input parameter iso 3166 code
  @remark   type : Inline
  @input	3 bytes iso 3166 code. 'szIso3166Code' is a buffer to get
  @output	country id 'HxCountry_e', it is defined in _hlib_country.h
  @return   ERR_OK   : found country index
  @return   ERR_FAIL : can't find matched country index
*/
HERROR MWC_UTIL_GetCountryIdByIso3166Code(HUINT8 *szIso3166Code, HxCountry_e *peCountryId);


#if 0
HERROR MWC_UTIL_CountDrawableCharNumInString (eHxCP_CodePage_e eEncoding, HUINT8 *szString, HUINT32 *pulCharNum);

HERROR MWC_UTIL_GetDrawableCharInString (eHxCP_CodePage_e eEncoding, HUINT8 *szString, HUINT32 ulIndex,
													HUINT8 *szCharWithCode, HUINT32 *pulCharLenWithCode,
													HUINT8 *szCharOnly, HUINT32 *pulCharLenOnly);

#endif


HERROR MWC_UTIL_GetSiStringEncodedType(SiTxtEncodedType_t *peEncType);
HERROR MWC_UTIL_SetSiStringEncodedType(SiTxtEncodedType_t eEncType);
HUINT32 MWC_UTIL_ConvSiStringToUtf8 (HUINT8 *szLangCode, HUINT32 ulSrcStrLen, HUINT8 *szSrcStr, HUINT8 **pszDstStr);

HERROR MWC_UTIL_GetEmphasizedStringFrom300468String (HUINT8 *szSrcStr, HUINT8 *szDstStr);

HUINT32 MWC_UTIL_LTrimString(HUINT8 *src, HINT32 length, HUINT8 *dest);
HUINT32 MWC_UTIL_RTrimString(HUINT8 *src, HINT32 length, HUINT8 *dest);
HUINT32 MWC_UTIL_BothTrimString( HUINT8 *src, HINT32 length, HUINT8 *dest);

void 	*MWC_UTIL_CodePage_Get8859toUCS2Table(HUINT32 ulIdx);
HBOOL		MWC_UTIL_IsEncodingValid(eHxCP_CodePage_e eEncoding, const CH_1BYTE *pInStr);


#endif /* !_ISOSVC_H_ */

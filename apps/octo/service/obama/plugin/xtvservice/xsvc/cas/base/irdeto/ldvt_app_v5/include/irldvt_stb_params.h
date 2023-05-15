/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  stb_params.h
	@brief	  octo system wide enumeration definition

	Description: commonly used enum values on ap, mw, pal layers of octo are defined

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__STB_PARAMS_H__
#define	__STB_PARAMS_H__

#include <dlib.h>

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

#define HANDLE_NULL		0xFFFFFFFF
#define ANT_IDX_NULL	0xFFFF

#define TICK_PER_SEC	1000					// OCTO 에서 VK로 얻어오는 Tick 은 무조건 1000 tick /sec 이다.

// Driver와 Application의 시나리오가 어떻던 OCTO의 기준 Volume MAX값은 100이다.
#define OCTO_SYSTEM_MAX_VOL_LEVEL		100

/*
	Timezone String from SI
*/
#define	MAX_TIMEZONE_STR_LEN			128

#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define ____from_service_db____
#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx


/*
  *	Recording 파일의 Packet Size를 Nexus PB를 사용할 경우 192, pure TS일 경우 188로 사용.
  */
#define PACKET_SIZE_PVR		192
#define PACKET_SIZE_TS			188


/** @brief [service db 속성 - SatType] */

// SI에서 Sat type이 필요할 수 잇다.
typedef enum tagSATELLITE_ID_ENUM
{ /* [WARN] forbid to change a db-saving enum value, [WARN] 절대 ifdef로 추가 하지 말것. 보이면 바로 지울 것임 */
	eSatType_ALL 				= 0,

	//	UX TP List기준으로 Satellite Type 재정의. (satTpData.json과 같은 데이타.)
	eSatType_AMOS_2_3			= 1,
	eSatType_APSTAR_2R			= 2,
	eSatType_ARABSAT_5A			= 3,
	eSatType_ASIASAT_3S			= 4,
	eSatType_ASIASAT_5			= 5,
	eSatType_ASTRA_1			= 6,
	eSatType_ASTRA_2			= 7,
	eSatType_ASTRA_3A_3B		= 8,
	eSatType_ASTRA_4A			= 9,
	eSatType_ATLANTIC_BIRD_1	= 10,
	eSatType_ATLANTIC_BIRD_2	= 11,
	eSatType_ATLANTIC_BIRD_3	= 12,
	eSatType_BADR_4_6			= 13,
	eSatType_EUROBIRD_1			= 14,
	eSatType_EUROBIRD_3			= 15,
	eSatType_EUROBIRD_9A		= 16,
	eSatType_EUTELSAT_W2A		= 17,
	eSatType_EUTELSAT_W3A		= 18,
	eSatType_EUTELSAT_W4_W7		= 19,
	eSatType_EUTELSAT_W6		= 20,
	eSatType_EXPRESS_AM1		= 21,
	eSatType_EXPRESS_AM22		= 22,
	eSatType_HELLAS_SAT2		= 23,
	eSatType_HISPASAT_1C_1D		= 24,
	eSatType_HOTBIRD			= 25,
	eSatType_INSAT_3A_4B		= 26,
	eSatType_INTELSAT_10_02		= 27,
	eSatType_INTELSAT_12		= 28,
	eSatType_INTELSAT_7_10		= 29,
	eSatType_INTELSAT_901		= 30,
	eSatType_INTELSAT_902		= 31,
	eSatType_INTELSAT_904		= 32,
	eSatType_INTELSAT_906		= 33,
	eSatType_INTELSAT_907		= 34,
	eSatType_NILESAT_101_102	= 35,
	eSatType_NSS_12				= 36,
	eSatType_NSS_7				= 37,
	eSatType_TELSTAR_12			= 38,
	eSatType_THOR_5_6			= 39,
	eSatType_TURKSAT_2A_3A		= 40,
	eSatType_YAMAL_201			= 41,
	eSatType_ARABSAT_2B		= 42,
	eSatType_EUTELSAT_W1		= 43,
	eSatType_EUTELSAT_W2		= 44,
	eSatType_NSS_703			= 45,
	eSatType_SIRIUS				= 46,
	eSatType_TELECOM_2D		= 47,
	eSatType_EUROBIRD_9		= 48,
	eSatType_INTELSAT_802		= 49,
	eSatType_THOR_3_5			= 50,
	eSatType_INSAT_2E4A			= 51,
	eSatType_INTELSAT_17		= 52,
	eSatType_YAHLIVE			= 53,

	//	아래 부분은 지우고 재 정리 되어야 함.
	//	HDR-1000S Compile을 위해서만 사용할것임
	eSatType_AGILA_2,
	eSatType_AMOS_1,
	eSatType_ANIK_F1,
	eSatType_APSTAR_1,
	eSatType_APSTAR_1A,
	eSatType_ARABSAT_2A3A,
	eSatType_ASIASAT_2,
	eSatType_ASTRA_3,
	eSatType_ATLANTA_BIRD_1,
	eSatType_BRASILSAT_B1,
	eSatType_BRASILSAT_B2,
	eSatType_BRASILSAT_B3,
	eSatType_BRASILSAT_B4,
	eSatType_EURASIASAT_1,
	eSatType_EUTELSAT_II_F2,
	eSatType_EUTELSAT_W3,
	eSatType_EXPRESS_3A,
	eSatType_EXPRESS_6A,
	eSatType_GORIZONT_26,
	eSatType_GORIZONT_33,
	eSatType_HISPASAT,
	eSatType_INSAT_2DT,
	eSatType_INSAT_2E3B,
	eSatType_INSAT_3C,
	eSatType_INTELSAT_605,
	eSatType_INTELSAT_704,
	eSatType_INTELSAT_706,
	eSatType_INTELSAT_707,
	eSatType_INTELSAT_709,
	eSatType_INTELSAT_805,
	eSatType_JCSAT_3,
	eSatType_KOREASAT_3,
	eSatType_LMI_1,
	eSatType_MEASAT_1,
	eSatType_MEASAT_2,
	eSatType_NAHUEL_1,
	eSatType_NILESAT,
	eSatType_NSS_803,
	eSatType_NSS_806,
	eSatType_OPTUS_B1,
	eSatType_OPTUS_B3,
	eSatType_PALAPA_C2,
	eSatType_PAS_1R,
	eSatType_PAS_2,
	eSatType_PAS_3R_6_6B,
	eSatType_PAS_7_10,
	eSatType_PAS_8,
	eSatType_PAS_9,
	eSatType_SATMEX_5,
	eSatType_SINOSAT_1,
	eSatType_ST_1,
	eSatType_TELECOM_2A,
	eSatType_TELECOM_2C,
	eSatType_TELKOM_1,
	eSatType_THAICOM_1A,
	eSatType_THAICOM_2_3,
	eSatType_THOR,
	eSatType_TURKSAT_1B,
	eSatType_TURKSAT_1C,
	eSatType_YAMAL_102,
	eSatType_AMC_1,
	eSatType_AMC_3,
	eSatType_AMC_4,
	eSatType_AMC_7,
	eSatType_ANIK_E2,
	eSatType_GALAXY_10R,
	eSatType_GALAXY_11,
	eSatType_GALAXY_1R,
	eSatType_GALAXY_3C,
	eSatType_GALAXY_4R,
	eSatType_GALAXY_9,
	eSatType_SATCOM_C3,
	eSatType_SATCOM_C4,
	eSatType_SOLIDARIDAD_2,
	eSatType_TELSTAR_4,
	eSatType_TELSTAR_5,
	eSatType_TELSTAR_7,
	eSatType_EUTELSAT_W4,
	eSatType_ATLANTA_BIRD_2,
	eSatType_ATLANTA_BIRD_3,
	eSatType_TURKSAT_2A,
	eSatType_VINASAT_1,

#if	0
	eSatType_AGILA_2		 	= 1,		/*AGILA2 at 146.0E*/
	eSatType_AMOS_1			= 2,		/*AMOS1 at 4.0W*/
	eSatType_ANIK_F1			= 3,		/*ANIK F1 at 107.3W*/
	eSatType_APSTAR_1		= 4,		/*APSTAR 2R at 76.5E*/
	eSatType_APSTAR_1A		= 5,		/*APSTAR 2R at 76.5E*/
	eSatType_APSTAR_2R		= 6,		/*APSTAR 2R at 76.5E*/
	eSatType_ARABSAT_2A3A	= 7,		/*ARABSAT 2A-3A at 26.0E*/
	eSatType_ARABSAT_2B		= 8,		/*ARABSAT 2B at 30.5E*/
	eSatType_ASIASAT_2		= 9,		/*ASIASAT 2 at 100.5E*/
	eSatType_ASIASAT_3S		= 10,		/*ASIASAT 3S at 105.5E*/
	eSatType_ASTRA_1			= 11,		/*ASTRA 1B-1H & 2C at 19.2E*/
	eSatType_ASTRA_2			= 12,		/*Eurobird1 & ASTRA 2A-2B-2D at 28.2E*/
	eSatType_ASTRA_3			= 13,		/* Astra 3A at 23.5°E */
	eSatType_ATLANTA_BIRD_1	= 14,		/*Atlantic Bird 1 at 12.5W*/
	eSatType_BRASILSAT_B1	= 15,		/*BrasilSat B1 at 70.0W*/
	eSatType_BRASILSAT_B2	= 16,		/*BrasilSat B2 at 65.0W*/
	eSatType_BRASILSAT_B3	= 17,		/*BrasilSat B3 at 84.0W*/
	eSatType_BRASILSAT_B4	= 18,
	eSatType_EURASIASAT_1	= 19,		/*TurkSat 1C & EurasiaSat 1 at 42.0E*/
	eSatType_EUTELSAT_II_F2	= 20,		/*EutelSat II F2 at 48.0E*/
	eSatType_EUTELSAT_W1		= 21,		/*Eutelsat W1 at 10.0E*/
	eSatType_EUTELSAT_W2		= 22,		/*Eutelsat W2 at 16.0E*/
	eSatType_EUTELSAT_W3		= 23,		/*Eutelsat W3 at 7.0E*/
	eSatType_EXPRESS_3A		= 24,		/*Express 3A at 11.0W*/
	eSatType_EXPRESS_6A		= 25,		/*Express 6A at 80.0E*/
	eSatType_GORIZONT_26		= 26,
	eSatType_GORIZONT_33		= 27,		/*Gorizont 33 at 145.0E*/
	eSatType_HISPASAT		= 28,		/*Hispasat 1A-1D at 30.0W*/
	eSatType_HOTBIRD			= 29,		/*Hot Bird 1-2-3-4-6 at 13.0E*/
	eSatType_INSAT_2DT		= 30,
	eSatType_INSAT_2E3B		= 31,		/*Insat 2E-3B at 83.0E*/
	eSatType_INSAT_3C		= 32,		/*Insat 2E-3B at 83.0E*/
	eSatType_INTELSAT_605	= 33,		/*IntelSat 605 at 27.5W*/
	eSatType_INTELSAT_704	= 34,		/*IntelSat 704 at 66.0E*/
	eSatType_INTELSAT_706	= 35,
	eSatType_INTELSAT_707	= 36,		/*IntelSat 707 & Thor 2-3 at 1.0W*/
	eSatType_INTELSAT_709	= 37,
	eSatType_INTELSAT_805	= 38,		/*IntelSat 805 at 55.5W*/
	eSatType_INTELSAT_901	= 39,		/*IntelSat 901 at 18.0W*/
	eSatType_INTELSAT_902	= 40,		/*IntelSat 902 at 62.0E*/
	eSatType_INTELSAT_904	= 41,		/*IntelSat 904 at 60.0E*/
	eSatType_INTELSAT_906	= 42,		/*IntelSat 906 at 64.0E*/
	eSatType_JCSAT_3			= 43,		/*JCSAT 3 at 128.0E*/
	eSatType_KOREASAT_3		= 44,		/*KoreaSat 3 at 116.0E*/
	eSatType_LMI_1			= 45,		/*LMI 1 at 75.0E*/
	eSatType_MEASAT_1		= 46,		/*Measat 1 at 91.5E*/
	eSatType_MEASAT_2		= 47,		/*Measat 2 at 148.0E*/
	eSatType_NAHUEL_1		= 48,		/*Nahuel 1 at 72.0W*/
	eSatType_NILESAT			= 49,		/*Nilesat 101-102 at 7.0W*/
	eSatType_NSS_703			= 50,		/*NSS 703 at 57.0E*/
	eSatType_NSS_803			= 51,
	eSatType_NSS_806			= 52,		/*NSS 806 at 40.5W*/
	eSatType_OPTUS_B1		= 53,		/*Optus B1 at 160.0E*/
	eSatType_OPTUS_B3		= 54,		/*Optus B3 at 156.0E*/
	eSatType_PALAPA_C2		= 55,		/*Palapa C2 at 113.0E*/
	eSatType_PAS_1R			= 56,		/*PAS 1R at 45.0W*/
	eSatType_PAS_2			= 57,		/*PAS 2 at 169.0E*/
	eSatType_PAS_3R_6_6B		= 58,		/*PAS 3R-6-6B at 43.0W*/
	eSatType_PAS_7_10		= 59,		/*PAS 7-10 at 68.5E*/
	eSatType_PAS_8			= 60,		/*PAS 8 at 166.0E*/
	eSatType_PAS_9			= 61,		/*PAS 9 at 58.0W*/
	eSatType_SATMEX_5		= 62,		/*SatMex 5 at 116.8W*/
	eSatType_SINOSAT_1		= 63,		/*SINOSAT 1 at 110.5E*/
	eSatType_SIRIUS			= 64,		/*ASTRA 1A & SIRIUS 2-3 at 5.0E*/
	eSatType_ST_1			= 65,		/*ST 1 at 88.0E*/
	eSatType_TELECOM_2A		= 66,		/*Telecom 2A at 3.0E*/
	eSatType_TELECOM_2C		= 67,		/*Telecom 2C & Atlantic Bird 3 at 5.0W*/
	eSatType_TELECOM_2D		= 68,		/*Telecom 2D & Atlantic Bird 2 at 8.0W*/
	eSatType_TELKOM_1		= 69,		/*Telcom1 & AAP 1 at 108.0E*/
	eSatType_TELSTAR_12		= 70,		/*TelStar 12 at 15.0W*/
	eSatType_THAICOM_1A		= 71,		/*Thaicom 1A at 120.0E*/
	eSatType_THAICOM_2_3		= 72,		/*Thaicom 2-3 at 78.5E*/
	eSatType_THOR			= 73,		/*the same TP Lists as eSatType_INTELSAT_707*/
	eSatType_TURKSAT_1B		= 74,		/*Turksat 1B at 31.3E*/
	eSatType_TURKSAT_1C		= 75,		/*Turksat 1C & Eurasiasat 1 at 42.0E*/
	eSatType_YAMAL_102		= 76,		/*Yamal 102 at 89.8E*/
	eSatType_AMC_1			= 77,		/*AMC1 at 103.0W*/
	eSatType_AMC_3			= 78,		/*AMC3 at 87.0W*/
	eSatType_AMC_4			= 79,		/*AMC4 at 101.0W*/
	eSatType_AMC_7			= 80,		/*AMC7 at 137.0W*/
	eSatType_ANIK_E2			= 81,		/*ANIK E2 at 111.1W*/
	eSatType_GALAXY_10R		= 82,		/*Galaxy 10R at 123.0W*/
	eSatType_GALAXY_11		= 83,		/*Galaxy 11 at 91.0W*/
	eSatType_GALAXY_1R		= 84,		/*Galaxy 1R at 133.0W*/
	eSatType_GALAXY_3C		= 85,		/*Galaxy 3C at 95.0W*/
	eSatType_GALAXY_4R		= 86,		/*Galaxy 4R at 99.0W*/
	eSatType_GALAXY_9		= 87,		/*Galaxy 9 at 127.0W*/
	eSatType_SATCOM_C3		= 88,		/*SatCom C3 at 131.0W*/
	eSatType_SATCOM_C4		= 89,		/*SatCom C4 at 135.0W*/
	eSatType_SOLIDARIDAD_2	= 90,		/*Solidaridad 2 at 113.0W*/
	eSatType_TELSTAR_4		= 91,		/*TelStar 4 at 89.0W*/
	eSatType_TELSTAR_5		= 92,		/*TelStar 5 at 97.0W*/
	eSatType_TELSTAR_7		= 93,		/*TelStar 7 at 129.0W*/
	eSatType_EUROBIRD_1		= 94,		/*Eurobird 1 at 28.2E*/
	eSatType_EUROBIRD_3		= 95, 		/*Eurobird 3 at 33.0E*/
	eSatType_EUROBIRD_9		= 96, 		/*Eurobird 9 at 9.0E*/
	eSatType_EUTELSAT_W4		= 97,		/*Eutelsat SESAT&W4 at 36.0E*/
	eSatType_EUTELSAT_W6		= 98,		/*Eutelsat W6 at 21.6E*/
	eSatType_EXPRESS_AM1		= 99,		/*Express AM1 at 40.0E*/
	eSatType_EXPRESS_AM22	= 100,		/*Express AM22 at 53.0E*/
	eSatType_HELLAS_SAT2		= 101,		/*Hellas Sat 2 at 39.0E*/
	eSatType_INTELSAT_10_02	= 102,		/*IntelSat 10-02 at 1.0W*/
	eSatType_INTELSAT_12		= 103,	 	/*IntelSat 12 at 45.0E*/
	eSatType_INTELSAT_7_10	= 104,	 	/*IntelSat 7&10 at 68.5E*/
	eSatType_INTELSAT_802	= 105,		/*IntelSat 802 at 33.0E*/
	eSatType_INTELSAT_907	= 106,		/*IntelSat 907 at 27.5W*/
	eSatType_NSS_7			= 107,		/*NSS 7 at 22.0W*/
	eSatType_ATLANTA_BIRD_2	= 108, 		/*Atlantic Bird 2 at 8.0W*/
	eSatType_ATLANTA_BIRD_3	= 109, 		/*Atlantic Bird 3 at 5.0W*/
	eSatType_THOR_3_5		= 110,		/*Thor 3&5 at 1.0W*/
	eSatType_TURKSAT_2A		= 111,		/*Turksat 2A&3A at 42.0E*/
	eSatType_ASIASAT_5 		= 112,		/*Asiasat 5 at 100.5E*/
	eSatType_VINASAT_1 		= 113,		/*Vinasat 1 at 132.0E*/
	eSatType_INSAT_2E4A		= 114,		/*Insat 2E/4A at 83.0E*/
	eSatType_INTELSAT_17		= 115,		/*IntelSat 17 at 66.0E*/
	eSatType_YAHLIVE		=116,		/* YahSat 1A	at 52.5E*/
#endif

	eSatType_LAST,
	MAX_NUM_DEFAULT_ANTENNAS	= (eSatType_LAST - 1),		// Last Item

	eSatType_USERANT_1 				= 0xF8,
	eSatType_USERANT_2 				= 0xF9,
	eSatType_USERANT_3 				= 0xFA,
	eSatType_USERANT_4				= 0xFB,
	eSatType_USERANT_5				= 0xFC,
	eSatType_USERANT_MAX				= 0xFE,
	MAX_NUM_USER_ANTENNAS		= (eSatType_USERANT_MAX - eSatType_USERANT_1 + 1),

	eSatType_NONE					= 0xFF
} SVC_SatType_t;

#if 0
typedef enum
{ /* [WARN] forbid to change a db-saving enum value, [WARN] 절대 ifdef로 추가 하지 말것. 보이면 바로 지울 것임 */
	eLangID_Albanian		= 0,			/* Albanian, BIBLIOGRAPHIC_WORD, an Indo-European language spoken by nearly 6 million people,[1] primarily in Albania and Serbia (Kosovo) but also in other areas of the Balkans in which there is an Albanian population, including the west of Republic of Macedonia, Montenegro, and southern Serbia. */
	eLangID_Arabic			= 1,			/* Arabic, Official language of 25 countries */
	eLangID_Australian		= 2,			/* Australian, Commonwealth of Australia */
	eLangID_Basaa			= 3,			/* Basaa, Republic of Cameroon, The Bantu languages (technically Narrow Bantu languages) constitute a grouping belonging to the Niger-Congo family. */
	eLangID_Basque			= 4,			/* Basque, the language spoken by the Basque people who inhabit the Pyrenees in North-Central Spain and the adjoining region of South-Western France. */
	eLangID_Bulgarian		= 5,			/* Bulgarian, */
	eLangID_Catalan			= 6,			/* Catalan, a Romance language, the national and official language of Andorra, and a co-official language in the Spanish autonomous communities of the Balearic Islands, Catalonia and Valencia?where it is known as Valencian? and in the city of Alghero in the Italian island of Sardinia.*/
	eLangID_Croatian		= 7,			/* Croatian, Republic of Croatia */
	eLangID_Czech			= 8,			/* Czech, BIBLIOGRAPHIC_WORD, Czech Republic */
	eLangID_Danish			= 9,			/* Danish, Kingdom of Denmark */
	eLangID_Dutch			= 10,			/* Dutch, BIBLIOGRAPHIC_WORD, Kingdom of the Netherlands */
	eLangID_English			= 11,			/* Englist, UK, Englasd, United Of America */
	eLangID_Estonian		= 12,			/* Estonian, Republic of Estonia */
	eLangID_Faroese			= 13,			/* Faroese, a West Nordic or West Scandinavian language spoken by 48,000 people in the Faroe Islands and about 12,000 Faroese in Denmark. */
	eLangID_Finnish			= 14,			/* Finnish, Republic of Finland */
	eLangID_French			= 15,			/* French, BIBLIOGRAPHIC_WORD, The French Republic, France */
	eLangID_Galician		= 16,			/* Galician, Gallegan, a language of the Western Ibero-Romance branch, spoken in Galicia, an autonomous community with the constitutional status of "historic nationality" located in northwestern Spain, as well as in small bordering zones in the neighbouring autonomous communities of Asturias and Castile and Leon. */
	eLangID_German			= 17,			/* German, BIBLIOGRAPHIC_WORD, Federal Republic of Germany */
	eLangID_Greek			= 18,			/* Greek, BIBLIOGRAPHIC_WORD, Hellenic Republic, Greece */
	eLangID_Hebrew			= 19,			/* Hebrew, */
	eLangID_Hindi			= 20,			/* Hindi, Republic of India */
	eLangID_Hungarian		= 21,			/* Hungarian, Republic of Hungary, Hungary */
	eLangID_Icelandic		= 22,			/* Icelandic, BIBLIOGRAPHIC_WORD, a North Germanic language, the language of Iceland. Its closest relative is Faroese and then West Norwegian dialects such as Sognamal. */
	eLangID_Irish			= 23,			/* Irish, a Goidelic language of the Indo-European language family, originating in Ireland and historically spoken by the Irish. */
	eLangID_Italian			= 24,			/* Italian, Italian Republic, Italy */
	eLangID_Japanese		= 25,			/* Japanese, Japan*/
	eLangID_Korean			= 26,			/* Korean, Republic of Korea */
	eLangID_Latvian			= 27,			/* Latvian, Republic of Latvia */
	eLangID_Lithuanian		= 28,			/* Lithuanian, Republic of Lithuania */
	eLangID_Norwegian		= 29,			/* Norwegian, Kingdom of Norway */
	eLangID_Persian			= 30,			/* Persian, Iran, Afghanistan, Tajikistan */
	eLangID_Polish			= 31,			/* Polish, Republic of Poland */
	eLangID_Portuguese		= 32,			/* Portuguese, Portuguese Republic */
	eLangID_Romanian		= 33,			/* Romanian, BIBLIOGRAPHIC_WORD, a Romance language spoken by around 24 to 28 million people,[1] primarily in Romania and Moldova. */
	eLangID_Russian			= 34,			/* Russian, Russian Federation */
	eLangID_Serbian			= 35,			/* Serbian, Republic of Serbia, */
	eLangID_Slovak			= 36,			/* Slovak, BIBLIOGRAPHIC_WORD, Slovak Republic */
	eLangID_Slovenian		= 37,			/* Slovenian, Republic of Slovenia */
	eLangID_Spanish			= 38,			/* Spanish, Kingdom of Spain */
	eLangID_Swedish			= 39,			/* Swedish, Kingdom of Sweden */
	eLangID_Turkish			= 40,			/* Turkish, Republic of Turkey */
	eLangID_Ukrainian		= 41,			/* Ukrainian, Ukraine */
	eLangID_Vietnamese		= 42,			/* Vietnamese, Socialist Republic of Vietnam */
	eLangID_Gaelic			= 43,
	eLangID_Welsh			= 44,
	eLangID_Thai			= 45,
	eLangID_OriginalAudio	= 46,
	eLangID_Undefined		= 47,			/* 639 : 'und' */
	eLangID_Narration		= 48,			/* UK Fsat 'nar' */

	eLangID_MAX,
	/* NOT real language */
	eLangID_NONE	= 0xff
} HxLANG_Id_e;
#endif

typedef enum {
	ISO639_DEFAULT_WORD		= 0,
	BIBLIOGRAPHIC_WORD		= 1,
	TERMINOLOGY_WORD		= 2,
	HUMAX_WORKAROUND_WORD	= 3,
	EN300468_SPECIFIC_WORD	= 4,
	UNDEFINED_WORD			= 5,
	LANG_639_WORD_MAX
} ISO639_wordMode_t;


/* alphabet order */
/*
	639 코드는 default, bibliographic, terminology string과
	HUMAX에서 각 OP와 방송사에서 work around로 얻은 스트링을 가질 수 있다.

	enum에
	숫자가 붙지 않은 것은 defualt: eLang_XXX,
	1이 붙은 것은 bibliographic: eLang_XXX_1,
	2가 붙은 것은 terminology: eLang_XXX_2,
	나머지 숫자는 work around로 얻은 순서대로: eLang_XXX_3 ~.
*/

typedef enum {
	eISO639_Albanian_1	= 0,	/* Albanian, BIBLIOGRAPHIC_WORD, an Indo-European language spoken by nearly 6 million people,[1] primarily in Albania and Serbia (Kosovo) but also in other areas of the Balkans in which there is an Albanian population, including the west of Republic of Macedonia, Montenegro, and southern Serbia. */
	eISO639_Albanian_2	= 1,	/* Albanian, TERMINOLOGY_WORD, an Indo-European language spoken by nearly 6 million people,[1] primarily in Albania and Serbia (Kosovo) but also in other areas of the Balkans in which there is an Albanian population, including the west of Republic of Macedonia, Montenegro, and southern Serbia. */
	eISO639_Arabic		= 2,	/* Arabic, Official language of 25 countries */
	eISO639_Australian	= 3,	/* Australian, Commonwealth of Australia */
	eISO639_Basaa		= 4,	/* Basaa, Republic of Cameroon, The Bantu languages (technically Narrow Bantu languages) constitute a grouping belonging to the Niger-Congo family. */
	eISO639_Basque		= 5,	/* Basque, the language spoken by the Basque people who inhabit the Pyrenees in North-Central Spain and the adjoining region of South-Western France. */
	eISO639_Bulgarian	= 6,	/* Bulgarian, */
	eISO639_Catalan		= 7,	/* Catalan, a Romance language, the national and official language of Andorra, and a co-official language in the Spanish autonomous communities of the Balearic Islands, Catalonia and Valencia?where it is known as Valencian? and in the city of Alghero in the Italian island of Sardinia.*/
	eISO639_Croatian	= 8,	/* Croatian, Republic of Croatia */
	eISO639_Czech_1		= 9,	/* Czech, BIBLIOGRAPHIC_WORD, Czech Republic */
	eISO639_Czech_2		= 10,	/* Czech, TERMINOLOGY_WORD, Czech Republic */
	eISO639_Danish		= 11,	/* Danish, Kingdom of Denmark */
	eISO639_Dutch_1		= 12,	/* Dutch, BIBLIOGRAPHIC_WORD, Kingdom of the Netherlands */
	eISO639_Dutch_2		= 13,	/* Dutch, TERMINOLOGY_WORD, Kingdom of the Netherlands */
	eISO639_Dutch_3		= 14,	/* Dutch, HUMAX_WORKAROUND_WORD dutch 01, Kingdom of the Netherlands */
	eISO639_English		= 15,	/* Englist, UK, Englasd, United Of America */
	eISO639_Estonian	= 16,	/* Estonian, Republic of Estonia */
	eISO639_Faroese		= 17,	/* Faroese, a West Nordic or West Scandinavian language spoken by 48,000 people in the Faroe Islands and about 12,000 Faroese in Denmark. */
	eISO639_Finnish		= 18,	/* Finnish, Republic of Finland */
	eISO639_French_1	= 19,	/* French, BIBLIOGRAPHIC_WORD, The French Republic, France */
	eISO639_French_2	= 20,	/* French, TERMINOLOGY_WORD, The French Republic, France */
	eISO639_Gaelic		= 21,	/* Gaelic, ISO639_DEFAULT_WORD, The Gaels are an ethno-linguistic group which originated in Ireland and subsequently spread to Scotland and the Isle of Man.  */
	eISO639_Galician	= 22,	/* Galician, Gallegan, a language of the Western Ibero-Romance branch, spoken in Galicia, an autonomous community with the constitutional status of "historic nationality" located in northwestern Spain, as well as in small bordering zones in the neighbouring autonomous communities of Asturias and Castile and Leon. */
	eISO639_German_1	= 23,	/* German, BIBLIOGRAPHIC_WORD, Federal Republic of Germany */
	eISO639_German_2	= 24,	/* German, TERMINOLOGY_WORD, Federal Republic of Germany */
	eISO639_Greek_1		= 25,	/* Greek, BIBLIOGRAPHIC_WORD, Hellenic Republic, Greece */
	eISO639_Greek_2		= 26,	/* Greek, TERMINOLOGY_WORD, Hellenic Republic, Greece */
	eISO639_Hebrew		= 27,	/* Hebrew, */
	eISO639_Hindi		= 28,	/* Hindi, Republic of India */
	eISO639_Hungarian	= 29,	/* Hungarian, Republic of Hungary, Hungary */
	eISO639_Icelandic_1	= 30,	/* Icelandic, BIBLIOGRAPHIC_WORD, a North Germanic language, the language of Iceland. Its closest relative is Faroese and then West Norwegian dialects such as Sognamal. */
	eISO639_Icelandic_2	= 31,	/* Icelandic, TERMINOLOGY_WORD, a North Germanic language, the language of Iceland. Its closest relative is Faroese and then West Norwegian dialects such as Sognamal. */
	eISO639_Irish		= 32,	/* Irish, a Goidelic language of the Indo-European language family, originating in Ireland and historically spoken by the Irish. */
	eISO639_Irish_3		= 33,	/* Irish, HUMAX_WORKAROUND_WORD 01, a Goidelic language of the Indo-European language family, originating in Ireland and historically spoken by the Irish. */
	eISO639_Irish_4		= 34,	/* Irish, HUMAX_WORKAROUND_WORD 02, a Goidelic language of the Indo-European language family, originating in Ireland and historically spoken by the Irish. */
	eISO639_Italian		= 35,	/* Italian, Italian Republic, Italy */
	eISO639_Japanese	= 36,	/* Japanese, Japan*/
	eISO639_Korean		= 37,	/* Korean, Republic of Korea */
	eISO639_Latvian		= 38,	/* Latvian, Republic of Latvia */
	eISO639_Lithuanian	= 39,	/* Lithuanian, Republic of Lithuania */
	eISO639_Norwegian	= 40,	/* Norwegian, Kingdom of Norway */
	eISO639_Persian_1	= 41,	/* Persian, BIBLIOGRAPHIC_WORD, Iran, Afghanistan, Tajikistan */
	eISO639_Persian_2	= 42,	/* Persian, TERMINOLOGY_WORD, Iran, Afghanistan, Tajikistan */
	eISO639_Polish		= 43,	/* Polish, Republic of Poland */
	eISO639_Portuguese	= 44,	/* Portuguese, Portuguese Republic */
	eISO639_Romanian_1	= 45,	/* Romanian, BIBLIOGRAPHIC_WORD, a Romance language spoken by around 24 to 28 million people,[1] primarily in Romania and Moldova. */
	eISO639_Romanian_2	= 46,	/* Romanian, TERMINOLOGY_WORD, a Romance language spoken by around 24 to 28 million people,[1] primarily in Romania and Moldova. */
	eISO639_Russian		= 47,	/* Russian, Russian Federation */
	eISO639_Serbian_1	= 48,	/* Serbian, BIBLIOGRAPHIC_WORD, Republic of Serbia, */
	eISO639_Serbian_2	= 49,	/* Serbian, TERMINOLOGY_WORD, Republic of Serbia, */
	eISO639_Slovak_1	= 50,	/* Slovak, BIBLIOGRAPHIC_WORD, Slovak Republic */
	eISO639_Slovak_2	= 51,	/* Slovak, TERMINOLOGY_WORD, Slovak Republic */
	eISO639_Slovenian	= 52,	/* Slovenian, Republic of Slovenia */
	eISO639_Spanish		= 53,	/* Spanish, Kingdom of Spain */
	eISO639_Spanish_3	= 54,	/* Spanish, HUMAX_WORKAROUND_WORD 01, Kingdom of Spain */
	eISO639_Spanish_4	= 55,	/* Spanish, HUMAX_WORKAROUND_WORD 02, Kingdom of Spain */
	eISO639_Swedish		= 56,	/* Swedish, Kingdom of Sweden */
	eISO639_Swedish_3	= 57,	/* Swedish, HUMAX_WORKAROUND_WORD 01, Kingdom of Sweden */
	eISO639_Turkish		= 58,	/* Turkish, Republic of Turkey */
	eISO639_Ukrainian	= 59,	/* Ukrainian, Ukraine */
	eISO639_Vietnamese	= 60,	/* Vietnamese, Socialist Republic of Vietnam */
	eISO639_Welsh_1		= 61,	/* Welsh, BIBLIOGRAPHIC_WORD, is a member of the Brythonic branch of Celtic spoken natively in Wales, in England by some along the Welsh border and in the Welsh immigrant colony in the Chubut Valley in Argentine Patagonia. */
	eISO639_Welsh_2		= 62,	/* Welsh, TERMINOLOGY_WORD, is a member of the Brythonic branch of Celtic spoken natively in Wales, in England by some along the Welsh border and in the Welsh immigrant colony in the Chubut Valley in Argentine Patagonia. */
	eISO639_Thai		= 63,	/* Thai, Thailand */
	/* Not Real */
	eISO639_300468		= 0xFC,	/* 300-468, Annex F, ISO 639 Language Descriptor for "original audio" Soundtrack */
	eISO639_Undefined	= 0xFD,
	eISO639_Humax_not_supported = 0xFE,
	eISO639_MAX			= 0xFF
} eISO639Idx_t;

#define	LANG_639_CODE_MAX		3


//이곳에 국가가 추가되면 isosvc.c에 MWC_UTIL_GetIso3166CodeByCountryIndex()에도 추가해 주세요
#if 0
/* alphabet order */
typedef enum
{ /* [WARN] forbid to change a db-saving enum value, [WARN] 절대 ifdef로 추가 하지 말것. 보이면 바로 지울 것임 */
	eCountry_AUSTRALIA		= 0, 	/* Commonwealth of Australia, English, de facto */
	eCountry_AUSTRIA		= 1, 	/* Republic of Austria, German, Slovene and Croatian locally */
	eCountry_CAMEROON		= 2, 	/* Republic of Cameroon, French, English */
	eCountry_CROATIA		= 3, 	/* Republic of Croatia, Croatian */
	eCountry_CZECH			= 4, 	/* Czech Republic, Czech */
	eCountry_DENMARK		= 5, 	/* Kingdom of Denmark, Danish */
	eCountry_ENGLAND		= 6, 	/* England, the United kingdom, English */
	eCountry_ESTONIA		= 7, 	/* Republic of Estonia, Estonian */
	eCountry_FINLAND		= 8, 	/* Republic of Finland, Finnish */
	eCountry_FRANCE			= 9, 	/* The French Republic, French */
	eCountry_GERMANY		= 10,	/* Federal Republic of Germany, German */
	eCountry_GREECE			= 11,	/* Hellenic Republic, Greek */
	eCountry_HUNGARY		= 12,	/* Republic of Hungary, Hungarian */
	eCountry_IRELAND		= 13,	/* Ireland, the United kingdom, English */
	eCountry_ITALY			= 14,	/* Italian Republic, Italian */
	eCountry_JAPAN			= 15,	/* Japanese, Japanese */
	eCountry_KOREA			= 16,	/* Republic of Korea, Korean */
	eCountry_LATVIA			= 17,	/* Republic of Latvia, Latvian */
	eCountry_LITHUANIA		= 18,	/* Republic of Lithuania, Lithuanian */
	eCountry_NETHERLANDS	= 19,	/* Kingdom of the Netherlands, Dutch */
	eCountry_NORWAY			= 20,	/* Kingdom of Norway, Norwegian */
	eCountry_POLAND			= 21,	/* Republic of Poland, Polish */
	eCountry_PORTUGAL		= 22,	/* Portuguese Republic, Portuguese */
	eCountry_RUSSIA			= 23,	/* Russian Federation, Russian */
	eCountry_SINGAPORE		= 24,	/* Republic of Singapore, English, Malay, Mandarin, Tamil*/
	eCountry_SLOVAKIA		= 25,	/* Slovak Republic, Slovak */
	eCountry_SLOVENIA		= 26,	/* Republic of Slovenia, Slovenian */
	eCountry_SPAIN			= 27,	/* Kingdom of Spain, Spanish */
	eCountry_SWEDEN			= 28,	/* Kingdom of Sweden, Swedish, de facto */
	eCountry_SWITZERLAND	= 29,	/* Swiss Confederation, German, French, Italian*/
	eCountry_TURKEY			= 30,	/* Republic of Turkey, Turkish */
	eCountry_UKRAINE		= 31,	/* Ukraine, Ukrainian */
	eCountry_USA			= 32,	/* United States of America, English, de facto */
	eCountry_VIETNAM		= 33,	/* Socialist Republic of Vietnam, Vietnamese */
	eCountry_BELGIUM		= 34,	/* Belgium */
	eCountry_LUXEMBOURG 	= 35,	/* Luxembourg */
	eCountry_ROMANIA		= 36,	/* Romania */
	eCountry_THAILAND		= 37,	/* Thailand */
	eCountry_UAE			= 38,	/* UAE(Dubai, Abu Dhabi) */
	eCountry_BAHRAIN		= 39,	/* Bahrain */
	eCountry_EGYPT			= 40,	/* Egypt */
	eCountry_IRAN			= 41,	/* Iran */
	eCountry_IRAQ			= 42, 	/* Iraq */
	eCountry_KUWAIT			= 43,	/* Kuwait */
	eCountry_OMAN			= 44,	/* Oman */
	eCountry_QATAR			= 45,	/* Qatar */
	eCountry_SAUDIARABIA	= 46,	/* Saudi Arabia */
	eCountry_AZERBAIJAN	= 47,	/* Azerbaijan */
	eCountry_BULGARIA		= 48,	/* Bulgaria */
	eCountry_TURKMENISTAN	= 49,	/* Turkmenistan */
	eCountry_UZBEKISTAN	= 50,	/* Uzbekistan */
	/**< add start */

	/**< add end */
	eCountry_Other_Country	= 0xFE,
	eCountry_MAX			= 0xFF
} Country_Idx_t;
#endif

// 국가 단위가 아닌 service 제공 지역 구분..
// 국가보다 큰 단위도 작은 단위도 될 수 있다...
typedef enum
{
	eRegion_NotSpecified	= 0,
	eRegion_EU				= 1,	// EU Sat, EU Cab, EU Ter ...
	eRegion_CentralEurope	= 2,
	eRegion_EastEurope		= 3,
	eRegion_Nordig			= 4,	// Nordig Countries : Swe, Den, Nor, Fin
	eRegion_ME				= 5,	// Middle East
	eRegion_FinlandCable	= 6,
	eRegion_FinlandDtt		= 7,
	eRegion_NorwayDtt		= 8,
	eRegion_MAX,
	eRegion_OtherRegion		= 0xFF
} Region_Idx_t;

/* default LCN을 할당하기 위한 rule이다. 이에 따라 tv/radio 채널의 시작 넘버가 정해진다 */
typedef enum tagNonLcnPolicy
{
	eDEFAULTLCN_NONE					= 0,	// Default LCN 에 대한 정책이 없다 : LCN 모듈에서 모든 것을 다 처리한다
	eDEFAULTLCN_NORMAL					= 1,	// Humax 기본 정책
//	eDEFAULTLCN_RAY_NORMAL				= 1,	// Humax의 Ray UI에서 사용하는 가본 정책
	eDEFAULTLCN_ALVIN_NORMAL			= 2,	// Alvin (LG 사양)에 적용되는 LG UI 사양인 Alvin에서 사용되는 정책
	eDEFAULTLCN_AFTER_LCN				= 3,	// Non-LCN Service 들이 LCN Service 다음에 오는 정책 : TV / Radio가 서비스 번호를 나눠 갖는 방식
	eDEFAULTLCN_AFTER_LCN_TYPEDEVIDE	= 4,	// Non-LCN Service 들이 LCN Service 다음에 오는 정책 : TV / Radio가 각각 따로 서비스 리스트를 가지는 방식
	eDEFAULTLCN_APSLCN					= 5,	// APS LCN이 적용되는 모델에서의 Default Non-LCN 정책. Non-LCN Service 들이 LCN Service 다음에 오며 LCN이 없으면 각각 100번부터 시작한다.
	eDEFAULTLCN_KDG						= 6,	// KDG LCN
	eDEFAULTLCN_SKYD					= 7,	// sky deutschland (구, premiere) LCN
	eDEFAULTLCN_CANALREADY				= 8,	// TNTSAT CanalReady, Canal+(1~600) + Terrestrial(601~999) + OtherFTA (1000~ )
	eDEFAULTLCN_NTVPLUS					= 9,
	eDEFAULTLCN_ADTV					= 10,	// 1 - 99 : ADTV TV 채널, 100 ~ 4999 : ADTV 채널이 아닌 TV 채널, 5000 - 5099 : ADTV Radio 채널, 5100 ~ 9999
	eDEFAULTLCN_UPC						= 11,	// UPC정책에 의해 NON_LCN은 300번부터 시작한다.
	eDEFAULTLCN_SPAIN_TDT				= 12,	// Spain TDT 방식 : 기본적으로 동일하나 동일 서비스의 개념이 약간 다르다.
	eDEFAULTLCN_DIGITURK				= 13,	// Digiturk Mode->Using LCN number, Other mode -> LCN + Non-LCN number.
	eDEFAULTLCN_NORDIG					= 14,	// Nordig, LCN + 현재 OrgNetId에 해당하는 Non-Lcn + 다른 OrgNetId에 해당하는 Non-Lcn
	eDEFAULTLCN_JAPAN					= 15,	// Japan NIT service list descriptor service_id 이용. DT는 serivce_id와 TS information descriptor의 remote_control_key_id의 연산 필요.
	eDEFAULTLCN_NONE_FSAT				= 16,	// Non-Freesat service numbers on Freesat Model.
	eDEFAULTLCN_AUSDTT					= 17,	// AUS_DTT 350 ~ 399
	eDEFAULTLCN_ERTELECOM				= 18,	// ER telecom (Russia) TV/Radio 2000번부터 시작
	eDEFAULTLCN_UKDTT					= 19,	// UK_DTT
	eDEFAULTLCN_ZIGGO						= 20,	// 네들란드 ZIGGO cable
	eDEFAULTLCN_MAX
} DefaultLcnPolicy_t;

/*
	* pre-programmed channel을 사용하는 모델의 number limit 정책
		- operator type에 따라, pre-programmed channel의 LCN을 할당하기 위한 rule이 정해진다.
		- start, end를 벗어난 채널은 로딩되지 않는다.
*/
typedef enum tagPreProgrammedChannelNumberPolicy
{
	ePreChNum_NONE				= 0,	/**< 별다른 제약이 없는 경우 */
	ePreChNum_APS				= 1,	/**< Astra Channel List 사양에 따라, 1~99까지의 채널만 로딩된다 */
	ePreChNum_SkyD				= 2,	/**< Astra Channel List 사양에 따라, 1~99까지의 채널만 로딩된다 */

	ePreChNum_MAX
} PreProgrammedChNumPolicy_t;



typedef enum
{
	eEPG_SPEC_BASE = 0,
	eEPG_SPEC_UKDTT,
	eEPG_SPEC_FSAT,
	eEPG_SPEC_TVTV,
	eEPG_SPEC_NORDIG,
	eEPG_SPEC_NORDIG_HOME,
	eEPG_SPEC_BOXER_EIT_PLUS,
	eEPG_SPEC_CIT,
	eEPG_SPEC_CPT,

/*********************************/
/* Japan spec.                   */
/*********************************/
	eEPG_SPEC_JP_ARIB,
	eEPG_SPEC_JP_JCOM,
	eEPG_SPEC_JP_SKAPA,


/*********************************/
/* 추가하시려면 위쪽에....     */
/*********************************/
	eEPG_SPEC_MAX
} EPG_SpecType_e;

typedef enum {
	eTsr_On,
	eTsr_Off
} Tsr_Set_t;

/******* JC8500BR 추가 ********/

typedef enum{
	ePPVSetupOn = 0,
	ePPVSetupOff = 1,
}PPV_Setup_t;

typedef enum{
	ePPVLimitMoney_100 = 0,
	ePPVLimitMoney_300 = 1,
	ePPVLimitMoney_500 = 2,
	ePPVLimitMoney_1000 = 3,
	ePPVLimitMoney_1500 = 4,
	ePPVLimitMoney_2000 = 5,
	ePPVLimitMoney_2500 = 6,
	ePPVLimitMoney_3000 = 7,
	ePPVLimitMoney_None = 8
} PPV_LimitMoney_t;

typedef enum{
	ePPVExpirationPowerOff = 0,
	ePPVExpirationChChange = 1
} PPV_Expiration_t;

typedef enum{
	eDisplaySideCutOn = 0,
	eDisplaySideCutOff = 1
} DisplaySideCut_t;

typedef enum{
	eDisplayHdmiTvPowerLinkOn = 0,
	eDisplayHdmiTvPowerLinkOff = 1,
}DisplayHdmiTvPowerLink_t;

typedef enum{
	eCaptionOn = 0,
	eCaptionOff = 1
}CaptionSetup_t;

typedef enum{
	eCaptionLanguageJapanFirst = 0,
	eCaptionLanguageForeignFirst = 1
}CaptionSetupLanguage_t;

typedef enum{
	eCaptionSuperOn = 0,
	eCaptionSuperOff = 1
}CaptionSetupSuper_t;

typedef enum{
	eCaptionSuperLanguageJapanFirst = 0,
	eCaptionSuperLanguageForeignFirst = 1
}CaptionSetupSuperLanguage_t;

typedef enum{
	eSetupLaunchMode_Normal,
	eSetupLaunchMode_Wizard,
	eSetupLaunchMode_Installer,
	eSetupLaunchMode_End,
}SetupLaunchMode_t;

typedef enum{
	eDisplayMode3D_Only3DContent = 0,		/* 컨텐츠만 3d 로 				*/
	eDisplayMode3D_Return2D =1,			/* 3d 컨텐츠 종료 후 2d로 복귀 	*/
	eDsiplayMode3D_Only2D = 2,				/* 3d도 2d 영상으로 표시 			*/
}GraphicDisplayMode3D_t;

typedef enum{
	ePinUnlockOption_PowrOff,
	ePinUnlockOption_ChChange,
	ePinUnlockOption_End,
}PinUnlockExpirationOption_t;

typedef enum{
	eAmPermissionLevel_0,
	eAmPermissionLevel_1,
	eAmPermissionLevel_2,
	eAmPermissionLevel_3,
	eAmPermissionLevel_END,
}AMPermissionLevel_t;

typedef enum{
	eStandbyMode_None = 0,
	eStandbyMode_PowerON = 1,
	eStandbyMode_PowerOFF = 2,
}StandbyMode_t;

typedef enum{
	eHddEnergyConservationOn = 0,
	eHddEnergyConservationOff  =1
}HddPowerSave_t;

typedef enum{
	eTargetMediaTV =0,
	eTargetMediaRadioData =1,
	eTargetMediaAll =2
}TargetMedia_t;

typedef enum{
	ePvrBasicRecordingDeviceHdd = 0,
	ePvrBasicRecordingDeviceBD = 1
}PvrBasicRecordingDevice_t;

typedef enum{
	ePvrRecordingModeDR = 1,
	ePvrRecordingModeX2 = 2,
	ePvrRecordingModeX3 = 3,
	ePvrRecordingModeX5 = 5,
	ePvrRecordingModeX7 = 7,
}PvrRecordingMode_t;

typedef enum{
	ePvrEncryptNone		= 0,
	ePvrEncrypt3Des 	= 1,
	ePvrEncryptAes		= 2,
	ePvrEncryptUnknown	= 3,
}PvrEncryptMode_t;

typedef enum{
	eInformationGettingOn = 0,
	eInformationGettingOff = 1
}InformationGetting_t;

typedef enum{
	eProgramInformationOn = 0,
	eProgramInformationOff = 1
}ProgramInformation_t;

typedef enum{
	ePvrTimebarNormal = 0,
	ePvrTimebarMini = 1,
	ePvrTimebarOff = 2
}PvrTimebar_t;

typedef enum
{
	eTargetStorageManual = 0,
	eTargetStorageIntHdd = 1,
	eTargetStorageIntSubHdd = 2,
	eTargetStorageExtHdd = 3
} TargetStorage_t;

typedef enum{
	eDisplayPowerOffOn = 0,
	eDisplayPowerOffOff = 1
}DisplayPowerOff_t;

typedef enum{
	eCommunityChOn =0,
	eCommunityChOff = 1,
}CommunityCh_t;

typedef enum{
	eChSkipOn = 0,
	eChSkipOff = 1,
}ChSkip_t;

typedef enum{
	eSeekInputMethod3Input = 0,
	eSeekInputMethodFreeset = 1,
}SeekInputMethod_t;


typedef enum{
	eHokaidoSaporo = 10,				// 0		hokaido saporo
	eHokaidoHakodate,					// 1		hokaido hakodate
	eHokaidoAsahigawa,				// 2		hokaido asahigawa
	eHokaidoObihiro,					// 3 		hokaido obihiro
	eHokaidoKusiro,						// 4		hokaido kusiro
	eHokadioKitami,						// 5		hokaido kitami
	eHokadioMuroran,					// 6		hokaido muroran
	eMiyagi,									// 9		miyagi
	eAkida,									// 10		akida
	eYamagata,							// 11	yamagata
	eIwate,									// 8		iwate
	eHukusima,							// 12	hukusima
	eAomori,								// 7		aomori
	eTokyo,									// 18	tokyo
	eKanakawa,							// 19	kanakawa
	eGunma,								// 15	gunma
	eIbaraki,								// 13	ibaraki
	eChiba,									// 17	chiba
	eTochigi,								// 14	tochigi
	eSaitama,								// 16	saitama
	eNagano,								// 25	nagano
	eNiigata,								// 20	niigata
	eYamanashi,							// 24	yamanashi
	eAichi,									// 28	aichi
	eiShikawa,								// 22	ishikawa
	eShizuoka,								// 27	shizuoka
	eHukui,									// 23	hukui
	eToyama,								// 21	toyama
	eMie,										// 29	mie
	eGihu,									// 26	gihu
	eOsaka,									// 32	osaka
	eKyoto,									// 31	kyoto
	eHyogo,									// 33	hyogo
	eWakayama,							// 35	wakayama
	eNara,									// 34	nara
	eShiga,									// 30	shiga
	eHiroshima,							// 39	hiroshima
	eOkayama,								// 38	okayama
	eShimane,								// 37	shimane
	eTottori,									// 36	tottori
	eYamaguchi,							// 40	yamaguchi
	eEhime,									// 43	ehime
	eKagawa,								// 42	kagawa
	eDokushima,							// 41	dokushima
	eKochi,									// 44	kochi
	eHukuoka,								// 45	hukuoka
	eKumamoto,							// 48	kumamoto
	eNagasaki,								// 47	nagasaki
	eKagoshima,							// 51	kagoshima
	eMiyazaki,								// 50	miyazaki
	eOita,									// 49	oita
	eSaga,									// 46	saga
	eOkinawa,								// 52	okinawa
	eTokyoEtcShima = 151,			// 53	tokyo_etc-shima		=> 151
	eKagoshimaEtcShima = 186,		// 54	kagoshima_etc-shima		=> 186

}Area_t;

typedef enum
{
	eFirstDtSearchQAM,
	eFirstDtSearchOFDM,
	eFirstDtSearchQAMFirst,
	eFirstDtSearchOFDMFirst,
}FirstDtSearch_t;



#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define ____SATELLITE_channel_params____
#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

/******************************************
		LNB Parameters
******************************************/
#define CH_UNIVERSAL_LNB_FREQ		0xFFFFFFFF
#define CH_MDU1_LNB_FREQ			(CH_UNIVERSAL_LNB_FREQ - 1)			// for turkish LNB types MDU1 using in digitiruk OP
#define CH_MDU2_LNB_FREQ			(CH_UNIVERSAL_LNB_FREQ - 2)			// for turkish LNB types MDU2 using in digitiruk OP
#define CH_MDU3_LNB_FREQ			(CH_UNIVERSAL_LNB_FREQ - 3)			// for turkish LNB types MDU3 using in digitiruk OP
#define CH_MDU4_LNB_FREQ			(CH_UNIVERSAL_LNB_FREQ - 4)			// for turkish LNB types MDU4 using in digitiruk OP
#define CH_MDU_USER_DEFINE		(CH_UNIVERSAL_LNB_FREQ - 5)			// for turkish LNB types User define using in digitiruk OP


/******************************************
		Antenna, Switch Parameters
******************************************/

/************************************************
		Tuner, Demudulator Parameters
*************************************************/

/************************************************
		Motor Parameters
*************************************************/
typedef enum
{
	eMOTOR_DIR_EAST		= 0,
	eMOTOR_DIR_WEST		= 1
} CH_MotorDirection_t;

typedef enum
{
	eMOTOR_DRIVE_MODE_CONTINUOUS	= 0,
	eMOTOR_DRIVE_MODE_TIMEOUT		= 1,
	eMOTOR_DRIVE_MODE_STEP			= 2
} CH_MotorDriveMode_t;

#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define ____DIGITAL_TERRESTRIAL_channel_params____
#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define ____ANALOG_TERRESTRIAL_channel_params____
#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

/* analog video decoder ID */
typedef enum
{
	ANA_VIDEO_DEC_ID1	= 0,
	ANA_VIDEO_DEC_ID2	= 1,
	ANA_VIDEO_DEC_MAX
} ANA_VideoDecoder_ID_t;

/* analog video color standard ID */
typedef enum
{ /* [WARN] forbid to change a db-saving enum value, [WARN] 절대 ifdef로 추가 하지 말것. 보이면 바로 지울 것임 */
	ANA_VIDEO_AUTO			= 0,
	ANA_VIDEO_NTSC			= 1,
	ANA_VIDEO_PAL			= 2,
	ANA_VIDEO_PAL_N			= 3,
	ANA_VIDEO_PAL_M			= 4,
	ANA_VIDEO_SECAM			= 5,
	ANA_VIDEO_NTSC_443		= 6,
	ANA_VIDEO_PAL_60		= 7,
	ANA_VIDEO_UNKNOWN_525	= 8,
	ANA_VIDEO_UNKNOWN_625	= 9,
	ANA_VIDEO_UNKNOWN		= 10,
	ANA_VIDEO_MAX			= 11
} ANA_VideoStandard_t;

/* analog audio decoder ID */
typedef enum
{
	ANA_AUDIO_DEC_ID1		= 0,
	ANA_AUDIO_DEC_ID2		= 1,
	ANA_AUIDO_DEC_MAX
} ANA_AudioDecoder_ID_t;

/* analog auido standard ID */
typedef enum
{ /* [WARN] forbid to change a db-saving enum value, [WARN] 절대 ifdef로 추가 하지 말것. 보이면 바로 지울 것임 */
	ANA_AUDIO_BG			= 0,
	ANA_AUDIO_I				= 1,
	ANA_AUDIO_DK			= 2,
	ANA_AUDIO_L				= 3,
	ANA_AUDIO_AUTO			= 4,
	ANA_AUDIO_MAX			= 5
} ANA_AudioStandard_t;

/**
 * Tune mode.
 */
typedef enum
{ /* [WARN] forbid to change a db-saving enum value, [WARN] 절대 ifdef로 추가 하지 말것. 보이면 바로 지울 것임 */
	SVC_CH_A_SET_TUNE	= 0,
	SVC_CH_A_SET_FREQ	= 1,
	SVC_CH_A_SET_SYSTEM	= 2,
	SVC_CH_A_SET_SEARCH	= 3,
	SVC_CH_RESERVED
} SVC_CH_ANA_TuneMode_e;

typedef enum
{ /* [WARN] forbid to change a db-saving enum value, [WARN] 절대 ifdef로 추가 하지 말것. 보이면 바로 지울 것임 */
    TUNERBAND_VUHF		= 0,
    TUNERBAND_CABLE		= 1,
    TUNERBAND_INVALID	= 2
} SVC_CH_ANA_TunerBand_e;

#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define ____CABLE_channel_params____
#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

typedef enum
{
	CAB_SYMBOLRATE_ANALOGUE			= 0,
	CAB_SYMBOLRATE_AUTO				= 0x0000FFFF	/*0xFFFFFFFF*/
} CAB_Symbolrate_t;

#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define ____video_params____
#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

#define	OSD_1920x1080_SCREEN_WIDTH		1920
#define	OSD_1920x1080_SCREEN_HEIGHT		1080
#define	OSD_720x576_SCREEN_WIDTH		720
#define	OSD_720x576_SCREEN_HEIGHT		576
#define	OSD_1280x720_SCREEN_WIDTH		1280
#define	OSD_1280x720_SCREEN_HEIGHT		720

/* @brief Video Codec 정보 .  */
#define CODEC_CURRENT	0x1111		/* Codec과 PID 변경시 PID_CURRENT와 같이 codec 변경은 없다는 표시로 사용 */

typedef enum
{
	eOsdZOrder_Ui,
	eOsdZOrder_HbbTV,
	eOsdZOrder_MHP,
	eOsdZOrder_MHEG,
	eOsdZOrder_BML,
	eOsdZOrder_ERROR
} OsdZOrder_t;

typedef enum
{
	eVIDEO_Dec_Main = 0,
	eVIDEO_Dec_Sub,
	eVIDEO_Dec_Max
} VIDEO_DecType_t;

#define	MAX_VIDEO_DECODER_NUM			2

/* @brief MPEG Input Stream 및 HDMI EDID 에 정의된 Frame Rate .......*/
typedef enum
{
	eFrameRate_Unknown,
	eFrameRate_24,
	eFrameRate_25,
	eFrameRate_30,
	eFrameRate_50,
	eFrameRate_60,
	eFrameRate_Max
} VIDEO_FrameRate_t;

typedef struct
{
	DxResolutionStatus_e	eResolution;
	DxAspectRatio_e			eAspectRatio;
	DxPictureRate_e			ePictureRate;
} VIDEO_Timing_t;

typedef struct
{
	DxRect_t	stInRect;
	HUINT32		ulInRefWidth;
	HUINT32		ulInRefHeight;

	DxRect_t	stOutRect;
	HUINT32		ulOutRefWidth;
	HUINT32		ulOutRefHeight;
} PIG_Rect_t;

/* Active format description 과 wide screen signaling 관련 파라미터 */
typedef enum
{
	eVIDEO_BLANK_SPEC_NONE,
	eVIDEO_BLANK_SPEC_WSS,
	eVIDEO_BLANK_SPEC_CGMS,
	eVIDEO_BLANK_SPEC_CGMS_JAPAN
} VideoBlankSpec_e;

typedef enum
{
	eAfd_None,
	eAfd_Full,	/* 1000 */
	eAfd_43,	/* 1001 */
	eAfd_169,	/* 1010 */
	eAfd_149,	/* 1011 */
	eAfd_43_SnP149,		/* 1101 */
	eAfd_169_SnP149,	/* 1110 */
	eAfd_169_SnP43		/* 1111 */
} VIDEO_Afd_t;

typedef enum
{
	eWss_Unknown,
	eWss_43_Full,	/* 0001 */
	eWss_43_LB149_Center,	/* 1000 */
//	eWss_43_LB149_Top,	/* 0100 */
	eWss_43_LB169_Center,	/* 1101 */
//	eWss_43_LB169_Top,	/* 0010 */
//	eWss_43_LB169_Over,	/* 1011 */
	eWss_43_SnP149,		/* 0111 */
	eWss_169_Full,	/* 1110 */
} VIDEO_Wss_t;

/* @brief Video Mute시 Level 정보.  */
typedef enum
{
	eVIDEO_MuteMode_None			= 0,				/**< Mute 하지 않음. */
	eVIDEO_MuteMode_Hide			= 1,				/**< Video Window Off */
	eVIDEO_MuteMode_BlackOut		= 2,				/**< Decoder Mute */
	eVIDEO_MuteMode_HideBlack		= 3					/**< Decoder Mute & Video Window Off (DAC Off) */
}VIDEO_MuteMode_t;

/*
	macrovision 및 cgms-a control mode
	analog video output에 대한 copyright 처리 시 사용.
*/
typedef enum
{
	eMACROVISION_OFF			= 0,		/* macrovision off */
	eMACROVISION_AGC			= 1,		/* agc process on, split burst off */
	eMACROVISION_2LINE			= 2,		/* agc process on, 2 line split burst on */
	eMACROVISION_4LINE			= 3,		/* agc process on, 4 line split burst on */
	eMACROVISION_TEST1			= 4,
	eMACROVISION_TEST2			= 5
} Macrovision_t;

typedef enum
{
	eHDCP_BLOCK_NONE,
	eHDCP_BLOCK_AVMUTE,
	eHDCP_BLOCK_HDMIBLANK,
	eHDCP_BLOCK_DOWNSCALE,

	eHDCP_BLOCK_MAX
} HdcpBlockMode_e;

typedef enum
{
	eSCART_EVENT_NONE,
	eSCART_EVENT_VCR_ASPECTRATIO,
	eSCART_EVENT_VCR_POWER,
	eSCART_EVENT_MAX
} ScartEvent_t;

/**
 *  * Define media Trick Mode
 *   */
typedef enum
{
	eMEDIA_SpeedTrick_PAUSE,
	eMEDIA_SpeedTrick_1RWSTEP,
	eMEDIA_SpeedTrick_2RWSTEP,
	eMEDIA_SpeedTrick_3RWSTEP,
	eMEDIA_SpeedTrick_4RWSTEP,
	eMEDIA_SpeedTrick_5RWSTEP,
	eMEDIA_SpeedTrick_6RWSTEP,
	eMEDIA_SpeedTrick_7RWSTEP,
	eMEDIA_SpeedTrick_8RWSTEP,
	eMEDIA_SpeedTrick_9RWSTEP,
	eMEDIA_SpeedTrick_10RWSTEP,
	eMEDIA_SpeedTrick_11RWSTEP,
	eMEDIA_SpeedTrick_12RWSTEP,      /* Rewind 12배속 */
	eMEDIA_SpeedTrick_13RWSTEP,      /* Rewind 13배속 */
	eMEDIA_SpeedTrick_14RWSTEP,      /* Rewind 16배속 */
	eMEDIA_SpeedTrick_15RWSTEP,      /* Rewind 32배속 */
	eMEDIA_SpeedTrick_16RWSTEP,      /* Rewind 64배속 */
	eMEDIA_SpeedTrick_1FFSTEP,
	eMEDIA_SpeedTrick_2FFSTEP,
	eMEDIA_SpeedTrick_3FFSTEP,
	eMEDIA_SpeedTrick_4FFSTEP,
	eMEDIA_SpeedTrick_5FFSTEP,
	eMEDIA_SpeedTrick_6FFSTEP,
	eMEDIA_SpeedTrick_7FFSTEP,
	eMEDIA_SpeedTrick_8FFSTEP,
	eMEDIA_SpeedTrick_9FFSTEP,
	eMEDIA_SpeedTrick_10FFSTEP,
	eMEDIA_SpeedTrick_11FFSTEP,
	eMEDIA_SpeedTrick_12FFSTEP,
	eMEDIA_SpeedTrick_13FFSTEP,      /* Forward 13배속 */
	eMEDIA_SpeedTrick_14FFSTEP,      /* Forward 16배속 */
	eMEDIA_SpeedTrick_15FFSTEP,      /* Forward 32배속 */
	eMEDIA_SpeedTrick_16FFSTEP           /* Forward 64배속 */
} eMEDIA_TrickSpeed_t;


/**
 *  * Define media stream type
 *   */
typedef enum
{
	eMEDIA_Type_MPEG_Unknown,
	eMEDIA_Type_MPEG_ES,        /* Elementary stream */
	eMEDIA_Type_MPEG_BES,       /* Broadcom elementary stream */
	eMEDIA_Type_MPEG_PES,       /* Packetized elementary stream */
	eMEDIA_Type_MPEG_TS,        /* Transport stream(188) */
	eMEDIA_Type_MPEG_TS_192,    /* Transport stream(192) */
	eMEDIA_Type_MPEG_DSS_ES,    /* DSS with ES payload (used for SD) */
	eMEDIA_Type_MPEG_DSS_PES,   /* DSS with PES payload (used for HD) */
	eMEDIA_Type_MPEG_VOB,       /* video object, used with DVD */
	eMEDIA_Type_MPEG_ASF,       /* Advanced Systems Format */
	eMEDIA_Type_MPEG_AVI,       /* Audio Video Interleave */
	eMEDIA_Type_MPEG_MPEG1,     /* MPEG1 System Stream */
	eMEDIA_Tyep_MPEG_MP4,       /* MP4 (MPEG-4 Part12) container */
	eMEDIA_Type_MPEG_MP4FRAGMENT, /* Fragmented MP4 Container */
	eMEDIA_Type_MPEG_FLV,       /* Flash video container */
	eMEDIA_Type_MPEG_MKV        /* Matroska container */
} eMEDIA_StreamType_t;


typedef enum{
	eMediaEncryptType_NONE,

	eMediaEncryptType_DVBCSA,
	eMediaEncryptType_ASA64,
	eMediaEncryptType_ASA128,
	eMediaEncryptType_AES128,
	eMediaEncryptType_TDES,

	eMediaEncryptType_UNKNOWN

}eMediaEncryptType_t;

/**
 *  * Media Divx Subtitle Type
 *   */
typedef enum
{
	eMEDIA_DivxSubtitleType_DXSA,     /**< XSUB */
	eMEDIA_DivxSubtitleType_DXSB,     /**< XSUB + */
	eMEDIA_DivxSubtitleType_MAX,      /**< MAX*/
} eMEDIA_DivxSubtitleType_t;


#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define ____audio_params____
#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx


/* @brief PMT -> ISO 639 language descriptor (Audio Type) */
/*
   AUDIO_Type_t 와 SUPPLEMENTARY_AUDIO_Type_t는 근본적으로 같은 descriptor이다.
   AUDIO_Type_t 는 13818-1 문서의 "2.6.18 ISO 639 language descriptor" 의
                Table 2-52 에 정의 된 Value이고.
   SUPPLEMENTARY_AUDIO_Type_t 는
                300-468 문서의 "6.4.9 Supplementary audio descriptor"에 정의된
                Value이다. 그러므로 같이 취급할 것.
   그리고 둘다 Value 0 이외에는 Mixed Audio Type 이다.
 */
typedef enum
{
	eAUDIO_TYPE_UNDEFINED			= 0,		/**< Normal Audio. */
	eAUDIO_TYPE_CLEAN_EFFECTS		= 1,
	eAUDIO_TYPE_HEARING_IMPAIRED	= 2,
	eAUDIO_TYPE_VISUAL_IMPAIRED		= 3,	/**< AD(Audio Description). */
	eAUDIO_TYPE_RESERVED			= 4
} AUDIO_Type_t;

/* @brief PMT -> SUPPLEMENTARY Audio descriptor (Editorial_classification) */
/*
   AUDIO_Type_t 와 SUPPLEMENTARY_AUDIO_Type_t는 근본적으로 같은 descriptor이다.
   AUDIO_Type_t 는 13818-1 문서의 "2.6.18 ISO 639 language descriptor" 의
                Table 2-52 에 정의 된 Value이고.
   SUPPLEMENTARY_AUDIO_Type_t 는
                300-468 문서의 "6.4.9 Supplementary audio descriptor"에 정의된
                Value이다. 그러므로 같이 취급할 것.
   그리고 둘다 Value 0 이외에는 Mixed Audio Type 이다.
 */
typedef enum
{
	eAUDIO_TYPE_SUPP_MAIN = 0,
	eAUDIO_TYPE_SUPP_AD_VISUAL_IMPARED,
	eAUDIO_TYPE_SUPP_CLEAN_AUDIO_HEARING_IMPAIRED,
	eAUDIO_TYPE_SUPP_SPOKEN_SUBITITE_VISUAL_IMPARED,
	eAUDIO_TYPE_SUPPLEMENTARY_MAIN
} SUPPLEMENTARY_AUDIO_Type_t;

typedef enum
{
	eDECODER_STATE_STOPPED,
	eDECODER_STATE_STARTING,
	eDECODER_STATE_DECODING,
	eDECODER_STATE_UNDERRUN
} DecoderState_t;

typedef enum
{
	eAUDIO_DEC_MAIN,
	eAUDIO_DEC_SUB,
	eAUDIO_DEC_PASS,
	NUM_AUDIO_DEC
} AudioDecType_t;

/*
	Digital Audio Output 에 SCMS Control Mode For CI Plus(EMI) Requirement.
	embedded cas 등에서 digtial audio copyright control 시에도 사용 한다.

	digital audio 에 대해서 protection on/off가 다 인 경우, copy free와 copy never를 사용한다.
*/
typedef enum
{ /* 순서 뒤바꾸지 말것... */
	eCOPYRIGHT_COPY_FREE,
	eCOPYRIGHT_COPY_ONCE,
	eCOPYRIGHT_COPY_NOMORE,
	eCOPYRIGHT_COPY_NEVER
} CopyRight_t;

typedef struct
{
	DxVideoCodec_e 				eVideoCodec;
	HUINT32 					ulHorizontalSize;			/* Video Horizontal Window Size */
	HUINT32 					ulVerticalSize;				/* Video Vertical Window Size */
	HUINT32 					ulDisplayHorizontalSize;	/* Actual Video Horizontal Window Size */
	HUINT32 					ulDisplayVerticalSize;		/* Actual Video Vertical Window Size */
	HBOOL 						bProgressive;				/* Progressive(1), Interlaced(0) */
	DxAspectRatio_e 		eAspectRatio;
	VIDEO_Afd_t 				eAfd;
	VIDEO_FrameRate_t 			eFrameRate;
	HUINT32 					ulChromaFormat;
	DxCompositeStandard_e 	eCompositeStandard;
	HBOOL 						bColorPrimarie;
	HBOOL						bAlpha;
	DxVideo3DFormat_e			e3DFormat;
} VideoStreamInfo_t;

typedef struct
{
	HBOOL				bOutputEnable;

	DxAudioCodec_e		eHdmiAudioCodec;
	DxAudioCodec_e		eSpdifAudioCodec;

	HUINT32				ulVolume;
	HUINT32				ulVolumRange;

	VIDEO_Timing_t		stOutputTiming;

	DxDfcStatus_b		ulDfcStatus;

	Macrovision_t		eVideoMacrovision;
	Macrovision_t		eCgmsMacrovision;
	CopyRight_t			eCsmsCopyRight;
	CopyRight_t			eVideoBlankCopyRight;

	VIDEO_Wss_t			eVideoBlankWss;

	HBOOL				bHdmiActive;
	DxAspectRatio_e		eHdmiAspectRatio;
	VIDEO_Afd_t 		eHdmiAfd;
	DxVideo3DFormat_e	eHdmi3DStatus;
} OutputStatus_t;


#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define ____AV_output_params____
#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
typedef enum
{
/* TV Service에서 사용 중. 함부로 TV Manager에서 사용하면 책임 못짐. */
	eAvHideClient_None		= 0x0,
	eAvHideClient_Decoder	= 0x1,
	eAvHideClient_Cas		= 0x2,
	eAvHideClient_Hdcp		= 0x4,

/* TV Manager에서 정의해서 사용하는 client. */
	eAvHideClient_Action	= 0x00010000,
	eAvHideClient_UserLock	= 0x00020000,

	eAvHideClient_Max
} AV_HideClient_t;

#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define ____cas_params____
#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// CAS ID may be used in every SI related module:
#define CAS_ID_MASK_ALL			0xFF00
#define CAS_ID_MASK_MG			0x0100
#define CAS_ID_MASK_VA			0x0500
#define CAS_ID_MASK_IR			0x0600
#define CAS_ID_MASK_CX			0x0B00
#define CAS_ID_MASK_CR			0x0D00
#define CAS_ID_MASK_NA			0x1800
#define CAS_ID_MASK_ND			0x0900

// URI 관련 정보 저장 struct
typedef struct
{
	unsigned short usServiceId;	// ucMode 가 CI_CC_URI_MODE_FROMCAM 일 때만 유효.
	unsigned char ucMode;			// see CI_CC_URI_MODE_
	unsigned char ucAps;			// see CI_CC_URI_APS_
	unsigned char ucEmi;			// see CI_CC_URI_EMI_
	unsigned char ucIct;			// see CI_CC_URI_ICT_
	unsigned char ucRct;			// see CI_CC_URI_RCT_
	unsigned char ucDot;			// see CI_CC_URI_DOT_
	unsigned long ulMinutesForRL;	// minutes for RL (0 means unlimited retention)
} CIPLUS_UriInfo_t;


#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define ____si_params____
#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

#define ________dvb_params_________

typedef enum enDescriptorTag
{
	// 여기에는 ETS 300 468에까지 정의해 놓은 descriptor만을 등록시킨다.
	// 0x80 ~ 0xFE 에 해당하는 private descriptor는 각자의 사양에서 직접 등록시키도록 합시다.

	TAG_PRIVATE_DATA					= 0x00,
	/** < ISO/IEC 13818-1에 정의된 descriptor tag */
 	TAG_VIDEO_STREAM					= 0x02,	/** < Video Stream Descriptor */
 	TAG_AUDIO_STREAM					= 0x03,	/** < Audio Stream Descriptor */
 	TAG_HIERARCHY						= 0x04,	/** < Hierarchy Descriptor */
 	TAG_REGISTRATION					= 0x05,	/** < Registration Descriptor */
 	TAG_DATA_STREAM_ALIGNMENT			= 0x06,	/** < Data Stream Alignment Descriptor */
 	TAG_TARGET_BACKGROUND_GRID			= 0x07,	/** < Target Background Grid Descriptor */
 	TAG_VIDEO_WINDOW					= 0x08,	/** < Video Window Descriptor */
 	TAG_CA								= 0x09,	/** < CA Descriptor */
 	TAG_ISO_639_LANGUAGE				= 0x0A,	/** < ISO 639 Language Descriptor */
 	TAG_SYSTEM_CLOCK					= 0x0B,	/** < System Clock Descriptor */
 	TAG_MULTIPLEX_BUFFER_UTILIZATION	= 0x0C,	/** < Multiplex Buffer Utilization Descriptor */
 	TAG_COPYRIGHT						= 0x0D,	/** < Copyright Descriptor */
 	TAG_MAXIMUM_BITRATE					= 0x0E,	/** < Maximum Bitrate Descriptor */
 	TAG_PRIVATE_DATA_INDICATOR			= 0x0F,	/** < Private Data Indicator Descriptor */
 	TAG_SMOOTHING_BUFFER				= 0x10,	/** < Smoothing Buffer Descriptor */
 	TAG_STD								= 0x11,	/** < STD Descriptor */
 	TAG_CAROUSEL_ID						= 0x13, /** < Carousel ID Descriptor */
 	TAG_ASSOCIATION_TAG					= 0x14,	/** < Association Tag Descriptor */
 	TAG_DEFERRED_ASSOCIATION_TAG		= 0x15,	/** < Deferred Association Tag Descriptor */
	TAG_AVC_VIDEO						= 0x28,	/** < AVC Video Descriptor */
	/** < ETS 300 468에 정의된 descriptor tag */
 	TAG_NETWORK_NAME					= 0x40,	/** < Network Name Descriptor */
 	TAG_SERVICE_LIST					= 0x41,	/** < Service List Descriptor */
 	TAG_STUFFING						= 0x42,	/** < Stuffing Descriptor */
 	TAG_SATELLITE_DELIVERY_SYSTEM		= 0x43,	/** < Satellite Delivery System Descriptor */
 	TAG_CABLE_DELIVERY_SYSTEM			= 0x44,	/** < Cable Delivery System Descriptor */
 	TAG_BOUQUET_NAME					= 0x47,	/** < Bouquet Name Descriptor */
 	TAG_SERVICE							= 0x48,	/** < Service Descriptor */
 	TAG_COUNTRY_AVAILABILITY			= 0x49,	/** < Country Availability Descriptor */
 	TAG_LINKAGE							= 0x4A,	/** < Linkage Descriptor */
 	TAG_NVOD_REFERENCE					= 0x4B,	/** < NVOD Reference Descriptor */
 	TAG_TIME_SHIFTED_SERVICE			= 0x4C,	/** < Time Shifted Service Descriptor */
 	TAG_SHORT_EVENT						= 0x4D,	/** < Short Event Descriptor */
 	TAG_EXTENDED_EVENT					= 0x4E,	/** < Extended Event Descriptor */
 	TAG_TIME_SHIFTED_EVENT				= 0x4F,	/** < Time Shifted Event Descriptor */
 	TAG_COMPONENT						= 0x50,	/** < Component Descriptor */
 	TAG_MOSAIC							= 0x51,	/** < Mosaic Descriptor */
 	TAG_STREAM_IDENTIFIER				= 0x52,	/** < Stream Identifier Descriptor */
 	TAG_CA_IDENTIFIER					= 0x53,	/** < CA Identifier Descriptor */
 	TAG_CONTENT							= 0x54,	/** < Content Descriptor */
 	TAG_PARENTAL_RATING					= 0x55,	/** < Parental Rating Descriptor */
 	TAG_TELETEXT						= 0x56,	/** < Teletext Descriptor */
 	TAG_TELEPHONE						= 0x57,	/** < Telephone Descriptor */
 	TAG_LOCAL_TIME_OFFSET				= 0x58,	/** < Local Time Offset Descriptor */
 	TAG_SUBTITLING						= 0x59,	/** < Subtitling Descriptor */
 	TAG_TERRESTRIAL_DELIVERY_SYSTEM		= 0x5A,	/** < Terrestrial Delivery System Descriptor */
 	TAG_MULTILINGUAL_NETWORK_NAME		= 0x5B,	/** < Multilingual Network Name Descriptor */
 	TAG_MULTILINGUAL_BOUQUET_NAME		= 0x5C,	/** < Multilingual Bouquet Name Descriptor */
 	TAG_MULTILINGUAL_SERVICE_NAME		= 0x5D,	/** < Multilingual Service Name Descriptor */
 	TAG_MULTILINGUAL_COMPONENT			= 0x5E,	/** < Multilingual Component Descriptor */
 	TAG_PRIVATE_DATA_SPECIFIER			= 0x5F,	/** < Private Data Specifier Descriptor */
 	TAG_SERVICE_MOVE					= 0x60,	/** < Service Move Descriptor */
 	TAG_SHORT_SMOOTHING_BUFFER			= 0x61,	/** < Short Smoothing Buffer Descriptor */
	TAG_FREQUENCY_LIST					= 0x62,	/** < Frequency List Descriptor */
	TAG_DATA_BROADCAST					= 0x64,	/** < Data Boadcast Descriptor */
	TAG_DATA_BROADCAST_ID				= 0x66,	/** < Data Boadcast Descriptor (NEW, 2001)*/
	TAG_PDC								= 0x69,	/** < PDC Descriptor (300 468 v1.7.1) */
	TAG_DOLBY_AC3						= 0x6A,	/** < AC3(Dolby) Descriptor */
	TAG_ANCILLARY_DATA					= 0x6B,
	TAG_CELL_LIST						= 0x6C,
	TAG_CELL_FREQ_LIST					= 0x6D,
	TAG_ANNOUNCEMENT_SUPPORT			= 0x6E,
	TAG_APPLICATION_SIGNALLING			= 0x6F,	/** < Application Signalling Descriptor */
	TAG_ADAPTATION_FIELD_DATA			= 0x70,
	TAG_SVC_IDENTIFIER					= 0x71,
	TAG_SVC_AVAILABILITY				= 0x72,
	TAG_DEFAULT_AUTORITY				= 0x73,	/** < Default Authority descriptor */
	TAG_RELATED_CONTENT					= 0x74,
	TAG_CONTENT_IDENTIFIER				= 0x76,	/** < Content identifier descriptor */
	TAG_S2_SATELLITE_DELIVERY_SYSTEM 	= 0x79,
	TAG_ENHANCED_AC3					= 0x7A, /** < Enhanced - AC3(Dolby Plus) Descriptor */
	TAG_DTS								= 0x7B, /** see DVB 300-468 annex G*/
	TAG_AAC								= 0x7C, /** see DVB 300-468 annex H*/
	TAG_XAIT_LOCATION					= 0x7D,
	TAG_FTA_CONTENT_MANAGEMENT			= 0x7E,
	TAG_EXTENSION						= 0x7F,

	/** < LCN : 나라마다 다를 수 있음. 일반적으로 0x83 */
//	TAG_LOGIC_CHANNEL						= 0x83, /** < Logic Channel Descriptor */
	TAG_OTA_GROUP						= 0xD5,  // OTA Group Descriptor

	TAG_END								= 0xFF
} DescriptorTag;

typedef enum enExtDescriptorTag
{
	// 여기에는 ETS 300 468의 v1.10.1 (2009-07) 버전에서 추가된 extension descriptor의 TAG를 기술한다.
	TAG_EXT_IMG_ICON					= 0x00,
	TAG_EXT_CPCM_DEL_SIGNALLING			= 0x01,
	TAG_EXT_CP							= 0x02,
	TAG_EXT_CP_IDENTIFIER				= 0x03,
	TAG_EXT_T2_DELIVERY_SYSTEM			= 0x04,
	TAG_EXT_SH_DELIVERY_SYSTEM			= 0x05,
	TAG_EXT_SUPPLEMENTARY_AUDIO			= 0x06,
	TAG_EXT_NETWORK_CHANGE_NOTIFY		= 0x07,
	TAG_EXT_MESSAGE						= 0x08,
	TAG_EXT_TARGET_REG					= 0x09,
	TAG_EXT_TARGET_REG_NAME				= 0x0A,
	// 0x0C ~ 0x7F is reserved for future use
	// 0x80 ~ 0xFF is user defined
	TAG_EXT_END							= 0xFF
} ExtDescriptorTag;

// TS 102 006에 정의된 SSU UNT desctiptor tag
typedef enum enSsuUntDescriptorTag {
	TAG_START_RESERVED					= 0x00, // reserved
	TAG_SCHEDULING						= 0x01, // Scheduling Descriptor
	TAG_UPDATE							= 0x02, // Update Descriptor
	TAG_SSU_LOCATION					= 0x03, // SSU Location Descriptor
 	TAG_MESSAGE							= 0x04,	// Message Descriptor
 	TAG_SSU_EVENT_NAME					= 0x05,	// SSU Event Name Descriptor
 	TAG_TARGET_SMARTCARD				= 0x06,	// Target Smartcard Descriptor
 	TAG_MAC_ADDRESS						= 0x07,	// MAC Address Descriptor
 	TAG_SERIAL_NUMBER					= 0x08, // Serial Number Descriptor
 	TAG_IP_ADDRESS						= 0x09, // IP Address Descriptor
 	TAG_IPV6_ADDRESS					= 0x0A, // IPv6 Address Descriptor
 	TAG_SSU_SUBGROUP_ASSOCIATION		= 0x0B, // SSU Subgroup Association Descriptor
 	// 0x0C~0x3F : reserved for future SSU use
 	// 0x40~0x7F : in the scope of DVB SI
// 	TAG_TELEPHONE						= 0x57, // Telephone Descriptor
// 	TAG_PRIVATE_DATA_SPECIFIER			= 0x5F, // Private Data Specifier Descriptor
 	// 0x80~0xFE : user private
	TAG_END_RESERVED					= 0xFF
} SsuUntDescriptorTag;

typedef enum enAribAdditionalDescriptorTag
{
	/** < 일본향 ARIB에서 정의한, 추가 descriptor를 등록한다. */

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAG_PARTIAL_TRANSPORT_STREAM		= 0x63,		/* 0x63 Partial Transport Stream descriptor (SIT)			*/
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAG_BROADCASTER_ID					= 0x85,		/* 0x85 Broadcaster ID (SIT)							*/
	TAG_DTCP							= 0x88,		/* 0x88 DTCP (PMT)							*/
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAG_HIERARCHICAL_TRANSMISSION		= 0xC0,		/* 0xC0 Hierarchical transmission descriptor (PMT) 			*/
	TAG_DIGITAL_COPY_CONTROL				= 0xC1,		/* 0xC1 Digital copy control descriptor (PMT)   				*/
	TAG_NETWORK_IDENTIFICATION			= 0xC2,		/* 0xC2 Network Identification descriptor (SIT)				*/
	TAG_PARTIAL_TRANSPORT_STREAM_TIME	= 0xC3,		/* 0xC3 Partial Transport Stream Time descriptor (SIT)		*/
	TAG_AUDIO_COMPONENT					= 0xC4,		/* 0xC4 Audio component descriptor (EIT)          			*/
	TAG_HYPER_LINK						= 0xC5,		/* 0xC5 Hyper link descriptor							*/
	TAG_TARGET_AREA						= 0xC6,		/* 0xC6 Target Area descriptor							*/
	TAG_DATA_CONTENTS					= 0xC7,		/* 0xC7 Data contents descriptor (EIT)            				*/
	TAG_VIDEO_DECODE_CONTROL			= 0xC8,		/* 0xC8 Video decode control descriptor (PMT)    		 	*/
	TAG_DOWNLOAD_CONTENT				= 0xC9,		/* 0xC9 Download content descriptor(Note1)   				*/
	TAG_CA_EMM_TS							= 0xCA,		/* 0xCA CA_EMM_TS descriptor                					*/
	TAG_CA_CONTRACT_INFOMATION			= 0xCB,		/* 0xCB CA contract information descriptor   				*/
	TAG_CA_SERVICE						= 0xCC,		/* 0xCC CA service descriptor                					*/
	TAG_TS_INFORMATION					= 0xCD,		/* 0xCD TS information descriptor (NIT)					*/
	TAG_EXTENDED_BROADCASTER			= 0xCE,		/* 0xCE Extended broadcaster (BIT)						*/
	TAG_LOGO_TRANSMISSION				= 0xCF,		/* 0xCF Logo transmission								*/
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAG_CABLE_TS_DIVISION_SYSTEM		= 0xF9,		/* 0xF9 Cable TS division system (NIT) 					*/
	TAG_TERRESTRIAL_DELIVERY_SYSTEM_ARIB= 0xFA,		/* 0xFA Terrestrial delivery system (NIT) 					*/
	TAG_PARTIAL_RECEPTION				= 0xFB,		/* 0xFB Partial reception (NIT) 							*/
	TAG_EMERGENCY_INFORMATION			= 0xFC,		/* 0xFC Emergency information descriptor (PMT/NIT)   		*/
	TAG_DATA_COMPONENT					= 0xFD,		/* 0xFD Data component descriptor (PMT)           			*/
	TAG_SYSTEM_MANAGEMENT				= 0xFE,		/* 0xFE System management descriptor (PMT/NIT/TLV-NIT)   */
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAG_SERIES								= 0xD5,		/* 0xD5 Series descriptor (EIT)                  				*/
	TAG_EVENT_GROUP						= 0xD6,		/* 0xD6 Event group descriptor               					*/
	TAG_SI_PARAMETER						= 0xD7,		/* 0xD7 SI parameter descriptor (BIT)             				*/
	TAG_BROADCASTER_NAME				= 0xD8,		/* 0xD8 Broadcaster name descriptor (BIT)         			*/
	TAG_COMPONENT_GROUP					= 0xD9,		/* 0xD9 Component group descriptor (EIT)          			*/
	TAG_CONTENT_AVAILABILITY				= 0xDE,		/* 0xDE Content availability descriptor (EIT)     				*/


	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAG_ARIB_ADDITIONAL_DESTAG_END		= 0xFF
} AribAdditionalDescriptorTag;

typedef enum
{
	/* for CI+ 1.3 operator profile CICAM NIT */
	TAG_CIPLUS_CONTENT_LABEL			= 0xCB,
	TAG_CIPLUS_SERVICE					= 0xCC,
	TAG_CI_PROTECTION					= 0xCE,
}CiPlusCiCamNitDescriptorTag;

typedef enum
{
	eSI_TableType_PAT = 0,
	eSI_TableType_CAT,
	eSI_TableType_PMT,
	eSI_TableType_NIT_ACT,
	eSI_TableType_NIT_OTH,
	eSI_TableType_SDT_ACT,
	eSI_TableType_SDT_OTH,
	eSI_TableType_BAT,
	eSI_TableType_TDT,
	eSI_TableType_TOT,
	eSI_TableType_DSI,
	eSI_TableType_DDB,
	eSI_TableType_EIT_ACTPF,
	eSI_TableType_EIT_OTHPF,
	eSI_TableType_EIT_ACTSC,
	eSI_TableType_EIT_OTHSC,
// CONFIG_OP_FREESAT_GEN2_ADDFILTER >>>
	eSI_TableType_EIT_OTHPFEXT,
	eSI_TableType_EIT_OTHSCEXT,
// CONFIG_OP_FREESAT_GEN2_ADDFILTER <<<
	eSI_TableType_UNT,
	eSI_TableType_CIT,
	eSI_TableType_CPT,
	eSI_TableType_SGT,
	eSI_TableType_AIT,
	eSI_TableType_DIT,
	eSI_TableType_PMT_N,

	eSI_TableType_POSTCODE_TREE,		/* For freesat */
	eSI_TableType_POSTCODE_REGION,		/* For freesat */


	eSI_TableType_ICT,		/* For Irdeto OTA */

	eSI_TableType_REGIONAL_POOL,		/* for TNTSat */

	eSI_TableType_BIT,		/* for ARIB */
	eSI_TableType_SIT,		/* for ARIB */
	eSI_TableType_LowProtectionSDTT,	/* for ARIB */
	eSI_TableType_HighProtectionSDTT,	/* for ARIB */
	eSI_TableType_CDT,		/* for ARIB */
	eSI_TableType_DII,		/* for ARIB */

	eSI_TableType_RCT,		/* for TV Anytime */
/*  다음 si table들은 구현하면서 하나씩 추가할 예정
	eSI_TableType_EIT_NVOD,
	eSI_TableType_EMM,
	eSI_TableType_ECM,
	eSI_TableType_RST,
	eSI_TableType_DSC,
	eSI_TableType_Reserved01,
	eSI_TableType_Reserved02,
	eSI_TableType_Reserved03,
	eSI_TableType_Reserved04,
*/
/*
	#if defined(XXX) 사용 금지!!!!!!!!
	보이면 즉시 삭제 하겠습니다.
 */
	MAX_SI_TABLE_TYPE
} SiTableType_e;

// PID
enum
{
	PAT_PID    = 0x0000,
	CAT_PID    = 0x0001,
	TSDT_PID   = 0x0002,
	NIT_PID    = 0x0010,
	SDT_PID    = 0x0011,
	BAT_PID    = 0x0011,
	EIT_PID    = 0x0012,
	RST_PID    = 0x0013,
	TOT_PID    = 0x0014,
	TDT_PID    = 0x0014,
	DIT_PID 	= 0x001E, 					/* Japan, ARIB */
	SIT_PID		= 0x001F, 					/* Japan, ARIB */
	LOW_PROTECTION_SDTT_PID 	= 0x0023,	/* Japan, ARIB */
	BIT_PID 	= 0x0024,					/* Japan, ARIB */
	HIGH_PROTECTION_SDTT_PID	= 0x0028,	/* Japan, ARIB */
	CDT_PID = 0x0029,						/* Japan, ARIB */

	PID_NULL   = 0x2000,
	PID_CURRENT = 0x2001
};

// Table ID (8 bits value)
enum
{
	PAT_TABLE_ID                             = 0x00,
	CAT_TABLE_ID                             = 0x01,
	PMT_TABLE_ID                             = 0x02,
	TSDT_TABLE_ID                            = 0x03,
	NIT_ACTUAL_TABLE_ID                      = 0x40,
	NIT_OTHER_TABLE_ID                       = 0x41,
	SDT_ACTUAL_TABLE_ID                      = 0x42,
	SDT_OTHER_TABLE_ID                       = 0x46,
	EIT_ACTUAL_TABLE_ID                      = 0x4e,
	EIT_OTHER_TABLE_ID                       = 0x4f,
	EIT_ACTUAL_SCHEDULE_TABLE_ID_0           = 0x50,
	EIT_ACTUAL_SCHEDULE_TABLE_ID_1           = 0x51,
	EIT_ACTUAL_SCHEDULE_TABLE_ID_2           = 0x52,
	EIT_ACTUAL_SCHEDULE_TABLE_ID_3           = 0x53,
	EIT_ACTUAL_SCHEDULE_TABLE_ID_4           = 0x54,
	EIT_ACTUAL_SCHEDULE_TABLE_ID_5           = 0x55,
	EIT_ACTUAL_SCHEDULE_TABLE_ID_6           = 0x56,
	EIT_ACTUAL_SCHEDULE_TABLE_ID_7           = 0x57,
	EIT_ACTUAL_SCHEDULE_TABLE_ID_8           = 0x58,
	EIT_ACTUAL_SCHEDULE_TABLE_ID_9           = 0x59,
	EIT_ACTUAL_SCHEDULE_TABLE_ID_10          = 0x5a,
	EIT_ACTUAL_SCHEDULE_TABLE_ID_11          = 0x5b,
	EIT_ACTUAL_SCHEDULE_TABLE_ID_12          = 0x5c,
	EIT_ACTUAL_SCHEDULE_TABLE_ID_13          = 0x5d,
	EIT_ACTUAL_SCHEDULE_TABLE_ID_14          = 0x5e,
	EIT_ACTUAL_SCHEDULE_TABLE_ID_15          = 0x5f,
	EIT_OTHER_SCHEDULE_TABLE_ID_0            = 0x60,
	EIT_OTHER_SCHEDULE_TABLE_ID_1            = 0x61,
	EIT_OTHER_SCHEDULE_TABLE_ID_2            = 0x62,
	EIT_OTHER_SCHEDULE_TABLE_ID_3            = 0x63,
	EIT_OTHER_SCHEDULE_TABLE_ID_4            = 0x64,
	EIT_OTHER_SCHEDULE_TABLE_ID_5            = 0x65,
	EIT_OTHER_SCHEDULE_TABLE_ID_6            = 0x66,
	EIT_OTHER_SCHEDULE_TABLE_ID_7            = 0x67,
	EIT_OTHER_SCHEDULE_TABLE_ID_8            = 0x68,
	EIT_OTHER_SCHEDULE_TABLE_ID_9            = 0x69,
	EIT_OTHER_SCHEDULE_TABLE_ID_10           = 0x6a,
	EIT_OTHER_SCHEDULE_TABLE_ID_11           = 0x6b,
	EIT_OTHER_SCHEDULE_TABLE_ID_12           = 0x6c,
	EIT_OTHER_SCHEDULE_TABLE_ID_13           = 0x6d,
	EIT_OTHER_SCHEDULE_TABLE_ID_14           = 0x6e,
	EIT_OTHER_SCHEDULE_TABLE_ID_15           = 0x6f,

	BAT_TABLE_ID                             = 0x4a,
	RST_TABLE_ID                             = 0x71,
	TDT_TABLE_ID                             = 0x70,
	TOT_TABLE_ID                             = 0x73,
	AIT_TABLE_ID                             = 0x74,

	DIT_ARIB_TABLE_ID			= 0x7E,	/* Already defined DIT_TABLE_ID */
	SIT_ARIB_TABLE_ID			= 0x7F,

	DIT_TABLE_ID                             = 0xc5,
	DIT_TABLE_ID_EXT                     = 0xc5c5,
	DGT_TABLE_ID                             = 0xc6,
	DBT_TABLE_ID                             = 0xc7,

	DSI_TABLE_ID                             = 0x3b,		/* dsm-cc data carousel */
	DII_TABLE_ID				= 0x3b,		/* dsm-cc data carousel */
	DDB_TABLE_ID                             = 0x3c,		/* dsm-cc data carousel */
	DESC_TABLE_ID							 = 0x3d,		/* dsm-cc data carousel stream description*/
	UNT_TABLE_ID                             = 0x4b,		/* dvb-ssu enhanced profile */
	RCT_TABLE_ID                             = 0x76,    /* related content table TS 102-323 & 300-468 */
	CIT_TABLE_ID                             = 0xa0,
	CPT_TABLE_ID                             = 0xa1,
	SGT_TABLE_ID                             = 0x91,		/* from SUPPORT_ASTRA_LCN */
	RPD_TABLE_ID						     = 0xe0,		/* from TNTSat - Regionalization Pool desiptor Section Table */
	DSC_TABLE_ID                             = 0xf0,
	ICT_TABLE_ID                             = 0xfe,

	SDTT_TABLE_ID					         = 0xc3,							/* Japan, ARIB */
	BIT_TABLE_ID					         = 0xc4,							/* Japan, ARIB */
	CDT_TABLE_ID						 	 = 0xc8,							/* Japan, ARIB */

	FSAT_EITPF_PP_OTH_TABLE_ID		= 0xd1,						// Freesat Tunneld Eit pf++ Oth

	ETC_TABLE_ID                             = 0xf1
};

// DSM-CC Message Id
enum
{
	DII_MESSAGE_ID						= 0x1002,
	DDB_MESSAGE_ID						= 0x1003,
	DSI_MESSAGE_ID						= 0x1006
};

enum
{
	eBOUQUET_ID_IRDETO			= 0x3622,
	eBOUQEUT_ID_IRDETO_SOL		= 0x3623
};


// 300-468 에서 정의한 UCS2 용 Emphasis 문자
#define CHAR_EMPHASIS_ON		0x86
#define CHAR_EMPHASIS_OFF		0x87

#define UCS2_EMPHASIS_ON		0xE086
#define UCS2_EMPHASIS_OFF		0xE087
#define UCS2_CR_LF				0xE08A

#define ________mpeg_params_________


/** @brief ISO/IEC 13818-1에서 정의한 Stream type assignments. And there're more value.(0x07-0x0e) */
#define	ISO_VIDEO_1				0x01
#define	ISO_VIDEO_2				0x02
#define	ISO_AUDIO_1				0x03
#define	ISO_AUDIO_2				0x04
#define	ISO_PRIVDATA_1			0x05
#define	ISO_PRIVDATA_2			0x06
#define	ISO_DSMCC				0x08
#define	ISO_DSMCC_U_N			0x0B
#define	ISO_AUDIO_AAC			0x0f	/** < ISO/IEC 13818-7 Audio with ADTS transport syntax (AAC) */
#define	ISO_AUDIO_4				0x11	/** < MPEG4 High Efficiency AAC audio coding */
#define	ISO_VIDEO_4				0x1B	/** < H264/AVC video coding */
//#define	ISO_PES					0x06 /* MPEG-2 Packetized Elementary Stream packets containing private data ARIB caption/super */
#define	ISO_DATA					0x0D /* ISO/IEC 13818-6 typeD ARIB */
#define	ISO_AUDIO_AC3				0x81

#define ________octo_defined________

typedef enum
{
	eEsType_None,
	eEsType_Video,
	eEsType_Audio,
	eEsType_PCR,
	eEsType_Subtitle,
	eEsType_Teletext,
	eEsType_ClosedCaption,
	eEsType_SuperImpose,
	eEsType_Data,
	eEsType_Reserved,
	eEsType_Unknown,
	eEstype_Max
} EsType_t;

#define	IsHardOfHearingSubtitle(type)			(((type) == 0x05) || (0x20 <= (type) && (type) <= 0x24))
#define	IsEbuSubtitle(type)						((type) <= 0x05)
#define	IsDvbSubtitle(type)						((0x10 <= (type) && (type) <= 0x14))

#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define ____OTA_params____
#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// OUI
typedef enum
{
	eSWUP_OUI_DVB		= 0x00015A,
	eSWUP_OUI_HUMAX		= 0x000378,
} SWUP_OuiType_e;

typedef enum
{
	eSWUP_INFO_SLOT_0				= 0,
	eSWUP_INFO_SLOT_1,
	eSWUP_INFO_SLOT_2,
	eSWUP_INFO_SLOT_MAX
} SWUP_InfoSlot_e;

typedef enum
{
	eSWUP_DOWNLOAD_DVBSSU = 0,
	eSWUP_DOWNLOAD_DTG,
	eSWUP_DOWNLOAD_HUMAXTYPE,
	eSWUP_DOWNLOAD_SKYD,
	eSWUP_DOWNLOAD_NORDIG,
	eSWUP_DOWNLOAD_FILE,
	eSWUP_DOWNLOAD_MAX
} SWUP_DownloadType_e;

typedef enum
{
	eSWUP_CHANNEL_SAT		= 	1,
	eSWUP_CHANNEL_CAB		= 	2,
	eSWUP_CHANNEL_TER		=	3,
	eSWUP_CHANNEL_FLASH		=	4,
	eSWUP_CHANNEL_FTP		=	5
} SWUP_ChannelType_e;

/* predefined data types in CPS 420 007 */
// OTA_DataType_t 는 OTA Signalling 에서 오는 Data Type
typedef enum
{
	eSWUP_DATATYPE_INVALID = 0,
	eSWUP_DATATYPE_LOADER,
	eSWUP_DATATYPE_APPL,
	eSWUP_DATATYPE_EEPROM,
	eSWUP_DATATYPE_FLASH,
	eSWUP_DATATYPE_PRODID,				// 5
	eSWUP_DATATYPE_GAME,
	eSWUP_DATATYPE_DVD,
	eSWUP_DATATYPE_FILE					// 8
} SWUP_DataType_e;

typedef enum
{
	eSWUP_FLAG_NO_INFO,
	eSWUP_FLAG_INFO_DETECTED
} SWUP_Flag_e;

#if 0

// CONFIG_MW_DOWNLOAD_OTA_IMAGE
// NAND Background OTA Download 에서 down 완료 인지 loader 에 전달.
typedef enum
{
	eOTA_DOWNMODE_BACKGROUND_NONE = 0,
	eOTA_DOWNMODE_BACKGROUND_COMPLETED
} OTA_DownMode_Bg_t;

typedef enum
{
	ERR_OTA_OK						= 0,
	ERR_OTA_INVALID_SYSTEMID,
	ERR_OTA_HEADER_CRC_ERROR,
	ERR_OTA_HEADER_FORMAT_ERROR,
	ERR_OTA_DATA_CRC_ERROR,
	ERR_OTA_APP_CRC_ERROR,
	ERR_OTA_UART_TIMEOUT_ERROR,
	ERR_OTA_INVALID_ADDRESS,
	ERR_OTA_FLASH_WRITE_FAIL,
	ERR_OTA_INVALID_INFO_BLOCK,
	ERR_OTA_ADDRESS_ODD,
	ERR_OTA_FLASH_WRITE_TIMEOUT,
	ERR_OTA_OTA_NOT_FOUND,
	ERR_OTA_INVALID_SIGNATURE,
	ERR_OTA_DECRYPTION_ERROR,
	ERR_OTA_EEPROM_ERROR,
	ERR_OTA_SECURE_BOOT_FUSING_FAIL,
	ERR_OTA_SIGNAL_FAIL,
	ERR_OTA_HW_ERROR,

	ERR_OTA_MAX
} OTA_ErrorType_t;
#endif


// ======================================
// MW/PAL 에서 Version 얻어올 때의 Type
typedef enum
{
	eVERSION_TYPE_NEXTOTA,			// OTA Loader에게 알려주기 위한 Update될 OTA Image Version
	eVERSION_TYPE_LOADER,
	eVERSION_TYPE_APPL,
	eVERSION_TYPE_EEPROM,
	eVERSION_TYPE_KERNEL,
	eVERSION_TYPE_ROOTFS,
	eVERSION_TYPE_RESOURCE,
	eVERSION_TYPE_DB,
	eVERSION_TYPE_BOOTLOADER

} VersionType_t;

enum
{
	eVERSION_MAX_STR_LEN			= 32,
	eVERSION_TOTAL_MAX_STR_LEN		= 48,
};

#if 0
/* HDF File에 적히는 Data Type */
/* OTA Data Type와 다른 점을 주의하세요 */
typedef enum
{
	HDF_DATATYPE_LOADER			= 0,
	HDF_DATATYPE_APPLICATION,
	HDF_DATATYPE_EEPROM,
	HDF_DATATYPE_FLASH,
	HDF_DATATYPE_SYSTEM_ID,
	HDF_DATATYPE_DEFAULT,
	HDF_DATATYPE_BOOTLD,
	HDF_DATATYPE_BETACODE,
	HDF_DATATYPE_FILE
} HDF_DataBlockType_t;
#endif

#define SWUP_ETH_ADDR_LENGTH	4
#define SWUP_USER_LENGTH 		20
#define SWUP_PASSWD_LENGTH 		20
#define SWUP_DIR_PATH_LENGTH 	200
#define SWUP_FILE_NAME_LENGTH 	50

typedef struct
{
	HUINT8			aServerIp[SWUP_ETH_ADDR_LENGTH];
	HUINT8			aUserName[SWUP_USER_LENGTH];
	HUINT8			aPasswd[SWUP_PASSWD_LENGTH];
	HUINT8			aDirPath[SWUP_DIR_PATH_LENGTH];
	HUINT8			aFileName[SWUP_FILE_NAME_LENGTH];
} SwUpdate_IpEmergencyInfo_t;


#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define ____system_device_params____
#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
typedef struct
{
	HUINT8		ucSCS;
	HUINT8		ucJTAG;
	HUINT8		ucOTP;
	HUINT8		ucCWE;
	HUINT32		ulParams[4];
	HUINT16		usSTBOwnerID;
	HUINT8		ucRemuxFusing;
	HUINT8		ucMarketID;
} Sys_FusingInfo_t;


typedef enum
{
	eWAKEUP_BY_KEY,
	eWAKEUP_BY_TIMER,
	eWAKEUP_BY_ACPOWER,
	eWAKEUP_BY_HDMICEC,
	eWAKEUP_BY_REBOOT
} WakeUpReason_t;

typedef enum
{
	eWAKEUP_FOR_UNDECIDED,
	eWAKEUP_FOR_WATCHTV,
} WakeUpSecondReason_t;

typedef enum
{
	eWAKEUP_RCUKEY_GROUP_NUMBER = 0x01,
	eWAKEUP_RCUKEY_GROUP_CH_UP_DOWN = 0x02,
	eWAKEUP_RCUKEY_GROUP_ARROW = 0x04,
	eWAKEUP_RCUKEY_GROUP_SINGLE = 0x08,
	eWAKEUP_RCUKEY_MAX = 0x10
} WakeUpRcuKeyGroup;

// CONFIG_VFD_DIMM_CONTROL >>>
typedef enum
{
	eDIMM_LEVEL_ZERO			= 0,
	eDIMM_LEVEL_LOW,
	eDIMM_LEVEL_MIDDLE,
	eDIMM_LEVEL_HIGH
} DimmingLevel_e;
// CONFIG_VFD_DIMM_CONTROL <<<

typedef enum
{
	ePOWER_LED_OFF,
	ePOWER_LED_WATCHING,
	ePOWER_LED_STANDBY
} PowerLedState_t;

typedef	enum
{
	ePANEL_SCROLL_LEFT,
	ePANEL_SCROLL_RIGHT
} PanelScrollDirection_t;

typedef	enum
{
	ePANEL_SCROLL_ONCE,
	ePANEL_SCROLL_REPEAT
} PanelScrollType_t;


typedef enum
{
	ePANEL_DIM_LevelOff = 0,
	ePANEL_DIM_Level01,	ePANEL_DIM_Level02,	ePANEL_DIM_Level03,	ePANEL_DIM_Level04,	ePANEL_DIM_Level05,
	ePANEL_DIM_Level06,	ePANEL_DIM_Level07,	ePANEL_DIM_Level08,	ePANEL_DIM_Level09,	ePANEL_DIM_Level10,
	ePANEL_DIM_Level11,	ePANEL_DIM_Level12,	ePANEL_DIM_Level13,	ePANEL_DIM_Level14,	ePANEL_DIM_Level15,
	ePANEL_DIM_Level16,	ePANEL_DIM_Level17,	ePANEL_DIM_Level18,	ePANEL_DIM_Level19,	ePANEL_DIM_Level20,
	ePANEL_DIM_Level21,	ePANEL_DIM_Level22,	ePANEL_DIM_Level23,	ePANEL_DIM_Level24,	ePANEL_DIM_Level25,
	ePANEL_DIM_Level26,	ePANEL_DIM_Level27,	ePANEL_DIM_Level28,	ePANEL_DIM_Level29,	ePANEL_DIM_Level30,
	ePANEL_DIM_Level31,	ePANEL_DIM_Level32,	ePANEL_DIM_Level33,	ePANEL_DIM_Level34,	ePANEL_DIM_Level35,
	ePANEL_DIM_Level36,	ePANEL_DIM_Level37,	ePANEL_DIM_Level38,	ePANEL_DIM_Level39,	ePANEL_DIM_Level40,
	ePANEL_DIM_Level41,	ePANEL_DIM_Level42,	ePANEL_DIM_Level43,	ePANEL_DIM_Level44,	ePANEL_DIM_Level45,
	ePANEL_DIM_Level46,	ePANEL_DIM_Level47,	ePANEL_DIM_Level48,	ePANEL_DIM_Level49,	ePANEL_DIM_Level50,
	ePANEL_DIM_Level51,	ePANEL_DIM_Level52,	ePANEL_DIM_Level53,	ePANEL_DIM_Level54,	ePANEL_DIM_Level55,
	ePANEL_DIM_Level56,	ePANEL_DIM_Level57,	ePANEL_DIM_Level58,	ePANEL_DIM_Level59,	ePANEL_DIM_Level60,
	ePANEL_DIM_Level61,	ePANEL_DIM_Level62,	ePANEL_DIM_Level63,	ePANEL_DIM_Level64,	ePANEL_DIM_Level65,
	ePANEL_DIM_Level66,	ePANEL_DIM_Level67,	ePANEL_DIM_Level68,	ePANEL_DIM_Level69,	ePANEL_DIM_Level70,
	ePANEL_DIM_Level71,	ePANEL_DIM_Level72,	ePANEL_DIM_Level73,	ePANEL_DIM_Level74,	ePANEL_DIM_Level75,
	ePANEL_DIM_Level76,	ePANEL_DIM_Level77,	ePANEL_DIM_Level78,	ePANEL_DIM_Level79,	ePANEL_DIM_Level80,
	ePANEL_DIM_Level81,	ePANEL_DIM_Level82,	ePANEL_DIM_Level83,	ePANEL_DIM_Level84,	ePANEL_DIM_Level85,
	ePANEL_DIM_Level86,	ePANEL_DIM_Level87,	ePANEL_DIM_Level88,	ePANEL_DIM_Level89,	ePANEL_DIM_Level90,
	ePANEL_DIM_Level91,	ePANEL_DIM_Level92,	ePANEL_DIM_Level93,	ePANEL_DIM_Level94,	ePANEL_DIM_Level95,
	ePANEL_DIM_Level96,	ePANEL_DIM_Level97,	ePANEL_DIM_Level98,	ePANEL_DIM_Level99,	ePANEL_DIM_Level100,
	ePANEL_DIM_LevelMin = ePANEL_DIM_Level01,
	ePANEL_DIM_LevelMax = ePANEL_DIM_Level100
} PanelDimmingLevel_t;

typedef	enum
{
	ePANEL_DISP_AlwaysOn,
	ePANEL_DISP_AutoDisable
} PanelDisplayType_t;

typedef enum
{
	eDIM_MENUITEM_Type03 = 0,	// Item 3개 : Off, Dark, Bright : Default Menu Item
//	eDIM_MENUITEM_Type11,		// Item 11개 : Off, 1, 2, 3, ..., 8, 9, Max
//	eDIM_MENUITEM_Type21,		// Item 21개 : Off, 1, 2, 3, ..., 18, 19, Max
	eDIM_MENUITEM_TypeMax
} DimMenuItemKind_t;

#if 0
typedef	enum
{
	ePANEL_DIM_OFF		= ePANEL_DIM_Level00,
	ePANEL_DIM_DARK		= ePANEL_DIM_Level49,
	ePANEL_DIM_BRIGHT	= ePANEL_DIM_LevelMax
} PanelDimMode_t;
#endif

typedef enum
{
	ePANEL_STR_PRIORITY_BG_ACTION		=	0,
	ePANEL_STR_PRIORITY_VIEW_ACTION,
	ePANEL_STR_PRIORITY_POPUP,
	ePANEL_STR_PRIORITY_WEB,
	ePANEL_STR_PRIORITY_MEDIA,
	ePANEL_STR_PRIORITY_EPG,
	ePANEL_STR_PRIORITY_MENU,
	ePANEL_STR_PRIORITY_WZD,
	ePANEL_STR_PRIORITY_CHLIST,
	ePANEL_STR_PRIORITY_MENU_POPUP,
	ePANEL_STR_PRIORITY_OTA,
	ePANEL_STR_PRIORITY_REC_ACTION,
	ePANEL_STR_PRIORITY_VOLUME,
	ePANEL_STR_PRIORITY_TOP,
	NUM_MAX_PanelStringPriority_e
} PanelStringPriority_e;

typedef enum
{
	ePANEL_STR_File = 0,
	ePANEL_STR_PlayStart,
	ePANEL_STR_Play,
	ePANEL_STR_Pause,
	ePANEL_STR_FFX2,
	ePANEL_STR_FFX4,
	ePANEL_STR_FFX8,
	ePANEL_STR_FFX16,
	ePANEL_STR_FFX32,
	ePANEL_STR_FBX2,
	ePANEL_STR_FBX4,
	ePANEL_STR_FBX8,
	ePANEL_STR_FBX16,
	ePANEL_STR_FBX32,
	ePANEL_STR_SLOWX2,
	ePANEL_STR_SLOWX4,
	ePANEL_STR_SLOWX8,
	ePANEL_STR_NoService,
	ePANEL_STR_Menu,
	ePANEL_STR_OTA,
	ePANEL_STR_Installation,
	ePANEL_STR_ChannelList,
	ePANEL_STR_Guide,
	ePANEL_STR_Music,
	ePANEL_STR_Photo,
	ePANEL_STR_Video,
	ePANEL_STR_Record,
	ePANEL_STR_MusicPlay,
	ePANEL_STR_PhotoPlay,

	// 여기에 추가하세요...
	ePANEL_STR_Max
} ePANEL_STR_Type_t;

#if 0
typedef enum
{
	eKeyDevice_RCU,
	eKeyDevice_FRONT,
	eKeyDevice_BROWSER	/* for WEB_BROWSER */
} KEY_Device_t;

typedef enum
{
	eKeyPress_DOWN,
	eKeyPress_UP,
	eKeyPress_REPEAT
} KEY_PressMode_e;

#endif

typedef enum
{
	ePanelType_7SEG
	,ePanelType_VFD
	,ePanelType_LCD
	,ePanelType_MAX
} ePanelType_t;


typedef struct tagPannelCapability
{
	ePanelType_t	 	type;
	HUINT32         	ulLen;
	HBOOL				bSupportDim;
	HBOOL				abLEDs[NUM_MAX_DxLED];
} Panel_Capability_t;

#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define ___wifi_param_e
#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
typedef enum {
	eInet_Addr_IPv4Address,
	eInet_Addr_BroadcastAddress,
	eInet_Addr_NetMask,
	eInet_Addr_MACaddress,
	eInet_Addr_GatewayAddress,
	eInet_Addr_DNSAddress,
} INET_ADDR_Type_t;

typedef enum {
	eInet_DNS_Primary = 0,
	eInet_DNS_Secondary,
	eInet_DNS_Tertiary,
	eInet_DNS_MAX,
} INET_DNS_Type_t;

typedef enum {
	eInet_Wifi_DevType_Auto,
	eInet_Wifi_DevType_Adhoc,		/**< Adhoc Server, ex)PC */
	eInet_Wifi_DevType_Managed,		/**< Access Point */
	eInet_Wifi_DevType_MAX,
	eInet_Wifi_DevType_Unknown,
} INET_Wifi_DevType_t;

typedef enum {
	eInet_Wlan_Dev_Auto,
	eInet_Wlan_Dev_Adhoc,
	eInet_Wlan_Dev_Infra,
	eInet_Wlan_Dev_Master,
	eInet_Wlan_Dev_Repeater,
	eInet_Wlan_Dev_Secondary,
	eInet_Wlan_Dev_Monitor,
	eInet_Wlan_Dev_MAX,
	eInet_Wlan_Dev_Unknown,
} INET_Wlan_Dev_NetType_t;

typedef enum {
	eInet_Wifi_Sec_Open = 0x0,
	eInet_Wifi_Sec_WEP = 0x0001,
//	eInet_Wifi_Sec_CISCO = 0x0002,
	eInet_Wifi_Sec_WPA = 0x0010,
	eInet_Wifi_Sec_WPA2= 0x0100,
	eInet_Wifi_Sec_WPA_WPA2 = 0x0110,
	eInet_Wifi_Sec_UnKnown = 99,
} INET_Wifi_Security_Type_t;

typedef enum {
	eInet_Wifi_Auth_Open = 0x0,
	eInet_Wifi_Auth_Shared,
	eInet_Wifi_Auth_WEPauto,
	eInet_Wifi_Auth_WPAPSK,
	eInet_Wifi_Auth_WPA2PSK,
	eInet_Wifi_Auth_802_1x,
	eInet_Wifi_Auth_Type_MAX,
	eInet_Wifi_Auth_UnKnown,
} INET_Wifi_Authen_Type_t;

typedef enum tag_Wifi_encrypt {
	eINet_Wifi_Encrypt_None = 0x0,	/**< needs to 10 hex digits shared key */

	eINet_Wifi_Encrypt_WEP_64 = 0x01,	/**< needs to 10 hex digits shared key */
	eINet_Wifi_Encrypt_WEP_128 = 0x02,	/**< needs to 26 hex digits shared key */
	eINet_Wifi_Encrypt_WRAP = 0x03,
	eINet_Wifi_Encrypt_WPA_TKIP = 0x0010,
	eINet_Wifi_Encrypt_WPA_AES = 0x0100,
	eINet_Wifi_Encrypt_WPA_TKIP_AES = 0x0110,

	eINet_Wifi_Encrypt_WEP_64_Hex = 11,
	eINet_Wifi_Encrypt_WEP_64_Ascii = 0x01,
	eInet_Wifi_Encrypt_WEP_128_Hex = 21,
	eInet_Wifi_Encrypt_WEP_128_Ascii = 0x02,

//	eINet_Wifi_Encrypt_Type_MAX,
	eINet_Wifi_Encrypt_Type_UnKnown = 99,
} INet_Wifi_Encypt_t;

typedef enum {
	eInet_Wifi_WPS_NONE,
	eInet_Wifi_WPS_PBC,
	eInet_Wifi_WPS_PIN,
	eInet_Wifi_WPS_UNKNOWN,
	eInet_Wifi_WPS_MAX,
} INet_Wifi_WPS_t;

typedef enum {
	eCM_NONE = 0x00,
	eCM_OTHER,
	eCM_NOT_READY,
	eCM_NOT_SYNC,
	eCM_PHY_SYNC_DONE,
	eCM_PARAM_ACQUIRED,
	eCM_RANGING_COMPLETE,
	eCM_IP_COMPLETE,
	eCM_TOD_CERTIFY,
	eCM_SECURITY_CERTIFY,
	eCM_PARAM_TRAANSE_COMPLETE,
	eCM_REGIST_COMPLETE,
	eCM_OPERATION,
	eCM_ACCESS_DENIED,
} INET_CM_Status_t;

typedef enum {
	eCM_DOWN_MOD_QAM64,
	eCM_DOWN_MOD_QAM256,
	eCM_DOWN_MOD_OTHER,
	eCM_DOWN_MOD_UNKNOWN,
} INET_CM_DownMod_t;

typedef enum {
	eCM_UP_MOD_TDMA,
	eCM_UP_MOD_ATDMA,
	eCM_UP_MOD_SCDMA,
	eCM_UP_MOD_TDMA_ATDMA,
	eCM_UP_MOD_UNKNOWN,
} INET_CM_UpMod_t;

typedef enum {
	eCM_ATTR_STATUS = 0,

	eCM_ATTR_DOWN_FREQ,
	eCM_ATTR_DOWN_MOD,
	eCM_ATTR_DOWN_SIGLEVEL,
	eCM_ATTR_DOWN_BER,
	eCM_ATTR_DOWN_SNR,

	eCM_ATTR_UP_FREQ,
	eCM_ATTR_UP_MOD,
	eCM_ATTR_UP_CHID,
	eCM_ATTR_UP_BANDWIDTH,
	eCM_ATTR_UP_SIGLEVEL,

	eCM_ATTR_UNKNOWN,
	eCM_ATTR_MAX,
} INET_CM_Attr_e;

/* DHCP Server IP type */
typedef enum {
	eDHCPD_IPv4 = 0x00,
	eDHCPD_IPv6,
	eDHCPD_IPv4_IPv6,
	eDHCPD_IPver_Max,
} INET_DHCPD_IpVer_t;

#define	NETWORK_INTERFACE_NAME_MAX		16
#define	WLAN_NAME_MAX		48
#define	WLAN_MAC_MAX		6

#define INET_DOWN_PATH_MAX		44
#define INET_ADDR_TYPE_MAX		4		// IP, MASK, GATEWAY, DNS
#define INET_ADDR_LENGTH		4		// NET_ADDR_LENGTH
typedef struct NET_IP_SETTING_Tag
{
	HUINT8		aucIp[INET_ADDR_LENGTH];
	HUINT8		aucMask[INET_ADDR_LENGTH];
	HUINT8 		aucGateway[INET_ADDR_LENGTH];
	HUINT8		aucDns[INET_ADDR_LENGTH];
} INET_IP_SETTING_t;

typedef enum
{
	eINET_IP_CONFIG_DHCP = 0,
	eINET_IP_CONFIG_MANUAL
} INET_IP_CONFIG_e;

#define FS_MAX_FILE_NAME_LEN						256

#define PAL_PVR_MAX_FILE_NAME_LENGTH 				255
#define PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH		255
#define PAL_PVR_CRYPTO_KEY_LEN						16
#define PAL_PVR_CRYPTO_IV_LEN						16

#ifdef WIN32
#define PAL_MAX_REC_NUM								CONFIG_MW_MM_MAX_REC_NUM
#define PAL_MAX_PB_NUM								(NUM_VIEW_ACTION + NUM_BG_ACTION)
#define PAL_MAX_TSR_NUM								CONFIG_MW_MM_MAX_TSRREC_NUM
#define PAL_MAX_REC_INST_NUM						(PAL_MAX_REC_NUM + PAL_MAX_TSR_NUM)
#else
#define PAL_MAX_REC_NUM								CONFIG_MW_MM_MAX_REC_NUM
#define PAL_MAX_PB_NUM								(NUM_VIEW_ACTION + NUM_BG_ACTION)
#define PAL_MAX_TSR_NUM								CONFIG_MW_MM_MAX_TSRREC_NUM
#define PAL_MAX_REC_INST_NUM						(PAL_MAX_REC_NUM + PAL_MAX_TSR_NUM)
#endif

#define PAL_PVR_HIDDEN_TSR_DIR_NAME					".tsr"


/* filter */
#define EXT_ALL_FILE						".*"

/* Ext File Name */
#define EXT_TS_FILE							".ts"
#define EXT_INDEX_FILE						".nts"

#define EXT_JMETA_TSMPFILE					".hjtsm"
#define CONFIG_USE_HJM
#ifdef CONFIG_USE_HJM
#define EXT_META_FILE						".hjm"
#else
#define EXT_META_FILE						".hmt"
#endif
#define EXT_THUMBNAIL_FILE					".png"

#define EXT_JPEG_FILE						".jpg"
#define EXT_AVI_FILE						".avi"
#define EXT_MP3_FILE						".mp3"
#define EXT_DIR_ONLY						".DIR"
#define EXT_MEDIA_INFO_FILE					".hmi"

#define EXT_FOR_CPWIZ						"cpwiz"		// specialized for copy wizard

#define SERIES_FOLDER_META_FILE				".series"


typedef	enum
{
	eSTREAMTYPE_VIDEO,
	eSTREAMTYPE_AUDIO
} StreamType_e;


/* CI Slot Number. */
#define PAL_MAX_CISLOT_NUMBER	2	/* 실제로 달려 있는 슬롯이 하나라도 이 값을 1로 변경하지는 말기. CI 모듈에서는 항시 2개까지 처리할 수 있도록 고려한다. see CI_MAX_NUM_OF_CAM */

typedef unsigned short CI_SLOT_ID; /* INVALID_CI_SLOT_ID 로 초기화되어야 한다 */
#define INVALID_CI_SLOT_ID	(0xFFFF)


// Moved From pal_fs.h
#define VOLUUIDSZ						36			/* maybe 36, unparsed */


#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define ___feature_params____
#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

/**
 *	@defgroup	DEFAULT VALUES
 *				osd transparency(eOSD_Trans_alphaLevel01=30%)
 */
typedef enum
{ /* [WARN] forbid to change a db-saving enum value, [WARN] 절대 ifdef로 추가 하지 말것. 보이면 바로 지울 것임 */
	eOSD_Trans_alphaLevel00	= 0,
	eOSD_Trans_alphaLevel01	= 1,
	eOSD_Trans_alphaLevel02	= 2,
	eOSD_Trans_alphaLevel03	= 3,
	eOSD_Trans_alphaLevel04	= 4,	// Kraken -> 4단계로 조절됨.
	eOSD_Trans_alphaMAX
} OSD_Transparency_t;


typedef enum
{
	eSRCHSVC_DONTADD = 0,

	eSRCHSVC_ADD,
	eSRCHSVC_UPDATE,
	eSRCHSVC_UPDATE_CHANGING_SVCNUM,
	eSRCHSVC_SKIP,
	eSRCHSVC_DELETE

} SRCHSVC_AppendFlag_t;

/*
	AP에서 주로 사용하는 ChListUiGroup_t이 여기 있는 이유는 default 값 지정을 위해서 MW에서 사용하기 때문
	db_param.h 에 넣기는 AP에서 db_setup.h를 항상 include하는 것이 아니므로,
	여기에 놔두는 것이 제일 좋을 거 같음
	더 좋은 방법이 있으면 옮겨도 됨, 090124, yh
*/
/** @brief SVC DB가 아닌 UI관점의 channel list group이다. service 검색 시 해당 그룹으로 grouping할 수 있다. */
typedef enum
{ /* [WARN] forbid to change a db-saving enum value, [WARN] 절대 ifdef로 추가 하지 말것. 보이면 바로 지울 것임 */
	eChListUiGroup_TV		= 0,		/**<< TV */
	eChListUiGroup_RADIO	= 1,		/**<< Radio */
	eChListUiGroup_HDTV		= 2,		/**<< HDTV */
	eChListUiGroup_FAV_1	= 3,		/**<< Favorite 1 */
	eChListUiGroup_FAV_2	= 4,		/**<< Favorite 2 */
	eChListUiGroup_FAV_3	= 5,		/**<< Favorite 3 */
	eChListUiGroup_FAV_4	= 6,		/**<< Favorite 4 */
	eChListUiGroup_FAV_5	= 7,		/**<< Favorite 5 */
	eChListUiGroup_RECENT	= 8,		/**<< 가장 최근에 본 서비스 리스트 */
	eChListUiGroup_All		= 9,		/**<< TV와 radio를 합친 그룹. $주의 : Data, subfeed, replacement, hidden등의 suplementary 제외$ */
	eChListUiGroup_CAS		= 10,		/**<< CAS 그룹 */
	eChListUiGroup_TRASH	= 11,		/**<< TRASH CAN 그룹 */
	eChListUiGroup_SkyDGenre	= 12,	/*	SKYD Group	Guide 에서만 사용 */
	eChListUiGroup_NtvPlus		= 13,	/*  Ntv Plus(for Russian, Uzbek) */
	eChListUiGroup_FSatGenre	= 14,	/*	Greesat Group에서 사용 */
	eChListUiGroup_BOUQUET	= 15,		/*  아래 KDG에서 새로 필요한 Groups */
	eChListUiGroup_CATEGORY	= 16,
	eChListUiGroup_FAVOURITE	= 17,
	eChListUiGroup_ALPHABET		= 18,
	eChListUiGroup_ProviderName = 19,	/** Service Descriptor 의 특정 Provider Name 을 Service 그룹으로 만든다. 이를 테면 ADTV, Aljazeera ... */
	eChListUiGroup_AlphabetAll = 20,	/** TV/RADIO ALL 채널을 보여준다. 사용자의 filter 변경으로 A, B, ..., Z, 0~9, Etc 로 변경*/

	eChListUiGroup_National		= 21,		/**<< Digiturk UI Group*/
	eChListUiGroup_News 		= 22,		/**<< Digiturk UI Group*/
	eChListUiGroup_Movie_Series		= 23,		/**<< Digiturk UI Group*/
	eChListUiGroup_Sports 		= 24,		/**<< Digiturk UI Group*/
	eChListUiGroup_Kids 			= 25,		/**<< Digiturk UI Group*/
	eChListUiGroup_Music 		= 26,		/**<< Digiturk UI Group*/
	eChListUiGroup_Documentary 	= 27,		/**<< Digiturk UI Group*/
	eChListUiGroup_Entertainment_Life 	= 28,		/**<< Digiturk UI Group*/
	eChListUiGroup_World		= 29,		/**<< Digiturk UI Group*/
	eChListUiGroup_Erotic		= 30,		/**<< Digiturk UI Group*/
	eChListUiGroup_TivuSat	= 31,		/**<< Italy Tivusat 그룹 */
	eChListUiGroup_JapanAll	= 32,		/**<< Japan UI Group (TV | RADIO | DATA) */
	eChListUiGroup_JapanRadioData = 33,	/**<< Japan UI Group (RADIO | DATA) */
	eChListUiGroup_NtvGrpStart = 34,
	eChListUiGroup_NtvGrpEnd	= eChListUiGroup_NtvGrpStart + 20,	/*  Ntv Plus Group 20:maximum */
	eChListUiGroup_MAX,

	eChListUiGroup_CURRENT		= -1	/* 현재의 채널 리스트 그룹으로 지정 */
} ChListUiGroup_t;


/* Favourite 그룹 수 (1~5) 5 */
#define		NUMBER_OF_FAV_GROUP		5

/*
	edit preferred list를 여러 모듈에서 사용하기 위해서 검색 조건을 확장한다.
	해당 조건에 맞게, group의 serivce list 검색 조건이 edit channel 내부 모듈에서 configuration된다.
*/
typedef enum
{
	eEditChLoadingCondition_Default,
	eEditChLoadingCondition_ApsNewServiceFound
} EditChLoadingCondition_t;

// Filter를 걸거나 하는 작업에는 전부 필요한 선언

#define FILTER_ID_NULL			0xFFFF

typedef enum
{
	eSI_TABLE_SKIPPED = 0,
	eSI_TABLE_WAITING,
	eSI_TABLE_RECEIVED,
	eSI_TABLE_CHANGED,
	eSI_TABLE_TIMEOUT,
	eSI_TABLE_RUNING,
	eSI_TABLE_FAIL
} eFilterTableStatus_t;

#define ACT_HEAD				0
#define OTH_HEAD				1

#define ACT_PF_HEAD				0
#define OTH_PF_HEAD				1
#define ACT_SC_HEAD				2
#define OTH_SC_HEAD				3
#define OTH_PFEXT_HEAD			4
#define OTH_SCEXT_HEAD			5

#define	MAX_SECTION_LIMIT		(unsigned) 4096
#define	MAX_DESCRIPTOR_LIMIT	(unsigned) 255

#define	MAX_SI_RAWSEC_NUM		4000

#define SECTION_BYTES_SKIP		3
#define SECTION_BYTES_CRC		4
#define SECTION_BYTES_TABLEID	1

//=================================================
// For SI retrieving
// ================================================

typedef	struct tagSI_RETRIEVE_PARAM
{
	HUINT32		hAction;
	HUINT16		usTsUniqId;
	HUINT8		ucDemuxId;
	HUINT16		usPid;
	HUINT8		ucMode;
	HUINT8		ucHead;
	HUINT8		ucTimeoutMode;
	HBOOL		bTableFilter;
	HBOOL		bUseExtId;
	HBOOL		bUseVerNum;
	HBOOL		bCrc;
	HUINT16		usExtId;
	HUINT8		ucVerNum;
	HINT32		nSecCntInPackage;
	HUINT32		nDurationInPackage;
} SiRetrieveParam_t;

//=================================================
// For PSI Table
// ================================================

/** @brief PAT Section */
typedef struct tagPAT_PROGINFO
{
	HUINT16		usProgNum;
	HUINT16		usProgMapPid;
} PatProgInfo_t;

typedef struct tagPAT_SECTION_INFO
{
	HUINT16	usTsId;
	HUINT8	ucVer;
	HUINT32	ulCrc32;
}PatSectionInfo_t;

/** @brief PMT Section */
typedef struct tagPMT_ELEMENT_INFO
{
	HUINT8	ucStreamType;
	HUINT16	usElemPid;
} PmtElementInfo_t, *pPmtElementInfo_t;

typedef struct tagPMT_PROGRAM_INFO
{
	HUINT16	usProgNum;
	HUINT8	ucVer;
	HBOOL	bCurNextInd;
	HUINT16	usPCRPid;
	HUINT32 ulCrc32;
} PmtProgramInfo_t;


// Installation UI에서 Search 종류
typedef enum
{
	// Basic
	eSearchKind_Automatic	= 0x0001,	// For Sat, Cab, Ter
	eSearchKind_Manual		= 0x0002,	// For Sat, Cab, Ter
	// Sat	: 0x01XX
	// Cab  : 0x02XX
	eSearchKind_Linear		= 0x0204	// For Cable
	// Ter  : 0x04XX
} eSearchKind_t;

#define	SEARCH_KIND_SAT_DEFAULT			(eSearchKind_Automatic|eSearchKind_Manual)
#define	SEARCH_KIND_CAB_DEFAULT			(eSearchKind_Automatic|eSearchKind_Manual)
#define	SEARCH_KIND_TER_DEFAULT			(eSearchKind_Automatic|eSearchKind_Manual)

#define	SEARCH_KIND_CHECK_AUTO(eSearchKind)			((eSearchKind&eSearchKind_Automatic) ? TRUE : FALSE)
#define	SEARCH_KIND_CHECK_MAN(eSearchKind)			((eSearchKind&eSearchKind_Manual) ? TRUE : FALSE)
#define	SEARCH_KIND_CHECK_LINEAR(eSearchKind)		((eSearchKind&eSearchKind_Linear) ? TRUE : FALSE)

typedef enum
{
	eAP_UI_DisplayVideoAspectRatio,
	eAP_UI_DisplaySubtitleTypeIconOnSubtitleList,
	eAP_UI_SupportLockUnlockInsteadOfDeleteMoveFromEditChannel,
	eAP_UI_RemoveDeleteFromEditChannelOption,
	eAP_UI_RemoveRenameFromEditChannelOption,
	eAP_UI_DeleteServiceListBeforeSearch,
	eAP_UI_SupportHumaxportal,
} eApUiSpec_t;

// TS 102 323와 DBOOK에 정의된 CRID_TYPE
typedef enum {
#if	0
	CRID_TYPE_PROG				= 0x01,
	CRID_TYPE_SERIES			= 0x02,
	CRID_TYPE_RECOMMEND		= 0x03,
	CRID_TYPE_ALL				= 0x0F
#else	//	UK Freesat specification and ZIGGO testing
	CRID_TYPE_PROG			= 0x31,
	CRID_TYPE_SERIES			= 0x32,
	CRID_TYPE_RECOMMEND		= 0x33,
	CRID_TYPE_ALL				= 0x3F
#endif
} CRID_TYPE;

//	CRID length for TV Anytime Descriptor (these parametes are used in EPG, RSV and SI also
#define	EPG_TVA_CRID_AUTH_LEN								32
#define	EPG_TVA_CRID_DATA_LEN								29
#define	EPG_TVA_CRID_IMI_LEN								3
// EPG_TVA_CRID_TOTAL_LEN 의 값은 APPKINT_FREESAT.H 의 APK_EPG_TVANYTIME_CRID_TOTAL_LEN 와 동일할것.
#define	EPG_TVA_CRID_TOTAL_LEN							(EPG_TVA_CRID_AUTH_LEN+EPG_TVA_CRID_DATA_LEN+EPG_TVA_CRID_IMI_LEN+3)
#define	EPG_TVA_CRID_PROGLIST_LEN							1024
#define	EPG_TVA_CRID_LEN										(EPG_TVA_CRID_AUTH_LEN + EPG_TVA_CRID_DATA_LEN + EPG_TVA_CRID_IMI_LEN)
#define	EPG_TVA_NUM_RECORDED_PROG_SAVE						5 	/* save already-recorded program crid to prevent to record it again */
#define	RSV_TVA_NUM_TO_RECORD_PROG_SAVE					20 	/* save program to record to prevent conflict on making initial series set after booting */



/* NTVPLUS Menu: UHF Tuning 관련 */
typedef enum
{ /* [WARN] forbid to change a db-saving enum value */
	ePALStandard_G  = 0,
	ePALStandard_D,
	ePALStandard_K,
	ePALStandard_I,
	ePALStandard_B,
	ePALStandard_Max
}	PAL_Standard_t;

typedef enum
{ /* [WARN] forbid to change a db-saving enum value */
	eUHFOffset_Minus_4,
	eUHFOffset_Minus_3,
	eUHFOffset_Minus_2,
	eUHFOffset_Minus_1,
	eUHFOffset_Zero,
	eUHFOffset_Plus_1,
	eUHFOffset_Plus_2,
	eUHFOffset_Plus_3,
	eUHFOffset_Plus_4,
	eUHFOffset_Max
}	UHF_Offset_t;

typedef enum
{
	ePixelFormat_ARGB_8888,
	ePixelFormat_RGBA_8888,
	ePixelFormat_ARGB_4444,
	ePixelFormat_RGBA_4444,
	ePixelFormat_ARGB_1555,
	ePixelFormat_RGB_565,
	ePixelFormat_CLUT_8,
	ePixelFormat_A_8,
	ePixelFormat_YCBCR_444,
	ePixelFormat_YCBCR_422
} PixelFormat_e;

// bluetooth 관련
/* bluetooth device의 연결 상태 */
typedef enum {
	eBT_Status_NotPaired,
	eBT_Status_Connected,
	eBT_Status_Disconnected,
	eBT_Status_Unknown,
	eBT_Status_PinRequested,
	eBT_Status_ProfileStarted
} BT_Status_t;

/* device type */
typedef enum {
	eBT_DevType_Misc,
	eBT_DevType_Computer,
	eBT_DevType_Phone,
	eBT_DevType_Network,
	eBT_DevType_AV,
	eBT_DevType_Peripheral,
	eBT_DevType_Imaging,
	eBT_DevType_Unknown,
} BT_DevType_t;

/* profile type */
typedef enum {
	eBT_Profile_Unknown	= 0x00000000,
	eBT_Profile_A2DP		= 0x00000001, 	/* Adavanced Audio Distribution */
	eBT_Profile_AVRCP		= 0x00000002,	/* Audio/Video Remote Control */
	eBT_Profile_HID			= 0x00000004,	/* Human Interface Device */
	eBT_Profile_All			= 0x0000FFFF,
	eBT_Profile_Auto		= 0x80000000,
} BT_Profile_t;

/* HLS file type */
typedef enum _HLS_FileType
{
	eHLS_FileType_None = 0,

	/* root m3u8 */
	eHLS_FileType_RootMask = 0x1000,
	eHLS_FileType_LiveRoot,
	eHLS_FileType_RecordRoot,
	eHLS_FileType_MusicRoot,
	eHLS_FileType_VideoRoot,
	eHLS_FileType_PhotoRoot,

	/* bitrate m3u8 */
	eHLS_FileType_BitrateMask = 0x2000,
	eHLS_FileType_LiveM3U8,
	eHLS_FileType_RecordM3U8,
	eHLS_FileType_MusicM3U8,
	eHLS_FileType_VideoM3U8,
	eHLS_FileType_PhotoM3U8,

	eHLS_FileType_Max
} HLS_FileType_t;


// From EPG
#define		EPG_COMPONENT_DESC_MAX							10

#ifndef __PAL_FS_H__
#define __PAL_FS_H__

#define PAL_FS_MODE_READ			0x00000010
#define PAL_FS_MODE_WRITE			0x00000020
#define PAL_FS_MODE_RDWR			0x00000100
#define PAL_FS_MODE_APPEND			0x00000200
#define PAL_FS_MODE_TRUNC			0x00000400
#define PAL_FS_MODE_NONBLOCK		0x00000800
#define PAL_FS_MODE_O_DIRECT		0x00001000
#define PAL_FS_MODE_O_LARGEFILE		0x00002000

/* directory mask */
#define PAL_FS_DIRINFO_ALL							0xFFFFFFFF
#define PAL_FS_DIRINFO_FILE							0x0000FFFF
#define PAL_FS_DIRINFO_DIR							0xFFFF0000

#define PAL_FS_DIRINFO_VIDEO						0x00000001
#define PAL_FS_DIRINFO_PLAYLIST						0x00000002
#define PAL_FS_DIRINFO_MOVIE						0x00000010
#define PAL_FS_DIRINFO_MUSIC						0x00000020
#define PAL_FS_DIRINFO_IMAGE						0x00000040

#if WIN32
#define NAME_MAX			256
#endif

#ifndef NAME_MAX
#include <limits.h>
#endif

typedef char				PAL_FS_INT8;
typedef short				PAL_FS_INT16;
typedef long				PAL_FS_INT32;
typedef long long			PAL_FS_INT64;
typedef unsigned char		PAL_FS_UINT8;
typedef unsigned short		PAL_FS_UINT16;
typedef unsigned long		PAL_FS_UINT32;
typedef unsigned long long	PAL_FS_UINT64;
typedef int					PAL_FS_BOOL;
typedef int					PAL_FS_ResultCode_t;
typedef int					PAL_FS_PARTITION_NUMBER_t;


enum
{
	PAL_FS_POS_START,
	PAL_FS_POS_CURRENT,
	PAL_FS_POS_END
};

// File System Error codes
enum
{
	PAL_FS_EOF_ERR					= 0xA100,
	PAL_FS_DISK_ERROR,
	PAL_FS_REACH_READ_POINTER,
	PAL_FS_DISK_FULL,
	PAL_FS_SEEK_OUTSIDE_FILE,
	PAL_FS_NOT_FORMATTED_DISK_ERR,
	PAL_FS_DISK_NOT_EXIST_ERR,
	PAL_FS_FILE_NOT_EXIST_ERR,
	PAL_FS_FOLDER_NOT_EXIST_ERR,
	PAL_FS_ERROR_FILE_IN_USE,
	PAL_FS_NOT_SUPPORTED_CMD_ERR,
	PAL_FS_BAD_PARAMETER_ERR,
	PAL_FS_OPERATION_PROTECT_ERR,
	PAL_FS_READ_ONLY_ERR,
	PAL_FS_ERROR_INVALID_HANDLE,
	PAL_FS_ERROR_INVALID_OPENMODE,
	PAL_FS_ERROR_NO_MEMORY,
	PAL_FS_ERROR_NO_MORE_RESOURCES,
	PAL_FS_ERROR_SECURITY_LOCKED,
	PAL_FS_ERROR_INVALID_VALUE,
	PAL_FS_ERROR_FORK,
	PAL_FS_ERROR_FORK_PID,
	PAL_FS_ERROR_FILE_OPEN,
	PAL_FS_ERROR_FILE_READ,
	PAL_FS_ERROR_FILE_WRITE,
	PAL_FS_ERROR_FILE_CLOSE,
	PAL_FS_ERROR_IO,					// EIO
	PAL_FS_ERR_OUTOFMEMORY,
} PAL_FS_ERR_CODE;


enum
{
	PAL_FS_INFO_NAMETYPE,
	PAL_FS_INFO_EXCEPTHIDDEN,
	PAL_FS_INFO_ALLLIST,
	PAL_FS_INFO_RECURSIVEALL,
	PAL_FS_INFO_TIME
};

enum
{
	PAL_FS_FILE_TYPE_FILE,
	PAL_FS_FILE_TYPE_DIR,
	PAL_FS_FILE_TYPE_CHAR_DEV,
	PAL_FS_FILE_TYPE_BLOCK,
	PAL_FS_FILE_TYPE_FIFO,
	PAL_FS_FILE_TYPE_LINK,
	PAL_FS_FILE_TYPE_SOCKET,
	PAL_FS_FILE_TYPE_UNKNOWN
};

typedef enum
{
	ePAL_FS_FILETYPE_LINEAR,
	ePAL_FS_FILETYPE_CIRCULAR
} PalFs_FileType_e;

typedef enum
{
	ePAL_FS_START_POS_BEGIN,
	ePAL_FS_START_POS_LASTVIEW,
	ePAL_FS_START_POS_END
} PalFs_StartPosition_e;


typedef struct
{
	unsigned int 		ulSender;			// PVR_Device : from recorder or player
	unsigned int 		ulInstanceId;		// instance id
	unsigned int	   	ulEvent;			// PAL_PVR_EVENT
	unsigned int		ulParam1;
	unsigned int		ulParam2;
	unsigned int 		ulParam3;
} PalFs_Msg_t;

typedef struct
{
	PAL_FS_UINT8		aucFileName[NAME_MAX];
	PAL_FS_UINT32		fileType;
	PAL_FS_UINT64		ulFileSize;
	PAL_FS_UINT32		aTime;
	PAL_FS_UINT32		mTime;
	PAL_FS_UINT32		cTime;
} PAL_FS_FileStat_t;

typedef struct
{
	PAL_FS_INT32		fileNumber;
	PAL_FS_FileStat_t	*s_astfilestats;
} PAL_FS_DirStat_t;

#endif /* !__PAL_FS_H__ */




#endif /* !__STB_PARAMS_H__ */

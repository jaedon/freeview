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


#define  ___________________________Main_Menu_id_______________________________

#define		MENUID_MAIN						0x10000
#define			ITEMID_LOADER_STATUS			0x11000
#define			ITEMID_EEPROM_MANUP				0x12000
#define			ITEMID_NONCRC_TEST				0x13000
#define			ITEMID_NVRAM_PARAM				0x14000
#define			ITEMID_UPGRADE_SW				0x15000
#if defined(CONFIG_OP_DIGITURK)
#define			ITEMID_ADDITIONAL_OPTION		0x16000
#endif
#if defined(CONFIG_MW_CAS_IRDETO_LOADER_DVT_V32)
#define			ITEMID_SECURECHIP_STATUS		0x17000
#endif

#define  ___________________________Item_id_______________________________

#define		MENUID_LOADER_STATUS			0x11000

#define		MENUID_EEPROM_MANUP				0x12000
#define			ITEMID_ERASE_ENTIRE				0x12100
#define			ITEMID_ERASE_IRDETO				0x12200
#define			ITEMID_ERASE_LOADER				0x12300
#define			ITEMID_ERASE_CA					0x12400
#define			ITEMID_ERASE_REST				0x12500

#define		MENUID_NONCRC_TEST				0x13000
#define			ITEMID_COUNT_PATTERN			0x13100
#define			ITEMID_WRITE_PATTERN			0x13200
#define			ITEMID_MODIFY_BIT				0x13300
#define			ITEMID_ERASE_NCENTIRE			0x13400

#define		MENUID_NVRAM_PARAM				0x14000
#define			ITEMID_TUNING_FREQ				0x14100
#define			ITEMID_SYMBOLRATE				0x14200
#define			ITEMID_QAMMODE					0x14300
#define			ITEMID_SPECTRUM					0x14400
#define			ITEMID_DOWNLOADPID				0x14500
#define			ITEMID_DOWNLOADTABLEID			0x14600
#if defined(CONFIG_OP_DIGITURK)
#define			ITEMID_LNB_TYPE					0x14700
#else
#define			ITEMID_LNB_FRE					0x14700
#endif
#define			ITEMID_DISEQC					0x14800
#define			ITEMID_22KHZ					0x14900
#define			ITEMID_POLARISATION				0x14A00
#define			ITEMID_FEC						0x14B00
#if defined(CONFIG_MW_CAS_IRDETO_LDVT_MULTI_TUNER)
#define			ITEMID_TUNERID					0x14C00
#endif
#if defined(CONFIG_MW_CH_DVB_S2)
#define			ITEMID_TRANSSPEC				0x14D00
#define			ITEMID_MODULATION				0x14E00
#endif

#define		MENUID_UPGRADE_SW				0x15000

#if defined(CONFIG_OP_DIGITURK)
#define		MENUID_ADDITIONAL_OPTIONS		0x16000
#define			ITEMID_STANDBY_FLAG				0x16100
#define			ITEMID_LANGUAGE_CODE			0x16200
#endif

#if defined(CONFIG_MW_CAS_IRDETO_LOADER_DVT_V32)
#define		MENUID_SECURECHIP_STATUS		0x17000
#define			ITEMID_SECUREBOOTFUSE			0x17100
#define			ITEMID_CWMODEFUSE				0x17200
#define			ITEMID_JTAGFUSE					0x17300
#endif

#define		MENUID_UNDEFINED				0xFFFFF

static	Item_t		_pstItems[] =
{
	{
		ITEMID_LOADER_STATUS,
		MENUID_LOADER_STATUS,
		0,
		(HUINT8*)"Loader Status",
		_pfnItemDefaultProc,
		_pfnItemDefaultDrawProc,
	},
	{
		ITEMID_EEPROM_MANUP,
		MENUID_EEPROM_MANUP,
		0,
		(HUINT8*)"EEPROM Manipulation",
		_pfnItemDefaultProc,
		_pfnItemDefaultDrawProc,
	},
	{
		ITEMID_NONCRC_TEST,
		MENUID_NONCRC_TEST,
		0,
#if defined(CONFIG_MW_CAS_IRDETO_LOADER_DVT_V32)
		(HUINT8*)"Non-MAC Partition Test",
#else
		(HUINT8*)"Non-CRC Partition Test",
#endif
		_pfnItemDefaultProc,
		_pfnItemDefaultDrawProc,
	},
	{
		ITEMID_NVRAM_PARAM,
		MENUID_NVRAM_PARAM,
		0,
		(HUINT8*)"Loader NVRAM Parameters",
		_pfnItemDefaultProc,
		_pfnItemDefaultDrawProc,
	},
	{
		ITEMID_UPGRADE_SW,
		MENUID_UPGRADE_SW,
		0,
		(HUINT8*)"Upgrade Software",
		_pfnItemDefaultProc,
		_pfnItemDefaultDrawProc,
	},
#if defined(CONFIG_OP_DIGITURK)
	{
		ITEMID_ADDITIONAL_OPTION,
		MENUID_ADDITIONAL_OPTIONS,
		0,
		(HUINT8*)"Additional Options",
		_pfnItemDefaultProc,
		_pfnItemDefaultDrawProc,
	},
#endif
#if defined(CONFIG_MW_CAS_IRDETO_LOADER_DVT_V32)
	{
		ITEMID_SECURECHIP_STATUS,
		MENUID_SECURECHIP_STATUS,
		0,
		(HUINT8*)"Secure Chipset Status",
		_pfnItemDefaultProc,
		_pfnItemDefaultDrawProc,
	},
#endif
	{
		ITEMID_ERASE_ENTIRE,
		MENUID_UNDEFINED,
		0,
		(HUINT8*)"Erase Entire EEPROM",
		_pfnItemDefaultProc,
		_pfnItemDefaultDrawProc,
	},
	{
		ITEMID_ERASE_IRDETO,
		MENUID_UNDEFINED,
		0,
		(HUINT8*)"Erase Irdeto Partition",
		_pfnItemDefaultProc,
		_pfnItemDefaultDrawProc,
	},
	{
		ITEMID_ERASE_LOADER,
		MENUID_UNDEFINED,
		0,
		(HUINT8*)"Erase Loader Partition",
		_pfnItemDefaultProc,
		_pfnItemDefaultDrawProc,
	},
	{
		ITEMID_ERASE_CA,
		MENUID_UNDEFINED,
		0,
		(HUINT8*)"Erase CA Partition",
		_pfnItemDefaultProc,
		_pfnItemDefaultDrawProc,
	},
	{
		ITEMID_ERASE_REST,
		MENUID_UNDEFINED,
		0,
		(HUINT8*)"Erase Rest of EEPROM",
		_pfnItemDefaultProc,
		_pfnItemDefaultDrawProc,
	},
	{
		ITEMID_COUNT_PATTERN,
		MENUID_UNDEFINED,
		0,
		(HUINT8*)"Count Patterns in Partition",
		_pfnItemDefaultProc,
		_pfnItemValueDrawProc,
	},
	{
		ITEMID_WRITE_PATTERN,
		MENUID_UNDEFINED,
		0,
		(HUINT8*)"Write Pattern to Partition",
		_pfnItemDefaultProc,
		_pfnItemValueDrawProc,
	},
	{
		ITEMID_MODIFY_BIT,
		MENUID_UNDEFINED,
		0,
		(HUINT8*)"Modify Last Bit of Partition",
		_pfnItemDefaultProc,
		_pfnItemValueDrawProc,
	},
	{
		ITEMID_ERASE_NCENTIRE,
		MENUID_UNDEFINED,
		0,
		(HUINT8*)"Erase Entire Partition",
		_pfnItemDefaultProc,
		_pfnItemValueDrawProc,
	},
	{
		ITEMID_TUNING_FREQ,
		MENUID_UNDEFINED,
		0,
		(HUINT8*)"Tuning Frequency (Mhz)",
		_pfnItemDefaultProc,
		_pfnItemValueNVRAMDrawProc,
	},
	{
		ITEMID_SYMBOLRATE,
		MENUID_UNDEFINED,
		0,
		(HUINT8*)"Symbol Rate (Ksym/s)",
		_pfnItemDefaultProc,
		_pfnItemValueNVRAMDrawProc,
	},
	{
		ITEMID_QAMMODE,
		MENUID_UNDEFINED,
		0,
		(HUINT8*)"QAM Mode",
		_pfnItemDefaultProc,
		_pfnItemValueNVRAMDrawProc,
	},
	{
		ITEMID_SPECTRUM,
		MENUID_UNDEFINED,
		0,
		(HUINT8*)"Spectrum",
		_pfnItemDefaultProc,
		_pfnItemValueNVRAMDrawProc,
	},
	{
		ITEMID_DOWNLOADPID,
		MENUID_UNDEFINED,
		0,
		(HUINT8*)"Download PID (Hex)",
		_pfnItemDefaultProc,
		_pfnItemValueNVRAMDrawProc,
	},
	{
		ITEMID_DOWNLOADTABLEID,
		MENUID_UNDEFINED,
		0,
		(HUINT8*)"Download Table ID (Hex)",
		_pfnItemDefaultProc,
		_pfnItemValueNVRAMDrawProc,
	},
#ifdef	CONFIG_MW_CH_SATELLITE
#if defined(CONFIG_OP_DIGITURK)
	{
		ITEMID_LNB_TYPE,
		MENUID_UNDEFINED,
		0,
		(HUINT8*)"LNB TYPE",
		_pfnItemDefaultProc,
		_pfnItemValueNVRAMDrawProc,
	},
#else
	{
		ITEMID_LNB_FRE,
		MENUID_UNDEFINED,
		0,
		(HUINT8*)"LNB Frequency(Mhz)",
		_pfnItemDefaultProc,
		_pfnItemValueNVRAMDrawProc,
	},
#endif
	{
		ITEMID_DISEQC,
		MENUID_UNDEFINED,
		0,
		(HUINT8*)"DisEqC",
		_pfnItemDefaultProc,
		_pfnItemValueNVRAMDrawProc,
	},
	{
		ITEMID_22KHZ,
		MENUID_UNDEFINED,
		0,
		(HUINT8*)"22KHz",
		_pfnItemDefaultProc,
		_pfnItemValueNVRAMDrawProc,
	},
	{
		ITEMID_POLARISATION,
		MENUID_UNDEFINED,
		0,
		(HUINT8*)"Polarisation",
		_pfnItemDefaultProc,
		_pfnItemValueNVRAMDrawProc,
	},
	{
		ITEMID_FEC,
		MENUID_UNDEFINED,
		0,
		(HUINT8*)"FEC",
		_pfnItemDefaultProc,
		_pfnItemValueNVRAMDrawProc,
	},
#if defined(CONFIG_MW_CAS_IRDETO_LDVT_MULTI_TUNER)
	{
		ITEMID_TUNERID,
		MENUID_UNDEFINED,
		0,
		(HUINT8*)"TunerID",
		_pfnItemDefaultProc,
		_pfnItemValueNVRAMDrawProc,
	},
#endif
#if defined(CONFIG_MW_CH_DVB_S2)
	{
		ITEMID_TRANSSPEC,
		MENUID_UNDEFINED,
		0,
		(HUINT8*)"Transport Spec",
		_pfnItemDefaultProc,
		_pfnItemValueNVRAMDrawProc,
	},
	{
		ITEMID_MODULATION,
		MENUID_UNDEFINED,
		0,
		(HUINT8*)"Modulation type",
		_pfnItemDefaultProc,
		_pfnItemValueNVRAMDrawProc,
	},
#endif
#endif
#if defined(CONFIG_OP_DIGITURK)
	{
		ITEMID_STANDBY_FLAG,
		MENUID_UNDEFINED,
		0,
		(HUINT8*)"Standby Mode",
		_pfnItemDefaultProc,
		_pfnItemValueAdditionalOptionDrawProc,
	},
	{
		ITEMID_LANGUAGE_CODE,
		MENUID_UNDEFINED,
		0,
		(HUINT8*)"OSD Language",
		_pfnItemDefaultProc,
		_pfnItemValueAdditionalOptionDrawProc,
	},
#endif

#if defined(CONFIG_MW_CAS_IRDETO_SECURE) && defined(CONFIG_MW_CAS_IRDETO_LOADER_DVT_V32)
	{
		ITEMID_SECUREBOOTFUSE,
		MENUID_UNDEFINED,
		0,
		(HUINT8*)"SecureBoot Fuse",
		_pfnItemDefaultProc,
		_pfnItemSCStatusDrawProc,
	},
	{
		ITEMID_CWMODEFUSE,
		MENUID_UNDEFINED,
		0,
		(HUINT8*)"CwMode Fuse",
		_pfnItemDefaultProc,
		_pfnItemSCStatusDrawProc,
	},
	{
		ITEMID_JTAGFUSE,
		MENUID_UNDEFINED,
		0,
		(HUINT8*)"JTAG Fuse",
		_pfnItemDefaultProc,
		_pfnItemSCStatusDrawProc,
	},
#endif
};

static	MenuItem_t	_pstMenuItems[] =
{
	{
		MENUID_MAIN,
		MENUID_MAIN,
		"DVT Test Application",
		"Press up and down to select option.",
		"Press OK to confirm selection",
		"",
		NULL,
		_pfnMenuDefaultProc,
		_pfnMenuDefaultDrawProc,
#if defined(CONFIG_MW_CAS_IRDETO_LOADER_DVT_V32)
	#if defined(CONFIG_OP_DIGITURK)
		7,
		0,
		{ ITEMID_LOADER_STATUS, ITEMID_EEPROM_MANUP, ITEMID_NONCRC_TEST, ITEMID_NVRAM_PARAM,ITEMID_UPGRADE_SW, ITEMID_ADDITIONAL_OPTION, ITEMID_SECURECHIP_STATUS, },
	#else
		6,
		0,
		{ ITEMID_LOADER_STATUS, ITEMID_EEPROM_MANUP, ITEMID_NONCRC_TEST, ITEMID_NVRAM_PARAM,ITEMID_UPGRADE_SW, ITEMID_SECURECHIP_STATUS, },
	#endif
#else
	#if defined(CONFIG_OP_DIGITURK)
		6,
		0,
		{ ITEMID_LOADER_STATUS, ITEMID_EEPROM_MANUP, ITEMID_NONCRC_TEST, ITEMID_NVRAM_PARAM,ITEMID_UPGRADE_SW, ITEMID_ADDITIONAL_OPTION},
	#else
		5,
		0,
		{ ITEMID_LOADER_STATUS, ITEMID_EEPROM_MANUP, ITEMID_NONCRC_TEST, ITEMID_NVRAM_PARAM,ITEMID_UPGRADE_SW, },
	#endif
#endif //end of CONFIG_MW_CAS_IRDETO_LOADER_DVT_V32
		{ NULL, },
	},
	{
		MENUID_LOADER_STATUS,
		MENUID_MAIN,
		"Loader Status",
		"Press Exit to return the Main Menu",
		"",
		"",
		NULL,
		_pfnLoaderStatusMenuDefaultProc,
		_pfnLoaderStatusMenuDrawProc,
		0,
		0,
		{ 0, },
		{ 0, },
	},
	{
		MENUID_EEPROM_MANUP,
		MENUID_MAIN,
		"EEPROM Manipulation",
		"Press up and down to select option.",
		"Press OK to confirm selection",
		"",
		NULL,
		_pfnMenuEEPROMProc,
		_pfnMenuEEPROMDrawProc,
		5,
		0,
		{ ITEMID_ERASE_ENTIRE, ITEMID_ERASE_IRDETO, ITEMID_ERASE_LOADER, ITEMID_ERASE_CA, ITEMID_ERASE_REST, },
		{ NULL, },
	},
	{
		MENUID_NONCRC_TEST,
		MENUID_MAIN,
#if defined(CONFIG_MW_CAS_IRDETO_LOADER_DVT_V32)
		"Non-MAC Partition Test",
#else
		"Non-CRC Partition Test",
#endif
		"Press up and down to select option.",
		"Press Left and Right to modify partition.",
		"Press OK to confirm selection",
		_pfnInitNCProc,
		_pfnMenuNCProc,
		_pfnMenuNCDrawProc,
		4,
		0,
		{ ITEMID_COUNT_PATTERN, ITEMID_WRITE_PATTERN, ITEMID_MODIFY_BIT, ITEMID_ERASE_NCENTIRE, },
		{ NULL, },
	},
#ifdef	CONFIG_MW_CH_CABLE
	{
		MENUID_NVRAM_PARAM,
		MENUID_MAIN,
		"Loader NVRAM Parameters",
		"Press up and down to select option.",
		"Press Left and Right to modify partition.",
		"Press OK to confirm selection",
		_pfnMenuNVRAMInitProc,
		_pfnMenuNVRAMProc,
		_pfnMenuNVRAMDrawProc,
		6,
		0,
		{ ITEMID_TUNING_FREQ, ITEMID_SYMBOLRATE, ITEMID_QAMMODE, ITEMID_SPECTRUM, ITEMID_DOWNLOADPID, ITEMID_DOWNLOADTABLEID, },
		{ NULL, },
	},
#elif	CONFIG_MW_CH_SATELLITE
	{
		MENUID_NVRAM_PARAM,
		MENUID_MAIN,
		"Loader NVRAM Parameters",
		"Press up and down to select option.",
		"Press Left and Right to modify partition.",
		"Press OK to confirm selection",
		_pfnMenuNVRAMInitProc,
		_pfnMenuNVRAMProc,
		_pfnMenuNVRAMDrawProc,
#if defined(CONFIG_MW_CH_DVB_S2)
#if defined(CONFIG_MW_CAS_IRDETO_LDVT_MULTI_TUNER)
		12,
#else
		11,
#endif
#else
#if defined(CONFIG_MW_CAS_IRDETO_LDVT_MULTI_TUNER)
		10,
#else
		9,
#endif
#endif
		0,
		{
#if defined(CONFIG_OP_DIGITURK)
			ITEMID_LNB_TYPE,
#else
			ITEMID_LNB_FRE,
#endif
			ITEMID_DISEQC,
			ITEMID_22KHZ,
			ITEMID_TUNING_FREQ,
			ITEMID_POLARISATION,
			ITEMID_SYMBOLRATE,
			ITEMID_FEC,
			ITEMID_TRANSSPEC,
			ITEMID_MODULATION,
#if defined(CONFIG_MW_CAS_IRDETO_LDVT_MULTI_TUNER)
			ITEMID_TUNERID,
#endif
#if defined(CONFIG_MW_CH_DVB_S2)
			ITEMID_DOWNLOADPID,
			ITEMID_DOWNLOADTABLEID,
#endif
		},
		{ NULL, },
	},
#endif
	{
		MENUID_UPGRADE_SW,
		MENUID_MAIN,
		"Upgrade Software",
		"Press Exit to return to Main Menu",
		"Press OK to confirm",
		"",
		NULL,
		_pfnOTAMenuProc,
		_pfnOTAMenuDrawProc,
		0,
		0,
		{ 0, },
		{ 0, },
	},
#if defined(CONFIG_OP_DIGITURK)
	{
		MENUID_ADDITIONAL_OPTIONS,
		MENUID_MAIN,
		"Additional Options",
		"Press Exit to return to Main Menu",
		"Press OK to confirm",
		"Press Blue to erase SPCB area",
		_pfnMenuAdditionalOptionInitProc,
		_pfnMenuAdditionalOptionProc,
		_pfnMenuAdditionalOptionDrawProc,
		2,
		0,
		{ ITEMID_STANDBY_FLAG, ITEMID_LANGUAGE_CODE, },
		{ 0, },
	},
#endif
#if defined(CONFIG_MW_CAS_IRDETO_SECURE) && defined(CONFIG_MW_CAS_IRDETO_LOADER_DVT_V32)
	{
		MENUID_SECURECHIP_STATUS,
		MENUID_MAIN,
		"Secure Chipset Status",
		"Press Exit to return the Main Menu",
		"Press OK to confirm",
		"",
		_pfnMenuSCStatusInitProc,
		_pfnMenuSCStatusProc,
		_pfnMenuSCStatusDrawProc,
		3,
		0,
		{ ITEMID_SECUREBOOTFUSE, ITEMID_CWMODEFUSE, ITEMID_JTAGFUSE },
		{ NULL, },
	},
#endif
};


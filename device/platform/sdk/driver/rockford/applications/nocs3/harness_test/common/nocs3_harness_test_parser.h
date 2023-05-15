
#ifndef __NOCS3_HARNESS_TEST_PARSER_H__
#define __NOCS3_HARNESS_TEST_PARSER_H__

#ifdef __cplusplus
extern "C"
{
#endif

	/* CSD test types */
#define NOCS3_HARNESS_TEST_TYPE_R2R				"-r2r"
#define NOCS3_HARNESS_TEST_TYPE_DSC				"-dsc"
#define NOCS3_HARNESS_TEST_TYPE_SCR				"-scr"
#define NOCS3_HARNESS_TEST_TYPE_CN				"-cn"
#define NOCS3_HARNESS_TEST_TYPE_SPV				"-spv"
#define NOCS3_HARNESS_TEST_TYPE_GPV				"-gpv"
#define NOCS3_HARNESS_TEST_TYPE_APV				"-apv"
#define NOCS3_HARNESS_TEST_TYPE_FP				"-fp"
#define NOCS3_HARNESS_TEST_TYPE_STATUS			"-status"
#define NOCS3_HARNESS_TEST_TYPE_HELP			"-help"
#define NOCS3_HARNESS_TEST_TYPE_VERSION			"-v"
#define NOCS3_HARNESS_TEST_TYPE_RESET			"-rst"
#define NOCS3_HARNESS_TEST_TYPE_FLASH			"-frw"
#define NOCS3_HARNESS_TEST_TYPE_RAM				"-mrw"
#define NOCS3_HARNESS_TEST_TYPE_ATTR			"-attr"
#define NOCS3_HARNESS_TEST_TYPE_SUSPEND			"-suspend"
#define NOCS3_HARNESS_TEST_TYPE_RAMSCRAM		"-ramscramb"
#define NOCS3_HARNESS_TEST_TYPE_SCE				"-sce"
#define NOCS3_HARNESS_TEST_DSC_ECMPID			"-ecmPid"
#define NOCS3_HARNESS_TEST_DSC_PECM				"-pecm"
#define NOCS3_HARNESS_TEST_TYPE_LCHIPID			"-lchipid"
#define NOCS3_HARNESS_TEST_TYPE_CHIPID			"-chipid"
#define NOCS3_HARNESS_TEST_TYPE_LR2R			"-lr2r"
#define NOCS3_HARNESS_TEST_TYPE_RSA				"-rsa"
#define NOCS3_HARNESS_TEST_TYPE_DH				"-dh"
#define NOCS3_HARNESS_TEST_TYPE_SHA				"-sha"
#define NOCS3_HARNESS_TEST_TYPE_HMAC			"-hmac"
#define NOCS3_HARNESS_TEST_TYPE_RAND			"-rand"
#define NOCS3_HARNESS_TEST_TYPE_ECDSA			"-ecdsa"
#define NOCS3_HARNESS_TEST_TYPE_ECDH			"-ecdh"


/* Common test options */
#define NOCS3_HARNESS_TEST_OPTION_KEY			"-key"
#define NOCS3_HARNESS_TEST_OPTION_EMI			"-emi"
#define NOCS3_HARNESS_TEST_OPTION_IVC			"-ivc"
#define NOCS3_HARNESS_TEST_OPTION_TV			"-tv"
#define NOCS3_HARNESS_TEST_OPTION_ENC			"-enc"
#define NOCS3_HARNESS_TEST_OPTION_DEC			"-dec"
#define NOCS3_HARNESS_TEST_OPTION_KF			"-kf"
#define NOCS3_HARNESS_TEST_OPTION_KC			"-kc"
#define NOCS3_HARNESS_TEST_OPTION_IF			"-if"
#define NOCS3_HARNESS_TEST_OPTION_IFE			"-ife"
#define NOCS3_HARNESS_TEST_OPTION_IC			"-ic"
#define NOCS3_HARNESS_TEST_OPTION_OF			"-of"
#define NOCS3_HARNESS_TEST_OPTION_OC			"-oc"
#define NOCS3_HARNESS_TEST_OPTION_KEY_SW		"SW"
#define NOCS3_HARNESS_TEST_OPTION_KEY_KL		"KL"
#define NOCS3_HARNESS_TEST_OPTION_KEY_CERT		"CERT"
#define NOCS3_HARNESS_TEST_OPTION_VPID			"-videoPid"
#define NOCS3_HARNESS_TEST_OPTION_APID			"-audioPid"
#define NOCS3_HARNESS_TEST_OPTION_PPID			"-pcrPid"
#define NOCS3_HARNESS_TEST_OPTION_EPID			"-ecmPid"
#define NOCS3_HARNESS_TEST_OPTION_PECM			"-pecm"
#define NOCS3_HARNESS_TEST_OPTION_MT			"-mt"
#define NOCS3_HARNESS_TEST_OPTION_RUNS			"-numbruns"
#define NOCS3_HARNESS_TEST_OPTION_VALUE			"-v"
#define NOCS3_HARNESS_TEST_OPTION_SIZE			"-s"
#define NOCS3_HARNESS_TEST_OPTION_ID			"-id"
#define NOCS3_HARNESS_TEST_OPTION_HELP			"help"
#define NOCS3_HARNESS_TEST_OPTION_FK			"-fk"
#define NOCS3_HARNESS_TEST_OPTION_MASK			"-m"
#define NOCS3_HARNESS_TEST_OPTION_KEYEMI		"-keyemi"
#define NOCS3_HARNESS_TEST_OPTION_SCEMI			"-sceemi"
#define NOCS3_HARNESS_TEST_OPTION_KEYGEN		"-gen"
#define NOCS3_HARNESS_TEST_OPTION_RSACMP		"-cmp"
#define NOCS3_HARNESS_TEST_OPTION_PUB			"-pub"
#define NOCS3_HARNESS_TEST_OPTION_PVT			"-pvt"
#define NOCS3_HARNESS_TEST_OPTION_KEYSIZE		"-keysize"
#define NOCS3_HARNESS_TEST_OPTION_MODN          "-modN"
#define NOCS3_HARNESS_TEST_OPTION_EXPD          "-expD"
#define NOCS3_HARNESS_TEST_OPTION_EXPE			"-expE"
#define NOCS3_HARNESS_TEST_OPTION_PAD			"-pad"
#define NOCS3_HARNESS_TEST_OPTION_HT			"-ht"
#define NOCS3_HARNESS_TEST_OPTION_KEYCOMP       "-com"
#define NOCS3_HARNESS_TEST_OPTION_GFILE    		"-G"
#define NOCS3_HARNESS_TEST_OPTION_PFILE         "-P"
#define NOCS3_HARNESS_TEST_OPTION_IPVTK         "-ipvtk"
#define NOCS3_HARNESS_TEST_OPTION_OPVTK         "-opvtk"
#define NOCS3_HARNESS_TEST_OPTION_OPUBK         "-opubk"
#define NOCS3_HARNESS_TEST_OPTION_PPUBK         "-ppubk"
#define NOCS3_HARNESS_TEST_OPTION_SS         	"-ss"
#define NOCS3_HARNESS_TEST_OPTION_SSX         	"-ssx"
#define NOCS3_HARNESS_TEST_OPTION_SSY         	"-ssy"
#define NOCS3_HARNESS_TEST_OPTION_PUBKX         "-pubkx"
#define NOCS3_HARNESS_TEST_OPTION_PUBKY         "-pubky"
#define NOCS3_HARNESS_TEST_OPTION_OPUBKX        "-opubkx"
#define NOCS3_HARNESS_TEST_OPTION_OPUBKY        "-opubky"
#define NOCS3_HARNESS_TEST_OPTION_RANDLEN       "-n"
#define NOCS3_HARNESS_TEST_OPTION_ADDR			"-a"
#define NOCS3_HARNESS_TEST_OPTION_CL			"-cl"
#define NOCS3_HARNESS_TEST_OPTION_FT			"-ft"
#define NOCS3_HARNESS_TEST_OPTION_PARAM			"-params"
#define NOCS3_HARNESS_TEST_OPTION_SIGN			"-sign"
#define NOCS3_HARNESS_TEST_OPTION_VERIFY		"-verify"
#define NOCS3_HARNESS_TEST_OPTION_SOF         	"-sof"
#define NOCS3_HARNESS_TEST_OPTION_SIF         	"-sif"
#define NOCS3_HARNESS_TEST_OPTION_MS         	"-ms"
#define NOCS3_HARNESS_TEST_OPTION_IN        	"-in"
#define NOCS3_HARNESS_TEST_OPTION_PARITY		"-p"
#define NOCS3_HARNESS_TEST_OPTION_XP          	"-xP"
#define NOCS3_HARNESS_TEST_OPTION_XQ          	"-xQ"
#define NOCS3_HARNESS_TEST_OPTION_XDP         	"-xDP"
#define NOCS3_HARNESS_TEST_OPTION_XDQ         	"-xDQ"
#define NOCS3_HARNESS_TEST_OPTION_XQINV         "-xQInv"

#define NOCS3_HARNESS_TEST_OPTION_SROF        "-sRof"
#define NOCS3_HARNESS_TEST_OPTION_SSOF        "-sSof"
#define NOCS3_HARNESS_TEST_OPTION_SRIF        "-sRif"
#define NOCS3_HARNESS_TEST_OPTION_SSIF        "-sSif"



#define NOCS3_COMMAND_PARSER_CHECK_STATUS(field, processed) \
	if ( status==NOCS3_HARNESS_TEST_PARSER_STATUS_CODE_SUCCESS ) \
	{\
		field = true;\
		cmdline_offset +=processed;\
		bCommandProcessed = true;\
	}\
	else if (  status==NOCS3_HARNESS_TEST_PARSER_STATUS_CODE_BADPARAMS)\
	{\
		return NOCS3_HARNESS_TEST_STATUS_CODE_BAD_PARAMS;\
	}\
	if ( cmdline_offset>=argc )\
	{\
		break;\
	}

#define NOCS3_COMMAND_PARSER_PARSE_SINGLE_REQUIRED(field, name) \
	if ( field == false )\
	{\
		if ( cmdline_offset<argc ) \
		{\
			if ( strcmp (argv[cmdline_offset], name)==0 )\
			{\
				field = true;\
				cmdline_offset ++;\
				bCommandProcessed = true;\
			}\
		}\
		else \
		{\
			return NOCS3_HARNESS_TEST_STATUS_CODE_BAD_PARAMS;\
		}\
	}

#define NOCS3_COMMAND_PARSER_PARSE_SINGLE_OPTIONAL(field, name) \
	if ( field == false )\
	{\
		if ( cmdline_offset<argc ) \
		{\
			if ( strcmp (argv[cmdline_offset], name)==0 )\
			{\
				field = true;\
				cmdline_offset ++;\
				bCommandProcessed = true;\
			}\
		}\
	}

#define NOCS3_COMMAND_PARSER_CHECK_NUM_ARG(required) \
	if ( (cmdline_offset+required)>argc ) \
	{\
		return NOCS3_HARNESS_TEST_STATUS_CODE_BAD_PARAMS;\
	}

#define NOCS3_COMMAND_PARSER_PARSE_NUMBER_REQUIRED(field, name, pValue, format) \
	if ( field == false )\
	{\
			NOCS3_COMMAND_PARSER_CHECK_NUM_ARG(2);\
			status = nocs3_harness_test_parse_number (argv[cmdline_offset], \
												argv[cmdline_offset+1], \
												name, \
												pValue, format);\
			NOCS3_COMMAND_PARSER_CHECK_STATUS(field, 2);\
	}

#define NOCS3_COMMAND_PARSER_PARSE_NUMBER_OPTIONAL(field, name, pValue, format) \
	if ( field == false )\
	{\
		if ( (cmdline_offset+1)<argc ) \
		{\
			status = nocs3_harness_test_parse_number (argv[cmdline_offset], \
												argv[cmdline_offset+1], \
												name, \
												pValue, format);\
			NOCS3_COMMAND_PARSER_CHECK_STATUS(field, 2);\
		}\
	}

#define NOCS3_COMMAND_PARSER_PARSE_ARRAY_REQUIRED(field, name, pValue, pSize, maxSize) \
	if ( field == false )\
	{\
			NOCS3_COMMAND_PARSER_CHECK_NUM_ARG(2);\
			status = nocs3_harness_test_parse_array (argv[cmdline_offset], \
												argv[cmdline_offset+1], \
												name, \
												pValue, \
												pSize, \
												maxSize);\
			NOCS3_COMMAND_PARSER_CHECK_STATUS(field, 2);\
	}

#define NOCS3_COMMAND_PARSER_PARSE_ARRAY_OPTIONAL(field, name, pValue, pSize, maxSize) \
	if ( field == false )\
	{\
		if ( (cmdline_offset+1)<argc ) \
		{\
			status = nocs3_harness_test_parse_array (argv[cmdline_offset], \
												argv[cmdline_offset+1], \
												name, \
												pValue, \
												pSize, \
												maxSize);\
			NOCS3_COMMAND_PARSER_CHECK_STATUS(field, 2);\
		}\
	}

#define NOCS3_COMMAND_PARSER_PARSE_ARRAY_REQUIRED_2(field, name, pValue, pSize) \
	if ( field == false )\
	{\
			NOCS3_COMMAND_PARSER_CHECK_NUM_ARG(2);\
			status = nocs3_harness_test_parse_array_2 (argv[cmdline_offset], \
												argv[cmdline_offset+1], \
												name, \
												pValue, \
												pSize);\
			NOCS3_COMMAND_PARSER_CHECK_STATUS(field, 2);\
	}

#define NOCS3_COMMAND_PARSER_PARSE_ARRAY_OPTIONAL_2(field, name, pValue, pSize) \
	if ( field == false )\
	{\
		if ( (cmdline_offset+1)<argc ) \
		{\
			status = nocs3_harness_test_parse_array_2 (argv[cmdline_offset], \
												argv[cmdline_offset+1], \
												name, \
												pValue, \
												pSize);\
			NOCS3_COMMAND_PARSER_CHECK_STATUS(field, 2);\
		}\
	}

#define NOCS3_COMMAND_PARSER_PARSE_FILE_REQUIRED(field, name, pFile, pMode) \
		if ( field == false )\
		{\
			NOCS3_COMMAND_PARSER_CHECK_NUM_ARG(2);\
			status = nocs3_harness_test_parse_file (argv[cmdline_offset], argv[cmdline_offset+1], name, pFile, pMode);\
			NOCS3_COMMAND_PARSER_CHECK_STATUS(field, 2);\
		}

#define NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL(field, name, pFile, pMode) \
		if ( field == false )\
		{\
			if ( (cmdline_offset+1)<argc ) \
			{\
				status = nocs3_harness_test_parse_file (argv[cmdline_offset], argv[cmdline_offset+1], name, pFile, pMode);\
				NOCS3_COMMAND_PARSER_CHECK_STATUS(field, 2);\
			}\
		}

#define NOCS3_COMMAND_PARSER_PARSE_FILE_NAME_REQUIRED(field, name, ppFileName) \
		if ( field == false )\
		{\
			NOCS3_COMMAND_PARSER_CHECK_NUM_ARG(2);\
			if ( strcmp (argv[cmdline_offset], name)==0 )\
			{\
				int length = strlen (argv[cmdline_offset+1]);\
				*ppFileName = (char *)malloc (length*sizeof(char));\
				if (*ppFileName )\
				{\
					strcpy(*ppFileName, argv[cmdline_offset+1]);\
					status = NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS;\
				}\
				else \
				{\
					status = NOCS3_HARNESS_TEST_STATUS_CODE_FAIL;\
				}\
				NOCS3_COMMAND_PARSER_CHECK_STATUS(field, 2);\
			}\
		}

#define NOCS3_COMMAND_PARSER_PARSE_FILE_NAME_OPTIONAL(field, name, ppFileName) \
		if ( field == false )\
		{\
			if ( (cmdline_offset+1)<argc ) \
			{\
				if ( strcmp (argv[cmdline_offset], name)==0 )\
				{\
					int length = strlen (argv[cmdline_offset+1]);\
					*ppFileName = (char *)malloc (length*sizeof(char));\
					if (*ppFileName )\
					{\
						strcpy(*ppFileName, argv[cmdline_offset+1]);\
						status = NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS;\
					}\
					else \
					{\
						status = NOCS3_HARNESS_TEST_STATUS_CODE_FAIL;\
					}\
					NOCS3_COMMAND_PARSER_CHECK_STATUS(field, 2);\
				}\
			}\
		}

#define NOCS3_COMMAND_PARSER_CHECK_IF_COMMAND_PARSED \
		if ( bCommandProcessed ==false )\
		{\
			printf("!!! Illegall command at %d\n", cmdline_offset);\
			return NOCS3_HARNESS_TEST_STATUS_CODE_BAD_PARAMS;\
		}

#define NOCS3_COMMAND_PARSER_PARSE_BINARY_TYPE_REQUIRED(bParsed, field, str1, value1, str2, value2)\
	if ( bParsed == false )\
	{\
		if ( cmdline_offset<argc ) \
		{\
			if ( strcmp (argv[cmdline_offset], str1)==0 )\
			{\
				field = value1;\
				bParsed = true;\
				cmdline_offset ++;\
				bCommandProcessed = true;\
			}\
			else if ( strcmp (argv[cmdline_offset], str2)==0 )\
			{\
				field = value2;\
				bParsed = true;\
				cmdline_offset ++;\
				bCommandProcessed = true;\
			}\
		}\
		else \
		{\
			return NOCS3_HARNESS_TEST_STATUS_CODE_BAD_PARAMS;\
		}\
	}\

#define NOCS3_COMMAND_PARSER_PARSE_TWO_BINARY_TYPE_REQUIRED(bParsed, in1, field, str1, value1, str2, value2)\
	if ( bParsed == false )\
	{\
		if ( (cmdline_offset+1)<argc ) \
		{\
			if ( strcmp (argv[cmdline_offset], in1)==0 )\
			{\
				if ( strcmp (argv[cmdline_offset+1], str1)==0 )\
				{\
					field = value1;\
					bParsed = true;\
					cmdline_offset +=2;\
					bCommandProcessed = true;\
				}\
				else if ( strcmp (argv[cmdline_offset+1], str2)==0 )\
				{\
					field = value2;\
					bParsed = true;\
					cmdline_offset +=2;\
					bCommandProcessed = true;\
				}\
				else \
				{\
					return NOCS3_HARNESS_TEST_STATUS_CODE_BAD_PARAMS;\
				}\
			}\
		}\
		else \
		{\
			return NOCS3_HARNESS_TEST_STATUS_CODE_BAD_PARAMS;\
		}\
	}

#define NOCS3_HARNESS_TEST_PARSE_CRYPTO_OPERATION_REQUIRED \
	NOCS3_COMMAND_PARSER_PARSE_BINARY_TYPE_REQUIRED(bOperationParsed,\
	pParams->operation,\
	NOCS3_HARNESS_TEST_OPTION_ENC,\
	NOCS3_HARNESS_TEST_OPERATION_ENCRYPTION,\
	NOCS3_HARNESS_TEST_OPTION_DEC,\
	NOCS3_HARNESS_TEST_OPERATION_DECRYPTION);

#define NOCS3_HARNESS_TEST_PARSE_KEY_TYPE_REQUIRED \
	NOCS3_COMMAND_PARSER_PARSE_TWO_BINARY_TYPE_REQUIRED(bKeyTypeParsed,\
	NOCS3_HARNESS_TEST_OPTION_KEY,\
	pParams->keyType,\
	NOCS3_HARNESS_TEST_OPTION_KEY_SW,\
	NOCS3_HARNESS_TEST_KEY_SOFTWARE,\
	NOCS3_HARNESS_TEST_OPTION_KEY_KL,\
	NOCS3_HARNESS_TEST_KEY_KEYLADDER);

#define NOCS3_HARNESS_TEST_PARSE_BOOT_MODE \
	if ( (pParams->pvName == NOCS3_HARNESS_TEST_PV_NAME_BOOTMODE) && (bBootModeParsered == false ) )\
	{\
		if ( cmdline_offset<argc ) \
		{\
			if ( strcmp (argv[cmdline_offset], NOCS3_HARNESS_TEST_BOOT_MODE_NAND_STR)==0 )\
			{\
				pParams->bootMode = NOCS3_HARNESS_TEST_BOOT_MODE_NAND;\
			}\
			else if ( strcmp (argv[cmdline_offset], NOCS3_HARNESS_TEST_BOOT_MODE_NOR_STR)==0 )\
			{\
				pParams->bootMode = NOCS3_HARNESS_TEST_BOOT_MODE_NOR;\
			}\
			else if ( strcmp (argv[cmdline_offset], NOCS3_HARNESS_TEST_BOOT_MODE_SPI_STR)==0 )\
			{\
				pParams->bootMode = NOCS3_HARNESS_TEST_BOOT_MODE_SPI;\
			}\
			else if ( strcmp (argv[cmdline_offset], NOCS3_HARNESS_TEST_BOOT_MODE_EMMC_STR)==0 )\
			{\
				pParams->bootMode = NOCS3_HARNESS_TEST_BOOT_MODE_EMMC;\
			}\
			if ( pParams->bootMode != NOCS3_HARNESS_TEST_BOOT_MODE_MAX )\
			{\
				bPvNameParsered = true;\
				cmdline_offset ++;\
				bCommandProcessed = true;\
			}\
		}\
		else \
		{\
			return NOCS3_HARNESS_TEST_STATUS_CODE_BAD_PARAMS;\
		}\
	}

#define NOCS3_HARNESS_TEST_PARSE_PV_NAME \
	if ( bPvNameParsered == false )\
	{\
		if ( cmdline_offset<argc ) \
		{\
			if ( strcmp (argv[cmdline_offset], NOCS3_HARNESS_TEST_PV_NAME_MSID_STR)==0 )\
			{\
				pParams->pvName = NOCS3_HARNESS_TEST_PV_NAME_MSID;\
			}\
			else if ( strcmp (argv[cmdline_offset], NOCS3_HARNESS_TEST_PV_NAME_STBCASN_STR)==0 )\
			{\
				pParams->pvName = NOCS3_HARNESS_TEST_PV_NAME_STBCASN;\
			}\
			else if ( strcmp (argv[cmdline_offset], NOCS3_HARNESS_TEST_PV_NAME_SCSPV_STR)==0 )\
			{\
				pParams->pvName = NOCS3_HARNESS_TEST_PV_NAME_SCSPV;\
			}\
			else if ( strcmp (argv[cmdline_offset], NOCS3_HARNESS_TEST_PV_NAME_BOOTMODE_STR)==0 )\
			{\
				pParams->pvName = NOCS3_HARNESS_TEST_PV_NAME_BOOTMODE;\
			}\
			else if ( strcmp (argv[cmdline_offset], NOCS3_HARNESS_TEST_PV_NAME_SCS_STR)==0 )\
			{\
				pParams->pvName = NOCS3_HARNESS_TEST_PV_NAME_SCS;\
			}\
			else if ( strcmp (argv[cmdline_offset], NOCS3_HARNESS_TEST_PV_NAME_DIFPWD_STR)==0 )\
			{\
				pParams->pvName = NOCS3_HARNESS_TEST_PV_NAME_DIFPWD;\
			}\
			else if ( strcmp (argv[cmdline_offset], NOCS3_HARNESS_TEST_PV_NAME_DIFCLOSE_STR)==0 )\
			{\
				pParams->pvName = NOCS3_HARNESS_TEST_PV_NAME_DIFCLOSE;\
			}\
			else if ( strcmp (argv[cmdline_offset], NOCS3_HARNESS_TEST_PV_NAME_DIFOPEN_STR)==0 )\
			{\
				pParams->pvName = NOCS3_HARNESS_TEST_PV_NAME_DIFOPEN;\
			}\
			else if ( strcmp (argv[cmdline_offset], NOCS3_HARNESS_TEST_PV_NAME_DIF_STR)==0 )\
			{\
				pParams->pvName = NOCS3_HARNESS_TEST_PV_NAME_DIF;\
			}\
			else if ( strcmp (argv[cmdline_offset], NOCS3_HARNESS_TEST_PV_NAME_KLD_STR)==0 )\
			{\
				pParams->pvName = NOCS3_HARNESS_TEST_PV_NAME_KLD;\
			}\
			else if ( strcmp (argv[cmdline_offset], NOCS3_HARNESS_TEST_PV_NAME_RAM_STR)==0 )\
			{\
				pParams->pvName = NOCS3_HARNESS_TEST_PV_NAME_RAM;\
			}\
			else if ( strcmp (argv[cmdline_offset], NOCS3_HARNESS_TEST_PV_NAME_FLASH_STR)==0 )\
			{\
				pParams->pvName = NOCS3_HARNESS_TEST_PV_NAME_FLASH;\
			}\
			else if ( strcmp (argv[cmdline_offset], NOCS3_HARNESS_TEST_PV_NAME_RAMUNSCRAMBSIZE_STR)==0 )\
			{\
				pParams->pvName = NOCS3_HARNESS_TEST_PV_NAME_RAMUNSCRAMBSIZE;\
			}\
			else if ( strcmp (argv[cmdline_offset], NOCS3_HARNESS_TEST_PV_NAME_VERSIONING_STR)==0 )\
			{\
				pParams->pvName = NOCS3_HARNESS_TEST_PV_NAME_VERSIONING;\
			}\
			else if ( strcmp (argv[cmdline_offset], NOCS3_HARNESS_TEST_PV_NAME_ERP_STR)==0 )\
			{\
				pParams->pvName = NOCS3_HARNESS_TEST_PV_NAME_ERP;\
			}\
			else if ( strcmp (argv[cmdline_offset], NOCS3_HARNESS_TEST_PV_NAME_EBFDP_STR)==0 )\
			{\
				pParams->pvName = NOCS3_HARNESS_TEST_PV_NAME_EBFDP;\
			}\
			else if ( strcmp (argv[cmdline_offset], NOCS3_HARNESS_TEST_PV_NAME_SCSDTECBAA_STR)==0 )\
			{\
				pParams->pvName = NOCS3_HARNESS_TEST_PV_NAME_SCSDTECBAA;\
			}\
			else if ( strcmp (argv[cmdline_offset], NOCS3_HARNESS_TEST_PV_NAME_SCSTASIZE_STR)==0 )\
			{\
				pParams->pvName = NOCS3_HARNESS_TEST_PV_NAME_SCSTASIZE;\
			}\
			else if ( strcmp (argv[cmdline_offset], NOCS3_HARNESS_TEST_PV_NAME_ESCSFWUS_STR)==0 )\
			{\
				pParams->pvName = NOCS3_HARNESS_TEST_PV_NAME_ESCSFWUS;\
			}\
			else if ( strcmp (argv[cmdline_offset], NOCS3_HARNESS_TEST_PV_NAME_PESCSFWUAS_STR)==0 )\
			{\
				pParams->pvName = NOCS3_HARNESS_TEST_PV_NAME_PESCSFWUAS;\
			}\
			if ( pParams->pvName != NOCS3_HARNESS_TEST_PV_NAME_MAX )\
			{\
				bPvNameParsered = true;\
				cmdline_offset ++;\
				bCommandProcessed = true;\
			}\
		}\
		else \
		{\
			return NOCS3_HARNESS_TEST_STATUS_CODE_BAD_PARAMS;\
		}\
	}

#define NOCS3_HARNESS_TEST_SCSPV_ID_EXT_BOOT_FLASH_DEV_PARAMS_STR	"CSD_SCS_PV_EXT_BOOT_FLASH_DEV_PARAMS"
#define NOCS3_HARNESS_TEST_SCSPV_ID_EXT_RAM_PARAMS_STR	"CSD_SCS_PV_EXT_RAM_PARAMS"
#define NOCS3_HARNESS_TEST_SCSPV_ID_SCS_TOTAL_AREA_SIZE_STR	"CSD_SCS_PV_SCS_TOTAL_AREA_SIZE"
#define NOCS3_HARNESS_TEST_SCSPV_ID_ACTIVE_INDICATOR_STR	"CSD_SCS_PV_ACTIVE_INDICATOR"
#define NOCS3_HARNESS_TEST_SCSPV_ID_FULL_EXT_FW_UPGRADE_ADDR_STR	"CSD_SCS_PV_FULL_EXT_FW_UPGRADE_ADDR"
#define NOCS3_HARNESS_TEST_SCSPV_ID_FULL_EXT_FW_UPGRADE_SIZE_STR	"CSD_SCS_PV_FULL_EXT_FW_UPGRADE_SIZE"
#define NOCS3_HARNESS_TEST_SCSPV_ID_PATCH_EXT_FW_UPGRADE_ADD_STR	"CSD_SCS_PV_PATCH_EXT_FW_UPGRADE_ADDR"
#define NOCS3_HARNESS_TEST_SCSPV_ID_PATCH_EXT_FW_UPGRADE_SIZE_STR	"CSD_SCS_PV_PATCH_EXT_FW_UPGRADE_SIZE"
#define NOCS3_HARNESS_TEST_SCSPV_ID_VERSIONING_REF_STR	"CSD_SCS_PV_VERSIONING_REF"


#define NOCS3_COMMAND_PARSER_PARSE_SCSPV_ID_OPTIONAL\
	if ( bPvIdParsered == false )\
	{\
		if ( (cmdline_offset+1)<argc ) \
		{\
			if ( strcmp (argv[cmdline_offset],NOCS3_HARNESS_TEST_OPTION_ID)==0 )\
			{\
				if ( strcmp (argv[cmdline_offset+1],NOCS3_HARNESS_TEST_SCSPV_ID_EXT_BOOT_FLASH_DEV_PARAMS_STR)==0 )\
				{\
					pParams->pvId = NOCS3_HARNESS_TEST_SCSPV_ID_EXT_BOOT_FLASH_DEV_PARAMS;\
				}\
				else if ( strcmp (argv[cmdline_offset+1],NOCS3_HARNESS_TEST_SCSPV_ID_EXT_RAM_PARAMS_STR)==0 )\
				{\
					pParams->pvId = NOCS3_HARNESS_TEST_SCSPV_ID_EXT_RAM_PARAMS;\
				}\
				else if ( strcmp (argv[cmdline_offset+1],NOCS3_HARNESS_TEST_SCSPV_ID_SCS_TOTAL_AREA_SIZE_STR)==0 )\
				{\
					pParams->pvId = NOCS3_HARNESS_TEST_SCSPV_ID_SCS_TOTAL_AREA_SIZE;\
				}\
				else if ( strcmp (argv[cmdline_offset+1],NOCS3_HARNESS_TEST_SCSPV_ID_ACTIVE_INDICATOR_STR)==0 )\
				{\
					pParams->pvId = NOCS3_HARNESS_TEST_SCSPV_ID_ACTIVE_INDICATOR;\
				}\
				else if ( strcmp (argv[cmdline_offset+1],NOCS3_HARNESS_TEST_SCSPV_ID_FULL_EXT_FW_UPGRADE_ADDR_STR)==0 )\
				{\
					pParams->pvId = NOCS3_HARNESS_TEST_SCSPV_ID_FULL_EXT_FW_UPGRADE_ADDR;\
				}\
				else if ( strcmp (argv[cmdline_offset+1],NOCS3_HARNESS_TEST_SCSPV_ID_FULL_EXT_FW_UPGRADE_SIZE_STR)==0 )\
				{\
					pParams->pvId = NOCS3_HARNESS_TEST_SCSPV_ID_FULL_EXT_FW_UPGRADE_SIZE;\
				}\
				else if ( strcmp (argv[cmdline_offset+1],NOCS3_HARNESS_TEST_SCSPV_ID_PATCH_EXT_FW_UPGRADE_ADD_STR)==0 )\
				{\
					pParams->pvId = NOCS3_HARNESS_TEST_SCSPV_ID_PATCH_EXT_FW_UPGRADE_ADD;\
				}\
				else if ( strcmp (argv[cmdline_offset+1],NOCS3_HARNESS_TEST_SCSPV_ID_PATCH_EXT_FW_UPGRADE_SIZE_STR)==0 )\
				{\
					pParams->pvId = NOCS3_HARNESS_TEST_SCSPV_ID_PATCH_EXT_FW_UPGRADE_SIZE;\
				}\
				else if ( strcmp (argv[cmdline_offset+1],NOCS3_HARNESS_TEST_SCSPV_ID_VERSIONING_REF_STR)==0 )\
				{\
					pParams->pvId = NOCS3_HARNESS_TEST_SCSPV_ID_VERSIONING_REF;\
				}\
				if ( pParams->pvId != NOCS3_HARNESS_TEST_SCSPV_ID_MAX )\
				{\
					bPvIdParsered = true;\
					cmdline_offset +=2;\
					bCommandProcessed = true;\
				}\
				else \
				{\
					return NOCS3_HARNESS_TEST_STATUS_CODE_BAD_PARAMS;\
				}\
			}\
		}\
	}


#define NOCS3_HARNESS_TEST_PARSE_ECM \
	NOCS3_COMMAND_PARSER_PARSE_TWO_BINARY_TYPE_REQUIRED(bEcmParsed,\
	NOCS3_HARNESS_TEST_OPTION_PECM,\
	pParams->bEcmRequired,\
	"TRUE",\
	true,\
	"FALSE",\
	false);

#define  NOCS3_HARNESS_TEST_FIND_FIELD_IN_TV_LINE(expected_size, field, skip, size_field)\
	for (end=start;end<size_array;end++)\
	{\
		if ( pString[end]==',')\
		{\
			break;\
		}\
	}\
	size = end-start;\
	if ( (size !=0) && (size>skip) )\
	{\
		size_field=nocs3_harness_test_utility_ascii2array(&pString[start+skip], field, (size-skip)/2);\
	}\
	start = end+1;


#define NOCS3_HELP_M_KEY		"-key KL|SW = Inicates if the key is protected through the KL or is a \
Software provided key"
#define NOCS3_HELP_M_EMI		"-emi EMI = Defines Encryption Method Indicator of the input data"
#define NOCS3_HELP_M_IVC		"[-ivc Value64|Value128] = IV Value.  Optional Parameter. A 64bit/128 bit \
from the command line (-ivc). If the option is not present and for particular EMI an IV is needed then \
it must be taken from the .csv file provided with option -tv"
#define NOCS3_HELP_M_TV		"-tv TestVectorFileName = Indicates the path and filename of the .CSV files \
which contains the test vector"
#define NOCS3_HELP_M_VPID		"-videoPid VPID = Sets video PID to be processed. "
#define NOCS3_HELP_M_APID		"-audioPid APID = Sets audio PID to be processed. "
#define NOCS3_HELP_M_PPID		"-pcrPid PPID = Sets PCR PID to be processed. "
#define NOCS3_HELP_M_IF		"-if InFile|-ife = Define what is the input stream. If -if then the input comes \
from the file (InFile is a path to an MPEG TS file). If -ife then the input streams comes from the STB FrontEnd."
#define NOCS3_HELP_M_OF		"[-of OutFile] = Output File. Optional Parameter. If this option is present, the \
descrambled output is sent into OutFile as well as to the video output.  If not specified the descrambled stream \
only goes to the video output."

#define NOCS3_HARNESS_TEST_BOOT_MODE_NAND_STR			"NAND"
#define NOCS3_HARNESS_TEST_BOOT_MODE_NOR_STR			"NOR"
#define NOCS3_HARNESS_TEST_BOOT_MODE_SPI_STR			"SPI"
#define NOCS3_HARNESS_TEST_BOOT_MODE_EMMC_STR			"EMMC"

#define NOCS3_HARNESS_TEST_PV_NAME_MSID_STR				"MSID"
#define NOCS3_HARNESS_TEST_PV_NAME_STBCASN_STR			"STBCASN"
#define NOCS3_HARNESS_TEST_PV_NAME_SCSPV_STR			"SCSPV"
#define NOCS3_HARNESS_TEST_PV_NAME_BOOTMODE_STR			"BOOTMODE"
#define NOCS3_HARNESS_TEST_PV_NAME_SCS_STR				"SCS"
#define NOCS3_HARNESS_TEST_PV_NAME_DIFPWD_STR			"DIFPWD"
#define NOCS3_HARNESS_TEST_PV_NAME_DIFCLOSE_STR			"DIFCLOSE"
#define NOCS3_HARNESS_TEST_PV_NAME_DIFOPEN_STR			"DIFOPEN"
#define NOCS3_HARNESS_TEST_PV_NAME_DIF_STR				"DIF"
#define NOCS3_HARNESS_TEST_PV_NAME_KLD_STR				"KLD"
#define NOCS3_HARNESS_TEST_PV_NAME_RAM_STR				"RAM"
#define NOCS3_HARNESS_TEST_PV_NAME_FLASH_STR			"FLASH"
#define NOCS3_HARNESS_TEST_PV_NAME_RAMUNSCRAMBSIZE_STR	"RAMUNSCRAMBSIZE"
#define NOCS3_HARNESS_TEST_PV_NAME_VERSIONING_STR		"VER"
#define NOCS3_HARNESS_TEST_PV_NAME_ERP_STR				"ERP"
#define NOCS3_HARNESS_TEST_PV_NAME_EBFDP_STR			"EBFDP"
#define NOCS3_HARNESS_TEST_PV_NAME_SCSDTECBAA_STR		"SCS_DTE_Boot_Code_Area_address"
#define NOCS3_HARNESS_TEST_PV_NAME_SCSTASIZE_STR		"SCSTASize"
#define NOCS3_HARNESS_TEST_PV_NAME_ESCSFWUS_STR			"ESCSFWUS"
#define NOCS3_HARNESS_TEST_PV_NAME_PESCSFWUAS_STR		"PESCSFWUAS"

typedef enum NOCS3_HARNESS_TEST_BOOT_MODE
{
	NOCS3_HARNESS_TEST_BOOT_MODE_NAND,
	NOCS3_HARNESS_TEST_BOOT_MODE_NOR,
	NOCS3_HARNESS_TEST_BOOT_MODE_SPI,
	NOCS3_HARNESS_TEST_BOOT_MODE_EMMC,

	/* Add new enum before this line */
	NOCS3_HARNESS_TEST_BOOT_MODE_MAX

}  NOCS3_HARNESS_TEST_BOOT_MODE;


typedef enum NOCS3_HARNESS_TEST_PV_NAME
{
	NOCS3_HARNESS_TEST_PV_NAME_MSID,
	NOCS3_HARNESS_TEST_PV_NAME_STBCASN,
	NOCS3_HARNESS_TEST_PV_NAME_SCSPV,
	NOCS3_HARNESS_TEST_PV_NAME_BOOTMODE,
	NOCS3_HARNESS_TEST_PV_NAME_SCS,
	NOCS3_HARNESS_TEST_PV_NAME_DIFPWD,
	NOCS3_HARNESS_TEST_PV_NAME_DIFCLOSE,
	NOCS3_HARNESS_TEST_PV_NAME_DIFOPEN,
	NOCS3_HARNESS_TEST_PV_NAME_DIF,
	NOCS3_HARNESS_TEST_PV_NAME_KLD,
	NOCS3_HARNESS_TEST_PV_NAME_RAM,
	NOCS3_HARNESS_TEST_PV_NAME_FLASH,
	NOCS3_HARNESS_TEST_PV_NAME_RAMUNSCRAMBSIZE,
	NOCS3_HARNESS_TEST_PV_NAME_VERSIONING,

	NOCS3_HARNESS_TEST_PV_NAME_ERP,
	NOCS3_HARNESS_TEST_PV_NAME_EBFDP,
	NOCS3_HARNESS_TEST_PV_NAME_SCSDTECBAA,
	NOCS3_HARNESS_TEST_PV_NAME_SCSTASIZE,
	NOCS3_HARNESS_TEST_PV_NAME_ESCSFWUS,
	NOCS3_HARNESS_TEST_PV_NAME_PESCSFWUAS,

	/* Add new enum before this line */
	NOCS3_HARNESS_TEST_PV_NAME_MAX

}  NOCS3_HARNESS_TEST_PV_NAME;

typedef enum NOCS3_HARNESS_TEST_SCSPV_ID
{
	NOCS3_HARNESS_TEST_SCSPV_ID_EXT_BOOT_FLASH_DEV_PARAMS,
	NOCS3_HARNESS_TEST_SCSPV_ID_EXT_RAM_PARAMS,
	NOCS3_HARNESS_TEST_SCSPV_ID_SCS_TOTAL_AREA_SIZE,
	NOCS3_HARNESS_TEST_SCSPV_ID_ACTIVE_INDICATOR,
	NOCS3_HARNESS_TEST_SCSPV_ID_FULL_EXT_FW_UPGRADE_ADDR,
	NOCS3_HARNESS_TEST_SCSPV_ID_FULL_EXT_FW_UPGRADE_SIZE,
	NOCS3_HARNESS_TEST_SCSPV_ID_PATCH_EXT_FW_UPGRADE_ADD,
	NOCS3_HARNESS_TEST_SCSPV_ID_PATCH_EXT_FW_UPGRADE_SIZE,
	NOCS3_HARNESS_TEST_SCSPV_ID_VERSIONING_REF,

	/* Add new enum before this line */
	NOCS3_HARNESS_TEST_SCSPV_ID_MAX

}  NOCS3_HARNESS_TEST_SCSPV_ID;
typedef enum NOCS3_HARNESS_TEST_DEBUG_INTERFACE_ACCESS_MODE
{
	NOCS3_HARNESS_TEST_DEBUG_INTERFACE_OPEN,
	NOCS3_HARNESS_TEST_DEBUG_INTERFACE_PASSWORD_PROTECTED,
	NOCS3_HARNESS_TEST_DEBUG_INTERFACE_CLOSED,

	/* Add new enum before this line */
	NOCS3_HARNESS_TEST_DEBUG_INTERFACE_MAX
} NOCS3_HARNESS_TEST_DEBUG_INTERFACE;

typedef enum NOCS3_HARNESS_TEST_PARSER_STATUS_CODE
{
	NOCS3_HARNESS_TEST_PARSER_STATUS_CODE_SUCCESS,
	NOCS3_HARNESS_TEST_PARSER_STATUS_CODE_NOT_FOUND,
	NOCS3_HARNESS_TEST_PARSER_STATUS_CODE_BADPARAMS,

	/* Add new enum before this line */
	NOCS3_HARNESS_TEST_PARSER_STATUS_CODE_MAX
} NOCS3_HARNESS_TEST_PARSER_STATUS_CODE;

typedef enum NOCS3_HARNESS_TEST_OPERATION
{
	NOCS3_HARNESS_TEST_OPERATION_ENCRYPTION,
	NOCS3_HARNESS_TEST_OPERATION_DECRYPTION,

	/* Add new enum before this line */
	NOCS3_HARNESS_TEST_OPERATION_MAX
} NOCS3_HARNESS_TEST_OPERATION;

typedef enum NOCS3_HARNESS_TEST_KEY
{
	NOCS3_HARNESS_TEST_KEY_SOFTWARE,
	NOCS3_HARNESS_TEST_KEY_KEYLADDER,
	NOCS3_HARNESS_TEST_KEY_CERT,

	/* Add new enum before this line */
	NOCS3_HARNESS_TEST_KEY_MAX
} NOCS3_HARNESS_TEST_KEY;

typedef unsigned char T8BytesVector[8];
typedef unsigned char T4BytesVector[4];

typedef struct NOCS3_Harness_Test_Test_Vector
{
	unsigned int						sizeNuid;
	T8BytesVector						nuid;

	unsigned int						sizeCipheredProtectingKeyL2;
	unsigned char						cipheredProtectingKeyL2[16];

	unsigned int						sizeCipheredProtectingKeyL1;
	unsigned char						cipheredProtectingKeyL1[16];

	unsigned int						sizeCipheredContentKey;
	unsigned char						cipheredContentKey[16];

	unsigned int						sizeHostKey;
	unsigned char						HostKey[16];

	unsigned int						sizeIvCbc;
	unsigned char						IvCbc[16];

	unsigned int						sizeIvCtr;
	unsigned char						IvCtr[16];

	unsigned int						sizeIvCiPlus;
	unsigned char						IvCiPlus[16];

	unsigned int						sizeDataIn;
	unsigned char						dataIn[16];

	unsigned int						sizeClearTextFlashProtectingKey;
	unsigned char						clearTextFlashProtectingKey[16];

	unsigned int						sizeq;
	unsigned char						q[48];
	
	unsigned int						sizea;
	unsigned char						a[48];
	
	unsigned int						sizeb;
	unsigned char						b[48];

	unsigned int						sizeGX;
	unsigned char						GX[48];

	unsigned int						sizeGY;
	unsigned char						GY[48];

	unsigned int						sizen;
	unsigned char						n[48];

	unsigned int						sizeh;	
	unsigned char 						h[16];
		

}  NOCS3_Harness_Test_Test_Vector;


NOCS3_HARNESS_TEST nocs3_harness_test_get_test_type (int argc, char * argv[]);

NOCS3_HARNESS_TEST_PARSER_STATUS_CODE nocs3_harness_test_parse_number (char * arg1, \
																				 char * arg2, \
																				 char * argv3, \
																				 unsigned int * pInt,\
																				 char * pFormat);
NOCS3_HARNESS_TEST_PARSER_STATUS_CODE nocs3_harness_test_parse_array (char * arg1, \
																				 char * arg2, \
																				 char * argv3, \
																				 unsigned char * pArray,
																				 unsigned int * pSize,
																				 unsigned int MaxSize);
NOCS3_HARNESS_TEST_PARSER_STATUS_CODE nocs3_harness_test_parse_array_2 (char * arg1, \
																				 char * arg2, \
																				 char * argv3, \
																				 unsigned char ** pArray,
																				 unsigned int * pSize);

NOCS3_HARNESS_TEST_PARSER_STATUS_CODE nocs3_harness_test_parse_file (char * arg1, \
																		char * arg2, \
																		char * argv3, \
																		FILE ** ppFile, \
																		char * pMode);

NOCS3_HARNESS_TEST_PARSER_STATUS_CODE nocs3_harness_test_find_nuid_test_vector_line (T8BytesVector nuid, \
															  char * pString, \
															  unsigned int size_array);

NOCS3_HARNESS_TEST_STATUS_CODE nocs3_harness_test_get_test_vector_4_nuid (T8BytesVector nuid, \
															  NOCS3_Harness_Test_Test_Vector * pVector, \
															  FILE * pTvFile);
NOCS3_HARNESS_TEST_STATUS_CODE nocs3_harness_test_dump_test_vector (NOCS3_Harness_Test_Test_Vector * pVector);

extern char gHarnessTestBootModeString[4][16];
extern char gHarnessTestActivationFlagString[2][16];
extern char gHarnessTestDebugInterfaceStatusString[3][16];

#ifdef __cplusplus
}
#endif

#endif /* __NOCS3_HARNESS_TEST_PARSER_H__ */
/**********************************************************************************************
	OCTO 2.0 Global Version Description
	----
	make obama will parse current file, make it to binary and write it to flash for S/W update
	/root/etc/moduleversion.json will be included each module version.
 **********************************************************************************************/

#ifndef	__OCTO20_VERSION_H__
#define	__OCTO20_VERSION_H__

#if	defined(CONFIG_PROD_HDR1000S)
	#define	SYSTEM_MODEL_NAME_PREFIX		(HCHAR*)""
	#define	SYSTEM_MODEL_NAME				(HCHAR*)"HDR-1000S"
	#define	SYSTEM_APP_VER_HEAD				(HCHAR*)"UKSFAA"
	#define	SYSTEM_APP_VERSION				(HUINT32)0x000019		//	0.00.23
	#define	SYSTEM_ID						(HCHAR*)"809C.7D00"
	#define	SYSTEM_UPDATE_DATE				(HCHAR*)"01 AUG 2012"

	#define	SYSTEM_CIPLUS_VERSION			(HCHAR*)"1.00.00\0"
#elif	defined(CONFIG_PROD_HMS1000S)
	#define	SYSTEM_MODEL_NAME_PREFIX		(HCHAR*)""
	#define	SYSTEM_MODEL_NAME				(HCHAR*)"HMS-1000S"
	#define	SYSTEM_APP_VER_HEAD				(HCHAR*)"DESFAB"
	#define	SYSTEM_APP_VERSION				(HUINT32)0x010101		//	1.01.00
	#define	SYSTEM_ID						(HCHAR*)"809C.5010"
	#define	SYSTEM_UPDATE_DATE				(HCHAR*)"15 NOV 2013"

	#define	SYSTEM_CIPLUS_VERSION			(HCHAR*)"1.00.00\0"
#elif	defined(CONFIG_PROD_IR4000HD)
	#define	SYSTEM_MODEL_NAME_PREFIX		(HCHAR*)""
	#define	SYSTEM_MODEL_NAME				(HCHAR*)"IR4000HD"
	#define	SYSTEM_APP_VER_HEAD				(HCHAR*)"MESIBE"
	#define	SYSTEM_APP_VERSION				(HUINT32)0x000008		//	0.00.08
	#define	SYSTEM_ID						(HCHAR*)"809C.3040"
	#define	SYSTEM_UPDATE_DATE				(HCHAR*)"24 JAN 2014"
#elif	defined(CONFIG_PROD_ICORDPRO)
	#define	SYSTEM_MODEL_NAME_PREFIX		(HCHAR*)""
	#define	SYSTEM_MODEL_NAME				(HCHAR*)"ICORDPRO"
	#define	SYSTEM_APP_VER_HEAD				(HCHAR*)"DESNAD"
	#define	SYSTEM_APP_VERSION				(HUINT32)0x000005		//	0.00.04
	#define	SYSTEM_ID						(HCHAR*)"809C.5020"
	#define	SYSTEM_UPDATE_DATE				(HCHAR*)"10 MAR 2014"

	#define	SYSTEM_CIPLUS_VERSION			(HCHAR*)"1.00.00\0"
#elif	defined(CONFIG_PROD_HD9000I)
	#define	SYSTEM_MODEL_NAME_PREFIX		(HCHAR*)""
	#define	SYSTEM_MODEL_NAME				(HCHAR*)"HD 9000i"
	#define	SYSTEM_APP_VER_HEAD				(HCHAR*)"RUCNAA"
	#define	SYSTEM_APP_VERSION				(HUINT32)0x00030a		//	0.00.01
	#define	SYSTEM_ID						(HCHAR*)"8050.8700"
	#define	SYSTEM_UPDATE_DATE				(HCHAR*)"07 Mar 2014"

	#define	SYSTEM_CIPLUS_VERSION			(HCHAR*)"1.00.00\0"
#elif	defined(CONFIG_PROD_HMS1000T)
	#define	SYSTEM_MODEL_NAME_PREFIX		(HCHAR*)""
	#define	SYSTEM_MODEL_NAME				(HCHAR*)"HMS-1000T"
	#define	SYSTEM_APP_VER_HEAD				(HCHAR*)"AUTFAC"
	#define	SYSTEM_APP_VERSION				(HUINT32)0x000007		//	0.00.07
	#define	SYSTEM_ID						(HCHAR*)"9010.7D03"
	#define	SYSTEM_UPDATE_DATE				(HCHAR*)"23 JAN 2014"

//	#define	SYSTEM_CIPLUS_VERSION			(HCHAR*)"1.00.00\0"
#elif	defined(CONFIG_PROD_IHDR6100C)
	#define	SYSTEM_MODEL_NAME_PREFIX		(HCHAR*)""
	#define	SYSTEM_MODEL_NAME				(HCHAR*)"IHDR-6100C"
	#define	SYSTEM_APP_VER_HEAD				(HCHAR*)"AAAAAA"
	#define	SYSTEM_APP_VERSION				(HUINT32)0x000000		//	0.00.00
	#define	SYSTEM_ID						(HCHAR*)"0000.0000"
	#define	SYSTEM_UPDATE_DATE				(HCHAR*)"15 NOV 2013"

//	#define	SYSTEM_CIPLUS_VERSION			(HCHAR*)"1.00.00\0"
#elif	defined(CONFIG_PROD_HDR4000T)
	#define	SYSTEM_MODEL_NAME_PREFIX		(HCHAR*)""
	#define	SYSTEM_MODEL_NAME				(HCHAR*)"HDR-4000T"
	#define	SYSTEM_APP_VER_HEAD				(HCHAR*)"UKSFABC"
	#define	SYSTEM_APP_VERSION				(HUINT32)0x000001		//	0.00.01
	#define	SYSTEM_ID						(HCHAR*)"0000.0000"
	#define	SYSTEM_UPDATE_DATE				(HCHAR*)"25 JAN 2014"

	//	#define SYSTEM_CIPLUS_VERSION			(HCHAR*)"1.00.00\0"
#elif	defined(CONFIG_PROD_YSR2000)
	#define	SYSTEM_MODEL_NAME_PREFIX		(HCHAR*)""
	#define	SYSTEM_MODEL_NAME				(HCHAR*)"YSR-2000"
	#define	SYSTEM_APP_VER_HEAD				(HCHAR*)"DKCXAB"
	#define	SYSTEM_APP_VERSION				(HUINT32)0x000003		//	0.00.03
	#define	SYSTEM_ID						(HCHAR*)"9010.7D33"
	#define	SYSTEM_UPDATE_DATE				(HCHAR*)"15 OCT 2014"

	//	#define	SYSTEM_CIPLUS_VERSION			(HCHAR*)"1.00.00\0"
#elif	defined(CONFIG_PROD_YS1000)
	#define	SYSTEM_MODEL_NAME_PREFIX		(HCHAR*)""
	#define	SYSTEM_MODEL_NAME				(HCHAR*)"YS-1000"
	#define	SYSTEM_APP_VER_HEAD				(HCHAR*)"DKCXAA"
	#define	SYSTEM_APP_VERSION				(HUINT32)0x000003		//	0.00.03
	#define	SYSTEM_ID						(HCHAR*)"9010.7D32"
	#define	SYSTEM_UPDATE_DATE				(HCHAR*)"15 OCT 2014"

	//	#define	SYSTEM_CIPLUS_VERSION			(HCHAR*)"1.00.00\0"
#else
	#error	"Model name is not defined!!!"
#endif

#endif


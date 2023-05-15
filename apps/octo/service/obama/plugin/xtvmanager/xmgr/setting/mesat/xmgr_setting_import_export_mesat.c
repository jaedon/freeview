/**
@file     xmgr_setting_import_export_me_sat.c
@brief setup info get / set application .

Description: setup info get /set application \n
Module: Application(Layer), util(module) \n
Remarks : \n

Copyright (c) 2009 HUMAX Co., Ltd.				\n
All rights reserved.							\n
*/

/**************************************************************************************************/
#define _________AP_Setup_Info_Private_Include_________________________________________________
/**************************************************************************************************/

#include <bus.h>

#include <mgr_action.h>
#include <xmgr_module_mgr.h>
#include <mgr_setting.h>

/**************************************************************************************************/
#define _________AP_Setup_Info_Private_Macro_________________________________________________
/**************************************************************************************************/

/* debug print */
//#define 		DEBUG_PRINT		1
/**************************************************************************************************/
#define _________AP_Setup_Info_Private_Struct_________________________________________________
/**************************************************************************************************/

typedef struct SetupInfoContext
{
  HUINT8 tmp; /* do nothing */	
} SetupInfoContents_t;

/**************************************************************************************************/
#define _________AP_Setup_Info_Private_Value_________________________________________________
/**************************************************************************************************/

static	SetupInfoContents_t		s_SetupInfoContext;

/**************************************************************************************************/
#define _________AP_Setup_Info_Private_Prototype_________________________________________________
/**************************************************************************************************/
STATIC SetupInfoContents_t* local_Setup_Info_GetContext   ( void );
//STATIC BUS_Result_t local_Setup_Info_InitContent(SetupInfoContents_t *pstContents , HINT32 p1);

//msg run
STATIC BUS_Result_t local_Setup_Info_MsgBusCreate  ( SetupInfoContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t local_Setup_Info_MsgBusDestroy  (SetupInfoContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3 );


STATIC HERROR	xmgr_setting_ExportDbSync_MeSat (void);
STATIC HERROR	xmgr_setting_ExportDbAsync_MeSat (void);
STATIC HERROR	xmgr_setting_ImportDbSync_MeSat (void);
STATIC HERROR	xmgr_setting_ImportDbAsync_MeSat (void);

STATIC BUS_Result_t	xproc_setting_ImportExportDb(HINT32 nMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

/* Error Type 정의 */
typedef enum
{
	eDxIMEXPORT_ERROR_NONE					= 0,
	eDxIMEXPORT_ERROR_FAILURE				= 1,
	eDxIMEXPORT_ERROR_NO_USB				= 2,
	eDxIMEXPORT_ERROR_NO_FILE				= 3,

	eDxIMEXPORT_ERROR_MAX
} DxImportExportErrorType_e;

/* USB memory 정보 */
#define AP_SPEC_MESAT_SETUPINFO_UMUP_FOLDER					"/umup"
#define AP_SPEC_MESAT_SETUPINFO_UMDOWN_FOLDER				"/umdown"
#define AP_SPEC_MESAT_SETUPINFO_HUMAXTV_FOLDER				"/var/lib/humaxtv"
#define AP_SPEC_MESAT_SETUPINFO_DLOGGER_FOLDER				"/var/lib/humaxtv/dlogger"

#define AP_SPEC_MESAT_SETUPINFO_FILENAME_REGISTRY_REG_USB	"/registry.reg"
#define AP_SPEC_MESAT_SETUPINFO_FILENAME_ANTINFO_USB 		"/DxAntInfo_t.db"
#define AP_SPEC_MESAT_SETUPINFO_FILENAME_BOUQUET_USB		"/DxBouquet_t.db"
#define AP_SPEC_MESAT_SETUPINFO_FILENAME_NETWORK_USB		"/DxNetwork_t.db"
#define AP_SPEC_MESAT_SETUPINFO_FILENAME_SERVICE_USB		"/DxService_t.db"
#define AP_SPEC_MESAT_SETUPINFO_FILENAME_DBSETUP_SYSTEM_USB	"/DxDBSETUP_SYSTEM.db"
//#define AP_SPEC_MESAT_SETUPINFO_FILENAME_DBSETUP_FACTORY_USB	"/DxDBSETUP_FACTORY.db"
#define AP_SPEC_MESAT_SETUPINFO_FILENAME_PROVIDER_USB		"/DxProvider_t.db"
#define AP_SPEC_MESAT_SETUPINFO_FILENAME_TRANSPONDER_USB	"/DxTransponder_t.db"
#define AP_SPEC_MESAT_SETUPINFO_FILENAME_GROUP_USB			"/DxGroup_t.db"
#define AP_SPEC_MESAT_SETUPINFO_FILENAME_GROUPLIST_USB		"/DxGroupList_t.db"
#define AP_SPEC_MESAT_SETUPINFO_FILENAME_UDBMODULE_USB		"/UDBMODULE.db"

#define AP_SPEC_MESAT_SETUPINFO_FILENAME_REGISTRY_REG		"/var/lib/humaxtv/registry.reg"
#define AP_SPEC_MESAT_SETUPINFO_FILENAME_ANTINFO			"/var/lib/humaxtv/dlogger/DxAntInfo_t.db"
#define AP_SPEC_MESAT_SETUPINFO_FILENAME_BOUQUET			"/var/lib/humaxtv/dlogger/DxBouquet_t.db"
#define AP_SPEC_MESAT_SETUPINFO_FILENAME_NETWORK			"/var/lib/humaxtv/dlogger/DxNetwork_t.db"
#define AP_SPEC_MESAT_SETUPINFO_FILENAME_SERVICE			"/var/lib/humaxtv/dlogger/DxService_t.db"
#define AP_SPEC_MESAT_SETUPINFO_FILENAME_DBSETUP_SYSTEM		"/var/lib/humaxtv/dlogger/DxDBSETUP_SYSTEM.db"
//#define AP_SPEC_MESAT_SETUPINFO_FILENAME_DBSETUP_FACTORY	"/var/lib/humaxtv/dlogger/DxDBSETUP_FACTORY.db"
#define AP_SPEC_MESAT_SETUPINFO_FILENAME_PROVIDER			"/var/lib/humaxtv/dlogger/DxProvider_t.db"
#define AP_SPEC_MESAT_SETUPINFO_FILENAME_TRANSPONDER		"/var/lib/humaxtv/dlogger/DxTransponder_t.db"
#define AP_SPEC_MESAT_SETUPINFO_FILENAME_GROUP				"/var/lib/humaxtv/dlogger/DxGroup_t.db"
#define AP_SPEC_MESAT_SETUPINFO_FILENAME_GROUPLIST			"/var/lib/humaxtv/dlogger/DxGroupList_t.db"
#define AP_SPEC_MESAT_SETUPINFO_FILENAME_UDBMODULE			"/var/lib/humaxtv/dlogger/UDBMODULE.db"

#define AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR 					128
#define AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME 					256

#define	USE_HxFILE			1

STATIC HERROR	xmgr_setting_CopyFile(HUINT8 *pszSource, HUINT8 *pszDest)
{
#if USE_HxFILE
	return HLIB_FILE_Copy((const HUINT8 *)pszSource, (const HUINT8 *)pszDest);
#else
//	return MWC_FILE_CopyFile((char *)pszSource, (char *)pszDest);
	return ERR_FAIL;
#endif
}

STATIC HERROR	xmgr_setting_PrepareDirectory(const HUINT8 *pszFullDirPath)
{
	HERROR		hErr;
	HBOOL		bIsDirExist;

	bIsDirExist = HLIB_DIR_IsExist((const HCHAR *)pszFullDirPath);
	if( bIsDirExist == TRUE )
	{/* 기존에 사용하던 디렉토리가 있으면 지운다. */
		HLIB_DIR_Delete((const HCHAR *)pszFullDirPath);
	}

	HxLOG_Print(" [%s,%d] pszFullDirPath	: %s \n" , __FUNCTION__, __LINE__ , pszFullDirPath);
	/* make directory */
	hErr = HLIB_DIR_Make((const HCHAR *)pszFullDirPath);
	if( hErr != ERR_OK )
	{
		HxLOG_Error("[Func:%s, Line:%d] Create Directory Fail!! %s \n" , __FUNCTION__, __LINE__,pszFullDirPath );
		return ERR_FAIL;
	}

	return ERR_OK;
}

#define DBFILE_CHECK_VAR(var)		\
	if( HLIB_FILE_IsValid(var) != ERR_OK){\
		HxLOG_Error("\nDB File ( %s ) is Not Exist!! \n",var);	\
		return ERR_FAIL;\
	}

STATIC HERROR	xmgr_setting_exportCheckFile(void)
{
	DBFILE_CHECK_VAR(AP_SPEC_MESAT_SETUPINFO_FILENAME_BOUQUET);
	DBFILE_CHECK_VAR(AP_SPEC_MESAT_SETUPINFO_FILENAME_NETWORK);
	DBFILE_CHECK_VAR(AP_SPEC_MESAT_SETUPINFO_FILENAME_SERVICE);
	DBFILE_CHECK_VAR(AP_SPEC_MESAT_SETUPINFO_FILENAME_PROVIDER);
	DBFILE_CHECK_VAR(AP_SPEC_MESAT_SETUPINFO_FILENAME_TRANSPONDER);
	DBFILE_CHECK_VAR(AP_SPEC_MESAT_SETUPINFO_FILENAME_GROUP);
	DBFILE_CHECK_VAR(AP_SPEC_MESAT_SETUPINFO_FILENAME_GROUPLIST);
	DBFILE_CHECK_VAR(AP_SPEC_MESAT_SETUPINFO_FILENAME_DBSETUP_SYSTEM);
	DBFILE_CHECK_VAR(AP_SPEC_MESAT_SETUPINFO_FILENAME_UDBMODULE);
	DBFILE_CHECK_VAR(AP_SPEC_MESAT_SETUPINFO_FILENAME_ANTINFO);
	DBFILE_CHECK_VAR(AP_SPEC_MESAT_SETUPINFO_FILENAME_REGISTRY_REG);

	return ERR_OK;
}

STATIC HERROR	xmgr_setting_importCheckFile(const HUINT8 *pszUsbFullDirPath)
{
	HCHAR			szUsbFullFilePath[AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME];


	HxSTD_MemSet( szUsbFullFilePath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME );
	HLIB_STD_StrNCpySafe((HCHAR *)szUsbFullFilePath, (const HCHAR *)pszUsbFullDirPath, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR);
	HxSTD_StrNCat( szUsbFullFilePath, AP_SPEC_MESAT_SETUPINFO_FILENAME_BOUQUET_USB, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR );
	DBFILE_CHECK_VAR(szUsbFullFilePath);

	HxSTD_MemSet( szUsbFullFilePath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME );
	HLIB_STD_StrNCpySafe((HCHAR *)szUsbFullFilePath, (const HCHAR *)pszUsbFullDirPath, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR);
	HxSTD_StrNCat( szUsbFullFilePath, AP_SPEC_MESAT_SETUPINFO_FILENAME_NETWORK_USB, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR );
	DBFILE_CHECK_VAR(szUsbFullFilePath);

	HxSTD_MemSet( szUsbFullFilePath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME );
	HLIB_STD_StrNCpySafe((HCHAR *)szUsbFullFilePath, (const HCHAR *)pszUsbFullDirPath, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR);
	HxSTD_StrNCat( szUsbFullFilePath, AP_SPEC_MESAT_SETUPINFO_FILENAME_SERVICE_USB, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR );
	DBFILE_CHECK_VAR(szUsbFullFilePath);

	HxSTD_MemSet( szUsbFullFilePath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME );
	HLIB_STD_StrNCpySafe((HCHAR *)szUsbFullFilePath, (const HCHAR *)pszUsbFullDirPath, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR);
	HxSTD_StrNCat( szUsbFullFilePath, AP_SPEC_MESAT_SETUPINFO_FILENAME_PROVIDER_USB, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR );
	DBFILE_CHECK_VAR(szUsbFullFilePath);

	HxSTD_MemSet( szUsbFullFilePath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME );
	HLIB_STD_StrNCpySafe((HCHAR *)szUsbFullFilePath, (const HCHAR *)pszUsbFullDirPath, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR);
	HxSTD_StrNCat( szUsbFullFilePath, AP_SPEC_MESAT_SETUPINFO_FILENAME_TRANSPONDER_USB, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR );
	DBFILE_CHECK_VAR(szUsbFullFilePath);

	HxSTD_MemSet( szUsbFullFilePath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME );
	HLIB_STD_StrNCpySafe((HCHAR *)szUsbFullFilePath, (const HCHAR *)pszUsbFullDirPath, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR);
	HxSTD_StrNCat( szUsbFullFilePath, AP_SPEC_MESAT_SETUPINFO_FILENAME_GROUP_USB, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR );
	DBFILE_CHECK_VAR(szUsbFullFilePath);

	HxSTD_MemSet( szUsbFullFilePath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME );
	HLIB_STD_StrNCpySafe((HCHAR *)szUsbFullFilePath, (const HCHAR *)pszUsbFullDirPath, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR);
	HxSTD_StrNCat( szUsbFullFilePath, AP_SPEC_MESAT_SETUPINFO_FILENAME_GROUPLIST_USB, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR );
	DBFILE_CHECK_VAR(szUsbFullFilePath);

	HxSTD_MemSet( szUsbFullFilePath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME );
	HLIB_STD_StrNCpySafe((HCHAR *)szUsbFullFilePath, (const HCHAR *)pszUsbFullDirPath, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR);
	HxSTD_StrNCat( szUsbFullFilePath, AP_SPEC_MESAT_SETUPINFO_FILENAME_DBSETUP_SYSTEM_USB, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR );
	DBFILE_CHECK_VAR(szUsbFullFilePath);

	HxSTD_MemSet( szUsbFullFilePath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME );
	HLIB_STD_StrNCpySafe((HCHAR *)szUsbFullFilePath, (const HCHAR *)pszUsbFullDirPath, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR);
	HxSTD_StrNCat( szUsbFullFilePath, AP_SPEC_MESAT_SETUPINFO_FILENAME_UDBMODULE_USB, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR );
	DBFILE_CHECK_VAR(szUsbFullFilePath);

	HxSTD_MemSet( szUsbFullFilePath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME );
	HLIB_STD_StrNCpySafe((HCHAR *)szUsbFullFilePath, (const HCHAR *)pszUsbFullDirPath, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR);
	HxSTD_StrNCat( szUsbFullFilePath, AP_SPEC_MESAT_SETUPINFO_FILENAME_ANTINFO_USB, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR );
	DBFILE_CHECK_VAR(szUsbFullFilePath);

	HxSTD_MemSet( szUsbFullFilePath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME );
	HLIB_STD_StrNCpySafe((HCHAR *)szUsbFullFilePath, (const HCHAR *)pszUsbFullDirPath, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR);
	HxSTD_StrNCat( szUsbFullFilePath, AP_SPEC_MESAT_SETUPINFO_FILENAME_REGISTRY_REG_USB, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR );
	DBFILE_CHECK_VAR(szUsbFullFilePath);

	return ERR_OK;
}

STATIC HERROR 	xmgr_setting_ImportExportngsCheckUsb (HCHAR *pszUsbMount)
{
	HERROR 				hErr = ERR_FAIL;
	HINT32				i =0;
	SvcFs_DevInfo_t 		stMwDevInfo;

	HxLOG_Trace();

	/*  USB stick 에만 해야함 */
	for( i =0; i < eSVC_FS_DEV_MAX; i++ )
	{
		HxSTD_MemSet( &stMwDevInfo, 0x00, sizeof( SvcFs_DevInfo_t ));
		hErr = SVC_FS_GetDeviceInfo( i , &stMwDevInfo );
		if( hErr == ERR_OK )
		{
			HxLOG_Print(" bConnected : %d,  bMounted : %d, eDevType : %d\n" , stMwDevInfo.bConnected, stMwDevInfo.bMounted, stMwDevInfo.eDevType );
			/* usb stick이 연결되어 있음 */
			if( (stMwDevInfo.bConnected == TRUE)
				&& (stMwDevInfo.bMounted == TRUE )
				&& (stMwDevInfo.eDevType == eSVC_FS_DevType_UsbMem )
			)
			{
				HLIB_STD_StrNCpySafe(pszUsbMount, (const HCHAR *)stMwDevInfo.szMountPath, SVC_FS_MOUNT_PATH_LEN);
				break;
			}
		}
	}

	if( i >= eSVC_FS_DEV_MAX )
	{
		HxLOG_Error("[Func:%s, Line:%d] No Usb\n" , __FUNCTION__, __LINE__ );
		hErr = ERR_FAIL;
	}

	HxLOG_Trace();
	return hErr;
}

STATIC HERROR	xmgr_setting_ExportDbAsync_MeSat(void)
{
	HCHAR		szUsbFullDirPath[AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR];
	HERROR 		hErr = ERR_FAIL;

	HxLOG_Trace();
	HxSTD_MemSet( szUsbFullDirPath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR );

	/*  USB stick 에만 해야함 */
	hErr = xmgr_setting_ImportExportngsCheckUsb( szUsbFullDirPath );
	if( hErr == ERR_OK )
	{
		BUS_MGR_Create("xproc_setting_ImportExportDb", APP_DEFAULT_PRIORITY, xproc_setting_ImportExportDb, 0, 0, 0, 0);
		BUS_PostMessage(NULL, eMEVT_SETTING_SETUP_INFO_GET, (Handle_t)NULL, 0, 0, 0);
	}
	else
	{
		HxLOG_Error("[Func:%s, Line:%d] No Usb\n" , __FUNCTION__, __LINE__ );
	}

	HxLOG_Trace();
	return hErr;
}

STATIC HERROR	xmgr_setting_ExportDbSync_MeSat(void)
{
	HERROR 			hErr = ERR_FAIL;
	HCHAR			szUsbFullFilePath[AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME],
					szUsbFullDirPath[AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR];
	HINT32			iError=eDxIMEXPORT_ERROR_FAILURE;

	HxLOG_Trace();

	HxSTD_MemSet( szUsbFullFilePath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME );
	HxSTD_MemSet( szUsbFullDirPath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR );

	/*  USB stick 에만 해야함 */
	hErr = xmgr_setting_ImportExportngsCheckUsb( szUsbFullDirPath );
	if( hErr != ERR_OK )
	{
		HxLOG_Error("[Func:%s, Line:%d] No Usb\n" , __FUNCTION__, __LINE__ );
		iError = eDxIMEXPORT_ERROR_NO_USB;
		goto Error;
		//return ERR_FAIL;
	}

	HxLOG_Print(" [%s,%d]szUsbFullDirPath : %s \n" , __FUNCTION__, __LINE__ , szUsbFullDirPath);
	HxSTD_StrNCat( szUsbFullDirPath, AP_SPEC_MESAT_SETUPINFO_UMDOWN_FOLDER,  (AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR /2 ) );

	hErr = xmgr_setting_exportCheckFile();
	if ( hErr!= ERR_OK )
	{
		HxLOG_Error("[Func:%s, Line:%d] InCorrect Files In Directory Fail\n",__FUNCTION__,__LINE__);
		goto Error;
	}

	hErr = xmgr_setting_PrepareDirectory((const HUINT8 *)szUsbFullDirPath);
	if( hErr != ERR_OK )
	{
		HxLOG_Error("[Func:%s, Line:%d] Create Directory Fail!! %s \n" , __FUNCTION__, __LINE__,szUsbFullDirPath );
		goto Error;
		//return ERR_FAIL;
	}

	/** si 관련 db **/
		/* DxBouquet_t */
	HxSTD_MemSet( szUsbFullFilePath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME );
	HLIB_STD_StrNCpySafe(szUsbFullFilePath, szUsbFullDirPath, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR);
	HxSTD_StrNCat( szUsbFullFilePath, AP_SPEC_MESAT_SETUPINFO_FILENAME_BOUQUET_USB , AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR);

	hErr = xmgr_setting_CopyFile((HUINT8 *)AP_SPEC_MESAT_SETUPINFO_FILENAME_BOUQUET , (HUINT8 *)szUsbFullFilePath);
	if( hErr != ERR_OK )	{ HxLOG_Error("[Func:%s, Line:%d] Copy Fail!! ,%s \n" , __FUNCTION__, __LINE__,AP_SPEC_MESAT_SETUPINFO_FILENAME_BOUQUET );	goto Error;}

		/* DxNetwork_t */
	HxSTD_MemSet( szUsbFullFilePath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME );
	HLIB_STD_StrNCpySafe(szUsbFullFilePath, szUsbFullDirPath, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR);
	HxSTD_StrNCat( szUsbFullFilePath, AP_SPEC_MESAT_SETUPINFO_FILENAME_NETWORK_USB, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR );

	hErr = xmgr_setting_CopyFile((HUINT8 *)AP_SPEC_MESAT_SETUPINFO_FILENAME_NETWORK , (HUINT8 *)szUsbFullFilePath);
	if( hErr != ERR_OK )	{ HxLOG_Error("[Func:%s, Line:%d] Copy Fail!! ,%s \n" , __FUNCTION__, __LINE__,AP_SPEC_MESAT_SETUPINFO_FILENAME_NETWORK );	goto Error;}

		/* DxService_t */
	HxSTD_MemSet( szUsbFullFilePath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME );
	HLIB_STD_StrNCpySafe(szUsbFullFilePath, szUsbFullDirPath, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR);
	HxSTD_StrNCat( szUsbFullFilePath, AP_SPEC_MESAT_SETUPINFO_FILENAME_SERVICE_USB, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR );

	hErr = xmgr_setting_CopyFile((HUINT8 *)AP_SPEC_MESAT_SETUPINFO_FILENAME_SERVICE , (HUINT8 *)szUsbFullFilePath);
	if( hErr != ERR_OK )	{ HxLOG_Error("[Func:%s, Line:%d] Copy Fail!! ,%s \n" , __FUNCTION__, __LINE__,AP_SPEC_MESAT_SETUPINFO_FILENAME_SERVICE );	goto Error;}

		/* DxProvider_t */
	HxSTD_MemSet( szUsbFullFilePath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME );
	HLIB_STD_StrNCpySafe(szUsbFullFilePath, szUsbFullDirPath, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR);
	HxSTD_StrNCat( szUsbFullFilePath, AP_SPEC_MESAT_SETUPINFO_FILENAME_PROVIDER_USB, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR );

	hErr = xmgr_setting_CopyFile((HUINT8 *)AP_SPEC_MESAT_SETUPINFO_FILENAME_PROVIDER , (HUINT8 *)szUsbFullFilePath);
	if( hErr != ERR_OK )	{ HxLOG_Error("[Func:%s, Line:%d] Copy Fail!! ,%s \n" , __FUNCTION__, __LINE__,AP_SPEC_MESAT_SETUPINFO_FILENAME_PROVIDER );	goto Error;}

		/* DxTransponder_t */
	HxSTD_MemSet( szUsbFullFilePath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME );
	HLIB_STD_StrNCpySafe(szUsbFullFilePath, szUsbFullDirPath, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR);
	HxSTD_StrNCat( szUsbFullFilePath, AP_SPEC_MESAT_SETUPINFO_FILENAME_TRANSPONDER_USB , AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR );

	hErr = xmgr_setting_CopyFile((HUINT8 *)AP_SPEC_MESAT_SETUPINFO_FILENAME_TRANSPONDER , (HUINT8 *)szUsbFullFilePath);
	if( hErr != ERR_OK )	{ HxLOG_Error("[Func:%s, Line:%d] Copy Fail!! ,%s \n" , __FUNCTION__, __LINE__,AP_SPEC_MESAT_SETUPINFO_FILENAME_TRANSPONDER );	goto Error;}

		/* DxGroup_t */
	HxSTD_MemSet( szUsbFullFilePath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME );
	HLIB_STD_StrNCpySafe(szUsbFullFilePath, szUsbFullDirPath, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR);
	HxSTD_StrNCat( szUsbFullFilePath, AP_SPEC_MESAT_SETUPINFO_FILENAME_GROUP_USB, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR );

	hErr = xmgr_setting_CopyFile((HUINT8 *)AP_SPEC_MESAT_SETUPINFO_FILENAME_GROUP , (HUINT8 *)szUsbFullFilePath);
	if( hErr != ERR_OK )	{ HxLOG_Error("[Func:%s, Line:%d] Copy Fail!! ,%s \n" , __FUNCTION__, __LINE__,AP_SPEC_MESAT_SETUPINFO_FILENAME_GROUP );	goto Error;}

	/* DxGroupList_t */
	HxSTD_MemSet( szUsbFullFilePath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME );
	HLIB_STD_StrNCpySafe(szUsbFullFilePath, szUsbFullDirPath, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR);
	HxSTD_StrNCat( szUsbFullFilePath, AP_SPEC_MESAT_SETUPINFO_FILENAME_GROUPLIST_USB, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR );

	hErr = xmgr_setting_CopyFile((HUINT8 *)AP_SPEC_MESAT_SETUPINFO_FILENAME_GROUPLIST , (HUINT8 *)szUsbFullFilePath);
	if( hErr != ERR_OK )	{ HxLOG_Error("[Func:%s, Line:%d] Copy Fail!! ,%s \n" , __FUNCTION__, __LINE__,AP_SPEC_MESAT_SETUPINFO_FILENAME_GROUPLIST );	goto Error;}

	/* setup 관련 db */
		/* DxDBSETUP_SYSTEM */
	HxSTD_MemSet( szUsbFullFilePath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME );
	HLIB_STD_StrNCpySafe(szUsbFullFilePath, szUsbFullDirPath, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR);
	HxSTD_StrNCat( szUsbFullFilePath, AP_SPEC_MESAT_SETUPINFO_FILENAME_DBSETUP_SYSTEM_USB, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR );

	hErr = xmgr_setting_CopyFile((HUINT8 *)AP_SPEC_MESAT_SETUPINFO_FILENAME_DBSETUP_SYSTEM , (HUINT8 *)szUsbFullFilePath);
	if( hErr != ERR_OK )	{ HxLOG_Error("[Func:%s, Line:%d] Copy Fail!! ,%s \n" , __FUNCTION__, __LINE__,AP_SPEC_MESAT_SETUPINFO_FILENAME_DBSETUP_SYSTEM );	goto Error;}

	/* setup 관련 db */

		/* DxUDBMODULE */
	HxSTD_MemSet( szUsbFullFilePath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME );
	HLIB_STD_StrNCpySafe(szUsbFullFilePath, szUsbFullDirPath, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR);
	HxSTD_StrNCat( szUsbFullFilePath, AP_SPEC_MESAT_SETUPINFO_FILENAME_UDBMODULE_USB, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR );

	hErr = xmgr_setting_CopyFile((HUINT8 *)AP_SPEC_MESAT_SETUPINFO_FILENAME_UDBMODULE , (HUINT8 *)szUsbFullFilePath);
	if( hErr != ERR_OK )	{ HxLOG_Error("[Func:%s, Line:%d] Copy Fail!! ,%s \n" , __FUNCTION__, __LINE__,AP_SPEC_MESAT_SETUPINFO_FILENAME_UDBMODULE );	goto Error;}

	/* antinfo 관련 db*/
	HxSTD_MemSet( szUsbFullFilePath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME );
	HLIB_STD_StrNCpySafe(szUsbFullFilePath, szUsbFullDirPath, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR);
	HxSTD_StrNCat( szUsbFullFilePath, AP_SPEC_MESAT_SETUPINFO_FILENAME_ANTINFO_USB, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR );

	hErr = xmgr_setting_CopyFile((HUINT8 *)AP_SPEC_MESAT_SETUPINFO_FILENAME_ANTINFO, (HUINT8 *)szUsbFullFilePath);
	if( hErr != ERR_OK )	{ HxLOG_Error("[Func:%s, Line:%d] Copy Fail!! ,%s \n" , __FUNCTION__, __LINE__,AP_SPEC_MESAT_SETUPINFO_FILENAME_ANTINFO );	goto Error;}

	/* registry.reg */
	HxSTD_MemSet( szUsbFullFilePath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME );
	HLIB_STD_StrNCpySafe(szUsbFullFilePath, szUsbFullDirPath, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR);
	HxSTD_StrNCat( szUsbFullFilePath, AP_SPEC_MESAT_SETUPINFO_FILENAME_REGISTRY_REG_USB, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR );

	hErr = xmgr_setting_CopyFile((HUINT8 *)AP_SPEC_MESAT_SETUPINFO_FILENAME_REGISTRY_REG , (HUINT8 *)szUsbFullFilePath);
	if( hErr != ERR_OK )	{ HxLOG_Error("[Func:%s, Line:%d] Copy Fail!! ,%s \n" , __FUNCTION__, __LINE__,AP_SPEC_MESAT_SETUPINFO_FILENAME_REGISTRY_REG );	goto Error;}


	BUS_PostMessageToOM(NULL, eMEVT_SYSCTL_EXPORT_DB_OK, HANDLE_NULL, 0, 0, 0);

	return ERR_OK;

Error:
	BUS_PostMessageToOM(NULL, eMEVT_SYSCTL_EXPORT_DB_FAIL, HANDLE_NULL, iError, 0, 0);
	return ERR_FAIL;
}

STATIC HERROR	xmgr_setting_ImportDbAsync_MeSat(void)
{
	HCHAR		szUsbFullDirPath[AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR];
	HERROR 		hErr = ERR_FAIL;

	HxLOG_Trace();

	HxSTD_MemSet( szUsbFullDirPath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR );

	/*  USB stick 에만 해야함 */
	hErr = xmgr_setting_ImportExportngsCheckUsb( szUsbFullDirPath );
	if( hErr == ERR_OK )
	{
		BUS_MGR_Create("xproc_setting_ImportExportDb", APP_DEFAULT_PRIORITY, xproc_setting_ImportExportDb, 0, 0, 0, 0);
		BUS_PostMessage(NULL, eMEVT_SETTING_SETUP_INFO_SET, (Handle_t)NULL, 0, 0, 0);
	}
	else
	{
		HxLOG_Error("[Func:%s, Line:%d] No Usb\n" , __FUNCTION__, __LINE__ );
	}

	HxLOG_Trace();
	return hErr;
}

STATIC HERROR	xmgr_setting_ImportDbSync_MeSat(void)
{
	HERROR 			hErr = ERR_FAIL;
	HCHAR			szUsbFullFilePath[AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME],
					szUsbFullDirPath[AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR];
	HINT32			iError=eDxIMEXPORT_ERROR_FAILURE;

	HxLOG_Trace();

	HxSTD_MemSet( szUsbFullFilePath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME );
	HxSTD_MemSet( szUsbFullDirPath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR );

	/*  USB stick 에만 해야함 */
	hErr = xmgr_setting_ImportExportngsCheckUsb( szUsbFullDirPath );
	if( hErr != ERR_OK )
	{
		HxLOG_Error("[Func:%s, Line:%d] No Usb\n" , __FUNCTION__, __LINE__ );
		iError=eDxIMEXPORT_ERROR_NO_USB;
		goto Error;
		//return ERR_FAIL;
	}

	//HxSTD_StrNCat( szUsbFullDirPath, AP_SPEC_MESAT_SETUPINFO_UMUP_FOLDER , (AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR/2));
	HxSTD_StrNCat( szUsbFullDirPath, AP_SPEC_MESAT_SETUPINFO_UMDOWN_FOLDER , (AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR/2));

	// check db file in dlogger
	hErr = xmgr_setting_importCheckFile((const HUINT8 *)szUsbFullDirPath);
	if ( hErr != ERR_OK )
	{
		HxLOG_Error("[Func:%s, Line:%d] InCorrect Files In Directory Fail!! %s \n" , __FUNCTION__, __LINE__,szUsbFullDirPath );
		goto Error;
	}

	hErr = xmgr_setting_PrepareDirectory((const HUINT8 *)AP_SPEC_MESAT_SETUPINFO_DLOGGER_FOLDER);
	if( hErr != ERR_OK )
	{
		HxLOG_Error("[Func:%s, Line:%d] Create Directory Fail!! %s \n" , __FUNCTION__, __LINE__,szUsbFullDirPath );
		goto Error;
		//return ERR_FAIL;
	}

	/** si 관련 db **/
		/* DxBouquet_t */
	HxSTD_MemSet( szUsbFullFilePath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME );
	HLIB_STD_StrNCpySafe(szUsbFullFilePath, szUsbFullDirPath, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR);
	HxSTD_StrNCat( szUsbFullFilePath, AP_SPEC_MESAT_SETUPINFO_FILENAME_BOUQUET_USB, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR );

	hErr = xmgr_setting_CopyFile((HUINT8 *)szUsbFullFilePath, (HUINT8 *)AP_SPEC_MESAT_SETUPINFO_FILENAME_BOUQUET);
	if( hErr != ERR_OK )	goto Error;	//{ HxLOG_Error("[Func:%s, Line:%d] Copy Fail!! ,%s \n" , __FUNCTION__, __LINE__, szUsbFullFilePath );}

		/* DxNetwork_t */
	HxSTD_MemSet( szUsbFullFilePath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME );
	HLIB_STD_StrNCpySafe(szUsbFullFilePath, szUsbFullDirPath, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR);
	HxSTD_StrNCat( szUsbFullFilePath, AP_SPEC_MESAT_SETUPINFO_FILENAME_NETWORK_USB , AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR);

	hErr = xmgr_setting_CopyFile((HUINT8 *)szUsbFullFilePath, (HUINT8 *)AP_SPEC_MESAT_SETUPINFO_FILENAME_NETWORK );
	if( hErr != ERR_OK )	goto Error;	//{ HxLOG_Error("[Func:%s, Line:%d] Copy Fail!! ,%s \n" , __FUNCTION__, __LINE__, szUsbFullFilePath );}

		/* DxService_t */
	HxSTD_MemSet( szUsbFullFilePath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME );
	HLIB_STD_StrNCpySafe(szUsbFullFilePath, szUsbFullDirPath, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR);
	HxSTD_StrNCat( szUsbFullFilePath, AP_SPEC_MESAT_SETUPINFO_FILENAME_SERVICE_USB, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR );

	hErr = xmgr_setting_CopyFile((HUINT8 *)szUsbFullFilePath, (HUINT8 *)AP_SPEC_MESAT_SETUPINFO_FILENAME_SERVICE );
	if( hErr != ERR_OK )	goto Error;	//{ HxLOG_Error("[Func:%s, Line:%d] Copy Fail!! ,%s \n" , __FUNCTION__, __LINE__, szUsbFullFilePath );}

		/* DxProvider_t */
	HxSTD_MemSet( szUsbFullFilePath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME );
	HLIB_STD_StrNCpySafe(szUsbFullFilePath, szUsbFullDirPath, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR);
	HxSTD_StrNCat( szUsbFullFilePath, AP_SPEC_MESAT_SETUPINFO_FILENAME_PROVIDER_USB, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR );

	hErr = xmgr_setting_CopyFile( (HUINT8 *)szUsbFullFilePath, (HUINT8 *)AP_SPEC_MESAT_SETUPINFO_FILENAME_PROVIDER );
	if( hErr != ERR_OK )	goto Error;	//{ HxLOG_Error("[Func:%s, Line:%d] Copy Fail!! ,%s \n" , __FUNCTION__, __LINE__, szUsbFullFilePath );}

		/* DxTransponder_t */
	HxSTD_MemSet( szUsbFullFilePath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME );
	HLIB_STD_StrNCpySafe(szUsbFullFilePath, szUsbFullDirPath, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR);
	HxSTD_StrNCat( szUsbFullFilePath, AP_SPEC_MESAT_SETUPINFO_FILENAME_TRANSPONDER_USB, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR );

	hErr = xmgr_setting_CopyFile( (HUINT8 *)szUsbFullFilePath, (HUINT8 *)AP_SPEC_MESAT_SETUPINFO_FILENAME_TRANSPONDER );
	if( hErr != ERR_OK )	goto Error;	//{ HxLOG_Error("[Func:%s, Line:%d] Copy Fail!! ,%s \n" , __FUNCTION__, __LINE__, szUsbFullFilePath );}

		/* DxGroup_t */
	HxSTD_MemSet( szUsbFullFilePath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME );
	HLIB_STD_StrNCpySafe(szUsbFullFilePath, szUsbFullDirPath, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR);
	HxSTD_StrNCat( szUsbFullFilePath, AP_SPEC_MESAT_SETUPINFO_FILENAME_GROUP_USB, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR );

	hErr = xmgr_setting_CopyFile( (HUINT8 *)szUsbFullFilePath, (HUINT8 *)AP_SPEC_MESAT_SETUPINFO_FILENAME_GROUP );
	if( hErr != ERR_OK )	goto Error;	//{ HxLOG_Error("[Func:%s, Line:%d] Copy Fail!! ,%s \n" , __FUNCTION__, __LINE__, szUsbFullFilePath );}

	/* DxGroupList_t */
	HxSTD_MemSet( szUsbFullFilePath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME );
	HLIB_STD_StrNCpySafe(szUsbFullFilePath, szUsbFullDirPath, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR);
	HxSTD_StrNCat( szUsbFullFilePath, AP_SPEC_MESAT_SETUPINFO_FILENAME_GROUPLIST_USB, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR );

	hErr = xmgr_setting_CopyFile( (HUINT8 *)szUsbFullFilePath, (HUINT8 *)AP_SPEC_MESAT_SETUPINFO_FILENAME_GROUPLIST );
	if( hErr != ERR_OK )	goto Error; //{ HxLOG_Error("[Func:%s, Line:%d] Copy Fail!! ,%s \n" , __FUNCTION__, __LINE__, szUsbFullFilePath );}

	/* setup 관련 db */
		/* DxDBSETUP_SYSTEM */
	HxSTD_MemSet( szUsbFullFilePath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME );
	HLIB_STD_StrNCpySafe(szUsbFullFilePath, szUsbFullDirPath, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR);
	HxSTD_StrNCat( szUsbFullFilePath, AP_SPEC_MESAT_SETUPINFO_FILENAME_DBSETUP_SYSTEM_USB, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR );

	hErr = xmgr_setting_CopyFile((HUINT8 *)szUsbFullFilePath, (HUINT8 *)AP_SPEC_MESAT_SETUPINFO_FILENAME_DBSETUP_SYSTEM );
	if( hErr != ERR_OK )	goto Error;	//{ HxLOG_Error("[Func:%s, Line:%d] Copy Fail!! ,%s \n" , __FUNCTION__, __LINE__, szUsbFullFilePath );}

	/* setup 관련 db */

		/* DxUDBMODULE */
	HxSTD_MemSet( szUsbFullFilePath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME );
	HLIB_STD_StrNCpySafe(szUsbFullFilePath, szUsbFullDirPath, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR);
	HxSTD_StrNCat( szUsbFullFilePath, AP_SPEC_MESAT_SETUPINFO_FILENAME_UDBMODULE_USB, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR );

	hErr = xmgr_setting_CopyFile((HUINT8 *)szUsbFullFilePath, (HUINT8 *)AP_SPEC_MESAT_SETUPINFO_FILENAME_UDBMODULE );
	if( hErr != ERR_OK )	goto Error;	//{ HxLOG_Error("[Func:%s, Line:%d] Copy Fail!! ,%s \n" , __FUNCTION__, __LINE__, szUsbFullFilePath );}

	/* DxAntInfo 관련 db */
	HxSTD_MemSet( szUsbFullFilePath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME );
	HLIB_STD_StrNCpySafe(szUsbFullFilePath, szUsbFullDirPath, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR);
	HxSTD_StrNCat( szUsbFullFilePath, AP_SPEC_MESAT_SETUPINFO_FILENAME_ANTINFO_USB, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR );

	hErr = xmgr_setting_CopyFile((HUINT8 *)szUsbFullFilePath, (HUINT8 *)AP_SPEC_MESAT_SETUPINFO_FILENAME_ANTINFO );
	if( hErr != ERR_OK )	goto Error;	//{ HxLOG_Error("[Func:%s, Line:%d] Copy Fail!! ,%s \n" , __FUNCTION__, __LINE__, szUsbFullFilePath );}

	/* registry.reg 관련 db */
	HxSTD_MemSet( szUsbFullFilePath, 0x00, AP_SPEC_MESAT_SETUPINFO_MAX_FILENAME );
	HLIB_STD_StrNCpySafe(szUsbFullFilePath, szUsbFullDirPath, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR);
	HxSTD_StrNCat( szUsbFullFilePath, AP_SPEC_MESAT_SETUPINFO_FILENAME_REGISTRY_REG_USB, AP_SPEC_MESAT_SETUPINFO_MAX_FULL_DIR );

	hErr = xmgr_setting_CopyFile((HUINT8 *)szUsbFullFilePath, (HUINT8 *)AP_SPEC_MESAT_SETUPINFO_FILENAME_REGISTRY_REG );
	if( hErr != ERR_OK )	goto Error; //{ HxLOG_Error("[Func:%s, Line:%d] Copy Fail!! ,%s \n" , __FUNCTION__, __LINE__, szUsbFullFilePath );}


	BUS_PostMessageToOM(NULL, eMEVT_SYSCTL_IMPORT_DB_OK, HANDLE_NULL, 0, 0, 0);

	HxLOG_Trace();
	return ERR_OK;

Error:
	HxLOG_Error("[Func:%s, Line:%d] Copy Fail!! ,%s \n" , __FUNCTION__, __LINE__, szUsbFullFilePath );
	BUS_PostMessageToOM(NULL, eMEVT_SYSCTL_IMPORT_DB_FAIL, HANDLE_NULL, iError, 0, 0);
	return ERR_FAIL;
}


STATIC SetupInfoContents_t* local_Setup_Info_GetContext(void)
{
	return &s_SetupInfoContext;
}

#if 0
STATIC BUS_Result_t local_Setup_Info_InitContent(SetupInfoContents_t *pstContents , HINT32 p1)
{
	HERROR		hErr = ERR_FAIL;

	return hErr;
}
#endif

//MSG_GWM_CREATE
STATIC BUS_Result_t local_Setup_Info_MsgBusCreate(SetupInfoContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	/* do nothing */
	return ERR_BUS_NO_ERROR;
}

//MSG_GWM_DESTROY
STATIC BUS_Result_t local_Setup_Info_MsgBusDestroy(SetupInfoContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	/* do nothing */
	return ERR_BUS_NO_ERROR;
}

//eMEVT_SETTING_SETUP_INFO_SET
STATIC BUS_Result_t local_Setup_Info_MsgAppSetupInfoSet(SetupInfoContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR hErr = ERR_FAIL;

	hErr = xmgr_setting_ImportDbSync_MeSat();
	if( hErr == ERR_OK )
	{
		HxLOG_Debug("xmgr_setting_ImportDbSync_MeSat() Ok \n");
	}
	else
	{
		HxLOG_Debug("xmgr_setting_ImportDbSync_MeSat() Fail \n");
	}
	return ERR_BUS_NO_ERROR;
}

//eMEVT_SETTING_SETUP_INFO_GET
STATIC BUS_Result_t local_Setup_Info_MsgAppSetupInfoGet(SetupInfoContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR hErr = ERR_FAIL;

	hErr = xmgr_setting_ExportDbSync_MeSat();
	if( hErr == ERR_OK )
	{
		HxLOG_Debug("xmgr_setting_ExportDbSync_MeSat() Ok \n");
	}
	else
	{
		HxLOG_Debug("xmgr_setting_ExportDbSync_MeSat() Fail \n");
	}
	return ERR_BUS_NO_ERROR;
}


STATIC BUS_Result_t	xproc_setting_ImportExportDb(HINT32 nMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			eResult	= ERR_BUS_NO_ERROR;
	SetupInfoContents_t *pstContents = local_Setup_Info_GetContext();

	switch (nMessage)
	{
	case eMEVT_BUS_CREATE:
		eResult = local_Setup_Info_MsgBusCreate(pstContents, p1, p2, p3);
		break;

	case eMEVT_BUS_DESTROY:
		eResult = local_Setup_Info_MsgBusDestroy(pstContents, p1, p2, p3);
		break;

	case eMEVT_SETTING_SETUP_INFO_SET:
		eResult = local_Setup_Info_MsgAppSetupInfoSet( pstContents, p1, p2, p3 );
		break;

	case eMEVT_SETTING_SETUP_INFO_GET:
		eResult = local_Setup_Info_MsgAppSetupInfoGet( pstContents, p1, p2, p3 );
		break;

	default:
		break;
	}

	if(eResult != ERR_BUS_NO_ERROR)
	{
		return eResult;
	}

	return BUS_ProcessMessageDefault(nMessage, handle, p1, p2, p3);
}

HERROR	xmgr_setting_ExportDb_MeSat(HBOOL bSync)
{
	if (bSync)
		return xmgr_setting_ExportDbSync_MeSat();

	return xmgr_setting_ExportDbAsync_MeSat();
}

HERROR	xmgr_setting_ImportDb_MeSat(HBOOL bSync)
{
	if (bSync)
		return xmgr_setting_ImportDbSync_MeSat();

	return xmgr_setting_ImportDbAsync_MeSat();
}


/**************************************************************************************************/
#define _________AP_Setup_Info_End_Of_File____________________________________________________
/**************************************************************************************************/




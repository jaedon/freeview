/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <oemcontextsizes.h>

ENTER_PKTEST_NAMESPACE_CODE

static OEM_DEVICESTORE_CONTEXT*  ctxOemDeviceStore;
static OEM_DEVICE_HANDLE         hDevice;
static DRM_BOOL                  fContextInitialized;

static DRM_BYTE*                 g_pbDeviceId;
static DRM_DWORD                 g_cbDeviceId;

static DRM_DWORD                 g_dwAttrCount;
static DRM_BYTE*                 g_pbAttrName;
static DRM_DWORD                 g_cbAttrName;
static DRM_BYTE*                 g_pbAttrValue;
static DRM_DWORD                 g_cbAttrValue;

DRM_RESULT DRM_CALL PreTestCase_OEM_DeviceStore(long lTCID, char *strTCName)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ctxOemDeviceStore = (OEM_DEVICESTORE_CONTEXT *) OEM_malloc( SIZEOF( OEM_DEVICESTORE_CONTEXT ) ) ;
    fContextInitialized  = FALSE;
    if(hDevice)
    {
        Oem_DeviceStore_FreeDevice(hDevice);
        hDevice = NULL;
    }
	g_pbAttrValue = NULL;
	g_pbAttrName = NULL;
	g_pbDeviceId = NULL;

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL PostTestCase_OEM_DeviceStore(long lTCID, char *strTCName)
{
    DRM_RESULT dr = DRM_SUCCESS   ;

    SAFE_OEM_FREE( g_pbDeviceId  );
    SAFE_OEM_FREE( g_pbAttrName  );
    SAFE_OEM_FREE( g_pbAttrValue );
    SAFE_OEM_FREE( ctxOemDeviceStore );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_ChkVar_OEM_DeviceStore_DeviceHandle(long argc, char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    if(argc == 1)
    {
        ChkBOOL(hDevice == (OEM_DEVICE_HANDLE)strtoul(argv[0], NULL, 0), DRM_E_FAIL);
    }
    else
    {
        ChkBOOL(hDevice != NULL, DRM_E_FAIL);
    }
ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_ChkVar_OEM_DeviceStore_AttrValue(long argc, char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cbArg;
    if(argc == 1)
    {
		cbArg = strlen( argv[0] ) ;
	    ChkBOOL( g_cbAttrValue == cbArg && ( strncmp( g_pbAttrValue, argv[0], cbArg ) == 0 ), DRM_E_FAIL );
    }
    else
    {
        ChkBOOL(g_pbAttrValue != NULL, DRM_E_FAIL);
    }
ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_SetctxOemDeviceStore(long argc, char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( argc == 1 );
    ctxOemDeviceStore = (OEM_DEVICESTORE_CONTEXT*)strtoul( argv[0], NULL, 0 ) ;
ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_Oem_DeviceStore_Initialize(long argc, char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( Oem_DeviceStore_Initialize( ctxOemDeviceStore, NULL ) );
    fContextInitialized = TRUE;
ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_Oem_DeviceStore_Uninitialize(long argc, char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;
    Oem_DeviceStore_Uninitialize( ctxOemDeviceStore ) ;
    fContextInitialized = FALSE;
ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_Oem_DeviceStore_FreeDevice(long argc, char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    Oem_DeviceStore_FreeDevice( hDevice );
    hDevice = NULL;

    return dr;
}

DRM_RESULT DRM_CALL Test_Oem_DeviceStore_RegisterDevice(long argc, char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTEBLOB DeviceId;

    /* check argc */
    DeviceId.cbBlob = strlen( argv[0] );
    DeviceId.pbBlob = argv[0];

    ChkDR( Oem_DeviceStore_RegisterDevice( ctxOemDeviceStore, DeviceId, &hDevice ) );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_Oem_DeviceStore_UnRegisterDevice(long argc, char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTEBLOB DeviceId;

    ChkArg( argc == 1 );

    DeviceId.pbBlob = argv[0];
    DeviceId.cbBlob = strlen( DeviceId.pbBlob );

    ChkDR( Oem_DeviceStore_UnRegisterDevice( ctxOemDeviceStore, DeviceId ) );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_Oem_DeviceStore_GetFirstDevice(long argc, char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;
    

    ChkDR( Oem_DeviceStore_GetFirstDevice( ctxOemDeviceStore, &hDevice ) );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_Oem_DeviceStore_GetNextDevice(long argc, char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( Oem_DeviceStore_GetNextDevice( ctxOemDeviceStore, &hDevice ) );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_Oem_DeviceStore_GetDeviceByID(long argc, char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTEBLOB DeviceId;

    ChkArg( argc == 1 );
    DeviceId.cbBlob = strlen(argv[0]);
    DeviceId.pbBlob = argv[0];

    ChkDR( Oem_DeviceStore_GetDeviceByID( ctxOemDeviceStore, DeviceId, &hDevice ) );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_Oem_DeviceStore_GetDeviceID(long argc, char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    g_cbDeviceId = 0;
    dr = Oem_DeviceStore_GetDeviceID( hDevice, NULL, &g_cbDeviceId );
    ChkBOOL( dr == DRM_E_BUFFERTOOSMALL, dr );
    
    ChkMem( g_pbDeviceId = OEM_malloc( g_cbDeviceId ) );
    ChkDR( Oem_DeviceStore_GetDeviceID( hDevice, g_pbDeviceId, &g_cbDeviceId ) );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_Oem_DeviceStore_GetDeviceAttr(long argc, char *argv[])
{
    DRM_RESULT   dr          = DRM_SUCCESS;
    DRM_BYTEBLOB attrName    = { 0, 0 };

    ChkArg( argc == 1 );

    g_cbAttrValue = 0;
    attrName.pbBlob = argv[0];
    attrName.cbBlob = strlen( argv[0] );
    
    dr = Oem_DeviceStore_GetDeviceAttr( hDevice, attrName, NULL, &g_cbAttrValue );
    ChkBOOL( dr == DRM_E_BUFFERTOOSMALL, dr );

    ChkMem( g_pbAttrValue = OEM_malloc( g_cbAttrValue ) );
    ChkDR ( Oem_DeviceStore_GetDeviceAttr( hDevice, attrName, g_pbAttrValue, &g_cbAttrValue ) );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_Oem_DeviceStore_GetDeviceAttrCount(long argc, char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( Oem_DeviceStore_GetDeviceAttrCount( hDevice, &g_dwAttrCount ) );
    if(argc >= 1)
    {
        ChkBOOL( g_dwAttrCount == strtoul( argv[0], NULL, 0 ), DRM_E_FAIL );
    }

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_Oem_DeviceStore_GetDeviceAttrByIndex(long argc, char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD dwAttrIndex = 0;

    ChkArg( argc == 1 );

    dwAttrIndex = strtoul( argv[0], NULL, 0 ) ;

    SAFE_OEM_FREE ( g_pbAttrName  );
    SAFE_OEM_FREE ( g_pbAttrValue );
    g_cbAttrName  = 0;
    g_cbAttrValue = 0;

    dr = Oem_DeviceStore_GetDeviceAttrByIndex( hDevice, dwAttrIndex, NULL, &g_cbAttrName, NULL, &g_cbAttrValue ) ;
    ChkBOOL( dr == DRM_E_BUFFERTOOSMALL, dr ) ;

    ChkMem( g_pbAttrName  = OEM_malloc( g_cbAttrName  ) ) ;
    ChkMem( g_pbAttrValue = OEM_malloc( g_cbAttrValue ) ) ;

    ChkDR( Oem_DeviceStore_GetDeviceAttrByIndex( hDevice, dwAttrIndex, g_pbAttrName, &g_cbAttrName, g_pbAttrValue, &g_cbAttrValue ) );

ErrorExit:

    return dr;
}

DRM_RESULT DRM_CALL Test_Oem_DeviceStore_SetDeviceAttr(long argc, char *argv[])
{
    DRM_RESULT   dr        = DRM_SUCCESS ;
    DRM_BYTEBLOB attrName  = { 0, 0 }    ; 
    DRM_BYTEBLOB attrValue = { 0, 0 }    ;

    ChkArg( argc == 2 ) ;

    attrName.pbBlob  = argv[0] ;
    attrName.cbBlob  = strlen( attrName.pbBlob )  ;
    attrValue.pbBlob = argv[1] ;
    attrValue.cbBlob = strlen( attrValue.pbBlob ) ;

    ChkDR( Oem_DeviceStore_SetDeviceAttr( hDevice, attrName, attrValue) );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_Oem_DeviceStore_CommitDevice(long argc, char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( Oem_DeviceStore_CommitDevice( hDevice ) );

ErrorExit:
    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE

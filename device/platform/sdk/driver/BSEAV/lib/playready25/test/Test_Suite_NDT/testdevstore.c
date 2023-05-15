/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include <tstutils.h>
#include <drmcommon.h>
#include <oemdevstore.h>
#include <drmdevstore.h>
#include <testdevstore.h>
#include <drmtestrsautils.h>

ENTER_PKTEST_NAMESPACE_CODE

static OEM_DEVICESTORE_CONTEXT*  ctxOemDeviceStore = {0};
static OEM_DEVICE_HANDLE         hDevice = NULL;
static DRM_BOOL                  fContextInitialized = FALSE;

static DRM_BYTE*                 g_pbDeviceId = NULL;
static DRM_DWORD                 g_cbDeviceId = 0;

static DRM_DWORD                 g_dwAttrCount = 0;
static DRM_BYTE*                 g_pbAttrName = NULL;
static DRM_DWORD                 g_cbAttrName = 0;
static DRM_BYTE*                 g_pbAttrValue = NULL;
static DRM_DWORD                 g_cbAttrValue = 0;

DRM_RESULT DRM_CALL PreTestCase_OEM_DeviceStore(long lTCID, const char *strTCName)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ctxOemDeviceStore = (OEM_DEVICESTORE_CONTEXT *) Oem_MemAlloc( SIZEOF( OEM_DEVICESTORE_CONTEXT ) ) ;
    ZEROMEM( ctxOemDeviceStore, SIZEOF( OEM_DEVICESTORE_CONTEXT ) );
    fContextInitialized  = FALSE;
    g_pbAttrValue = NULL;
    g_pbAttrName = NULL;
    g_pbDeviceId = NULL;

    return dr;
}

DRM_RESULT DRM_CALL PostTestCase_OEM_DeviceStore(long lTCID, const char *strTCName)
{
    DRM_RESULT dr = DRM_SUCCESS;

    SAFE_OEM_FREE( g_pbDeviceId  );
    SAFE_OEM_FREE( g_pbAttrName  );
    SAFE_OEM_FREE( g_pbAttrValue );
    SAFE_OEM_FREE( ctxOemDeviceStore );
    if(hDevice)
    {
        Oem_DeviceStore_FreeDevice(hDevice);
        hDevice = NULL;
    }

    return dr;
}

DRM_RESULT DRM_CALL Test_ChkVar_OEM_DeviceStore_DeviceHandle(long argc, __in_ecount(argc) char *argv[])
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

DRM_RESULT DRM_CALL Test_ChkVar_OEM_DeviceStore_AttrValue(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cbArg;
    if(argc == 1)
    {
        cbArg = ( DRM_DWORD ) DRMCRT_strlen( argv[0] ) ;
        ChkBOOL( g_cbAttrValue == cbArg && ( strncmp( (char *)g_pbAttrValue, argv[0], cbArg ) == 0 ), DRM_E_FAIL );
    }
    else
    {
        ChkBOOL(g_pbAttrValue != NULL, DRM_E_FAIL);
    }
ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_SetctxOemDeviceStore(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( argc == 1 );
    ctxOemDeviceStore = (OEM_DEVICESTORE_CONTEXT*)strtoul( argv[0], NULL, 0 ) ;
ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_Oem_DeviceStore_Initialize(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;
    OEM_DEVICESTORE_INITDATA *devStoreInitData = NULL;

    ChkDR( Oem_DeviceStore_Initialize( ctxOemDeviceStore, devStoreInitData ) );
    fContextInitialized = TRUE;
ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_Oem_DeviceStore_Uninitialize(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;
    Oem_DeviceStore_Uninitialize( ctxOemDeviceStore ) ;
    fContextInitialized = FALSE;

    return dr;
}

DRM_RESULT DRM_CALL Test_Oem_DeviceStore_FreeDevice(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    Oem_DeviceStore_FreeDevice( hDevice );
    hDevice = NULL;

    return dr;
}

DRM_RESULT DRM_CALL Test_Oem_DeviceStore_RegisterDevice(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTEBLOB DeviceId;

    /* check argc */
    DeviceId.cbBlob = ( DRM_DWORD ) DRMCRT_strlen( argv[0] );
    DeviceId.pbBlob = (DRM_BYTE*)argv[0];

    ChkDR( Oem_DeviceStore_RegisterDevice( ctxOemDeviceStore, DeviceId, &hDevice ) );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_Oem_DeviceStore_UnRegisterDevice(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTEBLOB DeviceId;

    ChkArg( argc == 1 );

    DeviceId.pbBlob = (DRM_BYTE*)argv[0];
    DeviceId.cbBlob = ( DRM_DWORD ) DRMCRT_strlen( (const DRM_CHAR *)DeviceId.pbBlob );

    ChkDR( Oem_DeviceStore_UnRegisterDevice( ctxOemDeviceStore, DeviceId ) );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_Oem_DeviceStore_GetFirstDevice(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;


    ChkDR( Oem_DeviceStore_GetFirstDevice( ctxOemDeviceStore, &hDevice ) );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_Oem_DeviceStore_GetNextDevice(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( Oem_DeviceStore_GetNextDevice( ctxOemDeviceStore, &hDevice ) );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_Oem_DeviceStore_GetDeviceByID(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTEBLOB DeviceId;

    ChkArg( argc == 1 );
    DeviceId.cbBlob = ( DRM_DWORD ) DRMCRT_strlen(argv[0]);
    DeviceId.pbBlob = (DRM_BYTE*)argv[0];

    ChkDR( Oem_DeviceStore_GetDeviceByID( ctxOemDeviceStore, DeviceId, &hDevice ) );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_Oem_DeviceStore_GetDeviceID(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    g_cbDeviceId = 0;
    dr = Oem_DeviceStore_GetDeviceID( hDevice, g_pbDeviceId, &g_cbDeviceId );
    ChkBOOL( dr == DRM_E_BUFFERTOOSMALL, dr );

    ChkMem( g_pbDeviceId = (DRM_BYTE*)Oem_MemAlloc( g_cbDeviceId ) );
    ChkDR( Oem_DeviceStore_GetDeviceID( hDevice, g_pbDeviceId, &g_cbDeviceId ) );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_Oem_DeviceStore_GetDeviceAttr(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT   dr          = DRM_SUCCESS;
    DRM_BYTEBLOB attrName    = { 0, 0 };

    ChkArg( argc == 1 );

    g_cbAttrValue = 0;
    attrName.pbBlob = (DRM_BYTE*)argv[0];
    attrName.cbBlob = ( DRM_DWORD ) DRMCRT_strlen( argv[0] );

    dr = Oem_DeviceStore_GetDeviceAttr( hDevice, attrName, g_pbAttrValue, &g_cbAttrValue );
    ChkBOOL( dr == DRM_E_BUFFERTOOSMALL, dr );

    SAFE_OEM_FREE( g_pbAttrValue );
    ChkMem( g_pbAttrValue = (DRM_BYTE*)Oem_MemAlloc( g_cbAttrValue ) );
    ChkDR ( Oem_DeviceStore_GetDeviceAttr( hDevice, attrName, g_pbAttrValue, &g_cbAttrValue ) );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_Oem_DeviceStore_GetDeviceAttrCount(long argc, __in_ecount(argc) char *argv[])
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

DRM_RESULT DRM_CALL Test_Oem_DeviceStore_GetDeviceAttrByIndex(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD dwAttrIndex = 0;

    ChkArg( argc == 1 );

    dwAttrIndex = strtoul( argv[0], NULL, 0 ) ;

    SAFE_OEM_FREE ( g_pbAttrName  );
    SAFE_OEM_FREE ( g_pbAttrValue );
    g_cbAttrName  = 0;
    g_cbAttrValue = 0;

    dr = Oem_DeviceStore_GetDeviceAttrByIndex( hDevice, dwAttrIndex, g_pbAttrName, &g_cbAttrName, g_pbAttrValue, &g_cbAttrValue ) ;
    ChkBOOL( dr == DRM_E_BUFFERTOOSMALL, dr ) ;

    ChkMem( g_pbAttrName  = (DRM_BYTE*)Oem_MemAlloc( g_cbAttrName  ) ) ;
    ChkMem( g_pbAttrValue = (DRM_BYTE*)Oem_MemAlloc( g_cbAttrValue ) ) ;

    ChkDR( Oem_DeviceStore_GetDeviceAttrByIndex( hDevice, dwAttrIndex, g_pbAttrName, &g_cbAttrName, g_pbAttrValue, &g_cbAttrValue ) );

ErrorExit:

    return dr;
}

DRM_RESULT DRM_CALL Test_Oem_DeviceStore_SetDeviceAttr(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT   dr        = DRM_SUCCESS ;
    DRM_BYTEBLOB attrName  = { 0, 0 }    ;
    DRM_BYTEBLOB attrValue = { 0, 0 }    ;

    ChkArg( argc == 2 ) ;

    attrName.pbBlob  = (DRM_BYTE*)argv[0] ;
    attrName.cbBlob  = ( DRM_DWORD ) DRMCRT_strlen( (const DRM_CHAR *)attrName.pbBlob )  ;
    attrValue.pbBlob = (DRM_BYTE*)argv[1] ;
    attrValue.cbBlob = ( DRM_DWORD ) DRMCRT_strlen( (const DRM_CHAR *)attrValue.pbBlob ) ;

    ChkDR( Oem_DeviceStore_SetDeviceAttr( hDevice, attrName, attrValue) );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_Oem_DeviceStore_CommitDevice(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( Oem_DeviceStore_CommitDevice( hDevice ) );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_DRM_DeviceStore_FreeDevice(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_DeviceStore_FreeDevice( hDevice );
    hDevice = NULL;

    return dr;
}

DRM_RESULT DRM_CALL Test_DRM_DeviceStore_UnRegisterDevice(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTEBLOB DeviceCert = { 0, 0 };
    DRM_ID DeviceSerialNumber;

    ChkArg( argc == 2 );

    ChkBOOL( LoadTestFile( NULL, argv[0], &DeviceCert.pbBlob, &DeviceCert.cbBlob ), DRM_E_TEST_FILE_LOAD_ERROR );
    ChkDR( ParseDRMID(&DeviceSerialNumber, argv[1]) );

    ChkDR( DRM_DeviceStore_UnRegisterDevice( ctxOemDeviceStore, DeviceCert, &DeviceSerialNumber ) );

ErrorExit:
    SAFE_OEM_FREE( DeviceCert.pbBlob );
    return dr;
}

DRM_RESULT DRM_CALL Test_DRM_DeviceStore_GetFirstDevice(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( DRM_DeviceStore_GetFirstDevice( ctxOemDeviceStore, &hDevice ) );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_DRM_DeviceStore_GetNextDevice(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( DRM_DeviceStore_GetNextDevice( ctxOemDeviceStore, &hDevice ) );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_DRM_DeviceStore_GetDeviceByID(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTEBLOB DeviceCert = { 0, 0 };
    DRM_ID DeviceSerialNumber;

    ChkArg( argc == 2 );
    ChkBOOL( LoadTestFile( NULL, argv[0], &DeviceCert.pbBlob, &DeviceCert.cbBlob ), DRM_E_TEST_FILE_LOAD_ERROR );
    ChkDR( ParseDRMID(&DeviceSerialNumber, argv[1]) );

    ChkDR( DRM_DeviceStore_GetDeviceByID( ctxOemDeviceStore, DeviceCert, &DeviceSerialNumber, &hDevice ) );

ErrorExit:
    SAFE_OEM_FREE( DeviceCert.pbBlob );
    return dr;
}

DRM_RESULT DRM_CALL Test_DRM_DeviceStore_GetDeviceInfo(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cbDeviceCertificate = 0;
    DRM_BYTE *pbDeviceCertificate = NULL;
    DRM_ID DeviceSerialNumber;

    dr = DRM_DeviceStore_GetDeviceInfo( hDevice,
                                           NULL,
                                           &cbDeviceCertificate,
                                           &DeviceSerialNumber );
    if(dr == DRM_E_BUFFERTOOSMALL)
    {
        ChkMem( pbDeviceCertificate = (DRM_BYTE*)Oem_MemAlloc(cbDeviceCertificate) );
        dr = DRM_DeviceStore_GetDeviceInfo( hDevice,
                                            pbDeviceCertificate,
                                            &cbDeviceCertificate,
                                            &DeviceSerialNumber );
    }
    ChkDR( dr );
ErrorExit:
    SAFE_OEM_FREE( pbDeviceCertificate );
    return dr;
}

DRM_RESULT DRM_CALL Test_DRM_DeviceStore_GetDeviceAttr(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT   dr          = DRM_SUCCESS;
    DRM_BYTEBLOB attrName    = { 0, 0 };

    ChkArg( argc == 1 );

    g_cbAttrValue = 0;
    attrName.pbBlob = (DRM_BYTE*)argv[0];
    attrName.cbBlob = ( DRM_DWORD ) DRMCRT_strlen( argv[0] );

    dr = DRM_DeviceStore_GetDeviceAttr( hDevice, attrName, NULL, &g_cbAttrValue );
    ChkBOOL( dr == DRM_E_BUFFERTOOSMALL, dr );

    SAFE_OEM_FREE( g_pbAttrValue );
    ChkMem( g_pbAttrValue = (DRM_BYTE*)Oem_MemAlloc( g_cbAttrValue ) );
    ChkDR ( DRM_DeviceStore_GetDeviceAttr( hDevice, attrName, g_pbAttrValue, &g_cbAttrValue ) );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_DRM_DeviceStore_GetDeviceAttrCount(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( DRM_DeviceStore_GetDeviceAttrCount( hDevice, &g_dwAttrCount ) );
    if(argc >= 1)
    {
        ChkBOOL( g_dwAttrCount == strtoul( argv[0], NULL, 0 ), DRM_E_FAIL );
    }

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_DRM_DeviceStore_GetDeviceAttrByIndex(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD dwAttrIndex = 0;

    ChkArg( argc == 1 );

    dwAttrIndex = strtoul( argv[0], NULL, 0 ) ;

    SAFE_OEM_FREE ( g_pbAttrName  );
    SAFE_OEM_FREE ( g_pbAttrValue );
    g_cbAttrName  = 0;
    g_cbAttrValue = 0;

    dr = DRM_DeviceStore_GetDeviceAttrByIndex( hDevice, dwAttrIndex, NULL, &g_cbAttrName, NULL, &g_cbAttrValue ) ;
    ChkBOOL( dr == DRM_E_BUFFERTOOSMALL, dr ) ;

    ChkMem( g_pbAttrName  = (DRM_BYTE*)Oem_MemAlloc( g_cbAttrName  ) ) ;
    ChkMem( g_pbAttrValue = (DRM_BYTE*)Oem_MemAlloc( g_cbAttrValue ) ) ;

    ChkDR( DRM_DeviceStore_GetDeviceAttrByIndex( hDevice, dwAttrIndex, g_pbAttrName, &g_cbAttrName, g_pbAttrValue, &g_cbAttrValue ) );

ErrorExit:

    return dr;
}

DRM_RESULT DRM_CALL Test_DRM_DeviceStore_SetDeviceAttr(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT   dr        = DRM_SUCCESS ;
    DRM_BYTEBLOB attrName  = { 0, 0 }    ;
    DRM_BYTEBLOB attrValue = { 0, 0 }    ;

    ChkArg( argc == 2 ) ;

    attrName.pbBlob  = (DRM_BYTE*)argv[0] ;
    attrName.cbBlob  = ( DRM_DWORD ) DRMCRT_strlen( (const DRM_CHAR *)attrName.pbBlob )  ;
    attrValue.pbBlob = (DRM_BYTE*)argv[1] ;
    attrValue.cbBlob = ( DRM_DWORD ) DRMCRT_strlen( (const DRM_CHAR *)attrValue.pbBlob ) ;

    ChkDR( DRM_DeviceStore_SetDeviceAttr( hDevice, attrName, attrValue) );

ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL Test_DRM_DeviceStore_IsDeviceValid(long argc, __in_ecount(argc) char *argv[])
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkBOOL( DRM_DeviceStore_IsDeviceValid( NULL, hDevice ), DRM_E_FAIL );

ErrorExit:

    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE

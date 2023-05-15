/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <drmmanager.h>
#include <drmdevstore.h>
#include <drmdevicestore.h>
#include <testdevicestore.h>


ENTER_PKTEST_NAMESPACE_CODE

/*Most of function has additional argument using as flags to
  decide which input should be overwritten:
    1st bit overwrite g_pManagerContext
*/

/*global variables*/
DRM_CONST_STRING *g_pwszDeviceStoreName = NULL;
DRM_APP_CONTEXT *g_pManagerContext = NULL;

DRM_DEVICESTORE_CONTEXT *g_pDeviceStoreContext = NULL;
DRM_DEVICESTORE_HANDLE g_hDeviceHandle = NULL;

/*multiple stores*/
char *g_szFilePath=NULL;

/* TestDeviceStoreRegisterDevice takes a device ID and creates
an entry for it in the HDS.

argv[0]: deviceid to register
argv[1]: optional: context is NULL
argv[2]: optional: device handle is NULL 
 
*/
DRM_RESULT DRM_CALL TestDeviceStoreRegisterDevice(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTEBLOB DeviceID = {0};
    DRM_DWORD iOption1 = 1; //default to TRUE so we pass in the real context
    DRM_DWORD iOption2 = 1;

    ChkArg( argc >= 1 );

    if (argv[0])
    {
        DeviceID.pbBlob = (DRM_BYTE *)argv[0];
        DeviceID.cbBlob = ( DRM_DWORD ) strlen( argv[0] );
    }

    if (argc >= 2 && argv[1] != NULL)
        iOption1 = atol(argv[1]);
    if (argc >= 3 && argv[2] != NULL)
        iOption2 = atol(argv[2]);
    
    ChkDR( DRM_DVS_RegisterDevice( iOption1 ? g_pDeviceStoreContext : NULL,
                                   DeviceID, 
                                   iOption2 ? &g_hDeviceHandle : NULL ) );

ErrorExit:
    return dr;
}

/*  TestDeviceStoreUnRegisterDevice removes a registered device's
entry from the HDS.

argv[0]: deviceid to unregister
argv[1]: optional: context is NULL
  
*/

DRM_RESULT DRM_CALL TestDeviceStoreUnRegisterDevice(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTEBLOB DeviceID = {0};
    DRM_DWORD iOption1 = 1; //default to TRUE so we pass in the real context

    ChkArg( argc >= 1 );

    if ( argv[0] )
    {
        DeviceID.pbBlob = (DRM_BYTE *)argv[0];
        DeviceID.cbBlob =  ( DRM_DWORD ) strlen( argv[0] );
    }

    if(argc>=2 && argv[1] != NULL)
        iOption1 = atol(argv[1]);
    

    ChkDR( DRM_DVS_UnRegisterDevice( iOption1? g_pDeviceStoreContext : NULL, DeviceID ) );

ErrorExit:
    return dr;
}

/* TestDeviceStoreCommitDevice is used to save changes made to
a device handle for a registered device to the HDS.

argv[0]: optional: Device handle is NULL. 

*/

DRM_RESULT DRM_CALL TestDeviceStoreCommitDevice(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD iOption1 = 1; //default to TRUE so we pass in the real handle

    if((argc >= 1) && (0 !=  ( DRM_DWORD ) strlen(argv[0])))
        iOption1 = atol(argv[0]);

    ChkDR( DRM_DVS_CommitDevice( iOption1 ? g_hDeviceHandle : NULL ) );

ErrorExit:
    return dr;
}

/*  TestDeviceStoreFreeDevice clears an existing device handle.
If the device handle's flag fDontCommitOnFree is TRUE, any changes
will be lost when the handle is freed.  Otherwise DRM_DVS_FreeDevice
will call DRM_DVS_CommitDevice during the free operation.

argv[0]: optional: device handle is NULL. 
argv[1]: optional: don't commit any changes during free operation

*/

DRM_RESULT DRM_CALL TestDeviceStoreFreeDevice(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    _DeviceHandle *pDeviceHandle = NULL;
    DRM_DWORD iOption1 = 1; //default to TRUE so we pass in the real handle

    if((argc >= 1) && (0 !=  ( DRM_DWORD ) strlen(argv[0])))
        iOption1 = atol(argv[0]);
    if (argc >= 2)
    {
        ChkArg(argv[1]);
        if (0 != MEMCMP(L"TRUE", argv[1],  ( DRM_DWORD ) strlen(argv[1])) && g_hDeviceHandle != NULL)
        {
            //Don't commit any changes when freeing this device handle
            pDeviceHandle = (_DeviceHandle *)g_hDeviceHandle;
            pDeviceHandle->fDontCommitOnFree = TRUE;
        }
    }

    DRM_DVS_FreeDevice( iOption1? g_hDeviceHandle : NULL );
    g_hDeviceHandle = NULL;

ErrorExit:
    return dr;
}

/* TestDeviceStoreGetDeviceByID returns a device handle for a
registered device, given a device ID.

argv[0]: deviceid to register
argv[1]: optional: context is NULL
argv[2]: optional: device handle is NULL

*/

DRM_RESULT DRM_CALL TestDeviceStoreGetDeviceByID(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTEBLOB DeviceID = {0};
    DRM_DWORD iOption1 = 1; //default to TRUE so we pass in the real context/handle
    DRM_DWORD iOption2 = 1;

    ChkArg( argc >= 1 );

    if (argv[0])
    {
        DeviceID.pbBlob = (DRM_BYTE *)argv[0];
        DeviceID.cbBlob =  ( DRM_DWORD ) strlen( argv[0] );
    }

    if (argc >= 2 && argv[1] != NULL)
        iOption1 = atol(argv[1]);
    if (argc >= 3 && argv[2] != NULL)
        iOption2 = atol(argv[2]);

    ChkDR( DRM_DVS_GetDeviceByID( iOption1? g_pDeviceStoreContext : NULL, 
                                  DeviceID, 
                                  iOption2? &g_hDeviceHandle : NULL) );

ErrorExit:
    return dr;
}

/* TestDeviceStoreGetFirstDevice returns the first registered device
the the HDS.

argv[0]: optional: context is NULL
argv[1]: optional: device handle is NULL

*/
DRM_RESULT DRM_CALL TestDeviceStoreGetFirstDevice(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD iOption1 = 1; //default to TRUE so we pass in the real context/handle
    DRM_DWORD iOption2 = 1;
    
    if((argc >= 1) && (0 !=  ( DRM_DWORD ) strlen(argv[0])))
        iOption1 = atol(argv[0]);
    if (argc >= 2 && argv[1] != NULL)
        iOption2 = atol(argv[1]);

    ChkDR( DRM_DVS_GetFirstDevice( iOption1? g_pDeviceStoreContext : NULL,
                                   iOption2? &g_hDeviceHandle : NULL ) );

ErrorExit:
    return dr;
}

/* TestDeviceStoreGetNextDevice returns the next registered device
found the in the HDS.  If no more devices are present the API call
fails with DRM_E_NOMORE.

argv[0]: optional: context is NULL
argv[1]: optional: device handle is NULL
*/

DRM_RESULT DRM_CALL TestDeviceStoreGetNextDevice(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD iOption1 = 1; //default to TRUE so we pass in the real context/handle
    DRM_DWORD iOption2 = 1;

    if((argc >= 1) && (0 !=  ( DRM_DWORD ) strlen(argv[0])))
        iOption1 = atol(argv[0]);
    if (argc >= 2 && argv[1] != NULL)
        iOption2 = atol(argv[1]);

    ChkDR( DRM_DVS_GetNextDevice( iOption1? g_pDeviceStoreContext : NULL,
                                  iOption2? &g_hDeviceHandle : NULL ) );

ErrorExit:
    return dr;
}

/* TestDeviceStoreGetDeviceID gets the DeviceID from the store
when given a handle to the device.

argv[0]: deviceid to retrieve
argv[1]: optional: device handle is NULL

*/
DRM_RESULT DRM_CALL TestDeviceStoreGetDeviceID(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE *pbDeviceID = NULL;
    DRM_DWORD cbDeviceID = 0;
    DRM_DWORD iOption1 = 1; //default to TRUE so we pass in the real handle

    ChkArg( argc >= 1 );

    if (argc >= 2 && argv[1] != NULL)
        iOption1 = atol(argv[1]);

    dr = DRM_DVS_GetDeviceID( g_hDeviceHandle, NULL, &cbDeviceID );

    if( dr == DRM_E_BUFFERTOOSMALL )
    {
        ChkMem( pbDeviceID = (DRM_BYTE *)Oem_MemAlloc( cbDeviceID ) );
        dr = DRM_DVS_GetDeviceID( iOption1? g_hDeviceHandle : NULL,
                                    pbDeviceID, &cbDeviceID );
    }
    ChkDR( dr );
    
    ChkArg( pbDeviceID != NULL
        && cbDeviceID ==  ( DRM_DWORD ) strlen(argv[0]) );

    ChkArg( MEMCMP( pbDeviceID, argv[0], cbDeviceID ) == 0 );

ErrorExit:
    SAFE_OEM_FREE(pbDeviceID);
    return dr;
}

/* Set an attribute for the device we currently have a handle to.
Changes must be committed with CommitDevice or FreeDevice with
g_hDeviceHandle->fDontCommitOnFree set to TRUE.

argv[0]: attribute name
argv[1]: attribute value
argv[2]: optional: device handle is NULL
*/

DRM_RESULT DRM_CALL TestDeviceStoreSetDeviceAttr(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTEBLOB AttrName = {0};
    DRM_BYTEBLOB AttrValue = {0};
    DRM_DWORD iOption1 = 1; //default to TRUE so we pass in the real handle

    ChkArg( argc >= 2 );

    if (argc >= 3 && argv[2] != NULL)
        iOption1 = atol(argv[2]);

    if(argv[0])
    {
        AttrName.pbBlob = (DRM_BYTE *)argv[0];
        AttrName.cbBlob =  ( DRM_DWORD ) strlen( argv[0] );
    }

    if(argv[1])
    {
        AttrValue.pbBlob = (DRM_BYTE *)argv[1];
        AttrValue.cbBlob =  ( DRM_DWORD ) strlen( argv[1] );
    }
    
    ChkDR( DRM_DVS_SetDeviceAttr( iOption1? g_hDeviceHandle : NULL, 
                                  AttrName, AttrValue ) );

ErrorExit:
    return dr;
}

/* Get a specified attribute from the device we currently have a
handle to.

argv[0]: attribute name
argv[1]: expected attribute value
argv[2]: optional: device handle is NULL 

*/
DRM_RESULT DRM_CALL TestDeviceStoreGetDeviceAttr(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTEBLOB AttrName = {0};
    DRM_BYTE *pbAttrValue = NULL;
    DRM_DWORD cbAttrValue = 0;
    DRM_DWORD iOption1 = 1; //default to TRUE so we pass in the real handle

    ChkArg( argc >= 2 );

    if(argc >= 3 && argv[2] != NULL)
        iOption1 = atol(argv[2]);

    if (argv[0])
    {
        AttrName.pbBlob = (DRM_BYTE *)argv[0];
        AttrName.cbBlob =  ( DRM_DWORD ) strlen( argv[0] );
    }
    
    dr = DRM_DVS_GetDeviceAttr( g_hDeviceHandle, AttrName, NULL, &cbAttrValue );

    if( dr == DRM_E_BUFFERTOOSMALL )
    {
        ChkMem( pbAttrValue = (DRM_BYTE *)Oem_MemAlloc( cbAttrValue ) );
        dr = DRM_DVS_GetDeviceAttr( iOption1? g_hDeviceHandle : NULL,
                                    AttrName, pbAttrValue, &cbAttrValue );
    }
    ChkDR( dr );
    ChkArg( pbAttrValue != NULL
        && cbAttrValue ==  ( DRM_DWORD ) strlen(argv[1]) );

    ChkArg( MEMCMP( pbAttrValue, argv[1], cbAttrValue ) == 0 );

ErrorExit:
    SAFE_OEM_FREE( pbAttrValue );
    return dr;
}

/* get an attribute of the device by it's index and verify that it matches what is given 

argv[0]: attribute index
argv[1]: expected attribute name
argv[2]: expected attribute value
argv[3]: optional: device handle is NULL 

*/

DRM_RESULT DRM_CALL TestDeviceStoreGetDeviceAttrByIndex(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT  dr           = DRM_SUCCESS;
    DRM_BYTE    *pbAttrName  = NULL;
    DRM_DWORD   cbAttrName   = 0;
    DRM_BYTE    *pbAttrValue = NULL;
    DRM_DWORD   cbAttrValue  = 0;
    DRM_DWORD   iAttrIndex   = 0;
    DRM_DWORD   iOption1     = 1; //default to TRUE so we pass in the real handle

    ChkArg( argc >= 3 );

    if ( argv[0] )
        iAttrIndex = atol( argv[0] );

    if ( argc >= 4 && argv[3] != NULL )
        iOption1 = atol( argv[3] );

    dr = DRM_DVS_GetDeviceAttrByIndex( g_hDeviceHandle, iAttrIndex, NULL, &cbAttrName, NULL, &cbAttrValue );

    if( dr == DRM_E_BUFFERTOOSMALL )
    {
        ChkMem( pbAttrName = (DRM_BYTE *)Oem_MemAlloc( cbAttrName ) );
        ChkMem( pbAttrValue = (DRM_BYTE *)Oem_MemAlloc( cbAttrValue ) );
        dr = DRM_DVS_GetDeviceAttrByIndex( iOption1? g_hDeviceHandle : NULL, iAttrIndex,
                                           pbAttrName, &cbAttrName, pbAttrValue, &cbAttrValue );
    }
    ChkDR( dr );
    ChkArg( pbAttrValue != NULL
        && cbAttrValue ==  ( DRM_DWORD ) strlen( argv[2] )
        && pbAttrName != NULL
        && cbAttrName ==  ( DRM_DWORD ) strlen( argv[1] ) );

    ChkBOOL( MEMCMP( pbAttrValue, argv[2], cbAttrValue ) == 0, DRM_E_FAIL );
    ChkBOOL( MEMCMP( pbAttrName, argv[1], cbAttrName ) == 0, DRM_E_FAIL );

ErrorExit:
    SAFE_OEM_FREE( pbAttrName );
    SAFE_OEM_FREE( pbAttrValue );
    return dr;
}


/* count the attributes of the device and verify that it matches 

argv[0]: expected attribute count
argv[1]: optional: device handle is NULL 

*/

DRM_RESULT DRM_CALL TestDeviceStoreGetDeviceAttrCount(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr       = DRM_SUCCESS;
    DRM_DWORD  dwCount  = 0;
    DRM_DWORD  iOption1 = 1; //default to TRUE so we pass in the real handle

    ChkArg( argc >= 1 );

    if ( argc >= 2 && argv[1] != NULL )
        iOption1 = atol( argv[1] );

    dr = DRM_DVS_GetDeviceAttrCount( iOption1? g_hDeviceHandle : NULL, &dwCount );

    ChkBOOL( (DRM_LONG)dwCount == atol( argv[0] ), DRM_E_FAIL );

ErrorExit:
    return dr;
}

/* TestDeviceStoreChangeTime changes the system time on the test machine.

argv[0]: offset in seconds, positive to go forward, negative to rollback

*/

DRM_RESULT DRM_CALL TestDeviceStoreChangeTime(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
    long lOffset;
 
    ChkArg(argc == 1 && argv[0]);
    lOffset = atol(argv[0]);
    tChangeSystemTime(lOffset);
#ifdef WINCE_TEST
    Sleep(2000);
#endif
ErrorExit:
    return dr;
}

/* TestDeviceStoreInitialize: allows us to call DRM_DVS_Initialize more than
once or with NULLs for some tests.  DRM_DVS_Initialize is called once during
PreTestCase.

argv[0]: optional -- Device Store context is NULL.
argv[1]: optional -- Manage Context is NULL.
 
*/

DRM_RESULT DRM_CALL TestDeviceStoreInitialize(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
    DRM_DWORD iOption1 = 1; //default to TRUE so we pass in the real contexts
    DRM_DWORD iOption2 = 1;

    if( DRM_DVS_IsDeviceStoreSupported() )
    {
        if((argc >= 1) && (0 !=  ( DRM_DWORD ) strlen(argv[0])))
            iOption1 = atol(argv[0]);
        if (argc >= 2 && argv[1] != NULL)
            iOption2 = atol(argv[1]);

        ChkDR( DRM_DVS_Initialize( iOption1 ? g_pDeviceStoreContext : NULL, 
                                   iOption2 ? g_pManagerContext : NULL ));
    }

ErrorExit:
    return dr;
}


/* TestDeviceStoreUninitialize: allows us to call DRM_DVS_Uninitialize more than
once  or with NULLs for some tests.  DRM_DVS_Uninitialize will also be called once 
during PostTestCase.

argv[0]: optional -- Device Store context is NULL.

*/

DRM_RESULT DRM_CALL TestDeviceStoreUninitialize(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr=DRM_SUCCESS;
    DRM_DWORD iOption1 = 1; //default to TRUE so we pass in the real context

    if( DRM_DVS_IsDeviceStoreSupported() )
    {
        if((argc >= 1) && (0 !=  ( DRM_DWORD ) strlen(argv[0])))
            iOption1 = atol(argv[0]);

        DRM_DVS_Uninitialize(iOption1 ? g_pDeviceStoreContext : NULL);
    }
    
    return dr;
}

/*_EnumerateAllDevicesAndAttr calls DRM_DVS_GetDevice for every registered
device in the storem and for every device calls DRM_DVS_GetDeviceAttrCount and 
DRM_DVS_GetDeviceAttrByIndex for every attribute the device has.*/
DRM_RESULT _EnumerateAllDevicesAndAttr()
{
    DRM_RESULT dr          = DRM_SUCCESS;
    DRM_DWORD  dwNumAttr   = 0;
    DRM_BYTE  *pbAttrName  = NULL;
    DRM_DWORD  cbAttrName  = 0;
    DRM_BYTE  *pbAttrValue = NULL;
    DRM_DWORD  cbAttrValue = 0;
    DRM_DWORD  i           = 0;

    ChkDR(DRM_DVS_GetFirstDevice(g_pDeviceStoreContext, &g_hDeviceHandle));

    //as long as we are getting a valid device handle, iterate through each device and get all its attributes.
    while(DRM_SUCCEEDED(dr))
    {
        ChkDR(DRM_DVS_GetDeviceAttrCount( g_hDeviceHandle, &dwNumAttr ));
        for (i=0; i < dwNumAttr; i++)
        {
            dr=DRM_DVS_GetDeviceAttrByIndex(g_hDeviceHandle, i, pbAttrName, &cbAttrName, pbAttrValue, &cbAttrValue);
            if (dr==DRM_E_BUFFERTOOSMALL)
            {
                ChkMem( pbAttrName = (DRM_BYTE *)Oem_MemAlloc( cbAttrName ) );
                ChkMem( pbAttrValue = (DRM_BYTE *)Oem_MemAlloc( cbAttrValue ) );
                ChkDR(DRM_DVS_GetDeviceAttrByIndex(g_hDeviceHandle, i, pbAttrName, &cbAttrName, pbAttrValue, &cbAttrValue));
            }
        }

        DRM_DVS_FreeDevice(g_hDeviceHandle);
        dr=DRM_DVS_GetNextDevice(g_pDeviceStoreContext, &g_hDeviceHandle);
    }

    //if we enumerated all n devices the loop stops with DRM_E_NOMORE
    //when trying to get device n+1.  This is expected.
    if (dr == DRM_E_NOMORE)
        dr = DRM_SUCCESS;

ErrorExit:
    SAFE_OEM_FREE(pbAttrName);
    SAFE_OEM_FREE(pbAttrValue);
    return dr;
}


/* TestDeviceStoreRegisterManyDevices allows us to register devices in bulk 
without cluttering test scripts up with dozens of calls to TestDeviceStoreRegisterDevice.
Additionally, it will let us set an arbitrary number of attributes for each device.

argv[0]= Device Name.  A numeric ID will be appended to the name for each separate device.
argv[1]= Number of devices to register.
argv[2]= Optional: Attribute Name.  The device name will prefix and a numeric ID will be appended to the
attribute name for each attribute set to a device.
argv[3]= Optional: Number of attributes to set for each device.
argv[4]= Optional: Attribute value to set.
*/

DRM_RESULT DRM_CALL TestDeviceStoreRegisterManyDevices(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT   dr               = DRM_SUCCESS;
    DRM_DWORD    dwNumDevices     = 0;
    DRM_DWORD    dwNumAttr        = 0;
    DRM_BYTEBLOB DeviceID         = {0};
    DRM_BYTEBLOB AttrName         = {0};
    DRM_BYTEBLOB AttrValue        = {0};
    DRM_CHAR     szDeviceID[100]  = {0};
    DRM_CHAR     szAttrName[100]  = {0};
    DRM_CHAR     szAttrValue[100] = {0};
    DRM_DWORD    i                = 0;
    DRM_DWORD    j                = 0;
    DRM_DWORD    cchCopy          = 0;

    ChkArg ( argc >= 2 );

    if (argc >= 3)
        ChkArg(argv[2]);

    if (argc >= 4)
    { //if we supplied a number of attributes to be set, keep track of it
        ChkArg(argv[3]);
        dwNumAttr = atol(argv[3]);
    }

    if (argc >= 5)
        ChkArg(argv[4]);

    dwNumDevices=atol(argv[1]);

    for(i=0; i<dwNumDevices; i++)
    {
        //construct unique device ID from parameters given
        cchCopy = min( NO_OF( szDeviceID ) - 1,  ( DRM_DWORD ) strlen( argv[0] ) );
        MEMCPY( szDeviceID, argv[0], cchCopy );
#pragma prefast(push)
#pragma prefast(disable:22018) /* prefast warns about integer overflow which is handled by ChkOverflow */
        ChkOverflow( NO_OF(szDeviceID),  NO_OF(szDeviceID) - cchCopy);
        cchCopy += TST_UTL_NumberToAnsiString(i, 10, 0, NO_OF(szDeviceID) - cchCopy, &szDeviceID[cchCopy]);
#pragma prefast(pop)
        DeviceID.pbBlob = (DRM_BYTE *)szDeviceID;
        DeviceID.cbBlob = cchCopy;

        ChkDR(DRM_DVS_RegisterDevice( g_pDeviceStoreContext, DeviceID, &g_hDeviceHandle ));

        if (argc >=5) //do we have attribute name/value pairs to set for device?
        {
            for (j=0; j < dwNumAttr; j++)
            {
                //construct unique attribute name and value pair from API args.
                cchCopy = min( NO_OF( szAttrName ) - 1,  ( DRM_DWORD ) strlen( argv[2] ) );
                MEMCPY( szAttrName, argv[2], cchCopy );
#pragma prefast(push)
#pragma prefast(disable:22018) /* prefast warns about integer overflow which is handled by ChkOverflow */
                ChkOverflow( NO_OF(szAttrName),  NO_OF(szAttrName) - cchCopy);
                cchCopy += TST_UTL_NumberToAnsiString(j, 10, 0, NO_OF(szAttrName) - cchCopy, &szAttrName[cchCopy]);
#pragma prefast(pop)
                AttrName.pbBlob = (DRM_BYTE *)szAttrName;
                AttrName.cbBlob = cchCopy;

                cchCopy = min( NO_OF( szAttrValue ) - 1,  ( DRM_DWORD ) strlen( argv[4] ) );
                MEMCPY( szAttrValue, argv[4], cchCopy );
#pragma prefast(push)
#pragma prefast(disable:22018) /* prefast warns about integer overflow which is handled by ChkOverflow */
                ChkOverflow( NO_OF(szAttrValue),  NO_OF(szAttrValue) - cchCopy);
                cchCopy += TST_UTL_NumberToAnsiString(j, 10, 0, NO_OF(szAttrValue) - cchCopy, &szAttrValue[cchCopy]);
#pragma prefast(pop)
                AttrValue.pbBlob = (DRM_BYTE *)szAttrValue;
                AttrValue.cbBlob = cchCopy;

                dr = DRM_DVS_SetDeviceAttr( g_hDeviceHandle, AttrName, AttrValue );
                ChkDR(dr)
            }
        }

        ChkDR(DRM_DVS_CommitDevice( g_hDeviceHandle ));
        DRM_DVS_FreeDevice( g_hDeviceHandle );
    }
    ChkDR(_EnumerateAllDevicesAndAttr());

ErrorExit:
    return dr;
}

/* TestDeviceStoreUnRegisterManyDevices allows us to unregister devices in bulk
that were set with TestDeviceStoreRegisterManyDevices without cluttering test 
scripts up with dozens of calls to TestDeviceStoreUnRegisterDevice.

argv[0]= Device Name.  A numeric ID will be appended to the name for each separate device,
as done in TestDeviceStoreUnRegisterManyDevices.
argv[1]= Number of devices to unregister.
*/

DRM_RESULT DRM_CALL TestDeviceStoreUnRegisterManyDevices(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT   dr                  = DRM_SUCCESS;
    DRM_DWORD    dwNumDevicesToUnReg = 0;
    DRM_CHAR     szDeviceID[100]     = {0};
    DRM_BYTEBLOB DeviceID            = {0};
    DRM_DWORD    i                   = 0;
    DRM_DWORD    cchCopy             = 0;
    
    ChkArg(argc == 2);

    dwNumDevicesToUnReg=atol(argv[1]);  

    for(i=0; i<dwNumDevicesToUnReg; i++)
    {
        //construct device ID from args passed in
        cchCopy = min( NO_OF( szDeviceID ) - 1,  ( DRM_DWORD ) strlen( argv[0] ) );
        MEMCPY( szDeviceID, argv[0], cchCopy );
#pragma prefast(push)
#pragma prefast(disable:22018) /* prefast warns about integer overflow which is handled by ChkOverflow */
        ChkOverflow( NO_OF(szDeviceID),  NO_OF(szDeviceID) - cchCopy);
        cchCopy += TST_UTL_NumberToAnsiString(i, 10, 0, NO_OF(szDeviceID) - cchCopy, &szDeviceID[cchCopy]);
#pragma prefast(pop)

        ChkBOOL( cchCopy <= NO_OF( szDeviceID ), DRM_E_FAIL );

        DeviceID.pbBlob = (DRM_BYTE *)szDeviceID;
        DeviceID.cbBlob = cchCopy;

        ChkDR(DRM_DVS_UnRegisterDevice( g_pDeviceStoreContext, DeviceID ));

    }

ErrorExit:
    return dr;
}

/* TestDeviceStoreSetManyAttr allows us to set an arbitrary number of attributes 
for a device without cluttering test scripts up with dozens of calls to TestDeviceStoreSetAttr

argv[0]= Device Name.
argv[1]= Attribute Name.  The device name will prefix and a numeric ID will be appended to the
attribute name for each attribute set to a device.
argv[2]= Number of attributes to set to the device.
argv[3]= Attribute value to set.
*/

DRM_RESULT DRM_CALL TestDeviceStoreSetManyAttr(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT   dr               = DRM_SUCCESS;
    DRM_DWORD    dwNumAttr        = 0;
    DRM_BYTEBLOB DeviceID         = {0};
    DRM_BYTEBLOB AttrName         = {0};
    DRM_BYTEBLOB AttrValue        = {0};
    DRM_CHAR     szAttrName[100]  = {0};
    DRM_CHAR     szAttrValue[100] = {0};
    DRM_DWORD    i                = 0;
    DRM_DWORD    cchCopy          = 0;

    ChkArg(argc == 4);

    dwNumAttr=atol(argv[2]);

    DeviceID.pbBlob = (DRM_BYTE *)argv[0];
    DeviceID.cbBlob =  ( DRM_DWORD ) strlen(argv[0]);

    ChkDR(DRM_DVS_GetDeviceByID(g_pDeviceStoreContext, DeviceID, &g_hDeviceHandle));

    for (i=0; i < dwNumAttr; i++)
    {
        //construct unique attribute name/value pair from given parameters.
        cchCopy = min( NO_OF( szAttrName ) - 1,  ( DRM_DWORD ) strlen( argv[1] ) );
        MEMCPY( szAttrName, argv[1], cchCopy );
#pragma prefast(push)
#pragma prefast(disable:22018) /* prefast warns about integer overflow which is handled by ChkOverflow */
        ChkOverflow( NO_OF(szAttrName),  NO_OF(szAttrName) - cchCopy);
        cchCopy += TST_UTL_NumberToAnsiString(i, 10, 0, NO_OF(szAttrName) - cchCopy, &szAttrName[cchCopy]);
#pragma prefast(pop)
        AttrName.pbBlob = (DRM_BYTE *)szAttrName;
        AttrName.cbBlob = cchCopy;

        cchCopy = min( NO_OF( szAttrValue ) - 1,  ( DRM_DWORD ) strlen( argv[3] ) );
        MEMCPY( szAttrValue, argv[3], cchCopy );
#pragma prefast(push)
#pragma prefast(disable:22018) /* prefast warns about integer overflow which is handled by ChkOverflow */
        ChkOverflow( NO_OF(szAttrValue),  NO_OF(szAttrValue) - cchCopy);
        cchCopy += TST_UTL_NumberToAnsiString(i, 10, 0, NO_OF(szAttrValue) - cchCopy, &szAttrValue[cchCopy]);
#pragma prefast(pop)
        AttrValue.pbBlob = (DRM_BYTE *)szAttrValue;
        AttrValue.cbBlob = cchCopy;

        ChkDR(DRM_DVS_SetDeviceAttr( g_hDeviceHandle, AttrName, AttrValue ));
    }

    ChkDR(DRM_DVS_CommitDevice( g_hDeviceHandle ));
    DRM_DVS_FreeDevice( g_hDeviceHandle );

ErrorExit:
    return dr;
}

/* TestDeviceStoreGetManyAttr allows us to get an arbitrary number of attributes 
from a device set with TestDeviceStoreSetManyAttr without cluttering test scripts 
up with dozens of calls to TestDeviceStoreGetAttr.

argv[0]= Device Name.
argv[1]= Attribute Name.  The device name will prefix and a numeric ID will be appended to the
attribute name for each attribute to get from a device.
argv[2]= Attribute value to compare.
*/
DRM_RESULT DRM_CALL TestDeviceStoreGetManyAttr(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT   dr               = DRM_SUCCESS;
    DRM_DWORD    dwNumAttr        = 0;
    DRM_BYTEBLOB DeviceID         = {0};
    DRM_BYTEBLOB AttrName         = {0};
    DRM_BYTEBLOB AttrValue        = {0};
    DRM_CHAR     szAttrName[100]  = {0};
    DRM_CHAR     szAttrValue[100] = {0};
    DRM_DWORD    i                = 0;
    DRM_DWORD    cchCopy          = 0;

    ChkArg( argc    == 3);
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );
    ChkArg( argv[2] != NULL );

    DeviceID.pbBlob = (DRM_BYTE *)argv[0];
    DeviceID.cbBlob =  ( DRM_DWORD ) strlen(argv[0]);

    ChkDR(DRM_DVS_GetDeviceByID(g_pDeviceStoreContext, DeviceID, &g_hDeviceHandle));

    ChkDR(DRM_DVS_GetDeviceAttrCount( g_hDeviceHandle, &dwNumAttr ));

    for (i=0; i < dwNumAttr-1; i++)
    {
        //construct unique attribute value/data pair from given parameters.
        cchCopy = min( NO_OF( szAttrName ) - 1,  ( DRM_DWORD ) strlen( argv[1] ) );
        MEMCPY( szAttrName, argv[1], cchCopy );

#pragma prefast(push)
#pragma prefast(disable:22018) /* prefast warns about integer overflow which is handled by ChkOverflow */
        ChkOverflow( NO_OF( szAttrName ),  NO_OF( szAttrName ) - cchCopy );
        cchCopy += TST_UTL_NumberToAnsiString(i, 10, 0, NO_OF( szAttrName ) - cchCopy, &szAttrName[cchCopy] );
#pragma prefast(pop)

        AttrName.pbBlob = (DRM_BYTE *)szAttrName;
        AttrName.cbBlob = cchCopy;

        cchCopy = min( NO_OF( szAttrValue ) - 1,  ( DRM_DWORD ) strlen( argv[2] ) );
        MEMCPY( szAttrValue, argv[2], cchCopy );
        ChkOverflow( NO_OF( szAttrValue ),  NO_OF( szAttrValue ) - cchCopy );
        cchCopy += TST_UTL_NumberToAnsiString(i, 10, 0, NO_OF(szAttrValue) - cchCopy, &szAttrValue[cchCopy]);

        dr = DRM_DVS_GetDeviceAttr( g_hDeviceHandle, AttrName, AttrValue.pbBlob, &AttrValue.cbBlob );

        if ( dr == DRM_E_BUFFERTOOSMALL )
        {
            ChkMem( AttrValue.pbBlob = (DRM_BYTE *)Oem_MemAlloc( AttrValue.cbBlob ) );
            ChkDR(DRM_DVS_GetDeviceAttr(g_hDeviceHandle, AttrName, AttrValue.pbBlob, &AttrValue.cbBlob));
        }

        ChkBOOL( 0 == MEMCMP(AttrValue.pbBlob, szAttrValue, AttrValue.cbBlob ), DRM_E_FAIL);
    }

ErrorExit:
    return dr;
}



DRM_RESULT DRM_CALL MODULE_PRETESTCASE(DeviceStore)(long lTCID, const char *strTCName)
{
    DRM_RESULT dr;
    
    ChkDR(Oem_Clock_SetResetState(NULL, FALSE));

    GET_SHARED_APP_CONTEXT( g_pManagerContext );
    GET_SHARED_DEVICE_STORE( g_pwszDeviceStoreName );

    if( DRM_DVS_IsDeviceStoreSupported() )
    {
        ChkMem(g_pDeviceStoreContext = (DRM_DEVICESTORE_CONTEXT*)Oem_MemAlloc(sizeof(DRM_DEVICESTORE_CONTEXT)));
        ChkDR( DRM_DVS_Initialize( g_pDeviceStoreContext, g_pManagerContext ) );
    }
ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(DeviceStore)(long lTCID, const char *strTCName)
{
    DRM_RESULT     dr            = DRM_SUCCESS;
    _DeviceHandle *pDeviceHandle = NULL;

    tResetSystemTimeOffset(); /* Reverse any time changes in the test case.*/

    if( DRM_DVS_IsDeviceStoreSupported() )
    {
        /*
        ** Free the device without committing
        */
        if( g_hDeviceHandle != NULL )
        {
            pDeviceHandle = (_DeviceHandle *)g_hDeviceHandle; 
            pDeviceHandle->fDontCommitOnFree = TRUE;
        }
        DRM_DVS_FreeDevice( g_hDeviceHandle );

        if( g_pDeviceStoreContext != NULL )
        {
            DRM_DVS_Uninitialize(g_pDeviceStoreContext);
            SAFE_OEM_FREE( g_pDeviceStoreContext );
        }
    }

    if( g_szFilePath )
    {
        ChkDR( tRemoveFile( g_szFilePath ) );
    }


ErrorExit:
    return dr;
}

BEGIN_TEST_API_LIST( DeviceStore )
    API_ENTRY(TestDeviceStoreGetFirstDevice)
    API_ENTRY(TestDeviceStoreGetNextDevice)
    API_ENTRY(TestDeviceStoreGetDeviceByID)
    API_ENTRY(TestDeviceStoreGetDeviceID)
    API_ENTRY(TestDeviceStoreGetDeviceAttrCount)
    API_ENTRY(TestDeviceStoreGetDeviceAttrByIndex)
    API_ENTRY(TestDeviceStoreGetDeviceAttr)
    API_ENTRY(TestDeviceStoreSetDeviceAttr)
    API_ENTRY(TestDeviceStoreCommitDevice)
    API_ENTRY(TestDeviceStoreFreeDevice)
    API_ENTRY(TestDeviceStoreRegisterDevice)
    API_ENTRY(TestDeviceStoreUnRegisterDevice)
    API_ENTRY(TestDeviceStoreChangeTime)
    API_ENTRY(TestDeviceStoreInitialize)
    API_ENTRY(TestDeviceStoreUninitialize)
    API_ENTRY(TestDeviceStoreRegisterManyDevices)
    API_ENTRY(TestDeviceStoreUnRegisterManyDevices)
    API_ENTRY(TestDeviceStoreSetManyAttr)
    API_ENTRY(TestDeviceStoreGetManyAttr)
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE

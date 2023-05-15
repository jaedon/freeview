/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <FuzzFunctions.h>

using namespace std;

#define  ONE_TS_PACKET_SIZE             188

void FuzzTSPacket( vector<BYTE> &vbPacket )
{
    WORD            wIndexFuzzTarget = 0;
    unsigned int    i                = 0;
    WORD            wIndexFuzzData   = 0;
    vector<BYTE>    vbFuzzedData( 256 );


    Oem_Random_GetBytes( NULL, ( DRM_BYTE * )&wIndexFuzzTarget, SIZEOF( wIndexFuzzTarget ) );
    Oem_Random_GetBytes( NULL, ( DRM_BYTE * )&vbFuzzedData[0], ( DRM_DWORD ) vbFuzzedData.size() );

    wIndexFuzzTarget &= 0x3FFF;
    i                = wIndexFuzzTarget;

    while( wIndexFuzzData < vbFuzzedData.size() )
    {
        vbPacket[i++] = vbFuzzedData[wIndexFuzzData++];
    }
    
    vbPacket[i++] = 0x00;
    vbPacket[i++] = 0x00;
    vbPacket[i++] = 0x00;
    vbPacket[i++] = 0x01;
    
}

void DuplicateTSPacket( vector<BYTE> &vbPacket )
{
    DWORD  dwSize = ( DWORD ) vbPacket.size();

    if( dwSize < ONE_TS_PACKET_SIZE )
    {
        return;
    }

    vbPacket.resize( dwSize + ONE_TS_PACKET_SIZE );

    DWORD dwCopyFrom = dwSize - ONE_TS_PACKET_SIZE -1;
    DWORD dwCopyTo   = dwSize;
    for( int i = 0; i < ONE_TS_PACKET_SIZE; i++ )
    {
        vbPacket[ dwCopyTo++ ] = vbPacket[ dwCopyFrom++ ];
    }
    
}

void DropTSPacket( vector<BYTE> &vbPacket, DWORD dwDropCount  )
{
    if( dwDropCount == 0 )
    {
        return;
    }
    
    if( vbPacket.size() > dwDropCount * ONE_TS_PACKET_SIZE )
    {
        vbPacket.resize( vbPacket.size() - ( dwDropCount * ONE_TS_PACKET_SIZE ) );
    }
}

/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#pragma once

extern void FuzzTSPacket( std::vector<BYTE> &vbPacket );
extern void DuplicateTSPacket( std::vector<BYTE> &vbPacket );

extern void DropTSPacket( std::vector<BYTE> &vbPacket, DWORD dwDropCount  );
    

/******************************************************************************
 
    Copyright
    This code is strictly confidential and the receiver is obliged to use it 
    exclusively for his or her own purposes. No part of Viaccess code may be 
    reproduced or transmitted in any form or by any means, electronic or 
    mechanical, including photocopying, recording, or by any information storage 
    and retrieval system, without permission in writing from Viaccess. 
    The information in this code is subject to change without notice. Viaccess 
    does not warrant that this code is error free. If you find any problems 
    with this code or wish to make comments, please report them to Viaccess.
 
    Trademarks 
    Viaccess is a registered trademark of Viaccess ® in France and/or other 
    countries. All other product and company names mentioned herein are the 
    trademarks of their respective owners.
    Viaccess may hold patents, patent applications, trademarks, copyrights 
    or other intellectual property rights over the code hereafter. Unless 
    expressly specified otherwise in a Viaccess written license agreement, the
    delivery of this code does not imply the concession of any license over 
    these patents, trademarks, copyrights or other intellectual property.
 
******************************************************************************/

/*
    $Revision: 10729 $
    $Date: 2009-04-08 17:26:54 +0200 (mer., 08 avr. 2009) $
*/

#ifndef _VA_XNVM_H_
#define _VA_XNVM_H_

#include "va_types.h"
#include "va_errors.h"

typedef enum
{
    eSEGMENT_SOFTWARE = 0,
    eSEGMENT_ACS_DATA,
    eSEGMENT_LOADER_DATA,

    eSEGMENT_LAST /* not used */
    
} tVA_XNVM_Segment;


/*
    Open a XNVM segment.
    
    Possible error codes are:
    kVA_ILLEGAL_HANDLE if segment already open
    
*/
DWORD   VA_XNVM_Open(DWORD dwVaXnvmHandle, tVA_XNVM_Segment eSegment);

/*
    Close a XNVM segment.
    
    Possible error codes are:
    kVA_INVALID_PARAMETER if invalid dwStbXnvmHandle
    kVA_ERROR             other error
    
*/
INT     VA_XNVM_Close(DWORD dwStbXnvmHandle);

/*
    Read in a XNVM segment.
    
    Possible error codes are:
    kVA_INVALID_PARAMETER if invalid dwStbXnvmHandle
                          if uiOffset and uiSize is upper the maximum size of the segment
                          if pReadData == NULL
    kVA_RESOURCE_BUSY     a read/write is already in progress
    kVA_OK                no error
    
*/
INT     VA_XNVM_Read (DWORD dwStbXnvmHandle, UINT32 uiOffset, UINT32 uiSize, BYTE* pReadData);

/*
    Write in a XNVM segment.
    
    Possible error codes are:
    kVA_INVALID_PARAMETER if invalid dwStbXnvmHandle
                          if uiOffset and uiSize is over the maximum size of the segment
                          if pWriteData == NULL
    kVA_RESOURCE_BUSY     a read/write is already in progress
    kVA_OK                no error
    
*/
INT     VA_XNVM_Write(DWORD dwStbXnvmHandle, UINT32 uiOffset, UINT32 uiSize, BYTE* pWriteData);


/*
    Asynchronous returns for VA_XNVM_Read, VA_XNVM_Write .
    
    Possible error codes are:
    kVA_INVALID_PARAMETER   if invalid dwVaXnvmHandle
    kVA_UNEXPECTED_CALLBACK if no valid request is supposed to be in progress
    kVA_OK                  no error
    
*/
INT     VA_XNVM_RequestDone(DWORD dwVaXnvmHandle);
INT     VA_XNVM_RequestFailed(DWORD dwVaXnvmHandle);

#endif /*_VA_XNVM_H*/


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
    Viaccess is a registered trademark of Viaccess Æ in France and/or other 
    countries. All other product and company names mentioned herein are the 
    trademarks of their respective owners.
    Viaccess may hold patents, patent applications, trademarks, copyrights 
    or other intellectual property rights over the code hereafter. Unless 
    expressly specified otherwise in a Viaccess written license agreement, the
    delivery of this code does not imply the concession of any license over 
    these patents, trademarks, copyrights or other intellectual property.
 
******************************************************************************/

/*
    $Revision: 10117 $
    $Date: 2009-02-25 17:56:19 +0100 (mer., 25 f√©vr. 2009) $
*/

#ifndef _VA_PVR_H_
#define _VA_PVR_H_

#include "va_types.h"
#include "va_os.h"

/* Constants */
#define kVA_PVR_NO_EXPIRATION_DATE  0
#define kVA_PVR_NO_PLAYBACK_COUNTER 0
#define kVA_PVR_METADATA_MAX_SIZE   64
#define kVA_PVR_ECM_MAX_SIZE        1024

typedef enum
{
    ePERSISTENT ,
    eTIMESHIFT

} tVA_PVR_RecordType ;




DWORD VA_PVR_OpenEcmChannel ( DWORD              dwAcsId,
                              tVA_PVR_RecordType eRecordType,
                              WORD               wEcmChannelPid );

INT VA_PVR_CloseEcmChannel ( DWORD   dwStbEcmChannelHandle );

INT VA_PVR_RecordEcm (  DWORD        dwStbEcmChannelHandle,
                        UINT32       uiEcmLength,
                        BYTE *       pEcm,
                        void *       pUserData );

INT VA_PVR_WriteMetadata (  DWORD    dwAcsId,
                            UINT32   uiMetadataLength,
                            BYTE *   pMetadata );

INT VA_PVR_ReadMetadata (   DWORD    dwAcsId,
                            UINT32   uiMetadataLength,
                            BYTE *   pMetadata );


#endif /* _VA_PVR_H_ */




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
    $Revision: 5833 $
    $Date: 2007-05-10 17:56:04 +0200 (jeu., 10 mai 2007) $
*/


#ifndef _VA_DMX_H_
#define _VA_DMX_H_

#include "va_types.h"
#include "va_errors.h"

#define kVA_DMX_MAX_FILTER_SIZE    10

DWORD VA_DMX_AllocateSectionFilter(
    DWORD dwAcsId, DWORD dwVaFilterHandle,
    WORD  wPid, UINT16 uiNbBuffers) ;

INT VA_DMX_FreeSectionFilter( DWORD dwStbFilterHandle );

INT VA_DMX_SetSectionFilterParameters(
    DWORD dwStbFilterHandle, 
    UINT32 uiLength, BYTE *pValue, BYTE *pMask);

INT VA_DMX_StartSectionFilter(DWORD  dwStbFilterHandle);

INT VA_DMX_StopSectionFilter(DWORD  dwStbFilterHandle);

INT VA_DMX_NotifyFilteredSection(DWORD   dwVaFilterHandle, 
    UINT32 uiBufferLength, BYTE *pBuffer, void *pUserData);

INT VA_DMX_UnlockSectionFilter(DWORD dwStbFilterHandle);


#endif /* _VA_DMX_H_ */


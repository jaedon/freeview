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
    $Revision: 7413 $
    $Date: 2007-09-21 09:38:56 +0200 (ven., 21 sept. 2007) $
*/


#ifndef _VA_NET_H_
#define _VA_NET_H_

#include "va_types.h"
#include "va_errors.h"

DWORD VA_NET_UnicastOpen( DWORD dwVaNetHandle, 
                         const char *pIPAddress, UINT16 uiPort, UINT32 uiTimeout);
INT VA_NET_UnicastReceive( DWORD dwStbNetHandle, UINT32 uiLength ,UINT32 uiTimeout );
INT VA_NET_UnicastReceiveRelease( DWORD dwStbNetHandle ) ;
INT VA_NET_UnicastSend ( DWORD dwStbNetHandle, 
                        UINT32 uiDataLength ,BYTE* pData, UINT32 uiTimeout ) ;
INT VA_NET_UnicastOpenDone( DWORD dwVaNetHandle ) ;
INT VA_NET_UnicastClose( DWORD dwStbNetHandle ) ;

DWORD VA_NET_MulticastOpen( DWORD dwVaNetHandle, 
                           const char *pIPAddress, UINT16 uiPort, UINT32 uiBufferSize ) ;
INT VA_NET_MulticastNotifyRelease( DWORD dwStbNetHandle ) ;
INT VA_NET_MulticastClose( DWORD dwStbNetHandle ) ;

INT VA_NET_UnicastReceiveDone( DWORD dwVaNetHandle , UINT32 uiDataLength, BYTE * pData) ;
INT VA_NET_UnicastReceiveFailed( DWORD dwVaNetHandle ) ;
INT VA_NET_UnicastSendDone( DWORD dwVaNetHandle ) ;
INT VA_NET_UnicastSendFailed( DWORD dwVaNetHandle ,UINT32 uiBytesSent ) ;
INT VA_NET_UnicastOpenDone( DWORD dwVaNetHandle ) ;
INT VA_NET_UnicastOpenFailed( DWORD dwVaNetHandle ) ;

INT VA_NET_MulticastOpenDone( DWORD dwVaNetHandle ) ;
INT VA_NET_MulticastOpenFailed( DWORD dwVaNetHandle ) ;
INT VA_NET_MulticastNotify ( DWORD dwVaNetHandle ,UINT32 uiDataLength, BYTE * pData) ;

#endif /* _VA_NET_H_ */


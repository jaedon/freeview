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



#ifndef _VA_SC_H_
#define _VA_SC_H_

#include "va_types.h"
#include "va_errors.h"

typedef enum
{
    eCARD_EXTRACTED = 0,
    eCARD_INSERTED,
    eCARD_READY

} tVA_SC_State;


INT VA_SC_Reset( DWORD dwScSlot, UINT8 uiPPSNegotiationValue );

INT VA_SC_ResetDone(DWORD dwScSlot, 
                           UINT8 uiAtrSize, BYTE *pATR,
                           UINT8 uiPPSNegotiatedvalue);

INT VA_SC_ResetFailed(DWORD dwScSlot);

INT VA_SC_GetState(DWORD dwScSlot, tVA_SC_State *pScState);

INT VA_SC_CardInserted(DWORD dwScSlot);

INT VA_SC_CardExtracted(DWORD dwScSlot);

INT VA_SC_SendCommand(DWORD dwScSlot,
    UINT32 uiCommandSize, BYTE *pCommand);

INT VA_SC_CommandDone(DWORD dwScSlot, 
    UINT32 uiReturnSize, BYTE *pReturn);

INT VA_SC_CommandFailed(DWORD dwScSlot);

INT VA_SC_Activate(DWORD dwScSlot);

INT VA_SC_Deactivate(DWORD dwScSlot);

INT VA_SC_Lock(DWORD dwScSlot);

INT VA_SC_Unlock(DWORD dwScSlot);

#endif /* _VA_SC_H_ */


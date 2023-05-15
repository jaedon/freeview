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


#ifndef VA_NVM_H
#define VA_NVM_H

#include "va_types.h"
#include "va_errors.h"


INT VA_NVM_Read(UINT32 uiOffset, UINT32 uiSize, BYTE *pReadData);
INT VA_NVM_Write(UINT32 uiOffset, UINT32 uiSize, BYTE *pWriteData);

INT VA_NVM_RequestDone(void);
INT VA_NVM_RequestFailed(void);


#endif /* VA_NVM_H */


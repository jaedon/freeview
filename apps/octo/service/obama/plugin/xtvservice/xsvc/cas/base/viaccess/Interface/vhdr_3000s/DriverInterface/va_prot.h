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
    $Revision: 7625 $
    $Date: 2007-10-03 16:16:57 +0200 (mer., 03 oct. 2007) $
*/

#ifndef _VA_PROT_H_
#define _VA_PROT_H_

#include "va_types.h"
#include "va_errors.h"


/* Bitmap for Copy Protections */
#define  kVA_PROT_BIT_CP_MACROVISION    0x01
#define  kVA_PROT_BIT_CP_GSMA_LSB       0x02
#define  kVA_PROT_BIT_CP_GSMA_MSB       0x04

/* Bitmap for Digital Ouptuts */
#define  kVA_PROT_BIT_SPDIF             0x0001
#define  kVA_PROT_BIT_ETHERNET          0x0002
#define  kVA_PROT_BIT_USB               0x0004
#define  kVA_PROT_BIT_ILINK             0x0008
#define  kVA_PROT_BIT_DVBCI             0x0010
#define  kVA_PROT_BIT_CLEAR_RECORDING   0x0020
#define  kVA_PROT_BIT_DVD               0x0040
#define  kVA_PROT_BIT_TIME_SHIFTING     0x0080

/* Figerprinting text string size max */
#define kVA_PROT_MAX_FINGERPRINTING_STRING_SIZE     16


INT VA_PROT_StartMacrovision(void);
INT VA_PROT_StopMacrovision (void);

INT VA_PROT_SetCopyProtection(BYTE byCopyProtection);

INT VA_PROT_StartFingerprinting(char *pFingerprintStr);
INT VA_PROT_StopFingerprinting(void);

INT VA_PROT_SetDigitalOutputProtection(WORD wOutputProtection);

INT VA_PROT_SuspendAnalogChannels(void);
INT VA_PROT_ResumeAnalogChannels(void);

INT VA_PROT_SuspendFreeChannels(void);
INT VA_PROT_ResumeFreeChannels(void);


#endif /* _VA_PROT_H_ */


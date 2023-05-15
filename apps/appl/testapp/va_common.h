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


#ifndef _VA_COMMON_H_
#define _VA_COMMON_H_

#include "va_types.h"
#include "va_errors.h"
#if 0
// == va_crypt ==
extern INT VA_CRYPT_GetCapabilities ( tVA_CRYPT_Capabilities * pCapabilities ) ;
extern INT VA_CRYPT_ImportKey ( const BYTE * pWrappedKey , UINT32 uiWrappedKeySize , DWORD dwWrappingKeyId , DWORD * pImportedKeyId ) ;
extern INT VA_CRYPT_ExportKey ( BYTE * pWrappedKey , UINT32 * pWrappedKeySize , DWORD dwWrappingKeyId , DWORD dwExportKeyId ) ;
extern INT VA_CRYPT_ReleaseKey ( DWORD dwKeyId ) ;
extern INT VA_CRYPT_CipherOpen ( const tVA_CRYPT_Config * pConfig , DWORD * pHandle ) ;
extern INT VA_CRYPT_CipherProcess ( DWORD dwHandle , const BYTE * pSrc , BYTE * pDst , UINT32 uiSize ) ;
extern INT VA_CRYPT_CipherClose ( DWORD dwHandle ) ;
extern UINT32 VA_CRYPT_GetRandom ( void ) ;

// == va_crypt ==
extern INT VA_CTRL_GetProgramInformation( DWORD dwAcsId, tVA_CTRL_ProgramInformation *pProgramInfo );
extern INT   VA_CTRL_Init(const tVA_CTRL_ConfigurationParameters *pParameters);
extern void  VA_CTRL_Start(void)
#endif	
#endif /*_VA_COMMON_H_*/

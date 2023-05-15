
#ifndef __NOCS3_CSD_HARNESS_TEST_CA_PVR_H__
#define __NOCS3_CSD_HARNESS_TEST_CA_PVR_H__

#include "ca_cert.h"
#include "types.h" 

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct CSD_Harness_Test_CA_PVR_Configuration
{
	bool								bIsDsc;
	bool								bEncryption;

	unsigned int						emi;
	unsigned int						audioPID;
	unsigned int						videoPID;
	unsigned int						pcrPID;
	
	/* Set to use ODD or EVEN Parity. */
	bool 								use_odd_parity;

	bool								bEcmEnabled;
	unsigned int						ecmPID;

	TCsdScrKeyPathHandle				*pScrKeyHandle;
	TCsdDscKeyPathHandle				*pDscVideoKeyHandle;
    	TCsdDscKeyPathHandle				*pDscAudioKeyHandle;
    	TCertKeyPathHandle				       *pDscKeyHandle;
        
	FILE								* pInFile;
	FILE								* pOutFile;
	DATA_IO *pIO; /* CFV */
	NOCS3_Harness_Test_Test_Vector		* pVector;

	/* IV passed in from command line */
	unsigned int						sizeIV;
	unsigned char						iv[CSD_MAX_IV_SIZE];

}	CSD_Harness_Test_CA_PVR_Configuration;

NOCS3_HARNESS_TEST_STATUS_CODE csd_harness_test_ca_pvr (CSD_Harness_Test_CA_PVR_Configuration * pConfig);

#ifdef __cplusplus
}
#endif

#endif /* __NOCS3_CSD_HARNESS_TEST_CA_PVR_H__ */

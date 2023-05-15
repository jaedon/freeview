#ifndef _DATA_FEEDER_H_
#define _DATA_FEEDER_H_

#define	MAX_NUM_OUTPUT_FILE			2

typedef struct BRAAGA_TEST_PicParams
{
	uint32_t			ui32IpPicHeight;
	uint32_t			ui32IpPicWidth;
}BRAAGA_TEST_PicParams;

typedef struct BRAAGA_TEST_FeederStruct
{
	bool								Running;
	void								*Buffer;
	
	BDSP_AF_P_sDRAM_CIRCULAR_BUFFER		sPDQ;
	BDSP_AF_P_sDRAM_CIRCULAR_BUFFER		sPRQ;

	BTST_TaskHandle						hFeederTask;	
	
	BCHP_Handle							hChp;
	BMEM_Handle							hMem;
	BINT_Handle							hInt;
	BREG_Handle							hReg;

	BKNI_EventHandle					FeederStartEvent;	
	BKNI_EventHandle					PictureDoneEvent;	
	/* BKNI_EventHandle					DescriptorDoneEvent; */
	size_t								BufferSize;
	FILE								*InFilePtr;
	BTMR_Handle							hTimerDevice;
	BTMR_TimerHandle					hTimer;
    BTMR_DefaultSettings				TmrDefSettings;

	BINT_CallbackHandle					PictureDoneIntCb;
	bool								Repeat;
	/* This will be needed for isr call backs */
	BDSP_TaskHandle						hDspTask;
	/* For Dumping CDB and ITB */
	FILE								*OutFilePtr[MAX_NUM_OUTPUT_FILE];

	/* Cdb and Itb buffers where encoder will write the output */
	BDSP_AF_P_sDRAM_CIRCULAR_BUFFER		sCdbBuffer;
	BDSP_AF_P_sDRAM_CIRCULAR_BUFFER		sItbBuffer;
	BRAAGA_TEST_PicParams				sPicParams;

}BRAAGA_TEST_FeederStruct;

typedef struct BRAAGA_TEST_CircBfrInfo
{
	bool								bWrapped;
	unsigned int						uSpaceLeftInBytes;
	unsigned int						uDataAvailableInBytes;
	unsigned int						uDepthTillEnd;
} BRAAGA_TEST_CircBfrInfo;


typedef struct BRAAGA_TEST_FeederStruct *BRAAGA_TEST_Feeder_Handle;

BRAAGA_TEST_Feeder_Handle BRAAGA_TEST_Feeder_Init(
	BCHP_Handle					hChp,
	BMEM_Handle					hMem,
	BINT_Handle                 hInt,
	BREG_Handle                 hReg
	);

BERR_Code	BRAAGA_TEST_FeederStart( 
								BRAAGA_TEST_Feeder_Handle			hFeeder, 
								char								*FileName
								);

void BRAAGA_TEST_Feeder_Shutdown(
	BRAAGA_TEST_Feeder_Handle hFeeder
	);


#endif /* _DATA_FEEDER_H_ */

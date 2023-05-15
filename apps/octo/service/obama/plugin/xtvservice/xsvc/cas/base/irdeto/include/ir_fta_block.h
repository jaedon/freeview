#ifndef __IR_FTA_BLOCK_H__
#define __IR_FTA_BLOCK_H__

typedef enum
{
	eFTABlockStateOK,
	eFTABlockStateFail,						// Black screen
	eFTABlockStateFail_NoCard,				// E04-4
	eFTABlockStateFail_Need_Activation,		// error 255
	eFTABlockStateFail_NotMatch,			// error 250
	eFTABlockStateFail_AlMajdService,		// error 251
	eFTABlockStateFail_ASIService,			// error 252
	eFTABlockStateMax
}eIrdetoFTABlockState;

typedef enum
{
	FTA_BLOCK_UPDATE_SERVICE_STATUS,
	FTA_BLOCK_UPDATE_IRCARD_STATUS_IN,
	FTA_BLOCK_UPDATE_IRCARD_STATUS_OK,
	FTA_BLOCK_UPDATE_IRCARD_STATUS_OUT,
	FTA_BLOCK_UPDATE_SMARTCARD_NUMBER,
	FTA_BLOCK_UPDATE_PRODUCT_STATUS,
	FTA_BLOCK_UPDATE_REGION_CODE,
	FTA_BLOCK_UPDATE_IR_STATUS_OK
}	FTA_BLOCK_SC_UPDATE_TYPE;

extern HUINT32 CAS_IR_FTA_BLOCK_update_Status (FTA_BLOCK_SC_UPDATE_TYPE eUpdateType);
extern HUINT32 CAS_IR_FTA_BLOCK_is_service_blocked (Handle_t hAction);

#endif	// __IR_FTA_BLOCK_H_

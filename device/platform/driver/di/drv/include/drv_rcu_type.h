#ifndef __DRV_RCU_TYPE_H__
#define __DRV_RCU_TYPE_H__

#define IR_MAX_KEY_TABLE_INDEX			0x70
#define IR_LENGTH_OF_KEY_NAME			20
#define IR_NUMBER_OF_KEY				0x100

#define RF4CE_NUMBER_OF_KEY				0x100

typedef struct 
{
	HUINT32   ulKeyCode;					 		/* ������ Key Code */
	HUINT8   aucKeyName[IR_LENGTH_OF_KEY_NAME];  	/* Key Name */
	HUINT32 ulKeySencdCounter;                                        /* Key ���� Ƚ�� */
} RCU_INFO;

#endif /* __DRV_RCU_TYPE_H__ */


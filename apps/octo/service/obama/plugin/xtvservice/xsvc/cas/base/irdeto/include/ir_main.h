#ifndef __IR_MAIN_H__
#define	__IR_MAIN_H__


 
/********************************************************************
*   type definition
********************************************************************/
int		CAS_IR_DRV_Init(void);
HIR_ERR	CAS_IR_SoftCellInit(void);
void		CAS_IR_CATASK_Init(void);
void		CAS_IR_SendMessage(HUINT16 msg_channel, HUINT16 msg_type, void* pMsg);



#endif

/*	end of file	*/



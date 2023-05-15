#ifndef __IR_SI_H__
#define	__IR_SI_H__


/********************************************************************
*   type definition
********************************************************************/




HUINT8	*CAS_IR_SI_ParseCAT(HUINT8	*pCatRaw, HINT32 nDmxId, HUINT16 *Length);
HUINT8	*CAS_IR_SI_ParsePMT(HUINT8 index, HUINT8* pPmtRaw, Handle_t hAction, HUINT16 *Length);
HUINT8	*CAS_IR_SI_ParseNullPMT(HUINT8 index, Handle_t hAction, HUINT16 *Length);
HIR_ERR	CAS_IR_SI_ParseSDT(Handle_t hAction, HUINT8* pSdtRaw, HUINT16 Length, HUINT8 *pucRunningStatus);

void	CAS_IR_SI_UpdateCAT(CAS_CAI_IF_msgCAT *pMsgSI);
void	CAS_IR_SI_UpdatePMT(CAS_CAI_IF_msgPMT *pMsgSI);
void 	CAS_IR_SI_UpdateSDT(CAS_CAI_IF_msgSDT * pMsgSI);
void	CAS_IR_SI_UpdateNIT(CAS_CAI_IF_msgNIT *pMsgSI);
void	CAS_IR_SI_UpdateBAT(CAS_CAI_IF_msgBAT *pMsgSI);

#endif

/*	end of file	*/






#include "nexus_platform.h"
#include "nexus_security_types.h"
#include "nexus_security.h"

#include "ca_defs.h"
#include "emitools_internal.h"
#include "emitools.h"

#include "bdbg.h"
BDBG_MODULE(EMITOOLS);

bool brcmEmiIsValid(TUnsignedInt16 xEmi)
{
    return emiTable[brcmEmiIndex(xEmi)].bValid;
}

size_t brcmEmiGetKeySize(TUnsignedInt16 xEmi)
{
    return emiTable[brcmEmiIndex(xEmi)].xKeySize;
}

size_t brcmEmiGetIvSize(TUnsignedInt16 xEmi)
{
    return emiTable[brcmEmiIndex(xEmi)].xIvSize;
}

bool brcmEmiIsZeroIv(TUnsignedInt16 xEmi)
{
    return emiTable[brcmEmiIndex(xEmi)].bZeroIv;
}

bool brcmEmiIsMpegTs(TUnsignedInt16 xEmi)
{
    return emiTable[brcmEmiIndex(xEmi)].bTsNotRaw;
}

size_t brcmEmiGetBlockSize(TUnsignedInt16 xEmi)
{
    return emiTable[brcmEmiIndex(xEmi)].xBlockSize;
}

NEXUS_SecurityAlgorithm brcmEmiGetAlgorithm(TUnsignedInt16 xEmi)
{
    return emiTable[brcmEmiIndex(xEmi)].xAlgo;
}

NEXUS_SecurityAlgorithmVariant brcmEmiGetVariant(TUnsignedInt16 xEmi)
{
    return emiTable[brcmEmiIndex(xEmi)].xAlgoVar;
}

NEXUS_SecurityTerminationMode brcmEmiGetTerminationMode(TUnsignedInt16 xEmi)
{
    return emiTable[brcmEmiIndex(xEmi)].xTerm;
}

NEXUS_SecuritySolitarySelect brcmEmiGetSolitarySelect(TUnsignedInt16 xEmi)
{
    return emiTable[brcmEmiIndex(xEmi)].xSolSel;
}


bool brcmEmiSetupAlgorithm(TUnsignedInt16 xEmi, NEXUS_SecurityAlgorithmSettings *algorithm)
{
    int index = brcmEmiIndex(xEmi);

    if (!brcmEmiIsValid(xEmi)) {
        BDBG_ERR(("%s: EMI %04x is not valid\n", __FUNCTION__, xEmi));
        return false;
    }

    NEXUS_Security_GetDefaultAlgorithmSettings(algorithm);

    algorithm->algorithm       = emiTable[index].xAlgo;
    algorithm->algorithmVar    = emiTable[index].xAlgoVar;
    algorithm->terminationMode = emiTable[index].xTerm;
    algorithm->solitarySelect  = emiTable[index].xSolSel;

    {
	/* TSC bit clear for TS */ /* HUMAX_PLATFORM_BASE */
	if(!(xEmi & 0xF000))
	{
            algorithm->scValue[NEXUS_SecurityPacketType_eGlobal] = NEXUS_SecurityAlgorithmScPolarity_eClear;
            algorithm->scValue[NEXUS_SecurityPacketType_eRestricted] = NEXUS_SecurityAlgorithmScPolarity_eClear;		
            algorithm->modifyScValue[NEXUS_SecurityPacketType_eGlobal] = true;
            algorithm->modifyScValue[NEXUS_SecurityPacketType_eRestricted] = true;
        }
    }
    if (emiTable[index].bTsNotRaw)
        algorithm->dvbScramLevel = NEXUS_SecurityDvbScrambleLevel_eTs;

    if ((emiTable[index].xAlgo    == NEXUS_SecurityAlgorithm_eAes) &&
        (emiTable[index].xAlgoVar == NEXUS_SecurityAlgorithmVariant_eCounter))
        algorithm->aesCounterSize = NEXUS_SecurityAesCounterSize_e128Bits;

    algorithm->ivMode = NEXUS_SecurityIVMode_eRegular;

    /* TODO: Remove this EMI specific part. Is it even needed? */
    if ((xEmi == 0x0021) || (xEmi == 0x4021) || (xEmi == 0x0041) || (xEmi == 0x4041))
        algorithm->enableExtIv = false;

    return true;
}

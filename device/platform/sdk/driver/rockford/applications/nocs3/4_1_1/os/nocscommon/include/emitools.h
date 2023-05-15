
#ifndef __EMITOOLS_H__
#define __EMITOOLS_H__

bool                           brcmEmiIsValid(TUnsignedInt16 xEmi);
size_t                         brcmEmiGetKeySize(TUnsignedInt16 xEmi);
size_t                         brcmEmiGetIvSize(TUnsignedInt16 xEmi);
bool                           brcmEmiIsZeroIv(TUnsignedInt16 xEmi);
bool                           brcmEmiIsMpegTs(TUnsignedInt16 xEmi);
size_t                         brcmEmiGetBlockSize(TUnsignedInt16 xEmi);
NEXUS_SecurityAlgorithm        brcmEmiGetAlgorithm(TUnsignedInt16 xEmi);
NEXUS_SecurityAlgorithmVariant brcmEmiGetVariant(TUnsignedInt16 xEmi);
NEXUS_SecurityTerminationMode  brcmEmiGetTerminationMode(TUnsignedInt16 xEmi);
NEXUS_SecuritySolitarySelect   brcmEmiGetSolitarySelect(TUnsignedInt16 xEmi);

bool brcmEmiSetupAlgorithm(TUnsignedInt16 xEmi, NEXUS_SecurityAlgorithmSettings *algorithm);

#endif /* __EMITOOLS_H__ */

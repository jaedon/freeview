
#ifndef __EMITOOLS_INTERNAL_H__
#define __EMITOOLS_INTERNAL_H__

#define NO_C99

/* Table entry per EMI */
typedef struct SEmiDescriptor
{
    TUnsignedInt16                 xEmi;         /* EMI value */
    bool                           bValid;       /* EMI is valid */
    size_t                         xKeySize;     /* Key size */
    size_t                         xIvSize;      /* IV size (0 for no IV) */
    bool                           bZeroIv;      /* true = use IV of all zeros */
    bool                           bTsNotRaw;    /* true = MPEG TS, false = RAW */
    size_t                         xBlockSize;   /* Block size (0 for MPEG TS EMIs) */
    NEXUS_SecurityAlgorithm        xAlgo;        /* Algorithm */
    NEXUS_SecurityAlgorithmVariant xAlgoVar;     /* Algorithm variant */
    NEXUS_SecurityTerminationMode  xTerm;        /* Termination mode */
    NEXUS_SecuritySolitarySelect   xSolSel;      /* Solitary Select */
} TEmiDescriptor;

/* The ENUM of table indices is built automatically from emitable.h */
#define EMIDESC(EMI, KEYSIZE, IVSIZE, ZEROIV, TS, BLKSZ, ALGO, VAR, TERM, SOLSEL) eEmiIdx ## EMI,

enum
{
#include "emitable.h"
  eEmiIdxMax
};

/* The table of EMI descriptors is built automatically from emitable.h */
#undef EMIDESC

/* You must keep this in sync with the structure above, since C90 doesn't */
/* allow us to specify subobjects to intialise */
#define EMIDESC(EMI, KEYSIZE, IVSIZE, ZEROIV, TS, BLKSZ, ALGO, VAR, TERM, SOLSEL) \
    { \
        EMI,                                     /* xEmi */ \
        (EMI != 0xcff0) ? true : false,          /* bValid */ \
        KEYSIZE,                                 /* xKeySize */ \
        IVSIZE,                                  /* xIvSize */ \
        ZEROIV,                                  /* bZeroIv */ \
        TS,                                      /* bTsNotRaw */ \
        BLKSZ,                                   /* xBlockSize */ \
        NEXUS_SecurityAlgorithm_ ## ALGO,        /* xAlgo */ \
        NEXUS_SecurityAlgorithmVariant_ ## VAR,  /* xAlgoVar */ \
        NEXUS_SecurityTerminationMode_ ## TERM,  /* xTerm */ \
        NEXUS_SecuritySolitarySelect_ ## SOLSEL, /* xSolSel */ \
    },

static TEmiDescriptor emiTable[] = {
#include "emitable.h"
};


/* This function uses a switch statement built automatically from emitable.h */
/* to efficiently convert an EMI to a table offset */
#undef EMIDESC
#define EMIDESC(EMI, KEYSIZE, IVSIZE, ZEROIV, TS, BLKSZ, ALGO, VAR, TERM, SOLSEL) case EMI: index = eEmiIdx ## EMI; break;



#ifdef NO_C99
static int brcmEmiIndex(TUnsignedInt16 xEmi)
#else
static inline int brcmEmiIndex(TUnsignedInt16 xEmi)
#endif
{
    int index = eEmiIdx0xcff0;

    switch (xEmi)
    {
#include "emitable.h"
    }

    return index;
}

#endif /* __EMITOOLS_INTERNAL_H__ */

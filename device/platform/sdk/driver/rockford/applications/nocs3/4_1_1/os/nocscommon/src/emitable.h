
/* Table of EMI descriptions */

/*      EMI     Keysz IVsz  IV_0   TS     blksz  Algo     /Var       Term          SolSel */

/* DVB CSA2 */
EMIDESC(0x0000,     8,   0, false, true,      0, eDvb,     eXpt,     eClear,       eClear)

/* DVB CSA3 */
EMIDESC(0x0001,    16,   0, false, true,      0, eDvbCsa3, eXpt,     eClear,       eClear)

/* Nagra ASA */
EMIDESC(0x0010,    16,   0, false, true,      0, eAsa,     eXpt,     eClear,       eClear)

/* Nagra ASA */
EMIDESC(0x0011,    16,   0, false, true,      0, eAsa,     eXpt,     eClear,       eClear)

/* AES-128 TS CBC ZeroIV SCTE52 */
#if NEXUS_PLATFORM_97241==NEXUS_PLATFORM_VERSION(12,4)
EMIDESC(0x0020,    16,  16, true,  true,      0, eAes,     eCbc,     eCbcResidual, eCbcXorIV)
#else
EMIDESC(0x0020,    16,  16, true,  true,      0, eAes,     eCbc,     eCbcResidual, eCbcXorIv)
#endif

/* AES-128 TS ECB */
EMIDESC(0x0021,    16,   0, false, true,      0, eAes,     eEcb,     eClear,       eClear)

/* AES-128 TS CBC CLEAR */
EMIDESC(0x0022,    16,  16, false, true,      0, eAes,     eCbc,     eClear,       eClear)

/* TDES TS CBC ZeroIV SCTE52 */
#if NEXUS_PLATFORM_97241==NEXUS_PLATFORM_VERSION(12,4)
EMIDESC(0x0040,    16,   8, true,  true,      0, e3DesAba, eCbc,     eCbcResidual, eCbcXorIV)
#else
EMIDESC(0x0040,    16,   8, true,  true,      0, e3DesAba, eCbc,     eCbcResidual, eCbcXorIv)
#endif

/* TDES TS ECB */
EMIDESC(0x0041,    16,   0, false, true,      0, e3DesAba, eEcb,     eClear,       eClear)

/* AES-128 RAW CBC ZeroIV */
EMIDESC(0x4020,    16,  16, true,  false,    16, eAes,     eCbc,     eClear,       eClear)

/* AES-128 RAW ECB */
EMIDESC(0x4021,    16,   0, false, false,    16, eAes,     eEcb,     eClear,       eClear)

/* AES-128 RAW CBC */
EMIDESC(0x4022,    16,  16, false, false,    16, eAes,     eCbc,     eCbcResidual, eClear)

/* AES-128 RAW CBC */
EMIDESC(0x4023,    16,  16, false, false,    16, eAes,     eCbc,     eCbcResidual, eClear)

/* AES-128 RAW CTR */
EMIDESC(0x4024,    16,  16, false, false,    16, eAes,     eCounter, eCbcResidual, eClear)

/* AES-128 RAW CBC */
EMIDESC(0x4026,    16,  16, false, false,    16, eAes,     eCbc,     eClear,       eClear)

/* AES-128 RAW CTR */
EMIDESC(0x4027,    16,  16, false, false,    16, eAes,     eCounter, eClear,       eClear)

/* TDES RAW CBC ZeroIV */
EMIDESC(0x4040,    16,   8, true,  false,     8, e3DesAba, eCbc,     eClear,       eClear)

/* TDES RAW ECB */
EMIDESC(0x4041,    16,   0, false, false,     8, e3DesAba, eEcb,     eClear,       eClear)

/* TDES RAW CBC */
EMIDESC(0x4043,    16,   8, false, false,     8, e3DesAba, eCbc,     eClear,       eClear)

/* Invalid */
EMIDESC(0xcff0,     0,   0, false, false,     0, eMax,     eMax,     eMax,         eMax)

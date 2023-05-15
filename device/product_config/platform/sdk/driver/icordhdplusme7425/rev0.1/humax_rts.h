#ifndef __HUMAX_RTS_H__
#define __HUMAX_RTS_H__

typedef struct {
    uint32_t uId;
    uint32_t uValue;
} RTS_CLIENT;

/*
  Model specific RTS will be updated with table below if defined during nexus platform init.
  Completes table first and updates gpRTS_Optimized valued from NULL to this table's address.  
  CAUTION!!: Last Entry's uId value shall be 0xFFFFFFFF
RTS_CLIENT gaMODEL_Optimized_RTS[] = {              
		{0,    0x0031f00a},                               
		~~~~~~~~~~~~~~~~~~                              
		{127,  0x0069200c},                               
		{0xFFFFFFFF, 0xFFFFFFFF}                          
}; 
*/                                                  
#if 0
const RTS_CLIENT *gpRTS_Optimized = NULL; /*gaMODEL_Optimized_RTS;*/
const RTS_CLIENT *gpRTS_MEMC1_Optimized = NULL; /*gaMODEL_Optimized_RTS;*/
#else                                                      
const RTS_CLIENT gpRTS_Optimized[]= /*gpRTS_Optimized;*/
{
	{0,0x0031e009  },/* XPT_WR_RS 2890ns */
	{1,0x8031002f  },/* XPT_WR_XC RR 2840ns */
	{2,0x8037c00b  },/* XPT_WR_CDB RR 3230ns */
	{3,0x80ad2038  },/* XPT_WR_ITB_MSG RR 10010ns */
	{4,0x803bf00f  },/* XPT_RD_RS RR 3470ns */
	{5,0x81b9201d  },/* XPT_RD_XC_RMX_MSG RR 25490ns */
	{6,0x8037c00c  },/* XPT_RD_XC_RAVE RR 3230ns */
	{7,0x80834037  },/* XPT_RD_PB RR 7590ns */
	{8,0x8016403f  },/* AVD_DBLK_0 RR 0ns */
	{9,0x8018f029  },/* AVD_ILCPU RR 1451ns */
	{10,0x80707035 }, /* AVD_OLCPU RR 6893ns */
	{11,0x804c1032 }, /* AVD_CAB RR 4666ns */
	{12,0x0024f006 }, /* AVD_ILSI 2144ns */
	{13,0x80164040 }, /* SVD_DBLK_0 RR 0ns */
	{14,0x8018f02a }, /* SVD_ILCPU RR 1451ns */
	{15,0x806e3034 }, /* SVD_OLCPU RR 6756ns */
	{16,0x804c1033 }, /* SVD_CAB RR 4666ns */
	{17,0x0024f007 }, /* SVD_ILSI 2144ns */
	{18,0x801a8002 }, /* SVD_BLCPU RR 1538ns */
	{19,0x80579014 }, /* SVD_BLSI RR 5065ns */
	{20,0x80646015 }, /* SVD_MVSCALE RR 5806ns */
	{21,0x800ab03c }, /* SVD_SPIXSTORE RR 0ns */
	{22,0x83618023 }, /* BSP RR 50000ns */
	{23,0x800b103d }, /* AVD_DBLK_1 RR 0ns */
	{24,0x800b103e }, /* SVD_DBLK_1 RR 0ns */
	{25,0x07fff050 }, /* UNASSIGNED off */
	{26,0x80828036 }, /* SATA RR 8000ns */
	{27,0x87fff04b }, /* TPCAP RR */
	{28,0x80186028 }, /* CPU_SCB RR 1500ns */
	{29,0x84429025 }, /* HIF_EDU RR 63000ns */
	{30,0x07fff051 }, /* UNASSIGNED off */
	{31,0x811db03a }, /* HIF_PCIe RR 17500ns */
	{32,0x07fff052 }, /* UNASSIGNED off */
	{33,0x02807021 }, /* SUN_MCIF_RD 37000ns */
	{34,0x02807020 }, /* SUN_MCIF_WR 37000ns */
	{35,0x87fff049 }, /* SDIO_EMMC RR */
	{36,0x87fff048 }, /* SDIO_CARD RR */
	{37,0x802d3008 }, /* GENET_WR RR 2620ns */
	{38,0x80b2b039 }, /* GENET_RD RR 10950ns */
	{39,0x07fff053 }, /* UNASSIGNED off */
	{40,0x8046c013 }, /* MOCA_DATA_WR RR 4096ns */
	{41,0x8181a03b }, /* MOCA_DATA_RD RR 23620ns */
	{42,0x83957024 }, /* MOCA_MIPS RR 53000ns */
	{43,0x802f102b }, /* USB_HI RR 2890ns */
	{44,0x802f102d }, /* USB_LO RR 2890ns */
	{45,0x802f102c }, /* USB_HI_1 RR 2890ns */
	{46,0x802f102e }, /* USB_LO_1 RR 2890ns */
	{47,0x07fff054 }, /* UNASSIGNED off */
	{48,0x07fff055 }, /* UNASSIGNED off */
	{49,0x8033d030 }, /* AUD_ZSP_0 RR 3000ns */
	{50,0x009e4019 }, /* AUD_AIO 9150ns */
	{51,0x80113001 }, /* AUD_ZSP_1 RR 1000ns */
	{52,0x07fff07f }, /* MADR2_WR off */
	{53,0x8033d00a }, /* BVNF_RDC RR 3000ns */
	{54,0x07fff07f }, /* MADR2_RD off */
	{55,0x07fff07f }, /* MADR2_QM off */
	{56,0x003cd011 }, /* BVNF_MFD2_0 3520ns */
	{57,0x003cd012 }, /* BVNF_MFD2_1 3520ns */
	{58,0x003cd010 }, /* BVNF_MFD1_0 3520ns */
	{59,0x001e5003 }, /* BVNF_MFD1_1 1760ns */
	{60,0x07fff07f }, /* BVNF_MFD0_0 off */
	{61,0x07fff07f }, /* BVNF_MFD0_1 off */
	{62,0x07fff07f }, /* BVNF_CAP4 off */
	{63,0x07fff07f }, /* BVNF_CAP3 off */
	{64,0x00a0001a }, /* BVNF_CAP2 9250ns */
	{65,0x07fff07f }, /* BVNF_CAP1 off */
	{66,0x001fb004 }, /* BVNF_CAP0 1840ns */
	{67,0x07fff07f }, /* BVNF_VFD4 off */
	{68,0x07fff07f }, /* BVNF_VFD3 off */
	{69,0x016e801c }, /* BVNF_VFD2 21180ns */
	{70,0x07fff07f }, /* BVNF_VFD1 off */
	{71,0x001fe005 }, /* BVNF_VFD0 1850ns */
	{72,0x003af00d }, /* MCVP0 3415ns */
	{73,0x003af00e }, /* MCVP1 3415ns */
	{74,0x07fff07f }, /* MADR_WR off */
	{75,0x07fff07f }, /* MADR_RD off */
	{76,0x07fff07f }, /* MADR_QM off */
	{77,0x07fff07f }, /* BVNF_GFD3 off */
	{78,0x07fff07f }, /* BVNF_GFD2 off */
	{79,0x00999018 }, /* BVNF_GFD1 8880ns */
	{80,0x000fc000 }, /* BVNF_GFD0 920ns */
	{81,0x07fff056 }, /* UNASSIGNED off */
	{82,0x0221301f }, /* VEC_VBI_ENC1 31500ns */
	{83,0x0221301e }, /* VEC_VBI_ENC0 31500ns */
	{84,0x00803016 }, /* MCVP_QM 7415ns */
	{85,0x07fff057 }, /* UNASSIGNED off */
	{86,0x07fff058 }, /* UNASSIGNED off */
	{87,0x80379031 }, /* M2M_DMA0 RR 3410ns */
	{88,0x87fff04d }, /* V3D_0 RR */
	{89,0x87fff04c }, /* V3D_1 RR */
	{90,0x87fff046 }, /* GFX RR */
	{91,0x07fff059 }, /* UNASSIGNED off */
	{92,0x07fff05a }, /* UNASSIGNED off */
	{93,0x07fff05b }, /* UNASSIGNED off */
	{94,0x87fff04a }, /* SID RR */
	{95,0x07fff07f }, /* VICE_VIP0 off */
	{96,0x07fff07f }, /* VICE_VIP1 off */
	{97,0x07fff07f }, /* VICE_VIP2 off */
	{98,0x07fff07f }, /* VICE_CME_RMB_CMB off */
	{99,0x07fff07f }, /* VICE_CME_CSC off */
	{100,0x07fff07f},  /* VICE_FME_CSC off */
	{101,0x07fff07f},  /* VICE_FME_Luma_CMB off */
	{102,0x07fff07f},  /* VICE_FME_Chroma_CMB off */
	{103,0x80feb01b},  /* VICE_SG RR 14720ns */
	{104,0x07fff07f},  /* VICE_DBLK off */
	{105,0x849da026},  /* VICE_CABAC0 RR 68260ns */
	{106,0x82e26022},  /* VICE_CABAC1 RR 42660ns */
	{107,0x07fff07f},  /* VICE_ARCSS0 off */
	{108,0x07fff07f},  /* VICE1_VIP0 off */
	{109,0x07fff07f},  /* VICE1_VIP1 off */
	{110,0x07fff07f},  /* VICE1_VIP2 off */
	{111,0x07fff05c},  /* UNASSIGNED off */
	{112,0x07fff05d},  /* UNASSIGNED off */
	{113,0x07fff05e},  /* UNASSIGNED off */
	{114,0x07fff05f},  /* UNASSIGNED off */
	{115,0x07fff060},  /* UNASSIGNED off */
	{116,0x07fff07f},  /* VICE_OPT off */
	{117,0x87fff04f},  /* MEMC_DIS1 RR */
	{118,0x87fff047},  /* MSA RR */
	{119,0x87fff04e},  /* MEMC_DIS RR */
	{120,0x80000041},  /* AVD_PFRI RR 0ns */
	{121,0x80000042},  /* SVD_PFRI RR 0ns */
	{122,0x80000043},  /* SVD_SPIXSCALE RR 0ns */
	{123,0x07fff07f},  /* VICE_REQ off */
	{124,0x800c2027},  /* LMB_HIGH RR 750ns */
	{125,0x80000044},  /* LMB_LOW RR 0ns */
	{126,0x87fff045},  /* DRAM_INIT RR */
	{127,0x00872017},  /* REFRESH 7812.5ns */
	{0xFFFFFFFF, 0xFFFFFFFF}  
};
const RTS_CLIENT gpRTS_Optimized_MEMC1[]= /*gpRTS_Optimized_MEMC1;*/
{
	{0,  0x07fff07f  },/* XPT_WR_RS off */
	{1,  0x07fff07f  },/* XPT_WR_XC off */
	{2,  0x07fff07f  },/* XPT_WR_CDB off */
	{3,  0x07fff07f  },/* XPT_WR_ITB_MSG off */
	{4,  0x07fff07f  },/* XPT_RD_RS off */
	{5,  0x07fff07f  },/* XPT_RD_XC_RMX_MSG off */
	{6,  0x07fff07f  },/* XPT_RD_XC_RAVE off */
	{7,  0x07fff07f  },/* XPT_RD_PB off */
	{8,  0x8016402a  },/* AVD_DBLK_0 RR 0ns */
	{9,  0x07fff07f  },/* AVD_ILCPU off */
	{10,  0x07fff07f }, /* AVD_OLCPU off */
	{11,  0x07fff07f }, /* AVD_CAB off */
	{12,  0x07fff07f }, /* AVD_ILSI off */
	{13,  0x8016402b }, /* SVD_DBLK_0 RR 0ns */
	{14,  0x07fff07f }, /* SVD_ILCPU off */
	{15,  0x07fff07f }, /* SVD_OLCPU off */
	{16,  0x07fff07f }, /* SVD_CAB off */
	{17,  0x07fff07f }, /* SVD_ILSI off */
	{18,  0x07fff07f }, /* SVD_BLCPU off */
	{19,  0x07fff07f }, /* SVD_BLSI off */
	{20,  0x07fff07f }, /* SVD_MVSCALE off */
	{21,  0x800ab027 }, /* SVD_SPIXSTORE RR 0ns */
	{22,  0x83618018 }, /* BSP RR 50000ns */
	{23,  0x800b1028 }, /* AVD_DBLK_1 RR 0ns */
	{24,  0x800b1029 }, /* SVD_DBLK_1 RR 0ns */
	{25,  0x07fff043 }, /* UNASSIGNED off */
	{26,  0x80828021 }, /* SATA RR 8000ns */
	{27,  0x07fff07f }, /* TPCAP off */
	{28,  0x80186025 }, /* CPU_SCB_1 RR 1500ns */
	{29,  0x8442901b }, /* HIF_EDU RR 63000ns */
	{30,  0x07fff044 }, /* UNASSIGNED off */
	{31,  0x811db023 }, /* HIF_PCIe RR 17500ns */
	{32,  0x07fff045 }, /* UNASSIGNED off */
	{33,  0x07fff07f }, /* SUN_MCIF_RD off */
	{34,  0x07fff07f }, /* SUN_MCIF_WR off */
	{35,  0x87fff03d }, /* SDIO_EMMC RR */
	{36,  0x87fff03c }, /* SDIO_CARD RR */
	{37,  0x802d3008 }, /* GENET_WR RR 2620ns */
	{38,  0x80b2b022 }, /* GENET_RD RR 10950ns */
	{39,  0x07fff046 }, /* UNASSIGNED off */
	{40,  0x8046c010 }, /* MOCA_DATA_WR RR 4096ns */
	{41,  0x8181a024 }, /* MOCA_DATA_RD RR 23620ns */
	{42,  0x83957019 }, /* MOCA_MIPS RR 53000ns */
	{43,  0x802f101c }, /* USB_HI RR 2890ns */
	{44,  0x802f101e }, /* USB_LO RR 2890ns */
	{45,  0x802f101d }, /* USB_HI_1 RR 2890ns */
	{46,  0x802f101f }, /* USB_LO_1 RR 2890ns */
	{47,  0x07fff047 }, /* UNASSIGNED off */
	{48,  0x07fff048 }, /* UNASSIGNED off */
	{49,  0x07fff07f }, /* AUD_ZSP_0 off */
	{50,  0x07fff07f }, /* AUD_AIO off */
	{51,  0x07fff07f }, /* AUD_ZSP_1 off */
	{52,  0x87fff037 }, /* MADR2_WR RR */
	{53,  0x07fff07f }, /* BVNF_RDC off */
	{54,  0x87fff036 }, /* MADR2_RD RR */
	{55,  0x87fff035 }, /* MADR2_QM RR */
	{56,  0x07fff07f }, /* BVNF_MFD2_0 off */
	{57,  0x07fff07f }, /* BVNF_MFD2_1 off */
	{58,  0x07fff07f }, /* BVNF_MFD1_0 off */
	{59,  0x07fff07f }, /* BVNF_MFD1_1 off */
	{60,  0x003cd00a }, /* BVNF_MFD0_0 3520ns */
	{61,  0x001e5001 }, /* BVNF_MFD0_1 1760ns */
	{62,  0x001fe005 }, /* BVNF_CAP4 1850ns */
	{63,  0x00a00015 }, /* BVNF_CAP3 9250ns */
	{64,  0x07fff07f }, /* BVNF_CAP2 off */
	{65,  0x001fb004 }, /* BVNF_CAP1 1840ns */
	{66,  0x07fff07f }, /* BVNF_CAP0 off */
	{67,  0x001e5002 }, /* BVNF_VFD4 1760ns */
	{68,  0x016e8017 }, /* BVNF_VFD3 21180ns */
	{69,  0x07fff07f }, /* BVNF_VFD2 off */
	{70,  0x001fe006 }, /* BVNF_VFD1 1850ns */
	{71,  0x07fff07f }, /* BVNF_VFD0 off */
	{72,  0x07fff07f }, /* MCVP0 off */
	{73,  0x07fff07f }, /* MCVP1 off */
	{74,  0x00395009 }, /* MADR_WR 3320ns */
	{75,  0x001c9000 }, /* MADR_RD 1660ns */
	{76,  0x00288007 }, /* MADR_QM 2350ns */
	{77,  0x007f4012 }, /* BVNF_GFD3 7360ns */
	{78,  0x00983014 }, /* BVNF_GFD2 8800ns */
	{79,  0x07fff07f }, /* BVNF_GFD1 off */
	{80,  0x07fff07f }, /* BVNF_GFD0 off */
	{81,  0x07fff049 }, /* UNASSIGNED off */
	{82,  0x07fff07f }, /* VEC_VBI_ENC1 off */
	{83,  0x07fff07f }, /* VEC_VBI_ENC0 off */
	{84,  0x07fff07f }, /* MCVP_QM off */
	{85,  0x07fff04a }, /* UNASSIGNED off */
	{86,  0x07fff04b }, /* UNASSIGNED off */
	{87,  0x80379020 }, /* M2M_DMA0 RR 3410ns */
	{88,  0x87fff040 }, /* V3D_0 RR */
	{89,  0x87fff03f }, /* V3D_1 RR */
	{90,  0x87fff03a }, /* GFX RR */
	{91,  0x07fff04c }, /* UNASSIGNED off */
	{92,  0x07fff04d }, /* UNASSIGNED off */
	{93,  0x07fff04e }, /* UNASSIGNED off */
	{94,  0x87fff03e }, /* SID RR */
	{95,  0x803fc00c }, /* VICE_VIP0 RR 3690ns */
	{96,  0x803fc00d }, /* VICE_VIP1 RR 3690ns */
	{97,  0x803fc00e }, /* VICE_VIP2 RR 3690ns */
	{98,  0x8041a00f }, /* VICE_CME_RMB_CMB RR 3800ns */
	{99,  0x840ea01a }, /* VICE_CME_CSC RR 60000ns */
	{100,  0x813d8016},  /* VICE_FME_CSC RR 18350ns */
	{101,  0x801f8003},  /* VICE_FME_Luma_CMB RR 1830ns */
	{102,  0x803f600b},  /* VICE_FME_Chroma_CMB RR 3670ns */
	{103,  0x07fff07f},  /* VICE_SG off */
	{104,  0x8000002c},  /* VICE_DBLK RR -1ns */
	{105,  0x07fff07f},  /* VICE_CABAC0 off */
	{106,  0x07fff07f},  /* VICE_CABAC1 off */
	{107,  0x804aa011},  /* VICE_ARCSS0 RR 4320ns */
	{108,  0x87fff032},  /* VICE1_VIP0 RR */
	{109,  0x87fff033},  /* VICE1_VIP1 RR */
	{110,  0x87fff034},  /* VICE1_VIP2 RR */
	{111,  0x07fff04f},  /* UNASSIGNED off */
	{112,  0x07fff050},  /* UNASSIGNED off */
	{113,  0x07fff051},  /* UNASSIGNED off */
	{114,  0x07fff052},  /* UNASSIGNED off */
	{115,  0x07fff053},  /* UNASSIGNED off */
	{116,  0x87fff039},  /* VICE_OPT RR */
	{117,  0x87fff042},  /* MEMC_DIS1 RR */
	{118,  0x87fff03b},  /* MSA RR */
	{119,  0x87fff041},  /* MEMC_DIS RR */
	{120,  0x8000002e},  /* AVD_PFRI RR 0ns */
	{121,  0x8000002f},  /* SVD_PFRI RR 0ns */
	{122,  0x80000030},  /* SVD_SPIXSCALE RR 0ns */
	{123,  0x8000002d},  /* VICE_REQ RR -1ns */
	{124,  0x800c2026},  /* LMB_HIGH_1 RR 750ns */
	{125,  0x80000031},  /* LMB_LOW RR 0ns */
	{126,  0x87fff038},  /* DRAM_INIT RR */
	{127,  0x00872013},  /* REFRESH 7812.5ns */
	{0xFFFFFFFF, 0xFFFFFFFF}  
};
#endif
#endif


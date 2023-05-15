#ifndef _V3D_REGS_H_

#define V3D_CTL_BASE    0x000
#define V3D_CLE_BASE    0x100
#define V3D_PTB_BASE    0x300
#define V3D_QPS_BASE    0x400
#define V3D_VPM_BASE    0x500
#define V3D_PCTR_BASE   0x600
#define V3D_DBG_BASE    0xe00

#//-- Global Control ------------------------------------------------------------
#define IDENT0          (V3D_CTL_BASE + 0x000)
#define IDENT1          (V3D_CTL_BASE + 0x004)
#define IDENT2          (V3D_CTL_BASE + 0x008)
#define IDENT3          (V3D_CTL_BASE + 0x00c)
#define SCRATCH         (V3D_CTL_BASE + 0x010)
#define L2CACTL         (V3D_CTL_BASE + 0x020)
#define SLCACTL         (V3D_CTL_BASE + 0x024)
#define INTCTL          (V3D_CTL_BASE + 0x030)
#define INTENA          (V3D_CTL_BASE + 0x034)
#define INTDIS          (V3D_CTL_BASE + 0x038)
#define ENDSWP          (V3D_CTL_BASE + 0x040)

#//-- Control List Executor ----------------------------------------------------
#define CT0CS           (V3D_CLE_BASE + 0x000)
#define CT1CS           (V3D_CLE_BASE + 0x004)
#define CT0EA           (V3D_CLE_BASE + 0x008)
#define CT1EA           (V3D_CLE_BASE + 0x00c)
#define CT0CA           (V3D_CLE_BASE + 0x010)
#define CT1CA           (V3D_CLE_BASE + 0x014)
#define CT00RA0         (V3D_CLE_BASE + 0x018)
#define CT01RA0         (V3D_CLE_BASE + 0x01c)
#define CT0LC           (V3D_CLE_BASE + 0x020)
#define CT1LC           (V3D_CLE_BASE + 0x024)
#define CT0PC           (V3D_CLE_BASE + 0x028)
#define CT1PC           (V3D_CLE_BASE + 0x02c)

#define PCS             (V3D_CLE_BASE + 0x030)
#define BFC             (V3D_CLE_BASE + 0x034)
#define RFC             (V3D_CLE_BASE + 0x038)


#define BPCA            (V3D_PTB_BASE + 0x000)
#define BPCS            (V3D_PTB_BASE + 0x004)
#define BPOA            (V3D_PTB_BASE + 0x008)
#define BPOS            (V3D_PTB_BASE + 0x00c)
#define BXCF            (V3D_PTB_BASE + 0x010)

#//-- QPU Scheduler ------------------------------------------------------------
#define SQRSV0          (V3D_QPS_BASE + 0x010)
#define SQRSV1          (V3D_QPS_BASE + 0x014)
#define SQCNTL          (V3D_QPS_BASE + 0x018)
#define SQCSTAT         (V3D_QPS_BASE + 0x01c)
#define SRQPC           (V3D_QPS_BASE + 0x030)
#define SRQUA           (V3D_QPS_BASE + 0x034)
#define SRQUL           (V3D_QPS_BASE + 0x038)
#define SRQCS           (V3D_QPS_BASE + 0x03c)

#//-- Vertex Pipe Memory Allocation --------------------------------------------
#define VPACNTL            (V3D_VPM_BASE + 0x000)
#define VPMBASE            (V3D_VPM_BASE + 0x004)

#//-- V3d support registers ----------------------------------------------------
#define PCTRC              (V3D_PCTR_BASE + 0x070)
#define PCTRE              (V3D_PCTR_BASE + 0x074)
#define PCTR0              (V3D_PCTR_BASE + 0x080)
#define PCTRS0             (V3D_PCTR_BASE + 0x084)
#define PCTR1              (V3D_PCTR_BASE + 0x088)
#define PCTRS1             (V3D_PCTR_BASE + 0x08c)
#define PCTR2              (V3D_PCTR_BASE + 0x090)
#define PCTRS2             (V3D_PCTR_BASE + 0x094)
#define PCTR3              (V3D_PCTR_BASE + 0x098)
#define PCTRS3             (V3D_PCTR_BASE + 0x09c)
#define PCTR4              (V3D_PCTR_BASE + 0x0a0)
#define PCTRS4             (V3D_PCTR_BASE + 0x0a4)
#define PCTR5              (V3D_PCTR_BASE + 0x0a8)
#define PCTRS5             (V3D_PCTR_BASE + 0x0ac)
#define PCTR6              (V3D_PCTR_BASE + 0x0b0)
#define PCTRS6             (V3D_PCTR_BASE + 0x0b4)
#define PCTR7              (V3D_PCTR_BASE + 0x0b8)
#define PCTRS7             (V3D_PCTR_BASE + 0x0bc)
#define PCTR8              (V3D_PCTR_BASE + 0x0c0)
#define PCTRS8             (V3D_PCTR_BASE + 0x0c4)
#define PCTR9              (V3D_PCTR_BASE + 0x0c8)
#define PCTRS9             (V3D_PCTR_BASE + 0x0cc)
#define PCTR10             (V3D_PCTR_BASE + 0x0d0)
#define PCTRS10            (V3D_PCTR_BASE + 0x0d4)
#define PCTR11             (V3D_PCTR_BASE + 0x0d8)
#define PCTRS11            (V3D_PCTR_BASE + 0x0dc)
#define PCTR12             (V3D_PCTR_BASE + 0x0e0)
#define PCTRS12            (V3D_PCTR_BASE + 0x0e4)
#define PCTR13             (V3D_PCTR_BASE + 0x0e8)
#define PCTRS13            (V3D_PCTR_BASE + 0x0ec)
#define PCTR14             (V3D_PCTR_BASE + 0x0f0)
#define PCTRS14            (V3D_PCTR_BASE + 0x0f4)
#define PCTR15             (V3D_PCTR_BASE + 0x0f8)
#define PCTRS15            (V3D_PCTR_BASE + 0x0fc)

#//-- Debugger module ----------------------------------------------------------
#define DBCFG           (V3D_DBG_BASE + 0x000)
#define DBSCS           (V3D_DBG_BASE + 0x004)
#define DBSCFG          (V3D_DBG_BASE + 0x008)
#define DBSSR           (V3D_DBG_BASE + 0x00c)
#define DBSDR0          (V3D_DBG_BASE + 0x010)
#define DBSDR1          (V3D_DBG_BASE + 0x014)
#define DBSDR2          (V3D_DBG_BASE + 0x018)
#define DBSDR3          (V3D_DBG_BASE + 0x01c)
#define DBQRUN          (V3D_DBG_BASE + 0x020)
#define DBQHLT          (V3D_DBG_BASE + 0x024)
#define DBQSTP          (V3D_DBG_BASE + 0x028)
#define DBQITE          (V3D_DBG_BASE + 0x02c)
#define DBQITC          (V3D_DBG_BASE + 0x030)
#define DBQGHC          (V3D_DBG_BASE + 0x034)
#define DBQGHG          (V3D_DBG_BASE + 0x038)
#define DBQGHH          (V3D_DBG_BASE + 0x03c)

#define DBGE            (V3D_DBG_BASE + 0x100)
#define FDBG0           (V3D_DBG_BASE + 0x104)
#define FDBGB           (V3D_DBG_BASE + 0x108)
#define FDBGR           (V3D_DBG_BASE + 0x10C)
#define FDBGS           (V3D_DBG_BASE + 0x110)
#define ERRSTAT         (V3D_DBG_BASE + 0x120)

#endif // _V3D_REGS_H_


/************************************************************************
 *
 *	TC90517.h
 *
 *	Copyright (c) 2007 by Humax	Corp.
 *
************************************************************************/
#ifndef TC90517_H
#define TC90517_H

#include "htype.h"

#define MXL683_I2C_ADDRESS1		((0xC0)>>1)
#define MXL683_I2C_ADDRESS2		((0xC0)>>1)
#define MXL683_I2C_ADDRESS3		((0xC0)>>1)


#define GINJO_FIRST_PAGE_BOUNDARY         0x0100 
#define GINJO_DFE_REGISTER_BOUNDARY       0x1000
#define GINJO_DFE_REG_WIDTH               8      // Ginjo DFE register address width is 8 bit   
#define GINJO_DFE_REG_ADDR_MASK           0x00FF // Ginjo DFE register address width is 8 bit 
#define GINJO_DFE_REG_PAGE_MASK           0x0300 // Bit8 and bit9 of register address is page number from bit8 position

#define DFE_PAGE_CTRL_REG                 0x0000

#define DEVICE_BASE_I2C_ADDRESS           96
#define MXL68X_MAX_DEVICES                3 

#define MXL681_DEVICE_ID                  0x03 
#define MXL683_DEVICE_ID                  0x02 

#define MAX_READ_TRY                      10

// Mailbox registers
#define MAILBOX_REG_BASE_ADDR             0xA800
#define MAILBOX_REG_NUM_PENDING_HOST_CMDS 0xA800
#define MAILBOX_REG_FIRMWARE_RC_VER       0xA802

// Following macro is used to populate Host command header that goes to MxL68x along with payload */
#define FORM_CMD_HEADER(Header, id, seqNum, len)  Header.syncChar = HOST_COMMAND_SYNC_IND + (seqNum % 0x0F); \
                                                  Header.commandId = id;  \
                                                  Header.payloadCheckSum = 0;\
                                                  Header.payloadLen = (HUINT8)len

#define RF_BW_6MHz                        6 
#define RF_BW_7MHz                        7 
#define RF_BW_8MHz                        8 

#define MxL_BER_STAT_TIME_INTERVAL        100   // 100ms time interval 

#define ISDBT_DATA_CARRIES_Seg            96    // Data carries bit number for 1 Seg 
#define ISDBT_FRAME_BIT_COUNT             204   // constent data, bit count in one frame 
#define ISDBT_TSP_FRAME                   8     // div factor to calculate TSP   

#define ISDBT_AC_DATA_LENGTH              24    // Total is 192 bits 
#define FW_CHAN_TUNE_DATA_LENGTH          8  
#define MAX_FW_RETURN_DATA_LENGTH         24

#define TUNE_DECREASE                     0x1
#define TUNE_INCREASE                     0x2

#define SPUR_SHIFT_CLOCK_ADJUST_MIN       205 
#define SPUR_SHIFT_CLOCK_ADJUST_MAX       227 

#define SEQ_STATE_RB_POLLING_TIMES        6

#define DFE_SEQ_STATE_VALUE               0x0A
#define DFE_SEQ_JUMP_VALUE                0x18

#define SPUR_SHIFT_FREQ_WINDOW            500000  // +- 0.5MHz

/* MxL68x Device/Tuner Registers */

#define POWER_SUPPLY_REG                    0x000E
#define XTAL_CFG_REG                        0x0001
#define XTAL_SHARING_REG                    0x0002
#define XTAL_CALI_SET_REG                   0x0003 // Xtal calibration enable register enable register 
#define DFE_XTAL_EXT_BIAS_REG               0x006D
#define DFE_DIG_ANA_XTAL_TH                 0x006E

#define DFE_CTRL_ACCUM_LOW_REG              0x0023 // Bit<7:0> 
#define DFE_CTRL_ACCUM_MID_REG              0x0024 // Bit<7:0>
#define DFE_CTRL_ACCUM_HI_REG               0x0025 // Bit<1:0>

#define DFE_REFSX_INTMOD_REG                0x004F
#define DFE_REFSX_INTMOD_BYP_REG            0x0050

#define TUNER_ENABLE_REG                    0x000B // Power up register, bit<0>
#define DFE_SEQ_BREAK_REG                   0x014B
#define START_TUNE_REG                      0x0012 // sequencer setting register 

#define DFE_SPUR_SHIFT_CTRL_REG             0x00F0 
#define DFE_SPUR_SHIFT_ADJ_REG              0x00F1 

#define DFE_SEQ_STATE_RB_REG                0x01D5

#define DFE_AGC_AUTO_REG                    0xB6
#define DFE_SEQ_TUNE_RF1_BO_REG             0x015F
#define DFE_SEQ_CDC_RF1_BO_REG              0x0160
#define DFE_GCOMP_IQ_SWITCH                 0x0086

#define IF_FREQ_SEL_REG                     0x0004 // IF frequency selection and manual set bypass register  
#define DIG_ANA_IF_CFG_0                    0x005A
#define DIG_ANA_IF_CFG_1                    0x005B
#define DIG_INT_AAF_EN_REG                  0x0059
#define DIG_ANA_IF_PWR                      0x005C
#define DFE_CSF_SS_SEL                      0x00EA
#define DFE_DACIF_GAIN                      0x00DC
#define DFE_LT_CONFIG_REG                   0x0196 

#define AGC_CONFIG_REG                      0x0008 // AGC configuration, include AGC select and AGC type  
#define AGC_SET_POINT_REG                   0x0009
#define AGC_FLIP_REG                        0x005E

#define IF_FREQ_SEL_REG                     0x0004 // IF frequency selection and manual set bypass register  
#define IF_PATH_GAIN_REG                    0x0005 // IF gain level and path selection register  
#define IF_FCW_LOW_REG                      0x0006 // Low register of IF FCW set when manual program IF frequency   
#define IF_FCW_HIGH_REG                     0x0007 // High register of IF FCW set when manual program IF frequency 

#define FINE_TUNE_SET_REG                   0x0013 // Fine tune operation register 
#define FINE_TUNE_CTRL_REG_0                0x0013 // Fine tune operation register 
#define FINE_TUNE_CTRL_REG_1                0x0014 // Fine tune operation register 

#define FINE_TUNE_OFFSET_LOW_REG            0x0014 // Fine tune frequency offset low byte 
#define FINE_TUNE_OFFSET_HIGH_REG           0x0015 // Fine tune frequency offset high byte

#define AGC_SAGCLOCK_STATUS_REG             0x002C 
#define RF_TOTAL_GAIN_RB_REG                0x0033 

#define RFPIN_RB_LOW_REG                    0x001D // RF power low 8 bit register 
#define RFPIN_RB_HIGH_REG                   0x001E // RF power high 8 bit register 

#define SIGNAL_TYPE_REG                     0x001E // Signal type

#define RF_REF_STATUS_REG                   0x002B // RF/REF lock status register 

#define DMD_RS_BER_PER_FRAME_MAILBOX_REG    0xA884
#define DMD_RS_BER_PER_FRAME                0x101C
#define DMD_RS_BER_PER_START                0x101B
#define DMD_RS_BER_PER_CLR                  0x101B
#define DMD_FEC_OUT_CTRL                    0x101D

#define DFE_RFLUT_SWP1_REG                  0x0049 
#define DIG_ANA_RFRSSI_REG                  0x0057 
#define DFE_RFLUT_BYP_REG                   0x00DB  
#define DFE_RFLUT_DIV_MOD_REG               0x00DB  
#define DFE_REFLUT_BYP_REG                  0x00EA  
#define DFE_REFSX_INT_MOD_REG               0x00EB  

#define DFE_SEQ_JUMP_REG                    0x014A
#define DFE_SEQ_BREAK_REG                   0x014B

#define ANA_DIG_RCTUNE_REG                  0x01A9 
#define DFE_SEQ_DIGANA_AGC_RCTUNE_REG       0x0153
#define DFE_SEQ_DIGANA_ADC_RCTUNE_REG       0x0154
#define DFE_SEQ_DIGANA_DNX_TC_RCTUNE_REG    0x0155

#define ANA_DIG_REFSX_XTAL_AMP_RB_REG       0x01A2
#define ANA_DIG_REFSX_XTAL_AMP_LIN_RB_REG   0x01A1
#define DIG_ANA_XTAL_XTAL_EXT_BIAS          0x006D

#define TUNE_MODE_REG                       0x000D
#define CHAN_TUNE_BW_REG                    0x000F // Band width register 
#define CHAN_TUNE_LOW_REG                   0x0010 // Tune frequency set low byte 
#define CHAN_TUNE_HI_REG                    0x0011 // Tune frequency set high byte 

#define FIRMWARE_DOWNLOAD_DONE_PERCENT      100 
#define FIRMWARE_DOWNLOAD_FAIL_PERCENT      0 

#define MAX_TRY_TIMES_FOR_RC_TUNE           10 
/* MxL68x Demod Register List */

#define DMD_CCTL_RSTRQ_N_REG                      0x3003 // SW reset from Host, bit[24], 1 is reset   
#define DMD_CCTL_DEMRQ_REG                        0x301F // Demod request, set it start demod, bit 0, 1 is enable. 
#define DMD_GPIO_FUNC_MODE_REG                    0x300B   

#define DMD_CHAN_SCAN_TUNE_XTAL_REG               0x3028 
#define DMD_CHAN_SCAN_TUNE_FREQ_REG               0x3029 

#define DFE_FW_REG_BUFFER_MSB                     0x302C
#define DFE_FW_REG_BUFFER_LSB                     0x302D

#define DFE_TT_SPUR_ENABLE                        0xA8B8

#define FIRMWARE_LOADED_CHECK_REG                 0x302B  // Bit<0>   
#define DMD_ID_VERSION_REG                        0x3001  // Chip Id & Chip Ver

//Customer Settings register
#define CUST_SET_REG                              0x302F
#define CUST_SET_FRM_SYNC_IMPRV                   0x0001
#define CUST_SET_NOISE_FLOOR_RAISE                0x0002
#define CUST_SET_DISABLE_SPUR                     0x0004

#define MPEG_OUT_CFG_MAILBOX_REG                  0xA8B6
#define MPEG_OUT_CFG_REG                          0x1122
#define MPEG_CLK_CFG_REG                          0x11AD
#define MPEG_CLK_PHASE_REG                        0x300D
#define MPEG_TS_DRV_STR_REG                       0x3009

#define DMD_TMCC_INFO_REG_0                       0x1126
#define DMD_TMCC_INFO_REG_1                       0x1127
#define DMD_TMCC_INFO_REG_2                       0x1128
#define DMD_TMCC_INFO_REG_3                       0x1129
#define DMD_STATUS_INFO_REG                       0x1148

#define DMD_TMCC_MGI_REG                          0x1125
#define DMD_TMCC_LOCKED_REG                       0x1125
#define DMD_TMCC_SEG_TYPE_REG                     0x1125
#define DMD_TMCC_SYS_IDENT_REG                    0x1126
#define DMD_TMCC_ALARM_REG                        0x1126
#define DMD_TMCC_MOD_A_REG                        0x1126
#define DMD_TMCC_MOD_B_REG                        0x1126
#define DMD_TMCC_MOD_C_REG                        0x1126
#define DMD_TMCC_CONV_A_REG                       0x1127
#define DMD_TMCC_CONV_B_REG                       0x1127
#define DMD_TMCC_CONV_C_REG                       0x1127
#define DMD_TMCC_INTV_A_REG                       0x1127
#define DMD_TMCC_INTV_B_REG                       0x1128
#define DMD_TMCC_INTV_C_REG                       0x1128
#define DMD_TMCC_SEG_A_REG                        0x1128
#define DMD_TMCC_SEG_B_REG                        0x1128
#define DMD_TMCC_SEG_C_REG                        0x1129
#define DMD_TMCC_PARTIAL_REG                      0x1129
#define DMD_CTRL_MODE_GI                          0x1148
#define DMD_CNR_DATA_ALL                          0x112D
#define DMD_AGC_INTGOUT_RB                        0x1190
#define DMD_RS_LOCK_A_B                           0x1190
#define DMD_RS_LOCK_B                             0x1190
#define DMD_RS_LOCK_C                             0x1191
#define DMD_STATUS                                0x1147
#define DMD_RS_BER_PER_END_FLG                    0x112E
#define DMD_RS_BER_A_DATA                         0x1130
#define DMD_RS_BER_B_DATA                         0x1132
#define DMD_RS_BER_C_DATA                         0x1134
#define DMD_RS_PER_A_DATA                         0x1136
#define DMD_RS_PER_B_DATA                         0x1138
#define DMD_RS_PER_C_DATA                         0x113A
#define DMD_RS_BER_ITER_A_DATA                    0x113C
#define DMD_RS_BER_ITER_B_DATA                    0x113E
#define DMD_RS_BER_ITER_C_DATA                    0x1140
#define DMD_RS_PER_ITER_A_DATA                    0x1142
#define DMD_RS_PER_ITER_B_DATA                    0x1144
#define DMD_RS_PER_ITER_C_DATA                    0x1146

#define DMD_RS_BER_PER_DATA_LO_MAILBOX            0xA8B4
#define DMD_RS_BER_PER_DATA_HI_MAILBOX            0xA8C6
#define DMD_RS_BER_PER_ITER_DATA_LO_MAILBOX       0xA8C8
#define DMD_RS_BER_PER_ITER_DATA_HI_MAILBOX       0xA8CA

//Mailbox regs used for CNR RB are same as ones used for BER PER RB
#define DMD_CNR_RB_LO_MAILBOX       0xA8C8
#define DMD_CNR_RB_HI_MAILBOX       0xA8CA

#define DMD_CARRIER_FREQ_OFF_LO_MAILBOX           0xA8C0
#define DMD_CARRIER_FREQ_OFF_HI_MAILBOX           0xA8BE
#define DMD_SAMP_FREQ_OFF_LO_MAILBOX              0xA8C4
#define DMD_SAMP_FREQ_OFF_HI_MAILBOX              0xA8C2

#define DMD_CNST_CTRL                             0x101C

#define MXL68X_FW_MAIL_BOX_REG            0xA85C 
#define DMD_INTERRUPT_MAILBOX_REG         0xA80A // interrupt mailbox register 

#define DMD_INT_MASK_AC_DATA              0x0001 // Bit<0>
#define DMD_INT_MASK_FW_CHAN_TUNE         0x0002 // Bit<1>

#define LAYER_A_MODULATION_MASK           0x0070
#define LAYER_A_MODULATION_BIT_SHIFT      4

#define LAYER_B_MODULATION_MASK           0x0700
#define LAYER_B_MODULATION_BIT_SHIFT      8

#define LAYER_C_MODULATION_MASK           0x7000
#define LAYER_C_MODULATION_BIT_SHIFT      12

#define LAYER_A_CR_MASK                   0x0007
#define LAYER_A_CR_BIT_SHIFT              0

#define LAYER_B_CR_MASK                   0x0070
#define LAYER_B_CR_BIT_SHIFT              4

#define LAYER_C_CR_MASK                   0x0700
#define LAYER_C_CR_BIT_SHIFT              8

#define LAYER_A_IL_MASK                   0x7000
#define LAYER_A_IL_BIT_SHIFT              12

#define LAYER_B_IL_MASK                   0x0007
#define LAYER_B_IL_BIT_SHIFT              0

#define LAYER_C_IL_MASK                   0x0070
#define LAYER_C_IL_BIT_SHIFT              4

#define LAYER_A_SEG_MASK                   0x0F00
#define LAYER_A_SEG_BIT_SHIFT              8

#define LAYER_B_SEG_MASK                   0xF000
#define LAYER_B_SEG_BIT_SHIFT              12

#define LAYER_C_SEG_MASK                   0x000F
#define LAYER_C_SEG_BIT_SHIFT              0




#define MXL68X_VERSION_SIZE          4
#define MAX_COMMAND_DATA_SIZE        252 

#define ISDBT_AC_DATA_LENGTH         24    // Total AC Data is 192 bits 
#define FW_CHAN_TUNE_DATA_LENGTH     8     // Total FW channel tune result data length 
#define MAX_FW_RETURN_DATA_LENGTH    24

/* IF Frequency in KHz */
#define MXL68x_IF_FREQ_3650_KHZ     3650
#define MXL68x_IF_FREQ_4000_KHZ     4000
#define MXL68x_IF_FREQ_4100_KHZ     4100
#define MXL68x_IF_FREQ_4150_KHZ     4150
#define MXL68x_IF_FREQ_4500_KHZ     4500
#define MXL68x_IF_FREQ_4570_KHZ     4570
#define MXL68x_IF_FREQ_5000_KHZ     5000
#define MXL68x_IF_FREQ_5380_KHZ     5380
#define MXL68x_IF_FREQ_6000_KHZ     6000
#define MXL68x_IF_FREQ_6280_KHZ     6280
#define MXL68x_IF_FREQ_7200_KHZ     7200
#define MXL68x_IF_FREQ_82500_KHZ    8250
#define MXL68x_IF_FREQ_35250_KHZ    35250
#define MXL68x_IF_FREQ_36000_KHZ    36000
#define MXL68x_IF_FREQ_36150_KHZ    36150
#define MXL68x_IF_FREQ_36650_KHZ    36650
#define MXL68x_IF_FREQ_44000_KHZ    44000

#define MAX_PRESET_IF_FREQ          17

/******************************************************************************
    User-Defined Types (Typedefs)
******************************************************************************/
typedef struct
{
	HUINT32 	ulStrength; 		/**< Signal strength percentage Unit */
	HUINT32 	ulQuality;			/**< Signal quality percentage Unit */
	HUINT32 	ulAgc;				/**< AGC - Half NIM */
	float		fSnr;				/**< SNR */
	HUINT32 	ulCorrectedNo;		/**< ulCorrectedNo */
	HUINT32 	ulUnCorrectedNo;	/**< ulUnCorrectedNo */
	HUINT32 	ulUnErroredNo;		/**< ulUnErroredNo */
	HUINT32 	ulInbandBer;		/**< ulInbandBer */
	HUINT32 	ulUnCorrectedFlag;	/**< ulUnCorrectedFlag */
	float fSignalInputPower;
	float fBer;
} MXL683_Signal_Info;
	

/* MxL68x SOC mode bandwidth type */
typedef enum
{
  MXL681_DEVICE = 0,           // MxL681  
  MXL683_DEVICE = 1,           // MxL683  
  UNKNOWN_DEVICE 
} MXL68x_DEVICE_TYPE_E;

/* Device Reset */
typedef enum
{
  MXL68x_DEMOD_RESET = 0,           /* Device Reset, Firmware should be downloaded after reset */
  MXL68x_DEMOD_DATAPATH_RESET,      /* Demod only reset */
  MXL68x_TUNER_RESET                /* Tuner only reset */
} MXL68x_RESET_TYPE_E;

/* Power supply set */
typedef enum
{
  MXL68x_SINGLE_SUPPLY = 0,
  MXL68x_DUAL_SUPPLY 
} MXL68x_PWR_SUPPLY_E;

/* XTAL set */
typedef enum
{
  MXL68x_XTAL_16MHz = 0,
  MXL68x_XTAL_24MHz 
} MXL68x_XTAL_FREQ_E;

/* Power mode set  */
typedef enum
{
  MXL68x_PWR_MODE_ACTIVE =0,
  MXL68x_PWR_MODE_STANDBY,
  MXL68x_PWR_MODE_EWS
} MXL68x_PWR_MODE_E;

/* Operation mode */
typedef enum
{
  MXL68x_TUNER_ONLY = 0,
  MXL68x_TUNER_DEMOD
} MXL68x_DEV_MODE_E;

/* IF source type */
typedef enum
{
  MXL68x_IF_INTERNAL = 0,
  MXL68x_IF_EXTERNAL = 1,
  MXL68x_IF_NA
} MXL68x_IF_SRC_E;

/* GPIO definition */
typedef enum
{
  MXL68x_GPIO_1 = 1, 
  MXL68x_GPIO_2 = 2,
  MXL68x_GPIO_3 = 3,
  MXL68x_GPIO_4 = 4,
  MXL68x_GPIO_5 = 5,
  MXL68x_GPIO_6 = 6,
  MXL68x_GPIO_7 = 7
} MXL68x_GPIO_ID_E;

/* GPIO direction definition */
typedef enum
{
  MXL68x_GPIO_INPUT = 0, 
  MXL68x_GPIO_OUTPUT = 1
} MXL68x_GPIO_TYPE_E;

/* GPIO data definition */
typedef enum
{
  MXL68x_GPIO_LOW = 0,
  MXL68x_GPIO_HIGH = 1
} MXL68x_GPIO_STATE_E;

typedef struct
{
  MXL68x_GPIO_ID_E gpioId;
  MXL68x_GPIO_TYPE_E gpioType;
  MXL68x_GPIO_STATE_E gpioState;
} MXL68x_GPIO_T, *PMXL68x_GPIO_T;

/* ISDB-T layer definition */
typedef enum
{
  MXL68x_ISDBT_LAYER_A = 0,  
  MXL68x_ISDBT_LAYER_B,
  MXL68x_ISDBT_LAYER_C,
  MXL68x_ISDBT_LAYER_ALL
} MXL68x_ISDBT_LAYER_E;

/* TMCC information */
typedef enum
{
  MXL68x_ISDBT_MODE1 = 0,             // ISDB-T mode1, 2k FFT  
  MXL68x_ISDBT_MODE2 = 1,             // ISDB-T mode2, 4k FFT
  MXL68x_ISDBT_MODE3 = 2,             // ISDB-T mode3, 8k FFT 
  MXL68x_ISDBT_MODE_UNKNOWN = 0xFF
} MXL68x_ISDBT_MODE_E;

typedef enum
{
  MXL68x_MODULATION_DQPSK = 0,
  MXL68x_MODULATION_QPSK  = 1,
  MXL68x_MODULATION_QAM16 = 2,
  MXL68x_MODULATION_QAM64 = 3, 
  MXL68x_MODULATION_UNKNOWN = 0xFF
} MXL68x_MODULATION_E;

typedef enum
{
  MXL68x_GI_1_32 = 0, 
  MXL68x_GI_1_4  = 1,
  MXL68x_GI_1_8  = 2,
  MXL68x_GI_1_16 = 3, 
  MXL68x_GI_UNKNOWN = 0xFF
} MXL68x_GUARD_INTERVAL_E;

typedef enum
{
  MXL68x_CR_1_2 = 0,
  MXL68x_CR_2_3 = 1,
  MXL68x_CR_3_4 = 2,
  MXL68x_CR_5_6 = 3,
  MXL68x_CR_7_8 = 4,
  MXL68x_CR_UNKNOWN = 0xFF
} MXL68x_CODE_RATE_E;

typedef enum
{
  MXL68x_INTERLEAVER_INDEX0 = 0,  
  MXL68x_INTERLEAVER_INDEX1 = 1,
  MXL68x_INTERLEAVER_INDEX2 = 2,
  MXL68x_INTERLEAVER_INDEX3 = 3, 
  MXL68x_INTERLEAVER_UNKNOWN = 0xFF
} MXL68x_INTERLEAVER_LEN_E;

typedef enum
{
  MXL68x_BROADCAST_TV = 0,  
  MXL68x_BROADCAST_AUDIO = 1,
  MXL68x_BROADCAST_NA = 2,
} MXL68x_BROADCAST_TYPE_E;

typedef struct
{
  MXL68x_MODULATION_E       modulationInfo;
  MXL68x_CODE_RATE_E        codeRateInfo;
  MXL68x_INTERLEAVER_LEN_E  interleaverLenInfo;
  HUINT8                     numOfSegments;
} MXL68x_TMCC_INFO_T;

/* MPEG Clock frequency */ 
typedef enum  
{
  MXL68x_MPEG_CLK_64MHZ = 0,     // Clock 64MHz  
  MXL68x_MPEG_CLK_32MHZ = 1,     // Clock 32 MHz 
  MXL68x_MPEG_CLK_16MHZ = 2,     // Clock 16 MHz
  MXL68x_MPEG_CLK_NA
} MXL68x_MPEG_CLK_FREQ_E;

/* MPEG Out data format */ 
typedef enum  
{
  MXL68x_MPEG_DATA_MSB_FIRST = 0,  // Serial MSB first
  MXL68x_MPEG_DATA_LSB_FIRST = 1,  // Serial LSB first   
  MXL68x_MPEG_DATA_NA
} MXL68x_MPEG_DATA_FMT_E;

typedef enum  
{
  MXL68x_POLARITY_NORMAL = 0,     // Normal Polarity   
  MXL68x_POLARITY_INVERTED = 1,   // Inverted Polarity
  MXL68x_POLARITY_NA
} MXL68x_MPEG_POLARITY_E;

typedef enum
{
  MXL68x_MPEG_SYNC_PULSE_1BYTE = 0,     // 1 Byte Pulse width
  MXL68x_MPEG_SYNC_PULSE_1BIT = 1,      // 1 Bit Pulse width  
  MXL68x_MPEG_SYNC_PULSE_NA
} MXL68x_MPEG_SYNC_PULSE_E;

typedef enum  
{
  MXL68x_TS_DRV_STR_1X = 0,
  MXL68x_TS_DRV_STR_3X = 1,
  MXL68x_TS_DRV_STR_5X = 2,
  MXL68x_TS_DRV_STR_7X = 3,
  MXL68x_TS_DRV_STR_NA
} MXL68x_TS_DRV_STR_E;

typedef enum  
{
  MXL68x_TS_PKT_204_BYTES = 0,      // 1 Bit Pulse width  
  MXL68x_TS_PKT_188_BYTES = 1,      // 1 Byte Pulse width
  MXL68x_TS_PKT_NA
} MXL68x_TS_PKT_SIZE_E;

typedef enum  
{
  MXL68x_MPEG_MODE_NORMAL = 0,  // MERR, MVAL, MSYNC, MCLK & MDAT
  MXL68x_MPEG_MODE_3WIRE = 1,   // MCLK, MSYNC & MDAT
  MXL68x_MPEG_MODE_NA
} MXL68x_MPEG_MODE_E;

/* MPEG out clock phase type */ 
typedef enum  
{
  MXL68x_MPEG_CLK_PHASE_SHIFT_0_DEG = 0,       
  MXL68x_MPEG_CLK_PHASE_SHIFT_90_DEG,
  MXL68x_MPEG_CLK_PHASE_SHIFT_180_DEG,
  MXL68x_MPEG_CLK_PHASE_SHIFT_270_DEG,
  MXL68x_MPEG_CLK_PHASE_SHIFT_NA
} MXL68x_MPEG_CLK_PHASE_E;

/* MPEG out error indicator type */
typedef enum
{
  MXL68x_MPEG_ERROR_IND_ENABLE = 0,
  MXL68x_MPEG_ERROR_IND_DISABLE = 1,
  MXL68x_MPEG_ERROR_IND_NA
} MXL68x_MPEG_ERROR_IND_E;

typedef struct
{
  MXL68x_MPEG_MODE_E        mpegMode;
  MXL68x_MPEG_CLK_FREQ_E    mpegClkFreq;
  MXL68x_MPEG_DATA_FMT_E    mpegDataFmt;
  MXL68x_MPEG_CLK_PHASE_E   mpegClkPhase;
  MXL68x_MPEG_POLARITY_E    mpegClkPol;
  MXL68x_MPEG_POLARITY_E    mpegSyncPol;
  MXL68x_MPEG_POLARITY_E    mpegValidPol;
  MXL68x_MPEG_SYNC_PULSE_E  mpegSyncPulseWidth;
  MXL68x_MPEG_ERROR_IND_E   mpegErrIndicator;
  MXL68x_TS_PKT_SIZE_E      tsPktSize;  
  MXL68x_TS_DRV_STR_E tsDriveStrength;
} MXL68x_MPEG_OUT_INF_T;

/* MxL68x SOC mode bandwidth type */
typedef enum
{
  MXL68x_BW_6MHz = 0,           // Bandwidth, 6MHz 
  MXL68x_BW_7MHz,               // Bandwidth, 7MHz 
  MXL68x_BW_8MHz                // Bandwidth, 8MHz 
} MXL68x_BW_E;

/* MxL68x SOC mode bandwidth type */
typedef enum
{
  MXL68x_AUTO_SPEC_INV_DISABLE = 0,           // Disbale Auto Spec Inv 
  MXL68x_AUTO_SPEC_INV_ENABLE,               // Enable Auto Spec Inv till first frm sync 
} MXL68x_AUTO_SPEC_INV;

/* Tuner application mode */
typedef enum
{
  MXL68x_ANA_NTSC_MODE = 0x0,
  MXL68x_ANA_PAL_BG = 0x01,
  MXL68x_ANA_PAL_I = 0x02,
  MXL68x_ANA_PAL_D = 0x03,
  MXL68x_ANA_SECAM_I = 0x04,
  MXL68x_ANA_SECAM_L = 0x05,
  MXL68x_DIG_DVB_C = 0x06, 
  MXL68x_DIG_ATSC = 0x07,
  MXL68x_DIG_DVB_T = 0x08,
  MXL68x_DIG_J83B = 0x09,
  MXL68x_DIG_ISDBT = 0x0A,
  MXL68x_SIGNAL_NA = 99
} MXL68x_SIGNAL_MODE_E;

/* AGC mode */
typedef enum
{
  MXL68x_AGC_SELF = 0,
  MXL68x_AGC_EXTERNAL = 1
} MXL68x_AGC_TYPE_E;

/* Channel Tune mode */
typedef enum
{
  MXL68x_VIEW_MODE = 0, 
  MXL68x_SCAN_MODE 
} MXL68x_TUNE_TYPE_E;

typedef enum
{
  MXL68x_NTSC_CARRIER_NA = 0,
  MXL68x_NTSC_CARRIER_HRC = 1,
  MXL68x_NTSC_CARRIER_IRC = 2
} MXL68x_NTSC_CARRIERS_E;

/* MxL68x Tuner work signal type */
typedef enum
{
  MXL68x_SIGNAL_TYPE_DIGITAL = 0,
  MXL68x_SIGNAL_TYPE_ANALOG 
} MXL68x_SIGNAL_TYPE_E;

typedef enum
{
  MXL68x_FINE_TUNE_STEP_UP = 0,
  MXL68x_FINE_TUNE_STEP_DOWN,
} MXL68x_FINE_TUNE_STEP_E;

typedef enum
{
  MXL68x_PLL_CLOCK = 0, 
  MXL68x_XTAL_CLOCK
} MXL68x_CLK_SWITCH_E;

// PER 
typedef struct
{
  HUINT32 pktErrCount_W_Iter;
  HUINT32 pktErrCount_WO_Iter;
  HUINT32 numOfPkts;
} MXL68x_PER_T;

// BER 
typedef struct
{
  HUINT32 bitErrCount_W_Iter;
  HUINT32 bitErrCount_WO_Iter;
  HUINT32 numOfBits;
} MXL68x_BER_T;

typedef enum 
{
  MXL_SUCCESS = 0,
  MXL_FAILURE = 1,
  MXL_INVALID_PARAMETER,
  MXL_NOT_INITIALIZED,
  MXL_ALREADY_INITIALIZED,
  MXL_NOT_SUPPORTED,
  MXL_NOT_READY
} MXL_STATUS_E;

typedef enum 
{
  MXL_DISABLE = 0,
  MXL_ENABLE  = 1,  
        
  MXL_FALSE = 0,
  MXL_TRUE  = 1,  

  MXL_INVALID = 0,
  MXL_VALID   = 1,

  MXL_NO      = 0,
  MXL_YES     = 1,  
  
  MXL_OFF     = 0,
  MXL_ON      = 1,  

  MXL_PORT_LOW  = 0,
  MXL_PORT_HIGH = 1,

  MXL_UNLOCKED = 0,
  MXL_LOCKED = 1 

} MXL_BOOL_E;


typedef struct
{
  HUINT16 modulationMask;
  HUINT16 codeRateMask;
  HUINT16 interleaverMask;
  HUINT16 segmentMask;
  HUINT8  modulationShift;
  HUINT8  codeRateShift;
  HUINT8  interleaverShift;
  HUINT8  segmentShift;
} MXL68x_TMCC_REG_MASK;

typedef enum
{
  MXL68x_DEV_ADDR1 = DEVICE_BASE_I2C_ADDRESS,
  MXL68x_DEV_ADDR2,
  MXL68x_DEV_ADDR3,
  MXL68x_DEV_ADDR4
} MXL68x_I2C_ADDRESS_E; // Permitted I2C address 

typedef enum
{
  MXL68x_INT_SOURCE1 = 0,  
  MXL68x_INT_SOURCE2 = 1,
  MXL68x_INT_SOURCE3 = 2,
  MXL68x_INT_SOURCE4 = 3,
  MXL68x_INT_NONE = 0xFF 
} MXL68x_INT_SOURCE_E;

typedef struct
{
  HUINT16 regAddr;
  HUINT16 mask;
  HUINT16 data;
} REG_CTRL_INFO_T, *PREG_CTRL_INFO_T;

typedef enum
{
  MXL68x_FW_STATUS_UNKNOWN = 0,
  MXL68x_FW_STATUS_BOOTLOADER,
  MXL68x_FW_STATUS_LOADED,
  MXL68x_FW_STATUS_CLKGATED,
} MXL68x_FW_STATUS_E;

typedef enum
{
  MXL68x_SPIN_UNTIL_EQ  = 0,
  MXL68x_SPIN_UNTIL_NEQ = 1,
} MXL68X_SPIN_CONDITION_E;

/* I2C COMMAND Communication: Command Code ID */
typedef enum
{
  HOST_CMD_SET_ADDR_CFG = 1,
  HOST_CMD_DOWNLOAD_SEGMENT_CFG,
  HOST_CMD_DOWNLOAD_DONE_CFG,

  HOST_CMD_RC_TUNE_CFG = 5,

  HOST_CMD_DEV_DEVICE_RESET_CFG = 10,
  HOST_CMD_DEV_XTAL_SETTINGS_CFG = 11,

// Perform Overwrite default in the Initialization procedure as shown in programming guide ¨C No need to use this as of now, done automatically by FW on start-up
  HOST_CMD_DEV_OVERWRITE_DEFAULT_CFG    = 13,

//FW Version info 
  HOST_CMD_DEV_VERSION_INFO_REQ         = 14,
//Power Mode to standby
  HOST_CMD_DEV_POWER_MODE_STANDBY_CFG   = 15,
//Wake up from Standby or EWS
  HOST_CMD_DEV_POWER_MODE_ON_CFG        = 16,
//Go to Auto EWS Mode
  HOST_CMD_DEV_POWER_MODE_AUTO_EWS_CFG  = 17,
// Read from multi-byte DFE register
  HOST_CMD_DEV_DFE_REG_READ             = 19,
// Write to multi-byte DFE register
  HOST_CMD_DEV_DFE_REG_WRITE            = 20,  

//Set the Int to Host (nINT Pin to 0)    
  HOST_CMD_DEMOD_INTR_CLEAR_CFG         = 24,
//Start streaming of AC Data through GPO pins (used instead of getting AC Data through Block Read command)
  HOST_CMD_DEMOD_ACDATA_GPO_CFG         = 27,  
//Retrieve AC Data through Block read
  HOST_CMD_DEMOD_AC_DATA_REQ            = 39,

// BER PER RB Host commands
  HOST_CMD_DEMOD_LAYER_A_BER_RB           = 40,
  HOST_CMD_DEMOD_LAYER_B_BER_RB           = 41,
  HOST_CMD_DEMOD_LAYER_C_BER_RB           = 42,    
  HOST_CMD_DEMOD_LAYER_A_PER_RB           = 43,
  HOST_CMD_DEMOD_LAYER_B_PER_RB           = 44,
  HOST_CMD_DEMOD_LAYER_C_PER_RB           = 45,
  HOST_CMD_DEMOD_CNR_RB                   = 46,
 

// Channel Tune (for digital ISDBT Channels in ISDB-T SoC Mode only)
  HOST_CMD_TUNER_CHAN_TUNE_CFG          = 51,
//App Mode dependent settings 
  HOST_CMD_TUNER_APP_DEP_SETTING        = 52,
// Reset DFE registers to Default
  HOST_CMD_TUNER_DFE_RESET_CFG          = 53,
// RC Tune procedure according to Prog Guide in the initialization procedures ¨C No need to use this as of now, done automatically by FW on start-up
  HOST_CMD_TUNER_INIT_RC_TUNE_CFG       = 54,
// Block read command for Tuner result (channel info like TMCC and such)
  HOST_CMD_TUNER_TUNE_RESULT_REQ        = 62,
// Enable Teletext Spur Mitigation algo in FW (should be enabled for analog mode)
  HOST_CMD_TUNER_TT_SPUR_MIT_EN_CFG     = 63,

  // MAX_NUM_HOST_COMMAND
} MXL_CMD_ID_E;



/* I2C COMMAND Communication: Response Code ID */
typedef enum
{
  RESPONSE_ID_SUCCESS = 0,
  RESPONSE_ID_NOT_READY,
  RESPONSE_ID_NOT_INTEGRATE,
  MAX_NUM_RESPONSE_ID
} MXL_RSP_ID_E;


/* Host command data structure */
typedef struct
{
  HUINT8 syncChar;                    /* 0x00: Bit[7-4]:Packet sync character (0x0E),  bit[3-0]: serial */
  MXL_CMD_ID_E commandId;            /* 0x01: command opcode = 0x02 for firmware download */
  HUINT8 payloadCheckSum;             /* 0x02: Checksum of packet payload */
  HUINT8 payloadLen;                  /* 0x03: Length of packet payload: 1 - 252 */
  HUINT8 data[MAX_COMMAND_DATA_SIZE]; /* 0x04-0x0xFF: command field data */
} HOST_COMMAND_T, *PHOST_COMMAND_T;

/* Firmware response data structure */
typedef struct
{
  HUINT8 syncChar;                    /* 0x00: Bit[7-4]:Packet sync character (0x0E),  bit[3-0]: serial */
  MXL_RSP_ID_E errorCode;            /* 0x01: Error code, 0x00 means ok. */
  HUINT8 payloadCheckSum;             /* 0x02: Checksum of packet payload */
  HUINT8 payloadLen;                  /* 0x03: Length of packet payload: 1 - 252 */
  HUINT8 data[MAX_COMMAND_DATA_SIZE]; /* 0x04-0xFF: response field data */
} FW_RESPONSE_T, *PFW_RESPONSE_T;

typedef struct
{
  MXL_BOOL_E ValidFlag;                  /* OUT, indicate if the data read back from FW is validate or not */
  HUINT16 DataLength;                     /* OUT, read back data length */
  HUINT8 Data[MAX_FW_RETURN_DATA_LENGTH]; /* OUT, store the AC data that read from firmware */  
} MXL68x_FW_INT_RET_DATA_T, *PMXL68x_FW_INT_RET_DATA_T;

typedef enum
{
  MXL68x_VER_ES1 = 0,
  MXL68x_VER_NA,
} MXL68x_DEV_VERSION;

typedef enum
{
  MXL68x_DRV_UNINITIALISED = 0,
  MXL68x_DRV_INITIALISED,
} MXL68x_DRV_STATUS;

typedef struct
{
  HUINT8                     i2cAddress;     // I2C address corresponding to this device 
  MXL68x_INT_SOURCE_E       intSrcId;       // Interrupt source ID  
  MXL68x_DRV_STATUS         driverStatus;   // Driver status 
  MXL68x_FW_STATUS_E        firmwareStatus; // Firmware status, unload or loaded. 
  MXL68x_FW_INT_RET_DATA_T  fwIntRetData;   // Firmware interrupt return data structure 
  MXL68x_DEV_VERSION        devVersion;     // Device version 
  MXL68x_XTAL_FREQ_E        xtalFreqInMHz;  // Device Xtal frequency selection 
  HUINT32                    ifOutFreqInKHz; // IF out frequency in KHz unit 
  HUINT8                     ifGainLevel;    // IF gain level 
  MXL_BOOL_E                pllClkReady;    // PLL clock readay status 
  MXL68x_DEV_MODE_E         devOpMode;      // Device operation mode, either tuner + demod or tuner - only 
  MXL68x_SIGNAL_MODE_E      signalType;     // current device application mode
  MXL68x_DEVICE_TYPE_E      mxl68xType;     // Indicate device is MxL681 or MxL683  
  MXL_BOOL_E                acDataGpoOut;   // Indicate if the AC data shall be stream out to GPO 
  MXL_BOOL_E                rfLoopThrough;  
  HINT16                    fwDownloadPercent; // Indicate firmware download percent. 
} MXL68x_DEV_CONTEXT_T, *PMXL68x_DEV_CONTEXT_T;

#define   MAX_SPUR_REG_NUM    4 

typedef struct
{
  HUINT8 SpurRegNum;  
  HUINT8 SpurRegAddr[MAX_SPUR_REG_NUM];
} CHAN_DEP_SPUR_REG_T, *PCHAN_DEP_SPUR_REG_T;

typedef struct
{
  HUINT32 centerFreqHz;
  HUINT8  rfLutSwp1Reg;
  HUINT8  rfLutDivInBypReg;
  HUINT8  refLutBypReg;
  HUINT8  refIntModReg;
} MXL68x_CHAN_DEP_FREQ_TABLE_T, *PMXL68x_CHAN_DEP_FREQ_TABLE_T;

#define MBIN_FORMAT_VERSION               '1'
#define MBIN_FILE_HEADER_ID               'M'
#define MBIN_SEGMENT_HEADER_ID            'S'
#define MBIN_MAX_FILE_LENGTH              (1<<23)

#define COMMAND_HEADER_LENGTH             4
#define MAX_COMMAND_DATA_SIZE             252 
#define MAX_BLOCK_WRITE_LENGTH            48   // Depends on I2C hardware limitation   
#define HOST_COMMAND_SYNC_IND             0xE0

typedef struct
{
  HUINT8 id;                 /* 0x00-0x00: file identifier ('M') */
  HUINT8 fmtVersion;         /* 0x01-0x01: file format version in ascii (default to '1') */
  HUINT8 headerLen;          /* 0x02-0x02: header length (<256) in bytes, should be sizeof(MBIN_FILE_HEADER_T) */
  HUINT8 numSegments;        /* 0x03-0x03: number of segments in the image (<256) */
  HUINT8 entryAddress[4];    /* 0x04-0x07: entry address of execution, in big endian */
  HUINT8 imageSize24[3];     /* 0x08-0x0A: 24-bit image size in bytes, not including header, in big endian */
  HUINT8 imageChecksum;      /* 0x0B-0x0B: 8-bit checksum of file data, not including header */
  HUINT8 reserved[4];        /* 0x0C-0x0F: reserved for future use */
} MBIN_FILE_HEADER_T;

typedef struct
{
  MBIN_FILE_HEADER_T  header;    /* 0x00: file header */
  HUINT8 data[1];                 /* 0x10: place holder for accessing starting of filedata */
} MBIN_FILE_T;

typedef struct
{
  HUINT8 id;                      /* 0x00-0x00: segment identifier (always 'S' for now) */
  HUINT8 len24[3];                /* 0x01-0x03: data length in 24-bit big endian, not including header */
                                 /* PLEASE note: */
                                 /*    len24 is the real length of following data segement, and the next memory */
                                 /*    segment (if any) will start from next 32-bit aligned address, which is   */
                                 /*    &data[(len24+3)&(~3)]                                                    */
  HUINT8 address[4];              /* 0x04-0x07: loading address of this segment, in big endian                   */
} MBIN_SEGMENT_HEADER_T;

typedef struct
{
  MBIN_SEGMENT_HEADER_T header;  /* 0x00: segment header */
  HUINT8 data[1];                 /* 0x08: place holder for accessing starting of data */
} MBIN_SEGMENT_T;


/******************************************************************************
    Global Variable Declarations
******************************************************************************/




int MXL683_InitDevice(int nChannelId);
int MXL683_SetTune( int nChannelId, HUINT8 *bChecklock , HUINT32 unWorkingId );
HBOOL MXL683_CheckLock ( int nChannelId, HUINT32 unWorkingID );
void MXL683_ResetGpio(int nChannelId);
int  MXL683_GetSignalInfo(int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo);
int  MXL683_SetPowerState( int nChannelId, CH_Power_e etPower );
int MXL683_EnableTsOutput ( int nChannelId );
int MXL683_DisableTsOutput ( int nChannelId );
int MXL683_SetTsOutputMode( int nChannelId, CH_TsOutMode_e etTsMode );
void MXL683_ProcessRecoverAntenna( int nChannelId, HUINT32 unWorkingId );
int MXL683_SetAntState( int nChannelId, CH_TER_AntStatus_e etAntState );
CH_TER_AntStatus_e MXL683_GetAntState( int nChannelId );
int MXL683_CheckAntState( int nChannelId );
//JAMES
int MXL683_DumpRegister( int nChannelId );
int MXL683_READ(int nChannelId, int nAddr);
int MXL683_WRITE(int nChannelId, int nAddr, int nData);
int MXL683_Check_TS_OUTPUT(int nChannelId);




MXL_STATUS_E MxLWare68x_API_CfgDrvInit(int devId);
MXL_STATUS_E MxLWare68x_API_CfgDrvDeInit(int devId);
MXL_STATUS_E MxLWare68x_API_CfgDevPowerSupply( int devId, MXL68x_PWR_SUPPLY_E devPwrSupplyCfg);
MXL_STATUS_E MxLWare68x_API_CfgDevOperationalMode(int devId, MXL68x_DEV_MODE_E deviceOpMode);
MXL_STATUS_E MxLWare68x_API_CfgDevXtal( int devId,  
		  MXL68x_XTAL_FREQ_E xtalFreqSel,    
                HUINT8 xtalCap,                                                                                             
                MXL_BOOL_E clkOutEnable,     
                MXL_BOOL_E clkOutDiv,
                MXL_BOOL_E enableXtalSharing,
                MXL_BOOL_E enableXtalCal);

MXL_STATUS_E MxLWare68x_API_CfgDevInit(
                int devId,                 
                MXL68x_IF_SRC_E ifSrc,
                MXL_BOOL_E enableRfLoopThru);

MXL_STATUS_E MxLWare68x_API_CfgDevFirmwareDownload( 
                int devId,
                HUINT32 mbinBufferSize,                    
                HUINT8 *mbinBufferPtr);

MXL_STATUS_E MxLWare68x_API_CfgDevPowerMode(
                int devId,
                MXL68x_PWR_MODE_E powerMode);

MXL_STATUS_E MxLWare68x_API_CfgDevGPIO (
                int devId,
                MXL68x_GPIO_ID_E gpioId,
                MXL68x_GPIO_TYPE_E  gpioType,
                MXL68x_GPIO_STATE_E gpioState);

MXL_STATUS_E MxLWare68x_API_CfgDevCustSpecificSettings(
                int devId,
                HUINT16 mask);
// Device Status Request    

MXL_STATUS_E MxLWare68x_API_ReqDevVersionInfo(
                int devId,
                HUINT8* chipIdPtr,  
                HUINT8* chipVersionPtr,
                HUINT8 mxlWareVer[MXL68X_VERSION_SIZE],
                HUINT8 firmwareVer[MXL68X_VERSION_SIZE],
                HUINT8 bootloaderVer[MXL68X_VERSION_SIZE]);

MXL_STATUS_E MxLWare68x_API_ReqDevGPIOStatus( 
                int devId,
                MXL68x_GPIO_ID_E gpioId,
                MXL68x_GPIO_TYPE_E  *gpioTypePtr,
                MXL68x_GPIO_STATE_E *gpioStatePtr);

MXL_STATUS_E MxLWare68x_API_ReqDevIntrStatus( 
                int devId,
                HUINT8* intrStatusPtr);

// Demod Configuration

MXL_STATUS_E MxLWare68x_API_CfgDemodClearBerPerEndFlag(
                int devId);

MXL_STATUS_E MxLWare68x_API_CfgDemodBerPer( 
                int devId,
                HUINT8 numFrames);

MXL_STATUS_E MxLWare68x_API_CfgDemodRSDecIterDec(
              int devId, 
              MXL_BOOL_E enableIterativeDecoder, 
              MXL_BOOL_E enableRsDecoder);

MXL_STATUS_E MxLWare68x_API_CfgDemodNullPacketFilter(
                int devId, 
                MXL_BOOL_E enableLayerA,
                MXL_BOOL_E enableLayerB,
                MXL_BOOL_E enableLayerC,
                MXL_BOOL_E enableSkipNullPacket);

MXL_STATUS_E MxLWare68x_API_CfgDemodMPEGOut( 
                int devId,  
                MXL68x_MPEG_MODE_E mpegMode,
                MXL68x_MPEG_CLK_FREQ_E mpegClkFreq,
                MXL68x_MPEG_DATA_FMT_E mpegDataFmt,
                MXL68x_MPEG_CLK_PHASE_E mpegClkPhase,    
                MXL68x_MPEG_POLARITY_E mpegSyncPol,
                MXL68x_MPEG_POLARITY_E mpegValidPol,
                MXL68x_MPEG_SYNC_PULSE_E mpegSyncPulseWidth,
                MXL68x_MPEG_ERROR_IND_E mpegErrIndicator,
		MXL68x_TS_PKT_SIZE_E tsPktSize,
		MXL68x_TS_DRV_STR_E tsDriveStrength);
	

// Demod Status Request
MXL_STATUS_E MxLWare68x_API_ReqDemodBerPerEndFlag(
                int devId, 
                HUINT8 *numFramesPtr,
                MXL_BOOL_E *berEndFlagPtr);

MXL_STATUS_E MxLWare68x_API_ReqDemodCNR(   
                int devId,
                HUINT32* cnrRawPtr,
                HINT32* cnrPtr);

MXL_STATUS_E MxLWare68x_API_ReqDemodLockStatus(        
                int devId,
                MXL_BOOL_E *tmccLockPtr,				
                MXL_BOOL_E *layerARSLockPtr,
                MXL_BOOL_E *layerBRSLockPtr,
                MXL_BOOL_E *layerCRSLockPtr);

MXL_STATUS_E MxLWare68x_API_ReqDemodNullPacketFilter(
                int devId, 
											 				MXL_BOOL_E* enableLayerAPtr,
												 			MXL_BOOL_E* enableLayerBPtr,
																MXL_BOOL_E* enableLayerCPtr,
																MXL_BOOL_E* enableSkipNullPacketPtr);

MXL_STATUS_E MxLWare68x_API_ReqDemodRsIterSetting(
                int devId, 
																MXL_BOOL_E* enableIterativeDecoder, 
																MXL_BOOL_E* enableRsDecoder);

MXL_STATUS_E MxLWare68x_API_ReqDemodMpegOutSet(
                int devId,
                MXL68x_MPEG_MODE_E *mpegModePtr,
                MXL68x_MPEG_CLK_FREQ_E *mpegClkFreqPtr,
                MXL68x_MPEG_DATA_FMT_E *mpegDataFmtPtr,
                MXL68x_MPEG_CLK_PHASE_E *mpegClkPhasePtr,    
                MXL68x_MPEG_POLARITY_E *mpegSyncPolPtr,
                MXL68x_MPEG_POLARITY_E *mpegValidPolPtr, 
                MXL68x_MPEG_SYNC_PULSE_E *mpegSyncPulseWidthPtr,         
                MXL68x_MPEG_ERROR_IND_E *mpegErrIndicatorPtr,      
		MXL68x_TS_PKT_SIZE_E *tsPktSizePtr,
		MXL68x_TS_DRV_STR_E *tsDriveStrength);

MXL_STATUS_E MxLWare68x_API_ReqDemodBER(
                int devId,   
                MXL68x_BER_T *BerPtr);

MXL_STATUS_E MxLWare68x_API_ReqDemodPER(
                int devId,   
                MXL68x_PER_T *layerAPktErrCountPtr,    
                MXL68x_PER_T *layerBPktErrCountPtr,
                MXL68x_PER_T *layerCPktErrCountPtr);

MXL_STATUS_E MxLWare68x_API_ReqDemodTMCCParams(
                int devId,
                MXL_BOOL_E *ewsAlarmPtr,   
                MXL68x_GUARD_INTERVAL_E *guardIntervalInfoPtr, 
                MXL68x_ISDBT_MODE_E *isdbtModeInfoPtr,   
                MXL68x_TMCC_INFO_T *tmccInfoLayerAPtr,  
                MXL68x_TMCC_INFO_T *tmccInfoLayerBPtr,              
                MXL68x_TMCC_INFO_T *tmccInfoLayerCPtr);

MXL_STATUS_E MxLWare68x_API_ReqDemodAcData(
	               int devId, 
                HUINT8 *acDataPtr);

MXL_STATUS_E MxLWare68x_API_ReqDemodFreqOffset(
	               int devId, 
                 HINT32 *carrierFreqOff,
                 HINT32 *samplingFreqOff);

MXL_STATUS_E MxLWare68x_API_ReqDemodFreqOffsetInKHzAndPPM(
	               int devId, 
                 HINT32 *carrierFreqOffInKHz,
                 HINT32 *samplingFreqOffInPPM);

MXL_STATUS_E MxLWare68x_API_ReqDemodSpectrumInversion(
                int devId,
                MXL_BOOL_E *isSpectrumInverted);

// Tuner Configuration
MXL_STATUS_E MxLWare68x_API_CfgTunerIFOutParams(
                int devId,
                HUINT16 ifOutFreqInKHz,
                MXL_BOOL_E invertIFPolarity,
                HUINT8 gainLevel);

MXL_STATUS_E MxLWare68x_API_CfgTunerApplicationMode(
                int devId,
                MXL68x_SIGNAL_MODE_E signalMode);

MXL_STATUS_E MxLWare68x_API_CfgTunerAGC( 
                int devId,
                MXL68x_AGC_TYPE_E agcType,
                HUINT8 setPoint,
                MXL_BOOL_E agcPolarityInversion);

MXL_STATUS_E MxLWare68x_API_CfgTunerChanTune(
                int devId,
                MXL68x_TUNE_TYPE_E tuneType,
                MXL68x_BW_E bandWidth,
                HUINT32 freqInHz,
                MXL68x_AUTO_SPEC_INV autoSpecInv,
                MXL_BOOL_E useChanTuneInt);

MXL_STATUS_E MxLWare68x_API_CfgTunerEnableFineTune(
                int devId,
                MXL_BOOL_E enableFineTune);

MXL_STATUS_E MxLWare68x_API_CfgTunerFineTune(
                int devId,
                MXL68x_FINE_TUNE_STEP_E fineTuneStep);

// Tuner Status Request
MXL_STATUS_E MxLWare68x_API_ReqTunerAGCStatus(  
                int devId,
                MXL_BOOL_E* agcLockStatusPtr,
                HUINT16* rawAgcGainPtr);

MXL_STATUS_E MxLWare68x_API_ReqTunerLockStatus(
                int devId,
                MXL_BOOL_E* rfSynthLockPtr,
                MXL_BOOL_E* refSynthLockPtr);

MXL_STATUS_E MxLWare68x_API_ReqTunerRxPower(
                int devId,
                HINT32* rxPwrPtr);

// Subrotine used in MxLWare 
MXL_STATUS_E Ctrl_CfgDemodEnable(int devId, MXL_BOOL_E demodEnable);
MXL_STATUS_E Ctrl_CfgDevSwitchClock(
                       int devId, 
                       MXL68x_CLK_SWITCH_E clkType);

MXL_STATUS_E Ctrl_SendDownloadCommand(int devId, MXL_CMD_ID_E commandId, void* dataPtr, HUINT32 dataLen, HUINT16 downloadBlockCnt);
MXL_STATUS_E Ctrl_CfgDevReset(int devId, MXL68x_RESET_TYPE_E resetType);
MXL_STATUS_E Ctrl_DfeRegRead(int devId, HUINT16 regAddr, HUINT8 numRegs, HUINT32* regDataPtr );
MXL_STATUS_E Ctrl_DfeRegWrite(int devId, HUINT16 regAddr, HUINT8 numRegs, HUINT32 regData );
MXL_STATUS_E Ctrl_GetDeviceResp(int devId, HUINT16 subAddr, PFW_RESPONSE_T responsePtr);




#endif


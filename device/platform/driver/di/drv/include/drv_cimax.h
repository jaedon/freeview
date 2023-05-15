/* $Header: */
/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name: drv_cimax.h
// Original Author:
// File Description:
// Remarks:

/*******************************************************************/
/* Copyright (c) 2008 HUMAX Co., Ltd.								*/
/* All rights reserved. 												*/
/*******************************************************************/
#ifndef	__DRV_CI_H__
#define	__DRV_CI_H__


/*******************************************************************/
/* INCLUDE FILES													*/
/*******************************************************************/
#include "drv_err.h"

/*******************************************************************/
/* EXTERN VARIABLES 												*/
/*******************************************************************/


/*******************************************************************/
/* MACRO DEFINITION 												*/
/*******************************************************************/
#define CAM_MAXSLOTID			2		// Kraken 은 CI Slot 1 개 임.

#define CAM_NO_DETECT			0
#define CAM_DETECT				1

#define CAM_POWER_OFF			0
#define CAM_POWER_ON			1

#define CAM_DISABLE				1		// buffer /OE 신호 이므로 '1' 이 Disable
#define CAM_ENABLE				0		// buffer /OE 신호 이므로 '0' 이 Enable


/**
 * @defgroup CIMaX_Error_Codes CIMaX+ Error Codes
 * @{
 */
/** A return value in case of no errors. */
#define CIMAX_NO_ERROR                 0
/** A return value in case of errors. */
#define CIMAX_ERROR                    -1
/** A return value in case of SPI communication errors. */
#define CIMAX_SPI_COMM_ERROR           -2
/** A return value in case of timeout error. */
#define CIMAX_TIMEOUT_ERROR            -3
/** A return value in case of init response error. */
#define CIMAX_INIT_RESP_ERROR          -4
/** A return value in case of write response error. */
#define CIMAX_WRITE_RESP_ERROR         -5
/** A return value in case of read response error. */
#define CIMAX_READ_RESP_ERROR          -6
/** A return value in case of command parse error. */
#define CIMAX_CMD_PARSE_ERROR          -7
/** A return value in case of boot command error. */
#define CIMAX_BOOT_CMD_ERROR           -8
/** A return value in case of invalid slot ID error. */
#define CIMAX_INVALID_SLOT_ID_ERROR    -10
/** A return value in case of CAM response error. */
#define CIMAX_CAM_RESPONSE_ERROR       -11
/** A return value in case of no CAM error. */
#define CIMAX_NO_CAM_ERROR             -12
/** A return value in case of CAM error. */
#define CIMAX_CAM_ERROR                -13
/** A return value in case of CAM write busy error. */
#define CIMAX_CAM_WRITE_BUSY_ERROR     -15
/** A return value in case of CAM command pending. */
#define CIMAX_CAM_CMD_PENDING          -16
/** @}*/

/**
 * @defgroup CIMaX_Register_Map CIMaX+ Register Map
 * @{
 */
#define BUFFIN_CFG                        0x0000
#define BUFFIN_ADDR_LSB                   0x0001
#define BUFFIN_ADDR_MSB                   0x0002
#define BUFFIN_DATA                       0x0003
#define BUFFOUT_CFG                       0x0004
#define BUFFOUT_ADDR_LSB                  0x0005
#define BUFFOUT_ADDR_MSB                  0x0006
#define BUFFOUT_DATA                      0x0007
#define BOOT_Key                          0x0008
#define BOOT_Status                       0x0009
#define BOOT_Test                         0x000A
#define usb2_0_irq_mask                   0x0010
#define usb2_0_status                     0x0011
#define usb2_0_rx                         0x0012
#define usb2_0_tx                         0x0013
#define SPI_Slave_Ctrl                    0x0018
#define SPI_Slave_Status                  0x0019
#define SPI_Slave_Rx                      0x001A
#define SPI_Slave_Tx                      0x001B
#define SPI_Slave_Mask                    0x001C
#define UCSG_Ctrl                         0x0020
#define UCSG_Status                       0x0021
#define UCSG_RxData                       0x0022
#define UCSG_TxData                       0x0023
#define PCtrl_Ctrl                        0x0028
#define PCtrl_Status                      0x0029
#define PCtrl_NbByte_LSB                  0x002A
#define PCtrl_NbByte_MSB                  0x002B
#define SPI_Master_Ctl                    0x0030
#define SPI_Master_NCS                    0x0031
#define SPI_Master_Status                 0x0032
#define SPI_Master_TxBuf                  0x0033
#define SPI_Master_RxBuf                  0x0034
#define BISTRAM_Ctl                       0x0038
#define BISTRAM_Bank                      0x0039
#define BISTRAM_Pat                       0x003A
#define BISTRAM_SM                        0x003B
#define BISTRAM_AddrLSB                   0x003C
#define BISTROM_Config                    0x0040
#define BISTROM_SignatureLSB              0x0041
#define BISTROM_SignatureMSB              0x0042
#define BISTROM_StartAddrLSB              0x0043
#define BISTROM_StartAddrMSB              0x0043
#define BISTROM_StopAddrLSB               0x0043
#define BISTROM_StopAddrMSB               0x0043
#define CkMan_Config                      0x0048
#define CkMan_Select                      0x0049
#define CkMan_Test                        0x004A
#define Revision_Number                   0x004B
#define ResMan_Config                     0x0050
#define ResMan_Status                     0x0051
#define ResMan_WD                         0x0052
#define ResMan_WD_MSB                     0x0053
#define CPU_Test                          0x0060
#define IrqMan_Config0                    0x0068
#define IrqMan_Config1                    0x0069
#define IrqMan_Irq0                       0x006A
#define IrqMan_NMI                        0x006B
#define IrqMan_SleepKey                   0x006C
#define Tim_Config                        0x0070
#define Tim_Value_LSB                     0x0071
#define Tim_Value_MSB                     0x0072
#define Tim_Comp_LSB                      0x0073
#define Tim_Comp_MSB                      0x0074
#define TI_Config                         0x0076
#define TI_Data                           0x0077
#define TI_Reg0                           0x0078
#define TI_Reg1                           0x0079
#define TI_Reg2                           0x007A
#define TI_Reg3                           0x007B
#define TI_Reg4                           0x007C
#define TI_ROM1                           0x007D
#define TI_ROM2                           0x007E
#define TI_ROM3                           0x007F
#define DVBCI_START_ADDR                  0x0100
#define DVBCI_END_ADDR                    0x017F
#define DATA                              0x0180
//#define CTRL                              0x0181
#define QB_HOST                           0x0182
#define LEN_HOST_LSB                      0x0183
#define LEN_HOST_MSB                      0x0184
#define FIFO_TX_TH_LSB                    0x0185
#define FIFO_TX_TH_MSB                    0x0186
#define FIFO_TX_D_NB_LSB                  0x0187
#define FIFO_TX_D_NB_MSB                  0x0188
#define QB_MOD_CURR                       0x0189
#define LEN_MOD_CURR_LSB                  0x018A
#define LEN_MOD_CURR_MSB                  0x018B
#define QB_MOD                            0x018C
#define LEN_MOD_LSB                       0x018D
#define LEN_MOD_MSB                       0x018E
#define FIFO_RX_TH_LSB                    0x018F
#define FIFO_RX_TH_MSB                    0x0190
#define FIFO_RX_D_NB_LSB                  0x0191
#define FIFO_RX_D_NB_MSB                  0x0192
#define IT_STATUS_0                       0x0193
#define IT_STATUS_1                       0x0194
#define IT_MASK_0                         0x0195
#define IT_MASK_1                         0x0196
#define IT_HOST_PIN_CFG                   0x0200
#define CFG_0                             0x0201
#define CFG_1                             0x0202
#define CFG_2                             0x0203
#define IT_HOST                           0x0204
#define MOD_IT_STATUS                     0x0205
#define MOD_IT_MASK                       0x0206
#define MOD_CTRL_A                        0x0207
#define MOD_CTRL_B                        0x0208
#define DEST_SEL                          0x0209
#define CAM_MSB_ADD                       0x020A
#define GPIO0_DIR                         0x020B
#define GPIO0_DATA_IN                     0x020C
#define GPIO0_DATA_OUT                    0x020D
#define GPIO0_STATUS                      0x020E
#define GPIO0_IT_MASK                     0x020F
#define GPIO0_DFT                         0x0210
#define GPIO0_MASK_DATA                   0x0211
#define GPIO1_DIR                         0x0212
#define GPIO1_DATA_IN                     0x0213
#define GPIO1_DATA_OUT                    0x0214
#define GPIO1_STATUS                      0x0215
#define GPIO1_IT_MASK                     0x0216
#define MEM_ACC_TIME_A                    0x0217
#define MEM_ACC_TIME_B                    0x0218
#define IO_ACC_TIME_A                     0x0219
#define IO_ACC_TIME_B                     0x021A
#define EXT_CH_ACC_TIME_A                 0x021B
#define EXT_CH_ACC_TIME_B                 0x021C
#define PAR_IF_0                          0x021D
#define PAR_IF_1                          0x021E
#define PAR_IF_CTRL                       0x021F
#define PCK_LENGTH                        0x0220
#define USB2TS_CTRL                       0x0221
#define USB2TS0_RDL                       0x0222
#define USB2TS1_RDL                       0x0223
#define TS2USB_CTRL                       0x0224
#define TSOUT_PAR_CTRL                    0x0225
#define TSOUT_PAR_CLK_SEL                 0x0226
#define S2P_CH0_CTRL                      0x0227
#define S2P_CH1_CTRL                      0x0228
#define P2S_CH0_CTRL                      0x0229
#define P2S_CH1_CTRL                      0x022A
#define TS_IT_STATUS                      0x022B
#define TS_IT_MASK                        0x022C
#define IN_SEL                            0x022D
#define OUT_SEL                           0x022E
#define ROUTER_CAM_CH                     0x022F
#define ROUTER_CAM_MOD                    0x0230
#define FIFO_CTRL                         0x0231
#define FIFO1_2_STATUS                    0x0232
#define FIFO3_4_STATUS                    0x0233
#define GAP_REMOVER_CH0_CTRL              0x0234
#define GAP_REMOVER_CH1_CTRL              0x0235
#define SYNC_RTV_CTRL                     0x0236
#define SYNC_RTV_CH0_SYNC_NB              0x0237
#define SYNC_RTV_CH0_PATTERN              0x0238
#define SYNC_RTV_CH1_SYNC_NB              0x0239
#define SYNC_RTV_CH1_PATTERN              0x023A
#define SYNC_RTV_OFFSET_PATT              0x023B
#define CTRL_FILTER                       0x023D
#define PID_EN_FILTER_CH0                 0x023E
#define PID_EN_FILTER_CH1                 0x023F
#define PID_LSB_FILTER_CH0_0              0x0240
#define PID_MSB_FILTER_CH0_0              0x0241
#define PID_LSB_FILTER_CH0_1              0x0242
#define PID_MSB_FILTER_CH0_1              0x0243
#define PID_LSB_FILTER_CH0_2              0x0244
#define PID_MSB_FILTER_CH0_2              0x0245
#define PID_LSB_FILTER_CH0_3              0x0246
#define PID_MSB_FILTER_CH0_3              0x0247
#define PID_LSB_FILTER_CH0_4              0x0248
#define PID_MSB_FILTER_CH0_4              0x0249
#define PID_LSB_FILTER_CH0_5              0x024A
#define PID_MSB_FILTER_CH0_5              0x024B
#define PID_LSB_FILTER_CH0_6              0x024C
#define PID_MSB_FILTER_CH0_6              0x024D
#define PID_LSB_FILTER_CH0_7              0x024E
#define PID_MSB_FILTER_CH0_7              0x024F
#define PID_LSB_FILTER_CH1_0              0x0260
#define PID_MSB_FILTER_CH1_0              0x0261
#define PID_LSB_FILTER_CH1_1              0x0262
#define PID_MSB_FILTER_CH1_1              0x0263
#define PID_LSB_FILTER_CH1_2              0x0264
#define PID_MSB_FILTER_CH1_2              0x0265
#define PID_LSB_FILTER_CH1_3              0x0266
#define PID_MSB_FILTER_CH1_3              0x0267
#define PID_LSB_FILTER_CH1_4              0x0268
#define PID_MSB_FILTER_CH1_4              0x0269
#define PID_LSB_FILTER_CH1_5              0x026A
#define PID_MSB_FILTER_CH1_5              0x026B
#define PID_LSB_FILTER_CH1_6              0x026C
#define PID_MSB_FILTER_CH1_6              0x026D
#define PID_LSB_FILTER_CH1_7              0x026E
#define PID_MSB_FILTER_CH1_7              0x026F
#define PID_OLD_LSB_REMAPPER_0            0x0280
#define PID_OLD_MSB_REMAPPER_0            0x0281
#define PID_OLD_LSB_REMAPPER_1            0x0282
#define PID_OLD_MSB_REMAPPER_1            0x0283
#define PID_OLD_LSB_REMAPPER_2            0x0284
#define PID_OLD_MSB_REMAPPER_2            0x0285
#define PID_OLD_LSB_REMAPPER_3            0x0286
#define PID_OLD_MSB_REMAPPER_3            0x0287
#define PID_OLD_LSB_REMAPPER_4            0x0288
#define PID_OLD_MSB_REMAPPER_4            0x0289
#define PID_OLD_LSB_REMAPPER_5            0x028A
#define PID_OLD_MSB_REMAPPER_5            0x028B
#define PID_OLD_LSB_REMAPPER_6            0x028C
#define PID_OLD_MSB_REMAPPER_6            0x028D
#define PID_OLD_LSB_REMAPPER_7            0x028E
#define PID_OLD_MSB_REMAPPER_7            0x028F
#define PID_NEW_LSB_REMAPPER_0            0x02A0
#define PID_NEW_MSB_REMAPPER_0            0x02A1
#define PID_NEW_LSB_REMAPPER_1            0x02A2
#define PID_NEW_MSB_REMAPPER_1            0x02A3
#define PID_NEW_LSB_REMAPPER_2            0x02A4
#define PID_NEW_MSB_REMAPPER_2            0x02A5
#define PID_NEW_LSB_REMAPPER_3            0x02A6
#define PID_NEW_MSB_REMAPPER_3            0x02A7
#define PID_NEW_LSB_REMAPPER_4            0x02A8
#define PID_NEW_MSB_REMAPPER_4            0x02A9
#define PID_NEW_LSB_REMAPPER_5            0x02AA
#define PID_NEW_MSB_REMAPPER_5            0x02AB
#define PID_NEW_LSB_REMAPPER_6            0x02AC
#define PID_NEW_MSB_REMAPPER_6            0x02AD
#define PID_NEW_LSB_REMAPPER_7            0x02AE
#define PID_NEW_MSB_REMAPPER_7            0x02AF
#define MERGER_DIV_MICLK                  0x02C0
#define PID_AND_SYNC_REMAPPER_CTRL        0x02C1
#define PID_EN_REMAPPER                   0x02C2
#define SYNC_SYMBOL                       0x02C3
#define PID_AND_SYNC_REMAPPER_INV_CTRL    0x02C4
#define BITRATE_CH0_LSB                   0x02C5
#define BITRATE_CH0_MSB                   0x02C6
#define BITRATE_CH1_LSB                   0x02C7
#define BITRATE_CH1_MSB                   0x02C8
#define STATUS_CLK_SWITCH_0               0x02C9
#define STATUS_CLK_SWITCH_1               0x02CA
#define RESET_CLK_SWITCH_0                0x02CB
#define RESET_CLK_SWITCH_1                0x02CC
#define PAD_DRVSTR_CTRL                   0x02CD
#define PAD_PUPD_CTRL                     0x02CE
#define PRE_HEADER_ADDER_CH0_0            0x02D0
#define PRE_HEADER_ADDER_CH0_1            0x02D1
#define PRE_HEADER_ADDER_CH0_2            0x02D2
#define PRE_HEADER_ADDER_CH0_3            0x02D3
#define PRE_HEADER_ADDER_CH0_4            0x02D4
#define PRE_HEADER_ADDER_CH0_5            0x02D5
#define PRE_HEADER_ADDER_CH0_6            0x02D6
#define PRE_HEADER_ADDER_CH0_7            0x02D7
#define PRE_HEADER_ADDER_CH0_8            0x02D8
#define PRE_HEADER_ADDER_CH0_9            0x02D9
#define PRE_HEADER_ADDER_CH0_10           0x02DA
#define PRE_HEADER_ADDER_CH0_11           0x02DB
#define PRE_HEADER_ADDER_CH1_0            0x02E0
#define PRE_HEADER_ADDER_CH1_1            0x02E1
#define PRE_HEADER_ADDER_CH1_2            0x02E2
#define PRE_HEADER_ADDER_CH1_3            0x02E3
#define PRE_HEADER_ADDER_CH1_4            0x02E4
#define PRE_HEADER_ADDER_CH1_5            0x02E5
#define PRE_HEADER_ADDER_CH1_6            0x02E6
#define PRE_HEADER_ADDER_CH1_7            0x02E7
#define PRE_HEADER_ADDER_CH1_8            0x02E8
#define PRE_HEADER_ADDER_CH1_9            0x02E9
#define PRE_HEADER_ADDER_CH1_10           0x02EA
#define PRE_HEADER_ADDER_CH1_11           0x02EB
#define PRE_HEADER_ADDER_CTRL             0x02EC
#define PRE_HEADER_ADDER_LEN              0x02ED
#define PRE_HEADER_REMOVER_CTRL           0x02EE
#define FSM_DVB                           0x02F0
#define TS2USB_FSM_DEBUG                  0x02F2
#define TSOUT_PAR_FSM_DEBUG               0x02F3
#define GAP_REMOVER_FSM_DEBUG             0x02F4
#define PID_AND_SYNC_REMAPPER_FSM_DEBUG   0x02F5
#define PRE_HEADER_ADDER_FSM_DEBUG        0x02F6
#define SYNC_RTV_FSM_DEBUG                0x02F7
#define CHECK_PHY_CLK                     0x0E00
#define USB_CTRL1                         0x0E01
#define USB_ISO2_out                      0x0800
#define USB_ISO1_out                      0x1000
#define USB_Interrupt_out                 0x1E00
#define USB_Bulk_in                       0x1F00
#define CC2_Buffer_out                    0x2000
#define USB_EP0                           0x30C0
#define CC2_Buffer_in                     0x4000
#define USB_ISO2_in                       0x5800
#define USB_ISO1_in                       0x6000
#define nmb_vector_address_lsb            0xFFFA
#define nmb_vector_address_msb            0xFFFB
#define reset_vector_address_lsb          0xFFFC
#define reset_vector_address_msb          0xFFFD
#define irb_vector_address_lsb            0xFFFE
#define irb_vector_address_msb            0xFFFF


/**************************************************************************//**
* @defgroup defines Defines
* @{
******************************************************************************/
/** CIMaX+ read thread stack size. */
#define CIMAX_READ_THREAD_STACK_SIZE      8192
/** CIMaX+ check INT period defined in milliseconds. */
#define CIMAX_CHECK_INT_PERIOD_MS         2
/** CIMaX+ register response timeout defined in milliseconds. */
#define CIMAX_REGISTER_RESPONSE_TIMEOUT_MS 200
/** CIMaX+ CAM response timeout defined in milliseconds. */
#define CIMAX_CAM_RESPONSE_TIMEOUT_MS      5000

/** CIMaX+ firmware start address. */
#define FW_START_ADDRESS                  0x8000
/** CIMaX+ firmware end address. */
#define FW_END_ADDRESS                    0xCFF9
/** CIMaX+ firmware vector table start address. */
#define FW_VECTOR_TABLE_START_ADDRESS     0xFFFA
/** CIMaX+ firmware vector table end address. */
#define FW_VECTOR_TABLE_END_ADDRESS		  0xFFFF
/** CIMaX+ firmware vector table size. */
#define FW_VECTOR_TABLE_SIZE              6
/** CIMaX+ firmware packet size. */
#define FW_PACKET_SIZE                    128


/*=============================================================================
* CIMaX+ commands
=============================================================================*/
/** CIMaX+ register header size. */
#define CIMAX_REGISTER_HEADER_SIZE        4
/** CIMaX+ register command position. */
#define CIMAX_REGISTER_CMD_POS            0
/** CIMaX+ register address most significant byte position. */
#define CIMAX_REGISTER_REG_ADDR_MSB_POS   1
/** CIMaX+ register address least significant byte position. */
#define CIMAX_REGISTER_REG_ADDR_LSB_POS   2
/** CIMaX+ register number of registers position. */
#define CIMAX_REGISTER_NUM_OF_REGS_POS    3

/** CIMaX+ register maximum payload size. */
#define CIMAX_REGISTER_MAX_PAYLOAD_SIZE   255

/** CIMaX+ register init request. */
#define CIMAX_REGISTER_INIT_REQ           0x00
/** CIMaX+ register write request. */
#define CIMAX_REGISTER_WRITE_REQ          0x7F
/** CIMaX+ register read request. */
#define CIMAX_REGISTER_READ_REQ           0xFF

/** CIMaX+ register init response OK. */
#define CIMAX_REGISTER_INIT_RESP_OK       0x4B
/** CIMaX+ register init response not OK. */
#define CIMAX_REGISTER_INIT_RESP_NOK      0xCB
/** CIMaX+ register write response OK. */
#define CIMAX_REGISTER_WRITE_RESP_OK      0x4D
/** CIMaX+ register write response not OK. */
#define CIMAX_REGISTER_WRITE_RESP_NOK     0xCD
/** CIMaX+ register read response OK. */
#define CIMAX_REGISTER_READ_RESP_OK       0x4C
/** CIMaX+ register read response not OK. */
#define CIMAX_REGISTER_READ_RESP_NOK      0xCC
/** CIMaX+* register command parse error response. */
#define CIMAX_REGISTER_CMD_PARSE_ERROR    0x51
/** CIMaX+* registr boot command not OK. */
#define CIMAX_REGISTER_BOOT_CMD_NOK       0x70

/*=============================================================================
* CAM commands
=============================================================================*/
/** CIMaX+ CAM packet counter value. */
#define CIMAX_CAM_PACKET_COUNTER_VALUE    0x01

/** CIMaX+ CAM inserted value. */
#define CIMAX_CAM_INSERTED                0x01
/** CIMaX+ CAM extracted value. */
#define CIMAX_CAM_EXTRACTED               0x00

/** CIMaX+ CAM slot mask. */
#define CIMAX_CAM_SLOT_MASK               0x80
/** CIMaX+ CAM slot bit position. */
#define CIMAX_CAM_SLOT_BIT_POSITION       7
/** CIMaX+ CAM type command mask. */
#define CIMAX_CAM_TYP_CMD_MASK            0x7F

/** CIMaX+ CAM header size. */
#define CIMAX_CAM_HEADER_SIZE             4
/** CIMaX+ CAM command or status position. */
#define CIMAX_CAM_COMMAND_OR_STATUS_POS   0
/** CIMaX+ CAM packet counter position. */
#define CIMAX_CAM_PACKET_COUNTER_POS      1
/** CIMaX+ CAM data length most significant byte position. */
#define CIMAX_CAM_DATA_LEN_MSB_POS        2
/** CIMaX+ CAM data length least significant byte position. */
#define CIMAX_CAM_DATA_LEN_LSB_POS        3
/** CIMaX+ CAM first byte of the data position. */
#define CIMAX_CAM_FIRST_DATA_BYTE_POS     4
/** CIMaX+ CAM second byte of the data position. */
#define CIMAX_CAM_SECOND_DATA_BYTE_POS    5

/** CIMaX+ CAM maximum payload size. */
#define CIMAX_CAM_MAX_PAYLOAD_SIZE        65535
/** CIMaX+ CAM maximum card information structure size. */
#define CIMAX_CAM_CIS_MAX_SIZE            512   /* To be checked  */
/** CIMaX+ CAM COR writing command payload size. */
#define CIMAX_CAM_COR_PAYLOAD_SIZE        5
/** CIMaX+ CAM COR additional payload size. */
#define CIMAX_CAM_COR_ADD_PAYLOAD_SIZE    2
/** CIMaX+ CAM buffer negotiation command payload size. */
#define CIMAX_CAM_NEGOTIATE_PAYLOAD_SIZE  2
/** CIMaX+ CAM detection response payload size. */
#define CIMAX_CAM_CAMDET_PAYLOAD_SIZE     1

/** CIMaX+ CAM COR address most significant byte position. */
#define CIMAX_CAM_COR_ADDR_MSB_POS        0
/** CIMaX+ CAM COR address least significant byte position. */
#define CIMAX_CAM_COR_ADDR_LSB_POS        1
/** CIMaX+ CAM COR coomand value position. */
#define CIMAX_CAM_COR_VALUE_POS           2
/** CIMaX+ CAM COR command last byte most significant byte position. */
#define CIMAX_CAM_COR_LAST_MSB_POS        3
/** CIMaX+ CAM COR command last byte least significant byte position. */
#define CIMAX_CAM_COR_LAST_LSB_POS        4

/** CIMaX+ CAM buffer negotiation command most significant byte position. */
#define CIMAX_CAM_NEGOTIATE_SIZE_MSB_POS  0
/** CIMaX+ CAM buffer negotiation command least significant byte position. */
#define CIMAX_CAM_NEGOTIATE_SIZE_LSB_POS  1

#define CIMAX_CAM_INIT_CMD                0x00
/** CIMaX+ CAMReset command. */
#define CIMAX_CAM_RESET_CMD               0x01
/** CIMaX+ GetCIS command. */
#define CIMAX_CAM_GET_CIS_CMD             0x02
/** CIMaX+ CAM WriteCOR command. */
#define CIMAX_CAM_WRITE_COR_CMD           0x03
/** CIMaX+ CAM Negotiate command. */
#define CIMAX_CAM_NEGOCIATE_CMD           0x04
/** CIMaX+ CAM WriteLPDU command. */
#define CIMAX_CAM_WRITE_LPDU_CMD          0x05
/** CIMaX+ CAM ReadLPDU command. */
#define CIMAX_CAM_READ_LPDU_CMD           0x06
/** CIMaX+ CAM GetEvt command. */
#define CIMAX_CAM_GET_EVT_CMD             0x0D
/** CIMaX+ CAM REGSTATUS command. */
#define CIMAX_CAM_REG_STATUS              0x0E

/** CIMaX+ CAM CAMReset A response. */
#define CIMAX_CAM_A_RESET_OK              0x40
/** CIMaX+ CAM CAMReset B response. */
#define CIMAX_CAM_B_RESET_OK              0xC0
/** CIMaX+ CAM GetCIS A response. */
#define CIMAX_CAM_A_GET_CIS_OK            0x41
/** CIMaX+ CAM GetCIS B response. */
#define CIMAX_CAM_B_GET_CIS_OK            0xC1
/** CIMaX+ CAM WriteCOR A response. */
#define CIMAX_CAM_A_WRITE_COR_OK          0x42
/** CIMaX+ CAM WriteCOR B response. */
#define CIMAX_CAM_B_WRITE_COR_OK          0xC2
/** CIMaX+ CAM Negotiate A response. */
#define CIMAX_CAM_A_NEGOTIATE_OK          0x43
/** CIMaX+ CAM Negotiate B response. */
#define CIMAX_CAM_B_NEGOTIATE_OK          0xC3
/** CIMaX+ CAM WriteLPDU A response. */
#define CIMAX_CAM_A_WRITE_LPDU_OK         0x44
/** CIMaX+ CAM WriteLPDU B response. */
#define CIMAX_CAM_B_WRITE_LPDU_OK         0xC4
/** CIMaX+ CAM CamDet A response. */
#define CIMAX_CAM_A_DET_OK                0x45
/** CIMaX+ CAM CamDet B response. */
#define CIMAX_CAM_B_DET_OK                0xC5
/** CIMaX+ CAM ReadLPDU A response. */
#define CIMAX_CAM_A_READ_LPDU_OK          0x46
/** CIMaX+ CAM ReadLPDU B OK. */
#define CIMAX_CAM_B_READ_LPDU_OK          0xC6
/** CIMaX+ CAM CAMNoCAM A response. */
#define CIMAX_CAM_A_NO_CAM                0x49
/** CIMaX+ CAM CAMNoCAM B response. */
#define CIMAX_CAM_B_NO_CAM                0xC9
/** CIMaX+ CAM CAMError A response. */
#define CIMAX_CAM_A_CAM_ERROR             0x4A
/** CIMaX+ CAM CAMError B response. */
#define CIMAX_CAM_B_CAM_ERROR             0xCA
/** CIMaX+ CAM DataReady A response. */
#define CIMAX_CAM_A_DATA_READY            0x4E
/** CIMaX+ CAM DataReady B response. */
#define CIMAX_CAM_B_DATA_READY            0xCE
/** CIMaX+ CAM StatusNoEvt response. */
#define CIMAX_CAM_NO_EVT                  0x55
/** CIMaX+ CAM CAMWriteBusy A response. */
#define CIMAX_CAM_A_WRITE_BUSY            0x54
/** CIMaX+ CAM CAMWriteBusy B response. */
#define CIMAX_CAM_B_WRITE_BUSY            0xD4
/** CIMaX+ CAM CMD_PENDING A response. */
#define CIMAX_CAM_A_CMD_PENDING           0x56
/** CIMaX+ CAM CMD_PENDING B response. */
#define CIMAX_CAM_B_CMD_PENDING           0xD6
/** CIMaX+ CAM REGSTATUSOK A response. */
#define CIMAX_CAM_A_REG_STATUS_OK         0x57
/** CIMaX+ CAM REGSTATUSOK B response. */
#define CIMAX_CAM_B_REG_STATUS_OK         0xD7

/*=============================================================================
* CAMError status
=============================================================================*/
/** CIMaX+ CAM Not Ready. */
#define CAMERROR_RESET                    0x0101
/** CIMaX+ CAM CIS buffer not allocate. */
#define CAMERROR_CIS_BUFFER               0x0201
/** CIMaX+ CAM CIS bad size (> 512 bytes). */
#define CAMERROR_CIS_BAD_SIZE             0x0202
/** CIMaX+ CAM CAM Not Activated. */
#define CAMERROR_CIS_CAM_NOT_ACTIVATED    0x0203
/** CIMaX+ CAM CAM not ready after software reset during write COR process. */
#define CAMERROR_COR_CAM_NOT_READY        0x0301
/** CIMaX+ CAM Checking of COR Value failed. */
#define CAMERROR_COR_VALUE_CHECKING       0x0302
/** CIMaX+ CAM CAM not responding after SR bit is raised during Buffer negociation phase. */
#define CAMERROR_NEGOTIATE_NOT_RESPONDING 0x0401
/** CIMaX+ CAM CAM buffer size length <> 2. */
#define CAMERROR_NEGOTIATE_SIZE_LENGTH    0x0402
/** CIMaX+ CAM CAM not ready to accept new buffer size negociate during Buffer negociation phase. */
#define CAMERROR_NEGOTIATE_NOT_READY      0x0403
/** CIMaX+ CAM LPDU not available. */
#define CAMERROR_LPDU_NOT_AVAILABLE       0x0601
/** @}*/



/*
	CIMAX+ register value define
*/
#define	DEFAULT_VALUE		0x00

/* FIFO_CTRL			0x0231 */
#define	RSTF_4		0x08
#define	RSTF_3		0x04
#define	RSTF_2		0x02
#define	RSTF_1		0x01

/* TS_IT_STATUS		0x022b */
#define	SYNC_1			0x08
#define	DSYNC_1			0x04
#define	SYNC_0			0x02
#define	DSYNC_0			0x01

/* SYNC_RTV_CTRL	0x0236 */
#define	EN_SYNC_1		0x08
#define	RST_1			0x04
#define	EN_SYNC_0		0x02
#define	RST_0			0x01

/* IN_SEL	0x022D */
#define	CH1_SRC_MASK		0x70
#define	CH0_SRC_MASK		0x07
#define	CH_IN_SHIFT			4
#define	CH_SRC_NULL			0x00
#define	CH_IN_TSPARALLE		0x01
#define	CH_IN_USBENDPOINT	0x02
#define	CH_IN_TSSERIAL1		0x04
#define	CH_IN_TSSERIAL2		0x05
#define	CH_IN_TSSERIAL3		0x06
#define	CH_IN_TSSERIAL4		0x07

/* OUT_SEL	0x022E */
#define	CH_OUT_TSSERIAL2			0x04
#define	CH_OUT_TSSERIAL1			0x02
#define	CH_OUT_TSPARALLE			0x01

/* CkMan_Config 0x0048 */
#define	EN_SER0_CLK		0x40
#define	EN_SER1_CLK		0x20
#define	EN_TS_CLK		0x10
#define	EN_108_CLK		0x08
#define	EN_72_CLK		0x04
#define	EN_54_CLK		0x02
#define	EN_27_CLK		0x01

/* CkMan_Select 0x0049 */
#define	SER_CLK_0_MASK	0x0C
#define	SER_CLK_SHIFT	2
#define	SER_CLK_1_MASK	0x30
#define	TS_CLK_MASK		0x03

#define	TS_SER_CLK_27MHZ	0x00
#define	TS_SER_CLK_54MHZ	0x01
#define	TS_SER_CLK_72MHZ	0x02
#define	TS_SER_CLK_108MHZ	0x03

/* S2P_CH0_CTRL 	0x0227 , S2P_CH1_CTRL		0x0228 */
#define	SER_EN_CH			0x20
#define	SYNC_EN_CH			0x10
#define	ISBE				0x04
#define	ICE					0x02
#define	RST					0x01

/* P2S_CH0_CTRL 	0x0229 , P2S_CH1_CTRL		0x022A */
#define	STRT				0x40
#define	RLEN				0x20
#define	OSCS				0x10
#define	EN_P2S				0x08
#define	OSBE				0x04
#define	OCE					0x02
#define	RST					0x01


/* ROUTER_CAM_MOD		0x0230	*/
#define	IN_OF_CAM0			0x07
#define	IN_OF_CAM1			0x70
#define	SHIFT_IN_OF_SLOT	4
#define	IN_CH0				0x01
#define	IN_CH1				0x02
#define	IN_OTHER_CAMOUT		0x05
#define	IN_GAP_REMOVER0		0x06
#define	IN_GAP_REMOVER1		0x07

/* ROUTER_CAM_CH		0x022F	*/
#define	OUT_OF_CH0			0x0F
#define	OUT_OF_CH1			0xF0
#define	SHIFT_OUT_OF_CH		4

#define	IN_SELECT_NONE		0x08
#define	IN_SELECT_CAM		0x00
#define	IN_SELECT_CH0		0x01
#define	IN_SELECT_CH1		0x02
#define	IN_OTHER_CAM		0x05


/* GPIO0_DATA_OUT	0x020D	*/
/* GPIO0_DFT		0x0210	*/
/* GPIO0_MASK_DATA	0x0211	*/
/* GPIO0_DIR	0x020B	*/
#define	EN_SLOT0		0x01
#define	EN_SLOT1		0x02

/* CFG_1		0x0202	*/
#define	UN_TS_PARALLEL		0x80
#define	EN_DVS				0x70
#define	EN_VCC				0x20
#define	EN_COPY				0x10
#define	EN_PCM_OUT			0x08
#define	EN_OOB				0x04
#define	SUPPORT_2SLOTS		0x02
#define	SUPPORT_DVB_CARD	0x01

/* CFG_2		0x0203	*/
#define	PCMCIA_LOCK			0x01

/* MEM_ACC_TIME_A		0x0217,  MEM_ACC_TIME_B 	0x0218	*/
#define	MASK_ATTRIBUTE_MEM_CYCLLE_TIME	0x70
#define	SHIFT_MEM_CYCLLE_TIME	4
#define	MASK_COMMON_MEM_CYCLLE_TIME	0x07
#define	MEM_CYCLLE_100NS	0x00
#define	MEM_CYCLLE_150NS	0x01
#define	MEM_CYCLLE_200NS	0x02
#define	MEM_CYCLLE_250NS	0x03
#define	MEM_CYCLLE_600NS	0x04


/* PAD_PUPD_CTRL                     0x02CE */
#define PUC1			0x80
#define PUC0			0x40
#define PDC5			0x20
#define PDC4			0x10
#define PDC3			0x08
#define PDC2			0x04
#define PDC1			0x02
#define PDC0			0x01



#define	NORMAL_CI0		0
#define	NORMAL_CI1		1
#define	DAISYCHAIN_CI0	2
#define	DAISYCHAIN_CI1	3
#define	REMUX0_CI0		4
#define	REMUX1_CI1		5
#define	REMUX1_CI0		6
#define	REMUX0_CI1		7
#define	SPECIAL_CI0		8
#define	SPECIAL_CI1		9

#define	CONNECT_BYPASS				0
#define	CONNECT_NORMAL				1
#define	CONNECT_DAISYCHAIN			2
#define	CONNECT_CUSTOM				3

#define	CI_INSERTED			1
#define	CI_EXTRACTED		0

#define	CI_INPUT_LIVE0		1
#define	CI_INPUT_LIVE1		2
#define	CI_INPUT_REMUX0		3
#define	CI_INPUT_REMUX1		4

/* RESET_CLK_SWITCH_REG1			0x02cc */
#define	SER0_CLK	0x08
#define	SER1_CLK	0x04
#define	MICLK0		0x02
#define MICLK1		0x01

/* RESET_CLK_SWITCH_REG0			0x02cb */
#define	P2S0_CLK	0x40
#define	P2S1_CLK	0x20
#define	RDF3_CLK	0x10
#define RDF4_CLK	0x08
#define	WRF3_CLK	0x04
#define WRF4_CLK	0x02
#define TSOUT_PAR_CLK	0x01


/* USB2TS_CTRL			0x0221 */
#define	USBTS2_ENABLE		0x02
#define	USBTS2_RESET		0x01
#define	SHIFT_IN_OF_BLOCK	2


/* USB2TS0_RDL			0x0222 */
/* USB2TS1_RDL			0x0223 */
#define	FIFO_RESET_VALUE	0x40


/* GAP_REMOVER_CH0_CTRL			0x0234 */
/* GAP_REMOVER_CH1_CTRL			0x0235 */
#define ENABLE		0x02
#define RESET		0x01
#define DIV_VALUE	0x09



/* Sync and Desync count */
#define SYNC_COUNT	4	/* Change desync to sync in sync retreiver */
#define DESYNC_COUNT	1	/* Change sync to desync in sync retreiver */

/* Default In Sel */
#define DEFAULT_IN_SEL_0	1	/* TSIN 0 */
#define DEFAULT_IN_SEL_1	2	/* TSIN 1 */

/* Response of interrupt signal, or command */
#define GET_CI_EXT_CTRL_RESP		1 /* a response command from CI external controller. */
#define GET_INT_PIN					0 /* a interrupt signal from CI external controller. */
#define INT_SIG						1 /* interrupt signal */

/******************************************************************************
*  Typedefs
******************************************************************************/
/**
 * @enum regOperation_t
 * @brief An enumeration to represent supported registers operations
 */


/*******************************************************************/
/* TYPE DEFINE													*/
/*******************************************************************/

/**
 * @enum camId_t
 * @brief Supported CAM slots.
 */
typedef enum
{
	/** CAM slot A. */
	CIMAX_CAM_SLOT_A_ID,
	/** CAM slot B. */
	CIMAX_CAM_SLOT_B_ID,
	/** Number of available CAM slots. */
	CIMAX_CAM_MAX_SLOT_NUM
} camId_t;

typedef enum
{
	CMD_ACCESS_REG= 0,
	CMD_NORMAL_ACTION
} CIMAX_CMD_TYPE;

typedef enum
{
	CIMAX_RESPONSE_OK = 0,
	CIMAX_RESPONSE_ERR
} CIMAX_TASK_RESPONSE;

typedef struct
{
	CIMAX_CMD_TYPE	type;
	HUINT8	cmd;
	HUINT16	addr;
	HUINT8	slot;
	HUINT8	*data;
	HUINT16	dataLen;
} CIMAX_TASK_REQUEST_MSG;

typedef struct
{
	HUINT8		cmd;
	HUINT8		slot;
	HUINT8		*data;
	HUINT16		dataLen;
	HINT32		response;
} CIMAX_TASK_RESPONSE_MSG;

typedef enum
{
	CIMAX_STATE_GETEVT = 0,
	CIMAX_STATE_GETEVT_WAIT,
	CIMAX_STATE_WAIT_READ_REG,
	CIMAX_STATE_WAIT_WRITE_REG,
	CIMAX_STATE_WAIT_NORMAL_CMD,
	CIMAX_STATE_COMPLETE,
	CIMAX_MAX_WAIT_CMD
} CIMAX_STATE_CMD;

typedef enum
{
	CI_SET_SAMEPATH = 0,
	CI_SET_NORMALPATH,
	CI_SET_DAISYCHAIN,
	CI_SET_DEFAULTPATH,
	CI_CHAINGE_NORMALPATH,
	CI_SET_SPECIALPATH,
	CI_MAX_SET
} CI_ROUTE_CMD;


typedef struct CAMINFO_T
{
	HINT32 		nDevId[CAM_MAXSLOTID];			/* device id : this value used for DI inside */
	HUINT16		usSlotId[CAM_MAXSLOTID];		/* slot id : this value used between MW and DI */
	HUINT8		ucOpen[CAM_MAXSLOTID];			/* 0:not open,	1:open */
	HUINT8		ucDetect[CAM_MAXSLOTID];		/* CAM_NO_DETECT or CAM_DETECT */
	HUINT8		ucCamDataReady[CAM_MAXSLOTID];
	HUINT8		ucIrqEnabled[CAM_MAXSLOTID];
	HUINT16		usStatus[CAM_MAXSLOTID];		/* DI_CI_STATE, event status */
}CAMINFO_t;

typedef struct DRV_CI_PathStatus
{
	HUINT32 used;
	HUINT32 connectStatus;
	HUINT32 connectWay;
	HUINT32 usedTunerId;
	HUINT32 inputId;
	HUINT32 inputSrc;
	HUINT32 slotId;
	HUINT32 outputSrc;
	HUINT32 outputId;
}DRV_CI_PATH_STATUS_t;

typedef struct DRV_CI_Entrance
{
	HUINT32 inputLogicalLimit;
	HUINT32 inputPhysicalLimit;
	HUINT32 outputLogicalLimit;
	HUINT32 outputPhysicalLimit;
}DRV_CI_ENTRANCE_INFO_t;


/*******************************************************************/
/* GLOBAL FUNCTION PROTOTYPE									*/
/*******************************************************************/

/* 이들 함수들은 DI 레벨에서만 사용되는 public 함수들이므로 MW 레벨에서는 DI_CI_xxx 들을 이용하도록 하자 */
DRV_Error		DRV_CI_Init (void);
HINT32			DRV_CI_CheckSlotId (HUINT16 usSlotId);
HUINT8			DRV_CI_GetCAMDetect (HINT32 nDevId,HUINT16  usSlotId);
HUINT16 		DRV_CI_GetCAMStatus(HINT32 nDevId);
DI_ERR_CODE		DRV_CI_Reset_CAM (HINT32 nDevId);
HINT32 			DRV_CI_ParseCis(HUINT8 *paramCIS);
DRV_Error 	DRV_CI_SWReset_CAM( HUINT8 slotId );
void DRV_CI_RegisterCallback (void (*CiCallbackFuncforDemux)(HUINT16  usSlotId, HUINT16  flag));

DRV_Error DRV_CI_InitForTest(HINT32 index);

HINT32 DRV_CIMAX_CAMReset(HUINT8 slotId);
HINT32 DRV_CIMAX_GetCIS(HUINT8 slotId, HUINT8 *cisBuff, HUINT16 *cisLen);
HINT32 DRV_CIMAX_WriteCOR(HUINT8 slotId, HUINT16 coraddr, HUINT8 cordata);
HINT32 DRV_CIMAX_DataAvailable(HUINT8  slotId, HINT32 *result);
HINT32 DRV_CIMAX_Negotiate(HUINT8 slotId, HUINT16  bufferSize, HUINT16 *negociatedbufferSize);
HINT32 DRV_CIMAX_WriteLPDU(HUINT8 slotId, HUINT8 *buffer, HUINT16 numOfBytes);
HINT32 DRV_CIMAX_ReadLPDU(HUINT8 slotId, HUINT8 *buffer, HUINT16 *numOfBytes);
HINT32 DRV_CIMAX_CAMDetected(HUINT16  slotId, HUINT32  *result);
DRV_Error DRV_CIMAX_ResetFIFO( HUINT32 slotId );
DRV_Error DRV_CIMAX_ResetFIFOBySynchro( HUINT32 slotId );

DRV_Error DRV_CI_MakePath( HUINT32 tunerId, HUINT32 inputId, HUINT32 input, HUINT32 slotId, HUINT32 output, HUINT32 channelId, HUINT32 connectWay, HUINT32 *pResetFIFO );
DRV_Error DRV_CI_GetAllCAMDetect (HUINT32 *pAllCamDetect);
DRV_Error DRV_CI_GetInputValue(HUINT32 slotId, HUINT32 *pInputValue);
DRV_Error DRV_CI_SavePathWay( HUINT32 slotId , HUINT32 savepathWay );
DRV_Error DRV_CI_GetPathStatus( HUINT32 slotId , DRV_CI_PATH_STATUS_t *pCiPathStatus );

extern void (*_pfnCiCallbackFunc)(HUINT16, HUINT16);

#endif /* !__DRV_CI_H__ */



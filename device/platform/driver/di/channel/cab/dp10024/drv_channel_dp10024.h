/* $Header:   //BASE/archives/VISU_X_C/driver/ddi_channel/Tda1002x.h-arc   1.1   Jan 29 2007 20:06:30   woobh  $ */

/********************************************************************
 * $Workfile:   Tda1002x.h  $
 * $Modtime:   Jan 29 2007 20:03:30  $
 * Project    : Common
 * Author     : B. H. Woo
 * Description:
 *
 *                              Copyright (c) 2006 by HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/

#ifndef _TDA1002x_H_
#define _TDA1002x_H_

#define DEMOD_ID_NULL						(0xFF)
#define DEMOD_ID_TDA_10021				(0x7C)
#define DEMOD_ID_TDA_10023				(0x7D)

/* demodulator types */
typedef enum tag_demodulator_type
{
	DEMODULATOR_TYPE_UNKNOWN = 0xFF,
	DEMODULATOR_TYPE_TDA10021 = 0,
	DEMODULATOR_TYPE_TDA10023
} DEMODULATOR_TYPE;

/****************************************************************************************
		TDA10021-SPECIFIC ITEMS
*****************************************************************************************/
/* TDA10021 MACRO */
/* The number of QAM lock trials */
#if defined(CONFIG_TVT_SIGNAL)
#define LOCK_TRIAL_TDA 				5	// 1->3 to support the Vt disable/enable actions
#else
#define LOCK_TRIAL_TDA 				3	// 1->3 to support the Vt disable/enable actions
#endif
//#define LOCK_TRIAL_TDA 				5	// 1->3 to support the Vt disable/enable actions

#define NUMBER_I2C_TRIAL			5     // 10 --> 5 by jino 2003/01/30

/* receiver operation options */
//	#define QAM_VARIABLEGAIN					// worse performance than fixed gain search

/* TDA QAM demodulator input frequency mode */
//#define LOW_IF_INPUT

/* set the TS output clock mode */
//#define PARALLEL_MODE_A_OUTPUT
#if 0
#ifdef emma2se_test
#define PARALLEL_MODE_A_OUTPUT
#else
#define SERIAL_OUTPUT
#endif
#endif
/* SNR value display */
#define ENABLE_SNR_PRINT
/* Gain control of the Nyquist filter and AAF */
//	#define R_FIXGAIN         		0x03	// G_NYQ=1.0, G_AAF=1.0, unlock
#define R_FIXGAIN	         		0x43	// Hv1.0, G_NYQ=2.0, G_AAF=1.0, best gain until now
#define R_AUTOGAIN	         		0x03	// VARIABLE gain search, Initial G_NYQ=1.0

/* Nyquist filter gain */
#define NYQ_AUTOGAIN_MIN			0 		// gain = 1.0
#define NYQ_AUTOGAIN_MAX			5		// gain = 6.0

#define NYQ_GAIN_DOWN				0
#define NYQ_GAIN_UP				1

/* AGC loop's time constant */
#define R_AGCONF_FAST				0x92	//RF ATC : 2, IF ATC : 2
#define R_AGCONF_SLOW				0xBF	//RF ATC : 7, IF ATC : 7

/* Timing recovery loop */
#define R_CLKCONF_WIDE				0x0A	// 480 ppm, 2nd&3rd DF gain=2 (VLSI default)

/* CARCONF = 0x12 @ VES1820X --- SR>3MHz                    *
 *         = 0x1a @ VES1820X --- SR<3MHz @ VLSI source code */
// symbol rate >= 3.0 MHz
#define R_CARCONF_HIGH_SR_ACQ 		0x5A 	// BW=0.03
#define R_CARCONF_HIGH_SR_TRK		0x52 	// BW=0.01

// 3.0 MHz > symbol rate >= 1.5 MHz
#define R_CARCONF_MID_SR_ACQ 		0x5C 	// BW=0.054. 0x0B (BW=0.034)
#define R_CARCONF_MID_SR_TRK 		0x55 	// BW=0.015. 0x06 (BW=0.017 )

// 1.5 MHz > symbol rate >= 1.0 MHz
#define R_CARCONF_LOW_SR_ACQ 		0x5D 	// BW=0.055
#define R_CARCONF_LOW_SR_TRK 		0x59 	// BW=0.028. 0x07 (BW=0.021)

/* Equalizer */
#define R_EQCONF_OFF      			0x75	// Eq. OFF, Center tap=9th tap, DFE
#define R_EQCONF_ON       			0x77	// Eq. ON, Center tap=9th tap, DFE

#define R_EQSTEP          			0x29	// 0x23 @ VES1820X
#define R_EQSTEP2          			0x12	// After QAM LOCK: 2(tracking), 2(acquisition)


/* threshold power of the central tap coefficient */
#define COEFTRESHOLD_VAL			490000	//562500

/* test register */
#define R_TEST		       			0x42	// GPIO pin acts as PIO , modified by jino, 2004/12/28, to support multi-tuner in NYMPH+
#define R_CONTROL					0x08   // Added by jino , 2004/12/28

/* FEC */
#define R_RSCONF1					0x38	// BER depth:1.25E-5
#define R_RSCONF2					0x78	// BER depth:1E-6
#define R_RSCONF3          			0xB8	// BER depth:1E-7
#define R_RSCONF4          			0xF8	// BER depth:1E-8

/* BER depth identifier */
#define BER_DEPTH5          		0x00	// 10E-5
#define BER_DEPTH6          		0x40	// 10E-6
#define BER_DEPTH7          		0x80	// 10E-7
#define BER_DEPTH8          		0xC0	// 10E-8

	// Carrier sweep convergence
//#define WAIT_CARLOCK          265533   // 2*SWDYN/SWSTEP*SWLENGTH file://org. value

//by kwonkh 20030312
#define WAIT_CARLOCK          398299  // (2*SWDYN/SWSTEP*SWLENGTH)*1.5


// Filter bank settling time after Nyquist filter gain change
#define WAIT_GNYQLOCK        		10000
// Carrier loop settling time after EQ turn-on & AGC time constant change
//#define WAIT_ALLLOCK        		200000
// FEC loop settling time after spectral inversion
#define WAIT_FRMLOCK        		30000

/* SNR estimation constants */
#define	MSE_THRESH_16QAM			195000	//195
#define	MSE_THRESH_32QAM			215000	//215
#define	MSE_THRESH_64QAM			210000	//210
#define	MSE_THRESH_128QAM			185000	//185
#define	MSE_THRESH_256QAM			180000	//180

#define	MSE_OFFSET_16QAM			108		//10.8
#define	MSE_OFFSET_32QAM			135		//13.5
#define	MSE_OFFSET_64QAM			125		//12.5
#define	MSE_OFFSET_128QAM			138		//13.8
#define	MSE_OFFSET_256QAM		    203		//20.3

#define	MSE_MUL_16QAM				32		//0.32
#define	MSE_MUL_32QAM				40		//0.4
#define	MSE_MUL_64QAM				40		//0.4
#define	MSE_MUL_128QAM				38		//0.38
#define	MSE_MUL_256QAM				100		// 1.0

#define	MSE_ADD_16QAM				138		// 1.38
#define	MSE_ADD_32QAM				500		// 5.0
#define	MSE_ADD_64QAM				500		// 5.0
#define	MSE_ADD_128QAM				400		// 4.0
#define	MSE_ADD_256QAM				40		// 0.4

/**************************************************************
 TDA10021 register identifier
***************************************************************/
#define SB_CONF		    	0x00
#define SB_AGCREF           0x01
#define SB_AGCCONF			0x02
#define SB_CLKCONF          0x03
#define SB_CARCONF          0x04
#define SB_LOCKTHR         	0x05
#define SB_EQCONF           0x06
#define SB_EQSTEP           0x07
#define SB_MSETH            0x08
#define SB_AREF            	0x09
#define SB_BDR_LSB          0x0A
#define SB_BDR_MID          0x0B
#define SB_BDR_MSB          0x0C
#define SB_BDR_INV          0x0D
#define SB_GAIN             0x0E
#define SB_TEST             0x0F
#define SB_RSCONF          	0x10
#define SB_SYNC	            0x11

#define SB_CPT_UNCOR        0x13
#define SB_BER_LSB         	0x14
#define SB_BER_MID          0x15
#define SB_BER_MSB          0x16
#define SB_VAGC             0x17
#define SB_MSE              0x18
#define SB_VAFC             0x19
#define SB_IDENTITY         0x1A
#define SB_ADC              0x1B
#define SB_EQCONF2          0x1C
#define SB_CKOFFSET         0x1D

#define SB_SATNYQ           0x21
#define SB_SATADC           0x22
#define SB_HALFADC          0x23
#define SB_SATDEC1          0x24
#define SB_SATDEC2          0x25
#define SB_SATDEC3          0x26
#define SB_SATAAF         	0x27

#define SB_SWEEP           	0x2D
#define SB_AGCCONF2         0x2E /* TDA10021 new */
#define SB_VAGC2           	0x2F /* TDA10021 new */
#define SB_HALFTHR          0x31
#define SB_ITSEL            0x32
#define SB_ITSTAT           0x33
#define SB_PWMREF           0x34
#define SB_TUNMAX          	0x35 /* TDA10021 new */
#define SB_TUNMIN          	0x36 /* TDA10021 new */
#define SB_DELTAF1        	0x37 /* TDA10021 new */
#define SB_DELTAF2        	0x38 /* TDA10021 new */
#define SB_CONSTI       	0x39 /* TDA10021 new */
#define SB_CONSTQ        	0x3A /* TDA10021 new */
#define SB_IFMAX         	0x3B /* TDA10021 new */
#define SB_IFMIN      		0x3C /* TDA10021 new */
#define SB_GAIN2			0x3D /* TDA10023 new */

/* Coeffient bit: 8 bits @VES1820x -> 11 bits @TDA10021 */
#define SB_REQCO_H0         0x40
#define SB_REQCO_H1         0x42
#define SB_REQCO_H2         0x44
#define SB_REQCO_H3         0x46
#define SB_REQCO_H4         0x48
#define SB_REQCO_H5         0x4A
#define SB_REQCO_H6         0x4C
#define SB_REQCO_H7         0x4E
#define SB_REQCO_H8         0x50
#define SB_REQCO_H9         0x52
#define SB_REQCO_H10        0x54
#define SB_REQCO_H11        0x56
#define SB_REQCO_H12        0x58
#define SB_REQCO_H13        0x5A
#define SB_REQCO_H14        0x5C
#define SB_REQCO_H15        0x5E
#define SB_REQCO_H16        0x60
#define SB_REQCO_H17        0x62
#define SB_REQCO_H18        0x64
#define SB_REQCO_H19        0x66
#define SB_REQCO_H20        0x68
#define SB_REQCO_H21        0x6A
#define SB_REQCO_H22        0x6C
#define SB_REQCO_H23        0x6E
#define SB_REQCO_H24        0x70
#define SB_REQCO_H25        0x72
#define SB_REQCO_L0         0x41
#define SB_REQCO_L1         0x43
#define SB_REQCO_L2         0x45
#define SB_REQCO_L3         0x47
#define SB_REQCO_L4         0x49
#define SB_REQCO_L5         0x4B
#define SB_REQCO_L6         0x4D
#define SB_REQCO_L7         0x4F
#define SB_REQCO_L8         0x51
#define SB_REQCO_L9         0x53
#define SB_REQCO_L10        0x55
#define SB_REQCO_L11        0x57
#define SB_REQCO_L12        0x59
#define SB_REQCO_L13        0x5B
#define SB_REQCO_L14        0x5D
#define SB_REQCO_L15        0x5F
#define SB_REQCO_L16        0x61
#define SB_REQCO_L17        0x63
#define SB_REQCO_L18        0x65
#define SB_REQCO_L19        0x67
#define SB_REQCO_L20        0x69
#define SB_REQCO_L21        0x6B
#define SB_REQCO_L22        0x6D
#define SB_REQCO_L23        0x6F
#define SB_REQCO_L24        0x71
#define SB_REQCO_L25        0x73

#define SB_IEQCO_H0         0x80
#define SB_IEQCO_H1         0x82
#define SB_IEQCO_H2         0x84
#define SB_IEQCO_H3         0x86
#define SB_IEQCO_H4         0x88
#define SB_IEQCO_H5         0x8A
#define SB_IEQCO_H6         0x8C
#define SB_IEQCO_H7         0x8E
#define SB_IEQCO_H8         0x90
#define SB_IEQCO_H9         0x92
#define SB_IEQCO_H10        0x94
#define SB_IEQCO_H11        0x96
#define SB_IEQCO_H12        0x98
#define SB_IEQCO_H13        0x9A
#define SB_IEQCO_H14        0x9C
#define SB_IEQCO_H15        0x9E
#define SB_IEQCO_H16        0xA0
#define SB_IEQCO_H17        0xA2
#define SB_IEQCO_H18        0xA4
#define SB_IEQCO_H19        0xA6
#define SB_IEQCO_H20        0xA8
#define SB_IEQCO_H21        0xAA
#define SB_IEQCO_H22        0xAC
#define SB_IEQCO_H23        0xAE
#define SB_IEQCO_H24        0xB0
#define SB_IEQCO_H25        0xB2
#define SB_IEQCO_L0         0x81
#define SB_IEQCO_L1         0x83
#define SB_IEQCO_L2         0x85
#define SB_IEQCO_L3         0x87
#define SB_IEQCO_L4         0x89
#define SB_IEQCO_L5         0x8B
#define SB_IEQCO_L6         0x8D
#define SB_IEQCO_L7         0x8F
#define SB_IEQCO_L8         0x91
#define SB_IEQCO_L9         0x93
#define SB_IEQCO_L10        0x95
#define SB_IEQCO_L11        0x97
#define SB_IEQCO_L12        0x99
#define SB_IEQCO_L13        0x9B
#define SB_IEQCO_L14        0x9D
#define SB_IEQCO_L15        0x9F
#define SB_IEQCO_L16        0xA1
#define SB_IEQCO_L17        0xA3
#define SB_IEQCO_L18        0xA5
#define SB_IEQCO_L19        0xA7
#define SB_IEQCO_L20        0xA9
#define SB_IEQCO_L21        0xAB
#define SB_IEQCO_L22        0xAD
#define SB_IEQCO_L23        0xAF
#define SB_IEQCO_L24        0xB1
#define SB_IEQCO_L25        0xB3
#define SB_AGCREFNYQ 		0xB4
#define SB_ERAGC_THD	    0xB5
#define SB_ERAGCNYQ_THD 	0xB6
#define SB_INVQ_AGC	    	0xBE

#define TDA10024_CPT_TSP_UNCOR1_IND     0x74
//#define TDA10024_CPT_TSP_COR1_IND        0x78
#define TDA10024_CPT_TSP_OK1_IND        0x7C

#if 0
/******** TDA10021 QAM receiver-specific functions */
/* tuner-related functions */
void enable_tuner_iic(unsigned long unitId);
void disable_tuner_iic(unsigned long unitId);

/* QAM demoduator-related functions */
unsigned char fast_lock_TDA(unsigned long unitId);
void SetQAMdemod(unsigned long unitId);
void AdjustCarrierRecovBW(unsigned long unitId);
unsigned char AdjustGain(unsigned long unitId);
unsigned char calc_no_decimation(unsigned long unitId);
unsigned char set_DecimateFilt(unsigned long unitId);
void set_baud_rate(unsigned long unitId, unsigned char ndec);
unsigned short pow_positive(unsigned short mantissa, unsigned char exponent);
void select_AntialiasFilt(unsigned long unitId);
void set_TS_clock_rate(unsigned long unitId);
unsigned short no_to_msec(unsigned long unitId, unsigned long symbols);
unsigned char check_agc_lock(unsigned long unitId);
unsigned char check_fec_lock_TDA(unsigned long unitId);
unsigned char HandleSpectralInversion_TDA(unsigned long unitId);
unsigned char SNR_Info_TDA(unsigned long unitId);
unsigned long measure_BER_TDA(unsigned long unitId);
long measure_carrier_offset(unsigned long unitId);
unsigned long measure_SNR_TDA( unsigned long unitId );
unsigned char BER_Info_TDA(unsigned long unitId);
unsigned char BER_Info_PostRSD(unsigned long unitId);
void gain_updown_NYQfilt(unsigned long unitId, unsigned char updown);
void get_loop_inform_TDA(unsigned long unitId);
short get_carrier_offset(unsigned long unitId);
short get_carrier_offset_new(unsigned long unitId);
short get_clock_offset(unsigned long unitId);
void AdaptBerWindow(unsigned long unitId);
#endif

#endif // _TDA1002x_H_

#ifndef _TDA10021_H_
#define _TDA10021_H_

#define TDA_10021_R_AGCONF_FAST				0x01	// KAGC = 1 @ initial phase
#define TDA_10021_R_AGCONF_SLOW				0x02	// KAGC = 2 @ final phase
//#define TDA_10021_R_AGCONF_SLOW				0x03	// Hv1.0, KAGC = 3 after lock

/* CARCONF = 0x12 @ VES1820X --- SR>3MHz                    *
 *         = 0x1a @ VES1820X --- SR<3MHz @ VLSI source code */
// symbol rate >= 3.0 MHz
#define TDA_10021_R_CARCONF_HIGH_SR_ACQ 	0x0A 	// BW=0.03
#define TDA_10021_R_CARCONF_HIGH_SR_TRK		0x02 	// BW=0.01

// 3.0 MHz > symbol rate >= 1.5 MHz
#define TDA_10021_R_CARCONF_MID_SR_ACQ 		0x0C 	// BW=0.054. 0x0B (BW=0.034)
#define TDA_10021_R_CARCONF_MID_SR_TRK 		0x05 	// BW=0.015. 0x06 (BW=0.017 )

// 1.5 MHz > symbol rate >= 1.0 MHz
#define TDA_10021_R_CARCONF_LOW_SR_ACQ 		0x0D 	// BW=0.055
#define TDA_10021_R_CARCONF_LOW_SR_TRK 		0x09 	// BW=0.028. 0x07 (BW=0.021)

/* Compare the results of followed two values */
//	#define TDA_10021_R_EQCONF2ND       			0x30	// Hv1.0, CTADAPT=0, Signed Algorithm. Tap leakage inhibited
#define TDA_10021_R_EQCONF2ND       			0x32	// VES1820x. Tap leakage allowed => Not bad

/* the number of symbols for the receiver lock */
// AGC loop settling time after reset
#define TDA_10021_WAIT_AGCLOCK        		100000	// 80000 @ v1.01

#define TDA_10021_SB_POLA             0x12
#define TDA_10021_SB_INTP1         	0x20

#define TDA_10021_SB_MDIV           	0x28 /* TDA10021 new */
#define TDA_10021_SB_NDIV          	0x29 /* TDA10021 new */
#define TDA_10021_SB_PLL            	0x2A /* TDA10021 new */
#define TDA_10021_SB_POLA2           	0x2B /* TDA10021 new */

#define TDA_10021_SB_CONTROL      	0x2C /* TDA10021 new */

#ifdef XTAL_4MHZ
#define TDA_10021_SYSCLK						58000	// M,N,P = 0x1c, 0,
#define TDA_10021_IFFREQ						36150	// IF freq=36.15 MHz MHz
#else // 28.92 MHz crystal usage
#define TDA_10021_SYSCLK						57840	// SACLK=28.92=4*7.23 MHz=1.6*IF
#define TDA_10021_IFFREQ						36150	// IF freq=36.15 MHz MHz
#endif
#endif // _TDA10021_H_

#ifndef _TDA10023_H_
#define _TDA10023_H_

#define XTAL_16MHZ

#define NYQ_ROLLOFF					0x2  	/* roll-off is 0.15 justin */
#define NYQ_GAAF					0x0  	// for low symbol rate it should be 0x00

#define TDA_10023_R_AGCONF_FAST		0x89	// KAGC = 1 @ initial phase
#define TDA_10023_R_AGCONF_SLOW				0x9b	// KAGC = 2 @ final phase
//#define TDA_10021_R_AGCONF_SLOW				0x03	// Hv1.0, KAGC = 3 after lock

/* CARCONF = 0x12 @ VES1820X --- SR>3MHz                    *
 *         = 0x1a @ VES1820X --- SR<3MHz @ VLSI source code */
// symbol rate >= 3.0 MHz
#define TDA_10023_R_CARCONF_HIGH_SR_ACQ 		0x5A 	// BW=0.03
#define TDA_10023_R_CARCONF_HIGH_SR_TRK		0x52 	// BW=0.01

// 3.0 MHz > symbol rate >= 1.5 MHz
#define TDA_10023_R_CARCONF_MID_SR_ACQ 		0x5C 	// BW=0.054. 0x0B (BW=0.034)
#define TDA_10023_R_CARCONF_MID_SR_TRK 		0x55 	// BW=0.015. 0x06 (BW=0.017 )

// 1.5 MHz > symbol rate >= 1.0 MHz
#define TDA_10023_R_CARCONF_LOW_SR_ACQ 		0x5D 	// BW=0.055
#define TDA_10023_R_CARCONF_LOW_SR_TRK 		0x59 	// BW=0.028. 0x07 (BW=0.021)

/* Compare the results of followed two values */
//	#define TDA_10023_R_EQCONF2ND       			0x30	// Hv1.0, CTADAPT=0, Signed Algorithm. Tap leakage inhibited
#define TDA_10023_R_EQCONF2ND       			0xB0    // 0x32	// VES1820x. Tap leakage allowed => Not bad
#define TDA_10023_R_INVQ_AGC       			0xBE    // 0x05 (INVQ_SYMB_NB) 0x0 (CPTLCK_CVAGC) PE_AGC_INTEG_NB (0x03)

/* the number of symbols for the receiver lock */
// AGC loop settling time after reset
// #define TDA_10023_WAIT_AGCLOCK        		250000	// 80000 @ v1.01
#define TDA_10023_WAIT_AGCLOCK        		400000	// 80000 @ v1.01

#define TDA_10023_SB_INTP1             		0x12
#define TDA_10023_SB_CONTROL					0x1E
#define TDA_10023_SB_RESET					0x1F
#define TDA_10023_SB_INTP2         			0x20

#define TDA_10023_SB_PLL1           			0x28 /* TDA10023 new */
#define TDA_10023_SB_PLL2          			0x29 /* TDA10023 new */
#define TDA_10023_SB_PLL3            			0x2A /* TDA10023 new */
#define TDA_10023_SB_INTS1           			0x2B /* TDA10023 new */
#define TDA_10023_SB_INTPS	      			0x2C /* TDA10023 new */

#define TDA_10023_SB_GAIN2					0x3D /* TDA10023 new */

#ifdef XTAL_16MHZ
#define TDA_10023_SYSCLK						58666	// 57808	// M,N,P = 0x1c, 0,
#define TDA_10023_IFFREQ						36130	// IF freq=36.15 MHz MHz
#else // 28.92 MHz crystal usage
#define TDA_10023_SYSCLK						57840	// SACLK=28.92=4*7.23 MHz=1.6*IF
#define TDA_10023_IFFREQ						36150	// IF freq=36.15 MHz MHz
#endif


/**********************************************************************
 Common functions
***********************************************************************/
/* Common functions which are required at all of the channel devices */

int DP10024_GpioReset(int nUnitId);
int DP10024_DumpRegister( int nChannelId);
 void DP10024_ClearErrCount ( int nUnitId );
int DP10024_GetStrengthAgc( int nChannelId, HINT32 *pnStrength, HUINT8 *pucAgc, HUINT8 *pucRfAgc, HUINT8 *pucIfAgc);
int DP10024_GetQualityBer( int nChannelId, HULONG *pulQuality, HULONG *pulBer,  HULONG *pulSnr, HULONG *pulUncorrected );
#if defined(PLATFORM_TN182I2)
void drv_ch_i2c_TUNERDump(int nUnitId);
#endif

/* This API will be deprecated */
int DP10024_LockInfo( int nChannelId, int *snr, int *ber_decimal, int *ber_float, int *ber_exp, int *uncorrected, int *signallevel);

#endif // _TDA10023_H_

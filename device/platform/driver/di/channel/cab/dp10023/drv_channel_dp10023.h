/************************************************************************
 *
 *	Ch-statel.h 
 *
 *	Copyright (c) 1998 by Humax	Corp.
 *
************************************************************************/
/*************************************************************************
1. QAM demodulator:
  
2. QAM mode:
	1: 16-QAM
	2: 32-QAM
	3: 64-QAM
	4: 128-QAM
	5: 256-QAM
	
3. SNR factor:
	0      : SNR <QAM_LOCK_THRESHOLD or
			 SNR > UNREALISTIC_SNR
	1 - 255: QAM_LOCK_THRESHOLD <= SNR < QAM_LOCK_THRESHOLD + 16.0
	256    : QAM_LOCK_THRESHOLD + 16.0 <= SNR < UNREALISTIC_SNR
	
4. Post BER factor: 
Post FEC BER is BER after Reed-Solomon decoding
 	O: FEC unlock
	1: 1.0e-3 <  post FEC BER
	2: 1.0e-4 <  post FEC BER <= 1.0e-3
	3: 1.0e-5 <  post FEC BER <= 1.0e-4
	4: 1.0e-6 <  post FEC BER <= 1.0e-5
	5: 1.0e-7 <  post FEC BER <= 1.0e-6
	6: 1.0e-8 <  post FEC BER <= 1.0e-7
	7: 1.0e-9 <  post FEC BER <= 1.0e-8
	8: 1.0e-10 < post FEC BER <= 1.0e-9
	9: 1.0e-11 < post FEC BER <= 1.0e-10
   10: 1.0e-11 > post FEC BER 

* Quasi Error Free BER = 2.0e-4 after Viterbi decoding
* Quasi Error Free BER = 1.0e-11 after Reed-Solomon decoding

*************************************************************************/
#ifndef TDA10023_H
#define TDA10023_H


/**********************************************************************
 Common identifier definition and structure
***********************************************************************/

#define	BUS_NUM						NUM_OF_CHANNEL
#define GET_DATA_INTERVAL			500 // Timer interval


/****************************************************************************************
		TDA10021-SPECIFIC ITEMS
*****************************************************************************************/
/* TDA10021 MACRO */
	/* The number of QAM lock trials */
	#define LOCK_TRIAL_TDA 				3   // justin 20050524	
	
	/* receiver operation options */
	/*	#define QAM_VARIABLEGAIN			// worse performance than fixed gain search*/

#define	JC5000N   // JC5000N 보드에 적용됨.   
#if defined(CONFIG_PLATFORM_TM201COMBO)
#define IFFREQ						6000	// 6000	JC8500BR /* 6MHz */
#else
#define IFFREQ						44000	 //44000	 kansai /* 44 MHz*/
#endif
#define SYSCLK						70400	// 57600   //70400 // justin	/* SYSCLK = 1.6 * IF = 1.6 * 44  , M=44, N=5, P=2 */	

	/* Gain control of the Nyquist filter and AAF */
	/*	#define R_FIXGAIN	         		0x03*/	/* G_NYQ=1.0, G_AAF=1.0, unlock*/
	#define R_FIXGAIN	         		0x43	/* Hv1.0, G_NYQ=2.0, G_AAF=1.0, best gain until now*/
	#define R_AUTOGAIN	         		0x03	/* VARIABLE gain search, Initial G_NYQ=1.0*/

	#define INIT_GAIN				0xA6
	  
	/* Nyquist filter gain */
	#define NYQ_AUTOGAIN_MIN			0 		/* gain = 1.0*/
	#define NYQ_AUTOGAIN_MAX			5		/* gain = 6.0*/
	
	#define NYQ_GAIN_DOWN				0
	#define NYQ_GAIN_UP				1

	#define NYQ_ROLLOFF					0x3 // 0x03 // roll-off is 0.13 for jqam 040329 justin 0x02   /* roll-off is 0.15 justin */
	#define NYQ_GAAF					0x0  // kansai=0x1   /* GAAF gain is 1  justin */
	
	/* AGC loop's time constant */
	#define R_AGCONF_FAST				0x89 // 0x9b // justin_3 0x93 // justin 0x01	/* KAGC = 1 @ initial phase*/
	#define R_AGCONF_SLOW				0x9b // justin_1 0x82 // justin 0x02	/* KAGC = 2 @ final phase*/
	/*#define R_AGCONF_SLOW				0x03	// Hv1.0, KAGC = 3 after lock*/
	
	/* Timing recovery loop */
	#define R_CLKCONF_WIDE				0x1A // justin 0x0A	/* 480 ppm, 2nd&3rd DF gain=2 (VLSI default)*/
	
	#define R_CARCONF_HIGH_SR_ACQ 	0x72  // justin 	0x0A 	/* BW=0.03*/
	#define R_CARCONF_HIGH_SR_TRK		0x72 // justin_1 0x02 	/* BW=0.01*/
	
	/* 3.0 MHz > symbol rate >= 1.5 MHz*/
	#define R_CARCONF_MID_SR_ACQ 		0x4C 	/* BW=0.054. 0x0B (BW=0.034)*/
	#define R_CARCONF_MID_SR_TRK 		0x45 	/* BW=0.015. 0x06 (BW=0.017 )*/
	
	/* 1.5 MHz > symbol rate >= 1.0 MHz*/
	#define R_CARCONF_LOW_SR_ACQ 		0x4D 	/* BW=0.055*/
	#define R_CARCONF_LOW_SR_TRK 		0x49 	/* BW=0.028. 0x07 (BW=0.021)*/
	
	/* Equalizer */
	#define R_EQCONF_OFF      			0x75	/* Eq. OFF, Center tap=9th tap, DFE*/
	#define R_EQCONF_ON       			0x77	/* Eq. ON, Center tap=9th tap, DFE*/
	
	#define R_EQSTEP          			0x1A	/* 0x23 @ VES1820X*/
	#define R_EQSTEP2          			0x12	/* After QAM LOCK: 2(tracking), 2(acquisition)*/
	
	/* Compare the results of followed two values */
	/*	#define R_EQCONF2ND       			0x30	// Hv1.0, CTADAPT=0, Signed Algorithm. Tap leakage inhibited*/
	#define R_EQCONF2ND       			0xB0 // justin 0x72 // justin 0x32	/* VES1820x. Tap leakage allowed => Not bad*/
	
	/* threshold power of the central tap coefficient */
	#define COEFTRESHOLD_VAL			490000	/*562500*/
	
	/* test register */
	#define R_TEST		       			0x40 // justin_4 0x41	/* GPIO pin is interrupt line*/
	
	/* FEC */
	#define R_RSCONF1					0x38	/* BER depth:1.25E-5*/
	#define R_RSCONF2					0x78	/* BER depth:1E-6*/
	#define R_RSCONF3          			 0xB8	/* BER depth:1E-7*/
	#define R_RSCONF4          			 0xF8	/* BER depth:1E-8*/
	
	/* BER depth identifier */
	#define BER_DEPTH5          		0x00	/* 10E-5*/
	#define BER_DEPTH6          		0x40	/* 10E-6*/
	#define BER_DEPTH7          		0x80	/* 10E-7*/
	#define BER_DEPTH8          		0xC0	/* 10E-8*/
	
	/* the number of symbols for the receiver lock */
	// AGC loop settling time after reset
	#define WAIT_AGCLOCK        		(4*100000)	// 80000 @ v1.01
	// Carrier sweep convergence

	#define WAIT_CARLOCK        		(4*262144)	// 2*SWDYN/SWSTEP*SWLENGTH = 2*2340/290*16384 = 262144

	// Filter bank settling time after Nyquist filter gain change
	#define WAIT_GNYQLOCK        		(4*10000)
	// Carrier loop settling time after EQ turn-on & AGC time constant change	
	#define WAIT_ALLLOCK        		(4*200000)
	// FEC loop settling time after spectral inversion	
	#define WAIT_FRMLOCK        		(4*30000)
	
	/* qamMode estimation constants */
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
	#define	MSE_MUL_256QAM				100		//1.0
	                                	
	#define	MSE_ADD_16QAM				138		//1.38
	#define	MSE_ADD_32QAM				500		//5.0
	#define	MSE_ADD_64QAM				500		//5.0
	#define	MSE_ADD_128QAM				400		//4.0
	#define	MSE_ADD_256QAM				40		//0.4
	
	/**************************************************************
	 TDA10021 register identifier
	***************************************************************/
	#define SB_CONF		    	0x00
	#define SB_AGCREF           0x01
	#define SB_AGCCONF		0x02
	#define SB_CLKCONF          0x03
	#define SB_CARCONF         0x04
	#define SB_LOCKTHR         	0x05
	#define SB_EQCONF           0x06
	#define SB_EQSTEP           	0x07
	#define SB_MSETH            	0x08
	#define SB_AREF            	0x09
	#define SB_BDR_LSB          0x0A
	#define SB_BDR_MID          0x0B
	#define SB_BDR_MSB         0x0C
	#define SB_BDR_INV          0x0D
	#define SB_GAIN             	0x0E
	#define SB_TEST             	0x0F
	#define SB_RSCONF          	0x10
	#define SB_SYNC	            	0x11
	#define SB_INTP1			0x12  	// justin #define SB_POLA             0x12
	#define SB_CPT_UNCOR     0x13
	#define SB_BER_LSB         	0x14
	#define SB_BER_MID          0x15
	#define SB_BER_MSB          0x16
	#define SB_VAGC             	0x17
	#define SB_MSE              	0x18
	#define SB_VAFC             	0x19
	#define SB_IDENTITY         0x1A
	#define SB_ADC              	0x1B
	#define SB_EQCONF2         0x1C
	#define SB_CKOFFSET        0x1D
	#define SB_RESET		0x1F  /* TDA10023, justin */
	#define SB_INTP2			0x20		// justin #define SB_INTP1         	0x20
	#define SB_SATNYQ           0x21
	#define SB_SATADC           0x22
	#define SB_HALFADC          0x23
	#define SB_SATDEC1          0x24
	#define SB_SATDEC2          0x25
	#define SB_SATDEC3          0x26
	#define SB_SATAAF         	0x27
	#define SB_MDIV           	0x28
	#define SB_NDIV          	0x29          
	#define SB_PLL            	0x2A          
	#define SB_INTS1           	0x2B 
	#define SB_CONTROL      	0x2C /* TDA10021 new */
	#define SB_SWEEP           	0x2D /* TDA10021 new */
	#define SB_AGCCONF2       0x2E /* TDA10021 new */
	#define SB_VAGC2           	0x2F /* TDA10021 new */
	#define SB_HALFTHR          0x31
	#define SB_ITSEL            	0x32
	#define SB_ITSEL2		0x33 // justin SB_ITSTAT           0x33
	#define SB_PWMREF           0x34
	#define SB_TUNMAX          	0x35 /* TDA10021 new */
	#define SB_TUNMIN          	0x36 /* TDA10021 new */
	#define SB_DELTAF1        	0x37 /* TDA10021 new */
	#define SB_DELTAF2        	0x38 /* TDA10021 new */
	#define SB_CONSTI       	0x39 /* TDA10021 new */             
	#define SB_CONSTQ        	0x3A /* TDA10021 new */             
	#define SB_IFMAX         	0x3B /* TDA10021 new */
	#define SB_IFMIN      		0x3C /* TDA10021 new */
	#define SB_GAIN2		0x3D /* TDA10023 new */
	#define SB_ITSTAT1		0x3E /* TDA10023 new */
	#define SB_ITSTAT2		0x3F /* TDA10023 new */
	
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
	#define SB_REQCO_H10       0x54                               
	#define SB_REQCO_H11       0x56                               
	#define SB_REQCO_H12       0x58                               
	#define SB_REQCO_H13       0x5A                               
	#define SB_REQCO_H14       0x5C                               
	#define SB_REQCO_H15       0x5E                               
	#define SB_REQCO_H16       0x60                                
	#define SB_REQCO_H17       0x62                                
	#define SB_REQCO_H18       0x64                                 
	#define SB_REQCO_H19       0x66                                
	#define SB_REQCO_H20       0x68                                
	#define SB_REQCO_H21       0x6A                               
	#define SB_REQCO_H22       0x6C               
	#define SB_REQCO_H23       0x6E               
	#define SB_REQCO_H24       0x70 
	#define SB_REQCO_H25       0x72 
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

	#define CPT_TSP_UNCOR1     0x74
	#define CPT_TSP_UNCOR2     0x75
	#define CPT_TSP_UNCOR3     0x76
	#define CPT_TSP_UNCOR4     0x77
	#define CPT_TSP_COR1      0x78
	#define CPT_TSP_COR2	0x79
	#define CPT_TSP_COR3      0x7A
	#define CPT_TSP_COR4      0x7B
	#define CPT_TSP_OK1        0x7C
	#define CPT_TSP_OK2        0x7D
	#define CPT_TSP_OK3        0x7E
	#define CPT_TSP_OK4        0x7F

	
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
	#define SB_IEQCO_L0          0x81 
	#define SB_IEQCO_L1          0x83 
	#define SB_IEQCO_L2          0x85 
	#define SB_IEQCO_L3          0x87 
	#define SB_IEQCO_L4          0x89 
	#define SB_IEQCO_L5          0x8B 
	#define SB_IEQCO_L6          0x8D 
	#define SB_IEQCO_L7          0x8F 
	#define SB_IEQCO_L8          0x91 
	#define SB_IEQCO_L9          0x93 
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


//---------------------
//  JQAM FILTER INDEX  
//---------------------
	#define TDA10023_JQAM_MODE_IND	0x00
	#define TDA10023_REQ_TS_IDH_IND	0x01
	#define TDA10023_REQ_TS_IDL_IND     0x02
	#define TDA10023_REQ_ON_IDH_IND    0x03
	#define TDA10023_REQ_ON_IDL_IND    	0x04
	#define TDA10023_REG5_IND           	0x05
	#define TDA10023_TS_STATUS_H_IND  0x06
	#define TDA10023_TS_STATUS_L_IND  0x07
	#define TDA10023_RCV_STATUS4_IND  0x08
	#define TDA10023_RCV_STATUS3_IND	0x09
	#define TDA10023_RCV_STATUS2_IND  0x0A
	#define TDA10023_RCV_STATUS1_IND	0x0B
	#define TDA10023_TS_ID1H_IND		0x0C
	#define TDA10023_TS_ID1L_IND        	0x0D
	#define TDA10023_ON_ID1H_IND		0x0E
	#define TDA10023_ON_ID1L_IND	       0x0F
	#define TDA10023_TS_ID2H_IND    	0x10
	#define TDA10023_TS_ID2L_IND        	0x11
	#define TDA10023_ON_ID2H_IND        	0x12
	#define TDA10023_ON_ID2L_IND        	0x13
	#define TDA10023_TS_ID3H_IND        	0x14
	#define TDA10023_TS_ID3L_IND        	0x15
	#define TDA10023_ON_ID3H_IND        	0x16
	#define TDA10023_ON_ID3L_IND        	0x17
	#define TDA10023_TS_ID4H_IND        	0x18
	#define TDA10023_TS_ID4L_IND        	0x19
	#define TDA10023_ON_ID4H_IND        	0x1A
	#define TDA10023_ON_ID4L_IND        	0x1B
	#define TDA10023_TS_ID5H_IND        	0x1C
	#define TDA10023_TS_ID5L_IND        	0x1D
	#define TDA10023_ON_ID5H_IND        	0x1E
	#define TDA10023_ON_ID5L_IND        	0x1F
	#define TDA10023_TS_ID6H_IND        	0x20
	#define TDA10023_TS_ID6L_IND        	0x21
	#define TDA10023_ON_ID6H_IND        	0x22
	#define TDA10023_ON_ID6L_IND        	0x23
	#define TDA10023_TS_ID7H_IND        	0x24
	#define TDA10023_TS_ID7L_IND        	0x25
	#define TDA10023_ON_ID7H_IND        	0x26
	#define TDA10023_ON_ID7L_IND        	0x27
	#define TDA10023_TS_ID8H_IND        	0x28
	#define TDA10023_TS_ID8L_IND        	0x29
	#define TDA10023_ON_ID8H_IND        	0x2A
	#define TDA10023_ON_ID8L_IND        	0x2B
	#define TDA10023_TS_ID9H_IND        	0x2C
	#define TDA10023_TS_ID9L_IND        	0x2D
	#define TDA10023_ON_ID9H_IND        	0x2E
	#define TDA10023_ON_ID9L_IND        	0x2F
	#define TDA10023_TS_ID10H_IND       	0x30
	#define TDA10023_TS_ID10L_IND       	0x31
	#define TDA10023_ON_ID10H_IND       	0x32
	#define TDA10023_ON_ID10L_IND       	0x33
	#define TDA10023_TS_ID11H_IND       	0x34
	#define TDA10023_TS_ID11L_IND       	0x35
	#define TDA10023_ON_ID11H_IND       	0x36
	#define TDA10023_ON_ID11L_IND       	0x37
	#define TDA10023_TS_ID12H_IND       	0x38
	#define TDA10023_TS_ID12L_IND       	0x39
	#define TDA10023_ON_ID12H_IND       	0x3A
	#define TDA10023_ON_ID12L_IND       	0x3B
	#define TDA10023_TS_ID13H_IND       	0x3C
	#define TDA10023_TS_ID13L_IND       	0x3D
	#define TDA10023_ON_ID13H_IND       	0x3E
	#define TDA10023_ON_ID13L_IND       	0x3F
	#define TDA10023_TS_ID14H_IND       	0x40
	#define TDA10023_TS_ID14L_IND       	0x41
	#define TDA10023_ON_ID14H_IND       	0x42
	#define TDA10023_ON_ID14L_IND       	0x43
	#define TDA10023_TS_ID15H_IND       	0x44
	#define TDA10023_TS_ID15L_IND       	0x45
	#define TDA10023_ON_ID15H_IND       	0x46
	#define TDA10023_ON_ID15L_IND       	0x47


// JQAM setting  
	#define JQAM_DISABLE	0x00
	#define JQAM_TSMF		0x01
	#define JQAM_AUTO		0x02

/* TDA10023 Core Registers */
#define	TDA10023_GPR						0x00
#define	TDA10023_AGCREF					0x01
#define	TDA10023_AGCCONF1				0x02
#define	TDA10023_CLKCONF					0x03
#define	TDA10023_CARCONF					0x04
#define	TDA10023_LOCKTHR					0x05
#define	TDA10023_EQCONF1					0x06
#define	TDA10023_EQSTEP					0x07
#define	TDA10023_MSETH					0x08
#define	TDA10023_AREF						0x09
#define	TDA10023_BDR_LSB					0x0A
#define	TDA10023_BDR_MID					0x0B
#define	TDA10023_BDR_MSB					0x0C
#define	TDA10023_BDR_INV					0x0D
#define	TDA10023_GAIN1					0x0E
#define	TDA10023_TEST						0x0F
#define	TDA10023_FECDVBCFG1				0x10
#define	TDA10023_STATUS					0x11
#define	TDA10023_INTP1					0x12
#define	TDA10023_FECDVBCFG2				0x13
#define	TDA10023_BER_LSB					0x14
#define	TDA10023_BER_MID					0x15
#define	TDA10023_BER_MSB					0x16
#define	TDA10023_AGCTUN					0x17
#define	TDA10023_MSE						0x18
#define	TDA10023_AFC						0x19
#define	TDA10023_IDENTITY					0x1A
#define	TDA10023_ADC						0x1B
#define	TDA10023_EQCONF2					0x1C
#define	TDA10023_CKOFF					0x1D
#define	TDA10023_CONTROL					0x1E
#define	TDA10023_RESET					0x1F
#define	TDA10023_INTP2					0x20
#define	TDA10023_SATNYQ					0x21
#define	TDA10023_SATADC					0x22
#define	TDA10023_HALFADC					0x23
#define	TDA10023_SATDEC1					0x24
#define	TDA10023_SATDEC2					0x25
#define	TDA10023_SATDEC3					0x26
#define	TDA10023_SATAAF					0x27
#define	TDA10023_PLL1						0x28
#define	TDA10023_PLL2						0x29
#define	TDA10023_PLL3						0x2A
#define	TDA10023_INTS1					0x2B
#define	TDA10023_INTP_S					0x2C
#define	TDA10023_SWEEP					0x2D
#define	TDA10023_AGCCONF2				0x2E
#define	TDA10023_AGCIF					0x2F
#define	TDA10023_SATTHR					0x30
#define	TDA10023_HALFTHR					0x31
#define	TDA10023_ITSEL1					0x32
#define	TDA10023_ITSEL2					0x33
#define	TDA10023_PWMREF					0x34
#define	TDA10023_TUNMAX					0x35
#define	TDA10023_TUNMIN					0x36
#define	TDA10023_DELTAF_LSB				0x37
#define	TDA10023_DELTAF_MSB				0x38
#define	TDA10023_CONSTI					0x39
#define	TDA10023_CONSTQ					0x3A
#define	TDA10023_IFMAX					0x3B
#define	TDA10023_IFMIN					0x3C
#define	TDA10023_GAIN2					0x3D
#define	TDA10023_ITSTAT1					0x3E
#define	TDA10023_ITSTAT2					0x3F
#define	TDA10023_REQCO					0x40	/* 0x40 ~ 0x73 : 26 equalizer coefficients(real part) */
#define	TDA10023_CPT_TSP_UNCOR1			0x74
#define	TDA10023_CPT_TSP_UNCOR2			0x75
#define	TDA10023_CPT_TSP_UNCOR3			0x76
#define	TDA10023_CPT_TSP_UNCOR4			0x77
#define	TDA10023_CPT_TSP_COR1			0x78
#define	TDA10023_CPT_TSP_COR2			0x79
#define	TDA10023_CPT_TSP_COR3			0x7A
#define	TDA10023_CPT_TSP_COR4			0x7B
#define	TDA10023_CPT_TSP_OK1				0x7C
#define	TDA10023_CPT_TSP_OK2				0x7D
#define	TDA10023_CPT_TSP_OK3				0x7E
#define	TDA10023_CPT_TSP_OK4				0x7F
#define	TDA10023_IEQCO					0x80	/* 0x80 ~ 0xB3 : 26 equalizer coefficients (imaginary part) */
#define	TDA10023_AGCREFNYQ				0xB4
#define	TDA10023_ERAGC_THD				0xB5
#define	TDA10023_ERAGCNYQ_THD			0xB6
#define	TDA10023_SCIN_THDL				0xB7
#define	TDA10023_SCIN_THDH				0xB8
#define	TDA10023_SCIN_CPTL				0xB9
#define	TDA10023_SCIN_CPTH				0xBA
#define	TDA10023_SCIN_CPT					0xBB
#define	TDA10023_SCIN_NBTSAT				0xBC
#define	TDA10023_SATDEC0					0xBD
#define	TDA10023_INVQ_AGC				0xBE
#define	TDA10023_BW_AGC					0xBF
#define	TDA10023_XTAL_PLL4				0xC0
#define	TDA10023_PLL5						0xC1
#define	TDA10023_CLBSTIM_I2CSWTCH		0xC2
#define	TDA10023_TIMING_SCAN				0xC3
#define	TDA10023_TIMING_PPM				0xC4
#define	TDA10023_TIMING_PARA1			0xC5
#define	TDA10023_TIMING_PARA2			0xC6
#define	TDA10023_TIMING_STATUS			0xC7
#define	TDA10023_CPT_TRANS				0xC8
#define	TDA10023_PRCENTAGE				0xC9
#define	TDA10023_ERTIM_THD				0xCA
#define	TDA10023_DSP						0xCB
#define	TDA10023_POWER1					0xCC
#define	TDA10023_POWER2					0xCD
#define	TDA10023_POWER3					0xCE
#define	TDA10023_CTRL1					0xD0
#define	TDA10023_CTRL2					0xD1
#define	TDA10023_TRELDAT					0xD2
#define	TDA10023_TRELCOR					0xD3
#define	TDA10023_FECSYNC					0xD4
#define	TDA10023_DRNDSD1					0xD5
#define	TDA10023_DRNDSD2					0xD6
#define	TDA10023_DRNDSD3					0xD7
#define	TDA10023_CPT_RSB_UNCOR1			0xD8
#define	TDA10023_CPT_RSB_UNCOR2			0xD9
#define	TDA10023_CPT_RSB_UNCOR3			0xDA
#define	TDA10023_CPT_RSB_UNCOR4			0xDB
#define	TDA10023_FECRSYNC					0xDC
#define	TDA10023_DEINTRLV					0xDD
#define	TDA10023_MCNS_STATUS				0xE0
#define	TDA10023_VITERBI_LOCK				0xE1
#define	TDA10023_RSUNCORLO				0xE2
#define	TDA10023_RSUNCORHI				0xE3
#define	TDA10023_RSBERLO					0xE4
#define	TDA10023_RSBERHI					0xE5
#define	TDA10023_RCFG						0xE6
#define	TDA10023_CPT_RSB_COR1			0xE7
#define	TDA10023_CPT_RSB_COR2			0xE8
#define	TDA10023_CPT_RSB_COR3			0xE9
#define	TDA10023_CPT_RSB_COR4			0xEA
#define	TDA10023_CPT_RSB_OK1				0xEB
#define	TDA10023_CPT_RSB_OK2				0xEC
#define	TDA10023_CPT_RSB_OK3				0xED
#define	TDA10023_CPT_RSB_OK4				0xEE

/* TDA10023 Jqam Registers */
#define	TDA10023_JQAM_MODE				0x00
#define	TDA10023_REQ_TS_IDH 				0x01
#define	TDA10023_REQ_TS_IDL				0x02
#define	TDA10023_REQ_ON_IDH				0x03
#define	TDA10023_REQ_ON_IDL				0x04
#define	TDA10023_REG5_IND					0x05
#define	TDA10023_TS_STATUS_H  			0x06
#define	TDA10023_TS_STATUS_L  			0x07
#define	TDA10023_RCV_STATUS4  			0x08
#define	TDA10023_RCV_STATUS3				0x09
#define	TDA10023_RCV_STATUS2  			0x0A
#define	TDA10023_RCV_STATUS1				0x0B
#define	TDA10023_TS_ID1H					0x0C
#define	TDA10023_TS_ID1L					0x0D
#define	TDA10023_ON_ID1H					0x0E
#define	TDA10023_ON_ID1L		   			0x0F
#define	TDA10023_TS_ID2H					0x10
#define	TDA10023_TS_ID2L					0x11
#define	TDA10023_ON_ID2H					0x12
#define	TDA10023_ON_ID2L					0x13
#define	TDA10023_TS_ID3H					0x14
#define	TDA10023_TS_ID3L					0x15
#define	TDA10023_ON_ID3H					0x16
#define	TDA10023_ON_ID3L					0x17
#define	TDA10023_TS_ID4H					0x18
#define	TDA10023_TS_ID4L					0x19
#define	TDA10023_ON_ID4H					0x1A
#define	TDA10023_ON_ID4L					0x1B
#define	TDA10023_TS_ID5H					0x1C
#define	TDA10023_TS_ID5L					0x1D
#define	TDA10023_ON_ID5H					0x1E
#define	TDA10023_ON_ID5L					0x1F
#define	TDA10023_TS_ID6H					0x20
#define	TDA10023_TS_ID6L					0x21
#define	TDA10023_ON_ID6H					0x22
#define	TDA10023_ON_ID6L					0x23
#define	TDA10023_TS_ID7H					0x24
#define	TDA10023_TS_ID7L					0x25
#define	TDA10023_ON_ID7H					0x26
#define	TDA10023_ON_ID7L					0x27
#define	TDA10023_TS_ID8H					0x28
#define	TDA10023_TS_ID8L					0x29
#define	TDA10023_ON_ID8H					0x2A
#define	TDA10023_ON_ID8L					0x2B
#define	TDA10023_TS_ID9H					0x2C
#define	TDA10023_TS_ID9L					0x2D
#define	TDA10023_ON_ID9H					0x2E
#define	TDA10023_ON_ID9L					0x2F
#define	TDA10023_TS_ID10H					0x30
#define	TDA10023_TS_ID10L					0x31
#define	TDA10023_ON_ID10H				0x32
#define	TDA10023_ON_ID10L					0x33
#define	TDA10023_TS_ID11H					0x34
#define	TDA10023_TS_ID11L					0x35
#define	TDA10023_ON_ID11H				0x36
#define	TDA10023_ON_ID11L					0x37
#define	TDA10023_TS_ID12H					0x38
#define	TDA10023_TS_ID12L					0x39
#define	TDA10023_ON_ID12H				0x3A
#define	TDA10023_ON_ID12L					0x3B
#define	TDA10023_TS_ID13H					0x3C
#define	TDA10023_TS_ID13L					0x3D
#define	TDA10023_ON_ID13H				0x3E
#define	TDA10023_ON_ID13L					0x3F
#define	TDA10023_TS_ID14H					0x40
#define	TDA10023_TS_ID14L					0x41
#define	TDA10023_ON_ID14H				0x42
#define	TDA10023_ON_ID14L					0x43
#define	TDA10023_TS_ID15H					0x44
#define	TDA10023_TS_ID15L					0x45
#define	TDA10023_ON_ID15H				0x46
#define	TDA10023_ON_ID15L					0x47


#define TDA10023_DEMOD_ADDRESS		0x0c//0x18
#define TDA10023_JQAM_ADDRESS			0x0e//0x1C

#define I2C_ADDRESS_TUNER		0xC0   
#define S22CX_TUNER_ADDRESS	0x60 //0xC0

#if defined (CONFIG_PLATFORM_TM201COMBO)					// JD - Add Definition related Demod I2C Addr
#define MASTER_CAB_DEMOD			1					// Unit ID와 비교하여 Master / Slave를 구분하기 위함
#define SLAVE_CAB_DEMOD				3
#define MASTER_DEMOD_ADDR_TDA10023	0x0C				// JD - 0x18 (Channel ID = 0)
#define	MASTER_JQAM_ADDR_TDA10023	0x0E				// JD - 0x1C
#define SLAVE_DEMOD_ADDR_TDA10023	0x0D				// JD - 0x1A (Channel ID = 2)
#define SLAVE_JQAM_ADDR_TDA10023	0x0F				// JD - 0x1E
#endif


typedef enum
{ /* Channel service message to application */
	CH_LOCKED = 1, //MSG_CH_START,
	CH_UNLOCKED,
	CH_STOPPED,
	TSMF_EWS_ON,
	TSMF_EWS_OFF,
	TSMF_GOOD,
	TSMF_BAD,
	TSMF_NONE
}TDA10023_Tsmf_t;

typedef	struct
{
	HUINT32		frequency;
	HINT32		netId;
	HINT32		tsId;

	HINT16		strength;
	HUINT32		quality;
	HUINT16		snr;
	
	HUINT8		qamMode;
	HUINT16		symbolRate;   // justin

	HUINT8		SpectralInverse; // justin
	HUINT8		emergencyFlag;
	HUINT8		tsmfStatus;
	HUINT8		currentlock;  // 포탈에서  rf 케이블 뽑았을때와 레벨이 실제로 0일경우를 분류하기 위해서 추가함  
} TDA10023_Status_t;

typedef struct
{
	HUINT16	tsNo;
	HINT32	tsID[15];
	HINT32	netID[15];
} TDA10023_TsmfInfo_t;

typedef struct
{
	HUINT32	UnErroredNo[3];
	HUINT32	CorrectedNo[3];
	HUINT32	UnCorrectedNo[3];
	HUINT32	InbandBer[3];
	HUINT32	UnCorrectedFlag[3];
} TDA10023_CorrectionInfo_t;




/**********************************************************************
 Common functions
***********************************************************************/
/* Common functions which are required at all of the channel devices */
int DP10023_Initialize(int nChannelId);
int DP10023_InitDevice( int nChannelId );
int DP10023_SetTune( int nChannelId, HUINT32 ulWorkingId, CH_CAB_TuneParam_t *pstSatTuneParam );
int DP10023_GetStatus(int nChannelId, HUINT32 frequency);

HBOOL DP10023_CheckLock( int nChannelId );
int DP10023_GetStrengthQuality( int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo);
int DP10023_SetPowerState( int nChannelId, CH_Power_e etPower );
int DP10023_EnableTsOutput( int nChannelId );
int DP10023_DisableTsOutput( int nChannelId );
int DP10023_UpdateTunedParam( int nChannelId, CH_CAB_TuneParam_t * pstTuneParam );
int DP10023_ResetGpio(int nChannelId);
void DP10023_DumpRegister( int nChannelId);

void DP10023_GetAGC(int nChannelId, HUINT8 *ucpRFAgc, HUINT8 *ucpIFAgc);
void DP10023_GetEwsInfo(int nChannelId, HUINT8 *ucIsEws);
int DP10023_CheckTsmfLock(int nChannelId, HUINT32 frequency);
void DP10023_TsmfData(int nChannelId, HUINT16 *tsNo, HINT32 *tsID, HINT32 *netID);
void DP10023_QAMStop(int nChannelId);

#endif


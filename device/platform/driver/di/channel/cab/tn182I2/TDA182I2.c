/*************************************************************************************
 *
 *	TDA182I2.c
 *
 *  QAM receiver : 10024(NXP)
 *  Tuner : FJ2207(NuTune)
 *
 *************************************************************************************/
#include "htype.h"
#include"di_channel.h"
#include"di_channel_priv.h"
#include "drv_channel.h"
#include "TDA182I2.h"
#include "TDA182I2Instance.h"
#include "vkernel.h"
#include "drv_i2c.h"


/********************************************************************
	 Print Option
 ********************************************************************/
//#include "ddi_err.h"

#define CH_PRINT				// All Print Enable /Disable
#define CH_PRINT_ERROR			// Error Print Enable / Disable
//#define CH_PRINT_WARNING
//#define CH_PRINT_DEBUG
//#define CH_PRINT_DATA

//#include "ch_print.h"

#define MASTER_TUNER	0
#define SLAVE_TUNER		1
#define M_TUNER_I2C_ADDR	0x60		//0xC0
#define S_TUNER_I2C_ADDR	0x63		//0xC6

extern HINT32 show_di_ch;

TDA182I2_ERR_CODE TDA182I2SetPowerState( HUINT32 tUnit, TDA182I2PowerState_t powerState);
TDA182I2_ERR_CODE TDA182I2GetIRQ_status( HUINT32 tUnit, HUINT8* puValue);

/* driver 1.3.1 */
// cal number / freq for index = 0 / freq for index = 1 / freq for index = 2 / freq for index = 3 / cprog_delta / cprog_offset / cprog_offset_0 / cprog_offset_1 / cprog_offset_2 / cprog_offset_3
static tmTDA182I2_CAL_Map_t TDA182I2_Cal_Map[TDA182I2_RFCAL_PROG_ROW] =
{
    { 0, { 52224000, 56320000, 44032000, 48128000 }, { 15, 0, 10, 17 } },
    { 1, { 93184000, 97280000, 84992000, 89088000 }, { 1, 0, -2, 3 } },
    { 2, { 115712000, 123904000, 106496000, 111616000 }, { 0, -1, 1, 2 } },
    { 3, { 169984000, 174080000, 161792000, 165888000 }, { 3, 0, 1, 2 } },
    { 4, { 232448000, 235520000, 224256000, 228352000 }, { 3, 0, 1, 2 } },
    { 5, { 322560000, 335872000, 301056000, 312320000 }, { 0, -1, 1, 2 } },
    { 6, { 397312000, 401408000, 389120000, 393216000 }, { -2, 0, -1, 1 } },
    { 7, { 465920000, 471040000, 455680000, 460800000 }, { 0, -2, -3, 1 } },
    { 8, { 570368000, 577536000, 555008000, 563200000 }, { -1, 0, -3, -2 } },
    { 9, { 658432000, 662528000, 647168000, 652288000 }, { -6, -3, 0, -5 } },
    { 10, { 762880000, 770048000, 748544000, 755712000 }, { -6, -3, 0, -5 } },
    { 11, { 809984000, 818176000, 792576000, 801792000 }, { -5, -2, 0, -4 } }
};

static tmTDA182I2_Freq_ROM_Map_t TDA182I2_Freq_ROM_Map[TDA182I2_FREQ_ROM_MAP_ROW] =
{
    { 39936000, 0xFF, 0x17, 0 },
    { 40960000, 0xFD, 0x17, 0 },
    { 41984000, 0xF1, 0x15, 0 },
    { 43008000, 0xE5, 0x13, 0 },
    { 44032000, 0xDB, 0x13, 0 },
    { 45056000, 0xD1, 0x12, 0 },
    { 46080000, 0xC7, 0x10, 0 },
    { 47104000, 0xBE, 0x0F, 0 },
    { 48128000, 0xB5, 0x0F, 0 },
    { 49152000, 0xAD, 0x0F, 0 },
    { 50176000, 0xA6, 0x0F, 0 },
    { 51200000, 0x9F, 0x0F, 0 },
    { 52224000, 0x98, 0x0F, 0 },
    { 53248000, 0x91, 0x0F, 0 },
    { 54272000, 0x8B, 0x0F, 0 },
    { 55296000, 0x86, 0x0F, 0 },
    { 56320000, 0x80, 0x0F, 0 },
    { 57344000, 0x7B, 0x0E, 0 },
    { 58368000, 0x76, 0x0E, 0 },
    { 59392000, 0x72, 0x0D, 0 },
    { 60416000, 0x6D, 0x0D, 0 },
    { 61440000, 0x69, 0x0C, 0 },
    { 62464000, 0x65, 0x0C, 0 },
    { 63488000, 0x61, 0x0B, 0 },
    { 64512000, 0x5E, 0x0B, 0 },
    { 64512000, 0x5A, 0x0B, 0 },
    { 65536000, 0x57, 0x0A, 0 },
    { 66560000, 0x54, 0x0A, 0 },
    { 67584000, 0x51, 0x09, 0 },
    { 68608000, 0x4E, 0x09, 0 },
    { 69632000, 0x4B, 0x09, 0 },
    { 70656000, 0x49, 0x08, 0 },
    { 71680000, 0x46, 0x08, 0 },
    { 72704000, 0x44, 0x08, 0 },
    { 73728000, 0x41, 0x07, 0 },
    { 74752000, 0x3F, 0x07, 0 },
    { 75776000, 0x3D, 0x07, 0 },
    { 76800000, 0x3B, 0x07, 0 },
    { 77824000, 0x39, 0x07, 0 },
    { 78848000, 0x37, 0x07, 0 },
    { 79872000, 0x35, 0x07, 0 },
    { 80896000, 0x33, 0x07, 0 },
    { 81920000, 0x32, 0x07, 0 },
    { 82944000, 0x30, 0x07, 0 },
    { 83968000, 0x2F, 0x07, 0 },
    { 84992000, 0x2D, 0x07, 0 },
    { 86016000, 0x2C, 0x07, 0 },
    { 87040000, 0x2A, 0x07, 0 },
    { 88064000, 0x29, 0x06, 0 },
    { 89088000, 0x27, 0x06, 0 },
    { 90112000, 0x26, 0x06, 0 },
    { 91136000, 0x25, 0x06, 0 },
    { 92160000, 0x24, 0x06, 0 },
    { 93184000, 0x22, 0x05, 0 },
    { 94208000, 0x21, 0x05, 0 },
    { 95232000, 0x20, 0x05, 0 },
    { 96256000, 0x1F, 0x05, 0 },
    { 97280000, 0x1E, 0x05, 0 },
    { 98304000, 0x1D, 0x05, 0 },
    { 99328000, 0x1C, 0x04, 0 },
    { 100352000, 0x1B, 0x04, 0 },
    { 101376000, 0x1A, 0x04, 0 },
    { 103424000, 0x19, 0x04, 0 },
    { 104448000, 0x18, 0x04, 0 },
    { 105472000, 0x17, 0x04, 0 },
    { 106496000, 0x16, 0x03, 0 },
    { 106496000, 0x15, 0x03, 0 },
    { 108544000, 0x14, 0x03, 0 },
    { 109568000, 0x13, 0x03, 0 },
    { 111616000, 0x12, 0x03, 0 },
    { 112640000, 0x11, 0x03, 0 },
    { 113664000, 0x11, 0x07, 0 },
    { 114688000, 0x10, 0x07, 0 },
    { 115712000, 0x0F, 0x07, 0 },
    { 117760000, 0x0E, 0x07, 0 },
    { 119808000, 0x0D, 0x06, 0 },
    { 121856000, 0x0C, 0x06, 0 },
    { 123904000, 0x0B, 0x06, 0 },
    { 125952000, 0x0A, 0x05, 0 },
    { 128000000, 0x09, 0x05, 0 },
    { 130048000, 0x08, 0x05, 0 },
    { 133120000, 0x07, 0x04, 0 },
    { 135168000, 0x06, 0x04, 0 },
    { 138240000, 0x05, 0x04, 0 },
    { 141312000, 0x04, 0x04, 0 },
    { 144384000, 0x03, 0x03, 0 },
    { 145408000, 0xE0, 0x3F, 1 },
    { 147456000, 0xDC, 0x37, 1 },
    { 148480000, 0xD9, 0x32, 1 },
    { 149504000, 0xD6, 0x2F, 1 },
    { 149504000, 0xD2, 0x2F, 1 },
    { 150528000, 0xCF, 0x2F, 1 },
    { 151552000, 0xCC, 0x2B, 1 },
    { 152576000, 0xC9, 0x27, 1 },
    { 153600000, 0xC5, 0x27, 1 },
    { 154624000, 0xC2, 0x25, 1 },
    { 155648000, 0xBF, 0x23, 1 },
    { 156672000, 0xBD, 0x20, 1 },
    { 157696000, 0xBA, 0x1F, 1 },
    { 158720000, 0xB7, 0x1F, 1 },
    { 159744000, 0xB4, 0x1F, 1 },
    { 160768000, 0xB1, 0x1F, 1 },
    { 161792000, 0xAF, 0x1F, 1 },
    { 162816000, 0xAC, 0x1F, 1 },
    { 163840000, 0xAA, 0x1F, 1 },
    { 164864000, 0xA7, 0x1F, 1 },
    { 165888000, 0xA5, 0x1F, 1 },
    { 166912000, 0xA2, 0x1F, 1 },
    { 167936000, 0xA0, 0x1F, 1 },
    { 168960000, 0x9D, 0x1F, 1 },
    { 169984000, 0x9B, 0x1F, 1 },
    { 171008000, 0x99, 0x1F, 1 },
    { 172032000, 0x97, 0x1E, 1 },
    { 173056000, 0x95, 0x1D, 1 },
    { 174080000, 0x92, 0x1C, 1 },
    { 175104000, 0x90, 0x1B, 1 },
    { 176128000, 0x8E, 0x1A, 1 },
    { 177152000, 0x8C, 0x19, 1 },
    { 178176000, 0x8A, 0x18, 1 },
    { 179200000, 0x88, 0x17, 1 },
    { 180224000, 0x86, 0x17, 1 },
    { 181248000, 0x84, 0x17, 1 },
    { 182272000, 0x82, 0x17, 1 },
    { 183296000, 0x81, 0x17, 1 },
    { 184320000, 0x7F, 0x17, 1 },
    { 185344000, 0x7D, 0x16, 1 },
    { 186368000, 0x7B, 0x15, 1 },
    { 187392000, 0x7A, 0x14, 1 },
    { 188416000, 0x78, 0x14, 1 },
    { 189440000, 0x76, 0x13, 1 },
    { 190464000, 0x75, 0x13, 1 },
    { 191488000, 0x73, 0x13, 1 },
    { 192512000, 0x71, 0x12, 1 },
    { 192512000, 0x70, 0x11, 1 },
    { 193536000, 0x6E, 0x11, 1 },
    { 194560000, 0x6D, 0x10, 1 },
    { 195584000, 0x6B, 0x10, 1 },
    { 196608000, 0x6A, 0x0F, 1 },
    { 197632000, 0x68, 0x0F, 1 },
    { 198656000, 0x67, 0x0F, 1 },
    { 199680000, 0x65, 0x0F, 1 },
    { 200704000, 0x64, 0x0F, 1 },
    { 201728000, 0x63, 0x0F, 1 },
    { 202752000, 0x61, 0x0F, 1 },
    { 203776000, 0x60, 0x0F, 1 },
    { 204800000, 0x5F, 0x0F, 1 },
    { 205824000, 0x5D, 0x0F, 1 },
    { 206848000, 0x5C, 0x0F, 1 },
    { 207872000, 0x5B, 0x0F, 1 },
    { 208896000, 0x5A, 0x0F, 1 },
    { 209920000, 0x58, 0x0F, 1 },
    { 210944000, 0x57, 0x0F, 1 },
    { 211968000, 0x56, 0x0F, 1 },
    { 212992000, 0x55, 0x0F, 1 },
    { 214016000, 0x54, 0x0F, 1 },
    { 215040000, 0x53, 0x0F, 1 },
    { 216064000, 0x52, 0x0F, 1 },
    { 217088000, 0x50, 0x0F, 1 },
    { 218112000, 0x4F, 0x0F, 1 },
    { 219136000, 0x4E, 0x0F, 1 },
    { 220160000, 0x4D, 0x0E, 1 },
    { 221184000, 0x4C, 0x0E, 1 },
    { 222208000, 0x4B, 0x0E, 1 },
    { 223232000, 0x4A, 0x0E, 1 },
    { 224256000, 0x49, 0x0D, 1 },
    { 225280000, 0x48, 0x0D, 1 },
    { 226304000, 0x47, 0x0D, 1 },
    { 227328000, 0x46, 0x0D, 1 },
    { 228352000, 0x45, 0x0C, 1 },
    { 229376000, 0x44, 0x0C, 1 },
    { 230400000, 0x43, 0x0C, 1 },
    { 231424000, 0x42, 0x0C, 1 },
    { 232448000, 0x42, 0x0B, 1 },
    { 233472000, 0x41, 0x0B, 1 },
    { 234496000, 0x40, 0x0B, 1 },
    { 234496000, 0x3F, 0x0B, 1 },
    { 235520000, 0x3E, 0x0B, 1 },
    { 236544000, 0x3D, 0x0B, 1 },
    { 237568000, 0x3C, 0x0B, 1 },
    { 239616000, 0x3B, 0x0A, 1 },
    { 240640000, 0x3A, 0x0A, 1 },
    { 241664000, 0x39, 0x0A, 1 },
    { 242688000, 0x38, 0x0A, 1 },
    { 244736000, 0x37, 0x09, 1 },
    { 245760000, 0x36, 0x09, 1 },
    { 246784000, 0x35, 0x09, 1 },
    { 248832000, 0x34, 0x09, 1 },
    { 249856000, 0x33, 0x09, 1 },
    { 250880000, 0x32, 0x08, 1 },
    { 252928000, 0x31, 0x08, 1 },
    { 253952000, 0x30, 0x08, 1 },
    { 256000000, 0x2F, 0x08, 1 },
    { 257024000, 0x2E, 0x08, 1 },
    { 259072000, 0x2D, 0x07, 1 },
    { 260096000, 0x2C, 0x07, 1 },
    { 262144000, 0x2B, 0x07, 1 },
    { 264192000, 0x2A, 0x07, 1 },
    { 265216000, 0x29, 0x07, 1 },
    { 267264000, 0x28, 0x07, 1 },
    { 269312000, 0x27, 0x07, 1 },
    { 270336000, 0x26, 0x07, 1 },
    { 272384000, 0x25, 0x07, 1 },
    { 274432000, 0x24, 0x07, 1 },
    { 276480000, 0x23, 0x07, 1 },
    { 277504000, 0x22, 0x07, 1 },
    { 279552000, 0x21, 0x07, 1 },
    { 281600000, 0x20, 0x07, 1 },
    { 283648000, 0x1F, 0x07, 1 },
    { 285696000, 0x1E, 0x0F, 1 },
    { 287744000, 0x1D, 0x0F, 1 },
    { 289792000, 0x1C, 0x0E, 1 },
    { 291840000, 0x1B, 0x0E, 1 },
    { 293888000, 0x1A, 0x0D, 1 },
    { 296960000, 0x19, 0x0D, 1 },
    { 299008000, 0x18, 0x0C, 1 },
    { 301056000, 0x17, 0x0C, 1 },
    { 304128000, 0x16, 0x0C, 1 },
    { 306176000, 0x15, 0x0B, 1 },
    { 309248000, 0x14, 0x0B, 1 },
    { 312320000, 0x13, 0x0B, 1 },
    { 314368000, 0x12, 0x0B, 1 },
    { 317440000, 0x11, 0x0A, 1 },
    { 320512000, 0x10, 0x0A, 1 },
    { 322560000, 0x0F, 0x0A, 1 },
    { 325632000, 0x0E, 0x09, 1 },
    { 328704000, 0x0D, 0x09, 1 },
    { 331776000, 0x0C, 0x08, 1 },
    { 335872000, 0x0B, 0x08, 1 },
    { 338944000, 0x0A, 0x08, 1 },
    { 343040000, 0x09, 0x07, 1 },
    { 346112000, 0x08, 0x07, 1 },
    { 350208000, 0x07, 0x07, 1 },
    { 354304000, 0x06, 0x07, 1 },
    { 358400000, 0x05, 0x07, 1 },
    { 362496000, 0x04, 0x07, 1 },
    { 365568000, 0x04, 0x07, 1 },
    { 367616000, 0xDA, 0x2A, 2 },
    { 367616000, 0xD9, 0x27, 2 },
    { 368640000, 0xD8, 0x27, 2 },
    { 369664000, 0xD6, 0x27, 2 },
    { 370688000, 0xD5, 0x27, 2 },
    { 371712000, 0xD3, 0x25, 2 },
    { 372736000, 0xD2, 0x23, 2 },
    { 373760000, 0xD0, 0x23, 2 },
    { 374784000, 0xCF, 0x21, 2 },
    { 375808000, 0xCD, 0x1F, 2 },
    { 376832000, 0xCC, 0x1F, 2 },
    { 377856000, 0xCA, 0x1F, 2 },
    { 378880000, 0xC9, 0x1F, 2 },
    { 379904000, 0xC7, 0x1F, 2 },
    { 380928000, 0xC6, 0x1F, 2 },
    { 381952000, 0xC4, 0x1F, 2 },
    { 382976000, 0xC3, 0x1F, 2 },
    { 384000000, 0xC1, 0x1F, 2 },
    { 385024000, 0xC0, 0x1F, 2 },
    { 386048000, 0xBF, 0x1F, 2 },
    { 387072000, 0xBD, 0x1F, 2 },
    { 388096000, 0xBC, 0x1F, 2 },
    { 389120000, 0xBB, 0x1F, 2 },
    { 390144000, 0xB9, 0x1F, 2 },
    { 391168000, 0xB8, 0x1F, 2 },
    { 392192000, 0xB7, 0x1F, 2 },
    { 393216000, 0xB5, 0x1F, 2 },
    { 394240000, 0xB4, 0x1F, 2 },
    { 395264000, 0xB3, 0x1F, 2 },
    { 396288000, 0xB1, 0x1F, 2 },
    { 397312000, 0xB0, 0x1F, 2 },
    { 398336000, 0xAF, 0x1F, 2 },
    { 399360000, 0xAD, 0x1F, 2 },
    { 400384000, 0xAC, 0x1F, 2 },
    { 401408000, 0xAB, 0x1F, 2 },
    { 402432000, 0xAA, 0x1F, 2 },
    { 403456000, 0xA8, 0x1E, 2 },
    { 404480000, 0xA7, 0x1D, 2 },
    { 405504000, 0xA6, 0x1D, 2 },
    { 405504000, 0xA5, 0x1C, 2 },
    { 406528000, 0xA3, 0x1C, 2 },
    { 407552000, 0xA2, 0x1B, 2 },
    { 408576000, 0xA1, 0x1B, 2 },
    { 409600000, 0xA0, 0x1B, 2 },
    { 410624000, 0x9F, 0x1A, 2 },
    { 411648000, 0x9D, 0x1A, 2 },
    { 412672000, 0x9C, 0x19, 2 },
    { 413696000, 0x9B, 0x18, 2 },
    { 414720000, 0x9A, 0x18, 2 },
    { 415744000, 0x99, 0x17, 2 },
    { 416768000, 0x98, 0x17, 2 },
    { 417792000, 0x97, 0x17, 2 },
    { 418816000, 0x95, 0x17, 2 },
    { 419840000, 0x94, 0x17, 2 },
    { 420864000, 0x93, 0x17, 2 },
    { 421888000, 0x92, 0x17, 2 },
    { 422912000, 0x91, 0x17, 2 },
    { 423936000, 0x90, 0x17, 2 },
    { 424960000, 0x8F, 0x17, 2 },
    { 425984000, 0x8E, 0x16, 2 },
    { 427008000, 0x8D, 0x16, 2 },
    { 428032000, 0x8C, 0x15, 2 },
    { 429056000, 0x8B, 0x15, 2 },
    { 430080000, 0x8A, 0x15, 2 },
    { 431104000, 0x88, 0x14, 2 },
    { 432128000, 0x87, 0x14, 2 },
    { 433152000, 0x86, 0x14, 2 },
    { 434176000, 0x85, 0x13, 2 },
    { 435200000, 0x84, 0x13, 2 },
    { 436224000, 0x83, 0x13, 2 },
    { 437248000, 0x82, 0x13, 2 },
    { 438272000, 0x81, 0x13, 2 },
    { 439296000, 0x80, 0x12, 2 },
    { 440320000, 0x7F, 0x12, 2 },
    { 441344000, 0x7E, 0x12, 2 },
    { 442368000, 0x7D, 0x11, 2 },
    { 444416000, 0x7C, 0x11, 2 },
    { 445440000, 0x7B, 0x10, 2 },
    { 446464000, 0x7A, 0x10, 2 },
    { 447488000, 0x79, 0x10, 2 },
    { 448512000, 0x78, 0x10, 2 },
    { 448512000, 0x77, 0x0F, 2 },
    { 449536000, 0x76, 0x0F, 2 },
    { 450560000, 0x75, 0x0F, 2 },
    { 451584000, 0x74, 0x0F, 2 },
    { 452608000, 0x73, 0x0F, 2 },
    { 453632000, 0x72, 0x0F, 2 },
    { 454656000, 0x71, 0x0F, 2 },
    { 455680000, 0x70, 0x0F, 2 },
    { 457728000, 0x6F, 0x0F, 2 },
    { 458752000, 0x6E, 0x0F, 2 },
    { 459776000, 0x6D, 0x0F, 2 },
    { 460800000, 0x6C, 0x0F, 2 },
    { 461824000, 0x6B, 0x0F, 2 },
    { 462848000, 0x6A, 0x0F, 2 },
    { 464896000, 0x69, 0x0F, 2 },
    { 465920000, 0x68, 0x0F, 2 },
    { 466944000, 0x67, 0x0F, 2 },
    { 467968000, 0x66, 0x0F, 2 },
    { 468992000, 0x65, 0x0F, 2 },
    { 471040000, 0x64, 0x0F, 2 },
    { 472064000, 0x63, 0x0F, 2 },
    { 473088000, 0x62, 0x0F, 2 },
    { 474112000, 0x61, 0x0F, 2 },
    { 476160000, 0x60, 0x0F, 2 },
    { 477184000, 0x5F, 0x0F, 2 },
    { 478208000, 0x5E, 0x0F, 2 },
    { 479232000, 0x5D, 0x0F, 2 },
    { 481280000, 0x5C, 0x0F, 2 },
    { 482304000, 0x5B, 0x0F, 2 },
    { 483328000, 0x5A, 0x0F, 2 },
    { 485376000, 0x59, 0x0F, 2 },
    { 486400000, 0x58, 0x0F, 2 },
    { 487424000, 0x57, 0x0F, 2 },
    { 489472000, 0x56, 0x0F, 2 },
    { 490496000, 0x55, 0x0F, 2 },
    { 490496000, 0x54, 0x0F, 2 },
    { 492544000, 0x53, 0x0E, 2 },
    { 493568000, 0x52, 0x0E, 2 },
    { 495616000, 0x51, 0x0E, 2 },
    { 496640000, 0x50, 0x0E, 2 },
    { 497664000, 0x4F, 0x0E, 2 },
    { 499712000, 0x4E, 0x0D, 2 },
    { 500736000, 0x4D, 0x0D, 2 },
    { 502784000, 0x4C, 0x0D, 2 },
    { 503808000, 0x4B, 0x0D, 2 },
    { 505856000, 0x4A, 0x0C, 2 },
    { 506880000, 0x49, 0x0C, 2 },
    { 508928000, 0x48, 0x0C, 2 },
    { 509952000, 0x47, 0x0C, 2 },
    { 512000000, 0x46, 0x0C, 2 },
    { 513024000, 0x45, 0x0B, 2 },
    { 515072000, 0x44, 0x0B, 2 },
    { 517120000, 0x43, 0x0B, 2 },
    { 518144000, 0x42, 0x0B, 2 },
    { 520192000, 0x41, 0x0B, 2 },
    { 521216000, 0x40, 0x0B, 2 },
    { 523264000, 0x3F, 0x0B, 2 },
    { 525312000, 0x3E, 0x0B, 2 },
    { 526336000, 0x3D, 0x0B, 2 },
    { 528384000, 0x3C, 0x0A, 2 },
    { 530432000, 0x3B, 0x0A, 2 },
    { 531456000, 0x3A, 0x0A, 2 },
    { 533504000, 0x39, 0x0A, 2 },
    { 534528000, 0x38, 0x0A, 2 },
    { 536576000, 0x37, 0x0A, 2 },
    { 537600000, 0x36, 0x09, 2 },
    { 539648000, 0x35, 0x09, 2 },
    { 541696000, 0x34, 0x09, 2 },
    { 543744000, 0x33, 0x09, 2 },
    { 544768000, 0x32, 0x09, 2 },
    { 546816000, 0x31, 0x09, 2 },
    { 548864000, 0x30, 0x08, 2 },
    { 550912000, 0x2F, 0x08, 2 },
    { 552960000, 0x2E, 0x08, 2 },
    { 555008000, 0x2D, 0x08, 2 },
    { 557056000, 0x2C, 0x08, 2 },
    { 559104000, 0x2B, 0x08, 2 },
    { 561152000, 0x2A, 0x07, 2 },
    { 563200000, 0x29, 0x07, 2 },
    { 565248000, 0x28, 0x07, 2 },
    { 567296000, 0x27, 0x07, 2 },
    { 569344000, 0x26, 0x07, 2 },
    { 570368000, 0x26, 0x07, 2 },
    { 571392000, 0x25, 0x07, 2 },
    { 573440000, 0x24, 0x07, 2 },
    { 575488000, 0x23, 0x07, 2 },
    { 577536000, 0x22, 0x0F, 2 },
    { 578560000, 0x21, 0x0F, 2 },
    { 580608000, 0x20, 0x0F, 2 },
    { 583680000, 0x1F, 0x0F, 2 },
    { 585728000, 0x1E, 0x0F, 2 },
    { 587776000, 0x1D, 0x0F, 2 },
    { 589824000, 0x1C, 0x0F, 2 },
    { 592896000, 0x1B, 0x0F, 2 },
    { 594944000, 0x1A, 0x0F, 2 },
    { 596992000, 0x19, 0x0F, 2 },
    { 600064000, 0x18, 0x0F, 2 },
    { 602112000, 0x17, 0x0F, 2 },
    { 604160000, 0x16, 0x0F, 2 },
    { 607232000, 0x15, 0x0F, 2 },
    { 609280000, 0x14, 0x0F, 2 },
    { 612352000, 0x13, 0x0F, 2 },
    { 615424000, 0x12, 0x0F, 2 },
    { 617472000, 0x11, 0x0F, 2 },
    { 619520000, 0x10, 0x0E, 2 },
    { 621568000, 0x0F, 0x0E, 2 },
    { 623616000, 0x0F, 0x0E, 2 },
    { 624640000, 0xA3, 0x1F, 3 },
    { 625664000, 0xA2, 0x1F, 3 },
    { 626688000, 0xA1, 0x1F, 3 },
    { 627712000, 0xA0, 0x1F, 3 },
    { 628736000, 0x9F, 0x1F, 3 },
    { 630784000, 0x9E, 0x1F, 3 },
    { 631808000, 0x9D, 0x1F, 3 },
    { 632832000, 0x9C, 0x1F, 3 },
    { 633856000, 0x9B, 0x1F, 3 },
    { 635904000, 0x9A, 0x1F, 3 },
    { 636928000, 0x99, 0x1F, 3 },
    { 637952000, 0x98, 0x1F, 3 },
    { 638976000, 0x97, 0x1F, 3 },
    { 641024000, 0x96, 0x1E, 3 },
    { 642048000, 0x95, 0x1E, 3 },
    { 643072000, 0x94, 0x1E, 3 },
    { 644096000, 0x93, 0x1D, 3 },
    { 646144000, 0x92, 0x1D, 3 },
    { 647168000, 0x91, 0x1C, 3 },
    { 648192000, 0x90, 0x1C, 3 },
    { 650240000, 0x8F, 0x1B, 3 },
    { 651264000, 0x8E, 0x1B, 3 },
    { 652288000, 0x8D, 0x1B, 3 },
    { 654336000, 0x8C, 0x1B, 3 },
    { 655360000, 0x8B, 0x1B, 3 },
    { 656384000, 0x8A, 0x1B, 3 },
    { 658432000, 0x89, 0x1A, 3 },
    { 659456000, 0x88, 0x1A, 3 },
    { 660480000, 0x87, 0x1A, 3 },
    { 661504000, 0x86, 0x19, 3 },
    { 662528000, 0x85, 0x19, 3 },
    { 664576000, 0x84, 0x18, 3 },
    { 665600000, 0x83, 0x18, 3 },
    { 666624000, 0x82, 0x18, 3 },
    { 668672000, 0x81, 0x18, 3 },
    { 669696000, 0x80, 0x17, 3 },
    { 671744000, 0x7F, 0x17, 3 },
    { 672768000, 0x7E, 0x17, 3 },
    { 674816000, 0x7D, 0x17, 3 },
    { 675840000, 0x7C, 0x17, 3 },
    { 676864000, 0x7B, 0x17, 3 },
    { 678912000, 0x7A, 0x17, 3 },
    { 679936000, 0x79, 0x17, 3 },
    { 681984000, 0x78, 0x17, 3 },
    { 683008000, 0x77, 0x17, 3 },
    { 685056000, 0x76, 0x17, 3 },
    { 686080000, 0x75, 0x17, 3 },
    { 688128000, 0x74, 0x17, 3 },
    { 689152000, 0x73, 0x17, 3 },
    { 691200000, 0x72, 0x16, 3 },
    { 693248000, 0x71, 0x16, 3 },
    { 694272000, 0x70, 0x16, 3 },
    { 696320000, 0x6F, 0x15, 3 },
    { 697344000, 0x6E, 0x15, 3 },
    { 699392000, 0x6D, 0x15, 3 },
    { 700416000, 0x6C, 0x15, 3 },
    { 702464000, 0x6B, 0x14, 3 },
    { 704512000, 0x6A, 0x14, 3 },
    { 704512000, 0x69, 0x14, 3 },
    { 706560000, 0x68, 0x14, 3 },
    { 707584000, 0x67, 0x13, 3 },
    { 709632000, 0x66, 0x13, 3 },
    { 711680000, 0x65, 0x13, 3 },
    { 712704000, 0x64, 0x13, 3 },
    { 714752000, 0x63, 0x13, 3 },
    { 716800000, 0x62, 0x13, 3 },
    { 717824000, 0x61, 0x13, 3 },
    { 719872000, 0x60, 0x13, 3 },
    { 721920000, 0x5F, 0x12, 3 },
    { 723968000, 0x5E, 0x12, 3 },
    { 724992000, 0x5D, 0x12, 3 },
    { 727040000, 0x5C, 0x12, 3 },
    { 729088000, 0x5B, 0x11, 3 },
    { 731136000, 0x5A, 0x11, 3 },
    { 732160000, 0x59, 0x11, 3 },
    { 734208000, 0x58, 0x11, 3 },
    { 736256000, 0x57, 0x10, 3 },
    { 738304000, 0x56, 0x10, 3 },
    { 740352000, 0x55, 0x10, 3 },
    { 741376000, 0x54, 0x10, 3 },
    { 743424000, 0x53, 0x10, 3 },
    { 745472000, 0x52, 0x0F, 3 },
    { 746496000, 0x51, 0x0F, 3 },
    { 748544000, 0x50, 0x0F, 3 },
    { 750592000, 0x4F, 0x0F, 3 },
    { 752640000, 0x4E, 0x0F, 3 },
    { 753664000, 0x4D, 0x0F, 3 },
    { 755712000, 0x4C, 0x0F, 3 },
    { 757760000, 0x4B, 0x0F, 3 },
    { 759808000, 0x4A, 0x0F, 3 },
    { 761856000, 0x49, 0x0F, 3 },
    { 762880000, 0x49, 0x0F, 3 },
    { 763904000, 0x48, 0x0F, 3 },
    { 765952000, 0x47, 0x0F, 3 },
    { 768000000, 0x46, 0x0F, 3 },
    { 770048000, 0x45, 0x0F, 3 },
    { 772096000, 0x44, 0x0F, 3 },
    { 774144000, 0x43, 0x0F, 3 },
    { 776192000, 0x42, 0x0F, 3 },
    { 778240000, 0x41, 0x0F, 3 },
    { 780288000, 0x40, 0x0F, 3 },
    { 783360000, 0x3F, 0x0F, 3 },
    { 785408000, 0x3E, 0x0F, 3 },
    { 787456000, 0x3D, 0x0F, 3 },
    { 789504000, 0x3C, 0x0F, 3 },
    { 790528000, 0x3B, 0x0F, 3 },
    { 792576000, 0x3A, 0x0F, 3 },
    { 794624000, 0x39, 0x0F, 3 },
    { 797696000, 0x38, 0x0F, 3 },
    { 799744000, 0x37, 0x0F, 3 },
    { 801792000, 0x36, 0x0F, 3 },
    { 803840000, 0x35, 0x0F, 3 },
    { 806912000, 0x34, 0x0F, 3 },
    { 808960000, 0x33, 0x0F, 3 },
    { 809984000, 0x33, 0x0F, 3 },
    { 811008000, 0x32, 0x0F, 3 },
    { 813056000, 0x31, 0x0F, 3 },
    { 816128000, 0x30, 0x0F, 3 },
    { 818176000, 0x2F, 0x0F, 3 },
    { 820224000, 0x2E, 0x0F, 3 },
    { 823296000, 0x2D, 0x0F, 3 },
    { 825344000, 0x2C, 0x0F, 3 },
    { 828416000, 0x2B, 0x0F, 3 },
    { 830464000, 0x2A, 0x0F, 3 },
    { 832512000, 0x29, 0x0F, 3 },
    { 834560000, 0x28, 0x0F, 3 },
    { 836608000, 0x27, 0x0F, 3 },
    { 839680000, 0x26, 0x0F, 3 },
    { 841728000, 0x25, 0x0F, 3 },
    { 844800000, 0x24, 0x0F, 3 },
    { 847872000, 0x23, 0x0F, 3 },
    { 849920000, 0x22, 0x0F, 3 },
    { 852992000, 0x21, 0x0E, 3 },
    { 855040000, 0x20, 0x0E, 3 },
    { 858112000, 0x1F, 0x0E, 3 },
    { 861184000, 0x1E, 0x0E, 3 },
    { 863232000, 0x1D, 0x0E, 3 },
    { 866304000, 0x1C, 0x0E, 3 },
    { 900096000, 0x10, 0x0C, 3 },
    { 929792000, 0x07, 0x0B, 3 },
    { 969728000, 0x00, 0x0A, 3 }
};

/* ********** */

/********************************************************************
	Static Function
 ********************************************************************/
/********************************************************************
 * FUNCTION:	TDA182I2Read:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
int drv_ch_i2c_TUNERRead(int nUnitId, HUINT8 ucRegAddr, HUINT8 *pucBuffer, int nNumberToRead)
{
	int nResult = DI_CH_OK;
	int nRet = 0;//, i;
	HUINT8	ucDevAddr = M_TUNER_I2C_ADDR;
	HUINT8	ucI2CBus = I2C_CHANNEL_CHANNEL;

	if(nUnitId == MASTER_TUNER )
	{
		ucDevAddr = M_TUNER_I2C_ADDR;
		ucI2CBus = I2C_CHANNEL_CHANNEL;
	}
#if defined(CONFIG_I2C_MAP_FOR_2TUNER)
	else if(nUnitId == SLAVE_TUNER )
	{
		ucDevAddr = S_TUNER_I2C_ADDR;
		ucI2CBus = I2C_CHANNEL_CHANNEL2;
	}
#endif
	else
	{
		ucDevAddr = M_TUNER_I2C_ADDR;
		ucI2CBus = I2C_CHANNEL_CHANNEL;
	}
	//CH_DI_Print(0, (" drv_ch_i2c_TUNERRead: ucDevAddr[0x%x], ucI2CBus[0x%x]\n", ucDevAddr, ucI2CBus));

	nRet = DRV_I2c_ReadA8( ucI2CBus, ucDevAddr, ucRegAddr, pucBuffer, nNumberToRead );
	if ( nRet != DI_CH_OK )
	{
		CH_DI_Print(0, (" Error[0x%x]", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		//CH_PrintDebug(" ucRegValue[0x%x]\n", ucRegValue);
	}

	return nResult;

}


int drv_ch_i2c_TUNERWrite(int nUnitId,  HUINT8 ucRegAddr, HUINT8 *pucBuffer, int nNumberToWrite)
{
	int nResult = DI_CH_OK;
	int nRet = 0;//, i;
	HUINT8	ucDevAddr = M_TUNER_I2C_ADDR;
	HUINT8	ucI2CBus = I2C_CHANNEL_CHANNEL;

	if(nUnitId == MASTER_TUNER )
	{
		ucDevAddr = M_TUNER_I2C_ADDR;
		ucI2CBus = I2C_CHANNEL_CHANNEL;
	}
#if defined(CONFIG_I2C_MAP_FOR_2TUNER)
	else if(nUnitId == SLAVE_TUNER )
	{
		ucDevAddr = S_TUNER_I2C_ADDR;
		ucI2CBus = I2C_CHANNEL_CHANNEL2;
	}
#endif
	else
	{
		ucDevAddr = M_TUNER_I2C_ADDR;
		ucI2CBus = I2C_CHANNEL_CHANNEL;
	}
	//CH_DI_Print(0, (" drv_ch_i2c_TUNERWrite: ucDevAddr[0x%x], ucI2CBus[0x%x]\n", ucDevAddr, ucI2CBus));

	nRet = DRV_I2c_WriteA8(ucI2CBus, ucDevAddr, ucRegAddr, pucBuffer, nNumberToWrite);
	if ( nRet != DI_CH_OK )
	{
		CH_DI_Print(0, (" Error[0x%x]", nRet));
		nResult = DI_CH_ERR;
	}
	else
	{
		//CH_PrintDebug(" Success \n");
	}

	return nResult;

}


/********************************************************************
 * FUNCTION:	TDA182I2Read:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2Read( pTDA182I2Object_t pObj, HUINT8 uSubAddress, HUINT8 uNbData)
{
	TDA182I2_ERR_CODE	reterr = tmNOERR;
	HUINT8*				pI2CMap = NULL;
	pI2CMap = &(pObj->I2CMap.uBx00.ID_byte_1) + uSubAddress;

	reterr = drv_ch_i2c_TUNERRead( pObj->tUnit, uSubAddress, pI2CMap, uNbData );

	return reterr;
}

void drv_ch_i2c_TUNERDump(int nUnitId)
{
	int i;
	HUINT8 ucdata=0;

	for(i=0; i<68; i++)
	{

		drv_ch_i2c_TUNERRead(nUnitId, i, &ucdata, 1);
		CH_DI_Print(0, ("[0x%02X] \t[0x%02x]\n", i, ucdata));
	}
}

/********************************************************************
 * FUNCTION:	TDA182I2Write:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2Write( pTDA182I2Object_t pObj, HUINT8 uSubAddress, HUINT8 uNbData)
{
	TDA182I2_ERR_CODE	reterr = tmNOERR;
	HUINT8*				pI2CMap = NULL;

	// pI2CMap initialization
	pI2CMap = &(pObj->I2CMap.uBx00.ID_byte_1);

	reterr = drv_ch_i2c_TUNERWrite(pObj->tUnit, uSubAddress, &(pI2CMap[uSubAddress]), uNbData);

	// return value
	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2GetAGC1_Gain_Read
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2_DumpRegister( HUINT32 tUnit )
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;
 	int i=0;
	unsigned char* ucdata;


	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		reterr = TDA182I2Read(pObj, 0x00, TDA182I2_I2C_MAP_NB_BYTES);

		for(i=0; i<= TDA182I2_I2C_MAP_NB_BYTES; i++)
		{

			ucdata = &(pObj->I2CMap.uBx00.ID_byte_1) + i;

			CH_DI_Print(0, ("[0x%02X] \t[0x%02x]\n", i, *ucdata));
		}
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetIR_Corr_Boost
 * DESCRIPTION:	Driver 1.3.2
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetIR_Corr_Boost( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		/* Set value */
        pObj->I2CMap.uBx1F.bF.IR_Corr_Boost = uValue;

        /* Read byte 0x1F */
        reterr = TDA182I2Write(pObj, 0x1F, 1);
    }

    return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetIR_Loop
 * DESCRIPTION:	Driver 1.3.2
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetIR_Loop ( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;


    /* Get Instance Object */
	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
    {
        /* Set value */
        pObj->I2CMap.uBx1E.bF.IR_Loop = uValue - 4;

        /* Read byte 0x1E */
        reterr = TDA182I2Write(pObj, 0x1E, 1);
    }

    return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2GetMajorRevision
 * DESCRIPTION:	Driver 1.3.2
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2GetMajorRevision( HUINT32 tUnit, HUINT8* puValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	if (puValue == NULL)
		return tmERR_BAD_PARAMETER;

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
    {
        /* Read byte 0x02 */
		reterr = TDA182I2Read(pObj, 0x02, 1);
		if(reterr == tmNOERR)
		{
            /* Get value */
            *puValue = (pObj->I2CMap.uBx02.ID_byte_3 & 0xF0)>>4;
        }
    }
    return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2GetMinorRevision
 * DESCRIPTION:	Driver 1.3.2
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2GetMinorRevision( HUINT32 tUnit, HUINT8* puValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	if (puValue == NULL)
		return tmERR_BAD_PARAMETER;

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
    {
        /* Read byte 0x02 */
		reterr = TDA182I2Read(pObj, 0x02, 1);
		if(reterr == tmNOERR)
		{
            /* Get value */
            *puValue = pObj->I2CMap.uBx02.ID_byte_3 & 0x0F;
        }
    }
    return reterr;
}


/********************************************************************
 * FUNCTION:	TDA182I2GetIRQWait
 * DESCRIPTION:	Driver 1.3.1
 * RETURN:
********************************************************************/

TDA182I2_ERR_CODE TDA182I2GetIRQWait( HUINT32 tUnit, HUINT8* pWait)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	if (pWait == NULL)
		return tmERR_BAD_PARAMETER;

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
    {
        *pWait = pObj->bIRQWait;
    }

    return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetIRQWait
 * DESCRIPTION:	Driver 1.3.2
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetIRQWait( HUINT32 tUnit, HUINT8 bWait)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

    /* Get Instance Object */
	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
    {
        pObj->bIRQWait = bWait;
    }

    return reterr;
}


/********************************************************************
 * FUNCTION:	TDA182I2SetLT_Enable
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetLT_Enable ( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// set value
		pObj->I2CMap.uBx0C.bF.LT_Enable = uValue;

		// write byte 0x0C
		reterr = TDA182I2Write(pObj, 0x0C, 1);
	}

    return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetPSM_AGC1
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetPSM_AGC1 ( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
    {
        // set value
        pObj->I2CMap.uBx1B.bF.PSM_AGC1 = uValue;

        // read byte 0x1B
        reterr = TDA182I2Write(pObj, 0x1B, 1);
    }

    return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetAGC1_6_15dB
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetAGC1_6_15dB ( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
    {
        // set value
        pObj->I2CMap.uBx0C.bF.AGC1_6_15dB = uValue;

        // write byte 0x0C
        reterr = TDA182I2Write(pObj, 0x0C, 1);
    }

    return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2GetAGC1_Gain_Read
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2GetAGC1_Gain_Read( HUINT32 tUnit, HUINT8* puValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	if (puValue == NULL)
		return tmERR_BAD_PARAMETER;

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		reterr = TDA182I2Read(pObj, 0x32, 1);

		if(reterr == tmNOERR)
		  {
			  // get value
			  *puValue = pObj->I2CMap.uBx32.bF.AGC1_Gain_Read;
		  }

	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2GetAGC2_Gain_Read
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2GetAGC2_Gain_Read( HUINT32 tUnit, HUINT8* puValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	if (puValue == NULL)
		return tmERR_BAD_PARAMETER;

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		reterr = TDA182I2Read(pObj, 0x32, 1);

		if(reterr == tmNOERR)
		  {
			  // get value
			  *puValue = pObj->I2CMap.uBx32.bF.AGC2_Gain_Read;
		  }

	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2GetAGC4_Gain_Read
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2GetAGC4_Gain_Read( HUINT32 tUnit, HUINT8* puValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	if (puValue == NULL)
		return tmERR_BAD_PARAMETER;

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		reterr = TDA182I2Read(pObj, 0x34, 1);

		if(reterr == tmNOERR)
		  {
			  // get value
			  *puValue = pObj->I2CMap.uBx34.IFAGCs_Gain_byte;
		  }

	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2GetAGC5_Gain_Read
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2GetAGC5_Gain_Read( HUINT32 tUnit, HUINT8* puValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	if (puValue == NULL)
		return tmERR_BAD_PARAMETER;

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		reterr = TDA182I2Read(pObj, 0x34, 1);

		if(reterr == tmNOERR)
		  {
			  // get value
			  *puValue = pObj->I2CMap.uBx34.bF.AGC5_Gain_Read;
		  }

	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2GetRFCalFreq:
 * DESCRIPTION:	driver 1.3.1
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2GetRFCalFreq( HUINT32 tUnit, HUINT32 idx, HUINT32 *puRF0, HUINT32 *puRF1)
{

	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	reterr = TDA182I2GetInstance(tUnit, &pObj);

    switch(idx)
    {
    case 0:
        if(puRF0!=NULL) *puRF0 = TDA182I2_Cal_Map[idx].Cal_Freq[pObj->I2CMap.uBx26.bF.RFCAL_Freq0];
        if(puRF1!=NULL) *puRF1 = TDA182I2_Cal_Map[idx+1].Cal_Freq[pObj->I2CMap.uBx26.bF.RFCAL_Freq1];
        break;

    case 1:
        if(puRF0!=NULL) *puRF0 = TDA182I2_Cal_Map[idx].Cal_Freq[pObj->I2CMap.uBx26.bF.RFCAL_Freq1];
        if(puRF1!=NULL) *puRF1 = TDA182I2_Cal_Map[idx+1].Cal_Freq[pObj->I2CMap.uBx27.bF.RFCAL_Freq2];
        break;

    case 2:
        if(puRF0!=NULL) *puRF0 = TDA182I2_Cal_Map[idx].Cal_Freq[pObj->I2CMap.uBx27.bF.RFCAL_Freq2];
        if(puRF1!=NULL) *puRF1 = TDA182I2_Cal_Map[idx+1].Cal_Freq[pObj->I2CMap.uBx27.bF.RFCAL_Freq3];
        break;

    case 3:
        if(puRF0!=NULL) *puRF0 = TDA182I2_Cal_Map[idx].Cal_Freq[pObj->I2CMap.uBx27.bF.RFCAL_Freq3];
        if(puRF1!=NULL) *puRF1 = TDA182I2_Cal_Map[idx+1].Cal_Freq[pObj->I2CMap.uBx28.bF.RFCAL_Freq4];
        break;

    case 4:
        if(puRF0!=NULL) *puRF0 = TDA182I2_Cal_Map[idx].Cal_Freq[pObj->I2CMap.uBx28.bF.RFCAL_Freq4];
        if(puRF1!=NULL) *puRF1 = TDA182I2_Cal_Map[idx+1].Cal_Freq[pObj->I2CMap.uBx28.bF.RFCAL_Freq5];
        break;

    case 5:
        if(puRF0!=NULL) *puRF0 = TDA182I2_Cal_Map[idx].Cal_Freq[pObj->I2CMap.uBx28.bF.RFCAL_Freq5];
        if(puRF1!=NULL) *puRF1 = TDA182I2_Cal_Map[idx+1].Cal_Freq[pObj->I2CMap.uBx29.bF.RFCAL_Freq6];
        break;

    case 6:
        if(puRF0!=NULL) *puRF0 = TDA182I2_Cal_Map[idx].Cal_Freq[pObj->I2CMap.uBx29.bF.RFCAL_Freq6];
        if(puRF1!=NULL) *puRF1 = TDA182I2_Cal_Map[idx+1].Cal_Freq[pObj->I2CMap.uBx29.bF.RFCAL_Freq7];
        break;

    case 7:
        if(puRF0!=NULL) *puRF0 = TDA182I2_Cal_Map[idx].Cal_Freq[pObj->I2CMap.uBx29.bF.RFCAL_Freq7];
        if(puRF1!=NULL) *puRF1 = TDA182I2_Cal_Map[idx+1].Cal_Freq[pObj->I2CMap.uBx2A.bF.RFCAL_Freq8];
        break;

    case 8:
        if(puRF0!=NULL) *puRF0 = TDA182I2_Cal_Map[idx].Cal_Freq[pObj->I2CMap.uBx2A.bF.RFCAL_Freq8];
        if(puRF1!=NULL) *puRF1 = TDA182I2_Cal_Map[idx+1].Cal_Freq[pObj->I2CMap.uBx2A.bF.RFCAL_Freq9];
        break;

    case 9:
        if(puRF0!=NULL) *puRF0 = TDA182I2_Cal_Map[idx].Cal_Freq[pObj->I2CMap.uBx2A.bF.RFCAL_Freq9];
        if(puRF1!=NULL) *puRF1 = TDA182I2_Cal_Map[idx+1].Cal_Freq[pObj->I2CMap.uBx2B.bF.RFCAL_Freq10];
        break;

    case 10:
        if(puRF0!=NULL) *puRF0 = TDA182I2_Cal_Map[idx].Cal_Freq[pObj->I2CMap.uBx2B.bF.RFCAL_Freq10];
        if(puRF1!=NULL) *puRF1 = TDA182I2_Cal_Map[idx+1].Cal_Freq[pObj->I2CMap.uBx2B.bF.RFCAL_Freq11];
        break;

    default:
        if(puRF0!=NULL) *puRF0 = 0;
        if(puRF1!=NULL) *puRF1 = 0;
        break;
    }

    return tmNOERR;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetRF_Filter_Cap
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetRF_Filter_Cap( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
    {
        // set value
        pObj->I2CMap.uBx2D.bF.RF_Filter_Cap = uValue;

        // read byte 0x2D
        reterr = TDA182I2Write(pObj, 0x2D, 1);
    }

    return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetRF_Filter_Band
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetRF_Filter_Band( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

    /* Get Instance Object */
	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
    {
        // set value
        pObj->I2CMap.uBx2C.bF.RF_Filter_Band = uValue;

        // read byte 0x2C
        reterr = TDA182I2Write(pObj, 0x2C, 1);
    }

    return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2GetMSM_XtalCal_End
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetGain_Taper( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
    {
        // set value
        pObj->I2CMap.uBx2E.bF.Gain_Taper = uValue;

        // read byte 0x2E
        reterr = TDA182I2Write(pObj, 0x2E, 1);
    }

    return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2GetMSM_XtalCal_End
 * DESCRIPTION:	dirver 1.3.1
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2GetMSM_XtalCal_End( HUINT32 tUnit, HUINT8* puValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	if (puValue == NULL)
		return tmERR_BAD_PARAMETER;

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// read byte 0x08
		reterr = TDA182I2Read(pObj, 0x08, 1);
	}

	if(reterr == tmNOERR)
	{
		// get value
		*puValue = pObj->I2CMap.uBx08.bF.MSM_XtalCal_End;
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2WaitXtalCal_End
 * DESCRIPTION:	driver 1.3.1
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2WaitXtalCal_End( HUINT32 tUnit, HUINT32 timeOut, HUINT32 waitStep)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;
	HUINT32					counter = timeOut/waitStep; /* Wait max timeOut/waitStepms */
	HUINT8					uMSM_XtalCal_End = 0;

	CH_DI_Print(5, ("[TDA182I2WaitXtalCal_End]timeout = %d, waitstep = %d, counter = %d\n", timeOut, waitStep, counter));

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		while(reterr == tmNOERR && (--counter)>0)
		{
			CH_DI_Print(6, ("[TDA182I2WaitXtalCal_End]counter = %d\n", counter));

			reterr = TDA182I2GetMSM_XtalCal_End(tUnit, &uMSM_XtalCal_End);
			if(uMSM_XtalCal_End == 1)
			{
				/* MSM_XtalCal_End triggered => Exit */
				CH_DI_Print(6, ("\n[TDA182I2WaitXtalCal_End]MSM_XtalCal_End triggered\n\n"));
				break;
			}

			VK_TASK_Sleep(waitStep);
			CH_DI_Print(6, ("[TDA182I2WaitXtalCal_End]uMSM_XtalCal_End not wait time = %d\n", waitStep));
		}

		if(counter == 0)
		{
			CH_DI_Print(5, ("[TDA182I2WaitXtalCal_End] FAIL !!\n"));
			reterr = tmERR_NOT_READY;
		}

	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetAGC5_loop_off:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetAGC5_loop_off( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// set value
		pObj->I2CMap.uBx25.bF.AGC5_loop_off = uValue;

		// read byte 0x25
		reterr = TDA182I2Write(pObj, 0x25, 1);
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetForce_AGC5_gain:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetForce_AGC5_gain( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// set value
		pObj->I2CMap.uBx25.bF.Force_AGC5_gain = uValue;

		// read byte 0x25
		reterr = TDA182I2Write(pObj, 0x25, 1);
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetAGC5_Gain:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetAGC5_Gain( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// set value
		pObj->I2CMap.uBx25.bF.AGC5_Gain = uValue;

		// read byte 0x25
		reterr = TDA182I2Write(pObj, 0x25, 1);
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetN_CP_Current:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetN_CP_Current( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// set value
		pObj->I2CMap.uBx30.bF.N_CP_Current = uValue;

		// read byte 0x30
		reterr = TDA182I2Write(pObj, 0x30, 1);
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetAGC1_Do_step:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetAGC1_Do_step( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// set value
		pObj->I2CMap.uBx24.bF.AGC1_Do_step = uValue;

		// read byte 0x25
		reterr = TDA182I2Write(pObj, 0x24, 1);
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetAGC2_Do_step:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetAGC2_Do_step( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// set value
		pObj->I2CMap.uBx2E.bF.AGC2_Do_step = uValue;

		// read byte 0x25
		reterr = TDA182I2Write(pObj, 0x2E, 1);
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetAGC5_Do_step:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetAGC5_Do_step( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// set value
		pObj->I2CMap.uBx25.bF.AGC5_Do_step = uValue;

		// read byte 0x25
		reterr = TDA182I2Write(pObj, 0x25, 1);
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetAGCs_Up_Step_assym:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetAGCs_Up_Step_assym( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// set value
		pObj->I2CMap.uBx0E.bF.AGCs_Up_Step_assym = uValue;

		// read byte 0x25
		reterr = TDA182I2Write(pObj, 0x0E, 1);
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetAGCs_Do_Step_assym:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetAGCs_Do_Step_assym( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// set value
		pObj->I2CMap.uBx11.bF.AGCs_Do_Step_assym = uValue;

		// read byte 0x25
		reterr = TDA182I2Write(pObj, 0x11, 1);
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetAGCs_Up_Step:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE  TDA182I2SetAGCs_Up_Step( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// set value
		pObj->I2CMap.uBx0E.bF.AGCs_Up_Step = uValue;

		// write byte 0x0E
		reterr = TDA182I2Write(pObj, 0x0E, 1);
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetAGC_Ovld_TOP:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE  TDA182I2SetAGC_Ovld_TOP( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// set value
		pObj->I2CMap.uBx22.bF.AGC_Ovld_TOP = uValue;

		// write byte 0x22
		reterr = TDA182I2Write(pObj, 0x22, 1);
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetIR_Target:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetIR_Target( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// set value
		pObj->I2CMap.uBx1E.bF.IR_Target = uValue;

		// read byte 0x1E
		reterr = TDA182I2Write(pObj, 0x1E, 1);
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetIR_mode_ram_store:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetIR_mode_ram_store( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// set value
		pObj->I2CMap.uBx1F.bF.IR_mode_ram_store = uValue;

		// write byte 0x1F
		reterr = TDA182I2Write(pObj, 0x1F, 1);
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetRSSI_Ck_Speed:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetRSSI_Ck_Speed( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// set value
		pObj->I2CMap.uBx36.bF.RSSI_Ck_Speed = uValue;

		// write byte 0x36
		reterr = TDA182I2Write(pObj, 0x36, 1);
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetPD_Udld:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetPD_Udld( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// set value
		pObj->I2CMap.uBx22.bF.PD_Udld = uValue;

		// write byte 0x22
		reterr = TDA182I2Write(pObj, 0x22, 1);
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetRFCAL_Phi2:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetRFCAL_Phi2( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// set value
		pObj->I2CMap.uBx37.bF.RFCAL_Phi2 = uValue;

		// write byte 0x37
		reterr = TDA182I2Write(pObj, 0x37, 1);
	}

	return reterr;
}

// REV 1.3.1
/********************************************************************
 * FUNCTION:	TDA182I2SetRF_Filter_Bypass:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetRF_Filter_Bypass( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

    /* Get Instance Object */
	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
    {
        // set value
        pObj->I2CMap.uBx2C.bF.RF_Filter_Bypass = uValue;

        // read byte 0x2C
        reterr = TDA182I2Write(pObj, 0x2C, 1);
    }

    return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetIRQ_Clear:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetIRQ_Clear( HUINT32 tUnit, HUINT8 irqStatus)	//driver 1.3.2
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// set IRQ_Clear  // driver 1.3.1
		// pObj->I2CMap.uBx0A.IRQ_clear |= 0x9F;	// driver 1.3.1
       	pObj->I2CMap.uBx0A.IRQ_clear |= (0x80|(irqStatus&0x1F));	//driver 1.3.2

		// write byte 0x0A
		reterr = TDA182I2Write(pObj, 0x0A, 1);

		// reset IRQ_Clear (buffer only, no write)
		//pObj->I2CMap.uBx0A.IRQ_clear &= (~0x9F);	// driver 1.3.1
        pObj->I2CMap.uBx0A.IRQ_clear &= (~(0x80|(irqStatus&0x1F)));	//driver 1.3.2
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2RFFilterCorrInit:
 * DESCRIPTION:	driver 1.3.1
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2RFFilterCorrInit( HUINT32 tUnit)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;
    HUINT8                  uData = 0, idx = 0;
    HUINT8*                 puData = NULL;
    HUINT32                 uCounter = 0, uRFCal0 = 0, uRFCal1 = 0;

	reterr = TDA182I2GetInstance(tUnit, &pObj);
    if(reterr == tmNOERR)
    {
        // read byte 0x38-0x43
        reterr = TDA182I2Read(pObj, 0x38, 12);
        if(reterr == tmNOERR)
        {
            puData = &pObj->I2CMap.uBx38.rfcal_log_0;

            // Store Cprog
            for(uCounter = 0; uCounter<TDA182I2_RFCAL_PROG_ROW; uCounter++)
            {
                uData = ((*puData)&0x7F);
                pObj->RFCAL_Prog[uCounter].DeltaCprog = uData>63?uData-128:uData;
                puData++;
            }

            // read byte 0x26-0x2B
            reterr = TDA182I2Read(pObj, 0x26, 6);
        }

        for(uCounter = 0; uCounter<TDA182I2_RFCAL_COEFFS_ROW; uCounter++)
        {
            idx = pObj->RFCAL_Coeffs[uCounter].Cal_number;

            if(reterr == tmNOERR)
            {
                reterr = TDA182I2GetRFCalFreq(pObj->tUnit, idx, &uRFCal0, &uRFCal1);
            }
            else
            {
                break;
            }

            pObj->RFCAL_Coeffs[uCounter].RF_B1 = pObj->RFCAL_Prog[idx].DeltaCprog + TDA182I2_Cal_Map[idx].CprogOffset_Val[pObj->RFCAL_Prog[idx].CprogOffset];
            pObj->RFCAL_Coeffs[uCounter].RF_A1 = (((pObj->RFCAL_Prog[idx+1].DeltaCprog + TDA182I2_Cal_Map[idx+1].CprogOffset_Val[pObj->RFCAL_Prog[idx].CprogOffset])
            									- (pObj->RFCAL_Prog[idx].DeltaCprog + TDA182I2_Cal_Map[idx].CprogOffset_Val[pObj->RFCAL_Prog[idx].CprogOffset])) * 1000000)
            									/((HINT32)(uRFCal1 - uRFCal0)/1000);
        }

	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2WaitIRQ:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2WaitIRQ( HUINT32 tUnit, HUINT32 timeOut, HUINT32 waitStep, HUINT8 irqStatus)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;
	HUINT32					counter = timeOut/waitStep; /* Wait max timeOut/waitStep ms */   //driver 1.3.1
	HUINT8					uIRQ = 0;

	/* driver 1.3.1 */
	HUINT8           		uIRQStatus = 0;
	HUINT8            		bIRQTriggered = 0;
	/* ********** */


	CH_DI_Print(4, ("[TDA182I2WaitIRQ] timeOut = %d, waitStep = %d, irqStatus = 0x%02x\n", timeOut, waitStep, irqStatus));
	CH_DI_Print(4, ("[TDA182I2WaitIRQ] counter = %d\n", counter));

		    reterr = TDA182I2GetInstance(tUnit, &pObj);

	/* Get IRQ Status */
	while(reterr == tmNOERR && (--counter)>0)
	{
		CH_DI_Print(4, ("[TDA182I2WaitIRQ] counter = %d\n", counter));
			// read byte 0x08

		reterr = TDA182I2GetIRQ_status(tUnit, &uIRQ);
		if(reterr == tmNOERR && uIRQ == 1)
	        {
			bIRQTriggered = 1;	//driver 1.3.1
	            CH_DI_Print(3, ("[TDA182I2WaitIRQ]IRQTriggered\n"));
	        }

        if(bIRQTriggered)
		{
		    /* IRQ triggered => Exit */
		    break;
		}

		CH_DI_Print(3, ("[TDA182I2WaitIRQ]IRQ not yet !!!\n"));

		/* driver 1.3.1 */
		if(irqStatus != 0x00)
		{
			CH_DI_Print(4, ("[TDA182I2WaitIRQ]irqStatus = 0x%02x \n", irqStatus));
			uIRQStatus = ((pObj->I2CMap.uBx08.IRQ_status)&0x1F);
			CH_PrintDebug("[TDA182I2WaitIRQ]uIRQStatus = 0x%02x \n", uIRQStatus);
			if(irqStatus == uIRQStatus)
			{
				CH_DI_Print(4, ("[TDA182I2WaitIRQ]irqStatus == uIRQStatus \n"));
				bIRQTriggered = 1;
			}
		}
		/* ********** */

		VK_TASK_Sleep(waitStep);
		CH_DI_Print(4, ("[TDA182I2WaitIRQ]VK_TASK_Sleep\n"));
	}

    if(counter == 0)
    {

		CH_DI_Print(4, ("[TDA182I2WaitIRQ]cannot find IRQ\r\n"));
        reterr = tmERR_NOT_READY;
    }

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2GetLO_Lock:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2GetLO_Lock( HUINT32 tUnit, HUINT8* puValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	// test the parameter
	if (puValue == NULL)
		return tmERR_BAD_PARAMETER;

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// read byte 0x05
		reterr = TDA182I2Read(pObj, 0x05, 1);
		if(reterr == tmNOERR)
		{
			// get value
			*puValue = pObj->I2CMap.uBx05.bF.LO_Lock ;
		}
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2GetIRQ_status:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2GetIRQ_status( HUINT32 tUnit, HUINT8* puValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	// test the parameter
	if (puValue == NULL)
		return tmERR_BAD_PARAMETER;

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		reterr = TDA182I2Read(pObj, 0x08, 1);
		if(reterr == tmNOERR)
		{
			// get value
			*puValue = pObj->I2CMap.uBx08.bF.IRQ_status;
		}

	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetLP_FC:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetLP_FC( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	/* Get Instance Object */
	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
        // set value
        pObj->I2CMap.uBx13.bF.LP_Fc = uValue;

        // write byte 0x13
        reterr = TDA182I2Write(pObj, 0x13, 1);
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetLP_FC_Offset:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetLP_FC_Offset( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	/* Get Instance Object */
	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
        // set value
        pObj->I2CMap.uBx13.bF.LP_FC_Offset = uValue;

        // write byte 0x13
        reterr = TDA182I2Write(pObj, 0x13, 1);
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetIF_Level:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetIF_Level( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	/* Get Instance Object */
	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// set value
        pObj->I2CMap.uBx12.bF.IF_level = uValue;

        // write byte 0x12
		reterr = TDA182I2Write(pObj, 0x12, 1);
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	tmddTDA182I2SetIF_ATSC_Notch:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetIF_ATSC_Notch( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	/* Get Instance Object */
	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// set value
        pObj->I2CMap.uBx13.bF.IF_ATSC_Notch = uValue;

        // write byte 0x13
        reterr = TDA182I2Write(pObj, 0x13, 1);
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetHi_Pass:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetHi_Pass( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	/* Get Instance Object */
	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// set value
        pObj->I2CMap.uBx23.bF.Hi_Pass = uValue;

        // read byte 0x23
        reterr = TDA182I2Write(pObj, 0x23, 1);
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetIF_HP_Fc:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetIF_HP_Fc( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	/* Get Instance Object */
	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// set value
        pObj->I2CMap.uBx13.bF.IF_HP_Fc = uValue;

        // write byte 0x13
        reterr = TDA182I2Write(pObj, 0x13, 1);
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetIF_Notch:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetIF_Notch( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	/* Get Instance Object */
	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// set value
        pObj->I2CMap.uBx23.bF.IF_Notch = uValue;

		// write byte 0x0C
        reterr = TDA182I2Write(pObj, 0x23, 1);
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetAGC1_TOP:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetAGC1_TOP( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t   	pObj = NULL;
    TDA182I2_ERR_CODE       reterr = tmERR_BAD_PARAMETER;

	/* Get Instance Object */
	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// set value
		pObj->I2CMap.uBx0C.bF.AGC1_TOP = uValue;

		// write byte 0x0C
		reterr = TDA182I2Write(pObj, 0x0C, 1);
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetAGC2_TOP:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetAGC2_TOP( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t   	pObj = NULL;
    TDA182I2_ERR_CODE       reterr = tmERR_BAD_PARAMETER;

	/* Get Instance Object */
	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// set value
		pObj->I2CMap.uBx0D.bF.AGC2_TOP = uValue;

		// write byte 0x0D
		reterr = TDA182I2Write(pObj, 0x0D, 1);
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetRFAGC_Top:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetRFAGC_Top( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t   	pObj = NULL;
    TDA182I2_ERR_CODE       reterr = tmERR_BAD_PARAMETER;

	/* Get Instance Object */
	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// set value
		pObj->I2CMap.uBx0F.bF.RFAGC_Top = uValue;

		// write byte 0x0F
		reterr = TDA182I2Write(pObj, 0x0F, 1);
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetIR_Mixer_Top:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetIR_Mixer_Top( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t   	pObj = NULL;
    TDA182I2_ERR_CODE       reterr = tmERR_BAD_PARAMETER;

	/* Get Instance Object */
	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// set value
		pObj->I2CMap.uBx10.bF.IR_Mixer_Top = uValue;

		// write byte 0x10
		reterr = TDA182I2Write(pObj, 0x10, 1);
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetAGC5_TOP:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetAGC5_TOP( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t   	pObj = NULL;
    TDA182I2_ERR_CODE       reterr = tmERR_BAD_PARAMETER;

	/* Get Instance Object */
	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// set value
		pObj->I2CMap.uBx11.bF.AGC5_TOP = uValue;

		// write byte 0x11
		reterr = TDA182I2Write(pObj, 0x11, 1);
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetPD_RFAGC_Adapt:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetPD_RFAGC_Adapt( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t   	pObj = NULL;
    TDA182I2_ERR_CODE       reterr = tmERR_BAD_PARAMETER;

	/* Get Instance Object */
	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// set value
		pObj->I2CMap.uBx0F.bF.PD_RFAGC_Adapt = uValue;

		// write byte 0x0F
		reterr = TDA182I2Write(pObj, 0x0F, 1);
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetRFAGC_Adapt_TOP:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetRFAGC_Adapt_TOP( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t   	pObj = NULL;
    TDA182I2_ERR_CODE       reterr = tmERR_BAD_PARAMETER;

	/* Get Instance Object */
	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// set value
		pObj->I2CMap.uBx0F.bF.RFAGC_Adapt_TOP = uValue;

		// write byte 0x0F
		reterr = TDA182I2Write(pObj, 0x0F, 1);
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetRF_Atten_3dB:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetRF_Atten_3dB( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t   	pObj = NULL;
    TDA182I2_ERR_CODE       reterr = tmERR_BAD_PARAMETER;

	/* Get Instance Object */
	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// set value
		pObj->I2CMap.uBx0F.bF.RF_Atten_3dB = uValue;

		// write byte 0x0F
		reterr = TDA182I2Write(pObj, 0x0F, 1);
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetAGC5_Ana:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetAGC5_Ana( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t   	pObj = NULL;
    TDA182I2_ERR_CODE       reterr = tmERR_BAD_PARAMETER;

	/* Get Instance Object */
	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// set value
		pObj->I2CMap.uBx11.bF.AGC5_Ana = uValue;

		// write byte 0x11
        reterr = TDA182I2Write(pObj, 0x11, 1);
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetPSM_StoB:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetPSM_StoB( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t   	pObj = NULL;
    TDA182I2_ERR_CODE       reterr = tmERR_BAD_PARAMETER;

	/* Get Instance Object */
	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// set value
		pObj->I2CMap.uBx1B.bF.PSM_StoB = uValue;

		// read byte 0x1B
		reterr = TDA182I2Write(pObj, 0x1B, 1);
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetAGCK_Step:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetAGCK_Step( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t   	pObj = NULL;
    TDA182I2_ERR_CODE       reterr = tmERR_BAD_PARAMETER;

	/* Get Instance Object */
	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// set value
		pObj->I2CMap.uBx0E.bF.AGCK_Step = uValue;

		// write byte 0x0E
		reterr = TDA182I2Write(pObj, 0x0E, 1);
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetIF_Freq:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetIF_Freq( HUINT32 tUnit, HUINT32 uValue)
{
	pTDA182I2Object_t   	pObj = NULL;
    TDA182I2_ERR_CODE       reterr = tmERR_BAD_PARAMETER;

	/* Get Instance Object */
	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{

		// set value
		pObj->I2CMap.uBx15.bF.IF_Freq = (HUINT8)(uValue / 50000);

		// write byte 0x15
		reterr = TDA182I2Write(pObj, 0x15, 1);
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetLPF_Gain_Mode:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetLPF_Gain_Mode( HUINT32 tUnit, HUINT8 uMode)
{
	pTDA182I2Object_t   	pObj = NULL;
    TDA182I2_ERR_CODE       reterr = tmERR_BAD_PARAMETER;

	// pObj initialization
	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		switch(uMode)
		{
			case TDA182I2_LPF_Gain_Unknown:
			default:
				break;

			case TDA182I2_LPF_Gain_Free:
				reterr = TDA182I2SetAGC5_loop_off(tUnit, FALSE); /* Disable AGC5 loop off */
				if(reterr == tmNOERR)
				{
					reterr = TDA182I2SetForce_AGC5_gain(tUnit, FALSE); /* Do not force AGC5 gain */
				}
				break;

			case TDA182I2_LPF_Gain_Frozen:
				reterr = TDA182I2SetAGC5_loop_off(tUnit, TRUE); /* Enable AGC5 loop off */
				if(reterr == tmNOERR)
				{
					reterr = TDA182I2SetForce_AGC5_gain(tUnit, TRUE); /* Force AGC5 gain */
				}

				if(reterr == tmNOERR)
				{
					reterr = TDA182I2SetAGC5_Gain(tUnit, 0);  /* Force gain to 0dB */
				}
				break;
			}

	}

	return reterr;
}
// REV 1.3.1
/********************************************************************
 * FUNCTION:	TDA182I2SetDigital_Clock_Mode:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetDigital_Clock_Mode( HUINT32 tUnit, HUINT8 uValue)
{
	pTDA182I2Object_t   	pObj = NULL;
	TDA182I2_ERR_CODE       reterr = tmERR_BAD_PARAMETER;
	//HUINT8*					pI2CMap;

	/* Get Instance Object */
	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// set value
		pObj->I2CMap.uBx14.bF.Digital_Clock_Mode = uValue;

		// write byte 0x14
		reterr = TDA182I2Write(pObj, 0x14, 1);
	}

	return reterr;
}
// REV 1.3.1
/********************************************************************
 * FUNCTION:	TDA182I2SetAGCK_Mode:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetAGCK_Mode( HUINT32  tUnit,		   HUINT8 uValue)
{
	pTDA182I2Object_t   	pObj = NULL;
    TDA182I2_ERR_CODE       reterr = tmERR_BAD_PARAMETER;

	/* Get Instance Object */
	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		// set value
		pObj->I2CMap.uBx0E.bF.AGCK_Mode = uValue;

		// write byte 0x0E
		reterr = TDA182I2Write(pObj, 0x0E, 1);
	}

	return reterr;
}
// REV 1.3.1
/********************************************************************
 * FUNCTION:	TDA182I2SetPowerState:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetPowerState( HUINT32 tUnit, TDA182I2PowerState_t powerState)
{
	pTDA182I2Object_t   	pObj = NULL;
    TDA182I2_ERR_CODE       reterr = tmERR_BAD_PARAMETER;

	/* Get Instance Object */
	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
        /* read bytes 0x06-0x14 */
		reterr = TDA182I2Read(pObj, 0x06, 15);	//driver 1.3.1
		/* set digital clock mode*/
		switch (powerState)
		{
			case tmTDA182I2_PowerStandbyWithLNAOnAndWithXtalOnAndSyntheOn:
			case tmTDA182I2_PowerStandbyWithLNAOnAndWithXtalOn:
			case tmTDA182I2_PowerStandbyWithXtalOn:
			case tmTDA182I2_PowerStandby:
				/* set 16 Mhz xtal clock*/
				reterr = TDA182I2SetDigital_Clock_Mode(tUnit, 0);

				/*---------------------------------------------------------ES2 PATCH*/
				if(reterr == tmNOERR && pObj->I2CMap.uBx02.bF.Major_rev == 1 && pObj->I2CMap.uBx02.bF.Minor_rev == 0) //driver 1.3.2
				{
					reterr = TDA182I2SetAGCK_Mode(tUnit, 0x2); /* RSSI disable for analog PR/CR 165 */
					if(reterr != tmNOERR)
					{
						CH_DI_Print(6, ("[TDA182I2SetPowerState]SetAGCKMode Error\n"));
						return tmERR_SET_FAILED;
					}
				}
				/*----------------------------------------------------ES2 PATCH END*/
				break;
			default:
				break;
		}
	}

		/* set power state*/
	if(reterr == tmNOERR)
	{
		switch (powerState)
		{
			case tmTDA182I2_PowerNormalMode:
				////PrintDebug("[TDA182I2SetPowerState] PowerNormalMode\n ");
				pObj->I2CMap.uBx06.bF.SM = 0x00;
				pObj->I2CMap.uBx06.bF.SM_Synthe = 0x00;
				pObj->I2CMap.uBx06.bF.SM_LT = 0x00;
				pObj->I2CMap.uBx06.bF.SM_XT = 0x00;
				pObj->curPower = powerState;
				break;

			case tmTDA182I2_PowerStandbyWithLNAOnAndWithXtalOnAndSyntheOn:
				////PrintDebug("[TDA182I2SetPowerState] PowerStandby\n ");
				pObj->I2CMap.uBx06.bF.SM = 0x01;
				pObj->I2CMap.uBx06.bF.SM_Synthe = 0x00;
				pObj->I2CMap.uBx06.bF.SM_LT = 0x00;
				pObj->I2CMap.uBx06.bF.SM_XT = 0x00;
				pObj->curPower = powerState;
				break;

			case tmTDA182I2_PowerStandbyWithLNAOnAndWithXtalOn:
				////PrintDebug("[TDA182I2SetPowerState] PowerStandby\n ");
				pObj->I2CMap.uBx06.bF.SM = 0x01;
				pObj->I2CMap.uBx06.bF.SM_Synthe = 0x01;
				pObj->I2CMap.uBx06.bF.SM_LT = 0x00;
				pObj->I2CMap.uBx06.bF.SM_XT = 0x00;
				pObj->curPower = powerState;
				break;

			case tmTDA182I2_PowerStandbyWithXtalOn:
				////PrintDebug("[TDA182I2SetPowerState] PowerStandby\n ");
				pObj->I2CMap.uBx06.bF.SM = 0x01;
				pObj->I2CMap.uBx06.bF.SM_Synthe = 0x01;
				pObj->I2CMap.uBx06.bF.SM_LT = 0x01;
				pObj->I2CMap.uBx06.bF.SM_XT = 0x00;
				pObj->curPower = powerState;
				break;

			case tmTDA182I2_PowerStandby:
				////PrintDebug("[TDA182I2SetPowerState] PowerStandby\n ");
				pObj->I2CMap.uBx06.bF.SM = 0x01;
				pObj->I2CMap.uBx06.bF.SM_Synthe = 0x01;
				pObj->I2CMap.uBx06.bF.SM_LT = 0x01;
				pObj->I2CMap.uBx06.bF.SM_XT = 0x01;
				pObj->curPower = powerState;
				break;

			default:
				/* power state not supported*/
				return tmERR_NOT_SUPPORTED;
		}

		/* write byte 0x06*/

		reterr = TDA182I2Write(pObj, 0x06, 1);
	}

	/* set digital clock mode*/
	if(reterr == tmNOERR)
	{
		switch (powerState)
		{
			case tmTDA182I2_PowerNormalMode:
				/* set sigma delta clock*/
				reterr = TDA182I2SetDigital_Clock_Mode(tUnit, 1);
				if(reterr != tmNOERR)
				{
					CH_DI_Print(0, ("[TDA182I2SetPowerState]SetDigital_ClockMode Error\n"));
					return tmERR_SET_FAILED;
				}

				//pObj->curPower = tmPowerOn;
				break;

			default:
				break;
		}
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2RFFilterCorrTune:
 * DESCRIPTION:	driver 1.3.1
 * RETURN:
********************************************************************/

TDA182I2_ERR_CODE TDA182I2RFFilterCorrTune( HUINT32 tUnit, HUINT32 uRF)
{
	pTDA182I2Object_t   	pObj = NULL;
    TDA182I2_ERR_CODE       reterr = tmERR_BAD_PARAMETER;

    HUINT32					uCounter = 0, uRFCal0 = 0, uRFCal1 = 0;
    HUINT8                  idx = 0, uSubBand = 0, gainTapper = 0, RFBand = 0;
    HINT32                  cProg = 0, RF_A1 = 0, RF_B1 = 0;

	/* Get Instance Object */
	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
        // read byte 0x26-0x2B
        reterr = TDA182I2Read(pObj, 0x26, 6);
        if(reterr == tmNOERR)
        {
            reterr = TDA182I2GetRFCalFreq(pObj->tUnit, 1, &uRFCal0, NULL);
        }

        if(reterr == tmNOERR)
        {
            reterr = TDA182I2GetRFCalFreq(pObj->tUnit, 4, &uRFCal1, NULL);
        }

        if(reterr == tmNOERR)
        {
            if(uRF < uRFCal0)
                uSubBand = 0;
            else if(uRF < 145700000)
                uSubBand = 1;
            else if(uRF < uRFCal1)
                uSubBand = 2;
            else if(uRF < 367400000)
                uSubBand = 3;
            else
            {
                reterr = TDA182I2GetRFCalFreq(pObj->tUnit, 7, &uRFCal0, NULL);

				if(reterr == tmNOERR)
                {
                    reterr = TDA182I2GetRFCalFreq(pObj->tUnit, 10, &uRFCal1, NULL);
                }

				if(reterr == tmNOERR)
                {
                    if(uRF < uRFCal0)
                        uSubBand = 4;
                    else if(uRF < 625000000)
                        uSubBand = 5;
                    else if(uRF < uRFCal1)
                        uSubBand = 6;
                    else
                        uSubBand = 7;
                }
            }
        }

        if(reterr == tmNOERR)
        {
            idx = pObj->RFCAL_Coeffs[uSubBand].Cal_number;
            reterr = TDA182I2GetRFCalFreq(pObj->tUnit, idx, &uRFCal0, NULL);
        }

        if(reterr == tmNOERR)
        {
            RF_A1 = pObj->RFCAL_Coeffs[uSubBand].RF_A1;
            RF_B1 = pObj->RFCAL_Coeffs[uSubBand].RF_B1;

            uCounter = 0;
            do uCounter ++;
            while (uRF >= TDA182I2_Freq_ROM_Map[uCounter].uRF_Max && uCounter < TDA182I2_FREQ_ROM_MAP_ROW);

            cProg = TDA182I2_Freq_ROM_Map[uCounter - 1].uCprog;
            gainTapper = TDA182I2_Freq_ROM_Map[uCounter - 1].uGainTapper;
            RFBand = TDA182I2_Freq_ROM_Map[uCounter - 1].uRFBand;

            cProg = (HINT32)(cProg + RF_B1 + (RF_A1*((HINT32)(uRF - uRFCal0)/1000))/1000000);

            if(cProg>255)   cProg = 255;
            if(cProg<0)     cProg = 0;

            reterr = TDA182I2SetRF_Filter_Bypass(pObj->tUnit, 1);
        }

        if(reterr == tmNOERR)
        {
            reterr = TDA182I2SetRF_Filter_Cap(tUnit, (HUINT8)cProg);
        }

        if(reterr == tmNOERR)
        {
            reterr = TDA182I2SetGain_Taper(tUnit, gainTapper);
        }

        if(reterr == tmNOERR)
        {
            reterr = TDA182I2SetRF_Filter_Band(tUnit, RFBand);
        }

    }

    return reterr;
}


/********************************************************************
 * FUNCTION:	TDA182I2SetRF_Freq:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetRF_Freq( HUINT32 tUnit, HUINT32 uValue)
{
	pTDA182I2Object_t   	pObj = NULL;
    TDA182I2_ERR_CODE       reterr = tmERR_BAD_PARAMETER;

	/* driver 1.3.1 */
	HUINT32					uRF = 0;
	/* ********** */

	CH_DI_Print(5, ("[TDA182I2SetRF_Freq]RF_Freq = %d \n", uValue));

	/* Get Instance Object */
	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		//***************************************
		// Configures the settings that depends on the chosen received TV standard
		// (standard and demodulator dependant)

		//pObj->I2CMap.uBx0E.AGCK_byte_1 = 0x1F;
		//pObj->I2CMap.uBx0F.RF_AGC_byte = 0x15;
		//pObj->I2CMap.uBx10.IR_Mixer_byte_1 = 0x0A;
		//pObj->I2CMap.uBx11.AGC5_byte_1 = 0x0A;
		//pObj->I2CMap.uBx12.IF_AGC_byte = 0x00;
		//pObj->I2CMap.uBx13.IF_Byte_1 = 0xE0;
		//pObj->I2CMap.uBx14.Reference_Byte = 0x03;
		//pObj->I2CMap.uBx15.IF_Frequency_byte = 0x00;

		//// write bytes 0x0E to 0x15
		//err = ddTDA182I2Write(pObj, 0x0E, 8);
		//tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "ddTDA182I2Write(0x%08X) failed.", tUnit));
		//***************************************

		/*****************************************/
		/* Tune the settings that depend on the RF input frequency, expressed in kHz.*/
		/* RF filters tuning, PLL locking*/
		/* State reached after 5ms*/

		if(reterr == tmNOERR)
		{
			/* set IRQ_clear*/
			reterr = TDA182I2SetIRQ_Clear(tUnit, 0x0C);
		}

		if(reterr == tmNOERR)
		{
			/* set power state on*/
			reterr = TDA182I2SetPowerState(tUnit, tmTDA182I2_PowerNormalMode);
		}

		if(reterr == tmNOERR)
		{
			/* set RF frequency expressed in kHz*/
			uRF = uValue / 1000;
			if ( pObj->I2CMap.uBx02.bF.Major_rev == 1 && pObj->I2CMap.uBx02.bF.Minor_rev == 0  )
			{
			pObj->I2CMap.uBx16.bF.RF_Freq_1 = (HUINT8)((uRF & 0x00FF0000) >> 16);
			pObj->I2CMap.uBx17.bF.RF_Freq_2 = (HUINT8)((uRF & 0x0000FF00) >> 8);
			pObj->I2CMap.uBx18.bF.RF_Freq_3 = (HUINT8)(uRF & 0x000000FF);
			}
			else
			{
				pObj->I2CMap.uBx16.bF.RF_Freq_1 = (HUINT8)((uRF & 0x00FF0000) >> 16);
				pObj->I2CMap.uBx18.bF.RF_Freq_3 = (HUINT8)((uRF & 0x0000FF00) >> 8);
				pObj->I2CMap.uBx17.bF.RF_Freq_2 = (HUINT8)(uRF & 0x000000FF);
			}

			/* write bytes 0x16 to 0x18*/
			reterr = TDA182I2Write(pObj, 0x16, 3);
		}

		if(reterr == tmNOERR)
		{
			/* set & trigger MSM*/
			pObj->I2CMap.uBx19.MSM_byte_1 = 0x41;
			pObj->I2CMap.uBx1A.MSM_byte_2 = 0x01;

			/* write bytes 0x19 to 0x1A*/
			reterr = TDA182I2Write(pObj, 0x19, 2);
			pObj->I2CMap.uBx1A.MSM_byte_2 = 0x00;
			if (reterr)
				CH_DI_Print(0, ("[TDA182I2SetRF_Freq]TDA182I2Write error \n"));
		}

		if(pObj->bIRQWait)
		{
			if(reterr == tmNOERR)
			{
				reterr = TDA182I2WaitIRQ(tUnit, 50, 5, 0x0C);
			}
			/* PATCH ES2 in case of WAIT IRQ Timeout */
			if (reterr == tmERR_NOT_READY && pObj->I2CMap.uBx02.bF.Major_rev == 1 && pObj->I2CMap.uBx02.bF.Minor_rev == 0  )
			{
				/* driver 1.3.1 */
				pObj->bSoftReset = 1;
				//reterr = tmERR_NOT_READY;
				CH_DI_Print(0, ("[TDA182I2SetRF_Freq]TDA182I2WaitIRQ Error\n"));
				/* ********** */
			}

			if(reterr == tmNOERR && pObj->bRFCAL_SW_Algo_Enable)
			{
				reterr= TDA182I2RFFilterCorrTune(tUnit, uValue);
			}

		}
		else
			CH_DI_Print(0, ("[TDA182I2SetRF_Freq]bResetWait disables\n"));

	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetXtalCal_Launch
 * DESCRIPTION:	Driver 1.4.0 Patch
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetXtalCal_Launch( HUINT32 tUnit )
{
	pTDA182I2Object_t		pObj = NULL;
	TDA182I2_ERR_CODE		reterr = tmERR_BAD_PARAMETER;

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		/* Set value */
		pObj->I2CMap.uBx1A.bF.XtalCal_Launch = 1;

		/* Read byte 0x1A */
		reterr = TDA182I2Write(pObj, 0x1A, 1);


		pObj->I2CMap.uBx1A.bF.XtalCal_Launch = 0;
	}

	return reterr;
}


/********************************************************************
 * FUNCTION:	TDA182I2Reset:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2Reset( HUINT32 tUnit)
{
	pTDA182I2Object_t   	pObj = NULL;
	TDA182I2_ERR_CODE       reterr = tmERR_BAD_PARAMETER;

	/* driver 1.3.1 */
	HUINT8                  uSubAddress = 0x00;
	//unsigned char			bIRQWait = 1;
	HUINT8                  uData[2] = { 0x00, 0x00 };
	/* ********* */

	/* driver 1.3.2 */
	/*---------------------------------------------------------ES2 PATCH*/
	HUINT8               	majorRevision = 0;
	HUINT8               	minorRevision = 0;
	/* ********* */

	reterr = TDA182I2GetInstance(tUnit, &pObj);

	/* driver 1.3.1 */
	//TDA182I2GetIRQWait(tUnit, &bIRQWait); //중복되는 code 라서 삭제함.
	/* ********* */

	/* driver 1.3.2 */
	/*---------------------------------------------------------ES2 PATCH*/
	if(reterr == tmNOERR)
	{
		/* driver 1.3.1 */
	        if(pObj->bIRQWait)
	        {
	        	CH_DI_Print(2, ("[TDA182I2Reset]bIRQWait enable \n"));
			reterr = TDA182I2WaitXtalCal_End( pObj->tUnit, 100, 5);
			#if 0
			// shkim2 09.10.31
			if ( reterr == tmERR_NOT_READY )
			{
				reterr = TDA182I2SetXtalCal_Launch( pObj->tUnit );
				CH_DI_Print(2, ("[TDA182I2Reset]SetXtalCal_Launch \n"));

				if(reterr == tmNOERR)
				{
					reterr = TDA182I2WaitXtalCal_End( pObj->tUnit, 100, 5);
					if(reterr == tmNOERR)
						CH_DI_Print(2, ("[TDA182I2Reset]WaitXtalCal_End retry success \n"));
					else
						CH_DI_Print(0, ("[TDA182I2Reset]WaitXtalCal_End Fail \n"));
				}
				else
					CH_DI_Print(0, ("[TDA182I2SetRF_Freq]SetXtalCal_Launch Error \n"));
			}
			#endif

			if ( reterr == tmERR_NOT_READY )
			{
				// Patch
				reterr = tmNOERR;
				pObj->bSoftReset = TRUE;

				CH_DI_Print(0, ("[TDA182I2WaitXtalCal_End]tmERR_NOT_READY \n"));
			}

	        }

	        if(pObj->bSoftReset)
	        {
	        	CH_DI_Print(2, ("[TDA182I2Reset]bSoftReset enable \n"));

			#if 0	//driver 1.3.2
	        	if(reterr == tmNOERR && majorRevision == 1 && minorRevision == 0)
		        {   /* Backup Xtal Cal */
		            uSubAddress = 0x65;
					reterr = drv_ch_i2c_TUNERRead( pObj->tUnit, uSubAddress, &uData[0], 2);
		            uData[0] &= 0xF0;
		            uData[1] &= 0x0F;
		            uData[0] |= uData[1] + 0x80;
			}
			#endif

			/* Soft_Reset */
			uSubAddress = 0x60;
			uData[1] = 0x2;
			reterr = drv_ch_i2c_TUNERWrite( pObj->tUnit, uSubAddress, &uData[1], 1);

			uData[1] = 0x3;
			reterr = drv_ch_i2c_TUNERWrite( pObj->tUnit, uSubAddress, &uData[1], 1);

			VK_TASK_Sleep(100);

			uData[1] = 0x00;
			reterr = drv_ch_i2c_TUNERWrite( pObj->tUnit, uSubAddress, &uData[1], 1);

			#if 0	//driver 1.3.2
			if(reterr == tmNOERR && majorRevision == 1 && minorRevision == 0)
			{
			    /* Restore Xtal Cal */
			    uSubAddress = 0x65;
				reterr = drv_ch_i2c_TUNERWrite( pObj->tUnit, uSubAddress, &uData[0], 1);
			}
			#endif


			reterr = TDA182I2SetXtalCal_Launch( pObj->tUnit );
			CH_DI_Print(2, ("[TDA182I2Reset]SetXtalCal_Launch \n"));

			if(reterr == tmNOERR)
			{
				reterr = TDA182I2WaitXtalCal_End( pObj->tUnit, 100, 5);
				if(reterr == tmERR_NOT_READY)
				{
					CH_DI_Print(0, ("[TDA182I2Reset]WaitXtalCal_End Fail \n"));
					drv_ch_i2c_TUNERDump( pObj->tUnit );
				}
				else
					CH_DI_Print(2, ("[TDA182I2WaitXtalCal_End] success \n"));
			}
			else
				CH_DI_Print(0, ("[TDA182I2SetRF_Freq]SetXtalCal_Launch Error \n"));

			pObj->bSoftReset = 0;

		}

		if(reterr == tmNOERR)
		{
			/* read all bytes */
			CH_DI_Print(5, ("[TDA182I2Reset] into(%d)\n", tUnit));
			reterr = TDA182I2Read(pObj, 0x00, TDA182I2_I2C_MAP_NB_BYTES);
		}

		/*-------------------------------------ES2 I2C map initialization : begin*/

		if(reterr == tmNOERR)
		{
			reterr = TDA182I2GetMajorRevision(tUnit, &majorRevision);
			reterr = TDA182I2GetMinorRevision(tUnit, &minorRevision);

			CH_DI_Print(2, ("[TDA182I2Reset]majorRevision= %d minorRevision= %d\n", majorRevision, minorRevision));
		}

	        if(reterr == tmNOERR && majorRevision == 1 && minorRevision == 0)
		{
			if(reterr == tmNOERR)
			{
						/* CP Div change  			77					  0x4D*/
				reterr = TDA182I2SetN_CP_Current(tUnit, 0x4D);
			}

			if(reterr == tmNOERR)
			{
				/* AGC5 DN time constant	1,022 ms	3 */
				reterr = TDA182I2SetAGC5_Do_step(tUnit, 3);
			}

			if(reterr == tmNOERR)
			{
			   /* AGCs Up time constant    Fast : 262,131,65,65ms  1 */
				reterr = TDA182I2SetAGCs_Up_Step(tUnit, 1);
			}

			if(reterr == tmNOERR)
			{
				/* AGC Overload TOP AGC1 TOP DN + 6 / 110 / 85	7 */
				reterr = TDA182I2SetAGC_Ovld_TOP(tUnit, 7);
			}

			#if 0	//driver 1.3.2
			if(reterr == tmNOERR)
			{
			    /* IR loop		7		7 */	//driver 1.3.2
			    reterr = TDA182I2SetIR_Loop(tUnit, 7);
			}

			if(reterr == tmNOERR)
			{
				/* IR target	65 dB	5->7 */	//driver 1.3.2
				reterr = TDA182I2SetIR_Target(tUnit, 7);
			}
			if(reterr == tmNOERR)
			{
			    /* IR corr boost    0    0 */	//driver 1.3.2
			    reterr = TDA182I2SetIR_Corr_Boost(tUnit, 0);
			}
			#endif

			if(reterr == tmNOERR)
			{
				/* IR target	65 dB	5 */
				reterr = TDA182I2SetIR_Target(tUnit, 5);
			}

			if(reterr == tmNOERR)
			{
				/* IR output calc	calc	1 */
				reterr = TDA182I2SetIR_mode_ram_store(tUnit, 1);
			}

			//#if 0	//driver 1.3.2
			if(reterr == tmNOERR)
			{
				/* RSSI_Ck_Speed	31,25 kHz	0 */
				reterr = TDA182I2SetRSSI_Ck_Speed(tUnit, 0);
			}
			//#endif

			if(reterr == tmNOERR) /* PD Underload not active */
			{
				reterr = TDA182I2SetPD_Udld(tUnit, 0x1);
			}

			if(reterr == tmNOERR) /* force PHI2 to -0.25 instead of +0.25 */
			{
				reterr = TDA182I2SetRFCAL_Phi2(tUnit, (HUINT8) 1);
			}

							/* driver 1.3.1 */
			if(reterr == tmNOERR) /*Disable RF Filter Bypass */
			{
				reterr = TDA182I2SetRF_Filter_Bypass(tUnit, 0);
			}

			#if 0
			/* driver 1.3.2 */
			if(reterr == tmNOERR)
			{
				/* Move IF_Reg from 2.8V to 3V */
				uSubAddress = 0x58;
				uData = 0x65;
				reterr = drv_ch_i2c_TUNERWrite( pObj->tUnit, uSubAddress, &uData, 1);
			}

			/* Enable RFCAL SW Algorithm */
			pObj->bRFCAL_SW_Algo_Enable = 1;
			#endif

		}
		else
		{
			if(reterr == tmNOERR)
			{
				/* RSSI_Ck_Speed	31,25 kHz	0 */
				reterr = TDA182I2SetRSSI_Ck_Speed(tUnit, 0);
			}
		}
		/*--------------------------------------ES2 I2C map initialization : end*/
		/* --------------------------------------------------- ES2 PATCH END*/

		if(reterr == tmNOERR)
		{
			/* set IRQ_clear */
				reterr = TDA182I2SetIRQ_Clear(tUnit, 0x1F);
		}

		if(reterr == tmNOERR)
		{
			/* set power state on */
			reterr = TDA182I2SetPowerState(tUnit, tmTDA182I2_PowerNormalMode);
		}

		if(reterr == tmNOERR)
		{
			/* set & trigger MSM */
			pObj->I2CMap.uBx19.MSM_byte_1 = 0x3B;
			pObj->I2CMap.uBx1A.MSM_byte_2 = 0x01;

			/* write bytes 0x19 to 0x1A */
			reterr = TDA182I2Write(pObj, 0x19, 2);
			pObj->I2CMap.uBx1A.MSM_byte_2 = 0x00;

		}

		if(pObj->bIRQWait)
		{
				/* driver 1.3.1 */
			CH_DI_Print(5, ("[TDA182I2Read] bIRQWait enble\n"));
			if(reterr == tmNOERR)
			{
				reterr = TDA182I2WaitIRQ(tUnit, 1500, 50, 0x1F);
			}


			/*---------------------------------------------------------ES2 PATCH*/
			/* PATCH ES2 in case of WAIT IRQ Timeout */
		        if(reterr == tmNOERR && majorRevision == 1 && minorRevision == 0)
			{
				pObj->bSoftReset = 1;
			}
			/*-----------------------------------------------------ES2 PATCH END*/

			if(reterr == tmNOERR && pObj->bRFCAL_SW_Algo_Enable)
			{
				reterr = TDA182I2RFFilterCorrInit(tUnit);
			}
		}

		/*------------------------------------------------------settings V3.0.1 */
		//#ifdef TMBSL_TDA18272	//driver 1.3.2
		/* LT_Enable	N	0*/
		if(reterr == tmNOERR)
		{
			reterr = TDA182I2SetLT_Enable(tUnit, 1 );
		}
		/* PSM AGC1	-5,9 mA	1*/
		if(reterr == tmNOERR)
		{
			reterr = TDA182I2SetPSM_AGC1(tUnit, 1 );
		}

		/* AGC1 gain range	[-12, +15dB]	0*/
		if(reterr == tmNOERR)
		{
			reterr = TDA182I2SetAGC1_6_15dB(tUnit, 0 );
		}
		//#endif
			/*--------------------------------------------------end settings V3.0.1 */
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetStandardMode:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetStandardMode( HUINT32 tUnit, TDA182I2StandardMode_t StandardMode)
{
	pTDA182I2Object_t   	pObj = NULL;
    TDA182I2_ERR_CODE       reterr = tmERR_BAD_PARAMETER;

	// pObj initialization
	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		pObj->StandardMode = StandardMode;
		CH_DI_Print(5, ("[TDA182I2SetStandardMode]StandardMode = 0x%x\n",StandardMode));
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetRf:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetRf ( HUINT32 tUnit, HUINT32 uRF)
{
	pTDA182I2Object_t   	pObj = NULL;
    TDA182I2_ERR_CODE       reterr = tmERR_BAD_PARAMETER;

	/* driver 1.3.1 */
	HUINT8					bIRQWait = 1;
	/* driver 1.3.2 */
	/*---------------------------------------------------------ES2 PATCH*/
    HUINT8               	majorRevision = 0;
    HUINT8               	minorRevision = 0;
	/*-----------------------------------------------------Spurious PATCH*/
    HUINT8   				ratioL, ratioH;
    HUINT8  				DeltaL, DeltaH;
    /* ********** */

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		/* driver 1.3.1 */
        reterr = TDA182I2GetIRQWait(tUnit, &bIRQWait);
		/* ********* */

		pObj->uRF = uRF;

		CH_DI_Print(5, ("[TDA182I2SetRf]LP_FC = 0x%x \n", pObj->Std_Array[pObj->StandardMode].LPF));
		if(reterr == tmNOERR)
		{
		/* Set LPF */
		reterr = TDA182I2SetLP_FC(tUnit, pObj->Std_Array[pObj->StandardMode].LPF);
		}
		if(reterr == tmNOERR)
		{
			/* Set LPF Offset */
			reterr = TDA182I2SetLP_FC_Offset(tUnit, pObj->Std_Array[pObj->StandardMode].LPF_Offset);
		}

		CH_DI_Print(5, ("[TDA182I2SetRf]IF_Level = 0x%x \n", pObj->Std_Array[pObj->StandardMode].IF_Gain));
		if(reterr == tmNOERR)
		{
			/* Set IF Gain */
			reterr = TDA182I2SetIF_Level(tUnit, pObj->Std_Array[pObj->StandardMode].IF_Gain);
		}

		CH_DI_Print(5, ("[TDA182I2SetRf]ATSC_Notch = 0x%x \n", pObj->Std_Array[pObj->StandardMode].IF_Notch));
		if(reterr == tmNOERR)
		{
			/* Set IF Notch */
			reterr = TDA182I2SetIF_ATSC_Notch(tUnit, pObj->Std_Array[pObj->StandardMode].IF_Notch);

		}

		if(reterr == tmNOERR)
		{
			/* Enable/disable HPF */
			if ( pObj->Std_Array[pObj->StandardMode].IF_HPF == TDA182I2_IF_HPF_Disabled )
			{
				CH_DI_Print(5, ("[TDA182I2SetRf]HPF_Disabled\n"));
				reterr = TDA182I2SetHi_Pass(tUnit, 0);
			}
			else
			{
				reterr = TDA182I2SetHi_Pass(tUnit, 1);
				if(reterr == tmNOERR)
				{

					CH_DI_Print(5, ("[TDA182I2SetRf]HPF_Enabled\n"));
					/* Set IF HPF */
					reterr = TDA182I2SetIF_HP_Fc(tUnit, (HUINT8)(pObj->Std_Array[pObj->StandardMode].IF_HPF - 1));

				}
			}
		}

		CH_DI_Print(5, ("[TDA182I2SetRf]IF_Notch = 0x%x \n", pObj->Std_Array[pObj->StandardMode].DC_Notch));
		if(reterr == tmNOERR)
		{
			/* Set DC Notch */
			reterr = TDA182I2SetIF_Notch(tUnit, pObj->Std_Array[pObj->StandardMode].DC_Notch);
		}

		CH_DI_Print(5, ("[TDA182I2SetRf]AGC1_TOP = 0x%x \n", pObj->Std_Array[pObj->StandardMode].AGC1_LNA_TOP));
		if(reterr == tmNOERR)
		{
			/* Set AGC1 LNA Top */
			reterr = TDA182I2SetAGC1_TOP(tUnit, pObj->Std_Array[pObj->StandardMode].AGC1_LNA_TOP);
		}

		CH_DI_Print(5, ("[TDA182I2SetRf]AGC2_TOP = 0x%x \n", pObj->Std_Array[pObj->StandardMode].AGC2_RF_Attenuator_TOP));
		if(reterr == tmNOERR)
		{
			/* Set AGC2 RF Top */
			reterr = TDA182I2SetAGC2_TOP(tUnit, pObj->Std_Array[pObj->StandardMode].AGC2_RF_Attenuator_TOP);
		}

		if(reterr == tmNOERR)
		{
			/* Set AGC3 RF AGC Top */
			if (pObj->uRF < tmTDA182I2_AGC3_RF_AGC_TOP_FREQ_LIM)
			{
				reterr = TDA182I2SetRFAGC_Top(tUnit, pObj->Std_Array[pObj->StandardMode].AGC3_RF_AGC_TOP_Low_band); //driver 1.3.2
			}
			else
			{
				reterr = TDA182I2SetRFAGC_Top(tUnit, pObj->Std_Array[pObj->StandardMode].AGC3_RF_AGC_TOP_High_band); //driver 1.3.2
			}
		}

		CH_DI_Print(5, ("[TDA182I2SetRf]Mixer_Top = 0x%x \n", pObj->Std_Array[pObj->StandardMode].AGC4_IR_Mixer_TOP));
		if(reterr == tmNOERR)
		{
			/* Set AGC4 IR Mixer Top */
			reterr = TDA182I2SetIR_Mixer_Top(tUnit, pObj->Std_Array[pObj->StandardMode].AGC4_IR_Mixer_TOP);

		}

		CH_DI_Print(5, ("[TDA182I2SetRf]AGC5_Top = 0x%x \n", pObj->Std_Array[pObj->StandardMode].AGC5_IF_AGC_TOP));
		if(reterr == tmNOERR)
		{
			/* Set AGC5 IF AGC Top */
			reterr = TDA182I2SetAGC5_TOP(tUnit, pObj->Std_Array[pObj->StandardMode].AGC5_IF_AGC_TOP);
		}

		CH_DI_Print(5, ("[TDA182I2SetRf]RFAGC_Adapt = 0x%x \n", pObj->Std_Array[pObj->StandardMode].AGC3_Adapt));
		if(reterr == tmNOERR)
		{
			/* Set AGC3 Adapt */
			reterr = TDA182I2SetPD_RFAGC_Adapt(tUnit, pObj->Std_Array[pObj->StandardMode].AGC3_Adapt);
		}

		CH_DI_Print(5, ("[TDA182I2SetRf]Adapt_TOP = 0x%x \n", pObj->Std_Array[pObj->StandardMode].AGC3_Adapt_TOP));
		if(reterr == tmNOERR)
		{
			/* Set AGC3 Adapt TOP */
			reterr = TDA182I2SetRFAGC_Adapt_TOP(tUnit, pObj->Std_Array[pObj->StandardMode].AGC3_Adapt_TOP);
		}

		CH_DI_Print(5, ("[TDA182I2SetRf]Atten = 0x%x \n", pObj->Std_Array[pObj->StandardMode].AGC5_Atten_3dB));
		if(reterr == tmNOERR)
		{
			/* Set AGC5 Atten 3dB */
			reterr = TDA182I2SetRF_Atten_3dB(tUnit, pObj->Std_Array[pObj->StandardMode].AGC5_Atten_3dB);
		}

		CH_DI_Print(5, ("[TDA182I2SetRf]Ana = 0x%x \n", pObj->Std_Array[pObj->StandardMode].AGC5_Detector_HPF));
		if(reterr == tmNOERR)
		{
			/* Set AGC5 Detector HPF */
			reterr = TDA182I2SetAGC5_Ana(tUnit, pObj->Std_Array[pObj->StandardMode].AGC5_Detector_HPF);
		}

		CH_DI_Print(5, ("[TDA182I2SetRf]AGCK_Mode = 0x%x \n", pObj->Std_Array[pObj->StandardMode].GSK&0x03));
		if(reterr == tmNOERR)
		{
			/* Set AGCK Mode */
			reterr = TDA182I2SetAGCK_Mode(tUnit, pObj->Std_Array[pObj->StandardMode].GSK&0x03);
			reterr = TDA182I2SetAGCK_Step(tUnit, (pObj->Std_Array[pObj->StandardMode].GSK&0x0C)>>2);

		}

		CH_DI_Print(5, ("[TDA182I2SetRf]StoB = 0x%x \n", pObj->Std_Array[pObj->StandardMode].H3H5_VHF_Filter6));
		if(reterr == tmNOERR)
		{
			/* Set H3H5 VHF Filter 6 */
			reterr = TDA182I2SetPSM_StoB(tUnit, pObj->Std_Array[pObj->StandardMode].H3H5_VHF_Filter6);
		}

		CH_DI_Print(5, ("[TDA182I2SetRf]IF_Freq = %d \n", pObj->Std_Array[pObj->StandardMode].IF - pObj->Std_Array[pObj->StandardMode].CF_Offset));
		if(reterr == tmNOERR)
		{
			/* Set IF */
			reterr = TDA182I2SetIF_Freq(tUnit, pObj->Std_Array[pObj->StandardMode].IF - pObj->Std_Array[pObj->StandardMode].CF_Offset);
		}

		CH_DI_Print(5, ("[TDA182I2SetRf]check revision... \n"));
		/* driver 1.3.2 */
		/*---------------------------------------------------------ES2 PATCH*/
		if(reterr == tmNOERR)
		{

            reterr = TDA182I2GetMajorRevision(tUnit, &majorRevision);

			CH_DI_Print(5, ("[TDA182I2SetRf]TDA182I2GetMajorRevision=%d \n", majorRevision));
            reterr = TDA182I2GetMinorRevision(tUnit, &minorRevision);

			CH_DI_Print(5, ("[TDA182I2SetRf]TDA182I2GetMajorRevision=%d \n", majorRevision));
			CH_DI_Print(0, ("[TDA182I2SetRf]majorRevision= %d minorRevision= %d\n", majorRevision, minorRevision));
        }
		CH_DI_Print(5, ("[TDA182I2SetRf]finish revision... \n"));

        if(reterr == tmNOERR && majorRevision == 1 && minorRevision == 0)
        {
			/* Set LPF Gain */
			reterr = TDA182I2SetLPF_Gain_Mode(tUnit, pObj->Std_Array[pObj->StandardMode].LPF_Gain);
			CH_DI_Print(5, ("[TDA182I2SetRf]LPF_Gain = 0x%x \n", pObj->Std_Array[pObj->StandardMode].LPF_Gain));
		}
        /* --------------------------------------------------- ES2 PATCH END*/


		if(reterr == tmNOERR)
		{
			/* Set RF */
			reterr = TDA182I2SetRF_Freq(tUnit, uRF + pObj->Std_Array[pObj->StandardMode].CF_Offset);

			/* driver 1.3.1 */
			/*---------------------------------------------------------ES2 PATCH*/
			if ( reterr == tmERR_NOT_READY && bIRQWait && majorRevision == 1 && minorRevision == 0)  		// driver 1.3.2
			{
				CH_DI_Print(0, (" [TDA182I2SetRf] ERR_NOT_READY (0x%x)\n",reterr));
				reterr = TDA182I2Reset(tUnit);
				reterr = TDA182I2SetRF_Freq(tUnit, uRF + pObj->Std_Array[pObj->StandardMode].CF_Offset);	// driver 1.3.1
			}
			/* --------------------------------------------------- ES2 PATCH END*/
		}


        if(reterr == tmNOERR && majorRevision == 1 && minorRevision == 0)
        {
        }
		else
		{
		/* driver 1.3.2*/
		/*-----------------------------------------------------Spurious PATCH*/
			if(reterr == tmNOERR)
			{
				/*	patch Spurious begin */
				ratioL = (HUINT8)(uRF / 16000000);
				ratioH = (HUINT8)(uRF / 16000000) + 1;
				DeltaL = (uRF - (ratioL * 16000000));
				DeltaH = ((ratioH * 16000000) - uRF);

				if (uRF < 72000000 )
				{
					/* Set sigma delta clock*/
					reterr = TDA182I2SetDigital_Clock_Mode(tUnit, 1);
				}
				else if (uRF < 104000000 )
				{
					/* Set 16 Mhz Xtal clock */
					reterr = TDA182I2SetDigital_Clock_Mode(tUnit, 0);
				}
				else if (uRF <= 120000000 )
				{
					/* Set sigma delta clock*/
					reterr = TDA182I2SetDigital_Clock_Mode(tUnit, 1);
				}
				else  /* RF above 120 MHz */
				{

					if(DeltaL <= DeltaH )
					{
						if (ratioL & 0x000001 )  /* ratioL odd */
						{
							/* Set 16 Mhz Xtal clock */
							reterr = TDA182I2SetDigital_Clock_Mode(tUnit, 0);
							CH_DI_Print(5, ("[TDA182I2SetRf]Set 16 Mhz Xtal clock \n"));
						}
						else /* ratioL even */
						{
							/* Set sigma delta clock*/
							reterr = TDA182I2SetDigital_Clock_Mode(tUnit, 1);
							CH_DI_Print(5, ("[TDA182I2SetRf]Set sigma delta clock \n"));
						}

					}
					else  /* (DeltaL > DeltaH ) */
					{
						if (ratioL & 0x000001 )  /*(ratioL odd)*/
						{
							/* Set sigma delta clock*/
							reterr = TDA182I2SetDigital_Clock_Mode(tUnit, 1);
							CH_DI_Print(5, ("[TDA182I2SetRf]Set sigma delta clock \n"));
						}
						else
						{
							/* Set 16 Mhz Xtal clock */
							reterr = TDA182I2SetDigital_Clock_Mode(tUnit, 0);
							CH_DI_Print(5, ("[TDA182I2SetRf]Set 16 Mhz Xtal clock \n"));
						}
					}
				}
			}
		/*------------------------------------------------ Spurious PATCH END*/
		}
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2GetLockStatus
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2GetLockStatus(       HUINT32 tUnit, TDA182I2FrontEndState_t* pLockStatus)
{
	pTDA182I2Object_t   	pObj = NULL;
    TDA182I2_ERR_CODE       reterr = tmERR_BAD_PARAMETER;
	HUINT8 					uValue, uValueLO;

	if(pLockStatus == NULL)
		return tmERR_BAD_PARAMETER;

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		reterr = TDA182I2GetLO_Lock(tUnit, &uValueLO);
		CH_DI_Print(3, ("[TDA182I2GetLockStatus]LO_Lock (%d)\n", uValueLO));
	}

	if(reterr == tmNOERR)
	{
		reterr = TDA182I2GetIRQ_status(tUnit, &uValue);

		CH_DI_Print(3, ("[TDA182I2GetLockStatus]IRQ_status (%d)\n", uValue));
		uValue = uValue & uValueLO;
	}
	if(reterr == tmNOERR)
	{
		*pLockStatus =	(uValue)? tmFrontEndStateLocked : tmFrontEndStateNotLocked;
	}
	else
	{
		*pLockStatus = tmFrontEndStateUnknown;
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SetFrequency:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SetFrequency( HUINT32 tUnit, pTDA182I2Request_t pTuneRequest)
{
	pTDA182I2Object_t   	pObj = NULL;
	TDA182I2_ERR_CODE       reterr = tmERR_BAD_PARAMETER;
	TDA182I2FrontEndState_t	eTunerLock = tmFrontEndStateUnknown;
	//unsigned char ucInputLevel = 0;

	CH_DI_Print(5, ("[TDA182I2SetFrequency] into from send request \n"));

	reterr = TDA182I2GetInstance(tUnit, &pObj);
	if(reterr == tmNOERR)
	{
		CH_DI_Print(5, ("[TDA182I2SetFrequency](0x%x)\n", pObj->tUnit));
		CH_DI_Print(5, ("[TDA182I2SetFrequency]Frequency: %d Hz\n", pTuneRequest->dwFrequency));
		CH_DI_Print(5, ("[TDA182I2SetFrequency]Standard: %d \n", pTuneRequest->dwStandard));
	}

	if(reterr == tmNOERR && pObj->lastTuneReqType != TRT_TUNER_ONLY)
	{
		reterr = TDA182I2SetPowerState(pObj->tUnit, tmTDA182I2_PowerNormalMode);
		if(reterr == tmNOERR)
		{
			pObj->curPower = tmPowerOn;
			pObj->lastTuneReqType = TRT_TUNER_ONLY;
		}
		else
		{
			CH_DI_Print(0, ("[TDA182I2SetFrequency]PowerState Error\n"));
			return tmERR_SET_FAILED;
		}
	}

	if(reterr == tmNOERR)
	{
		reterr = TDA182I2SetStandardMode(pObj->tUnit, pTuneRequest->dwStandard);
		if(reterr != tmNOERR)
		{
			CH_DI_Print(0, ("[TDA182I2SetStandardMode]StandardMode Error\n"));
			return tmERR_SET_FAILED;
		}
	}

	if(reterr == tmNOERR)
	{
		/* Set Tuner RF */
		reterr = TDA182I2SetRf (pObj->tUnit, pTuneRequest->dwFrequency);
		if(reterr != tmNOERR)
		{
			CH_DI_Print(0, ("[TDA182I2SetFrequency]Set RF Error\n"));
			return tmERR_SET_FAILED;
		}
	}

	if(reterr == tmNOERR)
	{
		CH_DI_Print(5, ("[TDA182I2SetFrequency]GetLockStatus\n"));

		/* Get Tuner PLL Lock status */
		reterr = TDA182I2GetLockStatus(pObj->tUnit, &eTunerLock);
		if(reterr != tmNOERR)
		{
			CH_DI_Print(0, ("[TDA182I2SetFrequency]GetLockStatus Error\n"));
			return tmERR_SET_FAILED;
		}
	}

	if(reterr == tmNOERR)
	{
		pTuneRequest->eTunerLock = (TDA182I2FrontEndState_t)eTunerLock;
	}

	if(reterr == tmNOERR)
	{
		/* Print the result of the Manager function */
		switch(eTunerLock)
		{
		case tmFrontEndStateLocked:
			CH_DI_Print(2, ("[TDA182I2SetFrequency] LOCK\n"));

			//TDA182I2GetPower_Level( pObj->tUnit, &ucInputLevel);
			//pTuneRequest->dwInputLevel = ucInputLevel;

			break;

		case tmFrontEndStateNotLocked:
			CH_DI_Print(2, ("[TDA182I2SetFrequency] NOT LOCKED\n"));
			break;

		case tmFrontEndStateSearching:
		default:
			CH_DI_Print(3, ("[TDA182I2SetFrequency] TIMEOUT\n"));
			break;
		}
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2Init:
 * DESCRIPTION:	initialization of the Tuner
 * RETURN:		always True
********************************************************************/
TDA182I2_ERR_CODE TDA182I2Init(   HUINT32 tUnit)
{
    pTDA182I2Object_t   	pObj = NULL;	// shkim2 : unused parameter
    TDA182I2_ERR_CODE       reterr = tmNOERR;

	if( tUnit > NUM_OF_CHANNEL )
		return tmERR_BAD_PARAMETER;

	reterr = TDA182I2AllocInstance(tUnit, &pObj);
	if (pObj == NULL)
	{
		CH_DI_Print(0, ("[TDA182I2Init]TDA182I2AllocInstance error : %p \n", pObj ));
		return tmERR_NOT_INITIALIZED;
	}

	return reterr;
}

/********************************************************************
 * FUNCTION:	TDA182I2SendRequest:
 * DESCRIPTION:
 * RETURN:
********************************************************************/
TDA182I2_ERR_CODE TDA182I2SendRequest ( HUINT32 tUnit, pTDA182I2Request_t pTuneRequest, TuneReqType_t tTuneReqType)
{
    pTDA182I2Object_t 		pObj = NULL;
    TDA182I2_ERR_CODE 		reterr = tmERR_BAD_PARAMETER;
    pTDA182I2Request_t 		pTunerOnlyRequest = NULL;
	CH_UNUSED(tTuneReqType);

	if( tUnit > NUM_OF_CHANNEL || !pTuneRequest   )
	{
		CH_DI_Print(0, ("[TDA182I2SendRequest]Error: Invalid Pointer! \n"));
        reterr = tmERR_BAD_PARAMETER;
		return reterr;
    }

	reterr = TDA182I2GetInstance(tUnit, &pObj);
    if(reterr == tmNOERR)
    {
		pTunerOnlyRequest = pTuneRequest;
		reterr = TDA182I2SetFrequency(pObj->tUnit, pTunerOnlyRequest);
    }
	else
	{
		CH_DI_Print(0, ("[TDA182I2SendRequest]GetInstance Error! \n"));
		reterr = tmERR_BAD_PARAMETER;
	}

	return reterr;
}



/* *************************************************************************************************** */
#define ___HEADER________________________________________________________________________________________
/* *************************************************************************************************** */

#include <hlib.h>


#include "_cp_internal.h"

/* *************************************************************************************************** */
#define ___GLOBAL_HEADER_________________________________________________________________________________
/* *************************************************************************************************** */


/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */


/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */
typedef HBOOL  (*UserDefineCode7Fto9F)(HUINT8 cDvbCh, HUINT16 *pusDvbCh);

#define CP_8859_TABLE_MAX_SIZE	96


/* *************************************************************************************************** */
#define ___STATIC_VARIABLES______________________________________________________________________________
/* *************************************************************************************************** */


STATIC const HUINT16 s_usISO8859_1_uni[CP_8859_TABLE_MAX_SIZE] = {
	/* 0xA0 */	0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7, 0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
	/* 0xB0 */	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7, 0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF,
	/* 0xC0 */	0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7, 0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
	/* 0xD0 */	0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7, 0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF,
	/* 0xE0 */	0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7, 0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
	/* 0xF0 */	0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7, 0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF,
};

STATIC const HUINT16 s_usISO8859_2_uni[CP_8859_TABLE_MAX_SIZE] = {
	/* 0xA0 */	0x00A0, 0x0104, 0x02D8, 0x0141, 0x00A4, 0x013D, 0x015A, 0x00A7,	0x00A8, 0x0160, 0x015E, 0x0164, 0x0179, 0x00AD, 0x017D, 0x017B,
	/* 0xB0 */	0x00B0, 0x0105, 0x02DB, 0x0142, 0x00B4, 0x013E, 0x015B, 0x02C7,	0x00B8, 0x0161, 0x015F, 0x0165, 0x017A, 0x02DD, 0x017E, 0x017C,
	/* 0xC0 */	0x0154, 0x00C1, 0x00C2, 0x0102, 0x00C4, 0x0139, 0x0106, 0x00C7,	0x010C, 0x00C9, 0x0118, 0x00CB, 0x011A, 0x00CD, 0x00CE, 0x010E,
	/* 0xD0 */	0x0110, 0x0143, 0x0147, 0x00D3, 0x00D4, 0x0150, 0x00D6, 0x00D7,	0x0158, 0x016E, 0x00DA, 0x0170, 0x00DC, 0x00DD, 0x0162, 0x00DF,
	/* 0xE0 */	0x0155, 0x00E1, 0x00E2, 0x0103, 0x00E4, 0x013A, 0x0107, 0x00E7,	0x010D, 0x00E9, 0x0119, 0x00EB, 0x011B, 0x00ED, 0x00EE, 0x010F,
	/* 0xF0 */	0x0111, 0x0144, 0x0148, 0x00F3, 0x00F4, 0x0151, 0x00F6, 0x00F7,	0x0159, 0x016F, 0x00FA, 0x0171, 0x00FC, 0x00FD, 0x0163, 0x02D9,
};

STATIC const HUINT16 s_usISO8859_3_uni[CP_8859_TABLE_MAX_SIZE] = {
	/* 0xA0 */	0x00A0, 0x0126, 0x02D8, 0x00A3, 0x00A4, 0xFFFD, 0x0124, 0x00A7,	0x00A8, 0x0130, 0x015E, 0x011E, 0x0134, 0x00AD, 0xFFFD, 0x017B,
	/* 0xB0 */	0x00B0, 0x0127, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x0125, 0x00B7,	0x00B8, 0x0131, 0x015F, 0x011F, 0x0135, 0x00BD, 0xFFFD, 0x017C,
	/* 0xC0 */	0x00C0, 0x00C1, 0x00C2, 0xFFFD, 0x00C4, 0x010A, 0x0108, 0x00C7,	0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
	/* 0xD0 */	0xFFFD, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x0120, 0x00D6, 0x00D7,	0x011C, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x016C, 0x015C, 0x00DF,
	/* 0xE0 */	0x00E0, 0x00E1, 0x00E2, 0xFFFD, 0x00E4, 0x010B, 0x0109, 0x00E7,	0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
	/* 0xF0 */	0xFFFD, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x0121, 0x00F6, 0x00F7,	0x011D, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x016D, 0x015D, 0x02D9,
};

STATIC const HUINT16 s_usISO8859_4_uni[CP_8859_TABLE_MAX_SIZE] = {
	/* 0xA0 */	0x00A0, 0x0104, 0x0138, 0x0156, 0x00A4, 0x0128, 0x013B, 0x00A7,	0x00A8, 0x0160, 0x0112, 0x0122, 0x0166, 0x00AD, 0x017D, 0x00AF,
	/* 0xB0 */	0x00B0, 0x0105, 0x02DB, 0x0157, 0x00B4, 0x0129, 0x013C, 0x02C7,	0x00B8, 0x0161, 0x0113, 0x0123, 0x0167, 0x014A, 0x017E, 0x014B,
	/* 0xC0 */	0x0100, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x012E,	0x010C, 0x00C9, 0x0118, 0x00CB, 0x0116, 0x00CD, 0x00CE, 0x012A,
	/* 0xD0 */	0x0110, 0x0145, 0x014C, 0x0136, 0x00D4, 0x00D5, 0x00D6, 0x00D7,	0x00D8, 0x0172, 0x00DA, 0x00DB, 0x00DC, 0x0168, 0x016A, 0x00DF,
	/* 0xE0 */	0x0101, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x012F,	0x010D, 0x00E9, 0x0119, 0x00EB, 0x0117, 0x00ED, 0x00EE, 0x012B,
	/* 0xF0 */	0x0111, 0x0146, 0x014D, 0x0137, 0x00F4, 0x00F5, 0x00F6, 0x00F7,	0x00F8, 0x0173, 0x00FA, 0x00FB, 0x00FC, 0x0169, 0x016B, 0x02D9,
};

STATIC const HUINT16 s_usISO8859_5_uni[CP_8859_TABLE_MAX_SIZE] = {
	/* 0xA0 */	0x00A0, 0x0401, 0x0402, 0x0403, 0x0404, 0x0405, 0x0406, 0x0407,	0x0408, 0x0409, 0x040A, 0x040B, 0x040C, 0x00AD, 0x040E, 0x040F,
	/* 0xB0 */	0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417,	0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F,
	/* 0xC0 */	0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427,	0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D, 0x042E, 0x042F,
	/* 0xD0 */	0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437,	0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F,
	/* 0xE0 */	0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447,	0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F,
	/* 0xF0 */	0x2116, 0x0451, 0x0452, 0x0453, 0x0454, 0x0455, 0x0456, 0x0457,	0x0458, 0x0459, 0x045A, 0x045B, 0x045C, 0x00A7, 0x045E, 0x045F,
};

STATIC const HUINT16 s_usISO8859_6_uni[CP_8859_TABLE_MAX_SIZE] = {
	/* 0xA0 */	0x00A0, 0xFFFD, 0xFFFD, 0xFFFD, 0x00A4, 0xFFFD, 0xFFFD, 0xFFFD,	0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0x060C, 0x00AD, 0xFFFD, 0xFFFD,
	/* 0xB0 */	0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,	0xFFFD, 0xFFFD, 0xFFFD, 0x061B, 0xFFFD, 0xFFFD, 0xFFFD, 0x061F,
	/* 0xC0 */	0xFFFD, 0x0621, 0x0622, 0x0623, 0x0624, 0x0625, 0x0626, 0x0627,	0x0628, 0x0629, 0x062A, 0x062B, 0x062C, 0x062D, 0x062E, 0x062F,
	/* 0xD0 */	0x0630, 0x0631, 0x0632, 0x0633, 0x0634, 0x0635, 0x0636, 0x0637,	0x0638, 0x0639, 0x063A, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,
	/* 0xE0 */	0x0640, 0x0641, 0x0642, 0x0643, 0x0644, 0x0645, 0x0646, 0x0647,	0x0648, 0x0649, 0x064A, 0x064B, 0x064C, 0x064D, 0x064E, 0x064F,
	/* 0xF0 */	0x0650, 0x0651, 0x0652, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,	0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,
};

STATIC const HUINT16 s_usISO8859_7_uni[CP_8859_TABLE_MAX_SIZE] = {
	/* 0xA0 */	0x00A0, 0x2018, 0x2019, 0x00A3, 0x20AC, 0x20AF, 0x00A6, 0x00A7,	0x00A8, 0x00A9, 0x037A, 0x00AB, 0x00AC, 0x00AD, 0xFFFD, 0x2015,
	/* 0xB0 */	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x0384, 0x0385, 0x0386, 0x00B7,	0x0388, 0x0389, 0x038A, 0x00BB, 0x038C, 0x00BD, 0x038E, 0x038F,
	/* 0xC0 */	0x0390, 0x0391, 0x0392, 0x0393, 0x0394, 0x0395, 0x0396, 0x0397,	0x0398, 0x0399, 0x039A, 0x039B, 0x039C, 0x039D, 0x039E, 0x039F,
	/* 0xD0 */	0x03A0, 0x03A1, 0xFFFD, 0x03A3, 0x03A4, 0x03A5, 0x03A6, 0x03A7,	0x03A8, 0x03A9, 0x03AA, 0x03AB, 0x03AC, 0x03AD, 0x03AE, 0x03AF,
	/* 0xE0 */	0x03B0, 0x03B1, 0x03B2, 0x03B3, 0x03B4, 0x03B5, 0x03B6, 0x03B7,	0x03B8, 0x03B9, 0x03BA, 0x03BB, 0x03BC, 0x03BD, 0x03BE, 0x03BF,
	/* 0xF0 */	0x03C0, 0x03C1, 0x03C2, 0x03C3, 0x03C4, 0x03C5, 0x03C6, 0x03C7,	0x03C8, 0x03C9, 0x03CA, 0x03CB, 0x03CC, 0x03CD, 0x03CE, 0xFFFD,
};

STATIC const HUINT16 s_usISO8859_8_uni[CP_8859_TABLE_MAX_SIZE] = {
	/* 0xA0 */	0x00A0, 0xFFFD, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,	0x00A8, 0x00A9, 0x00D7, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
	/* 0xB0 */	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,	0x00B8, 0x00B9, 0x00F7, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0xFFFD,
	/* 0xC0 */	0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,	0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,
	/* 0xD0 */	0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,	0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0x2017,
	/* 0xE0 */	0x05D0, 0x05D1, 0x05D2, 0x05D3, 0x05D4, 0x05D5, 0x05D6, 0x05D7,	0x05D8, 0x05D9, 0x05DA, 0x05DB, 0x05DC, 0x05DD, 0x05DE, 0x05DF,
	/* 0xF0 */	0x05E0, 0x05E1, 0x05E2, 0x05E3, 0x05E4, 0x05E5, 0x05E6, 0x05E7,	0x05E8, 0x05E9, 0x05EA, 0xFFFD, 0xFFFD, 0x200E, 0x200F, 0xFFFD,
};

STATIC const HUINT16 s_usISO8859_9_uni[CP_8859_TABLE_MAX_SIZE] = {
#if	0
	/* 0xA0 */	0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,	0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,
	/* 0xB0 */	0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,	0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,
	/* 0xC0 */	0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,	0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,
#else	//	확인되어야 함.
	/* 0xA0 */	0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7, 0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
	/* 0xB0 */	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7, 0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF,
	/* 0xC0 */	0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7, 0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
#endif
	/* 0xD0 */	0x011E, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7,	0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x0130, 0x015E, 0x00DF,
	/* 0xE0 */	0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7,	0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
	/* 0xF0 */	0x011F, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7,	0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x0131, 0x015F, 0x00FF,
};


STATIC const HUINT16 s_usISO8859_10_uni[CP_8859_TABLE_MAX_SIZE] = {
	/* 0xA0 */	0x00A0, 0x0104, 0x0112, 0x0122, 0x012A, 0x0128, 0x0136, 0x00A7,	0x013B, 0x0110, 0x0160, 0x0166, 0x017D, 0x00AD, 0x016A, 0x014A,
	/* 0xB0 */	0x00B0, 0x0105, 0x0113, 0x0123, 0x012B, 0x0129, 0x0137, 0x00B7,	0x013C, 0x0111, 0x0161, 0x0167, 0x017E, 0x2015, 0x016B, 0x014B,
	/* 0xC0 */	0x0100, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x012E,	0x010C, 0x00C9, 0x0118, 0x00CB, 0x0116, 0x00CD, 0x00CE, 0x00CF,
	/* 0xD0 */	0x00D0, 0x0145, 0x014C, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x0168,	0x00D8, 0x0172, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF,
	/* 0xE0 */	0x0101, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x012F,	0x010D, 0x00E9, 0x0119, 0x00EB, 0x0117, 0x00ED, 0x00EE, 0x00EF,
	/* 0xF0 */	0x00F0, 0x0146, 0x014D, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x0169,	0x00F8, 0x0173, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x0138,
};

STATIC const HUINT16 s_usISO8859_11_uni[CP_8859_TABLE_MAX_SIZE] = {
	/* 0xA0 */	0x00A0, 0x0E01, 0x0E02, 0x0E03, 0x0E04, 0x0E05, 0x0E06, 0x0E07, 0x0E08, 0x0E09, 0x0E0A, 0x0E0B, 0x0E0C, 0x0E0D, 0x0E0E, 0x0E0F,
	/* 0xB0 */	0x0E10, 0x0E11, 0x0E12, 0x0E13, 0x0E14, 0x0E15, 0x0E16, 0x0E17, 0x0E18, 0x0E19, 0x0E1A, 0x0E1B, 0x0E1C, 0x0E1D, 0x0E1E, 0x0E1F,
	/* 0xC0 */ 	0x0E20, 0x0E21, 0x0E22, 0x0E23, 0x0E24, 0x0E25, 0x0E26, 0x0E27, 0x0E28, 0x0E29, 0x0E2A, 0x0E2B, 0x0E2C, 0x0E2D, 0x0E2E, 0x0E2F,
	/* 0xD0 */	0x0E30, 0x0E31, 0x0E32, 0x0E33, 0x0E34, 0x0E35, 0x0E36, 0x0E37, 0x0E38, 0x0E39, 0x0E3A, 0x0020, 0x0020, 0x0020, 0x0020, 0x0E3F,
	/* 0xE0 */	0x0E40, 0x0E41, 0x0E42, 0x0E43, 0x0E44, 0x0E45, 0x0E46, 0x0E47, 0x0E48, 0x0E49, 0x0E4A, 0x0E4B, 0x0E4C, 0x0E4D, 0x0E4E, 0x0E4F,
	/* 0xF0 */	0x0E50, 0x0E51, 0x0E52, 0x0E53, 0x0E54, 0x0E55, 0x0E56, 0x0E57, 0x0E58, 0x0E59, 0x0E5A, 0x0E5B, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,
};

STATIC const HUINT16 s_usISO8859_13_uni[CP_8859_TABLE_MAX_SIZE] = {
	/* 0xA0 */	0x00A0, 0x201D, 0x00A2, 0x00A3, 0x00A4, 0x201E, 0x00A6, 0x00A7, 0x00D8, 0x00A9, 0x0156, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00C6,
	/* 0xB0 */	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x201C, 0x00B5, 0x00B6, 0x00B7,	0x00F8, 0x00B9, 0x0157, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00E6,
	/* 0xC0 */	0x0104, 0x012E, 0x0100, 0x0106, 0x00C4, 0x00C5, 0x0118, 0x0112,	0x010C, 0x00C9, 0x0179, 0x0116, 0x0122, 0x0136, 0x012A, 0x013B,
	/* 0xD0 */	0x0160, 0x0143, 0x0145, 0x00D3, 0x014C, 0x00D5, 0x00D6, 0x00D7,	0x0172, 0x0141, 0x015A, 0x016A, 0x00DC, 0x017B, 0x017D, 0x00DF,
	/* 0xE0 */	0x0105, 0x012F, 0x0101, 0x0107, 0x00E4, 0x00E5, 0x0119, 0x0113,	0x010D, 0x00E9, 0x017A, 0x0117, 0x0123, 0x0137, 0x012B, 0x013C,
	/* 0xF0 */	0x0161, 0x0144, 0x0146, 0x00F3, 0x014D, 0x00F5, 0x00F6, 0x00F7,	0x0173, 0x0142, 0x015B, 0x016B, 0x00FC, 0x017C, 0x017E, 0x2019,
};

STATIC const HUINT16 s_usISO8859_14_uni[CP_8859_TABLE_MAX_SIZE] = {
	/* 0xA0 */	0x00A0, 0x1E02, 0x1E03, 0x00A3, 0x010A, 0x010B, 0x1E0A, 0x00A7,	0x1E80, 0x00A9, 0x1E82, 0x1E0B, 0x1EF2, 0x00AD, 0x00AE, 0x0178,
	/* 0xB0 */	0x1E1E, 0x1E1F, 0x0120, 0x0121, 0x1E40, 0x1E41, 0x00B6, 0x1E56,	0x1E81, 0x1E57, 0x1E83, 0x1E60, 0x1EF3, 0x1E84, 0x1E85, 0x1E61,
	/* 0xC0 */	0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7,	0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
	/* 0xD0 */	0x0174, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x1E6A,	0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x0176, 0x00DF,
	/* 0xE0 */	0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7,	0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
	/* 0xF0 */	0x0175, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x1E6B,	0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x0177, 0x00FF,
};

STATIC const HUINT16 s_usISO8859_15_uni[CP_8859_TABLE_MAX_SIZE] = {
	/* 0xA0 */	0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x20AC, 0x00A5, 0x0160, 0x00A7,	0x0161, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
	/* 0xB0 */	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x017D, 0x00B5, 0x00B6, 0x00B7,	0x017E, 0x00B9, 0x00BA, 0x00BB, 0x0152, 0x0153, 0x0178, 0x00BF,
	/* 0xC0 */	0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,	0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,
	/* 0xD0 */	0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,	0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,
	/* 0xE0 */	0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,	0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,
	/* 0xF0 */	0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,	0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,
};

STATIC const HUINT16 s_usISO8859_16_uni[CP_8859_TABLE_MAX_SIZE] = {
	/* 0xA0 */	0x00A0, 0x0104, 0x0105, 0x0141, 0x20AC, 0x201E, 0x0160, 0x00A7,	0x0161, 0x00A9, 0x0218, 0x00AB, 0x0179, 0x00AD, 0x017A, 0x017B,
	/* 0xB0 */	0x00B0, 0x00B1, 0x010C, 0x0142, 0x017D, 0x201D, 0x00B6, 0x00B7,	0x017E, 0x010D, 0x0219, 0x00BB, 0x0152, 0x0153, 0x0178, 0x017C,
	/* 0xC0 */	0x00C0, 0x00C1, 0x00C2, 0x0102, 0x00C4, 0x0106, 0x00C6, 0x00C7,	0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
	/* 0xD0 */	0x0110, 0x0143, 0x00D2, 0x00D3, 0x00D4, 0x0150, 0x00D6, 0x015A,	0x0170, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x0118, 0x021A, 0x00DF,
	/* 0xE0 */	0x00E0, 0x00E1, 0x00E2, 0x0103, 0x00E4, 0x0107, 0x00E6, 0x00E7,	0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
	/* 0xF0 */	0x0111, 0x0144, 0x00F2, 0x00F3, 0x00F4, 0x0151, 0x00F6, 0x015B,	0x0171, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x0119, 0x021B, 0x00FF,
};



/* *************************************************************************************************** */
#define ___Private_FUNCTION______________________________________________________________________________
/* *************************************************************************************************** */


/* *************************************************************************************************** */
#define _______8859_Private_FUNCTION_____________________________________________________________________
/* *************************************************************************************************** */

STATIC	HINT32			_hcodepage_8859_NumOfBytes(const void 	*pSrc)
{
	char *pSrcString = (char *)pSrc;

	return strlen(pSrcString);
}

STATIC	HINT32			_hcodepage_8859_NumOfChars(const void 	*pSrc)
{
	char *pSrcString = (char *)pSrc;

	return strlen(pSrcString);
}

STATIC	HINT32			_hcodepage_8859_GetCharAt(const void 	*pSrc, const HINT32 nCharNum,  void *pCharBuf)
{
	HUINT8	*pSrcString = (HUINT8	*)pSrc;
	HUINT8 	*pcCharBuf	= (HUINT8	*)pCharBuf;

	*pcCharBuf = pSrcString[nCharNum];
	return 1;
}

STATIC	HINT32 			_hcodepage_8859_ConvertCodepageToUCS2(const void 	*pSrcStr, HUINT16 	*pDestUCS2Str, HINT32 nDestStrNumOfBytes,
															  const HUINT16			*pusMap8859toUCS2,
															  UserDefineCode7Fto9F	cbUserDefCode)
{
	HINT32	nLoop = 0;
	HINT32 	nSrcChars = 0;
	HINT32 	nMaxChars = 0;
	HINT32	nConvCharCnt = 0;
	HBOOL	bKnownChar = FALSE;
	HUINT8	cDvbCh = 0;
	HUINT16 usDvbCh = 0;
	HUINT8	*pSrcString = (HUINT8	*)pSrcStr;

	nSrcChars = _hcodepage_8859_NumOfChars(pSrcStr);
	if(HxCP_IsValidReturnValue(nSrcChars) == FALSE)
	{
		return eHxCP_ERROR_FAIL;
	}

	nDestStrNumOfBytes -= sizeof(HUINT8);
	nDestStrNumOfBytes /= sizeof(HUINT16);
	nMaxChars = (nSrcChars > nDestStrNumOfBytes) ? nDestStrNumOfBytes : nSrcChars;

	for(nLoop=0; nLoop < nMaxChars; nLoop++)
	{
		cDvbCh = pSrcString[nLoop];

		if(cDvbCh == 0x00)
		{
			break;
		}
		/* 0x01 ~ 0x1F */
		else if(cDvbCh < 0x20)
		{
			pDestUCS2Str[nConvCharCnt++] = cDvbCh;
		}
		/* 0x20 ~ 0x7E */
		else if(cDvbCh < 0x7F)
		{
			//ascii
			pDestUCS2Str[nConvCharCnt++] = cDvbCh;
		}
		/* 0x7F ~ 0x9F */
		else if(cDvbCh < 0xA0)
		{
			bKnownChar = FALSE;
			if(cbUserDefCode)
			{
				bKnownChar = cbUserDefCode(cDvbCh, &usDvbCh);
			}

			if (bKnownChar == FALSE)
			{
				continue;
			}
			pDestUCS2Str[nConvCharCnt++] = usDvbCh;
		}
		/* 0xA0 ~ 0xFF */
		else
		{
			if(pusMap8859toUCS2)
			{
				cDvbCh -= 0xA0;
				if(0xfffd != pusMap8859toUCS2[cDvbCh])
				{
					pDestUCS2Str[nConvCharCnt++] = pusMap8859toUCS2[cDvbCh];
					continue;
				}
			}

			{
				pDestUCS2Str[nConvCharCnt++] = cDvbCh;
			}
		}
	}
	pDestUCS2Str[nConvCharCnt] = 0x0000;

	return (nConvCharCnt*sizeof(HUINT16));
}

STATIC HBOOL  			_hcodepage_8859_xx_UserDefineCode7Fto9F_EmphasisChar(HUINT8 cDvbCh, HUINT16 *pusDvbCh)
{
	switch (cDvbCh)
	{
		// Emphasis 문자 :
	case CHAR_EMPHASIS_ON:		*pusDvbCh = UCS2_EMPHASIS_ON;		return TRUE;
	case CHAR_EMPHASIS_OFF:		*pusDvbCh = UCS2_EMPHASIS_OFF;		return TRUE;
	case CHAR_CR_LF:			*pusDvbCh = UCS2_CR_LF;				return TRUE;
	default:
		*pusDvbCh = (0xE000 | cDvbCh);
		break;
	}
	return TRUE;
}

STATIC HBOOL  			_hcodepage_8859_09_UserDefineCode7Fto9F_EmphasisChar(HUINT8 cDvbCh, HUINT16 *pusDvbCh)
{
	switch (cDvbCh)
	{
	case 0x80:
		// 독일에서는 8859-9의 0x80을 EURO Symbol 로 보내어서 처리함.
		*pusDvbCh = 0x20AC;	//euro symbol
		return TRUE;

		// Emphasis 문자 :
	case CHAR_EMPHASIS_ON:		*pusDvbCh = UCS2_EMPHASIS_ON;		return TRUE;
	case CHAR_EMPHASIS_OFF:		*pusDvbCh = UCS2_EMPHASIS_OFF;		return TRUE;
	case CHAR_CR_LF:			*pusDvbCh = UCS2_CR_LF;				return TRUE;
	default:
		*pusDvbCh = (0xE000 | cDvbCh);
		break;
	}
	return TRUE;
}

STATIC	HINT32			_hcodepage_8859_01_ConvertCodepageToUCS2(const void 	*pSrcStr, HUINT16 	*pDestUCS2Str, HINT32 nDestStrNumOfBytes)
{
	return _hcodepage_8859_ConvertCodepageToUCS2(pSrcStr, pDestUCS2Str, nDestStrNumOfBytes, s_usISO8859_1_uni, _hcodepage_8859_xx_UserDefineCode7Fto9F_EmphasisChar);
}

STATIC	HINT32			_hcodepage_8859_02_ConvertCodepageToUCS2(const void 	*pSrcStr, HUINT16 	*pDestUCS2Str, HINT32 nDestStrNumOfBytes)
{
	return _hcodepage_8859_ConvertCodepageToUCS2(pSrcStr, pDestUCS2Str, nDestStrNumOfBytes, s_usISO8859_2_uni, _hcodepage_8859_xx_UserDefineCode7Fto9F_EmphasisChar);
}

STATIC	HINT32			_hcodepage_8859_03_ConvertCodepageToUCS2(const void 	*pSrcStr, HUINT16 	*pDestUCS2Str, HINT32 nDestStrNumOfBytes)
{
	return _hcodepage_8859_ConvertCodepageToUCS2(pSrcStr, pDestUCS2Str, nDestStrNumOfBytes, s_usISO8859_3_uni, _hcodepage_8859_xx_UserDefineCode7Fto9F_EmphasisChar);
}

STATIC	HINT32			_hcodepage_8859_04_ConvertCodepageToUCS2(const void 	*pSrcStr, HUINT16 	*pDestUCS2Str, HINT32 nDestStrNumOfBytes)
{
	return _hcodepage_8859_ConvertCodepageToUCS2(pSrcStr, pDestUCS2Str, nDestStrNumOfBytes, s_usISO8859_4_uni, _hcodepage_8859_xx_UserDefineCode7Fto9F_EmphasisChar);
}

STATIC	HINT32			_hcodepage_8859_05_ConvertCodepageToUCS2(const void 	*pSrcStr, HUINT16 	*pDestUCS2Str, HINT32 nDestStrNumOfBytes)
{
	return _hcodepage_8859_ConvertCodepageToUCS2(pSrcStr, pDestUCS2Str, nDestStrNumOfBytes, s_usISO8859_5_uni, _hcodepage_8859_xx_UserDefineCode7Fto9F_EmphasisChar);
}

STATIC	HINT32			_hcodepage_8859_06_ConvertCodepageToUCS2(const void 	*pSrcStr, HUINT16 	*pDestUCS2Str, HINT32 nDestStrNumOfBytes)
{
	return _hcodepage_8859_ConvertCodepageToUCS2(pSrcStr, pDestUCS2Str, nDestStrNumOfBytes, s_usISO8859_6_uni, _hcodepage_8859_xx_UserDefineCode7Fto9F_EmphasisChar);
}

STATIC	HINT32			_hcodepage_8859_07_ConvertCodepageToUCS2(const void 	*pSrcStr, HUINT16 	*pDestUCS2Str, HINT32 nDestStrNumOfBytes)
{
	return _hcodepage_8859_ConvertCodepageToUCS2(pSrcStr, pDestUCS2Str, nDestStrNumOfBytes, s_usISO8859_7_uni, _hcodepage_8859_xx_UserDefineCode7Fto9F_EmphasisChar);
}

STATIC	HINT32			_hcodepage_8859_08_ConvertCodepageToUCS2(const void 	*pSrcStr, HUINT16 	*pDestUCS2Str, HINT32 nDestStrNumOfBytes)
{
	return _hcodepage_8859_ConvertCodepageToUCS2(pSrcStr, pDestUCS2Str, nDestStrNumOfBytes, s_usISO8859_8_uni, _hcodepage_8859_xx_UserDefineCode7Fto9F_EmphasisChar);
}

STATIC	HINT32			_hcodepage_8859_09_ConvertCodepageToUCS2(const void 	*pSrcStr, HUINT16 	*pDestUCS2Str, HINT32 nDestStrNumOfBytes)
{
	return _hcodepage_8859_ConvertCodepageToUCS2(pSrcStr, pDestUCS2Str, nDestStrNumOfBytes, s_usISO8859_9_uni, _hcodepage_8859_09_UserDefineCode7Fto9F_EmphasisChar);
}

STATIC	HINT32			_hcodepage_8859_10_ConvertCodepageToUCS2(const void 	*pSrcStr, HUINT16 	*pDestUCS2Str, HINT32 nDestStrNumOfBytes)
{
	return _hcodepage_8859_ConvertCodepageToUCS2(pSrcStr, pDestUCS2Str, nDestStrNumOfBytes, s_usISO8859_10_uni, _hcodepage_8859_xx_UserDefineCode7Fto9F_EmphasisChar);
}

STATIC	HINT32			_hcodepage_8859_11_ConvertCodepageToUCS2(const void 	*pSrcStr, HUINT16 	*pDestUCS2Str, HINT32 nDestStrNumOfBytes)
{
	return _hcodepage_8859_ConvertCodepageToUCS2(pSrcStr, pDestUCS2Str, nDestStrNumOfBytes, s_usISO8859_11_uni, _hcodepage_8859_xx_UserDefineCode7Fto9F_EmphasisChar);
}

STATIC	HINT32			_hcodepage_8859_13_ConvertCodepageToUCS2(const void 	*pSrcStr, HUINT16 	*pDestUCS2Str, HINT32 nDestStrNumOfBytes)
{
	return _hcodepage_8859_ConvertCodepageToUCS2(pSrcStr, pDestUCS2Str, nDestStrNumOfBytes, s_usISO8859_13_uni, _hcodepage_8859_xx_UserDefineCode7Fto9F_EmphasisChar);
}

STATIC	HINT32			_hcodepage_8859_14_ConvertCodepageToUCS2(const void 	*pSrcStr, HUINT16 	*pDestUCS2Str, HINT32 nDestStrNumOfBytes)
{
	return _hcodepage_8859_ConvertCodepageToUCS2(pSrcStr, pDestUCS2Str, nDestStrNumOfBytes, s_usISO8859_14_uni, _hcodepage_8859_xx_UserDefineCode7Fto9F_EmphasisChar);
}

STATIC	HINT32			_hcodepage_8859_15_ConvertCodepageToUCS2(const void 	*pSrcStr, HUINT16 	*pDestUCS2Str, HINT32 nDestStrNumOfBytes)
{
	return _hcodepage_8859_ConvertCodepageToUCS2(pSrcStr, pDestUCS2Str, nDestStrNumOfBytes, s_usISO8859_15_uni, _hcodepage_8859_xx_UserDefineCode7Fto9F_EmphasisChar);
}


STATIC	HINT32			_hcodepage_8859_16_ConvertCodepageToUCS2(const void 	*pSrcStr, HUINT16 	*pDestUCS2Str, HINT32 nDestStrNumOfBytes)
{
	return _hcodepage_8859_ConvertCodepageToUCS2(pSrcStr, pDestUCS2Str, nDestStrNumOfBytes, s_usISO8859_16_uni, _hcodepage_8859_xx_UserDefineCode7Fto9F_EmphasisChar);
}


STATIC	HINT32 			_hcodepage_8859_ConvertUCS2ToCodepage(const HUINT16 	*pSrcUCS2Str, void 	*pDestStr, HINT32 nDestStrNumOfBytes, const HUINT16  *pusMap8859toUCS2)
{
	HINT32		nLoop = 0;
	HINT32		i = 0;
	HINT32 		nSrcChars = 0;
	HINT32 		nMaxBytes = 0;
	HINT32		nConvChCnt = 0;
	HUINT8		*pDestString = (HUINT8	*)pDestStr;

	nSrcChars = HLIB_CP_NumOfChars(eHxCP_UNICODE_UCS2, pSrcUCS2Str);
	if(HxCP_IsValidReturnValue(nSrcChars) == FALSE)
	{
		return eHxCP_ERROR_FAIL;
	}

	nDestStrNumOfBytes -= sizeof(HUINT8);
	nMaxBytes = (nSrcChars > nDestStrNumOfBytes) ? nDestStrNumOfBytes : nSrcChars;

	for(nLoop=0; nLoop < nMaxBytes; nLoop++)
	{
		/*
		bsearch(const void *key, const void *base, size_t nel, size_t width, int (*compar)(const void *, const void *));
		*/
		if(pSrcUCS2Str[nLoop] < 0x00A0)
		{
			pDestString[nConvChCnt] = (pSrcUCS2Str[nLoop] & 0xFF);
			nConvChCnt++;
			continue;
		}

		for(i = 0; i < CP_8859_TABLE_MAX_SIZE; i++)
		{
			if(pusMap8859toUCS2[i] == pSrcUCS2Str[nLoop])
			{
				pDestString[nConvChCnt] = i & 0xFF;
				nConvChCnt++;
				break;
			}
		}

		if(i >= CP_8859_TABLE_MAX_SIZE)
		{
			pDestString[nConvChCnt] = ' ';
			nConvChCnt++;
		}
	}
	pDestString[nConvChCnt] = 0x00;

	return nMaxBytes;
}

STATIC	HINT32			_hcodepage_8859_01_ConvertUCS2ToCodepage(const HUINT16 	*pSrcUCS2Str, void	*pDestStr, HINT32 nDestStrNumOfBytes)
{
	return _hcodepage_8859_ConvertUCS2ToCodepage(pSrcUCS2Str, pDestStr,  nDestStrNumOfBytes, s_usISO8859_1_uni);
}

STATIC	HINT32			_hcodepage_8859_02_ConvertUCS2ToCodepage(const HUINT16 	*pSrcUCS2Str, void	*pDestStr, HINT32 nDestStrNumOfBytes)
{
	return _hcodepage_8859_ConvertUCS2ToCodepage(pSrcUCS2Str, pDestStr,  nDestStrNumOfBytes, s_usISO8859_2_uni);
}

STATIC	HINT32			_hcodepage_8859_03_ConvertUCS2ToCodepage(const HUINT16 	*pSrcUCS2Str, void	*pDestStr, HINT32 nDestStrNumOfBytes)
{
	return _hcodepage_8859_ConvertUCS2ToCodepage(pSrcUCS2Str, pDestStr,  nDestStrNumOfBytes, s_usISO8859_3_uni);
}

STATIC	HINT32			_hcodepage_8859_04_ConvertUCS2ToCodepage(const HUINT16 	*pSrcUCS2Str, void	*pDestStr, HINT32 nDestStrNumOfBytes)
{
	return _hcodepage_8859_ConvertUCS2ToCodepage(pSrcUCS2Str, pDestStr,  nDestStrNumOfBytes, s_usISO8859_4_uni);
}

STATIC	HINT32			_hcodepage_8859_05_ConvertUCS2ToCodepage(const HUINT16 	*pSrcUCS2Str, void	*pDestStr, HINT32 nDestStrNumOfBytes)
{
	return _hcodepage_8859_ConvertUCS2ToCodepage(pSrcUCS2Str, pDestStr,  nDestStrNumOfBytes, s_usISO8859_5_uni);
}

STATIC	HINT32			_hcodepage_8859_06_ConvertUCS2ToCodepage(const HUINT16 	*pSrcUCS2Str, void	*pDestStr, HINT32 nDestStrNumOfBytes)
{
	return _hcodepage_8859_ConvertUCS2ToCodepage(pSrcUCS2Str, pDestStr,  nDestStrNumOfBytes, s_usISO8859_6_uni);
}

STATIC	HINT32			_hcodepage_8859_07_ConvertUCS2ToCodepage(const HUINT16 	*pSrcUCS2Str, void	*pDestStr, HINT32 nDestStrNumOfBytes)
{
	return _hcodepage_8859_ConvertUCS2ToCodepage(pSrcUCS2Str, pDestStr,  nDestStrNumOfBytes, s_usISO8859_7_uni);
}

STATIC	HINT32			_hcodepage_8859_08_ConvertUCS2ToCodepage(const HUINT16 	*pSrcUCS2Str, void	*pDestStr, HINT32 nDestStrNumOfBytes)
{
	return _hcodepage_8859_ConvertUCS2ToCodepage(pSrcUCS2Str, pDestStr,  nDestStrNumOfBytes, s_usISO8859_8_uni);
}

STATIC	HINT32			_hcodepage_8859_09_ConvertUCS2ToCodepage(const HUINT16 	*pSrcUCS2Str, void	*pDestStr, HINT32 nDestStrNumOfBytes)
{
	return _hcodepage_8859_ConvertUCS2ToCodepage(pSrcUCS2Str, pDestStr,  nDestStrNumOfBytes, s_usISO8859_9_uni);
}

STATIC	HINT32			_hcodepage_8859_10_ConvertUCS2ToCodepage(const HUINT16 	*pSrcUCS2Str, void	*pDestStr, HINT32 nDestStrNumOfBytes)
{
	return _hcodepage_8859_ConvertUCS2ToCodepage(pSrcUCS2Str, pDestStr,  nDestStrNumOfBytes, s_usISO8859_10_uni);
}

STATIC	HINT32			_hcodepage_8859_11_ConvertUCS2ToCodepage(const HUINT16 	*pSrcUCS2Str, void	*pDestStr, HINT32 nDestStrNumOfBytes)
{
	return _hcodepage_8859_ConvertUCS2ToCodepage(pSrcUCS2Str, pDestStr,  nDestStrNumOfBytes, s_usISO8859_11_uni);
}

STATIC	HINT32			_hcodepage_8859_13_ConvertUCS2ToCodepage(const HUINT16 	*pSrcUCS2Str, void	*pDestStr, HINT32 nDestStrNumOfBytes)
{
	return _hcodepage_8859_ConvertUCS2ToCodepage(pSrcUCS2Str, pDestStr,  nDestStrNumOfBytes, s_usISO8859_13_uni);
}

STATIC	HINT32			_hcodepage_8859_14_ConvertUCS2ToCodepage(const HUINT16 	*pSrcUCS2Str, void	*pDestStr, HINT32 nDestStrNumOfBytes)
{
	return _hcodepage_8859_ConvertUCS2ToCodepage(pSrcUCS2Str, pDestStr,  nDestStrNumOfBytes, s_usISO8859_14_uni);
}

STATIC	HINT32			_hcodepage_8859_15_ConvertUCS2ToCodepage(const HUINT16 	*pSrcUCS2Str, void	*pDestStr, HINT32 nDestStrNumOfBytes)
{
	return _hcodepage_8859_ConvertUCS2ToCodepage(pSrcUCS2Str, pDestStr,  nDestStrNumOfBytes, s_usISO8859_15_uni);
}

STATIC	HINT32			_hcodepage_8859_16_ConvertUCS2ToCodepage(const HUINT16 	*pSrcUCS2Str, void	*pDestStr, HINT32 nDestStrNumOfBytes)
{
	return _hcodepage_8859_ConvertUCS2ToCodepage(pSrcUCS2Str, pDestStr,  nDestStrNumOfBytes, s_usISO8859_16_uni);
}


STATIC	HERROR			_hcodepage_8859_1_getinfo(eHxCP_Slot_t *pCPSlot)
{
	if(pCPSlot == NULL)
	{
		return ERR_FAIL;
	}

	pCPSlot->ucEndCharBytes	= 1;

	pCPSlot->cbNumOfBytes	= _hcodepage_8859_NumOfBytes;
	pCPSlot->cbNumOfChars	= _hcodepage_8859_NumOfChars;
	pCPSlot->cbGetCharAt	= _hcodepage_8859_GetCharAt;

	pCPSlot->cbUCS2ToCodepage	= _hcodepage_8859_01_ConvertUCS2ToCodepage;
	pCPSlot->cbCodeapgeToUCS2	= _hcodepage_8859_01_ConvertCodepageToUCS2;
	pCPSlot->pusUCS2Table = s_usISO8859_1_uni;

	pCPSlot->stCPTagSlot.eCodepage = eHxCP_ISO_8859_1;
	pCPSlot->stCPTagSlot.szCPTag[0] = EN300468_8859_3BTYE_FIRST_VALUE;
	pCPSlot->stCPTagSlot.szCPTag[1] = EN300468_8859_3BTYE_2ND_VALUE;
	pCPSlot->stCPTagSlot.szCPTag[2] = EN300468_8859_1_3BYTE_3TH_VALUE;
	pCPSlot->stCPTagSlot.szCPTag[3] = 0x00;
	pCPSlot->stCPTagSlot.nCPTagLen = 3;

#if defined(TEST_CP)
	pCPSlot->stCPTagSlot.szCPTag2[0] = 0x00;
	pCPSlot->stCPTagSlot.nCPTagLen2 = 0;
#endif

	return ERR_OK;
}

STATIC	HERROR			_hcodepage_8859_2_getinfo(eHxCP_Slot_t *pCPSlot)
{
	if(pCPSlot == NULL)
	{
		return ERR_FAIL;
	}

	pCPSlot->ucEndCharBytes	= 1;

	pCPSlot->cbNumOfBytes	= _hcodepage_8859_NumOfBytes;
	pCPSlot->cbNumOfChars	= _hcodepage_8859_NumOfChars;
	pCPSlot->cbGetCharAt	= _hcodepage_8859_GetCharAt;

	pCPSlot->cbUCS2ToCodepage	= _hcodepage_8859_02_ConvertUCS2ToCodepage;
	pCPSlot->cbCodeapgeToUCS2	= _hcodepage_8859_02_ConvertCodepageToUCS2;
	pCPSlot->pusUCS2Table = s_usISO8859_2_uni;

	pCPSlot->stCPTagSlot.eCodepage = eHxCP_ISO_8859_2;
	pCPSlot->stCPTagSlot.szCPTag[0] = EN300468_8859_3BTYE_FIRST_VALUE;
	pCPSlot->stCPTagSlot.szCPTag[1] = EN300468_8859_3BTYE_2ND_VALUE;
	pCPSlot->stCPTagSlot.szCPTag[2] = EN300468_8859_2_3BYTE_3TH_VALUE;
	pCPSlot->stCPTagSlot.szCPTag[3] = 0x00;
	pCPSlot->stCPTagSlot.nCPTagLen = 3;
#if defined(TEST_CP)
	pCPSlot->stCPTagSlot.szCPTag2[0] = 0x00;
	pCPSlot->stCPTagSlot.nCPTagLen2 = 0;
#endif

	return ERR_OK;
}

STATIC	HERROR			_hcodepage_8859_3_getinfo(eHxCP_Slot_t *pCPSlot)
{
	if(pCPSlot == NULL)
	{
		return ERR_FAIL;
	}

	pCPSlot->ucEndCharBytes	= 1;

	pCPSlot->cbNumOfBytes	= _hcodepage_8859_NumOfBytes;
	pCPSlot->cbNumOfChars	= _hcodepage_8859_NumOfChars;
	pCPSlot->cbGetCharAt	= _hcodepage_8859_GetCharAt;

	pCPSlot->cbUCS2ToCodepage	= _hcodepage_8859_03_ConvertUCS2ToCodepage;
	pCPSlot->cbCodeapgeToUCS2	= _hcodepage_8859_03_ConvertCodepageToUCS2;
	pCPSlot->pusUCS2Table = s_usISO8859_3_uni;

	pCPSlot->stCPTagSlot.eCodepage = eHxCP_ISO_8859_3;
	pCPSlot->stCPTagSlot.szCPTag[0] = EN300468_8859_3BTYE_FIRST_VALUE;
	pCPSlot->stCPTagSlot.szCPTag[1] = EN300468_8859_3BTYE_2ND_VALUE;
	pCPSlot->stCPTagSlot.szCPTag[2] = EN300468_8859_3_3BYTE_3TH_VALUE;
	pCPSlot->stCPTagSlot.szCPTag[3] = 0x00;
	pCPSlot->stCPTagSlot.nCPTagLen = 3;
#if defined(TEST_CP)
	pCPSlot->stCPTagSlot.szCPTag2[0] = 0x00;
	pCPSlot->stCPTagSlot.nCPTagLen2 = 0;
#endif

	return ERR_OK;
}


STATIC	HERROR			_hcodepage_8859_4_getinfo(eHxCP_Slot_t *pCPSlot)
{
	if(pCPSlot == NULL)
	{
		return ERR_FAIL;
	}

	pCPSlot->ucEndCharBytes	= 1;

	pCPSlot->cbNumOfBytes	= _hcodepage_8859_NumOfBytes;
	pCPSlot->cbNumOfChars	= _hcodepage_8859_NumOfChars;
	pCPSlot->cbGetCharAt	= _hcodepage_8859_GetCharAt;

	pCPSlot->cbUCS2ToCodepage	= _hcodepage_8859_04_ConvertUCS2ToCodepage;
	pCPSlot->cbCodeapgeToUCS2	= _hcodepage_8859_04_ConvertCodepageToUCS2;
	pCPSlot->pusUCS2Table = s_usISO8859_4_uni;

	pCPSlot->stCPTagSlot.eCodepage = eHxCP_ISO_8859_4;
	pCPSlot->stCPTagSlot.szCPTag[0] = EN300468_8859_3BTYE_FIRST_VALUE;
	pCPSlot->stCPTagSlot.szCPTag[1] = EN300468_8859_3BTYE_2ND_VALUE;
	pCPSlot->stCPTagSlot.szCPTag[2] = EN300468_8859_4_3BYTE_3TH_VALUE;
	pCPSlot->stCPTagSlot.szCPTag[3] = 0x00;
	pCPSlot->stCPTagSlot.nCPTagLen = 3;
#if defined(TEST_CP)
	pCPSlot->stCPTagSlot.szCPTag2[0] = 0x00;
	pCPSlot->stCPTagSlot.nCPTagLen2 = 0;
#endif

	return ERR_OK;
}

STATIC	HERROR			_hcodepage_8859_5_getinfo(eHxCP_Slot_t *pCPSlot)
{
	if(pCPSlot == NULL)
	{
		return ERR_FAIL;
	}

	pCPSlot->ucEndCharBytes	= 1;

	pCPSlot->cbNumOfBytes	= _hcodepage_8859_NumOfBytes;
	pCPSlot->cbNumOfChars	= _hcodepage_8859_NumOfChars;
	pCPSlot->cbGetCharAt	= _hcodepage_8859_GetCharAt;

	pCPSlot->cbUCS2ToCodepage	= _hcodepage_8859_05_ConvertUCS2ToCodepage;
	pCPSlot->cbCodeapgeToUCS2	= _hcodepage_8859_05_ConvertCodepageToUCS2;
	pCPSlot->pusUCS2Table = s_usISO8859_5_uni;

	pCPSlot->stCPTagSlot.eCodepage = eHxCP_ISO_8859_5;
	pCPSlot->stCPTagSlot.szCPTag[0] = eHxCP_DVBLANG_VALUE_8859_05;
	pCPSlot->stCPTagSlot.szCPTag[1] = 0x00;
	pCPSlot->stCPTagSlot.nCPTagLen = 1;
#if defined(TEST_CP)
	pCPSlot->stCPTagSlot.szCPTag2[0] = EN300468_8859_3BTYE_FIRST_VALUE;
	pCPSlot->stCPTagSlot.szCPTag2[1] = EN300468_8859_3BTYE_2ND_VALUE;
	pCPSlot->stCPTagSlot.szCPTag2[2] = EN300468_8859_5_3BYTE_3TH_VALUE;
	pCPSlot->stCPTagSlot.szCPTag2[3] = 0x00;
	pCPSlot->stCPTagSlot.nCPTagLen2 = 3;
#endif

	return ERR_OK;
}

STATIC	HERROR			_hcodepage_8859_6_getinfo(eHxCP_Slot_t *pCPSlot)
{
	if(pCPSlot == NULL)
	{
		return ERR_FAIL;
	}

	pCPSlot->ucEndCharBytes	= 1;

	pCPSlot->cbNumOfBytes	= _hcodepage_8859_NumOfBytes;
	pCPSlot->cbNumOfChars	= _hcodepage_8859_NumOfChars;
	pCPSlot->cbGetCharAt	= _hcodepage_8859_GetCharAt;

	pCPSlot->cbUCS2ToCodepage	= _hcodepage_8859_06_ConvertUCS2ToCodepage;
	pCPSlot->cbCodeapgeToUCS2	= _hcodepage_8859_06_ConvertCodepageToUCS2;
	pCPSlot->pusUCS2Table = s_usISO8859_6_uni;

	pCPSlot->stCPTagSlot.eCodepage = eHxCP_ISO_8859_6;
	pCPSlot->stCPTagSlot.szCPTag[0] = eHxCP_DVBLANG_VALUE_8859_06;
	pCPSlot->stCPTagSlot.szCPTag[1] = 0x00;
	pCPSlot->stCPTagSlot.nCPTagLen = 1;
#if defined(TEST_CP)
	pCPSlot->stCPTagSlot.szCPTag2[0] = EN300468_8859_3BTYE_FIRST_VALUE;
	pCPSlot->stCPTagSlot.szCPTag2[1] = EN300468_8859_3BTYE_2ND_VALUE;
	pCPSlot->stCPTagSlot.szCPTag2[2] = EN300468_8859_6_3BYTE_3TH_VALUE;
	pCPSlot->stCPTagSlot.szCPTag2[3] = 0x00;
	pCPSlot->stCPTagSlot.nCPTagLen2 = 3;
#endif

	return ERR_OK;
}

STATIC	HERROR			_hcodepage_8859_7_getinfo(eHxCP_Slot_t *pCPSlot)
{
	if(pCPSlot == NULL)
	{
		return ERR_FAIL;
	}

	pCPSlot->ucEndCharBytes	= 1;

	pCPSlot->cbNumOfBytes	= _hcodepage_8859_NumOfBytes;
	pCPSlot->cbNumOfChars	= _hcodepage_8859_NumOfChars;
	pCPSlot->cbGetCharAt	= _hcodepage_8859_GetCharAt;

	pCPSlot->cbUCS2ToCodepage	= _hcodepage_8859_07_ConvertUCS2ToCodepage;
	pCPSlot->cbCodeapgeToUCS2	= _hcodepage_8859_07_ConvertCodepageToUCS2;
	pCPSlot->pusUCS2Table = s_usISO8859_7_uni;

	pCPSlot->stCPTagSlot.eCodepage = eHxCP_ISO_8859_7;
	pCPSlot->stCPTagSlot.szCPTag[0] = eHxCP_DVBLANG_VALUE_8859_07;
	pCPSlot->stCPTagSlot.szCPTag[1] = 0x00;
	pCPSlot->stCPTagSlot.nCPTagLen = 1;
#if defined(TEST_CP)
	pCPSlot->stCPTagSlot.szCPTag2[0] = EN300468_8859_3BTYE_FIRST_VALUE;
	pCPSlot->stCPTagSlot.szCPTag2[1] = EN300468_8859_3BTYE_2ND_VALUE;
	pCPSlot->stCPTagSlot.szCPTag2[2] = EN300468_8859_7_3BYTE_3TH_VALUE;
	pCPSlot->stCPTagSlot.szCPTag2[3] = 0x00;
	pCPSlot->stCPTagSlot.nCPTagLen2 = 3;
#endif

	return ERR_OK;
}

STATIC	HERROR			_hcodepage_8859_8_getinfo(eHxCP_Slot_t *pCPSlot)
{
	if(pCPSlot == NULL)
	{
		return ERR_FAIL;
	}

	pCPSlot->ucEndCharBytes	= 1;

	pCPSlot->cbNumOfBytes	= _hcodepage_8859_NumOfBytes;
	pCPSlot->cbNumOfChars	= _hcodepage_8859_NumOfChars;
	pCPSlot->cbGetCharAt	= _hcodepage_8859_GetCharAt;

	pCPSlot->cbUCS2ToCodepage	= _hcodepage_8859_08_ConvertUCS2ToCodepage;
	pCPSlot->cbCodeapgeToUCS2	= _hcodepage_8859_08_ConvertCodepageToUCS2;
	pCPSlot->pusUCS2Table = s_usISO8859_8_uni;

	pCPSlot->stCPTagSlot.eCodepage = eHxCP_ISO_8859_8;
	pCPSlot->stCPTagSlot.szCPTag[0] = eHxCP_DVBLANG_VALUE_8859_08;
	pCPSlot->stCPTagSlot.szCPTag[1] = 0x00;
	pCPSlot->stCPTagSlot.nCPTagLen = 1;
#if defined(TEST_CP)
	pCPSlot->stCPTagSlot.szCPTag2[0] = EN300468_8859_3BTYE_FIRST_VALUE;
	pCPSlot->stCPTagSlot.szCPTag2[1] = EN300468_8859_3BTYE_2ND_VALUE;
	pCPSlot->stCPTagSlot.szCPTag2[2] = EN300468_8859_8_3BYTE_3TH_VALUE;
	pCPSlot->stCPTagSlot.szCPTag2[3] = 0x00;
	pCPSlot->stCPTagSlot.nCPTagLen2 = 3;
#endif

	return ERR_OK;
}

STATIC	HERROR			_hcodepage_8859_9_getinfo(eHxCP_Slot_t *pCPSlot)
{
	if(pCPSlot == NULL)
	{
		return ERR_FAIL;
	}

	pCPSlot->ucEndCharBytes	= 1;

	pCPSlot->cbNumOfBytes	= _hcodepage_8859_NumOfBytes;
	pCPSlot->cbNumOfChars	= _hcodepage_8859_NumOfChars;
	pCPSlot->cbGetCharAt	= _hcodepage_8859_GetCharAt;

	pCPSlot->cbUCS2ToCodepage	= _hcodepage_8859_09_ConvertUCS2ToCodepage;
	pCPSlot->cbCodeapgeToUCS2	= _hcodepage_8859_09_ConvertCodepageToUCS2;
	pCPSlot->pusUCS2Table = s_usISO8859_9_uni;

	pCPSlot->stCPTagSlot.eCodepage = eHxCP_ISO_8859_9;
	pCPSlot->stCPTagSlot.szCPTag[0] = eHxCP_DVBLANG_VALUE_8859_09;
	pCPSlot->stCPTagSlot.szCPTag[1] = 0x00;
	pCPSlot->stCPTagSlot.nCPTagLen = 1;
#if defined(TEST_CP)
	pCPSlot->stCPTagSlot.szCPTag2[0] = EN300468_8859_3BTYE_FIRST_VALUE;
	pCPSlot->stCPTagSlot.szCPTag2[1] = EN300468_8859_3BTYE_2ND_VALUE;
	pCPSlot->stCPTagSlot.szCPTag2[2] = EN300468_8859_9_3BYTE_3TH_VALUE;
	pCPSlot->stCPTagSlot.szCPTag2[3] = 0x00;
	pCPSlot->stCPTagSlot.nCPTagLen2 = 3;
#endif

	return ERR_OK;
}

STATIC	HERROR			_hcodepage_8859_10_getinfo(eHxCP_Slot_t *pCPSlot)
{
	if(pCPSlot == NULL)
	{
		return ERR_FAIL;
	}

	pCPSlot->ucEndCharBytes	= 1;

	pCPSlot->cbNumOfBytes	= _hcodepage_8859_NumOfBytes;
	pCPSlot->cbNumOfChars	= _hcodepage_8859_NumOfChars;
	pCPSlot->cbGetCharAt	= _hcodepage_8859_GetCharAt;

	pCPSlot->cbUCS2ToCodepage	= _hcodepage_8859_10_ConvertUCS2ToCodepage;
	pCPSlot->cbCodeapgeToUCS2	= _hcodepage_8859_10_ConvertCodepageToUCS2;
	pCPSlot->pusUCS2Table = s_usISO8859_10_uni;

	pCPSlot->stCPTagSlot.eCodepage = eHxCP_ISO_8859_10;
	pCPSlot->stCPTagSlot.szCPTag[0] = eHxCP_DVBLANG_VALUE_8859_10;
	pCPSlot->stCPTagSlot.szCPTag[1] = 0x00;
	pCPSlot->stCPTagSlot.nCPTagLen = 1;
#if defined(TEST_CP)
	pCPSlot->stCPTagSlot.szCPTag2[0] = EN300468_8859_3BTYE_FIRST_VALUE;
	pCPSlot->stCPTagSlot.szCPTag2[1] = EN300468_8859_3BTYE_2ND_VALUE;
	pCPSlot->stCPTagSlot.szCPTag2[2] = EN300468_8859_10_3BYTE_3TH_VALUE;
	pCPSlot->stCPTagSlot.szCPTag2[3] = 0x00;
	pCPSlot->stCPTagSlot.nCPTagLen2 = 3;
#endif

	return ERR_OK;
}

STATIC	HERROR			_hcodepage_8859_11_getinfo(eHxCP_Slot_t *pCPSlot)
{
	if(pCPSlot == NULL)
	{
		return ERR_FAIL;
	}

	pCPSlot->ucEndCharBytes	= 1;

	pCPSlot->cbNumOfBytes	= _hcodepage_8859_NumOfBytes;
	pCPSlot->cbNumOfChars	= _hcodepage_8859_NumOfChars;
	pCPSlot->cbGetCharAt	= _hcodepage_8859_GetCharAt;

	pCPSlot->cbUCS2ToCodepage	= _hcodepage_8859_11_ConvertUCS2ToCodepage;
	pCPSlot->cbCodeapgeToUCS2	= _hcodepage_8859_11_ConvertCodepageToUCS2;
	pCPSlot->pusUCS2Table = s_usISO8859_11_uni;

	pCPSlot->stCPTagSlot.eCodepage = eHxCP_ISO_8859_11;
	pCPSlot->stCPTagSlot.szCPTag[0] = eHxCP_DVBLANG_VALUE_8859_11;
	pCPSlot->stCPTagSlot.szCPTag[1] = 0x00;
	pCPSlot->stCPTagSlot.nCPTagLen = 1;
#if defined(TEST_CP)
	pCPSlot->stCPTagSlot.szCPTag2[0] = EN300468_8859_3BTYE_FIRST_VALUE;
	pCPSlot->stCPTagSlot.szCPTag2[1] = EN300468_8859_3BTYE_2ND_VALUE;
	pCPSlot->stCPTagSlot.szCPTag2[2] = EN300468_8859_11_3BYTE_3TH_VALUE;
	pCPSlot->stCPTagSlot.szCPTag2[3] = 0x00;
	pCPSlot->stCPTagSlot.nCPTagLen2 = 3;
#endif

	return ERR_OK;
}
STATIC	HERROR			_hcodepage_8859_13_getinfo(eHxCP_Slot_t *pCPSlot)
{
	if(pCPSlot == NULL)
	{
		return ERR_FAIL;
	}

	pCPSlot->ucEndCharBytes	= 1;

	pCPSlot->cbNumOfBytes	= _hcodepage_8859_NumOfBytes;
	pCPSlot->cbNumOfChars	= _hcodepage_8859_NumOfChars;
	pCPSlot->cbGetCharAt	= _hcodepage_8859_GetCharAt;

	pCPSlot->cbUCS2ToCodepage	= _hcodepage_8859_13_ConvertUCS2ToCodepage;
	pCPSlot->cbCodeapgeToUCS2	= _hcodepage_8859_13_ConvertCodepageToUCS2;
	pCPSlot->pusUCS2Table = s_usISO8859_13_uni;

	pCPSlot->stCPTagSlot.eCodepage = eHxCP_ISO_8859_13;
	pCPSlot->stCPTagSlot.szCPTag[0] = eHxCP_DVBLANG_VALUE_8859_13;
	pCPSlot->stCPTagSlot.szCPTag[1] = 0x00;
	pCPSlot->stCPTagSlot.nCPTagLen = 1;
#if defined(TEST_CP)
	pCPSlot->stCPTagSlot.szCPTag2[0] = EN300468_8859_3BTYE_FIRST_VALUE;
	pCPSlot->stCPTagSlot.szCPTag2[1] = EN300468_8859_3BTYE_2ND_VALUE;
	pCPSlot->stCPTagSlot.szCPTag2[2] = EN300468_8859_13_3BYTE_3TH_VALUE;
	pCPSlot->stCPTagSlot.szCPTag2[3] = 0x00;
	pCPSlot->stCPTagSlot.nCPTagLen2 = 3;
#endif

	return ERR_OK;
}

STATIC	HERROR			_hcodepage_8859_14_getinfo(eHxCP_Slot_t *pCPSlot)
{
	if(pCPSlot == NULL)
	{
		return ERR_FAIL;
	}

	pCPSlot->ucEndCharBytes	= 1;

	pCPSlot->cbNumOfBytes	= _hcodepage_8859_NumOfBytes;
	pCPSlot->cbNumOfChars	= _hcodepage_8859_NumOfChars;
	pCPSlot->cbGetCharAt	= _hcodepage_8859_GetCharAt;

	pCPSlot->cbUCS2ToCodepage	= _hcodepage_8859_14_ConvertUCS2ToCodepage;
	pCPSlot->cbCodeapgeToUCS2	= _hcodepage_8859_14_ConvertCodepageToUCS2;
	pCPSlot->pusUCS2Table = s_usISO8859_14_uni;

	pCPSlot->stCPTagSlot.eCodepage = eHxCP_ISO_8859_14;
	pCPSlot->stCPTagSlot.szCPTag[0] = eHxCP_DVBLANG_VALUE_8859_14;
	pCPSlot->stCPTagSlot.szCPTag[1] = 0x00;
	pCPSlot->stCPTagSlot.nCPTagLen = 1;
#if defined(TEST_CP)
	pCPSlot->stCPTagSlot.szCPTag2[0] = EN300468_8859_3BTYE_FIRST_VALUE;
	pCPSlot->stCPTagSlot.szCPTag2[1] = EN300468_8859_3BTYE_2ND_VALUE;
	pCPSlot->stCPTagSlot.szCPTag2[2] = EN300468_8859_14_3BYTE_3TH_VALUE;
	pCPSlot->stCPTagSlot.szCPTag2[3] = 0x00;
	pCPSlot->stCPTagSlot.nCPTagLen2 = 3;
#endif

	return ERR_OK;
}

STATIC	HERROR			_hcodepage_8859_15_getinfo(eHxCP_Slot_t *pCPSlot)
{
	if(pCPSlot == NULL)
	{
		return ERR_FAIL;
	}

	pCPSlot->ucEndCharBytes	= 1;

	pCPSlot->cbNumOfBytes	= _hcodepage_8859_NumOfBytes;
	pCPSlot->cbNumOfChars	= _hcodepage_8859_NumOfChars;
	pCPSlot->cbGetCharAt	= _hcodepage_8859_GetCharAt;

	pCPSlot->cbUCS2ToCodepage	= _hcodepage_8859_15_ConvertUCS2ToCodepage;
	pCPSlot->cbCodeapgeToUCS2	= _hcodepage_8859_15_ConvertCodepageToUCS2;
	pCPSlot->pusUCS2Table = s_usISO8859_15_uni;

	pCPSlot->stCPTagSlot.eCodepage = eHxCP_ISO_8859_15;
	pCPSlot->stCPTagSlot.szCPTag[0] = eHxCP_DVBLANG_VALUE_8859_15;
	pCPSlot->stCPTagSlot.szCPTag[1] = 0x00;
	pCPSlot->stCPTagSlot.nCPTagLen = 1;
#if defined(TEST_CP)
	pCPSlot->stCPTagSlot.szCPTag2[0] = EN300468_8859_3BTYE_FIRST_VALUE;
	pCPSlot->stCPTagSlot.szCPTag2[1] = EN300468_8859_3BTYE_2ND_VALUE;
	pCPSlot->stCPTagSlot.szCPTag2[2] = EN300468_8859_15_3BYTE_3TH_VALUE;
	pCPSlot->stCPTagSlot.szCPTag2[3] = 0x00;
	pCPSlot->stCPTagSlot.nCPTagLen2 = 3;
#endif

	return ERR_OK;
}

STATIC	HERROR			_hcodepage_8859_16_getinfo(eHxCP_Slot_t *pCPSlot)
{
	if(pCPSlot == NULL)
	{
		return ERR_FAIL;
	}

	pCPSlot->ucEndCharBytes	= 1;

	pCPSlot->cbNumOfBytes	= _hcodepage_8859_NumOfBytes;
	pCPSlot->cbNumOfChars	= _hcodepage_8859_NumOfChars;
	pCPSlot->cbGetCharAt	= _hcodepage_8859_GetCharAt;

	pCPSlot->cbUCS2ToCodepage	= _hcodepage_8859_16_ConvertUCS2ToCodepage;
	pCPSlot->cbCodeapgeToUCS2	= _hcodepage_8859_16_ConvertCodepageToUCS2;
	pCPSlot->pusUCS2Table = s_usISO8859_16_uni;

	pCPSlot->stCPTagSlot.eCodepage = eHxCP_ISO_8859_16;
	pCPSlot->stCPTagSlot.szCPTag[0] = eHxCP_DVBLANG_VALUE_8859_16;
	pCPSlot->stCPTagSlot.szCPTag[1] = 0x00;
	pCPSlot->stCPTagSlot.nCPTagLen = 1;

#if defined(TEST_CP)
	pCPSlot->stCPTagSlot.szCPTag2[0] = 0x00;
	pCPSlot->stCPTagSlot.nCPTagLen2 = 0;
#endif

	return ERR_OK;
}



/* *************************************************************************************************** */
#define _______8859_Private_FUNCTION___________________________________________________________________
/* *************************************************************************************************** */




STATIC	HERROR			_hcodepage_8859_addcodepage(void)
{
	HERROR		hErr = ERR_FAIL;
	eHxCP_Slot_t	stCPSlot;

	HxSTD_MemSet(&stCPSlot, 0, sizeof(eHxCP_Slot_t));
	hErr = _hcodepage_8859_1_getinfo(&stCPSlot);
	if(hErr == ERR_OK)
	{
		hErr = HLIB_CP_AddCodepage(&stCPSlot);
	}

	HxSTD_MemSet(&stCPSlot, 0, sizeof(eHxCP_Slot_t));
	hErr = _hcodepage_8859_2_getinfo(&stCPSlot);
	if(hErr == ERR_OK)
	{
		hErr = HLIB_CP_AddCodepage(&stCPSlot);
	}

	HxSTD_MemSet(&stCPSlot, 0, sizeof(eHxCP_Slot_t));
	hErr = _hcodepage_8859_3_getinfo(&stCPSlot);
	if(hErr == ERR_OK)
	{
		hErr = HLIB_CP_AddCodepage(&stCPSlot);
	}

	HxSTD_MemSet(&stCPSlot, 0, sizeof(eHxCP_Slot_t));
	hErr = _hcodepage_8859_4_getinfo(&stCPSlot);
	if(hErr == ERR_OK)
	{
		hErr = HLIB_CP_AddCodepage(&stCPSlot);
	}

	HxSTD_MemSet(&stCPSlot, 0, sizeof(eHxCP_Slot_t));
	hErr = _hcodepage_8859_5_getinfo(&stCPSlot);
	if(hErr == ERR_OK)
	{
		hErr = HLIB_CP_AddCodepage(&stCPSlot);
	}

	HxSTD_MemSet(&stCPSlot, 0, sizeof(eHxCP_Slot_t));
	hErr = _hcodepage_8859_6_getinfo(&stCPSlot);
	if(hErr == ERR_OK)
	{
		hErr = HLIB_CP_AddCodepage(&stCPSlot);
	}

	HxSTD_MemSet(&stCPSlot, 0, sizeof(eHxCP_Slot_t));
	hErr = _hcodepage_8859_7_getinfo(&stCPSlot);
	if(hErr == ERR_OK)
	{
		hErr = HLIB_CP_AddCodepage(&stCPSlot);
	}

	HxSTD_MemSet(&stCPSlot, 0, sizeof(eHxCP_Slot_t));
	hErr = _hcodepage_8859_8_getinfo(&stCPSlot);
	if(hErr == ERR_OK)
	{
		hErr = HLIB_CP_AddCodepage(&stCPSlot);
	}

	HxSTD_MemSet(&stCPSlot, 0, sizeof(eHxCP_Slot_t));
	hErr = _hcodepage_8859_9_getinfo(&stCPSlot);
	if(hErr == ERR_OK)
	{
		hErr = HLIB_CP_AddCodepage(&stCPSlot);
	}

	HxSTD_MemSet(&stCPSlot, 0, sizeof(eHxCP_Slot_t));
	hErr = _hcodepage_8859_10_getinfo(&stCPSlot);
	if(hErr == ERR_OK)
	{
		hErr = HLIB_CP_AddCodepage(&stCPSlot);
	}

	HxSTD_MemSet(&stCPSlot, 0, sizeof(eHxCP_Slot_t));
	hErr = _hcodepage_8859_11_getinfo(&stCPSlot);
	if(hErr == ERR_OK)
	{
		hErr = HLIB_CP_AddCodepage(&stCPSlot);
	}

	HxSTD_MemSet(&stCPSlot, 0, sizeof(eHxCP_Slot_t));
	hErr = _hcodepage_8859_13_getinfo(&stCPSlot);
	if(hErr == ERR_OK)
	{
		hErr = HLIB_CP_AddCodepage(&stCPSlot);
	}

	HxSTD_MemSet(&stCPSlot, 0, sizeof(eHxCP_Slot_t));
	hErr = _hcodepage_8859_14_getinfo(&stCPSlot);
	if(hErr == ERR_OK)
	{
		hErr = HLIB_CP_AddCodepage( &stCPSlot);
	}

	HxSTD_MemSet(&stCPSlot, 0, sizeof(eHxCP_Slot_t));
	hErr = _hcodepage_8859_15_getinfo(&stCPSlot);
	if(hErr == ERR_OK)
	{
		hErr = HLIB_CP_AddCodepage(&stCPSlot);
	}

	HxSTD_MemSet(&stCPSlot, 0, sizeof(eHxCP_Slot_t));
	hErr = _hcodepage_8859_16_getinfo(&stCPSlot);
	if(hErr == ERR_OK)
	{
		hErr = HLIB_CP_AddCodepage(&stCPSlot);
	}

	return ERR_OK;
}



/* *************************************************************************************************** */
#define ___PUBLIC_FUNCTION_______________________________________________________________________________
/* *************************************************************************************************** */


HERROR	INT_HCodepage_8859(void)
{
	HERROR		hErr = ERR_FAIL;

	hErr = _hcodepage_8859_addcodepage();
	if(hErr != ERR_OK)
	{
		CP_Print(DBG_PRINT, ("Error 8859 Init\n"));
	}

	return hErr;
}


/*********************** End of File ******************************/


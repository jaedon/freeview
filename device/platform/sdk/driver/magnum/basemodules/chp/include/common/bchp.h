/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp.h $
 * $brcm_Revision: Hydra_Software_Devel/42 $
 * $brcm_Date: 7/20/12 4:29p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/include/common/bchp.h $
 * 
 * Hydra_Software_Devel/42   7/20/12 4:29p rjlewis
 * SW7425-3461: Added more info fields.
 * 
 * Hydra_Software_Devel/41   7/2/12 11:43a rjlewis
 * SW7425-2144: temperature could actually be negative.
 * 
 * Hydra_Software_Devel/40   2/10/12 5:24p rjlewis
 * SW7425-2218: removed AvsLock/Unlock and added get handle.
 * 
 * Hydra_Software_Devel/39   2/1/12 7:03p rjlewis
 * SW7425-2216: Adding support for AVS Lock/Unlock.
 * 
 * Hydra_Software_Devel/38   10/19/11 10:29a rjlewis
 * SW7346-117: fixed term (milli for kilo)
 * 
 * Hydra_Software_Devel/37   9/1/11 5:43p jtna
 * SW7552-115: change name to BCHP_Feature_eRfmCapable
 * 
 * Hydra_Software_Devel/36   9/1/11 6:18p xhuang
 * SW7552-115: Add BCHP features for RFM
 * 
 * Hydra_Software_Devel/35   8/16/11 11:32a rjlewis
 * SW7346-117: Added support for new chip specific Enter/Exit Stand-by
 * mode.
 * 
 * Hydra_Software_Devel/34   8/3/11 5:45p rjlewis
 * SW7346-117: Removed BCHP tests -- handled in comments instead.
 * 
 * Hydra_Software_Devel/33   8/3/11 12:00p rjlewis
 * SW7346-117: added support for GetAvsData function.
 * 
 * Hydra_Software_Devel/32   7/27/11 9:42a davidp
 * SW7552-75: Add BCHP_Feature_eAVDCoreFreq.
 * 
 * Hydra_Software_Devel/31   7/13/11 6:03p pntruong
 * SWDTV-7838: Merged.  Included additional macros to improve code
 * formatting, and masked field data/enum to prevent overflow.
 * 
 * Hydra_Software_Devel/31   7/13/11 6:03p pntruong
 * SWDTV-7838: Merged.  Included additional macros to improve code
 * formatting, and masked field data/enum to prevent overflow.
 *
 * Hydra_Software_Devel/SWDTV-7838/1   7/12/11 3:04p jhaberf
 * SWDTV-7838: Added/modified some macros in order to making working with
 * register bit fields easier
 *
 * Hydra_Software_Devel/30   1/20/11 1:40p gmohile
 * SW7408-212 : Add support for avs settings
 *
 * Hydra_Software_Devel/29   10/5/10 10:56a davidp
 * SW7422-22: Add memory controller BCHP_feature enums to allow XVD to
 * internally set stripe and PFRI registers.
 *
 * Hydra_Software_Devel/28   10/28/08 12:39p maivu
 * PR48489: fix asm compilation of bchp.h. Checkin for Kevin.
 *
 * Hydra_Software_Devel/28   10/28/08 12:39p maivu
 * PR48489: fix asm compilation of bchp.h. Checkin for Kevin.
 *
 * Hydra_Software_Devel/28   10/28/08 11:45a cernekee
 * PR48489: fix asm compilation of bchp.h
 *
 * Hydra_Software_Devel/27   12/20/07 11:54a jessem
 * PR 38367: Added BCHP_Feature_eMemCtrl2Capable.
 *
 * Hydra_Software_Devel/26   10/31/06 9:09a erickson
 * PR24374: deprecate BCHP_REV_XX in magnum for 7401
 *
 * Hydra_Software_Devel/25   8/8/06 4:48p pntruong
 * PR22572: Added feature detection for second ddr memory controller.
 *
 * Hydra_Software_Devel/24   8/4/06 5:41p pntruong
 * PR22572: Need initial 93563 appframework bringup.
 *
 * Hydra_Software_Devel/24   8/4/06 5:40p pntruong
 * PR22572: Need initial 93563 appframework bringup.
 *
 * Hydra_Software_Devel/23   6/27/06 5:52p dlwin
 * PR 22372: Added support E[0-5] chip version.
 *
 * Hydra_Software_Devel/22   3/7/06 2:15p jasonh
 * PR 20059: Updated to work with revisions up to A5/B5/C5/D5
 *
 * Hydra_Software_Devel/21   1/23/06 11:43a mphillip
 * PR18165: Update for C3
 *
 * Hydra_Software_Devel/20   10/21/05 2:32p shyam
 * PR 17148 : Cut paste problem for B2 defines
 *
 * Hydra_Software_Devel/19   9/15/05 11:12a erickson
 * PR17148: added BCHP_REV_XX defines for more backward compatibility
 *
 * Hydra_Software_Devel/18   9/15/05 10:55a erickson
 * PR17148: moved BCHP checked from bstd.h and added backward
 * compatibility code (temporarily)
 *
 * Hydra_Software_Devel/17   9/14/05 2:14p erickson
 * PR17148: added BCHP_VER_XX definitions
 *
 * Hydra_Software_Devel/16   3/8/05 4:18p jasonh
 * PR 14339: Added support for 3DES feature check.
 *
 * Hydra_Software_Devel/15   2/24/05 10:21a jasonh
 * PR 14230: Added HDCP bond status detection.
 *
 * Hydra_Software_Devel/14   10/26/04 5:02p jasonh
 * PR 13068: Added detection of 2nd MPEG decoder.
 *
 * Hydra_Software_Devel/13   9/21/04 2:40p jasonh
 * PR 12728: Fix -pdantic errors
 *
 * Hydra_Software_Devel/12   6/10/04 6:04p jasonh
 * PR 11451: Added 3D graphics capable check for 7038.
 *
 * Hydra_Software_Devel/11   6/10/04 5:53p jasonh
 * PR 11257: Adding DVO port cabable check.
 *
 * Hydra_Software_Devel/10   6/10/04 5:43p jasonh
 * PR 11257: Added initial support of BCHP_GetFeature for 7038 chipsets.
 *
 * Hydra_Software_Devel/9   4/14/04 5:16p marcusk
 * PR 10602: Updated comments
 *
 * Hydra_Software_Devel/8   4/9/04 4:32p jasonh
 * PR 10548: Changed private shift function to non-private.
 *
 * Hydra_Software_Devel/7   12/24/03 11:34a pntruong
 * PR9117: Safe guard macro parameters, and removed trailing white spaces.
 *
 * Hydra_Software_Devel/6   12/24/03 10:58a marcusk
 * PR9117: Updated interrupt macros to be named consistent
 *
 * Hydra_Software_Devel/5   12/23/03 4:20p marcusk
 * PR9078: Updated with interrupt id creation and extraction macros.
 *
 * Hydra_Software_Devel/4   12/5/03 6:08p dlwin
 * PR 8873: Updated comment for BCHP_FIELD_ENUM() and  BCHP_FIELD_DATA()
 * removing the MASKing, since these marcos don't MASK.
 *
 * Hydra_Software_Devel/3   10/10/03 2:40p dlwin
 * Updated sample code.
 *
 * Hydra_Software_Devel/2   9/26/03 8:49a dlwin
 * Updated to return 16 bits for Chip ID and Chip Rev.
 *
 * Hydra_Software_Devel/1   9/23/03 9:47p dlwin
 * Initial version
 *
 ***************************************************************************/
#ifndef BCHP_H__
#define BCHP_H__

#ifndef __ASSEMBLY__
#include "bstd.h"
#include "breg_mem.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*=************************ Module Overview ********************************
<verbatim>

Overview
The purpose of this module is to give information about the chip that you
are using, such as chip ID and revision, as well as provide the register
list required to program the chip.

The chip interface contains the defines for all registers and each field
of the register for the entire chip. These defines are automatically
generated from RDB (Register DataBase) and are contained in header files
found in the basemodules\chp\{chip}\rdb\ directory. Individual header
files may be included directly or all can be included by using the header
file bchp_{chip}.h.

Registers must be read using the functions found in the register interface.
Field access must be done using the macros provided here in the
chip interface. These macros are as follows:

   o BCHP_GET_FIELD_DATA
   o BCHP_FIELD_ENUM
   o BCHP_FIELD_DATA
   o BCHP_MASK


Design
  The design for BCHP PI API is broken into three parts.

o Part 1 (open/close/configuration):

    These APIs are used for opening and closing BCHP device.

o Part 2 (get):

    These APIs are used for getting chip information, like Family Id, Chip Id, etc.

o Part 3 (get macro/set macro):

    These macros are provided to access bit field inside a register using
    RDB documenation.  See above discussion on this topic.


Usage
The usage of BCHP involves the following:

   * Configure/Open of BCHP

      * Get chip defaults for chips that exist on target system
      * Using these chip defaults, for each chip that requires a chip handle it
	    default settings for that chip.


   * Get chip information

      * Use BCHP_GetChipInfo() to get chip infromation
	  * Use bit field macros to access bit fields inside register that conforms
	    to RDB.


Interrupt Requirements
None


Sample Code

// Example of Bit Field Macros
{
	uint32_t ulReg;
	BREG_Read32(hReg, CMP_0_BLEND_0_CTRL, &ulReg);

	ulReg &= ~(
		BCHP_MASK(CMP_0_BLEND_0_CTRL, BLEND_SOURCE            ) |
		BCHP_MASK(CMP_0_BLEND_0_CTRL, CONSTANT_ALPHA          ) |
		BCHP_MASK(CMP_0_BLEND_0_CTRL, BACK_COLOR_BLEND_FACTOR ) |
		BCHP_MASK(CMP_0_BLEND_0_CTRL, FRONT_COLOR_BLEND_FACTOR));

	ulReg |= (
		BCHP_FIELD_ENUM(CMP_0_BLEND_0_CTRL, BLEND_SOURCE,
			SURFACE_V0    ) |
		BCHP_FIELD_DATA(CMP_0_BLEND_0_CTRL, CONSTANT_ALPHA,
			0xff          ) |
		BCHP_FIELD_ENUM(CMP_0_BLEND_0_CTRL, BACK_COLOR_BLEND_FACTOR,
			ZERO          ) |
		BCHP_FIELD_ENUM(CMP_0_BLEND_0_CTRL, FRONT_COLOR_BLEND_FACTOR,
			CONSTANT_ALPHA));

	BREG_Write32(hReg, CMP_0_BLEND_0_CTRL, ulReg);
}

//
// NOTE: The following sample code does not do any error checking.
//
// In this example, the target H/W board contains two chips that requires
// Chip Handles, Bcm7038 and Bcm3250.
//
static BCHP_Handle hChip7038;
static BREG_Handle hReg7038;
static BCHP_Handle hChip3250;
static BREG_Handle hReg3250;

main( void )
{
	BERR_Code retCode = BERR_SUCCESS;
	uint32 chipId;
	uint32 chipRev;

	// Initialize hReg7038, hReg3250 . . .

	// Now create the chip interface for the two chips
	BCHP_Open7038( &hChip7038, hReg7038 );
	BCHP_Open3250( &hChip3250, hReg3250 );

	// Get the chip information for Bcm7038
	BCHP_GetChipInfo( hChip7038, &eChipId, &eChipRev );
	printf( "Chip Information for Bcm7038\n" );
	printf( "ChipId=%d, ChipRev=%d\n", chipId, chipRev );

	// Get the chip information for Bcm3250
	BCHP_GetChipInfo( hChip3250, &eChipId, &eChipRev );
	printf( "Chip Information for Bcm3250\n" );
	printf( "ChipId=%d, ChipRev=%d\n", chipId, chipRev );
}


</verbatim>
***************************************************************************/


/***************************************************************************
Summary:
	Extracts the bitfield value of a register using RDB name.

Description:
	This macro extracts the bitfield value of a register using RDB name. It
	intends to hide away the mechanics of SHIFTing/ORing/MASKing to get the
	value.

Input:
	Memory - Memory variable of a register to extract the bitfield. The
	value of this variable is usually get filled with a BREG_ReadXX call.

	Register - Register instance name defined in RDB. These names are
	available in html documentations, and the RDB generated header files.
	Note this a  register instance name; not a regtype.

	Field - Field name defined in RDB. These names are available in html
	documentations, and the RDB generated header files.

Output:
	<None>

Returns:
	The extracted bitfield value.

Example:
	ulData = BCHP_GET_FIELD_DATA(ulReg, CMP_0_BLEND_0_CTRL, BLEND_SOURCE);

See Also:
	BCHP_FIELD_DATA, BCHP_FIELD_ENUM.
**************************************************************************/
#define BCHP_GET_FIELD_DATA(Memory,Register,Field) \
	((((Memory) & BCHP_MASK(Register,Field)) >> \
	BCHP_SHIFT(Register,Field)))

/***************************************************************************
Summary:
	Push a register bitfield value into a memory location using RDB
	name.

Description:
	This macro sets a register bitfield value into a memory location
	using an RDB name. It intends to hide away the mechanics of
	ANDing/SHIFTing/ORing/MASKing to set the value.

Input:
	Memory - Memory variable to set the value of the bitfield. The
	value of this variable is usually filled initially with a
	BREG_ReadXX call.

	Register - Register instance name defined in RDB. These names are
	available in html documentations, and the RDB generated header files.
	Note this a  register instance name; not a regtype.

	Field - Field name defined in RDB. The "Field" will be set to zero in
	"Memory" before "FieldValue" os ORed in. These names are available in
	html documentations, and the RDB generated header files.

	FieldValue - This is the value that the register field is to be set
	to. The FieldValue will SHIFTed and MASKed before being ORed into
	"Memory"

Output:
	The memory location specified by "Memory" will have the register bit
	field specified by "Field" set to "FieldValue".

Returns:
	<None>.

Example:
	ulReg = BREG_Read32( hRegister, BCHP_V3D_PLL_CTRL_PLL_6CH_CTRL_0 );
	BCHP_SET_FIELD_DATA( ulReg, V3D_PLL_CTRL_PLL_6CH_CTRL_0, POST_RESETB, 0 );
	BREG_Write32( hRegister, BCHP_V3D_PLL_CTRL_PLL_6CH_CTRL_0, ulReg );

See Also:
	BCHP_SET_FIELD_ENUM.
**************************************************************************/
#define BCHP_SET_FIELD_DATA(Memory,Register,Field,FieldValue) \
	((Memory)=(((Memory)&(~BCHP_MASK(Register,Field))) | \
	 BCHP_FIELD_DATA(Register,Field,FieldValue)))

/***************************************************************************
Summary:
	Push a register bitfield value into a memory location using RDB
	name.

Description:
	This macro sets a register bitfield value into a memory location
	using an RDB name. It intends to hide away the mechanics of
	ANDing/SHIFTing/ORing/MASKing to set the value.

Input:
	Memory - Memory variable to set the value of the bitfield. The
	value of this variable is usually filled initially with a
	BREG_ReadXX call.

	Register - Register instance name defined in RDB. These names are
	available in html documentations, and the RDB generated header files.
	Note this a  register instance name; not a regtype.

	Field - Field name defined in RDB. The "Field" will be set to zero in
	"Memory" before "FieldValue" os ORed in. These names are available in
	html documentations, and the RDB generated header files.

	Name - A value name that get push into the regiser bitfield.
	This name is available in html documentations (italicized) in the
	bitfield definition.

Output:
	The memory location specified by "Memory" will have the register bit
	field specified by "Field" set to "Name".

Returns:
	<None>.

Example:
	ulReg = BREG_Read32( hRegister, BCHP_VDEC_FE_0_SYNC_DETECT );
	BCHP_SET_FIELD_ENUM( ulReg, VDEC_FE_0_SYNC_DETECT, HEAD_SWITCH_SEL, FOUR_HEAD );
	BREG_Write32( hRegister, BCHP_VDEC_FE_0_SYNC_DETECT, ulReg );

See Also:
	BCHP_SET_FIELD_DATA
**************************************************************************/
#define BCHP_SET_FIELD_ENUM(Memory,Register,Field,Name) \
	((Memory)=(((Memory)&(~BCHP_MASK(Register,Field))) | \
	    BCHP_FIELD_ENUM(Register,Field,Name)))

/***************************************************************************
Summary:
	Push a value into a register bitfield using RDB name.

Description:
	This macro push a value into a register bitfield using RDB name. It
	intends to hide away the mechanics of SHIFTing/ORing/MASKing to push
	the value into a register bitfield.

Input:
	Memory - Memory variable of a register to be modified. The value
	of this variable is usually get filled with a BREG_ReadXX call.

	Register - Register instance name defined in RDB. These names are
	available in html documentations, and the RDB generated header files.
	Note this a register instance name; not a regtype.

	Field - Field name defined in RDB. These names are available in html
	documentations, and the RDB generated header files.

	Data - A data value that get push into the register bitfield.

Output:
	Memory - Memory variable will get modifled with the new value (Data)
	in approriate bitfield.

Returns:
	<None>

Example:
	ulReg |= BCHP_FIELD_DATA(CMP_0_BLEND_0_CTRL, CONSTANT_ALPHA, 0xff);

See Also:
	BCHP_GET_FIELD_DATA, BCHP_FIELD_ENUM, BCHP_SHIFT
**************************************************************************/
#define BCHP_FIELD_DATA(Register,Field,Data) \
	(((Data) << BCHP_SHIFT(Register,Field))&BCHP_MASK(Register,Field))

/***************************************************************************
Summary:
	Push a  value name into a register bitfield using RDB name.

Description:
	This macro will push a value name into a register bitfield using RDB
        name. It intends to hide away the mechanics of MASKing/SHIFTing/ORing
        to push the value into a register bitfield.

Input:
	Memory - Memory variable of a register to be modified.  The value
	of this variable is usually get filled with a BREG_ReadXX call.

	Register - Register instance name defined in RDB.  These names are
	available in html documentations, and the RDB generated header files.
	Note this a register instance name; not a regtype.

	Field - Field name defined in RDB.  These names are available in html
	documentations, and the RDB generated header files.

	Name - A value name that get push into the regiser bitfield.
	This name is available in html documentations (italicized) in the
	bitfield definition.

Output:
	Memory - Memory variable will get modified with the new value name
	(Name) in approriate bitfield.

Returns:
	<None>

Example:
	ulReg |= BCHP_FIELD_ENUM(CMP_0_BLEND_0_CTRL,
		FRONT_COLOR_BLEND_FACTOR, CONSTANT_ALPHA)

See Also:
	BCHP_FIELD_DATA, BCHP_GET_FIELD_DATA, BCHP_SHIFT
**************************************************************************/
#define BCHP_FIELD_ENUM(Register,Field,Name) \
	BCHP_FIELD_DATA(Register,Field, BCHP_##Register##_##Field##_##Name)

/*************************************************************************
Summary:
	Provide a mask for a specific register field.

Description:
	This macro returns a mask for the specified register field. This value
	is typically used with other masks to clear out bits of a register
	before setting in a read/modify/write operation.

Input:
	Register - Register instance name defined in RDB. These names are available
	in html documentations, and the RDB generated header files. Note this a
	register instance name; not a regtype.

	Field - Field name defined in RDB. These names are available in html
	documentations, and the RDB generated header files.

Output:
	<None>

Returns:
	The mask of the register field.

Example:
	ulReg &= ~BCHP_MASK(CMP_0_BLEND_0_CTRL, BLEND_SOURCE);

See Also:
	BCHP_GET_FIELD_DATA,
	BCHP_FIELD_DATA, BCHP_FIELD_ENUM, BCHP_SHIFT
**************************************************************************/
#define BCHP_MASK(Register,Field) \
	BCHP_##Register##_##Field##_MASK

/*************************************************************************
Summary:
	Provide a shift for a specific register field.

Description:
	This macro returns a shift value for the specified register field.
	This value is typically used by the BCHP_FIELD_NAME and
	BCHP_FIELD_ENUM macros to shift field values into the proper
	location. In other modules this value is used to determine a shift
	amount used in interrupt processing.

Input:
	Register - Register instance name defined in RDB. These names are available
	in html documentations, and the RDB generated header files. Note this a
	register instance name; not a regtype.

	Field - Field name defined in RDB. These names are available in html
	documentations, and the RDB generated header files.

Output:
	<None>

Returns:
	The shift value of the register field.

Example:
	ulShift = BCHP_SHIFT(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_00);

See Also:
	BCHP_MASK, BCHP_GET_FIELD_DATA,
	BCHP_FIELD_DATA, BCHP_FIELD_ENUM.
**************************************************************************/
#define BCHP_SHIFT(Register,Field) \
	BCHP_##Register##_##Field##_SHIFT

/*{secret}******************************************************************

Summary:
	Used only by BCHP interface to create interrupt IDs

	Interrupt IDs consist of the L2 interrupt register address
	or'd with an interrupt bit shift value.  Since L2 registers
	are 32 bits in length the shift value must range from 0-31.
	This requires 5 bits to represent the shift value.

	Since all L2 interrupt register addresses are 32 bit aligned and
	can be no larger than 0x1FFFFFF the register address is shifted
	up by three and the unused lower two bits are reused to store
	the interrupt shift.

See Also:
	BCHP_INT_ID_REG_SHIFT, BCHP_INT_ID_SHIFT_MASK,
	BCHP_INT_ID_CREATE, BCHP_INT_ID_GET_REG, BCHP_INT_ID_REG_SHIFT
**************************************************************************/
#define BCHP_INT_ID_REG_SHIFT            3

/*{secret}******************************************************************

Summary:
	Used only by BCHP interface to create interrupt IDs

	Interrupt IDs consist of the L2 interrupt register address
	or'd with an interrupt bit shift value.  Since L2 registers
	are 32 bits in length the shift value must range from 0-31.
	This requires 5 bits to represent the shift value.

	Since all L2 interrupt register addresses are 32 bit aligned and
	can be no larger than 0x1FFFFFF the register address is shifted
	up by three and the unused lower two bits are reused to store
	the interrupt shift.

See Also:
	BCHP_INT_ID_REG_SHIFT, BCHP_INT_ID_SHIFT_MASK,
	BCHP_INT_ID_CREATE, BCHP_INT_ID_GET_REG, BCHP_INT_ID_REG_SHIFT
**************************************************************************/
#define BCHP_INT_ID_SHIFT_MASK           0x0000001F

/*{secret}******************************************************************

Summary:
	Used only by BCHP interface to create interrupt IDs

See Also:
	BCHP_INT_ID_REG_SHIFT, BCHP_INT_ID_SHIFT_MASK,
	BCHP_INT_ID_CREATE, BCHP_INT_ID_GET_REG, BCHP_INT_ID_REG_SHIFT
**************************************************************************/
#define BCHP_INT_ID_CREATE( reg, shift ) \
	(((reg) << BCHP_INT_ID_REG_SHIFT) | (shift))

/*{secret}******************************************************************

Summary:
	Used by interrupt interface module to extract registers and shift values
	from a BINT_Id

See Also:
	BCHP_INT_ID_REG_SHIFT, BCHP_INT_ID_SHIFT_MASK,
	BCHP_INT_ID_CREATE, BCHP_INT_ID_GET_REG, BCHP_INT_ID_REG_SHIFT
**************************************************************************/
#define BCHP_INT_ID_GET_REG( intId ) \
	(((intId) & (~BCHP_INT_ID_SHIFT_MASK)) >> BCHP_INT_ID_REG_SHIFT)

/*{secret}******************************************************************

Summary:
	Used by interrupt interface module to extract registers and shift
	values from a BINT_Id

See Also:
	BCHP_INT_ID_REG_SHIFT, BCHP_INT_ID_SHIFT_MASK,
	BCHP_INT_ID_CREATE, BCHP_INT_ID_GET_REG, BCHP_INT_ID_REG_SHIFT
**************************************************************************/
#define BCHP_INT_ID_GET_SHIFT( intId ) \
	((intId) & BCHP_INT_ID_SHIFT_MASK)

#ifndef __ASSEMBLY__
/***************************************************************************
Summary:
	Chip Interface Handle.

Description:
	TODO: Will add the rest of the API later.  I'm declaring it here
	to get things compile.

See Also:
	BCHP_Open(), BCHP_Close().
**************************************************************************/
typedef struct BCHP_P_Context *BCHP_Handle; /* Opaque handle */

/***************************************************************************
Summary:
	This enumeration represents possible chipset features to query.

Description:
	This enumeration is provided to a call to BCHP_GetFeature
	in order to determine the capabilities of a given chipset.

	Each enumeration has a specific type associated with it. All feature
	enumerations that end with 'Capable' refer to a bool type. All feature
	enumerations that end with a 'Count' refer to an int type.

See Also:
	BCHP_GetFeature.
***************************************************************************/
typedef enum BCHP_Feature
{
    BCHP_Feature_e3DGraphicsCapable,   /* 3D capable? (bool) */
    BCHP_Feature_eDvoPortCapable,      /* DVO Port capable? (bool) */
    BCHP_Feature_eMacrovisionCapable,  /* Chip is macrovision capable? (bool) */
    BCHP_Feature_eMpegDecoderCount,    /* Number of MPEG decoders (int) */
    BCHP_Feature_eHdcpCapable,         /* HDCP capable? (bool) */
    BCHP_Feature_e3desCapable,         /* 3DES capable? (bool) */
    BCHP_Feature_e1080pCapable,        /* 1080p Capable? (bool) */
    BCHP_Feature_eRfmCapable,          /* RFM capable? (bool) */
    BCHP_Feature_eMemCtrl1Capable,            /* Second DDR memory controller present (bool). */
    BCHP_Feature_eMemCtrl2Capable,            /* Third DDR memory controller present (bool). */
    BCHP_Feature_eMemCtrl0DDR3ModeCapable,    /* True = DDR3 */
    BCHP_Feature_eMemCtrl0DDRDeviceTechCount, /* Size of memory part in MBits ie: 256, 512, 1024 */
    BCHP_Feature_eMemCtrl0DramWidthCount,     /* Size of data path 16, 32, 64 bits */
    BCHP_Feature_eMemCtrl1DDR3ModeCapable,    /* True = DDR3 */
    BCHP_Feature_eMemCtrl1DDRDeviceTechCount, /* Size of memory part in MBits ie: 256, 512, 1024 */
    BCHP_Feature_eMemCtrl1DramWidthCount,     /* Size of data path 16, 32, 64 bits */
    BCHP_Feature_eAVDCoreFreq                 /* AVD Core freqencey */
} BCHP_Feature;

/***************************************************************************
Summary:
	This structure represents settings for avs monitor

Description:
	This structure is provided to a call to BCHP_Monitor_Pvt
	Note that this data is only used by 65nm parts that support AVS hardware.

See Also:
	BCHP_Monitor_Pvt.
***************************************************************************/
typedef struct BCHP_AvsSettings
{
    unsigned hardStopOffset;         /* Range 0-15. This parameter should be programmed as per the
                                        board voltage drop (through PCB and filter components) between
                                        the output of the regulator [VREG_VFB_1P2 net] and the
                                        supply of the chip [D1.2V_BCM75XX]. Each unit is ~12.5mV.
                                        If the board design  has 20mV of drop then hardStopOffset=2
                                        should be programmed */
    unsigned maxVoltageStopOffset;   /* Percentage reduction 0-15 from maximum +16% Vreg setting (at regulator).
                                        Set this parameter to 4 if ambient temperature is expected
                                        to rise above +70C for extended period. */
} BCHP_AvsSettings;


/***************************************************************************
Summary:
    The data structure for receiving current AVS data.

Description:
    The AVS PVT Monitor can collect various pieces of information.
    This is used to return that information to the caller.
	Note that this data is only valid for 40nm parts that support AVS hardware.

See Also:
    BCHP_GetAvsData()

****************************************************************************/
typedef struct {
    unsigned voltage;     /* this is the last measured voltage (in millivolts) */
                          /* to convert use: float V = voltage/1000.; */
    signed temperature;   /* this is the last converted temperature (in thousands) read from internal chip thermister */
                          /* to convert use: float centigrade = temperature/1000.; */
    bool enabled;         /* AVS is enabled in the bootloader/CFE */
    bool tracking;        /* AVS-tracking is enabled in Nexus system software */
} BCHP_AvsData;


/***************************************************************************
Summary:
	Close a chip handle.

Description:
	Close the chip release internal resource.

See Also:
	BCHP_Open
**************************************************************************/
BERR_Code BCHP_Close(
	BCHP_Handle hChip                /* [in] Chip handle */
	);

/***************************************************************************
Summary:
	Get the chip information.

Description:
	This function queries the chip id and its revision.

See Also:
	BCHP_Close
**************************************************************************/
BERR_Code BCHP_GetChipInfo(
	const BCHP_Handle hChip,         /* [in] Chip handle */
	uint16_t *pChipId,               /* [out] Chip Id */
	uint16_t *pChipRev               /* [out] Chip Rev. */
	);

/***************************************************************************
Summary:
	Get a chip feature.

Description:
	This function returns specific information about a feature of a
	chip. If the feature enumeration is understood by the specific
	chip's interface, an appropriate value is returned. If the
	feature enumeration is unhandled, this function will return
	BERR_UNKNOWN meaning it is unknown whether this feature is supported
	by this chipset.

	The type of the returned value (pFeatureValue) is specified in the
	documentation of the feature enumeration. All feature enumerations
	that end with 'Capable' return a bool type. All feature enumerations
	that end with a 'Count' return an int type.

See Also:
	BCHP_Close
**************************************************************************/
BERR_Code BCHP_GetFeature(
	const BCHP_Handle hChip,         /* [in] Chip handle */
	const BCHP_Feature eFeature,     /* [in] Feature to be queried. */
	void *pFeatureValue              /* [out] Feature value. */
	);

/***************************************************************************
Summary:
	Monitor adaptive voltage scaling.

Description:
	This function is called to adjust the voltage in response to
        ring oscillator frequencies

See Also:
	BCHP_Close
**************************************************************************/
void BCHP_MonitorPvt(
	BCHP_Handle hChip,              /* [in] Chip handle */
	BCHP_AvsSettings *pSettings     /* [in] AVS settings. */
	);

/***************************************************************************
Summary:
    Get the current AVS data.

Description:
    This can be used to get various data associated with the AVS hardware.
	This data will only be valid on 40nm parts that support AVS hardware.

See Also:
    BCHP_Open

**************************************************************************/
BERR_Code BCHP_GetAvsData(
    BCHP_Handle hChip,   /* [in] Chip handle */
    BCHP_AvsData *pData  /* [out] pointer to location to return data */
	);

/***************************************************************************
Summary:
    Enter chip Standby mode.

Description:
    This will perform chip specific functions needed to enter power stand-by mode.

See Also:
    BCHP_Open

**************************************************************************/
BERR_Code BCHP_Standby(
    BCHP_Handle hChip   /* [in] Chip handle */
	);

/***************************************************************************
Summary:
    Exit chip Standby mode.

Description:
    This will perform chip specific functions needed to exit power stand-by mode.

See Also:
    BCHP_Open

**************************************************************************/
BERR_Code BCHP_Resume(
    BCHP_Handle hChip   /* [in] Chip handle */
	);

/***************************************************************************
Summary:
    Get handle for AVS module.

Description:
    This is used for testing purposes to gain access to AVS priv functions.
	This will return NULL on platforms that do not support AVS.

See Also:
    BCHP_Open

**************************************************************************/
void *BCHP_GetAvsHandle(
    BCHP_Handle hChip   /* [in] Chip handle */
	);

#endif /* ! __ASSEMBLY__ */

/**
Summary:
BCHP_VER_XX values are used with the BCHP_VER and BCHP_<<secondary_chip>>_VER macros
to provide version-specific software support.

Description:
Every magnum software build must have BCHP_VER defined to one of the following
values. This determines the version of chip the software is intended to run on.

Magnum has limited support for binary compatibility for other minor versions
of the same chip. In most cases this is made possible by backward compatible
hardware interfaces. In a small number of cases, either compile time or
run time chip version tests are used to provide binary compatibility. See magnum
architecture documentation for more details.
**/
#define BCHP_VER_A0 (0x00000000)
#define BCHP_VER_A1 (0x00000001)
#define BCHP_VER_A2 (0x00000002)
#define BCHP_VER_A3 (0x00000003)
#define BCHP_VER_A4 (0x00000004)
#define BCHP_VER_A5 (0x00000005)

#define BCHP_VER_B0 (0x00010000)
#define BCHP_VER_B1 (0x00010001)
#define BCHP_VER_B2 (0x00010002)
#define BCHP_VER_B3 (0x00010003)
#define BCHP_VER_B4 (0x00010004)
#define BCHP_VER_B5 (0x00010005)

#define BCHP_VER_C0 (0x00020000)
#define BCHP_VER_C1 (0x00020001)
#define BCHP_VER_C2 (0x00020002)
#define BCHP_VER_C3 (0x00020003)
#define BCHP_VER_C4 (0x00020004)
#define BCHP_VER_C5 (0x00020005)

#define BCHP_VER_D0 (0x00030000)
#define BCHP_VER_D1 (0x00030001)
#define BCHP_VER_D2 (0x00030002)
#define BCHP_VER_D3 (0x00030003)
#define BCHP_VER_D4 (0x00030004)
#define BCHP_VER_D5 (0x00030005)

#define BCHP_VER_E0 (0x00040000)
#define BCHP_VER_E1 (0x00040001)
#define BCHP_VER_E2 (0x00040002)
#define BCHP_VER_E3 (0x00040003)
#define BCHP_VER_E4 (0x00040004)
#define BCHP_VER_E5 (0x00040005)

/* Use one particular chip to purge BCHP_REV from Magnum code and customer builds. */
#if BCHP_CHIP == 7401
#if defined BCHP_REV_C0 || defined BCHP_REV_C1
#error BCHP_REV_XX is deprecreated. Use BCHP_VER==BCHP_VER_XX instead. Remove from code and build system.
#endif
#else

/* Backward compatibility for BCHP_REV_XX.
This code will be removed in the future. You should define BCHP_VER. */
#ifdef BCHP_REV_A0
#ifndef BCHP_VER
#define BCHP_VER BCHP_VER_A0
#elif BCHP_VER != BCHP_VER_A0
#error Mismatch of BCHP_VER and deprecated BCHP_REV_XX
#endif
#endif
#ifdef BCHP_REV_A1
#ifndef BCHP_VER
#define BCHP_VER BCHP_VER_A1
#elif BCHP_VER != BCHP_VER_A1
#error Mismatch of BCHP_VER and deprecated BCHP_REV_XX
#endif
#endif
#ifdef BCHP_REV_A2
#ifndef BCHP_VER
#define BCHP_VER BCHP_VER_A2
#elif BCHP_VER != BCHP_VER_A2
#error Mismatch of BCHP_VER and deprecated BCHP_REV_XX
#endif
#endif
#ifdef BCHP_REV_A3
#ifndef BCHP_VER
#define BCHP_VER BCHP_VER_A3
#elif BCHP_VER != BCHP_VER_A3
#error Mismatch of BCHP_VER and deprecated BCHP_REV_XX
#endif
#endif
#ifdef BCHP_REV_A4
#ifndef BCHP_VER
#define BCHP_VER BCHP_VER_A4
#elif BCHP_VER != BCHP_VER_A4
#error Mismatch of BCHP_VER and deprecated BCHP_REV_XX
#endif
#endif
#ifdef BCHP_REV_A5
#ifndef BCHP_VER
#define BCHP_VER BCHP_VER_A5
#elif BCHP_VER != BCHP_VER_A5
#error Mismatch of BCHP_VER and deprecated BCHP_REV_XX
#endif
#endif

#ifdef BCHP_REV_B0
#ifndef BCHP_VER
#define BCHP_VER BCHP_VER_B0
#elif BCHP_VER != BCHP_VER_B0
#error Mismatch of BCHP_VER and deprecated BCHP_REV_XX
#endif
#endif
#ifdef BCHP_REV_B1
#ifndef BCHP_VER
#define BCHP_VER BCHP_VER_B1
#elif BCHP_VER != BCHP_VER_B1
#error Mismatch of BCHP_VER and deprecated BCHP_REV_XX
#endif
#endif
#ifdef BCHP_REV_B2
#ifndef BCHP_VER
#define BCHP_VER BCHP_VER_B2
#elif BCHP_VER != BCHP_VER_B2
#error Mismatch of BCHP_VER and deprecated BCHP_REV_XX
#endif
#endif
#ifdef BCHP_REV_B3
#ifndef BCHP_VER
#define BCHP_VER BCHP_VER_B3
#elif BCHP_VER != BCHP_VER_B3
#error Mismatch of BCHP_VER and deprecated BCHP_REV_XX
#endif
#endif
#ifdef BCHP_REV_B4
#ifndef BCHP_VER
#define BCHP_VER BCHP_VER_B4
#elif BCHP_VER != BCHP_VER_B4
#error Mismatch of BCHP_VER and deprecated BCHP_REV_XX
#endif
#endif
#ifdef BCHP_REV_B5
#ifndef BCHP_VER
#define BCHP_VER BCHP_VER_B5
#elif BCHP_VER != BCHP_VER_B5
#error Mismatch of BCHP_VER and deprecated BCHP_REV_XX
#endif
#endif

#ifdef BCHP_REV_C0
#ifndef BCHP_VER
#define BCHP_VER BCHP_VER_C0
#elif BCHP_VER != BCHP_VER_C0
#error Mismatch of BCHP_VER and deprecated BCHP_REV_XX
#endif
#endif
#ifdef BCHP_REV_C1
#ifndef BCHP_VER
#define BCHP_VER BCHP_VER_C1
#elif BCHP_VER != BCHP_VER_C1
#error Mismatch of BCHP_VER and deprecated BCHP_REV_XX
#endif
#endif
#ifdef BCHP_REV_C2
#ifndef BCHP_VER
#define BCHP_VER BCHP_VER_C2
#elif BCHP_VER != BCHP_VER_C2
#error Mismatch of BCHP_VER and deprecated BCHP_REV_XX
#endif
#endif
#ifdef BCHP_REV_C3
#ifndef BCHP_VER
#define BCHP_VER BCHP_VER_C3
#elif BCHP_VER != BCHP_VER_C3
#error Mismatch of BCHP_VER and deprecated BCHP_REV_XX
#endif
#endif
#ifdef BCHP_REV_C4
#ifndef BCHP_VER
#define BCHP_VER BCHP_VER_C4
#elif BCHP_VER != BCHP_VER_C4
#error Mismatch of BCHP_VER and deprecated BCHP_REV_XX
#endif
#endif
#ifdef BCHP_REV_C5
#ifndef BCHP_VER
#define BCHP_VER BCHP_VER_C5
#elif BCHP_VER != BCHP_VER_C5
#error Mismatch of BCHP_VER and deprecated BCHP_REV_XX
#endif
#endif

#ifdef BCHP_REV_D0
#ifndef BCHP_VER
#define BCHP_VER BCHP_VER_D0
#elif BCHP_VER != BCHP_VER_D0
#error Mismatch of BCHP_VER and deprecated BCHP_REV_XX
#endif
#endif
#ifdef BCHP_REV_D1
#ifndef BCHP_VER
#define BCHP_VER BCHP_VER_D1
#elif BCHP_VER != BCHP_VER_D1
#error Mismatch of BCHP_VER and deprecated BCHP_REV_XX
#endif
#endif
#ifdef BCHP_REV_D2
#ifndef BCHP_VER
#define BCHP_VER BCHP_VER_D2
#elif BCHP_VER != BCHP_VER_D2
#error Mismatch of BCHP_VER and deprecated BCHP_REV_XX
#endif
#endif
#ifdef BCHP_REV_D3
#ifndef BCHP_VER
#define BCHP_VER BCHP_VER_D3
#elif BCHP_VER != BCHP_VER_D3
#error Mismatch of BCHP_VER and deprecated BCHP_REV_XX
#endif
#endif
#ifdef BCHP_REV_D4
#ifndef BCHP_VER
#define BCHP_VER BCHP_VER_D4
#elif BCHP_VER != BCHP_VER_D4
#error Mismatch of BCHP_VER and deprecated BCHP_REV_XX
#endif
#endif
#ifdef BCHP_REV_D5
#ifndef BCHP_VER
#define BCHP_VER BCHP_VER_D5
#elif BCHP_VER != BCHP_VER_D5
#error Mismatch of BCHP_VER and deprecated BCHP_REV_XX
#endif
#endif

#ifdef BCHP_REV_E0
#ifndef BCHP_VER
#define BCHP_VER BCHP_VER_E0
#elif BCHP_VER != BCHP_VER_E0
#error Mismatch of BCHP_VER and deprecated BCHP_REV_XX
#endif
#endif
#ifdef BCHP_REV_E1
#ifndef BCHP_VER
#define BCHP_VER BCHP_VER_E1
#elif BCHP_VER != BCHP_VER_E1
#error Mismatch of BCHP_VER and deprecated BCHP_REV_XX
#endif
#endif
#ifdef BCHP_REV_E2
#ifndef BCHP_VER
#define BCHP_VER BCHP_VER_E2
#elif BCHP_VER != BCHP_VER_E2
#error Mismatch of BCHP_VER and deprecated BCHP_REV_XX
#endif
#endif
#ifdef BCHP_REV_E3
#ifndef BCHP_VER
#define BCHP_VER BCHP_VER_E3
#elif BCHP_VER != BCHP_VER_E3
#error Mismatch of BCHP_VER and deprecated BCHP_REV_XX
#endif
#endif
#ifdef BCHP_REV_E4
#ifndef BCHP_VER
#define BCHP_VER BCHP_VER_E4
#elif BCHP_VER != BCHP_VER_E4
#error Mismatch of BCHP_VER and deprecated BCHP_REV_XX
#endif
#endif
#ifdef BCHP_REV_E5
#ifndef BCHP_VER
#define BCHP_VER BCHP_VER_E5
#elif BCHP_VER != BCHP_VER_E5
#error Mismatch of BCHP_VER and deprecated BCHP_REV_XX
#endif
#endif


#if BCHP_VER == BCHP_VER_A0 && !defined BCHP_REV_A0
#define BCHP_REV_A0 1
#endif
#if BCHP_VER == BCHP_VER_A1 && !defined BCHP_REV_A1
#define BCHP_REV_A1 1
#endif
#if BCHP_VER == BCHP_VER_A2 && !defined BCHP_REV_A2
#define BCHP_REV_A2 1
#endif
#if BCHP_VER == BCHP_VER_A3 && !defined BCHP_REV_A3
#define BCHP_REV_A3 1
#endif
#if BCHP_VER == BCHP_VER_A4 && !defined BCHP_REV_A4
#define BCHP_REV_A4 1
#endif
#if BCHP_VER == BCHP_VER_A5 && !defined BCHP_REV_A5
#define BCHP_REV_A5 1
#endif

#if BCHP_VER == BCHP_VER_B0 && !defined BCHP_REV_B0
#define BCHP_REV_B0 1
#endif
#if BCHP_VER == BCHP_VER_B1 && !defined BCHP_REV_B1
#define BCHP_REV_B1 1
#endif
#if BCHP_VER == BCHP_VER_B2 && !defined BCHP_REV_B2
#define BCHP_REV_B2 1
#endif
#if BCHP_VER == BCHP_VER_B3 && !defined BCHP_REV_B3
#define BCHP_REV_B3 1
#endif
#if BCHP_VER == BCHP_VER_B4 && !defined BCHP_REV_B4
#define BCHP_REV_B4 1
#endif
#if BCHP_VER == BCHP_VER_B5 && !defined BCHP_REV_B5
#define BCHP_REV_B5 1
#endif

#if BCHP_VER == BCHP_VER_C0 && !defined BCHP_REV_C0
#define BCHP_REV_C0 1
#endif
#if BCHP_VER == BCHP_VER_C1 && !defined BCHP_REV_C1
#define BCHP_REV_C1 1
#endif
#if BCHP_VER == BCHP_VER_C2 && !defined BCHP_REV_C2
#define BCHP_REV_C2 1
#endif
#if BCHP_VER == BCHP_VER_C3 && !defined BCHP_REV_C3
#define BCHP_REV_C3 1
#endif
#if BCHP_VER == BCHP_VER_C4 && !defined BCHP_REV_C4
#define BCHP_REV_C4 1
#endif
#if BCHP_VER == BCHP_VER_C5 && !defined BCHP_REV_C5
#define BCHP_REV_C5 1
#endif

#if BCHP_VER == BCHP_VER_D0 && !defined BCHP_REV_D0
#define BCHP_REV_D0 1
#endif
#if BCHP_VER == BCHP_VER_D1 && !defined BCHP_REV_D1
#define BCHP_REV_D1 1
#endif
#if BCHP_VER == BCHP_VER_D2 && !defined BCHP_REV_D2
#define BCHP_REV_D2 1
#endif
#if BCHP_VER == BCHP_VER_D3 && !defined BCHP_REV_D3
#define BCHP_REV_D3 1
#endif
#if BCHP_VER == BCHP_VER_D4 && !defined BCHP_REV_D4
#define BCHP_REV_D4 1
#endif
#if BCHP_VER == BCHP_VER_D5 && !defined BCHP_REV_D5
#define BCHP_REV_D5 1
#endif

#if BCHP_VER == BCHP_VER_E0 && !defined BCHP_REV_E0
#define BCHP_REV_E0 1
#endif
#if BCHP_VER == BCHP_VER_E1 && !defined BCHP_REV_E1
#define BCHP_REV_E1 1
#endif
#if BCHP_VER == BCHP_VER_E2 && !defined BCHP_REV_E2
#define BCHP_REV_E2 1
#endif
#if BCHP_VER == BCHP_VER_E3 && !defined BCHP_REV_E3
#define BCHP_REV_E3 1
#endif
#if BCHP_VER == BCHP_VER_E4 && !defined BCHP_REV_E4
#define BCHP_REV_E4 1
#endif
#if BCHP_VER == BCHP_VER_E5 && !defined BCHP_REV_E5
#define BCHP_REV_E5 1
#endif

#endif /* 7401 purge of BCHP_REV */

/* Check for required definitions */
#ifndef BCHP_CHIP
#error BCHP_CHIP must be defined to the main chip id.
#endif

#ifndef BCHP_VER
#error BCHP_VER must be defined to a standard value.
#endif

#ifdef __cplusplus
}
#endif

#endif

/* End of File */

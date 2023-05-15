/*
 * S&T Interactive TV Kit Text Processing Environment
 *
 * Copyright (c) 2007-2008 Strategy & Technology Limited. All Rights Reserved.
 *
 * Copyright in the whole and every part of this file belongs to 
 * Strategy & Technology Limited ("the Owner") and may not be used, 
 * sold, supplied, transferred, copied, reproduced in whole or in part, 
 * in any manner or form or in or on any media for the benefit of any person 
 * other than in accordance with the terms of the Owner's agreement or 
 * otherwise without the prior written consent of the Owner.
 */

#if !defined ITK_FONTDRIVER_H__
#define ITK_FONTDRIVER_H__
/**
 * @file
 * Font Driver
 */
#include "itk_display.h"
 
/**
 * A text character represented as an index into a font's set of glyphs.
 */
typedef itk_uint32_t itk_charindex_t;

/**
 * A text character represented as a Unicode UCS-4 (UTF-32) value.
 */
typedef itk_uint32_t itk_unicode_t;

/**
 * A font id. Once registered with the system, a font is assigned a unique id,
 * with which it will be referred to from then on.
 */
typedef void* itk_fontid_t;

/**
 * Binary data that consitutes a font file.
 */
typedef unsigned char* itk_binaryfont_t;

/**
 * Physical metrics for a single character.
 */
typedef struct itk_pmetrics_t
{
    /**
     * Amount to advance the cursor after drawing the character (in 256ths pixel)
     */
    itk_int32_t setWidth;

    /**
     * X drawing position offset in pixels (positive moves character to the right).
     */
    itk_int32_t xPos;

    /**
     * Y drawing position offset in pixels (positive moves character down).
     */
    itk_int32_t yPos;  

    /**
     * Width of the character, in pixels
     */
    itk_int32_t xSize;

    /**
     * Height of the character, in pixels
     */
    itk_int32_t ySize;
} itk_pmetrics_t;

/**
 * The font data structure is used to pass general font metrics and information
 * about the font as a whole. Note that for bounding box metrics, the origin
 * position is considered to be bottom left, with positive X values increasing
 * to the right, and positive y values increasing upwards.
 */
typedef struct itk_fontdata_t
{
  /**
   * The name of the font.
   */
  char *name;         

  /**
   * The metrics resolution of logical (i.e. unscaled) metrics of the font.
   * Effectively, this is the resolution of the 'em' font unit - there are
   * this many em in one font unit.
   */
  itk_uint16_t metricsRes; 

  /**
   * Character bounding box, left extent (in em).
   */
  itk_int16_t xMin;            

  /**
   * Character bounding box, right extent (in em).
   */
  itk_int16_t xMax;               

  /**
   * Character bounding box, bottom extent (in em).
   */
  itk_int16_t yMin;             

  /**
   * Character bounding box, top extent (in em).
   */
  itk_int16_t yMax;             

  /**
   * The total number of characters in the font.
   */
  itk_uint32_t charCount; 

  /**
   * The size of a character index in this font. Since a font
   * may have more than 255 characters, a character index may be
   * anything up to 4 bytes long.
   *  e.g. 1 for a font with <256 characters,
   *       2 for a font with 256-65535 characters,
   *       4 for a font with >65535 characters. 
   */
  itk_uint16_t charSize;   

  /**
   * The number of fixed bitmap sizes supported in this font. A value
   * of zero indicates the font is scalable to any size.
   */
  itk_uint16_t sizesCount; 

  /**
   * The array of fixed bitmap sizes supported, or NULL for scalable fonts.
   */
  itk_uint16_t *bitmapSizes;  

  /**
   * The unique font id assigned to this font.
   */
  itk_fontid_t id; 
} itk_fontdata_t;

/**
 * This structure specifies an aspect ratio conversion factor.
 * These are used to ensure that text is rendered in the appropriate
 * aspect ratio for the screen and text style. To convert the aspect
 * ratio using this factor, the physical width should be multiplied
 * by the numerator and then divided by the denominator. This order is
 * important to preserve accuracy.
 *
 * For example, to convert for a standard definition 14:9 display, 
 * the numerator will be set to 45 and the denominator to 54. The 
 * aspect ratio conversion will be done by multiplying the width by 
 * 45 and then dividing by 54.
 */
typedef struct itk_ar_factor_t
{
    /**
     * the amount to multiply width by, to convert aspect ratio
     */
    itk_int32_t numerator;

    /**
     * the amount to divide width by, to convert aspect ratio
     */
    itk_int32_t denominator;
} itk_ar_factor_t;


/**
 * This structure is used to return data about any static fonts the driver
 * supports. Static fonts are supplied by and built into to the driver, as 
 * opposed to fonts dynamically added at runtime via the addFont() and 
 * removeFont() calls.
 */
typedef struct itk_static_font_list_t
{
    /** 
     * The number of static fonts the driver supplies.
     */
    itk_uint16_t  font_count;

    /**
     * Pointer to an array of font URLs that correspond to the static fonts
     * that the driver supplies. The size of the array must be equal to
     * font_count. 
     */
    char **font_urls;

    /**
     * Array of font ids that correspond to the static fonts
     * that the driver supplies. The size of the array must be equal to
     * font_count.
     */
    itk_fontid_t *font_ids;

} itk_fontlist_t;

/**
 * This is used to give a driver a hint as to the expected format of a new
 * font.
 */
typedef enum itk_font_type_t
{
    /**
     * A standard PFR format font
     */
    ITK_FONT_TYPE_PFR = 0,

    /**
     * A ZIP or ZLIB compressed PFR format font
     */
    ITK_FONT_TYPE_PFR_COMPRESSED,

    /**
     * A standard TrueType or OpenType font
     */
    ITK_FONT_TYPE_TTF,

    /**
     * A ZIP or ZLIB compressed TrueType or OpenType font
     */
    ITK_FONT_TYPE_TTF_COMPRESSED,

    /**
     * Any other font format
     */
    ITK_FONT_TYPE_UNKNOWN
} itk_font_type_t;


struct itk_fontdriver_t;

/**
 * The font driver interface.
 */
typedef struct itk_fontdriver_t
{
    /**
     * Initialises the font driver.
     *
     * This function will be called to initialise the font driver when the 
     * system is starting up.
     *
     * @param[in]  thiz  Pointer to the font driver instance in use.
     * @param[in]  pipe  The iTK graphics pipeline that MUST be used to allocate 
     *                   the iTK masks returned by getGlyphBitmap
     * @param[in]  bpp   The pixel depth, or bits per pixel, to use when rendering.
     *                   This will be either 1 or 8.
	 * @return   ITKE_OK   if succeeded
	 * @return   an ITKE_ error code if initialisation failed.
     * @remark   Mandatory. This function is required.
	 */
    itk_errcode_t (*initialise)( struct itk_fontdriver_t *thiz,
        itk_graphics_pipeline_t *pipe, itk_uint8_t bpp );

    /**
     * Shuts down the font driver.
     *
     * @param[in]  thiz  Pointer to the font driver instance in use.
     * @remark   Optional.  If not implemented the jump table entry shall be NULL.
     */
    void (*shutdown)( struct itk_fontdriver_t *thiz );

    /**
     * Returns the list of static fonts the driver supplies. 
     *
     * Static fonts are those supplied by the font driver itself, as opposed to 
     * dynamic fonts added at runtime via the addFont() call.
     *
     * In the returned structure:
     *    font_count: should be set to the number of static fonts
     *    font_urls: should point to a *populated* array of font URL strings
     *    font_ids: should point to an *empty* array of font ids.
     *
     * Following this call, font ids will be assigned to each of the static
     * fonts and the font_ids array will be populated with these values. These font
     * ids will then be used subsequently to identify the fonts.
     *
     * If the driver does not supply any static fonts, the font_count field 
     * should be set to zero, and font_urls and font_ids should both be set
     * to NULL.
     *
     * @param[in]  thiz  Pointer to the font driver instance in use.
  	 * @return   the list of static fonts provided by the driver
     * @remark   Mandatory. This function is required.
     */
    struct itk_static_font_list_t (*getStaticFonts)( struct itk_fontdriver_t *thiz );

    /**
     * Convert Unicode character code to font character index
     *
     * @param[in]  thiz  Pointer to the font driver instance in use.
     * @param[in]  id   The ID of the font to look up the character in.
     * @param[in]  ch   The Unicode character code for the character to look up.
     * @return   The index of the character within the font, or zero if not found.
     * @remark   Mandatory. This function is required.
     */
    itk_charindex_t (*unicodeToIndex)( struct itk_fontdriver_t *thiz,
        itk_fontid_t id, itk_unicode_t ch );

    /**
     * Convert font character index to Unicode character code.
     * 
     * @param[in]  thiz  Pointer to the font driver instance in use.
     * @param[in]  id   The ID of the font to look up the character in.
     * @param[in]  ch   The index of the character in the selected font.
     * @return   The Unicode character code of the indexed character, or zero if
     *           the index is out of range.
     * @remark   Mandatory. This function is required.
     */
    itk_unicode_t (*indexToUnicode)( struct itk_fontdriver_t *thiz,
        itk_fontid_t id, itk_charindex_t ch );

    /**
     * Get the logical width of a character
     *
     * @param[in]  thiz  Pointer to the font driver instance in use.
     * @param[in]  id   The ID of the font to look up the character in.
     * @param[in]  ch   The index of the character in the selected font.
     * @return   The logical width of the indexed character, or zero if invalid
     *           index.
     * @remark   Mandatory. This function is required.
     */
    itk_uint32_t (*getLogicalWidth)( struct itk_fontdriver_t *thiz,
        itk_fontid_t id, itk_charindex_t ch );

    /**
     * Get the logical kerning between two characters, from ch1 to ch2.
     *
     * @param[in]  thiz  Pointer to the font driver instance in use.
     * @param[in]  id   The ID of the font to look up the character in.
     * @param[in]  ch1  The index of the first character in the kerning pair.
     * @param[in]  ch2  The index of the second character in the kerning pair.
     * @return   The logical kerning distance between the two characters in em;
     *           or zero in the case of an error.
     * @remark   Mandatory. This function is required.
     */
    itk_int32_t (*getLogicalKerning)( struct itk_fontdriver_t *thiz, 
        itk_fontid_t id, itk_charindex_t ch1, itk_charindex_t ch2 );

    /**
     * Get the physical metrics of a character at given point size. The metrics
     * must reflect the drawing mode that was specified when the driver was
     * initialised.
     *
     * @param[in]  thiz  Pointer to the font driver instance in use.
     * @param[in]  id   The ID of the font to look up the character in.
     * @param[in]  ch   The index of the desired character.
     * @param[in]  pointSize  The point size at which metrics are required.
     * @param[in]  ar   The aspect ratio conversion factor to apply to the width
     * @param[out] metrics    Pointer to a itk_fd_phys_metrics_t structure to be
     *                        filled in with data.   
     * @return   ITKE_OK   if succeeded
     * @return   an ITKE_ error code if failed.
     * @remark   Mandatory. This function is required.
     */
    itk_errcode_t (*getPhysicalMetrics)( struct itk_fontdriver_t *thiz,
        itk_fontid_t id, itk_charindex_t ch, itk_uint16_t pointSize, 
        itk_ar_factor_t ar, itk_pmetrics_t *metrics );

    /**
     * Get general font information
     *
     * @param[in]  thiz  Pointer to the font driver instance in use.
     * @param[in]   id   The ID of the font to look up.
     * @param[out]  fontData  Pointer to a itk_fd_font_data_t to be filled in
     *                        with data for the given font.
     * @return   ITKE_OK   if succeeded
     * @return   an ITKE_ error code if failed.
     * @remark   Mandatory. This function is required.
     */
    itk_errcode_t (*getFontData)( struct itk_fontdriver_t *thiz, 
        itk_fontid_t id, itk_fontdata_t *fontData );

    /**
     * Render the given character to produce a character mask. 
     *
     * The returned itk_mask_t MUST have been allocated from the graphics 
     * pipeline supplied when the driver was initialised, using the
     * the createMask() pipeline call. Note that the mask returned by the
     * pipeline may not match the format requested, in this case the font
     * driver must use the format returned. The mask should persist at
     * least until the next getGlyphBitmap() call, or until the shutdown()
     * call is made. The font driver is responsible for ensuring the mask
     * is released when it is no longer required. This could be at the
     * next getGlyphBitmap() call, or later if masks are being cached for
     * performance reasons, for example. At the shutdown() call, any 
     * remaining allocated masks must be released.
     *
     * Some characters have no rendered form - such as a space character,
     * where nothing is drawn (only the cursor position is advanced). 
     * Depending on the font rendering software in use, this could result
     * in either a small, empty bitmap, or a zero-sized bitmap. If a zero-
     * sized bitmap is generated, the returned itk_mask_t should have its 
     * 'width' and 'height' fields set to zero, and its 'mask' field set 
     * to NULL.
     *
     * If the driver was initialised with 1 bit-per-pixel, the font driver
     * should request a 1-bit depth mask from the pipeline. This will then
     * be filled in as a simple bitmap, with 0 representing empty pixels 
     * and 1 representing solid pixels. If the pipeline returns an 8-bit
     * depth mask, 0 should represent empty pixels, and 255 should represent
     * solid pixels.
     *
     * If the driver was initialised with 8 bits per pixel, the font driver
     * should request an 8-bit depth mask from the pipeline. This is then
     * filled in as an alpha mask, each pixel represented by a value from 0
     * (fully transparent) to 255 (fully solid). If the pipeline returns only
     * a 1-bit depth mask, the character should be rendered with 1 bit depth,
     * with 0 representing empty pixels, and 1 representing solid pixels.
     *
     * Pixels are stored in order, left to right along each row, starting with
     * the top row and working down. In 1 bit-per-pixel masks, the start of
     * each row should be byte aligned, padding with zeros if needed.
     * 
     * @param[in]  thiz  Pointer to the font driver instance in use.
     * @param[in]  id   The ID of the font to look up the character in.
     * @param[in]  ch   The index of the desired character.
     * @param[in]  pointSize  The point size at which the character should be rendered.
     * @param[in]  ar   The aspect ratio conversion factor to apply to the character width
     * @return   pointer to an itk_mask_t containing the glyph, or NULL
     * @remark   Mandatory. This function is required.
     */
    itk_mask_t *(*getGlyphBitmap)( struct itk_fontdriver_t *thiz, itk_fontid_t id, 
        itk_charindex_t ch, itk_uint16_t pointSize, itk_ar_factor_t ar );

    /**
     * Add a new font to the list supported by this driver. 
     *
     * This is called when the system is looking for a driver to handle a new 
     * font, e.g. a downloaded font.
     *
     * If the driver can understand and render the font, it should store the 
     * details as required and return ITKE_OK. Whenever text needs to be rendered
     * with this font, the driver will then be called with the specified font id.
     *
     * If the driver does not understand the font or does not support the font
     * type it should return ITKE_FAIL.
     *
     * @param[in]  thiz  Pointer to the font driver instance in use.
     * @param[in]  newFont  The binary font data.
     * @param[in]  name     The name/URL of this new font.
     * @param[in]  dataSize Size of the binary font data in bytes.
     * @param[in]  id       The ID assigned to the new font.
     * @param[in]  type     The type of font data (PFR, TTF, etc)
     * @return ITKE_OK if successful
     * @return ITKE_FAIL if the font was not accepted (e.g. not supported, corrupted, etc.)
     * @remark   Optional.  If not implemented the jump table entry shall be NULL.
     */
    itk_errcode_t (*addFont)( struct itk_fontdriver_t *thiz, itk_binaryfont_t newFont, 
        char *name, itk_int32_t dataSize, itk_fontid_t id, itk_font_type_t type );

    /**
     * Delete the given font from the driver.
     *
     * If the font id specified matches a dynamic font that was added to the driver
     * via the addFont call, the driver should remove the font and stop handling
     * that font id. (The font id may subsequently be reused and allocated to a 
     * different font and/or driver.)
     *
     * @param[in]  thiz  Pointer to the font driver instance in use.
     * @param[in]  id   The ID of the font to remove.
 	 * @remark   Mandatory if addFont() is implemented. If not implemented the jump table entry shall be NULL.
     */
    void (*removeFont)( struct itk_fontdriver_t *thiz, itk_fontid_t id );

} itk_fontdriver_t;

/**
 * @}
 */

#endif

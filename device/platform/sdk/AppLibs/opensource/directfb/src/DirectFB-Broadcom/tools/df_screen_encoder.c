/******************************************************************************
 *    (c)2012 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <directfb.h>

#define DFBCHECK(x)                                                       \
    {                                                                     \
        err = x;                                                          \
        if (err != DFB_OK) {                                              \
            fprintf( stderr, "%s <%d>:\n\t", __FILE__, __LINE__ );        \
        }                                                                 \
    }

static IDirectFBScreen           *primary_screen;
static IDirectFBScreen           *secondary_screen;
static IDirectFB                 *dfb;
static IDirectFBFont             *font;

void set_resolution ( int selected_screen, DFBScreenOutputResolution resolution, const char* resolution_string)
{

    DFBResult err;
    IDirectFBScreen *screen;
    DFBScreenOutputConfig         screen_output_config;

    if (selected_screen == 0)
        screen = primary_screen;
    else
        screen = secondary_screen;

    DFBCHECK(screen->GetOutputConfiguration (screen, 0, &screen_output_config));

    screen_output_config.flags |= DSOCONF_RESOLUTION;

    printf("Setting %s for %s screen\n",
    resolution_string,
    (selected_screen)? "Secondary":"Primary");

    screen_output_config.resolution = resolution;

    DFBCHECK(screen->SetOutputConfiguration(screen, 0, &screen_output_config));

}

void toggle_connector ( int selected_screen, DFBScreenOutputConnectors connector, const char* connector_string)
{
    DFBResult err;
    IDirectFBScreen *screen;
    DFBScreenEncoderConfig  screen_encoderCfg;

    if (selected_screen == 0)
        screen = primary_screen;
    else
        screen = secondary_screen;

    DFBCHECK(screen->GetEncoderConfiguration(screen, 0, &screen_encoderCfg));

    screen_encoderCfg.flags |= DSECONF_CONNECTORS;

    printf("%s %s connector for %s screen\n",
    (screen_encoderCfg.out_connectors & connector)? "Disabling":"Enabling",
    connector_string,
    (selected_screen)? "Secondary":"Primary");

    screen_encoderCfg.out_connectors ^= connector;

    DFBCHECK(screen->SetEncoderConfiguration(screen, 0, &screen_encoderCfg));
}

void draw_help (IDirectFBSurface *surface, const char *title)
{
    int fontheight, line = 1, width, height;

    font->GetHeight( font, &fontheight );
    surface->SetFont( surface, font );

    fontheight += 2;
    surface->SetColor( surface, 0xEC, 0x1F, 0x34, 0xff );
    surface->GetSize( surface, &width, &height);
    surface->FillRectangle( surface, 0, 0, width, height);
    surface->SetColor( surface, 0, 0, 0, 0xff );
    surface->DrawString( surface, title, -1, 60, (fontheight * line++), DSTF_LEFT | DSTF_TOP );
    surface->DrawString( surface, "q Quit", -1, 80, (fontheight * line++), DSTF_LEFT | DSTF_TOP );
    surface->DrawString( surface, "0 Select Primary", -1, 80, (fontheight * line++), DSTF_LEFT | DSTF_TOP );
    surface->DrawString( surface, "1 Select Secondary", -1, 80,(fontheight * line++), DSTF_LEFT | DSTF_TOP );
    surface->DrawString( surface, "2 Toggle YPbPr", -1, 80,(fontheight * line++), DSTF_LEFT | DSTF_TOP );
    surface->DrawString( surface, "3 Toggle CVBS", -1, 80, (fontheight * line++), DSTF_LEFT | DSTF_TOP );
    surface->DrawString( surface, "4 Toggle HDMI", -1, 80, (fontheight * line++), DSTF_LEFT | DSTF_TOP );
    surface->DrawString( surface, "5 Select PAL", -1, 80, (fontheight * line++), DSTF_LEFT | DSTF_TOP );
    surface->DrawString( surface, "6 Select NTSC", -1, 80, (fontheight * line++), DSTF_LEFT | DSTF_TOP );
    surface->Flip( surface, NULL, DSFLIP_WAITFORSYNC);
}

int main( int argc, char *argv[] )
{
    DFBResult ret;
    DFBResult err;
    IDirectFBDisplayLayer     *secondary_layer;
    IDirectFBEventBuffer      *keybuffer;
    IDirectFBSurface          *primary_surface;
    IDirectFBSurface          *secondary_surface;
    DFBFontDescription         font_desc;
    DFBSurfaceDescription      surface_dsc;

    DFBInputEvent evt;
    int quit, screen_control = 0;

    /* Initialize DirectFB including command line parsing. */
    ret = DirectFBInit( &argc, &argv );
    if (ret)
    {
        DirectFBError( "DirectFBInit() failed", ret );
        return -1;
    }

    /* Create the super interface. */
    ret = DirectFBCreate( &dfb );
    if (ret)
    {
        DirectFBError( "DirectFBCreate() failed", ret );
        return -3;
    }

    err = dfb->SetCooperativeLevel( dfb, DFSCL_EXCLUSIVE );
    if (err)
    {
        DirectFBError( "Failed to get exclusive access", err );
    }

    DFBCHECK(dfb->GetScreen( dfb, DSCID_PRIMARY, &primary_screen ));
    if (err)
    {
        DirectFBError( "Failed to get primary screen", err );
    }

    DFBCHECK(dfb->GetScreen( dfb, DSCID_PRIMARY + 1, &secondary_screen ));
    if (err)
    {
        DirectFBError( "Failed to get secondary screen", err );
    }

    DFBCHECK(dfb->GetDisplayLayer(dfb, DLID_PRIMARY+1, &secondary_layer));
    if (err)
    {
        DirectFBError( "Failed to get secondary layer", err );
    }

    /* create an input buffer for key events */
    DFBCHECK(dfb->CreateInputEventBuffer( dfb, DICAPS_KEYS,
                                          DFB_FALSE, &keybuffer ));

    surface_dsc.flags = DSDESC_CAPS;
    surface_dsc.caps = DSCAPS_PRIMARY;

    DFBCHECK(dfb->CreateSurface( dfb, &surface_dsc, &primary_surface ));
    if (err)
    {
        DirectFBError( "Failed to get secondary surface", err );
    }

    DFBCHECK(secondary_layer->GetSurface(secondary_layer, &secondary_surface));
    if (err)
    {
        DirectFBError( "Failed to get secondary surface", err );
    }

    font_desc.flags = DFDESC_HEIGHT;
    font_desc.height = 32;
    DFBCHECK(dfb->CreateFont( dfb, FONT, &font_desc, &font ));

    draw_help(primary_surface, "Primary screen");
    draw_help(secondary_surface, "Secondary screen");

    quit = 0;
    while (!quit)
    {
        while (keybuffer->GetEvent( keybuffer, DFB_EVENT(&evt)) == DFB_OK)
        {
            if (evt.type == DIET_KEYPRESS) {
                switch (DFB_LOWER_CASE(evt.key_symbol))
                {
                    case DIKS_ESCAPE:
                    case DIKS_SMALL_Q:
                    case DIKS_BACK:
                    case DIKS_STOP:
                    case DIKS_EXIT:
                    case DIKS_POWER:
                        /* quit main loop */
                        quit = 1;
                        break;

                    case DIKS_0:
                        screen_control = 0;
                        fprintf(stderr, "Controlling Primary screen\n");
                        break;

                    case DIKS_1:
                        screen_control = 1;
                        fprintf(stderr, "Controlling Secondary screen\n");
                        break;

                    case DIKS_2:
                        toggle_connector(screen_control, DSOC_COMPONENT, "Component");
                        break;

                    case DIKS_3:
                        toggle_connector(screen_control, DSOC_CVBS, "CVBS");
                        break;

                    case DIKS_4:
                        toggle_connector(screen_control, DSOC_HDMI, "HDMI");
                        break;

                    case DIKS_5:
                        set_resolution(screen_control, DSOR_720_576, "PAL");
                        DFBCHECK(secondary_surface->Flip(secondary_surface, NULL, 0));
                        break;

                    case DIKS_6:
                        set_resolution(screen_control, DSOR_720_480, "NTSC");
                        break;

                    default:
                        break;
                }
            }
        }
    }

    keybuffer->Release( keybuffer );
    font->Release( font );
    primary_surface->Release ( primary_surface );
    secondary_surface->Release ( secondary_surface );
    secondary_layer->Release( secondary_layer );
    primary_screen->Release( primary_screen );
    secondary_screen->Release( secondary_screen );

    dfb->Release( dfb );

    return 0;
}

/*
   (c) Copyright 2001-2010  The world wide DirectFB Open Source Community (directfb.org)
   (c) Copyright 2000-2004  Convergence (integrated media) GmbH

   All rights reserved.

   Written by Denis Oliver Kropp <dok@directfb.org>,
              Andreas Hundt <andi@fischlustig.de>,
              Sven Neumann <neo@directfb.org>,
              Ville Syrjälä <syrjala@sci.fi> and
              Claudio Ciccani <klan@users.sf.net>.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the
   Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

/*
 * Translates a Linux input keycode into a DirectFB keycode.
 */

#include "keyboard_pointer.h"

int
translate_key( unsigned short code )
{
     if (code < D_ARRAY_SIZE( basic_keycodes ))
          return basic_keycodes[code];

     if (code >= KEY_OK)
          if (code - KEY_OK < D_ARRAY_SIZE( ext_keycodes ))
               return ext_keycodes[code-KEY_OK];

     return DIKI_UNKNOWN;
}

/*
 * Translates key and button events.
 */
static bool
key_event( const struct input_event *levt,
           DFBInputEvent            *devt )
{
     int code = levt->code;

     /* map touchscreen and smartpad events to button mouse */
     if (code == BTN_TOUCH || code == BTN_TOOL_FINGER)
          code = BTN_MOUSE;

     if ((code >= BTN_MOUSE && code < BTN_JOYSTICK) || code == BTN_TOUCH) {
          /* ignore repeat events for buttons */
          if (levt->value == 2)
               return false;

          devt->type   = levt->value ? DIET_BUTTONPRESS : DIET_BUTTONRELEASE;
          /* don't set DIEF_BUTTONS, it will be set by the input core */
          devt->button = DIBI_FIRST + code - BTN_MOUSE;
     }
     else {
          int key = translate_key( code );

          if (key == DIKI_UNKNOWN)
               return false;

          devt->type = levt->value ? DIET_KEYPRESS : DIET_KEYRELEASE;

          if (DFB_KEY_TYPE(key) == DIKT_IDENTIFIER) {
               devt->key_id = key;
               devt->flags |= DIEF_KEYID;
          }
          else {
               devt->key_symbol = key;
               devt->flags |= DIEF_KEYSYMBOL;
          }

          devt->flags |= DIEF_KEYCODE;
          devt->key_code = code;
     }

     if (levt->value == 2)
          devt->flags |= DIEF_REPEAT;

     return true;
}

/*
 * Translates relative axis events.
 */
static bool
rel_event( const struct input_event *levt,
           DFBInputEvent            *devt )
{
     switch (levt->code) {
          case REL_X:
               devt->axis = DIAI_X;
               devt->axisrel = levt->value;
               break;

          case REL_Y:
               devt->axis = DIAI_Y;
               devt->axisrel = levt->value;
               break;

          case REL_Z:
          case REL_WHEEL:
               devt->axis = DIAI_Z;
               devt->axisrel = -levt->value;
               break;

          default:
               if (levt->code > REL_MAX || levt->code > DIAI_LAST)
                    return false;
               devt->axis = levt->code;
               devt->axisrel = levt->value;
     }

     devt->type    = DIET_AXISMOTION;
     devt->flags  |= DIEF_AXISREL;

     return true;
}

/*
 * Translates absolute axis events.
 */
static bool
abs_event( const struct input_event *levt,
           DFBInputEvent            *devt )
{
     switch (levt->code) {
          case ABS_X:
               devt->axis = DIAI_X;
               break;

          case ABS_Y:
               devt->axis = DIAI_Y;
               break;

          case ABS_Z:
          case ABS_WHEEL:
               devt->axis = DIAI_Z;
               break;

          default:
               if (levt->code >= ABS_PRESSURE || levt->code > DIAI_LAST)
                    return false;
               devt->axis = levt->code;
     }

     devt->type    = DIET_AXISMOTION;
     devt->flags  |= DIEF_AXISABS;
     devt->axisabs = levt->value;

     return true;
}

/*
 * Translates a Linux input event into a DirectFB input event.
 */
 bool
translate_event( const struct input_event *levt,
                 DFBInputEvent            *devt )
{
     devt->flags     = DIEF_TIMESTAMP;
     devt->timestamp = levt->time;

     switch (levt->type) {
          case EV_KEY:
               return key_event( levt, devt );

          case EV_REL:
               return rel_event( levt, devt );

          case EV_ABS:
               return abs_event( levt, devt );

          default:
               ;
     }

     return false;
}

void
flush_xy( BDFBInputDeviceData *data, bool last )
{
     DFBInputEvent evt = { .type = DIET_UNKNOWN };

     if (data->dx) {
          evt.type    = DIET_AXISMOTION;
          evt.flags   = DIEF_AXISREL;
          evt.axis    = DIAI_X;
          evt.axisrel = data->dx;

          /* Signal immediately following event. */
          if (!last || data->dy)
               evt.flags |= DIEF_FOLLOW;
          printf("Calling dfb_input_dispatch() from flus_xy()1 \n");

          dfb_input_dispatch( data->device, &evt );

          data->dx = 0;
     }

     if (data->dy) {
          evt.type    = DIET_AXISMOTION;
          evt.flags   = DIEF_AXISREL;
          evt.axis    = DIAI_Y;
          evt.axisrel = data->dy;

          /* Signal immediately following event. */
          if (!last)
               evt.flags |= DIEF_FOLLOW;
          printf("Calling dfb_input_dispatch() from flus_xy()2 \n");

          dfb_input_dispatch( data->device, &evt );

          data->dy = 0;
     }
}


#ifndef __bcmnexus_rf4ce_H__
#define __bcmnexus_rf4ce_H__


#include <pthread.h>
#include <directfb.h>

#include <direct/modules.h>

#include <fusion/reactor.h>

#include <core/coretypes.h>

#define DFB_BCMNEXUS_RF4CE_KEYCODES_ABI_VERSION 1


DECLARE_MODULE_DIRECTORY( dfb_bcmnexus_rf4ce_keycodes );

/*
** For mapping hardware codes to DirectFB key codes.
*/
typedef struct {

    DFBInputDeviceKeySymbol        dfbKeySymbol;
    uint32_t                       hwEventCode;
} KeyMapElement;

typedef struct {
     int  (*GetBasicKeycodesSize) (void);
     int  (*GetAdvanKeycodesSize) (void);     
     void (*GetBasicKeycodes)     (KeyMapElement *);
     void (*GetAdvanKeycodes)     (KeyMapElement *);
} Rf4ceKeycodesMapFuncs;

typedef struct {
     DirectModuleEntry            *module;
     Rf4ceKeycodesMapFuncs           *funcs;
}bcmnexus_RF4CE_keycodes;

#endif

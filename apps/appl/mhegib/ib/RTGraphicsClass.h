/*
 * RTGraphicsClass.h
 */

#ifndef __RTGRAPHICSCLASS_H__
#define __RTGRAPHICSCLASS_H__

#include "ISO13522-MHEG-5.h"

void RTGraphicsClass_Preparation(RTGraphicsClass *);
bool RTGraphicsClass_Activation(RTGraphicsClass *);
bool RTGraphicsClass_Deactivation(RTGraphicsClass *);
void RTGraphicsClass_Destruction(RTGraphicsClass *);

void RTGraphicsClass_render(RTGraphicsClass *, MHEGDisplay *, XYPosition *, OriginalBoxSize *);

#endif	/* __RTGRAPHICSCLASS_H__ */


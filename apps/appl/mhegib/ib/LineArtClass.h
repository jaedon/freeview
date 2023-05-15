/*
 * LineArtClass.h
 */

#ifndef __LINEARTCLASS_H__
#define __LINEARTCLASS_H__

#include "ISO13522-MHEG-5.h"

void default_LineArtClassInstanceVars(LineArtClass *, LineArtClassInstanceVars *);
void free_LineArtClassInstanceVars(LineArtClassInstanceVars *);

void LineArtClass_Preparation(LineArtClass *);
bool LineArtClass_Activation(LineArtClass *);
bool LineArtClass_Deactivation(LineArtClass *);
void LineArtClass_Destruction(LineArtClass *);

void LineArtClass_render(LineArtClass *, MHEGDisplay *, XYPosition *, OriginalBoxSize *);

#endif	/* __LINEARTCLASS_H__ */


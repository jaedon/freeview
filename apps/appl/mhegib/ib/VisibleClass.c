/*
 * VisibleClass.c
 */

#include "MHEGEngine.h"
#include "BitmapClass.h"
#include "LineArtClass.h"
#include "RectangleClass.h"
#include "DynamicLineArtClass.h"
#include "TextClass.h"
#include "VideoClass.h"
#include "RTGraphicsClass.h"
#include "SliderClass.h"
#include "EntryFieldClass.h"
#include "HyperTextClass.h"
#include "HotspotClass.h"
#include "PushButtonClass.h"
#include "SwitchButtonClass.h"

bool
VisibleClass_Activation(RootClass *v, XYPosition *pos)
{
	/* Visible 클래스들, 즉,
	   Bitmap, LineArt, Rectangle, DynamicLineArt, Text, Video, RTGraphics, Slider, EntryField, HyperText, Hotspot, PushButton, SwitchButton 클래스들을 대상으로 한다. */

	bool did = false;

	switch(v->inst.rtti)
	{
	case RTTI_BitmapClass:
		did = BitmapClass_Activation((BitmapClass *) v);
		if (did && pos) VK_memcpy(&((BitmapClass *) v)->inst.Position, pos, sizeof(XYPosition));
		break;

	case RTTI_LineArtClass:
		did = LineArtClass_Activation((LineArtClass *) v);
		if (did && pos) VK_memcpy(&((LineArtClass *) v)->inst.Position, pos, sizeof(XYPosition));
		break;

	case RTTI_RectangleClass:
		did = RectangleClass_Activation((RectangleClass *) v);
		if (did && pos) VK_memcpy(&((RectangleClass *) v)->inst.Position, pos, sizeof(XYPosition));
		break;

	case RTTI_DynamicLineArtClass:
		did = DynamicLineArtClass_Activation((DynamicLineArtClass *) v);
		if (did && pos) VK_memcpy(&((DynamicLineArtClass *) v)->inst.Position, pos, sizeof(XYPosition));
		break;

	case RTTI_TextClass:
		did = TextClass_Activation((TextClass *) v);
		if (did && pos) VK_memcpy(&((TextClass *) v)->inst.Position, pos, sizeof(XYPosition));
		break;

	case RTTI_VideoClass:
		did = VideoClass_Activation((VideoClass *) v);
		if (did && pos) VK_memcpy(&((VideoClass *) v)->inst.Position, pos, sizeof(XYPosition));
		break;

	case RTTI_RTGraphicsClass:
		did = RTGraphicsClass_Activation((RTGraphicsClass *) v);
//		if (did && pos) VK_memcpy(&((RTGraphicsClass *) v)->inst.Position, pos, sizeof(XYPosition));
		break;

	case RTTI_SliderClass:
		did = SliderClass_Activation((SliderClass *) v);
		if (did && pos) VK_memcpy(&((SliderClass *) v)->inst.Position, pos, sizeof(XYPosition));
		break;

	case RTTI_EntryFieldClass:
		did = EntryFieldClass_Activation((EntryFieldClass *) v);
		if (did && pos) VK_memcpy(&((EntryFieldClass *) v)->inst.Position, pos, sizeof(XYPosition));
		break;

	case RTTI_HyperTextClass:
		did = HyperTextClass_Activation((HyperTextClass *) v);
		if (did && pos) VK_memcpy(&((HyperTextClass *) v)->inst.Position, pos, sizeof(XYPosition));
		break;

	case RTTI_HotspotClass:
		did = HotspotClass_Activation((HotspotClass *) v);
//		if (did && pos) VK_memcpy(&((HotspotClass *) v)->inst.Position, pos, sizeof(XYPosition));
		break;

	case RTTI_PushButtonClass:
		did = PushButtonClass_Activation((PushButtonClass *) v);
//		if (did && pos) VK_memcpy(&((PushButtonClass *) v)->inst.Position, pos, sizeof(XYPosition));
		break;

	case RTTI_SwitchButtonClass:
		did = SwitchButtonClass_Activation((SwitchButtonClass *) v);
//		if (did && pos) VK_memcpy(&((SwitchButtonClass *) v)->inst.Position, pos, sizeof(XYPosition));
		break;

	default:
		error("Unknown VisibleClass type: %u", v->inst.rtti);
		break;
	}

	return did;
}

bool
VisibleClass_Deactivation(RootClass *v)
{
	/* Visible 클래스들, 즉,
	   Bitmap, LineArt, Rectangle, DynamicLineArt, Text, Video, RTGraphics, Slider, EntryField, HyperText, Hotspot, PushButton, SwitchButton 클래스들을 대상으로 한다. */

	bool did = false;

	switch(v->inst.rtti)
	{
	case RTTI_BitmapClass:
		did = BitmapClass_Deactivation((BitmapClass *) v);
		if (did) VK_memcpy(&((BitmapClass *) v)->inst.Position, &((BitmapClass *) v)->original_position, sizeof(XYPosition));
		break;

	case RTTI_LineArtClass:
		did = LineArtClass_Deactivation((LineArtClass *) v);
		if (did) VK_memcpy(&((LineArtClass *) v)->inst.Position, &((LineArtClass *) v)->original_position, sizeof(XYPosition));
		break;

	case RTTI_RectangleClass:
		did = RectangleClass_Deactivation((RectangleClass *) v);
		if (did) VK_memcpy(&((RectangleClass *) v)->inst.Position, &((RectangleClass *) v)->original_position, sizeof(XYPosition));
		break;

	case RTTI_DynamicLineArtClass:
		did = DynamicLineArtClass_Deactivation((DynamicLineArtClass *) v);
		if (did) VK_memcpy(&((DynamicLineArtClass *) v)->inst.Position, &((DynamicLineArtClass *) v)->original_position, sizeof(XYPosition));
		break;

	case RTTI_TextClass:
		did = TextClass_Deactivation((TextClass *) v);
		if (did) VK_memcpy(&((TextClass *) v)->inst.Position, &((TextClass *) v)->original_position, sizeof(XYPosition));
		break;

	case RTTI_VideoClass:
		did = VideoClass_Deactivation((VideoClass *) v);
		if (did) VK_memcpy(&((VideoClass *) v)->inst.Position, &((VideoClass *) v)->original_position, sizeof(XYPosition));
		break;

	case RTTI_RTGraphicsClass:
		did = RTGraphicsClass_Deactivation((RTGraphicsClass *) v);
//		if (did) VK_memcpy(&((RTGraphicsClass *) v)->inst.Position, &((RTGraphicsClass *) v)->original_position, sizeof(XYPosition));
		break;

	case RTTI_SliderClass:
		did = SliderClass_Deactivation((SliderClass *) v);
		if (did) VK_memcpy(&((SliderClass *) v)->inst.Position, &((SliderClass *) v)->original_position, sizeof(XYPosition));
		break;

	case RTTI_EntryFieldClass:
		did = EntryFieldClass_Deactivation((EntryFieldClass *) v);
		if (did) VK_memcpy(&((EntryFieldClass *) v)->inst.Position, &((EntryFieldClass *) v)->original_position, sizeof(XYPosition));
		break;

	case RTTI_HyperTextClass:
		did = HyperTextClass_Deactivation((HyperTextClass *) v);
		if (did) VK_memcpy(&((HyperTextClass *) v)->inst.Position, &((HyperTextClass *) v)->original_position, sizeof(XYPosition));
		break;

	case RTTI_HotspotClass:
		did = HotspotClass_Deactivation((HotspotClass *) v);
//		if (did) VK_memcpy(&((HotspotClass *) v)->inst.Position, &((HotspotClass *) v)->original_position, sizeof(XYPosition));
		break;

	case RTTI_PushButtonClass:
		did = PushButtonClass_Deactivation((PushButtonClass *) v);
//		if (did) VK_memcpy(&((PushButtonClass *) v)->inst.Position, &((PushButtonClass *) v)->original_position, sizeof(XYPosition));
		break;

	case RTTI_SwitchButtonClass:
		did = SwitchButtonClass_Deactivation((SwitchButtonClass *) v);
//		if (did) VK_memcpy(&((SwitchButtonClass *) v)->inst.Position, &((SwitchButtonClass *) v)->original_position, sizeof(XYPosition));
		break;

	default:
		error("Unknown VisibleClass type: %u", v->inst.rtti);
		break;
	}

	return did;
}

void
VisibleClass_render(RootClass *v, MHEGDisplay *d, XYPosition *pos, OriginalBoxSize *box)
{
	/* Visible 클래스들, 즉,
	   Bitmap, LineArt, Rectangle, DynamicLineArt, Text, Video, RTGraphics, Slider, EntryField, HyperText, Hotspot, PushButton, SwitchButton 클래스들을 대상으로 한다. */

	switch(v->inst.rtti)
	{
	case RTTI_BitmapClass:
		BitmapClass_render((BitmapClass *) v, d, pos, box);
		break;

	case RTTI_LineArtClass:
		LineArtClass_render((LineArtClass *) v, d, pos, box);
		break;

	case RTTI_RectangleClass:
		RectangleClass_render((RectangleClass *) v, d, pos, box);
		break;

	case RTTI_DynamicLineArtClass:
		DynamicLineArtClass_render((DynamicLineArtClass *) v, d, pos, box);
		break;

	case RTTI_TextClass:
		TextClass_render((TextClass *) v, d, pos, box);
		break;

	case RTTI_VideoClass:
		VideoClass_render((VideoClass *) v, d, pos, box);
		break;

	case RTTI_RTGraphicsClass:
		RTGraphicsClass_render((RTGraphicsClass *) v, d, pos, box);
		break;

	case RTTI_SliderClass:
		SliderClass_render((SliderClass *) v, d, pos, box);
		break;

	case RTTI_EntryFieldClass:
		EntryFieldClass_render((EntryFieldClass *) v, d, pos, box);
		break;

	case RTTI_HyperTextClass:
		HyperTextClass_render((HyperTextClass *) v, d, pos, box);
		break;

	case RTTI_HotspotClass:
		HotspotClass_render((HotspotClass *) v, d, pos, box);
		break;

	case RTTI_PushButtonClass:
		PushButtonClass_render((PushButtonClass *) v, d, pos, box);
		break;

	case RTTI_SwitchButtonClass:
		SwitchButtonClass_render((SwitchButtonClass *) v, d, pos, box);
		break;

	default:
		error("Unknown VisibleClass type: %u", v->inst.rtti);
		break;
	}

	return;
}


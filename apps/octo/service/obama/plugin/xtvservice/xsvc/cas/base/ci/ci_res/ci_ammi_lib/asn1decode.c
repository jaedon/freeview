
// hmkim : added.
#include "../ci_ammi.h"
#ifdef _CI_DEBUG_AMMI_DECODE_FILE_DATA_

#include <stdio.h>
#include <stdbool.h>

#include "asn1decode.h"
#include "der_decode.h"
#include "asn1tag.h"
#include "output.h"

#if 0 // hmkim : commented out.
void verbose(char *, ...);
#endif

int asn1decode_BOOLEAN(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	verbose("<BOOLEAN>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("BOOLEAN");
	left -= sublen;

	if(is_BOOLEAN(tag.class, tag.number))
	{
		der_decode_BOOLEAN(der, out, tag.length);
		left -= tag.length;
	}
	else
	{
		return der_error("BOOLEAN");
	}

	if(left != 0)
		return der_error("BOOLEAN: %d bytes left", left);

	verbose("</BOOLEAN>\n");

	return length;
}

bool is_BOOLEAN(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_BOOLEAN);
}

int asn1decode_INTEGER(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	verbose("<INTEGER>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("INTEGER");
	left -= sublen;

	if(is_INTEGER(tag.class, tag.number))
	{
		der_decode_INTEGER(der, out, tag.length);
		left -= tag.length;
	}
	else
	{
		return der_error("INTEGER");
	}

	if(left != 0)
		return der_error("INTEGER: %d bytes left", left);

	verbose("</INTEGER>\n");

	return length;
}

bool is_INTEGER(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_INTEGER);
}

int asn1decode_Null(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	verbose("<Null>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Null");
	left -= sublen;

	if(is_Null(tag.class, tag.number))
	{
		der_decode_Null(der, out, tag.length);
		left -= tag.length;
	}
	else
	{
		return der_error("Null");
	}

	if(left != 0)
		return der_error("Null: %d bytes left", left);

	verbose("</Null>\n");

	return length;
}

bool is_Null(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_NULL);
}

int asn1decode_InterchangedObject(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<InterchangedObject>\n");

	/* CHOICE */
	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("InterchangedObject");
	left -= sublen;

	if(is_ApplicationClass(tag.class, tag.number))
	{
		if((sublen = asn1decode_ApplicationClass(der, out, tag.length)) < 0)
			return der_error("InterchangedObject");
		left -= tag.length;
	}
	else if(is_SceneClass(tag.class, tag.number))
	{
		if((sublen = asn1decode_SceneClass(der, out, tag.length)) < 0)
			return der_error("InterchangedObject");
		left -= tag.length;
	}
	else
	{
		return der_error("InterchangedObject");
	}

	if(left != 0)
		return der_error("InterchangedObject: %d bytes left", left);

	verbose("</InterchangedObject>\n");

	return length;
}

int asn1decode_Root(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Root>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Root");

	if(is_ObjectIdentifier(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectIdentifier(der, out, length)) < 0)
			return der_error("Root");
		left -= sublen;
	}
	else
	{
		return der_error("Root: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("Root: %d bytes left", left);

	verbose("</Root>\n");

	return length;
}

int asn1decode_ObjectIdentifier(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ObjectIdentifier>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ObjectIdentifier");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("ObjectIdentifier");
		left -= sublen;
	}
	else
	{
		return der_error("ObjectIdentifier: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("ObjectIdentifier: %d bytes left", left);

	verbose("</ObjectIdentifier>\n");

	return length;
}

int asn1decode_Group(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Group>\n");

	/* SET */
	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("Group");
		left -= sublen;
		if(is_Root(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_Root(der, out, sublen + tag.length)) < 0)
				return der_error("Group");
			left -= tag.length;
		}
		else if(is_StandardIdentifier(tag.class, tag.number))
		{
			if((sublen = asn1decode_StandardIdentifier(der, out, tag.length)) < 0)
				return der_error("Group");
			left -= tag.length;
		}
		else if(is_StandardVersion(tag.class, tag.number))
		{
			if((sublen = asn1decode_StandardVersion(der, out, tag.length)) < 0)
				return der_error("Group");
			left -= tag.length;
		}
		else if(is_ObjectInformation(tag.class, tag.number))
		{
			if((sublen = asn1decode_ObjectInformation(der, out, tag.length)) < 0)
				return der_error("Group");
			left -= tag.length;
		}
		else if(is_OnStartUp(tag.class, tag.number))
		{
			if((sublen = asn1decode_OnStartUp(der, out, tag.length)) < 0)
				return der_error("Group");
			left -= tag.length;
		}
		else if(is_OnCloseDown(tag.class, tag.number))
		{
			if((sublen = asn1decode_OnCloseDown(der, out, tag.length)) < 0)
				return der_error("Group");
			left -= tag.length;
		}
		else if(is_OriginalGroupCachePriority(tag.class, tag.number))
		{
			if((sublen = asn1decode_OriginalGroupCachePriority(der, out, tag.length)) < 0)
				return der_error("Group");
			left -= tag.length;
		}
		else if(is_Items(tag.class, tag.number))
		{
			if((sublen = asn1decode_Items(der, out, tag.length)) < 0)
				return der_error("Group");
			left -= tag.length;
		}
		else
		{
			return der_error("Group: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	if(left != 0)
		return der_error("Group: %d bytes left", left);

	verbose("</Group>\n");

	return length;
}

int asn1decode_StandardIdentifier(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<StandardIdentifier>\n");

	/* SEQUENCE */
	output_token(out, ":StdID");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("StandardIdentifier");
	left -= sublen;

	if(is_JointIsoItuIdentifier(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_JointIsoItuIdentifier(der, out, sublen + tag.length)) < 0)
			return der_error("StandardIdentifier");
		left -= tag.length;
	}
	else
	{
		return der_error("StandardIdentifier: missing JointIsoItuIdentifier");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("StandardIdentifier");
	left -= sublen;

	if(is_MHEGStandardIdentifier(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_MHEGStandardIdentifier(der, out, sublen + tag.length)) < 0)
			return der_error("StandardIdentifier");
		left -= tag.length;
	}
	else
	{
		return der_error("StandardIdentifier: missing MHEGStandardIdentifier");
	}

	if(left != 0)
		return der_error("StandardIdentifier: %d bytes left", left);

	verbose("</StandardIdentifier>\n");

	return length;
}

int asn1decode_JointIsoItuIdentifier(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<JointIsoItuIdentifier>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("JointIsoItuIdentifier");

	if(is_INTEGER(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_INTEGER(der, out, length)) < 0)
			return der_error("JointIsoItuIdentifier");
		left -= sublen;
	}
	else
	{
		return der_error("JointIsoItuIdentifier: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("JointIsoItuIdentifier: %d bytes left", left);

	verbose("</JointIsoItuIdentifier>\n");

	return length;
}

int asn1decode_MHEGStandardIdentifier(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<MHEGStandardIdentifier>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("MHEGStandardIdentifier");

	if(is_INTEGER(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_INTEGER(der, out, length)) < 0)
			return der_error("MHEGStandardIdentifier");
		left -= sublen;
	}
	else
	{
		return der_error("MHEGStandardIdentifier: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("MHEGStandardIdentifier: %d bytes left", left);

	verbose("</MHEGStandardIdentifier>\n");

	return length;
}

int asn1decode_StandardVersion(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<StandardVersion>\n");

	output_token(out, ":StdVersion");

	if((sublen = der_decode_INTEGER(der, out, length)) < 0)
		return der_error("StandardVersion");
	left -= sublen;

	if(left != 0)
		return der_error("StandardVersion: %d bytes left", left);

	verbose("</StandardVersion>\n");

	return length;
}

int asn1decode_ObjectInformation(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ObjectInformation>\n");

	output_token(out, ":ObjectInfo");

	if((sublen = der_decode_OctetString(der, out, length)) < 0)
		return der_error("ObjectInformation");
	left -= sublen;

	if(left != 0)
		return der_error("ObjectInformation: %d bytes left", left);

	verbose("</ObjectInformation>\n");

	return length;
}

int asn1decode_OnStartUp(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<OnStartUp>\n");

	output_token(out, ":OnStartUp");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("OnStartUp");

	if(is_ActionClass(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ActionClass(der, out, length)) < 0)
			return der_error("OnStartUp");
		left -= sublen;
	}
	else
	{
		return der_error("OnStartUp: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("OnStartUp: %d bytes left", left);

	verbose("</OnStartUp>\n");

	return length;
}

int asn1decode_OnCloseDown(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<OnCloseDown>\n");

	output_token(out, ":OnCloseDown");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("OnCloseDown");

	if(is_ActionClass(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ActionClass(der, out, length)) < 0)
			return der_error("OnCloseDown");
		left -= sublen;
	}
	else
	{
		return der_error("OnCloseDown: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("OnCloseDown: %d bytes left", left);

	verbose("</OnCloseDown>\n");

	return length;
}

int asn1decode_OriginalGroupCachePriority(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<OriginalGroupCachePriority>\n");

	output_token(out, ":OrigGCPriority");

	if((sublen = der_decode_INTEGER(der, out, length)) < 0)
		return der_error("OriginalGroupCachePriority");
	left -= sublen;

	if(left != 0)
		return der_error("OriginalGroupCachePriority: %d bytes left", left);

	verbose("</OriginalGroupCachePriority>\n");

	return length;
}

int asn1decode_Items(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Items>\n");

	output_token(out, ":Items");

	output_token(out, "(");

	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("Items");

		if(is_GroupItem(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_GroupItem(der, out, sublen + tag.length)) < 0)
				return der_error("Items");
			left -= sublen;
		}
		else
		{
			return der_error("Items: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("Items: %d bytes left", left);

	verbose("</Items>\n");

	return length;
}

int asn1decode_GroupItem(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GroupItem>\n");

	/* CHOICE */
	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GroupItem");
	left -= sublen;

	if(is_ResidentProgramClass(tag.class, tag.number))
	{
		if((sublen = asn1decode_ResidentProgramClass(der, out, tag.length)) < 0)
			return der_error("GroupItem");
		left -= tag.length;
	}
	else if(is_RemoteProgramClass(tag.class, tag.number))
	{
		if((sublen = asn1decode_RemoteProgramClass(der, out, tag.length)) < 0)
			return der_error("GroupItem");
		left -= tag.length;
	}
	else if(is_InterchangedProgramClass(tag.class, tag.number))
	{
		if((sublen = asn1decode_InterchangedProgramClass(der, out, tag.length)) < 0)
			return der_error("GroupItem");
		left -= tag.length;
	}
	else if(is_PaletteClass(tag.class, tag.number))
	{
		if((sublen = asn1decode_PaletteClass(der, out, tag.length)) < 0)
			return der_error("GroupItem");
		left -= tag.length;
	}
	else if(is_FontClass(tag.class, tag.number))
	{
		if((sublen = asn1decode_FontClass(der, out, tag.length)) < 0)
			return der_error("GroupItem");
		left -= tag.length;
	}
	else if(is_CursorShapeClass(tag.class, tag.number))
	{
		if((sublen = asn1decode_CursorShapeClass(der, out, tag.length)) < 0)
			return der_error("GroupItem");
		left -= tag.length;
	}
	else if(is_BooleanVariableClass(tag.class, tag.number))
	{
		if((sublen = asn1decode_BooleanVariableClass(der, out, tag.length)) < 0)
			return der_error("GroupItem");
		left -= tag.length;
	}
	else if(is_IntegerVariableClass(tag.class, tag.number))
	{
		if((sublen = asn1decode_IntegerVariableClass(der, out, tag.length)) < 0)
			return der_error("GroupItem");
		left -= tag.length;
	}
	else if(is_OctetStringVariableClass(tag.class, tag.number))
	{
		if((sublen = asn1decode_OctetStringVariableClass(der, out, tag.length)) < 0)
			return der_error("GroupItem");
		left -= tag.length;
	}
	else if(is_ObjectRefVariableClass(tag.class, tag.number))
	{
		if((sublen = asn1decode_ObjectRefVariableClass(der, out, tag.length)) < 0)
			return der_error("GroupItem");
		left -= tag.length;
	}
	else if(is_ContentRefVariableClass(tag.class, tag.number))
	{
		if((sublen = asn1decode_ContentRefVariableClass(der, out, tag.length)) < 0)
			return der_error("GroupItem");
		left -= tag.length;
	}
	else if(is_LinkClass(tag.class, tag.number))
	{
		if((sublen = asn1decode_LinkClass(der, out, tag.length)) < 0)
			return der_error("GroupItem");
		left -= tag.length;
	}
	else if(is_StreamClass(tag.class, tag.number))
	{
		if((sublen = asn1decode_StreamClass(der, out, tag.length)) < 0)
			return der_error("GroupItem");
		left -= tag.length;
	}
	else if(is_BitmapClass(tag.class, tag.number))
	{
		if((sublen = asn1decode_BitmapClass(der, out, tag.length)) < 0)
			return der_error("GroupItem");
		left -= tag.length;
	}
	else if(is_LineArtClass(tag.class, tag.number))
	{
		if((sublen = asn1decode_LineArtClass(der, out, tag.length)) < 0)
			return der_error("GroupItem");
		left -= tag.length;
	}
	else if(is_DynamicLineArtClass(tag.class, tag.number))
	{
		if((sublen = asn1decode_DynamicLineArtClass(der, out, tag.length)) < 0)
			return der_error("GroupItem");
		left -= tag.length;
	}
	else if(is_RectangleClass(tag.class, tag.number))
	{
		if((sublen = asn1decode_RectangleClass(der, out, tag.length)) < 0)
			return der_error("GroupItem");
		left -= tag.length;
	}
	else if(is_HotspotClass(tag.class, tag.number))
	{
		if((sublen = asn1decode_HotspotClass(der, out, tag.length)) < 0)
			return der_error("GroupItem");
		left -= tag.length;
	}
	else if(is_SwitchButtonClass(tag.class, tag.number))
	{
		if((sublen = asn1decode_SwitchButtonClass(der, out, tag.length)) < 0)
			return der_error("GroupItem");
		left -= tag.length;
	}
	else if(is_PushButtonClass(tag.class, tag.number))
	{
		if((sublen = asn1decode_PushButtonClass(der, out, tag.length)) < 0)
			return der_error("GroupItem");
		left -= tag.length;
	}
	else if(is_TextClass(tag.class, tag.number))
	{
		if((sublen = asn1decode_TextClass(der, out, tag.length)) < 0)
			return der_error("GroupItem");
		left -= tag.length;
	}
	else if(is_EntryFieldClass(tag.class, tag.number))
	{
		if((sublen = asn1decode_EntryFieldClass(der, out, tag.length)) < 0)
			return der_error("GroupItem");
		left -= tag.length;
	}
	else if(is_HyperTextClass(tag.class, tag.number))
	{
		if((sublen = asn1decode_HyperTextClass(der, out, tag.length)) < 0)
			return der_error("GroupItem");
		left -= tag.length;
	}
	else if(is_SliderClass(tag.class, tag.number))
	{
		if((sublen = asn1decode_SliderClass(der, out, tag.length)) < 0)
			return der_error("GroupItem");
		left -= tag.length;
	}
	else if(is_TokenGroupClass(tag.class, tag.number))
	{
		if((sublen = asn1decode_TokenGroupClass(der, out, tag.length)) < 0)
			return der_error("GroupItem");
		left -= tag.length;
	}
	else if(is_ListGroupClass(tag.class, tag.number))
	{
		if((sublen = asn1decode_ListGroupClass(der, out, tag.length)) < 0)
			return der_error("GroupItem");
		left -= tag.length;
	}
	else
	{
		return der_error("GroupItem");
	}

	if(left != 0)
		return der_error("GroupItem: %d bytes left", left);

	verbose("</GroupItem>\n");

	return length;
}

int asn1decode_ApplicationClass(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ApplicationClass>\n");

	/* SET */
	output_token(out, "{:Application");

	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("ApplicationClass");
		left -= sublen;
		if(is_Group(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_Group(der, out, sublen + tag.length)) < 0)
				return der_error("ApplicationClass");
			left -= tag.length;
		}
		else if(is_OnSpawnCloseDown(tag.class, tag.number))
		{
			if((sublen = asn1decode_OnSpawnCloseDown(der, out, tag.length)) < 0)
				return der_error("ApplicationClass");
			left -= tag.length;
		}
		else if(is_OnRestart(tag.class, tag.number))
		{
			if((sublen = asn1decode_OnRestart(der, out, tag.length)) < 0)
				return der_error("ApplicationClass");
			left -= tag.length;
		}
		else if(is_DefaultAttributes(tag.class, tag.number))
		{
			if((sublen = asn1decode_DefaultAttributes(der, out, tag.length)) < 0)
				return der_error("ApplicationClass");
			left -= tag.length;
		}
		else
		{
			return der_error("ApplicationClass: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	output_token(out, "}");

	if(left != 0)
		return der_error("ApplicationClass: %d bytes left", left);

	verbose("</ApplicationClass>\n");

	return length;
}

int asn1decode_OnSpawnCloseDown(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<OnSpawnCloseDown>\n");

	output_token(out, ":OnSpawnCloseDown");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("OnSpawnCloseDown");

	if(is_ActionClass(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ActionClass(der, out, length)) < 0)
			return der_error("OnSpawnCloseDown");
		left -= sublen;
	}
	else
	{
		return der_error("OnSpawnCloseDown: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("OnSpawnCloseDown: %d bytes left", left);

	verbose("</OnSpawnCloseDown>\n");

	return length;
}

int asn1decode_OnRestart(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<OnRestart>\n");

	output_token(out, ":OnRestart");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("OnRestart");

	if(is_ActionClass(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ActionClass(der, out, length)) < 0)
			return der_error("OnRestart");
		left -= sublen;
	}
	else
	{
		return der_error("OnRestart: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("OnRestart: %d bytes left", left);

	verbose("</OnRestart>\n");

	return length;
}

int asn1decode_DefaultAttributes(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<DefaultAttributes>\n");

	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("DefaultAttributes");

		if(is_DefaultAttribute(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_DefaultAttribute(der, out, sublen + tag.length)) < 0)
				return der_error("DefaultAttributes");
			left -= sublen;
		}
		else
		{
			return der_error("DefaultAttributes: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	if(left != 0)
		return der_error("DefaultAttributes: %d bytes left", left);

	verbose("</DefaultAttributes>\n");

	return length;
}

int asn1decode_DefaultAttribute(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<DefaultAttribute>\n");

	/* CHOICE */
	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DefaultAttribute");
	left -= sublen;

	if(is_CharacterSet(tag.class, tag.number))
	{
		if((sublen = asn1decode_CharacterSet(der, out, tag.length)) < 0)
			return der_error("DefaultAttribute");
		left -= tag.length;
	}
	else if(is_BackgroundColour(tag.class, tag.number))
	{
		if((sublen = asn1decode_BackgroundColour(der, out, tag.length)) < 0)
			return der_error("DefaultAttribute");
		left -= tag.length;
	}
	else if(is_TextContentHook(tag.class, tag.number))
	{
		if((sublen = asn1decode_TextContentHook(der, out, tag.length)) < 0)
			return der_error("DefaultAttribute");
		left -= tag.length;
	}
	else if(is_TextColour(tag.class, tag.number))
	{
		if((sublen = asn1decode_TextColour(der, out, tag.length)) < 0)
			return der_error("DefaultAttribute");
		left -= tag.length;
	}
	else if(is_Font(tag.class, tag.number))
	{
		if((sublen = asn1decode_Font(der, out, tag.length)) < 0)
			return der_error("DefaultAttribute");
		left -= tag.length;
	}
	else if(is_FontAttributes(tag.class, tag.number))
	{
		if((sublen = asn1decode_FontAttributes(der, out, tag.length)) < 0)
			return der_error("DefaultAttribute");
		left -= tag.length;
	}
	else if(is_InterchangedProgramContentHook(tag.class, tag.number))
	{
		if((sublen = asn1decode_InterchangedProgramContentHook(der, out, tag.length)) < 0)
			return der_error("DefaultAttribute");
		left -= tag.length;
	}
	else if(is_StreamContentHook(tag.class, tag.number))
	{
		if((sublen = asn1decode_StreamContentHook(der, out, tag.length)) < 0)
			return der_error("DefaultAttribute");
		left -= tag.length;
	}
	else if(is_BitmapContentHook(tag.class, tag.number))
	{
		if((sublen = asn1decode_BitmapContentHook(der, out, tag.length)) < 0)
			return der_error("DefaultAttribute");
		left -= tag.length;
	}
	else if(is_LineArtContentHook(tag.class, tag.number))
	{
		if((sublen = asn1decode_LineArtContentHook(der, out, tag.length)) < 0)
			return der_error("DefaultAttribute");
		left -= tag.length;
	}
	else if(is_ButtonRefColour(tag.class, tag.number))
	{
		if((sublen = asn1decode_ButtonRefColour(der, out, tag.length)) < 0)
			return der_error("DefaultAttribute");
		left -= tag.length;
	}
	else if(is_HighlightRefColour(tag.class, tag.number))
	{
		if((sublen = asn1decode_HighlightRefColour(der, out, tag.length)) < 0)
			return der_error("DefaultAttribute");
		left -= tag.length;
	}
	else if(is_SliderRefColour(tag.class, tag.number))
	{
		if((sublen = asn1decode_SliderRefColour(der, out, tag.length)) < 0)
			return der_error("DefaultAttribute");
		left -= tag.length;
	}
	else
	{
		return der_error("DefaultAttribute");
	}

	if(left != 0)
		return der_error("DefaultAttribute: %d bytes left", left);

	verbose("</DefaultAttribute>\n");

	return length;
}

int asn1decode_CharacterSet(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<CharacterSet>\n");

	output_token(out, ":CharacterSet");

	if((sublen = der_decode_INTEGER(der, out, length)) < 0)
		return der_error("CharacterSet");
	left -= sublen;

	if(left != 0)
		return der_error("CharacterSet: %d bytes left", left);

	verbose("</CharacterSet>\n");

	return length;
}

int asn1decode_BackgroundColour(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<BackgroundColour>\n");

	output_token(out, ":BackgroundColour");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("BackgroundColour");

	if(is_Colour(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Colour(der, out, length)) < 0)
			return der_error("BackgroundColour");
		left -= sublen;
	}
	else
	{
		return der_error("BackgroundColour: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("BackgroundColour: %d bytes left", left);

	verbose("</BackgroundColour>\n");

	return length;
}

int asn1decode_TextContentHook(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<TextContentHook>\n");

	output_token(out, ":TextCHook");

	if((sublen = der_decode_INTEGER(der, out, length)) < 0)
		return der_error("TextContentHook");
	left -= sublen;

	if(left != 0)
		return der_error("TextContentHook: %d bytes left", left);

	verbose("</TextContentHook>\n");

	return length;
}

int asn1decode_TextColour(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<TextColour>\n");

	output_token(out, ":TextColour");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("TextColour");

	if(is_Colour(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Colour(der, out, length)) < 0)
			return der_error("TextColour");
		left -= sublen;
	}
	else
	{
		return der_error("TextColour: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("TextColour: %d bytes left", left);

	verbose("</TextColour>\n");

	return length;
}

int asn1decode_Font(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Font>\n");

	output_token(out, ":Font");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Font");

	if(is_FontBody(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_FontBody(der, out, length)) < 0)
			return der_error("Font");
		left -= sublen;
	}
	else
	{
		return der_error("Font: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("Font: %d bytes left", left);

	verbose("</Font>\n");

	return length;
}

int asn1decode_FontBody(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<FontBody>\n");

	/* CHOICE */
	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("FontBody");
	left -= sublen;

	if(is_DirectFont(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_DirectFont(der, out, sublen + tag.length)) < 0)
			return der_error("FontBody");
		left -= tag.length;
	}
	else if(is_IndirectFont(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_IndirectFont(der, out, sublen + tag.length)) < 0)
			return der_error("FontBody");
		left -= tag.length;
	}
	else
	{
		return der_error("FontBody");
	}

	if(left != 0)
		return der_error("FontBody: %d bytes left", left);

	verbose("</FontBody>\n");

	return length;
}

int asn1decode_DirectFont(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<DirectFont>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DirectFont");

	if(is_OctetString(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_OctetString(der, out, length)) < 0)
			return der_error("DirectFont");
		left -= sublen;
	}
	else
	{
		return der_error("DirectFont: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("DirectFont: %d bytes left", left);

	verbose("</DirectFont>\n");

	return length;
}

int asn1decode_IndirectFont(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<IndirectFont>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("IndirectFont");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("IndirectFont");
		left -= sublen;
	}
	else
	{
		return der_error("IndirectFont: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("IndirectFont: %d bytes left", left);

	verbose("</IndirectFont>\n");

	return length;
}

int asn1decode_FontAttributes(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<FontAttributes>\n");

	output_token(out, ":FontAttributes");

	if((sublen = der_decode_OctetString(der, out, length)) < 0)
		return der_error("FontAttributes");
	left -= sublen;

	if(left != 0)
		return der_error("FontAttributes: %d bytes left", left);

	verbose("</FontAttributes>\n");

	return length;
}

int asn1decode_InterchangedProgramContentHook(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<InterchangedProgramContentHook>\n");

	output_token(out, ":InterchgPrgCHook");

	if((sublen = der_decode_INTEGER(der, out, length)) < 0)
		return der_error("InterchangedProgramContentHook");
	left -= sublen;

	if(left != 0)
		return der_error("InterchangedProgramContentHook: %d bytes left", left);

	verbose("</InterchangedProgramContentHook>\n");

	return length;
}

int asn1decode_StreamContentHook(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<StreamContentHook>\n");

	output_token(out, ":StreamCHook");

	if((sublen = der_decode_INTEGER(der, out, length)) < 0)
		return der_error("StreamContentHook");
	left -= sublen;

	if(left != 0)
		return der_error("StreamContentHook: %d bytes left", left);

	verbose("</StreamContentHook>\n");

	return length;
}

int asn1decode_BitmapContentHook(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<BitmapContentHook>\n");

	output_token(out, ":BitmapCHook");

	if((sublen = der_decode_INTEGER(der, out, length)) < 0)
		return der_error("BitmapContentHook");
	left -= sublen;

	if(left != 0)
		return der_error("BitmapContentHook: %d bytes left", left);

	verbose("</BitmapContentHook>\n");

	return length;
}

int asn1decode_LineArtContentHook(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<LineArtContentHook>\n");

	output_token(out, ":LineArtCHook");

	if((sublen = der_decode_INTEGER(der, out, length)) < 0)
		return der_error("LineArtContentHook");
	left -= sublen;

	if(left != 0)
		return der_error("LineArtContentHook: %d bytes left", left);

	verbose("</LineArtContentHook>\n");

	return length;
}

int asn1decode_ButtonRefColour(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ButtonRefColour>\n");

	output_token(out, ":ButtonRefColour");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ButtonRefColour");

	if(is_Colour(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Colour(der, out, length)) < 0)
			return der_error("ButtonRefColour");
		left -= sublen;
	}
	else
	{
		return der_error("ButtonRefColour: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("ButtonRefColour: %d bytes left", left);

	verbose("</ButtonRefColour>\n");

	return length;
}

int asn1decode_HighlightRefColour(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<HighlightRefColour>\n");

	output_token(out, ":HighlightRefColour");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("HighlightRefColour");

	if(is_Colour(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Colour(der, out, length)) < 0)
			return der_error("HighlightRefColour");
		left -= sublen;
	}
	else
	{
		return der_error("HighlightRefColour: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("HighlightRefColour: %d bytes left", left);

	verbose("</HighlightRefColour>\n");

	return length;
}

int asn1decode_SliderRefColour(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SliderRefColour>\n");

	output_token(out, ":SliderRefColour");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SliderRefColour");

	if(is_Colour(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Colour(der, out, length)) < 0)
			return der_error("SliderRefColour");
		left -= sublen;
	}
	else
	{
		return der_error("SliderRefColour: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("SliderRefColour: %d bytes left", left);

	verbose("</SliderRefColour>\n");

	return length;
}

int asn1decode_SceneClass(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SceneClass>\n");

	/* SET */
	output_token(out, "{:Scene");

	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("SceneClass");
		left -= sublen;
		if(is_Group(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_Group(der, out, sublen + tag.length)) < 0)
				return der_error("SceneClass");
			left -= tag.length;
		}
		else if(is_InputEventRegister(tag.class, tag.number))
		{
			if((sublen = asn1decode_InputEventRegister(der, out, tag.length)) < 0)
				return der_error("SceneClass");
			left -= tag.length;
		}
		else if(is_SceneCoordinateSystem(tag.class, tag.number))
		{
			if((sublen = asn1decode_SceneCoordinateSystem(der, out, tag.length)) < 0)
				return der_error("SceneClass");
			left -= tag.length;
		}
		else if(is_AspectRatio(tag.class, tag.number))
		{
			if((sublen = asn1decode_AspectRatio(der, out, tag.length)) < 0)
				return der_error("SceneClass");
			left -= tag.length;
		}
		else if(is_MovingCursor(tag.class, tag.number))
		{
			if((sublen = asn1decode_MovingCursor(der, out, tag.length)) < 0)
				return der_error("SceneClass");
			left -= tag.length;
		}
		else if(is_NextScenes(tag.class, tag.number))
		{
			if((sublen = asn1decode_NextScenes(der, out, tag.length)) < 0)
				return der_error("SceneClass");
			left -= tag.length;
		}
		else if(is_InputEventMask(tag.class, tag.number))
		{
			if((sublen = asn1decode_InputEventMask(der, out, tag.length)) < 0)
				return der_error("SceneClass");
			left -= tag.length;
		}
		else
		{
			return der_error("SceneClass: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	output_token(out, "}");

	if(left != 0)
		return der_error("SceneClass: %d bytes left", left);

	verbose("</SceneClass>\n");

	return length;
}

int asn1decode_InputEventRegister(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<InputEventRegister>\n");

	output_token(out, ":InputEventReg");

	if((sublen = der_decode_INTEGER(der, out, length)) < 0)
		return der_error("InputEventRegister");
	left -= sublen;

	if(left != 0)
		return der_error("InputEventRegister: %d bytes left", left);

	verbose("</InputEventRegister>\n");

	return length;
}

int asn1decode_SceneCoordinateSystem(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SceneCoordinateSystem>\n");

	/* SEQUENCE */
	output_token(out, ":SceneCS");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SceneCoordinateSystem");
	left -= sublen;

	if(is_XScene(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_XScene(der, out, sublen + tag.length)) < 0)
			return der_error("SceneCoordinateSystem");
		left -= tag.length;
	}
	else
	{
		return der_error("SceneCoordinateSystem: missing XScene");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SceneCoordinateSystem");
	left -= sublen;

	if(is_YScene(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_YScene(der, out, sublen + tag.length)) < 0)
			return der_error("SceneCoordinateSystem");
		left -= tag.length;
	}
	else
	{
		return der_error("SceneCoordinateSystem: missing YScene");
	}

	if(left != 0)
		return der_error("SceneCoordinateSystem: %d bytes left", left);

	verbose("</SceneCoordinateSystem>\n");

	return length;
}

int asn1decode_XScene(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<XScene>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("XScene");

	if(is_INTEGER(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_INTEGER(der, out, length)) < 0)
			return der_error("XScene");
		left -= sublen;
	}
	else
	{
		return der_error("XScene: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("XScene: %d bytes left", left);

	verbose("</XScene>\n");

	return length;
}

int asn1decode_YScene(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<YScene>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("YScene");

	if(is_INTEGER(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_INTEGER(der, out, length)) < 0)
			return der_error("YScene");
		left -= sublen;
	}
	else
	{
		return der_error("YScene: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("YScene: %d bytes left", left);

	verbose("</YScene>\n");

	return length;
}

int asn1decode_AspectRatio(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<AspectRatio>\n");

	/* SEQUENCE */
	output_token(out, ":AspectRatio");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("AspectRatio");
	left -= sublen;

	if(is_Width(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Width(der, out, sublen + tag.length)) < 0)
			return der_error("AspectRatio");
		left -= tag.length;
	}
	else
	{
		return der_error("AspectRatio: missing Width");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("AspectRatio");
	left -= sublen;

	if(is_Height(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Height(der, out, sublen + tag.length)) < 0)
			return der_error("AspectRatio");
		left -= tag.length;
	}
	else
	{
		return der_error("AspectRatio: missing Height");
	}

	if(left != 0)
		return der_error("AspectRatio: %d bytes left", left);

	verbose("</AspectRatio>\n");

	return length;
}

int asn1decode_Width(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Width>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Width");

	if(is_INTEGER(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_INTEGER(der, out, length)) < 0)
			return der_error("Width");
		left -= sublen;
	}
	else
	{
		return der_error("Width: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("Width: %d bytes left", left);

	verbose("</Width>\n");

	return length;
}

int asn1decode_Height(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Height>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Height");

	if(is_INTEGER(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_INTEGER(der, out, length)) < 0)
			return der_error("Height");
		left -= sublen;
	}
	else
	{
		return der_error("Height: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("Height: %d bytes left", left);

	verbose("</Height>\n");

	return length;
}

int asn1decode_MovingCursor(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<MovingCursor>\n");

	output_token(out, ":MovingCursor");

	if((sublen = der_decode_BOOLEAN(der, out, length)) < 0)
		return der_error("MovingCursor");
	left -= sublen;

	if(left != 0)
		return der_error("MovingCursor: %d bytes left", left);

	verbose("</MovingCursor>\n");

	return length;
}

int asn1decode_NextScenes(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NextScenes>\n");

	output_token(out, ":NextScenes");

	output_token(out, "(");

	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("NextScenes");

		if(is_NextScene(tag.class, tag.number))
		{
			left -= sublen;
			if((sublen = asn1decode_NextScene(der, out, tag.length)) < 0)
				return der_error("NextScenes");
			left -= sublen;
		}
		else
		{
			return der_error("NextScenes: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("NextScenes: %d bytes left", left);

	verbose("</NextScenes>\n");

	return length;
}

int asn1decode_NextScene(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NextScene>\n");

	/* SEQUENCE */
	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NextScene");
	left -= sublen;

	if(is_SceneRef(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_SceneRef(der, out, sublen + tag.length)) < 0)
			return der_error("NextScene");
		left -= tag.length;
	}
	else
	{
		return der_error("NextScene: missing SceneRef");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NextScene");
	left -= sublen;

	if(is_SceneWeight(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_SceneWeight(der, out, sublen + tag.length)) < 0)
			return der_error("NextScene");
		left -= tag.length;
	}
	else
	{
		return der_error("NextScene: missing SceneWeight");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("NextScene: %d bytes left", left);

	verbose("</NextScene>\n");

	return length;
}

int asn1decode_InputEventMask(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<InputEventMask>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("InputEventMask");

	if(is_OctetString(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_OctetString(der, out, length)) < 0)
			return der_error("InputEventMask");
		left -= sublen;
	}
	else
	{
		return der_error("InputEventMask: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("InputEventMask: %d bytes left", left);

	verbose("</InputEventMask>\n");

	return length;
}

int asn1decode_SceneRef(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SceneRef>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SceneRef");

	if(is_OctetString(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_OctetString(der, out, length)) < 0)
			return der_error("SceneRef");
		left -= sublen;
	}
	else
	{
		return der_error("SceneRef: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("SceneRef: %d bytes left", left);

	verbose("</SceneRef>\n");

	return length;
}

int asn1decode_SceneWeight(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SceneWeight>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SceneWeight");

	if(is_INTEGER(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_INTEGER(der, out, length)) < 0)
			return der_error("SceneWeight");
		left -= sublen;
	}
	else
	{
		return der_error("SceneWeight: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("SceneWeight: %d bytes left", left);

	verbose("</SceneWeight>\n");

	return length;
}

int asn1decode_Ingredient(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Ingredient>\n");

	/* SET */
	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("Ingredient");
		left -= sublen;
		if(is_Root(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_Root(der, out, sublen + tag.length)) < 0)
				return der_error("Ingredient");
			left -= tag.length;
		}
		else if(is_InitiallyActive(tag.class, tag.number))
		{
			if((sublen = asn1decode_InitiallyActive(der, out, tag.length)) < 0)
				return der_error("Ingredient");
			left -= tag.length;
		}
		else if(is_ContentHook(tag.class, tag.number))
		{
			if((sublen = asn1decode_ContentHook(der, out, tag.length)) < 0)
				return der_error("Ingredient");
			left -= tag.length;
		}
		else if(is_OriginalContent(tag.class, tag.number))
		{
			if((sublen = asn1decode_OriginalContent(der, out, tag.length)) < 0)
				return der_error("Ingredient");
			left -= tag.length;
		}
		else if(is_Shared(tag.class, tag.number))
		{
			if((sublen = asn1decode_Shared(der, out, tag.length)) < 0)
				return der_error("Ingredient");
			left -= tag.length;
		}
		else
		{
			return der_error("Ingredient: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	if(left != 0)
		return der_error("Ingredient: %d bytes left", left);

	verbose("</Ingredient>\n");

	return length;
}

int asn1decode_InitiallyActive(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<InitiallyActive>\n");

	output_token(out, ":InitiallyActive");

	if((sublen = der_decode_BOOLEAN(der, out, length)) < 0)
		return der_error("InitiallyActive");
	left -= sublen;

	if(left != 0)
		return der_error("InitiallyActive: %d bytes left", left);

	verbose("</InitiallyActive>\n");

	return length;
}

int asn1decode_ContentHook(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ContentHook>\n");

	output_token(out, ":CHook");

	if((sublen = der_decode_INTEGER(der, out, length)) < 0)
		return der_error("ContentHook");
	left -= sublen;

	if(left != 0)
		return der_error("ContentHook: %d bytes left", left);

	verbose("</ContentHook>\n");

	return length;
}

int asn1decode_OriginalContent(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<OriginalContent>\n");

	output_token(out, ":OrigContent");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("OriginalContent");

	if(is_ContentBody(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ContentBody(der, out, length)) < 0)
			return der_error("OriginalContent");
		left -= sublen;
	}
	else
	{
		return der_error("OriginalContent: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("OriginalContent: %d bytes left", left);

	verbose("</OriginalContent>\n");

	return length;
}

int asn1decode_ContentBody(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ContentBody>\n");

	/* CHOICE */
	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ContentBody");
	left -= sublen;

	if(is_IncludedContent(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_IncludedContent(der, out, sublen + tag.length)) < 0)
			return der_error("ContentBody");
		left -= tag.length;
	}
	else if(is_ReferencedContent(tag.class, tag.number))
	{
		if((sublen = asn1decode_ReferencedContent(der, out, tag.length)) < 0)
			return der_error("ContentBody");
		left -= tag.length;
	}
	else
	{
		return der_error("ContentBody");
	}

	if(left != 0)
		return der_error("ContentBody: %d bytes left", left);

	verbose("</ContentBody>\n");

	return length;
}

int asn1decode_IncludedContent(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<IncludedContent>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("IncludedContent");

	if(is_OctetString(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_OctetString(der, out, length)) < 0)
			return der_error("IncludedContent");
		left -= sublen;
	}
	else
	{
		return der_error("IncludedContent: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("IncludedContent: %d bytes left", left);

	verbose("</IncludedContent>\n");

	return length;
}

int asn1decode_ReferencedContent(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ReferencedContent>\n");

	/* SEQUENCE */
	output_token(out, ":ContentRef");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ReferencedContent");
	left -= sublen;

	if(is_ContentReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ContentReference(der, out, sublen + tag.length)) < 0)
			return der_error("ReferencedContent");
		left -= tag.length;
	}
	else
	{
		return der_error("ReferencedContent: missing ContentReference");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ReferencedContent");
	left -= sublen;

	if(is_ContentSize(tag.class, tag.number))
	{
		if((sublen = asn1decode_ContentSize(der, out, tag.length)) < 0)
			return der_error("ReferencedContent");
		left -= tag.length;
	}
	else
	{
		fseek(der, -sublen, SEEK_CUR);
		left += sublen;
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ReferencedContent");
	left -= sublen;

	if(is_ContentCachePriority(tag.class, tag.number))
	{
		if((sublen = asn1decode_ContentCachePriority(der, out, tag.length)) < 0)
			return der_error("ReferencedContent");
		left -= tag.length;
	}
	else
	{
		fseek(der, -sublen, SEEK_CUR);
		left += sublen;
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("ReferencedContent: %d bytes left", left);

	verbose("</ReferencedContent>\n");

	return length;
}

int asn1decode_ContentSize(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ContentSize>\n");

	output_token(out, ":ContentSize");

	if((sublen = der_decode_INTEGER(der, out, length)) < 0)
		return der_error("ContentSize");
	left -= sublen;

	if(left != 0)
		return der_error("ContentSize: %d bytes left", left);

	verbose("</ContentSize>\n");

	return length;
}

int asn1decode_ContentCachePriority(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ContentCachePriority>\n");

	output_token(out, ":CCPriority");

	if((sublen = der_decode_INTEGER(der, out, length)) < 0)
		return der_error("ContentCachePriority");
	left -= sublen;

	if(left != 0)
		return der_error("ContentCachePriority: %d bytes left", left);

	verbose("</ContentCachePriority>\n");

	return length;
}

int asn1decode_Shared(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Shared>\n");

	output_token(out, ":Shared");

	if((sublen = der_decode_BOOLEAN(der, out, length)) < 0)
		return der_error("Shared");
	left -= sublen;

	if(left != 0)
		return der_error("Shared: %d bytes left", left);

	verbose("</Shared>\n");

	return length;
}

int asn1decode_LinkClass(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<LinkClass>\n");

	/* SET */
	output_token(out, "{:Link");

	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("LinkClass");
		left -= sublen;
		if(is_Ingredient(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_Ingredient(der, out, sublen + tag.length)) < 0)
				return der_error("LinkClass");
			left -= tag.length;
		}
		else if(is_LinkCondition(tag.class, tag.number))
		{
			if((sublen = asn1decode_LinkCondition(der, out, tag.length)) < 0)
				return der_error("LinkClass");
			left -= tag.length;
		}
		else if(is_LinkEffect(tag.class, tag.number))
		{
			if((sublen = asn1decode_LinkEffect(der, out, tag.length)) < 0)
				return der_error("LinkClass");
			left -= tag.length;
		}
		else
		{
			return der_error("LinkClass: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	output_token(out, "}");

	if(left != 0)
		return der_error("LinkClass: %d bytes left", left);

	verbose("</LinkClass>\n");

	return length;
}

int asn1decode_LinkCondition(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<LinkCondition>\n");

	/* SEQUENCE */
	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("LinkCondition");
	left -= sublen;

	if(is_EventSource(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_EventSource(der, out, sublen + tag.length)) < 0)
			return der_error("LinkCondition");
		left -= tag.length;
	}
	else
	{
		return der_error("LinkCondition: missing EventSource");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("LinkCondition");
	left -= sublen;

	if(is_EventType(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_EventType(der, out, sublen + tag.length)) < 0)
			return der_error("LinkCondition");
		left -= tag.length;
	}
	else
	{
		return der_error("LinkCondition: missing EventType");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("LinkCondition");
	left -= sublen;

	if(is_EventData(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_EventData(der, out, sublen + tag.length)) < 0)
			return der_error("LinkCondition");
		left -= tag.length;
	}
	else
	{
		fseek(der, -sublen, SEEK_CUR);
		left += sublen;
	}

	if(left != 0)
		return der_error("LinkCondition: %d bytes left", left);

	verbose("</LinkCondition>\n");

	return length;
}

int asn1decode_EventSource(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<EventSource>\n");

	output_token(out, ":EventSource");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("EventSource");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("EventSource");
		left -= sublen;
	}
	else
	{
		return der_error("EventSource: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("EventSource: %d bytes left", left);

	verbose("</EventSource>\n");

	return length;
}

int asn1decode_EventType(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<EventType>\n");

	output_token(out, ":EventType");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("EventType");

	if(is_EventTypeEnum(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_EventTypeEnum(der, out, length)) < 0)
			return der_error("EventType");
		left -= sublen;
	}
	else
	{
		return der_error("EventType: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("EventType: %d bytes left", left);

	verbose("</EventType>\n");

	return length;
}

int asn1decode_EventTypeEnum(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* ENUMERATED */
	char *enum_names[] = {
		"IsAvailable",
		"ContentAvailable",
		"IsDeleted",
		"IsRunning",
		"IsStopped",
		"UserInput",
		"AnchorFired",
		"TimerFired",
		"AsynchStopped",
		"InteractionCompleted",
		"TokenMovedFrom",
		"TokenMovedTo",
		"StreamEvent",
		"StreamPlaying",
		"StreamStopped",
		"CounterTrigger",
		"HighlightOn",
		"HighlightOff",
		"CursorEnter",
		"CursorLeave",
		"IsSelected",
		"IsDeselected",
		"TestEvent",
		"FirstItemPresented",
		"LastItemPresented",
		"HeadItems",
		"TailItems",
		"ItemSelected",
		"ItemDeselected",
		"EntryFieldFull",
		"EngineEvent",
		"FocusMoved",
		"SliderValueChanged",
	};

	/* keep the compiler happy */
	tag = tag;

	verbose("<EventTypeEnum>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("EventTypeEnum");
	left -= sublen;

	if(is_EventTypeEnum(tag.class, tag.number))
	{
		if((sublen = der_decode_ENUMERATED(der, out, tag.length, 33, enum_names)) < 0)
			return der_error("EventTypeEnum");
		left -= tag.length;
	}
	else
	{
		return der_error("EventTypeEnum");
	}

	if(left != 0)
		return der_error("EventTypeEnum: %d bytes left", left);

	verbose("</EventTypeEnum>\n");

	return length;
}

int der_decode_EventTypeEnum(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;

	/* ENUMERATED */
	char *enum_names[] = {
		"IsAvailable",
		"ContentAvailable",
		"IsDeleted",
		"IsRunning",
		"IsStopped",
		"UserInput",
		"AnchorFired",
		"TimerFired",
		"AsynchStopped",
		"InteractionCompleted",
		"TokenMovedFrom",
		"TokenMovedTo",
		"StreamEvent",
		"StreamPlaying",
		"StreamStopped",
		"CounterTrigger",
		"HighlightOn",
		"HighlightOff",
		"CursorEnter",
		"CursorLeave",
		"IsSelected",
		"IsDeselected",
		"TestEvent",
		"FirstItemPresented",
		"LastItemPresented",
		"HeadItems",
		"TailItems",
		"ItemSelected",
		"ItemDeselected",
		"EntryFieldFull",
		"EngineEvent",
		"FocusMoved",
		"SliderValueChanged",
	};

	verbose("<EventTypeEnum>\n");

	if((sublen = der_decode_ENUMERATED(der, out, length, 33, enum_names)) < 0)
		return der_error("EventTypeEnum");
	left -= sublen;

	if(left != 0)
		return der_error("EventTypeEnum: %d bytes left", left);

	verbose("</EventTypeEnum>\n");

	return length;
}

int asn1decode_EventData(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<EventData>\n");

	output_token(out, ":EventData");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("EventData");

	if(is_EventDataBody(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_EventDataBody(der, out, length)) < 0)
			return der_error("EventData");
		left -= sublen;
	}
	else
	{
		return der_error("EventData: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("EventData: %d bytes left", left);

	verbose("</EventData>\n");

	return length;
}

int asn1decode_EventDataBody(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<EventDataBody>\n");

	/* CHOICE */
	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("EventDataBody");
	left -= sublen;

	if(is_OctetString(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_OctetString(der, out, sublen + tag.length)) < 0)
			return der_error("EventDataBody");
		left -= tag.length;
	}
	else if(is_BOOLEAN(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_BOOLEAN(der, out, sublen + tag.length)) < 0)
			return der_error("EventDataBody");
		left -= tag.length;
	}
	else if(is_INTEGER(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_INTEGER(der, out, sublen + tag.length)) < 0)
			return der_error("EventDataBody");
		left -= tag.length;
	}
	else
	{
		return der_error("EventDataBody");
	}

	if(left != 0)
		return der_error("EventDataBody: %d bytes left", left);

	verbose("</EventDataBody>\n");

	return length;
}

int asn1decode_LinkEffect(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<LinkEffect>\n");

	while(left > 0) // hmkim : added.
	{

	output_token(out, ":LinkEffect");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("LinkEffect");

	if(is_ActionClass(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ActionClass(der, out, length)) < 0)
			return der_error("LinkEffect");
		left -= sublen;
	}
	else
	{
		return der_error("LinkEffect: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	} // hmkim : added.

	if(left != 0)
		return der_error("LinkEffect: %d bytes left", left);

	verbose("</LinkEffect>\n");

	return length;
}

int asn1decode_Program(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Program>\n");

	/* SET */
	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("Program");
		left -= sublen;
		if(is_Ingredient(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_Ingredient(der, out, sublen + tag.length)) < 0)
				return der_error("Program");
			left -= tag.length;
		}
		else if(is_Name(tag.class, tag.number))
		{
			if((sublen = asn1decode_Name(der, out, tag.length)) < 0)
				return der_error("Program");
			left -= tag.length;
		}
		else if(is_InitiallyAvailable(tag.class, tag.number))
		{
			if((sublen = asn1decode_InitiallyAvailable(der, out, tag.length)) < 0)
				return der_error("Program");
			left -= tag.length;
		}
		else
		{
			return der_error("Program: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	if(left != 0)
		return der_error("Program: %d bytes left", left);

	verbose("</Program>\n");

	return length;
}

int asn1decode_Name(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Name>\n");

	output_token(out, ":Name");

	if((sublen = der_decode_OctetString(der, out, length)) < 0)
		return der_error("Name");
	left -= sublen;

	if(left != 0)
		return der_error("Name: %d bytes left", left);

	verbose("</Name>\n");

	return length;
}

int asn1decode_InitiallyAvailable(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<InitiallyAvailable>\n");

	output_token(out, ":InitiallyAvailable");

	if((sublen = der_decode_BOOLEAN(der, out, length)) < 0)
		return der_error("InitiallyAvailable");
	left -= sublen;

	if(left != 0)
		return der_error("InitiallyAvailable: %d bytes left", left);

	verbose("</InitiallyAvailable>\n");

	return length;
}

int asn1decode_ResidentProgramClass(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ResidentProgramClass>\n");

	output_token(out, "{:ResidentPrg");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ResidentProgramClass");

	if(is_Program(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Program(der, out, length)) < 0)
			return der_error("ResidentProgramClass");
		left -= sublen;
	}
	else
	{
		return der_error("ResidentProgramClass: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	output_token(out, "}");

	if(left != 0)
		return der_error("ResidentProgramClass: %d bytes left", left);

	verbose("</ResidentProgramClass>\n");

	return length;
}

int asn1decode_RemoteProgramClass(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<RemoteProgramClass>\n");

	/* SET */
	output_token(out, "{:RemotePrg");

	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("RemoteProgramClass");
		left -= sublen;
		if(is_Program(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_Program(der, out, sublen + tag.length)) < 0)
				return der_error("RemoteProgramClass");
			left -= tag.length;
		}
		else if(is_ProgramConnectionTag(tag.class, tag.number))
		{
			if((sublen = asn1decode_ProgramConnectionTag(der, out, tag.length)) < 0)
				return der_error("RemoteProgramClass");
			left -= tag.length;
		}
		else
		{
			return der_error("RemoteProgramClass: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	output_token(out, "}");

	if(left != 0)
		return der_error("RemoteProgramClass: %d bytes left", left);

	verbose("</RemoteProgramClass>\n");

	return length;
}

int asn1decode_ProgramConnectionTag(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ProgramConnectionTag>\n");

	output_token(out, ":ConnectionTag");

	if((sublen = der_decode_INTEGER(der, out, length)) < 0)
		return der_error("ProgramConnectionTag");
	left -= sublen;

	if(left != 0)
		return der_error("ProgramConnectionTag: %d bytes left", left);

	verbose("</ProgramConnectionTag>\n");

	return length;
}

int asn1decode_InterchangedProgramClass(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<InterchangedProgramClass>\n");

	output_token(out, "{:InterchgPrg");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("InterchangedProgramClass");

	if(is_Program(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Program(der, out, length)) < 0)
			return der_error("InterchangedProgramClass");
		left -= sublen;
	}
	else
	{
		return der_error("InterchangedProgramClass: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	output_token(out, "}");

	if(left != 0)
		return der_error("InterchangedProgramClass: %d bytes left", left);

	verbose("</InterchangedProgramClass>\n");

	return length;
}

int asn1decode_PaletteClass(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<PaletteClass>\n");

	output_token(out, "{:Palette");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("PaletteClass");

	if(is_Ingredient(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Ingredient(der, out, length)) < 0)
			return der_error("PaletteClass");
		left -= sublen;
	}
	else
	{
		return der_error("PaletteClass: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	output_token(out, "}");

	if(left != 0)
		return der_error("PaletteClass: %d bytes left", left);

	verbose("</PaletteClass>\n");

	return length;
}

int asn1decode_FontClass(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<FontClass>\n");

	output_token(out, "{:Font");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("FontClass");

	if(is_Ingredient(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Ingredient(der, out, length)) < 0)
			return der_error("FontClass");
		left -= sublen;
	}
	else
	{
		return der_error("FontClass: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	output_token(out, "}");

	if(left != 0)
		return der_error("FontClass: %d bytes left", left);

	verbose("</FontClass>\n");

	return length;
}

int asn1decode_CursorShapeClass(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<CursorShapeClass>\n");

	output_token(out, "{:CursorShape");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("CursorShapeClass");

	if(is_Ingredient(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Ingredient(der, out, length)) < 0)
			return der_error("CursorShapeClass");
		left -= sublen;
	}
	else
	{
		return der_error("CursorShapeClass: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	output_token(out, "}");

	if(left != 0)
		return der_error("CursorShapeClass: %d bytes left", left);

	verbose("</CursorShapeClass>\n");

	return length;
}

int asn1decode_Variable(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Variable>\n");

	/* SET */
	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("Variable");
		left -= sublen;
		if(is_Ingredient(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_Ingredient(der, out, sublen + tag.length)) < 0)
				return der_error("Variable");
			left -= tag.length;
		}
		else if(is_OriginalValue(tag.class, tag.number))
		{
			if((sublen = asn1decode_OriginalValue(der, out, tag.length)) < 0)
				return der_error("Variable");
			left -= tag.length;
		}
		else
		{
			return der_error("Variable: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	if(left != 0)
		return der_error("Variable: %d bytes left", left);

	verbose("</Variable>\n");

	return length;
}

int asn1decode_OriginalValue(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<OriginalValue>\n");

	output_token(out, ":OrigValue");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("OriginalValue");

	if(is_OriginalValueBody(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_OriginalValueBody(der, out, length)) < 0)
			return der_error("OriginalValue");
		left -= sublen;
	}
	else
	{
		return der_error("OriginalValue: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("OriginalValue: %d bytes left", left);

	verbose("</OriginalValue>\n");

	return length;
}

int asn1decode_OriginalValueBody(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<OriginalValueBody>\n");

	/* CHOICE */
	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("OriginalValueBody");
	left -= sublen;

	if(is_BOOLEAN(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_BOOLEAN(der, out, sublen + tag.length)) < 0)
			return der_error("OriginalValueBody");
		left -= tag.length;
	}
	else if(is_INTEGER(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_INTEGER(der, out, sublen + tag.length)) < 0)
			return der_error("OriginalValueBody");
		left -= tag.length;
	}
	else if(is_OctetString(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_OctetString(der, out, sublen + tag.length)) < 0)
			return der_error("OriginalValueBody");
		left -= tag.length;
	}
	else if(is_ObjectReferenceValue(tag.class, tag.number))
	{
		if((sublen = asn1decode_ObjectReferenceValue(der, out, tag.length)) < 0)
			return der_error("OriginalValueBody");
		left -= tag.length;
	}
	else if(is_ContentReferenceValue(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ContentReferenceValue(der, out, sublen + tag.length)) < 0)
			return der_error("OriginalValueBody");
		left -= tag.length;
	}
	else
	{
		return der_error("OriginalValueBody");
	}

	if(left != 0)
		return der_error("OriginalValueBody: %d bytes left", left);

	verbose("</OriginalValueBody>\n");

	return length;
}

int asn1decode_ObjectReferenceValue(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ObjectReferenceValue>\n");

	output_token(out, ":ObjectRef");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ObjectReferenceValue");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("ObjectReferenceValue");
		left -= sublen;
	}
	else
	{
		return der_error("ObjectReferenceValue: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("ObjectReferenceValue: %d bytes left", left);

	verbose("</ObjectReferenceValue>\n");

	return length;
}

int asn1decode_ContentReferenceValue(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ContentReferenceValue>\n");

	output_token(out, ":ContentRef");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ContentReferenceValue");

	if(is_ContentReference69(tag.class, tag.number))
	{
		left -= sublen;
		if((sublen = asn1decode_ContentReference69(der, out, tag.length)) < 0)
			return der_error("ContentReferenceValue");
		left -= sublen;
	}
	else
	{
		return der_error("ContentReferenceValue: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("ContentReferenceValue: %d bytes left", left);

	verbose("</ContentReferenceValue>\n");

	return length;
}

int asn1decode_BooleanVariableClass(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<BooleanVariableClass>\n");

	output_token(out, "{:BooleanVar");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("BooleanVariableClass");

	if(is_Variable(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Variable(der, out, length)) < 0)
			return der_error("BooleanVariableClass");
		left -= sublen;
	}
	else
	{
		return der_error("BooleanVariableClass: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	output_token(out, "}");

	if(left != 0)
		return der_error("BooleanVariableClass: %d bytes left", left);

	verbose("</BooleanVariableClass>\n");

	return length;
}

int asn1decode_IntegerVariableClass(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<IntegerVariableClass>\n");

	output_token(out, "{:IntegerVar");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("IntegerVariableClass");

	if(is_Variable(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Variable(der, out, length)) < 0)
			return der_error("IntegerVariableClass");
		left -= sublen;
	}
	else
	{
		return der_error("IntegerVariableClass: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	output_token(out, "}");

	if(left != 0)
		return der_error("IntegerVariableClass: %d bytes left", left);

	verbose("</IntegerVariableClass>\n");

	return length;
}

int asn1decode_OctetStringVariableClass(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<OctetStringVariableClass>\n");

	output_token(out, "{:OStringVar");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("OctetStringVariableClass");

	if(is_Variable(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Variable(der, out, length)) < 0)
			return der_error("OctetStringVariableClass");
		left -= sublen;
	}
	else
	{
		return der_error("OctetStringVariableClass: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	output_token(out, "}");

	if(left != 0)
		return der_error("OctetStringVariableClass: %d bytes left", left);

	verbose("</OctetStringVariableClass>\n");

	return length;
}

int asn1decode_ObjectRefVariableClass(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ObjectRefVariableClass>\n");

	output_token(out, "{:ObjectRefVar");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ObjectRefVariableClass");

	if(is_Variable(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Variable(der, out, length)) < 0)
			return der_error("ObjectRefVariableClass");
		left -= sublen;
	}
	else
	{
		return der_error("ObjectRefVariableClass: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	output_token(out, "}");

	if(left != 0)
		return der_error("ObjectRefVariableClass: %d bytes left", left);

	verbose("</ObjectRefVariableClass>\n");

	return length;
}

int asn1decode_ContentRefVariableClass(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ContentRefVariableClass>\n");

	output_token(out, "{:ContentRefVar");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ContentRefVariableClass");

	if(is_Variable(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Variable(der, out, length)) < 0)
			return der_error("ContentRefVariableClass");
		left -= sublen;
	}
	else
	{
		return der_error("ContentRefVariableClass: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	output_token(out, "}");

	if(left != 0)
		return der_error("ContentRefVariableClass: %d bytes left", left);

	verbose("</ContentRefVariableClass>\n");

	return length;
}

int asn1decode_Presentable(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Presentable>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Presentable");

	if(is_Ingredient(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Ingredient(der, out, length)) < 0)
			return der_error("Presentable");
		left -= sublen;
	}
	else
	{
		return der_error("Presentable: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("Presentable: %d bytes left", left);

	verbose("</Presentable>\n");

	return length;
}

int asn1decode_TokenManager(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<TokenManager>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("TokenManager");

	if(is_MovementTable(tag.class, tag.number))
	{
		left -= sublen;
		if((sublen = asn1decode_MovementTable(der, out, tag.length)) < 0)
			return der_error("TokenManager");
		left -= sublen;
	}
	else
	{
		return der_error("TokenManager: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("TokenManager: %d bytes left", left);

	verbose("</TokenManager>\n");

	return length;
}

int asn1decode_MovementTable(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<MovementTable>\n");

	output_token(out, ":MovementTable");

	output_token(out, "(");

	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("MovementTable");

		if(is_Movement(tag.class, tag.number))
		{
			left -= sublen;
			if((sublen = asn1decode_Movement(der, out, tag.length)) < 0)
				return der_error("MovementTable");
			left -= sublen;
		}
		else
		{
			return der_error("MovementTable: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("MovementTable: %d bytes left", left);

	verbose("</MovementTable>\n");

	return length;
}

int asn1decode_Movement(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Movement>\n");

	output_token(out, "(");

	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("Movement");

		if(is_TargetElement(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_TargetElement(der, out, sublen + tag.length)) < 0)
				return der_error("Movement");
			left -= sublen;
		}
		else
		{
			return der_error("Movement: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("Movement: %d bytes left", left);

	verbose("</Movement>\n");

	return length;
}

int asn1decode_TargetElement(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<TargetElement>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("TargetElement");

	if(is_INTEGER(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_INTEGER(der, out, length)) < 0)
			return der_error("TargetElement");
		left -= sublen;
	}
	else
	{
		return der_error("TargetElement: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("TargetElement: %d bytes left", left);

	verbose("</TargetElement>\n");

	return length;
}

int asn1decode_TokenGroupClass(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<TokenGroupClass>\n");

	output_token(out, "{:TokenGroup");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("TokenGroupClass");

	if(is_TokenGroupBody(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_TokenGroupBody(der, out, length)) < 0)
			return der_error("TokenGroupClass");
		left -= sublen;
	}
	else
	{
		return der_error("TokenGroupClass: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	output_token(out, "}");

	if(left != 0)
		return der_error("TokenGroupClass: %d bytes left", left);

	verbose("</TokenGroupClass>\n");

	return length;
}

int asn1decode_TokenGroupBody(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<TokenGroupBody>\n");

	/* SET */
	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("TokenGroupBody");
		left -= sublen;
		if(is_Presentable(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_Presentable(der, out, sublen + tag.length)) < 0)
				return der_error("TokenGroupBody");
			left -= tag.length;
		}
		else if(is_TokenManager(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_TokenManager(der, out, sublen + tag.length)) < 0)
				return der_error("TokenGroupBody");
			left -= tag.length;
		}
		else if(is_TokenGroupItems(tag.class, tag.number))
		{
			if((sublen = asn1decode_TokenGroupItems(der, out, tag.length)) < 0)
				return der_error("TokenGroupBody");
			left -= tag.length;
		}
		else if(is_NoTokenActionSlots(tag.class, tag.number))
		{
			if((sublen = asn1decode_NoTokenActionSlots(der, out, tag.length)) < 0)
				return der_error("TokenGroupBody");
			left -= tag.length;
		}
		else
		{
			return der_error("TokenGroupBody: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	if(left != 0)
		return der_error("TokenGroupBody: %d bytes left", left);

	verbose("</TokenGroupBody>\n");

	return length;
}

int asn1decode_TokenGroupItems(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<TokenGroupItems>\n");

	output_token(out, ":TokenGroupItems");

	output_token(out, "(");

	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("TokenGroupItems");

		if(is_TokenGroupItem(tag.class, tag.number))
		{
			left -= sublen;
			if((sublen = asn1decode_TokenGroupItem(der, out, tag.length)) < 0)
				return der_error("TokenGroupItems");
			left -= sublen;
		}
		else
		{
			return der_error("TokenGroupItems: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("TokenGroupItems: %d bytes left", left);

	verbose("</TokenGroupItems>\n");

	return length;
}

int asn1decode_TokenGroupItem(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<TokenGroupItem>\n");

	/* SEQUENCE */
	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("TokenGroupItem");
	left -= sublen;

	if(is_AVisible(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_AVisible(der, out, sublen + tag.length)) < 0)
			return der_error("TokenGroupItem");
		left -= tag.length;
	}
	else
	{
		return der_error("TokenGroupItem: missing AVisible");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("TokenGroupItem");
	left -= sublen;

	if(is_ActionSlots(tag.class, tag.number))
	{
		if((sublen = asn1decode_ActionSlots(der, out, tag.length)) < 0)
			return der_error("TokenGroupItem");
		left -= tag.length;
	}
	else
	{
		fseek(der, -sublen, SEEK_CUR);
		left += sublen;
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("TokenGroupItem: %d bytes left", left);

	verbose("</TokenGroupItem>\n");

	return length;
}

int asn1decode_AVisible(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<AVisible>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("AVisible");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("AVisible");
		left -= sublen;
	}
	else
	{
		return der_error("AVisible: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("AVisible: %d bytes left", left);

	verbose("</AVisible>\n");

	return length;
}

int asn1decode_ActionSlots(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ActionSlots>\n");

	output_token(out, ":ActionSlots");

	output_token(out, "(");

	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("ActionSlots");

		if(is_ActionSlot(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_ActionSlot(der, out, sublen + tag.length)) < 0)
				return der_error("ActionSlots");
			left -= sublen;
		}
		else
		{
			return der_error("ActionSlots: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("ActionSlots: %d bytes left", left);

	verbose("</ActionSlots>\n");

	return length;
}

int asn1decode_ActionSlot(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ActionSlot>\n");

	/* CHOICE */
	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ActionSlot");
	left -= sublen;

	if(is_ActionClassSeq(tag.class, tag.number))
	{
		if((sublen = asn1decode_ActionClassSeq(der, out, tag.length)) < 0)
			return der_error("ActionSlot");
		left -= tag.length;
	}
	else if(is_Null(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Null(der, out, sublen + tag.length)) < 0)
			return der_error("ActionSlot");
		left -= tag.length;
	}
	else
	{
		return der_error("ActionSlot");
	}

	if(left != 0)
		return der_error("ActionSlot: %d bytes left", left);

	verbose("</ActionSlot>\n");

	return length;
}

int asn1decode_NoTokenActionSlots(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NoTokenActionSlots>\n");

	output_token(out, ":NoTokenActionSlots");

	output_token(out, "(");

	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("NoTokenActionSlots");

		if(is_ActionSlotSeq(tag.class, tag.number))
		{
			left -= sublen;
			if((sublen = asn1decode_ActionSlotSeq(der, out, tag.length)) < 0)
				return der_error("NoTokenActionSlots");
			left -= sublen;
		}
		else
		{
			return der_error("NoTokenActionSlots: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("NoTokenActionSlots: %d bytes left", left);

	verbose("</NoTokenActionSlots>\n");

	return length;
}

int asn1decode_ActionSlotSeq(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ActionSlotSeq>\n");

	/* CHOICE */
	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ActionSlotSeq");
	left -= sublen;

	if(is_ActionClassSeq(tag.class, tag.number))
	{
		if((sublen = asn1decode_ActionClassSeq(der, out, tag.length)) < 0)
			return der_error("ActionSlotSeq");
		left -= tag.length;
	}
	else if(is_Null(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Null(der, out, sublen + tag.length)) < 0)
			return der_error("ActionSlotSeq");
		left -= tag.length;
	}
	else
	{
		return der_error("ActionSlotSeq");
	}

	if(left != 0)
		return der_error("ActionSlotSeq: %d bytes left", left);

	verbose("</ActionSlotSeq>\n");

	return length;
}

int asn1decode_ActionClassSeq(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ActionClassSeq>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ActionClassSeq");

	if(is_ActionClass(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ActionClass(der, out, length)) < 0)
			return der_error("ActionClassSeq");
		left -= sublen;
	}
	else
	{
		return der_error("ActionClassSeq: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("ActionClassSeq: %d bytes left", left);

	verbose("</ActionClassSeq>\n");

	return length;
}

int asn1decode_ListGroupClass(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ListGroupClass>\n");

	/* SET */
	output_token(out, "{:ListGroup");

	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("ListGroupClass");
		left -= sublen;
		if(is_TokenGroupBody(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_TokenGroupBody(der, out, sublen + tag.length)) < 0)
				return der_error("ListGroupClass");
			left -= tag.length;
		}
		else if(is_Positions(tag.class, tag.number))
		{
			if((sublen = asn1decode_Positions(der, out, tag.length)) < 0)
				return der_error("ListGroupClass");
			left -= tag.length;
		}
		else if(is_WrapAround(tag.class, tag.number))
		{
			if((sublen = asn1decode_WrapAround(der, out, tag.length)) < 0)
				return der_error("ListGroupClass");
			left -= tag.length;
		}
		else if(is_MultipleSelection(tag.class, tag.number))
		{
			if((sublen = asn1decode_MultipleSelection(der, out, tag.length)) < 0)
				return der_error("ListGroupClass");
			left -= tag.length;
		}
		else
		{
			return der_error("ListGroupClass: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	output_token(out, "}");

	if(left != 0)
		return der_error("ListGroupClass: %d bytes left", left);

	verbose("</ListGroupClass>\n");

	return length;
}

int asn1decode_Positions(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Positions>\n");

	output_token(out, ":Positions");

	output_token(out, "(");

	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("Positions");

		if(is_Position(tag.class, tag.number))
		{
			left -= sublen;
			if((sublen = asn1decode_Position(der, out, tag.length)) < 0)
				return der_error("Positions");
			left -= sublen;
		}
		else
		{
			return der_error("Positions: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("Positions: %d bytes left", left);

	verbose("</Positions>\n");

	return length;
}

int asn1decode_Position(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Position>\n");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Position");

	if(is_XYPosition(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_XYPosition(der, out, length)) < 0)
			return der_error("Position");
		left -= sublen;
	}
	else
	{
		return der_error("Position: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("Position: %d bytes left", left);

	verbose("</Position>\n");

	return length;
}

int asn1decode_WrapAround(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<WrapAround>\n");

	output_token(out, ":WrapAround");

	if((sublen = der_decode_BOOLEAN(der, out, length)) < 0)
		return der_error("WrapAround");
	left -= sublen;

	if(left != 0)
		return der_error("WrapAround: %d bytes left", left);

	verbose("</WrapAround>\n");

	return length;
}

int asn1decode_MultipleSelection(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<MultipleSelection>\n");

	output_token(out, ":MultipleSelection");

	if((sublen = der_decode_BOOLEAN(der, out, length)) < 0)
		return der_error("MultipleSelection");
	left -= sublen;

	if(left != 0)
		return der_error("MultipleSelection: %d bytes left", left);

	verbose("</MultipleSelection>\n");

	return length;
}

int asn1decode_Visible(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Visible>\n");

	/* SET */
	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("Visible");
		left -= sublen;
		if(is_Presentable(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_Presentable(der, out, sublen + tag.length)) < 0)
				return der_error("Visible");
			left -= tag.length;
		}
		else if(is_OriginalBoxSize(tag.class, tag.number))
		{
			if((sublen = asn1decode_OriginalBoxSize(der, out, tag.length)) < 0)
				return der_error("Visible");
			left -= tag.length;
		}
		else if(is_OriginalPosition(tag.class, tag.number))
		{
			if((sublen = asn1decode_OriginalPosition(der, out, tag.length)) < 0)
				return der_error("Visible");
			left -= tag.length;
		}
		else if(is_OriginalPaletteRef(tag.class, tag.number))
		{
			if((sublen = asn1decode_OriginalPaletteRef(der, out, tag.length)) < 0)
				return der_error("Visible");
			left -= tag.length;
		}
		else
		{
			return der_error("Visible: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	if(left != 0)
		return der_error("Visible: %d bytes left", left);

	verbose("</Visible>\n");

	return length;
}

int asn1decode_OriginalBoxSize(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<OriginalBoxSize>\n");

	output_token(out, ":OrigBoxSize");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("OriginalBoxSize");

	if(is_BoxSize(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_BoxSize(der, out, length)) < 0)
			return der_error("OriginalBoxSize");
		left -= sublen;
	}
	else
	{
		return der_error("OriginalBoxSize: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("OriginalBoxSize: %d bytes left", left);

	verbose("</OriginalBoxSize>\n");

	return length;
}

int asn1decode_BoxSize(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<BoxSize>\n");

	/* SEQUENCE */
	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("BoxSize");
	left -= sublen;

	if(is_XLength(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_XLength(der, out, sublen + tag.length)) < 0)
			return der_error("BoxSize");
		left -= tag.length;
	}
	else
	{
		return der_error("BoxSize: missing XLength");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("BoxSize");
	left -= sublen;

	if(is_YLength(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_YLength(der, out, sublen + tag.length)) < 0)
			return der_error("BoxSize");
		left -= tag.length;
	}
	else
	{
		return der_error("BoxSize: missing YLength");
	}

	if(left != 0)
		return der_error("BoxSize: %d bytes left", left);

	verbose("</BoxSize>\n");

	return length;
}

int asn1decode_XLength(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<XLength>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("XLength");

	if(is_INTEGER(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_INTEGER(der, out, length)) < 0)
			return der_error("XLength");
		left -= sublen;
	}
	else
	{
		return der_error("XLength: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("XLength: %d bytes left", left);

	verbose("</XLength>\n");

	return length;
}

int asn1decode_YLength(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<YLength>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("YLength");

	if(is_INTEGER(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_INTEGER(der, out, length)) < 0)
			return der_error("YLength");
		left -= sublen;
	}
	else
	{
		return der_error("YLength: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("YLength: %d bytes left", left);

	verbose("</YLength>\n");

	return length;
}

int asn1decode_OriginalPosition(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<OriginalPosition>\n");

	output_token(out, ":OrigPosition");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("OriginalPosition");

	if(is_XYPosition(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_XYPosition(der, out, length)) < 0)
			return der_error("OriginalPosition");
		left -= sublen;
	}
	else
	{
		return der_error("OriginalPosition: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("OriginalPosition: %d bytes left", left);

	verbose("</OriginalPosition>\n");

	return length;
}

int asn1decode_OriginalPaletteRef(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<OriginalPaletteRef>\n");

	output_token(out, ":OrigPaletteRef");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("OriginalPaletteRef");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("OriginalPaletteRef");
		left -= sublen;
	}
	else
	{
		return der_error("OriginalPaletteRef: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("OriginalPaletteRef: %d bytes left", left);

	verbose("</OriginalPaletteRef>\n");

	return length;
}

int asn1decode_BitmapClass(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<BitmapClass>\n");

	/* SET */
	output_token(out, "{:Bitmap");

	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("BitmapClass");
		left -= sublen;
		if(is_Visible(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_Visible(der, out, sublen + tag.length)) < 0)
				return der_error("BitmapClass");
			left -= tag.length;
		}
		else if(is_Tiling(tag.class, tag.number))
		{
			if((sublen = asn1decode_Tiling(der, out, tag.length)) < 0)
				return der_error("BitmapClass");
			left -= tag.length;
		}
		else if(is_OriginalTransparency(tag.class, tag.number))
		{
			if((sublen = asn1decode_OriginalTransparency(der, out, tag.length)) < 0)
				return der_error("BitmapClass");
			left -= tag.length;
		}
		else
		{
			return der_error("BitmapClass: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	output_token(out, "}");

	if(left != 0)
		return der_error("BitmapClass: %d bytes left", left);

	verbose("</BitmapClass>\n");

	return length;
}

int asn1decode_Tiling(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Tiling>\n");

	output_token(out, ":Tiling");

	if((sublen = der_decode_BOOLEAN(der, out, length)) < 0)
		return der_error("Tiling");
	left -= sublen;

	if(left != 0)
		return der_error("Tiling: %d bytes left", left);

	verbose("</Tiling>\n");

	return length;
}

int asn1decode_OriginalTransparency(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<OriginalTransparency>\n");

	output_token(out, ":OrigTransparency");

	if((sublen = der_decode_INTEGER(der, out, length)) < 0)
		return der_error("OriginalTransparency");
	left -= sublen;

	if(left != 0)
		return der_error("OriginalTransparency: %d bytes left", left);

	verbose("</OriginalTransparency>\n");

	return length;
}

int asn1decode_LineArtClass(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<LineArtClass>\n");

	output_token(out, "{:LineArt");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("LineArtClass");

	if(is_LineArtBody(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_LineArtBody(der, out, length)) < 0)
			return der_error("LineArtClass");
		left -= sublen;
	}
	else
	{
		return der_error("LineArtClass: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	output_token(out, "}");

	if(left != 0)
		return der_error("LineArtClass: %d bytes left", left);

	verbose("</LineArtClass>\n");

	return length;
}

int asn1decode_LineArtBody(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<LineArtBody>\n");

	/* SET */
	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("LineArtBody");
		left -= sublen;
		if(is_Visible(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_Visible(der, out, sublen + tag.length)) < 0)
				return der_error("LineArtBody");
			left -= tag.length;
		}
		else if(is_BorderedBoundingBox(tag.class, tag.number))
		{
			if((sublen = asn1decode_BorderedBoundingBox(der, out, tag.length)) < 0)
				return der_error("LineArtBody");
			left -= tag.length;
		}
		else if(is_OriginalLineWidth(tag.class, tag.number))
		{
			if((sublen = asn1decode_OriginalLineWidth(der, out, tag.length)) < 0)
				return der_error("LineArtBody");
			left -= tag.length;
		}
		else if(is_OriginalLineStyle(tag.class, tag.number))
		{
			if((sublen = asn1decode_OriginalLineStyle(der, out, tag.length)) < 0)
				return der_error("LineArtBody");
			left -= tag.length;
		}
		else if(is_OriginalRefLineColour(tag.class, tag.number))
		{
			if((sublen = asn1decode_OriginalRefLineColour(der, out, tag.length)) < 0)
				return der_error("LineArtBody");
			left -= tag.length;
		}
		else if(is_OriginalRefFillColour(tag.class, tag.number))
		{
			if((sublen = asn1decode_OriginalRefFillColour(der, out, tag.length)) < 0)
				return der_error("LineArtBody");
			left -= tag.length;
		}
		else
		{
			return der_error("LineArtBody: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	if(left != 0)
		return der_error("LineArtBody: %d bytes left", left);

	verbose("</LineArtBody>\n");

	return length;
}

int asn1decode_BorderedBoundingBox(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<BorderedBoundingBox>\n");

	output_token(out, ":BBBox");

	if((sublen = der_decode_BOOLEAN(der, out, length)) < 0)
		return der_error("BorderedBoundingBox");
	left -= sublen;

	if(left != 0)
		return der_error("BorderedBoundingBox: %d bytes left", left);

	verbose("</BorderedBoundingBox>\n");

	return length;
}

int asn1decode_OriginalLineWidth(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<OriginalLineWidth>\n");

	output_token(out, ":OrigLineWidth");

	if((sublen = der_decode_INTEGER(der, out, length)) < 0)
		return der_error("OriginalLineWidth");
	left -= sublen;

	if(left != 0)
		return der_error("OriginalLineWidth: %d bytes left", left);

	verbose("</OriginalLineWidth>\n");

	return length;
}

int asn1decode_OriginalLineStyle(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<OriginalLineStyle>\n");

	output_token(out, ":OrigLineStyle");

	if((sublen = der_decode_INTEGER(der, out, length)) < 0)
		return der_error("OriginalLineStyle");
	left -= sublen;

	if(left != 0)
		return der_error("OriginalLineStyle: %d bytes left", left);

	verbose("</OriginalLineStyle>\n");

	return length;
}

int asn1decode_OriginalRefLineColour(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<OriginalRefLineColour>\n");

	output_token(out, ":OrigRefLineColour");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("OriginalRefLineColour");

	if(is_Colour(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Colour(der, out, length)) < 0)
			return der_error("OriginalRefLineColour");
		left -= sublen;
	}
	else
	{
		return der_error("OriginalRefLineColour: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("OriginalRefLineColour: %d bytes left", left);

	verbose("</OriginalRefLineColour>\n");

	return length;
}

int asn1decode_OriginalRefFillColour(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<OriginalRefFillColour>\n");

	output_token(out, ":OrigRefFillColour");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("OriginalRefFillColour");

	if(is_Colour(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Colour(der, out, length)) < 0)
			return der_error("OriginalRefFillColour");
		left -= sublen;
	}
	else
	{
		return der_error("OriginalRefFillColour: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("OriginalRefFillColour: %d bytes left", left);

	verbose("</OriginalRefFillColour>\n");

	return length;
}

int asn1decode_RectangleClass(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<RectangleClass>\n");

	output_token(out, "{:Rectangle");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("RectangleClass");

	if(is_LineArtBody(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_LineArtBody(der, out, length)) < 0)
			return der_error("RectangleClass");
		left -= sublen;
	}
	else
	{
		return der_error("RectangleClass: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	output_token(out, "}");

	if(left != 0)
		return der_error("RectangleClass: %d bytes left", left);

	verbose("</RectangleClass>\n");

	return length;
}

int asn1decode_DynamicLineArtClass(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<DynamicLineArtClass>\n");

	output_token(out, "{:DynamicLineArt");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DynamicLineArtClass");

	if(is_LineArtBody(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_LineArtBody(der, out, length)) < 0)
			return der_error("DynamicLineArtClass");
		left -= sublen;
	}
	else
	{
		return der_error("DynamicLineArtClass: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	output_token(out, "}");

	if(left != 0)
		return der_error("DynamicLineArtClass: %d bytes left", left);

	verbose("</DynamicLineArtClass>\n");

	return length;
}

int asn1decode_TextClass(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<TextClass>\n");

	output_token(out, "{:Text");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("TextClass");

	if(is_TextBody(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_TextBody(der, out, length)) < 0)
			return der_error("TextClass");
		left -= sublen;
	}
	else
	{
		return der_error("TextClass: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	output_token(out, "}");

	if(left != 0)
		return der_error("TextClass: %d bytes left", left);

	verbose("</TextClass>\n");

	return length;
}

int asn1decode_TextBody(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<TextBody>\n");

	/* SET */
	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("TextBody");
		left -= sublen;
		if(is_Visible(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_Visible(der, out, sublen + tag.length)) < 0)
				return der_error("TextBody");
			left -= tag.length;
		}
		else if(is_OriginalFont(tag.class, tag.number))
		{
			if((sublen = asn1decode_OriginalFont(der, out, tag.length)) < 0)
				return der_error("TextBody");
			left -= tag.length;
		}
		else if(is_FontAttributes(tag.class, tag.number))
		{
			if((sublen = asn1decode_FontAttributes(der, out, tag.length)) < 0)
				return der_error("TextBody");
			left -= tag.length;
		}
		else if(is_TextColour(tag.class, tag.number))
		{
			if((sublen = asn1decode_TextColour(der, out, tag.length)) < 0)
				return der_error("TextBody");
			left -= tag.length;
		}
		else if(is_BackgroundColour(tag.class, tag.number))
		{
			if((sublen = asn1decode_BackgroundColour(der, out, tag.length)) < 0)
				return der_error("TextBody");
			left -= tag.length;
		}
		else if(is_CharacterSet(tag.class, tag.number))
		{
			if((sublen = asn1decode_CharacterSet(der, out, tag.length)) < 0)
				return der_error("TextBody");
			left -= tag.length;
		}
		else if(is_HorizontalJustification(tag.class, tag.number))
		{
			if((sublen = asn1decode_HorizontalJustification(der, out, tag.length)) < 0)
				return der_error("TextBody");
			left -= tag.length;
		}
		else if(is_VerticalJustification(tag.class, tag.number))
		{
			if((sublen = asn1decode_VerticalJustification(der, out, tag.length)) < 0)
				return der_error("TextBody");
			left -= tag.length;
		}
		else if(is_LineOrientation(tag.class, tag.number))
		{
			if((sublen = asn1decode_LineOrientation(der, out, tag.length)) < 0)
				return der_error("TextBody");
			left -= tag.length;
		}
		else if(is_StartCorner(tag.class, tag.number))
		{
			if((sublen = asn1decode_StartCorner(der, out, tag.length)) < 0)
				return der_error("TextBody");
			left -= tag.length;
		}
		else if(is_TextWrapping(tag.class, tag.number))
		{
			if((sublen = asn1decode_TextWrapping(der, out, tag.length)) < 0)
				return der_error("TextBody");
			left -= tag.length;
		}
		else
		{
			return der_error("TextBody: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	if(left != 0)
		return der_error("TextBody: %d bytes left", left);

	verbose("</TextBody>\n");

	return length;
}

int asn1decode_OriginalFont(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<OriginalFont>\n");

	output_token(out, ":OrigFont");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("OriginalFont");

	if(is_FontBody(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_FontBody(der, out, length)) < 0)
			return der_error("OriginalFont");
		left -= sublen;
	}
	else
	{
		return der_error("OriginalFont: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("OriginalFont: %d bytes left", left);

	verbose("</OriginalFont>\n");

	return length;
}

int asn1decode_HorizontalJustification(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<HorizontalJustification>\n");

	output_token(out, ":HJustification");

	if((sublen = der_decode_JustificationEnum(der, out, length)) < 0)
		return der_error("HorizontalJustification");
	left -= sublen;

	if(left != 0)
		return der_error("HorizontalJustification: %d bytes left", left);

	verbose("</HorizontalJustification>\n");

	return length;
}

int asn1decode_JustificationEnum(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* ENUMERATED */
	char *enum_names[] = {
		"start",
		"end",
		"centre",
		"justified",
	};

	/* keep the compiler happy */
	tag = tag;

	verbose("<JustificationEnum>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("JustificationEnum");
	left -= sublen;

	if(is_JustificationEnum(tag.class, tag.number))
	{
		if((sublen = der_decode_ENUMERATED(der, out, tag.length, 4, enum_names)) < 0)
			return der_error("JustificationEnum");
		left -= tag.length;
	}
	else
	{
		return der_error("JustificationEnum");
	}

	if(left != 0)
		return der_error("JustificationEnum: %d bytes left", left);

	verbose("</JustificationEnum>\n");

	return length;
}

int der_decode_JustificationEnum(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;

	/* ENUMERATED */
	char *enum_names[] = {
		"start",
		"end",
		"centre",
		"justified",
	};

	verbose("<JustificationEnum>\n");

	if((sublen = der_decode_ENUMERATED(der, out, length, 4, enum_names)) < 0)
		return der_error("JustificationEnum");
	left -= sublen;

	if(left != 0)
		return der_error("JustificationEnum: %d bytes left", left);

	verbose("</JustificationEnum>\n");

	return length;
}

int asn1decode_VerticalJustification(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<VerticalJustification>\n");

	output_token(out, ":VJustification");

	if((sublen = der_decode_JustificationEnum(der, out, length)) < 0)
		return der_error("VerticalJustification");
	left -= sublen;

	if(left != 0)
		return der_error("VerticalJustification: %d bytes left", left);

	verbose("</VerticalJustification>\n");

	return length;
}

int asn1decode_LineOrientation(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<LineOrientation>\n");

	output_token(out, ":LineOrientation");

	if((sublen = der_decode_LineOrientationEnum(der, out, length)) < 0)
		return der_error("LineOrientation");
	left -= sublen;

	if(left != 0)
		return der_error("LineOrientation: %d bytes left", left);

	verbose("</LineOrientation>\n");

	return length;
}

int asn1decode_LineOrientationEnum(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* ENUMERATED */
	char *enum_names[] = {
		"vertical",
		"horizontal",
	};

	/* keep the compiler happy */
	tag = tag;

	verbose("<LineOrientationEnum>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("LineOrientationEnum");
	left -= sublen;

	if(is_LineOrientationEnum(tag.class, tag.number))
	{
		if((sublen = der_decode_ENUMERATED(der, out, tag.length, 2, enum_names)) < 0)
			return der_error("LineOrientationEnum");
		left -= tag.length;
	}
	else
	{
		return der_error("LineOrientationEnum");
	}

	if(left != 0)
		return der_error("LineOrientationEnum: %d bytes left", left);

	verbose("</LineOrientationEnum>\n");

	return length;
}

int der_decode_LineOrientationEnum(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;

	/* ENUMERATED */
	char *enum_names[] = {
		"vertical",
		"horizontal",
	};

	verbose("<LineOrientationEnum>\n");

	if((sublen = der_decode_ENUMERATED(der, out, length, 2, enum_names)) < 0)
		return der_error("LineOrientationEnum");
	left -= sublen;

	if(left != 0)
		return der_error("LineOrientationEnum: %d bytes left", left);

	verbose("</LineOrientationEnum>\n");

	return length;
}

int asn1decode_StartCorner(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<StartCorner>\n");

	output_token(out, ":StartCorner");

	if((sublen = der_decode_StartCornerEnum(der, out, length)) < 0)
		return der_error("StartCorner");
	left -= sublen;

	if(left != 0)
		return der_error("StartCorner: %d bytes left", left);

	verbose("</StartCorner>\n");

	return length;
}

int asn1decode_StartCornerEnum(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* ENUMERATED */
	char *enum_names[] = {
		"upper-left",
		"upper-right",
		"lower-left",
		"lower-right",
	};

	/* keep the compiler happy */
	tag = tag;

	verbose("<StartCornerEnum>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("StartCornerEnum");
	left -= sublen;

	if(is_StartCornerEnum(tag.class, tag.number))
	{
		if((sublen = der_decode_ENUMERATED(der, out, tag.length, 4, enum_names)) < 0)
			return der_error("StartCornerEnum");
		left -= tag.length;
	}
	else
	{
		return der_error("StartCornerEnum");
	}

	if(left != 0)
		return der_error("StartCornerEnum: %d bytes left", left);

	verbose("</StartCornerEnum>\n");

	return length;
}

int der_decode_StartCornerEnum(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;

	/* ENUMERATED */
	char *enum_names[] = {
		"upper-left",
		"upper-right",
		"lower-left",
		"lower-right",
	};

	verbose("<StartCornerEnum>\n");

	if((sublen = der_decode_ENUMERATED(der, out, length, 4, enum_names)) < 0)
		return der_error("StartCornerEnum");
	left -= sublen;

	if(left != 0)
		return der_error("StartCornerEnum: %d bytes left", left);

	verbose("</StartCornerEnum>\n");

	return length;
}

int asn1decode_TextWrapping(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<TextWrapping>\n");

	output_token(out, ":TextWrapping");

	if((sublen = der_decode_BOOLEAN(der, out, length)) < 0)
		return der_error("TextWrapping");
	left -= sublen;

	if(left != 0)
		return der_error("TextWrapping: %d bytes left", left);

	verbose("</TextWrapping>\n");

	return length;
}

int asn1decode_StreamClass(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<StreamClass>\n");

	/* SET */
	output_token(out, "{:Stream");

	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("StreamClass");
		left -= sublen;
		if(is_Presentable(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_Presentable(der, out, sublen + tag.length)) < 0)
				return der_error("StreamClass");
			left -= tag.length;
		}
		else if(is_Multiplex(tag.class, tag.number))
		{
			if((sublen = asn1decode_Multiplex(der, out, tag.length)) < 0)
				return der_error("StreamClass");
			left -= tag.length;
		}
		else if(is_Storage(tag.class, tag.number))
		{
			if((sublen = asn1decode_Storage(der, out, tag.length)) < 0)
				return der_error("StreamClass");
			left -= tag.length;
		}
		else if(is_Looping(tag.class, tag.number))
		{
			if((sublen = asn1decode_Looping(der, out, tag.length)) < 0)
				return der_error("StreamClass");
			left -= tag.length;
		}
		else
		{
			return der_error("StreamClass: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	output_token(out, "}");

	if(left != 0)
		return der_error("StreamClass: %d bytes left", left);

	verbose("</StreamClass>\n");

	return length;
}

int asn1decode_Multiplex(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Multiplex>\n");

	output_token(out, ":Multiplex");

	output_token(out, "(");

	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("Multiplex");

		if(is_StreamComponent(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_StreamComponent(der, out, sublen + tag.length)) < 0)
				return der_error("Multiplex");
			left -= sublen;
		}
		else
		{
			return der_error("Multiplex: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("Multiplex: %d bytes left", left);

	verbose("</Multiplex>\n");

	return length;
}

int asn1decode_StreamComponent(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<StreamComponent>\n");

	/* CHOICE */
	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("StreamComponent");
	left -= sublen;

	if(is_AudioClass(tag.class, tag.number))
	{
		if((sublen = asn1decode_AudioClass(der, out, tag.length)) < 0)
			return der_error("StreamComponent");
		left -= tag.length;
	}
	else if(is_VideoClass(tag.class, tag.number))
	{
		if((sublen = asn1decode_VideoClass(der, out, tag.length)) < 0)
			return der_error("StreamComponent");
		left -= tag.length;
	}
	else if(is_RTGraphicsClass(tag.class, tag.number))
	{
		if((sublen = asn1decode_RTGraphicsClass(der, out, tag.length)) < 0)
			return der_error("StreamComponent");
		left -= tag.length;
	}
	else
	{
		return der_error("StreamComponent");
	}

	if(left != 0)
		return der_error("StreamComponent: %d bytes left", left);

	verbose("</StreamComponent>\n");

	return length;
}

int asn1decode_Storage(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Storage>\n");

	output_token(out, ":Storage");

	if((sublen = der_decode_StorageEnum(der, out, length)) < 0)
		return der_error("Storage");
	left -= sublen;

	if(left != 0)
		return der_error("Storage: %d bytes left", left);

	verbose("</Storage>\n");

	return length;
}

int asn1decode_StorageEnum(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* ENUMERATED */
	char *enum_names[] = {
		"memory",
		"stream",
	};

	/* keep the compiler happy */
	tag = tag;

	verbose("<StorageEnum>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("StorageEnum");
	left -= sublen;

	if(is_StorageEnum(tag.class, tag.number))
	{
		if((sublen = der_decode_ENUMERATED(der, out, tag.length, 2, enum_names)) < 0)
			return der_error("StorageEnum");
		left -= tag.length;
	}
	else
	{
		return der_error("StorageEnum");
	}

	if(left != 0)
		return der_error("StorageEnum: %d bytes left", left);

	verbose("</StorageEnum>\n");

	return length;
}

int der_decode_StorageEnum(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;

	/* ENUMERATED */
	char *enum_names[] = {
		"memory",
		"stream",
	};

	verbose("<StorageEnum>\n");

	if((sublen = der_decode_ENUMERATED(der, out, length, 2, enum_names)) < 0)
		return der_error("StorageEnum");
	left -= sublen;

	if(left != 0)
		return der_error("StorageEnum: %d bytes left", left);

	verbose("</StorageEnum>\n");

	return length;
}

int asn1decode_Looping(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Looping>\n");

	output_token(out, ":Looping");

	if((sublen = der_decode_INTEGER(der, out, length)) < 0)
		return der_error("Looping");
	left -= sublen;

	if(left != 0)
		return der_error("Looping: %d bytes left", left);

	verbose("</Looping>\n");

	return length;
}

int asn1decode_AudioClass(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<AudioClass>\n");

	/* SET */
	output_token(out, "{:Audio");

	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("AudioClass");
		left -= sublen;
		if(is_Presentable(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_Presentable(der, out, sublen + tag.length)) < 0)
				return der_error("AudioClass");
			left -= tag.length;
		}
		else if(is_ComponentTag(tag.class, tag.number))
		{
			if((sublen = asn1decode_ComponentTag(der, out, tag.length)) < 0)
				return der_error("AudioClass");
			left -= tag.length;
		}
		else if(is_OriginalVolume(tag.class, tag.number))
		{
			if((sublen = asn1decode_OriginalVolume(der, out, tag.length)) < 0)
				return der_error("AudioClass");
			left -= tag.length;
		}
		else
		{
			return der_error("AudioClass: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	output_token(out, "}");

	if(left != 0)
		return der_error("AudioClass: %d bytes left", left);

	verbose("</AudioClass>\n");

	return length;
}

int asn1decode_ComponentTag(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ComponentTag>\n");

	output_token(out, ":ComponentTag");

	if((sublen = der_decode_INTEGER(der, out, length)) < 0)
		return der_error("ComponentTag");
	left -= sublen;

	if(left != 0)
		return der_error("ComponentTag: %d bytes left", left);

	verbose("</ComponentTag>\n");

	return length;
}

int asn1decode_OriginalVolume(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<OriginalVolume>\n");

	output_token(out, ":OrigVolume");

	if((sublen = der_decode_INTEGER(der, out, length)) < 0)
		return der_error("OriginalVolume");
	left -= sublen;

	if(left != 0)
		return der_error("OriginalVolume: %d bytes left", left);

	verbose("</OriginalVolume>\n");

	return length;
}

int asn1decode_VideoClass(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<VideoClass>\n");

	/* SET */
	output_token(out, "{:Video");

	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("VideoClass");
		left -= sublen;
		if(is_Visible(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_Visible(der, out, sublen + tag.length)) < 0)
				return der_error("VideoClass");
			left -= tag.length;
		}
		else if(is_ComponentTag(tag.class, tag.number))
		{
			if((sublen = asn1decode_ComponentTag(der, out, tag.length)) < 0)
				return der_error("VideoClass");
			left -= tag.length;
		}
		else if(is_Termination(tag.class, tag.number))
		{
			if((sublen = asn1decode_Termination(der, out, tag.length)) < 0)
				return der_error("VideoClass");
			left -= tag.length;
		}
		else
		{
			return der_error("VideoClass: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	output_token(out, "}");

	if(left != 0)
		return der_error("VideoClass: %d bytes left", left);

	verbose("</VideoClass>\n");

	return length;
}

int asn1decode_Termination(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Termination>\n");

	output_token(out, ":Termination");

	if((sublen = der_decode_TerminationEnum(der, out, length)) < 0)
		return der_error("Termination");
	left -= sublen;

	if(left != 0)
		return der_error("Termination: %d bytes left", left);

	verbose("</Termination>\n");

	return length;
}

int asn1decode_TerminationEnum(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* ENUMERATED */
	char *enum_names[] = {
		"freeze",
		"disappear",
	};

	/* keep the compiler happy */
	tag = tag;

	verbose("<TerminationEnum>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("TerminationEnum");
	left -= sublen;

	if(is_TerminationEnum(tag.class, tag.number))
	{
		if((sublen = der_decode_ENUMERATED(der, out, tag.length, 2, enum_names)) < 0)
			return der_error("TerminationEnum");
		left -= tag.length;
	}
	else
	{
		return der_error("TerminationEnum");
	}

	if(left != 0)
		return der_error("TerminationEnum: %d bytes left", left);

	verbose("</TerminationEnum>\n");

	return length;
}

int der_decode_TerminationEnum(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;

	/* ENUMERATED */
	char *enum_names[] = {
		"freeze",
		"disappear",
	};

	verbose("<TerminationEnum>\n");

	if((sublen = der_decode_ENUMERATED(der, out, length, 2, enum_names)) < 0)
		return der_error("TerminationEnum");
	left -= sublen;

	if(left != 0)
		return der_error("TerminationEnum: %d bytes left", left);

	verbose("</TerminationEnum>\n");

	return length;
}

int asn1decode_RTGraphicsClass(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<RTGraphicsClass>\n");

	/* SET */
	output_token(out, "{:RTGraphics");

	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("RTGraphicsClass");
		left -= sublen;
		if(is_Visible(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_Visible(der, out, sublen + tag.length)) < 0)
				return der_error("RTGraphicsClass");
			left -= tag.length;
		}
		else if(is_ComponentTag(tag.class, tag.number))
		{
			if((sublen = asn1decode_ComponentTag(der, out, tag.length)) < 0)
				return der_error("RTGraphicsClass");
			left -= tag.length;
		}
		else if(is_Termination(tag.class, tag.number))
		{
			if((sublen = asn1decode_Termination(der, out, tag.length)) < 0)
				return der_error("RTGraphicsClass");
			left -= tag.length;
		}
		else
		{
			return der_error("RTGraphicsClass: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	output_token(out, "}");

	if(left != 0)
		return der_error("RTGraphicsClass: %d bytes left", left);

	verbose("</RTGraphicsClass>\n");

	return length;
}

int asn1decode_Interactible(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Interactible>\n");

	/* SET */
	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("Interactible");
		left -= sublen;
		if(is_EngineResp(tag.class, tag.number))
		{
			if((sublen = asn1decode_EngineResp(der, out, tag.length)) < 0)
				return der_error("Interactible");
			left -= tag.length;
		}
		else if(is_HighlightRefColour(tag.class, tag.number))
		{
			if((sublen = asn1decode_HighlightRefColour(der, out, tag.length)) < 0)
				return der_error("Interactible");
			left -= tag.length;
		}
		else
		{
			return der_error("Interactible: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	if(left != 0)
		return der_error("Interactible: %d bytes left", left);

	verbose("</Interactible>\n");

	return length;
}

int asn1decode_EngineResp(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<EngineResp>\n");

	output_token(out, ":EngineResp");

	if((sublen = der_decode_BOOLEAN(der, out, length)) < 0)
		return der_error("EngineResp");
	left -= sublen;

	if(left != 0)
		return der_error("EngineResp: %d bytes left", left);

	verbose("</EngineResp>\n");

	return length;
}

int asn1decode_SliderClass(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SliderClass>\n");

	/* SET */
	output_token(out, "{:Slider");

	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("SliderClass");
		left -= sublen;
		if(is_Visible(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_Visible(der, out, sublen + tag.length)) < 0)
				return der_error("SliderClass");
			left -= tag.length;
		}
		else if(is_Interactible(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_Interactible(der, out, sublen + tag.length)) < 0)
				return der_error("SliderClass");
			left -= tag.length;
		}
		else if(is_Orientation(tag.class, tag.number))
		{
			if((sublen = asn1decode_Orientation(der, out, tag.length)) < 0)
				return der_error("SliderClass");
			left -= tag.length;
		}
		else if(is_MaxValue(tag.class, tag.number))
		{
			if((sublen = asn1decode_MaxValue(der, out, tag.length)) < 0)
				return der_error("SliderClass");
			left -= tag.length;
		}
		else if(is_MinValue(tag.class, tag.number))
		{
			if((sublen = asn1decode_MinValue(der, out, tag.length)) < 0)
				return der_error("SliderClass");
			left -= tag.length;
		}
		else if(is_InitialValue(tag.class, tag.number))
		{
			if((sublen = asn1decode_InitialValue(der, out, tag.length)) < 0)
				return der_error("SliderClass");
			left -= tag.length;
		}
		else if(is_InitialPortion(tag.class, tag.number))
		{
			if((sublen = asn1decode_InitialPortion(der, out, tag.length)) < 0)
				return der_error("SliderClass");
			left -= tag.length;
		}
		else if(is_StepSize(tag.class, tag.number))
		{
			if((sublen = asn1decode_StepSize(der, out, tag.length)) < 0)
				return der_error("SliderClass");
			left -= tag.length;
		}
		else if(is_SliderStyle(tag.class, tag.number))
		{
			if((sublen = asn1decode_SliderStyle(der, out, tag.length)) < 0)
				return der_error("SliderClass");
			left -= tag.length;
		}
		else if(is_SliderRefColour(tag.class, tag.number))
		{
			if((sublen = asn1decode_SliderRefColour(der, out, tag.length)) < 0)
				return der_error("SliderClass");
			left -= tag.length;
		}
		else
		{
			return der_error("SliderClass: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	output_token(out, "}");

	if(left != 0)
		return der_error("SliderClass: %d bytes left", left);

	verbose("</SliderClass>\n");

	return length;
}

int asn1decode_Orientation(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Orientation>\n");

	output_token(out, ":Orientation");

	if((sublen = der_decode_OrientationEnum(der, out, length)) < 0)
		return der_error("Orientation");
	left -= sublen;

	if(left != 0)
		return der_error("Orientation: %d bytes left", left);

	verbose("</Orientation>\n");

	return length;
}

int asn1decode_OrientationEnum(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* ENUMERATED */
	char *enum_names[] = {
		"left",
		"right",
		"up",
		"down",
	};

	/* keep the compiler happy */
	tag = tag;

	verbose("<OrientationEnum>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("OrientationEnum");
	left -= sublen;

	if(is_OrientationEnum(tag.class, tag.number))
	{
		if((sublen = der_decode_ENUMERATED(der, out, tag.length, 4, enum_names)) < 0)
			return der_error("OrientationEnum");
		left -= tag.length;
	}
	else
	{
		return der_error("OrientationEnum");
	}

	if(left != 0)
		return der_error("OrientationEnum: %d bytes left", left);

	verbose("</OrientationEnum>\n");

	return length;
}

int der_decode_OrientationEnum(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;

	/* ENUMERATED */
	char *enum_names[] = {
		"left",
		"right",
		"up",
		"down",
	};

	verbose("<OrientationEnum>\n");

	if((sublen = der_decode_ENUMERATED(der, out, length, 4, enum_names)) < 0)
		return der_error("OrientationEnum");
	left -= sublen;

	if(left != 0)
		return der_error("OrientationEnum: %d bytes left", left);

	verbose("</OrientationEnum>\n");

	return length;
}

int asn1decode_MaxValue(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<MaxValue>\n");

	output_token(out, ":MaxValue");

	if((sublen = der_decode_INTEGER(der, out, length)) < 0)
		return der_error("MaxValue");
	left -= sublen;

	if(left != 0)
		return der_error("MaxValue: %d bytes left", left);

	verbose("</MaxValue>\n");

	return length;
}

int asn1decode_MinValue(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<MinValue>\n");

	output_token(out, ":MinValue");

	if((sublen = der_decode_INTEGER(der, out, length)) < 0)
		return der_error("MinValue");
	left -= sublen;

	if(left != 0)
		return der_error("MinValue: %d bytes left", left);

	verbose("</MinValue>\n");

	return length;
}

int asn1decode_InitialValue(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<InitialValue>\n");

	output_token(out, ":InitialValue");

	if((sublen = der_decode_INTEGER(der, out, length)) < 0)
		return der_error("InitialValue");
	left -= sublen;

	if(left != 0)
		return der_error("InitialValue: %d bytes left", left);

	verbose("</InitialValue>\n");

	return length;
}

int asn1decode_InitialPortion(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<InitialPortion>\n");

	output_token(out, ":InitialPortion");

	if((sublen = der_decode_INTEGER(der, out, length)) < 0)
		return der_error("InitialPortion");
	left -= sublen;

	if(left != 0)
		return der_error("InitialPortion: %d bytes left", left);

	verbose("</InitialPortion>\n");

	return length;
}

int asn1decode_StepSize(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<StepSize>\n");

	output_token(out, ":StepSize");

	if((sublen = der_decode_INTEGER(der, out, length)) < 0)
		return der_error("StepSize");
	left -= sublen;

	if(left != 0)
		return der_error("StepSize: %d bytes left", left);

	verbose("</StepSize>\n");

	return length;
}

int asn1decode_SliderStyle(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SliderStyle>\n");

	output_token(out, ":SliderStyle");

	if((sublen = der_decode_SliderStyleEnum(der, out, length)) < 0)
		return der_error("SliderStyle");
	left -= sublen;

	if(left != 0)
		return der_error("SliderStyle: %d bytes left", left);

	verbose("</SliderStyle>\n");

	return length;
}

int asn1decode_SliderStyleEnum(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* ENUMERATED */
	char *enum_names[] = {
		"normal",
		"thermometer",
		"proportional",
	};

	/* keep the compiler happy */
	tag = tag;

	verbose("<SliderStyleEnum>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SliderStyleEnum");
	left -= sublen;

	if(is_SliderStyleEnum(tag.class, tag.number))
	{
		if((sublen = der_decode_ENUMERATED(der, out, tag.length, 3, enum_names)) < 0)
			return der_error("SliderStyleEnum");
		left -= tag.length;
	}
	else
	{
		return der_error("SliderStyleEnum");
	}

	if(left != 0)
		return der_error("SliderStyleEnum: %d bytes left", left);

	verbose("</SliderStyleEnum>\n");

	return length;
}

int der_decode_SliderStyleEnum(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;

	/* ENUMERATED */
	char *enum_names[] = {
		"normal",
		"thermometer",
		"proportional",
	};

	verbose("<SliderStyleEnum>\n");

	if((sublen = der_decode_ENUMERATED(der, out, length, 3, enum_names)) < 0)
		return der_error("SliderStyleEnum");
	left -= sublen;

	if(left != 0)
		return der_error("SliderStyleEnum: %d bytes left", left);

	verbose("</SliderStyleEnum>\n");

	return length;
}

int asn1decode_EntryFieldClass(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<EntryFieldClass>\n");

	/* SET */
	output_token(out, "{:EntryField");

	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("EntryFieldClass");
		left -= sublen;
		if(is_TextBody(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_TextBody(der, out, sublen + tag.length)) < 0)
				return der_error("EntryFieldClass");
			left -= tag.length;
		}
		else if(is_Interactible(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_Interactible(der, out, sublen + tag.length)) < 0)
				return der_error("EntryFieldClass");
			left -= tag.length;
		}
		else if(is_InputType(tag.class, tag.number))
		{
			if((sublen = asn1decode_InputType(der, out, tag.length)) < 0)
				return der_error("EntryFieldClass");
			left -= tag.length;
		}
		else if(is_CharList(tag.class, tag.number))
		{
			if((sublen = asn1decode_CharList(der, out, tag.length)) < 0)
				return der_error("EntryFieldClass");
			left -= tag.length;
		}
		else if(is_ObscuredInput(tag.class, tag.number))
		{
			if((sublen = asn1decode_ObscuredInput(der, out, tag.length)) < 0)
				return der_error("EntryFieldClass");
			left -= tag.length;
		}
		else if(is_MaxLength(tag.class, tag.number))
		{
			if((sublen = asn1decode_MaxLength(der, out, tag.length)) < 0)
				return der_error("EntryFieldClass");
			left -= tag.length;
		}
		else
		{
			return der_error("EntryFieldClass: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	output_token(out, "}");

	if(left != 0)
		return der_error("EntryFieldClass: %d bytes left", left);

	verbose("</EntryFieldClass>\n");

	return length;
}

int asn1decode_InputType(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<InputType>\n");

	output_token(out, ":InputType");

	if((sublen = der_decode_InputTypeEnum(der, out, length)) < 0)
		return der_error("InputType");
	left -= sublen;

	if(left != 0)
		return der_error("InputType: %d bytes left", left);

	verbose("</InputType>\n");

	return length;
}

int asn1decode_InputTypeEnum(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* ENUMERATED */
	char *enum_names[] = {
		"alpha",
		"numeric",
		"any",
		"listed",
	};

	/* keep the compiler happy */
	tag = tag;

	verbose("<InputTypeEnum>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("InputTypeEnum");
	left -= sublen;

	if(is_InputTypeEnum(tag.class, tag.number))
	{
		if((sublen = der_decode_ENUMERATED(der, out, tag.length, 4, enum_names)) < 0)
			return der_error("InputTypeEnum");
		left -= tag.length;
	}
	else
	{
		return der_error("InputTypeEnum");
	}

	if(left != 0)
		return der_error("InputTypeEnum: %d bytes left", left);

	verbose("</InputTypeEnum>\n");

	return length;
}

int der_decode_InputTypeEnum(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;

	/* ENUMERATED */
	char *enum_names[] = {
		"alpha",
		"numeric",
		"any",
		"listed",
	};

	verbose("<InputTypeEnum>\n");

	if((sublen = der_decode_ENUMERATED(der, out, length, 4, enum_names)) < 0)
		return der_error("InputTypeEnum");
	left -= sublen;

	if(left != 0)
		return der_error("InputTypeEnum: %d bytes left", left);

	verbose("</InputTypeEnum>\n");

	return length;
}

int asn1decode_CharList(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<CharList>\n");

	output_token(out, ":CharList");

	if((sublen = der_decode_OctetString(der, out, length)) < 0)
		return der_error("CharList");
	left -= sublen;

	if(left != 0)
		return der_error("CharList: %d bytes left", left);

	verbose("</CharList>\n");

	return length;
}

int asn1decode_ObscuredInput(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ObscuredInput>\n");

	output_token(out, ":ObscuredInput");

	if((sublen = der_decode_BOOLEAN(der, out, length)) < 0)
		return der_error("ObscuredInput");
	left -= sublen;

	if(left != 0)
		return der_error("ObscuredInput: %d bytes left", left);

	verbose("</ObscuredInput>\n");

	return length;
}

int asn1decode_MaxLength(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<MaxLength>\n");

	output_token(out, ":MaxLength");

	if((sublen = der_decode_INTEGER(der, out, length)) < 0)
		return der_error("MaxLength");
	left -= sublen;

	if(left != 0)
		return der_error("MaxLength: %d bytes left", left);

	verbose("</MaxLength>\n");

	return length;
}

int asn1decode_HyperTextClass(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<HyperTextClass>\n");

	/* SET */
	output_token(out, "{:HyperText");

	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("HyperTextClass");
		left -= sublen;
		if(is_TextBody(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_TextBody(der, out, sublen + tag.length)) < 0)
				return der_error("HyperTextClass");
			left -= tag.length;
		}
		else if(is_Interactible(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_Interactible(der, out, sublen + tag.length)) < 0)
				return der_error("HyperTextClass");
			left -= tag.length;
		}
		else
		{
			return der_error("HyperTextClass: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	output_token(out, "}");

	if(left != 0)
		return der_error("HyperTextClass: %d bytes left", left);

	verbose("</HyperTextClass>\n");

	return length;
}

int asn1decode_Button(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Button>\n");

	/* SET */
	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("Button");
		left -= sublen;
		if(is_Visible(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_Visible(der, out, sublen + tag.length)) < 0)
				return der_error("Button");
			left -= tag.length;
		}
		else if(is_Interactible(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_Interactible(der, out, sublen + tag.length)) < 0)
				return der_error("Button");
			left -= tag.length;
		}
		else if(is_ButtonRefColour(tag.class, tag.number))
		{
			if((sublen = asn1decode_ButtonRefColour(der, out, tag.length)) < 0)
				return der_error("Button");
			left -= tag.length;
		}
		else
		{
			return der_error("Button: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	if(left != 0)
		return der_error("Button: %d bytes left", left);

	verbose("</Button>\n");

	return length;
}

int asn1decode_HotspotClass(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<HotspotClass>\n");

	output_token(out, "{:Hotspot");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("HotspotClass");

	if(is_Button(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Button(der, out, length)) < 0)
			return der_error("HotspotClass");
		left -= sublen;
	}
	else
	{
		return der_error("HotspotClass: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	output_token(out, "}");

	if(left != 0)
		return der_error("HotspotClass: %d bytes left", left);

	verbose("</HotspotClass>\n");

	return length;
}

int asn1decode_PushButtonClass(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<PushButtonClass>\n");

	output_token(out, "{:PushButton");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("PushButtonClass");

	if(is_PushButtonBody(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_PushButtonBody(der, out, length)) < 0)
			return der_error("PushButtonClass");
		left -= sublen;
	}
	else
	{
		return der_error("PushButtonClass: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	output_token(out, "}");

	if(left != 0)
		return der_error("PushButtonClass: %d bytes left", left);

	verbose("</PushButtonClass>\n");

	return length;
}

int asn1decode_PushButtonBody(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<PushButtonBody>\n");

	/* SET */
	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("PushButtonBody");
		left -= sublen;
		if(is_Button(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_Button(der, out, sublen + tag.length)) < 0)
				return der_error("PushButtonBody");
			left -= tag.length;
		}
		else if(is_OriginalLabel(tag.class, tag.number))
		{
			if((sublen = asn1decode_OriginalLabel(der, out, tag.length)) < 0)
				return der_error("PushButtonBody");
			left -= tag.length;
		}
		else if(is_CharacterSet(tag.class, tag.number))
		{
			if((sublen = asn1decode_CharacterSet(der, out, tag.length)) < 0)
				return der_error("PushButtonBody");
			left -= tag.length;
		}
		else
		{
			return der_error("PushButtonBody: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	if(left != 0)
		return der_error("PushButtonBody: %d bytes left", left);

	verbose("</PushButtonBody>\n");

	return length;
}

int asn1decode_OriginalLabel(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<OriginalLabel>\n");

	output_token(out, ":OrigLabel");

	if((sublen = der_decode_OctetString(der, out, length)) < 0)
		return der_error("OriginalLabel");
	left -= sublen;

	if(left != 0)
		return der_error("OriginalLabel: %d bytes left", left);

	verbose("</OriginalLabel>\n");

	return length;
}

int asn1decode_SwitchButtonClass(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SwitchButtonClass>\n");

	/* SET */
	output_token(out, "{:SwitchButton");

	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("SwitchButtonClass");
		left -= sublen;
		if(is_PushButtonBody(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_PushButtonBody(der, out, sublen + tag.length)) < 0)
				return der_error("SwitchButtonClass");
			left -= tag.length;
		}
		else if(is_ButtonStyle(tag.class, tag.number))
		{
			if((sublen = asn1decode_ButtonStyle(der, out, tag.length)) < 0)
				return der_error("SwitchButtonClass");
			left -= tag.length;
		}
		else
		{
			return der_error("SwitchButtonClass: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	output_token(out, "}");

	if(left != 0)
		return der_error("SwitchButtonClass: %d bytes left", left);

	verbose("</SwitchButtonClass>\n");

	return length;
}

int asn1decode_ButtonStyle(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ButtonStyle>\n");

	output_token(out, ":ButtonStyle");

	if((sublen = der_decode_ButtonStyleEnum(der, out, length)) < 0)
		return der_error("ButtonStyle");
	left -= sublen;

	if(left != 0)
		return der_error("ButtonStyle: %d bytes left", left);

	verbose("</ButtonStyle>\n");

	return length;
}

int asn1decode_ButtonStyleEnum(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* ENUMERATED */
	char *enum_names[] = {
		"pushbutton",
		"radiobutton",
		"checkbox",
	};

	/* keep the compiler happy */
	tag = tag;

	verbose("<ButtonStyleEnum>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ButtonStyleEnum");
	left -= sublen;

	if(is_ButtonStyleEnum(tag.class, tag.number))
	{
		if((sublen = der_decode_ENUMERATED(der, out, tag.length, 3, enum_names)) < 0)
			return der_error("ButtonStyleEnum");
		left -= tag.length;
	}
	else
	{
		return der_error("ButtonStyleEnum");
	}

	if(left != 0)
		return der_error("ButtonStyleEnum: %d bytes left", left);

	verbose("</ButtonStyleEnum>\n");

	return length;
}

int der_decode_ButtonStyleEnum(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;

	/* ENUMERATED */
	char *enum_names[] = {
		"pushbutton",
		"radiobutton",
		"checkbox",
	};

	verbose("<ButtonStyleEnum>\n");

	if((sublen = der_decode_ENUMERATED(der, out, length, 3, enum_names)) < 0)
		return der_error("ButtonStyleEnum");
	left -= sublen;

	if(left != 0)
		return der_error("ButtonStyleEnum: %d bytes left", left);

	verbose("</ButtonStyleEnum>\n");

	return length;
}

int asn1decode_ActionClass(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ActionClass>\n");

	output_token(out, "(");

	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("ActionClass");

		if(is_ElementaryAction(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_ElementaryAction(der, out, sublen + tag.length)) < 0)
				return der_error("ActionClass");
			left -= sublen;
		}
		else
		{
			return der_error("ActionClass: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("ActionClass: %d bytes left", left);

	verbose("</ActionClass>\n");

	return length;
}

int asn1decode_ElementaryAction(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ElementaryAction>\n");

	/* CHOICE */
	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ElementaryAction");
	left -= sublen;

	if(is_Activate(tag.class, tag.number))
	{
		if((sublen = asn1decode_Activate(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_Add(tag.class, tag.number))
	{
		if((sublen = asn1decode_Add(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_AddItem(tag.class, tag.number))
	{
		if((sublen = asn1decode_AddItem(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_Append(tag.class, tag.number))
	{
		if((sublen = asn1decode_Append(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_BringToFront(tag.class, tag.number))
	{
		if((sublen = asn1decode_BringToFront(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_Call(tag.class, tag.number))
	{
		if((sublen = asn1decode_Call(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_CallActionSlot(tag.class, tag.number))
	{
		if((sublen = asn1decode_CallActionSlot(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_Clear(tag.class, tag.number))
	{
		if((sublen = asn1decode_Clear(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_Clone(tag.class, tag.number))
	{
		if((sublen = asn1decode_Clone(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_CloseConnection(tag.class, tag.number))
	{
		if((sublen = asn1decode_CloseConnection(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_Deactivate(tag.class, tag.number))
	{
		if((sublen = asn1decode_Deactivate(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_DelItem(tag.class, tag.number))
	{
		if((sublen = asn1decode_DelItem(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_Deselect(tag.class, tag.number))
	{
		if((sublen = asn1decode_Deselect(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_DeselectItem(tag.class, tag.number))
	{
		if((sublen = asn1decode_DeselectItem(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_Divide(tag.class, tag.number))
	{
		if((sublen = asn1decode_Divide(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_DrawArc(tag.class, tag.number))
	{
		if((sublen = asn1decode_DrawArc(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_DrawLine(tag.class, tag.number))
	{
		if((sublen = asn1decode_DrawLine(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_DrawOval(tag.class, tag.number))
	{
		if((sublen = asn1decode_DrawOval(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_DrawPolygon(tag.class, tag.number))
	{
		if((sublen = asn1decode_DrawPolygon(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_DrawPolyline(tag.class, tag.number))
	{
		if((sublen = asn1decode_DrawPolyline(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_DrawRectangle(tag.class, tag.number))
	{
		if((sublen = asn1decode_DrawRectangle(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_DrawSector(tag.class, tag.number))
	{
		if((sublen = asn1decode_DrawSector(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_Fork(tag.class, tag.number))
	{
		if((sublen = asn1decode_Fork(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_GetAvailabilityStatus(tag.class, tag.number))
	{
		if((sublen = asn1decode_GetAvailabilityStatus(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_GetBoxSize(tag.class, tag.number))
	{
		if((sublen = asn1decode_GetBoxSize(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_GetCellItem(tag.class, tag.number))
	{
		if((sublen = asn1decode_GetCellItem(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_GetCursorPosition(tag.class, tag.number))
	{
		if((sublen = asn1decode_GetCursorPosition(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_GetEngineSupport(tag.class, tag.number))
	{
		if((sublen = asn1decode_GetEngineSupport(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_GetEntryPoint(tag.class, tag.number))
	{
		if((sublen = asn1decode_GetEntryPoint(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_GetFillColour(tag.class, tag.number))
	{
		if((sublen = asn1decode_GetFillColour(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_GetFirstItem(tag.class, tag.number))
	{
		if((sublen = asn1decode_GetFirstItem(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_GetHighlightStatus(tag.class, tag.number))
	{
		if((sublen = asn1decode_GetHighlightStatus(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_GetInteractionStatus(tag.class, tag.number))
	{
		if((sublen = asn1decode_GetInteractionStatus(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_GetItemStatus(tag.class, tag.number))
	{
		if((sublen = asn1decode_GetItemStatus(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_GetLabel(tag.class, tag.number))
	{
		if((sublen = asn1decode_GetLabel(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_GetLastAnchorFired(tag.class, tag.number))
	{
		if((sublen = asn1decode_GetLastAnchorFired(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_GetLineColour(tag.class, tag.number))
	{
		if((sublen = asn1decode_GetLineColour(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_GetLineStyle(tag.class, tag.number))
	{
		if((sublen = asn1decode_GetLineStyle(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_GetLineWidth(tag.class, tag.number))
	{
		if((sublen = asn1decode_GetLineWidth(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_GetListItem(tag.class, tag.number))
	{
		if((sublen = asn1decode_GetListItem(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_GetListSize(tag.class, tag.number))
	{
		if((sublen = asn1decode_GetListSize(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_GetOverwriteMode(tag.class, tag.number))
	{
		if((sublen = asn1decode_GetOverwriteMode(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_GetPortion(tag.class, tag.number))
	{
		if((sublen = asn1decode_GetPortion(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_GetPosition(tag.class, tag.number))
	{
		if((sublen = asn1decode_GetPosition(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_GetRunningStatus(tag.class, tag.number))
	{
		if((sublen = asn1decode_GetRunningStatus(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_GetSelectionStatus(tag.class, tag.number))
	{
		if((sublen = asn1decode_GetSelectionStatus(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_GetSliderValue(tag.class, tag.number))
	{
		if((sublen = asn1decode_GetSliderValue(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_GetTextContent(tag.class, tag.number))
	{
		if((sublen = asn1decode_GetTextContent(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_GetTextData(tag.class, tag.number))
	{
		if((sublen = asn1decode_GetTextData(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_GetTokenPosition(tag.class, tag.number))
	{
		if((sublen = asn1decode_GetTokenPosition(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_GetVolume(tag.class, tag.number))
	{
		if((sublen = asn1decode_GetVolume(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_Launch(tag.class, tag.number))
	{
		if((sublen = asn1decode_Launch(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_LockScreen(tag.class, tag.number))
	{
		if((sublen = asn1decode_LockScreen(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_Modulo(tag.class, tag.number))
	{
		if((sublen = asn1decode_Modulo(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_Move(tag.class, tag.number))
	{
		if((sublen = asn1decode_Move(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_MoveTo(tag.class, tag.number))
	{
		if((sublen = asn1decode_MoveTo(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_Multiply(tag.class, tag.number))
	{
		if((sublen = asn1decode_Multiply(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_OpenConnection(tag.class, tag.number))
	{
		if((sublen = asn1decode_OpenConnection(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_Preload(tag.class, tag.number))
	{
		if((sublen = asn1decode_Preload(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_PutBefore(tag.class, tag.number))
	{
		if((sublen = asn1decode_PutBefore(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_PutBehind(tag.class, tag.number))
	{
		if((sublen = asn1decode_PutBehind(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_Quit(tag.class, tag.number))
	{
		if((sublen = asn1decode_Quit(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_ReadPersistent(tag.class, tag.number))
	{
		if((sublen = asn1decode_ReadPersistent(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_Run(tag.class, tag.number))
	{
		if((sublen = asn1decode_Run(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_ScaleBitmap(tag.class, tag.number))
	{
		if((sublen = asn1decode_ScaleBitmap(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_ScaleVideo(tag.class, tag.number))
	{
		if((sublen = asn1decode_ScaleVideo(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_ScrollItems(tag.class, tag.number))
	{
		if((sublen = asn1decode_ScrollItems(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_Select(tag.class, tag.number))
	{
		if((sublen = asn1decode_Select(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SelectItem(tag.class, tag.number))
	{
		if((sublen = asn1decode_SelectItem(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SendEvent(tag.class, tag.number))
	{
		if((sublen = asn1decode_SendEvent(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SendToBack(tag.class, tag.number))
	{
		if((sublen = asn1decode_SendToBack(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetBoxSize(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetBoxSize(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetCachePriority(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetCachePriority(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetCounterEndPosition(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetCounterEndPosition(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetCounterPosition(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetCounterPosition(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetCounterTrigger(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetCounterTrigger(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetCursorPosition(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetCursorPosition(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetCursorShape(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetCursorShape(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetData(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetData(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetEntryPoint(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetEntryPoint(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetFillColour(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetFillColour(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetFirstItem(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetFirstItem(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetFontRef(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetFontRef(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetHighlightStatus(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetHighlightStatus(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetInteractionStatus(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetInteractionStatus(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetLabel(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetLabel(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetLineColour(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetLineColour(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetLineStyle(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetLineStyle(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetLineWidth(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetLineWidth(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetOverwriteMode(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetOverwriteMode(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetPaletteRef(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetPaletteRef(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetPortion(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetPortion(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetPosition(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetPosition(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetSliderValue(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetSliderValue(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetSpeed(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetSpeed(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetTimer(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetTimer(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetTransparency(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetTransparency(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetVariable(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetVariable(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetVolume(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetVolume(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_Spawn(tag.class, tag.number))
	{
		if((sublen = asn1decode_Spawn(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_Step(tag.class, tag.number))
	{
		if((sublen = asn1decode_Step(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_Stop(tag.class, tag.number))
	{
		if((sublen = asn1decode_Stop(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_StorePersistent(tag.class, tag.number))
	{
		if((sublen = asn1decode_StorePersistent(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_Subtract(tag.class, tag.number))
	{
		if((sublen = asn1decode_Subtract(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_TestVariable(tag.class, tag.number))
	{
		if((sublen = asn1decode_TestVariable(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_Toggle(tag.class, tag.number))
	{
		if((sublen = asn1decode_Toggle(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_ToggleItem(tag.class, tag.number))
	{
		if((sublen = asn1decode_ToggleItem(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_TransitionTo(tag.class, tag.number))
	{
		if((sublen = asn1decode_TransitionTo(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_Unload(tag.class, tag.number))
	{
		if((sublen = asn1decode_Unload(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_UnlockScreen(tag.class, tag.number))
	{
		if((sublen = asn1decode_UnlockScreen(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetBackgroundColour(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetBackgroundColour(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetCellPosition(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetCellPosition(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetInputReg(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetInputReg(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetTextColour(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetTextColour(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetFontAttributes(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetFontAttributes(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetVideoDecodeOffset(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetVideoDecodeOffset(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_GetVideoDecodeOffset(tag.class, tag.number))
	{
		if((sublen = asn1decode_GetVideoDecodeOffset(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_GetFocusPosition(tag.class, tag.number))
	{
		if((sublen = asn1decode_GetFocusPosition(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetFocusPosition(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetFocusPosition(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetBitmapDecodeOffset(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetBitmapDecodeOffset(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_GetBitmapDecodeOffset(tag.class, tag.number))
	{
		if((sublen = asn1decode_GetBitmapDecodeOffset(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else if(is_SetSliderParameters(tag.class, tag.number))
	{
		if((sublen = asn1decode_SetSliderParameters(der, out, tag.length)) < 0)
			return der_error("ElementaryAction");
		left -= tag.length;
	}
	else
	{
		return der_error("ElementaryAction");
	}

	if(left != 0)
		return der_error("ElementaryAction: %d bytes left", left);

	verbose("</ElementaryAction>\n");

	return length;
}

int asn1decode_Activate(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Activate>\n");

	output_token(out, ":Activate");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Activate");

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, length)) < 0)
			return der_error("Activate");
		left -= sublen;
	}
	else
	{
		return der_error("Activate: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("Activate: %d bytes left", left);

	verbose("</Activate>\n");

	return length;
}

int asn1decode_Add(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Add>\n");

	/* SEQUENCE */
	output_token(out, ":Add");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Add");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("Add");
		left -= tag.length;
	}
	else
	{
		return der_error("Add: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Add");
	left -= sublen;

	if(is_Value(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Value(der, out, sublen + tag.length)) < 0)
			return der_error("Add");
		left -= tag.length;
	}
	else
	{
		return der_error("Add: missing Value");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("Add: %d bytes left", left);

	verbose("</Add>\n");

	return length;
}

int asn1decode_AddItem(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<AddItem>\n");

	/* SEQUENCE */
	output_token(out, ":AddItem");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("AddItem");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("AddItem");
		left -= tag.length;
	}
	else
	{
		return der_error("AddItem: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("AddItem");
	left -= sublen;

	if(is_ItemIndex(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ItemIndex(der, out, sublen + tag.length)) < 0)
			return der_error("AddItem");
		left -= tag.length;
	}
	else
	{
		return der_error("AddItem: missing ItemIndex");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("AddItem");
	left -= sublen;

	if(is_VisibleReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_VisibleReference(der, out, sublen + tag.length)) < 0)
			return der_error("AddItem");
		left -= tag.length;
	}
	else
	{
		return der_error("AddItem: missing VisibleReference");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("AddItem: %d bytes left", left);

	verbose("</AddItem>\n");

	return length;
}

int asn1decode_Append(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Append>\n");

	/* SEQUENCE */
	output_token(out, ":Append");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Append");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("Append");
		left -= tag.length;
	}
	else
	{
		return der_error("Append: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Append");
	left -= sublen;

	if(is_AppendValue(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_AppendValue(der, out, sublen + tag.length)) < 0)
			return der_error("Append");
		left -= tag.length;
	}
	else
	{
		return der_error("Append: missing AppendValue");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("Append: %d bytes left", left);

	verbose("</Append>\n");

	return length;
}

int asn1decode_BringToFront(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<BringToFront>\n");

	output_token(out, ":BringToFront");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("BringToFront");

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, length)) < 0)
			return der_error("BringToFront");
		left -= sublen;
	}
	else
	{
		return der_error("BringToFront: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("BringToFront: %d bytes left", left);

	verbose("</BringToFront>\n");

	return length;
}

int asn1decode_Call(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Call>\n");

	/* SEQUENCE */
	output_token(out, ":Call");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Call");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("Call");
		left -= tag.length;
	}
	else
	{
		return der_error("Call: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Call");
	left -= sublen;

	if(is_CallSucceeded(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_CallSucceeded(der, out, sublen + tag.length)) < 0)
			return der_error("Call");
		left -= tag.length;
	}
	else
	{
		return der_error("Call: missing CallSucceeded");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Call");
	left -= sublen;

	if(is_Parameters(tag.class, tag.number))
	{
		if((sublen = asn1decode_Parameters(der, out, tag.length)) < 0)
			return der_error("Call");
		left -= tag.length;
	}
	else
	{
		fseek(der, -sublen, SEEK_CUR);
		left += sublen;
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("Call: %d bytes left", left);

	verbose("</Call>\n");

	return length;
}

int asn1decode_CallActionSlot(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<CallActionSlot>\n");

	/* SEQUENCE */
	output_token(out, ":CallActionSlot");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("CallActionSlot");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("CallActionSlot");
		left -= tag.length;
	}
	else
	{
		return der_error("CallActionSlot: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("CallActionSlot");
	left -= sublen;

	if(is_Index(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Index(der, out, sublen + tag.length)) < 0)
			return der_error("CallActionSlot");
		left -= tag.length;
	}
	else
	{
		return der_error("CallActionSlot: missing Index");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("CallActionSlot: %d bytes left", left);

	verbose("</CallActionSlot>\n");

	return length;
}

int asn1decode_Clear(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Clear>\n");

	output_token(out, ":Clear");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Clear");

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, length)) < 0)
			return der_error("Clear");
		left -= sublen;
	}
	else
	{
		return der_error("Clear: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("Clear: %d bytes left", left);

	verbose("</Clear>\n");

	return length;
}

int asn1decode_Clone(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Clone>\n");

	/* SEQUENCE */
	output_token(out, ":Clone");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Clone");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("Clone");
		left -= tag.length;
	}
	else
	{
		return der_error("Clone: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Clone");
	left -= sublen;

	if(is_CloneRefVar(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_CloneRefVar(der, out, sublen + tag.length)) < 0)
			return der_error("Clone");
		left -= tag.length;
	}
	else
	{
		return der_error("Clone: missing CloneRefVar");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("Clone: %d bytes left", left);

	verbose("</Clone>\n");

	return length;
}

int asn1decode_CloseConnection(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<CloseConnection>\n");

	/* SEQUENCE */
	output_token(out, ":CloseConnection");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("CloseConnection");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("CloseConnection");
		left -= tag.length;
	}
	else
	{
		return der_error("CloseConnection: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("CloseConnection");
	left -= sublen;

	if(is_ConnectionTag(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ConnectionTag(der, out, sublen + tag.length)) < 0)
			return der_error("CloseConnection");
		left -= tag.length;
	}
	else
	{
		return der_error("CloseConnection: missing ConnectionTag");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("CloseConnection: %d bytes left", left);

	verbose("</CloseConnection>\n");

	return length;
}

int asn1decode_Deactivate(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Deactivate>\n");

	output_token(out, ":Deactivate");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Deactivate");

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, length)) < 0)
			return der_error("Deactivate");
		left -= sublen;
	}
	else
	{
		return der_error("Deactivate: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("Deactivate: %d bytes left", left);

	verbose("</Deactivate>\n");

	return length;
}

int asn1decode_DelItem(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<DelItem>\n");

	/* SEQUENCE */
	output_token(out, ":DelItem");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DelItem");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("DelItem");
		left -= tag.length;
	}
	else
	{
		return der_error("DelItem: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DelItem");
	left -= sublen;

	if(is_VisibleReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_VisibleReference(der, out, sublen + tag.length)) < 0)
			return der_error("DelItem");
		left -= tag.length;
	}
	else
	{
		return der_error("DelItem: missing VisibleReference");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("DelItem: %d bytes left", left);

	verbose("</DelItem>\n");

	return length;
}

int asn1decode_Deselect(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Deselect>\n");

	output_token(out, ":Deselect");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Deselect");

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, length)) < 0)
			return der_error("Deselect");
		left -= sublen;
	}
	else
	{
		return der_error("Deselect: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("Deselect: %d bytes left", left);

	verbose("</Deselect>\n");

	return length;
}

int asn1decode_DeselectItem(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<DeselectItem>\n");

	/* SEQUENCE */
	output_token(out, ":DeselectItem");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DeselectItem");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("DeselectItem");
		left -= tag.length;
	}
	else
	{
		return der_error("DeselectItem: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DeselectItem");
	left -= sublen;

	if(is_ItemIndex(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ItemIndex(der, out, sublen + tag.length)) < 0)
			return der_error("DeselectItem");
		left -= tag.length;
	}
	else
	{
		return der_error("DeselectItem: missing ItemIndex");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("DeselectItem: %d bytes left", left);

	verbose("</DeselectItem>\n");

	return length;
}

int asn1decode_Divide(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Divide>\n");

	/* SEQUENCE */
	output_token(out, ":Divide");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Divide");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("Divide");
		left -= tag.length;
	}
	else
	{
		return der_error("Divide: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Divide");
	left -= sublen;

	if(is_Value(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Value(der, out, sublen + tag.length)) < 0)
			return der_error("Divide");
		left -= tag.length;
	}
	else
	{
		return der_error("Divide: missing Value");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("Divide: %d bytes left", left);

	verbose("</Divide>\n");

	return length;
}

int asn1decode_DrawArc(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<DrawArc>\n");

	/* SEQUENCE */
	output_token(out, ":DrawArc");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DrawArc");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("DrawArc");
		left -= tag.length;
	}
	else
	{
		return der_error("DrawArc: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DrawArc");
	left -= sublen;

	if(is_X(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_X(der, out, sublen + tag.length)) < 0)
			return der_error("DrawArc");
		left -= tag.length;
	}
	else
	{
		return der_error("DrawArc: missing X");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DrawArc");
	left -= sublen;

	if(is_Y(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Y(der, out, sublen + tag.length)) < 0)
			return der_error("DrawArc");
		left -= tag.length;
	}
	else
	{
		return der_error("DrawArc: missing Y");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DrawArc");
	left -= sublen;

	if(is_EllipseWidth(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_EllipseWidth(der, out, sublen + tag.length)) < 0)
			return der_error("DrawArc");
		left -= tag.length;
	}
	else
	{
		return der_error("DrawArc: missing EllipseWidth");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DrawArc");
	left -= sublen;

	if(is_EllipseHeight(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_EllipseHeight(der, out, sublen + tag.length)) < 0)
			return der_error("DrawArc");
		left -= tag.length;
	}
	else
	{
		return der_error("DrawArc: missing EllipseHeight");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DrawArc");
	left -= sublen;

	if(is_StartAngle(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_StartAngle(der, out, sublen + tag.length)) < 0)
			return der_error("DrawArc");
		left -= tag.length;
	}
	else
	{
		return der_error("DrawArc: missing StartAngle");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DrawArc");
	left -= sublen;

	if(is_ArcAngle(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ArcAngle(der, out, sublen + tag.length)) < 0)
			return der_error("DrawArc");
		left -= tag.length;
	}
	else
	{
		return der_error("DrawArc: missing ArcAngle");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("DrawArc: %d bytes left", left);

	verbose("</DrawArc>\n");

	return length;
}

int asn1decode_DrawLine(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<DrawLine>\n");

	/* SEQUENCE */
	output_token(out, ":DrawLine");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DrawLine");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("DrawLine");
		left -= tag.length;
	}
	else
	{
		return der_error("DrawLine: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DrawLine");
	left -= sublen;

	if(is_X1(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_X1(der, out, sublen + tag.length)) < 0)
			return der_error("DrawLine");
		left -= tag.length;
	}
	else
	{
		return der_error("DrawLine: missing X1");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DrawLine");
	left -= sublen;

	if(is_Y1(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Y1(der, out, sublen + tag.length)) < 0)
			return der_error("DrawLine");
		left -= tag.length;
	}
	else
	{
		return der_error("DrawLine: missing Y1");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DrawLine");
	left -= sublen;

	if(is_X2(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_X2(der, out, sublen + tag.length)) < 0)
			return der_error("DrawLine");
		left -= tag.length;
	}
	else
	{
		return der_error("DrawLine: missing X2");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DrawLine");
	left -= sublen;

	if(is_Y2(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Y2(der, out, sublen + tag.length)) < 0)
			return der_error("DrawLine");
		left -= tag.length;
	}
	else
	{
		return der_error("DrawLine: missing Y2");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("DrawLine: %d bytes left", left);

	verbose("</DrawLine>\n");

	return length;
}

int asn1decode_DrawOval(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<DrawOval>\n");

	/* SEQUENCE */
	output_token(out, ":DrawOval");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DrawOval");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("DrawOval");
		left -= tag.length;
	}
	else
	{
		return der_error("DrawOval: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DrawOval");
	left -= sublen;

	if(is_X(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_X(der, out, sublen + tag.length)) < 0)
			return der_error("DrawOval");
		left -= tag.length;
	}
	else
	{
		return der_error("DrawOval: missing X");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DrawOval");
	left -= sublen;

	if(is_Y(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Y(der, out, sublen + tag.length)) < 0)
			return der_error("DrawOval");
		left -= tag.length;
	}
	else
	{
		return der_error("DrawOval: missing Y");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DrawOval");
	left -= sublen;

	if(is_EllipseWidth(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_EllipseWidth(der, out, sublen + tag.length)) < 0)
			return der_error("DrawOval");
		left -= tag.length;
	}
	else
	{
		return der_error("DrawOval: missing EllipseWidth");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DrawOval");
	left -= sublen;

	if(is_EllipseHeight(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_EllipseHeight(der, out, sublen + tag.length)) < 0)
			return der_error("DrawOval");
		left -= tag.length;
	}
	else
	{
		return der_error("DrawOval: missing EllipseHeight");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("DrawOval: %d bytes left", left);

	verbose("</DrawOval>\n");

	return length;
}

int asn1decode_DrawPolygon(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<DrawPolygon>\n");

	/* SEQUENCE */
	output_token(out, ":DrawPolygon");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DrawPolygon");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("DrawPolygon");
		left -= tag.length;
	}
	else
	{
		return der_error("DrawPolygon: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DrawPolygon");
	left -= sublen;

	if(is_PointList(tag.class, tag.number))
	{
		if((sublen = asn1decode_PointList(der, out, tag.length)) < 0)
			return der_error("DrawPolygon");
		left -= tag.length;
	}
	else
	{
		return der_error("DrawPolygon: missing PointList");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("DrawPolygon: %d bytes left", left);

	verbose("</DrawPolygon>\n");

	return length;
}

int asn1decode_DrawPolyline(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<DrawPolyline>\n");

	/* SEQUENCE */
	output_token(out, ":DrawPolyline");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DrawPolyline");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("DrawPolyline");
		left -= tag.length;
	}
	else
	{
		return der_error("DrawPolyline: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DrawPolyline");
	left -= sublen;

	if(is_PointList(tag.class, tag.number))
	{
		if((sublen = asn1decode_PointList(der, out, tag.length)) < 0)
			return der_error("DrawPolyline");
		left -= tag.length;
	}
	else
	{
		return der_error("DrawPolyline: missing PointList");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("DrawPolyline: %d bytes left", left);

	verbose("</DrawPolyline>\n");

	return length;
}

int asn1decode_DrawRectangle(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<DrawRectangle>\n");

	/* SEQUENCE */
	output_token(out, ":DrawRectangle");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DrawRectangle");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("DrawRectangle");
		left -= tag.length;
	}
	else
	{
		return der_error("DrawRectangle: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DrawRectangle");
	left -= sublen;

	if(is_X1(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_X1(der, out, sublen + tag.length)) < 0)
			return der_error("DrawRectangle");
		left -= tag.length;
	}
	else
	{
		return der_error("DrawRectangle: missing X1");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DrawRectangle");
	left -= sublen;

	if(is_Y1(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Y1(der, out, sublen + tag.length)) < 0)
			return der_error("DrawRectangle");
		left -= tag.length;
	}
	else
	{
		return der_error("DrawRectangle: missing Y1");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DrawRectangle");
	left -= sublen;

	if(is_X2(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_X2(der, out, sublen + tag.length)) < 0)
			return der_error("DrawRectangle");
		left -= tag.length;
	}
	else
	{
		return der_error("DrawRectangle: missing X2");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DrawRectangle");
	left -= sublen;

	if(is_Y2(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Y2(der, out, sublen + tag.length)) < 0)
			return der_error("DrawRectangle");
		left -= tag.length;
	}
	else
	{
		return der_error("DrawRectangle: missing Y2");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("DrawRectangle: %d bytes left", left);

	verbose("</DrawRectangle>\n");

	return length;
}

int asn1decode_DrawSector(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<DrawSector>\n");

	/* SEQUENCE */
	output_token(out, ":DrawSector");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DrawSector");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("DrawSector");
		left -= tag.length;
	}
	else
	{
		return der_error("DrawSector: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DrawSector");
	left -= sublen;

	if(is_X(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_X(der, out, sublen + tag.length)) < 0)
			return der_error("DrawSector");
		left -= tag.length;
	}
	else
	{
		return der_error("DrawSector: missing X");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DrawSector");
	left -= sublen;

	if(is_Y(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Y(der, out, sublen + tag.length)) < 0)
			return der_error("DrawSector");
		left -= tag.length;
	}
	else
	{
		return der_error("DrawSector: missing Y");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DrawSector");
	left -= sublen;

	if(is_EllipseWidth(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_EllipseWidth(der, out, sublen + tag.length)) < 0)
			return der_error("DrawSector");
		left -= tag.length;
	}
	else
	{
		return der_error("DrawSector: missing EllipseWidth");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DrawSector");
	left -= sublen;

	if(is_EllipseHeight(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_EllipseHeight(der, out, sublen + tag.length)) < 0)
			return der_error("DrawSector");
		left -= tag.length;
	}
	else
	{
		return der_error("DrawSector: missing EllipseHeight");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DrawSector");
	left -= sublen;

	if(is_StartAngle(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_StartAngle(der, out, sublen + tag.length)) < 0)
			return der_error("DrawSector");
		left -= tag.length;
	}
	else
	{
		return der_error("DrawSector: missing StartAngle");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DrawSector");
	left -= sublen;

	if(is_ArcAngle(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ArcAngle(der, out, sublen + tag.length)) < 0)
			return der_error("DrawSector");
		left -= tag.length;
	}
	else
	{
		return der_error("DrawSector: missing ArcAngle");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("DrawSector: %d bytes left", left);

	verbose("</DrawSector>\n");

	return length;
}

int asn1decode_Fork(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Fork>\n");

	/* SEQUENCE */
	output_token(out, ":Fork");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Fork");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("Fork");
		left -= tag.length;
	}
	else
	{
		return der_error("Fork: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Fork");
	left -= sublen;

	if(is_ForkSucceeded(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ForkSucceeded(der, out, sublen + tag.length)) < 0)
			return der_error("Fork");
		left -= tag.length;
	}
	else
	{
		return der_error("Fork: missing ForkSucceeded");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Fork");
	left -= sublen;

	if(is_Parameters(tag.class, tag.number))
	{
		if((sublen = asn1decode_Parameters(der, out, tag.length)) < 0)
			return der_error("Fork");
		left -= tag.length;
	}
	else
	{
		fseek(der, -sublen, SEEK_CUR);
		left += sublen;
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("Fork: %d bytes left", left);

	verbose("</Fork>\n");

	return length;
}

int asn1decode_GetAvailabilityStatus(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GetAvailabilityStatus>\n");

	/* SEQUENCE */
	output_token(out, ":GetAvailabilityStatus");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetAvailabilityStatus");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("GetAvailabilityStatus");
		left -= tag.length;
	}
	else
	{
		return der_error("GetAvailabilityStatus: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetAvailabilityStatus");
	left -= sublen;

	if(is_AvailabilityStatusVar(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_AvailabilityStatusVar(der, out, sublen + tag.length)) < 0)
			return der_error("GetAvailabilityStatus");
		left -= tag.length;
	}
	else
	{
		return der_error("GetAvailabilityStatus: missing AvailabilityStatusVar");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("GetAvailabilityStatus: %d bytes left", left);

	verbose("</GetAvailabilityStatus>\n");

	return length;
}

int asn1decode_GetBoxSize(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GetBoxSize>\n");

	/* SEQUENCE */
	output_token(out, ":GetBoxSize");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetBoxSize");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("GetBoxSize");
		left -= tag.length;
	}
	else
	{
		return der_error("GetBoxSize: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetBoxSize");
	left -= sublen;

	if(is_XBoxSizeVar(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_XBoxSizeVar(der, out, sublen + tag.length)) < 0)
			return der_error("GetBoxSize");
		left -= tag.length;
	}
	else
	{
		return der_error("GetBoxSize: missing XBoxSizeVar");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetBoxSize");
	left -= sublen;

	if(is_YBoxSizeVar(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_YBoxSizeVar(der, out, sublen + tag.length)) < 0)
			return der_error("GetBoxSize");
		left -= tag.length;
	}
	else
	{
		return der_error("GetBoxSize: missing YBoxSizeVar");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("GetBoxSize: %d bytes left", left);

	verbose("</GetBoxSize>\n");

	return length;
}

int asn1decode_GetCellItem(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GetCellItem>\n");

	/* SEQUENCE */
	output_token(out, ":GetCellItem");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetCellItem");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("GetCellItem");
		left -= tag.length;
	}
	else
	{
		return der_error("GetCellItem: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetCellItem");
	left -= sublen;

	if(is_CellIndex(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_CellIndex(der, out, sublen + tag.length)) < 0)
			return der_error("GetCellItem");
		left -= tag.length;
	}
	else
	{
		return der_error("GetCellItem: missing CellIndex");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetCellItem");
	left -= sublen;

	if(is_ItemRefVar(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ItemRefVar(der, out, sublen + tag.length)) < 0)
			return der_error("GetCellItem");
		left -= tag.length;
	}
	else
	{
		return der_error("GetCellItem: missing ItemRefVar");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("GetCellItem: %d bytes left", left);

	verbose("</GetCellItem>\n");

	return length;
}

int asn1decode_GetCursorPosition(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GetCursorPosition>\n");

	/* SEQUENCE */
	output_token(out, ":GetCursorPosition");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetCursorPosition");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("GetCursorPosition");
		left -= tag.length;
	}
	else
	{
		return der_error("GetCursorPosition: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetCursorPosition");
	left -= sublen;

	if(is_XOut(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_XOut(der, out, sublen + tag.length)) < 0)
			return der_error("GetCursorPosition");
		left -= tag.length;
	}
	else
	{
		return der_error("GetCursorPosition: missing XOut");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetCursorPosition");
	left -= sublen;

	if(is_YOut(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_YOut(der, out, sublen + tag.length)) < 0)
			return der_error("GetCursorPosition");
		left -= tag.length;
	}
	else
	{
		return der_error("GetCursorPosition: missing YOut");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("GetCursorPosition: %d bytes left", left);

	verbose("</GetCursorPosition>\n");

	return length;
}

int asn1decode_GetEngineSupport(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GetEngineSupport>\n");

	/* SEQUENCE */
	output_token(out, ":GetEngineSupport");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetEngineSupport");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("GetEngineSupport");
		left -= tag.length;
	}
	else
	{
		return der_error("GetEngineSupport: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetEngineSupport");
	left -= sublen;

	if(is_Feature(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Feature(der, out, sublen + tag.length)) < 0)
			return der_error("GetEngineSupport");
		left -= tag.length;
	}
	else
	{
		return der_error("GetEngineSupport: missing Feature");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetEngineSupport");
	left -= sublen;

	if(is_Answer(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Answer(der, out, sublen + tag.length)) < 0)
			return der_error("GetEngineSupport");
		left -= tag.length;
	}
	else
	{
		return der_error("GetEngineSupport: missing Answer");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("GetEngineSupport: %d bytes left", left);

	verbose("</GetEngineSupport>\n");

	return length;
}

int asn1decode_GetEntryPoint(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GetEntryPoint>\n");

	/* SEQUENCE */
	output_token(out, ":GetEntryPoint");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetEntryPoint");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("GetEntryPoint");
		left -= tag.length;
	}
	else
	{
		return der_error("GetEntryPoint: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetEntryPoint");
	left -= sublen;

	if(is_EntryPointVar(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_EntryPointVar(der, out, sublen + tag.length)) < 0)
			return der_error("GetEntryPoint");
		left -= tag.length;
	}
	else
	{
		return der_error("GetEntryPoint: missing EntryPointVar");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("GetEntryPoint: %d bytes left", left);

	verbose("</GetEntryPoint>\n");

	return length;
}

int asn1decode_GetFillColour(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GetFillColour>\n");

	/* SEQUENCE */
	output_token(out, ":GetFillColour");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetFillColour");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("GetFillColour");
		left -= tag.length;
	}
	else
	{
		return der_error("GetFillColour: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetFillColour");
	left -= sublen;

	if(is_FillColourVar(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_FillColourVar(der, out, sublen + tag.length)) < 0)
			return der_error("GetFillColour");
		left -= tag.length;
	}
	else
	{
		return der_error("GetFillColour: missing FillColourVar");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("GetFillColour: %d bytes left", left);

	verbose("</GetFillColour>\n");

	return length;
}

int asn1decode_GetFirstItem(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GetFirstItem>\n");

	/* SEQUENCE */
	output_token(out, ":GetFirstItem");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetFirstItem");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("GetFirstItem");
		left -= tag.length;
	}
	else
	{
		return der_error("GetFirstItem: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetFirstItem");
	left -= sublen;

	if(is_FirstItemVar(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_FirstItemVar(der, out, sublen + tag.length)) < 0)
			return der_error("GetFirstItem");
		left -= tag.length;
	}
	else
	{
		return der_error("GetFirstItem: missing FirstItemVar");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("GetFirstItem: %d bytes left", left);

	verbose("</GetFirstItem>\n");

	return length;
}

int asn1decode_GetHighlightStatus(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GetHighlightStatus>\n");

	/* SEQUENCE */
	output_token(out, ":GetHighlightStatus");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetHighlightStatus");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("GetHighlightStatus");
		left -= tag.length;
	}
	else
	{
		return der_error("GetHighlightStatus: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetHighlightStatus");
	left -= sublen;

	if(is_HighlightStatusVar(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_HighlightStatusVar(der, out, sublen + tag.length)) < 0)
			return der_error("GetHighlightStatus");
		left -= tag.length;
	}
	else
	{
		return der_error("GetHighlightStatus: missing HighlightStatusVar");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("GetHighlightStatus: %d bytes left", left);

	verbose("</GetHighlightStatus>\n");

	return length;
}

int asn1decode_GetInteractionStatus(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GetInteractionStatus>\n");

	/* SEQUENCE */
	output_token(out, ":GetInteractionStatus");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetInteractionStatus");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("GetInteractionStatus");
		left -= tag.length;
	}
	else
	{
		return der_error("GetInteractionStatus: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetInteractionStatus");
	left -= sublen;

	if(is_InteractionStatusVar(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_InteractionStatusVar(der, out, sublen + tag.length)) < 0)
			return der_error("GetInteractionStatus");
		left -= tag.length;
	}
	else
	{
		return der_error("GetInteractionStatus: missing InteractionStatusVar");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("GetInteractionStatus: %d bytes left", left);

	verbose("</GetInteractionStatus>\n");

	return length;
}

int asn1decode_GetItemStatus(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GetItemStatus>\n");

	/* SEQUENCE */
	output_token(out, ":GetItemStatus");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetItemStatus");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("GetItemStatus");
		left -= tag.length;
	}
	else
	{
		return der_error("GetItemStatus: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetItemStatus");
	left -= sublen;

	if(is_ItemIndex(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ItemIndex(der, out, sublen + tag.length)) < 0)
			return der_error("GetItemStatus");
		left -= tag.length;
	}
	else
	{
		return der_error("GetItemStatus: missing ItemIndex");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetItemStatus");
	left -= sublen;

	if(is_ItemStatusVar(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ItemStatusVar(der, out, sublen + tag.length)) < 0)
			return der_error("GetItemStatus");
		left -= tag.length;
	}
	else
	{
		return der_error("GetItemStatus: missing ItemStatusVar");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("GetItemStatus: %d bytes left", left);

	verbose("</GetItemStatus>\n");

	return length;
}

int asn1decode_GetLabel(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GetLabel>\n");

	/* SEQUENCE */
	output_token(out, ":GetLabel");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetLabel");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("GetLabel");
		left -= tag.length;
	}
	else
	{
		return der_error("GetLabel: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetLabel");
	left -= sublen;

	if(is_LabelVar(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_LabelVar(der, out, sublen + tag.length)) < 0)
			return der_error("GetLabel");
		left -= tag.length;
	}
	else
	{
		return der_error("GetLabel: missing LabelVar");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("GetLabel: %d bytes left", left);

	verbose("</GetLabel>\n");

	return length;
}

int asn1decode_GetLastAnchorFired(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GetLastAnchorFired>\n");

	/* SEQUENCE */
	output_token(out, ":GetLastAnchorFired");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetLastAnchorFired");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("GetLastAnchorFired");
		left -= tag.length;
	}
	else
	{
		return der_error("GetLastAnchorFired: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetLastAnchorFired");
	left -= sublen;

	if(is_LastAnchorFiredVar(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_LastAnchorFiredVar(der, out, sublen + tag.length)) < 0)
			return der_error("GetLastAnchorFired");
		left -= tag.length;
	}
	else
	{
		return der_error("GetLastAnchorFired: missing LastAnchorFiredVar");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("GetLastAnchorFired: %d bytes left", left);

	verbose("</GetLastAnchorFired>\n");

	return length;
}

int asn1decode_GetLineColour(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GetLineColour>\n");

	/* SEQUENCE */
	output_token(out, ":GetLineColour");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetLineColour");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("GetLineColour");
		left -= tag.length;
	}
	else
	{
		return der_error("GetLineColour: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetLineColour");
	left -= sublen;

	if(is_LineColourVar(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_LineColourVar(der, out, sublen + tag.length)) < 0)
			return der_error("GetLineColour");
		left -= tag.length;
	}
	else
	{
		return der_error("GetLineColour: missing LineColourVar");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("GetLineColour: %d bytes left", left);

	verbose("</GetLineColour>\n");

	return length;
}

int asn1decode_GetLineStyle(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GetLineStyle>\n");

	/* SEQUENCE */
	output_token(out, ":GetLineStyle");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetLineStyle");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("GetLineStyle");
		left -= tag.length;
	}
	else
	{
		return der_error("GetLineStyle: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetLineStyle");
	left -= sublen;

	if(is_LineStyleVar(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_LineStyleVar(der, out, sublen + tag.length)) < 0)
			return der_error("GetLineStyle");
		left -= tag.length;
	}
	else
	{
		return der_error("GetLineStyle: missing LineStyleVar");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("GetLineStyle: %d bytes left", left);

	verbose("</GetLineStyle>\n");

	return length;
}

int asn1decode_GetLineWidth(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GetLineWidth>\n");

	/* SEQUENCE */
	output_token(out, ":GetLineWidth");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetLineWidth");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("GetLineWidth");
		left -= tag.length;
	}
	else
	{
		return der_error("GetLineWidth: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetLineWidth");
	left -= sublen;

	if(is_LineWidthVar(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_LineWidthVar(der, out, sublen + tag.length)) < 0)
			return der_error("GetLineWidth");
		left -= tag.length;
	}
	else
	{
		return der_error("GetLineWidth: missing LineWidthVar");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("GetLineWidth: %d bytes left", left);

	verbose("</GetLineWidth>\n");

	return length;
}

int asn1decode_GetListItem(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GetListItem>\n");

	/* SEQUENCE */
	output_token(out, ":GetListItem");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetListItem");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("GetListItem");
		left -= tag.length;
	}
	else
	{
		return der_error("GetListItem: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetListItem");
	left -= sublen;

	if(is_ItemIndex(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ItemIndex(der, out, sublen + tag.length)) < 0)
			return der_error("GetListItem");
		left -= tag.length;
	}
	else
	{
		return der_error("GetListItem: missing ItemIndex");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetListItem");
	left -= sublen;

	if(is_ItemRefVar(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ItemRefVar(der, out, sublen + tag.length)) < 0)
			return der_error("GetListItem");
		left -= tag.length;
	}
	else
	{
		return der_error("GetListItem: missing ItemRefVar");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("GetListItem: %d bytes left", left);

	verbose("</GetListItem>\n");

	return length;
}

int asn1decode_GetListSize(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GetListSize>\n");

	/* SEQUENCE */
	output_token(out, ":GetListSize");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetListSize");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("GetListSize");
		left -= tag.length;
	}
	else
	{
		return der_error("GetListSize: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetListSize");
	left -= sublen;

	if(is_SizeVar(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_SizeVar(der, out, sublen + tag.length)) < 0)
			return der_error("GetListSize");
		left -= tag.length;
	}
	else
	{
		return der_error("GetListSize: missing SizeVar");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("GetListSize: %d bytes left", left);

	verbose("</GetListSize>\n");

	return length;
}

int asn1decode_GetOverwriteMode(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GetOverwriteMode>\n");

	/* SEQUENCE */
	output_token(out, ":GetOverwriteMode");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetOverwriteMode");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("GetOverwriteMode");
		left -= tag.length;
	}
	else
	{
		return der_error("GetOverwriteMode: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetOverwriteMode");
	left -= sublen;

	if(is_OverwriteModeVar(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_OverwriteModeVar(der, out, sublen + tag.length)) < 0)
			return der_error("GetOverwriteMode");
		left -= tag.length;
	}
	else
	{
		return der_error("GetOverwriteMode: missing OverwriteModeVar");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("GetOverwriteMode: %d bytes left", left);

	verbose("</GetOverwriteMode>\n");

	return length;
}

int asn1decode_GetPortion(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GetPortion>\n");

	/* SEQUENCE */
	output_token(out, ":GetPortion");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetPortion");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("GetPortion");
		left -= tag.length;
	}
	else
	{
		return der_error("GetPortion: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetPortion");
	left -= sublen;

	if(is_PortionVar(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_PortionVar(der, out, sublen + tag.length)) < 0)
			return der_error("GetPortion");
		left -= tag.length;
	}
	else
	{
		return der_error("GetPortion: missing PortionVar");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("GetPortion: %d bytes left", left);

	verbose("</GetPortion>\n");

	return length;
}

int asn1decode_GetPosition(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GetPosition>\n");

	/* SEQUENCE */
	output_token(out, ":GetPosition");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetPosition");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("GetPosition");
		left -= tag.length;
	}
	else
	{
		return der_error("GetPosition: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetPosition");
	left -= sublen;

	if(is_XPositionVar(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_XPositionVar(der, out, sublen + tag.length)) < 0)
			return der_error("GetPosition");
		left -= tag.length;
	}
	else
	{
		return der_error("GetPosition: missing XPositionVar");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetPosition");
	left -= sublen;

	if(is_YPositionVar(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_YPositionVar(der, out, sublen + tag.length)) < 0)
			return der_error("GetPosition");
		left -= tag.length;
	}
	else
	{
		return der_error("GetPosition: missing YPositionVar");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("GetPosition: %d bytes left", left);

	verbose("</GetPosition>\n");

	return length;
}

int asn1decode_GetRunningStatus(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GetRunningStatus>\n");

	/* SEQUENCE */
	output_token(out, ":GetRunningStatus");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetRunningStatus");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("GetRunningStatus");
		left -= tag.length;
	}
	else
	{
		return der_error("GetRunningStatus: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetRunningStatus");
	left -= sublen;

	if(is_RunningStatusVar(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_RunningStatusVar(der, out, sublen + tag.length)) < 0)
			return der_error("GetRunningStatus");
		left -= tag.length;
	}
	else
	{
		return der_error("GetRunningStatus: missing RunningStatusVar");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("GetRunningStatus: %d bytes left", left);

	verbose("</GetRunningStatus>\n");

	return length;
}

int asn1decode_GetSelectionStatus(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GetSelectionStatus>\n");

	/* SEQUENCE */
	output_token(out, ":GetSelectionStatus");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetSelectionStatus");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("GetSelectionStatus");
		left -= tag.length;
	}
	else
	{
		return der_error("GetSelectionStatus: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetSelectionStatus");
	left -= sublen;

	if(is_SelectionStatusVar(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_SelectionStatusVar(der, out, sublen + tag.length)) < 0)
			return der_error("GetSelectionStatus");
		left -= tag.length;
	}
	else
	{
		return der_error("GetSelectionStatus: missing SelectionStatusVar");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("GetSelectionStatus: %d bytes left", left);

	verbose("</GetSelectionStatus>\n");

	return length;
}

int asn1decode_GetSliderValue(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GetSliderValue>\n");

	/* SEQUENCE */
	output_token(out, ":GetSliderValue");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetSliderValue");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("GetSliderValue");
		left -= tag.length;
	}
	else
	{
		return der_error("GetSliderValue: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetSliderValue");
	left -= sublen;

	if(is_SliderValueVar(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_SliderValueVar(der, out, sublen + tag.length)) < 0)
			return der_error("GetSliderValue");
		left -= tag.length;
	}
	else
	{
		return der_error("GetSliderValue: missing SliderValueVar");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("GetSliderValue: %d bytes left", left);

	verbose("</GetSliderValue>\n");

	return length;
}

int asn1decode_GetTextContent(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GetTextContent>\n");

	/* SEQUENCE */
	output_token(out, ":GetTextContent");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetTextContent");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("GetTextContent");
		left -= tag.length;
	}
	else
	{
		return der_error("GetTextContent: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetTextContent");
	left -= sublen;

	if(is_TextContentVar(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_TextContentVar(der, out, sublen + tag.length)) < 0)
			return der_error("GetTextContent");
		left -= tag.length;
	}
	else
	{
		return der_error("GetTextContent: missing TextContentVar");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("GetTextContent: %d bytes left", left);

	verbose("</GetTextContent>\n");

	return length;
}

int asn1decode_GetTextData(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GetTextData>\n");

	/* SEQUENCE */
	output_token(out, ":GetTextData");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetTextData");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("GetTextData");
		left -= tag.length;
	}
	else
	{
		return der_error("GetTextData: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetTextData");
	left -= sublen;

	if(is_TextDataVar(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_TextDataVar(der, out, sublen + tag.length)) < 0)
			return der_error("GetTextData");
		left -= tag.length;
	}
	else
	{
		return der_error("GetTextData: missing TextDataVar");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("GetTextData: %d bytes left", left);

	verbose("</GetTextData>\n");

	return length;
}

int asn1decode_GetTokenPosition(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GetTokenPosition>\n");

	/* SEQUENCE */
	output_token(out, ":GetTokenPosition");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetTokenPosition");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("GetTokenPosition");
		left -= tag.length;
	}
	else
	{
		return der_error("GetTokenPosition: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetTokenPosition");
	left -= sublen;

	if(is_TokenPositionVar(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_TokenPositionVar(der, out, sublen + tag.length)) < 0)
			return der_error("GetTokenPosition");
		left -= tag.length;
	}
	else
	{
		return der_error("GetTokenPosition: missing TokenPositionVar");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("GetTokenPosition: %d bytes left", left);

	verbose("</GetTokenPosition>\n");

	return length;
}

int asn1decode_GetVolume(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GetVolume>\n");

	/* SEQUENCE */
	output_token(out, ":GetVolume");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetVolume");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("GetVolume");
		left -= tag.length;
	}
	else
	{
		return der_error("GetVolume: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetVolume");
	left -= sublen;

	if(is_VolumeVar(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_VolumeVar(der, out, sublen + tag.length)) < 0)
			return der_error("GetVolume");
		left -= tag.length;
	}
	else
	{
		return der_error("GetVolume: missing VolumeVar");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("GetVolume: %d bytes left", left);

	verbose("</GetVolume>\n");

	return length;
}

int asn1decode_Launch(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Launch>\n");

	output_token(out, ":Launch");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Launch");

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, length)) < 0)
			return der_error("Launch");
		left -= sublen;
	}
	else
	{
		return der_error("Launch: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("Launch: %d bytes left", left);

	verbose("</Launch>\n");

	return length;
}

int asn1decode_LockScreen(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<LockScreen>\n");

	output_token(out, ":LockScreen");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("LockScreen");

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, length)) < 0)
			return der_error("LockScreen");
		left -= sublen;
	}
	else
	{
		return der_error("LockScreen: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("LockScreen: %d bytes left", left);

	verbose("</LockScreen>\n");

	return length;
}

int asn1decode_Modulo(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Modulo>\n");

	/* SEQUENCE */
	output_token(out, ":Modulo");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Modulo");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("Modulo");
		left -= tag.length;
	}
	else
	{
		return der_error("Modulo: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Modulo");
	left -= sublen;

	if(is_Value(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Value(der, out, sublen + tag.length)) < 0)
			return der_error("Modulo");
		left -= tag.length;
	}
	else
	{
		return der_error("Modulo: missing Value");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("Modulo: %d bytes left", left);

	verbose("</Modulo>\n");

	return length;
}

int asn1decode_Move(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Move>\n");

	/* SEQUENCE */
	output_token(out, ":Move");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Move");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("Move");
		left -= tag.length;
	}
	else
	{
		return der_error("Move: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Move");
	left -= sublen;

	if(is_MovementIdentifier(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_MovementIdentifier(der, out, sublen + tag.length)) < 0)
			return der_error("Move");
		left -= tag.length;
	}
	else
	{
		return der_error("Move: missing MovementIdentifier");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("Move: %d bytes left", left);

	verbose("</Move>\n");

	return length;
}

int asn1decode_MoveTo(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<MoveTo>\n");

	/* SEQUENCE */
	output_token(out, ":MoveTo");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("MoveTo");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("MoveTo");
		left -= tag.length;
	}
	else
	{
		return der_error("MoveTo: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("MoveTo");
	left -= sublen;

	if(is_Index(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Index(der, out, sublen + tag.length)) < 0)
			return der_error("MoveTo");
		left -= tag.length;
	}
	else
	{
		return der_error("MoveTo: missing Index");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("MoveTo: %d bytes left", left);

	verbose("</MoveTo>\n");

	return length;
}

int asn1decode_Multiply(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Multiply>\n");

	/* SEQUENCE */
	output_token(out, ":Multiply");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Multiply");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("Multiply");
		left -= tag.length;
	}
	else
	{
		return der_error("Multiply: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Multiply");
	left -= sublen;

	if(is_Value(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Value(der, out, sublen + tag.length)) < 0)
			return der_error("Multiply");
		left -= tag.length;
	}
	else
	{
		return der_error("Multiply: missing Value");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("Multiply: %d bytes left", left);

	verbose("</Multiply>\n");

	return length;
}

int asn1decode_OpenConnection(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<OpenConnection>\n");

	/* SEQUENCE */
	output_token(out, ":OpenConnection");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("OpenConnection");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("OpenConnection");
		left -= tag.length;
	}
	else
	{
		return der_error("OpenConnection: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("OpenConnection");
	left -= sublen;

	if(is_OpenSucceeded(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_OpenSucceeded(der, out, sublen + tag.length)) < 0)
			return der_error("OpenConnection");
		left -= tag.length;
	}
	else
	{
		return der_error("OpenConnection: missing OpenSucceeded");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("OpenConnection");
	left -= sublen;

	if(is_Protocol(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Protocol(der, out, sublen + tag.length)) < 0)
			return der_error("OpenConnection");
		left -= tag.length;
	}
	else
	{
		return der_error("OpenConnection: missing Protocol");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("OpenConnection");
	left -= sublen;

	if(is_Address(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Address(der, out, sublen + tag.length)) < 0)
			return der_error("OpenConnection");
		left -= tag.length;
	}
	else
	{
		return der_error("OpenConnection: missing Address");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("OpenConnection");
	left -= sublen;

	if(is_ConnectionTag(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ConnectionTag(der, out, sublen + tag.length)) < 0)
			return der_error("OpenConnection");
		left -= tag.length;
	}
	else
	{
		return der_error("OpenConnection: missing ConnectionTag");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("OpenConnection: %d bytes left", left);

	verbose("</OpenConnection>\n");

	return length;
}

int asn1decode_Preload(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Preload>\n");

	output_token(out, ":Preload");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Preload");

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, length)) < 0)
			return der_error("Preload");
		left -= sublen;
	}
	else
	{
		return der_error("Preload: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("Preload: %d bytes left", left);

	verbose("</Preload>\n");

	return length;
}

int asn1decode_PutBefore(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<PutBefore>\n");

	/* SEQUENCE */
	output_token(out, ":PutBefore");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("PutBefore");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("PutBefore");
		left -= tag.length;
	}
	else
	{
		return der_error("PutBefore: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("PutBefore");
	left -= sublen;

	if(is_ReferenceVisible(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ReferenceVisible(der, out, sublen + tag.length)) < 0)
			return der_error("PutBefore");
		left -= tag.length;
	}
	else
	{
		return der_error("PutBefore: missing ReferenceVisible");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("PutBefore: %d bytes left", left);

	verbose("</PutBefore>\n");

	return length;
}

int asn1decode_PutBehind(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<PutBehind>\n");

	/* SEQUENCE */
	output_token(out, ":PutBehind");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("PutBehind");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("PutBehind");
		left -= tag.length;
	}
	else
	{
		return der_error("PutBehind: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("PutBehind");
	left -= sublen;

	if(is_ReferenceVisible(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ReferenceVisible(der, out, sublen + tag.length)) < 0)
			return der_error("PutBehind");
		left -= tag.length;
	}
	else
	{
		return der_error("PutBehind: missing ReferenceVisible");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("PutBehind: %d bytes left", left);

	verbose("</PutBehind>\n");

	return length;
}

int asn1decode_Quit(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Quit>\n");

	output_token(out, ":Quit");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Quit");

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, length)) < 0)
			return der_error("Quit");
		left -= sublen;
	}
	else
	{
		return der_error("Quit: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("Quit: %d bytes left", left);

	verbose("</Quit>\n");

	return length;
}

int asn1decode_ReadPersistent(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ReadPersistent>\n");

	/* SEQUENCE */
	output_token(out, ":ReadPersistent");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ReadPersistent");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("ReadPersistent");
		left -= tag.length;
	}
	else
	{
		return der_error("ReadPersistent: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ReadPersistent");
	left -= sublen;

	if(is_ReadSucceeded(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ReadSucceeded(der, out, sublen + tag.length)) < 0)
			return der_error("ReadPersistent");
		left -= tag.length;
	}
	else
	{
		return der_error("ReadPersistent: missing ReadSucceeded");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ReadPersistent");
	left -= sublen;

	if(is_OutVariables(tag.class, tag.number))
	{
		if((sublen = asn1decode_OutVariables(der, out, tag.length)) < 0)
			return der_error("ReadPersistent");
		left -= tag.length;
	}
	else
	{
		return der_error("ReadPersistent: missing OutVariables");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ReadPersistent");
	left -= sublen;

	if(is_InFileName(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_InFileName(der, out, sublen + tag.length)) < 0)
			return der_error("ReadPersistent");
		left -= tag.length;
	}
	else
	{
		return der_error("ReadPersistent: missing InFileName");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("ReadPersistent: %d bytes left", left);

	verbose("</ReadPersistent>\n");

	return length;
}

int asn1decode_Run(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Run>\n");

	output_token(out, ":Run");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Run");

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, length)) < 0)
			return der_error("Run");
		left -= sublen;
	}
	else
	{
		return der_error("Run: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("Run: %d bytes left", left);

	verbose("</Run>\n");

	return length;
}

int asn1decode_ScaleBitmap(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ScaleBitmap>\n");

	/* SEQUENCE */
	output_token(out, ":ScaleBitmap");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ScaleBitmap");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("ScaleBitmap");
		left -= tag.length;
	}
	else
	{
		return der_error("ScaleBitmap: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ScaleBitmap");
	left -= sublen;

	if(is_XScale(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_XScale(der, out, sublen + tag.length)) < 0)
			return der_error("ScaleBitmap");
		left -= tag.length;
	}
	else
	{
		return der_error("ScaleBitmap: missing XScale");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ScaleBitmap");
	left -= sublen;

	if(is_YScale(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_YScale(der, out, sublen + tag.length)) < 0)
			return der_error("ScaleBitmap");
		left -= tag.length;
	}
	else
	{
		return der_error("ScaleBitmap: missing YScale");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("ScaleBitmap: %d bytes left", left);

	verbose("</ScaleBitmap>\n");

	return length;
}

int asn1decode_ScaleVideo(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ScaleVideo>\n");

	/* SEQUENCE */
	output_token(out, ":ScaleVideo");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ScaleVideo");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("ScaleVideo");
		left -= tag.length;
	}
	else
	{
		return der_error("ScaleVideo: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ScaleVideo");
	left -= sublen;

	if(is_XScale(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_XScale(der, out, sublen + tag.length)) < 0)
			return der_error("ScaleVideo");
		left -= tag.length;
	}
	else
	{
		return der_error("ScaleVideo: missing XScale");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ScaleVideo");
	left -= sublen;

	if(is_YScale(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_YScale(der, out, sublen + tag.length)) < 0)
			return der_error("ScaleVideo");
		left -= tag.length;
	}
	else
	{
		return der_error("ScaleVideo: missing YScale");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("ScaleVideo: %d bytes left", left);

	verbose("</ScaleVideo>\n");

	return length;
}

int asn1decode_ScrollItems(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ScrollItems>\n");

	/* SEQUENCE */
	output_token(out, ":ScrollItems");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ScrollItems");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("ScrollItems");
		left -= tag.length;
	}
	else
	{
		return der_error("ScrollItems: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ScrollItems");
	left -= sublen;

	if(is_ItemsToScroll(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ItemsToScroll(der, out, sublen + tag.length)) < 0)
			return der_error("ScrollItems");
		left -= tag.length;
	}
	else
	{
		return der_error("ScrollItems: missing ItemsToScroll");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("ScrollItems: %d bytes left", left);

	verbose("</ScrollItems>\n");

	return length;
}

int asn1decode_Select(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Select>\n");

	output_token(out, ":Select");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Select");

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, length)) < 0)
			return der_error("Select");
		left -= sublen;
	}
	else
	{
		return der_error("Select: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("Select: %d bytes left", left);

	verbose("</Select>\n");

	return length;
}

int asn1decode_SelectItem(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SelectItem>\n");

	/* SEQUENCE */
	output_token(out, ":SelectItem");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SelectItem");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SelectItem");
		left -= tag.length;
	}
	else
	{
		return der_error("SelectItem: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SelectItem");
	left -= sublen;

	if(is_ItemIndex(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ItemIndex(der, out, sublen + tag.length)) < 0)
			return der_error("SelectItem");
		left -= tag.length;
	}
	else
	{
		return der_error("SelectItem: missing ItemIndex");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SelectItem: %d bytes left", left);

	verbose("</SelectItem>\n");

	return length;
}

int asn1decode_SendEvent(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SendEvent>\n");

	/* SEQUENCE */
	output_token(out, ":SendEvent");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SendEvent");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SendEvent");
		left -= tag.length;
	}
	else
	{
		return der_error("SendEvent: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SendEvent");
	left -= sublen;

	if(is_EmulatedEventSource(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_EmulatedEventSource(der, out, sublen + tag.length)) < 0)
			return der_error("SendEvent");
		left -= tag.length;
	}
	else
	{
		return der_error("SendEvent: missing EmulatedEventSource");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SendEvent");
	left -= sublen;

	if(is_EmulatedEventType(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_EmulatedEventType(der, out, sublen + tag.length)) < 0)
			return der_error("SendEvent");
		left -= tag.length;
	}
	else
	{
		return der_error("SendEvent: missing EmulatedEventType");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SendEvent");
	left -= sublen;

	if(is_EmulatedEventData(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_EmulatedEventData(der, out, sublen + tag.length)) < 0)
			return der_error("SendEvent");
		left -= tag.length;
	}
	else
	{
		fseek(der, -sublen, SEEK_CUR);
		left += sublen;
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SendEvent: %d bytes left", left);

	verbose("</SendEvent>\n");

	return length;
}

int asn1decode_SendToBack(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SendToBack>\n");

	output_token(out, ":SendToBack");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SendToBack");

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, length)) < 0)
			return der_error("SendToBack");
		left -= sublen;
	}
	else
	{
		return der_error("SendToBack: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SendToBack: %d bytes left", left);

	verbose("</SendToBack>\n");

	return length;
}

int asn1decode_SetBoxSize(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetBoxSize>\n");

	/* SEQUENCE */
	output_token(out, ":SetBoxSize");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetBoxSize");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetBoxSize");
		left -= tag.length;
	}
	else
	{
		return der_error("SetBoxSize: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetBoxSize");
	left -= sublen;

	if(is_XNewBoxSize(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_XNewBoxSize(der, out, sublen + tag.length)) < 0)
			return der_error("SetBoxSize");
		left -= tag.length;
	}
	else
	{
		return der_error("SetBoxSize: missing XNewBoxSize");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetBoxSize");
	left -= sublen;

	if(is_YNewBoxSize(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_YNewBoxSize(der, out, sublen + tag.length)) < 0)
			return der_error("SetBoxSize");
		left -= tag.length;
	}
	else
	{
		return der_error("SetBoxSize: missing YNewBoxSize");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetBoxSize: %d bytes left", left);

	verbose("</SetBoxSize>\n");

	return length;
}

int asn1decode_SetCachePriority(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetCachePriority>\n");

	/* SEQUENCE */
	output_token(out, ":SetCachePriority");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetCachePriority");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetCachePriority");
		left -= tag.length;
	}
	else
	{
		return der_error("SetCachePriority: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetCachePriority");
	left -= sublen;

	if(is_NewCachePriority(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewCachePriority(der, out, sublen + tag.length)) < 0)
			return der_error("SetCachePriority");
		left -= tag.length;
	}
	else
	{
		return der_error("SetCachePriority: missing NewCachePriority");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetCachePriority: %d bytes left", left);

	verbose("</SetCachePriority>\n");

	return length;
}

int asn1decode_SetCounterEndPosition(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetCounterEndPosition>\n");

	/* SEQUENCE */
	output_token(out, ":SetCounterEndPosition");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetCounterEndPosition");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetCounterEndPosition");
		left -= tag.length;
	}
	else
	{
		return der_error("SetCounterEndPosition: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetCounterEndPosition");
	left -= sublen;

	if(is_NewCounterEndPosition(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewCounterEndPosition(der, out, sublen + tag.length)) < 0)
			return der_error("SetCounterEndPosition");
		left -= tag.length;
	}
	else
	{
		return der_error("SetCounterEndPosition: missing NewCounterEndPosition");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetCounterEndPosition: %d bytes left", left);

	verbose("</SetCounterEndPosition>\n");

	return length;
}

int asn1decode_SetCounterPosition(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetCounterPosition>\n");

	/* SEQUENCE */
	output_token(out, ":SetCounterPosition");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetCounterPosition");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetCounterPosition");
		left -= tag.length;
	}
	else
	{
		return der_error("SetCounterPosition: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetCounterPosition");
	left -= sublen;

	if(is_NewCounterPosition(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewCounterPosition(der, out, sublen + tag.length)) < 0)
			return der_error("SetCounterPosition");
		left -= tag.length;
	}
	else
	{
		return der_error("SetCounterPosition: missing NewCounterPosition");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetCounterPosition: %d bytes left", left);

	verbose("</SetCounterPosition>\n");

	return length;
}

int asn1decode_SetCounterTrigger(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetCounterTrigger>\n");

	/* SEQUENCE */
	output_token(out, ":SetCounterTrigger");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetCounterTrigger");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetCounterTrigger");
		left -= tag.length;
	}
	else
	{
		return der_error("SetCounterTrigger: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetCounterTrigger");
	left -= sublen;

	if(is_TriggerIdentifier(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_TriggerIdentifier(der, out, sublen + tag.length)) < 0)
			return der_error("SetCounterTrigger");
		left -= tag.length;
	}
	else
	{
		return der_error("SetCounterTrigger: missing TriggerIdentifier");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetCounterTrigger");
	left -= sublen;

	if(is_NewCounterValue(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewCounterValue(der, out, sublen + tag.length)) < 0)
			return der_error("SetCounterTrigger");
		left -= tag.length;
	}
	else
	{
		fseek(der, -sublen, SEEK_CUR);
		left += sublen;
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetCounterTrigger: %d bytes left", left);

	verbose("</SetCounterTrigger>\n");

	return length;
}

int asn1decode_SetCursorPosition(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetCursorPosition>\n");

	/* SEQUENCE */
	output_token(out, ":SetCursorPosition");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetCursorPosition");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetCursorPosition");
		left -= tag.length;
	}
	else
	{
		return der_error("SetCursorPosition: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetCursorPosition");
	left -= sublen;

	if(is_XCursor(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_XCursor(der, out, sublen + tag.length)) < 0)
			return der_error("SetCursorPosition");
		left -= tag.length;
	}
	else
	{
		return der_error("SetCursorPosition: missing XCursor");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetCursorPosition");
	left -= sublen;

	if(is_YCursor(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_YCursor(der, out, sublen + tag.length)) < 0)
			return der_error("SetCursorPosition");
		left -= tag.length;
	}
	else
	{
		return der_error("SetCursorPosition: missing YCursor");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetCursorPosition: %d bytes left", left);

	verbose("</SetCursorPosition>\n");

	return length;
}

int asn1decode_SetCursorShape(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetCursorShape>\n");

	/* SEQUENCE */
	output_token(out, ":SetCursorShape");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetCursorShape");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetCursorShape");
		left -= tag.length;
	}
	else
	{
		return der_error("SetCursorShape: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetCursorShape");
	left -= sublen;

	if(is_NewCursorShape(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewCursorShape(der, out, sublen + tag.length)) < 0)
			return der_error("SetCursorShape");
		left -= tag.length;
	}
	else
	{
		fseek(der, -sublen, SEEK_CUR);
		left += sublen;
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetCursorShape: %d bytes left", left);

	verbose("</SetCursorShape>\n");

	return length;
}

int asn1decode_SetData(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetData>\n");

	/* SEQUENCE */
	output_token(out, ":SetData");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetData");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetData");
		left -= tag.length;
	}
	else
	{
		return der_error("SetData: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetData");
	left -= sublen;

	if(is_NewContent(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewContent(der, out, sublen + tag.length)) < 0)
			return der_error("SetData");
		left -= tag.length;
	}
	else
	{
		return der_error("SetData: missing NewContent");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetData: %d bytes left", left);

	verbose("</SetData>\n");

	return length;
}

int asn1decode_SetEntryPoint(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetEntryPoint>\n");

	/* SEQUENCE */
	output_token(out, ":SetEntryPoint");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetEntryPoint");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetEntryPoint");
		left -= tag.length;
	}
	else
	{
		return der_error("SetEntryPoint: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetEntryPoint");
	left -= sublen;

	if(is_NewEntryPoint(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewEntryPoint(der, out, sublen + tag.length)) < 0)
			return der_error("SetEntryPoint");
		left -= tag.length;
	}
	else
	{
		return der_error("SetEntryPoint: missing NewEntryPoint");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetEntryPoint: %d bytes left", left);

	verbose("</SetEntryPoint>\n");

	return length;
}

int asn1decode_SetFillColour(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetFillColour>\n");

	/* SEQUENCE */
	output_token(out, ":SetFillColour");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetFillColour");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetFillColour");
		left -= tag.length;
	}
	else
	{
		return der_error("SetFillColour: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetFillColour");
	left -= sublen;

	if(is_NewColour(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewColour(der, out, sublen + tag.length)) < 0)
			return der_error("SetFillColour");
		left -= tag.length;
	}
	else
	{
		fseek(der, -sublen, SEEK_CUR);
		left += sublen;
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetFillColour: %d bytes left", left);

	verbose("</SetFillColour>\n");

	return length;
}

int asn1decode_SetFirstItem(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetFirstItem>\n");

	/* SEQUENCE */
	output_token(out, ":SetFirstItem");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetFirstItem");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetFirstItem");
		left -= tag.length;
	}
	else
	{
		return der_error("SetFirstItem: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetFirstItem");
	left -= sublen;

	if(is_NewFirstItem(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewFirstItem(der, out, sublen + tag.length)) < 0)
			return der_error("SetFirstItem");
		left -= tag.length;
	}
	else
	{
		return der_error("SetFirstItem: missing NewFirstItem");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetFirstItem: %d bytes left", left);

	verbose("</SetFirstItem>\n");

	return length;
}

int asn1decode_SetFontRef(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetFontRef>\n");

	/* SEQUENCE */
	output_token(out, ":SetFontRef");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetFontRef");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetFontRef");
		left -= tag.length;
	}
	else
	{
		return der_error("SetFontRef: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetFontRef");
	left -= sublen;

	if(is_NewFont(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewFont(der, out, sublen + tag.length)) < 0)
			return der_error("SetFontRef");
		left -= tag.length;
	}
	else
	{
		return der_error("SetFontRef: missing NewFont");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetFontRef: %d bytes left", left);

	verbose("</SetFontRef>\n");

	return length;
}

int asn1decode_SetHighlightStatus(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetHighlightStatus>\n");

	/* SEQUENCE */
	output_token(out, ":SetHighlightStatus");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetHighlightStatus");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetHighlightStatus");
		left -= tag.length;
	}
	else
	{
		return der_error("SetHighlightStatus: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetHighlightStatus");
	left -= sublen;

	if(is_NewHighlightStatus(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewHighlightStatus(der, out, sublen + tag.length)) < 0)
			return der_error("SetHighlightStatus");
		left -= tag.length;
	}
	else
	{
		return der_error("SetHighlightStatus: missing NewHighlightStatus");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetHighlightStatus: %d bytes left", left);

	verbose("</SetHighlightStatus>\n");

	return length;
}

int asn1decode_SetInteractionStatus(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetInteractionStatus>\n");

	/* SEQUENCE */
	output_token(out, ":SetInteractionStatus");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetInteractionStatus");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetInteractionStatus");
		left -= tag.length;
	}
	else
	{
		return der_error("SetInteractionStatus: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetInteractionStatus");
	left -= sublen;

	if(is_NewInteractionStatus(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewInteractionStatus(der, out, sublen + tag.length)) < 0)
			return der_error("SetInteractionStatus");
		left -= tag.length;
	}
	else
	{
		return der_error("SetInteractionStatus: missing NewInteractionStatus");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetInteractionStatus: %d bytes left", left);

	verbose("</SetInteractionStatus>\n");

	return length;
}

int asn1decode_SetLabel(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetLabel>\n");

	/* SEQUENCE */
	output_token(out, ":SetLabel");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetLabel");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetLabel");
		left -= tag.length;
	}
	else
	{
		return der_error("SetLabel: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetLabel");
	left -= sublen;

	if(is_NewLabel(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewLabel(der, out, sublen + tag.length)) < 0)
			return der_error("SetLabel");
		left -= tag.length;
	}
	else
	{
		return der_error("SetLabel: missing NewLabel");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetLabel: %d bytes left", left);

	verbose("</SetLabel>\n");

	return length;
}

int asn1decode_SetLineColour(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetLineColour>\n");

	/* SEQUENCE */
	output_token(out, ":SetLineColour");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetLineColour");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetLineColour");
		left -= tag.length;
	}
	else
	{
		return der_error("SetLineColour: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetLineColour");
	left -= sublen;

	if(is_NewColour(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewColour(der, out, sublen + tag.length)) < 0)
			return der_error("SetLineColour");
		left -= tag.length;
	}
	else
	{
		return der_error("SetLineColour: missing NewColour");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetLineColour: %d bytes left", left);

	verbose("</SetLineColour>\n");

	return length;
}

int asn1decode_SetLineStyle(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetLineStyle>\n");

	/* SEQUENCE */
	output_token(out, ":SetLineStyle");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetLineStyle");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetLineStyle");
		left -= tag.length;
	}
	else
	{
		return der_error("SetLineStyle: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetLineStyle");
	left -= sublen;

	if(is_NewLineStyle(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewLineStyle(der, out, sublen + tag.length)) < 0)
			return der_error("SetLineStyle");
		left -= tag.length;
	}
	else
	{
		return der_error("SetLineStyle: missing NewLineStyle");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetLineStyle: %d bytes left", left);

	verbose("</SetLineStyle>\n");

	return length;
}

int asn1decode_SetLineWidth(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetLineWidth>\n");

	/* SEQUENCE */
	output_token(out, ":SetLineWidth");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetLineWidth");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetLineWidth");
		left -= tag.length;
	}
	else
	{
		return der_error("SetLineWidth: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetLineWidth");
	left -= sublen;

	if(is_NewLineWidth(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewLineWidth(der, out, sublen + tag.length)) < 0)
			return der_error("SetLineWidth");
		left -= tag.length;
	}
	else
	{
		return der_error("SetLineWidth: missing NewLineWidth");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetLineWidth: %d bytes left", left);

	verbose("</SetLineWidth>\n");

	return length;
}

int asn1decode_SetOverwriteMode(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetOverwriteMode>\n");

	/* SEQUENCE */
	output_token(out, ":SetOverwriteMode");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetOverwriteMode");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetOverwriteMode");
		left -= tag.length;
	}
	else
	{
		return der_error("SetOverwriteMode: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetOverwriteMode");
	left -= sublen;

	if(is_NewOverwriteMode(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewOverwriteMode(der, out, sublen + tag.length)) < 0)
			return der_error("SetOverwriteMode");
		left -= tag.length;
	}
	else
	{
		return der_error("SetOverwriteMode: missing NewOverwriteMode");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetOverwriteMode: %d bytes left", left);

	verbose("</SetOverwriteMode>\n");

	return length;
}

int asn1decode_SetPaletteRef(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetPaletteRef>\n");

	/* SEQUENCE */
	output_token(out, ":SetPaletteRef");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetPaletteRef");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetPaletteRef");
		left -= tag.length;
	}
	else
	{
		return der_error("SetPaletteRef: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetPaletteRef");
	left -= sublen;

	if(is_NewPaletteRef(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewPaletteRef(der, out, sublen + tag.length)) < 0)
			return der_error("SetPaletteRef");
		left -= tag.length;
	}
	else
	{
		return der_error("SetPaletteRef: missing NewPaletteRef");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetPaletteRef: %d bytes left", left);

	verbose("</SetPaletteRef>\n");

	return length;
}

int asn1decode_SetPortion(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetPortion>\n");

	/* SEQUENCE */
	output_token(out, ":SetPortion");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetPortion");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetPortion");
		left -= tag.length;
	}
	else
	{
		return der_error("SetPortion: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetPortion");
	left -= sublen;

	if(is_NewPortion(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewPortion(der, out, sublen + tag.length)) < 0)
			return der_error("SetPortion");
		left -= tag.length;
	}
	else
	{
		return der_error("SetPortion: missing NewPortion");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetPortion: %d bytes left", left);

	verbose("</SetPortion>\n");

	return length;
}

int asn1decode_SetPosition(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetPosition>\n");

	/* SEQUENCE */
	output_token(out, ":SetPosition");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetPosition");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetPosition");
		left -= tag.length;
	}
	else
	{
		return der_error("SetPosition: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetPosition");
	left -= sublen;

	if(is_NewXPosition(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewXPosition(der, out, sublen + tag.length)) < 0)
			return der_error("SetPosition");
		left -= tag.length;
	}
	else
	{
		return der_error("SetPosition: missing NewXPosition");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetPosition");
	left -= sublen;

	if(is_NewYPosition(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewYPosition(der, out, sublen + tag.length)) < 0)
			return der_error("SetPosition");
		left -= tag.length;
	}
	else
	{
		return der_error("SetPosition: missing NewYPosition");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetPosition: %d bytes left", left);

	verbose("</SetPosition>\n");

	return length;
}

int asn1decode_SetSliderValue(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetSliderValue>\n");

	/* SEQUENCE */
	output_token(out, ":SetSliderValue");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetSliderValue");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetSliderValue");
		left -= tag.length;
	}
	else
	{
		return der_error("SetSliderValue: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetSliderValue");
	left -= sublen;

	if(is_NewSliderValue(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewSliderValue(der, out, sublen + tag.length)) < 0)
			return der_error("SetSliderValue");
		left -= tag.length;
	}
	else
	{
		return der_error("SetSliderValue: missing NewSliderValue");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetSliderValue: %d bytes left", left);

	verbose("</SetSliderValue>\n");

	return length;
}

int asn1decode_SetSpeed(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetSpeed>\n");

	/* SEQUENCE */
	output_token(out, ":SetSpeed");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetSpeed");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetSpeed");
		left -= tag.length;
	}
	else
	{
		return der_error("SetSpeed: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetSpeed");
	left -= sublen;

	if(is_NewSpeed(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewSpeed(der, out, sublen + tag.length)) < 0)
			return der_error("SetSpeed");
		left -= tag.length;
	}
	else
	{
		return der_error("SetSpeed: missing NewSpeed");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetSpeed: %d bytes left", left);

	verbose("</SetSpeed>\n");

	return length;
}

int asn1decode_SetTimer(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetTimer>\n");

	/* SEQUENCE */
	output_token(out, ":SetTimer");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetTimer");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetTimer");
		left -= tag.length;
	}
	else
	{
		return der_error("SetTimer: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetTimer");
	left -= sublen;

	if(is_TimerID(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_TimerID(der, out, sublen + tag.length)) < 0)
			return der_error("SetTimer");
		left -= tag.length;
	}
	else
	{
		return der_error("SetTimer: missing TimerID");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetTimer");
	left -= sublen;

	if(is_NewTimer(tag.class, tag.number))
	{
		if((sublen = asn1decode_NewTimer(der, out, tag.length)) < 0)
			return der_error("SetTimer");
		left -= tag.length;
	}
	else
	{
		fseek(der, -sublen, SEEK_CUR);
		left += sublen;
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetTimer: %d bytes left", left);

	verbose("</SetTimer>\n");

	return length;
}

int asn1decode_NewTimer(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewTimer>\n");

	/* SEQUENCE */
	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewTimer");
	left -= sublen;

	if(is_TimerValue(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_TimerValue(der, out, sublen + tag.length)) < 0)
			return der_error("NewTimer");
		left -= tag.length;
	}
	else
	{
		return der_error("NewTimer: missing TimerValue");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewTimer");
	left -= sublen;

	if(is_AbsoluteTime(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_AbsoluteTime(der, out, sublen + tag.length)) < 0)
			return der_error("NewTimer");
		left -= tag.length;
	}
	else
	{
		fseek(der, -sublen, SEEK_CUR);
		left += sublen;
	}

	if(left != 0)
		return der_error("NewTimer: %d bytes left", left);

	verbose("</NewTimer>\n");

	return length;
}

int asn1decode_SetTransparency(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetTransparency>\n");

	/* SEQUENCE */
	output_token(out, ":SetTransparency");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetTransparency");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetTransparency");
		left -= tag.length;
	}
	else
	{
		return der_error("SetTransparency: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetTransparency");
	left -= sublen;

	if(is_NewTransparency(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewTransparency(der, out, sublen + tag.length)) < 0)
			return der_error("SetTransparency");
		left -= tag.length;
	}
	else
	{
		return der_error("SetTransparency: missing NewTransparency");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetTransparency: %d bytes left", left);

	verbose("</SetTransparency>\n");

	return length;
}

int asn1decode_SetVariable(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetVariable>\n");

	/* SEQUENCE */
	output_token(out, ":SetVariable");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetVariable");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetVariable");
		left -= tag.length;
	}
	else
	{
		return der_error("SetVariable: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetVariable");
	left -= sublen;

	if(is_NewVariableValue(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewVariableValue(der, out, sublen + tag.length)) < 0)
			return der_error("SetVariable");
		left -= tag.length;
	}
	else
	{
		return der_error("SetVariable: missing NewVariableValue");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetVariable: %d bytes left", left);

	verbose("</SetVariable>\n");

	return length;
}

int asn1decode_SetVolume(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetVolume>\n");

	/* SEQUENCE */
	output_token(out, ":SetVolume");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetVolume");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetVolume");
		left -= tag.length;
	}
	else
	{
		return der_error("SetVolume: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetVolume");
	left -= sublen;

	if(is_NewVolume(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewVolume(der, out, sublen + tag.length)) < 0)
			return der_error("SetVolume");
		left -= tag.length;
	}
	else
	{
		return der_error("SetVolume: missing NewVolume");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetVolume: %d bytes left", left);

	verbose("</SetVolume>\n");

	return length;
}

int asn1decode_Spawn(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Spawn>\n");

	output_token(out, ":Spawn");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Spawn");

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, length)) < 0)
			return der_error("Spawn");
		left -= sublen;
	}
	else
	{
		return der_error("Spawn: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("Spawn: %d bytes left", left);

	verbose("</Spawn>\n");

	return length;
}

int asn1decode_Stop(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Stop>\n");

	output_token(out, ":Stop");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Stop");

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, length)) < 0)
			return der_error("Stop");
		left -= sublen;
	}
	else
	{
		return der_error("Stop: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("Stop: %d bytes left", left);

	verbose("</Stop>\n");

	return length;
}

int asn1decode_Step(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Step>\n");

	/* SEQUENCE */
	output_token(out, ":Step");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Step");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("Step");
		left -= tag.length;
	}
	else
	{
		return der_error("Step: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Step");
	left -= sublen;

	if(is_NbOfSteps(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NbOfSteps(der, out, sublen + tag.length)) < 0)
			return der_error("Step");
		left -= tag.length;
	}
	else
	{
		return der_error("Step: missing NbOfSteps");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("Step: %d bytes left", left);

	verbose("</Step>\n");

	return length;
}

int asn1decode_StorePersistent(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<StorePersistent>\n");

	/* SEQUENCE */
	output_token(out, ":StorePersistent");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("StorePersistent");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("StorePersistent");
		left -= tag.length;
	}
	else
	{
		return der_error("StorePersistent: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("StorePersistent");
	left -= sublen;

	if(is_StoreSucceeded(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_StoreSucceeded(der, out, sublen + tag.length)) < 0)
			return der_error("StorePersistent");
		left -= tag.length;
	}
	else
	{
		return der_error("StorePersistent: missing StoreSucceeded");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("StorePersistent");
	left -= sublen;

	if(is_InVariables(tag.class, tag.number))
	{
		if((sublen = asn1decode_InVariables(der, out, tag.length)) < 0)
			return der_error("StorePersistent");
		left -= tag.length;
	}
	else
	{
		return der_error("StorePersistent: missing InVariables");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("StorePersistent");
	left -= sublen;

	if(is_OutFileName(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_OutFileName(der, out, sublen + tag.length)) < 0)
			return der_error("StorePersistent");
		left -= tag.length;
	}
	else
	{
		return der_error("StorePersistent: missing OutFileName");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("StorePersistent: %d bytes left", left);

	verbose("</StorePersistent>\n");

	return length;
}

int asn1decode_Subtract(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Subtract>\n");

	/* SEQUENCE */
	output_token(out, ":Subtract");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Subtract");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("Subtract");
		left -= tag.length;
	}
	else
	{
		return der_error("Subtract: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Subtract");
	left -= sublen;

	if(is_Value(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Value(der, out, sublen + tag.length)) < 0)
			return der_error("Subtract");
		left -= tag.length;
	}
	else
	{
		return der_error("Subtract: missing Value");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("Subtract: %d bytes left", left);

	verbose("</Subtract>\n");

	return length;
}

int asn1decode_TestVariable(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<TestVariable>\n");

	/* SEQUENCE */
	output_token(out, ":TestVariable");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("TestVariable");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("TestVariable");
		left -= tag.length;
	}
	else
	{
		return der_error("TestVariable: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("TestVariable");
	left -= sublen;

	if(is_Operator(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Operator(der, out, sublen + tag.length)) < 0)
			return der_error("TestVariable");
		left -= tag.length;
	}
	else
	{
		return der_error("TestVariable: missing Operator");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("TestVariable");
	left -= sublen;

	if(is_ComparisonValue(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ComparisonValue(der, out, sublen + tag.length)) < 0)
			return der_error("TestVariable");
		left -= tag.length;
	}
	else
	{
		return der_error("TestVariable: missing ComparisonValue");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("TestVariable: %d bytes left", left);

	verbose("</TestVariable>\n");

	return length;
}

int asn1decode_Toggle(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Toggle>\n");

	output_token(out, ":Toggle");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Toggle");

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, length)) < 0)
			return der_error("Toggle");
		left -= sublen;
	}
	else
	{
		return der_error("Toggle: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("Toggle: %d bytes left", left);

	verbose("</Toggle>\n");

	return length;
}

int asn1decode_ToggleItem(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ToggleItem>\n");

	/* SEQUENCE */
	output_token(out, ":ToggleItem");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ToggleItem");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("ToggleItem");
		left -= tag.length;
	}
	else
	{
		return der_error("ToggleItem: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ToggleItem");
	left -= sublen;

	if(is_ItemIndex(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ItemIndex(der, out, sublen + tag.length)) < 0)
			return der_error("ToggleItem");
		left -= tag.length;
	}
	else
	{
		return der_error("ToggleItem: missing ItemIndex");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("ToggleItem: %d bytes left", left);

	verbose("</ToggleItem>\n");

	return length;
}

int asn1decode_TransitionTo(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<TransitionTo>\n");

	/* SEQUENCE */
	output_token(out, ":TransitionTo");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("TransitionTo");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("TransitionTo");
		left -= tag.length;
	}
	else
	{
		return der_error("TransitionTo: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("TransitionTo");
	left -= sublen;

	if(is_ConnectionTag(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ConnectionTag(der, out, sublen + tag.length)) < 0)
			return der_error("TransitionTo");
		left -= tag.length;
	}
	else if(!is_Null(tag.class, tag.number))
	{
		return der_error("TransitionTo: missing Null");
	}

	#if 1 // hmkim : added.
	if(left == 0)
	{
		output_token(out, ")");
		verbose("</TransitionTo>\n");
		return length;
	}
	#endif

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("TransitionTo");
	left -= sublen;

	if(is_TransitionEffect(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_TransitionEffect(der, out, sublen + tag.length)) < 0)
			return der_error("TransitionTo");
		left -= tag.length;
	}
	else
	{
		fseek(der, -sublen, SEEK_CUR);
		left += sublen;
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("TransitionTo: %d bytes left", left);

	verbose("</TransitionTo>\n");

	return length;
}

int asn1decode_Unload(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Unload>\n");

	output_token(out, ":Unload");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Unload");

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, length)) < 0)
			return der_error("Unload");
		left -= sublen;
	}
	else
	{
		return der_error("Unload: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("Unload: %d bytes left", left);

	verbose("</Unload>\n");

	return length;
}

int asn1decode_UnlockScreen(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<UnlockScreen>\n");

	output_token(out, ":UnlockScreen");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("UnlockScreen");

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, length)) < 0)
			return der_error("UnlockScreen");
		left -= sublen;
	}
	else
	{
		return der_error("UnlockScreen: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("UnlockScreen: %d bytes left", left);

	verbose("</UnlockScreen>\n");

	return length;
}

int asn1decode_SetBackgroundColour(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetBackgroundColour>\n");

	/* SEQUENCE */
	output_token(out, ":SetBackgroundColour");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetBackgroundColour");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetBackgroundColour");
		left -= tag.length;
	}
	else
	{
		return der_error("SetBackgroundColour: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetBackgroundColour");
	left -= sublen;

	if(is_NewColour(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewColour(der, out, sublen + tag.length)) < 0)
			return der_error("SetBackgroundColour");
		left -= tag.length;
	}
	else
	{
		return der_error("SetBackgroundColour: missing NewColour");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetBackgroundColour: %d bytes left", left);

	verbose("</SetBackgroundColour>\n");

	return length;
}

int asn1decode_SetCellPosition(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetCellPosition>\n");

	/* SEQUENCE */
	output_token(out, ":SetCellPosition");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetCellPosition");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetCellPosition");
		left -= tag.length;
	}
	else
	{
		return der_error("SetCellPosition: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetCellPosition");
	left -= sublen;

	if(is_Index(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Index(der, out, sublen + tag.length)) < 0)
			return der_error("SetCellPosition");
		left -= tag.length;
	}
	else
	{
		return der_error("SetCellPosition: missing Index");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetCellPosition");
	left -= sublen;

	if(is_XPosition(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_XPosition(der, out, sublen + tag.length)) < 0)
			return der_error("SetCellPosition");
		left -= tag.length;
	}
	else
	{
		return der_error("SetCellPosition: missing XPosition");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetCellPosition");
	left -= sublen;

	if(is_YPosition(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_YPosition(der, out, sublen + tag.length)) < 0)
			return der_error("SetCellPosition");
		left -= tag.length;
	}
	else
	{
		return der_error("SetCellPosition: missing YPosition");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetCellPosition: %d bytes left", left);

	verbose("</SetCellPosition>\n");

	return length;
}

int asn1decode_SetInputReg(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetInputReg>\n");

	/* SEQUENCE */
	output_token(out, ":SetInputReg");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetInputReg");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetInputReg");
		left -= tag.length;
	}
	else
	{
		return der_error("SetInputReg: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetInputReg");
	left -= sublen;

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, sublen + tag.length)) < 0)
			return der_error("SetInputReg");
		left -= tag.length;
	}
	else
	{
		return der_error("SetInputReg: missing GenericInteger");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetInputReg: %d bytes left", left);

	verbose("</SetInputReg>\n");

	return length;
}

int asn1decode_SetTextColour(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetTextColour>\n");

	/* SEQUENCE */
	output_token(out, ":SetTextColour");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetTextColour");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetTextColour");
		left -= tag.length;
	}
	else
	{
		return der_error("SetTextColour: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetTextColour");
	left -= sublen;

	if(is_NewColour(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewColour(der, out, sublen + tag.length)) < 0)
			return der_error("SetTextColour");
		left -= tag.length;
	}
	else
	{
		return der_error("SetTextColour: missing NewColour");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetTextColour: %d bytes left", left);

	verbose("</SetTextColour>\n");

	return length;
}

int asn1decode_SetFontAttributes(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetFontAttributes>\n");

	/* SEQUENCE */
	output_token(out, ":SetFontAttributes");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetFontAttributes");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetFontAttributes");
		left -= tag.length;
	}
	else
	{
		return der_error("SetFontAttributes: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetFontAttributes");
	left -= sublen;

	if(is_GenericOctetString(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericOctetString(der, out, sublen + tag.length)) < 0)
			return der_error("SetFontAttributes");
		left -= tag.length;
	}
	else
	{
		return der_error("SetFontAttributes: missing GenericOctetString");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetFontAttributes: %d bytes left", left);

	verbose("</SetFontAttributes>\n");

	return length;
}

int asn1decode_SetVideoDecodeOffset(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetVideoDecodeOffset>\n");

	/* SEQUENCE */
	output_token(out, ":SetVideoDecodeOffset");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetVideoDecodeOffset");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetVideoDecodeOffset");
		left -= tag.length;
	}
	else
	{
		return der_error("SetVideoDecodeOffset: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetVideoDecodeOffset");
	left -= sublen;

	if(is_NewXOffset(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewXOffset(der, out, sublen + tag.length)) < 0)
			return der_error("SetVideoDecodeOffset");
		left -= tag.length;
	}
	else
	{
		return der_error("SetVideoDecodeOffset: missing NewXOffset");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetVideoDecodeOffset");
	left -= sublen;

	if(is_NewYOffset(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewYOffset(der, out, sublen + tag.length)) < 0)
			return der_error("SetVideoDecodeOffset");
		left -= tag.length;
	}
	else
	{
		return der_error("SetVideoDecodeOffset: missing NewYOffset");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetVideoDecodeOffset: %d bytes left", left);

	verbose("</SetVideoDecodeOffset>\n");

	return length;
}

int asn1decode_GetVideoDecodeOffset(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GetVideoDecodeOffset>\n");

	/* SEQUENCE */
	output_token(out, ":GetVideoDecodeOffset");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetVideoDecodeOffset");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("GetVideoDecodeOffset");
		left -= tag.length;
	}
	else
	{
		return der_error("GetVideoDecodeOffset: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetVideoDecodeOffset");
	left -= sublen;

	if(is_XOffsetVar(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_XOffsetVar(der, out, sublen + tag.length)) < 0)
			return der_error("GetVideoDecodeOffset");
		left -= tag.length;
	}
	else
	{
		return der_error("GetVideoDecodeOffset: missing XOffsetVar");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetVideoDecodeOffset");
	left -= sublen;

	if(is_YOffsetVar(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_YOffsetVar(der, out, sublen + tag.length)) < 0)
			return der_error("GetVideoDecodeOffset");
		left -= tag.length;
	}
	else
	{
		return der_error("GetVideoDecodeOffset: missing YOffsetVar");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("GetVideoDecodeOffset: %d bytes left", left);

	verbose("</GetVideoDecodeOffset>\n");

	return length;
}

int asn1decode_GetFocusPosition(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GetFocusPosition>\n");

	/* SEQUENCE */
	output_token(out, ":GetFocusPosition");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetFocusPosition");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("GetFocusPosition");
		left -= tag.length;
	}
	else
	{
		return der_error("GetFocusPosition: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetFocusPosition");
	left -= sublen;

	if(is_FocusPositionVar(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_FocusPositionVar(der, out, sublen + tag.length)) < 0)
			return der_error("GetFocusPosition");
		left -= tag.length;
	}
	else
	{
		return der_error("GetFocusPosition: missing FocusPositionVar");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("GetFocusPosition: %d bytes left", left);

	verbose("</GetFocusPosition>\n");

	return length;
}

int asn1decode_SetFocusPosition(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetFocusPosition>\n");

	/* SEQUENCE */
	output_token(out, ":SetFocusPosition");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetFocusPosition");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetFocusPosition");
		left -= tag.length;
	}
	else
	{
		return der_error("SetFocusPosition: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetFocusPosition");
	left -= sublen;

	if(is_NewFocusPosition(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewFocusPosition(der, out, sublen + tag.length)) < 0)
			return der_error("SetFocusPosition");
		left -= tag.length;
	}
	else
	{
		return der_error("SetFocusPosition: missing NewFocusPosition");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetFocusPosition: %d bytes left", left);

	verbose("</SetFocusPosition>\n");

	return length;
}

int asn1decode_SetBitmapDecodeOffset(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetBitmapDecodeOffset>\n");

	/* SEQUENCE */
	output_token(out, ":SetBitmapDecodeOffset");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetBitmapDecodeOffset");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetBitmapDecodeOffset");
		left -= tag.length;
	}
	else
	{
		return der_error("SetBitmapDecodeOffset: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetBitmapDecodeOffset");
	left -= sublen;

	if(is_NewXOffset(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewXOffset(der, out, sublen + tag.length)) < 0)
			return der_error("SetBitmapDecodeOffset");
		left -= tag.length;
	}
	else
	{
		return der_error("SetBitmapDecodeOffset: missing NewXOffset");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetBitmapDecodeOffset");
	left -= sublen;

	if(is_NewYOffset(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewYOffset(der, out, sublen + tag.length)) < 0)
			return der_error("SetBitmapDecodeOffset");
		left -= tag.length;
	}
	else
	{
		return der_error("SetBitmapDecodeOffset: missing NewYOffset");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetBitmapDecodeOffset: %d bytes left", left);

	verbose("</SetBitmapDecodeOffset>\n");

	return length;
}

int asn1decode_GetBitmapDecodeOffset(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GetBitmapDecodeOffset>\n");

	/* SEQUENCE */
	output_token(out, ":GetBitmapDecodeOffset");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetBitmapDecodeOffset");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("GetBitmapDecodeOffset");
		left -= tag.length;
	}
	else
	{
		return der_error("GetBitmapDecodeOffset: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetBitmapDecodeOffset");
	left -= sublen;

	if(is_XOffsetVar(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_XOffsetVar(der, out, sublen + tag.length)) < 0)
			return der_error("GetBitmapDecodeOffset");
		left -= tag.length;
	}
	else
	{
		return der_error("GetBitmapDecodeOffset: missing XOffsetVar");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GetBitmapDecodeOffset");
	left -= sublen;

	if(is_YOffsetVar(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_YOffsetVar(der, out, sublen + tag.length)) < 0)
			return der_error("GetBitmapDecodeOffset");
		left -= tag.length;
	}
	else
	{
		return der_error("GetBitmapDecodeOffset: missing YOffsetVar");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("GetBitmapDecodeOffset: %d bytes left", left);

	verbose("</GetBitmapDecodeOffset>\n");

	return length;
}

int asn1decode_SetSliderParameters(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SetSliderParameters>\n");

	/* SEQUENCE */
	output_token(out, ":SetSliderParameters");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetSliderParameters");
	left -= sublen;

	if(is_Target(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Target(der, out, sublen + tag.length)) < 0)
			return der_error("SetSliderParameters");
		left -= tag.length;
	}
	else
	{
		return der_error("SetSliderParameters: missing Target");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetSliderParameters");
	left -= sublen;

	if(is_NewMinValue(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewMinValue(der, out, sublen + tag.length)) < 0)
			return der_error("SetSliderParameters");
		left -= tag.length;
	}
	else
	{
		return der_error("SetSliderParameters: missing NewMinValue");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetSliderParameters");
	left -= sublen;

	if(is_NewMaxValue(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewMaxValue(der, out, sublen + tag.length)) < 0)
			return der_error("SetSliderParameters");
		left -= tag.length;
	}
	else
	{
		return der_error("SetSliderParameters: missing NewMaxValue");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SetSliderParameters");
	left -= sublen;

	if(is_NewStepSize(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewStepSize(der, out, sublen + tag.length)) < 0)
			return der_error("SetSliderParameters");
		left -= tag.length;
	}
	else
	{
		return der_error("SetSliderParameters: missing NewStepSize");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("SetSliderParameters: %d bytes left", left);

	verbose("</SetSliderParameters>\n");

	return length;
}

int asn1decode_AbsoluteTime(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<AbsoluteTime>\n");

	output_token(out, ":AbsoluteTime");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("AbsoluteTime");

	if(is_GenericBoolean(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericBoolean(der, out, length)) < 0)
			return der_error("AbsoluteTime");
		left -= sublen;
	}
	else
	{
		return der_error("AbsoluteTime: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("AbsoluteTime: %d bytes left", left);

	verbose("</AbsoluteTime>\n");

	return length;
}

int asn1decode_Address(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Address>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Address");

	if(is_GenericOctetString(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericOctetString(der, out, length)) < 0)
			return der_error("Address");
		left -= sublen;
	}
	else
	{
		return der_error("Address: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("Address: %d bytes left", left);

	verbose("</Address>\n");

	return length;
}

int asn1decode_Answer(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Answer>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Answer");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("Answer");
		left -= sublen;
	}
	else
	{
		return der_error("Answer: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("Answer: %d bytes left", left);

	verbose("</Answer>\n");

	return length;
}

int asn1decode_AppendValue(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<AppendValue>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("AppendValue");

	if(is_GenericOctetString(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericOctetString(der, out, length)) < 0)
			return der_error("AppendValue");
		left -= sublen;
	}
	else
	{
		return der_error("AppendValue: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("AppendValue: %d bytes left", left);

	verbose("</AppendValue>\n");

	return length;
}

int asn1decode_ArcAngle(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ArcAngle>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ArcAngle");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("ArcAngle");
		left -= sublen;
	}
	else
	{
		return der_error("ArcAngle: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("ArcAngle: %d bytes left", left);

	verbose("</ArcAngle>\n");

	return length;
}

int asn1decode_AvailabilityStatusVar(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<AvailabilityStatusVar>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("AvailabilityStatusVar");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("AvailabilityStatusVar");
		left -= sublen;
	}
	else
	{
		return der_error("AvailabilityStatusVar: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("AvailabilityStatusVar: %d bytes left", left);

	verbose("</AvailabilityStatusVar>\n");

	return length;
}

int asn1decode_CallSucceeded(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<CallSucceeded>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("CallSucceeded");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("CallSucceeded");
		left -= sublen;
	}
	else
	{
		return der_error("CallSucceeded: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("CallSucceeded: %d bytes left", left);

	verbose("</CallSucceeded>\n");

	return length;
}

int asn1decode_CellIndex(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<CellIndex>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("CellIndex");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("CellIndex");
		left -= sublen;
	}
	else
	{
		return der_error("CellIndex: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("CellIndex: %d bytes left", left);

	verbose("</CellIndex>\n");

	return length;
}

int asn1decode_CloneRefVar(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<CloneRefVar>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("CloneRefVar");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("CloneRefVar");
		left -= sublen;
	}
	else
	{
		return der_error("CloneRefVar: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("CloneRefVar: %d bytes left", left);

	verbose("</CloneRefVar>\n");

	return length;
}

int asn1decode_ComparisonValue(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ComparisonValue>\n");

	/* CHOICE */
	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ComparisonValue");
	left -= sublen;

	if(is_NewGenericBoolean(tag.class, tag.number))
	{
		if((sublen = asn1decode_NewGenericBoolean(der, out, tag.length)) < 0)
			return der_error("ComparisonValue");
		left -= tag.length;
	}
	else if(is_NewGenericInteger(tag.class, tag.number))
	{
		if((sublen = asn1decode_NewGenericInteger(der, out, tag.length)) < 0)
			return der_error("ComparisonValue");
		left -= tag.length;
	}
	else if(is_NewGenericOctetString(tag.class, tag.number))
	{
		if((sublen = asn1decode_NewGenericOctetString(der, out, tag.length)) < 0)
			return der_error("ComparisonValue");
		left -= tag.length;
	}
	else if(is_NewGenericObjectReference(tag.class, tag.number))
	{
		if((sublen = asn1decode_NewGenericObjectReference(der, out, tag.length)) < 0)
			return der_error("ComparisonValue");
		left -= tag.length;
	}
	else if(is_NewGenericContentReference(tag.class, tag.number))
	{
		if((sublen = asn1decode_NewGenericContentReference(der, out, tag.length)) < 0)
			return der_error("ComparisonValue");
		left -= tag.length;
	}
	else
	{
		return der_error("ComparisonValue");
	}

	if(left != 0)
		return der_error("ComparisonValue: %d bytes left", left);

	verbose("</ComparisonValue>\n");

	return length;
}

int asn1decode_ConnectionTag(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ConnectionTag>\n");

	output_token(out, ":ConnectionTag");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ConnectionTag");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("ConnectionTag");
		left -= sublen;
	}
	else
	{
		return der_error("ConnectionTag: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("ConnectionTag: %d bytes left", left);

	verbose("</ConnectionTag>\n");

	return length;
}

int asn1decode_Denominator(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Denominator>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Denominator");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("Denominator");
		left -= sublen;
	}
	else
	{
		return der_error("Denominator: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("Denominator: %d bytes left", left);

	verbose("</Denominator>\n");

	return length;
}

int asn1decode_EllipseHeight(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<EllipseHeight>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("EllipseHeight");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("EllipseHeight");
		left -= sublen;
	}
	else
	{
		return der_error("EllipseHeight: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("EllipseHeight: %d bytes left", left);

	verbose("</EllipseHeight>\n");

	return length;
}

int asn1decode_EllipseWidth(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<EllipseWidth>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("EllipseWidth");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("EllipseWidth");
		left -= sublen;
	}
	else
	{
		return der_error("EllipseWidth: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("EllipseWidth: %d bytes left", left);

	verbose("</EllipseWidth>\n");

	return length;
}

int asn1decode_EmulatedEventData(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<EmulatedEventData>\n");

	/* CHOICE */
	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("EmulatedEventData");
	left -= sublen;

	if(is_NewGenericBoolean(tag.class, tag.number))
	{
		if((sublen = asn1decode_NewGenericBoolean(der, out, tag.length)) < 0)
			return der_error("EmulatedEventData");
		left -= tag.length;
	}
	else if(is_NewGenericInteger(tag.class, tag.number))
	{
		if((sublen = asn1decode_NewGenericInteger(der, out, tag.length)) < 0)
			return der_error("EmulatedEventData");
		left -= tag.length;
	}
	else if(is_NewGenericOctetString(tag.class, tag.number))
	{
		if((sublen = asn1decode_NewGenericOctetString(der, out, tag.length)) < 0)
			return der_error("EmulatedEventData");
		left -= tag.length;
	}
	else
	{
		return der_error("EmulatedEventData");
	}

	if(left != 0)
		return der_error("EmulatedEventData: %d bytes left", left);

	verbose("</EmulatedEventData>\n");

	return length;
}

int asn1decode_EmulatedEventSource(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<EmulatedEventSource>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("EmulatedEventSource");

	if(is_GenericObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericObjectReference(der, out, length)) < 0)
			return der_error("EmulatedEventSource");
		left -= sublen;
	}
	else
	{
		return der_error("EmulatedEventSource: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("EmulatedEventSource: %d bytes left", left);

	verbose("</EmulatedEventSource>\n");

	return length;
}

int asn1decode_EmulatedEventType(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<EmulatedEventType>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("EmulatedEventType");

	if(is_EventTypeEnum(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_EventTypeEnum(der, out, length)) < 0)
			return der_error("EmulatedEventType");
		left -= sublen;
	}
	else
	{
		return der_error("EmulatedEventType: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("EmulatedEventType: %d bytes left", left);

	verbose("</EmulatedEventType>\n");

	return length;
}

int asn1decode_EntryPointVar(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<EntryPointVar>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("EntryPointVar");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("EntryPointVar");
		left -= sublen;
	}
	else
	{
		return der_error("EntryPointVar: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("EntryPointVar: %d bytes left", left);

	verbose("</EntryPointVar>\n");

	return length;
}

int asn1decode_ForkSucceeded(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ForkSucceeded>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ForkSucceeded");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("ForkSucceeded");
		left -= sublen;
	}
	else
	{
		return der_error("ForkSucceeded: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("ForkSucceeded: %d bytes left", left);

	verbose("</ForkSucceeded>\n");

	return length;
}

int asn1decode_Feature(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Feature>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Feature");

	if(is_GenericOctetString(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericOctetString(der, out, length)) < 0)
			return der_error("Feature");
		left -= sublen;
	}
	else
	{
		return der_error("Feature: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("Feature: %d bytes left", left);

	verbose("</Feature>\n");

	return length;
}

int asn1decode_FillColourVar(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<FillColourVar>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("FillColourVar");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("FillColourVar");
		left -= sublen;
	}
	else
	{
		return der_error("FillColourVar: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("FillColourVar: %d bytes left", left);

	verbose("</FillColourVar>\n");

	return length;
}

int asn1decode_FirstItemVar(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<FirstItemVar>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("FirstItemVar");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("FirstItemVar");
		left -= sublen;
	}
	else
	{
		return der_error("FirstItemVar: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("FirstItemVar: %d bytes left", left);

	verbose("</FirstItemVar>\n");

	return length;
}

int asn1decode_HighlightStatusVar(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<HighlightStatusVar>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("HighlightStatusVar");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("HighlightStatusVar");
		left -= sublen;
	}
	else
	{
		return der_error("HighlightStatusVar: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("HighlightStatusVar: %d bytes left", left);

	verbose("</HighlightStatusVar>\n");

	return length;
}

int asn1decode_Index(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Index>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Index");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("Index");
		left -= sublen;
	}
	else
	{
		return der_error("Index: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("Index: %d bytes left", left);

	verbose("</Index>\n");

	return length;
}

int asn1decode_InFileName(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<InFileName>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("InFileName");

	if(is_GenericOctetString(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericOctetString(der, out, length)) < 0)
			return der_error("InFileName");
		left -= sublen;
	}
	else
	{
		return der_error("InFileName: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("InFileName: %d bytes left", left);

	verbose("</InFileName>\n");

	return length;
}

int asn1decode_InteractionStatusVar(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<InteractionStatusVar>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("InteractionStatusVar");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("InteractionStatusVar");
		left -= sublen;
	}
	else
	{
		return der_error("InteractionStatusVar: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("InteractionStatusVar: %d bytes left", left);

	verbose("</InteractionStatusVar>\n");

	return length;
}

int asn1decode_InVariables(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<InVariables>\n");

	output_token(out, "(");

	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("InVariables");

		if(is_ObjectReference(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_ObjectReference(der, out, sublen + tag.length)) < 0)
				return der_error("InVariables");
			left -= sublen;
		}
		else
		{
			return der_error("InVariables: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("InVariables: %d bytes left", left);

	verbose("</InVariables>\n");

	return length;
}

int asn1decode_ItemIndex(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ItemIndex>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ItemIndex");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("ItemIndex");
		left -= sublen;
	}
	else
	{
		return der_error("ItemIndex: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("ItemIndex: %d bytes left", left);

	verbose("</ItemIndex>\n");

	return length;
}

int asn1decode_ItemRefVar(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ItemRefVar>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ItemRefVar");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("ItemRefVar");
		left -= sublen;
	}
	else
	{
		return der_error("ItemRefVar: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("ItemRefVar: %d bytes left", left);

	verbose("</ItemRefVar>\n");

	return length;
}

int asn1decode_ItemStatusVar(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ItemStatusVar>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ItemStatusVar");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("ItemStatusVar");
		left -= sublen;
	}
	else
	{
		return der_error("ItemStatusVar: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("ItemStatusVar: %d bytes left", left);

	verbose("</ItemStatusVar>\n");

	return length;
}

int asn1decode_ItemsToScroll(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ItemsToScroll>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ItemsToScroll");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("ItemsToScroll");
		left -= sublen;
	}
	else
	{
		return der_error("ItemsToScroll: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("ItemsToScroll: %d bytes left", left);

	verbose("</ItemsToScroll>\n");

	return length;
}

int asn1decode_LabelVar(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<LabelVar>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("LabelVar");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("LabelVar");
		left -= sublen;
	}
	else
	{
		return der_error("LabelVar: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("LabelVar: %d bytes left", left);

	verbose("</LabelVar>\n");

	return length;
}

int asn1decode_LastAnchorFiredVar(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<LastAnchorFiredVar>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("LastAnchorFiredVar");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("LastAnchorFiredVar");
		left -= sublen;
	}
	else
	{
		return der_error("LastAnchorFiredVar: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("LastAnchorFiredVar: %d bytes left", left);

	verbose("</LastAnchorFiredVar>\n");

	return length;
}

int asn1decode_LineColourVar(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<LineColourVar>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("LineColourVar");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("LineColourVar");
		left -= sublen;
	}
	else
	{
		return der_error("LineColourVar: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("LineColourVar: %d bytes left", left);

	verbose("</LineColourVar>\n");

	return length;
}

int asn1decode_LineStyleVar(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<LineStyleVar>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("LineStyleVar");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("LineStyleVar");
		left -= sublen;
	}
	else
	{
		return der_error("LineStyleVar: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("LineStyleVar: %d bytes left", left);

	verbose("</LineStyleVar>\n");

	return length;
}

int asn1decode_LineWidthVar(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<LineWidthVar>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("LineWidthVar");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("LineWidthVar");
		left -= sublen;
	}
	else
	{
		return der_error("LineWidthVar: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("LineWidthVar: %d bytes left", left);

	verbose("</LineWidthVar>\n");

	return length;
}

int asn1decode_MovementIdentifier(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<MovementIdentifier>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("MovementIdentifier");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("MovementIdentifier");
		left -= sublen;
	}
	else
	{
		return der_error("MovementIdentifier: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("MovementIdentifier: %d bytes left", left);

	verbose("</MovementIdentifier>\n");

	return length;
}

int asn1decode_NbOfSteps(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NbOfSteps>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NbOfSteps");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("NbOfSteps");
		left -= sublen;
	}
	else
	{
		return der_error("NbOfSteps: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NbOfSteps: %d bytes left", left);

	verbose("</NbOfSteps>\n");

	return length;
}

int asn1decode_NewAbsoluteColour(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewAbsoluteColour>\n");

	output_token(out, ":NewAbsoluteColour");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewAbsoluteColour");

	if(is_GenericOctetString(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericOctetString(der, out, length)) < 0)
			return der_error("NewAbsoluteColour");
		left -= sublen;
	}
	else
	{
		return der_error("NewAbsoluteColour: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewAbsoluteColour: %d bytes left", left);

	verbose("</NewAbsoluteColour>\n");

	return length;
}

int asn1decode_NewCachePriority(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewCachePriority>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewCachePriority");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("NewCachePriority");
		left -= sublen;
	}
	else
	{
		return der_error("NewCachePriority: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewCachePriority: %d bytes left", left);

	verbose("</NewCachePriority>\n");

	return length;
}

int asn1decode_NewColour(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewColour>\n");

	/* CHOICE */
	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewColour");
	left -= sublen;

	if(is_NewColourIndex(tag.class, tag.number))
	{
		if((sublen = asn1decode_NewColourIndex(der, out, tag.length)) < 0)
			return der_error("NewColour");
		left -= tag.length;
	}
	else if(is_NewAbsoluteColour(tag.class, tag.number))
	{
		if((sublen = asn1decode_NewAbsoluteColour(der, out, tag.length)) < 0)
			return der_error("NewColour");
		left -= tag.length;
	}
	else
	{
		return der_error("NewColour");
	}

	if(left != 0)
		return der_error("NewColour: %d bytes left", left);

	verbose("</NewColour>\n");

	return length;
}

int asn1decode_NewColourIndex(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewColourIndex>\n");

	output_token(out, ":NewColourIndex");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewColourIndex");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("NewColourIndex");
		left -= sublen;
	}
	else
	{
		return der_error("NewColourIndex: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewColourIndex: %d bytes left", left);

	verbose("</NewColourIndex>\n");

	return length;
}

int asn1decode_NewContent(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewContent>\n");

	/* CHOICE */
	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewContent");
	left -= sublen;

	if(is_NewIncludedContent(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_NewIncludedContent(der, out, sublen + tag.length)) < 0)
			return der_error("NewContent");
		left -= tag.length;
	}
	else if(is_NewReferencedContent(tag.class, tag.number))
	{
		if((sublen = asn1decode_NewReferencedContent(der, out, tag.length)) < 0)
			return der_error("NewContent");
		left -= tag.length;
	}
	else
	{
		return der_error("NewContent");
	}

	if(left != 0)
		return der_error("NewContent: %d bytes left", left);

	verbose("</NewContent>\n");

	return length;
}

int asn1decode_NewContentCachePriority(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewContentCachePriority>\n");

	output_token(out, ":NewCCPriority");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewContentCachePriority");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("NewContentCachePriority");
		left -= sublen;
	}
	else
	{
		return der_error("NewContentCachePriority: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewContentCachePriority: %d bytes left", left);

	verbose("</NewContentCachePriority>\n");

	return length;
}

int asn1decode_NewCounterEndPosition(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewCounterEndPosition>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewCounterEndPosition");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("NewCounterEndPosition");
		left -= sublen;
	}
	else
	{
		return der_error("NewCounterEndPosition: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewCounterEndPosition: %d bytes left", left);

	verbose("</NewCounterEndPosition>\n");

	return length;
}

int asn1decode_NewCounterPosition(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewCounterPosition>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewCounterPosition");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("NewCounterPosition");
		left -= sublen;
	}
	else
	{
		return der_error("NewCounterPosition: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewCounterPosition: %d bytes left", left);

	verbose("</NewCounterPosition>\n");

	return length;
}

int asn1decode_NewContentSize(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewContentSize>\n");

	output_token(out, ":NewContentSize");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewContentSize");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("NewContentSize");
		left -= sublen;
	}
	else
	{
		return der_error("NewContentSize: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewContentSize: %d bytes left", left);

	verbose("</NewContentSize>\n");

	return length;
}

int asn1decode_NewCounterValue(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewCounterValue>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewCounterValue");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("NewCounterValue");
		left -= sublen;
	}
	else
	{
		return der_error("NewCounterValue: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewCounterValue: %d bytes left", left);

	verbose("</NewCounterValue>\n");

	return length;
}

int asn1decode_NewCursorShape(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewCursorShape>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewCursorShape");

	if(is_GenericObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericObjectReference(der, out, length)) < 0)
			return der_error("NewCursorShape");
		left -= sublen;
	}
	else
	{
		return der_error("NewCursorShape: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewCursorShape: %d bytes left", left);

	verbose("</NewCursorShape>\n");

	return length;
}

int asn1decode_NewEntryPoint(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewEntryPoint>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewEntryPoint");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("NewEntryPoint");
		left -= sublen;
	}
	else
	{
		return der_error("NewEntryPoint: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewEntryPoint: %d bytes left", left);

	verbose("</NewEntryPoint>\n");

	return length;
}

int asn1decode_NewFirstItem(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewFirstItem>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewFirstItem");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("NewFirstItem");
		left -= sublen;
	}
	else
	{
		return der_error("NewFirstItem: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewFirstItem: %d bytes left", left);

	verbose("</NewFirstItem>\n");

	return length;
}

int asn1decode_NewFont(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewFont>\n");

	/* CHOICE */
	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewFont");
	left -= sublen;

	if(is_NewFontName(tag.class, tag.number))
	{
		if((sublen = asn1decode_NewFontName(der, out, tag.length)) < 0)
			return der_error("NewFont");
		left -= tag.length;
	}
	else if(is_NewFontReference(tag.class, tag.number))
	{
		if((sublen = asn1decode_NewFontReference(der, out, tag.length)) < 0)
			return der_error("NewFont");
		left -= tag.length;
	}
	else
	{
		return der_error("NewFont");
	}

	if(left != 0)
		return der_error("NewFont: %d bytes left", left);

	verbose("</NewFont>\n");

	return length;
}

int asn1decode_NewFontName(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewFontName>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewFontName");

	if(is_NewGenericOctetString(tag.class, tag.number))
	{
		left -= sublen;
		if((sublen = asn1decode_NewGenericOctetString(der, out, tag.length)) < 0)
			return der_error("NewFontName");
		left -= sublen;
	}
	else
	{
		return der_error("NewFontName: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewFontName: %d bytes left", left);

	verbose("</NewFontName>\n");

	return length;
}

int asn1decode_NewFontReference(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewFontReference>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewFontReference");

	if(is_NewGenericObjectReference(tag.class, tag.number))
	{
		left -= sublen;
		if((sublen = asn1decode_NewGenericObjectReference(der, out, tag.length)) < 0)
			return der_error("NewFontReference");
		left -= sublen;
	}
	else
	{
		return der_error("NewFontReference: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewFontReference: %d bytes left", left);

	verbose("</NewFontReference>\n");

	return length;
}

int asn1decode_NewGenericBoolean(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewGenericBoolean>\n");

	output_token(out, ":GBoolean");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewGenericBoolean");

	if(is_GenericBoolean(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericBoolean(der, out, length)) < 0)
			return der_error("NewGenericBoolean");
		left -= sublen;
	}
	else
	{
		return der_error("NewGenericBoolean: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewGenericBoolean: %d bytes left", left);

	verbose("</NewGenericBoolean>\n");

	return length;
}

int asn1decode_NewGenericInteger(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewGenericInteger>\n");

	output_token(out, ":GInteger");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewGenericInteger");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("NewGenericInteger");
		left -= sublen;
	}
	else
	{
		return der_error("NewGenericInteger: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewGenericInteger: %d bytes left", left);

	verbose("</NewGenericInteger>\n");

	return length;
}

int asn1decode_NewGenericOctetString(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewGenericOctetString>\n");

	output_token(out, ":GOctetString");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewGenericOctetString");

	if(is_GenericOctetString(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericOctetString(der, out, length)) < 0)
			return der_error("NewGenericOctetString");
		left -= sublen;
	}
	else
	{
		return der_error("NewGenericOctetString: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewGenericOctetString: %d bytes left", left);

	verbose("</NewGenericOctetString>\n");

	return length;
}

int asn1decode_NewGenericObjectReference(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewGenericObjectReference>\n");

	output_token(out, ":GObjectRef");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewGenericObjectReference");

	if(is_GenericObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericObjectReference(der, out, length)) < 0)
			return der_error("NewGenericObjectReference");
		left -= sublen;
	}
	else
	{
		return der_error("NewGenericObjectReference: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewGenericObjectReference: %d bytes left", left);

	verbose("</NewGenericObjectReference>\n");

	return length;
}

int asn1decode_NewGenericContentReference(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewGenericContentReference>\n");

	output_token(out, ":GContentRef");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewGenericContentReference");

	if(is_GenericContentReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericContentReference(der, out, length)) < 0)
			return der_error("NewGenericContentReference");
		left -= sublen;
	}
	else
	{
		return der_error("NewGenericContentReference: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewGenericContentReference: %d bytes left", left);

	verbose("</NewGenericContentReference>\n");

	return length;
}

int asn1decode_NewHighlightStatus(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewHighlightStatus>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewHighlightStatus");

	if(is_GenericBoolean(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericBoolean(der, out, length)) < 0)
			return der_error("NewHighlightStatus");
		left -= sublen;
	}
	else
	{
		return der_error("NewHighlightStatus: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewHighlightStatus: %d bytes left", left);

	verbose("</NewHighlightStatus>\n");

	return length;
}

int asn1decode_NewIncludedContent(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewIncludedContent>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewIncludedContent");

	if(is_GenericOctetString(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericOctetString(der, out, length)) < 0)
			return der_error("NewIncludedContent");
		left -= sublen;
	}
	else
	{
		return der_error("NewIncludedContent: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewIncludedContent: %d bytes left", left);

	verbose("</NewIncludedContent>\n");

	return length;
}

int asn1decode_NewInteractionStatus(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewInteractionStatus>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewInteractionStatus");

	if(is_GenericBoolean(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericBoolean(der, out, length)) < 0)
			return der_error("NewInteractionStatus");
		left -= sublen;
	}
	else
	{
		return der_error("NewInteractionStatus: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewInteractionStatus: %d bytes left", left);

	verbose("</NewInteractionStatus>\n");

	return length;
}

int asn1decode_NewLabel(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewLabel>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewLabel");

	if(is_GenericOctetString(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericOctetString(der, out, length)) < 0)
			return der_error("NewLabel");
		left -= sublen;
	}
	else
	{
		return der_error("NewLabel: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewLabel: %d bytes left", left);

	verbose("</NewLabel>\n");

	return length;
}

int asn1decode_NewLineStyle(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewLineStyle>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewLineStyle");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("NewLineStyle");
		left -= sublen;
	}
	else
	{
		return der_error("NewLineStyle: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewLineStyle: %d bytes left", left);

	verbose("</NewLineStyle>\n");

	return length;
}

int asn1decode_NewLineWidth(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewLineWidth>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewLineWidth");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("NewLineWidth");
		left -= sublen;
	}
	else
	{
		return der_error("NewLineWidth: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewLineWidth: %d bytes left", left);

	verbose("</NewLineWidth>\n");

	return length;
}

int asn1decode_NewOverwriteMode(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewOverwriteMode>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewOverwriteMode");

	if(is_GenericBoolean(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericBoolean(der, out, length)) < 0)
			return der_error("NewOverwriteMode");
		left -= sublen;
	}
	else
	{
		return der_error("NewOverwriteMode: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewOverwriteMode: %d bytes left", left);

	verbose("</NewOverwriteMode>\n");

	return length;
}

int asn1decode_NewPaletteRef(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewPaletteRef>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewPaletteRef");

	if(is_GenericObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericObjectReference(der, out, length)) < 0)
			return der_error("NewPaletteRef");
		left -= sublen;
	}
	else
	{
		return der_error("NewPaletteRef: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewPaletteRef: %d bytes left", left);

	verbose("</NewPaletteRef>\n");

	return length;
}

int asn1decode_NewPortion(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewPortion>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewPortion");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("NewPortion");
		left -= sublen;
	}
	else
	{
		return der_error("NewPortion: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewPortion: %d bytes left", left);

	verbose("</NewPortion>\n");

	return length;
}

int asn1decode_NewReferencedContent(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewReferencedContent>\n");

	/* SEQUENCE */
	output_token(out, ":NewRefContent");

	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewReferencedContent");
	left -= sublen;

	if(is_GenericContentReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericContentReference(der, out, sublen + tag.length)) < 0)
			return der_error("NewReferencedContent");
		left -= tag.length;
	}
	else
	{
		return der_error("NewReferencedContent: missing GenericContentReference");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewReferencedContent");
	left -= sublen;

	if(is_NewContentSize(tag.class, tag.number))
	{
		struct der_tag null_tag;
		if((sublen = der_decode_Tag(der, &null_tag)) < 0)
			return der_error("NewReferencedContent");
		if(is_Null(null_tag.class, null_tag.number))
		{
			left -= sublen;
		}
		else
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_NewContentSize(der, out, tag.length)) < 0)
				return der_error("NewReferencedContent");
			left -= tag.length;
		}
	}
	else
	{
		return der_error("NewReferencedContent: missing NewContentSize");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewReferencedContent");
	left -= sublen;

	if(is_NewContentCachePriority(tag.class, tag.number))
	{
		if((sublen = asn1decode_NewContentCachePriority(der, out, tag.length)) < 0)
			return der_error("NewReferencedContent");
		left -= tag.length;
	}
	else
	{
		fseek(der, -sublen, SEEK_CUR);
		left += sublen;
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("NewReferencedContent: %d bytes left", left);

	verbose("</NewReferencedContent>\n");

	return length;
}

int asn1decode_NewSliderValue(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewSliderValue>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewSliderValue");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("NewSliderValue");
		left -= sublen;
	}
	else
	{
		return der_error("NewSliderValue: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewSliderValue: %d bytes left", left);

	verbose("</NewSliderValue>\n");

	return length;
}

int asn1decode_NewSpeed(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewSpeed>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewSpeed");

	if(is_Rational(tag.class, tag.number))
	{
		left -= sublen;
		if((sublen = asn1decode_Rational(der, out, tag.length)) < 0)
			return der_error("NewSpeed");
		left -= sublen;
	}
	else
	{
		return der_error("NewSpeed: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewSpeed: %d bytes left", left);

	verbose("</NewSpeed>\n");

	return length;
}

int asn1decode_NewTransparency(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewTransparency>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewTransparency");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("NewTransparency");
		left -= sublen;
	}
	else
	{
		return der_error("NewTransparency: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewTransparency: %d bytes left", left);

	verbose("</NewTransparency>\n");

	return length;
}

int asn1decode_NewVariableValue(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewVariableValue>\n");

	/* CHOICE */
	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewVariableValue");
	left -= sublen;

	if(is_NewGenericInteger(tag.class, tag.number))
	{
		if((sublen = asn1decode_NewGenericInteger(der, out, tag.length)) < 0)
			return der_error("NewVariableValue");
		left -= tag.length;
	}
	else if(is_NewGenericBoolean(tag.class, tag.number))
	{
		if((sublen = asn1decode_NewGenericBoolean(der, out, tag.length)) < 0)
			return der_error("NewVariableValue");
		left -= tag.length;
	}
	else if(is_NewGenericOctetString(tag.class, tag.number))
	{
		if((sublen = asn1decode_NewGenericOctetString(der, out, tag.length)) < 0)
			return der_error("NewVariableValue");
		left -= tag.length;
	}
	else if(is_NewGenericObjectReference(tag.class, tag.number))
	{
		if((sublen = asn1decode_NewGenericObjectReference(der, out, tag.length)) < 0)
			return der_error("NewVariableValue");
		left -= tag.length;
	}
	else if(is_NewGenericContentReference(tag.class, tag.number))
	{
		if((sublen = asn1decode_NewGenericContentReference(der, out, tag.length)) < 0)
			return der_error("NewVariableValue");
		left -= tag.length;
	}
	else
	{
		return der_error("NewVariableValue");
	}

	if(left != 0)
		return der_error("NewVariableValue: %d bytes left", left);

	verbose("</NewVariableValue>\n");

	return length;
}

int asn1decode_NewVolume(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewVolume>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewVolume");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("NewVolume");
		left -= sublen;
	}
	else
	{
		return der_error("NewVolume: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewVolume: %d bytes left", left);

	verbose("</NewVolume>\n");

	return length;
}

int asn1decode_NewXPosition(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewXPosition>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewXPosition");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("NewXPosition");
		left -= sublen;
	}
	else
	{
		return der_error("NewXPosition: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewXPosition: %d bytes left", left);

	verbose("</NewXPosition>\n");

	return length;
}

int asn1decode_NewYPosition(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewYPosition>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewYPosition");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("NewYPosition");
		left -= sublen;
	}
	else
	{
		return der_error("NewYPosition: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewYPosition: %d bytes left", left);

	verbose("</NewYPosition>\n");

	return length;
}

int asn1decode_Numerator(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Numerator>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Numerator");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("Numerator");
		left -= sublen;
	}
	else
	{
		return der_error("Numerator: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("Numerator: %d bytes left", left);

	verbose("</Numerator>\n");

	return length;
}

int asn1decode_OpenSucceeded(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<OpenSucceeded>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("OpenSucceeded");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("OpenSucceeded");
		left -= sublen;
	}
	else
	{
		return der_error("OpenSucceeded: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("OpenSucceeded: %d bytes left", left);

	verbose("</OpenSucceeded>\n");

	return length;
}

int asn1decode_Operator(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Operator>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Operator");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("Operator");
		left -= sublen;
	}
	else
	{
		return der_error("Operator: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("Operator: %d bytes left", left);

	verbose("</Operator>\n");

	return length;
}

int asn1decode_OutFileName(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<OutFileName>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("OutFileName");

	if(is_GenericOctetString(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericOctetString(der, out, length)) < 0)
			return der_error("OutFileName");
		left -= sublen;
	}
	else
	{
		return der_error("OutFileName: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("OutFileName: %d bytes left", left);

	verbose("</OutFileName>\n");

	return length;
}

int asn1decode_OutVariables(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<OutVariables>\n");

	output_token(out, "(");

	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("OutVariables");

		if(is_ObjectReference(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_ObjectReference(der, out, sublen + tag.length)) < 0)
				return der_error("OutVariables");
			left -= sublen;
		}
		else
		{
			return der_error("OutVariables: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("OutVariables: %d bytes left", left);

	verbose("</OutVariables>\n");

	return length;
}

int asn1decode_OverwriteModeVar(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<OverwriteModeVar>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("OverwriteModeVar");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("OverwriteModeVar");
		left -= sublen;
	}
	else
	{
		return der_error("OverwriteModeVar: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("OverwriteModeVar: %d bytes left", left);

	verbose("</OverwriteModeVar>\n");

	return length;
}

int asn1decode_Parameter(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Parameter>\n");

	/* CHOICE */
	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Parameter");
	left -= sublen;

	if(is_NewGenericBoolean(tag.class, tag.number))
	{
		if((sublen = asn1decode_NewGenericBoolean(der, out, tag.length)) < 0)
			return der_error("Parameter");
		left -= tag.length;
	}
	else if(is_NewGenericInteger(tag.class, tag.number))
	{
		if((sublen = asn1decode_NewGenericInteger(der, out, tag.length)) < 0)
			return der_error("Parameter");
		left -= tag.length;
	}
	else if(is_NewGenericOctetString(tag.class, tag.number))
	{
		if((sublen = asn1decode_NewGenericOctetString(der, out, tag.length)) < 0)
			return der_error("Parameter");
		left -= tag.length;
	}
	else if(is_NewGenericObjectReference(tag.class, tag.number))
	{
		if((sublen = asn1decode_NewGenericObjectReference(der, out, tag.length)) < 0)
			return der_error("Parameter");
		left -= tag.length;
	}
	else if(is_NewGenericContentReference(tag.class, tag.number))
	{
		if((sublen = asn1decode_NewGenericContentReference(der, out, tag.length)) < 0)
			return der_error("Parameter");
		left -= tag.length;
	}
	else
	{
		return der_error("Parameter");
	}

	if(left != 0)
		return der_error("Parameter: %d bytes left", left);

	verbose("</Parameter>\n");

	return length;
}

int asn1decode_Parameters(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Parameters>\n");

	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("Parameters");

		if(is_Parameter(tag.class, tag.number))
		{
			fseek(der, -sublen, SEEK_CUR);
			if((sublen = asn1decode_Parameter(der, out, sublen + tag.length)) < 0)
				return der_error("Parameters");
			left -= sublen;
		}
		else
		{
			return der_error("Parameters: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	if(left != 0)
		return der_error("Parameters: %d bytes left", left);

	verbose("</Parameters>\n");

	return length;
}

int asn1decode_Point(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Point>\n");

	/* SEQUENCE */
	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Point");
	left -= sublen;

	if(is_X(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_X(der, out, sublen + tag.length)) < 0)
			return der_error("Point");
		left -= tag.length;
	}
	else
	{
		return der_error("Point: missing X");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Point");
	left -= sublen;

	if(is_Y(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Y(der, out, sublen + tag.length)) < 0)
			return der_error("Point");
		left -= tag.length;
	}
	else
	{
		return der_error("Point: missing Y");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("Point: %d bytes left", left);

	verbose("</Point>\n");

	return length;
}

int asn1decode_PointList(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<PointList>\n");

	output_token(out, "(");

	while(left > 0)
	{
		if((sublen = der_decode_Tag(der, &tag)) < 0)
			return der_error("PointList");

		if(is_Point(tag.class, tag.number))
		{
			left -= sublen;
			if((sublen = asn1decode_Point(der, out, tag.length)) < 0)
				return der_error("PointList");
			left -= sublen;
		}
		else
		{
			return der_error("PointList: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
		}
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("PointList: %d bytes left", left);

	verbose("</PointList>\n");

	return length;
}

int asn1decode_PortionVar(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<PortionVar>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("PortionVar");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("PortionVar");
		left -= sublen;
	}
	else
	{
		return der_error("PortionVar: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("PortionVar: %d bytes left", left);

	verbose("</PortionVar>\n");

	return length;
}

int asn1decode_Protocol(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Protocol>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Protocol");

	if(is_GenericOctetString(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericOctetString(der, out, length)) < 0)
			return der_error("Protocol");
		left -= sublen;
	}
	else
	{
		return der_error("Protocol: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("Protocol: %d bytes left", left);

	verbose("</Protocol>\n");

	return length;
}

int asn1decode_Rational(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Rational>\n");

	/* SEQUENCE */
	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Rational");
	left -= sublen;

	if(is_Numerator(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Numerator(der, out, sublen + tag.length)) < 0)
			return der_error("Rational");
		left -= tag.length;
	}
	else
	{
		return der_error("Rational: missing Numerator");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Rational");
	left -= sublen;

	if(is_Denominator(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_Denominator(der, out, sublen + tag.length)) < 0)
			return der_error("Rational");
		left -= tag.length;
	}
	else
	{
		fseek(der, -sublen, SEEK_CUR);
		left += sublen;
	}

	if(left != 0)
		return der_error("Rational: %d bytes left", left);

	verbose("</Rational>\n");

	return length;
}

int asn1decode_ReadSucceeded(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ReadSucceeded>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ReadSucceeded");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("ReadSucceeded");
		left -= sublen;
	}
	else
	{
		return der_error("ReadSucceeded: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("ReadSucceeded: %d bytes left", left);

	verbose("</ReadSucceeded>\n");

	return length;
}

int asn1decode_ReferenceVisible(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ReferenceVisible>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ReferenceVisible");

	if(is_GenericObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericObjectReference(der, out, length)) < 0)
			return der_error("ReferenceVisible");
		left -= sublen;
	}
	else
	{
		return der_error("ReferenceVisible: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("ReferenceVisible: %d bytes left", left);

	verbose("</ReferenceVisible>\n");

	return length;
}

int asn1decode_RunningStatusVar(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<RunningStatusVar>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("RunningStatusVar");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("RunningStatusVar");
		left -= sublen;
	}
	else
	{
		return der_error("RunningStatusVar: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("RunningStatusVar: %d bytes left", left);

	verbose("</RunningStatusVar>\n");

	return length;
}

int asn1decode_SelectionStatusVar(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SelectionStatusVar>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SelectionStatusVar");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("SelectionStatusVar");
		left -= sublen;
	}
	else
	{
		return der_error("SelectionStatusVar: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("SelectionStatusVar: %d bytes left", left);

	verbose("</SelectionStatusVar>\n");

	return length;
}

int asn1decode_SizeVar(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SizeVar>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SizeVar");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("SizeVar");
		left -= sublen;
	}
	else
	{
		return der_error("SizeVar: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("SizeVar: %d bytes left", left);

	verbose("</SizeVar>\n");

	return length;
}

int asn1decode_SliderValueVar(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<SliderValueVar>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("SliderValueVar");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("SliderValueVar");
		left -= sublen;
	}
	else
	{
		return der_error("SliderValueVar: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("SliderValueVar: %d bytes left", left);

	verbose("</SliderValueVar>\n");

	return length;
}

int asn1decode_StartAngle(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<StartAngle>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("StartAngle");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("StartAngle");
		left -= sublen;
	}
	else
	{
		return der_error("StartAngle: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("StartAngle: %d bytes left", left);

	verbose("</StartAngle>\n");

	return length;
}

int asn1decode_StoreSucceeded(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<StoreSucceeded>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("StoreSucceeded");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("StoreSucceeded");
		left -= sublen;
	}
	else
	{
		return der_error("StoreSucceeded: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("StoreSucceeded: %d bytes left", left);

	verbose("</StoreSucceeded>\n");

	return length;
}

int asn1decode_Target(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Target>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Target");

	if(is_GenericObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericObjectReference(der, out, length)) < 0)
			return der_error("Target");
		left -= sublen;
	}
	else
	{
		return der_error("Target: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("Target: %d bytes left", left);

	verbose("</Target>\n");

	return length;
}

int asn1decode_TextContentVar(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<TextContentVar>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("TextContentVar");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("TextContentVar");
		left -= sublen;
	}
	else
	{
		return der_error("TextContentVar: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("TextContentVar: %d bytes left", left);

	verbose("</TextContentVar>\n");

	return length;
}

int asn1decode_TextDataVar(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<TextDataVar>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("TextDataVar");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("TextDataVar");
		left -= sublen;
	}
	else
	{
		return der_error("TextDataVar: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("TextDataVar: %d bytes left", left);

	verbose("</TextDataVar>\n");

	return length;
}

int asn1decode_TimerID(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<TimerID>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("TimerID");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("TimerID");
		left -= sublen;
	}
	else
	{
		return der_error("TimerID: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("TimerID: %d bytes left", left);

	verbose("</TimerID>\n");

	return length;
}

int asn1decode_TimerValue(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<TimerValue>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("TimerValue");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("TimerValue");
		left -= sublen;
	}
	else
	{
		return der_error("TimerValue: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("TimerValue: %d bytes left", left);

	verbose("</TimerValue>\n");

	return length;
}

int asn1decode_TokenPositionVar(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<TokenPositionVar>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("TokenPositionVar");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("TokenPositionVar");
		left -= sublen;
	}
	else
	{
		return der_error("TokenPositionVar: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("TokenPositionVar: %d bytes left", left);

	verbose("</TokenPositionVar>\n");

	return length;
}

int asn1decode_TransitionEffect(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<TransitionEffect>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("TransitionEffect");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("TransitionEffect");
		left -= sublen;
	}
	else
	{
		return der_error("TransitionEffect: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("TransitionEffect: %d bytes left", left);

	verbose("</TransitionEffect>\n");

	return length;
}

int asn1decode_TriggerIdentifier(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<TriggerIdentifier>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("TriggerIdentifier");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("TriggerIdentifier");
		left -= sublen;
	}
	else
	{
		return der_error("TriggerIdentifier: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("TriggerIdentifier: %d bytes left", left);

	verbose("</TriggerIdentifier>\n");

	return length;
}

int asn1decode_Value(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Value>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Value");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("Value");
		left -= sublen;
	}
	else
	{
		return der_error("Value: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("Value: %d bytes left", left);

	verbose("</Value>\n");

	return length;
}

int asn1decode_VisibleReference(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<VisibleReference>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("VisibleReference");

	if(is_GenericObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericObjectReference(der, out, length)) < 0)
			return der_error("VisibleReference");
		left -= sublen;
	}
	else
	{
		return der_error("VisibleReference: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("VisibleReference: %d bytes left", left);

	verbose("</VisibleReference>\n");

	return length;
}

int asn1decode_VolumeVar(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<VolumeVar>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("VolumeVar");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("VolumeVar");
		left -= sublen;
	}
	else
	{
		return der_error("VolumeVar: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("VolumeVar: %d bytes left", left);

	verbose("</VolumeVar>\n");

	return length;
}

int asn1decode_X(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<X>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("X");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("X");
		left -= sublen;
	}
	else
	{
		return der_error("X: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("X: %d bytes left", left);

	verbose("</X>\n");

	return length;
}

int asn1decode_X1(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<X1>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("X1");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("X1");
		left -= sublen;
	}
	else
	{
		return der_error("X1: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("X1: %d bytes left", left);

	verbose("</X1>\n");

	return length;
}

int asn1decode_X2(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<X2>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("X2");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("X2");
		left -= sublen;
	}
	else
	{
		return der_error("X2: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("X2: %d bytes left", left);

	verbose("</X2>\n");

	return length;
}

int asn1decode_XBoxSizeVar(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<XBoxSizeVar>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("XBoxSizeVar");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("XBoxSizeVar");
		left -= sublen;
	}
	else
	{
		return der_error("XBoxSizeVar: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("XBoxSizeVar: %d bytes left", left);

	verbose("</XBoxSizeVar>\n");

	return length;
}

int asn1decode_XCursor(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<XCursor>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("XCursor");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("XCursor");
		left -= sublen;
	}
	else
	{
		return der_error("XCursor: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("XCursor: %d bytes left", left);

	verbose("</XCursor>\n");

	return length;
}

int asn1decode_XNewBoxSize(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<XNewBoxSize>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("XNewBoxSize");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("XNewBoxSize");
		left -= sublen;
	}
	else
	{
		return der_error("XNewBoxSize: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("XNewBoxSize: %d bytes left", left);

	verbose("</XNewBoxSize>\n");

	return length;
}

int asn1decode_XOut(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<XOut>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("XOut");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("XOut");
		left -= sublen;
	}
	else
	{
		return der_error("XOut: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("XOut: %d bytes left", left);

	verbose("</XOut>\n");

	return length;
}

int asn1decode_XPositionVar(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<XPositionVar>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("XPositionVar");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("XPositionVar");
		left -= sublen;
	}
	else
	{
		return der_error("XPositionVar: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("XPositionVar: %d bytes left", left);

	verbose("</XPositionVar>\n");

	return length;
}

int asn1decode_XScale(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<XScale>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("XScale");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("XScale");
		left -= sublen;
	}
	else
	{
		return der_error("XScale: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("XScale: %d bytes left", left);

	verbose("</XScale>\n");

	return length;
}

int asn1decode_Y(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Y>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Y");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("Y");
		left -= sublen;
	}
	else
	{
		return der_error("Y: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("Y: %d bytes left", left);

	verbose("</Y>\n");

	return length;
}

int asn1decode_Y1(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Y1>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Y1");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("Y1");
		left -= sublen;
	}
	else
	{
		return der_error("Y1: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("Y1: %d bytes left", left);

	verbose("</Y1>\n");

	return length;
}

int asn1decode_Y2(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Y2>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Y2");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("Y2");
		left -= sublen;
	}
	else
	{
		return der_error("Y2: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("Y2: %d bytes left", left);

	verbose("</Y2>\n");

	return length;
}

int asn1decode_YBoxSizeVar(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<YBoxSizeVar>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("YBoxSizeVar");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("YBoxSizeVar");
		left -= sublen;
	}
	else
	{
		return der_error("YBoxSizeVar: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("YBoxSizeVar: %d bytes left", left);

	verbose("</YBoxSizeVar>\n");

	return length;
}

int asn1decode_YCursor(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<YCursor>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("YCursor");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("YCursor");
		left -= sublen;
	}
	else
	{
		return der_error("YCursor: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("YCursor: %d bytes left", left);

	verbose("</YCursor>\n");

	return length;
}

int asn1decode_YNewBoxSize(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<YNewBoxSize>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("YNewBoxSize");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("YNewBoxSize");
		left -= sublen;
	}
	else
	{
		return der_error("YNewBoxSize: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("YNewBoxSize: %d bytes left", left);

	verbose("</YNewBoxSize>\n");

	return length;
}

int asn1decode_YOut(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<YOut>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("YOut");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("YOut");
		left -= sublen;
	}
	else
	{
		return der_error("YOut: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("YOut: %d bytes left", left);

	verbose("</YOut>\n");

	return length;
}

int asn1decode_YPositionVar(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<YPositionVar>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("YPositionVar");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("YPositionVar");
		left -= sublen;
	}
	else
	{
		return der_error("YPositionVar: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("YPositionVar: %d bytes left", left);

	verbose("</YPositionVar>\n");

	return length;
}

int asn1decode_YScale(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<YScale>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("YScale");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("YScale");
		left -= sublen;
	}
	else
	{
		return der_error("YScale: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("YScale: %d bytes left", left);

	verbose("</YScale>\n");

	return length;
}

int asn1decode_XOffsetVar(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<XOffsetVar>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("XOffsetVar");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("XOffsetVar");
		left -= sublen;
	}
	else
	{
		return der_error("XOffsetVar: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("XOffsetVar: %d bytes left", left);

	verbose("</XOffsetVar>\n");

	return length;
}

int asn1decode_YOffsetVar(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<YOffsetVar>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("YOffsetVar");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("YOffsetVar");
		left -= sublen;
	}
	else
	{
		return der_error("YOffsetVar: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("YOffsetVar: %d bytes left", left);

	verbose("</YOffsetVar>\n");

	return length;
}

int asn1decode_NewXOffset(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewXOffset>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewXOffset");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("NewXOffset");
		left -= sublen;
	}
	else
	{
		return der_error("NewXOffset: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewXOffset: %d bytes left", left);

	verbose("</NewXOffset>\n");

	return length;
}

int asn1decode_NewYOffset(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewYOffset>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewYOffset");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("NewYOffset");
		left -= sublen;
	}
	else
	{
		return der_error("NewYOffset: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewYOffset: %d bytes left", left);

	verbose("</NewYOffset>\n");

	return length;
}

int asn1decode_FocusPositionVar(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<FocusPositionVar>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("FocusPositionVar");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("FocusPositionVar");
		left -= sublen;
	}
	else
	{
		return der_error("FocusPositionVar: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("FocusPositionVar: %d bytes left", left);

	verbose("</FocusPositionVar>\n");

	return length;
}

int asn1decode_NewFocusPosition(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewFocusPosition>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewFocusPosition");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("NewFocusPosition");
		left -= sublen;
	}
	else
	{
		return der_error("NewFocusPosition: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewFocusPosition: %d bytes left", left);

	verbose("</NewFocusPosition>\n");

	return length;
}

int asn1decode_NewMinValue(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewMinValue>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewMinValue");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("NewMinValue");
		left -= sublen;
	}
	else
	{
		return der_error("NewMinValue: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewMinValue: %d bytes left", left);

	verbose("</NewMinValue>\n");

	return length;
}

int asn1decode_NewMaxValue(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewMaxValue>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewMaxValue");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("NewMaxValue");
		left -= sublen;
	}
	else
	{
		return der_error("NewMaxValue: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewMaxValue: %d bytes left", left);

	verbose("</NewMaxValue>\n");

	return length;
}

int asn1decode_NewStepSize(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<NewStepSize>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("NewStepSize");

	if(is_GenericInteger(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GenericInteger(der, out, length)) < 0)
			return der_error("NewStepSize");
		left -= sublen;
	}
	else
	{
		return der_error("NewStepSize: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("NewStepSize: %d bytes left", left);

	verbose("</NewStepSize>\n");

	return length;
}

int asn1decode_ObjectReference(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ObjectReference>\n");

	/* CHOICE */
	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ObjectReference");
	left -= sublen;

	if(is_ExternalReference(tag.class, tag.number))
	{
		if((sublen = asn1decode_ExternalReference(der, out, tag.length)) < 0)
			return der_error("ObjectReference");
		left -= tag.length;
	}
	else if(is_InternalReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_InternalReference(der, out, sublen + tag.length)) < 0)
			return der_error("ObjectReference");
		left -= tag.length;
	}
	else
	{
		return der_error("ObjectReference");
	}

	if(left != 0)
		return der_error("ObjectReference: %d bytes left", left);

	verbose("</ObjectReference>\n");

	return length;
}

int asn1decode_ExternalReference(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ExternalReference>\n");

	/* SEQUENCE */
	output_token(out, "(");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ExternalReference");
	left -= sublen;

	if(is_GroupIdentifier(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_GroupIdentifier(der, out, sublen + tag.length)) < 0)
			return der_error("ExternalReference");
		left -= tag.length;
	}
	else
	{
		return der_error("ExternalReference: missing GroupIdentifier");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ExternalReference");
	left -= sublen;

	if(is_ObjectNumber(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectNumber(der, out, sublen + tag.length)) < 0)
			return der_error("ExternalReference");
		left -= tag.length;
	}
	else
	{
		return der_error("ExternalReference: missing ObjectNumber");
	}

	output_token(out, ")");

	if(left != 0)
		return der_error("ExternalReference: %d bytes left", left);

	verbose("</ExternalReference>\n");

	return length;
}

int asn1decode_InternalReference(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<InternalReference>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("InternalReference");

	if(is_ObjectNumber(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectNumber(der, out, length)) < 0)
			return der_error("InternalReference");
		left -= sublen;
	}
	else
	{
		return der_error("InternalReference: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("InternalReference: %d bytes left", left);

	verbose("</InternalReference>\n");

	return length;
}

int asn1decode_GroupIdentifier(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GroupIdentifier>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GroupIdentifier");

	if(is_OctetString(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_OctetString(der, out, length)) < 0)
			return der_error("GroupIdentifier");
		left -= sublen;
	}
	else
	{
		return der_error("GroupIdentifier: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("GroupIdentifier: %d bytes left", left);

	verbose("</GroupIdentifier>\n");

	return length;
}

int asn1decode_ObjectNumber(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ObjectNumber>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ObjectNumber");

	if(is_INTEGER(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_INTEGER(der, out, length)) < 0)
			return der_error("ObjectNumber");
		left -= sublen;
	}
	else
	{
		return der_error("ObjectNumber: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("ObjectNumber: %d bytes left", left);

	verbose("</ObjectNumber>\n");

	return length;
}

int asn1decode_ContentReference(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ContentReference>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ContentReference");

	if(is_OctetString(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_OctetString(der, out, length)) < 0)
			return der_error("ContentReference");
		left -= sublen;
	}
	else
	{
		return der_error("ContentReference: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("ContentReference: %d bytes left", left);

	verbose("</ContentReference>\n");

	return length;
}

int asn1decode_ContentReference69(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ContentReference69>\n");

	if((sublen = der_decode_OctetString(der, out, length)) < 0)
		return der_error("ContentReference69");
	left -= sublen;

	if(left != 0)
		return der_error("ContentReference69: %d bytes left", left);

	verbose("</ContentReference69>\n");

	return length;
}

int asn1decode_GenericObjectReference(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GenericObjectReference>\n");

	/* CHOICE */
	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GenericObjectReference");
	left -= sublen;

	if(is_DirectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_DirectReference(der, out, sublen + tag.length)) < 0)
			return der_error("GenericObjectReference");
		left -= tag.length;
	}
	else if(is_IndirectReference(tag.class, tag.number))
	{
		if((sublen = asn1decode_IndirectReference(der, out, tag.length)) < 0)
			return der_error("GenericObjectReference");
		left -= tag.length;
	}
	else
	{
		return der_error("GenericObjectReference");
	}

	if(left != 0)
		return der_error("GenericObjectReference: %d bytes left", left);

	verbose("</GenericObjectReference>\n");

	return length;
}

int asn1decode_DirectReference(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<DirectReference>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("DirectReference");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("DirectReference");
		left -= sublen;
	}
	else
	{
		return der_error("DirectReference: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("DirectReference: %d bytes left", left);

	verbose("</DirectReference>\n");

	return length;
}

int asn1decode_IndirectReference(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<IndirectReference>\n");

	output_token(out, ":IndirectRef");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("IndirectReference");

	if(is_ObjectReference(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ObjectReference(der, out, length)) < 0)
			return der_error("IndirectReference");
		left -= sublen;
	}
	else
	{
		return der_error("IndirectReference: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("IndirectReference: %d bytes left", left);

	verbose("</IndirectReference>\n");

	return length;
}

int asn1decode_GenericContentReference(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GenericContentReference>\n");

	/* CHOICE */
	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GenericContentReference");
	left -= sublen;

	if(is_ContentReference69(tag.class, tag.number))
	{
		if((sublen = asn1decode_ContentReference69(der, out, tag.length)) < 0)
			return der_error("GenericContentReference");
		left -= tag.length;
	}
	else if(is_IndirectReference(tag.class, tag.number))
	{
		if((sublen = asn1decode_IndirectReference(der, out, tag.length)) < 0)
			return der_error("GenericContentReference");
		left -= tag.length;
	}
	else
	{
		return der_error("GenericContentReference");
	}

	if(left != 0)
		return der_error("GenericContentReference: %d bytes left", left);

	verbose("</GenericContentReference>\n");

	return length;
}

int asn1decode_GenericInteger(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GenericInteger>\n");

	/* CHOICE */
	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GenericInteger");
	left -= sublen;

	if(is_INTEGER(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_INTEGER(der, out, sublen + tag.length)) < 0)
			return der_error("GenericInteger");
		left -= tag.length;
	}
	else if(is_IndirectReference(tag.class, tag.number))
	{
		if((sublen = asn1decode_IndirectReference(der, out, tag.length)) < 0)
			return der_error("GenericInteger");
		left -= tag.length;
	}
	else
	{
		return der_error("GenericInteger");
	}

	if(left != 0)
		return der_error("GenericInteger: %d bytes left", left);

	verbose("</GenericInteger>\n");

	return length;
}

int asn1decode_GenericBoolean(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GenericBoolean>\n");

	/* CHOICE */
	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GenericBoolean");
	left -= sublen;

	if(is_BOOLEAN(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_BOOLEAN(der, out, sublen + tag.length)) < 0)
			return der_error("GenericBoolean");
		left -= tag.length;
	}
	else if(is_IndirectReference(tag.class, tag.number))
	{
		if((sublen = asn1decode_IndirectReference(der, out, tag.length)) < 0)
			return der_error("GenericBoolean");
		left -= tag.length;
	}
	else
	{
		return der_error("GenericBoolean");
	}

	if(left != 0)
		return der_error("GenericBoolean: %d bytes left", left);

	verbose("</GenericBoolean>\n");

	return length;
}

int asn1decode_GenericOctetString(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<GenericOctetString>\n");

	/* CHOICE */
	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("GenericOctetString");
	left -= sublen;

	if(is_OctetString(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_OctetString(der, out, sublen + tag.length)) < 0)
			return der_error("GenericOctetString");
		left -= tag.length;
	}
	else if(is_IndirectReference(tag.class, tag.number))
	{
		if((sublen = asn1decode_IndirectReference(der, out, tag.length)) < 0)
			return der_error("GenericOctetString");
		left -= tag.length;
	}
	else
	{
		return der_error("GenericOctetString");
	}

	if(left != 0)
		return der_error("GenericOctetString: %d bytes left", left);

	verbose("</GenericOctetString>\n");

	return length;
}

int asn1decode_OctetString(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<OctetString>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("OctetString");
	left -= sublen;

	if(is_OctetString(tag.class, tag.number))
	{
		der_decode_OctetString(der, out, tag.length);
		left -= tag.length;
	}
	else
	{
		return der_error("OctetString");
	}

	if(left != 0)
		return der_error("OctetString: %d bytes left", left);

	verbose("</OctetString>\n");

	return length;
}

int asn1decode_Colour(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<Colour>\n");

	/* CHOICE */
	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("Colour");
	left -= sublen;

	if(is_ColourIndex(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_ColourIndex(der, out, sublen + tag.length)) < 0)
			return der_error("Colour");
		left -= tag.length;
	}
	else if(is_AbsoluteColour(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_AbsoluteColour(der, out, sublen + tag.length)) < 0)
			return der_error("Colour");
		left -= tag.length;
	}
	else
	{
		return der_error("Colour");
	}

	if(left != 0)
		return der_error("Colour: %d bytes left", left);

	verbose("</Colour>\n");

	return length;
}

int asn1decode_ColourIndex(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<ColourIndex>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("ColourIndex");

	if(is_INTEGER(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_INTEGER(der, out, length)) < 0)
			return der_error("ColourIndex");
		left -= sublen;
	}
	else
	{
		return der_error("ColourIndex: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("ColourIndex: %d bytes left", left);

	verbose("</ColourIndex>\n");

	return length;
}

int asn1decode_AbsoluteColour(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<AbsoluteColour>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("AbsoluteColour");

	if(is_OctetString(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_OctetString(der, out, length)) < 0)
			return der_error("AbsoluteColour");
		left -= sublen;
	}
	else
	{
		return der_error("AbsoluteColour: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("AbsoluteColour: %d bytes left", left);

	verbose("</AbsoluteColour>\n");

	return length;
}

int asn1decode_XYPosition(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<XYPosition>\n");

	/* SEQUENCE */
	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("XYPosition");
	left -= sublen;

	if(is_XPosition(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_XPosition(der, out, sublen + tag.length)) < 0)
			return der_error("XYPosition");
		left -= tag.length;
	}
	else
	{
		return der_error("XYPosition: missing XPosition");
	}

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("XYPosition");
	left -= sublen;

	if(is_YPosition(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_YPosition(der, out, sublen + tag.length)) < 0)
			return der_error("XYPosition");
		left -= tag.length;
	}
	else
	{
		return der_error("XYPosition: missing YPosition");
	}

	if(left != 0)
		return der_error("XYPosition: %d bytes left", left);

	verbose("</XYPosition>\n");

	return length;
}

int asn1decode_XPosition(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<XPosition>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("XPosition");

	if(is_INTEGER(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_INTEGER(der, out, length)) < 0)
			return der_error("XPosition");
		left -= sublen;
	}
	else
	{
		return der_error("XPosition: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("XPosition: %d bytes left", left);

	verbose("</XPosition>\n");

	return length;
}

int asn1decode_YPosition(FILE *der, FILE *out, int length)
{
	int left = length;
	int sublen;
	struct der_tag tag;

	/* keep the compiler happy */
	tag = tag;

	verbose("<YPosition>\n");

	if((sublen = der_decode_Tag(der, &tag)) < 0)
		return der_error("YPosition");

	if(is_INTEGER(tag.class, tag.number))
	{
		fseek(der, -sublen, SEEK_CUR);
		if((sublen = asn1decode_INTEGER(der, out, length)) < 0)
			return der_error("YPosition");
		left -= sublen;
	}
	else
	{
		return der_error("YPosition: unexpected tag [%s %u]", asn1class_name(tag.class), tag.number);
	}

	if(left != 0)
		return der_error("YPosition: %d bytes left", left);

	verbose("</YPosition>\n");

	return length;
}

bool is_InterchangedObject(unsigned char class, unsigned int number)
{
		return is_ApplicationClass(class, number)
		    || is_SceneClass(class, number);
}

bool is_Root(unsigned char class, unsigned int number)
{
		return is_ObjectIdentifier(class, number);
}

bool is_ObjectIdentifier(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_Group(unsigned char class, unsigned int number)
{
	return is_Root(class, number)
	    || is_StandardIdentifier(class, number)
	    || is_StandardVersion(class, number)
	    || is_ObjectInformation(class, number)
	    || is_OnStartUp(class, number)
	    || is_OnCloseDown(class, number)
	    || is_OriginalGroupCachePriority(class, number)
	    || is_Items(class, number);
}

bool is_StandardIdentifier(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_StandardIdentifier);
}

bool is_JointIsoItuIdentifier(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_JointIsoItuIdentifier);
}

bool is_MHEGStandardIdentifier(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_MHEGStandardIdentifier);
}

bool is_StandardVersion(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_StandardVersion);
}

bool is_ObjectInformation(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_ObjectInformation);
}

bool is_OnStartUp(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_OnStartUp);
}

bool is_OnCloseDown(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_OnCloseDown);
}

bool is_OriginalGroupCachePriority(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_OriginalGroupCachePriority);
}

bool is_Items(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Items);
}

bool is_GroupItem(unsigned char class, unsigned int number)
{
		return is_ResidentProgramClass(class, number)
		    || is_RemoteProgramClass(class, number)
		    || is_InterchangedProgramClass(class, number)
		    || is_PaletteClass(class, number)
		    || is_FontClass(class, number)
		    || is_CursorShapeClass(class, number)
		    || is_BooleanVariableClass(class, number)
		    || is_IntegerVariableClass(class, number)
		    || is_OctetStringVariableClass(class, number)
		    || is_ObjectRefVariableClass(class, number)
		    || is_ContentRefVariableClass(class, number)
		    || is_LinkClass(class, number)
		    || is_StreamClass(class, number)
		    || is_BitmapClass(class, number)
		    || is_LineArtClass(class, number)
		    || is_DynamicLineArtClass(class, number)
		    || is_RectangleClass(class, number)
		    || is_HotspotClass(class, number)
		    || is_SwitchButtonClass(class, number)
		    || is_PushButtonClass(class, number)
		    || is_TextClass(class, number)
		    || is_EntryFieldClass(class, number)
		    || is_HyperTextClass(class, number)
		    || is_SliderClass(class, number)
		    || is_TokenGroupClass(class, number)
		    || is_ListGroupClass(class, number);
}

bool is_ApplicationClass(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_ApplicationClass);
}

bool is_OnSpawnCloseDown(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_OnSpawnCloseDown);
}

bool is_OnRestart(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_OnRestart);
}

bool is_DefaultAttributes(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_DefaultAttributes);
}

bool is_DefaultAttribute(unsigned char class, unsigned int number)
{
		return is_CharacterSet(class, number)
		    || is_BackgroundColour(class, number)
		    || is_TextContentHook(class, number)
		    || is_TextColour(class, number)
		    || is_Font(class, number)
		    || is_FontAttributes(class, number)
		    || is_InterchangedProgramContentHook(class, number)
		    || is_StreamContentHook(class, number)
		    || is_BitmapContentHook(class, number)
		    || is_LineArtContentHook(class, number)
		    || is_ButtonRefColour(class, number)
		    || is_HighlightRefColour(class, number)
		    || is_SliderRefColour(class, number);
}

bool is_CharacterSet(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_CharacterSet);
}

bool is_BackgroundColour(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_BackgroundColour);
}

bool is_TextContentHook(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_TextContentHook);
}

bool is_TextColour(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_TextColour);
}

bool is_Font(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Font);
}

bool is_FontBody(unsigned char class, unsigned int number)
{
		return is_DirectFont(class, number)
		    || is_IndirectFont(class, number);
}

bool is_DirectFont(unsigned char class, unsigned int number)
{
		return is_OctetString(class, number);
}

bool is_IndirectFont(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_FontAttributes(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_FontAttributes);
}

bool is_InterchangedProgramContentHook(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_InterchangedProgramContentHook);
}

bool is_StreamContentHook(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_StreamContentHook);
}

bool is_BitmapContentHook(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_BitmapContentHook);
}

bool is_LineArtContentHook(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_LineArtContentHook);
}

bool is_ButtonRefColour(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_ButtonRefColour);
}

bool is_HighlightRefColour(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_HighlightRefColour);
}

bool is_SliderRefColour(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SliderRefColour);
}

bool is_SceneClass(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SceneClass);
}

bool is_InputEventRegister(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_InputEventRegister);
}

bool is_SceneCoordinateSystem(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SceneCoordinateSystem);
}

bool is_XScene(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_XScene);
}

bool is_YScene(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_YScene);
}

bool is_AspectRatio(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_AspectRatio);
}

bool is_Width(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Width);
}

bool is_Height(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Height);
}

bool is_MovingCursor(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_MovingCursor);
}

bool is_NextScenes(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_NextScenes);
}

bool is_NextScene(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_NextScene);
}

bool is_InputEventMask(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_InputEventMask);
}

bool is_SceneRef(unsigned char class, unsigned int number)
{
		return is_OctetString(class, number);
}

bool is_SceneWeight(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SceneWeight);
}

bool is_Ingredient(unsigned char class, unsigned int number)
{
	return is_Root(class, number)
	    || is_InitiallyActive(class, number)
	    || is_ContentHook(class, number)
	    || is_OriginalContent(class, number)
	    || is_Shared(class, number);
}

bool is_InitiallyActive(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_InitiallyActive);
}

bool is_ContentHook(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_ContentHook);
}

bool is_OriginalContent(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_OriginalContent);
}

bool is_ContentBody(unsigned char class, unsigned int number)
{
		return is_IncludedContent(class, number)
		    || is_ReferencedContent(class, number);
}

bool is_IncludedContent(unsigned char class, unsigned int number)
{
		return is_OctetString(class, number);
}

bool is_ReferencedContent(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_ReferencedContent);
}

bool is_ContentSize(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_ContentSize);
}

bool is_ContentCachePriority(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_ContentCachePriority);
}

bool is_Shared(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Shared);
}

bool is_LinkClass(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_LinkClass);
}

bool is_LinkCondition(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_LinkCondition);
}

bool is_EventSource(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_EventType(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_EventType);
}

bool is_EventTypeEnum(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_EventTypeEnum);
}

bool is_EventData(unsigned char class, unsigned int number)
{
		return is_EventDataBody(class, number);
}

bool is_EventDataBody(unsigned char class, unsigned int number)
{
		return is_OctetString(class, number)
		    || is_BOOLEAN(class, number)
		    || is_INTEGER(class, number);
}

bool is_LinkEffect(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_LinkEffect);
}

bool is_Program(unsigned char class, unsigned int number)
{
	return is_Ingredient(class, number)
	    || is_Name(class, number)
	    || is_InitiallyAvailable(class, number);
}

bool is_Name(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Name);
}

bool is_InitiallyAvailable(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_InitiallyAvailable);
}

bool is_ResidentProgramClass(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_ResidentProgramClass);
}

bool is_RemoteProgramClass(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_RemoteProgramClass);
}

bool is_ProgramConnectionTag(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_ProgramConnectionTag);
}

bool is_InterchangedProgramClass(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_InterchangedProgramClass);
}

bool is_PaletteClass(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_PaletteClass);
}

bool is_FontClass(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_FontClass);
}

bool is_CursorShapeClass(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_CursorShapeClass);
}

bool is_Variable(unsigned char class, unsigned int number)
{
	return is_Ingredient(class, number)
	    || is_OriginalValue(class, number);
}

bool is_OriginalValue(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_OriginalValue);
}

bool is_OriginalValueBody(unsigned char class, unsigned int number)
{
		return is_BOOLEAN(class, number)
		    || is_INTEGER(class, number)
		    || is_OctetString(class, number)
		    || is_ObjectReferenceValue(class, number)
		    || is_ContentReferenceValue(class, number);
}

bool is_ObjectReferenceValue(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_ObjectReferenceValue);
}

bool is_ContentReferenceValue(unsigned char class, unsigned int number)
{
		return is_ContentReference69(class, number);
}

bool is_BooleanVariableClass(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_BooleanVariableClass);
}

bool is_IntegerVariableClass(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_IntegerVariableClass);
}

bool is_OctetStringVariableClass(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_OctetStringVariableClass);
}

bool is_ObjectRefVariableClass(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_ObjectRefVariableClass);
}

bool is_ContentRefVariableClass(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_ContentRefVariableClass);
}

bool is_Presentable(unsigned char class, unsigned int number)
{
		return is_Ingredient(class, number);
}

bool is_TokenManager(unsigned char class, unsigned int number)
{
		return is_MovementTable(class, number);
}

bool is_MovementTable(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_MovementTable);
}

bool is_Movement(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Movement);
}

bool is_TargetElement(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_TargetElement);
}

bool is_TokenGroupClass(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_TokenGroupClass);
}

bool is_TokenGroupBody(unsigned char class, unsigned int number)
{
	return is_Presentable(class, number)
	    || is_TokenManager(class, number)
	    || is_TokenGroupItems(class, number)
	    || is_NoTokenActionSlots(class, number);
}

bool is_TokenGroupItems(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_TokenGroupItems);
}

bool is_TokenGroupItem(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_TokenGroupItem);
}

bool is_AVisible(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_ActionSlots(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_ActionSlots);
}

bool is_ActionSlot(unsigned char class, unsigned int number)
{
		return is_ActionClassSeq(class, number)
		    || is_Null(class, number);
}

bool is_NoTokenActionSlots(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_NoTokenActionSlots);
}

bool is_ActionSlotSeq(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_ActionSlotSeq);
}

bool is_ActionClassSeq(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_ActionClassSeq);
}

bool is_ListGroupClass(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_ListGroupClass);
}

bool is_Positions(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Positions);
}

bool is_Position(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Position);
}

bool is_WrapAround(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_WrapAround);
}

bool is_MultipleSelection(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_MultipleSelection);
}

bool is_Visible(unsigned char class, unsigned int number)
{
	return is_Presentable(class, number)
	    || is_OriginalBoxSize(class, number)
	    || is_OriginalPosition(class, number)
	    || is_OriginalPaletteRef(class, number);
}

bool is_OriginalBoxSize(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_OriginalBoxSize);
}

bool is_BoxSize(unsigned char class, unsigned int number)
{
		return is_XLength(class, number);
}

bool is_XLength(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_XLength);
}

bool is_YLength(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_YLength);
}

bool is_OriginalPosition(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_OriginalPosition);
}

bool is_OriginalPaletteRef(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_OriginalPaletteRef);
}

bool is_BitmapClass(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_BitmapClass);
}

bool is_Tiling(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Tiling);
}

bool is_OriginalTransparency(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_OriginalTransparency);
}

bool is_LineArtClass(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_LineArtClass);
}

bool is_LineArtBody(unsigned char class, unsigned int number)
{
	return is_Visible(class, number)
	    || is_BorderedBoundingBox(class, number)
	    || is_OriginalLineWidth(class, number)
	    || is_OriginalLineStyle(class, number)
	    || is_OriginalRefLineColour(class, number)
	    || is_OriginalRefFillColour(class, number);
}

bool is_BorderedBoundingBox(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_BorderedBoundingBox);
}

bool is_OriginalLineWidth(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_OriginalLineWidth);
}

bool is_OriginalLineStyle(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_OriginalLineStyle);
}

bool is_OriginalRefLineColour(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_OriginalRefLineColour);
}

bool is_OriginalRefFillColour(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_OriginalRefFillColour);
}

bool is_RectangleClass(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_RectangleClass);
}

bool is_DynamicLineArtClass(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_DynamicLineArtClass);
}

bool is_TextClass(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_TextClass);
}

bool is_TextBody(unsigned char class, unsigned int number)
{
	return is_Visible(class, number)
	    || is_OriginalFont(class, number)
	    || is_FontAttributes(class, number)
	    || is_TextColour(class, number)
	    || is_BackgroundColour(class, number)
	    || is_CharacterSet(class, number)
	    || is_HorizontalJustification(class, number)
	    || is_VerticalJustification(class, number)
	    || is_LineOrientation(class, number)
	    || is_StartCorner(class, number)
	    || is_TextWrapping(class, number);
}

bool is_OriginalFont(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_OriginalFont);
}

bool is_HorizontalJustification(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_HorizontalJustification);
}

bool is_JustificationEnum(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_JustificationEnum);
}

bool is_VerticalJustification(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_VerticalJustification);
}

bool is_LineOrientation(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_LineOrientation);
}

bool is_LineOrientationEnum(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_LineOrientationEnum);
}

bool is_StartCorner(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_StartCorner);
}

bool is_StartCornerEnum(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_StartCornerEnum);
}

bool is_TextWrapping(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_TextWrapping);
}

bool is_StreamClass(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_StreamClass);
}

bool is_Multiplex(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Multiplex);
}

bool is_StreamComponent(unsigned char class, unsigned int number)
{
		return is_AudioClass(class, number)
		    || is_VideoClass(class, number)
		    || is_RTGraphicsClass(class, number);
}

bool is_Storage(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Storage);
}

bool is_StorageEnum(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_StorageEnum);
}

bool is_Looping(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Looping);
}

bool is_AudioClass(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_AudioClass);
}

bool is_ComponentTag(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_ComponentTag);
}

bool is_OriginalVolume(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_OriginalVolume);
}

bool is_VideoClass(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_VideoClass);
}

bool is_Termination(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Termination);
}

bool is_TerminationEnum(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_TerminationEnum);
}

bool is_RTGraphicsClass(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_RTGraphicsClass);
}

bool is_Interactible(unsigned char class, unsigned int number)
{
	return is_EngineResp(class, number)
	    || is_HighlightRefColour(class, number);
}

bool is_EngineResp(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_EngineResp);
}

bool is_SliderClass(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SliderClass);
}

bool is_Orientation(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Orientation);
}

bool is_OrientationEnum(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_OrientationEnum);
}

bool is_MaxValue(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_MaxValue);
}

bool is_MinValue(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_MinValue);
}

bool is_InitialValue(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_InitialValue);
}

bool is_InitialPortion(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_InitialPortion);
}

bool is_StepSize(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_StepSize);
}

bool is_SliderStyle(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SliderStyle);
}

bool is_SliderStyleEnum(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SliderStyleEnum);
}

bool is_EntryFieldClass(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_EntryFieldClass);
}

bool is_InputType(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_InputType);
}

bool is_InputTypeEnum(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_InputTypeEnum);
}

bool is_CharList(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_CharList);
}

bool is_ObscuredInput(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_ObscuredInput);
}

bool is_MaxLength(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_MaxLength);
}

bool is_HyperTextClass(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_HyperTextClass);
}

bool is_Button(unsigned char class, unsigned int number)
{
	return is_Visible(class, number)
	    || is_Interactible(class, number)
	    || is_ButtonRefColour(class, number);
}

bool is_HotspotClass(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_HotspotClass);
}

bool is_PushButtonClass(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_PushButtonClass);
}

bool is_PushButtonBody(unsigned char class, unsigned int number)
{
	return is_Button(class, number)
	    || is_OriginalLabel(class, number)
	    || is_CharacterSet(class, number);
}

bool is_OriginalLabel(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_OriginalLabel);
}

bool is_SwitchButtonClass(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SwitchButtonClass);
}

bool is_ButtonStyle(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_ButtonStyle);
}

bool is_ButtonStyleEnum(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_ButtonStyleEnum);
}

bool is_ActionClass(unsigned char class, unsigned int number)
{
		return is_ElementaryAction(class, number);
}

bool is_ElementaryAction(unsigned char class, unsigned int number)
{
		return is_Activate(class, number)
		    || is_Add(class, number)
		    || is_AddItem(class, number)
		    || is_Append(class, number)
		    || is_BringToFront(class, number)
		    || is_Call(class, number)
		    || is_CallActionSlot(class, number)
		    || is_Clear(class, number)
		    || is_Clone(class, number)
		    || is_CloseConnection(class, number)
		    || is_Deactivate(class, number)
		    || is_DelItem(class, number)
		    || is_Deselect(class, number)
		    || is_DeselectItem(class, number)
		    || is_Divide(class, number)
		    || is_DrawArc(class, number)
		    || is_DrawLine(class, number)
		    || is_DrawOval(class, number)
		    || is_DrawPolygon(class, number)
		    || is_DrawPolyline(class, number)
		    || is_DrawRectangle(class, number)
		    || is_DrawSector(class, number)
		    || is_Fork(class, number)
		    || is_GetAvailabilityStatus(class, number)
		    || is_GetBoxSize(class, number)
		    || is_GetCellItem(class, number)
		    || is_GetCursorPosition(class, number)
		    || is_GetEngineSupport(class, number)
		    || is_GetEntryPoint(class, number)
		    || is_GetFillColour(class, number)
		    || is_GetFirstItem(class, number)
		    || is_GetHighlightStatus(class, number)
		    || is_GetInteractionStatus(class, number)
		    || is_GetItemStatus(class, number)
		    || is_GetLabel(class, number)
		    || is_GetLastAnchorFired(class, number)
		    || is_GetLineColour(class, number)
		    || is_GetLineStyle(class, number)
		    || is_GetLineWidth(class, number)
		    || is_GetListItem(class, number)
		    || is_GetListSize(class, number)
		    || is_GetOverwriteMode(class, number)
		    || is_GetPortion(class, number)
		    || is_GetPosition(class, number)
		    || is_GetRunningStatus(class, number)
		    || is_GetSelectionStatus(class, number)
		    || is_GetSliderValue(class, number)
		    || is_GetTextContent(class, number)
		    || is_GetTextData(class, number)
		    || is_GetTokenPosition(class, number)
		    || is_GetVolume(class, number)
		    || is_Launch(class, number)
		    || is_LockScreen(class, number)
		    || is_Modulo(class, number)
		    || is_Move(class, number)
		    || is_MoveTo(class, number)
		    || is_Multiply(class, number)
		    || is_OpenConnection(class, number)
		    || is_Preload(class, number)
		    || is_PutBefore(class, number)
		    || is_PutBehind(class, number)
		    || is_Quit(class, number)
		    || is_ReadPersistent(class, number)
		    || is_Run(class, number)
		    || is_ScaleBitmap(class, number)
		    || is_ScaleVideo(class, number)
		    || is_ScrollItems(class, number)
		    || is_Select(class, number)
		    || is_SelectItem(class, number)
		    || is_SendEvent(class, number)
		    || is_SendToBack(class, number)
		    || is_SetBoxSize(class, number)
		    || is_SetCachePriority(class, number)
		    || is_SetCounterEndPosition(class, number)
		    || is_SetCounterPosition(class, number)
		    || is_SetCounterTrigger(class, number)
		    || is_SetCursorPosition(class, number)
		    || is_SetCursorShape(class, number)
		    || is_SetData(class, number)
		    || is_SetEntryPoint(class, number)
		    || is_SetFillColour(class, number)
		    || is_SetFirstItem(class, number)
		    || is_SetFontRef(class, number)
		    || is_SetHighlightStatus(class, number)
		    || is_SetInteractionStatus(class, number)
		    || is_SetLabel(class, number)
		    || is_SetLineColour(class, number)
		    || is_SetLineStyle(class, number)
		    || is_SetLineWidth(class, number)
		    || is_SetOverwriteMode(class, number)
		    || is_SetPaletteRef(class, number)
		    || is_SetPortion(class, number)
		    || is_SetPosition(class, number)
		    || is_SetSliderValue(class, number)
		    || is_SetSpeed(class, number)
		    || is_SetTimer(class, number)
		    || is_SetTransparency(class, number)
		    || is_SetVariable(class, number)
		    || is_SetVolume(class, number)
		    || is_Spawn(class, number)
		    || is_Step(class, number)
		    || is_Stop(class, number)
		    || is_StorePersistent(class, number)
		    || is_Subtract(class, number)
		    || is_TestVariable(class, number)
		    || is_Toggle(class, number)
		    || is_ToggleItem(class, number)
		    || is_TransitionTo(class, number)
		    || is_Unload(class, number)
		    || is_UnlockScreen(class, number)
		    || is_SetBackgroundColour(class, number)
		    || is_SetCellPosition(class, number)
		    || is_SetInputReg(class, number)
		    || is_SetTextColour(class, number)
		    || is_SetFontAttributes(class, number)
		    || is_SetVideoDecodeOffset(class, number)
		    || is_GetVideoDecodeOffset(class, number)
		    || is_GetFocusPosition(class, number)
		    || is_SetFocusPosition(class, number)
		    || is_SetBitmapDecodeOffset(class, number)
		    || is_GetBitmapDecodeOffset(class, number)
		    || is_SetSliderParameters(class, number);
}

bool is_Activate(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Activate);
}

bool is_Add(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Add);
}

bool is_AddItem(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_AddItem);
}

bool is_Append(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Append);
}

bool is_BringToFront(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_BringToFront);
}

bool is_Call(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Call);
}

bool is_CallActionSlot(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_CallActionSlot);
}

bool is_Clear(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Clear);
}

bool is_Clone(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Clone);
}

bool is_CloseConnection(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_CloseConnection);
}

bool is_Deactivate(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Deactivate);
}

bool is_DelItem(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_DelItem);
}

bool is_Deselect(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Deselect);
}

bool is_DeselectItem(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_DeselectItem);
}

bool is_Divide(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Divide);
}

bool is_DrawArc(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_DrawArc);
}

bool is_DrawLine(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_DrawLine);
}

bool is_DrawOval(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_DrawOval);
}

bool is_DrawPolygon(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_DrawPolygon);
}

bool is_DrawPolyline(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_DrawPolyline);
}

bool is_DrawRectangle(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_DrawRectangle);
}

bool is_DrawSector(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_DrawSector);
}

bool is_Fork(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Fork);
}

bool is_GetAvailabilityStatus(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_GetAvailabilityStatus);
}

bool is_GetBoxSize(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_GetBoxSize);
}

bool is_GetCellItem(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_GetCellItem);
}

bool is_GetCursorPosition(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_GetCursorPosition);
}

bool is_GetEngineSupport(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_GetEngineSupport);
}

bool is_GetEntryPoint(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_GetEntryPoint);
}

bool is_GetFillColour(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_GetFillColour);
}

bool is_GetFirstItem(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_GetFirstItem);
}

bool is_GetHighlightStatus(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_GetHighlightStatus);
}

bool is_GetInteractionStatus(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_GetInteractionStatus);
}

bool is_GetItemStatus(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_GetItemStatus);
}

bool is_GetLabel(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_GetLabel);
}

bool is_GetLastAnchorFired(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_GetLastAnchorFired);
}

bool is_GetLineColour(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_GetLineColour);
}

bool is_GetLineStyle(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_GetLineStyle);
}

bool is_GetLineWidth(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_GetLineWidth);
}

bool is_GetListItem(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_GetListItem);
}

bool is_GetListSize(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_GetListSize);
}

bool is_GetOverwriteMode(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_GetOverwriteMode);
}

bool is_GetPortion(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_GetPortion);
}

bool is_GetPosition(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_GetPosition);
}

bool is_GetRunningStatus(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_GetRunningStatus);
}

bool is_GetSelectionStatus(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_GetSelectionStatus);
}

bool is_GetSliderValue(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_GetSliderValue);
}

bool is_GetTextContent(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_GetTextContent);
}

bool is_GetTextData(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_GetTextData);
}

bool is_GetTokenPosition(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_GetTokenPosition);
}

bool is_GetVolume(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_GetVolume);
}

bool is_Launch(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Launch);
}

bool is_LockScreen(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_LockScreen);
}

bool is_Modulo(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Modulo);
}

bool is_Move(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Move);
}

bool is_MoveTo(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_MoveTo);
}

bool is_Multiply(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Multiply);
}

bool is_OpenConnection(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_OpenConnection);
}

bool is_Preload(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Preload);
}

bool is_PutBefore(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_PutBefore);
}

bool is_PutBehind(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_PutBehind);
}

bool is_Quit(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Quit);
}

bool is_ReadPersistent(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_ReadPersistent);
}

bool is_Run(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Run);
}

bool is_ScaleBitmap(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_ScaleBitmap);
}

bool is_ScaleVideo(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_ScaleVideo);
}

bool is_ScrollItems(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_ScrollItems);
}

bool is_Select(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Select);
}

bool is_SelectItem(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SelectItem);
}

bool is_SendEvent(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SendEvent);
}

bool is_SendToBack(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SendToBack);
}

bool is_SetBoxSize(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetBoxSize);
}

bool is_SetCachePriority(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetCachePriority);
}

bool is_SetCounterEndPosition(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetCounterEndPosition);
}

bool is_SetCounterPosition(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetCounterPosition);
}

bool is_SetCounterTrigger(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetCounterTrigger);
}

bool is_SetCursorPosition(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetCursorPosition);
}

bool is_SetCursorShape(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetCursorShape);
}

bool is_SetData(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetData);
}

bool is_SetEntryPoint(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetEntryPoint);
}

bool is_SetFillColour(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetFillColour);
}

bool is_SetFirstItem(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetFirstItem);
}

bool is_SetFontRef(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetFontRef);
}

bool is_SetHighlightStatus(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetHighlightStatus);
}

bool is_SetInteractionStatus(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetInteractionStatus);
}

bool is_SetLabel(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetLabel);
}

bool is_SetLineColour(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetLineColour);
}

bool is_SetLineStyle(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetLineStyle);
}

bool is_SetLineWidth(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetLineWidth);
}

bool is_SetOverwriteMode(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetOverwriteMode);
}

bool is_SetPaletteRef(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetPaletteRef);
}

bool is_SetPortion(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetPortion);
}

bool is_SetPosition(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetPosition);
}

bool is_SetSliderValue(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetSliderValue);
}

bool is_SetSpeed(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetSpeed);
}

bool is_SetTimer(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetTimer);
}

bool is_NewTimer(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_NewTimer);
}

bool is_SetTransparency(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetTransparency);
}

bool is_SetVariable(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetVariable);
}

bool is_SetVolume(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetVolume);
}

bool is_Spawn(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Spawn);
}

bool is_Stop(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Stop);
}

bool is_Step(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Step);
}

bool is_StorePersistent(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_StorePersistent);
}

bool is_Subtract(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Subtract);
}

bool is_TestVariable(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_TestVariable);
}

bool is_Toggle(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Toggle);
}

bool is_ToggleItem(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_ToggleItem);
}

bool is_TransitionTo(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_TransitionTo);
}

bool is_Unload(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Unload);
}

bool is_UnlockScreen(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_UnlockScreen);
}

bool is_SetBackgroundColour(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetBackgroundColour);
}

bool is_SetCellPosition(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetCellPosition);
}

bool is_SetInputReg(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetInputReg);
}

bool is_SetTextColour(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetTextColour);
}

bool is_SetFontAttributes(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetFontAttributes);
}

bool is_SetVideoDecodeOffset(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetVideoDecodeOffset);
}

bool is_GetVideoDecodeOffset(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_GetVideoDecodeOffset);
}

bool is_GetFocusPosition(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_GetFocusPosition);
}

bool is_SetFocusPosition(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetFocusPosition);
}

bool is_SetBitmapDecodeOffset(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetBitmapDecodeOffset);
}

bool is_GetBitmapDecodeOffset(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_GetBitmapDecodeOffset);
}

bool is_SetSliderParameters(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_SetSliderParameters);
}

bool is_AbsoluteTime(unsigned char class, unsigned int number)
{
		return is_GenericBoolean(class, number);
}

bool is_Address(unsigned char class, unsigned int number)
{
		return is_GenericOctetString(class, number);
}

bool is_Answer(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_AppendValue(unsigned char class, unsigned int number)
{
		return is_GenericOctetString(class, number);
}

bool is_ArcAngle(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_AvailabilityStatusVar(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_CallSucceeded(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_CellIndex(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_CloneRefVar(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_ComparisonValue(unsigned char class, unsigned int number)
{
		return is_NewGenericBoolean(class, number)
		    || is_NewGenericInteger(class, number)
		    || is_NewGenericOctetString(class, number)
		    || is_NewGenericObjectReference(class, number)
		    || is_NewGenericContentReference(class, number);
}

bool is_ConnectionTag(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_Denominator(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_EllipseHeight(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_EllipseWidth(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_EmulatedEventData(unsigned char class, unsigned int number)
{
		return is_NewGenericBoolean(class, number)
		    || is_NewGenericInteger(class, number)
		    || is_NewGenericOctetString(class, number);
}

bool is_EmulatedEventSource(unsigned char class, unsigned int number)
{
		return is_GenericObjectReference(class, number);
}

bool is_EmulatedEventType(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_EmulatedEventType);
}

bool is_EntryPointVar(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_ForkSucceeded(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_Feature(unsigned char class, unsigned int number)
{
		return is_GenericOctetString(class, number);
}

bool is_FillColourVar(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_FirstItemVar(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_HighlightStatusVar(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_Index(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_InFileName(unsigned char class, unsigned int number)
{
		return is_GenericOctetString(class, number);
}

bool is_InteractionStatusVar(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_InVariables(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_InVariables);
}

bool is_ItemIndex(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_ItemRefVar(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_ItemStatusVar(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_ItemsToScroll(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_LabelVar(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_LastAnchorFiredVar(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_LineColourVar(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_LineStyleVar(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_LineWidthVar(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_MovementIdentifier(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_NbOfSteps(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_NewAbsoluteColour(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_NewAbsoluteColour);
}

bool is_NewCachePriority(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_NewColour(unsigned char class, unsigned int number)
{
		return is_NewColourIndex(class, number)
		    || is_NewAbsoluteColour(class, number);
}

bool is_NewColourIndex(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_NewColourIndex);
}

bool is_NewContent(unsigned char class, unsigned int number)
{
		return is_NewIncludedContent(class, number)
		    || is_NewReferencedContent(class, number);
}

bool is_NewContentCachePriority(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_NewContentCachePriority);
}

bool is_NewCounterEndPosition(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_NewCounterPosition(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_NewContentSize(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_NewContentSize);
}

bool is_NewCounterValue(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_NewCursorShape(unsigned char class, unsigned int number)
{
		return is_GenericObjectReference(class, number);
}

bool is_NewEntryPoint(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_NewFirstItem(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_NewFont(unsigned char class, unsigned int number)
{
		return is_NewFontName(class, number)
		    || is_NewFontReference(class, number);
}

bool is_NewFontName(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_NewFontName);
}

bool is_NewFontReference(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_NewFontReference);
}

bool is_NewGenericBoolean(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_NewGenericBoolean);
}

bool is_NewGenericInteger(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_NewGenericInteger);
}

bool is_NewGenericOctetString(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_NewGenericOctetString);
}

bool is_NewGenericObjectReference(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_NewGenericObjectReference);
}

bool is_NewGenericContentReference(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_NewGenericContentReference);
}

bool is_NewHighlightStatus(unsigned char class, unsigned int number)
{
		return is_GenericBoolean(class, number);
}

bool is_NewIncludedContent(unsigned char class, unsigned int number)
{
		return is_GenericOctetString(class, number);
}

bool is_NewInteractionStatus(unsigned char class, unsigned int number)
{
		return is_GenericBoolean(class, number);
}

bool is_NewLabel(unsigned char class, unsigned int number)
{
		return is_GenericOctetString(class, number);
}

bool is_NewLineStyle(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_NewLineWidth(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_NewOverwriteMode(unsigned char class, unsigned int number)
{
		return is_GenericBoolean(class, number);
}

bool is_NewPaletteRef(unsigned char class, unsigned int number)
{
		return is_GenericObjectReference(class, number);
}

bool is_NewPortion(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_NewReferencedContent(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_NewReferencedContent);
}

bool is_NewSliderValue(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_NewSpeed(unsigned char class, unsigned int number)
{
		return is_Rational(class, number);
}

bool is_NewTransparency(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_NewVariableValue(unsigned char class, unsigned int number)
{
		return is_NewGenericInteger(class, number)
		    || is_NewGenericBoolean(class, number)
		    || is_NewGenericOctetString(class, number)
		    || is_NewGenericObjectReference(class, number)
		    || is_NewGenericContentReference(class, number);
}

bool is_NewVolume(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_NewXPosition(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_NewYPosition(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_Numerator(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_OpenSucceeded(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_Operator(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_OutFileName(unsigned char class, unsigned int number)
{
		return is_GenericOctetString(class, number);
}

bool is_OutVariables(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_OutVariables);
}

bool is_OverwriteModeVar(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_Parameter(unsigned char class, unsigned int number)
{
		return is_NewGenericBoolean(class, number)
		    || is_NewGenericInteger(class, number)
		    || is_NewGenericOctetString(class, number)
		    || is_NewGenericObjectReference(class, number)
		    || is_NewGenericContentReference(class, number);
}

bool is_Parameters(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Parameters);
}

bool is_Point(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Point);
}

bool is_PointList(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_PointList);
}

bool is_PortionVar(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_Protocol(unsigned char class, unsigned int number)
{
		return is_GenericOctetString(class, number);
}

bool is_Rational(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_Rational);
}

bool is_ReadSucceeded(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_ReferenceVisible(unsigned char class, unsigned int number)
{
		return is_GenericObjectReference(class, number);
}

bool is_RunningStatusVar(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_SelectionStatusVar(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_SizeVar(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_SliderValueVar(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_StartAngle(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_StoreSucceeded(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_Target(unsigned char class, unsigned int number)
{
		return is_GenericObjectReference(class, number);
}

bool is_TextContentVar(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_TextDataVar(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_TimerID(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_TimerValue(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_TokenPositionVar(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_TransitionEffect(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_TriggerIdentifier(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_Value(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_VisibleReference(unsigned char class, unsigned int number)
{
		return is_GenericObjectReference(class, number);
}

bool is_VolumeVar(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_X(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_X1(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_X2(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_XBoxSizeVar(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_XCursor(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_XNewBoxSize(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_XOut(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_XPositionVar(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_XScale(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_Y(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_Y1(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_Y2(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_YBoxSizeVar(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_YCursor(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_YNewBoxSize(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_YOut(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_YPositionVar(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_YScale(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_XOffsetVar(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_YOffsetVar(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_NewXOffset(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_NewYOffset(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_FocusPositionVar(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_NewFocusPosition(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_NewMinValue(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_NewMaxValue(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_NewStepSize(unsigned char class, unsigned int number)
{
		return is_GenericInteger(class, number);
}

bool is_ObjectReference(unsigned char class, unsigned int number)
{
		return is_ExternalReference(class, number)
		    || is_InternalReference(class, number);
}

bool is_ExternalReference(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_ExternalReference);
}

bool is_InternalReference(unsigned char class, unsigned int number)
{
		return is_ObjectNumber(class, number);
}

bool is_GroupIdentifier(unsigned char class, unsigned int number)
{
		return is_OctetString(class, number);
}

bool is_ObjectNumber(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_ObjectNumber);
}

bool is_ContentReference(unsigned char class, unsigned int number)
{
		return is_OctetString(class, number);
}

bool is_ContentReference69(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_ContentReference69);
}

bool is_GenericObjectReference(unsigned char class, unsigned int number)
{
		return is_DirectReference(class, number)
		    || is_IndirectReference(class, number);
}

bool is_DirectReference(unsigned char class, unsigned int number)
{
		return is_ObjectReference(class, number);
}

bool is_IndirectReference(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_IndirectReference);
}

bool is_GenericContentReference(unsigned char class, unsigned int number)
{
		return is_ContentReference69(class, number)
		    || is_IndirectReference(class, number);
}

bool is_GenericInteger(unsigned char class, unsigned int number)
{
		return is_INTEGER(class, number)
		    || is_IndirectReference(class, number);
}

bool is_GenericBoolean(unsigned char class, unsigned int number)
{
		return is_BOOLEAN(class, number)
		    || is_IndirectReference(class, number);
}

bool is_GenericOctetString(unsigned char class, unsigned int number)
{
		return is_OctetString(class, number)
		    || is_IndirectReference(class, number);
}

bool is_OctetString(unsigned char class, unsigned int number)
{
		return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_OCTETSTRING);
}

bool is_Colour(unsigned char class, unsigned int number)
{
		return is_ColourIndex(class, number)
		    || is_AbsoluteColour(class, number);
}

bool is_ColourIndex(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_ColourIndex);
}

bool is_AbsoluteColour(unsigned char class, unsigned int number)
{
		return is_OctetString(class, number);
}

bool is_XYPosition(unsigned char class, unsigned int number)
{
		return is_XPosition(class, number);
}

bool is_XPosition(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_XPosition);
}

bool is_YPosition(unsigned char class, unsigned int number)
{
	return MATCH_TAGCLASS(class, number, ASN1TAGCLASS_YPosition);
}

#endif // hmkim : added.

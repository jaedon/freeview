/*
 * Content.c
 */

#include "MHEGEngine.h"
#include "Content.h"
#include "Generic.h"
#include "utils.h"

/*
 * caller_gid is used to resolve Generic references in the NewContent
 * the content may need to be loaded from a file
 * if it can't load the content it calls MHEGEngine_addMissingContent()
 * with obj as the object that needs the content
 * returns false if it can't load the content
 */

bool
NewContent_getContent(NewContent *n, OctetString *caller_gid, RootClass *obj, OctetString *out)
{
	bool rc = true;
	OctetString *content;
	ContentReference *ref;

	switch(n->choice)
	{
	case NewContent_new_included_content:
		content = GenericOctetString_getOctetString(&n->u.new_included_content, caller_gid);
		free_OctetString(out); // hmkim : added to get rid of any existing content.
		out->size = content->size;
		if(out->size > 0)
		{
			out->data = safe_malloc(out->size);
			VK_memcpy(out->data, content->data, out->size);
		}
		else
		{
			out->data = NULL;
		}
		break;

	case NewContent_new_referenced_content:
		ref = GenericContentReference_getContentReference(&n->u.new_referenced_content.generic_content_reference, caller_gid);
		#if 0 // org
		if(!(rc = MHEGEngine_loadFile(ref, out)))
			MHEGEngine_addMissingContent(obj, ref);
		#else // hmkim : modified.
		// If Content is currently set to included data, NewContent shall be set or refer to included data.
		// If Content is currently set to a reference to an external data source, then NewContent shall be set or refer to a reference to an external data source.
		if (n->u.new_referenced_content.generic_content_reference.choice == GenericContentReference_indirect_reference
			&& n->u.new_referenced_content.generic_content_reference.u.indirect_reference.choice == ObjectReference_external_reference)
		{
			free_OctetString(out); // get rid of any existing content.
			if(!(rc = MHEGEngine_loadFile(ref, out)))
			{
				verbose("NewContent_getContent: MissingContent~");
				MHEGEngine_enterCriticalMissingContent;
				MHEGEngine_addMissingContent(obj, ref);
				MHEGEngine_leaveCriticalMissingContent;
			}
		}
		else
		{
			if (ref)
			{
				free_OctetString(out); // get rid of any existing content.
				out->size = ref->size;
				if(out->size > 0)
				{
					out->data = safe_malloc(out->size);
					VK_memcpy(out->data, ref->data, out->size);
				}
				else
				{
					out->data = NULL;
				}
			}
			else // GenericContentReference_getContentReference() -> MHEGEngine_findObjectReference() 에서 IndirectRef 가 존재하지 않는 경우로 false 리턴한다.
			{
				rc = false;
			}
		}
		#endif
		break;

	default:
		rc = false;
		error("Unknown NewContent type: %d", n->choice);
		break;
	}

	return rc;
}

/*
 * if it can't load the content it calls MHEGEngine_addMissingContent()
 * with obj as the object that needs the content
 * returns false if it can't load the content
 */

bool
ContentBody_getContent(ContentBody *c, RootClass *obj, OctetString *out)
{
	bool rc = true;

	switch(c->choice)
	{
	case ContentBody_included_content:
		out->size = c->u.included_content.size;
		if(out->size > 0)
		{
			out->data = safe_malloc(out->size);
			VK_memcpy(out->data, c->u.included_content.data, out->size);
		}
		else
		{
			out->data = NULL;
		}
		break;

	case ContentBody_referenced_content:
		if(!(rc = MHEGEngine_loadFile(&c->u.referenced_content.content_reference, out)))
		{
			verbose("ContentBody_getContent: MissingContent~");
			MHEGEngine_enterCriticalMissingContent;
			MHEGEngine_addMissingContent(obj, &c->u.referenced_content.content_reference);
			MHEGEngine_leaveCriticalMissingContent;
		}
		break;

	default:
		rc = false;
		error("Unknown ContentBody type: %d", c->choice);
		break;
	}

	return rc;
}

/*
 * returns a ptr to the Referenced Content OctetString
 * retuns NULL if the content is included
 */

OctetString *
ContentBody_getReference(ContentBody *c)
{
	OctetString *rc = NULL;

	switch(c->choice)
	{
	case ContentBody_included_content:
		break;

	case ContentBody_referenced_content:
		rc = &c->u.referenced_content.content_reference;
		break;

	default:
		error("Unknown ContentBody type: %d", c->choice);
		break;
	}

	return rc;
}


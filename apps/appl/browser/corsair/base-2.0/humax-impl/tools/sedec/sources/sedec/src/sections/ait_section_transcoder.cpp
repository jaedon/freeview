// ait_section_transcoder.cpp: implementation of the AITSectionTranscoder class.
//
//////////////////////////////////////////////////////////////////////

#include "section_common.h"
#include "ait_section_transcoder.h"
#include "descriptor.h"
#include "application.h"
#include "descriptor_factory.h"
#include "simple_application_location_descriptor.h"
#include "simple_application_boundary_descriptor.h"
#include "transport_protocol_descriptor.h"
#include "application_descriptor.h"

#include <string.h>
#include <cstddef>
#include <stdlib.h>


AITSectionTranscoder::AITSectionTranscoder()
{
	m_section_name = strdup("AITSectionTranscoder");
}

AITSectionTranscoder::AITSectionTranscoder(unsigned char *raw_buffer)
: AITSection(raw_buffer, (( raw_buffer[1] << 8 | raw_buffer[2] ) & 0x0fff ) + 3)
{

}


AITSectionTranscoder::AITSectionTranscoder(unsigned char *raw_buffer, unsigned int raw_length)
: AITSection(raw_buffer, raw_length)
{

}

AITSectionTranscoder::~AITSectionTranscoder()
{
	if(m_section_name) {
		free(m_section_name);
		m_section_name = NULL;
	}
}

Descriptor* AITSectionTranscoder::findDescriptor(list<Descriptor*>descriptors,
												Descriptor::DESCRIPTOR_TAG tag)
{
	for (std::list<Descriptor*>::iterator it=descriptors.begin();
		it != descriptors.end(); ++it)
	{
		Descriptor *desc = (Descriptor*)*it;
		if( tag == desc->GetDescriptorTag())
			return desc;
	}
	return NULL;
}

void AITSectionTranscoder::SetApplicationUrl(char *base_url, char *init_path)
{
#if defined(CONFIG_DEBUG)
	SECTION_DEBUG("base_url +++: %s\n", base_url);
	SECTION_DEBUG("init_path +++: %s\n", init_path);
#endif

	for (std::list<Application*>::iterator it=m_applications.begin();
			it != m_applications.end(); ++it)
	{
		Application *app = (Application*)*it;
		Descriptor *desc = findDescriptor(app->GetDescriptors(),
									Descriptor::SIMPLE_APPLICATION_LOCATION_DESCRIPTOR);
		((SimpleApplicationLocationDescriptor*)desc)->SetInitialPath(init_path);

		desc = findDescriptor(app->GetDescriptors(),
								Descriptor::TRANSPORT_PROTOCOL_DESCRIPTOR);
		((TransportProtocolDescriptor*)desc)->SetBaseUrl(base_url);
	}
}

void AITSectionTranscoder::SetApplicationVersion(int major, int minor, int micro)
{
	for (std::list<Application*>::iterator it=m_applications.begin();
			it != m_applications.end(); ++it)
	{
		Application *app = (Application*)*it;
		Descriptor *desc = findDescriptor(app->GetDescriptors(),
											Descriptor::APPLICATION_DESCRIPTOR);
		((ApplicationDescriptor*)desc)->SetApplicationVersion(major, minor, micro);
	}
}

void AITSectionTranscoder::GetApplicationVersion(int &major, int &minor, int &micro)
{
	for (std::list<Application*>::iterator it=m_applications.begin();
			it != m_applications.end(); ++it)
	{
		Application *app = (Application*)*it;
		Descriptor *desc = findDescriptor(app->GetDescriptors(),
											Descriptor::APPLICATION_DESCRIPTOR);
		((ApplicationDescriptor*)desc)->GetApplicationVersion(&major, &minor, &micro);
	}
}

void AITSectionTranscoder::GetApplicationUrl(const char **base_url, const char **init_path)
{
	for (std::list<Application*>::iterator it=m_applications.begin();
			it != m_applications.end(); ++it)
	{
		Application *app = (Application*)*it;
		Descriptor *desc = findDescriptor(app->GetDescriptors(),
											Descriptor::SIMPLE_APPLICATION_LOCATION_DESCRIPTOR);
		*init_path = ((SimpleApplicationLocationDescriptor*)desc)->GetInitialPath();

		desc = findDescriptor(app->GetDescriptors(),
								Descriptor::TRANSPORT_PROTOCOL_DESCRIPTOR);
		*base_url = ((TransportProtocolDescriptor*)desc)->GetBaseUrl();
	}

}

void AITSectionTranscoder::SetTransportProtocolLabel(const char *label)
{
	for (std::list<Application*>::iterator it=m_applications.begin();
			it != m_applications.end(); ++it)
	{
		Application *app = (Application*)*it;
		Descriptor *desc = findDescriptor(app->GetDescriptors(),
											Descriptor::APPLICATION_DESCRIPTOR);
		((ApplicationDescriptor*)desc)->SetTransportProtocolLabel(label);

		desc = findDescriptor(app->GetDescriptors(),
								Descriptor::TRANSPORT_PROTOCOL_DESCRIPTOR);
		((TransportProtocolDescriptor*)desc)->SetTransportProtocolLabel((unsigned char)atoi(label));
	}
}

void AITSectionTranscoder::SetApplicationComponentTag(unsigned char comTag)
{
	for (std::list<Application*>::iterator it=m_applications.begin();
			it != m_applications.end(); ++it)
	{
		Application *app = (Application*)*it;
		Descriptor *desc = findDescriptor(app->GetDescriptors(),
								Descriptor::TRANSPORT_PROTOCOL_DESCRIPTOR);
		((TransportProtocolDescriptor*)desc)->SetComponentTag(comTag);
	}
}

void AITSectionTranscoder::SetBoundaryExtension(unsigned char ext_count,
								unsigned char* ext_length,
								unsigned char** ext_byte)
{
	for (std::list<Application*>::iterator it=m_applications.begin();
			it != m_applications.end(); ++it)
	{
		Application *app = (Application*)*it;
		Descriptor *desc = findDescriptor(app->GetDescriptors(),
											Descriptor::SIMPLE_APPLICATION_BOUNDARY_DESCRIPTOR);

		if ( desc == NULL )
		{
			std::list<Descriptor*> list = app->GetDescriptors();
			desc = new SimpleApplicationBoundaryDescriptor();
			((SimpleApplicationBoundaryDescriptor*)desc)->SetBoundaryExtension(ext_count, ext_length, ext_byte);
			list.push_back(desc);
			app->SetDescriptors(list);
		}
		else
		{
			((SimpleApplicationBoundaryDescriptor*)desc)->SetBoundaryExtension(ext_count, ext_length, ext_byte);
		}
	}
}

void AITSectionTranscoder::AddApplicationBoundary(const char* ext_byte)
{
	for (std::list<Application*>::iterator it=m_applications.begin();
			it != m_applications.end(); ++it)
	{
		Application *app = (Application*)*it;
		Descriptor *desc = findDescriptor(app->GetDescriptors(),
											Descriptor::SIMPLE_APPLICATION_BOUNDARY_DESCRIPTOR);

		if (desc != NULL)
			((SimpleApplicationBoundaryDescriptor*)desc)->AddBoundaryExtension(ext_byte);
	}
}

void AITSectionTranscoder::RemoveApplicationBoundary()
{
	for (std::list<Application*>::iterator it=m_applications.begin();
			it != m_applications.end(); ++it)
	{
		Application *app = (Application*)*it;
		Descriptor *desc = findDescriptor(app->GetDescriptors(),
											Descriptor::SIMPLE_APPLICATION_BOUNDARY_DESCRIPTOR);

		if (desc != NULL)
		{
			std::list<Descriptor*> list = app->GetDescriptors();
			list.remove(desc);

			app->SetDescriptors(list);
		}
	}
}

void AITSectionTranscoder::GetApplicationComponentTag(unsigned char *comTag)
{
	for (std::list<Application*>::iterator it=m_applications.begin();
			it != m_applications.end(); ++it)
	{
		Application *app = (Application*)*it;
		Descriptor *desc = findDescriptor(app->GetDescriptors(),
								Descriptor::TRANSPORT_PROTOCOL_DESCRIPTOR);
		*comTag = ((TransportProtocolDescriptor*)desc)->GetComponentTag();
	}
}

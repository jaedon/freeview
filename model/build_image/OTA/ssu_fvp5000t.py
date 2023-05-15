#! /usr/bin/env python
#
# Copyright (C) 2009  Lorenzo Pallara, l.pallara@avalpa.com
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#                                  
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#                                  
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


import os

from dvbobjects.PSI.PAT import *
from dvbobjects.PSI.NIT import *
from dvbobjects.PSI.SDT import *
from dvbobjects.PSI.PMT import *
from dvbobjects.PSI.UNT import *
from dvbobjects.PSI.TDT import *
from dvbobjects.DVB.Descriptors import *
from dvbobjects.MPEG.Descriptors import *
from dvbobjects.DVB.DataCarousel import *
from ssu_version import *

#
# Shared values
#
avalpa_network_id = 0x1
avalpa_transport_stream_id = 0x1# demo value, an official value should be demanded to dvb org
avalpa_original_transport_stream_id = 0x1 # demo value, an official value should be demanded to dvb org (original network id
avalpa1_service_id = 0x66 # demo value
avalpa1_pmt_pid = 0x66
avalpa1_dsmcc_pid = 0x141
dsmccB_association_tag = 0xB # association aka component tag for the dsmcc carousel
HumaxOUI = 0x000378 # humax OUI


#
# Update software version
#
version_hi = cur_sw_ver_hi
version_lo = cur_sw_ver_lo
#

# 809C 7D00 1.00.03
comp_desc1 = compatibility_descriptor(
	compatibility_descriptor_loop = [
		compatibility_descriptor_loop_item (
			descriptor_type = 0x01, # hardware descriptor
			specifier_type = 0x01, # IEEE OUI
			specifier_data = HumaxOUI,
			model = systemid_hi,
			version = systemid_lo,
			compatibility_descriptor_subloop = [],
		),
		compatibility_descriptor_loop_item (
			descriptor_type = 0x01, # hardware descriptor
			specifier_type = 0x01, # IEEE OUI
			specifier_data = HumaxOUI,
			model = systemid_hi2,
			version = systemid_lo2,
			compatibility_descriptor_subloop = [],
		),
		compatibility_descriptor_loop_item (
			descriptor_type = 0x02, # software descriptor
			specifier_type = 0x01, # IEEE OUI
			specifier_data = HumaxOUI,
			model = version_hi,
			version = version_lo,
			compatibility_descriptor_subloop = [],
		),
	]
)

#
# Network Information Table
#

nit = network_information_section(
	network_id = avalpa_network_id,
        network_descriptor_loop = [
	    network_descriptor(network_name = "Avalpa",), 
	    linkage_descriptor(
			transport_stream_id = avalpa_transport_stream_id,
			original_network_id = avalpa_original_transport_stream_id,
			service_id = avalpa1_service_id,
			linkage_type = 0x09,
			OUI_loop = [
				OUI_data (
					OUI = HumaxOUI,
					selector_bytes = "",
				)
			],
			private_data_bytes = "",
			),
            ],
	transport_stream_loop = [
	    transport_stream_loop_item(
		transport_stream_id = avalpa_transport_stream_id,
		original_network_id = avalpa_original_transport_stream_id,
		transport_descriptor_loop = [
		    service_list_descriptor(
			dvb_service_descriptor_loop = [
			    service_descriptor_loop_item(
				service_ID = avalpa1_service_id, 
				service_type = 0x0C, # data broadcast service
			    ),
			],
		    ),
		    terrestrial_delivery_system_descriptor(
		    center_frequency = 49000000,
		    bandwidth = 0,
		    priority = 0,
		    timeslice_ind=0,
		    mpe_fec_ind=0,
		    constellation=2,
		    hierarchy_information=0,
		    code_rate_HP_stream=4,
		    code_rate_LP_stream =4,
		    guard_interval=0,
		    transmission_mode =1,
		    other_frequency_flag=0,
		    ),
		],		
	     ),
	  ],
        version_number = 1, # you need to change the table number every time you edit, so the decoder will compare its version with the new one and update the table
        section_number = 0,
        last_section_number = 0,
        )

#
# Program Association Table (ISO/IEC 13818-1 2.4.4.3)
#

pat = program_association_section(
	transport_stream_id = avalpa_transport_stream_id,
        program_loop = [
    	    program_loop_item(
	        program_number = avalpa1_service_id,
    		PID = avalpa1_pmt_pid,
    	    ),  
    	    program_loop_item(
	        program_number = 0, # special program for the NIT
    		PID = 16,
    	    ), 
        ],
        version_number = 1, # you need to change the table number every time you edit, so the decoder will compare its version with the new one and update the table
        section_number = 0,
        last_section_number = 0,
        )

#
# Service Description Table (ETSI EN 300 468 5.2.3) 
# this is a basic SDT with the minimum desciptors, OpenCaster has a big library ready to use
#

sdt = service_description_section(
	transport_stream_id = avalpa_transport_stream_id,
	original_network_id = avalpa_original_transport_stream_id,
        service_loop = [
	    service_loop_item(
		service_ID = avalpa1_service_id,
		EIT_schedule_flag = 0, # 0 no current even information is broadcasted, 1 broadcasted
		EIT_present_following_flag = 0, # 0 no next event information is broadcasted, 1 is broadcasted
		running_status = 4, # 4 service is running, 1 not running, 2 starts in a few seconds, 3 pausing
		free_CA_mode = 0, # 0 means service is not scrambled, 1 means at least a stream is scrambled
		service_descriptor_loop = [
		    service_descriptor(
			service_type = 0x0C, # data broadcast service
			service_provider_name = "Humax",
			service_name = "Humax OTA",
		    ),    
		],
	    ),	
        ],
        version_number = 1, # you need to change the table number every time you edit, so the decoder will compare its version with the new one and update the table
        section_number = 0,
        last_section_number = 0,
        )



#
# Program Map Table (ISO/IEC 13818-1 2.4.4.8)
#

pmt = program_map_section(
	program_number = avalpa1_service_id,
	PCR_PID = 8191,
	program_info_descriptor_loop = [],
	stream_loop = [
		stream_loop_item(
			stream_type = 11, # data stream type
			elementary_PID = avalpa1_dsmcc_pid,
			element_info_descriptor_loop = [
				data_broadcast_id_descriptor(
					data_broadcast_ID = 0x000A, # DVB-SSU
					OUI_info_loop = [
						OUI_info_loop_item (
							OUI = HumaxOUI,
							update_type = 0x01, # with broadcasted UNT, 0x01 without UNT: it requires a stream_identifier_descriptor
							update_versioning_flag = 0, # no version change
							update_version = 0, # increment this at update change
							selector_bytes = "",
						),
					],
					private_data_bytes = "",
				),
				stream_identifier_descriptor(
					component_tag = dsmccB_association_tag,
				),
			]
		),
	],
        version_number = 1, # you need to change the table number every time you edit, so the decoder will compare its version with the new one and update the table
        section_number = 0,
        last_section_number = 0,
        )    




#
# DSMCC description
#

g1 = Group(
    PATH="DII-1.sec",
    transactionId = 0x90F70003,
    downloadId    = 0x00000001,
    blockSize     = 0x0fb1,
    version	  = 1,
    )
g1.set(
	compatibilityDescriptor = comp_desc1.pack(), 
	modules = [
         Module(
    	    INPUT="hdf/fvp5000t.hdf",
    	    moduleId = 0x0001,
    	    moduleVersion = 0x00,
    	    descriptors = [name_descriptor(name="fvp-5000t")],
    	    ),
    ],
    )

dsi = SuperGroup(
    PATH = "/",
    transactionId = 0x90F70001,
    version	  = 1,
    )
dsi.set(
    compatibilityDescriptor = "", # as specified in etsi 102 006
)
dsi.addGroup(g1)


#
# PSI marshalling and encapsulation
#

out = open("./nit.sec", "wb")
out.write(nit.pack())
out.close
out = open("./nit.sec", "wb") # python  flush bug
out.close
os.system('/usr/local/bin/sec2ts 16 < ./nit.sec > ./firstnit.ts')

out = open("./pat.sec", "wb")
out.write(pat.pack())
out.close
out = open("./pat.sec", "wb") # python   flush bug
out.close
os.system('/usr/local/bin/sec2ts 0 < ./pat.sec > ./firstpat.ts')

out = open("./sdt.sec", "wb")
out.write(sdt.pack())
out.close
out = open("./sdt.sec", "wb") # python   flush bug
out.close
os.system('/usr/local/bin/sec2ts 17 < ./sdt.sec > ./firstsdt.ts')

out = open("./pmt.sec", "wb")
out.write(pmt.pack())
out.close
out = open("./pmt.sec", "wb") # python   flush bug
out.close
os.system('/usr/local/bin/sec2ts ' + str(avalpa1_pmt_pid) + ' < ./pmt.sec > ./firstpmt.ts')

# 
# DSMCC streams generation
#

# for an object carousel the call would be: dsi.generate("objectcarousel", srg_ior_file_content)
dsi.generate("datacarousel")

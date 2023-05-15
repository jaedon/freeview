#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rr.h"
/**********************************************************

REMOTE RECORD RECEIVER SAMPLE CODE TEST HARNESS
(c) Red Bee Media 2012, all rights reserved
This code is strictly confidential and may only be used or
reproduced under strict licence

This source code is provided as part of the SES Remote Recording
Receiver specification version as listed below and is based on
work by S&T, TV Genius, and SES S.A.

***********************************************************/

int main(int argc, char* arg[])
{
	switch (argc)
	{
	case 5:
		{
			//execute a test for a given set of parameters...
			
			long receiverId;
			struct receiverInformation info;
			char registrationCode[REGISTRATION_CODE_LENGTH_CHARS +1];
			long confirmedReceiverId;
			char confirmationCode[CONFIRMATION_CODE_LENGTH_CHARS + 1];
			struct receiverInformation confirmedInfo;
			unsigned char hash[CONFIRMATION_CODE_LENGTH_RAW_CHARS];

			//parse the arguments...
			receiverId = atol(arg[1]);
			info.platform = (enum receiverPlatform) atoi(arg[2]);
			info.manufacturer = (enum receiverManufacturer) atoi (arg[3]);
			info.tuner_config = (enum receiverConfiguration) atoi (arg[4]);

			//print out initial information...
			printf("receiver ID = %li\n", receiverId);
			if (validateReceiverId(receiverId) < 0)
			{
				printf("receiver id is not 28 bit.");
				return 0;
			}
			printf("platform ID = %i\n", info.platform);
			printf("manufacturer ID = %i\n", info.manufacturer);
			printf("tuner config ID = %i\n", info.tuner_config);
			printf("\nNow calculating derived codes...\n");

			//calculate the registration code...
			getRegistrationCodeFromReceiverId(receiverId, registrationCode);
			printf("registrationCode = %s\n", registrationCode);

			//calculate the received ID...
			confirmedReceiverId = getReceieverIdFromRegistrationCode(registrationCode);
			if (confirmedReceiverId < 0)
			{
				printf("registration code is invalid\n");
			}
			else
			{
				printf("registration code is valid, confirmed receiver id = %li\n", confirmedReceiverId);
			}

			//calculate the MD5 hash...
			getHashedRegistrationCode(registrationCode, hash);
			printf("MD5 hash bytes = %d, %d \n", hash[0], hash[1]);

			//calculate the confirmation code...
			getConfirmationCodeFromRegistrationCodeAndReceiverInformation(registrationCode, info, (char *) confirmationCode);
			printf("ConfirmationCode = %s\n", confirmationCode);

			//confirm the receiver information...
			getReceiverInformationFromConfirmationCode(&confirmedInfo, confirmationCode, receiverId);
			if ((info.platform == confirmedInfo.platform) && (info.manufacturer == confirmedInfo.manufacturer) && (info.tuner_config == confirmedInfo.tuner_config))
			{
				printf("receiver information matches:\n");
			}
			else
			{
				printf("receiver information DOES NOT match:\n");
			}
			printf("platform ID = %i\n", confirmedInfo.platform);
			printf("manufacturer ID = %i\n", confirmedInfo.manufacturer);
			printf("tuner config ID = %i\n", confirmedInfo.tuner_config);

			return 0;
		}

	case 2:
		{
			if (!strcmp(arg[1], "-test"))
			{
				long id;

				//execute the full test suite...
				printf("Testing registration code...\n");

				//run through every 1000 receiver IDs
				for (id = 0; id < 1 << 28; id = id + 1000)
				{
					char registrationCode[REGISTRATION_CODE_LENGTH_CHARS +1];
					char confirmationCode[CONFIRMATION_CODE_LENGTH_CHARS + 1];
					struct receiverInformation info;
					struct receiverInformation derivedInfo;
					int i,j,k;

					//check the registration code...
					getRegistrationCodeFromReceiverId(id, registrationCode);

					//check the length of the code...
					if (strlen(registrationCode) != REGISTRATION_CODE_LENGTH_CHARS)
					{
						printf("test failed for receiver id %li, registration code %s is incorrect length for receiver id %li", id, registrationCode, getReceieverIdFromRegistrationCode(registrationCode) );
						exit (0);
					}

					if (getReceieverIdFromRegistrationCode(registrationCode) != id)
					{
						printf("test failed for receiver id %li, registration code %s contains receiver id %li", id, registrationCode, getReceieverIdFromRegistrationCode(registrationCode) );
						exit (0);
					}

					//run through every manufacturers...
					for (i = MANUFACTURER_RESERVED; i <= MANUFACTURER_XORO; i++)
					{
						//run through every platform...
						for (j = PLATFORM_RESERVED; j <= PLATFORM_HD_PLUS; j++)
						{
							//run through every receiver configuration...
							for (k = CONFIG_INCOMPATIBLE; k <= CONFIG_MORE_THAN_TWO_UNCONSTRAINED_TPS; k++)
							{
								//set up the receiver information
								info.manufacturer = i;
								info.platform = j;
								info.tuner_config = k;

								//get and parse the confirmation codes...
								getConfirmationCodeFromRegistrationCodeAndReceiverInformation(registrationCode, info, confirmationCode);
								getReceiverInformationFromConfirmationCode(&derivedInfo, confirmationCode, id);

								//check the length of the code
								if (strlen(confirmationCode) != CONFIRMATION_CODE_LENGTH_CHARS)
								{
									printf("test failed for receiver id %li, confirmation code %s is incorrect length for receiver id %li", id, registrationCode, getReceieverIdFromRegistrationCode(registrationCode) );
									exit (0);
								}

								//check the receiver config..
								if ((info.manufacturer != derivedInfo.manufacturer) || (info.platform != derivedInfo.platform) || (info.tuner_config != derivedInfo.tuner_config))
								{
									printf("test failed for receiver id %li, manufacturer %i platform %i tuner config %i", id, info.manufacturer, info.platform, info.tuner_config );
									exit (0);
								}
							}
						}
					}


					//output a status update...
					printf ("%f%% complete\r", 100.0 * id/ (1 << 28));
				}
				printf("all registration codes valid\n");
				
				return 0;
			}
		}

	default:
		{
			//an incorrect number of parameters was specified, provide some help...
			printf ("No valid operation requires %i arguments\n\n", argc-1);
			printf ("syntax\n   remote_record_sample <receiverId> <platform_id> <manufacturer_id> <tuner_config>\n");
			printf ("   remote_record_sample -test\n\n");
			printf ("examples:\n   remote_record_sample 184537583 1 34 3\n   remote_record_sample 148572648 1 22 1\n");
		}
	}
	return 0;
}
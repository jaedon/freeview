
#include <hlib.h>

#include "_hlib_country.h"

typedef struct tag_iso3166_1
{
	HCHAR			szAlpha2_code[3];
	HCHAR			szAlpha3_code[4];
	HxCountry_e		eCode;
	HCHAR			szISOEngCountryName[64];
} ISO3166_1_t;


#define	ISO3166_ARRAYSIZE	(sizeof(s_ISO3166_1) / sizeof(s_ISO3166_1[0]))
/*
 below numeric number came from http://en.wikipedia.org/wiki/ISO_3166-1
 */
static const ISO3166_1_t s_ISO3166_1[] = {
	{ "AF", "AFG", eCountryID_AFG, "Afghanistan" 								 },
	{ "AX", "ALA", eCountryID_ALA, "Aland Islands"								 },
	{ "AL", "ALB", eCountryID_ALB, "Albania" 									 },
	{ "DZ", "DZA", eCountryID_DZA, "Algeria" 									 },
	{ "AS", "ASM", eCountryID_ASM, "American Samoa"								 },
	{ "AD", "AND", eCountryID_AND, "Andorra" 									 },
	{ "AO", "AGO", eCountryID_AGO, "Angola"										 },
	{ "AI", "AIA", eCountryID_AIA, "Anguilla"									 },
	{ "AQ", "ATA", eCountryID_ATA, "Antarctica"									 },
	{ "AG", "ATG", eCountryID_ATG, "Antigua and Barbuda" 						 },
	{ "AR", "ARG", eCountryID_ARG, "Argentina"									 },
	{ "AM", "ARM", eCountryID_ARM, "Armenia" 									 },
	{ "AW", "ABW", eCountryID_ABW, "Aruba"										 },
	{ "AU", "AUS", eCountryID_AUS, "Australia"									 },
	{ "AT", "AUT", eCountryID_AUT, "Austria" 									 },
	{ "AZ", "AZE", eCountryID_AZE, "Azerbaijan"									 },
	{ "BS", "BHS", eCountryID_BHS, "Bahamas" 									 },
	{ "BH", "BHR", eCountryID_BHR, "Bahrain" 									 },
	{ "BD", "BGD", eCountryID_BGD, "Bangladesh"									 },
	{ "BB", "BRB", eCountryID_BRB, "Barbados"									 },
	{ "BY", "BLR", eCountryID_BLR, "Belarus" 									 },
	{ "BE", "BEL", eCountryID_BEL, "Belgium" 									 },
	{ "BZ", "BLZ", eCountryID_BLZ, "Belize"										 },
	{ "BJ", "BEN", eCountryID_BEN, "Benin"										 },
	{ "BM", "BMU", eCountryID_BMU, "Bermuda" 									 },
	{ "BT", "BTN", eCountryID_BTN, "Bhutan"										 },
	{ "BO", "BOL", eCountryID_BOL, "Bolivia, Plurinational State of" 			 },
	{ "BQ", "BES", eCountryID_BES, "Bonaire, Sint Eustatius and Saba"			 },
	{ "BA", "BIH", eCountryID_BIH, "Bosnia and Herzegovina"						 },
	{ "BW", "BWA", eCountryID_BWA, "Botswana"									 },
	{ "BV", "BVT", eCountryID_BVT, "Bouvet Island"								 },
	{ "BR", "BRA", eCountryID_BRA, "Brazil"										 },
	{ "IO", "IOT", eCountryID_IOT, "British Indian Ocean Territory"				 },
	{ "BN", "BRN", eCountryID_BRN, "Brunei Darussalam"							 },
	{ "BG", "BGR", eCountryID_BGR, "Bulgaria"									 },
	{ "BF", "BFA", eCountryID_BFA, "Burkina Faso"								 },
	{ "BI", "BDI", eCountryID_BDI, "Burundi" 									 },
	{ "KH", "KHM", eCountryID_KHM, "Cambodia"									 },
	{ "CM", "CMR", eCountryID_CMR, "Cameroon"									 },
	{ "CA", "CAN", eCountryID_CAN, "Canada"										 },
	{ "CV", "CPV", eCountryID_CPV, "Cape Verde"									 },
	{ "KY", "CYM", eCountryID_CYM, "Cayman Islands"								 },
	{ "CF", "CAF", eCountryID_CAF, "Central African Republic"					 },
	{ "TD", "TCD", eCountryID_TCD, "Chad"										 },
	{ "CL", "CHL", eCountryID_CHL, "Chile"										 },
	{ "CN", "CHN", eCountryID_CHN, "China"										 },
	{ "CX", "CXR", eCountryID_CXR, "Christmas Island"							 },
	{ "CC", "CCK", eCountryID_CCK, "Cocos (Keeling) Islands" 					 },
	{ "CO", "COL", eCountryID_COL, "Colombia"									 },
	{ "KM", "COM", eCountryID_COM, "Comoros" 									 },
	{ "CG", "COG", eCountryID_COG, "Congo"										 },
	{ "CD", "COD", eCountryID_COD, "Congo, the Democratic Republic of the"		 },
	{ "CK", "COK", eCountryID_COK, "Cook Islands"								 },
	{ "CR", "CRI", eCountryID_CRI, "Costa Rica"									 },
	{ "CI", "CIV", eCountryID_CIV, "Cote d'Ivoire"								 },
	{ "HR", "HRV", eCountryID_HRV, "Croatia" 									 },
	{ "CU", "CUB", eCountryID_CUB, "Cuba"										 },
	{ "CW", "CUW", eCountryID_CUW, "Curacao" 									 },
	{ "CY", "CYP", eCountryID_CYP, "Cyprus"										 },
	{ "CZ", "CZE", eCountryID_CZE, "Czech Republic"								 },
	{ "DK", "DNK", eCountryID_DNK, "Denmark" 									 },
	{ "DJ", "DJI", eCountryID_DJI, "Djibouti"									 },
	{ "DM", "DMA", eCountryID_DMA, "Dominica"									 },
	{ "DO", "DOM", eCountryID_DOM, "Dominican Republic"							 },
	{ "EC", "ECU", eCountryID_ECU, "Ecuador" 									 },
	{ "EG", "EGY", eCountryID_EGY, "Egypt"										 },
	{ "SV", "SLV", eCountryID_SLV, "El Salvador" 								 },
	{ "GQ", "GNQ", eCountryID_GNQ, "Equatorial Guinea"							 },
	{ "ER", "ERI", eCountryID_ERI, "Eritrea" 									 },
	{ "EE", "EST", eCountryID_EST, "Estonia" 									 },
	{ "ET", "ETH", eCountryID_ETH, "Ethiopia"									 },
	{ "FK", "FLK", eCountryID_FLK, "Falkland Islands (Malvinas)" 				 },
	{ "FO", "FRO", eCountryID_FRO, "Faroe Islands"								 },
	{ "FJ", "FJI", eCountryID_FJI, "Fiji"										 },
	{ "FI", "FIN", eCountryID_FIN, "Finland" 									 },
	{ "FR", "FRA", eCountryID_FRA, "France"										 },
	{ "GF", "GUF", eCountryID_GUF, "French Guiana"								 },
	{ "PF", "PYF", eCountryID_PYF, "French Polynesia"							 },
	{ "TF", "ATF", eCountryID_ATF, "French Southern Territories" 				 },
	{ "GA", "GAB", eCountryID_GAB, "Gabon"										 },
	{ "GM", "GMB", eCountryID_GMB, "Gambia"										 },
	{ "GE", "GEO", eCountryID_GEO, "Georgia" 									 },
	{ "DE", "DEU", eCountryID_DEU, "Germany" 									 },
	{ "GH", "GHA", eCountryID_GHA, "Ghana"										 },
	{ "GI", "GIB", eCountryID_GIB, "Gibraltar"									 },
	{ "GR", "GRC", eCountryID_GRC, "Greece"										 },
	{ "GL", "GRL", eCountryID_GRL, "Greenland"									 },
	{ "GD", "GRD", eCountryID_GRD, "Grenada" 									 },
	{ "GP", "GLP", eCountryID_GLP, "Guadeloupe"									 },
	{ "GU", "GUM", eCountryID_GUM, "Guam"										 },
	{ "GT", "GTM", eCountryID_GTM, "Guatemala"									 },
	{ "GG", "GGY", eCountryID_GGY, "Guernsey"									 },
	{ "GN", "GIN", eCountryID_GIN, "Guinea"										 },
	{ "GW", "GNB", eCountryID_GNB, "Guinea-Bissau"								 },
	{ "GY", "GUY", eCountryID_GUY, "Guyana"										 },
	{ "HT", "HTI", eCountryID_HTI, "Haiti"										 },
	{ "HM", "HMD", eCountryID_HMD, "Heard Island and McDonald Islands"			 },
	{ "VA", "VAT", eCountryID_VAT, "Holy See (Vatican City State)"				 },
	{ "HN", "HND", eCountryID_HND, "Honduras"									 },
	{ "HK", "HKG", eCountryID_HKG, "Hong Kong"									 },
	{ "HU", "HUN", eCountryID_HUN, "Hungary" 									 },
	{ "IS", "ISL", eCountryID_ISL, "Iceland" 									 },
	{ "IN", "IND", eCountryID_IND, "India"										 },
	{ "ID", "IDN", eCountryID_IDN, "Indonesia"									 },
	{ "IR", "IRN", eCountryID_IRN, "Iran, Islamic Republic of"					 },
	{ "IQ", "IRQ", eCountryID_IRQ, "Iraq"										 },
	{ "IE", "IRL", eCountryID_IRL, "Ireland" 									 },
	{ "IM", "IMN", eCountryID_IMN, "Isle of Man" 								 },
	{ "IL", "ISR", eCountryID_ISR, "Israel"										 },
	{ "IT", "ITA", eCountryID_ITA, "Italy"										 },
	{ "JM", "JAM", eCountryID_JAM, "Jamaica" 									 },
	{ "JP", "JPN", eCountryID_JPN, "Japan"										 },
	{ "JE", "JEY", eCountryID_JEY, "Jersey"										 },
	{ "JO", "JOR", eCountryID_JOR, "Jordan"										 },
	{ "KZ", "KAZ", eCountryID_KAZ, "Kazakhstan"									 },
	{ "KE", "KEN", eCountryID_KEN, "Kenya"										 },
	{ "KI", "KIR", eCountryID_KIR, "Kiribati"									 },
	{ "KP", "PRK", eCountryID_PRK, "Korea, Democratic People's Republic of"		 },
	{ "KR", "KOR", eCountryID_KOR, "Korea, Republic of"							 },
	{ "KW", "KWT", eCountryID_KWT, "Kuwait"										 },
	{ "KG", "KGZ", eCountryID_KGZ, "Kyrgyzstan"									 },
	{ "LA", "LAO", eCountryID_LAO, "Lao People's Democratic Republic"			 },
	{ "LV", "LVA", eCountryID_LVA, "Latvia"										 },
	{ "LB", "LBN", eCountryID_LBN, "Lebanon" 									 },
	{ "LS", "LSO", eCountryID_LSO, "Lesotho" 									 },
	{ "LR", "LBR", eCountryID_LBR, "Liberia" 									 },
	{ "LY", "LBY", eCountryID_LBY, "Libyan Arab Jamahiriya"						 },
	{ "LI", "LIE", eCountryID_LIE, "Liechtenstein"								 },
	{ "LT", "LTU", eCountryID_LTU, "Lithuania"									 },
	{ "LU", "LUX", eCountryID_LUX, "Luxembourg"									 },
	{ "MO", "MAC", eCountryID_MAC, "Macao"										 },
	{ "MK", "MKD", eCountryID_MKD, "Macedonia"									 },
	{ "MG", "MDG", eCountryID_MDG, "Madagascar"									 },
	{ "MW", "MWI", eCountryID_MWI, "Malawi"										 },
	{ "MY", "MYS", eCountryID_MYS, "Malaysia"									 },
	{ "MV", "MDV", eCountryID_MDV, "Maldives"									 },
	{ "ML", "MLI", eCountryID_MLI, "Mali"										 },
	{ "MT", "MLT", eCountryID_MLT, "Malta"										 },
	{ "MH", "MHL", eCountryID_MHL, "Marshall Islands"							 },
	{ "MQ", "MTQ", eCountryID_MTQ, "Martinique"									 },
	{ "MR", "MRT", eCountryID_MRT, "Mauritania"									 },
	{ "MU", "MUS", eCountryID_MUS, "Mauritius"									 },
	{ "YT", "MYT", eCountryID_MYT, "Mayotte" 									 },
	{ "MX", "MEX", eCountryID_MEX, "Mexico"										 },
	{ "FM", "FSM", eCountryID_FSM, "Micronesia, Federated States of" 			 },
	{ "MD", "MDA", eCountryID_MDA, "Moldova, Republic of"						 },
	{ "MC", "MCO", eCountryID_MCO, "Monaco"										 },
	{ "MN", "MNG", eCountryID_MNG, "Mongolia"									 },
	{ "ME", "MNE", eCountryID_MNE, "Montenegro"									 },
	{ "MS", "MSR", eCountryID_MSR, "Montserrat"									 },
	{ "MA", "MAR", eCountryID_MAR, "Morocco" 									 },
	{ "MZ", "MOZ", eCountryID_MOZ, "Mozambique"									 },
	{ "MM", "MMR", eCountryID_MMR, "Myanmar" 									 },
	{ "NA", "NAM", eCountryID_NAM, "Namibia" 									 },
	{ "NR", "NRU", eCountryID_NRU, "Nauru"										 },
	{ "NP", "NPL", eCountryID_NPL, "Nepal"										 },
	{ "NL", "NLD", eCountryID_NLD, "Netherlands" 								 },
	{ "NC", "NCL", eCountryID_NCL, "New Caledonia"								 },
	{ "NZ", "NZL", eCountryID_NZL, "New Zealand" 								 },
	{ "NI", "NIC", eCountryID_NIC, "Nicaragua"									 },
	{ "NE", "NER", eCountryID_NER, "Niger"										 },
	{ "NG", "NGA", eCountryID_NGA, "Nigeria" 									 },
	{ "NU", "NIU", eCountryID_NIU, "Niue"										 },
	{ "NF", "NFK", eCountryID_NFK, "Norfolk Island"								 },
	{ "MP", "MNP", eCountryID_MNP, "Northern Mariana Islands"					 },
	{ "NO", "NOR", eCountryID_NOR, "Norway"										 },
	{ "OM", "OMN", eCountryID_OMN, "Oman"										 },
	{ "PK", "PAK", eCountryID_PAK, "Pakistan"									 },
	{ "PW", "PLW", eCountryID_PLW, "Palau"										 },
	{ "PS", "PSE", eCountryID_PSE, "Palestinian Territory, Occupied" 			 },
	{ "PA", "PAN", eCountryID_PAN, "Panama"										 },
	{ "PG", "PNG", eCountryID_PNG, "Papua New Guinea"							 },
	{ "PY", "PRY", eCountryID_PRY, "Paraguay"									 },
	{ "PE", "PER", eCountryID_PER, "Peru"										 },
	{ "PH", "PHL", eCountryID_PHL, "Philippines" 								 },
	{ "PN", "PCN", eCountryID_PCN, "Pitcairn"									 },
	{ "PL", "POL", eCountryID_POL, "Poland"										 },
	{ "PT", "PRT", eCountryID_PRT, "Portugal"									 },
	{ "PR", "PRI", eCountryID_PRI, "Puerto Rico" 								 },
	{ "QA", "QAT", eCountryID_QAT, "Qatar"										 },
	{ "RE", "REU", eCountryID_REU, "Reunion" 									 },
	{ "RO", "ROU", eCountryID_ROU, "Romania" 									 },
	{ "RU", "RUS", eCountryID_RUS, "Russian Federation"							 },
	{ "RW", "RWA", eCountryID_RWA, "Rwanda"										 },
	{ "BL", "BLM", eCountryID_BLM, "Saint Barthelemy"							 },
	{ "SH", "SHN", eCountryID_SHN, "Saint Helena, Ascension and Tristan da Cunha" },
	{ "KN", "KNA", eCountryID_KNA, "Saint Kitts and Nevis"						 },
	{ "LC", "LCA", eCountryID_LCA, "Saint Lucia" 								 },
	{ "MF", "MAF", eCountryID_MAF, "Saint Martin (French part)"					 },
	{ "PM", "SPM", eCountryID_SPM, "Saint Pierre and Miquelon"					 },
	{ "VC", "VCT", eCountryID_VCT, "Saint Vincent and the Grenadines"			 },
	{ "WS", "WSM", eCountryID_WSM, "Samoa"										 },
	{ "SM", "SMR", eCountryID_SMR, "San Marino"									 },
	{ "ST", "STP", eCountryID_STP, "Sao Tome and Principe"						 },
	{ "SA", "SAU", eCountryID_SAU, "Saudi Arabia"								 },
	{ "SN", "SEN", eCountryID_SEN, "Senegal" 									 },
	{ "RS", "SRB", eCountryID_SRB, "Serbia"										 },
	{ "SC", "SYC", eCountryID_SYC, "Seychelles"									 },
	{ "SL", "SLE", eCountryID_SLE, "Sierra Leone"								 },
	{ "SG", "SGP", eCountryID_SGP, "Singapore"									 },
	{ "SX", "SXM", eCountryID_SXM, "Sint Maarten (Dutch part)"					 },
	{ "SK", "SVK", eCountryID_SVK, "Slovakia"									 },
	{ "SI", "SVN", eCountryID_SVN, "Slovenia"									 },
	{ "SB", "SLB", eCountryID_SLB, "Solomon Islands" 							 },
	{ "SO", "SOM", eCountryID_SOM, "Somalia" 									 },
	{ "ZA", "ZAF", eCountryID_ZAF, "South Africa"								 },
	{ "GS", "SGS", eCountryID_SGS, "South Georgia and the South Sandwich Islands" },
	{ "SS", "SSD", eCountryID_SSD, "South Sudan" 								 },
	{ "ES", "ESP", eCountryID_ESP, "Spain"										 },
	{ "LK", "LKA", eCountryID_LKA, "Sri Lanka"									 },
	{ "SD", "SDN", eCountryID_SDN, "Sudan"										 },
	{ "SR", "SUR", eCountryID_SUR, "Suriname"									 },
	{ "SJ", "SJM", eCountryID_SJM, "Svalbard and Jan Mayen"						 },
	{ "SZ", "SWZ", eCountryID_SWZ, "Swaziland"									 },
	{ "SE", "SWE", eCountryID_SWE, "Sweden"										 },
	{ "CH", "CHE", eCountryID_CHE, "Switzerland" 								 },
	{ "SY", "SYR", eCountryID_SYR, "Syrian Arab Republic"						 },
	{ "TW", "TWN", eCountryID_TWN, "Taiwan, Province of China"					 },
	{ "TJ", "TJK", eCountryID_TJK, "Tajikistan"									 },
	{ "TZ", "TZA", eCountryID_TZA, "Tanzania, United Republic of"				 },
	{ "TH", "THA", eCountryID_THA, "Thailand"									 },
	{ "TL", "TLS", eCountryID_TLS, "Timor-Leste" 								 },
	{ "TG", "TGO", eCountryID_TGO, "Togo"										 },
	{ "TK", "TKL", eCountryID_TKL, "Tokelau" 									 },
	{ "TO", "TON", eCountryID_TON, "Tonga"										 },
	{ "TT", "TTO", eCountryID_TTO, "Trinidad and Tobago" 						 },
	{ "TN", "TUN", eCountryID_TUN, "Tunisia" 									 },
	{ "TR", "TUR", eCountryID_TUR, "Turkey"										 },
	{ "TM", "TKM", eCountryID_TKM, "Turkmenistan"								 },
	{ "TC", "TCA", eCountryID_TCA, "Turks and Caicos Islands"					 },
	{ "TV", "TUV", eCountryID_TUV, "Tuvalu"										 },
	{ "UG", "UGA", eCountryID_UGA, "Uganda"										 },
	{ "UA", "UKR", eCountryID_UKR, "Ukraine" 									 },
	{ "AE", "ARE", eCountryID_ARE, "United Arab Emirates"						 },
	{ "GB", "GBR", eCountryID_GBR, "United Kingdom"								 },
	{ "US", "USA", eCountryID_USA, "United States"								 },
	{ "UM", "UMI", eCountryID_UMI, "United States Minor Outlying Islands"		 },
	{ "UY", "URY", eCountryID_URY, "Uruguay" 									 },
	{ "UZ", "UZB", eCountryID_UZB, "Uzbekistan"									 },
	{ "VU", "VUT", eCountryID_VUT, "Vanuatu" 									 },
	{ "VE", "VEN", eCountryID_VEN, "Venezuela, Bolivarian Republic of"			 },
	{ "VN", "VNM", eCountryID_VNM, "Viet Nam"									 },
	{ "VG", "VGB", eCountryID_VGB, "Virgin Islands, British" 					 },
	{ "VI", "VIR", eCountryID_VIR, "Virgin Islands, U.S."						 },
	{ "WF", "WLF", eCountryID_WLF, "Wallis and Futuna"							 },
	{ "EH", "ESH", eCountryID_ESH, "Western Sahara"								 },
	{ "YE", "YEM", eCountryID_YEM, "Yemen"										 },
	{ "ZM", "ZMB", eCountryID_ZMB, "Zambia"										 },
	{ "ZW", "ZWE", eCountryID_ZWE, "Zimbabwe"									 }
};

#define ____PRIVATE_FUNCTION____

static HINT32	hlib_country_CompareAlpha2(const ISO3166_1_t *lhs, const ISO3166_1_t *rhs)
{
	return HLIB_STD_StrCaseCmp(lhs->szAlpha2_code, rhs->szAlpha2_code);
}

static HINT32	hlib_country_CompareAlpha3(const ISO3166_1_t *lhs, const ISO3166_1_t *rhs)
{
	return HLIB_STD_StrCaseCmp(lhs->szAlpha3_code, rhs->szAlpha3_code);
}

static HINT32	hlib_country_CompareCode(const ISO3166_1_t *lhs, const ISO3166_1_t *rhs)
{
	return (HINT32)lhs->eCode - (HINT32)rhs->eCode;
}

static HINT32	hlib_country_CompareName(const ISO3166_1_t *lhs, const ISO3166_1_t *rhs)
{
	return HLIB_STD_StrCaseCmp(lhs->szISOEngCountryName, rhs->szISOEngCountryName);
}


static ISO3166_1_t *	hlib_country_GetListSortedByAlpha2 (void)
{
	static ISO3166_1_t	*s_list;

	if (s_list == NULL)
	{
		HUINT32	n = ISO3166_ARRAYSIZE;
		s_list = (ISO3166_1_t *)HLIB_MEM_MemDup(s_ISO3166_1, sizeof(ISO3166_1_t) * n);
		if (s_list == NULL)
			return NULL;
		qsort(s_list, n, sizeof(ISO3166_1_t), (int (*)(const void *, const void *))hlib_country_CompareAlpha2);
	}
	return s_list;
}

static ISO3166_1_t *	hlib_country_GetListSortedByAlpha3 (void)
{
	static ISO3166_1_t	*s_list;

	if (s_list == NULL)
	{
		HUINT32	n = ISO3166_ARRAYSIZE;
		s_list = (ISO3166_1_t *)HLIB_MEM_MemDup(s_ISO3166_1, sizeof(ISO3166_1_t) * n);
		if (s_list == NULL)
			return NULL;
		qsort(s_list, n, sizeof(ISO3166_1_t), (int (*)(const void *, const void *))hlib_country_CompareAlpha3);
	}
	return s_list;
}

static ISO3166_1_t *	hlib_country_GetListSortedByCode (void)
{
	static ISO3166_1_t	*s_list;

	if (s_list == NULL)
	{
		HUINT32	n = ISO3166_ARRAYSIZE;
		s_list = (ISO3166_1_t *)HLIB_MEM_MemDup(s_ISO3166_1, sizeof(ISO3166_1_t) * n);
		if (s_list == NULL)
			return NULL;
		qsort(s_list, n, sizeof(ISO3166_1_t), (int (*)(const void *, const void *))hlib_country_CompareCode);
	}
	return s_list;
}

static ISO3166_1_t *	hlib_country_GetListSortedByName (void)
{
	static ISO3166_1_t	*s_list;

	if (s_list == NULL)
	{
		HUINT32	n = ISO3166_ARRAYSIZE;
		s_list = (ISO3166_1_t *)HLIB_MEM_MemDup(s_ISO3166_1, sizeof(ISO3166_1_t) * n);
		if (s_list == NULL)
			return NULL;
		qsort(s_list, n, sizeof(ISO3166_1_t), (int (*)(const void *, const void *))hlib_country_CompareName);
	}
	return s_list;
}


#define ____PUBLIC_FUNCTION____
HxCountry_e		HLIB_COUNTRY_3166_Alpha2ToId (const HCHAR *codeAlpha2)
{
	ISO3166_1_t		key;
	ISO3166_1_t		*ret;
	ISO3166_1_t		*list;

	HxLOG_Assert(codeAlpha2);

	HLIB_STD_StrNCpySafe(key.szAlpha2_code, codeAlpha2, 3);
	list = hlib_country_GetListSortedByAlpha2();
	ret  = (ISO3166_1_t *)bsearch(&key, list, ISO3166_ARRAYSIZE, sizeof(ISO3166_1_t), (void *)hlib_country_CompareAlpha2);
	if (ret == NULL)
	{
		HxLOG_Error("%s(%s) failed\n", __FUNCTION__, codeAlpha2);
		return eCountryID_NONE;
	}
	return ret->eCode;
}


HxCountry_e		HLIB_COUNTRY_3166_Alpha3ToId		(const HCHAR *codeAlpha3)
{
	ISO3166_1_t 	key;
	ISO3166_1_t 	*ret;
	ISO3166_1_t 	*list;

	HxLOG_Assert(codeAlpha3);

	HLIB_STD_StrNCpySafe(key.szAlpha3_code, codeAlpha3, 4);
	list = hlib_country_GetListSortedByAlpha3();
	ret  = (ISO3166_1_t *)bsearch(&key, list, ISO3166_ARRAYSIZE, sizeof(ISO3166_1_t), (void *)hlib_country_CompareAlpha3);
	if (ret == NULL)
	{
		HxLOG_Error("%s(%s) failed\n", __FUNCTION__, codeAlpha3);
		return eCountryID_NONE;
	}
	return ret->eCode;
}

HxCountry_e		HLIB_COUNTRY_FullNameToId		(const HCHAR *szFullName)
{
	ISO3166_1_t 	key;
	ISO3166_1_t 	*ret;
	ISO3166_1_t 	*list;

	HxLOG_Assert(szFullName);

	HLIB_STD_StrNCpySafe(key.szISOEngCountryName, szFullName, 64);
	list = hlib_country_GetListSortedByName();
	ret  = (ISO3166_1_t *)bsearch(&key, list, ISO3166_ARRAYSIZE, sizeof(ISO3166_1_t), (void *)hlib_country_CompareName);
	if (ret == NULL)
	{
		HxLOG_Error("%s(%s) failed\n", __FUNCTION__, szFullName);
		return eCountryID_NONE;
	}
	return ret->eCode;
}


const HCHAR *HLIB_COUNTRY_IdTo3166_Alpha2		(HxCountry_e eId)
{
	ISO3166_1_t 	key;
	ISO3166_1_t 	*ret;
	ISO3166_1_t 	*list;

	key.eCode = eId;

	list = hlib_country_GetListSortedByCode();
	ret  = (ISO3166_1_t *)bsearch(&key, list, ISO3166_ARRAYSIZE, sizeof(ISO3166_1_t), (void *)hlib_country_CompareCode);
	if (ret == NULL)
	{
		HxLOG_Error("%s(%d) failed\n", __FUNCTION__, eId);
		return NULL;
	}
	return ret->szAlpha2_code;
}


const HCHAR *HLIB_COUNTRY_IdTo3166_Alpha3		(HxCountry_e eId)
{
	ISO3166_1_t 	key;
	ISO3166_1_t 	*ret;
	ISO3166_1_t 	*list;

	key.eCode = eId;

	list = hlib_country_GetListSortedByCode();
	ret  = (ISO3166_1_t *)bsearch(&key, list, ISO3166_ARRAYSIZE, sizeof(ISO3166_1_t), (void *)hlib_country_CompareCode);
	if (ret == NULL)
	{
		HxLOG_Error("%s(%d) failed\n", __FUNCTION__, eId);
		return NULL;
	}
	return ret->szAlpha3_code;
}



const HCHAR *HLIB_COUNTRY_IdToFullName		(HxCountry_e eId)
{
	ISO3166_1_t 	key;
	ISO3166_1_t 	*ret;
	ISO3166_1_t 	*list;

	key.eCode = eId;

	list = hlib_country_GetListSortedByCode();
	ret  = (ISO3166_1_t *)bsearch(&key, list, ISO3166_ARRAYSIZE, sizeof(ISO3166_1_t), (void *)hlib_country_CompareCode);
	if (ret == NULL)
	{
		HxLOG_Error("%s(%d) failed\n", __FUNCTION__, eId);
		return NULL;
	}
	return ret->szISOEngCountryName;
}


const HCHAR *HLIB_COUNTRY_3166_Alpha2ToFullName		(const HCHAR *codeAlpha2)
{
	ISO3166_1_t 	key;
	ISO3166_1_t 	*ret;
	ISO3166_1_t 	*list;

	HxLOG_Assert(codeAlpha2);

	HLIB_STD_StrNCpySafe(key.szAlpha2_code, codeAlpha2, 3);
	list = hlib_country_GetListSortedByAlpha2();
	ret  = (ISO3166_1_t *)bsearch(&key, list, ISO3166_ARRAYSIZE, sizeof(ISO3166_1_t), (void *)hlib_country_CompareAlpha2);
	if (ret == NULL)
	{
		HxLOG_Error("%s(%s) failed\n", __FUNCTION__, codeAlpha2);
		return NULL;
	}
	return ret->szISOEngCountryName;
}


const HCHAR *HLIB_COUNTRY_3166_Alpha3ToFullName		(const HCHAR *codeAlpha3)
{
	ISO3166_1_t 	key;
	ISO3166_1_t 	*ret;
	ISO3166_1_t 	*list;

	HxLOG_Assert(codeAlpha3);

	HLIB_STD_StrNCpySafe(key.szAlpha3_code, codeAlpha3, 4);
	list = hlib_country_GetListSortedByAlpha3();
	ret  = (ISO3166_1_t *)bsearch(&key, list, ISO3166_ARRAYSIZE, sizeof(ISO3166_1_t), (void *)hlib_country_CompareAlpha3);
	if (ret == NULL)
	{
		HxLOG_Error("%s(%s) failed\n", __FUNCTION__, codeAlpha3);
		return NULL;
	}
	return ret->szISOEngCountryName;
}

HBOOL HLIB_COUNTRY_IsValidCountryId(HxCountry_e eId)
{
	if((eCountryID_NONE != eId)
		&& (eCountryID_OTHER != eId)
		&& (0 < eId))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/* end of file */

#############################################
#					MW/CH Configuration					#
#############################################

comment "MW/CH Configuration. (MW_CH_XXX)"

	comment	 "Select delivery systems. You can choose more than one delivery system."
	config MW_CH_SATELLITE
		bool "MW_CH_SATELLITE - Support satellite tunning"
		default y

		config MW_CH_DVB_S2
			depends on MW_CH_SATELLITE
			bool "MW_CH_DVB_S2 - Specify DVB-S2"
			default n

		config MW_CH_SAT_1TUNER
			depends on MW_CH_SATELLITE
			bool "MW_CH_SAT_1TUNER - SCD 1 tuner"
			default n

		config MW_CH_SAT_RFINPUT_SELECTABLE
			depends on MW_CH_SATELLITE
			bool "MW_CH_SAT_RFINPUT_SELECTABLE - Tuner can selectable RF-Inputs"
			default n

	config MW_CH_TERRESTRIAL
		bool "MW_CH_TERRESTRIAL - Support terrestrial tunning"
		default y

		config MW_CH_ANTENNA_5V
			depends on MW_CH_TERRESTRIAL
			bool "MW_CH_ANTENNA_5V - Support 5V Antenna"
			default y

		choice
			depends on MW_CH_TERRESTRIAL
			prompt "Select terrestrial spec."
			default MW_CH_DVB_T

			config MW_CH_DVB_T
				bool "MW_CH_DVB_T - Support DVB-T"

			config MW_CH_DVB_T2
				bool "MW_CH_DVB_T2 - Support DVB-T2"

			config MW_CH_ISDB_T
				bool "MW_CH_ISDB_T - Support Japanese ISDB-T"
		endchoice

	config MW_CH_CABLE
		bool "MW_CH_CABLE - Support cable tunning"
		default n

		config MW_CH_CAB_AUTO_SR
			depends on MW_CH_CABLE
			bool "MW_CH_CAB_AUTO_SR - Cable Auto Symbol-Rate"
			default n

	config MW_CH_ANALOG
		bool "MW_CH_ANALOG - Support analog tunning"
		default n

	config MW_CH_AUTOMATIC_RETUNE
		bool "MW_CH_AUTOMATIC_RETUNE - Re-tune the TP with the auto parameters when the tuner fails to tuning TP."
		default n

	config MW_CH_LOOP_THROUGH
		bool "MW_CH_LOOP_THROUGH - Support loop-through"
		default n

	config MW_CH_EMULATION
		bool "_EMULATION - Emulate the channels by the network IP"
		default n



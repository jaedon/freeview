/// <reference path="../def/config.d.ts" />
/**
 * Created by bglee@humaxdigital.com on 2014. 3. 21..
 */

import __DEF__ = require("../def/config.def");

var config: ifProductSetting = {
    config: {
        model: __DEF__.eProductName.FVP4000T,
        op: __DEF__.eOP.UKDTT,
        useCAS: __DEF__.eCAS.NONE,
        maxTuner: 3,
        usefrontPanel : '',
        usePrismLiteS: false,
        useLiveTuningInfoShow: true,
        netflix : true,
        voiceAssistant : false //alexa
    },
    page: {
        /* TODO
         * replace original CPgGuide
         * bglee@humaxdigital.com, 9:47, 2014. 9. 1.
         */
//        CPgGuide: 'pages/tvguide/page_tv_guide.js',
        CPgSettings: 'product/fvp4000t/pages/settings/page_settings.js',
        CPgLanguage: 'product/fvp4000t/pages/settings/general_settings/page_language.js',
        CPgHiddenSetting: 'product/fvp4000t/pages/page_hidden.js',
        CPgRecording : 'product/fvp4000t/pages/settings/pvr_settings/page_recording.js',
        CPgOnDemand : 'pages/on_demand/page_on_demand.js',
        CPgInfoHub : 'product/fvp4000t/pages/live/page_infohub2.js'
    },
    languageTable: {
        gla: __DEF__.eISO639.eng,
        wel: __DEF__.eISO639.eng,
        gle: __DEF__.eISO639.eng
    },
    disableService: ["CAS_IR_EMM_FORCED_OTA"],
    frontPanel: null,
    release: false
};
export = config;
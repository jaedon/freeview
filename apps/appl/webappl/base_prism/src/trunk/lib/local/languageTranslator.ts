/**
 * Created by bglee@humaxdigital.com on 2014. 5. 11..
 * using require.js
 */
/// <reference path="../../def/require.d.ts" />
/// <reference path="../../def/jquery.d.ts" />
/// <reference path="../../def/dateformat.d.ts" />

declare var hx: any;
declare var dateFormat;

var _json;
var _iso639table;

var toISO639 = function(aLanguage: string) {
    if (!_iso639table[aLanguage]) {
        throw "check _LANGUAGE_TABLE_";
    }
    return _iso639table[aLanguage];
};
function translateIDtoLangauge(aID: string) {
    var ret: string = _json[aID];
    if (!ret) {
        hx.log("warning", "undefined key in Language table - [" + aID + "]");
        return aID;
        //throw "undefined key in Language table - [" + aID + "]";
    }
    return ret;
}
function load(aLanguageCode: string, aCallback?: () => void) {
    var iso639 = toISO639(aLanguageCode);
    require(["text!res/" + iso639 + ".json"], (aData) => {
        /* COMMENT
         * res/*.json files are not json format
         * * bglee@humaxdigital.com, 15:09, 2014. 6. 2.
         */
//        _json = JSON.parse(aData);
        eval(aData);
        _json = hx["_" + iso639];
        delete hx["_" + iso639];
        if (aCallback) {
            aCallback();
        }
    });
}
function reload(aLanguage: string, aCallback?: () => void) {
    load(aLanguage, function() {
        $("[data-langid]").each(function(n) {
            var l = translateIDtoLangauge;
            var $n = $(this);
            var emTemp = '';
            if ($n.find('em').length > 0) {
                emTemp = $n.find('em').attr('data-langID');
            }
            $n.text(l($n.attr('data-langID')));
            if (emTemp !== '') {
                $n.append($('<em>').attr('data-langID', emTemp).html(l(emTemp)));
            }

            dateFormat.set(undefined,
                [l('LOC_SUN_ID'), l('LOC_MON_ID'), l('LOC_TUE_ID'), l('LOC_WED_ID'), l('LOC_THU_ID'), l('LOC_FRI_ID'), l('LOC_SAT_ID'), l('LOC_SUNDAY_ID'), l('LOC_MONDAY_ID'), l('LOC_TUESDAY_ID'), l('LOC_WEDNESDAY_ID'), l('LOC_THURSDAY_ID'), l('LOC_FRIDAY_ID'), l('LOC_SATURDAY_ID')],
                [l('LOC_JAN_ID'), l('LOC_FEB_ID'), l('LOC_MAR_ID'), l('LOC_APR_ID'), l('LOC_MAY_01_ID'), l('LOC_JUN_ID'), l('LOC_JUL_ID'), l('LOC_AUG_ID'), l('LOC_SEP_ID'), l('LOC_OCT_ID'), l('LOC_NOV_ID'), l('LOC_DEC_ID'), l('LOC_JANUARY_ID'), l('LOC_FEBRUARY_ID'), l('LOC_MARCH_ID'), l('LOC_APRIL_ID'), l('LOC_MAY_ID'), l('LOC_JUNE_ID'), l('LOC_JULY_ID'), l('LOC_AUGUST_ID'), l('LOC_SEPTEMBER_ID'), l('LOC_OCTOBER_ID'), l('LOC_NOVEMBER_ID'), l('LOC_DECEMBER_ID')]);
        });
        if(aCallback){
            aCallback();
        }
    });
}

translateIDtoLangauge['load'] = load;
translateIDtoLangauge['reload'] = reload;

class CLanguageTranslate {
    constructor(aISO639Table: any) {
        _iso639table = aISO639Table;
        return translateIDtoLangauge;
    }
}

export = CLanguageTranslate;
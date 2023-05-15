/// <reference path="../../def/sugar.d.ts" />
// file name : fw/common/infra/illang.js
/* jslint sloppy:true, nomen:true, vars:true */
//TODO : get language setting from infrastructure layer,
define(["require", "exports"], function (require, exports) {
    var _lang = {};
    var l = function (aStringID) {
        var ret = _lang[aStringID];
        if (!ret) {
            throw "Not defined key in Language table - [" + aStringID + "]";
        }
        if (ENV.op === ENV.listOP.MIDDLE_EAST) {
            var LangObj = hx.svc.SETTING_UTIL.getLanguage();
            if (LangObj) {
                var menuLang = LangObj.LangMenu;
                if ((menuLang == "ara") || (menuLang == "per")) {
                    // Find Arabic character in string
                    var IsArabicCharacters = function (str) {
                        var charCode = 0, bArabic = false, j = 0;
                        for (j = 0; j < str.length; j++) {
                            charCode = str.charCodeAt(j);
                            if (charCode > 0x600 && charCode < 0x6FF) {
                                bArabic = true;
                                break;
                            }
                        }
                        return bArabic;
                    };
                    var isArabicChar = IsArabicCharacters(ret);
                    var text = '';
                    if (isArabicChar === true) {
                        text = "\u202B" + ret + "\u202C"; // "\u202B" - right-to-left embedding, "\u202C" - pop directional formatting
                    }
                    else {
                        text = ret;
                    }
                    return text;
                }
            }
        }
        return ret;
    };
    var load = function (lang, cb) {
        $.hxGetScript("res/" + lang + ".json", function () {
            _lang = hx['_' + lang];
            if (cb) {
                cb();
            }
        }, 'il');
    };
    var reload = function (lang, cb) {
        this.load(lang, function () {
            $("[data-langid]").each(function (n) {
                var $n = $(this);
                var emTemp = '';
                if ($n.find('em').length > 0) {
                    emTemp = $n.find('em').attr('data-langID');
                }
                $n.text(l($n.attr('data-langID')));
                if (emTemp !== '') {
                    $n.append($('<em>').attr('data-langID', emTemp).html(l(emTemp)));
                }
                dateFormat.set(undefined, [l('LOC_SUN_ID'), l('LOC_MON_ID'), l('LOC_TUE_ID'), l('LOC_WED_ID'), l('LOC_THU_ID'), l('LOC_FRI_ID'), l('LOC_SAT_ID'), l('LOC_SUNDAY_ID'), l('LOC_MONDAY_ID'), l('LOC_TUESDAY_ID'), l('LOC_WEDNESDAY_ID'), l('LOC_THURSDAY_ID'), l('LOC_FRIDAY_ID'), l('LOC_SATURDAY_ID')], [l('LOC_JAN_ID'), l('LOC_FEB_ID'), l('LOC_MAR_ID'), l('LOC_APR_ID'), l('LOC_MAY_01_ID'), l('LOC_JUN_ID'), l('LOC_JUL_ID'), l('LOC_AUG_ID'), l('LOC_SEP_ID'), l('LOC_OCT_ID'), l('LOC_NOV_ID'), l('LOC_DEC_ID'), l('LOC_JANUARY_ID'), l('LOC_FEBRUARY_ID'), l('LOC_MARCH_ID'), l('LOC_APRIL_ID'), l('LOC_MAY_ID'), l('LOC_JUNE_ID'), l('LOC_JULY_ID'), l('LOC_AUGUST_ID'), l('LOC_SEPTEMBER_ID'), l('LOC_OCTOBER_ID'), l('LOC_NOVEMBER_ID'), l('LOC_DECEMBER_ID')]);
            });
            if (cb) {
                cb();
            }
        });
    };
    l.load = load;
    l.reload = reload;
    return l;
});

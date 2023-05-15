/// <reference path="../../def/sugar.d.ts" />
// file name : pages/install/page03_select_lang.js
/*jslint sloppy:true, nomen:true, vars:true */
/* DESIGN : design_result/02_WIZ/WIZ_2.html */
import __PAGE_KERNEL__ = require("pages/page_kernel");
declare var hx: any;

class cPgInstallLang extends __PAGE_KERNEL__{
    _bIsGoWizardPage: boolean;

    // TODO: Check variable type
    _szMenuLang: any;

    containerEl: HTMLElement;
    $article: any;
    lang: any;
    ul: any;
    strEl: HTMLElement;
    $hArea: any;
    $title: HTMLElement;
    stepEl: HTMLElement;
    bottomDimEl: HTMLElement;

    constructor() {
        super();
    }

    create(aEntity, aPageUrl) {
        var i: number = 0, arLangStr: any = [], stLangObj: any, ulFocusIndex: number;

        hx.logadd('CPgInstall03');

        // fling do not have to work when Install Wizard
        hx.svc.BRIDGE.enablePlayback(false);

        for ( i = 0; i < hx.al.CPgInstall03.items.itemStr.length; i+=1) {
            arLangStr[i] = hx.l(hx.al.CPgInstall03.items.itemStr[i]);
        }

        $.extend(this, {
            containerEl : document.querySelector('#wiz'),
            $article : $('<article class="cont">'),
            lang : arLangStr,
            ul : null,

            strEl : document.createElement('p'),
            $hArea : $('<h1>', {
                'class' : 'hArea'
            }),
            $title : $('<span>', {
                'class' : 'tit',
                'html' : ENV.listModel.FVP4000T === ENV.model? hx.l('LOC_PREFERRED_LANGUAGE_UPPER_ID') : hx.l('LOC_BIGLANGUAGE_ID'),
                'data-langID' : ENV.listModel.FVP4000T === ENV.model? 'LOC_PREFERRED_LANGUAGE_UPPER_ID' : 'LOC_BIGLANGUAGE_ID'
            }),
            stepEl : document.createElement('span'),
            bottomDimEl : document.createElement('div')
        });
        this.strEl.innerHTML = ENV.listModel.FVP4000T === ENV.model? "Select your language and press OK to continue." : hx.l('LOC_SELECT_LANGUAGE_ID');
        this.strEl.setAttribute('data-langID', ENV.listModel.FVP4000T === ENV.model? "Select your language and press OK to continue." : 'LOC_SELECT_LANGUAGE_ID');
        this.stepEl.classList.add(hx.al.CPgInstall03.items.step);
        this.bottomDimEl.classList.add('cov');
        this.bottomDimEl.style.height = '220px';

        if (aPageUrl.query === 'prev') {
            this.$article.css('left', '-50px');
        } else {
            this.$article.css('left', '50px');
        }

        /////////////////append To DOM
        if (!this.containerEl) {
            this.containerEl = document.createElement('section');
            this.containerEl.id = 'wiz';
            document.querySelector('#wrap').appendChild(this.containerEl);
        }
        this.ul = this._set_List();

        this.$hArea[0].appendChild(this.$title[0]);
        this.$hArea[0].appendChild(this.stepEl);
        this.$article[0].appendChild(this.strEl);
        this.$article[0].appendChild(this.ul.$[0]);
        this.containerEl.style.display = 'none';
        this.containerEl.appendChild(this.$hArea[0]);
        this.containerEl.appendChild(this.$article[0]);
        this.containerEl.appendChild(this.bottomDimEl);
        this.containerEl.style.display = 'block';

        /////////////Do Initial Script
        hx.il.ani.action({
            targets : this.$hArea,
            cls : 'aniFadeIn'
        });
        hx.il.ani.action({
            targets : [this.$article],
            cls : 'aniFadeIn',
            style : 'left:0'
        });

        hx.al.compose(this).append(this.ul);
        this.ul.on();

        this._bIsGoWizardPage = false;

        stLangObj = hx.svc.SETTING_UTIL.getLanguage();
        this._szMenuLang = stLangObj.LangMenu;
        // set main language from settng
        this._szMenuLang = hx.config.lang;

        ulFocusIndex = this._convIso639ToIdx(this._szMenuLang);
        this.ul.setItemFocus(ulFocusIndex);
    }

    destroy() {
        if (this.containerEl) {
            if (!this._bIsGoWizardPage) {
                this.containerEl.parentElement.removeChild(this.containerEl);
            } else {
                this.containerEl.style.display = 'none';
                this.containerEl.removeChild(this.$hArea[0]);
                this.containerEl.removeChild(this.$article[0]);
                this.containerEl.removeChild(this.bottomDimEl);
                this.containerEl.style.display = 'block';
            }
            this.containerEl = undefined;
        }
        this.$article = undefined;
        this.lang = undefined;
        this.ul = undefined;
        this.strEl = undefined;
        this.$hArea = undefined;
        this.$title = undefined;
        this.stepEl = undefined;
        this.bottomDimEl = undefined;

        // fling do not have to work when Install Wizard
        hx.svc.BRIDGE.enablePlayback(true);
    }
    _on_alClicked(aParam) {
        var data: any = aParam.alData, target: any = data.me;
        var bConsumed: boolean = false, lIdx: number = data.me.getItemFocus();
        //alert(aParam.alData.result);
        //save language set~!!!!
        if (target === this.ul) {
            bConsumed = true;
            this._szMenuLang = this._convIdxToIso639(lIdx);
            if (ENV.listModel.FVP4000T === ENV.model) {
                hx.svc.SETTING_UTIL.setLanguage({
                        LangMenu : this._szMenuLang,
                        LangAudio : this._szMenuLang,
                        LangSubTtl : this._szMenuLang
                });
            } else {
                hx.l.reload(this._szMenuLang);
                hx.svc.SETTING_UTIL.setLanguage({
                        LangMenu : this._szMenuLang
                });
            }
            this._bIsGoWizardPage = true;
            hx.al.decompose(this);
            this._fadeOutAni(hx.al.CPgInstall03.items.nextPage, false);
        }
        return bConsumed;
    }
    _on_alScrolldown(aParam) {
        var data = aParam.alData, lIdx = data.me.getItemFocus();

        this._szMenuLang = this._convIdxToIso639(lIdx);
        if (ENV.listModel.FVP4000T !== ENV.model) {
            hx.l.reload(this._szMenuLang);
        }
        return true;
    }
    _on_alScrollup (aParam) {
        var data = aParam.alData, lIdx = data.me.getItemFocus();

        this._szMenuLang = this._convIdxToIso639(lIdx);
        if (ENV.listModel.FVP4000T !== ENV.model) {
            hx.l.reload(this._szMenuLang);
        }
        return true;
    }
    _on_alKey(aParam) {
        var bConsumed:boolean = false;

        switch( aParam.alKey ) {
            case hx.key.KEY_MENU:
            case hx.key.KEY_GUIDE:
            case hx.key.KEY_ON_DEMAND:
                bConsumed = true;
                break;
        }
        return bConsumed;
    }
    _set_List() {
        return hx.al.component('wizList', {
            ul_attr : {
                'class' : 'lang',
                'style' : 'margin-top:50px;'
            },
            arr : this.lang,
            viewCount : 5
        });
    }
    _fadeOutAni (aTargetPage, aIsGoPrev) {
        var left: string;

        if (aIsGoPrev) {
            left = '20px';
        } else {
            left = '-20px';
        }

        hx.il.ani.action({
            targets : this.$article,
            cls : 'aniFadeOut',
            style : 'left:' + left
        });

        hx.il.ani.action({
            targets : this.$title,
            cls : 'aniFadeOut',
            cb : () => {
                this.sendEventToEntity({
                    alEvent : 'alChangeInstallPage',
                    'target' : aTargetPage,
                    me : this
                });
            }
        });
    }
    _convIso639ToIdx(aszIso639) {
        var i: number = 0, ulIndex: number = 0;

        for ( i = 0; i < hx.al.CPgInstall03.items.itemStr.length; i+=1) {
            if (aszIso639 === hx.al.CPgInstall03.items.langIsoCode[i]) {
                ulIndex = i;
                break;
            }
        }

        return ulIndex;
    }
    _convIdxToIso639(alIdx) {
        var szLangeIsoCode: string = 'eng';

        if (alIdx >= hx.al.CPgInstall03.items.itemStr.length) {
            return szLangeIsoCode;
        }

        szLangeIsoCode = hx.al.CPgInstall03.items.langIsoCode[alIdx];

        return szLangeIsoCode;
    }
}

hx.al.CPgInstall03 = cPgInstallLang;

/*
hx.al.CPgInstall03 = function() {
};

hx.al.CPgInstall03.prototype = {
    create : function(entity, pageUrl) {
        var that = this, i, langStr = [],
        LangObj,
        focusIndex;
        hx.logadd('CPgInstall03');

        // fling do not have to work when Install Wizard
        hx.svc.BRIDGE.enablePlayback({
            enabled : false
        });

        for ( i = 0; i < hx.al.CPgInstall03.items.itemStr.length; i+=1) {
            langStr[i] = hx.l(hx.al.CPgInstall03.items.itemStr[i]);
        }

        $.extend(that, {
            containerEl : document.querySelector('#wiz'),
            $article : $('<article class="cont">'),
            lang : langStr,
            ul : null,

            strEl : document.createElement('p'),
            $hArea : $('<h1>', {
                'class' : 'hArea'
            }),
            $title : $('<span>', {
                'class' : 'tit',
                'html' : ENV.listModel.FVP4000T === ENV.model? hx.l('LOC_BIG_PREFERRED_LANGUAGE_ID') : hx.l('LOC_BIGLANGUAGE_ID'),
                'data-langID' : ENV.listModel.FVP4000T === ENV.model? 'LOC_BIG_PREFERRED_LANGUAGE_ID' : 'LOC_BIGLANGUAGE_ID'
            }),
            stepEl : document.createElement('span'),
            bottomDimEl : document.createElement('div')
        });
        that.strEl.innerHTML = ENV.listModel.FVP4000T === ENV.model? hx.l('LOC_PRERERRED_LANGUAGE_WZD_STR_ID') : hx.l('LOC_SELECT_LANGUAGE_ID');
        that.strEl.setAttribute('data-langID', ENV.listModel.FVP4000T === ENV.model? 'LOC_PRERERRED_LANGUAGE_WZD_STR_ID' : 'LOC_SELECT_LANGUAGE_ID');
        that.stepEl.classList.add('step1');
        that.bottomDimEl.classList.add('cov');
        that.bottomDimEl.style.height = '220px';

        if (pageUrl.query === 'prev') {
            that.$article.css('left', '-50px');
        } else {
            that.$article.css('left', '50px');
        }

        /////////////////append To DOM
        if (!that.containerEl) {
            that.containerEl = document.createElement('section');
            that.containerEl.id = 'wiz';
            document.querySelector('#wrap').appendChild(that.containerEl);
        }
        that.ul = that.fnSetList();

        that.$hArea[0].appendChild(that.$title[0]);
        that.$hArea[0].appendChild(that.stepEl);
        that.$article[0].appendChild(that.strEl);
        that.$article[0].appendChild(that.ul.$[0]);
        that.containerEl.style.display = 'none';
        that.containerEl.appendChild(that.$hArea[0]);
        that.containerEl.appendChild(that.$article[0]);
        that.containerEl.appendChild(that.bottomDimEl);
        that.containerEl.style.display = 'block';

        /////////////Do Initial Script
        hx.il.ani.action({
            targets : that.$hArea,
            cls : 'aniFadeIn'
        });
        hx.il.ani.action({
            targets : [that.$article],
            cls : 'aniFadeIn',
            style : 'left:0'
        });

        hx.al.compose(that).append(that.ul);
        that.ul.on();

        that.isGoWizardPage = false;

        LangObj = hx.svc.SETTING_UTIL.getLanguage();
        that.menuLang = LangObj.LangMenu;
        // set main language from settng
        that.menuLang = hx.config.lang;

        focusIndex = that.fnConvIso639ToIdx(that.menuLang);
        that.ul.setItemFocus(focusIndex);
    },
    destroy : function() {
        var that = this;
        if (that.containerEl) {
            if (!that.isGoWizardPage) {
                that.containerEl.parentElement.removeChild(that.containerEl);
            } else {
                that.containerEl.style.display = 'none';
                that.containerEl.removeChild(that.$hArea[0]);
                that.containerEl.removeChild(that.$article[0]);
                that.containerEl.removeChild(that.bottomDimEl);
                that.containerEl.style.display = 'block';
            }
            that.containerEl = undefined;
        }
        that.$article = undefined;
        that.lang = undefined;
        that.ul = undefined;
        that.strEl = undefined;
        that.$hArea = undefined;
        that.$title = undefined;
        that.stepEl = undefined;
        that.bottomDimEl = undefined;

        // fling do not have to work when Install Wizard
        hx.svc.BRIDGE.enablePlayback({
            enabled : true
        });
    },
    fnSetList : function() {
        var that = this;
        return hx.al.component('wizList', {
            ul_attr : {
                'class' : 'lang',
                'style' : 'margin-top:50px;'
            },
            arr : that.lang,
            viewCount : 5
        });
    },
    _on_alClicked : function(param) {
        var that = this, data = param.alData, target = data.me, bConsumed = false, idx = data.me.getItemFocus();
        //alert(param.alData.result);
        //save language set~!!!!
        if(idx === -1) { // end of item
            idx = hx.al.CPgInstall03.items.itemStr.length -1;
        }
        if (target === that.ul) {
            bConsumed = true;
            that.menuLang = that.fnConvIdxToIso639(idx);
            if (ENV.listModel.FVP4000T === ENV.model) {
                hx.svc.SETTING_UTIL.setLanguage({
                        LangMenu : 'eng',
                        LangAudio : that.menuLang,
                        LangSubTtl : that.menuLang
                });
            } else {
                hx.l.reload(that.menuLang);
                hx.svc.SETTING_UTIL.setLanguage({
                        LangMenu : that.menuLang
                    }
                });
            }
            that.isGoWizardPage = true;
            hx.al.decompose(that);
            that._fadeOutAni(hx.al.CPgInstall03.items.nextPage, false);
        }
        return bConsumed;
    },
    _on_alScrolldown : function(param) {
        var that = this, data = param.alData, idx = data.me.getItemFocus();
        if(idx === -1) { // end of item
            idx = hx.al.CPgInstall03.items.itemStr.length -1;
        }        
        that.menuLang = that.fnConvIdxToIso639(idx);
        if (ENV.listModel.FVP4000T !== ENV.model) {
            hx.l.reload(that.menuLang);
        }
        return true;
    },
    _on_alScrollup : function(param) {
        var that = this, data = param.alData, idx = data.me.getItemFocus();
        if(idx === -1) { // end of item
            idx = hx.al.CPgInstall03.items.itemStr.length -1;
        }  
        that.menuLang = that.fnConvIdxToIso639(idx);
        if (ENV.listModel.FVP4000T !== ENV.model) {
            hx.l.reload(that.menuLang);
        }
        return true;
    },
    _on_alKey : function(param) {
        var that = this, bConsumed = false;
        switch( param.alKey ) {
            case hx.key.KEY_MENU:
            case hx.key.KEY_GUIDE:
                bConsumed = true;
                break;
        }
        return bConsumed;
    },
    fnFadeOutAni : function(targetPage, isGoPrev) {
        var that = this, left;

        if (isGoPrev) {
            left = '20px';
        } else {
            left = '-20px';
        }

        hx.il.ani.action({
            targets : that.$article,
            cls : 'aniFadeOut',
            style : 'left:' + left
        });

        hx.il.ani.action({
            targets : that.$title,
            cls : 'aniFadeOut',
            cb : function() {
                that.sendEventToEntity({
                    alEvent : 'alChangeInstallPage',
                    'target' : targetPage,
                    me : that
                });
            }
        });
    },
    fnConvIso639ToIdx : function(iso639) {
        var that = this, i = 0, ulIndex = 0;

        for ( i = 0; i < hx.al.CPgInstall03.items.itemStr.length; i+=1) {
            if (iso639 === hx.al.CPgInstall03.items.langIsoCode[i]) {
                ulIndex = i;
                break;
            }
        }

        return ulIndex;
    },
    fnConvIdxToIso639 : function(idx) {
        var that = this, szLangeIsoCode = 'eng';

        if (idx >= hx.al.CPgInstall03.items.itemStr.length) {
            return szLangeIsoCode;
        }

        szLangeIsoCode = hx.al.CPgInstall03.items.langIsoCode[idx];

        return szLangeIsoCode;
    }
};
*/

/********************      Static Variables     ********************/
if (hx.config.wzd.page03) {
    hx.al.CPgInstall03.items = hx.config.wzd.page03;
} else {
    hx.al.CPgInstall03.items = {
        itemStr : ['LOC_SET_ENGLISH_ID', 'LOC_SET_GERMAN_ID', 'LOC_SET_TURKISH_ID', 'LOC_SET_RUSSIAN_ID'],
        langIsoCode : ['eng', 'ger', 'tur', 'rus'],
        nextPage : 'CPgInstall04',
        prevPage : '',
        step : 'step1'
    };
}
export = cPgInstallLang;
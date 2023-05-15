/// <reference path="../../def/sugar.d.ts" />
// file name : product/hms1000t/pages/install/page03_select_regionT.js
/*jslint sloppy:true, nomen:true, vars:true */
/* DESIGN : design_result/02_WIZ/WIZ_2.html */
import __PAGE_KERNEL__ = require("pages/page_kernel");
declare var hx: any;

class cPgInstallSelRegion extends __PAGE_KERNEL__{
    _bIsGoWizardPage: boolean;
    _ulMenuRegion: number;

    _containerEl: HTMLElement;
    _$article: any;
    _region: any;
    _ul: any;
    _strEl: HTMLElement;
    _$hArea: any;
    _$title: HTMLElement;
    _stepEl: HTMLElement;
    _bottomDimEl: HTMLElement;


    constructor() {
        super();
    }

    create (aEntity, aPageUrl) {
        var i: number = 0, regionStr: any = [];
        var ulRegionId: number = 0, ulFocusIndex: number = 0, regionObj: any;

         hx.logadd('CPgInstall03');

        // fling do not have to work when Install Wizard
        hx.svc.BRIDGE.enablePlayback(false);

        for ( i = 0; i < hx.al.CPgInstall03.items.itemStr.length; i+=1) {
            regionStr[i] = hx.l(hx.al.CPgInstall03.items.itemStr[i]);
        }

        $.extend(this, {
            _containerEl : document.querySelector('#wiz'),
            _$article : $('<article class="cont">'),
            _region : regionStr,
            _ul : null,

            _strEl : document.createElement('p'),
            _$hArea : $('<h1>', {
                'class' : 'hArea'
            }),
            _$title : $('<span>', {
                'class' : 'tit',
                'html' : hx.l('LOC_REGION_ID').toUpperCase(),
                'data-langID' : 'LOC_REGION_ID'
            }),
            _stepEl : document.createElement('span'),
            _bottomDimEl : document.createElement('div')
        });
        this._strEl.innerHTML = hx.l('LOC_SELECT_REGION_ID');
        this._strEl.setAttribute('data-langID', 'LOC_SELECT_REGION_ID');
        this._stepEl.classList.add('step1');
        this._bottomDimEl.classList.add('cov');
        this._bottomDimEl.style.height = '220px';

        if (aPageUrl.query === 'prev') {
            this._$article.css('left', '-50px');
        } else {
            this._$article.css('left', '50px');
        }

        /////////////////append To DOM
        if (!this._containerEl) {
            this._containerEl = document.createElement('section');
            this._containerEl.id = 'wiz';
            document.querySelector('#wrap').appendChild(this._containerEl);
        }
        this._ul = this._set_list();

        this._$hArea[0].appendChild(this._$title[0]);
        this._$hArea[0].appendChild(this._stepEl);
        this._$article[0].appendChild(this._strEl);
        this._$article[0].appendChild(this._ul.$[0]);
        this._containerEl.style.display = 'none';
        this._containerEl.appendChild(this._$hArea[0]);
        this._containerEl.appendChild(this._$article[0]);
        this._containerEl.appendChild(this._bottomDimEl);
        this._containerEl.style.display = 'block';

        /////////////Do Initial Script
        hx.il.ani.action({
            targets : this._$hArea,
            cls : 'aniFadeIn'
        });
        hx.il.ani.action({
            targets : [this._$article],
            cls : 'aniFadeIn',
            style : 'left:0'
        });
        hx.al.compose(this).append(this._ul);

        this._ul.on();
        this._bIsGoWizardPage = false;

        regionObj = hx.svc.SETTING_UTIL.getRegionId();

        for ( i = 0; i < hx.config.wzd.page03.regionId.length; i+=1) {
            if (regionObj === hx.config.wzd.page03.regionId[i]) {
                ulRegionId = i;
            }
        }

        this._ulMenuRegion = ulRegionId;
        ulFocusIndex = this._ulMenuRegion;
        this._ul.setItemFocus(ulFocusIndex);
    }
    destroy() {
        if (this._containerEl) {
            if (!this._bIsGoWizardPage) {
                this._containerEl.parentElement.removeChild(this._containerEl);
            } else {
                this._containerEl.style.display = 'none';
                this._containerEl.removeChild(this._$hArea[0]);
                this._containerEl.removeChild(this._$article[0]);
                this._containerEl.removeChild(this._bottomDimEl);
                this._containerEl.style.display = 'block';
            }
            this._containerEl = undefined;
        }
        this._$article = undefined;
        this._region = undefined;
        this._ul = undefined;
        this._strEl = undefined;
        this._$hArea = undefined;
        this._$title = undefined;
        this._stepEl = undefined;
        this._bottomDimEl = undefined;

        // fling do not have to work when Install Wizard
        hx.svc.BRIDGE.enablePlayback(true);
    }
    _on_alClicked (aParam) {
        var data: any = aParam.alData, target: any = data.me, bConsumed: boolean = false, ulRegionId: number = 0;

        //save region set~!!!!
        if (target === this._ul) {
            bConsumed = true;
            this._ulMenuRegion = this._conv_region_to_idx(data.result);
            ulRegionId = hx.al.CPgInstall03.items.regionId[this._ulMenuRegion];
            hx.svc.SETTING_UTIL.setRegionId(ulRegionId);
            this._bIsGoWizardPage = true;
            hx.al.decompose(this);
            this._fadeOutAni(hx.al.CPgInstall03.items.nextPage, false);
        }

        return bConsumed;
    }
    _on_alScrolldown(aParam) {
        var data: any = aParam.alData;

        this._ulMenuRegion = data.index;

        return true;
    }
    _on_alScrollup(aParam) {
        var data: any = aParam.alData;

        this._ulMenuRegion = data.index;

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
    _set_list() {
        return hx.al.component('wizList', {
            ul_attr : {
                'class' : 'lang',
                'style' : 'margin-top:50px;'
            },
            arr : this._region,
            viewCount : 5
        });
    }
    _fadeOutAni(aTargetPage, aIsGoPrev) {
        var left: string;

        if (aIsGoPrev) {
            left = '20px';
        } else {
            left = '-20px';
        }

        hx.il.ani.action({
            targets : this._$article,
            cls : 'aniFadeOut',
            style : 'left:' + left
        });

        hx.il.ani.action({
            targets : this._$title,
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
    _conv_region_to_idx(aParamRegion) {
        var i: number = 0, ulIndex: number = 0;

        for ( i = 0; i < hx.al.CPgInstall03.items.itemStr.length; i+=1) {
            if (aParamRegion === hx.l(hx.al.CPgInstall03.items.itemStr[i])) {
                ulIndex = i;
                break;
            }
        }

        return ulIndex;
    }
    _conv_idx_to_region(aulIdx) {
        var ulRegionPostalCode: number = 0;

        if (aulIdx >= hx.al.CPgInstall03.items.itemStr.length) {
            return ulRegionPostalCode;
        }

        ulRegionPostalCode = hx.al.CPgInstall03.items.postalCode[aulIdx];

        return ulRegionPostalCode;
    }
}

hx.al.CPgInstall03 = cPgInstallSelRegion;

/*
hx.al.CPgInstall03 = function() {
};

hx.al.CPgInstall03.prototype = {
    create : function(entity, pageUrl) {
        hx.logadd('CPgInstall03');
        var that = this, i, _article;
        var regionStr = [];

        // fling do not have to work when Install Wizard
        hx.svc.BRIDGE.enablePlayback({
            enabled : false
        });

        for ( i = 0; i < hx.al.CPgInstall03.items.itemStr.length; i+=1) {
            regionStr[i] = hx.l(hx.al.CPgInstall03.items.itemStr[i]);
        }

        $.extend(that, {
            _containerEl : document.querySelector('#wiz'),
            _$article : $('<article class="cont">'),
            _region : regionStr,
            _ul : null,

            _strEl : document.createElement('p'),
            _$hArea : $('<h1>', {
                'class' : 'hArea'
            }),
            _$title : $('<span>', {
                'class' : 'tit',
                'html' : hx.l('LOC_REGION_ID').toUpperCase(),
                'data-langID' : 'LOC_REGION_ID'
            }),
            _stepEl : document.createElement('span'),
            _bottomDimEl : document.createElement('div')
        });
        that._strEl.innerHTML = hx.l('LOC_SELECT_REGION_ID');
        that._strEl.setAttribute('data-langID', 'LOC_SELECT_REGION_ID');
        that._stepEl.classList.add('step1');
        that._bottomDimEl.classList.add('cov');
		that._bottomDimEl.style.height = '220px';

        if (pageUrl.query === 'prev') {
            that._$article.css('left', '-50px');
        } else {
            that._$article.css('left', '50px');
        }

        /////////////////append To DOM
        if (!that._containerEl) {
            that._containerEl = document.createElement('section');
            that._containerEl.id = 'wiz';
            document.querySelector('#wrap').appendChild(that._containerEl);
        }
        that._ul = that._set_list();

        that._$hArea[0].appendChild(that._$title[0]);
        that._$hArea[0].appendChild(that._stepEl);
        that._$article[0].appendChild(that._strEl);
        that._$article[0].appendChild(that._ul.$[0]);
        that._containerEl.style.display = 'none';
        that._containerEl.appendChild(that._$hArea[0]);
        that._containerEl.appendChild(that._$article[0]);
        that._containerEl.appendChild(that._bottomDimEl);
        that._containerEl.style.display = 'block';

        /////////////Do Initial Script
        hx.il.ani.action({
            targets : that._$hArea,
            cls : 'aniFadeIn'
        });
        hx.il.ani.action({
            targets : [that._$article],
            cls : 'aniFadeIn',
            style : 'left:0'
        });

        hx.al.compose(that).append(that._ul);
        that._ul.on();

        that._isGoWizardPage = false;

        var regionId;
        var regionObj = hx.svc.SETTING_UTIL.getRegionId();

        for ( i = 0; i < hx.config.wzd.page03.regionId.length; i+=1) {
            if (regionObj === hx.config.wzd.page03.regionId[i]) {
                regionId = i;
            }
        }

        that._menuRegion = regionId;
        var focusIndex;

        focusIndex = that._menuRegion;
        that._ul.setItemFocus(focusIndex);
    },
    destroy : function() {
        var that = this;
        if (that._containerEl) {
            if (!that._isGoWizardPage) {
                that._containerEl.parentElement.removeChild(that._containerEl);
            } else {
                that._containerEl.style.display = 'none';
                that._containerEl.removeChild(that._$hArea[0]);
                that._containerEl.removeChild(that._$article[0]);
                that._containerEl.removeChild(that._bottomDimEl);
                that._containerEl.style.display = 'block';
            }
            that._containerEl = undefined;
        }
        that._$article = undefined;
        that._region = undefined;
        that._ul = undefined;
        that._strEl = undefined;
        that._$hArea = undefined;
        that._$title = undefined;
        that._stepEl = undefined;
        that._bottomDimEl = undefined;

        // fling do not have to work when Install Wizard
        hx.svc.BRIDGE.enablePlayback({
            enabled : true
        });
    },
    _set_list : function() {
        var that = this;
        return hx.al.component('wizList', {
            ul_attr : {
                'class' : 'lang',
                'style' : 'margin-top:50px;'
            },
            arr : that._region,
            viewCount : 5
        });
    },
    _on_alClicked : function(param) {
        var that = this, data = param.alData, target = data.me, bConsumed = false, regionId;

        //alert(param.alData.result);
        //save region set~!!!!
        if (target === that._ul) {
            bConsumed = true;
            that._menuRegion = that._conv_region_to_idx(data.result);
            regionId = hx.al.CPgInstall03.items.regionId[that._menuRegion];
            hx.svc.SETTING_UTIL.setRegionId({regionId
            });
            that._isGoWizardPage = true;
            hx.al.decompose(that);
            that._fadeOutAni(hx.al.CPgInstall03.items.nextPage, false);
        }

        return bConsumed;
    },
    _on_alScrolldown : function(param) {
        var that = this, data = param.alData, target = data.me;

        that._menuRegion = data.index;

        return true;
    },
    _on_alScrollup : function(param) {
        var that = this, data = param.alData, target = data.me;

        that._menuRegion = data.index;

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
    _fadeOutAni : function(targetPage, isGoPrev) {
        var that = this, left;

        if (isGoPrev) {
            left = '20px';
        } else {
            left = '-20px';
        }

        hx.il.ani.action({
            targets : that._$article,
            cls : 'aniFadeOut',
            style : 'left:' + left
        });

        hx.il.ani.action({
            targets : that._$title,
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
    _conv_region_to_idx : function(paramRegion) {
        var that = this, i = 0, ulIndex = 0;

        for ( i = 0; i < hx.al.CPgInstall03.items.itemStr.length; i+=1) {
            if (paramRegion === hx.l(hx.al.CPgInstall03.items.itemStr[i])) {
                ulIndex = i;
                break;
            }
        }

        return ulIndex;
    },
    _conv_idx_to_region : function(idx) {
        var that = this, regionPostalCode = 0;

        if (idx >= hx.al.CPgInstall03.items.itemStr.length) {
            return regionPostalCode;
        }

        regionPostalCode = hx.al.CPgInstall03.items.postalCode[idx];

        return regionPostalCode;
    }
};
*/
/********************      Static Variables     ********************/
if (hx.config.wzd.page03) {
    hx.al.CPgInstall03.items = hx.config.wzd.page03;
}
export = cPgInstallSelRegion;
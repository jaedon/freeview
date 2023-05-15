///<reference path="../../../def/sugar.d.ts" />
///<reference path="../../../def/prism.d.ts" />
import __KERNEL_SETTINGS__ = require("pages/settings/kernel_settings");
import __CONTROL_TRD_CONFLICT_DLG__ = require("pages/components_based_controls/trd_conflict_dialog");
declare var hx;

class cCPgAutomaticSearch extends __KERNEL_SETTINGS__{
    $colum : any;
    $tv : any;
    $radio : any;

    _progressVariable : number = 0;
    _isSearching : string = 'true';
    _getChupdateEvt = false;
    _tvListindex : any = [];
    _radioListindex : any = [];

    _channelCount : number = 0;
    _tempTvCount : number = 0;
    _tempTvList : any = [];
    _tvCount : number = 0;
    _tempRadioCount : number = 0;
    _radioCount : number = 0;
    _tempRadioList : any = [];
    _isChangePageInSetting = false;
    _button : any;
    _progress : any;
    _tvlist : any;
    _radiolist : any;
    _freq : number;
    _status : any;
    _isNoChannel : boolean;
    _tvcount : any;
    _radiocount : any;
    _allResult : any;
    _$channel : any;
    _resultSet : any;
    _savingProgress : any;

    constructor() {
        super();
    }

    create (aEntity, aPageUrl) {
        super.create();
        this.$colum = $('<article>', { 'class': 'alignC colum2' });
        this.$tv = $('<article>', { 'class': 'tv' });
        this._tvcount = this.string(hx.l('LOC_TV_ID') + ' : 0');
        this._tvlist = this.Search_result(this.tvList);
        this.$radio = $('<article>', { 'class': 'radio' });
        this._radiocount = this.string(hx.l('LOC_RADIO_ID') + ' : 0');
        this._radiolist = this.Search_result(this.radioList);
        this._progress = this.progress3();
        this._allResult = this.allResult();
        this._button = this.button('LOC_STOP_ID');

        var $title : any = $('#set .title');
        if ($title[0]) {
            $title[0].children[0].innerHTML = hx.l('LOC_AUTOMATIC_CHANNEL_SEARCH_ID').toUpperCase();
        }
        $('#set')
            .append(this.$colum
                .append(this.$tv
                    .append(this._tvcount.$
                        .after(this._tvlist.$))
                    .after(this.$radio
                        .append(this._radiocount.$
                            .after(this._radiolist.$))
                        .after(this._progress.$
                            .after(this._allResult.$
                                .after(this._button.$)
                        )
                    )
                )
            )
        );

        hx.al.compose(this).append(this._button);

        /* initialize because page change can be a problem*/
        this._isChangePageInSetting = false;
        this._progressVariable = 0;
        this._tvlist.count = 0;
        this._radiolist.count = 0;
        this._getChupdateEvt = false;

        /* request service */
        hx.svc.CH_LIST.addEventCb('ChannelListUpdate', this, () => {
            this._on_ChannelListUpdate();
        });
        hx.svc.CHANNEL_SEARCH_T.addEventCb('channelScan', this, (aParam) => {
            this._on_channelScan(aParam);
        });
        $title.ani({
            style : this.aniConfig.fadeIn,
            trans : {
                dur : 100,
                delay : 300
            }
        });
        this.$colum.ani({
            style : this.aniConfig.menu.changeIn,
            trans : {
                dur : 200,
                delay : 400
            },
            cb :() => {
                hx.log("info", "button on!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
                this._button.on();
                this._button.$.addClass('on');
                this._isChangePageInSetting = false;
            },
            noEvent: true
        });

        /* Initialize */
        this._progress.$.find('dt').html('');

        if (ENV.op === ENV.listOP.AUSDTT || ENV.op === ENV.listOP.UKDTT) {
            hx.il.criticalTasking.crChannelSearchTasking = true;

            // DMR uri set null ('bring in client' do not have to work)
            hx.svc.DMR.setMedia(null);

            hx.svc.AV_CTRL.stopCurChannel({
                    stopAV : 1,
                    stopTsr : 1
            });

            hx.svc.WEB_STORAGE.defaultZapInfo();

            if (aPageUrl && aPageUrl.param === 'smartRetune') {
                // this is Smart Retune.
                // do not remove all channels to keep scheduled user data.
            } else {
                hx.svc.CH_LIST.removeWholeChannelList();
            }

            hx.svc.CHANNEL_SEARCH_T.StartTerChannelScan({
                isAuto : true
            });
        } else {
            this._progressVariable = 100;
            this._progress.$.find('dd').eq(0).html(100 + '%');
            this._progress.$.find('dd').eq(1).find('span').css('width', 100 + '%');
            this._progress.$.find('dt').html(hx.l('LOC_NONE_ID'));
            this._button.$.html(hx.l('LOC_EXIT_ID'));
        }

        var retObj : any = hx.svc.CH_LIST.getChlist({
                group : 'ALL'
        });

        var _chList : any = retObj.chList;
        if (_chList) {
            if (_chList.length === 0) {//if _isNoChannel is true and satelliteName is ASTRA_1 , we should load preferredChList
                this._isNoChannel = true;
            } else {
                this._isNoChannel = false;
            }
        } else {
            this._isNoChannel = true;
        }
    }
    destroy () {
        super.destroy();
        hx.svc.CH_LIST.clearEventCb(this);
        hx.svc.CHANNEL_SEARCH_T.clearEventCb(this);
        this._isSearching = 'true';
        this.$colum.remove();
        this._channelCount = 0;
        this._tempTvCount = 0;
        this._tvCount = 0;
        this._tempRadioCount = 0;
        this._radioCount = 0;
        if (!this._isChangePageInSetting) {
            $('#set').remove();
            if ($('#opt').length > 0) {
                $('#opt').remove();
            }
        } else {
            this._isChangePageInSetting = false;
        }
        this.$colum = undefined;
        this.$tv = undefined;
        this._tvcount = undefined;
        this._tvlist = undefined;
        this.$radio = undefined;
        this._radiocount = undefined;
        this._radiolist = undefined;
        this._progress = undefined;
        this._allResult = undefined;
        this._button = undefined;
        this._tvListindex = [];
        this._radioListindex = [];
        this._tempTvList = [];
        this._tempRadioList = [];
    }
    /*********** Direct SVC Event Start ****************/

    /*
     *  param is structured
     *     retObj.group = group;
     *     retObj.chList = chList;
     */
    _on_ChListEvt (aRetObj) {
        var $chList : any = this._$channel;
        var o : any, tvLength : number, radioLength : number;
        var TotalCnt : number;

        var noSearchResultString : string = '<strong>' + hx.l('LOC_CHANNELSEARCH_NOT_COMPLETED_ID') + '</strong>' + hx.l('LOC_SELECTNEXT_SELECTPREVIOUS_ID');
        if ((aRetObj.chList === undefined) || (aRetObj.chList.length === 0)) {
            this._status = 'result_2';
            $chList.html('0' + ' (' + hx.l('LOC_TV_ID') + ': ' + '0' + ', ' + hx.l('LOC_RADIO_ID') + ': ' + '0'+ ')');
            this._resultSet.html(noSearchResultString);
        } else {
            o = hx.svc.CH_LIST.getChannelCount({
                    group : 'TV'
            });
            tvLength = o.chCnt;
            o = hx.svc.CH_LIST.getChannelCount({
                    group : 'RADIO'
            });
            radioLength = o.chCnt;

            TotalCnt = tvLength + radioLength;

            $chList.html(TotalCnt + ' (' + hx.l('LOC_TV_ID') + ': ' + tvLength + ', ' + hx.l('LOC_RADIO_ID') + ': ' + radioLength + ')');
        }
    }
    /*********** Direct SVC Event End ****************/
    _on_channelScan (aParam) {
        var j : number, k : number, p : number, u : number, showString : string;
        var useTvLength : number;
        var useRadioLength : number;
        var searchedTvListLength : number = 0;
        var searchedRadioListLength : number = 0;
        var i : number, len : number, lchannelType : number;
        var showtvListindex : number = 0;
        var showtvList : any = [];
        var showradioListindex : number = 0;
        var showradioList : any = [];
        var tvListLength : number;
        var radioListLength : number;
        var transpec : number = 0;
        var showtvLength : number;
        var showradioLength : number;
        var tvindex : number = 0;
        var radioindex : number = 0;
        var tvname : string = '';
        var radioname : string = '';
        /* initialize */
        this._tvListindex = [];
        this._radioListindex = [];

        useTvLength = this._tvlist._$items.length;
        useRadioLength = this._radiolist._$items.length;
        this._tempTvList.length = 0;
        this._tempRadioList.length = 0;

        for ( j = 0; j < useTvLength; j += 1) {
            this._tempTvList.push(this._tvlist.getItem(j).text());
            this._tvlist.getItem(j).remove();
        }
        for ( k = 0; k < useTvLength; k += 1) {
            this._tvlist._$items.pop();
        }
        for ( p = 0; p < useRadioLength; p += 1) {
            this._tempRadioList.push(this._radiolist.getItem(p).text());
            this._radiolist.getItem(p).remove();
        }
        for ( u = 0; u < useRadioLength; u += 1) {
            this._radiolist._$items.pop();
        }

        /* save index about tvList, radioList from searchedInfos */
        this._radiolist.count = 0;

        len = aParam.owSearchedInfos.length;
        for ( i = 0; i < len; i += 1) {
            lchannelType = aParam.owSearchedInfos.getInfo(i, "channelType");
            if (lchannelType === 1) {
                searchedTvListLength += 1;
                this._tvListindex.push(i);
            } else if (lchannelType === 2) {
                searchedRadioListLength += 1;
                this._radioListindex.push(i);
            }
        }

        /* push show item from end to maximum 6*/
        tvListLength = this._tvListindex.length;
        radioListLength = this._radioListindex.length;
        for ( i = 0; i < tvListLength; i += 1) {
            showtvListindex = this._tvListindex.pop();
            showtvList.push(showtvListindex);
            if (i === 5) {
                break;
            }
        }
        for ( i = 0; i < radioListLength; i += 1) {
            showradioListindex = this._radioListindex.pop();
            showradioList.push(showradioListindex);
            if (i === 5) {
                break;
            }
        }

        /* Show searching TP */
        //that._progress.$.find('dt').html(that._allTPList[param.transponderCount - 1]);
        showString = '';
        this._freq = aParam.tpInfo.startFrequency;

        if (ENV.op === ENV.listOP.AUSDTT || ENV.op === ENV.listOP.UKDTT) {
            if (aParam.tpInfo.bandwidth === '7Mhz') {
                aParam.tpInfo.bandwidth = '7MHz';
            } else if (aParam.tpInfo.bandwidth === '8Mhz') {
                aParam.tpInfo.bandwidth = '8MHz';
            }
            if (aParam.tpInfo.bandwidth === '7MHz') {
                length = hx.config.settings.channelgroupT.bandwidth7MHz.centreFrequency.length;
                for ( i = 0; i < length; i+=1) {
                    if (this._freq === hx.config.settings.channelgroupT.bandwidth7MHz.centreFrequency[i]) {
                        break;
                    }
                }

                if(hx.config.settings.channelgroupT.bandwidth7MHz.channel[i]){
                    aParam.channelNumber = hx.config.settings.channelgroupT.bandwidth7MHz.channel[i];
                }
            } else if (aParam.tpInfo.bandwidth === '8MHz') {
                length = hx.config.settings.channelgroupT.bandwidth8MHz.centreFrequency.length;
                for ( i = 0; i < length; i+=1) {
                    if (this._freq === hx.config.settings.channelgroupT.bandwidth8MHz.centreFrequency[i]) {
                        break;
                    }
                }
                if(hx.config.settings.channelgroupT.bandwidth8MHz.channel[i]){
                    aParam.channelNumber = hx.config.settings.channelgroupT.bandwidth8MHz.channel[i];
                }
            }
            if(aParam.channelNumber){
                showString = 'CH' + aParam.channelNumber + ', ' + this._freq + 'kHz, ' + aParam.tpInfo.bandwidth;
            }else{
                showString = this._freq + 'kHz, ' + aParam.tpInfo.bandwidth;
            }

        }

        if (this._freq) {
            this._progress.$.find('dt').html(showString);
        } else {
            this._progress.$.find('dt').html('');
        }

        this._tvCount = searchedTvListLength;
        this._radioCount = searchedRadioListLength;
        this._channelCount = aParam.owSearchedInfos.length;

        /* show tvList and radioList in shearchedInfos maximum 6 item */
        showtvLength = showtvList.length;
        showradioLength = showradioList.length;

        for ( i = 0; i < showtvLength; i += 1) {
            tvindex = showtvList.pop();
            tvname = aParam.owSearchedInfos.getInfo(tvindex, "name");
            this._tvlist.addItem(tvname);
        }
        for ( i = 0; i < showradioLength; i += 1) {
            radioindex = showradioList.pop();
            radioname = aParam.owSearchedInfos.getInfo(radioindex, "name");
            this._radiolist.addItem(radioname);
        }
        if (this._tvlist._$items.length === 0) {
            for ( i = 0; i < this._tempTvList.length; i += 1) {
                this._tvlist.addItem(this._tempTvList[i]);
            }
        }
        if (this._radiolist._$items.length === 0) {
            for ( i = 0; i < this._tempRadioList.length; i += 1) {
                this._radiolist.addItem(this._tempRadioList[i]);
            }
        }

        this._tvcount.$.html(hx.l('LOC_TV_ID') + ' : ' + this._tvCount);
        this._radiocount.$.html(hx.l('LOC_RADIO_ID') + ' : ' + this._radioCount);

        /* Show search progress */
        this._progressVariable = aParam.progress;
        if (aParam.progress < 0) {
            this._progressVariable = 0;
        }

        //progressVariable will be used to check search completion
        this._progress.$.find('dd').eq(0).html(this._progressVariable + '%');
        this._progress.$.find('dd').eq(1).find('span').css('width', this._progressVariable + '%');

        /* Show All searched channel count */
        this._allResult.$.find('em').html(' ' + this._channelCount);

        if (aParam.progress === 100) {
            if (this._channelCount > 0) {
                this._button.$.html(hx.l('LOC_SAVE_ID'));
            } else {
                this._button.$.html(hx.l('LOC_EXIT_ID'));
            }
        }
    }
    private _isTRDConflictDlgOpened = false;
    private _popup = [];
    private _checkTRD() {
        new __CONTROL_TRD_CONFLICT_DLG__(
            (aDlg) => {
                this._button.off();
                this._popup.push(aDlg);
                this._isTRDConflictDlgOpened = true;
            },
            (aDlg) => {
                var index = this._popup.indexOf(aDlg);
                if (index !== -1) {
                    this._popup.splice(index, 1);
                }
                this._button.on();
            },
            () => {
                this._isTRDConflictDlgOpened = false;
                this._savingProgress = hx.msg('progress', {
                    text: hx.l('LOC_SAVING_CHANNELS_ID'),
                    auto_close: false
                    //timeout : 5000,
                });
                hx.svc.CHANNEL_SEARCH_T.saveChannelSearchResult();
                hx.msg.close('progress', this._savingProgress);
                this._changeToLastChannel();
                this._goExit();
            }
        );
    }
    _on_ChannelListUpdate () {
        // tune to available channel
        if (this._getChupdateEvt === true) {//already get ChannelListUpdate Event
            return;
        }
        this._getChupdateEvt = true;

        if (ENV.op === ENV.listOP.AUSDTT || ENV.op === ENV.listOP.UKDTT) {
            this._checkTRD();
        } else {
            hx.svc.CHANNEL_SEARCH_T.saveChannelSearchResult();
            hx.msg.close('progress', this._savingProgress);
            this._changeToLastChannel();
            this._goExit();
        }

    }
    _changeToLastChannel () {
        // tune to available channel
        var result : any = hx.svc.AV_CTRL.lastChannel();
        if (result === false) {
            hx.svc.AV_CTRL.availableChannel();
        }
    }
    _goExit () {
        var targetPage : any;
        hx.il.criticalTasking.crChannelSearchTasking = false;
        if (ENV.op === ENV.listOP.AUSDTT || ENV.op === ENV.listOP.UKDTT) {
            targetPage = 'CPgSettings';
        }
        this._isChangePageInSetting = true;
        this.sendEventToEntity({
            alEvent : 'alChangePage',
            target : targetPage,
            me : this
        });
    }
    _on_alKey (aParam) : boolean {
        var bConsumed : boolean = false;
        var popup = this._popup;
        if (popup.length) {
            for (var i = 0; i < popup.length; i++) {
                bConsumed = popup[i].doKey(aParam.alKey);
                if (bConsumed) {
                    return bConsumed;
                }
            }
        }
        switch (aParam.alKey) {
            case hx.key.KEY_ESCAPE:
                bConsumed = true;
                if (this._isSearching === 'true') {
                    /*  WHEN STOP BUTTON */
                    if (this._progressVariable !== 100) {
                        this._makeMsgbox();
                        hx.svc.CHANNEL_SEARCH_T.pauseScan();
                        this._isSearching = 'false';
                    }
                    /* WHEN SAVE BUTTON */
                    else {
                        this._makeMsgbox2();
                        hx.svc.CHANNEL_SEARCH_T.pauseScan();
                        this._isSearching = 'false';
                    }
                }
                break;
            case hx.key.KEY_MENU:
            case hx.key.KEY_GUIDE:
            case hx.key.KEY_ON_DEMAND:
                bConsumed = true;
                break;
            case hx.key.KEY_POWER:
                hx.svc.CHANNEL_SEARCH_T.stopScan();
                bConsumed = false;
                break;
        }
        return bConsumed;
    }
    _on_alClicked (aParam) : boolean {
        var data : any = aParam.alData, targetPage : any = null, bConsumed : boolean = false;
        if (data.me === this._button) {
            bConsumed = true;
            /*STOP & SAVE BUTTON */
            if (this._progressVariable !== 100) {
                this._makeMsgbox();
                hx.svc.CHANNEL_SEARCH_T.pauseScan();
                this._isSearching = 'false';
            } else {
                if (this._channelCount > 0) {
                    if (ENV.op === ENV.listOP.AUSDTT || ENV.op === ENV.listOP.UKDTT) {
                        if (this._isTRDConflictDlgOpened) {
                            this._checkTRD();
                            return bConsumed;
                        }
                    } else {
                        this._savingProgress = hx.msg('progress', {
                            text : hx.l('LOC_SAVING_CHANNELS_ID'),
                            auto_close : false
                            //timeout : 5000,
                        });
                    }
                    hx.svc.CHANNEL_SEARCH_T.loadChannelSearchResult();
                } else {
                    hx.svc.CHANNEL_SEARCH_T.stopScan();
                    this._changeToLastChannel();
                    this._goExit();
                }
            }
        }
        return bConsumed;
    }
    _on_alExit (aParam) : boolean {
        var data : any = aParam.alData, bConsumed : boolean = false;
        if (data.me === this._button) {
            bConsumed = true;
            /*  WHEN STOP BUTTON */
            if (this._progressVariable !== 100) {
                var noWork;
            }
            /* WHEN SAVE BUTTON */
            else {
                var noWork2;
                /*
                 that.$line.removeClass('on');
                 that.$line2.addClass('on');
                 that._button4.on();

                 hx.svc.CHANNEL_SEARCH_T.pauseScan();
                 */
            }
        }
        return bConsumed;
    }
    _makeMsgbox () {
        var that = this;
        var yesBtn : string = hx.l('LOC_YES_ID'), noBtn : string = hx.l('LOC_NO_ID');
        var msgbox = hx.msg('confirm', {
            text : hx.l('LOC_STOP_THE_CHANNEL_SEARCH_ID'),
            auto_close : false,
            btn_title_arr : [yesBtn, noBtn],
            dia_class : 'osd',
            callback_fn : function(p) {
                if (p === yesBtn) {
                    hx.msg.close('confirm', msgbox);
                    that._makeMsgbox2();
                } else if (p === noBtn) {
                    hx.msg.close('confirm', msgbox);
                    that._button.on();

                    that._tempTvCount = that._tvCount;
                    that._tempRadioCount = that._radioCount;
                    hx.svc.CHANNEL_SEARCH_T.resumeScan();
                    that._isSearching = 'true';
                } else if (p === 'key_esc' || p === 'key_back') {
                    hx.msg.close('confirm', msgbox);
                    that._button.on();

                    that._tempTvCount = that._tvCount;
                    that._tempRadioCount = that._radioCount;
                    hx.svc.CHANNEL_SEARCH_T.resumeScan();
                    that._isSearching = 'true';
                }
            }
        });
    }
    _makeMsgbox2 () {
        var that = this;
        var yesBtn : string = hx.l('LOC_YES_ID'), noBtn : string = hx.l('LOC_NO_ID');
        var msgbox2 = hx.msg('confirm', {
            text : hx.l('LOC_SAVE_CHANNEL_SEARCH_ID'),
            auto_close : false,
            btn_title_arr : [yesBtn, noBtn],
            dia_class : 'osd',
            callback_fn : function(p) {
                if (p === yesBtn && that._channelCount > 0) {
                    hx.msg.close('confirm', msgbox2);
                    if (ENV.op === ENV.listOP.AUSDTT || ENV.op === ENV.listOP.UKDTT) {
                        if (that._isTRDConflictDlgOpened) {
                            that._checkTRD();
                            return;
                        }
                    } else {
                        that._savingProgress = hx.msg('progress', {
                            text: hx.l('LOC_SAVING_CHANNELS_ID'),
                            auto_close: false
                            //timeout : 5000,
                        });
                    }
                    hx.svc.CHANNEL_SEARCH_T.loadChannelSearchResult();

                } else if (p === noBtn || (p === yesBtn && that._channelCount <= 0)) {
                    hx.msg.close('confirm', msgbox2);
                    hx.svc.CHANNEL_SEARCH_T.stopScan();
                    that._changeToLastChannel();
                    that._goExit();
                } else if (p === 'key_esc' || p === 'key_back') {
                    hx.msg.close('confirm', msgbox2);
                    that._button.on();

                    that._tempTvCount = that._tvCount;
                    that._tempRadioCount = that._radioCount;
                    hx.svc.CHANNEL_SEARCH_T.resumeScan();
                    that._isSearching = 'true';
                }
            }
        });
    }

    /* DATAS IN THIS PAGE */
    private tvList = [];
    private radioList = [];
    private aniConfig = {
        fadeIn : {
            start : {
                'opacity': 0
            },
            end : {
                'opacity': 1
            }
        },
        dep2: {
            fadeIn: {
                start: {
                    'opacity': 0,
                    'left': '795px'
                },
                end: {
                    'opacity': 1,
                    'left': '775px'
                }
            },
            fadeOut: {
                start: {
                    'opacity': 1,
                    'left': '775px'
                },
                end: {
                    'opacity': 0,
                    'left': '795px'
                }
            }
        },
        menu: {
            changeIn: {
                start: {
                    'opacity': 0,
                    'left': '20px'
                },
                end: {
                    'opacity': 1,
                    'left': 0
                }
            },
            changeOut3: {
                start: {
                    'opacity': 1,
                    'left': 0
                },
                end: {
                    'opacity': 0,
                    'left': '20px'
                }
            }
        }
    }
    /* COMPONENTS IN THIS PAGE */
    private string(aParam) {
        return hx.al.component('text', {
            tag: 'h2',
            text: aParam,
            attr: {
                'data-langID': false,
                'class': ''
            }
        });
    }

    private button(aParam) {
        return (new hx.al.component('button', {
            attr: {
                'class': 'button',
                'data-langID': aParam
            },
            usePointerKey: true
        }));
    }

    private Search_result(aParam) {
        var searchresult = hx.al.component('Search_result', {
            itemAttr: {},
            attr: {
                'style': 'height:208px'
            },
            data: aParam,
            count: aParam.length,
            fx: function (i, o) {
                o.html(this.data[i]);
            }
        });
        return searchresult;
    }

    private progress3() {
        return hx.al.component('searchProgress', {
            tag: 'dl',
            data: [' ', '0%', 'width:0%'],
            attr: {
                'class': 'progress type3'
            }
        });
    }

    private allResult() {
        return hx.al.component('txt2', {
            tag: 'p',
            attr: {
                'class': 'srcResult'
            },
            data: [hx.l('LOC_CHANNELS_FOUND_ID'), '0']
        });
    }
}


export = cCPgAutomaticSearch;
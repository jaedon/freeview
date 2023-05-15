///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cOta.js
/*jslint vars:true */
/*jslint nomen:true */
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/component_kernel"], function (require, exports, __COMPONENT_KERNEL__) {
    var OtaNoti = (function (_super) {
        __extends(OtaNoti, _super);
        function OtaNoti() {
            _super.call(this);
            this.aniAction = hx.il.ani.action2;
            this.isShow = false;
            this._step = 0;
            this._sceneIndex = 0;
            this._type = 'otanoti';
            this.dissolveInterval = null;
            this.aniStyleMap = {
                fadeIn: {
                    start: {
                        'opacity': 0
                    },
                    end: {
                        'opacity': 1
                    }
                },
                fadeOut: {
                    start: {
                        'opacity': 1
                    },
                    end: {
                        'opacity': 0
                    }
                },
                step: {
                    fadeIn: {
                        start: {
                            'opacity': 0,
                            '-o-transform': 'scale(0.9,0.9)'
                        },
                        end: {
                            'opacity': 1,
                            '-o-transform': 'scale(1.0,1.0)'
                        }
                    },
                    fadeOut: {
                        start: {
                            'opacity': 1,
                            '-o-transform': 'scale(0.9,0.9)'
                        },
                        end: {
                            'opacity': 0,
                            '-o-transform': 'scale(1.0,1.0)'
                        }
                    }
                },
                pmoItem: {
                    nextOut: {
                        start: {
                            'opacity': 1,
                            'left': 0
                        },
                        end: {
                            'opacity': 0,
                            'left': '-30px'
                        }
                    },
                    nextIn: {
                        start: {
                            'opacity': 0,
                            'left': '70px'
                        },
                        end: {
                            'opacity': 1,
                            'left': 0
                        }
                    },
                    prevOut: {
                        start: {
                            'opacity': 1,
                            'left': 0
                        },
                        end: {
                            'opacity': 0,
                            'left': '30px'
                        }
                    },
                    prevIn: {
                        start: {
                            'opacity': 0,
                            'left': '-70px'
                        },
                        end: {
                            'opacity': 1,
                            'left': 0
                        }
                    }
                }
            };
        }
        OtaNoti.prototype._changeScene = function (that) {
            var stepLists = that.section.querySelectorAll('ul');
            var scenesLists = stepLists[that._step].querySelectorAll('li');
            if (scenesLists.length <= 1) {
                clearInterval(that.dissolveInterval);
                that.dissolveInterval = null;
                return;
            }
            var $currentDesp = $(scenesLists[that._sceneIndex]);
            that._sceneIndex += 1;
            if (that._sceneIndex === scenesLists.length) {
                that._sceneIndex = 0;
            }
            var $newDesp = $(scenesLists[that._sceneIndex]);
            $newDesp.show();
            $currentDesp.ani({
                style: that.aniStyleMap.fadeOut,
                trans: {
                    dur: 350,
                    delay: 100
                }
            });
            $newDesp.ani({
                style: that.aniStyleMap.fadeIn,
                trans: {
                    dur: 320
                }
            });
        };
        OtaNoti.prototype._getItemData = function () {
            var updatedList, datas = [];
            updatedList = hx.svc.SETTING_UTIL.getUpdatedList();
            datas.push(updatedList.updatedList);
            return datas;
        };
        OtaNoti.prototype.execAni = function () {
            var that = this;
            var $Noti = this.$, $step = this.$step, $wrap_title = this.$wrap_title, $pmoItem = this.$pmoItem; //
            var $arrowR = this.$arrowR, aniInfo = this.isShow ? 'fadeOut' : 'fadeIn', fadeIn = this.aniStyleMap.fadeIn;
            $Noti.ani({
                style: fadeIn,
                trans: {
                    dur: 300
                }
            });
            $step.ani({
                style: this.aniStyleMap.step.fadeIn,
                trans: {
                    dur: 320,
                    delay: 300
                }
            });
            $wrap_title.ani({
                style: fadeIn,
                trans: {
                    dur: 420,
                    delay: 300
                }
            });
            $arrowR.ani({
                style: fadeIn,
                trans: {
                    dur: 420,
                    delay: 300
                }
            });
            $pmoItem.ani({
                style: fadeIn,
                trans: {
                    dur: 420,
                    delay: 300
                },
                cb: function () {
                    if (that.dissolveInterval !== null) {
                        clearInterval(that.dissolveInterval);
                        that.dissolveInterval = null;
                    }
                    that.dissolveInterval = setInterval(that._changeScene, 7000, that);
                }
            });
            this.isShow = !this.isShow;
        };
        OtaNoti.prototype.init = function () {
        };
        OtaNoti.prototype.create = function (aParam) {
            this._nativeFocus = false;
            this.dissolveInterval = null;
            $.extend(this, aParam);
            this.section = this.getTemplateEl({
                'step': this._step,
                'type': this._type
            });
            this.$ = $(this.section);
            this._leftArr = this.getOtherTemplateEl({
                'id': 'arL',
                'cls': 'arrow arL'
            });
            this._rightArr = this.getOtherTemplateEl({
                'id': 'arR',
                'cls': 'arrow arR'
            });
            this._stepList = this.getStepListTemplateEl();
            var article = this.section.querySelector('#step');
            article.appendChild(this._leftArr);
            article.appendChild(this._rightArr);
            article.appendChild(this._stepList);
            this.$wrap_title = this.$.find('.wrap_title');
            this.$step = this.$.find('.step');
            this.$pmo = this.$.find('.wrap_pmo');
            this.$pmoDesp = this.$.find('.pmoDesp');
            this.$pmoItem = $(this._stepList);
            this.$arrowR = $(this._rightArr);
            this.$arrowL = $(this._leftArr);
            this.execAni();
        };
        OtaNoti.prototype._moveStep = function (action) {
            var datas = [];
            var uidataObj;
            datas = this._getItemData();
            uidataObj = JSON.parse(datas);
            var uidataListItems = uidataObj.uidata.listitem;
            var currentStep = this._step;
            var isPrev = (action === 'prev');
            var nextFlag = (action === 'prev') ? -1 : 1;
            var nextStep = this._step + nextFlag;
            var isFirst = (isPrev && currentStep === 1);
            var isLast = (!isPrev && currentStep === uidataListItems.length - 2);
            var toBeShowArrowL = (!isPrev && currentStep === 0);
            var toBeShowArrowR = (isPrev && currentStep === uidataListItems.length - 1);
            var toChangetxt = true;
            var changeInfo = {
                action: action,
                isPrev: isPrev,
                nextFlag: nextFlag,
                isFirst: isFirst,
                isLast: isLast,
                toBeShowArrowL: toBeShowArrowL,
                toBeShowArrowR: toBeShowArrowR,
                toChangetxt: toChangetxt
            };
            this.changeStepOut(changeInfo);
        };
        OtaNoti.prototype.changeStepOut = function (aArg) {
            var stepLists = this.section.querySelectorAll('ul');
            var fadeOut = this.aniStyleMap.fadeOut;
            var $pmoItem = $(this.$pmo.find('.pmoItem').get(this._step));
            if (aArg.isLast) {
                this.$arrowR.ani({
                    style: fadeOut,
                    trans: {
                        dur: 300
                    }
                });
            }
            if (aArg.isFirst) {
                this.$arrowL.ani({
                    style: fadeOut,
                    trans: {
                        dur: 300
                    }
                });
            }
            if (aArg.toChangetxt) {
                this.$wrap_title.ani({
                    style: this.aniStyleMap.fadeOut,
                    trans: {
                        dur: 100
                    }
                });
            }
            clearInterval(this.dissolveInterval);
            this.dissolveInterval = null;
            $pmoItem.ani({
                style: this.aniStyleMap.pmoItem[aArg.action + 'Out'],
                trans: {
                    dur: 300
                },
                cb: this.changeStepIn({
                    changeInfo: aArg
                })
            });
            return false;
        };
        OtaNoti.prototype.changeStepIn = function (aArg) {
            var htmlTxt, txt1, txt2, txt3, txt4, txt5;
            var changeInfo = aArg.changeInfo, that = this;
            this._step += changeInfo.nextFlag;
            txt3 = '<p class="tit_txt01">';
            txt4 = hx.l('LOC_UPDATED_LIST_CAP_ID');
            txt5 = '</em></span>';
            htmlTxt = txt3 + txt4 + txt5;
            this.$wrap_title.html(htmlTxt);
            var $pmoItem = $(this.$pmo.find('.pmoItem').get(this._step));
            var fadeIn = this.aniStyleMap.fadeIn;
            if (!changeInfo.isFirst && changeInfo.toBeShowArrowL) {
                this.$arrowL.ani({
                    style: fadeIn,
                    trans: {
                        dur: 300,
                        delay: 300
                    }
                });
            }
            else if (!changeInfo.isLast && changeInfo.toBeShowArrowR) {
                this.$arrowR.ani({
                    style: fadeIn,
                    trans: {
                        dur: 300,
                        delay: 300
                    }
                });
            }
            if (changeInfo.toChangetxt) {
                this.$wrap_title.ani({
                    style: fadeIn,
                    trans: {
                        dur: 200,
                        delay: 300
                    }
                });
            }
            $pmoItem.ani({
                style: this.aniStyleMap.pmoItem[changeInfo.action + 'In'],
                trans: {
                    dur: 420
                },
                cb: function () {
                    if (that.dissolveInterval !== null) {
                        clearInterval(that.dissolveInterval);
                        that.dissolveInterval = null;
                    }
                    var stepLists = that.section.querySelectorAll('ul');
                    var scenesLists = stepLists[that._step].querySelectorAll('li');
                    var i;
                    for (i = 0; i < scenesLists.length; i += 1) {
                        if (scenesLists[i].style.opacity === '1') {
                            that._sceneIndex = i;
                            break;
                        }
                    }
                    that.dissolveInterval = setInterval(that._changeScene, 7100, that);
                }
            });
        };
        OtaNoti.prototype._doRight = function () {
            var datas = [];
            var uidataObj;
            datas = this._getItemData();
            uidataObj = JSON.parse(datas);
            var uidataListItems = uidataObj.uidata.listitem;
            if (this._step < uidataListItems.length - 1) {
                this._moveStep('next');
            }
        };
        OtaNoti.prototype._doLeft = function () {
            if (this._step > 0) {
                this._moveStep('prev');
            }
        };
        OtaNoti.prototype.destroy = function () {
            var Type = this._type, dataObj = {};
            clearInterval(this.dissolveInterval);
            dataObj[Type] = "true";
            hx.svc.WEB_STORAGE.setOtaNoti(dataObj);
            if (this.callback) {
                this.callback();
            }
            this.$.remove();
            this.$ = null;
            if (!!Type && Type === 'otanoti') {
                var param = {};
                param.alEvent = 'alClicked';
                param.alData = {};
                param.alData.me = this;
                param.alData.msg = 'destroyed';
                this.sendEventToParent(param);
            }
        };
        OtaNoti.prototype._on_alKey = function (aParam) {
            var bConsumed = false, k = hx.key, kc = aParam.alKey;
            switch (kc) {
                case k.KEY_LEFT:
                    this._doLeft();
                    bConsumed = true;
                    break;
                case k.KEY_RIGHT:
                    this._doRight();
                    bConsumed = true;
                    break;
                case k.KEY_OK:
                case k.KEY_ESCAPE:
                    this._doBack();
                    bConsumed = true;
                    break;
                case k.KEY_BACK:
                    if (this._step === 0) {
                        this._doBack();
                        bConsumed = true;
                    }
                    else {
                        this._moveStep('prev');
                    }
                    break;
                case k.KEY_UP:
                case k.KEY_DOWN:
                case k.KEY_CHANNEL_UP:
                case k.KEY_CHANNEL_DOWN:
                case k.KEY_PLAY:
                case k.KEY_OPTION:
                case k.KEY_SEARCH:
                case k.KEY_MENU:
                case k.KEY_GUIDE:
                case k.KEY_PLAYER:
                case k.KEY_ON_DEMAND:
                case k.KEY_0:
                case k.KEY_1:
                case k.KEY_2:
                case k.KEY_3:
                case k.KEY_4:
                case k.KEY_5:
                case k.KEY_6:
                case k.KEY_7:
                case k.KEY_8:
                case k.KEY_9:
                    bConsumed = true;
                    break;
            }
            if (hx.config.netflix) {
                switch (kc) {
                    case k.KEY_NETFLIX:
                        bConsumed = true;
                        break;
                }
            }
            return bConsumed;
        };
        OtaNoti.prototype._doBack = function () {
            this.parent.on();
            this.destroy();
            return true;
        };
        OtaNoti.prototype.getTemplateEl = function (aArgObject) {
            var txt;
            var tpl1 = '<section id="Noti" style="opacity: 0;"><article class="step first" id="step"><hgroup class="wrap_title" style="opacity: 0;">';
            var tpl2 = '<p class="tit_txt01">';
            var tpl3 = '</p></hgroup></article></section>';
            var div = document.createElement('div');
            txt = hx.l('LOC_UPDATED_LIST_CAP_ID');
            div.innerHTML = (tpl1 + tpl2 + txt + tpl3);
            return div.firstChild;
        };
        OtaNoti.prototype.getStepListTemplateEl = function () {
            var datas = [];
            var uidataObj;
            datas = this._getItemData();
            uidataObj = JSON.parse(datas);
            var stringdata = uidataObj.uidata.listitem[0][1].string;
            var uidataListItems = uidataObj.uidata.listitem;
            var i, j, len, listLen;
            var tpl = '<aside class="wrap_pmo"><div class="item">';
            if (ENV.listModel.IR4000HD === ENV.model) {
                var newimg = 'product/ir4000hd/image/Ota_Noti01.png';
                var defaultimg = 'product/ir4000hd/image/Ota_Noti01.png';
            }
            else {
                var newimg = 'images/Noti01.png';
                var defaultimg = 'images/Noti02.png';
            }
            len = uidataListItems.length;
            for (i = 0; i < len; i += 1) {
                tpl += '<ul class="pmoItem" style="opacity:' + (i === 0 ? 1 : 0) + '">';
                for (j = 1, listLen = uidataListItems[i].length; j < listLen; j += 1) {
                    tpl += '<li class="pmoDesp on" style="opacity:' + (j === 1 ? 1 : 0) + '">';
                    if (uidataListItems[i][0].type[0] === "defaultimage") {
                        tpl += '<div class="promImg"><img src="' + defaultimg + '"></div>';
                    }
                    else if (uidataListItems[i][0].type[0] === "newimage") {
                        tpl += '<div class="promImg"><img src="' + newimg + '"></div>';
                    }
                    tpl += '<div class="info01 info_txt02">';
                    tpl += '<span class="info_txt01">' + uidataListItems[i][1].string[0] + '</span>';
                    tpl += '</div>';
                    tpl += '<div class="button">' + 'CLOSE' + '</div>';
                    tpl += '</li>';
                }
                tpl += '</ul>';
            }
            tpl += '</div></aside>';
            var div = document.createElement('div');
            div.innerHTML = tpl;
            return div.firstChild;
        };
        OtaNoti.prototype.getOtherTemplateEl = function (aArgObject) {
            var tpl = '<div id="' + aArgObject.id + '" class="' + aArgObject.cls + '" style="opacity: 0;"></div>';
            var div = document.createElement('div');
            div.innerHTML = tpl;
            return div.firstChild;
        };
        return OtaNoti;
    })(__COMPONENT_KERNEL__);
    exports.OtaNoti = OtaNoti;
});

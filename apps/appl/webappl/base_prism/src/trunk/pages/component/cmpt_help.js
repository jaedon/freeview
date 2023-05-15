///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cHelp.js
/*jslint sloppy:true, nomen:true, vars:true*/
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/component_kernel"], function (require, exports, __COMPONENT_KERNEL__) {
    var HelpView = (function (_super) {
        __extends(HelpView, _super);
        function HelpView() {
            _super.call(this);
            this.aniAction = hx.il.ani.action2;
            this.isShow = false;
            this._step = 0;
            this._sceneIndex = 0;
            this._type = '';
            this.dissolveInterval = null;
            this._contents = hx.config.help;
            //animation info
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
        HelpView.prototype.init = function () {
        };
        HelpView.prototype._changeScene = function (aParam) {
            var stepLists = aParam.section.querySelectorAll('ul');
            var scenesLists = stepLists[aParam._step].querySelectorAll('li');
            if (scenesLists.length <= 1) {
                clearInterval(aParam.dissolveInterval);
                aParam.dissolveInterval = null;
                return;
            }
            var $currentDesp = $(scenesLists[aParam._sceneIndex]);
            aParam._sceneIndex += 1;
            if (aParam._sceneIndex === scenesLists.length) {
                aParam._sceneIndex = 0;
            }
            var $newDesp = $(scenesLists[aParam._sceneIndex]);
            $newDesp.show();
            $currentDesp.ani({
                style: aParam.aniStyleMap.fadeOut,
                trans: {
                    dur: 350,
                    delay: 100
                }
            });
            $newDesp.ani({
                style: aParam.aniStyleMap.fadeIn,
                trans: {
                    dur: 320
                }
            });
        };
        HelpView.prototype.execAni = function () {
            var $hep = this.$, $step = this.$step, $tit = this.$tit, $pmoItem = this.$pmoItem, $indicate = this.$indicate, $arrowR = this.$arrowR, aniInfo = this.isShow ? 'fadeOut' : 'fadeIn', that = this, fadeIn = this.aniStyleMap.fadeIn;
            $hep.ani({
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
            $tit.ani({
                style: fadeIn,
                trans: {
                    dur: 420,
                    delay: 300
                }
            });
            $indicate.ani({
                style: fadeIn,
                trans: {
                    dur: 420,
                    delay: 760
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
        HelpView.prototype.create = function (aParam) {
            this._nativeFocus = false;
            this.dissolveInterval = null;
            //variable
            $.extend(this, aParam);
            //this._type = 'TVG';
            this.section = this.getTemplateEl({
                'step': this._step,
                'type': this._type
            });
            this.$ = $(this.section);
            // close button
            this._close = this.getOtherTemplateEl({
                'id': 'close',
                'cls': 'close'
            });
            // arrow button
            this._leftArr = this.getOtherTemplateEl({
                'id': 'arL',
                'cls': 'arrow arL'
            });
            this._rightArr = this.getOtherTemplateEl({
                'id': 'arR',
                'cls': 'arrow arR'
            });
            this._stepList = this.getStepListTemplateEl(this._contents[this._type].step);
            // indicate
            this._indicate = this.getIndecateTemplateEl({
                'len': this._contents[this._type].step.length
            });
            this._indicate.querySelectorAll('p')[0].classList.add('on');
            var article = this.section.querySelector('#step');
            article.appendChild(this._close);
            article.appendChild(this._leftArr);
            article.appendChild(this._rightArr);
            article.appendChild(this._stepList);
            article.appendChild(this._indicate);
            this.$tit = this.$.find('.tit');
            this.$step = this.$.find('.step');
            this.$pmo = this.$.find('.wrap_pmo');
            this.$pmoDesp = this.$.find('.pmoDesp');
            this.$pmoItem = $(this._stepList);
            this.$arrowR = $(this._rightArr);
            this.$arrowL = $(this._leftArr);
            this.$indicate = $(this._indicate).find('p');
            this.$close = $(this._close);
            this.execAni();
        };
        /**
         * @private
         * @event
         * @param {String}
    
         */
        HelpView.prototype._moveStep = function (aAction) {
            var currentStep = this._step;
            var isPrev = (aAction === 'prev');
            var nextFlag = (aAction === 'prev') ? -1 : 1;
            var nextStep = this._step + nextFlag;
            var isFirst = (isPrev && currentStep === 1);
            var isLast = (!isPrev && currentStep === this._contents[this._type].step.length - 2);
            var toBeShowArrowL = (!isPrev && currentStep === 0);
            var toBeShowArrowR = (isPrev && currentStep === this._contents[this._type].step.length - 1);
            var toChangetxt = (this._contents[this._type].step[currentStep][0].txtnum !== this._contents[this._type].step[nextStep][0].txtnum) ? true : false;
            var changeInfo = {
                action: aAction,
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
        HelpView.prototype.changeStepOut = function (aArg) {
            // contentsSteps = contents.step;
            var that = this;
            var stepLists = that.section.querySelectorAll('ul');
            var stepTitle = document.querySelector('#stepTitle');
            var fadeOut = this.aniStyleMap.fadeOut;
            var $pmoItem = $(this.$pmo.find('.pmoItem').get(this._step));
            var $tit = this.$tit;
            if (aArg.isLast) {
                this.$arrowR.ani({
                    style: fadeOut,
                    trans: {
                        dur: 300
                    }
                });
            }
            else if (this.$close.css('opacity') === '1') {
                this.$close.ani({
                    style: fadeOut,
                    trans: {
                        dur: 100
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
            //tit animation
            if (aArg.toChangetxt) {
                this.$tit.ani({
                    style: this.aniStyleMap.fadeOut,
                    trans: {
                        dur: 100
                    }
                });
            }
            clearInterval(that.dissolveInterval);
            that.dissolveInterval = null;
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
        HelpView.prototype.changeStepIn = function (aParam) {
            var htmlTxt, txt1, txt2, txt3, txt4, txt5;
            var changeInfo = aParam.changeInfo, contents = this._contents[this._type], that = this;
            this.$indicate[this._step].classList.remove('on');
            this._step += changeInfo.nextFlag;
            this.$indicate[this._step].classList.add('on');
            txt1 = '<span><h1 id="stepTitle">0';
            txt2 = (contents.step[this._step][0].txtnum + 1);
            txt3 = '</h1></span><span><em class="txt">';
            txt4 = this._conv_String(contents.step[this._step][0].txt);
            txt5 = '</em></span>';
            htmlTxt = txt1 + txt2 + txt3 + txt4 + txt5;
            this.$tit.html(htmlTxt);
            var $pmoItem = $(this.$pmo.find('.pmoItem').get(this._step));
            var fadeIn = this.aniStyleMap.fadeIn;
            if (changeInfo.isLast) {
                this.$close.ani({
                    style: fadeIn,
                    trans: {
                        dur: 200,
                        delay: 300
                    }
                });
            }
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
                this.$tit.ani({
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
        HelpView.prototype._doRight = function () {
            if (this._step < this._contents[this._type].step.length - 1) {
                this._moveStep('next');
            }
        };
        HelpView.prototype._doLeft = function () {
            if (this._step > 0) {
                this._moveStep('prev');
            }
        };
        HelpView.prototype.destroy = function () {
            var _this = this;
            var helpType = this._type, dataObj = {};
            clearInterval(this.dissolveInterval);
            dataObj[helpType] = true;
            hx.svc.WEB_STORAGE.setHelp(dataObj);
            if (this.callback) {
                this.callback();
            }
            this.$.remove();
            this.$ = null;
            $('#wrap').find('#hep').remove();
            // send to live
            if (!!helpType && helpType === 'ltv') {
                var param = {};
                param.alEvent = 'alClicked';
                param.alData = {};
                param.alData.me = this;
                param.alData.msg = 'destroyed';
                this.sendEventToParent(param);
            }
            if (!!helpType && helpType === 'home') {
                this.parent.sendEventToEntity({
                    alEvent: 'alMessageToPage',
                    target: 'CPgHubProxy?show=toggle'
                });
            }
            if (this._cbFnEnd) {
                this._cbFnEnd();
            }
            this.getParent()._subObj = this.getParent()._subObj.filter(function (obj) {
                return obj !== _this;
            });
        };
        HelpView.prototype._on_alKey = function (aParam) {
            var bConsumed = false, k = hx.key, kc = aParam.alKey;
            hx.log("info", "Help - HelpView(Comp) - _on_alKey(" + aParam.alKey + ")");
            switch (kc) {
                case k.KEY_LEFT:
                    this._doLeft();
                    bConsumed = true;
                    break;
                case k.KEY_RIGHT:
                    this._doRight();
                    bConsumed = true;
                    break;
                case k.KEY_ESCAPE:
                    this._doBack();
                    bConsumed = true;
                    break;
                case k.KEY_BACK:
                    if (this._step === 0) {
                        this._doBack();
                    }
                    else {
                        this._doLeft();
                    }
                    bConsumed = true;
                    break;
                case k.KEY_OK:
                    var stepLists = this.section.querySelectorAll('ul');
                    if (this._step === (stepLists.length - 1)) {
                        this._doBack();
                    }
                    bConsumed = true;
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
                case k.KEY_RECORD:
                case k.KEY_PAUSE:
                case k.KEY_STOP:
                case k.KEY_FAST_FWD:
                case k.KEY_REWIND:
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
        HelpView.prototype._doBack = function () {
            this.parent.on();
            this.destroy();
            return true;
        };
        HelpView.prototype._conv_String = function (aTextArray) {
            var i, len, retString = '';
            len = aTextArray.length;
            for (i = 0; i < len; i += 1) {
                if (typeof (aTextArray[i]) === 'object') {
                    var formatObj = aTextArray[i];
                    // FIXME : It is just work around... :-).....
                    if (formatObj.param.length === 1) {
                        retString += hx.util.format(hx.l(formatObj.stringFormat), formatObj.param[0]);
                    }
                    else if (formatObj.param.length === 2) {
                        retString += hx.util.format(hx.l(formatObj.stringFormat), formatObj.param[0], formatObj.param[1]);
                    }
                    else if (formatObj.param.length === 3) {
                        retString += hx.util.format(hx.l(formatObj.stringFormat), formatObj.param[0], formatObj.param[1], formatObj.param[2]);
                    }
                }
                else if (typeof (aTextArray[i]) === 'string') {
                    if (aTextArray[i].substr(0, 4) !== 'LOC_') {
                        retString += aTextArray[i];
                    }
                    else {
                        retString += hx.l(aTextArray[i]);
                    }
                }
            }
            return retString;
        };
        HelpView.prototype.getTemplateEl = function (aParam) {
            var txt;
            var tpl1 = '<section id="hep" style="opacity: 0;"><article class="step first" id="step" style="opacity: 0;"><div class="tit" style="opacity: 0;"><span><h1 id="stepTitle">0';
            var tpl2 = (aParam.step + 1);
            var tpl3 = '</h1></span><span><em class="txt">';
            //var tpl4 = txt;
            var tpl5 = '</em></span></div><p class="line"> </p></article></section>';
            var div = document.createElement('div');
            txt = this._conv_String(this._contents[aParam.type].txt[0]);
            div.innerHTML = (tpl1 + tpl2 + tpl3 + txt + tpl5);
            return div.firstChild;
        };
        HelpView.prototype.getStepListTemplateEl = function (aParam) {
            var tpl = '<aside class="wrap_pmo"><div class="pmoList" id="pmoList" style="left: -0px;" >';
            var i, len;
            // list
            len = aParam.length;
            for (i = 0; i < len; i += 1) {
                var listObj = aParam[i], j, listLen;
                tpl += '<ul class="pmoItem" style="opacity:' + (i === 0 ? 1 : 0) + '">';
                for (j = 0, listLen = listObj.length; j < listLen; j += 1) {
                    tpl += '<li class="pmoDesp on" style="opacity:' + (j === 0 ? 1 : 0) + '">';
                    tpl += '<figure><img src="' + listObj[j].scene + '"></figure>';
                    tpl += '<p>' + this._conv_String(listObj[j].caption) + '</p>';
                    tpl += '</li>';
                }
                tpl += '</ul>';
            }
            tpl += '</div></aside>';
            var div = document.createElement('div');
            div.innerHTML = tpl;
            return div.firstChild;
        };
        HelpView.prototype.getIndecateTemplateEl = function (aArgObject) {
            var tpl = '<nav class="indicate" style="opacity: 1;">';
            var i, len;
            len = aArgObject.len;
            for (i = 0; i < len; i += 1) {
                tpl += '<p class="button"></p>';
            }
            var div = document.createElement('div');
            div.innerHTML = tpl;
            return div.firstChild;
        };
        HelpView.prototype.getOtherTemplateEl = function (aArgObject) {
            var tpl = '<p id="' + aArgObject.id + '" class="' + aArgObject.cls + '" style="opacity: 0;"></p>';
            var div = document.createElement('div');
            div.innerHTML = tpl;
            return div.firstChild;
        };
        return HelpView;
    })(__COMPONENT_KERNEL__);
    exports.HelpView = HelpView;
});

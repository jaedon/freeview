/**
 * Created by bglee on 2015-02-06.
 */
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", 'prismux/controls/ctrlbase', "prismux/layout/prism.layout.arrow"], function (require, exports, __CTRL_BASE__, CArrowLayout) {
    var CSliderControl = (function (_super) {
        __extends(CSliderControl, _super);
        function CSliderControl(aModel) {
            _super.call(this, $("<div>", { id: "ctrl-slider" }));
            this._pos = 0;
            this._$title = $("<div>", { "class": "-title" });
            this._$slideList = $("<div>", { "class": "-slideList" });
            this._$exit = $("<div>", { "class": "-exit" });
            this._$line = $("<div>", { "class": "-line" });
            this._arrows = {
                left: null,
                right: null
            };
            this.registerSignal(["ReqClose"]);
            if (aModel.length === 0) {
                throw "what?";
            }
            if (aModel.length > 1) {
                this._arrows.left = new CArrowLayout(this._element, "-left", "url(./images/312_Help_Arrow_L.png)");
                this._arrows.right = new CArrowLayout(this._element, "-right", "url(./images/312_Help_Arrow_R.png)");
                this._arrows.right.show();
                this._$step = $("<div>", { "class": "-step" });
                this._$indicator = $("<div>", { "class": "-indicator" });
                this.$self().append(this._$step).append(this._$indicator);
                for (var i = 0; i < aModel.length; i++) {
                    this._$indicator.append($("<div>", { "class": "-button" }));
                }
            }
            for (var i = 0; i < aModel.length; i++) {
                var $container = $("<div>", { "class": "-container" });
                var $img = $("<div>", { "class": "-img" });
                var $desc = $("<div>", { "class": "-desc" });
                $img[0].style.backgroundImage = "url(" + aModel[i].image + ")";
                $desc.html(aModel[i].desc);
                $container.append($img);
                $container.append($desc);
                this._$slideList.append($container);
            }
            this.$self().append(this._$title).append(this._$line).append(this._$slideList).append(this._$exit);
            this._model = aModel;
            this.setSlider(0);
        }
        CSliderControl.prototype._correct = function (aPos) {
            if (aPos < 0) {
                aPos = 0;
            }
            else if (aPos >= this._model.length) {
                aPos = this._model.length - 1;
            }
            return aPos;
        };
        CSliderControl.prototype._padZero = function (origin, digit) {
            var zero = "0";
            var ret = origin.toString();
            while (ret.length < digit) {
                ret = zero + ret;
            }
            return ret;
        };
        CSliderControl.prototype._changeScean = function (aPos) {
            var _this = this;
            var prevPos = this._pos;
            var $childs = this._$slideList.children();
            if (aPos === prevPos) {
                if (aPos === 0 && !($childs[0].classList.contains("-on"))) {
                    $childs[0].classList.add("-on");
                }
                return;
            }
            if ($childs.length < 2) {
                return;
            }
            $childs[prevPos].classList.remove("-on");
            $childs[aPos].classList.add("-on");
            this._$step.addClass("-fadeIn");
            this._$title.addClass("-fadeIn");
            if (aPos > prevPos) {
                $childs[prevPos].classList.add("-nextOut");
                $childs[aPos].classList.add("-nextIn");
            }
            else {
                $childs[prevPos].classList.add("-prevOut");
                $childs[aPos].classList.add("-prevIn");
            }
            setTimeout(function () {
                _this._$step.removeClass("-fadeIn");
                _this._$title.removeClass("-fadeIn");
                if (aPos > prevPos) {
                    $childs[prevPos].classList.remove("-nextOut");
                    $childs[aPos].classList.remove("-nextIn");
                }
                else {
                    $childs[prevPos].classList.remove("-prevOut");
                    $childs[aPos].classList.remove("-prevIn");
                }
            }, 400);
        };
        CSliderControl.prototype.setExitIcon = function (aSet) {
            if (aSet) {
                this._$exit.addClass(CSliderControl.CLASS_SHOW);
                this._$exit.addClass("-fadeIn");
            }
            else {
                this._$exit.removeClass(CSliderControl.CLASS_SHOW);
                this._$exit.removeClass("-fadeIn");
            }
        };
        CSliderControl.prototype.setStep = function (aPos) {
            if (this._$step) {
                var step = this._padZero((aPos + 1).toString(), 2);
                this._$step.text(step);
            }
        };
        CSliderControl.prototype.setIndicator = function (aPos) {
            if (this._$indicator) {
                var $childs = this._$indicator.children();
                $childs[this._pos].classList.remove('-on');
                $childs[aPos].classList.add('-on');
            }
        };
        CSliderControl.prototype.setArrow = function (aPos) {
            if (this._model.length < 2) {
                return;
            }
            if (aPos === 0) {
                this._arrows.left.hide();
                this._arrows.right.show();
            }
            else if (aPos === this._model.length - 1) {
                this._arrows.right.hide();
                this._arrows.left.show();
            }
            else {
                this._arrows.right.show();
                this._arrows.left.show();
            }
        };
        CSliderControl.prototype.setSlider = function (aPos) {
            var pos = this._correct(aPos);
            var model = this._model;
            this._$title.text(model[pos].title);
            this.setArrow(pos);
            this.setIndicator(pos);
            this.setStep(pos);
            this._changeScean(pos);
            this._pos = pos;
            this.setExitIcon(pos === model.length - 1);
        };
        CSliderControl.prototype._doKeyLeft = function () {
            this.setSlider(this._pos - 1);
            return this._pos < this._model.length;
        };
        CSliderControl.prototype._doKeyRight = function () {
            this.setSlider(this._pos + 1);
            return this._pos < this._model.length - 1;
        };
        CSliderControl.prototype._doKeyBack = function () {
            var posPrev = this._pos;
            this._doKeyLeft();
            return posPrev !== this._pos;
        };
        CSliderControl.prototype._doKeyEscape = function () {
            return this._pos !== this._model.length - 1;
        };
        CSliderControl.prototype._doKeyOk = function () {
            if (this._pos === this._model.length - 1) {
                this.emit.call(this, CSliderControl.EVENT_REQ_CLOSE);
            }
            return true;
        };
        CSliderControl.prototype.getPosition = function () {
            return this._pos;
        };
        CSliderControl.prototype.connectWhenReqClosed = function (aHolder, aSlotName, aSlot) {
            var self = this;
            this.connect(CSliderControl.EVENT_REQ_CLOSE, aHolder, aSlotName);
            this.connect(CSliderControl.EVENT_REQ_CLOSE, {
                disconnectWhenReqClosed: function () {
                    self.disconnect(aHolder);
                    self.disconnect(this);
                }
            }, 'disconnectWhenReqClosed');
        };
        CSliderControl.CLASS_SHOW = "-show";
        CSliderControl.EVENT_REQ_CLOSE = "ReqClose";
        return CSliderControl;
    })(__CTRL_BASE__.CControl);
    exports.CSliderControl = CSliderControl;
    var CModalDialogControl = (function (_super) {
        __extends(CModalDialogControl, _super);
        function CModalDialogControl(element) {
            _super.call(this, element);
            this._keyFlooding = ['Back', 'Escape', 'Power'];
        }
        CModalDialogControl.prototype.doKey = function (aKeyStr, aArgs) {
            _super.prototype.doKey.call(this, aKeyStr, aArgs);
            return this._keyFlooding.indexOf(aKeyStr) === -1;
        };
        return CModalDialogControl;
    })(__CTRL_BASE__.CGroupControl);
    exports.CModalDialogControl = CModalDialogControl;
    var CModalDialogSildeControl = (function (_super) {
        __extends(CModalDialogSildeControl, _super);
        function CModalDialogSildeControl(aParent, aID, aModel, autoClose) {
            var _this = this;
            if (autoClose === void 0) { autoClose = 0; }
            _super.call(this, document.createElement("div"));
            this.setId(aID);
            this._slider = new CSliderControl(aModel);
            this.setOwnedChildControls([this._slider]);
            aParent.appendChild(this._element);
            if (autoClose > 0) {
                setTimeout(function () {
                    _this._slider.emit(CSliderControl.EVENT_REQ_CLOSE);
                }, autoClose * 1000);
            }
        }
        CModalDialogSildeControl.prototype.connectWhenReqClosed = function (aHolder, aSlotName, aSlot) {
            this._slider.connectWhenReqClosed(aHolder, aSlotName, aSlot);
        };
        return CModalDialogSildeControl;
    })(CModalDialogControl);
    exports.CModalDialogSildeControl = CModalDialogSildeControl;
    var CCheckBoxControl = (function (_super) {
        __extends(CCheckBoxControl, _super);
        function CCheckBoxControl(action) {
            _super.call(this, $(document.createElement('div')));
            this.CHECKED = '-checked';
            this._fnAction = null;
            this.addClass('-ctrl-checkbox');
            this._checked = true;
            this.$self().addClass(this.CHECKED);
            this.setAction(action);
            this.registerSignal([CCheckBoxControl.ON_SELECTED]);
        }
        CCheckBoxControl.prototype.draw = function () {
        };
        CCheckBoxControl.prototype.doSelect = function () {
            if (this._checked) {
                this.$self().removeClass(this.CHECKED);
            }
            else {
                this.$self().addClass(this.CHECKED);
            }
            this._checked = !this._checked;
            this.emit(CButtonControl.ON_SELECTED, this._fnAction(this._checked));
            return false;
        };
        CCheckBoxControl.prototype.setAction = function (fn) {
            this._fnAction = fn;
        };
        CCheckBoxControl.ON_SELECTED = 'OnSelected';
        return CCheckBoxControl;
    })(__CTRL_BASE__.CControl);
    exports.CCheckBoxControl = CCheckBoxControl;
    var CButtonControl = (function (_super) {
        __extends(CButtonControl, _super);
        function CButtonControl(text, action) {
            _super.call(this, $(document.createElement('div')));
            this._fnAction = null;
            this.setText(text);
            this.setAction(action);
            this.registerSignal([CButtonControl.ON_SELECTED]);
        }
        CButtonControl.prototype.draw = function () {
            /* author: bglee@humaxdigital.com
             * date: 2015-03-11 오전 10:39
             * comments:
                - leaf control does not need to management focusables
             */
        };
        CButtonControl.prototype.doSelect = function () {
            this.emit(CButtonControl.ON_SELECTED, this._fnAction());
            return false;
        };
        CButtonControl.prototype.setText = function (text) {
            this.$self().text(text);
        };
        CButtonControl.prototype.setAction = function (fn) {
            this._fnAction = fn;
        };
        CButtonControl.ON_SELECTED = 'OnSelected';
        return CButtonControl;
    })(__CTRL_BASE__.CControl);
    exports.CButtonControl = CButtonControl;
    var CCheckBoxGroupControl = (function (_super) {
        __extends(CCheckBoxGroupControl, _super);
        function CCheckBoxGroupControl(element, checkboxInfo) {
            _super.call(this, element);
            this.onSelected = function () {
                throw 'do implement onSelected event function';
            };
            if (checkboxInfo.text) {
                this._elText = document.createElement('div');
                this._elText.classList.add('-text');
                this.setText(checkboxInfo.text);
            }
            element.classList.add('-ctrl-checkbox-group');
            this._setCheckBox(checkboxInfo.fn);
            this.setKeyBuilder(1 /* EVertical */);
            this.buildNaviMapForChild(__CTRL_BASE__.KBuilderTopDown);
        }
        CCheckBoxGroupControl.prototype.setText = function (text) {
            this._elText.textContent = text;
            this.$self().append(this._elText);
        };
        CCheckBoxGroupControl.prototype._setCheckBox = function (action) {
            var ctrl = new CCheckBoxControl(action);
            ctrl.connect(CCheckBoxControl.ON_SELECTED, this, 'onSelected');
            this.setOwnedChildControls([ctrl]);
        };
        CCheckBoxGroupControl.prototype._doDrawCommon = function (aParent, aRect, aDrawParam) {
            var ret = [];
            var i, len, c, el;
            for (i = 0, len = this._children.length; i < len; i++) {
                c = this._children[i];
                el = c._element;
                el.attributes["data"] = i;
                aParent.appendChild(el);
                c.draw(aRect);
                if (c.isFocusable()) {
                    ret.push(el);
                }
            }
            return ret;
        };
        return CCheckBoxGroupControl;
    })(__CTRL_BASE__.CGroupControl);
    exports.CCheckBoxGroupControl = CCheckBoxGroupControl;
    var CButtonGroupControl = (function (_super) {
        __extends(CButtonGroupControl, _super);
        function CButtonGroupControl(element, title, buttonInfo, horizontal) {
            if (horizontal === void 0) { horizontal = true; }
            _super.call(this, element);
            this._model = [];
            this.onSelected = function () {
                throw 'do implement onSelected event function';
            };
            if (title) {
                this._elTitle = document.createElement('div');
                this._elTitle.classList.add('-title');
                this.setTitle(title);
            }
            element.classList.add('-ctrl-button-group');
            this._setModel(buttonInfo);
            this.setKeyBuilder(horizontal ? 2 /* EHorizontal */ : 1 /* EVertical */);
            this.buildNaviMapForChild(__CTRL_BASE__.KBuilderLeftRight);
        }
        CButtonGroupControl.prototype.setTitle = function (title) {
            this._elTitle.textContent = title;
            this.$self().append(this._elTitle);
        };
        CButtonGroupControl.prototype._setModel = function (model) {
            this._model = model;
            var controls = [];
            for (var i = 0, ctrl; i < model.length; i++) {
                ctrl = new CButtonControl(model[i].text, model[i].fn);
                ctrl.connect(CButtonControl.ON_SELECTED, this, 'onSelected');
                controls.push(ctrl);
            }
            this.setOwnedChildControls(controls);
        };
        CButtonGroupControl.prototype._doDrawCommon = function (aParent, aRect, aDrawParam) {
            var ret = [];
            var i, len, c, el;
            for (i = 0, len = this._children.length; i < len; i++) {
                c = this._children[i];
                el = c._element;
                el.attributes["data"] = i;
                aParent.appendChild(el);
                c.draw(aRect);
                if (c.isFocusable()) {
                    ret.push(el);
                }
            }
            return ret;
        };
        return CButtonGroupControl;
    })(__CTRL_BASE__.CGroupControl);
    exports.CButtonGroupControl = CButtonGroupControl;
    var CModalButtonControl = (function (_super) {
        __extends(CModalButtonControl, _super);
        function CModalButtonControl(element, title, buttonInfo, horizontal) {
            if (horizontal === void 0) { horizontal = true; }
            _super.call(this, element);
            this._model = [];
            this._elTitle = document.createElement('div');
            this.onSelected = function () {
                throw 'do implement onSelected event function';
            };
            this._elTitle.classList.add('-title');
            this.setTitle(title);
            this._setModel(buttonInfo);
            this.setKeyBuilder(horizontal ? 2 /* EHorizontal */ : 1 /* EVertical */);
            this.buildNaviMapForChild(__CTRL_BASE__.KBuilderLeftRight);
            this.draw();
        }
        CModalButtonControl.prototype.setTitle = function (title) {
            this._elTitle.innerHTML = title;
            this.$self().append(this._elTitle);
        };
        CModalButtonControl.prototype._setModel = function (model) {
            this._model = model;
            var controls = [];
            for (var i = 0, ctrl; i < model.length; i++) {
                ctrl = new CButtonControl(model[i].text, model[i].fn);
                ctrl.connect(CButtonControl.ON_SELECTED, this, 'onSelected');
                controls.push(ctrl);
            }
            this.setOwnedChildControls(controls);
        };
        return CModalButtonControl;
    })(CModalDialogControl);
    exports.CModalButtonControl = CModalButtonControl;
});

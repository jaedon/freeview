/**
 * Created by bglee on 2015-02-06.
 */

import __CTRL_BASE__ = require('prismux/controls/ctrlbase');
import CArrowLayout = require("prismux/layout/prism.layout.arrow");
export interface ifSliderModel {
    title: string;
    image: string;
    desc: string;
}
export class CSliderControl extends __CTRL_BASE__.CControl {
    static CLASS_SHOW = "-show";
    static EVENT_REQ_CLOSE = "ReqClose";
    private _pos = 0;
    private _model: any[];
    private _$title = $("<div>", { "class": "-title" });
    private _$slideList = $("<div>", { "class": "-slideList" });
    private _$exit = $("<div>", { "class": "-exit" });
    private _$line = $("<div>", { "class": "-line" });
    private _$step;
    private _$indicator;
    private _arrows = {
        left: null,
        right: null
    };
    constructor(aModel: ifSliderModel[]) {
        super($("<div>", { id: "ctrl-slider" }));
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
            this.$self()
                .append(this._$step)
                .append(this._$indicator);
            for ( var i = 0 ; i < aModel.length ; i++ ) {
                this._$indicator.append($("<div>", { "class": "-button" }));
            }
        }
        for ( var i = 0 ; i < aModel.length ; i++ ) {
            var $container = $("<div>", { "class": "-container" });
            var $img = $("<div>", { "class": "-img" });
            var $desc = $("<div>", { "class": "-desc"});
            $img[0].style.backgroundImage = "url(" + aModel[i].image + ")";
            $desc.html(aModel[i].desc);
            $container.append($img);
            $container.append($desc);

            this._$slideList.append($container);
        }
        this.$self()
            .append(this._$title)
            .append(this._$line)
            .append(this._$slideList)
            .append(this._$exit);
        this._model = aModel;
        this.setSlider(0);
    }
    private _correct(aPos: number): number {
        if (aPos < 0) {
            aPos = 0;
        } else if (aPos >= this._model.length) {
            aPos = this._model.length - 1;
        }
        return aPos;
    }
    private _padZero(origin: string, digit: number): string {
        var zero = "0";
        var ret = origin.toString();
        while ( ret.length < digit ) {
            ret = zero + ret;
        }
        return ret;
    }
    private _changeScean (aPos: number): void {
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
        } else {
            $childs[prevPos].classList.add("-prevOut");
            $childs[aPos].classList.add("-prevIn");
        }

        setTimeout(() => {
            this._$step.removeClass("-fadeIn");
            this._$title.removeClass("-fadeIn");
            if (aPos > prevPos) {
                $childs[prevPos].classList.remove("-nextOut");
                $childs[aPos].classList.remove("-nextIn");
            } else {
                $childs[prevPos].classList.remove("-prevOut");
                $childs[aPos].classList.remove("-prevIn");
            }
        }, 400);

    }
    setExitIcon(aSet: boolean) {
        if (aSet) {
            this._$exit.addClass(CSliderControl.CLASS_SHOW);
            this._$exit.addClass("-fadeIn");
        } else {
            this._$exit.removeClass(CSliderControl.CLASS_SHOW);
            this._$exit.removeClass("-fadeIn");
        }
    }
    setStep(aPos: number) {
        if (this._$step) {
            var step = this._padZero((aPos + 1).toString(), 2);
            this._$step.text(step);
        }
    }
    setIndicator(aPos: number) {
        if (this._$indicator) {
            var $childs = this._$indicator.children();
            $childs[this._pos].classList.remove('-on');
            $childs[aPos].classList.add('-on');
        }
    }
    setArrow(aPos: number) {
        if (this._model.length < 2) {
            return;
        }
        if (aPos === 0) {
            this._arrows.left.hide();
            this._arrows.right.show();
        } else if (aPos === this._model.length - 1) {
            this._arrows.right.hide();
            this._arrows.left.show();
        } else {
            this._arrows.right.show();
            this._arrows.left.show();
        }
    }
    setSlider(aPos: number) {
        var pos = this._correct(aPos);
        var model = this._model;
        this._$title.text(model[pos].title);
        this.setArrow(pos);
        this.setIndicator(pos);
        this.setStep(pos);
        this._changeScean(pos);
        this._pos = pos;
        this.setExitIcon(pos === model.length - 1);
    }
    _doKeyLeft() {
        this.setSlider(this._pos - 1);
        return this._pos < this._model.length;
    }
    _doKeyRight() {
        this.setSlider(this._pos + 1);
        return this._pos < this._model.length - 1;
    }
    _doKeyBack() {
        var posPrev = this._pos;
        this._doKeyLeft();
        return posPrev !== this._pos;
    }
    _doKeyEscape() {
        return this._pos !== this._model.length - 1;
    }
    _doKeyOk() {
        if (this._pos === this._model.length - 1) {
            this.emit.call(this, CSliderControl.EVENT_REQ_CLOSE);
        }
        return true;
    }
    getPosition() {
        return this._pos;
    }
    connectWhenReqClosed(aHolder: any, aSlotName: string, aSlot: () => void) {
        var self = this;
        this.connect(CSliderControl.EVENT_REQ_CLOSE, aHolder, aSlotName);
        this.connect(CSliderControl.EVENT_REQ_CLOSE, {
            disconnectWhenReqClosed: function() {
                self.disconnect(aHolder);
                self.disconnect(this);
            }
        }, 'disconnectWhenReqClosed');
    }
}
export class CModalDialogControl extends  __CTRL_BASE__.CGroupControl {
    private _keyFlooding = ['Back', 'Escape', 'Power'];
    constructor(element: HTMLElement) {
        super(element);
    }
    doKey(aKeyStr: string, aArgs?: any) {
        super.doKey(aKeyStr, aArgs);
        return this._keyFlooding.indexOf(aKeyStr) === -1;
    }
}
export class CModalDialogSildeControl extends CModalDialogControl {
    private _slider: CSliderControl;
    constructor(aParent: HTMLElement, aID: string, aModel: ifSliderModel[], autoClose = 0) {
        super(document.createElement("div"));
        this.setId(aID);
        this._slider = new CSliderControl(aModel);
        this.setOwnedChildControls([this._slider]);
        aParent.appendChild(this._element);
        if (autoClose > 0) {
            setTimeout(() => {
                this._slider.emit(CSliderControl.EVENT_REQ_CLOSE);
            }, autoClose * 1000)
        }
    }
    connectWhenReqClosed(aHolder: any, aSlotName: string, aSlot: () => void) {
        this._slider.connectWhenReqClosed(aHolder, aSlotName, aSlot);
    }
}
export interface ifDialogCheckBoxInfo {
    text: string;
    fn: (checked: boolean) => void;
}
export interface ifDialogSelectiveButtonInfo {
    text: string;
    fn: () => void;
    el?: HTMLElement
}
export class CCheckBoxControl extends __CTRL_BASE__.CControl {
    static ON_SELECTED = 'OnSelected';
    private CHECKED: string = '-checked'
    private _checked: boolean;
    private _fnAction = null;
    constructor(action) {
        super($(document.createElement('div')));
        this.addClass('-ctrl-checkbox');
        this._checked = true;
        this.$self().addClass(this.CHECKED);
        this.setAction(action);
        this.registerSignal([CCheckBoxControl.ON_SELECTED]);
    }
    draw() {}
    doSelect() {
        if (this._checked) {
            this.$self().removeClass(this.CHECKED);
        } else {
            this.$self().addClass(this.CHECKED);
        }
        this._checked = !this._checked;
        this.emit(CButtonControl.ON_SELECTED, this._fnAction(this._checked));
        return false;
    }
    setAction(fn) {
        this._fnAction = fn;
    }
}
export class CButtonControl extends __CTRL_BASE__.CControl {
    static ON_SELECTED = 'OnSelected';
    private _fnAction = null;
    constructor(text, action) {
        super($(document.createElement('div')));
        this.setText(text);
        this.setAction(action);
        this.registerSignal([CButtonControl.ON_SELECTED]);
    }
    draw() {
        /* author: bglee@humaxdigital.com
         * date: 2015-03-11 오전 10:39
         * comments:
            - leaf control does not need to management focusables
         */
    }
    doSelect() {
        this.emit(CButtonControl.ON_SELECTED, this._fnAction());
        return false;
    }
    setText(text) {
        this.$self().text(text);
    }
    setAction(fn) {
        this._fnAction = fn;
    }
}
export class CCheckBoxGroupControl extends __CTRL_BASE__.CGroupControl {
    private _elText: HTMLElement;
    onSelected: Function = () => {
        throw 'do implement onSelected event function';
    };
    constructor(element: HTMLElement, checkboxInfo: ifDialogCheckBoxInfo) {
        super(element);
        if (checkboxInfo.text) {
            this._elText = document.createElement('div');
            this._elText.classList.add('-text');
            this.setText(checkboxInfo.text);
        }
        element.classList.add('-ctrl-checkbox-group');
        this._setCheckBox(checkboxInfo.fn);
        this.setKeyBuilder(__CTRL_BASE__.TParamOrientation.EVertical);
        this.buildNaviMapForChild(__CTRL_BASE__.KBuilderTopDown);
    }
    setText(text) {
        this._elText.textContent = text;
        this.$self().append(this._elText);
    }
    _setCheckBox(action) {
        var ctrl = new CCheckBoxControl(action);
        ctrl.connect(CCheckBoxControl.ON_SELECTED, this, 'onSelected');
        this.setOwnedChildControls([ctrl]);
    }
    _doDrawCommon(aParent: HTMLElement, aRect: __CTRL_BASE__.TRect, aDrawParam: { [key: string]: any; }) {
        var ret: HTMLElement[] = [];
        var i, len, c: __CTRL_BASE__.CControl, el: HTMLElement;
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
    }
}
export class CButtonGroupControl extends __CTRL_BASE__.CGroupControl {
    private _model: ifDialogSelectiveButtonInfo[] = [];
    private _elTitle: HTMLElement;
    onSelected: Function = () => {
        throw 'do implement onSelected event function';
    };

    constructor(element: HTMLElement, title: string, buttonInfo: ifDialogSelectiveButtonInfo[], horizontal = true) {
        super(element);
        if (title) {
            this._elTitle = document.createElement('div');
            this._elTitle.classList.add('-title');
            this.setTitle(title);
        }
        element.classList.add('-ctrl-button-group');
        this._setModel(buttonInfo);
        this.setKeyBuilder(horizontal ? __CTRL_BASE__.TParamOrientation.EHorizontal: __CTRL_BASE__.TParamOrientation.EVertical);
        this.buildNaviMapForChild(__CTRL_BASE__.KBuilderLeftRight);
    }
    setTitle(title) {
        this._elTitle.textContent = title;
        this.$self().append(this._elTitle);
    }
    _setModel(model: ifDialogSelectiveButtonInfo[]) {
        this._model = model;
        var controls = [];
        for (var i = 0, ctrl;  i < model.length; i++) {
            ctrl = new CButtonControl(model[i].text, model[i].fn);
            ctrl.connect(CButtonControl.ON_SELECTED, this, 'onSelected');
            controls.push(ctrl);
        }
        this.setOwnedChildControls(controls);
    }
    _doDrawCommon(aParent: HTMLElement, aRect: __CTRL_BASE__.TRect, aDrawParam: { [key: string]: any; }) {
        var ret: HTMLElement[] = [];
        var i, len, c: __CTRL_BASE__.CControl, el: HTMLElement;
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
    }
}

export class CModalButtonControl extends CModalDialogControl {
    private _model: ifDialogSelectiveButtonInfo[] = [];
    private _elTitle = document.createElement('div');
    onSelected: Function = () => {
        throw 'do implement onSelected event function';
    };

    constructor(element: HTMLElement, title: string, buttonInfo: ifDialogSelectiveButtonInfo[], horizontal = true) {
        super(element);
        this._elTitle.classList.add('-title');
        this.setTitle(title);
        this._setModel(buttonInfo);
        this.setKeyBuilder(horizontal ? __CTRL_BASE__.TParamOrientation.EHorizontal: __CTRL_BASE__.TParamOrientation.EVertical);
        this.buildNaviMapForChild(__CTRL_BASE__.KBuilderLeftRight);
        this.draw();
    }
    setTitle(title) {
        this._elTitle.innerHTML = title;
        this.$self().append(this._elTitle);
    }
    _setModel(model: ifDialogSelectiveButtonInfo[]) {
        this._model = model;
        var controls = [];
        for (var i = 0, ctrl;  i < model.length; i++) {
            ctrl = new CButtonControl(model[i].text, model[i].fn);
            ctrl.connect(CButtonControl.ON_SELECTED, this, 'onSelected');
            controls.push(ctrl);
        }
        this.setOwnedChildControls(controls);
    }
}
///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cContextMenu.js
/*jslint sloppy:true, nomen:true, vars:true */

declare var hx: ifSugar;
import __COMPONENT_KERNEL__ = require("pages/component_kernel");
export class ContextMenu extends __COMPONENT_KERNEL__ {
    _itemHeight : number = 50;
    _zIndex : number = 1;
    _indexOfAllItem : number = 0;
    _list_MaxItem : number = 0;
    _list_StartIdx : number = 0;
    _list_EndIdx : number = 0;
    items : any;
    _nativeFocus : boolean ;
    $ : JQuery;
    renderTo : any;
    compose : any;
    param : any;
    _addItem : any;
    _$items : any;
    setItemActive : any;
    startIndex : number;
    relation : any;
    direction : any;
    getActiveItemData : any;
    _clear : any;
    relationSpace : any;
    toEl : any;
    onKeyUpdated : any;
    _activeIndex : number;
    onCancel : any;
    onOk : any;
    hideOption : any;
    _nav_up : any;
    _nav_down : any;

    constructor() {
        super();
    }
    init () : void {
    }
    create (aParam : any) : void {
        this.items = [];
        this._nativeFocus = false;
        //variable
        $.extend(this, aParam);

        this.$ = $(this.getTemplateEl(this));

        this.update(this.items);
        if (this.renderTo !== null) {
            this.$.appendTo(this.renderTo);
        }

//            hx.contextMenu = this;
//
        if (this.compose) {
            hx.al.compose(this.compose).append(this);
        }
        this.param = aParam;
    }
    add (aArgObject : any) : void {
        var tpl : any = this.getItemTemplateEl(aArgObject);
        this._addItem(tpl, aArgObject);
    }
    changeText (index, argStr) {
        this._$items[index][0].innerHTML = argStr;
    }
    update (aItems : any) : void {
        var i : number, iLen : number;

        this._clear();
        for ( i = 0, iLen = aItems.length; i < iLen; i += 1) {
            var item = aItems[i];
            this.add(item);
        }
        var height = this._itemHeight * iLen;
        var width = (<HTMLElement>this.$[0].childNodes[0]).style.width;
        this.$[0].style.width = width + 'px';
        this.$[0].style.height = height + 'px';
        this.setItemActive(0);
    }

    /**
     * @private
     * @event
     * @param {Object}
     *          - alData {Object}
     *          - alEvent {String}
     *          - alKey {Number}
     *          - alPage {Object}
     *          - alRepeat {Boolean}
     *          - alSeqID {Number}
     *          - alStarter {Number}
     */
    _on_alKey (aParam : any) : boolean {
        hx.log("info", "ContextMenu - _on_alKey(" + aParam.alKey + ")");
        var bConsumed : boolean = true, k : any = hx.key, kc : number = aParam.alKey;

        if (this.onKeyUpdated) {
            this.onKeyUpdated();
        }

        switch(kc) {
            case k.KEY_DOWN:
                this._doDown();
                break;
            case k.KEY_UP:
                this._doUp();
                break;
            case k.KEY_OK:
                this._doOk();
                break;
            case k.KEY_BACK:
            case k.KEY_ESCAPE:
            case k.KEY_OPTION:
                this._doCancel();
                break;
            case k.KEY_POWER:
                bConsumed = false;
                break;
        }
        return bConsumed;
    }
    swapItem (aData : any) : void {
        /*
         var swap = data.swap;
         if (swap !== null) {
         var swapText = swap.text;
         var swapValue = swap.value;

         swap.text = data.text;
         data.text = swapText;
         swap.value = data.value;
         data.value = swapValue;

         that.changeText(that._activeIndex, swapText);
         }*/
        this.changeText(this._activeIndex, aData.text);
    }
    _doOk () : void {
        var data : any = this.getActiveItemData();

        this.swapItem(data);
        this.onOk(this._activeIndex, data);
    }
    _doCancel() : void {
        if (this.onCancel) {
            this.onCancel();
        }
    }
    _doBack () : void {
        this.hideOption();
    }
    _doUp () : void {
        var bMove : boolean = true;
        bMove = this._scroll_pageListUp(this._activeIndex);
        if (bMove === true) {
            this._nav_up();
        }
    }
    _doDown () : void {
        var bMove : boolean = true;
        bMove = this._scroll_pageListDown(this._activeIndex);
        if (bMove === true) {
            this._nav_down();
        }
    }
    destroy () : void {
        this._onHide();
        this._$items = null;
        this._clear();
        this.$.remove();
        this.$ = null;
    }
    _onShow () : void {
        var that = this;
        var top : number = 0, left : number = 0, ulCursor : number = 0;

        if (that.param === null) {
            that.param = {};
        }
        var textAlign : string = 'left';
        // null|undefined
        if (that.param.direction !== null) {
            that.direction = that.param.direction;
        }
        if (that.param.relation !== null) {
            that.relation = that.param.relation;
        }
        if (that.param.relationSpace !== null) {
            that.relationSpace = that.param.relationSpace;
        }
        if (that.param.onOk !== null) {
            that.onOk = that.param.onOk;
        }
        if ((that.param.mode !== undefined) && (that.param.mode === 'scroll')) {
            ulCursor = that._scroll_init();
        }
        if ($.isArray(that.param.items)) {
            if ((that.param.mode !== undefined) && (that.param.mode === 'scroll')) {
                var itemList : any;

                itemList = that._scroll_getItemList();
                that.update(itemList);
            } else {
                that.update(that.param.items);
            }
        }
        if ((that.param.mode !== undefined) && (that.param.mode === 'scroll')) {
            that.startIndex = ulCursor;
        } else {
            that.startIndex = (that.param.startIndex !== undefined) ? that.param.startIndex : 0;
        }

        /*
         if(this.relation == null || this.relation.length == 0){
         throw Error('ContextMenu Relation not Found!!!');
         }*/

        var relationSpace : any = that.relationSpace || 0;
        var relationOffset : any = {
            top : 0,
            left : 0
        };
        var relationOuterHeight : number = 0;
        var relationOuterWidth : number = 0;
        if (that.relation !== undefined) {
            if (that.relation.offset) {
                relationOffset = that.relation.offset();
                relationOuterHeight = that.relation.outerHeight(true);
                relationOuterWidth = that.relation.outerWidth(true);
            } else if ($.isObject(that.relation)) {
                relationOffset = that.relation;
            }

        }
        var cmpOuterWidth : number = that.$.outerWidth(true);
        var cmpOuterHeight : number = that.$.outerHeight(true);
        left = relationOffset.left;

        switch(that.direction) {
            case 'up' :
                top = (relationOffset.top - cmpOuterHeight) - relationSpace;
                break;
            case 'down' :
                //top = ((relationOffset.top - this.relation.outerHeight(true)) + cmpOuterHeight) + relationSpace;
                top = (relationOffset.top + (that.relation ? relationOuterHeight : 0)) + relationSpace;
                break;
            case 'left' :
                top = (relationOffset.top);
                left = (relationOffset.left - cmpOuterWidth) - relationSpace;
                textAlign = 'right';
                break;
            case 'right' :
                top = (relationOffset.top);
                left = (relationOffset.left + (that.relation ? relationOuterWidth : 0)) + relationSpace;
                break;
        }

        that.$.css({
            textAlign : textAlign,
            top : top,
            left : left
        });

        if (that.on) {
            that.on();
        }

        that.setItemActive(that.startIndex);
    }
    _hide () : void {
        var data = this.getActiveItemData();

        //if (data.swap) {
        this.swapItem(data);
        //}
    }
    _onHide () : void {
        var data = this.getActiveItemData();

        //if (data.swap) {
        this.swapItem(data);
        //}
        this.$.removeClass('on');
        this.off();
    }
    getTemplateEl (argObject : any) : any {
        var tpl : string = '<div class="layer wrap_optMenu" style="display:none;z-index:' + this._zIndex + '"><ul class="optMenu" style="position:relative;display:inline-block;"></ul></div>';
        return this.toEl(tpl);
    }
    getItemTemplateEl (argObject : any) : any {
        var tpl : string = '<li class="optMenu-item" style="width:auto;">' + argObject.text + '</li>';
        return this.toEl(tpl);
    }
    _scroll_init () : number {
        var ulEndIdx : number = 0, ulCurosr : number = 0;

        if (this.param.items.length < this.param.column) {
            this._list_StartIdx = 0;
            ulEndIdx = this.param.items.length;
            this._list_MaxItem = ulEndIdx;
            this._list_EndIdx = ulEndIdx;
            ulCurosr = this.param.startIndex;
        } else {
            if ((this.param.items.length - this.param.startIndex) < this.param.column) {
                this._list_MaxItem = this.param.column;
                this._list_EndIdx = this.param.items.length;
                this._list_StartIdx = this._list_EndIdx - this._list_MaxItem;
                ulCurosr = this.param.startIndex - (this.param.items.length - this._list_MaxItem);
            } else {
                this._list_StartIdx = (this.param.startIndex !== undefined) ? this.param.startIndex : 0;
                ulEndIdx = (this.param.column !== undefined) ? this.param.column : 0;
                ulEndIdx = (this.param.items.length > ulEndIdx) ? ulEndIdx : this.param.items.length;
                this._list_MaxItem = ulEndIdx;
                this._list_EndIdx = this._list_StartIdx + ulEndIdx;
                if (this._list_EndIdx === 0) {
                    this._list_EndIdx = this.param.items.length;
                }
                ulCurosr = 0;
            }
        }
        return ulCurosr;
    }
    _scroll_getItemList () : any {
        var itemlist : any = [], i : number = 0, ulIdx : number = 0;

        for ( i = 0; i < this.param.items.length; i += 1) {
            if ((i >= this._list_StartIdx) && (i < this._list_EndIdx)) {
                itemlist[ulIdx] = this.param.items[i];
                ulIdx += 1;
            }
        }
        return itemlist;
    }
    _scroll_pageListUp (index : number) : boolean {
        if ((this.param.mode !== undefined) && (this.param.mode === 'scroll')) {
            var itemList : any, data : any = this.getActiveItemData();

            if ((index === 0) && (this._list_StartIdx !== 0)) {
                this._list_StartIdx = this._list_StartIdx - 1;
                this._list_EndIdx = this._list_StartIdx + this._list_MaxItem;
                itemList = this._scroll_getItemList();
                this.update(itemList);
                this.startIndex = 0;
                this.setItemActive(this.startIndex);
                return false;
            }
            if (hx.config.useAlphabetGroup === 1) {
                if ((this.param.items.length > this._list_MaxItem) && (index === 0)) {
                    this._list_StartIdx = this.param.items.length - this._list_MaxItem;
                    this._list_EndIdx = this.param.items.length;
                    itemList = this._scroll_getItemList();
                    this.update(itemList);
                    this.startIndex = this._list_MaxItem - 1;
                    this.setItemActive(this.startIndex);
                    return false;
                }
            }
        }
        return true;
    }
    _scroll_pageListDown(index : number) : boolean {
        if ((this.param.mode !== undefined) && (this.param.mode === 'scroll')) {
            var itemList : any, data : any = this.getActiveItemData();

            if ((index === (this._list_MaxItem - 1)) && (this._list_EndIdx < this.param.items.length)) {
                this._list_StartIdx = this._list_StartIdx + 1;
                this._list_EndIdx = this._list_StartIdx + this._list_MaxItem;
                itemList = this._scroll_getItemList();
                this.update(itemList);
                this.startIndex = this._list_MaxItem - 1;
                this.setItemActive(this.startIndex);
                return false;
            }
            if (hx.config.useAlphabetGroup === 1) {
                if ((this.param.items.length > this._list_MaxItem) && (index === this._list_MaxItem - 1)) {
                    this._list_StartIdx = 0;
                    this._list_EndIdx = this._list_StartIdx + this._list_MaxItem;
                    itemList = this._scroll_getItemList();
                    this.update(itemList);
                    this.startIndex = 0;
                    this.setItemActive(this.startIndex);
                    return false;
                }
            }
        }
        return true;
    }
}

$.extend(ContextMenu.prototype, hx.ui.uiComponent.template, hx.ui.uiComponent.listable, hx.ui.uiComponent.visibility);

if (!hx.ui) {
    hx.ui = {};
}
hx.ui.ContextMenu = function(args : any) : any {
    return hx.al.component.call(this, 'ContextMenu', args);
};

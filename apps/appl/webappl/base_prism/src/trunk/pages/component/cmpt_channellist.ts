///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cChannelList.js
/*jslint sloppy:true, nomen:true, vars:true*/
import __COMPONENT_KERNEL__ = require("pages/component_kernel");

declare var hx;

export class ChannelList extends __COMPONENT_KERNEL__ {
    channelIndex : number = 0;
    checkedCount : number = 0;
    totalCount : number = 0;
    listIndex : number = 0;
    mode : string = '';
    backupItems : any  = [];
    backupItemArr : any  = [];
    addList : any  = [];
    item : any  = {};
    group : string = undefined;
    _tvList : any  = {
        group : '',
        chList : []
    };
    _radioList : any  = {
        group : '',
        chList : []
    };
    _hdtvList : any  = {
        group : '',
        chList : []
    };
    _favList1 : any  = {};
    _favList2 : any  = {};
    _favList3 : any  = {};
    _favList4 : any  = {};
    _favList5 : any  = {};
    _favList1Backup : any  = [];
    _favList2Backup : any  = [];
    _favList3Backup : any  = [];
    _favList4Backup : any  = [];
    _favList5Backup : any  = [];
    favList1Name : string = '';
    favList2Name : string = '';
    favList3Name : string = '';
    favList4Name : string = '';
    favList5Name : string = '';
    favList1NameBackup : string = '';
    favList2NameBackup : string = '';
    favList3NameBackup : string = '';
    favList4NameBackup : string = '';
    favList5NameBackup : string = '';
    favChannelIndex : number = 0;
    isChange = false;
    originalItems : any  = [];
    originalIdx : number = 0;

    parent : any ;
    _pageRowCount : number;
    _$items : any ;
    _keyTable : any ;
    originalFocus : number;
    selectObjName : string;
    selectObjIndex : number;
    title : string;
    constructor() {
        super();
    }
    create(aParam  : {type: string}) : void {
        $.extend(this, aParam);
        this._pageRowCount = 8;
        this._$items = [];
        this.$ = $(this.getTemplateEl(this));
        this._keyTable = [];
        this._keyTable[hx.key.KEY_UP] = this._doUp;
        this._keyTable[hx.key.KEY_DOWN] = this._doDown;
        this._keyTable[hx.key.KEY_OK] = this._doOk;
        this._keyTable[hx.key.KEY_CHANNEL_UP] = this._doChannelUp;
        this._keyTable[hx.key.KEY_CHANNEL_DOWN] = this._doChannelDown;

        if (aParam.type === "main") {
            this._initMainChannel();
            this.group = this._tvList.group;
            this._setList(this._tvList.chList, 0);
        } else {
            this._initFavChannel();
            this.group = this._favList1.group;
            this._setList(this._favList1.chList, 0);
        }
    }
    _initMainChannel () : void {
        var favList : any, favGroup : string;

        if(this.parent.$favChannelList.group) {
            favGroup = this.parent.$favChannelList.group;
        } else {
            favGroup = 'Favourite1';
        }

        switch(favGroup) {
            case 'Favourite1':
                favList = this.parent.$favChannelList._favList1;
                break;
            case 'Favourite2':
                favList = this.parent.$favChannelList._favList2;
                break;
            case 'Favourite3':
                favList = this.parent.$favChannelList._favList3;
                break;
            case 'Favourite4':
                favList = this.parent.$favChannelList._favList4;
                break;
            case 'Favourite5':
                favList = this.parent.$favChannelList._favList5;
                break;
            default:
                favList = this.parent.$favChannelList._favList1;
        }

        //TV List
        this._tvList = this._backupList(hx.svc.CH_LIST.getChlist({
            group : 'TV'
        }));
        this._chSelectionNotInFav(this._tvList.chList, favList.chList, 'tv');

        //that._radioList.group = 'RADIO';
        /*
         that._radioList = that._backupList(hx.svc.CH_LIST.getChlist({
         group : 'RADIO'
         }));

         that._hdtvList = that._backupList(hx.svc.CH_LIST.getChlist({
         group : 'HDTV'
         }));
         */
    }
    _initFavChannel () : void {
        var chList : any;

        //get favourite List
        this._favList1 = this._backupList(hx.svc.CH_LIST.getChlist({
            group : 'Favourite1'
        }));
        chList = this._favList1.chList;
        this._favList1Backup = chList.slice(0, chList.length);


        this._favList2 = this._backupList(hx.svc.CH_LIST.getChlist({
            group : 'Favourite2'
        }));
        chList = this._favList2.chList;
        this._favList2Backup = chList.slice(0, chList.length);

        this._favList3 = this._backupList(hx.svc.CH_LIST.getChlist({
            group : 'Favourite3'
        }));
        chList = this._favList3.chList;
        this._favList3Backup = chList.slice(0, chList.length);

        this._favList4 = this._backupList(hx.svc.CH_LIST.getChlist({
            group : 'Favourite4'
        }));
        chList = this._favList4.chList;
        this._favList4Backup = chList.slice(0, chList.length);

        this._favList5 = this._backupList(hx.svc.CH_LIST.getChlist({
            group : 'Favourite5'
        }));
        chList = this._favList5.chList;
        this._favList5Backup = chList.slice(0, chList.length);

        //get favourite Name
        this.favList1Name = hx.svc.CH_LIST.getFavListName({
            favtype : 'Favourite1'
        });
        this.favList2Name = hx.svc.CH_LIST.getFavListName({
            favtype : 'Favourite2'
        });
        this.favList3Name = hx.svc.CH_LIST.getFavListName({
            favtype : 'Favourite3'
        });
        this.favList4Name = hx.svc.CH_LIST.getFavListName({
            favtype : 'Favourite4'
        });
        this.favList5Name = hx.svc.CH_LIST.getFavListName({
            favtype : 'Favourite5'
        });
        this.favList1NameBackup = this.favList1Name;
        this.favList2NameBackup = this.favList2Name;
        this.favList3NameBackup = this.favList3Name;
        this.favList4NameBackup = this.favList4Name;
        this.favList5NameBackup = this.favList5Name;

        this.parent._subOptList[2].sub_menu[0].name = this.favList1Name;
        this.parent._subOptList[2].sub_menu[1].name = this.favList2Name;
        this.parent._subOptList[2].sub_menu[2].name = this.favList3Name;
        this.parent._subOptList[2].sub_menu[3].name = this.favList4Name;
        this.parent._subOptList[2].sub_menu[4].name = this.favList5Name;
    }

    _chSelectionNotInFav(aList : any, aFavlist : any, aType : string) : void {
        var i : number, j : number, isFav : boolean;
        for(i = 0; i < aFavlist.length; i+=1) {
            isFav = false;
            for(j = 0; j < aList.length; j+=1) {
                if(aFavlist[i].ccid === aList[j].ccid) {
                    isFav = true;
                    break;
                }
            }
            if(isFav) {
                if (aType === 'tv') {
                    this._tvList.chList.splice(j, 1);
                } else if (aType === 'radio') {
                    this._radioList.chList.splice(j, 1);
                } else if (aType === 'hdtv') {
                    this._hdtvList.chList.splice(j, 1);
                } else if (aType === 'parentTv') {
                    this.parent.$mainChannelList._tvList.chList.splice(j, 1);
                } else if (aType === 'parentRadio') {
                    this.parent.$mainChannelList._radioList.chList.splice(j, 1);
                } else if (aType === 'parentHdtv') {
                    this.parent.$mainChannelList._hdtvList.chList.splice(j, 1);
                }
            }
        }
    }
    _backupList (aList : any) : any {
        var backupList : any = {}, arr : any = [];
        var i : number, len : number;
        backupList.group = aList.group;
        for ( i = 0, len = aList.chList.length; i < len; i += 1) {
            var item = aList.chList[i];
            arr.push({
                'index' : i,
                'sid' : item.sid,
                'className' : '',
                'ccid' : item.ccid,
                'majorChannel' : item.majorChannel,
                'name' : item.name
            });
        }
        backupList.chList = arr;
        item = undefined;
        arr = undefined;
        return backupList;
    }
    _setList (aList : any, aStartIndex : number) : void {//draw function
        var ul : any = this.$[0].querySelector("ul");
        var liList : number = 0;
        this.totalCount = aList.length;
        this._$items = [];
        ul.innerHTML = "";
        var i : number;
        for ( i = 0; i < this.totalCount; i += 1) {
            var item : any = aList[i];
            this._$items.push({
                'index' : i,
                'sid' : item.sid,
                'className' : '',
                'ccid' : item.ccid,
                'majorChannel' : item.majorChannel,
                'name' : item.name,
                'grp' : item.grp,
                'mov' : item.mov
            });
        }

        for ( i = 0; i < this.totalCount; i += 1) {
            if (this.totalCount >= 8) {
                if (i >= aStartIndex && i < aStartIndex + 8) {
                    ul.appendChild(this.getListTemplateEl(this._$items[i], i));
                }
            } else {
                ul.appendChild(this.getListTemplateEl(this._$items[i], i));
            }
        }

        liList = ul.querySelectorAll('li').length;
        if (aStartIndex + 8 > aList.length && this.totalCount >= this._pageRowCount) {
            for ( i = 0; i < (aStartIndex + 8) - aList.length; i += 1) {
                ul.appendChild(this.getListTemplateEl(this._$items[i], i));
                liList = ul.querySelectorAll('li').length;
            }
        }
        this._setTitle();
        this._setInfo();
    }
    _setTitle() : void {
        var title : string = '';

        switch(this.group) {
            case 'Favourite1':
                title = this.favList1Name;
                break;
            case 'Favourite2':
                title = this.favList2Name;
                break;
            case 'Favourite3':
                title = this.favList3Name;
                break;
            case 'Favourite4':
                title = this.favList4Name;
                break;
            case 'Favourite5':
                title = this.favList5Name;
                break;
            case 'TV':
                title = hx.l('LOC_TV_ID');
                break;
            case 'RADIO':
                title = hx.l('LOC_RADIO_ID');
                break;
            case 'HDTV':
                title = hx.l('LOC_HDTV_ID');
                break;
            default:
                title = this.group;
        }
        (<HTMLElement>this.$[0].querySelector('h1')).innerHTML = title;
    }
    _setInfo () {
        (<HTMLElement>this.$[0].querySelector('p')).innerHTML = this.checkedCount + '/' + this.totalCount;
    }
    setFocus (aIndex : number) : void {
        var liList : any = this.$[0].querySelectorAll('li'),
            focusObj : any = liList[aIndex];
        var focusedObj : any = this.$[0].querySelector('.on');
        if(focusedObj) {
            focusedObj.classList.remove('on');
        }

        if (focusObj) {
            focusObj.classList.add('on');
        } else if (liList.length > 0) {
            liList[0].classList.add('on');
        }
    }
    _setIdFocus (aId : any) : void {
        var focusObj : any = this.$[0].querySelector('ul').querySelector('[id = "' + (aId) + '"]');
        if (focusObj) {
            focusObj.classList.add('on');
        }
    }
    _on_alKey (aParam : {alKey: number; alData: any} ) : boolean {
        hx.log("info", "ChannelList(Comp) - _on_alKey(" + aParam.alKey + ")");
        var fx : any = this._keyTable[aParam.alKey];
        var bConsumed : boolean = false;
        if (fx) {
            fx.call(this, aParam);
            if (aParam.alData === undefined) {
                aParam.alData = {};
            }
            aParam.alData.me = this;
            this.sendEventToParent(aParam);
            bConsumed = true;
        }
        return bConsumed;
    }
    _doUp () {
        var focusIndex : number;
        if (this.mode === 'moveItem' || this.mode === 'moveGroup') {
            this.doMoveItem("up");
        } else {
            var focusObj : any = this.$[0].querySelector('.on');
            if (focusObj) {
                focusIndex = this._findIdx(focusObj, this._$items);
                var liList : any = this.$[0].querySelectorAll('li');
                var ul : any = this.$[0].querySelector('ul');
                var nextFocusIndex : number = focusIndex - 1 < 0 ? this.totalCount - 1 : focusIndex - 1;
                if (liList[0] === focusObj && this.totalCount > 7) {
                    ul.removeChild(liList[this._pageRowCount - 1]);
                    ul.insertBefore(this.getListTemplateEl(this._$items[nextFocusIndex], nextFocusIndex), liList[0]);
                }

                var nextObj : any = focusObj.previousSibling;
                if (nextObj) {
                    focusObj.classList.remove('on');
                    nextObj.classList.add('on');
                }
            }
        }
        return false;
    }

    _doDown() : boolean {
        var focusIndex : number;
        if (this.mode === 'moveItem' || this.mode === 'moveGroup') {
            this.doMoveItem("down");
        } else {
            var focusObj : any = this.$[0].querySelector('.on');
            if (focusObj) {
                focusIndex = this._findIdx(focusObj, this._$items);
                var liList : any = this.$[0].querySelectorAll('li');
                var ul : any = this.$[0].querySelector('ul');
                var nextFocusIndex : number = focusIndex + 1 >= this.totalCount ? 0 : focusIndex + 1;
                if (liList[this._pageRowCount - 1] === focusObj) {
                    ul.removeChild(liList[0]);
                    ul.appendChild(this.getListTemplateEl(this._$items[nextFocusIndex], nextFocusIndex));
                }

                var nextObj : any = focusObj.nextSibling;
                if (nextObj) {
                    focusObj.classList.remove('on');
                    nextObj.classList.add('on');
                }
            }
        }
        return false;
    }
    _doChannelUp () : any {
        var i : number, firstIndex : number, focusIndex : number, ulFocusIndex : number, newObj : any;
        var liList : any, ul : any;
        if (this.mode === 'moveItem' || this.mode === 'moveGroup') {
            for(i = 0; i < 8; i+=1) {
                var moveObj = this.$[0].querySelector('.mov');
                if(!moveObj) {
                    return;
                }
                var moveObjIndex : number = this._findIdx(moveObj, this.backupItems),
                    nextObj : any, nextFocusIndex : number;
                liList = this.$[0].querySelectorAll('li');
                ul = this.$[0].querySelector('ul');

                if (liList.length > 7 && liList[0] === moveObj) {// When first node
                    focusIndex = this._findIdx(liList[1], this.backupItems);
                    nextFocusIndex = focusIndex - 1 < 0 ? this.backupItems.length - 1  : focusIndex - 1;
                    if(nextFocusIndex === moveObjIndex) {
                        nextFocusIndex = nextFocusIndex - 1 < 0 ? this.backupItems.length - 1  : nextFocusIndex - 1;
                    }

                    if (this.backupItems.length > 8) {
                        ul.removeChild(ul.lastChild);
                        ul.removeChild(moveObj);
                        ul.insertBefore(this.getListTemplateEl(this.backupItems[nextFocusIndex], nextFocusIndex), ul.firstChild);
                        ul.insertBefore(moveObj, ul.firstChild);
                    } else {
                        ul.removeChild(moveObj);
                        ul.appendChild(moveObj);
                    }
                } else {
                    var previousObj : any = moveObj.previousSibling;
                    if (previousObj) {
                        ul.removeChild(moveObj);
                        ul.insertBefore(moveObj, previousObj);
                    }
                }
            }
        } else {
            if(this._$items.length > 8) {
                var focusObj : any = this.$[0].querySelector('.on');
                if (focusObj) {
                    liList = this.$[0].querySelectorAll('li');
                    ul = this.$[0].querySelector('ul');

                    for(i = 0; i < ul.childNodes.length; i+=1) {
                        if(ul.childNodes[i].className === 'on') {
                            break;
                        }
                    }

                    firstIndex = this._findIdx(ul.childNodes[0], this._$items);
                    focusIndex = this._findIdx(focusObj, this._$items);
                    ulFocusIndex = focusIndex - firstIndex;
                    if(ulFocusIndex < 0) {
                        ulFocusIndex = this._$items.length - firstIndex;
                    }

                    if(firstIndex - 8 >= 0) {
                        for(i = 0; i < 8; i+=1) {
                            ul.removeChild(ul.lastChild);
                        }

                        for(i = 0; i < 8; i+=1) {
                            newObj = document.createElement( "li");
                            newObj.classList.remove('sel');
                            newObj.innerHTML = this._$items[firstIndex - 8 + i].majorChannel + ' ' + this._$items[firstIndex - 8 + i].name;
                            if(this._$items[firstIndex - 8 + i].checked) {
                                newObj.classList.add('sel');
                            }
                            ul.appendChild(newObj);
                        }
                        focusObj.classList.remove('on');
                        ul.childNodes[ulFocusIndex].classList.add('on');
                    } else {
                        for(i = 0; i < 8; i+=1) {
                            ul.removeChild(ul.lastChild);
                        }

                        for(i = (this._$items.length + firstIndex - 8); i < this._$items.length; i+=1) {
                            newObj = document.createElement( "li");
                            newObj.classList.remove('sel');
                            newObj.innerHTML = this._$items[i].majorChannel + ' ' + this._$items[i].name;
                            if(this._$items[i].checked) {
                                newObj.classList.add('sel');
                            }
                            ul.appendChild(newObj);
                        }

                        for(i = 0; i < firstIndex; i+=1) {
                            newObj = document.createElement( "li");
                            newObj.classList.remove('sel');
                            newObj.innerHTML = this._$items[i].majorChannel + ' ' + this._$items[i].name;
                            if(this._$items[i].checked) {
                                newObj.classList.add('sel');
                            }
                            ul.appendChild(newObj);
                        }
                        focusObj.classList.remove('on');
                        ul.childNodes[ulFocusIndex].classList.add('on');
                    }
                }
            }
        }
        return false;
    }
    _doChannelDown () : any {
        var i : number, firstIndex : number, focusIndex : number, ulFocusIndex : number, newObj : any;
        var liList : any, ul : any;
        if (this.mode === 'moveItem' || this.mode === 'moveGroup') {
            //that.doMoveItem("down");
            for(i = 0; i < 8; i+=1) {
                var moveObj = this.$[0].querySelector('.mov');
                if(!moveObj) {
                    return;
                }
                var moveObjIndex : number = this._findIdx(moveObj, this.backupItems),
                    nextObj : any, nextFocusIndex : number;
                liList = this.$[0].querySelectorAll('li');
                ul = this.$[0].querySelector('ul');

                if (liList[this._pageRowCount - 1] === moveObj) {// When last node
                    focusIndex = this._findIdx(liList[this._pageRowCount - 2], this.backupItems);
                    nextFocusIndex = focusIndex + 1 >= this.backupItems.length ? 0 : focusIndex + 1;
                    if(nextFocusIndex === moveObjIndex) {
                        nextFocusIndex = nextFocusIndex + 1 >= this.backupItems.length ? 0 : nextFocusIndex + 1;
                    }

                    if (this.backupItems.length > 8) {
                        ul.removeChild(liList[0]);
                        ul.appendChild(this.getListTemplateEl(this.backupItems[nextFocusIndex], nextFocusIndex));
                        ul.appendChild(moveObj);
                    } else {
                        ul.removeChild(moveObj);
                        ul.insertBefore(moveObj, ul.firstChild);
                    }
                } else {
                    nextObj = moveObj.nextSibling;
                    if (nextObj) {
                        ul.removeChild(moveObj);
                        ul.insertBefore(moveObj, nextObj.nextSibling);
                    }
                }
            }
        } else {
            if(this._$items.length > 8) {
                var focusObj : any  = this.$[0].querySelector('.on');
                if (focusObj) {
                    liList = this.$[0].querySelectorAll('li');
                    ul = this.$[0].querySelector('ul');

                    for(i = 0; i < ul.childNodes.length; i+=1) {
                        if(ul.childNodes[i].className === 'on') {
                            break;
                        }
                    }
                    firstIndex = this._findIdx(ul.childNodes[0], this._$items);
                    focusIndex = this._findIdx(focusObj, this._$items);
                    ulFocusIndex = focusIndex - firstIndex;
                    if(ulFocusIndex < 0) {
                        ulFocusIndex = this._$items.length - firstIndex;
                    }
                    if(firstIndex + 8 + 8 <= this._$items.length - 1) {
                        for(i = 0; i < 8; i+=1) {
                            ul.removeChild(ul.lastChild);
                        }

                        for(i = 0; i < 8; i+=1) {
                            //if(that._$items[firstIndex + 8 + i]) {
                            newObj = document.createElement( "li");
                            newObj.classList.remove('sel');
                            newObj.innerHTML = this._$items[firstIndex + 8 + i].majorChannel + ' ' + this._$items[firstIndex + 8 + i].name;
                            if(this._$items[firstIndex + 8 + i].checked) {
                                newObj.classList.add('sel');
                            }
                            ul.appendChild(newObj);
                            //}
                        }
                        focusObj.classList.remove('on');
                        ul.childNodes[ulFocusIndex].classList.add('on');
                    } else {
                        for(i = 0; i < 8; i+=1) {
                            ul.removeChild(ul.lastChild);
                        }

                        for(i = 0; i < 8; i+=1) {
                            var index = firstIndex + 8 + i;
                            //if(that._$items[index]) {
                            if(index > this._$items.length - 1) {
                                index -= this._$items.length;
                            }
                            if(index >= this._$items.length) { //exception when List Num is 9
                                index -= this._$items.length;
                            }
                            newObj = document.createElement( "li");
                            newObj.classList.remove('sel');
                            newObj.innerHTML = this._$items[index].majorChannel + ' ' + this._$items[index].name;
                            if(this._$items[index].checked) {
                                newObj.classList.add('sel');
                            }
                            ul.appendChild(newObj);
                            //}
                        }
                        focusObj.classList.remove('on');
                        ul.childNodes[ulFocusIndex].classList.add('on');
                    }
                }
            }
        }
        return false;
    }
    doMoveItem (aType : string) : void {
        var moveObj : any = this.$[0].querySelector('.mov');
        if(!moveObj) {
            return;
        }

        var moveObjIndex : number = this._findIdx(moveObj, this.backupItems),
            nextObj : any, focusIndex : number, nextFocusIndex : number,
            liList : any = this.$[0].querySelectorAll('li'),
            ul : any = this.$[0].querySelector('ul');
        if (aType === 'down') {
            if (liList[this._pageRowCount - 1] === moveObj) {// When last node
                focusIndex = this._findIdx(liList[this._pageRowCount - 2], this.backupItems);
                nextFocusIndex = focusIndex + 1 >= this.backupItems.length ? 0 : focusIndex + 1;
                if(nextFocusIndex === moveObjIndex) {
                    nextFocusIndex = nextFocusIndex + 1 >= this.backupItems.length ? 0 : nextFocusIndex + 1;
                }

                if (this.backupItems.length > 8) {
                    ul.removeChild(liList[0]);
                    ul.appendChild(this.getListTemplateEl(this.backupItems[nextFocusIndex], nextFocusIndex));
                    ul.appendChild(moveObj);
                } else {
                    ul.removeChild(moveObj);
                    ul.insertBefore(moveObj, ul.firstChild);
                }
            } else {
                nextObj = moveObj.nextSibling;
                if (nextObj) {
                    ul.removeChild(moveObj);
                    ul.insertBefore(moveObj, nextObj.nextSibling);
                }
            }
        } else if (aType === 'up') {
            if (liList.length > 7 && liList[0] === moveObj) {// When first node
                focusIndex = this._findIdx(liList[1], this.backupItems);
                nextFocusIndex = focusIndex - 1 < 0 ? this.backupItems.length - 1  : focusIndex - 1;
                if(nextFocusIndex === moveObjIndex) {
                    nextFocusIndex = nextFocusIndex - 1 < 0 ? this.backupItems.length - 1  : nextFocusIndex - 1;
                }

                if (this.backupItems.length > 8) {
                    ul.removeChild(ul.lastChild);
                    ul.removeChild(moveObj);
                    ul.insertBefore(this.getListTemplateEl(this.backupItems[nextFocusIndex], nextFocusIndex), ul.firstChild);
                    ul.insertBefore(moveObj, ul.firstChild);
                } else {
                    ul.removeChild(moveObj);
                    ul.appendChild(moveObj);
                }
            } else {
                var previousObj : any = moveObj.previousSibling;
                if (previousObj) {
                    ul.removeChild(moveObj);
                    ul.insertBefore(moveObj, previousObj);
                }
            }
        }
    }
    reload (aTargetList : any, aStartIdx : number) : void {
        this.checkedCount = 0;
        this._setList(aTargetList, aStartIdx);   //change _$item and redraw
    }
    changeGroup (aChannelType : string) : boolean {
        var favGroup : string, favList : any;

        if(this.parent.$favChannelList.group) {
            favGroup = this.parent.$favChannelList.group;
        } else {
            favGroup = 'Favourite1';
        }

        switch(favGroup) {
            case 'Favourite1':
                favList = this.parent.$favChannelList._favList1;
                break;
            case 'Favourite2':
                favList = this.parent.$favChannelList._favList2;
                break;
            case 'Favourite3':
                favList = this.parent.$favChannelList._favList3;
                break;
            case 'Favourite4':
                favList = this.parent.$favChannelList._favList4;
                break;
            case 'Favourite5':
                favList = this.parent.$favChannelList._favList5;
                break;
            default:
                favList = this.parent.$favChannelList._favList1;
        }

        switch(aChannelType) {
            case hx.l('LOC_TV_ID') :
                this._tvList = this._backupList(hx.svc.CH_LIST.getChlist({
                    group : 'TV'
                }));
                this.group = this._tvList.group;
                this._chSelectionNotInFav(this._tvList.chList, favList.chList, 'tv');
                this._setList(this._tvList.chList, 0);
                break;
            case hx.l('LOC_RADIO_ID') :
                this._radioList = this._backupList(hx.svc.CH_LIST.getChlist({
                    group : 'RADIO'
                }));
                this.group = this._radioList.group;
                this._chSelectionNotInFav(this._radioList.chList, favList.chList, 'radio');
                this._setList(this._radioList.chList, 0);
                break;
            case hx.l('LOC_HDTV_ID') :
                this._hdtvList = this._backupList(hx.svc.CH_LIST.getChlist({
                    group : 'HDTV'
                }));
                this.group = this._hdtvList.group;
                this._chSelectionNotInFav(this._hdtvList.chList, favList.chList, 'hdtv');
                this._setList(this._hdtvList.chList, 0);
                break;
        }
        this.checkedCount = 0;
        this._setInfo();
        this.setFocus(0);
        this.parent._optBack(this);
        return false;
    }
    changeFavourite (aChannelType : string) : boolean {
        var favList : any;

        switch(aChannelType) {
            case this.favList1Name :
                this.group = this._favList1.group;
                favList = this._favList1;
                this._setList(this._favList1.chList, 0);
                break;
            case this.favList2Name :
                this.group = this._favList2.group;
                favList = this._favList2;
                this._setList(this._favList2.chList, 0);
                break;
            case this.favList3Name :
                this.group = this._favList3.group;
                favList = this._favList3;
                this._setList(this._favList3.chList, 0);
                break;
            case this.favList4Name :
                this.group = this._favList4.group;
                favList = this._favList4;
                this._setList(this._favList4.chList, 0);
                break;
            case this.favList5Name :
                this.group = this._favList5.group;
                favList = this._favList5;
                this._setList(this._favList5.chList, 0);
                break;
        }

        var parentMainChlist : any = this.parent.$mainChannelList;     //update MainList because favList is changed
        switch(parentMainChlist.group) {
            case 'TV':
                parentMainChlist._tvList = this._backupList(hx.svc.CH_LIST.getChlist({
                    group : 'TV'
                }));
                this._chSelectionNotInFav(parentMainChlist._tvList.chList, favList.chList, 'parentTv');
                parentMainChlist._setList(parentMainChlist._tvList.chList, 0);
                break;
            case 'RADIO':
                parentMainChlist._radioList = this._backupList(hx.svc.CH_LIST.getChlist({
                    group : 'RADIO'
                }));
                this._chSelectionNotInFav(parentMainChlist._radioList.chList, favList.chList, 'parentRadio');
                parentMainChlist._setList(parentMainChlist._radioList.chList, 0);
                break;
            case 'HDTV':
                parentMainChlist._hdtvList = this._backupList(hx.svc.CH_LIST.getChlist({
                    group : 'HDTV'
                }));
                this._chSelectionNotInFav(parentMainChlist._hdtvList.chList, favList.chList, 'parentHdtv');
                parentMainChlist._setList(parentMainChlist._hdtvList.chList, 0);
                break;
        }
        this.checkedCount = 0;
        this._setInfo();
        this._setTitle();
        this.setFocus(0);
        this.parent._optBack(this);
        return false;
    }
    renameFavourite () : void {
        var fn : Function;
        fn =(_status, _input)=> {
            if (_status === 'key_back' || _status === 'key_esc') {
                this.parent._optBack(this);
            } else if (_status === hx.l('LOC_OK_ID')) {
                this.parent._optBack(this);
                if (_input) {
                    if(_input.password !== "") {
                        switch(this.group) {
                            case 'Favourite1':
                                if(_input.password === this.favList2Name || _input.password === this.favList3Name || _input.password === this.favList4Name || _input.password === this.favList5Name || _input.password === 'TV' || _input.password === 'Radio' || _input.password === 'HDTV' || _input.password === 'Free TV' || _input.password === 'Pay TV') {
                                    hx.msg('response', {
                                        text : hx.l('LOC_TYPE_DIFFERENT_NAME_ID'),
                                        auto_close : true,
                                        close_time : 3000
                                    });
                                } else {
                                    this.favList1Name = _input.password;
                                    this.parent._subOptList[2].sub_menu[0].name = this.favList1Name;
                                    this._setTitle();
                                    this.isChange = true;
                                }
                                break;
                            case 'Favourite2':
                                if(_input.password === this.favList1Name || _input.password === this.favList3Name || _input.password === this.favList4Name || _input.password === this.favList5Name || _input.password === 'TV' || _input.password === 'Radio' || _input.password === 'HDTV' || _input.password === 'Free TV' || _input.password === 'Pay TV') {
                                    hx.msg('response', {
                                        text : hx.l('LOC_TYPE_DIFFERENT_NAME_ID'),
                                        auto_close : true,
                                        close_time : 3000
                                    });
                                } else {
                                    this.favList2Name = _input.password;
                                    this.parent._subOptList[2].sub_menu[1].name = this.favList2Name;
                                    this._setTitle();
                                    this.isChange = true;
                                }
                                break;
                            case 'Favourite3':
                                if(_input.password === this.favList1Name || _input.password === this.favList2Name || _input.password === this.favList4Name || _input.password === this.favList5Name || _input.password === 'TV' || _input.password === 'Radio' || _input.password === 'HDTV' || _input.password === 'Free TV' || _input.password === 'Pay TV') {
                                    hx.msg('response', {
                                        text : hx.l('LOC_TYPE_DIFFERENT_NAME_ID'),
                                        auto_close : true,
                                        close_time : 3000
                                    });
                                } else {
                                    this.favList3Name = _input.password;
                                    this.parent._subOptList[2].sub_menu[2].name = this.favList3Name;
                                    this._setTitle();
                                    this.isChange = true;
                                }
                                break;
                            case 'Favourite4':
                                if(_input.password === this.favList1Name || _input.password === this.favList2Name || _input.password === this.favList3Name || _input.password === this.favList5Name || _input.password === 'TV' || _input.password === 'Radio' || _input.password === 'HDTV' || _input.password === 'Free TV' || _input.password === 'Pay TV') {
                                    hx.msg('response', {
                                        text : hx.l('LOC_TYPE_DIFFERENT_NAME_ID'),
                                        auto_close : true,
                                        close_time : 3000
                                    });
                                } else {
                                    this.favList4Name = _input.password;
                                    this.parent._subOptList[2].sub_menu[3].name = this.favList4Name;
                                    this._setTitle();
                                    this.isChange = true;
                                }
                                break;
                            case 'Favourite5':
                                if(_input.password === this.favList1Name || _input.password === this.favList2Name || _input.password === this.favList3Name || _input.password === this.favList4Name || _input.password === 'TV' || _input.password === 'Radio' || _input.password === 'HDTV' || _input.password === 'Free TV' || _input.password === 'Pay TV') {
                                    hx.msg('response', {
                                        text : hx.l('LOC_TYPE_DIFFERENT_NAME_ID'),
                                        auto_close : true,
                                        close_time : 3000
                                    });
                                } else {
                                    this.favList5Name = _input.password;
                                    this.parent._subOptList[2].sub_menu[4].name = this.favList5Name;
                                    this._setTitle();
                                    this.isChange = true;
                                }
                                break;
                        }
                    }
                }
            } else if (_status === 'close') {
                this.parent._optBack(this);
            }
        };
        var passwdTmp : any = hx.msg('passwd', {
            text : hx.l('LOC_ENTER_FAVOURITE_LIST_NAME_ID'),
            responseText : hx.l('LOC_TYPE_NAME_PRESS_ENTER_ID'),
            wifi : 'Name', //wifi
            string : hx.l('LOC_LIST_NAME_ID'),
            auto_close : false,
            dia_class : 'osd',
            callback_fn : fn,
            maxLength : 200
        });
    }
    addToFav () : void {
        this.parent.addToFav();
        this.parent._optBack(this);
    }
    selectAll () : void {
        var i : number, len : number;
        var liList : any = this.$[0].querySelectorAll('li');
        if (liList.length > 0) {
            for ( i = 0, len = liList.length; i < len; i += 1) {
                liList[i].classList.add("sel");
            }
        }
        for ( i = 0, len = this._$items.length; i < len; i += 1) {
            this._$items[i].checked = true;
        }
        this.checkedCount = len;
        this._setInfo();
        this.parent._optBack(this);
    }
    deselectAll() : void {
        var i : number, len : number;
        var liList : any = this.$[0].querySelectorAll('li');
        if (liList.length > 0) {
            for ( i = 0, len = liList.length; i < len; i += 1) {
                liList[i].classList.remove("sel");
            }
        }
        for ( i = 0, len = this._$items.length; i < len; i += 1) {
            this._$items[i].checked = false;
        }
        this.checkedCount = 0;
        this._setInfo();
        this.parent._optBack(this);
    }
    removeItem () : void {
        var i : number, len : number = this._$items.length, favList : any;

        if(this.checkedCount === 0 &&  this.totalCount !== 0) {
            for (i = 0; i < len; i +=1) {
                if($('#set #editChannelList').eq(1).children('ul').children('.on')[0].innerHTML === (this._$items[i].majorChannel + ' ' +  this._$items[i].name)) {
                    this.isChange = true;
                    this._$items.splice(i, 1);
                    break;
                }
            }
        } else {
            for ( i = 0; i < len; i += 1) {
                if (this._$items[i].checked) {
                    this.isChange = true;
                    this._$items.splice(i, 1);
                    i -= 1;
                    len -= 1;
                }
            }
        }

        switch(this.group) {
            case "Favourite1" :
                this._favList1.chList = this._$items;
                favList = this._favList1;
                break;
            case "Favourite2" :
                this._favList2.chList = this._$items;
                favList = this._favList2;
                break;
            case "Favourite3" :
                this._favList3.chList = this._$items;
                favList = this._favList3;
                break;
            case "Favourite4" :
                this._favList4.chList = this._$items;
                favList = this._favList4;
                break;
            case "Favourite5" :
                this._favList5.chList = this._$items;
                favList = this._favList5;
                break;
        }

        var parentMainChlist : any = this.parent.$mainChannelList;     //update MainList because favList is changed
        switch(parentMainChlist.group) {
            case 'TV':
                parentMainChlist._tvList = this._backupList(hx.svc.CH_LIST.getChlist({
                    group : 'TV'
                }));
                this._chSelectionNotInFav(parentMainChlist._tvList.chList, favList.chList, 'parentTv');
                parentMainChlist._setList(parentMainChlist._tvList.chList, 0);
                break;
            case 'RADIO':
                parentMainChlist._radioList = this._backupList(hx.svc.CH_LIST.getChlist({
                    group : 'RADIO'
                }));
                this._chSelectionNotInFav(parentMainChlist._radioList.chList, favList.chList, 'parentRadio');
                parentMainChlist._setList(parentMainChlist._radioList.chList, 0);
                break;
            case 'HDTV':
                parentMainChlist._hdtvList = this._backupList(hx.svc.CH_LIST.getChlist({
                    group : 'HDTV'
                }));
                this._chSelectionNotInFav(parentMainChlist._hdtvList.chList, favList.chList, 'parentHdtv');
                parentMainChlist._setList(parentMainChlist._hdtvList.chList, 0);
                break;
        }
        this.reload(this._$items, 0);
        this.setFocus(0);
        this.parent._optBack(this);
    }
    moveOkItem () {
        var focusObj : any = this.$[0].querySelector('.on'),
            moveObj : any, ul : any, liList : any;
        var startIndex : number, length : number, nextIdx : number, nextItemText : string, nextItemIdx : number, i : number, bisLastIdx : boolean;

        if (this.mode === 'moveItem') {
            bisLastIdx = false;
            moveObj = this.$[0].querySelector('.mov');
            ul = this.$[0].querySelector('ul');
            liList = this.$[0].querySelectorAll('li');
            this.item[0]['checked'] = false;
            this.item[0]['mov'] = false;
            this.checkedCount = 0;

            for(i = 0; i < liList.length; i+=1) {
                if(liList[i] === moveObj) {
                    this.originalFocus = i;
                    nextIdx = i + 1;
                    if(nextIdx > liList.length - 1) {
                        bisLastIdx = true;
                        if(this.totalCount > 8) {
                            nextIdx = liList.length - 2;
                        } else {
                            nextIdx = liList.length - 1;
                        }
                    } else if(liList.length === 1) {
                        nextIdx = 0;
                    }
                }
            }
            nextItemText = liList[nextIdx].innerText;
            for(i = 0; i < this.backupItems.length; i+=1) {
                if((this.backupItems[i].majorChannel + ' ' + this.backupItems[i].name) === nextItemText) {
                    nextItemIdx = i;
                    break;
                }
            }

            if(bisLastIdx) {
                if(nextItemIdx === undefined) {
                    nextItemIdx = this.backupItems.length;
                } else {
                    nextItemIdx += 1;
                }

                if(nextItemIdx > this.backupItems.length) {
                    nextItemIdx = 0;
                }
            }

            this.backupItems.splice(nextItemIdx, 0, this.item[0]);
            startIndex = this._findIdx(liList[0], this.backupItems);
            this._setList(this.backupItems, startIndex);
            this.mode = "";
            this.setFocus(this.originalFocus);
            this._adjustList(this.backupItems);
            this.isChange = true;
        } else if (this.mode === 'moveGroup') {
            bisLastIdx = false;
            moveObj = this.$[0].querySelector('.mov');
            ul = this.$[0].querySelector('ul');
            liList = this.$[0].querySelectorAll('li');
            startIndex = this._findIdx(liList[0], this._$items);
            this.checkedCount = 0;
            length = this.backupItemArr.length;
            for(i = 0; i < liList.length; i+=1) {
                if(liList[i] === moveObj) {
                    this.originalFocus = i;
                    nextIdx = i + 1;
                    if(nextIdx > liList.length - 1) {
                        bisLastIdx = true;
                        if(this.totalCount > 8) {
                            nextIdx = liList.length - 2;
                        } else {
                            nextIdx = liList.length - 1;
                        }
                    } else if(liList.length === 1) {
                        nextIdx = 0;
                    }
                }
            }

            nextItemText = liList[nextIdx].innerText;
            ul.removeChild(moveObj);
            for(i = 0; i < this.backupItems.length; i+=1) {
                if((this.backupItems[i].majorChannel + ' ' + this.backupItems[i].name) === nextItemText) {
                    nextItemIdx = i;
                    break;
                }
            }

            if(bisLastIdx) {
                if(nextItemIdx === undefined) {
                    nextItemIdx = this.backupItems.length;
                } else {
                    nextItemIdx += 1;
                }

                if(nextItemIdx > this.backupItems.length) {
                    nextItemIdx = 0;
                }
            }

            while (length > 0) {
                length-=1;
                this.backupItemArr[length].checked = false;
                this.backupItems.splice(nextItemIdx, 0, this.backupItemArr[length]);
            }
            for( i = 0; i < this._$items.length; i+=1) {
                this._$items[i].checked = false;
            }

            this._$items = this.copyArray(this.backupItems);

            for(i = 0; i < this._$items.length; i+=1) {
                if(this.selectObjName === this._$items[i].name) {
                    this.selectObjIndex = i;
                }
            }
            var startObjIndex : number = this.selectObjIndex - this.originalFocus;
            if(startObjIndex < 0) {
                startObjIndex += this._$items.length;
            }
            this._setList(this._$items, startObjIndex);
            this.mode = "";
            this.setFocus(this.originalFocus);
            this._adjustList(this._$items);
            this.isChange = true;
        }
        this.parent._optBack(this);
    }
    moveCancelItem () : void {
        var i : number;

        if (this.item[0]) {
            this.item[0]['mov'] = false;
        }
        for(i = 0; i < this.originalFocus; i+=1) {
            this.originalIdx = this.originalIdx - 1 < 0 ? this.originalItems.length - 1  : this.originalIdx - 1;
        }
        this._setList(this.originalItems, this.originalIdx);
        this.checkedCount = 0;
        this.setFocus(this.originalFocus);
        this._setInfo();
        this.on();
        this.mode = '';
        this.parent._optBack(this);
    }
    _doOk () : boolean {
        var	focusObj : any = this.$[0].querySelector('.on'),
            moveObj : any, ul : any, liList : any;
        var startIndex : number, length : number, nextIdx : number, nextItemText : string, nextItemIdx : number, i : number, bisLastIdx : boolean;

        if (this.mode === 'moveItem') {
            bisLastIdx = false;
            moveObj = this.$[0].querySelector('.mov');
            ul = this.$[0].querySelector('ul');
            liList = this.$[0].querySelectorAll('li');
            //when item top
            this.item[0]['checked'] = false;
            this.item[0]['mov'] = false;
            this.checkedCount = 0;

            for(i = 0; i < liList.length; i+=1) {
                if(liList[i] === moveObj) {
                    this.originalFocus = i;
                    nextIdx = i + 1;
                    if(nextIdx > liList.length - 1) {
                        bisLastIdx = true;
                        if(this.totalCount > 8) {
                            nextIdx = liList.length - 2;
                        } else {
                            nextIdx = liList.length - 1;
                        }
                    } else if(liList.length === 1) {
                        nextIdx = 0;
                    }
                }
            }
            nextItemText = liList[nextIdx].innerText;
            for(i = 0; i < this.backupItems.length; i+=1) {
                if((this.backupItems[i].majorChannel + ' ' + this.backupItems[i].name) === nextItemText) {
                    nextItemIdx = i;
                    break;
                }
            }

            if(bisLastIdx) {
                if(nextItemIdx === undefined) {
                    nextItemIdx = this.backupItems.length;
                } else {
                    nextItemIdx += 1;
                }

                if(nextItemIdx > this.backupItems.length) {
                    nextItemIdx = 0;
                }
            }

            this.backupItems.splice(nextItemIdx, 0, this.item[0]);
            startIndex = this._findIdx(liList[0], this.backupItems);
            this._setList(this.backupItems, startIndex);
            this.mode = "";
            this.setFocus(this.originalFocus);
            this._adjustList(this.backupItems);
            this.isChange = true;
        } else if (this.mode === 'moveGroup') {
            bisLastIdx = false;
            moveObj = this.$[0].querySelector('.mov');
            ul = this.$[0].querySelector('ul');
            liList = this.$[0].querySelectorAll('li');
            startIndex = this._findIdx(liList[0], this._$items);
            this.checkedCount = 0;
            //when item top
            length = this.backupItemArr.length;
            for(i = 0; i < liList.length; i+=1) {
                if(liList[i] === moveObj) {
                    this.originalFocus = i;
                    nextIdx = i + 1;
                    if(nextIdx > liList.length - 1) {
                        bisLastIdx = true;
                        if(this.totalCount > 8) {
                            nextIdx = liList.length - 2;
                        } else {
                            nextIdx = liList.length - 1;
                        }
                    } else if(liList.length === 1) {
                        nextIdx = 0;
                    }
                }
            }

            nextItemText = liList[nextIdx].innerText;
            ul.removeChild(moveObj);
            for(i = 0; i < this.backupItems.length; i+=1) {
                if((this.backupItems[i].majorChannel + ' ' + this.backupItems[i].name) === nextItemText) {
                    nextItemIdx = i;
                    break;
                }
            }

            if(bisLastIdx) {
                if(nextItemIdx === undefined) {
                    nextItemIdx = this.backupItems.length;
                } else {
                    nextItemIdx += 1;
                }

                if(nextItemIdx > this.backupItems.length) {
                    nextItemIdx = 0;
                }
            }

            while (length > 0) {
                length-=1;
                this.backupItemArr[length].checked = false;
                this.backupItems.splice(nextItemIdx, 0, this.backupItemArr[length]);
            }
            for( i = 0; i < this._$items.length; i+=1) {
                this._$items[i].checked = false;
            }

            this._$items = this.copyArray(this.backupItems);

            for(i = 0; i < this._$items.length; i+=1) {
                if(this.selectObjName === this._$items[i].name) {
                    this.selectObjIndex = i;
                }
            }
            var startObjIndex : number = this.selectObjIndex - this.originalFocus;
            if(startObjIndex < 0) {
                startObjIndex += this._$items.length;
            }
            this._setList(this._$items, startObjIndex);
            this.mode = "";
            this.setFocus(this.originalFocus);
            this._adjustList(this._$items);
            this.isChange = true;
            //this._setIdFocus(liList[0].id);
        } else if (focusObj) {
            if (focusObj.classList.toggle("sel")) {
                this._$items[this._findIdx(focusObj, this._$items)].checked = true;
                this.checkedCount+=1;
                this._setInfo();
                this._doDown();
            } else {
                this._$items[this._findIdx(focusObj, this._$items)].checked = false;
                this.checkedCount-=1;
                this._setInfo();
                this._doDown();
            }
        }
        return false;
    }
    moveItem () {
        var opt : any = document.getElementById('opt'), i : number, tempLength : number;
        opt.classList.remove('on');
        this.on();
        if(this.checkedCount <= 0 && this.totalCount <= 0) {  //exception
            return;
        }
        this.originalItems.length = 0;
        this.originalIdx = 0;
        this.originalFocus = 0;
        this.originalItems = this.copyArray(this._$items);
        var focusObj : any = this.$[0].querySelector('.on');
        var liList : any = this.$[0].querySelectorAll('li');
        for(i = 0; i < liList.length; i+=1) {
            if(liList[i] === focusObj) {
                this.originalFocus = i;
                break;
            }
        }
        var focusObjIdx : number = this._findIdx(focusObj, this._$items);
        this.originalIdx = focusObjIdx;

        var selectObj : any, selectObjIndex : number;
        if(this.checkedCount === 0 && this.totalCount !== 0) {
            this.backupItems = this.copyArray(this._$items);
            selectObj = this.$[0].querySelector('.on');
            selectObjIndex = this._findIdx(selectObj, this.backupItems);
            //copy Array..
            this.item = this.backupItems.splice(selectObjIndex, 1);
            selectObj.classList.add('mov');
            selectObj.classList.add('on');
            this.checkedCount += 1;
            this._setInfo();
            this.mode = "moveItem";
        } else if (this.checkedCount === 1) {
            this.mode = "moveItem";
            var idx : number = 200, len : number;
            this.backupItems = [];
            //set move to checked item
            for (i = 0, len = this._$items.length; i < len; i+=1) {
                if(this._$items[i]['checked']) {
                    this._$items[i]['mov'] = true;
                    idx = i;
                    break;
                }
            }
            this.backupItems = this.copyArray(this._$items);
            this.item = this.backupItems.splice(idx, 1);
            if(idx < 8) {
                this._setList(this._$items, 0);
            } else {
                var bisSelItemOutOfShowList : boolean = true;
                for (i = 0, len = liList.length; i < len; i+=1) {
                    if (liList[i].innerText === (this._$items[idx].majorChannel + ' ' + this._$items[idx].name)) {
                        bisSelItemOutOfShowList = false;
                    }
                }
                if (bisSelItemOutOfShowList) {
                    this._setList(this._$items, idx);
                } else {
                    var findIdx;
                    if (liList[0].innerText === (this._$items[idx].majorChannel + ' ' + this._$items[idx].name)) { // if selItem is firstItem in showList
                        findIdx = idx - 1;
                    } else {
                        findIdx = this._findIdx(liList[0], this.backupItems);
                    }
                    this._setList(this._$items, findIdx);
                }
            }
        } else if (this.checkedCount > 1) {
            this.mode = "moveGroup";
            var smallestIdx : number = 200, len : number, backupArr : any = [];
            backupArr.length = 0;
            this.backupItems = [];
            this.backupItems.length = 0;
            this.backupItemArr.length = 0;

            //find checked smalledstIdx
            for (i = 0; i < this._$items.length; i+=1) {
                if(this._$items[i].checked) {
                    smallestIdx = (smallestIdx > i) ? i : smallestIdx;
                }
            }

            //rearrange Item
            var bisFirstSel : boolean = false;
            for ( i = 0, len = this._$items.length; i < len; i+=1) {
                if(i === smallestIdx) {
                    backupArr.push({
                        'grp' : true,
                        'ccid' : false,
                        'majorChannel' : '',
                        'name' : ''
                    });
                }
                if (!this._$items[i].checked) {
                    backupArr.push(this._$items[i]);
                } else {
                    if(!bisFirstSel) {
                        bisFirstSel = true;
                        this.selectObjName = this._$items[i].name;
                    }
                    this.backupItemArr.push(this._$items[i]);
                }
            }
            this.backupItems = this.copyArray(backupArr);
            tempLength = this._$items.length;
            if(smallestIdx < 8) {
                this._setList(backupArr, 0);
            } else {
                this._setList(backupArr, this._findIdx(liList[0], this.backupItems));
            }
            this.totalCount = tempLength;
            this._setInfo();
        } else {
            return;
        }
    }
    getTemplateEl (aParam : any) : any {
        var tpl = '<article id="editChannelList" class="' + aParam.className + '"><hgroup><h1>' + this.title + '</h1><p></p></hgroup><ul></ul></article>';
        var div = document.createElement('div');
        div.innerHTML = tpl;
        return div.firstChild;
    }
    getListTemplateEl (param : any, i : number) : any {
        var className : string = '';
        if (param.checked) {
            className = 'sel';
        } else if (param.grp) {
            className = 'mov grp';
        } else if (param.mov) {
            className = 'mov on';
        }
        var tpl : string = '<li id=' + param.sid + ' class=' + className + ' id=' + param.ccid + '>' + param.majorChannel + ' ' + param.name + '</li>';
        var div : any = document.createElement('div');
        div.innerHTML = tpl;
        return div.firstChild;
    }
    copyArray (aArr : any) : any {
        var length : number = aArr.length;
        var result : any = [];
        var index : number = 0;
        var i : number;
        if (length > 0) {
            for ( i = 0; i < length; i+=1) {
                if (aArr[i]) {
                    if(aArr[i].ccid) {
                        result[index] = aArr[i];
                        index += 1;
                    }
                }
            }
        }
        return result;
    }
    returnToOriginList() : void {
        var i : number;

        if (this.item[0]) {
            this.item[0]['mov'] = false;
        }
        for(i = 0; i < this.originalFocus; i+=1) {
            this.originalIdx = this.originalIdx - 1 < 0 ? this.originalItems.length - 1  : this.originalIdx - 1;
        }

        this._setList(this.originalItems, this.originalIdx);
        this.checkedCount = 0;
        this.setFocus(this.originalFocus);
        this._setInfo();
        this.on();
        this.mode = '';
    }
    _adjustList (aList : any) : void {
        switch(this.group) {
            case 'Favourite1':
                this._favList1.chList = aList;
                break;
            case 'Favourite2':
                this._favList2.chList = aList;
                break;
            case 'Favourite3':
                this._favList3.chList = aList;
                break;
            case 'Favourite4':
                this._favList4.chList = aList;
                break;
            case 'Favourite5':
                this._favList5.chList = aList;
                break;
            default:
                this._favList1.chList = aList;
        }
    }
    _findIdx(aObj : any, aList : any) : number {
        var i : number,	focusText : string = aObj.innerText;

        for(i = 0; i < aList.length; i+=1) {
            if((aList[i].majorChannel + ' ' + aList[i].name) === focusText) {
                return i;
            }
        }
        return 0;
    }
}
export class editChannelOptList extends __COMPONENT_KERNEL__ {
    _totalCount : number = 0;
    _optList : any  = [];
    target : any  = null;
    _keyTable : any ;

    constructor() {
        super();
    }
    create (aParam : any) : void {
        $.extend(this, aParam);
        this.$ = $(this.getTemplateOptEl(this));
        this._keyTable = [];
        this._keyTable[hx.key.KEY_UP] = this._doUp;
        this._keyTable[hx.key.KEY_DOWN] = this._doDown;
        this._keyTable[hx.key.KEY_OK] = this._doOk;
    }
    setOptList (aOptList : any, aTarget : any) : void {
        this.target = aTarget;
        this._optList = aOptList;

        this.fillOptList(aOptList);
    }
    fillOptList (aOptList : any) : void {
        var ul : any = this.$[0].querySelector('ul');
        var i : number;

        ul.innerHTML = "";
        this._totalCount = aOptList.length;
        for ( i = 0; i < aOptList.length; i+=1) {
            var li : any = this.getTemplateOptLiEl({
                'tit' : aOptList[i].name,
                'cb' : aOptList[i].cb,
                'index' : i
            });
            ul.appendChild(li);
        }
        this._setFocus(0);
    }
    _setFocus(aIndex : number) : void {
        var i : number,
            liList : any = this.$[0].querySelectorAll('li');

        for(i=0; i < liList.length; i+=1) {
            liList[i].classList.remove('on');
        }
        liList[aIndex].classList.add('on');
    }
    _on_alKey(aParam : any) : boolean {
        hx.log("info", "editChannelOptList(Comp) - _on_alKey(" + aParam.alKey + ")");
        var fx : any = this._keyTable[aParam.alKey];
        var bConsumed : boolean = false;
        if (fx) {
            fx.call(this, aParam);
            if (aParam.alData === undefined) {
                aParam.alData = {};
            }
            aParam.alData.me = this;
            this.sendEventToParent(aParam);
            bConsumed = true;
        }
        return bConsumed;
    }
    _doUp () : boolean {
        var focusObj : any = this.$[0].querySelector('.on');
        if (focusObj) {
            var liList : any = this.$[0].querySelectorAll('li');
            var ul : any = this.$[0].querySelector('ul');
            var focusIndex : number = this._findIdx(focusObj, liList);
            var nextFocusIndex : number = focusIndex - 1 < 0 ? this._totalCount - 1 : focusIndex - 1;
            var nextObj : any = liList[nextFocusIndex];
            focusObj.classList.remove('on');
            nextObj.classList.add('on');
        }
        return false;
    }
    _doDown () : boolean {
        var focusObj : any = this.$[0].querySelector('.on');
        if (focusObj) {
            var liList : any = this.$[0].querySelectorAll('li');
            var ul : any = this.$[0].querySelector('ul');
            var focusIndex : number = this._findIdx(focusObj, liList);
            var nextFocusIndex : number = focusIndex + 1 >= this._totalCount ? 0 : focusIndex + 1;
            var nextObj : any = liList[nextFocusIndex];
            focusObj.classList.remove('on');
            nextObj.classList.add('on');
        }
        return false;
    }
    _doOk () : boolean {
        var focusObj : any = this.$[0].querySelector('.on');
        var cb : any = focusObj.dataset.cb;
        if (cb !== "undefined") {
            this.target[cb](focusObj.innerHTML);
        } else {// subMenu
            this.fillOptList(this._optList[this._findIdxWithName(focusObj, this._optList)].sub_menu);
        }
        return false;
    }
    getTemplateOptEl (aArgObject : any) : any {
        var title : string = aArgObject.tit;
        var tpl : string = '<section id="' + aArgObject.id + '">' + '<p class="tit">' + title + '</p>' + '<ul class="menu"></ul></section>';
        var div : any = document.createElement('div');
        div.innerHTML = tpl;
        return div.firstChild;
    }
    getTemplateOptLiEl (aArgObject : any) : any {
        var title : string = aArgObject.tit;
        var index : number = aArgObject.index;
        var tpl : string = '<li' + ' data-cb=' + aArgObject.cb + '>' + title + '</li>';
        var div : any = document.createElement('div');
        div.innerHTML = tpl;
        return div.firstChild;
    }
    _findIdx(aObj : any, aList : any) : number {
        var i : number,	focusText : string = aObj.innerText;

        for(i = 0; i < aList.length; i+=1) {
            if(aList[i].innerText === focusText) {
                return i;
            }
        }
        return 0;
    }
    _findIdxWithName (aObj : any, aList : any) : number {
        var i : number,	focusText : string = aObj.innerText;

        for(i = 0; i < aList.length; i+=1) {
            if(aList[i].name === focusText) {
                return i;
            }
        }
        return 0;
    }
}

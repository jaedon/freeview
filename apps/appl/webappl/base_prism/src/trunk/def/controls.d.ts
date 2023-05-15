/// <reference path="jquery.d.ts"/>
/**
 * Created by bglee@humaxdigital.com on 2014. 4. 1..
 */
interface JQueryStatic {
    absoluteClone(aParam): JQuery;
    afterTransition(aHandler): JQuery;
    afterTransitionOneShow(aHandler): JQuery;
}
interface ifCEventSource {
    _eventReceivers: any;
    _registerEvent(eventList): void;
    destroy(): void;
    addEventHandler(aEventName, aReceiver, aHandler): void;
    removeEventHandler(aReceiver, aEventName?): void;
    _fireEvent(...aArgs: any[]): void;
}
interface ifCControl extends ifCEventSource {
    CONST: any;
    _$self: JQuery;
    _currentIndex: number;
    _drawParam: any;
    _volitileParam: any[];
    _keyMap: any;
    _keyMapBuilder: any;
    _transitioning: boolean;
    _controls: any[];
    _shown: boolean;
    doShow: Function;
    init($aEl): void;
    $self(): JQuery;
    destroy(): void;
    addVolitileParam(aParamNameList: any): void;
    doNavi(aNaviHandler?): any;
    isFocusable(): boolean;
    hasFocus(): boolean;
    setFocus(aFocus?): void;
    setFocusedItem(aIndex?, aForce?): void;
    getFocusedItem(): JQuery;
    $focusedItem(): JQuery;
    setFocusable(aFocusable?): void;
    setActiveFocus(aActiveFocus?): void;
    getActiveFocusInfo(): any;
    changeFocus($aItem): boolean;
    hasNaviMap(): boolean;
    draw(aParam?): void;
    setId(aIdName: string): void;
    addClass(aClassName: string): any;
    removeClass(aClassName: string): any;
    isShown(): boolean;
    show(aParam?): void;
    hide(): void;
    setTransition(aTransition): void;
    isTransitioning(): boolean;
    setDrawParam(aDrawParam?, aValue?, aVolitile?): void;
    getDrawParam(aParamName): any;
    getMargins(): any;
    getWidth(): any;
    getHeight(): any;
    getTop(): any;
    getLeft(): any;
    setActiveIndex(index): void;

    onItemSelected: Function;
    onFocusChanged?: (aOld, aNew) => void;

    doKey(aKeyCode): boolean;
    doKeyOk(aKeyCode): boolean;

    doKeyOption?(aKeyCode?): boolean;
    doKeyRec?(aKeyCode?): boolean;
    doKeyEscape?(aKeyCode?): boolean;
    doKeyLeft?(aKeyCode?): boolean;
    doKeyRight?(aKeyCode?): boolean;
    doKeyBack?(aKeyCode?): boolean;
    doKeyUp?(aKeyCode?): boolean;
    doKeyDown?(aKeyCode?): boolean;
    doKeyPageDown?(aKeyCode?): boolean;
    doKeyPageUp?(aKeyCode?): boolean;
    doKeyNumber?(aKeyCode?): boolean;
    doKeyStop?(aKeyCode?): boolean;

    doHide(aParam?): void;
}
interface ifCGroupControl extends ifCControl {
    _$container: JQuery;
    destroyChildControl(): void;
    setChildControl(aControls: any[], aFocusIndex?: number, aKeyMap?): void;
    _insertChildControl($target, index, control, setFocus): void;
    insertChildControl(aIndex: number, aControl: any, aSetFocus: boolean): void;
    removeChildControl(aControl): boolean;
    _onChildFocusChanged(oldIndex, newIndex): void;
    initActiveFocus(): void;
    getFocusedChild(): any;
    getFocusedChildIndex(): number;
    //TODO: fix typing error 'Chld'
    setFocusedChld(aControl?): boolean;
    doDraw(aParam): JQuery;
}
interface ifCVerticalGroupControl extends ifCGroupControl {
    setCommonTag(aCommonTagInfo): void;
}
interface ifCHorizontalGroupControl extends ifCGroupControl {
}
interface ifCViewGroupControl extends ifCGroupControl {
    _enabledTop: boolean
    safeDraw(): void;
    setLeft(aLeft?, aWithoutAnim?): void;
    setTop(aLeft?, aWithoutAnim?): void;
    getClientRect(aLeft?, aWithoutAnim?): any;
    enableTop(): void;
    doScroll(): void;
//    _onChildFocusChanged
}
interface ifCDataProvider extends ifCEventSource {
    init(aArray): void;
    setArray(aArray): void;
    destroy(): void;
    getItem(aKey): any;
    getItemIndex(aItem, aMatcher): number;
    getCount(): number;
    insertItem(aKey, aItem): void;
    insertItems(aKey, aItems): void;
    appendItem(aItem): void;
    appendItems(aItems): void;
    removeItem(aKey): void;
    //TODO: what?
    removeRecordingItem(aKey): void;
    prependItem(aItem): void;
    updateItem(aKey, aItem?): void;
    move(aFrom, aTo): void;
    getDataArray: any;
}
interface ifCDataControl extends ifCControl {
    _dataProvider: any;
     _selectedIndex: number;
    _drawer;
    _drawnItems: any;
    _viewGroupControl: ifCViewGroupControl;
    _ignoreDrawnItemMove: boolean;
    _movingCanceling: boolean;
    _contentAvail: any;
    _movingModeStartViewPos: any;
    _movingModeStartKey: number;
    _requestedFocusedItemKey : number;
    _prevParam: any;
    _moving: boolean;
    _prevDrawnItems: any;
    _marqueeSelector: any;
    init(): void;
    destory(): void;
//    _setParentViewGroupControl
    setDataProvider(aDataProvider): void;
    setDataDrawer(aDrawer): void;
    getCurrentIndex(): number;
    getHeight(): number;
    getWidth(): number;
    getNextAvail(aKeyCode: string): any;
    getSelectedIndex(): number;
    getFocusedItem(): any;
    setFocusedItemByKey(aKey: number): void;
    draw(aParam?): void;
    doDraw(aParam?): JQuery;
    redraw(): void;
    clearDrawnItems(): void;
//    _setDrawnItem
//    _getDrawItem
//    _pickDrawnItems
    isMovingMode(): boolean;
    enterMovingMode(): void;
    leaveMovingMode(aCancel): void;
//    _finalizeMovingModeLeft(): void;
//    _setItemMoving
//    _getMovableInfo
//    _move(aFrom, aTo): void;
//    _setPreviousDrawItems(aPrevDrawnItems): void;
    clearPreviousDrawItems(aPrevDrawItems): void;
    marqueeOn(selector): void;
    marqueeOff(): void;
    onActiveFocusedGained(): void;
    onActiveFocusedLost(): void;
    onFocusChanged($old, $new): void;
}
interface ifCListControl extends ifCDataControl {
    getItemPos(aKey): any;
}
interface ifCVariableListControl extends ifCDataControl {
    getItemPos(aKey): any;
}
interface ifCGridControl extends ifCDataControl {
    getHeight(): number;
//    _getMovableInfo
}
interface ifCHorizontalGridControl extends ifCDataControl {
    getHeight(): number;
}
interface ifCCarouselControl extends ifCControl {
    _rollDataProvider: any;
    setDataDrawer(aDrawer): void;
    setAnchorDrawer(aAnchorDrawer): void;
    setData(aMenuItem): void;
    getViewCount(): number;
    getCurrentIndex(): number;
    getCurrentItem(): any;
    setCurrentIndex(aIndex: number): void;
    getCurrentViewingItemInfos($aEl: JQuery): any;
    jqCurrent(): JQuery;
    safeUpdate(aFnUpdate: Function): void;
    doDraw(aParam): void;
    doDrawHorizontal(aParam): void;
    doDrawVertical(aParam): void;
//    _update
//    _animate
    onFocusChanged?: (aParam: any) => void;
    onFocusStartToChange?: (aParam: any) => void;
    drawPrismChannelList?: (aParam: any) => void;
    drawPrismLiteSChannelList?: (aParam: any) => boolean;

    doKeyPlay?: (aKeyCode?) => boolean;
}
interface ifCLayoutControl extends ifCControl {
    _itemDrawers: Function;
    doDraw(aParam?): JQuery;
    setItemDrawers(aDrawers): void;
}
interface ifCVerticalLayoutControl extends ifCLayoutControl {
    // inherited properties & methods only
}
interface ifCHorizontalLayoutControl extends ifCLayoutControl {
    // inherited properties & methods only
}
interface ifCViewControl extends ifCControl {
    doDraw($aEl): void;
    doDrawItem?(): JQuery;
}
interface ifCLayeredGroupControl extends ifCGroupControl {
    _layerStack: any[];
    _$layer: JQuery;
    _createParam: any;
    _contextualProperties: any[];
    setCommonTag(aCommonTagInfo): void;
    setContextualProperties(aContextualProperties): void;
    stackSize(): number;
    $layer(): JQuery;
    createLayer(aParam): number;
    removeLayer(aParam?): void;
}

declare var CONTROLS_CONFIG: {
    KEY_DOWN : number;
    KEY_UP : number;
    KEY_LEFT : number;
    KEY_RIGHT : number;
    KEY_BACK : number;
    KEY_PAGEUP : number;
    KEY_PAGEDOWN : number;
    KEY_REWIND : number;
    KEY_FAST_FWD : number;
    KEY_PLAY_SPEED_DOWN : number;
    KEY_PLAY_SPEED_UP : number;
    KEY_PLAY : number;
    KEY_PAUSE : number;
    KEY_ESCAPE : number;
    KEY_STOP : number;
    KEY_OK: number;
    CUSTOM_KEY_HANDLERS : any;
    KEY_NETFLIX : number;
};
declare var CGroupControl: {
    new ($aEl?: JQuery): ifCGroupControl;
};
declare var CVerticalGroupControl: {
    new ($aEl?: JQuery): ifCVerticalGroupControl
};
declare var CHorizontalGroupControl: {
    new ($aEl?: JQuery): ifCHorizontalGroupControl
};
declare var CViewGroupControl: {
    new ($aEl?: JQuery): ifCViewGroupControl;
};
declare var CDataProvider: {
    new (aArray): ifCDataProvider;
};
declare var CListControl: {
    new ($aEl?: JQuery): ifCListControl;
};
declare var CVariableListControl: {
    new ($aEl?: JQuery): ifCVariableListControl;
};
declare var CGridControl: {
    new ($aEl?: JQuery): ifCGridControl;
};
declare var CHorizontalGridControl: {
    new ($aEl?: JQuery): ifCHorizontalGridControl;
};
declare var CCarouselControl: {
    new ($aEl?: JQuery): ifCCarouselControl;
};
declare var CVerticalLayoutControl: {
    new ($aEl?: JQuery): ifCVerticalLayoutControl;
};
declare var CHorizontalLayoutControl: {
    new ($aEl?: JQuery): ifCHorizontalLayoutControl;
};
declare var CViewControl: {
    new ($aEl?: JQuery): ifCViewControl;
};
declare var CLayeredGroupControl: {
    new ($aEl?: JQuery): ifCLayeredGroupControl
};

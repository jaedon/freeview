var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", 'model/model', 'pages/util/controls', 'prismux/prism.dlg'], function (require, exports, __MODEL__, __CONTROLS__, __PRISM_CDIALOG__) {
    var mPrism;
    (function (mPrism) {
        var MAX_DATA_COUNT = 11;
        var ONDEMAND_PAGE_INDEX = 1; //
        var START_INDEX = 0, END_INDEX = 0;
        var EFunction = {
            EDelete: 0,
            EMove: 2,
            EDeleteAllFiles: 9,
            EOpen: 10,
            ENaviLeft: 11,
            ENaviRight: 12,
            EAllApps: 13,
        };
        var fnCreateHome = function (a) {
            createHomeMenuLayout(a);
            if (!a._container) {
                var container = new CVerticalGroupControl(null);
                container.addClass('container');
                container.setChildControl([a._topLayout, a._bottomGroup], 0);
                container.setDrawParam({});
                container.onFocusChanged = function (aOld, aNew) {
                    if (aOld && aOld.$self()) {
                        aOld.$self().addClass('disable');
                    }
                    if (aNew && aNew.$self()) {
                        aNew.$self().removeClass('disable');
                    }
                };
                a._container = container;
                a.setChildControl([a._container]);
                a.draw({
                    setActiveFocus: true
                });
            }
        };
        var fnShow = function (a) {
            a.$self().addClass('show');
            if (a._container) {
                a._container.$self().addClass('show');
            }
        };
        var fnHide = function (a) {
            a.$self().removeClass('show');
            if (a._container) {
                a._container.$self().removeClass('show');
            }
        };
        var createHomeMenuLayout = function (a) {
            // top(menu) layout
            if (!a._topLayout) {
                var topMenuLayout = new CHorizontalLayoutControl(null);
                topMenuLayout.addClass('topArea');
                topMenuLayout.setDrawParam({
                    padding: 30,
                    setFocusRolling: true
                });
                var menuItemDrawer = function (index, $item) {
                    $item.css('width', a._menuItemInfo[index].width);
                    $item.append($('<div>', {
                        'class': 'title',
                        'html': a._menuItemInfo[index].title
                    }));
                    return a._menuItemInfo[index] == a._activeItem;
                };
                var menuItemDrawers = [];
                var numberOfMenuItems = a._menuItemInfo.length;
                for (var i = 0; i < numberOfMenuItems; i += 1) {
                    menuItemDrawers.push(menuItemDrawer);
                }
                topMenuLayout.setItemDrawers(menuItemDrawers);
                topMenuLayout.draw = function (param) {
                    if (!this._keyMap) {
                        CHorizontalLayoutControl.prototype.draw.call(this, param);
                    }
                };
                topMenuLayout.onActiveFocusedGained = function ($old, $new) {
                    //a._bottomGroup.$self().removeClass('up');
                    //a._bottomGroup.$self().addClass('down');
                    //a._topLayout.$self().find('.icon').css('opacity', '1');
                };
                topMenuLayout.onFocusChanged = function ($old, $new) {
                    var index = parseInt($new.attr('data'), 10);
                    a._activeItem = a._menuItemInfo[index];
                };
                topMenuLayout.onItemSelected = function (index, $item) {
                    a.onHomeMenuSelected(a._menuItemInfo[index].name);
                };
                a._topLayout = topMenuLayout;
            }
            // bottom(others) layout
            if (!a._bottomGroup) {
                var bottomGroupControl = new CVerticalGroupControl(null);
                bottomGroupControl.addClass('bottomArea');
                bottomGroupControl.addClass('disable');
                a._bottomGroup = bottomGroupControl;
            }
        };
        var createAppLayout = function (a) {
            if (a._appListControl) {
                a._appListControl.setDataProvider(createAppDataProvider(a, a._appData));
            }
            else {
                var appListControl = new __CONTROLS__.CListControl(null);
                appListControl.addClass('appList');
                appListControl.setDataProvider(createAppDataProvider(a, a._appData));
                appListControl.setDrawParam({
                    itemWidth: 110,
                    itemHeight: 110,
                    align: 'horizontal',
                    setFocusRolling: true
                });
                appListControl.onActiveFocusedGained = function () {
                    appListControl.$self().addClass('active');
                };
                appListControl.onActiveFocusedLost = function () {
                    appListControl.$self().removeClass('active');
                };
                appListControl.onItemSelected = function (param) {
                    if (param && param.item) {
                        a.onHomeItemSelected(param.item.getData(), param.item.getDataType());
                    }
                };
                appListControl._getMovableInfo = function (key) {
                    var movableInfo = __CONTROLS__.CVariableListControl.prototype._getMovableInfo.call(appListControl, key);
                    if (movableInfo.left < START_INDEX) {
                        movableInfo.left = -1;
                    }
                    if (movableInfo.right > END_INDEX) {
                        movableInfo.right = -1;
                    }
                    return movableInfo;
                };
                appListControl.setDataDrawer(fnItemViewDrawer);
                a._appListControl = appListControl;
            }
            if (a._contentViewControl) {
                a._bottomGroup.setChildControl([a._contentViewControl, a._appListControl], 0, true);
            }
            else {
                a._bottomGroup.setChildControl([a._appListControl], 0, true);
            }
        };
        var createContentLayout = function (a) {
            if (a._contentViewControl) {
                a._contentListControl.setDataProvider(createContentDataProvider(a, a._contentData));
            }
            else {
                var contentViewGroupControl = new CViewGroupControl(null);
                contentViewGroupControl.addClass('contentView');
                contentViewGroupControl.setDrawParam({
                    showScroll: false,
                    showHidden: true
                });
                contentViewGroupControl.$self().prepend(a._$contentTitle);
                a._contentViewControl = contentViewGroupControl;
                var contentListControl = new __CONTROLS__.CListControl(null);
                contentListControl.addClass('contentList');
                contentListControl.setDataProvider(createContentDataProvider(a, a._contentData));
                contentListControl.setDrawParam({
                    itemWidth: 186,
                    itemHeight: 110,
                    align: 'horizontal'
                });
                contentListControl.onActiveFocusedGained = function () {
                    contentViewGroupControl.$self().addClass('active');
                };
                contentListControl.onActiveFocusedLost = function () {
                    contentViewGroupControl.$self().removeClass('active');
                };
                contentListControl.onItemSelected = function (param) {
                    if (param && param.item) {
                        a.onHomeItemSelected(param.item.getData(), param.item.getDataType());
                    }
                };
                contentListControl.setDataDrawer(fnItemViewDrawer);
                a._contentListControl = contentListControl;
                a._contentViewControl.setChildControl([a._contentListControl], 0);
            }
            if (a._appListControl) {
                a._bottomGroup.setChildControl([a._contentViewControl, a._appListControl], 0, true);
            }
            else {
                a._bottomGroup.setChildControl([a._contentViewControl], 0, true);
            }
        };
        var fnItemViewDrawer = function (param) {
            var $item = param ? param.$item : null;
            var item = param ? param.item : null;
            if (item && $item) {
                var $viewItem = null;
                switch (item.getDataType()) {
                    case 2 /* EApp */:
                        $viewItem = $('<div>', {
                            'class': 'appItem',
                        });
                        $viewItem.css('background', 'url(' + item.getThumbnail() + ') no-repeat center');
                        if (item._data.PlayerType !== '/promoted_players' && item._data.PlayerType !== '/other_players') {
                            $viewItem.css('background-size', '111% 106%');
                        }
                        break;
                    case 1 /* EContent */:
                        $viewItem = $('<div>', {
                            'class': 'contentItem',
                        }).append($('<img>', {
                            'class': 'contentImg',
                            'alt': '',
                            'src': item.getThumbnail()
                        })).append($('<img>', {
                            'class': 'contentLogo',
                            'alt': '',
                            'src': item.getLogo() || ''
                        })).append($('<div>', {
                            'class': 'contentTitle',
                            'text': item.getTitle()
                        }).append($('<div>', {
                            'class': 'contentSecondary',
                            'text': item.getDescription()
                        })));
                        break;
                    case 3 /* EMoreContent */:
                        $viewItem = $('<div>', {
                            'class': 'contentMore',
                            'text': 'SEE MORE'
                        });
                        $item.addClass('moreVideo');
                        break;
                    default:
                        break;
                }
                if ($viewItem) {
                    $item.append($viewItem);
                }
            }
        };
        var createContentDataProvider = function (a, contentList) {
            var i;
            var allDataList = [];
            // Create list
            var contentLen = contentList ? contentList.length : 0;
            for (i = 0; i < contentLen; i += 1) {
                allDataList.push(new CContentViewItem(contentList[i]));
            }
            // for 'More Video' tab to get more recommendations
            allDataList.push(new CContentViewItem(null));
            return new CDataProvider(allDataList);
        };
        var createAppDataProvider = function (a, appList) {
            var i;
            var allDataList = [];
            // Create list
            START_INDEX = 0;
            var appLen = appList ? appList.length : 0;
            for (i = 0; i < appLen; i += 1) {
                allDataList.push(new CAppViewItem(appList[i]));
            }
            END_INDEX = appLen - 1;
            return new CDataProvider(allDataList);
        };
        var CHubDlg = (function (_super) {
            __extends(CHubDlg, _super);
            function CHubDlg(layeredGroupControl) {
                _super.call(this, layeredGroupControl);
            }
            CHubDlg.prototype.init = function (layeredGroupControl) {
                var a = this;
                _super.prototype.init.call(this, layeredGroupControl);
                a.setId('hubDlg');
                a._menuItemInfo = [{
                    name: 'tvguide',
                    title: 'TV GUIDE',
                    width: 102
                }, {
                    name: 'ondemand',
                    title: 'PLAYERS',
                    width: 104
                }, {
                    name: 'toppicks',
                    title: 'TOP PICKS',
                    width: 118
                }, {
                    name: 'recordings',
                    title: 'RECORDINGS',
                    width: 145
                }, {
                    name: 'media',
                    title: 'MEDIA',
                    width: 73
                }, {
                    name: 'settings',
                    title: 'SETTINGS',
                    width: 108
                }];
                a._activeItem = a._menuItemInfo[0];
                a._$disconnectIcon = $('<div>', {
                    'class': 'disconnectIcon'
                });
                a._$disconnectMsg = $('<div>', {
                    'class': 'disconnectMsg',
                    'html': "Check your box is connected to the network and you have agreed to the Privacy Policy, Terms & Conditions." + "<br>" + "Agreeing will make full use of our services."
                });
                a._$contentTitle = $('<p>', {
                    'class': 'title',
                    'text': 'Recommended'
                });
                fnCreateHome(a);
            };
            CHubDlg.prototype.destroy = function () {
                var a = this;
                a.destroyChildControl();
                _super.prototype.destroy.call(a);
            };
            CHubDlg.prototype.setLanguage = function (language) {
                var a = this;
                a._languageInfo = language;
            };
            CHubDlg.prototype.setHideAppData = function (hideData) {
                var a = this;
                a._hideAppData = hideData;
            };
            CHubDlg.prototype.setHomeData = function (data, type) {
                var a = this;
                createHomeMenuLayout(a);
                if (!data) {
                    a.clear();
                    a.drawContainer();
                    return;
                }
                var i, dataList = [];
                var listLength = data.length;
                for (i = 0; i < listLength; i += 1) {
                    if (i >= MAX_DATA_COUNT) {
                        break;
                    }
                    dataList.push(data[i]);
                }
                switch (type) {
                    case 2 /* EApp */:
                        a.updateAppList(dataList);
                        break;
                    case 1 /* EContent */:
                        if (hx.emulMode) {
                            dataList = [{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}];
                        }
                        a.updateContentList(dataList);
                        break;
                    default:
                        break;
                }
                a.drawContainer();
            };
            CHubDlg.prototype.drawContainer = function () {
                var a = this;
                if (!a._contentViewControl && !a._appListControl) {
                    a._container.$self().append(a._$disconnectIcon).append(a._$disconnectMsg);
                    a._container.setChildControl([a._topLayout], 0, true);
                }
                else {
                    a._$disconnectIcon.remove();
                    a._$disconnectMsg.remove();
                    a._container.setChildControl([a._topLayout, a._bottomGroup], 0, true);
                    a._bottomGroup.setActiveFocus(false);
                }
                a._container.draw({});
                a._container.setFocusedChld(a._topLayout);
            };
            CHubDlg.prototype.clear = function () {
                var a = this;
                if (a._bottomGroup) {
                    a._$disconnectIcon.remove();
                    a._$disconnectMsg.remove();
                    a._$contentTitle.remove();
                    if (a._contentViewControl) {
                        if (a._contentListControl) {
                            a._contentListControl.destroy();
                            a._contentListControl = null;
                            a._contentData = [];
                        }
                        a._contentViewControl.destroy();
                        a._contentViewControl = null;
                    }
                    if (a._appListControl) {
                        a._appListControl.destroy();
                        a._appListControl = null;
                        a._appData = [];
                    }
                    a._bottomGroup.destroy();
                    a._bottomGroup = null;
                }
            };
            CHubDlg.prototype.show = function (cb) {
                var a = this;
                a._shown = true;
                //a.destroyChildControl();
                fnShow(a);
                if (cb) {
                    cb();
                }
            };
            CHubDlg.prototype.hide = function (cb) {
                var a = this;
                a._shown = false;
                //a.destroyChildControl();
                if (cb) {
                    cb();
                }
                fnHide(a);
            };
            CHubDlg.prototype.doOpen = function (param) {
                var a = this;
                //a.destroyChildControl();
            };
            CHubDlg.prototype.doKeyPageUp = function () {
                return true;
            };
            CHubDlg.prototype.doKeyPageDown = function () {
                return true;
            };
            CHubDlg.prototype.doKeyOption = function () {
                return true;
            };
            CHubDlg.prototype.updateAppList = function (aList) {
                var that = this;
                that._appData = aList;
                if (aList && aList.length) {
                    createAppLayout(that);
                }
            };
            CHubDlg.prototype.updateContentList = function (aList) {
                var that = this;
                that._contentData = aList;
                if (aList && aList.length) {
                    createContentLayout(that);
                }
            };
            return CHubDlg;
        })(__PRISM_CDIALOG__.CDialog);
        mPrism.CHubDlg = CHubDlg;
        var CViewItem = (function () {
            function CViewItem(data) {
                var a = this;
                a._title = "";
                a._children = [];
                a._isMoving = false;
                a._isDelete = false;
                if (data) {
                    a.init(data);
                }
            }
            CViewItem.prototype.init = function (data) {
                var a = this;
                a._data = data;
                a._title = data.title;
            };
            CViewItem.prototype.getData = function () {
                return this._data;
            };
            CViewItem.prototype.getTitle = function () {
                return this._title || this._data.name;
            };
            CViewItem.prototype.getChildren = function () {
                return this._children;
            };
            CViewItem.prototype.getDescription = function () {
                return this._data.description || this._data.secondary;
            };
            CViewItem.prototype.getThumbnail = function () {
                return this._thumbnail || this._data.thumbnail || this._data.thumbnailUrl || this._data.thumbnailURL;
            };
            CViewItem.prototype.getLogo = function () {
                return this._logo || this._data.logoURL;
            };
            CViewItem.prototype.getDataType = function () {
                return this._type;
            };
            CViewItem.prototype.getWidth = function () {
                return this._width;
            };
            CViewItem.prototype.isMoving = function () {
                return this._isMoving;
            };
            CViewItem.prototype.isDelete = function () {
                return this._isDelete;
            };
            CViewItem.prototype.setData = function (data) {
                this._data = data;
            };
            CViewItem.prototype.setTitle = function (title) {
                this._title = title;
            };
            CViewItem.prototype.setChildren = function (children) {
                this._children = children;
            };
            CViewItem.prototype.setMove = function (move) {
                this._isMoving = move;
            };
            CViewItem.prototype.setDelete = function (del) {
                this._isDelete = del;
            };
            CViewItem.prototype.setThumbnail = function (thumbnail) {
                this._thumbnail = thumbnail;
            };
            CViewItem.prototype.setDataType = function (type) {
                this._type = type;
            };
            CViewItem.prototype.setWidth = function (width) {
                this._width = width;
            };
            CViewItem.prototype.isSelected = function () {
                return this._data.selected;
            };
            return CViewItem;
        })();
        var CActionViewItem = (function (_super) {
            __extends(CActionViewItem, _super);
            function CActionViewItem(data) {
                var a = this;
                _super.call(this, data);
                a.setMove(false);
                a.setDelete(false);
            }
            CActionViewItem.prototype.setNavigation = function (direction) {
                this._direction = direction;
            };
            CActionViewItem.prototype.getNavigation = function () {
                return this._direction;
            };
            CActionViewItem.prototype.setAllDelete = function (param) {
                this._actionAllDelete = param;
            };
            CActionViewItem.prototype.getAllDelete = function () {
                return this._actionAllDelete;
            };
            return CActionViewItem;
        })(CViewItem);
        var CContentViewItem = (function (_super) {
            __extends(CContentViewItem, _super);
            function CContentViewItem(data) {
                var a = this;
                _super.call(this, data);
                a.setDataType(data ? 1 /* EContent */ : 3 /* EMoreContent */);
            }
            return CContentViewItem;
        })(CViewItem);
        var CAppViewItem = (function (_super) {
            __extends(CAppViewItem, _super);
            function CAppViewItem(data) {
                var a = this;
                _super.call(this, data);
                a.setDataType(2 /* EApp */);
                a.setThumbnail();
            }
            CAppViewItem.prototype.setThumbnail = function () {
                var a = this;
                var i = 0;
                if (a._data.icons) {
                    for (i; i < a._data.icons.length; i += 1) {
                        var thumbnail = a._data.icons[i];
                        if (thumbnail.ImageType == "HMS_FOCUS") {
                            this._thumbnail = a._data.icons[i].Url;
                        }
                    }
                }
                else {
                    this._thumbnail = _super.prototype.getThumbnail.call(a);
                }
            };
            return CAppViewItem;
        })(CViewItem);
        if (!window.prism) {
            window.prism = {};
        }
        prism.CHubDlg = CHubDlg;
        prism.createHubDlg = function (layeredGroupControl) {
            var hubDlg = new CHubDlg(layeredGroupControl);
            return hubDlg;
        };
    })(mPrism || (mPrism = {}));
});

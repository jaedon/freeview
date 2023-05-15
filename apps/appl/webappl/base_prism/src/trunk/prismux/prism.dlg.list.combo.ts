// file name : prismux/prism.dlg.list.combo.js
/*jslint sloppy:true, nomen:true, vars:true */

( function(prism) {
    var CDialog = prism.CDialog;
    var CListComboDlg = function(layeredGroupControl) {
        var a = this;
        a.init(layeredGroupControl);
    };
    CListComboDlg.prototype = new CDialog();
    CListComboDlg.prototype.init = function(layeredGroupControl) {
        var a = this;
        CDialog.prototype.init.call(a, layeredGroupControl);
        a.addClass('listDlg');
    };
    CListComboDlg.prototype.setTitleHtml = function(html) {
        var a = this;
        if (!a._$title) {
            var $title = $('<div>', {
                'class' : 'text'
            });
            a._$title = $title;
        }
        a._$title.html(html);
    };
    CListComboDlg.prototype.setButtonInfoList = function(buttonInfoList) {
        var a = this;
        a._buttonInfoList = buttonInfoList;
    };
    CListComboDlg.prototype.doOpen = function(param) {
        var a = this;
        var i, length;
        var initialFocus = 1;
        var childControl = [];
        var buttonDrawers = [];
        var titleDrawers = [
            function(index, $item) {
                $item.append(a._$title);
                $item.removeClass('-f');
            }];
        var buttonDrawer = function(index, $item) {
            $item.addClass('btn').html(a._buttonInfoList[index].text);
        };
        var titleLayout = new CVerticalLayoutControl(null);
        titleLayout.setDrawParam({
            padding : 15
        });
        titleLayout.setItemDrawers(titleDrawers);
        childControl.push(titleLayout);
        var ctrlHGroup = new CHorizontalGroupControl(null);
        var listViewControl = new CViewGroupControl(null);
        listViewControl.addClass('dlgMenuBg');
        var listControl = new CListControl(null);
        listControl.addClass('dlgMenu');
        listControl.setDataProvider(new CDataProvider(a._items));
        if (param && param.itemHeight) {
            listControl.setDrawParam({
                itemHeight : param.itemHeight
            });
        } else {
            listControl.setDrawParam({
                itemWidth : 471,
                itemHeight : 53,
                margin : [5, 10, 5, 10]
            });
        }
        a._listControl = listControl;
        if ((param.defaultCursor !== undefined) && (param.defaultCursor < a._listComboDlgVisibleMaxLength)) {
            a._checkList[param.defaultCursor] = true;
            a._defaultCursor = param.defaultCursor;
        }
        listControl.setDataDrawer(function(param) {
            var $span;
            if (a._items.length <= a._listComboDlgVisibleMaxLength) {
                param.$item.addClass('dlgMenuItem combo noScroll');
            } else {
                param.$item.addClass('dlgMenuItem combo');
            }
            if (param.item.cssClass) {
                param.$item.addClass(param.item.cssClass);
                if ((param.index + 1) === menuItems.length) {
                    param.$item.addClass('last');
                }
            }
            param.$item.append('<span class="b">' + '&nbsp;' + '</span>' + '<span class="l">' + param.item.name + '</span>');
            $span = param.$item.find('span');
            if (a._checkList) {
                if (a._checkList[param.index]) {
                    $span.addClass('selected');
                }
            }
            if (param.index === a._defaultCursor) {
                param.$item.addClass('-fd -afd-leaf');
            }
        });
        listControl.onItemSelected = function(param) {
            var $span = param.$item.find('span');
            if (!a._checkList) {
                a._checkList = [];
            }
            if ($span.hasClass('selected')) {
                a._checkList[param.itemKey] = false;
                $span.removeClass('selected');
            } else {
                a._checkList[param.itemKey] = true;
                $span.addClass('selected');
            }
        };
        listViewControl._$self.css({
            'width' : '439px',
            'height' : '265px',
            'padding' : '5px 10px'
        });
        listViewControl.setChildControl([listControl]);
        listViewControl.setDrawParam({
            margin : [5, 10, 5, 10],
            align : 'left',
            scrollByItemSize : true,
            showScroll : true,
            scrollUnit : 261 * 3,
            visibleAfterDraw : true
        });
        var buttonLayout = new CVerticalLayoutControl(null);
        buttonLayout.addClass('buttonArea');
        buttonLayout.setDrawParam({
            padding : 15,
            left : 22
        });
        if (a._buttonInfoList) {
            length = a._buttonInfoList.length;
            for ( i = 0; i < length; i += 1) {
                buttonDrawers.push(buttonDrawer);
            }
        }
        buttonLayout.setItemDrawers(buttonDrawers);
        buttonLayout.onItemSelected = function(index, $item) {
            var fn = null;
            if (a._buttonInfoList) {
                fn = a._buttonInfoList[index].fn;
            }
            if (fn) {
                fn();
            }
        };
        ctrlHGroup.setChildControl([listViewControl, buttonLayout], 0);
        childControl.push(ctrlHGroup);
        a.setChildControl(childControl, 1);
        a.setDrawParam({
            align : 'hcenter',
            padding : 33,
            margins : [34, 50, 60]
        });
    };

    CListComboDlg.prototype.doKey = function(keyCode) {
        var a = this;
        var consumed = CDialog.prototype.doKey.call(a, keyCode);
        switch(keyCode) {
            case CONTROLS_CONFIG.KEY_UP:
            case CONTROLS_CONFIG.KEY_DOWN:
            case CONTROLS_CONFIG.KEY_LEFT:
            case CONTROLS_CONFIG.KEY_RIGHT:
            case CONTROLS_CONFIG.KEY_PAGEUP:
            case CONTROLS_CONFIG.KEY_PAGEDOWN:
                consumed = true;
                break;
        }
        return consumed;
    };

    CListComboDlg.prototype.doKeyOption = function(keyCode) {
        return true;
    };

    CListComboDlg.prototype.doKeyBack = function(keyCode) {
        var a = this;
        a.close();
        return true;
    };

    prism.createListComboDlg = function(layeredGroupControl, listComboDlgInfo, callback) {
        if (layeredGroupControl) {
            var listComboDlg = new CListComboDlg(layeredGroupControl);
            listComboDlg._listComboDlgVisibleMaxLength = 5;
            listComboDlg._checkList = [];
            if (listComboDlgInfo) {
                if (listComboDlgInfo.textHtml) {
                    listComboDlg.setTitleHtml(listComboDlgInfo.textHtml);
                }
                if (listComboDlgInfo.listItems) {
                    listComboDlg._items = listComboDlgInfo.listItems;
                }
                if (listComboDlgInfo.cls) {
                    listComboDlg.addClass(listComboDlgInfo.cls);
                }
                if (listComboDlgInfo.timeOutValue) {
                    listComboDlg.setTimeout(listComboDlgInfo.timeOutValue);
                }
            }
            if (listComboDlgInfo.buttonList) {
                var i, len = listComboDlgInfo.buttonList.length, btnStr, id;
                var buttonInfoList = [];
                for ( i = 0; i < len; i++) {
                    btnStr = listComboDlgInfo.buttonList[i].str ? listComboDlgInfo.buttonList[i].str : listComboDlgInfo.buttonList[i];
                    id = listComboDlgInfo.buttonList[i].id ? listComboDlgInfo.buttonList[i].id : undefined;
                    buttonInfoList.push({
                        text : btnStr,
                        fn : id ? ( function(callback, i, id, checkList) {
                            return function() {
                                if (callback) {
                                    callback(i, id, listComboDlg._checkList);
                                }
                            };
                        }(callback, i, id, listComboDlg._checkList)) : ( function(callback, i, checkList) {
                            return function() {
                                if (callback) {
                                    callback(i, listComboDlg._checkList);
                                }
                            };
                        }(callback, i, listComboDlg._checkList))
                    });
                }
                listComboDlg.setButtonInfoList(buttonInfoList);
            }
            return listComboDlg;
        } else {
            var globalDlg = prism.createGlobalDialog('createListComboDlg', listComboDlgInfo, callback);
            return globalDlg;
        }
    };
}(prism));
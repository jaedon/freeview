/// <reference path="../def/sugar.d.ts" />
/// <reference path="../def/prism.d.ts" />
// file name : prismux/prism.dlg.photoplayback.js
/*jslint sloppy:true, nomen:true, vars:true */
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", 'prismux/prism.dlg.playback'], function (require, exports, __PRISM_CPLAYBACKDLG__) {
    var mPrism;
    (function (mPrism) {
        var MAX_WIDTH = 1280;
        var MAX_HEIGHT = 720;
        var TDirection = {
            ELeft: 0,
            ERight: 1
        };
        var mPhotoOption = null;
        var mTimer = null;
        var mDirection = TDirection.ERight;
        var fnUpdateSlideshowSpeed = function (a) {
            clearInterval(a._sliderTimer);
            a._sliderTimer = setInterval(function () {
                if (mDirection === TDirection.ELeft) {
                    a.prev();
                }
                else {
                    a.next();
                }
                if (a.onPlayStatePlaying) {
                    a.onPlayStatePlaying(a._slider.getCurrentItem());
                }
            }, Number(mPhotoOption.slideshowSpeed) < 3000 ? 3000 : Number(mPhotoOption.slideshowSpeed));
        };
        var fnCreatePhotoOption = function (a, items) {
            var slideshowSpeedList = ['0', '3000', '5000', '10000'];
            var settingDlg = prism.createSettingDlg(a._parent);
            mPhotoOption = a.doGetPhotoOption();
            var slideshowSpeedIndex = slideshowSpeedList.indexOf(mPhotoOption.slideshowSpeed);
            if (slideshowSpeedIndex === -1) {
                slideshowSpeedIndex = 1;
            }
            settingDlg.setDrawParam('width', 700);
            settingDlg.setTitleHtml(tr('LOC_SEL_OPTION_VAL_ID'));
            settingDlg.setSettingListItems([{
                type: 'multiSelection',
                text: tr('LOC_STR_SLIDESHOW_SPEED_ID'),
                config: {
                    selectionList: [{
                        text: 'Off',
                        value: 0
                    }, {
                        text: '3 secs',
                        value: 3000
                    }, {
                        text: '5 secs',
                        value: 5000
                    }, {
                        text: '10 secs',
                        value: 10000
                    }],
                    activeIndex: slideshowSpeedIndex
                }
            }, {
                type: 'multiSelection',
                text: tr('LOC_PLAY_MUSIC_ID'),
                config: {
                    selectionList: [{
                        text: 'Off',
                        value: 0
                    }, {
                        text: 'On',
                        value: 1
                    }],
                    activeIndex: Number(mPhotoOption.playMusic)
                }
            }]);
            settingDlg.onItemChanged = function (index, settingItem, changedValue) {
            };
            settingDlg.onOk = function (changedInfoList) {
                var i, length, optionIndex, optionValue, changedInfo, option = {};
                var options = ['slideshowSpeed', 'playMusic'];
                mPhotoOption = a.doGetPhotoOption();
                length = changedInfoList.length;
                for (i = 0; i < length; i += 1) {
                    changedInfo = changedInfoList[i];
                    optionIndex = changedInfo.index;
                    optionValue = changedInfo.settingItem.config.selectionList[changedInfo.changedValue].value;
                    option[options[optionIndex]] = optionValue;
                    mPhotoOption[options[optionIndex]] = optionValue;
                }
                a.doSetPhotoOption(option);
                clearInterval(a._sliderTimer);
                fnUpdateSlideshowSpeed(a);
                return option;
            };
            settingDlg.doKeyEscape = function () {
                return settingDlg.doKeyBack();
            };
            settingDlg.open({
                transition: {
                    newLayer: 'fadeIn'
                },
                dimm: true,
                onClosed: function () {
                    clearInterval(a._sliderTimer);
                    fnUpdateSlideshowSpeed(a);
                }
            });
            return true;
        };
        var fnAdjustSize = function (resolution) {
            var width = MAX_WIDTH;
            var height = MAX_HEIGHT;
            var widthRatio = 1;
            var heightRatio = 1;
            if (resolution) {
                var size = resolution.split('x');
                if (size.length === 2) {
                    width = size[0];
                    height = size[1];
                    if (width > MAX_WIDTH || height > MAX_HEIGHT) {
                        widthRatio = width / MAX_WIDTH;
                        heightRatio = height / MAX_HEIGHT;
                        if (widthRatio > heightRatio) {
                            width = 1280;
                            height = 'auto';
                        }
                        else {
                            width = 'auto';
                            height = 720;
                        }
                    }
                }
            }
            width = width === 'auto' ? width : width + 'px';
            height = height === 'auto' ? height : height + 'px';
            return hx.util.format('width:{0};height:{1};', width, height);
        };
        var fnCreateThumbnailSlider = function (a, items, startIndex) {
            mPhotoOption = a.doGetPhotoOption();
            a._parent.createLayer({
                addClass: 'thumbnailSlider',
                fnAfterCreate: function () {
                    var childControl = [];
                    var thumbnailSlider = new CCarouselControl(null);
                    thumbnailSlider.setData(items);
                    thumbnailSlider.setCurrentIndex(startIndex);
                    thumbnailSlider.setDrawParam({
                        viewCount: 9,
                        anchorIndex: 4,
                        itemWidth: 175,
                        itemHeight: 175,
                        anchorWidth: 195,
                        transparentAnchor: false,
                        align: 'horizontal',
                        animation: true,
                        animationInterval: 0.5,
                        maxKeyQueueCount: 0,
                        applyOnce: {
                            drawEffect: 'spreadOut'
                        }
                    });
                    thumbnailSlider.setAnchorDrawer(function (param) {
                        var anchorItem = param.currentItem;
                        var $anchor = param.$anchor;
                        var title = anchorItem.data.getTitle();
                        $anchor.addClass('muAnchorItem');
                    });
                    thumbnailSlider.setDataDrawer(function (param) {
                        var $item = param.$item;
                        var item = param.item;
                        var src = item.getThumbnailUrl();
                        var $img = $('<img>', {
                            'src': src,
                            'style': "display:none;",
                        });
                        $img[0].onload = function () {
                            this.style.display = 'block';
                        };
                        $item.append($img);
                        $item.addClass('border');
                    });
                    thumbnailSlider.doKeyOption = function () {
                        return fnCreatePhotoOption(a, a._parent);
                    };
                    thumbnailSlider.onFocusChanged = function (param) {
                        var item = param.item;
                        var currentIndex = thumbnailSlider.getCurrentIndex();
                        var title = item.getTitle();
                        param.$anchor.html(hx.util.format('<div class="title">{0}</div>', title));
                        // clearTimeout(mTimer);
                        // mTimer = setTimeout(function() {
                        // a.onThumbnailSliderFocusChanged(currentIndex);
                        // }, 300);
                    };
                    thumbnailSlider.onItemSelected = function (param) {
                        a._parent.removeLayer();
                        a._slider.setCurrentIndex(thumbnailSlider.getCurrentIndex());
                        a._slider.draw();
                        a._playMusic = false;
                        a.play();
                    };
                    thumbnailSlider.doKeyPlay = function () {
                        a._parent.removeLayer();
                        a._slider.setCurrentIndex(thumbnailSlider.getCurrentIndex());
                        a._slider.draw();
                        a.play();
                        return true;
                    };
                    thumbnailSlider.doKeyBack = function () {
                        a._parent.removeLayer();
                        return true;
                    };
                    childControl.push(thumbnailSlider);
                    this.setChildControl(childControl);
                    var $optionMenuInd = $('<div>', {
                        'class': 'optionMenuInd'
                    });
                    $optionMenuInd.text(tr('LOC_STR_PLAY_MODE_ID'));
                    $('.thumbnailSlider').append($optionMenuInd);
                }
            });
            a._parent.draw();
        };
        var CPhotoPlaybackDlg = (function (_super) {
            __extends(CPhotoPlaybackDlg, _super);
            function CPhotoPlaybackDlg(layeredGroupControl) {
                _super.call(this);
                this._startIndex = 0;
                this._bgmItemList = null;
                this._playMusic = true;
                this.init(layeredGroupControl);
                this.addClass('photoPlayback');
                this._startIndex = 0;
            }
            CPhotoPlaybackDlg.prototype.setData = function (items) {
                this._items = items;
                if (this._slider) {
                    this._slider.setData(this._items);
                    this._slider.setCurrentIndex(this._startIndex);
                    clearInterval(this._sliderTimer);
                    fnUpdateSlideshowSpeed(this);
                    this._slider.draw();
                }
            };
            CPhotoPlaybackDlg.prototype.setBGMList = function (bgmItemList) {
                this._bgmItemList = bgmItemList;
            };
            CPhotoPlaybackDlg.prototype.getPrevPlayItem = function () {
                var a = this;
                var prevIndex = null;
                var prevPlayItem = null;
                a._selectedIndex = a._selectedIndex || 0;
                if (a._bgmItemList) {
                    if (a._bgmItemList.length > a._selectedIndex) {
                        if (0 === a._selectedIndex) {
                            prevIndex = a._bgmItemList.length - 1;
                            prevPlayItem = a._bgmItemList[prevIndex];
                            a._selectedIndex = prevIndex;
                        }
                        else if (0 > a._selectedIndex) {
                            prevIndex = a._selectedIndex -= 1;
                            prevPlayItem = a._bgmItemList[prevIndex];
                        }
                    }
                }
                return prevPlayItem;
            };
            CPhotoPlaybackDlg.prototype.getNextPlayItem = function () {
                var a = this;
                var nextIndex = null;
                var nextPlayItem = null;
                a._selectedIndex = a._selectedIndex || 0;
                if (a._bgmItemList) {
                    if (a._bgmItemList.length > a._selectedIndex) {
                        if (a._bgmItemList.length === a._selectedIndex + 1) {
                            nextIndex = 0;
                            nextPlayItem = a._bgmItemList[nextIndex];
                            a._selectedIndex = 0;
                        }
                        else if (a._bgmItemList.length > a._selectedIndex + 1) {
                            nextIndex = a._selectedIndex += 1;
                            nextPlayItem = a._bgmItemList[nextIndex];
                        }
                    }
                }
                return nextPlayItem;
            };
            CPhotoPlaybackDlg.prototype.setStartIndex = function (index) {
                this._startIndex = index;
            };
            CPhotoPlaybackDlg.prototype.doOpen = function () {
                var a = this;
                var initialFocus = 0;
                var childControl = [];
                var slider = new CCarouselControl(null);
                var operationDisabled = false;
                mPhotoOption = a.doGetPhotoOption();
                if (a._items && a._items.length) {
                    operationDisabled = !a._items[0].isOperationEnabled();
                }
                slider.setData(a._items);
                slider.setCurrentIndex(a._startIndex);
                slider.setDrawParam({
                    viewCount: 1,
                    anchorIndex: 0,
                    itemWidth: 1280,
                    anchorWidth: 1280,
                    transparentAnchor: false,
                    align: 'horizontal',
                    animation: true,
                    animationInterval: 0.5,
                    maxKeyQueueCount: 0
                });
                slider.setAnchorDrawer(function (param) {
                    var anchorItem = param.currentItem;
                    param.$anchor.addClass('muAnchorItem');
                    param.$anchor.html(anchorItem);
                });
                slider.setDataDrawer(function (param) {
                    var $item = param.$item;
                    var item = param.item;
                    var resolution = item.getResolution();
                    var fullPath = item.getFullPath();
                    var largeThumbnailUrl = item.getLargeThumbnailUrl();
                    var isOverMaxResolution = hx.il.isOverMaxResolution(resolution);
                    var src = isOverMaxResolution ? '' : fullPath;
                    src = isOverMaxResolution ? largeThumbnailUrl : src;
                    var path = src.slice(0, src.lastIndexOf(item.getTitle()));
                    var fileName = src.slice(src.lastIndexOf(item.getTitle()), src.length);
                    fileName = encodeURIComponent(fileName);
                    src = path + fileName;
                    prism.log('largeThumbnailUrl : ' + largeThumbnailUrl);
                    var size = fnAdjustSize(resolution);
                    $item.find('.item').remove();
                    var $box = $('<div>', {
                        'class': 'item'
                    });
                    var $img = $('<img>', {
                        'style': hx.util.format("display:none;{0}", size)
                    });
                    var openNotificationDlg = function (msg, timeout) {
                        if (a._notificationDlg) {
                            a._notificationDlg.close();
                        }
                        var notificationDlg = prism.createNotificationDlg(a._parent, msg);
                        notificationDlg.setTimeout(timeout || 1000);
                        notificationDlg.open({
                            transition: {
                                newLayer: 'moveDown'
                            }
                        });
                        a._notificationDlg = notificationDlg;
                    };
                    if (isOverMaxResolution) {
                        var errorMsg = tr('LOC_FILE_SIZE_LIMIT_EXCEEDED_3300_2500_ID');
                        openNotificationDlg(errorMsg, 3000);
                    }
                    $img[0].onload = function () {
                        $img.fadeIn(1000);
                    };
                    $img[0].onerror = function () {
                        if (isOverMaxResolution) {
                            var errorMsg = tr('LOC_FILE_SIZE_LIMIT_EXCEEDED_3300_2500_ID');
                            openNotificationDlg(errorMsg);
                        }
                        else {
                            openNotificationDlg(tr('LOC_MSG_PHOTO_ERROR_01_ID'), 3000);
                        }
                    };
                    $img.attr('src', src);
                    if (operationDisabled) {
                        $item.removeClass('-f');
                    }
                    $box.append($img).appendTo($item);
                });
                slider.doKeyLeft = function () {
                    mDirection = TDirection.ELeft;
                    CCarouselControl.prototype.doKeyLeft.call(slider);
                    return true;
                };
                slider.doKeyRight = function () {
                    mDirection = TDirection.ERight;
                    CCarouselControl.prototype.doKeyRight.call(slider);
                    return true;
                };
                slider.doKeyUp = function () {
                    return true;
                };
                slider.doKeyDown = function () {
                    return true;
                };
                slider.doKeyPlay = function () {
                    a._parent.removeLayer();
                    a.play();
                    return true;
                };
                slider.onFocusStartToChange = function (param) {
                    slider._$self.find('.-carousel-dist0').fadeOut(500);
                };
                slider.onFocusChanged = function (param) {
                    param.$anchor.html(param.item);
                };
                slider.onItemSelected = function (param) {
                    if (!operationDisabled) {
                        a.pause();
                        fnCreateThumbnailSlider(a, a._items, slider.getCurrentIndex());
                    }
                };
                slider.doKeyOption = function () {
                    if (!operationDisabled) {
                        clearInterval(a._sliderTimer);
                        return fnCreatePhotoOption(a, a._parent);
                    }
                    return true;
                };
                a._slider = slider;
                childControl.push(slider);
                a.setChildControl(childControl, initialFocus);
                a._$play = $('<div>', {
                    'class': 'play'
                });
                a._$pause = $('<div>', {
                    'class': 'pause'
                });
                a._$self.append(a._$play).append(a._$pause);
                mPhotoOption = a.doGetPhotoOption();
                a.play();
            };
            CPhotoPlaybackDlg.prototype.doPlay = function (item) {
                var a = this;
                var bIsPhotoPlayback = a.isPhotoPbOngoing();
                mPhotoOption = a.doGetPhotoOption();
                var playMusic = mPhotoOption.playMusic;
                if (item && bIsPhotoPlayback) {
                    return;
                }
                clearInterval(a._sliderTimer);
                if (Number(mPhotoOption.slideshowSpeed) !== 0) {
                    a._$play.css('opacity', 1);
                    setTimeout(function () {
                        a._$play.css('opacity', 0);
                    }, 3000);
                    fnUpdateSlideshowSpeed(a);
                    if ((Number(playMusic) === 1) && a._playMusic) {
                        a.doRequestPlayBGM();
                    }
                    a._playMusic = true;
                }
                return true;
            };
            CPhotoPlaybackDlg.prototype.pause = function () {
                var a = this;
                a.setSpeed(0);
                a.doPause();
            };
            CPhotoPlaybackDlg.prototype.doPause = function () {
                var a = this;
                if (a._sliderTimer) {
                    a._$pause.css('opacity', 1);
                    setTimeout(function () {
                        a._$pause.css('opacity', 0);
                    }, 3000);
                    clearInterval(a._sliderTimer);
                }
            };
            CPhotoPlaybackDlg.prototype.doHide = function () {
                var a = this;
                a.stop();
                a._parent.removeLayer();
                a.close();
                return true;
            };
            CPhotoPlaybackDlg.prototype.doPrev = function () {
                var a = this;
                a._slider.doKeyLeft();
            };
            CPhotoPlaybackDlg.prototype.doNext = function () {
                var a = this;
                a._slider.doKeyRight();
            };
            CPhotoPlaybackDlg.prototype.doStop = function () {
                var a = this;
                clearInterval(a._sliderTimer);
            };
            CPhotoPlaybackDlg.prototype.doKeyEscape = function () {
                var a = this;
                a.stop();
                a._parent.removeLayer();
                return false;
            };
            CPhotoPlaybackDlg.prototype.doKeyStop = function () {
                var a = this;
                a.stop();
                a._parent.removeLayer();
                return true;
            };
            CPhotoPlaybackDlg.prototype.doKeyPlay = function () {
                var a = this;
                a.play();
                return true;
            };
            CPhotoPlaybackDlg.prototype.doKeyPause = function () {
                var a = this;
                a.pause();
                fnCreateThumbnailSlider(a, a._items, a._slider.getCurrentIndex());
                return true;
            };
            CPhotoPlaybackDlg.prototype.doKeyBack = function () {
                var a = this;
                a._parent.removeLayer();
                return true;
            };
            CPhotoPlaybackDlg.prototype.doFastForward = function () {
                var a = this;
                a._slider.doKeyRight();
                //return true;
            };
            CPhotoPlaybackDlg.prototype.doFastRewind = function () {
                var a = this;
                a._slider.doKeyLeft();
                //return true;
            };
            CPhotoPlaybackDlg.prototype.onThumbnailSliderFocusChanged = function (currentIndex) {
                var a = this;
                a._slider.setCurrentIndex(currentIndex);
                a._slider.draw();
            };
            return CPhotoPlaybackDlg;
        })(__PRISM_CPLAYBACKDLG__.CPlaybackDlg);
        mPrism.CPhotoPlaybackDlg = CPhotoPlaybackDlg;
        if (!window.prism) {
            window.prism = {};
        }
        prism.createPhotoPlaybackDlg = function (layeredGroupControl) {
            var photoPlayback = new CPhotoPlaybackDlg(layeredGroupControl);
            return photoPlayback;
        };
    })(mPrism || (mPrism = {}));
    return mPrism;
});

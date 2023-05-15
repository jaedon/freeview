// file name : common.lib/common.js
/// <reference path="../../def/jquery.d.ts"/>
/// <reference path="../../def/require.d.ts"/>
/// <reference path="../../def/dateformat.d.ts"/>

interface Date {
    addTime(time: number);
    toSeconds();
    toDays(): number;
    addDays(days: number);
    toGMTString(): string;
}
interface String {
    startsWith(str: string);
    endsWith(str: string);
    format(text);
}
//interface Array {
//    remove(from, to?): Array;
//    swap(x, y): void;
//}
//Array.prototype.swap = function (x, y) {
//    var b = this[x];
//    this[x] = this[y];
//    this[y] = b;
//    return this;
//};
declare var escape: (str: string) => string;
declare var unescape: (str: string) => string;

Date.prototype.addTime = function (time) {
    var a = this;
    a.setTime(a.getTime() + time);
};
Date.prototype.toSeconds = function () {
    var a = this;
    return Math.ceil(a.getTime() / 1000);
};
Date.prototype.toDays = function () {
    var a = this;
    var day = new Date(a);
    day.setMilliseconds(0);
    day.setSeconds(0);
    day.setMinutes(0);
    day.setHours(0);
    return Math.ceil(day.getTime() / 86400000);
};
Date.prototype.addDays = function (days) {
    var dat = new Date(this.valueOf());
    dat.setDate(dat.getDate() + days);
    return dat;
};
String.prototype.startsWith = function (str) {
    return this.slice(0, str.length) == str;
};
String.prototype.endsWith = function (str) {
    return this.indexOf(str, this.length - str.length) !== -1;
};
/*
Array.min = function (array) { return Math.min.apply(Math, array); };
Array.max = function (array) { return Math.max.apply(Math, array); };
*/
interface HTMLImageElement {
    decode(success, fail);
}

HTMLImageElement.prototype.decode = function (success, fail) {
    var a = this;
    a.decodedImage = new Image();
    a.decodedImage.onload = function () {
        if (success) {
            success(a.src);
        }
    };
    a.decodedImage.onerror = function () {
        if (fail) {
            fail(a.src);
        }
    };
    a.decodedImage.src = a.src;
};
var setTimeout_w = function (callback, time_sec, time_to_check?) {
    if (time_sec <= 0) {
        if (callback) {
            callback();
        }
        return 0;
    }

    var tick = 0,
        time_check = typeof time_to_check === 'number' ? time_to_check : 30;
    var interval_id = setInterval(function () {
        tick += time_check;
        console.log("[FUNCTION : setTimeout_w] [tick] " + tick + " [time_to_check] " + time_to_check + " [time_sec] " + time_sec);
        if (tick >= time_sec) {
            if (typeof callback === 'function') {
                callback();
                console.log('[FUNCTION : setTimeout_w] run callback]');
            }
            clearInterval(interval_id);
        }
    }, time_check * 1000);

    return interval_id;
};

/*
* Event
*/
/*
 * class CStringBuilder
 */
function CStringBuilder(value?) {
    this.strings = new Array("");
    this.append(value);
};
// Appends the given value to the end of this instance.
CStringBuilder.prototype.append = function (value) {
    if (value) {
        this.strings.push(value);
    }
};
// Clears the string buffer
CStringBuilder.prototype.clear = function () {
    this.strings.length = 1;
};
// Converts this instance to a String.
CStringBuilder.prototype.toString = function () {
    return this.strings.join("");
};

function StringFormat(text, ...args: any[]) {
    if (arguments.length <= 1) {
        return text;
    }
    var token, tokenCount = arguments.length - 2;
    for (token = 0; token <= tokenCount; token += 1) {
        text = text.replace(new RegExp("\\{" + token + "\\}", "gi"), arguments[token + 1]);
    }
    return text;
};
function StringEllipsis(text, length) {
    if (text) {
        //console.log(StringFormat('String.ellipsis length: {0}, text: {1}', text.length, text));
        if (typeof (text) == 'string') {
            if (text.length > length) {
                return text.substring(0, length) + '...';
            }
            else {
                return text;
            }
        }
    }
    return '';
};

/*
 * class CCollection
 */
var CCollection = function () { };
CCollection.prototype = new Array();
CCollection.prototype.item = function (index) {
    return this[index];
};
/*
 * Making time string utils
 */
// http://thudjs.tumblr.com/post/637855087/stylesheet-onload-or-lack-thereof
function loadStyleSheet(path, fn, scope?) {
    var head = document.getElementsByTagName('head')[0], // reference to
        // document.head for
        // appending/
        // removing link
        // nodes
        link = document.createElement('link'); // create the link node
    link.setAttribute('href', path);
    link.setAttribute('rel', 'stylesheet');
    link.setAttribute('type', 'text/css');

    var sheet, cssRules;
    // get the correct properties to check for depending on the browser
    if ('sheet' in link) {
        sheet = 'sheet';
        cssRules = 'cssRules';
    } else {
        sheet = 'styleSheet';
        cssRules = 'rules';
    }

    var interval_id = setInterval(function () { // start checking whether the
        // style sheet has successfully
        // loaded
        try {
            if (link[sheet] && link[sheet][cssRules].length) { // SUCCESS! our
                // style sheet
                // has loaded
                clearInterval(interval_id); // clear the counters
                clearTimeout(timeout_id);
                fn.call(scope || window, true, link); // fire the callback
                // with success == true
            }
        } catch (e) {
        } finally {
        }
    }, 10), // how often to check if the stylesheet is loaded
        timeout_id = setTimeout(function () { // start counting down till fail
            clearInterval(interval_id); // clear the counters
            clearTimeout(timeout_id);
            head.removeChild(link); // since the style sheet didn't load, remove the
            // link node from the DOM
            fn.call(scope || window, false, link); // fire the callback with
            // success == false
        }, 1000); // how long to wait before failing

    head.appendChild(link); // insert the link node into the DOM and start
    // loading the style sheet

    return link; // return the link node;
};

var loadImage = function (targetUrl, callback, errorCallback) {
    function ProcessResponse()
    {
        if(xhr.readyState==4)
        {
            console.log('loadImage xhr.status'+xhr.status);
            if (xhr.status==200)
            {
                callback(xhr.response);
            } else {
                errorCallback(xhr.status);
            }
        }
    }
    var xhr = new XMLHttpRequest();
    xhr.open("GET",targetUrl, true);
    xhr.onreadystatechange=ProcessResponse;
    xhr.responseType = "blob";
    xhr.send(null);
};
var decodeText = function (text) {
    try {
        return decodeURIComponent(escape(text));
    }
    catch (e) {
        return text;
    }
};

function removeStyleSheet(link) {
    var head = document.getElementsByTagName('head')[0];
    head.removeChild(link);
}

var isEmulMode = (typeof window["oipfObjectFactory"] === 'undefined');
/*
 **
 * GMT -> HH:MM Converter
 * second based GMT Converter
 * ! not millisecond
 **
 */
function gmtConverter(gmt) {
    var _gmt = new Date(gmt * 1000);
    return _gmt.format("HH:MM");
}
/*
$.fn.extend({
    sorter: function () {
        return this.each(function () {
            var sorted = $(this).children('li').sort(function (a, b) {
                // Find the list items and sort them
                return $(a).text().toLowerCase() > $(b).text().toLowerCase() ? 1 : -1;
            });
            $(this).append(sorted);
        });
    }
});
*/
// Array Remove - By John Resig (MIT Licensed)
Array.prototype["remove"] = function (from, to?) {
    var rest = this.slice((to || from) + 1 || this.length);
    this.length = from < 0 ? this.length + from : from;
    return this.push.apply(this, rest);
};

function extend(a, b) {
    var prop;
    for (prop in b) {
        if (b[prop] === undefined) {
            delete a[prop];

            // Avoid "Member not found" error in IE8 caused by setting window.constructor
        } else if (prop !== "constructor" || a !== window) {
            a[prop] = b[prop];
        }
    }

    return a;
}

interface JQuery {
    createLayer(param?);
    makeDimm(dimmingClass?, aPrepend?);
    anim(prop, value, cb?);
    absoluteClone(param?);
    afterTransitionOneShot(handler, aWithEvent: boolean);
    afterTransition(handler);
    cssLater(...args: any[]);
    fixedClone(param?);
    setTransition(value);
}

(function ($) {

    var KCssPropTransition = 'transition';
    var KCssEvntTransitionEnd = 'transitionend';
    var KCssTransitionDuration = 'transition-duration';

    var browser = (function () {
        var N = navigator.appName, ua = navigator.userAgent, tem;
        var M = ua.match(/(opera|chrome|safari|firefox|msie)\/?\s*(\.?\d+(\.\d+)*)/i);
        if (M && (tem = ua.match(/version\/([\.\d]+)/i)) != null) M[2] = tem[1];
        M = M ? [M[1], M[2]] : [N, navigator.appVersion, '-?'];
        return M;
    } ());

    if (browser[0] == "chrome") {
        KCssPropTransition = '-webkit-transition';
        KCssEvntTransitionEnd = 'webkitTransitionEnd';
        KCssTransitionDuration = '-webkit-transition-duration';
    } else if (browser[0] == "opera") {
        KCssPropTransition = '-o-transition';
        KCssEvntTransitionEnd = 'oTransitionEnd otransitionend';
        KCssTransitionDuration = '-o-transition-duration';
    } else if (browser[0] == "msie") {
        KCssPropTransition = '-ms-transition';
        KCssEvntTransitionEnd = 'msTransitionEnd mstransitionend';
        KCssTransitionDuration = '-ms-transition-duration';
    }

    var KCssTransitionParamPos = 'top .3s linear, left .3s linear';
    var KCssTransitionParamOpa = 'opacity .3s linear';

    $.fn.makeDimm = function (dimmingClass?, aPrepend = false) {
        var target = this;
        // console.log('makeDimm [' + target.attr('id') + ', ' + target.position().left + ', ' + target.position().top + ']');
        var dimmer = $('<div></div>');
        dimmer.css({
            'position': 'absolute',
            'left': target.position().left + 'px',
            'top': target.position().top + 'px',
            'width': target.width(),
            'height': target.height(),
        });
        var dimmClass = dimmingClass ? dimmingClass : 'dlgDimming';
        dimmer.addClass(dimmClass);
        if (aPrepend) {
            target.before(dimmer);
        } else {
            target.parent().append(dimmer);
        }
        return dimmer;
    };
    $.fn.fixedClone = function (param?) {
        var target = this;
        // console.log('fixedClone [' + target.attr('id') + ', ' + target.position().left + ', ' + target.position().top + ']');
        //var clone = $('<div></div>');
        var children = param ? param.children : false;
        var clone = target.clone().attr('id', '');
        if (!children) {
            clone.empty();
        }
        clone.css({
            'position': 'fixed',
            'left': target.offset().left + 'px',
            'top': target.offset().top + 'px',
            'width': target.width(),
            'height': target.height()
        });
        if (param) {
            if (param.id) {
                clone.attr('id', param.id);
            }
        }
        return clone;
    };
    $.fn.absoluteClone = function (param?) {
        var target = this;
        // console.log('absoluteClone [' + target.attr('id') + ', ' + target.position().left + ', ' + target.position().top + ']');
        var clone = $('<div></div>');
        // var clone = target.clone();
        clone.css({
            'position': 'absolute',
            'left': parseInt(target.position().left, 10) + 'px',
            'top': parseInt(target.position().top, 10) + 'px',
            'width': parseInt(target.outerWidth(), 10) + 'px',
            'height': parseInt(target.outerHeight(), 10) + 'px',
        });
        if (param) {
            if (param.id) {
                clone.attr('id', param.id);
            }
        }
        if (param && param.prepend) {
            target.parent().prepend(clone);
        } else {
            target.parent().append(clone);
        }
        return clone;
    };
    $.fn.createLayer = function (aParam?) {
        var target = this;
        //console.log('createLayer [' + target.attr('id') + ', ' + target.position().left + ', ' + target.position().top + ']');
        var layer = $('<div></div>');
        layer.css({
            'position': 'absolute',
            'width': target.width(),
            'height': target.height(),
        });
        if (aParam) {
            if (aParam.id) {
                layer.attr('id', aParam.id);
            }
            if (aParam.clas) {
                layer.addClass(aParam.clas);
            }
        }
        target.append(layer);
        return layer;
    };
    $.fn.afterTransition = function (handler) {
        var target = this;
        target.bind(KCssEvntTransitionEnd, function (e) {
            if (e.target == this) {
                handler(e);
            }
        });
        return target;
    };
    $.fn.cssLater = function (...aArgs: any[]) {
        var args = $.makeArray(arguments);
        var self = $(this);
        setTimeout(function () {
            self.css.apply(self, args);
        }, 1);
        return self;
    };
    $.fn.afterTransitionOneShot = function (aCallback: Function, aWithEvent: boolean = false) {
        var target = this;
        if (aWithEvent) {
            var element = target[0];
            var durations = ['0'];
            if (getComputedStyle && getComputedStyle(element).transitionDuration) {
                durations = getComputedStyle(element).transitionDuration.split(',');
            } else {
                durations = element.style.transitionDuration.split(',');
            }

            var delay = parseFloat(durations[0].replace('s', '')) * 1000 || 300;
            var timeout;
            if (delay) {
                timeout = setTimeout(aCallback, delay * 3);
            } else {
                throw "Invalid transiton duration";
            }

            target.one(KCssEvntTransitionEnd, function () {
                clearTimeout(timeout);
                aCallback();
            });
        } else {
            var element = target[0];
            var durations = ['0'];
            if (getComputedStyle && getComputedStyle(element).transitionDuration) {
                durations = getComputedStyle(element).transitionDuration.split(',');
            } else {
                durations = element.style.transitionDuration.split(',');
            }

            var delay = parseFloat(durations[0].replace('s', '')) * 1000 || 300;
            if (delay) {
                setTimeout(aCallback, delay);
            } else {
                throw "Invalid transiton duration";
            }
        }
        return target;
    };
    $.fn.makeHighlight = function (highLightClassName, $dimmTarget, dimmClassName) {
        var $a = this;
        var $clone = $a.clone().insertAfter($a);
        var offset = $a.offset();
        $clone.css({
            'position': 'fixed',
            'z-index': '10',
            'top': offset.top,
            'left': offset.left,
            'width': $a.width(),
            'height': $a.height(),
            'margin-left': '0px'
        });
        if (highLightClassName) {
            $clone.addClass(highLightClassName);
        }
        if ($dimmTarget) {
            var $dimmer = $dimmTarget.fixedClone().insertBefore($a.first());
            if (dimmClassName) {
                $dimmer.addClass(dimmClassName);
            }
        }
        return {
            $highlight: $clone,
            $dimm: $dimmer
        };
    };
    $.fn.anim = function (prop, value, cb) {
        var $self = this;
        // var animClass = '-global-anim-' + prop;
        // $self.addClass(animClass);
        $self.cssLater(prop, value);
        $self.afterTransitionOneShot(function () {
            // $self.removeClass(animClass);
            if (cb) {
                cb();
            }
        });
    };
    $.fn.setTransition = function (value) {
        var el: HTMLElement = this[0];
        el.style.transition = value;
    };
})(jQuery);

class CCircularArray {

    _array;
    _idxCur;
    _idxLast;

    constructor(array?) {
        var a = this;
        if (array) {
            a.setArray(array);
        }
    }
    destroy() {
        delete this._array;
        delete this._idxCur;
        delete this._idxLast;
    }
    getArray() {
        var a = this;
        return a._array;
    }
    setArray(array) {
        var a = this;
        a._array = array;
        a._idxCur = 0;
        a._idxLast = array.length - 1;
    }
    appendArray(array) {
        var a = this;
        a._array = a._array.concat(array);
        a._idxLast = array.length - 1;
    }
    cur(): number {
        var a = this;
        return a._idxCur;
    }
    curItem() {
        var a = this;
        return a._array[a._idxCur];
    }
    setCur(index) {
        var a = this;
        a._idxCur = a.indexing(index);
    }
    inc(count?) {
        var a = this;
        count = typeof count != 'undefined' ? count : 1;
        a._idxCur = a.indexing(a._idxCur + count);
    }
    dec(count?) {
        var a = this;
        count = typeof count != 'undefined' ? count : 1;
        a._idxCur = a.indexing(a._idxCur - count);
    }
    indexing(index) {
        var ret;
        var length = this._array.length;
        if (index >= 0) {
            ret = index % length;
        } else {
            ret = length + index % length;
            if (ret === length) {
                ret = 0;
            }
        }
        return ret;
    }
    at(index) {
        return this._array[this.indexing(index)];
    }
    get(count, offset) {
        var a = this;
        var CResult = function () {
        };
        CResult.prototype = new Array();
        CResult.prototype.firstAvailIndex = function () {
            var i, a = this;
            for (i = 0; i < a.length; i += 1) {
                if (a[i]) return i;
            }
            return -1;
        };
        CResult.prototype.lastAvailIndex = function () {
            var i, a = this;
            for (i = a.length - 1; i >= 0; i -= 1) {
                if (a[i]) return i;
            }
            return -1;
        };
        var ret = new CResult();
        offset = typeof offset != 'undefined' ? offset : 0;
        var len = a._array.length;
        var i;

        if (count > len) {
            var halfCount = Math.floor(len / 2);
            var centering = halfCount <= offset && halfCount <= count - offset;

            for (i = 0; i < count; i += 1) {
                ret.push(null);
            }
            if (centering) {
                var start = offset - halfCount;
                for (i = 0; i < len; i += 1) {
                    ret[i + start] = a.at(a._idxCur - halfCount + i);
                }
            } else if (len - 1 <= offset) { // has enough space in upper index.
                var start = offset - len - 1;
                for (i = 0; i < len; i += 1) {
                    ret[i + start] = a.at(a._idxCur + i + 1);
                }
            } else if (len <= count - offset) { // has enough space in lower index.
                var start = <number>offset;
                for (i = 0; i < len; i += 1) {
                    ret[i + start] = a.at(a._idxCur + i);
                }
            } else {
                for (i = 0; i < len; i += 1) {
                    ret[i] = a.at(a._idxCur - offset + i);
                }
            }
        }
        else {
            for (i = 0; i < count; i += 1) {
                ret.push(a.at(a._idxCur - offset + i));
            }
        }
        return ret;
    }
    del(index) {
        var a = this;
        var compromised = a.indexing(index);
        if (isNaN(compromised)) {
            return;
        }
        a._array.splice(compromised, 1);
    }

    each(fn, offset?) {
        var a = this;
        var index = offset ? a.indexing(a._idxCur + offset) : a._idxCur;
        var need = false;
        do {
            need = fn(index, a.at(index));
            index = a.indexing(index + 1);
        } while (need && index != a._idxCur);
    }

    eachReverse(fn) {
        var a = this;
        var index = a._idxCur;
        var need = false;
        do {
            need = fn(index, a.at(index));
            index = a.indexing(index - 1);
        } while (need && index != a._idxCur);
    }

    eachArray(fn) {
        var a = this;
        var index = a._idxCur;
        var need = false;
        do {
            if (index < a._array.length) {
                need = fn(index, a.at(index));
            } else {
                need = fn(-1, undefined);
            }
            index++;
        } while (need);
    }
    length() {
        var a = this;
        var ret = 0;
        if (a._array) {
            ret = a._array.length;
        }
        return ret;
    }
    clone() {
        var a = this;
        var ret = new CCircularArray(a._array);
        ret.setCur(a._idxCur);
        return ret;
    }
}

function getISO639toUIString(value) {
    switch (value) {
        case 'alb': return 'Welsh';
        case 'ara': return 'Arabic';
        case 'cat': return 'Catalan';
        case 'ces': return 'Czech';
        case 'cym': return 'Welsh';
        case 'dan': return 'Danish';
        case 'deu': return 'German';
        case 'dut': return 'Dutch';
        case 'ell': return 'Greek';
        case 'eng': return 'English';
        case 'esl': return 'Spanish';
        case 'esp': return 'Spanish';
        case 'fao': return 'Faroese';
        case 'fas': return 'Persian';
        case 'fin': return 'Finnish';
        case 'fra': return 'French';
        case 'fre': return 'French';
        case 'gai': return 'Irish';
        case 'ger': return 'German';
        case 'gla': return 'Gaelic';
        case 'gle': return 'Irish';
        case 'gre': return 'Greek';
        case 'hrv': return 'Croatian';
        case 'hun': return 'Hungrian';
        case 'ice': return 'Icelandic';
        case 'iri': return 'Irish';
        case 'isl': return 'Icelandic';
        case 'ita': return 'Italian';
        case 'kor': return 'Korean';
        case 'nla': return 'Dutch';
        case 'nld': return 'Dutch';
        case 'nor': return 'Norwegian';
        case 'per': return 'Persian';
        case 'pol': return 'Polish';
        case 'por': return 'Portuguese';
        case 'ron': return 'Romanian';
        case 'rum': return 'Romanian';
        case 'rus': return 'Russian';
        case 'slk': return 'Slovak';
        case 'slo': return 'Slovak';
        case 'slv': return 'Slovenian';
        case 'sqi': return 'Albanian';
        case 'scr': return 'Croatian';
        case 'spa': return 'Spanish';
        case 'sve': return 'Swedish';
        case 'swe': return 'Swedish';
        case 'tha': return 'Thai';
        case 'tur': return 'Turkish';
        case 'wel': return 'Welsh';
        case 'und': return 'Undefined';
        default:
            return value;
    }
}


/*
* FileSysInfo Entity Objects
*/
function CFileSysInfo() {
    this._icons = {
        srcHDD: 'image/icon/video_list_icon_hdd.png',
        srcUSB: ['image/icon/video_list_icon_usb.png', 'image/icon/video_list_icon_usb01.png', 'image/icon/video_list_icon_usb02.png', 'image/icon/video_list_icon_usb01.png'],
        srcPlusFolder: 'image/icon/icon_folder_plus.png',
        srcEmptyFolder: 'image/icon/icon_folder.png',
        srcUpperFolder: 'image/icon/icon_folder_upper.png'
    };
};
CFileSysInfo.prototype.getName = function () {
    var a = this;
    var ret = new String();
    switch (a.type) {
        case 'drive':
            if (a.storageInterface == 1) {
                ret = 'HDD';
            } else {
                ret = 'USB ' + a.seq;
            }
            break;
        case 'folder':
            if (a._isUpperFolder) {
                ret = 'Upper Folder';
            } else {
                ret = a.fullPath.substring(a.fullPath.lastIndexOf("/") + 1, a.fullPath.length);
            }
            break;
        case 'file':
            ret = a.fullPath.substring(a.fullPath.lastIndexOf("/") + 1, a.fullPath.length);
            break;
    }
    return ret;
};
CFileSysInfo.prototype.getDriveName = function () {
    var a = this;
    var ret = new String();
    switch (a.type) {
        case 'drive':
            if (a.storageInterface == 1) {
                ret = 'HDD';
            } else {
                ret = 'USB ' + a.seq;
            }
            break;
        case 'folder':
        case 'file':
            var driveNum = '';
            if (a.fullPath.indexOf('/mnt/hd') >= 0) {
                ret = 'HDD';
            } else if (a.fullPath.indexOf('/media/drive1') >= 0) {
                ret = 'USB 1';
            } else if (a.fullPath.indexOf('/media/drive2') >= 0) {
                ret = 'USB 2';
            } else if (a.fullPath.indexOf('/media/drive3') >= 0) {
                ret = 'USB 3';
            } else {
                ret = 'Storages';
            }
            break;
    }
    return ret;
};
CFileSysInfo.prototype.getMountPath = function () {
    var a = this;
    var ret = '';
    ret = parseMountPath(a.fullPath);
    return ret;
};
CFileSysInfo.prototype.getDisplayPath = function () {
    var a = this;
    var ret = '';
    var words = ['/mnt/hd1', '/mnt/hd2', '/mnt/hd3', '/media/drive1', '/media/drive2', '/media/drive3'];
    var driveName = '[' + a.getDriveName() + '] ';
    var isHitted = false;
    var length = words.length;
    var path = '';
    if (a.type == 'folder' && a._isUpperFolder) {
        path = a.currentPath;
    } else {
        path = a.fullPath;
    }
    var i;
    for (i = 0; i < length; i += 1) {
        if (path.indexOf(words[i]) >= 0) {
            path = path.replace(words[i], '');
            break;
        }
    }
    ret = driveName + path;
    return ret;
};
CFileSysInfo.prototype.getTargetPath = function () {
    var a = this;
    return a.fullPath;
};
CFileSysInfo.prototype.getIcon = function () {
    return '';
};
CFileSysInfo.prototype.isValid = function () {
    return true;
};
CFileSysInfo.prototype.isUpperFolder = function () {
    return false;
};
CFileSysInfo.prototype.print = function () {
    var member, a = this;
    alert('Print CFileSystInfo');
    for (member in a) {
        if (typeof a[member] != 'function') {
            alert(member + ' = ' + a[member]);
        }
    };
};

function CDriveInfo(uuid, id, fullPath, storageInterface, kind, label, availableSize, formatType) {
    this.storageInterfaceEnum = { HDD: 1, USB: 2 };
    this.type = 'drive';
    this.uuid = uuid;
    this.id = id;
    this.fullPath = fullPath;
    this.storageInterface = storageInterface;
    this.kind = kind;
    this.label = label;
    this.availableSize = availableSize || 0;
    //eg. vfat, tntfs, xfs
    this.formatType = formatType || 'unknown';
    this.seq = this.fullPath.substring(this.fullPath.length - 1, this.fullPath.length);
};
CDriveInfo.prototype = new CFileSysInfo();

CDriveInfo.prototype.getIcon = function () {
    var a = this;
    var src = new String();
    if (a.storageInterface == a.storageInterfaceEnum.HDD) {
        src = a._icons.srcHDD;
    } else {
        src = a._icons.srcUSB[a.kind];
    }
    var icon = '<img src="' + src + '" />';
    return icon;
};
CDriveInfo.prototype.isValid = function () {
    var a = this;
    var ret = false;
    //if internal HDD then id==1 is valid, /mnt/hd2
    if (a.storageInterface == a.storageInterfaceEnum.HDD) {
        if (a.id == 1) {
            ret = true;
        }
    } else {
        ret = true;
    }
    return ret;
};


function CFolderInfo(fullPath, hasItem, isUpperFolder) {
    this.type = 'folder';
    this.fullPath = fullPath;
    this.hasItem = hasItem || 0;
    this._isUpperFolder = isUpperFolder || false;
    this.currentPath = null;
}
CFolderInfo.prototype = new CFileSysInfo();
CFolderInfo.prototype.getTargetPath = function () {
    var a = this;
    if (a._isUpperFolder) {
        return a.currentPath;
    }
    return a.fullPath;
};
CFolderInfo.prototype.getIcon = function () {
    var a = this;
    var src = new String();
    if (a._isUpperFolder) {
        src = a._icons.srcUpperFolder;
    } else {
        if (a.hasItem) {
            src = a._icons.srcPlusFolder;
        } else {
            src = a._icons.srcEmptyFolder;
        }
    }
    var icon = '<img src="' + src + '" />';
    return icon;
};
CFolderInfo.prototype.isValid = function () {
    var a = this;
    var ret = true;
    if (a.fullPath.indexOf('Tsr') >= 0) {
        ret = false;
    }
    return ret;
};
CFolderInfo.prototype.isUpperFolder = function () {
    var a = this;
    return a._isUpperFolder;
};


function CFileInfo(fullPath) {
    this.type = 'file';
    this.fullPath = fullPath;
};
CFileInfo.prototype = new CFileSysInfo();

/*
* End FileSysInfo Entity Objects
*/
/* CMediaContent control */
function parseMountPath(path) {
    if (!path || path.length == 0) {
        throw 'arguement path can not null or empty';
    }

    var ret = '';
    var tempPath = path;
    if (path.indexOf('file://') >= 0) {
        tempPath = path.replace('file://', '');
    }
    var matchs = tempPath.match('^/([a-z0-9_.$]+)/([a-z0-9_.$]+)');
    if (matchs && matchs.length > 0) {
        ret = matchs[0];
    } else {
        ret = path;
    }
    return ret;
};
function mc_getName(content) {
    var path = content.fullPath;
    return path.substring(path.lastIndexOf('/') + 1);
}
function mc_getPath(path) {
    var upper = path.lastIndexOf('/');
    var ret = path.substr(0, upper);
    while (upper != -1 && ret[upper - 1] == '/') {
        upper = ret.lastIndexOf('/');
        ret = ret.substr(0, upper);
    }
    return ret;
}

/* create element */
function createElement(aTag: string, aClass: string[], aParent: HTMLElement) {
    var target = document.createElement(aTag);
    if(aClass) {
        var i, len = aClass.length;
        for(i=0; i<len; i++) {
            target.classList.add(aClass[i]);
        }
    }
    aParent.appendChild(target);
    return target;
}

/* showcase */
enum TShowcaseGenreIcon {
    EFilm   //0
}
enum TShowcaseIcon {
    ERecording,
    ESeriesrec,
    EReminder,
    EPay,
    ESsound,
    EHd,
    EDolby,
    EGuidance,
    ESubtitle,
    ESigned,
    EAd,
    EThreed,
    EHdlinkage,
    ESeries,
    ESc,
    EOndemand
}
interface IShowcaseIcon {
    pay?: string;
    ssound?: string;
    hd?: string;
    dolby?: string;
    guidance?: string;
    guidancetext?: string;
    subtitle?: string;
    signed?: string;
    ad?: string;
    threed?: string;
    hdlinkage?: string;
    series?: string;
    sc?: string;
    ondemand?: string;
    rating?: string;
    recording?: boolean;
    reminder?: boolean;
    seriesrec?: boolean;
}
interface IShowcaseLink {
    ondemand_link? : any;
    promo_link? : any;
    restart_link? : any;
    events_link? : any;
}
interface IShowcaseEvent {
    sc_id: number;
    lock?: boolean;
    available: boolean;
    svc_id: number;
    fsvc_id: string;
    event_id: number;
    sc_title: string;
    sc_title2: string;
    sc_title3?: string;
    sc_description: string;
    sc_image: string;
    channel_logo: string;
    pick_flag?: string;
    startTime?: number;
    duration?: number;
    genre?: string;
    availability_text?: string;
    icons?: IShowcaseIcon;
    links: any;
    //for xml
    thumbnail_image?: string;
    large_image?: string;
    zoom_image?: string;
    title?: string;
    subtitle?: string;
    short_synopsis?: string;
    event_details?: {};
    isNew?: boolean;
    isHot?: boolean;
    programmeProvider?: any;
}

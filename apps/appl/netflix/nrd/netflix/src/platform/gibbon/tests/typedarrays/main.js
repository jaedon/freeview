var tests = [
    'ArrayBuffer-DataView-alloc-large-long-lived.js',
    'ArrayBuffer-DataView-alloc-long-lived.js',
    'ArrayBuffer-Int32Array-byteOffset.js',
    'ArrayBuffer-Int8Array-alloc-huge-long-lived.js',
    'ArrayBuffer-Int8Array-alloc-large-long-lived-fragmented.js',
    'ArrayBuffer-Int8Array-alloc-long-lived-buffer.js',
    'ArrayBuffer-Int8Array-alloc-long-lived.js',
    'ArrayBuffer-Int8Array-alloc.js',
    'Float64Array-to-Int16Array-set.js',
    'Int16Array-bubble-sort-with-byteLength.js',
    'Int16Array-bubble-sort.js',
    'Int16Array-load-int-mul.js',
    'Int16Array-to-Int32Array-set.js',
    'Int32Array-Int8Array-view-alloc.js',
    'Int32Array-alloc-huge-long-lived.js',
    'Int32Array-alloc-huge.js',
    'Int32Array-alloc-large-long-lived.js',
    'Int32Array-alloc-large.js',
    'Int32Array-alloc-long-lived.js',
    'Int32Array-alloc.js',
    'Int8Array-alloc-long-lived.js',
    'Int8Array-load-with-byteLength.js',
    'Int8Array-load.js',
    'array-with-int32-add-sub.js',
    'array-with-int32-or-double-sum.js',
    'dfg-bool-to-int32-reuse.js',
    'dfg-inline-arguments-become-int32.js',
    'dfg-inline-arguments-int32.js',
    'dfg-int16array.js',
    'dfg-int32-to-double-on-known-number.js',
    'dfg-int32-to-double-on-set-local-and-exit.js',
    'dfg-int32-to-double-on-set-local-and-sometimes-exit.js',
    'dfg-int32array-overflow-values.js',
    'dfg-int32array.js',
    'dfg-int52-change-format.js',
    'dfg-int52-spill-trickier.js',
    'dfg-int52-spill.js',
    'dfg-int8array.js',
    'dfg-rshift-by-zero-eliminate-valuetoint32.js',
    'dfg-uint16array.js',
    'dfg-uint32-to-number-in-middle-of-copy-propagation.js',
    'dfg-uint32-to-number-on-captured-variable.js',
    'dfg-uint32-to-number-skip-then-exit.js',
    'dfg-uint32-to-number.js',
    'dfg-uint32array-overflow-constant.js',
    'dfg-uint32array-overflow-values.js',
    'dfg-uint32array.js',
    'dfg-uint8array.js',
    'dfg-uint8clampedarray-out-of-bounds-put-by-val-alias.js',
    'dfg-uint8clampedarray.js',
    'dfg-value-to-int32-with-side-effect.js',
    'to-int32-boolean.js',
    'toInt32UInt32.js',
    'ArrayBuffer-Int8Array-alloc-large-long-lived.js', // the last two take a while to finish
    'Int16Array-alloc-long-lived.js'
];

function description(msg)
{
    nrdp.log.info('TEST: ' + msg);
}

function debug(msg)
{
    nrdp.log.console(msg);
}

function testPassed(msg)
{
    // nrdp.log.console('PASS ' + msg);
}

var failed = 0;
function testFailed(msg)
{
    ++failed;
    w2.text = '';
    nrdp.log.error('FAIL ' + msg);
}

function areArraysEqual(_a, _b)
{
    try {
        if (_a.length !== _b.length)
            return false;
        for (var i = 0; i < _a.length; i++)
            if (_a[i] !== _b[i])
                return false;
    } catch (ex) {
        return false;
    }
    return true;
}

function isMinusZero(n)
{
    // the only way to tell 0 from -0 in JS is the fact that 1/-0 is
    // -Infinity instead of Infinity
    return n === 0 && 1/n < 0;
}

function isResultCorrect(_actual, _expected)
{
    if (_expected === 0)
        return _actual === _expected && (1/_actual) === (1/_expected);
    if (_actual === _expected)
        return true;
    if (typeof(_expected) == "number" && isNaN(_expected))
        return typeof(_actual) == "number" && isNaN(_actual);
    if (Object.prototype.toString.call(_expected) == Object.prototype.toString.call([]))
        return areArraysEqual(_actual, _expected);
    return false;
}

function stringify(v)
{
    if (v === 0 && 1/v < 0)
        return "-0";
    else return "" + v;
}

function evalAndLog(_a)
{
    if (typeof _a != "string")
        debug("WARN: tryAndLog() expects a string argument");

    // Log first in case things go horribly wrong or this causes a sync event.
    debug(_a);

    var _av;
    try {
        _av = eval(_a);
    } catch (e) {
        testFailed(_a + " threw exception " + e);
    }
    return _av;
}

function shouldBe(_a, _b)
{
    if (typeof _a != "string" || typeof _b != "string")
        debug("WARN: shouldBe() expects string arguments");
    var exception;
    var _av;
    try {
        _av = eval(_a);
    } catch (e) {
        exception = e;
    }
    var _bv = eval(_b);

    if (exception)
        testFailed(_a + " should be " + _bv + ". Threw exception " + exception);
    else if (isResultCorrect(_av, _bv))
        testPassed(_a + " is " + _b);
    else if (typeof(_av) == typeof(_bv))
        testFailed(_a + " should be " + _bv + ". Was " + stringify(_av) + ".");
    else
        testFailed(_a + " should be " + _bv + " (of type " + typeof _bv + "). Was " + _av + " (of type " + typeof _av + ").");
}

function shouldBeTrue(_a) { shouldBe(_a, "true"); }
function shouldBeFalse(_a) { shouldBe(_a, "false"); }
function shouldBeNaN(_a) { shouldBe(_a, "NaN"); }
function shouldBeNull(_a) { shouldBe(_a, "null"); }

function shouldBeEqualToString(a, b)
{
    var unevaledString = '"' + b.replace(/\\/g, "\\\\").replace(/"/g, "\"").replace(/\n/g, "\\n").replace(/\r/g, "\\r") + '"';
    shouldBe(a, unevaledString);
}

function shouldEvaluateTo(actual, expected) {
    // A general-purpose comparator.  'actual' should be a string to be
    // evaluated, as for shouldBe(). 'expected' may be any type and will be
    // used without being eval'ed.
    if (expected == null) {
        // Do this before the object test, since null is of type 'object'.
        shouldBeNull(actual);
    } else if (typeof expected == "undefined") {
        shouldBeUndefined(actual);
    } else if (typeof expected == "function") {
        // All this fuss is to avoid the string-arg warning from shouldBe().
        try {
            actualValue = eval(actual);
        } catch (e) {
            testFailed("Evaluating " + actual + ": Threw exception " + e);
            return;
        }
        shouldBe("'" + actualValue.toString().replace(/\n/g, "") + "'",
                 "'" + expected.toString().replace(/\n/g, "") + "'");
    } else if (typeof expected == "object") {
        shouldBeTrue(actual + " == '" + expected + "'");
    } else if (typeof expected == "string") {
        shouldBe(actual, expected);
    } else if (typeof expected == "boolean") {
        shouldBe("typeof " + actual, "'boolean'");
        if (expected)
            shouldBeTrue(actual);
        else
            shouldBeFalse(actual);
    } else if (typeof expected == "number") {
        shouldBe(actual, stringify(expected));
    } else {
        debug(expected + " is unknown type " + typeof expected);
        shouldBeTrue(actual, "'"  +expected.toString() + "'");
    }
}

function shouldBeNonZero(_a)
{
    var exception;
    var _av;
    try {
        _av = eval(_a);
    } catch (e) {
        exception = e;
    }

    if (exception)
        testFailed(_a + " should be non-zero. Threw exception " + exception);
    else if (_av != 0)
        testPassed(_a + " is non-zero.");
    else
        testFailed(_a + " should be non-zero. Was " + _av);
}

function shouldBeNonNull(_a)
{
    var exception;
    var _av;
    try {
        _av = eval(_a);
    } catch (e) {
        exception = e;
    }

    if (exception)
        testFailed(_a + " should be non-null. Threw exception " + exception);
    else if (_av != null)
        testPassed(_a + " is non-null.");
    else
        testFailed(_a + " should be non-null. Was " + _av);
}

function shouldBeUndefined(_a)
{
    var exception;
    var _av;
    try {
        _av = eval(_a);
    } catch (e) {
        exception = e;
    }

    if (exception)
        testFailed(_a + " should be undefined. Threw exception " + exception);
    else if (typeof _av == "undefined")
        testPassed(_a + " is undefined.");
    else
        testFailed(_a + " should be undefined. Was " + _av);
}

function shouldBeDefined(_a)
{
    var exception;
    var _av;
    try {
        _av = eval(_a);
    } catch (e) {
        exception = e;
    }

    if (exception)
        testFailed(_a + " should be defined. Threw exception " + exception);
    else if (_av !== undefined)
        testPassed(_a + " is defined.");
    else
        testFailed(_a + " should be defined. Was " + _av);
}

function shouldBeGreaterThanOrEqual(_a, _b) {
    if (typeof _a != "string" || typeof _b != "string")
        debug("WARN: shouldBeGreaterThanOrEqual expects string arguments");

    var exception;
    var _av;
    try {
        _av = eval(_a);
    } catch (e) {
        exception = e;
    }
    var _bv = eval(_b);

    if (exception)
        testFailed(_a + " should be >= " + _b + ". Threw exception " + exception);
    else if (typeof _av == "undefined" || _av < _bv)
        testFailed(_a + " should be >= " + _b + ". Was " + _av + " (of type " + typeof _av + ").");
    else
        testPassed(_a + " is >= " + _b);
}

function dfgShouldBe(func, call, result)
{
    shouldBe(call, result);
}

function shouldThrowType(_a, _e)
{
    var exception;
    var _av;
    try {
        _av = eval(_a);
    } catch (e) {
        exception = e;
    }

    if (exception) {
        if (exception instanceof _e)
            testPassed(_a + " threw exception of type " + _e.name + ".");
        else
            testFailed(_a + " should throw " + (typeof _e == "undefined" ? "an exception" : _e) + ". Threw exception " + exception + ".");
    } else
        testFailed(_a + " should throw an instance of " + _e.name);
}

function gc() {
    nrdp.gibbon.garbageCollect();
}

// It's possible for an async test to call finishJSTest() before js-test-post.js
// has been parsed.
function finishJSTest()
{
    shouldBeTrue("successfullyParsed");
    debug('TEST COMPLETE');
}

// shadow print()
function print()
{
    var out = [];
    for (var i=0; i<arguments.length; ++i) {
        if (typeof arguments[i] === "object") {
            out.push(JSON.stringify(arguments[i]));
        } else {
            out.push(arguments[i]);
        }
    }
    nrdp.console(out.join(" "));
}

function assertEq(left, right, message)
{
    if (left === right)
        testPassed(left + " === " + right);
    else if (message)
        testFailed(message);
    else
        testFailed("assertEq failed: " + left + "(of type " + (typeof left) + ") !== " + right + "(of type " + (typeof right) + ")");
}

var w, w2;
var idx = 0;

function queryParam(name)
{
    var params = nrdp.gibbon.location.split('?');
    if (params.length > 1) {
        params = params[params.length - 1].split('&');
        for (var i=0; i<params.length; ++i) {
            var s = params[i].split('=');
            if (s.length == 2 && s[0] == name) {
                return s[1];
            } else if (s.length == 1 && s[0] == name) {
                return true;
            }
        }
    }
    return undefined;
}

function main()
{
    function calculateFilters()
    {
        var filter = queryParam('filter');
        if (filter !== undefined) {
            var filters = [];
            function createFilter(filter)
            {
                if (filter.indexOf(',') != -1) {
                    var split = filter.split(',');
                    for (var i=0; i<split.length; ++i)
                        createFilter(split[i]);
                    return;
                }
                var res = filter.match(/([0-9]+)-([0-9]+)/);
                if (res && res.length == 3) {
                    filters.push({type:'range', from:res[1], to:res[2]});
                    // tests = tests.splice(res[1], res[2] - res[1] + 1);
                    return;
                }
                res = parseInt(filter);
                if (!isNaN(res)) {
                    filters.push({type:'range', from:res, to:res});
                    // tests = tests.splice(res, 1);
                    return;
                }
                filters.push({type:'match', match:filter});
            }
            createFilter(filter);

            if (filters.length) {
                var copy = [];
                for (var i=0; i<tests.length; ++i) {
                    function match() {
                        for (var f=0; f<filters.length; ++f) {
                            switch (filters[f].type) {
                            case 'range':
                                if (i >= filters[f].from && i <= filters[f].to)
                                    return true;
                                break;
                            case 'match':
                                if (tests[i].indexOf(filters[f]) != -1) {
                                    return true;
                                }
                                break;
                            }
                        }
                        return false;
                    }
                    if (match()) {
                        copy.push(tests[i]);
                    }
                }
                tests = copy;
            }
        }
    }
    calculateFilters();

    var scene = nrdp.gibbon.scene.widget = nrdp.gibbon.makeWidget({width:1280, height:720, color:'#00ff00', layout:'v'});
    w = nrdp.gibbon.makeWidget();
    w2 = nrdp.gibbon.makeWidget();
    w.parent = scene;
    w2.parent = scene;

    function next() {
        if (failed > 0)
            w.color = '#ff0000';
        w.text = idx + '/' + tests.length + ' tests run, ' + failed + ' failed, ' + (idx - failed) + ' passed';
        w2.text = tests[idx];
        nrdp.log.warn('Running test: ' + tests[idx] + ' ' + idx + '/' + tests.length + ' ' + failed + ' failed ' + (idx - failed) + ' passed');
        nrdp.gibbon.load({url:tests[idx]}, function(data) {
            if (data.statusCode != 200) {
                testFailed("Couldn't download test: " + data.url);
                setTimeout(next, 0);
                return;
            }
            try {
                nrdp.gibbon.eval(data.data, data.url);
            } catch (err) {
                nrdp.log.error("Caught exception " + err);
                testFailed(data.url + ' ' + err);
            }
            ++idx;
            if (idx == tests.length) {
                nrdp.log.warn(idx + '/' + tests.length + ' tests run, ' + failed + ' failed, ' + (idx - failed) + ' passed');
                if (!queryParam('noAutoQuit'))
                    nrdp.exit();
            } else {
                nrdp.gibbon.setTimeout(next, 0);
            }
        });
    }
    next();
}

nrdp.gibbon.init(main);




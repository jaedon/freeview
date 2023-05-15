/**
 * Created by bglee@humaxdigital.com on 2014. 3. 14..
 */

interface ifBrowser {
    webkit: any;
    opera: any;
    msie: any;
}
interface JQueryStatic {
    browser: ifBrowser;
}
interface Window {
    debug(aLog: string): void;
}
interface ifConsole {
    reset: string;
    info: string;
    debug: string;
    warn: string;
    error: string;
    time: string;
    timeEnd: string;
    normal: string;
//    reset : '\033[22;39m',
//    info : '\033[01;34m',
//    debug : '\033[01;32m',
//    warn : '\033[22;33m',
//    error : '\033[22;31m',
//    time : '\033[01;37m',
//    timeEnd : '\033[01;37m',
//    normal : '\033[01;36m'
}

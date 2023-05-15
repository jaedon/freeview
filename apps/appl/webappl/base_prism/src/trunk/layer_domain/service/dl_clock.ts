/// <reference path="../../def/sugar.d.ts" />
/**
 * Created by bglee@humaxdigital.com on 2014. 5. 11..
 */
//    CLOCK: ['layer_domain/service/dl_clock.js']

declare var hx: ifSugar;
import __KERNEL__ = require("./dl_kernel");

class cDLClock extends __KERNEL__ implements ifSVCClock {
    private clockTimer;
    constructor(aServiceName: string) {
        super(aServiceName);
        this.defineEvents(['SecondChanged', 'MinuteChanged', 'HourChanged', 'DayChanged']);
        //this.startClock();
    }
    startClock() {        
        if(this.clockTimer){            
            return;
        }
        var now = new Date();
        var date = now;
        var sec = now.getSeconds();
        var min = now.getMinutes();
        var hour = now.getHours();
        var day = now.getDate();
        this.clockTimer = setInterval(() => {
            var now = new Date();
            if (30000 < Math.abs(now.getTime() - date.getTime()) || Math.abs(now.getTimezoneOffset() - date.getTimezoneOffset())) {
                sec = -1;
                min = -1;
                hour = -1;
                day = -1;
            }
            date = now;
            if (sec != now.getSeconds()) {
                if (min != now.getMinutes()) {
                    if (hour != now.getHours()) {
                        if (day != now.getDate()) {
                            day = now.getDate();
                            this.fireEvent('DayChanged', now);
                        }
                        hour = now.getHours();
                        this.fireEvent('HourChanged', now);
                    }
                    min = now.getMinutes();
                    this.fireEvent('MinuteChanged', now);
                }
                sec = now.getSeconds();
                this.fireEvent('SecondChanged', now);
            }
        }, 1000);
    }
    stopClock() {        
        if(this.clockTimer){
            clearInterval(this.clockTimer);
            this.clockTimer = null;
        }
    }
}
export = cDLClock;
/**
 * Created by bglee@humaxdigital.com on 2014. 7. 31..
 * define interface for !public! service method
 * service parameters
 * [prefix]_[service name]_[function name]_[argument 0 - 9]
 */

// EPG
interface if_EPG_getProgrammes_0 {
    channel?: any;
    isOnAir?: boolean;
    clause: string;
    startTime?: number;
    endTime?: number;
    info?: string;
    cb?: Function;
    notUI?: boolean;
    filter?: string;
}
interface if_EPG_findProgrammes_0 {
    channel?: any; //Channel;
    clause: string;
    filter?: string;
    cb?: Function;
    notUI?: boolean;
}

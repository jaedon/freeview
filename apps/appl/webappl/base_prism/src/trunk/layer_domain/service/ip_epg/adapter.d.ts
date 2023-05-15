/**
 * Created by bglee on 2015-03-29.
 */
/// <reference path="../../../def/q.d.ts" />
/// <reference path="../../../def/jquery.d.ts" />

interface IHttpControl{
    setHeader(property: string, value: any) : void;
    setBaseUrl(url: string): void;
    getBaseUrl(): string;
}

interface IQuerySchedule{
    serviceId : any;
    startTime : number;
    hours : number;
    catchupOnly?: boolean;
    retDef? : Q.Deferred<any>;
}

interface IQueryAIT{
    url : string;
    retDef? : Q.Deferred<any>;
}

interface IQueryMethodMDS{
    getScheduleList(scheduleQueryList : IQuerySchedule[]): Q.Promise<any>;//Q.all
    getAITList(IQueryAIT : IQueryAIT[]) : Q.Promise<any>;
    setEnableHttp(enable:boolean) : Q.Promise<any>;
}


interface IAdaptorIpEpg{
    getChannels(regionId: number, prevHeader?): Q.Promise<IDeferResolved>;
    getSchedules(serviceId, startTime: number, hours: number): Q.Promise<any>|any;
    getDetailInfo(programmeId, isBackward?: boolean): Q.Promise<any>|any;
    getThumbnail(programmeId): Q.Promise<any>|any;
    getChannelLogo(serviceId): Q.Promise<any>|any;
    getApplication(networkId): Q.Promise<any>|any;
    getOnDemandSuggestions(inputString: String, prevHeader?): Q.Promise<any>|any;
    getOnDemandResults(searchTerm: String, networkID: number[], prevHeader?): Q.Promise<any>|any;
    getContentOwningServices(prevHeader?): Q.Promise<any>|any;
    getContentOwningLogo(serviceId, prevHeader?): Q.Promise<any>|any;
    getRecommendationCategories(type: String): Q.Promise<any>|any;
    getRecommendationResults(type: String, networkIds: number[], groupId?: String, serviceId?: String, prevHeader?): Q.Promise<any>|any;
    parse: any;
}

interface IDeferResolved {
    status: number;
    header: any;
    data: any;
    query: string;
}

interface ITraceInfo {
    query: string;
    status: number;
    mechanism: number[];
    position: number;
}

interface IJQueryXHRWithQuery extends JQueryXHR {
    query: string;
    traceInfo?: ITraceInfo;
}
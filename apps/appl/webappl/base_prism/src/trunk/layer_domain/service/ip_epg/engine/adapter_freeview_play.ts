/**
 * Created by bglee on 2015-03-28.
 */
/// <reference path="../../../../def/sugar.d.ts" />
/// <reference path="../adapter.d.ts" />
import __engine__ = require('./engine_freeview_play');
import __parser__ = require('./parser_freeview_play');
import __MODEL__ = require('model/model');

declare var hx: ifSugar;

function fnConvertImageVariant(aVariant: __MODEL__.EImageVariant) {
    var convertedString = '';
    switch(aVariant) {
        case __MODEL__.EImageVariant.EColour_16_9:
            convertedString = '16x9_colour';
            break;
        case __MODEL__.EImageVariant.EColour_1_1:
            convertedString = 'square_colour';
            break;
        case __MODEL__.EImageVariant.EWhite_16_9:
            convertedString = '16x9_white';
            break;
        case __MODEL__.EImageVariant.EWhite_1_1:
            convertedString = 'square_white';
            break;
        default:
            break;
    }
    return convertedString;
}

interface ILogoImage {
    type: string;
    uri: string;
    width?: number;
}
interface ILogoImageSlot {
    [serviceId: string]: ILogoImage;
}
interface IOwningImage {
    type: string;
    uri: string;
    width?: number;
}
interface IOwningImageSlot {
    [serviceId: string]: IOwningImage;
}

class CWrapperInterface {
    isWrapper = true;
    private _mapBind = {};
    protected _adapter: CAdaptorMds;
    constructor(adapter: CAdaptorMds) {
        this._adapter = adapter;
    }
    _bind(key, callback: (arg) => string) {
        this._mapBind[key] = callback;
    }
    update(key: string, arg) {
        var fx = this._mapBind[key];
        if (fx) {
            var ret = fx(arg);
            // for external
            console.error('CWrapperInterface update _bind(' + key + ')');
            if (this['on' + key]) {
                this['on' + key](ret);
            }
            return ret;
        }
        console.error('cannot find related _bind(' + key + ')');
        return this;
    }
}
class COIPFProgrammeWrapper extends CWrapperInterface implements Programme {
    private _program: IProgramInformation;
    private _schedule: IScheduleEvent;
    private _ondemand: IOnDemandProgram;
    private _channel: Channel;
    private _parsedOndemand: IParsedElementOndemand;
    private _parsedProgram: IParsedElementProgram;
    private _fieldsExtended = {
        'freeview/guidance': null,
        'freeview/linkage': '[]',
        'thumbnailURL': null
    };
    private _available: boolean;
    private _eitName: string;
    private _eitSynopsis: string;
    private _pluginObject: any;
    private _availableIconUrl: string;
    constructor(adapter: CAdaptorMds, raw: IParsedScheduleInternal, channel: Channel, thumbnailWidth, pluginObject?: any) {
        super(adapter);
        var updateThumbnail = () => {
            if (!this._program) {
                return;
            }
            var mediaUri = this._program.querySelector('MediaUri');
            if (mediaUri) {
                //TODO: do i need width?
                var contentType = mediaUri.getAttribute('contentType');
                console.debug('mediaUri: ', mediaUri);
                hx.svc.IP_EPG.getThumbnail(mediaUri.textContent,thumbnailWidth,{"content-type" : contentType}).then((data) => {
                    this.update('thumbnail', data);
                }, () => {
                    this.update('thumbnail', this.defaultThumbnail);
                });
            } else {
                this.update('thumbnail', this.defaultThumbnail);
            }
        };
        this._program = raw.program;
        this._schedule = raw.schedule;
        this._ondemand = raw.ondemand;
        this._channel = channel;
        this._available = raw.available;
        this._availableIconUrl = raw.availableIconUrl;
        this._pluginObject = pluginObject;
        this._bind('program', (program: IProgramInformation) => {
            this._program = program;
            updateThumbnail();
            return 'program';
        });
        this._bind('thumbnail', (url: string) => {
            this._fieldsExtended.thumbnailURL = url;
            return url;
        });
        updateThumbnail();
        if (this._channel && !this._channel.logoURL) {
            this._bind('channellogo', (url: string) => {
                return this._channel.logoURL = url;
            });
            hx.svc.IP_EPG.getChannelLogo(channel.sid).then((image) => {
                this.update('channellogo', image);
            }, function() {});
        }
    }
    get name(): string {
        if (this._eitName) {
            return this._eitName;
        }
        return this._program.querySelector('Title[type="main"]').textContent.trim();
    }
    set name(value: string) {
        this._eitName = value;
    }
    get longName(): string {
        var secondaryTitle = this._program.querySelector('Title[type="secondary"]');
        if (secondaryTitle) {
            return secondaryTitle.textContent;
        }
        return '';
    }
    get description(): string {
        return '';
    }
    get longDescription(): string {
        if (this._eitSynopsis) {
            return this._eitSynopsis;
        }
        var synopsis = (this._program.querySelector('Synopsis[length="long"]') || this._program.querySelector('Synopsis[length="medium"]') || this._program.querySelector('Synopsis[length="short"]'));
        if (synopsis) {
            return synopsis.textContent;
        }
        return '';
    }
    set longDescription(value: string) {
        this._eitSynopsis = value;
    }
    get startTime(): number {
        if (this._schedule) {
            return new Date(this._schedule.querySelector('PublishedStartTime').textContent).valueOf() / 1000;
        } else {
            return null;
        }
    }
    get endTime(): number {
        if (this._schedule) {
            var startTime = this.startTime;
            var duration = this.duration;
            if (startTime && duration) {
                return startTime + duration;
            }
        }
        return null;
    }
    get duration(): number {
        if (this._schedule) {
            return __parser__.util.duration(this._schedule.querySelector('PublishedDuration').textContent);
        } else {
            return null;
        }
    }
    get channelID(): string {
        //throw 'does not prepared';
        return '';
    }
    get episode(): number {
        //throw 'does not prepared';
        return 0;
    }
    get totalEpisodes(): number {
        if (this._pluginObject) {
            return this._pluginObject.totalEpisodes;
        }
        return 0;
    }
    get programmeID(): string {
        if (this._pluginObject) {
            return this._pluginObject.programmeID;
        }
        return this.program.crid;
    }
    get programmeIDType(): number {
        //throw 'does not prepared';
        return 0;
    }
    get IMI(): string {
        //throw 'does not prepared';
        return '';
    }
    get groupCRIDs(): string[] {
        //throw 'does not prepared';
        if (this._pluginObject) {
            return this._pluginObject.groupCRIDs;
        }
        return [];
    }
    get parentalRating(): ParentalRatingCollection {
        if (this._pluginObject) {
            return this._pluginObject.parentalRating;
        }
        return null;
    }
    get channel(): Channel {
        return this._channel;
    }
    get blocked(): boolean {
        //throw 'does not prepared';
        return false;
    }
    get showType(): number {
        //throw 'does not prepared';
        return 0;
    }
    get subtitles(): boolean {
        //throw 'does not prepared';
        return false;
    }
    get isHD(): boolean {
        //throw 'does not prepared';
        var ondemand = this._ondemand;
        if (!ondemand) {
            return false;
        }
        var horizontalSize = ondemand.querySelector('InstanceDescription VideoAttributes>HorizontalSize');
        if (!horizontalSize) {
            return false;
        }
        return parseInt(horizontalSize.textContent) >= 720;
    }
    get isAD(): boolean {
        //throw 'does not prepared';
        return false;
    }
    get audioType(): number {
        //throw 'does not prepared';
        return 0;
    }
    get isMultilingual(): boolean {
        //throw 'does not prepared';
        return false;
    }
    get genre(): string[] {
        var genre = this.program.genre;
        return genre;
        //var genre = this._program.querySelector('Genre');
        //if (genre) {
        //    return /urn:fvc:metadata:cs:ContentSubjectCS:[0-9-]+:([0-9]+)/g.exec(genre.getAttribute('href')).slice(1);
        //}
        //return [];
    }
    get defaultThumbnail(): string {
        var defaultThumb = (this._pluginObject && this._pluginObject.getGenreImage) ? this._pluginObject.getGenreImage() : null;    // check RF first
        if (!defaultThumb) {
            var genres = this.program.genre;
            if (genres && genres.length) {
                defaultThumb = hx.il.genreList.getGenreImage(genres[0]);
            }
        }
        return defaultThumb || __MODEL__.TDefaultThumbnailList.DEFAULT;
    }
    get hasRecording(): boolean {
        //throw 'does not prepared';
        return false;
    }
    get audioLanguages(): string[] {
        //throw 'does not prepared';
        return [];
    }
    get subtitleLanguages(): string[] {
        //throw 'does not prepared';
        return [];
    }
    get locked(): boolean {
        //throw 'does not prepared';
        return false;
    }
    getField(fieldId: string): string {
        switch (fieldId) {
            case 'freeview/guidance':
            case 'freeview/linkage':
            case 'thumbnailURL':
                hx.log('info', fieldId);
                break;
        }
        return this._fieldsExtended[fieldId];
    }
    get logoURL(): string {
        return this._channel ? this._channel.logoURL : '';
    }
    get thumbnailURL(): string {
        return this.program.icon.url;
    }
    get recording(): ScheduledRecording {
        if (this._pluginObject) {
            return this._pluginObject.recording;
        }
        return;
    }
    // extended by humax
    get subtitleType(): number {
        //throw 'does not prepared';
        return 0;
    }
    get subtitleTypes(): string[] {
        //throw 'does not prepared';
        return [];
    }
    get scheduledRecording(): ScheduledRecording {
        //throw 'does not prepared';
        return null;
    }
    get recordings(): any[] {
        //throw 'does not prepared';
        return [];
    }
    get mediaDeeplinkID(): string {
        return '';
    }
    get mediaDeeplinkUrl(): string {
        return '';
    }
    get mediaDeeplinkExpireDate(): number {
        return 0;
    }
    // extension for freeview play
    get ondemand(): IParsedElementOndemand {
        if (!this._ondemand) {
            return null;
        }
        if (this._parsedOndemand) {
            return this._parsedOndemand;
        }
        return this._parsedOndemand = __parser__.element.ondemand(this._ondemand);
    }
    get program(): IParsedElementProgram {
        if (!this._program) {
            return null;
        }
        if (this._parsedProgram) {
            return this._parsedProgram;
        }
        return this._parsedProgram = __parser__.element.programInformation(this._program);
    }
    get available(): boolean {
        return this._available;
    }
    set available(available: boolean) {
        this._available = available;
    }
    get availableIconUrl() : string {
        return this._availableIconUrl;
    }
    set availableIconUrl(url: string) {
        this._availableIconUrl = url;
    }
    getThumbnail(param) {
        // This may be not for programme detail(width = 400),
        // But for other cases to get thumbnail which has different width.
        if (!param) {
            hx.log('debug', '[COIPFProgrammeWrapper.getThumbnail] !param');
            return '';
        }
        var updateCb = param.updateCb;
        var width = param.width;
        if (!updateCb || !width) {
            hx.log('debug', '[COIPFProgrammeWrapper.getThumbnail] !updateCb || !width');
            return '';
        }
        if (this.program && this.program.icon && this.program.icon.url) {
            hx.svc.IP_EPG.getThumbnail(this.program.icon.url, width).then((data) => {
                updateCb(data);
            }, (err) => {
                updateCb(this.defaultThumbnail);
            });
        } else {
            return this.defaultThumbnail;
        }
    }
    getChannelLogo(param) {
        // This may be not for programme detail(width = 400),
        // But for other cases to get channelLogo which has different width.
        if (!param) {
            hx.log('debug', '[COIPFProgrammeWrapper.getChannelLogo] !param');
            return '';
        }
        var updateCb = param.updateCb;
        var width = param.width;
        if (!updateCb || !width) {
            hx.log('debug', '[COIPFProgrammeWrapper.getChannelLogo] !updateCb || !width');
            return '';
        }
        if (this._channel && this._channel.sid) {
            hx.svc.IP_EPG.getChannelLogo(this._channel.sid, width).then((data) => {
                updateCb(data);
            }, () => {
                updateCb('');
            });
        } else {
            return '';
        }
    }
}
class COIPFConverter {
    private _adapter: CAdaptorMds;
    constructor(adapter: CAdaptorMds) {
        this._adapter = adapter;
    }
    programme(raw: IParsedScheduleInternal, channel: Channel, pluginObject?: Programme, thumbnailWidth = 400): Programme {
        return new COIPFProgrammeWrapper(this._adapter, raw, channel, thumbnailWidth, pluginObject);
    }
}
class CAdaptorMds implements IAdaptorIpEpg, IHttpControl{
    static _instance: CAdaptorMds;
    private _engine: __engine__;
    private _channelLogoUrls: ILogoImageSlot = {};
    private _contentOwningLogoUrls: IOwningImageSlot = {};
    constructor(getterUrlAuth: () => string) {
        if (CAdaptorMds._instance) {
            return CAdaptorMds._instance;
        }
        CAdaptorMds._instance = this;
        this._engine = new __engine__(getterUrlAuth);
    }
    _parseServiceElement(aServiceElements: NodeList, aCallback) {
        var itemServiceInformation;
        var itemRelatedMaterial; //section 6.1.3 of the B2C spec
        var arrayRelatedMaterial;
        var idxRelatedMaterial;
        var lengthRelatedMaterial;
        var selectorHowRelated;
        var itemMediaLocator;
        var itemMediaUri;
        var arrayServiceInformation = aServiceElements;
        var lengthServiceInformation = arrayServiceInformation.length;
        setTimeout(() => {
            for (var i = 0; i < lengthServiceInformation; i++) {
                itemServiceInformation = arrayServiceInformation.item(i);
                arrayRelatedMaterial = itemServiceInformation.querySelectorAll('RelatedMaterial');//B2C Spec version 1.1 & https://digitaluk.atlassian.net/browse/FVCHUM-252
                lengthRelatedMaterial = arrayRelatedMaterial.length;
                itemMediaUri = null;
                for (idxRelatedMaterial = 0; idxRelatedMaterial < lengthRelatedMaterial; idxRelatedMaterial++) {
                    itemRelatedMaterial = arrayRelatedMaterial[idxRelatedMaterial];
                    selectorHowRelated = itemRelatedMaterial.querySelector('HowRelated');
                    if (selectorHowRelated && selectorHowRelated.getAttribute('href') == 'urn:tva:metadata:cs:HowRelatedCS:2012:19') {//B2C Spec version 1.0 6.6 // Image
                        //check Uri
                        itemMediaLocator = itemRelatedMaterial.querySelector('MediaLocator');
                        if (itemMediaLocator) {
                            itemMediaUri = itemMediaLocator.querySelector('MediaUri');
                            break;
                        }
                    }
                }
                if (itemMediaUri) {
                    aCallback(itemServiceInformation, itemMediaUri);
                }
            }
        });
    }
    setChannelLogoUrls(aServiceElements: NodeList) {
        if (!aServiceElements) {
            hx.log('error', '[setChannelLogoUrls] !aServiceElements');
            return;
        }
        this._parseServiceElement(aServiceElements, (aServiceInfo, aMediaUri) => {
            this._channelLogoUrls[aServiceInfo.getAttribute('serviceId')] = {
                type: aMediaUri.getAttribute('contentType'),
                uri: aMediaUri.textContent
            };
            console.log(aMediaUri);
        });
    }
    setContentOwningLogoUrls(aServiceElements: NodeList) {
        if (!aServiceElements) {
            hx.log('error', '[setContentOwningLogoUrls] !aServiceElements');
            return;
        }
        this._parseServiceElement(aServiceElements, (aServiceInfo, aMediaUri) => {
            this._contentOwningLogoUrls[aServiceInfo.getAttribute('serviceId')] = {
                type: aMediaUri.getAttribute('contentType'),
                uri: aMediaUri.textContent
            };
            console.log(aMediaUri);
        });
    }
    /* public function - london: 12339, cambridge and bedfordshire: 12320, east anglia 12321*/
    getChannels(regionId: number, prevHeader?): Q.Promise<IDeferResolved> {
        if (!regionId) {
            return <Q.Promise<IDeferResolved>>Q.reject('!regionId');
        }
        return this._engine.xml(prevHeader, '/services?nid=' + regionId).then(
            (res: IDeferResolved) => {
                var dom: ITVAMain = <any>res.data;
                res.data = <any>dom.querySelectorAll('ServiceInformation');
                return res;
            }
        );
    }
    getContentOwningServices(serviceType, prevHeader?): Q.Promise<IDeferResolved> {
        if (!serviceType) {
            return <Q.Promise<IDeferResolved>>Q.reject(('!serviceType'));
        }
        return this._engine.xml(prevHeader, '/services?service_type=' + serviceType).then(
            (res: IDeferResolved) => {
                var dom: ITVAMain = <any>res.data;
                res.data = <any>dom.querySelectorAll('ServiceInformation');
                return res;
            }
        );
    }
    /* public function */
    getSchedulesQuery(serviceId, baseTime: number, hours: number, catchupOnly?: boolean): string {
        baseTime = (baseTime - (baseTime % (21600000))) / 1000;
        var targetTime = baseTime + (hours * 3600);
        if (baseTime > targetTime) {
            var temp = baseTime;
            baseTime = targetTime;
            targetTime = temp;
        }
        var query = '/schedules?start=' + baseTime + '&end=' + targetTime + '&sids[]=' + serviceId;
        if (catchupOnly) {
            query += '&catchup_only=true';
        }
        hx.log('mds', '> query: ' + query + '[' + new Date(baseTime*1000).toUTCString() + ']['+ new Date(targetTime*1000).toUTCString() + ']');
        return query;
    }
    getXmlByQuery(query: string, prevHeader?): Q.Promise<IDeferResolved> {
        return this._engine.xml(prevHeader, query);
    }
    getSchedules(serviceId, baseTime: number, hours: number, prevHeader?): Q.Promise<IDeferResolved> {
        return this._engine.xml(prevHeader, this.getSchedulesQuery(serviceId, baseTime, hours));
    }
    /* public function */
    getDetailInfo(programId, encode: boolean, prevHeader?): Q.Promise<IDeferResolved> {
        if (encode) {
            //programId = encodeURIComponent(programId);
        }

        programId = encodeURIComponent(programId);

        var query = '/program?pid=' + programId;
        return this._engine.xml(prevHeader, query);
    }
    /* public function */
    getThumbnail(url: string, width = 400, prevHeader?, type?): Q.Promise<IDeferResolved> {
        if (!url) {
            return <Q.Promise<IDeferResolved>>Q.reject('getThumbnail : url in null');
        }
        var delimiter = /\?/.test(url) ? '&' : '?';
        return this._engine.image(prevHeader, url + delimiter + 'w=' + width, type);
        // XXX: if you need to encode base64 way
    }
    /* public function */
    getChannelLogo(serviceId, width = 80, prevHeader?): Q.Promise<IDeferResolved> {
        var logoImage = this._channelLogoUrls[serviceId];
        if (!logoImage || (logoImage && !logoImage.uri)) {
            return <Q.Promise<IDeferResolved>>Q.reject('getChannelLogo : service[' + serviceId + '] does not exist');
        }
        var delimiter = /\?/.test(logoImage.uri) ? '&' : '?';
        return this._engine.image(prevHeader, logoImage.uri + delimiter + 'w=' + width);
    }
    getContentOwningLogo(serviceId, width, prevHeader?): Q.Promise<IDeferResolved> {
        var owningImage = this._contentOwningLogoUrls[serviceId];
        if (!owningImage || (owningImage && !owningImage.uri)) {
            return <Q.Promise<IDeferResolved>>Q.reject('getContentOwningLogo : service[' + serviceId + '] does not exist');
        }
        var delimiter = /\?/.test(owningImage.uri) ? '&' : '?';
        return this._engine.image(prevHeader, owningImage.uri + delimiter + 'w=' + width);
    }
    getApplication(networkID: number[], prevHeader?, image_variant?): Q.Promise<IDeferResolved> {
        if (networkID.length === 0) {
            return <Q.Promise<IDeferResolved>>Q.reject(('networkID.length === 0'));
        }
        return this._engine.xml(prevHeader, '/applications?nids[]=' + networkID.join('&nids[]='), fnConvertImageVariant(image_variant));
    }
    getOnDemandSuggestions(inputString: String, prevHeader?): Q.Promise<IDeferResolved> {
        if (!inputString) {
            return <Q.Promise<IDeferResolved>>Q.reject(('!inputString'));
        }
        inputString = encodeURIComponent(inputString.toString());
        return this._engine.xml(prevHeader, '/programs/suggestions?q=' + inputString + '&type=ondemand');
    }
    getOnDemandResults(searchTerm: String, networkID: number[], prevHeader?, pageNum?): Q.Promise<IDeferResolved> {
        if (!searchTerm || networkID.length === 0) {
            return <Q.Promise<IDeferResolved>>Q.reject(('!searchTerm or networkID.length === 0'));
        }
        searchTerm = encodeURIComponent(searchTerm.toString());
        return this._engine.xml(prevHeader, '/programs/search?q=' + searchTerm + '&type=ondemand&nids[]=' + networkID.join('&nids[]=') + ((pageNum && pageNum > 1) ? '&page=' + pageNum : ''));
    }
    getRecommendationCategories(type, prevHeader?): Q.Promise<IDeferResolved> {
        if (!type) {
            return <Q.Promise<IDeferResolved>>Q.reject(('!type'));
        }
        return this._engine.xml(prevHeader, '/groups/recommendations?type=' + encodeURIComponent(type.toString()));
    }
    getRecommendationResults(type: String, networkIds: number[], groupId?: String, serviceId?: String, prevHeader?): Q.Promise<IDeferResolved> {
        if ((!groupId && (networkIds.length === 0)) && !serviceId) {
            return <Q.Promise<IDeferResolved>>Q.reject(('(!groupId && (networkIds.length === 0)) && !serviceId'));
        }
        var suffix = '/programs/recommendations?';
        if (groupId) {
            suffix = suffix + 'groupId=' + encodeURIComponent(groupId.toString()) + '&type=' + type + '&nids[]=' + networkIds.join('&nids[]=');
        } else if (serviceId) {
            suffix = suffix + 'type=' + type + '&sids[]=' + encodeURIComponent(serviceId.toString());
        }
        return this._engine.xml(prevHeader, suffix);
    }
    getAuthToken(retryAfter?: number|string, mdsUrl?:string): Q.Promise<any> {
        return this._engine.renewAuthInfo(retryAfter,mdsUrl);
    /*.then(function(){
            return this;
        }, function(res) {
            throw res;
        });
        */
    }
    getBaseUrl(): string {
        return this._engine.getBaseUrl();
    }

    setBaseUrl(url: string): void {
        this._engine.setBaseUrl(url);
    }

    setHeader(property: string, value: any) : void {
        this._engine.setHeader(property,value);
    }

    parse = __parser__;
    convert = {
        to: new COIPFConverter(this)
    };
}
export = CAdaptorMds;

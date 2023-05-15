var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", './engine_freeview_play', './parser_freeview_play', 'model/model'], function (require, exports, __engine__, __parser__, __MODEL__) {
    function fnConvertImageVariant(aVariant) {
        var convertedString = '';
        switch (aVariant) {
            case 1 /* EColour_16_9 */:
                convertedString = '16x9_colour';
                break;
            case 2 /* EColour_1_1 */:
                convertedString = 'square_colour';
                break;
            case 3 /* EWhite_16_9 */:
                convertedString = '16x9_white';
                break;
            case 4 /* EWhite_1_1 */:
                convertedString = 'square_white';
                break;
            default:
                break;
        }
        return convertedString;
    }
    var CWrapperInterface = (function () {
        function CWrapperInterface(adapter) {
            this.isWrapper = true;
            this._mapBind = {};
            this._adapter = adapter;
        }
        CWrapperInterface.prototype._bind = function (key, callback) {
            this._mapBind[key] = callback;
        };
        CWrapperInterface.prototype.update = function (key, arg) {
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
        };
        return CWrapperInterface;
    })();
    var COIPFProgrammeWrapper = (function (_super) {
        __extends(COIPFProgrammeWrapper, _super);
        function COIPFProgrammeWrapper(adapter, raw, channel, thumbnailWidth, pluginObject) {
            var _this = this;
            _super.call(this, adapter);
            this._fieldsExtended = {
                'freeview/guidance': null,
                'freeview/linkage': '[]',
                'thumbnailURL': null
            };
            var updateThumbnail = function () {
                if (!_this._program) {
                    return;
                }
                var mediaUri = _this._program.querySelector('MediaUri');
                if (mediaUri) {
                    //TODO: do i need width?
                    var contentType = mediaUri.getAttribute('contentType');
                    console.debug('mediaUri: ', mediaUri);
                    hx.svc.IP_EPG.getThumbnail(mediaUri.textContent, thumbnailWidth, { "content-type": contentType }).then(function (data) {
                        _this.update('thumbnail', data);
                    }, function () {
                        _this.update('thumbnail', _this.defaultThumbnail);
                    });
                }
                else {
                    _this.update('thumbnail', _this.defaultThumbnail);
                }
            };
            this._program = raw.program;
            this._schedule = raw.schedule;
            this._ondemand = raw.ondemand;
            this._channel = channel;
            this._available = raw.available;
            this._availableIconUrl = raw.availableIconUrl;
            this._pluginObject = pluginObject;
            this._bind('program', function (program) {
                _this._program = program;
                updateThumbnail();
                return 'program';
            });
            this._bind('thumbnail', function (url) {
                _this._fieldsExtended.thumbnailURL = url;
                return url;
            });
            updateThumbnail();
            if (this._channel && !this._channel.logoURL) {
                this._bind('channellogo', function (url) {
                    return _this._channel.logoURL = url;
                });
                hx.svc.IP_EPG.getChannelLogo(channel.sid).then(function (image) {
                    _this.update('channellogo', image);
                }, function () {
                });
            }
        }
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "name", {
            get: function () {
                if (this._eitName) {
                    return this._eitName;
                }
                return this._program.querySelector('Title[type="main"]').textContent.trim();
            },
            set: function (value) {
                this._eitName = value;
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "longName", {
            get: function () {
                var secondaryTitle = this._program.querySelector('Title[type="secondary"]');
                if (secondaryTitle) {
                    return secondaryTitle.textContent;
                }
                return '';
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "description", {
            get: function () {
                return '';
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "longDescription", {
            get: function () {
                if (this._eitSynopsis) {
                    return this._eitSynopsis;
                }
                var synopsis = (this._program.querySelector('Synopsis[length="long"]') || this._program.querySelector('Synopsis[length="medium"]') || this._program.querySelector('Synopsis[length="short"]'));
                if (synopsis) {
                    return synopsis.textContent;
                }
                return '';
            },
            set: function (value) {
                this._eitSynopsis = value;
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "startTime", {
            get: function () {
                if (this._schedule) {
                    return new Date(this._schedule.querySelector('PublishedStartTime').textContent).valueOf() / 1000;
                }
                else {
                    return null;
                }
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "endTime", {
            get: function () {
                if (this._schedule) {
                    var startTime = this.startTime;
                    var duration = this.duration;
                    if (startTime && duration) {
                        return startTime + duration;
                    }
                }
                return null;
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "duration", {
            get: function () {
                if (this._schedule) {
                    return __parser__.util.duration(this._schedule.querySelector('PublishedDuration').textContent);
                }
                else {
                    return null;
                }
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "channelID", {
            get: function () {
                //throw 'does not prepared';
                return '';
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "episode", {
            get: function () {
                //throw 'does not prepared';
                return 0;
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "totalEpisodes", {
            get: function () {
                if (this._pluginObject) {
                    return this._pluginObject.totalEpisodes;
                }
                return 0;
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "programmeID", {
            get: function () {
                if (this._pluginObject) {
                    return this._pluginObject.programmeID;
                }
                return this.program.crid;
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "programmeIDType", {
            get: function () {
                //throw 'does not prepared';
                return 0;
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "IMI", {
            get: function () {
                //throw 'does not prepared';
                return '';
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "groupCRIDs", {
            get: function () {
                //throw 'does not prepared';
                if (this._pluginObject) {
                    return this._pluginObject.groupCRIDs;
                }
                return [];
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "parentalRating", {
            get: function () {
                if (this._pluginObject) {
                    return this._pluginObject.parentalRating;
                }
                return null;
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "channel", {
            get: function () {
                return this._channel;
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "blocked", {
            get: function () {
                //throw 'does not prepared';
                return false;
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "showType", {
            get: function () {
                //throw 'does not prepared';
                return 0;
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "subtitles", {
            get: function () {
                //throw 'does not prepared';
                return false;
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "isHD", {
            get: function () {
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
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "isAD", {
            get: function () {
                //throw 'does not prepared';
                return false;
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "audioType", {
            get: function () {
                //throw 'does not prepared';
                return 0;
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "isMultilingual", {
            get: function () {
                //throw 'does not prepared';
                return false;
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "genre", {
            get: function () {
                var genre = this.program.genre;
                return genre;
                //var genre = this._program.querySelector('Genre');
                //if (genre) {
                //    return /urn:fvc:metadata:cs:ContentSubjectCS:[0-9-]+:([0-9]+)/g.exec(genre.getAttribute('href')).slice(1);
                //}
                //return [];
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "defaultThumbnail", {
            get: function () {
                var defaultThumb = (this._pluginObject && this._pluginObject.getGenreImage) ? this._pluginObject.getGenreImage() : null; // check RF first
                if (!defaultThumb) {
                    var genres = this.program.genre;
                    if (genres && genres.length) {
                        defaultThumb = hx.il.genreList.getGenreImage(genres[0]);
                    }
                }
                return defaultThumb || __MODEL__.TDefaultThumbnailList.DEFAULT;
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "hasRecording", {
            get: function () {
                //throw 'does not prepared';
                return false;
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "audioLanguages", {
            get: function () {
                //throw 'does not prepared';
                return [];
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "subtitleLanguages", {
            get: function () {
                //throw 'does not prepared';
                return [];
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "locked", {
            get: function () {
                //throw 'does not prepared';
                return false;
            },
            enumerable: true,
            configurable: true
        });
        COIPFProgrammeWrapper.prototype.getField = function (fieldId) {
            switch (fieldId) {
                case 'freeview/guidance':
                case 'freeview/linkage':
                case 'thumbnailURL':
                    hx.log('info', fieldId);
                    break;
            }
            return this._fieldsExtended[fieldId];
        };
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "logoURL", {
            get: function () {
                return this._channel ? this._channel.logoURL : '';
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "thumbnailURL", {
            get: function () {
                return this.program.icon.url;
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "recording", {
            get: function () {
                if (this._pluginObject) {
                    return this._pluginObject.recording;
                }
                return;
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "subtitleType", {
            // extended by humax
            get: function () {
                //throw 'does not prepared';
                return 0;
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "subtitleTypes", {
            get: function () {
                //throw 'does not prepared';
                return [];
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "scheduledRecording", {
            get: function () {
                //throw 'does not prepared';
                return null;
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "recordings", {
            get: function () {
                //throw 'does not prepared';
                return [];
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "mediaDeeplinkID", {
            get: function () {
                return '';
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "mediaDeeplinkUrl", {
            get: function () {
                return '';
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "mediaDeeplinkExpireDate", {
            get: function () {
                return 0;
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "ondemand", {
            // extension for freeview play
            get: function () {
                if (!this._ondemand) {
                    return null;
                }
                if (this._parsedOndemand) {
                    return this._parsedOndemand;
                }
                return this._parsedOndemand = __parser__.element.ondemand(this._ondemand);
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "program", {
            get: function () {
                if (!this._program) {
                    return null;
                }
                if (this._parsedProgram) {
                    return this._parsedProgram;
                }
                return this._parsedProgram = __parser__.element.programInformation(this._program);
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "available", {
            get: function () {
                return this._available;
            },
            set: function (available) {
                this._available = available;
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(COIPFProgrammeWrapper.prototype, "availableIconUrl", {
            get: function () {
                return this._availableIconUrl;
            },
            set: function (url) {
                this._availableIconUrl = url;
            },
            enumerable: true,
            configurable: true
        });
        COIPFProgrammeWrapper.prototype.getThumbnail = function (param) {
            var _this = this;
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
                hx.svc.IP_EPG.getThumbnail(this.program.icon.url, width).then(function (data) {
                    updateCb(data);
                }, function (err) {
                    updateCb(_this.defaultThumbnail);
                });
            }
            else {
                return this.defaultThumbnail;
            }
        };
        COIPFProgrammeWrapper.prototype.getChannelLogo = function (param) {
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
                hx.svc.IP_EPG.getChannelLogo(this._channel.sid, width).then(function (data) {
                    updateCb(data);
                }, function () {
                    updateCb('');
                });
            }
            else {
                return '';
            }
        };
        return COIPFProgrammeWrapper;
    })(CWrapperInterface);
    var COIPFConverter = (function () {
        function COIPFConverter(adapter) {
            this._adapter = adapter;
        }
        COIPFConverter.prototype.programme = function (raw, channel, pluginObject, thumbnailWidth) {
            if (thumbnailWidth === void 0) { thumbnailWidth = 400; }
            return new COIPFProgrammeWrapper(this._adapter, raw, channel, thumbnailWidth, pluginObject);
        };
        return COIPFConverter;
    })();
    var CAdaptorMds = (function () {
        function CAdaptorMds(getterUrlAuth) {
            this._channelLogoUrls = {};
            this._contentOwningLogoUrls = {};
            this.parse = __parser__;
            this.convert = {
                to: new COIPFConverter(this)
            };
            if (CAdaptorMds._instance) {
                return CAdaptorMds._instance;
            }
            CAdaptorMds._instance = this;
            this._engine = new __engine__(getterUrlAuth);
        }
        CAdaptorMds.prototype._parseServiceElement = function (aServiceElements, aCallback) {
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
            setTimeout(function () {
                for (var i = 0; i < lengthServiceInformation; i++) {
                    itemServiceInformation = arrayServiceInformation.item(i);
                    arrayRelatedMaterial = itemServiceInformation.querySelectorAll('RelatedMaterial'); //B2C Spec version 1.1 & https://digitaluk.atlassian.net/browse/FVCHUM-252
                    lengthRelatedMaterial = arrayRelatedMaterial.length;
                    itemMediaUri = null;
                    for (idxRelatedMaterial = 0; idxRelatedMaterial < lengthRelatedMaterial; idxRelatedMaterial++) {
                        itemRelatedMaterial = arrayRelatedMaterial[idxRelatedMaterial];
                        selectorHowRelated = itemRelatedMaterial.querySelector('HowRelated');
                        if (selectorHowRelated && selectorHowRelated.getAttribute('href') == 'urn:tva:metadata:cs:HowRelatedCS:2012:19') {
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
        };
        CAdaptorMds.prototype.setChannelLogoUrls = function (aServiceElements) {
            var _this = this;
            if (!aServiceElements) {
                hx.log('error', '[setChannelLogoUrls] !aServiceElements');
                return;
            }
            this._parseServiceElement(aServiceElements, function (aServiceInfo, aMediaUri) {
                _this._channelLogoUrls[aServiceInfo.getAttribute('serviceId')] = {
                    type: aMediaUri.getAttribute('contentType'),
                    uri: aMediaUri.textContent
                };
                console.log(aMediaUri);
            });
        };
        CAdaptorMds.prototype.setContentOwningLogoUrls = function (aServiceElements) {
            var _this = this;
            if (!aServiceElements) {
                hx.log('error', '[setContentOwningLogoUrls] !aServiceElements');
                return;
            }
            this._parseServiceElement(aServiceElements, function (aServiceInfo, aMediaUri) {
                _this._contentOwningLogoUrls[aServiceInfo.getAttribute('serviceId')] = {
                    type: aMediaUri.getAttribute('contentType'),
                    uri: aMediaUri.textContent
                };
                console.log(aMediaUri);
            });
        };
        /* public function - london: 12339, cambridge and bedfordshire: 12320, east anglia 12321*/
        CAdaptorMds.prototype.getChannels = function (regionId, prevHeader) {
            if (!regionId) {
                return Q.reject('!regionId');
            }
            return this._engine.xml(prevHeader, '/services?nid=' + regionId).then(function (res) {
                var dom = res.data;
                res.data = dom.querySelectorAll('ServiceInformation');
                return res;
            });
        };
        CAdaptorMds.prototype.getContentOwningServices = function (serviceType, prevHeader) {
            if (!serviceType) {
                return Q.reject(('!serviceType'));
            }
            return this._engine.xml(prevHeader, '/services?service_type=' + serviceType).then(function (res) {
                var dom = res.data;
                res.data = dom.querySelectorAll('ServiceInformation');
                return res;
            });
        };
        /* public function */
        CAdaptorMds.prototype.getSchedulesQuery = function (serviceId, baseTime, hours, catchupOnly) {
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
            hx.log('mds', '> query: ' + query + '[' + new Date(baseTime * 1000).toUTCString() + '][' + new Date(targetTime * 1000).toUTCString() + ']');
            return query;
        };
        CAdaptorMds.prototype.getXmlByQuery = function (query, prevHeader) {
            return this._engine.xml(prevHeader, query);
        };
        CAdaptorMds.prototype.getSchedules = function (serviceId, baseTime, hours, prevHeader) {
            return this._engine.xml(prevHeader, this.getSchedulesQuery(serviceId, baseTime, hours));
        };
        /* public function */
        CAdaptorMds.prototype.getDetailInfo = function (programId, encode, prevHeader) {
            if (encode) {
            }
            programId = encodeURIComponent(programId);
            var query = '/program?pid=' + programId;
            return this._engine.xml(prevHeader, query);
        };
        /* public function */
        CAdaptorMds.prototype.getThumbnail = function (url, width, prevHeader, type) {
            if (width === void 0) { width = 400; }
            if (!url) {
                return Q.reject('getThumbnail : url in null');
            }
            var delimiter = /\?/.test(url) ? '&' : '?';
            return this._engine.image(prevHeader, url + delimiter + 'w=' + width, type);
            // XXX: if you need to encode base64 way
        };
        /* public function */
        CAdaptorMds.prototype.getChannelLogo = function (serviceId, width, prevHeader) {
            if (width === void 0) { width = 80; }
            var logoImage = this._channelLogoUrls[serviceId];
            if (!logoImage || (logoImage && !logoImage.uri)) {
                return Q.reject('getChannelLogo : service[' + serviceId + '] does not exist');
            }
            var delimiter = /\?/.test(logoImage.uri) ? '&' : '?';
            return this._engine.image(prevHeader, logoImage.uri + delimiter + 'w=' + width);
        };
        CAdaptorMds.prototype.getContentOwningLogo = function (serviceId, width, prevHeader) {
            var owningImage = this._contentOwningLogoUrls[serviceId];
            if (!owningImage || (owningImage && !owningImage.uri)) {
                return Q.reject('getContentOwningLogo : service[' + serviceId + '] does not exist');
            }
            var delimiter = /\?/.test(owningImage.uri) ? '&' : '?';
            return this._engine.image(prevHeader, owningImage.uri + delimiter + 'w=' + width);
        };
        CAdaptorMds.prototype.getApplication = function (networkID, prevHeader, image_variant) {
            if (networkID.length === 0) {
                return Q.reject(('networkID.length === 0'));
            }
            return this._engine.xml(prevHeader, '/applications?nids[]=' + networkID.join('&nids[]='), fnConvertImageVariant(image_variant));
        };
        CAdaptorMds.prototype.getOnDemandSuggestions = function (inputString, prevHeader) {
            if (!inputString) {
                return Q.reject(('!inputString'));
            }
            inputString = encodeURIComponent(inputString.toString());
            return this._engine.xml(prevHeader, '/programs/suggestions?q=' + inputString + '&type=ondemand');
        };
        CAdaptorMds.prototype.getOnDemandResults = function (searchTerm, networkID, prevHeader, pageNum) {
            if (!searchTerm || networkID.length === 0) {
                return Q.reject(('!searchTerm or networkID.length === 0'));
            }
            searchTerm = encodeURIComponent(searchTerm.toString());
            return this._engine.xml(prevHeader, '/programs/search?q=' + searchTerm + '&type=ondemand&nids[]=' + networkID.join('&nids[]=') + ((pageNum && pageNum > 1) ? '&page=' + pageNum : ''));
        };
        CAdaptorMds.prototype.getRecommendationCategories = function (type, prevHeader) {
            if (!type) {
                return Q.reject(('!type'));
            }
            return this._engine.xml(prevHeader, '/groups/recommendations?type=' + encodeURIComponent(type.toString()));
        };
        CAdaptorMds.prototype.getRecommendationResults = function (type, networkIds, groupId, serviceId, prevHeader) {
            if ((!groupId && (networkIds.length === 0)) && !serviceId) {
                return Q.reject(('(!groupId && (networkIds.length === 0)) && !serviceId'));
            }
            var suffix = '/programs/recommendations?';
            if (groupId) {
                suffix = suffix + 'groupId=' + encodeURIComponent(groupId.toString()) + '&type=' + type + '&nids[]=' + networkIds.join('&nids[]=');
            }
            else if (serviceId) {
                suffix = suffix + 'type=' + type + '&sids[]=' + encodeURIComponent(serviceId.toString());
            }
            return this._engine.xml(prevHeader, suffix);
        };
        CAdaptorMds.prototype.getAuthToken = function (retryAfter, mdsUrl) {
            return this._engine.renewAuthInfo(retryAfter, mdsUrl);
            /*.then(function(){
                    return this;
                }, function(res) {
                    throw res;
                });
                */
        };
        CAdaptorMds.prototype.getBaseUrl = function () {
            return this._engine.getBaseUrl();
        };
        CAdaptorMds.prototype.setBaseUrl = function (url) {
            this._engine.setBaseUrl(url);
        };
        CAdaptorMds.prototype.setHeader = function (property, value) {
            this._engine.setHeader(property, value);
        };
        return CAdaptorMds;
    })();
    return CAdaptorMds;
});

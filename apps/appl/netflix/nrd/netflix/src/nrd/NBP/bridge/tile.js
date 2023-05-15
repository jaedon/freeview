/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/*global nrdp */

nrdp.tile = {
    classname: "TileBridge",
    _path: "tile",

    get version() { return this._syncData.version; },
    get capabilities() { return nrdp.capabilities.tile; },
    get enabled() { return nrdp.options.tilesEnabled; },
    get errorCodes() { return this._syncData.errorCodes; },

    setSplash: function setSplash(splashes, cb) {
        function sendError(error, errorCode) {
            if (cb) {
                cb({ success: false, error: error, errorCode: errorCode });
            }
        }
        if (!splashes)
            splashes = [];

        var splashFields = [
            { name: "width", type: "number" },
            { name: "height", type: "number" },
            { name: "data", type: function(t) { return (t instanceof ArrayBuffer || t instanceof Uint8Array || typeof t === 'string'); } }
        ];

        for (var s=0; s<splashes.length; ++s) {
            var splash = splashes[s];
            for (var f=0; f<splashFields.length; ++f) {
                var prop = splashFields[f].name;
                if (!splash.hasOwnProperty(prop)) {
                    if (!splashFields[f].optional) {
                        sendError("Splash is missing required field: " + prop, this.errorCodes.RequiredFieldMissing);
                        return;
                    }
                    continue;
                }
                if (splashFields[f].type instanceof Function ? (!splashFields[f].type(splash[prop])) : typeof splash[prop] != splashFields[f].type) {
                    sendError("Splash property: " + prop + " has wrong type", this.errorCodes.InvalidPropertyType);
                    return;
                }

                if (splashFields[f].maxSize && splash[prop].length > splashFields[f].maxSize) {
                    splash[prop] = splash[prop].substr(0, splashFields[f].maxSize);
                } else if (typeof splash[prop] == 'number' && splash[prop] < 0) {
                    sendError("Invalid integral negative property " + prop, this.errorCodes.InvalidPropertyValue);
                    return;
                }
            }
        }

        var id = this._nextIdx++;
        this._cbs[id] = cb;
        nrdp._invoke(this._path, "setSplash", { id: id, splashes: splashes },
                     function(result) {
                         if (!result.success) {
                             sendError(result.error, result.errorCode);
                         } else if (cb) {
                             cb({success: true, errorCode: this.errorCodes.Success});
                         }
                     });
    },

    setTiles: function setTiles(tiles, cb) {
        function sendError(error, errorCode) {
            if (cb) {
                cb({ success: false, error: error, errorCode: errorCode });
            }
        }

        if (tiles) {
            if (!(tiles instanceof Object)) {
                sendError("Invalid tile type", this.errorCodes.InvalidTileType);
                return;
            } else if (!(tiles.groups instanceof Array) || !tiles.groups.length) {
                sendError("Invalid tile object, no groups", this.errorCodes.NoTileGroups);
                return;
            }

            if (tiles.groups.length > this.capabilities.maxNumGroups) {
                sendError("Too many groups", this.errorCodes.GroupMaximumExceeded);
                return;
            }

            var tileFields = [
                { name: "url", type: "string", maxSize: 1024 },
                { name: "width", type: "number" },
                { name: "height", type: "number" },
                { name: "expiry", type: "number" },
                { name: "title", type: "string", maxSize: 50, optional: true },
                { name: "description", type: "string", maxSize: 150, optional: true },
                { name: "shortText", type: "string", maxSize: 150, optional: true },
                { name: "deepLink", type: "string", maxSize: 4096, optional: true }
            ];

            for (var g=0; g<tiles.groups.length; ++g) {
                var group = tiles.groups[g];
                if (!(group.tiles instanceof Array) || !group.tiles.length) {
                    sendError("Invalid tile group, no tiles in group", this.errorCodes.EmptyTileGroup);
                    return;
                }
                if (group.tiles.length > this.capabilities.maxTilesPerGroup) {
                    sendError("Too many tiles", this.errorCodes.TileMaximumExceeded);
                    return;
                }
                for (var t=0; t<group.tiles.length; ++t) {
                    var tile = group.tiles[t];
                    for (var f=0; f<tileFields.length; ++f) {
                        var prop = tileFields[f].name;
                        if (!tile.hasOwnProperty(prop)) {
                            if (!tileFields[f].optional) {
                                sendError("Tile is missing required field: " + prop, this.errorCodes.RequiredFieldMissing);
                                return;
                            }

                            continue;
                        }

                        if (typeof tile[prop] != tileFields[f].type) {
                            sendError("Tile property: " + prop + " has wrong type", this.errorCodes.InvalidPropertyType);
                            return;
                        }

                        if (tileFields[f].maxSize && tile[prop].length > tileFields[f].maxSize) {
                            tile[prop] = tile[prop].substr(0, tileFields[f].maxSize);
                        } else if (typeof tile[prop] == 'number' && tile[prop] < 0) {
                            sendError("Invalid integral negative property " + prop, this.errorCodes.InvalidPropertyValue);
                            return;
                        }
                    }
                }
            }
        }
        var id = this._nextIdx++;
        this._cbs[id] = cb;
        nrdp._invoke(this._path, "setTiles", { id: id, tiles: tiles },
                     function(result) {
                         if (!result.success) {
                             sendError(result.error, result.errorCodes);
                         } else if (cb) {
                             cb({success: true, errorCode: this.errorCodes.Success});
                         }
                     });
    },

    validateTiles: function validateTiles(cb) {
        var id = this._nextIdx++;
        this._cbs[id] = cb;
        nrdp._invoke(this._path, "validateTiles", { id: id });
    },

    validateSplash: function validateSplash(cb) {
        var id = this._nextIdx++;
        this._cbs[id] = cb;
        nrdp._invoke(this._path, "validateSplash", { id: id });
    },

    _handleEvent: function _handleEvent(event) {
        var cb = this._cbs[event.data.id];
        delete this._cbs[event.data.id];
        if (cb instanceof Function) {
            cb(event.data);
        }
        return true;
    },

    _syncData: {},
    _nextIdx: 1,
    _cbs: {}
};

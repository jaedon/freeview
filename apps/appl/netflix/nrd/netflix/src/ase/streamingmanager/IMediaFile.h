/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef IMEDIA_FILE_PARSER_H
#define IMEDIA_FILE_PARSER_H

#include <nrdase/AseCommonDataTypes.h>
#include <nrdbase/AseErrorCodes.h>
#include <nrdbase/ObjectCount.h>

#include <nrdase/AseTimeStamp.h>
#include <nrdase/AseBuffer.h>
#include <nrdase/ITrackInfo.h>
#include <nrdase/ByteRange.h>

#include "MediaPresentationRequest.h"

NRDP_DECLARE_OBJECTCOUNT(IMediaFile);

namespace netflix {
namespace ase {

class MediaPresentationRequest;
typedef shared_ptr<MediaPresentationRequest> MediaPresentationRequestPtr;

/** Interface for an object handling a media file
 *
 *  Subclasses should be defined for specific media file types
 *
 *  This object manages the download and storage of header and index
 *  data from the file. It provides methods to obtain the header and
 *  index data and to trigger download of the header and index data.
 *  The object encapsulates file-format-specific procedures for
 *  download and parsing of header and index information. Download
 *  of media is handled by the MediaStream based on the index data
 *  returned by this object.
 *
 *  No assumptions are made about the number of MEDIA requests required
 *  to download the data - the MEDIA requests are decoupled from the
 *  triggering based on header and index. The user (MediaStream) will
 *  repeatedly query the object to see if it needs an additional
 *  MEDIA request and if so the IMediaConsumer interface is used to give
 *  the object an opportunity to receive MEDIA data.
 *
 *  The object should maintain a record of the header and index data
 *  that the MediaStream requires that remains to be downloaded.
 *
 *  On creation it should assume that the header is required.
 *
 *  The object provides methods to obtain the header and query the index
 *  data. These return either with the required information or the status
 *  of the request for that information, which could be that it has not
 *  yet been requested (by the user), that it is waiting for it to
 *  arrive or that the request for that data failed permanantly.
 *
 *  NOTE: A factory object will be provided to create instances of this
 *        IMediaFile object. The inputs to the factory need to include
 *        the media file MIME type and initialization information from
 *        the manifest (URL, byte range or byte ranges, for example
 *        explicit header and index ranges if available).
 */
class IMediaFile : public enable_shared_from_this<IMediaFile>
{
public:
    typedef shared_ptr<IMediaFile> IMediaFilePtr;
    typedef std::vector<MediaFragmentInfo> MediaFileIndex;
    typedef shared_ptr<MediaFileIndex> MediaFileIndexPtr;
    typedef shared_ptr<MediaFileIndex const> MediaFileIndexConstPtr;

    virtual ~IMediaFile();

    /** Get a new Request object
     *  @param pRequest [out] The new MediaRequest object
     *  @return 0   for success, negative error code, or positive number
     *              indicating minimum time before next call in ms
     */
    virtual AseErrorCode getNextRequest( MediaPresentationRequestPtr& pRequest ) = 0;

    // IMediaFile

    /** Test if the MediaFile is in a good state */
    virtual bool good() const = 0;

    /** Test if the MediaFile has the media duration */
    virtual bool hasDuration() const = 0;

    /** Test if the MediaFile object has the header */
    virtual bool hasHeader() const = 0;

    /** Test if the MediaFile object has (any of) the index */
    virtual bool hasIndex() const = 0;

    /** Test if the index is complete */
    virtual bool indexComplete() const = 0;

    /** Get the content duration */
    virtual bool getDuration( AseTimeInterval& duration ) const = 0;

    /** Get the fragment duration in ms */
    virtual bool getFragmentDuration( AseTimeInterval& fragmentDuration ) const = 0;

    /** Get the byte range of the header+index */
    virtual ByteRange getHeaderRange() const = 0;

    /** Provide a byte range hint for the header+index */
    virtual void provideHeaderRangeHint( ByteRange const& range ) = 0;

    /** Get the media header
     * @param [out] media header buffer
     * @return True if the header was returned
     */
    virtual bool getHeader( AseBufferPtr& mediaHeader ) const = 0;

    /** query the media index given the time range
     * @param [out] pIndex The index vector
     * @param [out] indexStart start time of the first fragment
     * @param [out] indexEnd end of the last fragment in the index
     * @param [out] mediaEnd end time of the last fragment in the media
     * @param [out] mediaOffset the byte offset of the first fragment
     * @return true if index information was returned, false if not yet available
     */
    virtual bool getIndex(  MediaFileIndexConstPtr& pIndex,
                            AseTimeStamp&           indexStart,
                            AseTimeStamp&           indexEnd,
                            ullong&               mediaOffset ) const = 0;


    /** Gets the streamKey to identify the corresponding stream
     * @return streamKey
     */
    virtual std::string const& getStreamKey() const = 0;

    /** Obtain the last error if getHeader or getIndex returned FAILED
     * @return the error code
     */
    virtual AseErrorCode getLastError() const = 0;

    /** Get a idividualization token from the file */
    virtual std::vector<unsigned char> const& getFileToken() const = 0;

    /** reset to a good state if IMediaFile is in the middle fo something */
    virtual void reset() = 0;


protected:
    IMediaFile();

private:
    IMediaFile( IMediaFile const& );
    IMediaFile& operator=( IMediaFile const& );
};

typedef IMediaFile::IMediaFilePtr IMediaFilePtr;
typedef IMediaFile::MediaFileIndex MediaFileIndex;
typedef IMediaFile::MediaFileIndexPtr MediaFileIndexPtr;
typedef IMediaFile::MediaFileIndexConstPtr MediaFileIndexConstPtr;

inline IMediaFile::IMediaFile()
{
    NRDP_OBJECTCOUNT_REF(IMediaFile, this);
}

}}
#endif /* IMEDIA_FILE_PARSER_H */

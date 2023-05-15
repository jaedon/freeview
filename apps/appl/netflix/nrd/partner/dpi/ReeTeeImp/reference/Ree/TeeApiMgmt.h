/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef TEEAPIMGMT_H_
#define TEEAPIMGMT_H_

#include <nrddpi/TeeApiComm.h>
#include <nrddpi/MgmtTypes.h>
#include <nrdbase/tr1.h>
#include <nrdbase/NFErr.h>

/**
 * @class 	TeeApiMgmt
 * @brief 	a singleton class with member functions that provide TEE management functionality like init, shutdown etc
 */

namespace netflix {
class TeeApiMgmt
{
public:
    /**
     * constructor
     *
     * @param	initFlags, flags to initialize TEE with
     */
    TeeApiMgmt(uint32_t defaultInitFlags = 0xFFFFFFFF);

    /**
     * destructor
     */
    ~TeeApiMgmt();

    /*
     * @function  initTee
     * @brief 	  initializes the TEE with defaultInitFlags and encrypted manuf SS
     *
     * @param     manufSS, encrypted manufacturing secure store content
     * @return 	  int error, 0 on success, <0 on failure
     *
     */

    netflix::NFErr initTee (const DataBuffer &manufSS);

    /*
     * @function  shutdownTee
     * @brief 	  shuts down the TEE
     *
     * @param
     * @return 	  int error, 0 on success, <0 on failure
     *
     */
    netflix::NFErr shutdownTee ();

    /*
     * @function  resetTeeDebugFlags
     * @brief 	  changes the debug flags in TEE to the ones provided as args
     *
     * @param     newFlags, new debug flags
     * @return 	  int error, 0 on success, <0 on failure
     *
     */
    netflix::NFErr resetTeeDebugFlags (uint32_t newFlags);

    /*
    * @function  translateTeeError
    * @brief     translates Tee error related to Management operations into NFErr object
    */
    static netflix::NFErr translateTeeError (nfTeeResult_t teeResult);

    private:
    shared_ptr<TeeApiComm> teeApiComm;
    uint32_t defaultInitFlags;
};
}

#endif //TEEAPIMGMT_H_

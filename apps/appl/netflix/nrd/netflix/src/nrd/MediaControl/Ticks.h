/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef TICKS_H
#define TICKS_H

#include <nrdbase/StdInt.h>
#include <sys/types.h>


namespace netflix {

/**
 * @class Ticks Ticks.h
 * @brief A unit of time with multiple resolutions.
 */
class Ticks
{
public:
    /** Resolution. */
    enum Resolution
    {
        RES_500HZ = 0, RES_1KHZ = 1, RES_90KHZ = 2, RES_100NS = 3
    };

    /** Create a new invalid instance. */
    Ticks()
        : mIsValid(false),
          mInternal(0),
          mResolution(RES_1KHZ)
    {}

    /**
     * Create a new instance of the specified value and resolution.
     *
     * @param[in] value the value in units of the resolution.
     * @param[in] resolution the time resolution.
     */
    Ticks(ullong value, Resolution resolution)
        : mIsValid(true),
          mInternal(value),
          mResolution(resolution)
    {}

    /**
     * Returns the value in units of the specified resolution.
     *
     * @param[in] resolution the time resolution.
     * @return the value.
     */
    ullong value(Resolution resolution) const
    {
        if (static_cast<uint32_t>(resolution) < 4 &&
            static_cast<uint32_t>(mResolution) < 4)
        {
            static const ullong resolutionTable[4] = {
                500ull, 1000ull, 90000ull, 10000000ull
            };

            if (resolution == mResolution)
                return mInternal;

            return (mInternal * resolutionTable[static_cast<uint32_t>(resolution)]) / resolutionTable[static_cast<uint32_t>(mResolution)];
        }

        return 0;
    }

    /**
     * Assignment operator.
     *
     * @param[in] other the other Ticks instance.
     * @return this Ticks instance.
     */
    Ticks& operator=(const Ticks& other)
    {
        mInternal = other.value(mResolution);
        mIsValid  = other.valid();

        return *this;
    }

    /**
     * Returns true if the this value is equal to the other value.
     *
     * @param[in] other the other Ticks.
     * @return true if equal.
     */
    bool operator==(const Ticks& other) const
    {
        return (value(RES_100NS) == other.value(RES_100NS));
    }

    /**
     * Returns true if this value is not equal to the other value.
     *
     * @param[in] other the other Ticks.
     * @return true if not equal.
     */
    bool operator!=(const Ticks& other) const
    {
        return (value(RES_100NS) != other.value(RES_100NS));
    }

    /**
     * Returns true if this value is less than the other value.
     *
     * @param[in] other the other Ticks.
     * @return true if this is less.
     */
    bool operator<(const Ticks& other) const
    {
        return (value(RES_100NS) < other.value(RES_100NS));
    }

    /**
     * Returns true if this value is less than or equal to the other
     * value.
     *
     * @param[in] other the other Ticks.
     * @return true if this is less than or equal.
     */
    bool operator<=(const Ticks& other) const
    {
        return (value(RES_100NS) <= other.value(RES_100NS));
    }

    /**
     * Returns true if this value is greater than the other value.
     *
     * @param[in] other the other Ticks.
     * @return true if this is greater.
     */
    bool operator>(const Ticks& other) const
    {
        return (value(RES_100NS) > other.value(RES_100NS));
    }

    /**
     * Returns true if this value is greater than or equal to the other
     * value.
     *
     * @param[in] other the other Ticks.
     * @return true if this is greater than or equal.
     */
    bool operator>=(const Ticks& other) const
    {
        return (value(RES_100NS) >= other.value(RES_100NS));
    }

    /**
     * Adds the other Ticks to this one.
     *
     * @param[in] other the other Ticks.
     * @return true this Ticks instance.
     */
    Ticks& operator+=(const Ticks& other)
    {
        mInternal += other.value(mResolution);

        return *this;
    }

    /**
     * Creates a new Ticks that is the sum of this and the other.
     *
     * @param[in] other the other Ticks.
     * @return the new Ticks instance.
     */
    Ticks operator+(const Ticks& other) const;

    /**
     * Creates a new Ticks that is the sum of this and the provided
     * value.
     *
     * @param[in] value a value in units of this Ticks' resolution.
     * @return the new Ticks instance.
     */
    Ticks operator+(const ullong value) const;

    /**
     * Subtracts a Ticks from this one.
     *
     * @param[in] other the other Ticks.
     * @return this Ticks instance.
     */
    Ticks& operator-=(const Ticks& other);

    /**
     * Creates a new Ticks that is the difference between this and the
     * other.
     *
     * @param[in] other the other Ticks.
     * @return a new Ticks instance.
     */
    Ticks operator-(const Ticks& other) const;

    /**
     * Creates a new Ticks that is the difference between this and the
     * provided value.
     *
     * @param[in] value a value in units of this Ticks' resolution.
     * @return the new Ticks instance.
     */
    Ticks operator-(const ullong value) const;

    /**
     * Return true if this Ticks is valid.
     *
     * @return true if valid.
     */
    bool valid() const
    {
        return mIsValid;
    }

    /**
     * Sets this Ticks validity. Once set false, the Ticks value is
     * lost.
     *
     * @param[in] value true if this Ticks is valid, false if it is
     *            not.
     */
    void valid(bool value)
    {
        if(!value) { mInternal = 0; }

        mIsValid = value;
    }

protected:
    bool mIsValid;          //!< True if the Ticks is valid.
    ullong mInternal;     //!< Internal value.
    Resolution mResolution; //!< Unit of resolution.
};

/**
 * @class Tick500hz Ticks.h
 * @brief A Ticks that uses a resolution of 500Hz.
 */
class Ticks500hz : public Ticks
{
public:
    /**
     * Construct a new Ticks with the specified value in units of
     * 500Hz.
     *
     * @param[in] value the value.
     */
    Ticks500hz(ullong value)
        : Ticks(value, RES_500HZ)
    {}

    /**
     * Assignment operator.
     *
     * @param[in] other the other Ticks.
     * @return this Ticks instance.
     */
    Ticks500hz& operator=(const Ticks& other)
    {
        mInternal = other.value(mResolution);

        return *this;
    }
};

/**
 * @class Tick1khz Ticks.h
 * @brief A Ticks that uses a resolution of 1kHz.
 */
class Ticks1khz : public Ticks
{
public:
    /**
     * Construct a new Ticks with the specified value in units of
     * 1kHz.
     *
     * @param[in] value the value.
     */
    Ticks1khz(ullong value)
        : Ticks(value, RES_1KHZ)
    {}

    /**
     * Assignment operator.
     *
     * @param[in] other the other Ticks.
     * @return this Ticks instance.
     */
    Ticks1khz& operator=(const Ticks& other)
    {
        mInternal = other.value(mResolution);
        return *this;
    }
};

/**
 * @class Tick1khz Ticks.h
 * @brief A Ticks that uses a resolution of 90kHz.
 */
class Ticks90khz : public Ticks
{
public:
    /**
     * Construct a new Ticks with the specified value in units of
     * 90kHz.
     *
     * @param[in] value the value.
     */
    Ticks90khz(ullong value)
        : Ticks(value, RES_90KHZ)
    {}

    /**
     * Assignment operator.
     *
     * @param[in] other the other Ticks.
     * @return this Ticks instance.
     */
    Ticks90khz& operator=(const Ticks& other)
    {
        mInternal = other.value(mResolution);
        return *this;
    }
};

/**
 * @class Tick1khz Ticks.h
 * @brief A Ticks that uses a resolution of 100ns.
 */
class Ticks100ns : public Ticks
{
public:
    /**
     * Construct a new Ticks with the specified value in units of
     * 100ns.
     *
     * @param[in] value the value.
     */
    Ticks100ns(ullong value)
        : Ticks(value, RES_100NS)
    {}

    /**
     * Assignment operator.
     *
     * @param[in] other the other Ticks.
     * @return this Ticks instance.
     */
    Ticks100ns& operator=(const Ticks& other)
    {
        mInternal = other.value(mResolution);
        return *this;
    }
};

inline Ticks Ticks::operator+(const Ticks& other) const
{
    return Ticks(mInternal + other.value(mResolution), mResolution);
}

inline Ticks Ticks::operator+(const ullong other) const
{
    return Ticks(mInternal + other, mResolution);
}

inline Ticks Ticks::operator-(const Ticks& other) const
{
    if(value(RES_100NS) < other.value(RES_100NS))
        return Ticks100ns(0);
    return Ticks(mInternal - other.value(mResolution), mResolution);
}

inline Ticks Ticks::operator-(const ullong other) const
{
    if(value(mResolution) < other)
        return Ticks(0, mResolution);
    return Ticks(mInternal - other, mResolution);
}

inline Ticks&
Ticks::operator-=(const Ticks& other)
{
    if(value(RES_100NS) < other.value(RES_100NS))
        mInternal = 0;
    else
        mInternal -= other.value(mResolution);
    return *this;
}

} // namespace netflix

#endif // TICKS_H

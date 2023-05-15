//****************************************************************************
//
//  Copyright (c) 2001-2003  Broadcom Corporation
//  All Rights Reserved
//  No portions of this material may be reproduced in any form without the
//  written permission of:
//          Broadcom Corporation
//          16215 Alton Parkway
//          Irvine, California 92618
//  All information contained in this document is Broadcom Corporation
//  company private, proprietary, and trade secret.
//
//****************************************************************************
//  $Id: UnsignedIntCommandParameter.h 1.5 2007/05/15 21:45:53Z ceyles Release $
//
//  Filename:       UnsignedIntCommandParameter.h
//  Author:         David Pullen
//  Creation Date:  Feb 22, 2001
//
//****************************************************************************

#ifndef UNSIGNEDINTCOMMANDPARAMETER_H
#define UNSIGNEDINTCOMMANDPARAMETER_H

//********************** Include Files ***************************************

// My base class.
#include "CommandParameter.h"

// For the max unsigned int values.
#include <limits.h>

//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Class Declaration ***********************************


/** \ingroup BfcConsole
*
*      This is the derived class for unsigned int parameters.  It handles
*      parsing an unsigned int, and limiting it to an optional range.
*
*      \note  All of the string pointers in the class MUST be static strings
*             that will not change or be deleted, etc., because only the string
*             pointers are stored!
*/
class BcmUnsignedIntCommandParameter : public BcmCommandParameter
{
public:

    /// Default/Initializing Constructor.  Initializes the state of the object,
    /// storing the values specified.
    ///
    /// If pParameterName is NULL, "Number" will be used.
    ///
    /// \param
    ///      clientId - the id that the client wants to associate with this
    ///                 parameter.
    /// \param
    ///      isOptional - true if the parameter is optional, false if it is
    ///                   required.
    /// \param
    ///      pParameterName - the name of this parameter (for printing).
    /// \param
    ///      minValue - the minimum value that will be allowed.
    /// \param
    ///      maxValue - the maximum value that will be allowed.
    ///
    BcmUnsignedIntCommandParameter(unsigned int clientId = 0, 
                                   bool isOptional = false,
                                   const char *pParameterName = NULL,
                                   unsigned int minValue = 0,
                                   unsigned int maxValue = UINT_MAX);

    /// Destructor.  Frees up any memory/objects allocated, cleans up internal
    /// state.
    ///
    virtual ~BcmUnsignedIntCommandParameter();

    /// Prints the value that is currently stored (regardless of whether or not
    /// parsing was successful) to the specified ostream.
    ///
    /// This method must be implemented by the derived class.
    ///
    /// \param
    ///      outstream - the ostream to be used.
    ///
    /// \return
    ///      A reference to the ostream for chaining insertion operators.
    ///
    virtual ostream &PrintValue(ostream &outstream) const;

    /// Prints the value that is currently stored (regardless of whether or not
    /// parsing was successful) to the specified string.
    ///
    /// This method must be implemented by the derived class.
    ///
    /// \param
    ///      string - the string into which the value should be printed.
    ///
    virtual void PrintValue(char string[64]) const;

    /// Prints the valid value that is accepted 
    /// This method is optional to the derived class.
    ///
    /// \param
    ///      string - the string into which the value should be printed.
    ///
    virtual void PrintValidValue(char string[64]) const;

public:

    /// Accessor for the value, using the type-specific method from the base
    /// class.  We only provide the ones that are relevant for our object type.
    ///
    /// \return
    ///     The value that was parsed or set.
    ///
    virtual unsigned int AsUnsignedInt(void) const;

public:

    /// Stores the specified value.
    ///
    /// \param
    ///     value - the value to be stored.
    ///
    /// \retval
    ///     true if the value was stored successfully.
    /// \retval
    ///     false if the value was not stored successfully.
    ///
    virtual bool SetAsUnsignedInt(unsigned int value);

protected:

    /// This must be implemented by the derived class.  It is a "template method"
    /// that allows the derived class to do the real work of parsing the string
    /// and updating its state.
    ///
    /// The derived class can assume that the pParameterString pointer is not
    /// NULL.
    ///
    /// \param
    ///      pParameterString - pointer to the string containing the parameters
    ///                         to be parsed.  It will not be modified.
    /// \param
    ///      offset - the offset into the string where parsing should begin.
    ///
    /// \retval
    ///      true if the parameter was successfully parsed.
    /// \retval
    ///      false if the parameter was not successfully parsed.
    ///
    virtual bool ParseImpl(const char *pParameterString, unsigned int &offset);

    /// Gives the derived class a chance to do some derived-class specific stuff
    /// during the print.
    ///
    /// \param
    ///      outstream - the ostream to be used.
    ///
    /// \return
    ///      A reference to the ostream for chaining insertion operators.
    ///
    virtual ostream &PrintImpl(ostream &outstream) const;

private:

    /// The value we store/parse/retrieve.
    unsigned int fValue;

    //@{
    /// The range that I'm limiting the value to.
    unsigned int fMinValue;
    unsigned int fMaxValue;
    //@}

private:

    /// Copy Constructor.  Not supported.
    BcmUnsignedIntCommandParameter(const BcmUnsignedIntCommandParameter &otherInstance);

    /// Assignment operator.  Not supported.
    BcmUnsignedIntCommandParameter & operator = (const BcmUnsignedIntCommandParameter &otherInstance);

};


//********************** Inline Method Implementations ***********************


/// Insertion operator for this class.  It allows you to do the following:
///
///    cout << commandParameter << endl;
///
inline ostream & operator << (ostream &outputStream, const BcmUnsignedIntCommandParameter &commandParameter)
{
    return commandParameter.Print(outputStream);
}


#endif



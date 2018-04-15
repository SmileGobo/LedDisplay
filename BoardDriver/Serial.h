/****************************************************************************
*
*   Copyright (c) 2005 - 2008 Dave Hylands     <dhylands@gmail.com>
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License version 2 as
*   published by the Free Software Foundation.
*
*   Alternatively, this software may be distributed under the terms of BSD
*   license.
*
*   See README and COPYING for more details.
*
****************************************************************************/
/**
*
*   @file   Serial.h
*
*   @brief  This file contains the definitions for the Serial class.
*
****************************************************************************/
/**
*   @defgroup   Serial   Serial Port
*
*   @brief      Performs serial communications using Unix style serial ports.
*
****************************************************************************/

#if !defined( Serial_H )
#define Serial_H                   ///< Include Guard

#include <stddef.h>
#include <stdint.h>
#include <map>
/**
 * @addtogroup Serial
 * @{
 */

//---------------------------------------------------------------------------
/**
*   The Serial class encapsulates serial port communcations using
*   unix style serial communications. This also works under cygwin (for Win32)
*/

class Serial
{
public:

    //------------------------------------------------------------------------
    // Default constructor

    Serial();

    //------------------------------------------------------------------------
    // Destructor

    ~Serial();

    //------------------------------------------------------------------------
    // Opens a serial port.

    bool Open( const char *devName, const char *param = NULL );

    //------------------------------------------------------------------------
    // Closes a previsouly opened serial port.

    void Close();

    //------------------------------------------------------------------------
    //  Reads data from the serial port.

    size_t Read( void *buf, size_t bytesToRead );

    //------------------------------------------------------------------------
    //  Sets the timeout to use when waiting for data. 0 = infinite
    //  The timeout is specified in milliseconds.

    bool SetTimeout( unsigned timeout );

    //------------------------------------------------------------------------
    //  Writes data to the serial port.

    size_t Write( const void *buf, size_t bytesToWrite );

    //------------------------------------------------------------------------
    //  Strobes the DTR and RTS lines.

    //void StrobeDTRRTS();

private:
    typedef std::map<unsigned int,unsigned int> BaudTable;
    static BaudTable& GetTable();
    //------------------------------------------------------------------------
    // The copy constructor and assignment operator are not need for this
    // class so we declare them private and don't provide an implementation.

    Serial( const Serial & copy );
    Serial &operator =( const Serial &rhs );

    //------------------------------------------------------------------------
    int     m_fd;

};

// ---- Inline Functions ----------------------------------------------------

/** @} */

#endif // SERILAPORT_H

#include "Serial.h"

/****************************************************************************
*
*   Copyright (c) 2009 Dave Hylands     <dhylands@gmail.com>
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
*   @file   Serial.cpp
*
*   @brief  This file implements the Serial class using the posix
*           API, which is supported by cygwin and linux.
*
****************************************************************************/

#include "Serial.h"
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/unistd.h>
#include <sys/ioctl.h>

static struct
{
    speed_t     speed;
    unsigned    baudRate;
} gBaudTable[] =
{

};


// ---- Private Function Prototypes -----------------------------------------
// ---- Functions -----------------------------------------------------------

/**
 * @addtogroup Serial
 * @{
 */

//***************************************************************************
/**
*   Constructor
*/

Serial::Serial()
    : m_fd( -1 )
{
}

//***************************************************************************
/**
*   Destructor
*/

Serial::~Serial(){
    Close();
}

//***************************************************************************
/**
*   Closes a previously opened serial port
*/

void Serial::Close()
{
    if ( m_fd >= 0 )
    {
        close( m_fd );

        m_fd = -1;
    }

} // Close

//***************************************************************************
/**
*   Opens a serial port.
*/

bool Serial::Open(const  std::string& inDevName, const char *param )
{

    // Translate the params, if any

    speed_t speed = B0;
    if ( param == NULL ){
        speed = B38400;
        baudRate = 38400;
    }
    else
    {
        baudRate = atoi( param );

        for ( unsigned i = 0; i < ARRAY_LEN( gBaudTable ); i++ )
        {
            if ( gBaudTable[ i ].baudRate == baudRate )
            {
                speed = gBaudTable[ i ].speed;
                break;
            }
        }

        if ( speed == B0 )
        {
            LogError( "Unrecognized baud rate: '%s'\n", param );
            return false;
        }
    }

    if (( m_fd = open( devName, O_RDWR | O_EXCL )) < 0 ){
        //LogError( "Unable to open serial port '%s': %s\n", devName, strerror( errno ));
        return false;
    }

    // Setup the serial port

    struct termios  attr;

    if ( tcgetattr( m_fd, &attr ) < 0 )
    {
        LogError( "A: Call to tcgetattr failed: %s\n", strerror( errno ));
        return false;
    }

    attr.c_iflag = 0;
    attr.c_oflag = 0;
    attr.c_cflag = CLOCAL | CREAD | CS8;
    attr.c_lflag = 0;
    attr.c_cc[ VTIME ] = 0;   // timeout in tenths of a second
    attr.c_cc[ VMIN ] = 1;

    cfsetispeed( &attr, speed );
    cfsetospeed( &attr, speed );

    if ( tcsetattr( m_fd, TCSAFLUSH, &attr ) < 0 )
    {
        //LogError( "Call to tcsetattr failed: %s\n", strerror( errno ));
        return false;
    }

    return true;

} // Open

//***************************************************************************
/**
*   Reads data from the serial port.
*/

size_t Serial::Read( void *buf, size_t bufSize )
{
    int  bytesRead;

    if ( m_fd < 0 )
    {
        return 0;
    }

    bytesRead = read( m_fd, buf, bufSize );
    if ( bytesRead < 0 )
    {
        if ( errno != EBADF )
        {
            // We get EBADF returned if the serial port is closed on
            // us.

            //LogError( "read failed: %d\n", errno );
        }
        return 0;
    }

    return bytesRead;

} // Read

//***************************************************************************
/**
*   Sets the timeout to use when waiting for data. 0 = infinite
*   The timeout is specified in milliseconds.
*/
bool Serial::SetTimeout( unsigned timeout )
{
    struct termios  attr;

    if ( m_fd < 0 )
    {
        return false;
    }

    if ( tcgetattr( m_fd, &attr ) < 0 )
    {
        //LogError( "B: Call to tcgetattr failed: %s\n", strerror( errno ));
        return false;
    }

    if ( timeout == 0 )
    {
        attr.c_cc[ VTIME ] = 0;
        attr.c_cc[ VMIN ]  = 1;
    }
    else
    {
        // Note. termios only supports timeouts specified in tenths of
        // a second, so we need convert from milliseconds.

        timeout = ( timeout + 99 ) / 100;
        if ( timeout > 255 )
        {
            timeout = 255;
        }

        attr.c_cc[ VTIME ] = (uint8_t)timeout;   // timeout in tenths of a second
        attr.c_cc[ VMIN ]  = 0;
    }

    if ( tcsetattr( m_fd, TCSAFLUSH, &attr ) < 0 )
    {
       // LogError( "Call to tcsetattr failed: %s\n", strerror( errno ));
        return false;
    }

    return true;

} // SetTimeout

//***************************************************************************
/**
*   Writes data to the serial port.
*/

size_t Serial::Write( const void *buf, size_t bytesToWrite )
{
    int  bytesWritten;

    bytesWritten = write( m_fd, buf, bytesToWrite );
    if ( bytesWritten < 0 )
    {
        LogError( "write failed: %d\n", errno );
        bytesWritten = 0;
    }

    return bytesWritten;

}

Serial::BaudTable &Serial::GetTable(){
    static BaudTable table;
    if (table.is)
} // Write

//***************************************************************************
/**
*   Resets the target using the DTR and CTS line
*/
/*
void Serial::StrobeDTRRTS()
{
    int rc;
    unsigned int status;

    rc = ioctl(m_fd, TIOCMGET, &status);
    if ( rc < 0 )
    {
        perror("ioctl('TIOCMGET')");
        return;
    }
    //printf("Clearing DTR/RTS\n");
    status &= ~(TIOCM_DTR | TIOCM_RTS);
    rc = ioctl(m_fd, TIOCMSET, &status);
    if ( rc < 0 )
    {
        perror("ioctl('TIOCMSET')");
        return;
    }
    usleep(250 * 1000);

    printf("Setting DTR/RTS\n");
    status |= (TIOCM_DTR | TIOCM_RTS);
    rc = ioctl(m_fd, TIOCMSET, &status);
    if ( rc < 0 )
    {
        perror("ioctl('TIOCMSET')");
        return;
    }
    usleep(50 * 1000);

} // StrobeDTRRTS*/

/** @} */

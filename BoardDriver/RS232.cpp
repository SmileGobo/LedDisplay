///***Source for RS232 wrapper class***///
//--------------------------------------//
/*  This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "RS232.h"
#include <iostream>

using namespace RS232;

//linux

//being base 0 sometimes requesting COM4 means you need to pass 3 as the port number - check this when trying to open a new port
//and you get an error saying port is unavailable.
/*
const char SerialConnection::m_comPorts[30][16] = {"/dev/ttyS0","/dev/ttyS1","/dev/ttyS2","/dev/ttyS3","/dev/ttyS4","/dev/ttyS5",
													"/dev/ttyS6","/dev/ttyS7","/dev/ttyS8","/dev/ttyS9","/dev/ttyS10","/dev/ttyS11",
													"/dev/ttyS12","/dev/ttyS13","/dev/ttyS14","/dev/ttyS15","/dev/ttyUSB0",
													"/dev/ttyUSB1","/dev/ttyUSB2","/dev/ttyUSB3","/dev/ttyUSB4","/dev/ttyUSB5",
													"/dev/ttyAMA0","/dev/ttyAMA1","/dev/ttyACM0","/dev/ttyACM1",
													"/dev/rfcomm0","/dev/rfcomm1","/dev/ircomm0","/dev/ircomm1"};
*/
Serial::Serial():
    m_port(-1),m_open(false),
    m_newPortSettings(), m_oldPortSettings()
{

}

Serial::~Serial(){
    Close();
}

int Serial::Open(const std::string& dev_name,const unsigned baud)
{
	int baudrate, status;
	//set baudrate
	switch(baud)
	{
	case 50:
		baudrate = B50;
		break;
	case 75:
		baudrate = B75;
		break;
	case 110:
		baudrate = B110;
		break;
	case 134:
		baudrate = B134;
		break;
	case 150:
		baudrate = B150;
		break;
	case 200:
		baudrate = B200;
		break;
	case 300:
		baudrate = B300;
		break;
	case 600:
		baudrate = B600;
		break;
	case 1200:
		baudrate = B1200;
		break;
	case 1800:
		baudrate = B1800;
		break;
	case 2400:
		baudrate = B2400;
		break;
	case 4800:
		baudrate = B4800;
		break;
	case 9600:
		baudrate = B9600;
		break;
	case 19200:
		baudrate = B19200;
		break;
	case 38400:
		baudrate = B38400;
		break;
	case 57600:
		baudrate = B57600;
		break;
	case 115200:
		baudrate = B115200;
		break;
	case 230400:
		baudrate = B230400;
		break;
	case 460800:
		baudrate = B460800;
		break;
	case 500000:
		baudrate = B500000;
		break;
	case 576000:
		baudrate = B576000;
		break;
	case 921600:
		baudrate = B921600;
		break;
	case 1000000:
		baudrate = B1000000;
		break;
	default: std::cout << baud << ": Invalid baudrate." << std::endl;
		return 1;
	}

	//attempt to open port
    m_port = open(dev_name.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if(m_port == -1)
	{
		//C++ equivalant of this?
		perror("SerConn::OpenPort: Unable to open port");
		return 1;
	}
	//apply port settings
    auto error = tcgetattr(m_port, &m_oldPortSettings);
	if(error == -1)
	{
        Close();
		perror("SerConn::OpenPort: Unable to read port settings");
		return 1;
	}
	//clear and set port settings struct
	memset(&m_newPortSettings, 0, sizeof(m_newPortSettings));
	m_newPortSettings.c_cflag = baudrate | CS8 | CLOCAL | CREAD;
	m_newPortSettings.c_iflag = IGNPAR;
	m_newPortSettings.c_oflag = 0;
	m_newPortSettings.c_lflag = 0;
	m_newPortSettings.c_cc[VMIN] = 0; //block until n bytes rcd
	m_newPortSettings.c_cc[VTIME] = 0; //block until timer expires

    error = tcsetattr(m_port, TCSANOW, &m_newPortSettings);
	if(error == -1)
	{
        close(m_port);
		perror("SerConn::OpenPort: Unable to apply port settings");
		return 1;
	}

	//check port status
    if(ioctl(m_port, TIOCMGET, &status) == -1)
	{
		perror("SerConn::OpenPort: Unable to get port status");
		return 1;
	}
	//enable dtr and rts
	status |= TIOCM_DTR;
	status |= TIOCM_RTS;
	//update port status
    if(ioctl(m_port, TIOCMSET, &status) == -1)
	{
		perror("SerConn::OpenPort: Unable to update port status");
		return 1;
	}
    SetOpen(true);
	return 0;
}

int Serial::ReadByte(unsigned char& byte)
{
    if(m_port == -1)
	{
		perror("SerConn::ReadByte: Invalid port specified");
		return 1;
	}

    read(m_port, &byte, 1);
	return 0;
}

int Serial::ReadByteArray(unsigned char* buffer, unsigned short size){
    if(!IsOpen())	{
		perror("SerConn::ReadByteArray: Invalid port specified");
		return -1;
	}

#ifndef __STRICT_ANSI__ //-ansi flag in GCC
	if(size > SSIZE_MAX) size = (int)SSIZE_MAX;
#else
	if(size > 4096) size = 4096;
#endif

    return read(m_port, buffer, size);
}

int Serial::SendByte(unsigned char byte){
    if(!IsOpen())	{
		perror("SerConn::SendByte: Invalid port specified");
		return -1;
	}
    int n = write(m_port, &byte, 1);
	if(n < 0) return -1;
	return 0;
}

int Serial::SendByteArray(unsigned char* buffer, unsigned short size)
{
    if(!IsOpen())	{
		perror("SerConn::SendByteArray: Invalid port specified");
		return -1;
	}
    return write(m_port, buffer, size);
}

int Serial::Close(){
    /*int status;
    if(ioctl(m_port, TIOCMGET, &status) == -1)
	{
        perror("SerConn::Close: Unable to get port status");
	}
	//turn off DTR and RTS
	status &= ~TIOCM_DTR;
	status &= ~TIOCM_RTS;

    if(ioctl(m_port, TIOCMSET, &status) == -1)
	{
        perror("SerConn::Close: Unable to set port status");
	}

    close(m_port);
    tcsetattr(m_port, TCSANOW, &m_oldPortSettings);*/
    if(IsOpen()){
        SetOpen(false);
        close(m_port);
    }
	return 0;
}


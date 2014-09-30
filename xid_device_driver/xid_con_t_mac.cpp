/* Copyright (c) 2010, Cedrus Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * Neither the name of Cedrus Corporation nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "xid_con_t.h"
#include "constants.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <IOKit/serial/ioss.h>
#include <termios.h>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <iostream>

struct cedrus::xid_con_t::DarwinConnPimpl
{
    DarwinConnPimpl()
    : m_FileDescriptor ( 0 )
    {}

    int m_FileDescriptor;
    termios m_OptionsOriginal;
    termios	m_OptionsCurrent;
};

cedrus::xid_con_t::xid_con_t(
    const std::string &port_name,
    int port_speed,
    int delay_ms,
    bytesize byte_size,
    bitparity bit_parity,
    stopbits stop_bits
    )
    : baud_rate_(port_speed),
      byte_size_(byte_size),
      bit_parity_(bit_parity),
      stop_bits_(stop_bits),
      handshaking_(HANDSHAKE_NONE),
      port_name_(port_name),
      delay_(delay_ms),
      needs_interbyte_delay_(true),
      m_connection_dead (false),
      m_darwinPimpl( new DarwinConnPimpl )
{
}


cedrus::xid_con_t::~xid_con_t(void)
{
    if(m_darwinPimpl->m_FileDescriptor != 0)
        close();
}

bool cedrus::xid_con_t::close()
{
    bool status = (::close(m_darwinPimpl->m_FileDescriptor) != -1);
    m_darwinPimpl->m_FileDescriptor = 0;

    return status;
}

bool cedrus::xid_con_t::flush_write_to_device_buffer()
{
    // TCOFLUSH flushes data written but not transmitted.    
    return ( tcflush( m_darwinPimpl->m_FileDescriptor, TCOFLUSH ) == 0 );
}

bool cedrus::xid_con_t::flush_read_from_device_buffer()
{
    // TCIFLUSH flushes data received but not read.
    return ( tcflush( m_darwinPimpl->m_FileDescriptor, TCIFLUSH ) == 0 );
}

int cedrus::xid_con_t::open()
{
    int status = XID_NO_ERR;

    m_darwinPimpl->m_FileDescriptor = ::open(port_name_.c_str(), O_RDWR | O_NOCTTY /*| O_NONBLOCK | O_FSYNC*/ );

    if(m_darwinPimpl->m_FileDescriptor == -1)
    {
        m_darwinPimpl->m_FileDescriptor = 0;
        status = XID_PORT_NOT_AVAILABLE;
    }
    else
    {
        if (ioctl(m_darwinPimpl->m_FileDescriptor, TIOCEXCL) == -1)
        {
            status = XID_PORT_NOT_AVAILABLE;
        }
        else
        {
            if ( !setup_com_port() )
                status = XID_ERROR_SETTING_UP_PORT;
        }
    }

    if ( status == XID_NO_ERR )
        m_connection_dead = false;

    return status;
}

bool cedrus::xid_con_t::setup_com_port()
{
	bool status = true;

	// http://developer.apple.com/documentation/DeviceDrivers/Conceptual/WorkingWSerial/WWSerial_SerialDevs/chapter_2_section_7.html#//apple_ref/doc/uid/TP40000972-TP30000384-CIHIAAFF

    // Get the current options and save them so we can restore the
    // default settings later.
    if (tcgetattr(m_darwinPimpl->m_FileDescriptor, &(m_darwinPimpl->m_OptionsOriginal)) == OS_FILE_ERROR)
        status = false;
    else
    {
        // The serial port attributes such as timeouts and baud rate are set by
        // modifying the termios structure and then calling tcsetattr to
        // cause the changes to take effect. Note that the
        // changes will not take effect without the tcsetattr() call.
        // See tcsetattr(4) ("man 4 tcsetattr") for details.
        
        m_darwinPimpl->m_OptionsCurrent = m_darwinPimpl->m_OptionsOriginal;
    }

	if ( status )
	{
		// Set raw input (non-canonical) mode, with reads blocking until either
		// a single character has been received or a one second timeout expires.
		// See tcsetattr(4) ("man 4 tcsetattr") and termios(4) ("man 4 termios")
		// for details.

		cfmakeraw(&(m_darwinPimpl->m_OptionsCurrent));
		// the following two settings cause reads to be non-blocking, despite
		// the fact that the port has been set to block
		m_darwinPimpl->m_OptionsCurrent.c_cc[VMIN] = 0;
		m_darwinPimpl->m_OptionsCurrent.c_cc[VTIME] = 0;

		m_darwinPimpl->m_OptionsCurrent.c_iflag = ( IGNBRK | IGNPAR );	/* input flags */

		m_darwinPimpl->m_OptionsCurrent.c_oflag = 0;	/* output flags */
		m_darwinPimpl->m_OptionsCurrent.c_cflag = 0;	/* control flags */

		tcflag_t flags = m_darwinPimpl->m_OptionsCurrent.c_cflag;

		flags |= ( CREAD | HUPCL | CLOCAL );

		switch ( handshaking_ )
		{
			case HANDSHAKE_XON_XOFF:
				m_darwinPimpl->m_OptionsCurrent.c_iflag |= ( IXON | IXOFF | IXANY );
				break;

			case HANDSHAKE_HARDWARE:
				flags |= CRTSCTS;	// CTS and RTS flow control of output
				break;

			case HANDSHAKE_NONE:
				m_darwinPimpl->m_OptionsCurrent.c_iflag &= ~( IXON | IXOFF | IXANY );
				flags &= ~ CRTSCTS;
				break;

			default:
				break;
		}

		flags &= ~( PARENB | PARODD );
		m_darwinPimpl->m_OptionsCurrent.c_iflag &= ~( INPCK | ISTRIP );
		switch ( bit_parity_ )
		{
			case BITPARITY_ODD:
				m_darwinPimpl->m_OptionsCurrent.c_iflag |= INPCK | ISTRIP;
				flags |= PARODD; // this sets the parity to odd -- even is the default
			case BITPARITY_EVEN:
				m_darwinPimpl->m_OptionsCurrent.c_iflag |= INPCK;
				flags |= PARENB; // this enables parity
				break;

			case BITPARITY_NONE:
				break;

			default:
				break;
		}

		// The baud rate, word length, and handshake options can be set as follows:

		flags &= ~CSIZE;
		switch ( byte_size_ )
		{
			case BYTESIZE_6:
				flags |= CS6;
				break;

			case BYTESIZE_7:
				flags |= CS7;
				break;

			case BYTESIZE_8:
				flags |= CS8;
				break;

			default:
				break;
		}

		if ( stop_bits_ == STOP_BIT_2 )
			flags |= CSTOPB;
		else
			flags &= ~CSTOPB;

		cfsetspeed(&(m_darwinPimpl->m_OptionsCurrent), baud_rate_);
		m_darwinPimpl->m_OptionsCurrent.c_cflag = ( flags );

		// Cause the new options to take effect immediately.
		usleep(10*1000);

		if (tcsetattr(m_darwinPimpl->m_FileDescriptor, TCSAFLUSH, &(m_darwinPimpl->m_OptionsCurrent)) == OS_FILE_ERROR)
			status = false;
	}

	if ( handshaking_ == HANDSHAKE_HARDWARE )
	{

		if ( status )
		{
			// To set the modem handshake lines, use the following ioctls.
			// See tty(4) ("man 4 tty") and ioctl(2) ("man 2 ioctl") for details.
			if (ioctl(m_darwinPimpl->m_FileDescriptor, TIOCSDTR) == OS_FILE_ERROR)
				status = false;
		}

		if ( status )
		{
			// Clear Data Terminal Ready (DTR)
			if (ioctl(m_darwinPimpl->m_FileDescriptor, TIOCCDTR) == OS_FILE_ERROR)
				status = false;
		}

		unsigned long handshake = 0;

		if ( status )
		{
			handshake = TIOCM_DTR | TIOCM_DSR | TIOCM_RTS | TIOCM_CTS;
			// Set the modem lines depending on the bits set in handshake.
			if (ioctl(m_darwinPimpl->m_FileDescriptor, TIOCMSET, &handshake) == OS_FILE_ERROR)
				status = false;
		}

		if ( status )
		{
			// To read the state of the modem lines, use the following ioctl.
			// See tty(4) ("man 4 tty") and ioctl(2) ("man 2 ioctl") for details.

			// Store the state of the modem lines in handshake.
			if (ioctl(m_darwinPimpl->m_FileDescriptor, TIOCMGET, &handshake) == OS_FILE_ERROR)
				status = false;
		}
	} // handshaking

	// :whschultz:20070507
	// Found Apple documentation stating how to set the read latency on a standard serial port.
	// http://developer.apple.com/samplecode/SerialPortSample/listing2.html
    // http://developer.apple.com/library/mac/samplecode/SerialPortSample/index.html
#if defined(MAC_OS_X_VERSION_10_3) && (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_3)
	if ( status )
	{
		unsigned long mics = 1UL;

		// Set the receive latency in microseconds. Serial drivers use this value to determine how often to
		// dequeue characters received by the hardware. Most applications don't need to set this value: if an
		// app reads lines of characters, the app can't do anything until the line termination character has been
		// received anyway. The most common applications which are sensitive to read latency are MIDI and IrDA
		// applications.

		// set latency to 1 microsecond
		if (ioctl(m_darwinPimpl->m_FileDescriptor, IOSSDATALAT, &mics) == OS_FILE_ERROR) //does that actually happen??
			status = false;
	}
#endif

	if ( status )
	{
		flush_write_to_device_buffer();
		flush_read_from_device_buffer();
	}

	return status;
}

bool cedrus::xid_con_t::read(
    unsigned char *in_buffer,
    int bytes_to_read,
    int *bytes_read)
{
    if ( m_connection_dead )
        return false;

    int status = true;
    int read = ::read(m_darwinPimpl->m_FileDescriptor, in_buffer, bytes_to_read);

    if( read == -1)
    {
        status = false;
        if ( errno == ENXIO )
            m_connection_dead = true;
    }
    else
        *bytes_read = read;

    return status;
}

bool cedrus::xid_con_t::write(
    unsigned char * const in_buffer,
    int bytes_to_write,
    int *bytes_written)
{
    if ( m_connection_dead )
        return false;

    unsigned char *p = in_buffer;
    int status = true;
    int written = 0;

    if(needs_interbyte_delay_)
    {
        for(int i = 0; i < bytes_to_write && status; ++i)
        {
            ssize_t byte_count = ::write(m_darwinPimpl->m_FileDescriptor, p, 1);

            if( byte_count == -1)
            {
                status = false;
                if ( errno == ENXIO )
                    m_connection_dead = true;
                break;
            }

            written += byte_count;

            if(written == bytes_to_write)
                break;

            usleep(delay_*1000);

            ++p;
        }
        *bytes_written = written;
    }
    else
    {
        written = ::write(m_darwinPimpl->m_FileDescriptor, in_buffer, bytes_to_write);
        tcdrain(m_darwinPimpl->m_FileDescriptor);

        if( written == -1)
        {
            if ( errno == ENXIO )
                m_connection_dead = true;

            status = false;
        }
        else
            *bytes_written = written;
    }

    return status;
}

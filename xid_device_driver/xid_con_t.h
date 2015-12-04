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

#ifndef XID_CON_T_H
#define XID_CON_T_H

#include "interface_xid_con.h"
#include <boost/shared_ptr.hpp>
#include "ftd2xx.h"

#ifdef __APPLE__
#   define SLEEP_FUNC usleep
#   define SLEEP_INC 1000
#   define OS_DEPENDENT_LONG unsigned long
#elif defined(_WIN32)
#   include <windows.h>
#   define SLEEP_FUNC Sleep
#   define SLEEP_INC 1
#   define OS_DEPENDENT_LONG DWORD
#endif

namespace cedrus
{
    /**
     * @class xid_con_t xid_con_t.h "xid_device_driver/xid_con_t.h"
     *
     * @brief Implementation of communication with an XID device
     *
     * This class implements the nuts and bolts of communicating with an
     * XID device.  Inter-byte delays are optionally added if it's
     * communicating with a 4 MHz XID device.  It also handles the parsing
     * of the XID protocol 6-byte packats.
     */
    class xid_con_t : public interface_xid_con
    {
    public:
        enum { INTERBYTE_DELAY = 1 };

        /**
         * xid_con_t constructor
         *
         * @param[in] port_name COM port name the device is connected to.
         * @param[in] port_speed speed of the port. This defaults to 115200bps
         * @param[in] byte_size Byte size used by the device
         * @param[in] bit_parity bit parity of the device.  Defaults to none.
         * @param[in] stop_bits stop bits of the device. Defaults to 0
         */
        xid_con_t(
                  const DWORD location,
                  DWORD port_speed = 115200,
                  BYTE byte_size = FT_BITS_8,
                  BYTE bit_parity = FT_PARITY_NONE,
                  BYTE stop_bits = FT_STOP_BITS_1
                  );

        virtual ~xid_con_t();

        /**
         * Closes the device
         *
         * @returns true if closing the device was successful, or false if it was unsuccessful.
         */
        bool close();

        /**
         * Flush the device's input buffer.
         *
         * @returns true if flushing the device was successful, or false if it was unsuccessful.
         */
        bool flush_write_to_device_buffer();

        /**
         * Flush the device's output buffer.
         *
         * @returns true if flushing the device was successful, or false if it was unsuccessful.
         */
        bool flush_read_from_device_buffer();

        /**
         * Opens the COM port for use.
         *
         * @returns NO_ERR if opening the device for communication was
         * successful. If the operation was unsuccessful, it can return one of
         * the following:  PORT_NOT_AVAILABLE, ERROR_SETTING_UP_PORT,
         * ERROR_FLUSHING_PORT.  Enum values are defined in constants.h.
         */
        int open();

        /**
         * Read data from the device.
         *
         * @param[out] in_buffer character array to be used for reading data
         * from the device into.  It is the user's responsibility to ensure
         * bytes_to_read is less than the size of in_buffer
         * @param[in] bytes_to_read specifies the number of bytes to expect
         * to be returned from the device
         * @param[out] bytes_read number of bytes actually read
         *
         * @returns true if the read was successful, false otherwise.
         */
        bool read( unsigned char *in_buffer, int bytes_to_read, int *bytes_read);

        /**
         * Write data to the device.
         *
         * If the device is a 4MHz XID unit, an inter-byte delay is used between
         * each byte sent to the device.
         *
         * @param[in] in_buffer command to be sent to the device
         * @param[in] bytes_to_write number of bytes in the command to be sent
         * @param[out] bytes_written number of bytes written to the device
         */
        bool write(
            unsigned char * const in_buffer,
            int bytes_to_write,
            int *bytes_written);

        int send_xid_command(
            const char in_command[],
            char out_response[],
            int max_out_response_size);

        int send_xid_command_slow(
            const char in_command[],
            char out_response[],
            int max_out_response_size);

        int send_xid_command_pst_proof(
            const char in_command[],
            char out_response[],
            int max_out_response_size);

        int get_baud_rate () const;

        void set_baud_rate ( int rate );

        bool has_lost_connection();

    private:

        bool setup_com_port();
        unsigned long GetTickCount() const;
 
        /**
         * Send an XID command to the device.
         *
         * XID commands are documented at http://www.cedrus.com/xid/
         *
         * @param[in] in_command command to send to the device
         * @param[out] out_response response from the device
         * @param[in] max_out_response_size maximum allowed response size
         * @param[in] num_retries specifies the number of retries we're
         * willing to allow until while we wait for the device to respond
         */
        int send_xid_command(
            const char in_command[],
            char out_response[],
            int max_out_response_size,
            int num_retries);

        DWORD baud_rate_;
        BYTE byte_size_;
        BYTE bit_parity_;
        BYTE stop_bits_;
        DWORD m_location;

        bool m_connection_dead;

        struct DarwinConnPimpl;
        boost::shared_ptr<DarwinConnPimpl> m_darwinPimpl;

        struct WindowsConnPimpl;
        boost::shared_ptr<WindowsConnPimpl> m_winPimpl;
    };
} // namespace cedrus

#endif // XID_CON_T_H

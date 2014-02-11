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

#ifndef PORT_SETTINGS_T_H
#define PORT_SETTINGS_T_H

#include <string>

#include "XidDriverImpExpDefs.h"

namespace cedrus
{
    /**
     * @class port_settings_t port_settings_t.h "xid_device_driver/port_settings_t.h"
     * @brief Handles serial port settings used by the XID devices.
     *
     * This is an internal class used by @link xid_con_t @endlink and shouldn't be
     * needed by users of the library.
     */
    class port_settings_t
    {
    public:
        enum bitparity
        {
            BITPARITY_NONE = 0,
            BITPARITY_ODD  = 1,
            BITPARITY_EVEN = 2
        };

        enum stopbits
        {
            STOP_BIT_1 = 0,
            STOP_BIT_2 = 1
        };

        enum bytesize
        {
            BYTESIZE_6 = 6,
            BYTESIZE_7 = 7,
            BYTESIZE_8 = 8,

            FIRST_BYTESIZE = BYTESIZE_6
        };

        enum handshaking
        {
            HANDSHAKE_NONE     = 0,
            HANDSHAKE_XON_XOFF = 1,
            HANDSHAKE_HARDWARE = 2,
        };
        port_settings_t() {}

        port_settings_t(
            const std::string &port_name,
            int port_speed,
            bytesize byte_size = BYTESIZE_8,
            bitparity bit_parity = BITPARITY_NONE,
            stopbits stop_bits = STOP_BIT_1);

        virtual ~port_settings_t(void);

        const CEDRUS_XIDDRIVER_IMPORTEXPORT std::string& port_name() const;
        
        int CEDRUS_XIDDRIVER_IMPORTEXPORT baud_rate() const;
        void CEDRUS_XIDDRIVER_IMPORTEXPORT baud_rate(int speed);

        bitparity CEDRUS_XIDDRIVER_IMPORTEXPORT bit_parity() const;
        void CEDRUS_XIDDRIVER_IMPORTEXPORT bit_parity(bitparity parity);

        bytesize CEDRUS_XIDDRIVER_IMPORTEXPORT byte_size() const;
        void CEDRUS_XIDDRIVER_IMPORTEXPORT byte_size(bytesize size);

        handshaking CEDRUS_XIDDRIVER_IMPORTEXPORT handshake() const;
        void CEDRUS_XIDDRIVER_IMPORTEXPORT handshake(handshaking shake);

        stopbits CEDRUS_XIDDRIVER_IMPORTEXPORT stop_bits() const;
        void CEDRUS_XIDDRIVER_IMPORTEXPORT stop_bits(stopbits bits);
        
    private:
        std::string port_name_;
        int baud_rate_;
        bytesize byte_size_;
        bitparity bit_parity_;
        stopbits stop_bits_;
        handshaking handshaking_;
    };
} // namespace cedrus

#endif // PORT_SETTINGS_T_H

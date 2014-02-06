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

#include "port_settings_t.h"


cedrus::port_settings_t::port_settings_t(
    const std::string &port_name,
    int port_speed,
    bytesize byte_size,
    bitparity bit_parity,
    stopbits stop_bits)
    : port_name_(port_name),
      baud_rate_(port_speed),
      byte_size_(byte_size),
      bit_parity_(bit_parity),
      stop_bits_(stop_bits),
      handshaking_(HANDSHAKE_NONE)
{
}


cedrus::port_settings_t::~port_settings_t(void)
{
}

const std::string& cedrus::port_settings_t::port_name() const
{
    return port_name_;
}

int cedrus::port_settings_t::baud_rate() const
{
    return baud_rate_;
}

void cedrus::port_settings_t::baud_rate(int speed)
{
    baud_rate_ = speed;
}

cedrus::port_settings_t::bitparity cedrus::port_settings_t::bit_parity() const
{
    return bit_parity_;
}

void cedrus::port_settings_t::bit_parity(bitparity parity)
{
    bit_parity_ = parity;
}

cedrus::port_settings_t::bytesize cedrus::port_settings_t::byte_size() const
{
    return byte_size_;
}

void cedrus::port_settings_t::byte_size(bytesize size)
{
    byte_size_ = size;
}

cedrus::port_settings_t::handshaking cedrus::port_settings_t::handshake() const
{
    return handshaking_;
}

void cedrus::port_settings_t::handshake(handshaking shake)
{
    handshaking_ = shake;
}

cedrus::port_settings_t::stopbits cedrus::port_settings_t::stop_bits() const
{
    return stop_bits_;
}

void cedrus::port_settings_t::stop_bits(stopbits bits)
{
    stop_bits_ = bits;
}

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

#pragma once

namespace
{
    // if we decide that other classes can use this, then i will re-declare it in the header
    void AdjustEndiannessUintToChars
    (
        const unsigned int the_int,
        unsigned char* c1,
        unsigned char* c2,
        unsigned char* c3,
        unsigned char* c4
    )
    {
        unsigned int num = the_int;

        *c1 = num & 0x000000ff;
        num >>= 8;
        *c2 = num & 0x000000ff;
        num >>= 8;
        *c3 = num & 0x000000ff;
        num >>= 8;
        *c4 = num & 0x000000ff;
    }

    unsigned int AdjustEndiannessCharsToUint
    (
        const char c1,
        const char c2,
        const char c3,
        const char c4
    )
    {
        unsigned int result = 0;

        result |= (c4 & 0x000000ff);
        result <<= 8;
        result |= (c3 & 0x000000ff);
        result <<= 8;
        result |= (c2 & 0x000000ff);
        result <<= 8;
        result |= (c1 & 0x000000ff);

        return result;
    }
}

namespace Cedrus
{
    enum
    {
        XID_NO_ERR                  =   0,
        XID_UNDEFINED               =  -1,
        XID_PORT_NOT_AVAILABLE      =  -2,
        XID_ERROR_SETTING_UP_PORT   =  -3,
        XID_GENERAL_ERROR           = -10,
    };

    enum
    {
        INVALID_RETURN_VALUE = -99
    };

    enum KeyState
    {
        NO_KEY_DETECTED = 0,
        FOUND_KEY_DOWN = 1,
        FOUND_KEY_UP = 2
    };

    enum
    {
        MAX_PORTS = 255
    };

    // These correspond to ASCII values
    enum
    {
        PRODUCT_ID_LUMINA = 48, // '0'
        PRODUCT_ID_SV1 = 49,
        PRODUCT_ID_RB = 50,
        PRODUCT_ID_MPOD = 51,
        PRODUCT_ID_CPOD = 52,
        PRODUCT_ID_STIMTRACKER = 83 // 'S'
    };

} // namespace Cedrus

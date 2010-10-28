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

#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace cedrus
{
    enum
    {
        NO_ERR                  =   0,
        UNDEFINED               =  -1,
        PORT_NOT_AVAILABLE      =  -2,
        ERROR_SETTING_UP_PORT   =  -3,
        ERROR_FLUSHING_PORT     =  -4,
        ERROR_READING_PORT      =  -5,
        ERROR_WRITING_TO_PORT   =  -6,
        ERROR_CLOSING_PORT      =  -7,
        GENERAL_ERROR           = -10,
        PORT_OPENED_BUT_ABORTED = -11,
    };

    enum
    {
        UNKNOWN_MODEL_ID = 0
    };

    enum
    {
        ANY_MODEL_ID = -2,
    };
    enum
    {
        NO_XID_DEVICE_FOUND = -1
    };
    
    enum
    {
        INVALID_RETURN_VALUE = -99
    };

    enum
    {
        RB530  =  1,
        RB730  =  2,
        RB830  =  3,
        RB834  =  4,
        RB630  =  5,
        SV1    = 21,
        LUMINA = 31
    };
    
    enum
    {
        XID_ID_LUMINA = 0,
        XID_ID_SV1    = 1,
        XID_ID_RB     = 2
    };
    
    enum
    {
        AD_NO_PROMPTS = 0x01,
        AD_ASK_BEFORE_CONVERTING = 0x02,
        AD_CONVERSION_IRREVERSIBLE = 0x04
    };
    
    enum
    {
        XID_MODE = '0'
    };

    enum key_state
    {
        NO_KEY_DETECTED = 0,
        FOUND_KEY_DOWN = 1,
        FOUND_KEY_UP = 2
    };

} // namespace cedrus

#endif // CONSTANTS_H
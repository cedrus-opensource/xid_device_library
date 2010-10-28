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

#ifndef STRING_TOKENIZER_H
#define STRING_TOKENIZER_H

#include <vector>

namespace cedrus
{
    /**
     * Split a delimited string into a std::vector of strings.
     *
     * Note that this will not correctly split a string with empty fields, i.e.
     * "value,,value,value".  The 2nd empty field will not be returned.
     *
     * @param[in] str the delimited string
     * @param[in] delimiters delimiters used to split the string
     */
    template<typename string_type>
    std::vector<string_type> tokenize(
        const string_type &str, const string_type &delimiters)
    {
        std::vector<string_type> tokens;
        string_type::size_type last_pos = 0, pos = 0;
        int count = 0;

        if(str.length() < 1)
            return tokens;

        pos = str.find_first_of(delimiters, last_pos);
        while(string_type::npos != pos || string_type::npos != last_pos)
        {
            // found a token, add it to the vector
            tokens.push_back(str.substr(last_pos, pos-last_pos));

            // skip delimiters
            last_pos = str.find_first_not_of(delimiters, pos);

            if((string_type::npos != pos) && (str.substr(pos, last_pos-pos).length() > 1))
            {
                count = str.substr(pos, last_pos-pos).length();
            }

            pos = str.find_first_of(delimiters, last_pos);
        }

        return tokens;
    }
}

#endif // STRING_TOKENIZER
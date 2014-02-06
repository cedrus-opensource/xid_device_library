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

#ifndef xid_device_scanner_t_H
#define xid_device_scanner_t_H

#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

#include "XidDriverImpExpDefs.h"

namespace cedrus
{
    class xid_con_t;

    class CEDRUS_XIDDRIVER_IMPORTEXPORT xid_device_scanner_t
    {
    public:
        xid_device_scanner_t(void);
        virtual ~xid_device_scanner_t(void);

        void drop_every_connection();

        /**
         * Returns the number of valid XID devices connected to the computer
         *
         * In the process of scanning for XID devices, this adds each device
         * to the xid_connections_ vector.
         *
         * @returns number of XID devices detected.
         */
        int detect_valid_xid_devices();

        /**
         * Returns an XID connection object for use by the xid_device_t class.
         * 
         * The connection must be opened after retreiving it.
         *
         * Note: This only returns response devices.  Use stimtracker_connection_at_index()
         * to get stimtracker devices.
         *
         * @param[in] i index of the device
         * @returns an xid connection object for use by an instance of xid_device_t.
         */
        boost::shared_ptr<xid_con_t> response_device_connection_at_index(unsigned int i);

        boost::shared_ptr<xid_con_t> stimtracker_connection_at_index(unsigned int i);

        int rb_device_count() const;
        int st_device_count() const;

    private:
        /**
        * determines the number of available serial port devices.
        *
        * This information is needed by detect_valid_xid_devices()
        */
        void load_available_com_ports();

        std::vector<std::string> available_com_ports_;
        std::vector<boost::shared_ptr<cedrus::xid_con_t> > rb_connections_;
        std::vector<boost::shared_ptr<cedrus::xid_con_t> > st_connections_;
    };
} // namespace cedrus

#endif // xid_device_scanner_t_H

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
#include <boost/function.hpp>

#include "XidDriverImpExpDefs.h"

namespace cedrus
{
    class xid_con_t;
    class base_device_t;
    class xid_device_t;
    class stim_tracker_t;
    class xid_device_config_t;

    class xid_device_scanner_t
    {
    public:
        CEDRUS_XIDDRIVER_IMPORTEXPORT xid_device_scanner_t(const std::string &config_file_location);
        virtual CEDRUS_XIDDRIVER_IMPORTEXPORT ~xid_device_scanner_t(void);

        void CEDRUS_XIDDRIVER_IMPORTEXPORT close_all_connections();

        void CEDRUS_XIDDRIVER_IMPORTEXPORT drop_every_connection();

        void CEDRUS_XIDDRIVER_IMPORTEXPORT drop_connection_by_ptr( boost::shared_ptr<cedrus::base_device_t> device );

        void CEDRUS_XIDDRIVER_IMPORTEXPORT check_connections_drop_dead_ones();

        bool CEDRUS_XIDDRIVER_IMPORTEXPORT read_in_devconfigs ( const std::string &config_file_location );

        // This does a clean scan for devices.
        //  reportFunction is for reporting errors during the scanning process
        //  progressFunction is for reporting progress on a 0-100 scale and the
        // return value is to signal that we need to cancel the scanning process.
        // true for stop, false for don't
        int CEDRUS_XIDDRIVER_IMPORTEXPORT detect_valid_xid_devices(
            boost::function< void ( std::string ) > reportFunction = NULL,
            boost::function< bool ( unsigned int ) > progressFunction = NULL );

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
        boost::shared_ptr<base_device_t> CEDRUS_XIDDRIVER_IMPORTEXPORT device_connection_at_index(unsigned int i) const;

        unsigned int CEDRUS_XIDDRIVER_IMPORTEXPORT device_count() const;

        const boost::shared_ptr<const xid_device_config_t> CEDRUS_XIDDRIVER_IMPORTEXPORT devconfig_at_index(unsigned int i) const;

        unsigned int CEDRUS_XIDDRIVER_IMPORTEXPORT devconfig_count() const;

    private:

        std::vector<boost::shared_ptr<cedrus::base_device_t> > devices_;

        std::vector< boost::shared_ptr<cedrus::xid_device_config_t> > m_masterConfigList;
    };
} // namespace cedrus

#endif // xid_device_scanner_t_H

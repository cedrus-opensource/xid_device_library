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

#ifndef XID_DEVICE_CONFIG_T_H
#define XID_DEVICE_CONFIG_T_H

#include <map>
#include <string>
#include <boost/shared_ptr.hpp>

#include "XidDriverImpExpDefs.h"

namespace cedrus
{
    /**
     * @class xid_device_config_t xid_device_config_t.h "xid_device_driver/xid_device_config.h"
     *
     * @brief device configuration class
     */
    class xid_device_config_t
    {
    public:
        /**
         * Returns a device configuration object.
         *
         * This class reads the .devconfig files distributed with this library.
         * The user must specify the runtime path to the devconfig files otherwise
         * very conservative defaults are used.  Button counts on the device
         * may not match up with reality and button ordering may be different
         * than expected.
         *
         * If the path passed into this function is empty, an invalid
         * boost::shared_ptr will be returned.
         *
         * @param[in] product_id product id of the XID device
         * @param[in] model_id Used with RB-series devices. 0 otherwise.
         * @param[in] devconfig_location Path to the devconfig files.  See the
         * constructor of CedrusXidDeviceEnumerator for an example of looking
         * up the path to the devconfig files.
         *
         * @returns a device configuration object
         */
        static boost::shared_ptr<xid_device_config_t> config_for_device(
            int product_id, int model_id, const std::string &devconfig_location);

        ~xid_device_config_t(void);

        /**
         * Gets the actual button pressed based on the devconfig key mapping
         *
         * @param[in] key key reported by the response pad
         * @returns the mapped key number based on the .devconfig file.
         */
        int CEDRUS_XIDDRIVER_IMPORTEXPORT get_mapped_key(int key) const;
        
        /**
         * Number of response lines (or buttons) a device has
         */
        int CEDRUS_XIDDRIVER_IMPORTEXPORT number_of_lines() const;

        /**
         * Whether or not the device requires an inter-byte delay
         */
        bool CEDRUS_XIDDRIVER_IMPORTEXPORT needs_interbyte_delay() const;

        /**
         * The digital output prefix
         */ 
        char CEDRUS_XIDDRIVER_IMPORTEXPORT digital_out_prefix() const;

    private:
        xid_device_config_t(const std::string &devconfig_location);
        void load_devconfig(int product_id, int model_id);

        std::string config_file_location_;
        bool needs_interbyte_delay_;
        int number_of_lines_;
        char digital_out_prefix_;

        std::map<int,int> key_map_;
    };
} // namespace cedrus

#endif // XID_DEVICE_CONFIG_T

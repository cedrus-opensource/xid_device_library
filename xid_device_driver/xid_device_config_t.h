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
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>

#include "XidDriverImpExpDefs.h"

namespace cedrus
{
    struct device_port
    {
        device_port():
            port_name(""),
            port_number(-1),
            number_of_lines(-1)
        {}

        bool usable_as_response()
        {
            return !key_map.empty();
        };

        std::string port_name;
        int port_number;
        int number_of_lines;
        std::map<int,int> key_map;
    };

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
        static boost::shared_ptr<xid_device_config_t> config_for_device(boost::property_tree::ptree * pt);

        CEDRUS_XIDDRIVER_IMPORTEXPORT ~xid_device_config_t(void);

        /**
         * Gets the actual button pressed based on the devconfig key mapping
         *
         * @param[in] key key reported by the response pad
         * @returns the mapped key number based on the .devconfig file.
         */
        int CEDRUS_XIDDRIVER_IMPORTEXPORT get_mapped_key(int port, int key) const;

        /**
         * Whether or not the device requires an inter-byte delay
         */
        bool CEDRUS_XIDDRIVER_IMPORTEXPORT needs_interbyte_delay() const;

        /**
         * The digital output prefix
         */ 
        char CEDRUS_XIDDRIVER_IMPORTEXPORT digital_out_prefix() const;

        bool CEDRUS_XIDDRIVER_IMPORTEXPORT is_port_on_ignore_list( std::string port_name ) const;

        /**
         * Returns the name of the device
         *
         * @returns name of the device
         */
        std::string CEDRUS_XIDDRIVER_IMPORTEXPORT get_device_name();

        /**
         * product id of the device.
         * 
         * @returns product id
         * 0: Lumina LP-400 response pad system
         * 1: SV-1 voice key system
         * 2: RB series response pad.
         */
        int CEDRUS_XIDDRIVER_IMPORTEXPORT get_product_id() const;

        /**
         * model ID of the device.
         * 
         * This is generally only valid on RB series response pads.
         *
         * @returns model id of the device
         * 1: RB-530
         * 2: RB-730
         * 3: RB-830
         * 4: RB-834
         */
        int CEDRUS_XIDDRIVER_IMPORTEXPORT get_model_id() const;

        int CEDRUS_XIDDRIVER_IMPORTEXPORT get_num_lines_on_port(int port) const;

        std::vector<device_port> CEDRUS_XIDDRIVER_IMPORTEXPORT get_vector_of_ports() const;

        /**
         * Resets the internal device reaction time timer.
         * 
         * This should be called when a stimulus is presented
         */
        void CEDRUS_XIDDRIVER_IMPORTEXPORT reset_rt_timer();

        bool CEDRUS_XIDDRIVER_IMPORTEXPORT does_config_match_device( int device_id, int model_id, int major_firmware_ver )  const;


    public:
        xid_device_config_t(boost::property_tree::ptree * pt);

        bool needs_interbyte_delay_;

        std::string device_name_;
        int major_firmware_ver_;
        int product_id_;
        int model_id_;

        std::map<int, device_port> m_device_ports;
        std::vector<std::string> ports_to_ignore_;
    };
} // namespace cedrus

#endif // XID_DEVICE_CONFIG_T
